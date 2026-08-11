"""Prompt assembly -- what context each generation step is given, and nothing else.

Kept in its own file because under raw orchestration the prompt IS the interface. A
framework would hide this behind a role and a task description; here the exact bytes
that shape the output are assembled in one readable place, and the blackboard records
the result verbatim before the call goes out.

THE CONTEXT RULE. Every step gets three things and no more: the GDD's own words for
what it must satisfy, the real source of the contract it binds to, and an existing
file from this project to match the conventions of. Summaries are not passed where the
source will fit -- a paraphrase of §4.7 Stub 8 would be the agent asserting a contract
rather than quoting one, and the whole point of `uiCheckSnapshotFidelity` is that this
project does not trust restatements of that contract.
"""

from __future__ import annotations

import re
from pathlib import Path

from candidates import Candidate
from gdd import GddFacts, find_section
from scan import CodebaseFacts

SYSTEM = """\
You are a code-generation step inside a goal-oriented agent working on Stratocracy, a
turn-based hex strategy game in Unreal Engine 5.8 and C++17.

You write source files. You do not choose what to build -- a priority scorer already
did that -- and you do not decide whether your output is accepted; a human reviews
every file before it lands.

HOUSE STYLE, matched from the files you are shown rather than invented:

- Comments explain WHY, not what. This project's headers carry long reasoning blocks
  naming the alternative that was rejected and the reason. Match that register.
- Never claim something was "measured" or "verified" unless the fact is stated in the
  source you were given. An invented measurement is worse than no comment.
- Tabs for indentation in .cpp/.h under Source/, matching the existing files.
- No hardcoded gameplay values. Anything the scenario or the data tables supply is
  READ from them. This is a graded property of the work, not a preference.
- Unreal conventions: `U`/`F`/`E` prefixes, `UPROPERTY`/`UFUNCTION` where reflection is
  needed, `TEXT()` around literals, `FString`/`TArray` over std types at engine-facing
  boundaries.

OUTPUT FORMAT -- exactly this, once per file, and nothing outside the blocks except a
short plain-text rationale before the first block:

=== FILE: Source/Some/Path.h ===
<the complete file contents>
=== END FILE ===

Emit COMPLETE files. Never abbreviate with "// ... rest unchanged ...".
"""


def _read(code: CodebaseFacts, rel: str) -> str:
    path = code.root / rel
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _slice(text: str, start: str, end: str | None = None, limit: int = 9000) -> str:
    """A region of a source file, bounded by literal markers.

    Whole files are passed where they are small enough to matter; this exists for the
    ones that are not, such as the 1300-line vendored driver whose only relevant part
    is one function.
    """
    i = text.find(start)
    if i < 0:
        return ""
    j = text.find(end, i + len(start)) if end else -1
    return text[i: j if j > 0 else i + limit][:limit]


def _quote(label: str, body: str) -> str:
    if not body.strip():
        return ""
    return f"\n----- {label} -----\n{body.rstrip()}\n"


def _invariants(candidate: Candidate, gddf: GddFacts) -> str:
    out = []
    for i in candidate.closes:
        inv = gddf.invariants.get(i)
        if inv:
            out.append(f"{inv.id}: {inv.text}")
    return "\n\n".join(out)


# ---------------------------------------------------------------------------
# Per-candidate context
# ---------------------------------------------------------------------------

# A candidate is built in one or more STEPS, each its own API call and its own replay
# slot. The scoreboard is split into three because one call could not finish it: asking
# for header, implementation and Automation test together returned
# `stop_reason: max_tokens` with the header complete and the other two never started --
# extended thinking shares the output budget, so a single call's ceiling is reached long
# before three files of this project's comment density are written. Splitting also gives
# each step a prompt containing the previous step's actual output rather than a
# description of it.
STEP_PLANS: dict[str, list[str]] = {
    "bridge_view_model": ["bridge_view_model"],
    "scoreboard_widget": ["scoreboard_h", "scoreboard_cpp", "scoreboard_test"],
    "scoreboard_host": ["host_h", "host_cpp"],
}


def plan(candidate: Candidate) -> list[str]:
    return STEP_PLANS.get(candidate.key, [candidate.key])


