"""The candidate set -- what the agent is allowed to consider building.

It is SEEDED FROM THE GDD, not from a wishlist. The agent takes every acceptance ID
the GDD declares, subtracts the ones a test in the tree already asserts, and is left
with the open ones. Each open ID names a subject -- the thing that would have to exist
for that ID to close -- and this file is the map from ID to subject.

WHY THAT MAP IS WRITTEN DOWN RATHER THAN INFERRED. An acceptance ID says what must be
TRUE, not what must be BUILT: `T-UI-03` says the scoreboard binds 1:1 to snapshot
fields, and no parser turns that into "author a UUserWidget in a module that can see
the bridge". The GDD does state the answer, in prose, in §4.11 and §4.9 -- so each
entry below carries the sentence it is derived from in `gdd_basis`, and the blackboard
prints it. A reader can then check the map against the document instead of taking it
on trust. Inventing a subject the GDD does not name would be the failure mode this
guards against; several entries here exist precisely to record that the GDD names NO
subject, and those are marked `unruled` and are excluded from selection.

PREREQUISITES ARE NOT FROM THE MAP. They come from `scan.py`'s capability probes --
what the tree can actually reach today. That is the half a document cannot supply.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from gdd import GddFacts
from scan import CodebaseFacts


@dataclass
class Candidate:
    key: str
    title: str
    # The acceptance IDs this capability would let close, once it exists.
    closes: list[str]
    # The scan probe that answers "does this already exist?".
    probe: str
    # Probes that must be PRESENT before this is buildable. Dependency order.
    requires: list[str]
    # Where the work lands, for the blackboard and the write step.
    artifact: str
    # The GDD sentence this subject is derived from.
    gdd_basis: str
    # True when the GDD deliberately leaves the shape unstated. Never selected:
    # choosing a shape here would pre-empt a Director ruling.
    unruled: bool = False
    unruled_reason: str = ""

    exists: bool = False
    blocked_by: list[str] = field(default_factory=list)
    open_ids: list[str] = field(default_factory=list)


# The subject map. Each entry cites the GDD text it comes from.
_SUBJECTS: list[Candidate] = [
    Candidate(
        key="bridge_view_model",
        title="Bridge view-model surface: GameState -> UiWorld -> UiSnapshot on FStratBridge",
        closes=[],   # closes nothing on its own; it is what the others stand on
        probe="gamestate_to_uiworld",
        requires=[],
        artifact="Source/StratBridge/ (projection + retained scenario)",
        gdd_basis=(
            "§4.9: the bridge owns the authoritative `strat::GameState`. §4.7 Stub 8: "
            "`buildUiSnapshot(UiWorld)` is the only projection, and the widgets bind to "
            "the view model it produces. The tree supplies `uiWorldOf(Session)` only, "
            "which takes the driver's type and not the bridge's."
        ),
    ),
    Candidate(
        key="scoreboard_widget",
        title="Live standings scoreboard widget (§2.11.4)",
        closes=["T-UI-03"],
        probe="scoreboard_widget",
        requires=["ui_module_exists", "gamestate_to_uiworld"],
        artifact="Source/StratUI/ + Source/StratUI/Tests/",
        gdd_basis=(
            "§4.11 row 8: what the row's flip waits on are 'the real Stratocracy widgets "
            "those IDs assert against, which are measured absent at a13626f'. T-UI-03 "
            "names the live standings scoreboard as its subject."
        ),
    ),
    Candidate(
        key="production_widget",
        title="Production menu widget (§2.11.5)",
        closes=["T-UI-04"],
        probe="production_widget",
        requires=["ui_module_exists", "gamestate_to_uiworld", "buildlist_query"],
        artifact="Source/StratUI/",
        gdd_basis=(
            "§4.11 row 8, same sentence as the scoreboard; T-UI-04 names the production "
            "menu, binding to 'the buildlist derived from the four Stub-2 unit rows plus "
            "current fameTotal'."
        ),
    ),
    Candidate(
        key="buildlist_query",
        title="Buildlist query on the Ui.h contract",
        closes=[],
        probe="buildlist_query",
        requires=[],
        artifact="stratocracy-crew/cpp_reference/Ui.h (vendored -- outside this repo)",
        gdd_basis=(
            "§4.7 Stub 8: 'There is deliberately no third query: T-UI-04's buildlist has "
            "no stated shape -- field or query -- and inventing one here would pre-empt a "
            "Director ruling.'"
        ),
        unruled=True,
        unruled_reason=(
            "shape unstated in the GDD by explicit decision, and the file is vendored "
            "certified bytes in another repo -- T-INT-01 hash-matches it"
        ),
    ),
    Candidate(
        key="bridge_event_list",
        title="Bridge ordered event list (§4.9 'command in / events out')",
        closes=[],
        probe="bridge_event_list",
        requires=[],
        artifact="Source/StratBridge/",
        gdd_basis=(
            "StratBridge.h: 'NOT IN THIS ROUND: the ordered event list (§4.9 part 2's "
            "command in / events out) and the widgets... The event list is ruled to live "
            "headless, and no acceptance ID names it as its subject.'"
        ),
    ),
    Candidate(
        key="scoreboard_host",
        title="Runtime host: own a bridge, seed the scenario, put the scoreboard on screen",
        closes=[],   # closes nothing itself; it is what a runtime assertion stands on
        probe="scoreboard_host",
        requires=["ui_module_exists", "gamestate_to_uiworld", "scoreboard_widget"],
        artifact="Source/StratUI/ (HUD or subsystem)",
        gdd_basis=(
            "§4.9 names the bridge as the only code that knows both worlds, and §4.7 "
            "Stub 8 has widgets bind to the view model it projects. Nothing outside the "
            "Automation tests constructs an FStratBridge -- StratBridge.Build.cs says so "
            "in as many words ('Nothing in the game module calls the bridge yet') -- so "
            "no widget can be fed at runtime, only in a test."
        ),
    ),
    Candidate(
        key="presentation_statelessness",
        title="Presentation statelessness pass (rebuild widgets from the view model)",
        closes=["T-INT-05"],
        probe="scoreboard_widget",   # it asserts OVER widgets; no widgets, nothing to assert
        # `scoreboard_host` is a REAL prerequisite and not a convenience. T-INT-05 asserts
        # that widgets can be REBUILT from the current view model after any event
        # sequence; that quantifies over a runtime path which builds them from the view
        # model in the first place, and no such path exists while the only code
        # constructing a bridge is a test. Added 2026-08-11 after the widget landed and
        # the gap became visible -- the candidate set is edited when the tree teaches
        # something, which is the same reason the probes were.
        requires=["ui_module_exists", "gamestate_to_uiworld", "scoreboard_widget",
                  "scoreboard_host"],
        artifact="Source/StratUI/Tests/",
        gdd_basis=(
            "§4.11 row 9: 'T-INT-05 did not run, and what it lacks is the real "
            "Stratocracy widgets it asserts over (§4.9).'"
        ),
    ),
]


def build(gdd_facts: GddFacts, code: CodebaseFacts) -> list[Candidate]:
    """Instantiates the candidate set against the current tree.

    An ID counts as OPEN when the GDD declares it and no test in the tree asserts it.
    A candidate whose every `closes` ID is already asserted is dropped: there is
    nothing left for it to buy.
    """
    declared = set(gdd_facts.all_ids())
    asserted = set(code.asserted_ids)

    out: list[Candidate] = []
    for spec in _SUBJECTS:
        c = Candidate(**{k: v for k, v in vars(spec).items()
                         if k not in {"exists", "blocked_by", "open_ids"}})
        c.exists = code.has(c.probe)
        c.open_ids = [i for i in c.closes if i in declared and i not in asserted]
        c.blocked_by = [r for r in c.requires if not code.has(r)]

        # Already built, and everything it was for is asserted -- no work here.
        if c.exists and not c.open_ids:
            continue
        out.append(c)
    return out