def build(
    candidate: Candidate,
    gddf: GddFacts,
    code: CodebaseFacts,
    gdd_text: str,
    step: str | None = None,
    produced: dict[str, str] | None = None,
) -> tuple[str, str]:
    """Returns (system, user) for one generation step.

    `produced` carries files generated by earlier steps of the same candidate, so a
    later step binds to the real bytes rather than to a restatement of them.
    """
    step = step or candidate.key
    produced = produced or {}

    parts: list[str] = [
        f"GOAL: {candidate.title}",
        "",
        f"WHY THIS ONE: the priority scorer selected it. {candidate.gdd_basis}",
        "",
        f"WHERE IT LANDS: {candidate.artifact}",
    ]

    inv = _invariants(candidate, gddf)
    if inv:
        parts += ["", "ACCEPTANCE IDs THIS MUST SATISFY (the GDD's own words):", "", inv]

    if step == "bridge_view_model":
        parts += [
            "",
            "THE PROBLEM, stated precisely. `strat::buildUiSnapshot` takes a",
            "`strat::UiWorld`. The only function in the tree that builds one is the",
            "headless driver's `uiWorldOf(const Session&)` -- and `FStratBridge` owns a",
            "`strat::GameState`, not a `Session`. So no engine-side caller can reach the",
            "view model at all. Close that gap.",
            "",
            "WHAT TO PRODUCE: complete replacement files for StratBridge.h and",
            "StratBridge.cpp that add:",
            "  1. A `UiWorld` built from the bridge's own `GameState` + tables + the",
            "     scenario it loaded. Mirror `uiWorldOf` field for field.",
            "  2. A `UiSnapshot` query that delegates to `strat::buildUiSnapshot` and",
            "     adds NO arithmetic of its own.",
            "  3. Retention of the parsed `strat::Scenario`, because `UiWorld::guided`",
            "     has no other source and `isGuidedMarked` is wrong without it.",
            "",
            "CONSTRAINTS THAT ARE NOT NEGOTIABLE:",
            "- `StratBridge.h` declares NO reflected type (no UCLASS/USTRUCT/UENUM). Its",
            "  header comment explains why: UHT must never be shown the vendored `strat`",
            "  headers. Keep that true.",
            "- Do not modify anything under Source/StratRules/. Those are vendored",
            "  certified bytes and T-INT-01 hash-matches them against a crew commit.",
            "- The `Unit` a `UiUnit` carries is built from the loaded `UnitDef` plus",
            "  current HP, exactly as `combatUnit` does. Look up every stat; store none.",
            _quote("Source/StratBridge/StratBridge.h (replace this file)",
                   _read(code, "Source/StratBridge/StratBridge.h")),
            _quote("Source/StratBridge/StratBridge.cpp (replace this file)",
                   _read(code, "Source/StratBridge/StratBridge.cpp")),
            _quote("Source/StratRules/Ui.h -- the contract you project into (READ ONLY)",
                   _slice(_read(code, "Source/StratRules/Ui.h"),
                          "struct UiUnit", "// T-UI-05", limit=9000)),
            _quote("Source/StratRules/Replay.h -- GameState (READ ONLY)",
                   _slice(_read(code, "Source/StratRules/Replay.h"),
                          "struct GameUnit", "// §4.10's CANONICAL", limit=4000)),
            _quote("Source/StratRules/Driver.good.cpp -- uiWorldOf, the reference "
                   "implementation to mirror (READ ONLY)",
                   _slice(_read(code, "Source/StratRules/Driver.good.cpp"),
                          "static Unit combatUnit", "// Renders §4.7 Stub 8", limit=3000)),
        ]

    elif step in {"scoreboard_h", "scoreboard_cpp", "scoreboard_test"}:
        want = {
            "scoreboard_h": [
                "WHAT TO PRODUCE: exactly ONE file, Source/StratUI/StratScoreboardWidget.h.",
                "",
                "It declares a `USTRUCT` row type carrying the values the scoreboard",
                "shows, and a `UUserWidget` subclass that displays them. Declare the",
                "refresh entry point that takes the bridge and fills the rows. Do NOT",
                "write the .cpp -- it is the next step.",
                "",
                "THE HEADER MUST NOT INCLUDE StratBridge.h. Forward declare",
                "`class FStratBridge;` and include the bridge only from the .cpp. UHT",
                "parses this header because it declares reflected types, and",
                "StratBridge.h pulls in the vendored `strat` headers -- which is exactly",
                "what StratBridge.h's own comment says must never be put in front of UHT.",
            ],
            "scoreboard_cpp": [
                "WHAT TO PRODUCE: exactly ONE file,",
                "Source/StratUI/StratScoreboardWidget.cpp -- the implementation of the",
                "header shown below, which is already committed and must not be changed.",
            ],
            "scoreboard_test": [
                "WHAT TO PRODUCE: exactly ONE file,",
                "Source/StratUI/Tests/StratScoreboardParity.cpp -- the Automation test.",
                "",
                "It must follow the existing pattern exactly:",
                "`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, flags `EditorContext | EngineFilter`,",
                "names of the form `Stratocracy.StratUI.T-UI-03.<Clause>`, loading",
                "`/Game/StratData/DT_Units`, `/Game/StratData/DT_Terrain` and",
                "`Data/ferrum_crossing.json` through the bridge.",
                "",
                "Assert, clause by clause, that every value the widget would display",
                "equals the snapshot field it names -- read from the snapshot, never",
                "recomputed in the test.",
                "",
                "It must ALSO run `strat::uiCheckSnapshotFidelity` over the bridge's own",
                "projection and fail on any reported failure. That is what stops this",
                "widget faithfully displaying a projection that lies.",
            ],
        }[step]
        parts += ["", *want]
        parts += [
            "",
            "THE BINDING RULE, which is the whole of T-UI-03. The widget displays values",
            "it was HANDED. It performs no arithmetic over snapshot fields: no summing,",
            "no differencing, no percentage. If a displayed number is not equal to one",
            "snapshot field, it is computed on the bridge side and handed over as a",
            "field of the row struct -- never in the widget.",
            "",
            "The chevron is a COMPARISON, not arithmetic, and it is still computed",
            "outside the widget: expose it as a field on the row struct. Do NOT add any",
            "field to `strat::UiSnapshot` -- T-UI-05 clause (c) fails a snapshot field",
            "that has no contract entry, and that struct is vendored anyway.",
            "",
            _quote("GDD §2.11.4 -- the scoreboard's rules, verbatim",
                   find_section(gdd_text, "#### 2.11.4 The live Fame scoreboard", 4200)),
            _quote("Source/StratBridge/StratBridge.h -- the view-model surface you bind "
                   "to (READ ONLY, current state of the tree)",
                   _read(code, "Source/StratBridge/StratBridge.h")),
            _quote("Source/StratRules/Ui.h -- the snapshot structs (READ ONLY)",
                   _slice(_read(code, "Source/StratRules/Ui.h"),
                          "struct UiHexView", "// Read-only projection", limit=6000)),
            _quote("Source/StratUI/StratUI.Build.cs -- your module (READ ONLY)",
                   _read(code, "Source/StratUI/StratUI.Build.cs")),
        ]

        # Earlier steps of this same candidate, as the bytes they actually are. A
        # later step that was handed a DESCRIPTION of the header would be free to
        # implement a signature the header does not declare.
        for rel, body in produced.items():
            parts.append(_quote(
                f"{rel} -- generated by the previous step; treat as FIXED and do not "
                f"restate it", body))

        if step == "scoreboard_test":
            parts.append(_quote(
                "Source/StratBridge/Tests/StratBridgeParity.cpp -- the Automation "
                "pattern to copy (READ ONLY, first 200 lines)",
                "\n".join(_read(code, "Source/StratBridge/Tests/StratBridgeParity.cpp")
                          .splitlines()[:200])))
            # THE DECLARATIONS OF EVERY FUNCTION THIS STEP IS TOLD TO CALL. The first
            # run omitted this slice: the step was instructed to call
            # `uiCheckSnapshotFidelity` while the `Ui.h` excerpt above stopped at
            # `// Read-only projection`, which is before the T-UI-05 machinery starts.
            # With no declaration to read, the model invented a plausible return shape
            # -- an iterable of `{id, reason}` instead of a `UiFidelityResult` struct
            # holding `ok` and a `failures` vector of `{clause, field, detail}` -- and
            # said so in its own comment. It did not guess any other API, because every
            # other API it was told to call was in the prompt. A step must be given the
            # declaration of everything it is asked to call.
            parts.append(_quote(
                "Source/StratRules/Ui.h -- T-UI-05 fidelity machinery, the declarations "
                "of uiCheckSnapshotFidelity and the types it returns (READ ONLY)",
                _slice(_read(code, "Source/StratRules/Ui.h"),
                       "enum class UiFieldKind", "// Queries (§4.7 Stub 8)",
                       limit=4000)))

    elif step in {"host_h", "host_cpp"}:
        parts += [
            "",
            "THE PROBLEM. Nothing outside the Automation tests ever constructs an",
            "`FStratBridge`. `StratBridge.Build.cs` says so itself -- 'Nothing in the",
            "game module calls the bridge yet.' So the scoreboard is proven by a test",
            "and cannot appear in a running game: there is no live bridge, no seeded",
            "scenario, and nothing that creates the widget or refreshes it.",
            "",
            "Build the host. An `AHUD` subclass is the natural shape: the map's GameMode",
            "can point at it without the game module ever depending on this one, which",
            "matters because the dependency runs StratBridge -> Stratocracy and cannot",
            "be reversed.",
            "",
            ("WHAT TO PRODUCE: exactly ONE file, Source/StratUI/StratScoreboardHUD.h."
             if step == "host_h" else
             "WHAT TO PRODUCE: exactly ONE file, Source/StratUI/StratScoreboardHUD.cpp,"
             " implementing the header below, which is FIXED."),
            "",
            "IT MUST, on BeginPlay: load the two DataTables, seed the bridge from the",
            "shipped scenario, create the widget, add it to the viewport, and refresh",
            "it. Expose a BlueprintCallable refresh so the game can call it again after",
            "a command.",
            "",
            "CONSTRAINTS THAT ARE NOT NEGOTIABLE:",
            "",
            "1. THE HEADER MUST NOT INCLUDE StratBridge.h. It declares a UCLASS, so UHT",
            "   parses it, and StratBridge.h pulls in the vendored `strat` headers --",
            "   the thing StratBridge.h's own comment exists to prevent. Forward declare",
            "   `class FStratBridge;` and hold it as `TUniquePtr<FStratBridge>`. That",
            "   means an incomplete type at the member declaration, so DECLARE the",
            "   destructor in the header and DEFINE it in the .cpp -- TUniquePtr cannot",
            "   instantiate its deleter against an incomplete type at an implicit",
            "   destructor. Include StratBridge.h only from the .cpp.",
            "",
            "2. NO CROSS-MODULE `strat::` CALLS. The vendored sources carry no _API",
            "   macro, so UnrealEditor-StratBridge.dll exports FStratBridge and nothing",
            "   beside it; a direct `strat::` call from this module fails to LINK with",
            "   LNK2019. This is measured, twice. Everything goes through methods on",
            "   FStratBridge.",
            "",
            "3. NO HARDCODED ASSET PATHS OR GAMEPLAY VALUES. The two DataTables, the",
            "   scenario file name and the viewing side are `UPROPERTY(EditDefaultsOnly)`",
            "   so they are set on a Blueprint subclass, not baked into C++. The widget",
            "   class is `UPROPERTY(EditDefaultsOnly) TSubclassOf<UStratScoreboardWidget>`",
            "   -- never a ConstructorHelpers path literal.",
            "",
            "4. FAIL LOUDLY AND SURVIVE. A missing table or an unreadable scenario logs",
            "   through `LogStratUI` with the bridge's own refusal reason and leaves the",
            "   HUD without a scoreboard. It must not crash and must not display a panel",
            "   of zeroes -- the widget already refuses rather than showing zeroes, and",
            "   this must not defeat that.",
            _quote("Source/StratUI/StratScoreboardWidget.h -- what you spawn and refresh "
                   "(READ ONLY)", _read(code, "Source/StratUI/StratScoreboardWidget.h")),
            _quote("Source/StratBridge/StratBridge.h -- the bridge API, the ONLY way to "
                   "reach the rules (READ ONLY)",
                   _read(code, "Source/StratBridge/StratBridge.h")),
            _quote("Source/StratUI/StratUI.Build.cs -- your module (READ ONLY)",
                   _read(code, "Source/StratUI/StratUI.Build.cs")),
            _quote("Source/StratUI/StratUI.h -- the log category (READ ONLY)",
                   _read(code, "Source/StratUI/StratUI.h")),
            _quote("Source/StratUI/Tests/StratScoreboardParity.cpp -- how the test loads "
                   "the tables and seeds the scenario; do the same thing (READ ONLY, "
                   "first 190 lines)",
                   "\n".join(_read(code, "Source/StratUI/Tests/StratScoreboardParity.cpp")
                             .splitlines()[:190])),
        ]
        for rel, body in produced.items():
            parts.append(_quote(
                f"{rel} -- generated by the previous step; treat as FIXED and do not "
                f"restate it", body))

    else:
        # Every other candidate reaches generation only if the scorer selects it, which
        # it cannot today. Rather than ship a silent fallback that would produce a
        # plausible-looking prompt with no curated context, this refuses loudly.
        raise NotImplementedError(
            f"No prompt context is defined for step '{step}' of candidate "
            f"'{candidate.key}'. Add one to prompts.py before the scorer can select it."
        )

    return SYSTEM, "\n".join(p for p in parts if p is not None)
