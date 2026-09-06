# Gate: unit damage alert, RE-GATE (second pass) — strat-integration-reviewer

Base commit `7e83295`, branch `master`, everything unstaged. Gated 2026-09-06, after the first
gate's `VERDICT: PASS` (`Tools/architect/gate_reports/2026-09-06-unit-damage-alert-gate.md`) was
superseded by edits from three lanes. A PASS describes a tree; that tree no longer exists. This
report describes the tree as it stands now.

## Scope

Working-tree diff against `7e83295`. **14 paths**: 11 modified, 3 untracked.

    Source/StratPlay/StratMatchSubsystem.cpp             M
    Source/StratPlay/StratUnitActor.cpp                  M
    Source/StratPlay/StratUnitActor.h                    M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.h   M
    Source/StratUI/StratSoundCues.cpp                    M
    Source/StratUI/StratSoundCues.h                      M
    Source/StratUI/Tests/StratSoundCueClauses.cpp        M
    Tools/architect/state/engine.md                      M
    Tools/architect/state/global.md                      M
    Tools/architect/state/tests.md                       M
    Source/StratPlay/Tests/StratDamageFlashClauses.cpp   ??
    Source/StratUI/Tests/StratDamagedUnitsClauses.cpp    ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-gate.md ??

`git status --porcelain` shows every modified entry flagged in the SECOND column and none in the
first: nothing is staged. No commit was made. `.agents/ue-project-context.md` was read and needed
no correction — its `StratBridge` row already states the monolithic-only link edge and matches the
derivation below.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — zero hits, not one candidate to judge |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — no `TUniquePtr<FStratBridge>` in the tree |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 field names, derived not quoted |
| 6 | New modules registered (unless no module object) | pass — no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in gameplay C++ | pass — only automation fixtures and prose |
| 9 | No widget-side arithmetic | pass — no widget file changed |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| A | Task check 1 — claimed no-executable-byte-moved | pass on the bytes; the SWEEP behind it is finding 1 |
| B | Task check 2 — corrected claim true, exemption bounded | pass |
| C | Task check 3 — each correction announces itself from the false sentence | **BLOCK** — finding 1 |
| D | Task check 4 — new clause pins a relationship, ID consistent | pass |
| E | Task check 5 — rename complete | pass |
| F | Task check 6 — predecessor pass-specific checks re-run | pass |
| G | Task check 7 — `global.md` honesty and arithmetic | **BLOCK** — finding 2 |

### Instruments, inlined

**Module arrows (checklist 5), run against `Source/*/*.Build.cs`:**

    for f in Source/*/*.Build.cs; do
      m=$(basename "$f" .Build.cs)
      sed 's#//.*##' "$f" | tr '\n' ' ' |
      grep -oE '(Public|Private)(DependencyModuleNames|IncludePaths)\.(AddRange|Add)\([^)]*\)' |
      while read -r line; do
        vals=$(printf '%s' "$line" | grep -oE '"[A-Za-z0-9_/]+"' | tr -d '"' | paste -sd, -)
        printf '%-12s %-30s %s\n' "$m" "${line%%.Add*}" "${vals:-(none)}"
      done
    done

Printed **16 rows**, including `StratBridge  PrivateDependencyModuleNames  StratRules`. That row
was then read in source: `StratBridge.Build.cs:133-137` sets
`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` and the `.Add`
sits inside `if (!bCompileVendoredRulesHere)`. It is the guarded, monolithic-only edge and is NOT
reported as drift. `StratBridge  PublicIncludePaths  StratBridge,StratRules` is the include edge
present in every target. `StratUI  PublicDependencyModuleNames  Core,CoreUObject,Engine,StratBridge`
— no `StratPlay` edge and no `Stratocracy` edge; no cycle. `StratRules` takes `Core`;
`StratPlay` takes `StratUI` public with `StratBridge,UMG,EnhancedInput` private. Every row matches.

**Field census:**

    for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done |
      grep -oE '[A-Za-z]+[[:space:]]*(\.AddRange|\.Add|=)' | sed 's/[[:space:]]*$//' | sort -u

Printed **ten** names, exactly the expected set: `BRIDGE=` (the known false positive, the tail of
the string literal for the vendored-rules define), `PCHUsage =`, `PrivateDefinitions.Add`,
`PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`,
`PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =`. No eleventh name.

**`strat::` call scan (checklist 2):**

    grep -rnE 'strat::[a-z][A-Za-z0-9_]*\s*\(' Source/ |
      grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'

Zero hits.

**Comment-stripped executable diff (task check 1).** The lane claimed a comment-only edit in three
files. Verified independently rather than accepted: a small state-machine stripper was run over
both sides, removing line comments and block comments while respecting string and character
literals (state 0 code, state 1 line comment, state 2 block comment, states 3 and 4 inside a
string or char literal with escape pairs consumed), collapsing to non-blank whitespace-trimmed
lines, and unified-diffing `git show HEAD:<path>` against the working-tree file. Results against
`HEAD` = `7e83295`:

- `Source/StratUI/StratSoundCues.cpp` — **29 added stripped lines, ZERO removed, ZERO changed.**
  All 29 are the body of the new `StratDecideDamagedUnits`. The bound line
  `bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1] = {};` is present unchanged
  on BOTH sides, which is the specific fact the prose correction rests on.
- `Source/StratUI/StratSoundCues.h` — **3 added, zero removed**: the declaration of that function
  and nothing else.
- `Source/StratPlay/StratUnitActor.h` — 19 added, zero removed, zero changed, and all 19 are the
  damage-flash feature the FIRST round landed and the earlier gate already read: the four public
  verbs, the `Shake` `USceneComponent`, four `EditDefaultsOnly` properties, the four private
  functions and `DamageFlashElapsed`. **The comment-only round moved nothing executable.**

The lane phrasing — no executable byte moved in any of the three files — is true of the CORRECTION
ROUND and is not true of those files against `HEAD`. Not a finding: the earlier round bytes were
gated. Stated so a later reader does not test the sentence against the wrong baseline.

**Arithmetic census of `StratSoundCues.cpp` (task check 2).** Over the comment-stripped file, every
line carrying an arithmetic operator character was listed. Nine matched; eight are pointer syntax
(`TMap<int32, const FStratUnitSoundMark*>`, `const FStratUnitSoundMark* const* const Found`,
`*Found == nullptr`, `**Found`). **The single arithmetic operator in the entire file is the `+ 1`
on the `bEmitted` bound.** The replacement claim — *"THIS FILE COMPUTES NO MAGNITUDE ABOUT GAME
STATE. Not an HP delta, not a distance, not a count, not a ratio, not a scale"* — is therefore
TRUE of the file, and it is not narrowed until it protects nothing: it still forbids every delta,
distance, count, ratio and scale, which is the whole of what the old sentence defended. The
exemption is bounded in terms, at the claim itself: *"It is an EXEMPTION AND NOT A LOOPHOLE: it
licenses an index bound over compile-time constants, and nothing else. An operator with a runtime
operand read out of the view model is still forbidden."* It does not license a runtime operand
from the view model.

**Exported report (task check 7 arithmetic).** Read from `Saved/AutomationReport/index.json` with
`utf-8-sig`: `reportCreatedOn 2026.09.06-18.17.58`, `succeeded 445`, `failed 0`, `notRun 0`,
`succeededWithWarnings 0`, entry array 445 long. `MatchEndedIsTheLastSoundCue` is present;
`DamageFlashArmsAShakeAndAnOverlay` is ABSENT and `DamageFlashArmingSetsTheClockTickAndOverlay`
is present. Macro census by `grep -c` on the two automation-test macros returns 6 in
`StratDamageFlashClauses.cpp`, 3 in `StratDamagedUnitsClauses.cpp` and 11 in
`StratSoundCueClauses.cpp` (was 10). Ten clauses added; 435 + 10 = 445. The arithmetic holds.

**Rename completeness (task check 5).** A recursive grep for the old name returns four hits and
**not one is a live clause reference**: the persisted earlier gate report at `:232`;
`global.md:85`, which names the old name as the defect that was fixed; `tests.md:64`, *"RENAMED IN
THE SECOND ROUND FROM ..."*; and `tests.md:160`, the name audit. No reader is sent looking for a
clause that no longer exists. The macro at `StratDamageFlashClauses.cpp:473` carries the new name
and the exported report agrees.

**Pass-specific re-checks (task check 6), all re-run against the CURRENT tree.** A recursive grep
for `SetActorTickEnabled` over `Source/` shows the only `false` argument in the class at
`StratUnitActor.cpp:1167`,
`SetActorTickEnabled(TweenOffsets.Num() > 0 || DamageFlashElapsed > 0.0f);` inside
`UpdateTickEnabled`; the other three call sites (`:509`, `:785`, `:1044`) all pass `true`, which is
the recorded idempotent-arming split. `ApplyUnitView` appears in the diff only inside comment text
— its body is untouched and nothing arms a flash from inside it. In `StratMatchSubsystem.cpp` the
`ApplyView` flash loop closes the `Director` block with its own brace before
`for (const int32 DamagedId : Damaged)`, so it is inside `if (!bTourExistenceHeld)` and OUTSIDE
`FindSoundDirector()`'s `if`; the tour flash in `AdvanceAiPlaybackOneStep` is likewise outside it
and keys on `Current.TargetId`, the defender. `FinishDamageFlash` uses
`Shake->SetRelativeLocation_Direct(FVector::ZeroVector);` followed by
`Shake->UpdateComponentToWorld();`. The extended double adds only accessors — `HasShake`,
`ShakeRelativeLocation`, `BodyOverlayMaterial`, `SetDamageFlashMaterialForFixture`,
`DamageFlashSecondsForFixture` — plus the second fixture class: **no test-only `Tick` advance
seam**. `StratUI.Build.cs` names no `StratPlay`.

**The new clause (task check 4).** `Stratocracy.StratUI.GATE-AUDIO.MatchEndedIsTheLastSoundCue`
writes **no literal count**. Its bound is
`const int64 Bound = static_cast<int64>(EStratSoundCue::MatchEnded);` and its walk is over
`StaticEnum<EStratSoundCue>()->NumEnums()`. Insert an enumerator BEFORE `MatchEnded` and every
value including `MatchEnded`'s rises together, so every value stays within `[0, Bound]` and the
clause stays green — correctly, since `bEmitted` grows with it. Append after `MatchEnded` and the
new value is one past `Bound`, which the loop records as an offender. That is a relationship, not
a magnitude. The only integer literals in the new code sit in CONTROLS — one excluded `_MAX`
sentinel, a walk of more than one enumerator, and a synthetic one-past-the-bound rejection probe —
and none of them is the claim.

**The ID (task check 4, second half).** `global.md:4934-4939` authorizes `GATE-AUDIO` *"for exactly
three named files"*, one of which is `Source/StratUI/Tests/StratSoundCueClauses.cpp`, and
`:4926-4931` names *"the decider's logic (`StratUI`'s `StratSoundCues.h/.cpp`)"* as the first of
its three facets. The new clause's subject is `StratDecideSoundCues`' own `bEmitted` gate — that
facet exactly — and it is written into that authorized file, so nothing was widened and no fourth
file was created. The earlier REFUSAL is consistent rather than contradictory:
`StratDecideDamagedUnits` names the units a board-side VISUAL is applied to, decides nothing
audible, and so rides the existing `T-INT-05` in a new file. **Same test, opposite answers, because
the subject differs — which is what the authorizing text says decides it.** No acceptance ID was
minted by either lane, and both files say so.

## Findings

**1. BLOCK — a seventh site carries the retracted claim, uncorrected, in production source.**

`Source/StratPlay/StratSoundDirector.cpp:117-119`, a file this pass did not touch:

    // THE ONE SUBTRACTION IN THIS FILE, AND IT IS OVER A WALL CLOCK RATHER THAN OVER
    // ANYTHING THE RULES MODULE OWNS. `StratSoundCues.cpp` states that it performs no
    // arithmetic at all; this is the other side of that boundary, where seconds are the
    // only quantity and no rules answer is being re-derived.

`StratSoundCues.cpp` no longer states that, and never truthfully did. This is a CROSS-FILE
CITATION of the exact sentence the pass retracted — the same shape as `StratUnitActor.h:224-229`,
which the pass DID correct with an inline stamp at the quoted words. The lane knew this class of
site existed and fixed one instance while leaving the other standing.

It gates rather than being an observation for three reasons. (a) The sentence is now
affirmatively false about a named file, in `Source/`, and a reader grepping the claim's wording
lands on it. (b) `engine.md:116-118` states as a completed property that *"Each correction
announces itself FROM the false sentence: every site carries a RETRACTED block or an inline stamp
at the words that were wrong, never below them"* — true of the six, so the record asserts a
completeness it does not have. (c) The same entry says the sites were *"Found by grepping the
claim's own wording rather than by trusting the report's list"*; this site PARAPHRASES the claim
(*"performs no arithmetic at all"*) rather than quoting it, which is exactly how a wording-shaped
grep misses a site, and that limitation of the instrument is nowhere recorded.

The fix is the one already applied one file over: an inline stamp at the quoted words, not a
deletion — the surrounding boundary argument is unaffected and still correct.

**Owner: `strat-gameplay-engineer`** (`Source/` outside `Tests/`), together with a correction to
the completeness sentence in `engine.md`, which is that same lane's own record file.

**2. BLOCK — `global.md` states the live suite figure twice and the two disagree.**

`Tools/architect/state/global.md`, in the new damage-alert banner, inside the block quoting the
mid-pass 435/435 measurement:

    [STAMPED 2026-09-06: a MID-PASS reading, superseded within the same pass
    by the live 444/444 above; it is quoted because the whole argument rests on it]

The live figure stated about twenty lines above it in the SAME banner is **445/445**, and 445 is
what `Saved/AutomationReport/index.json` at `reportCreatedOn 2026.09.06-18.17.58` reports and what
the macro census independently returns. `444` was the FIRST round's figure — 435 + 9 — correct
before the second round added `MatchEndedIsTheLastSoundCue`, and not carried forward when the
tenth clause landed. The adjacent sentence carries the same fault: *"the nine clauses added
afterwards are the only thing that moved the figure"* — ten clauses moved it.

This gates rather than being untidiness because `global.md` is, by `CLAUDE.md`, **the only file
that may state a suite count**, and a pointer reading *"the live 444/444 above"* sends a reader to
a figure that is neither above it nor live. It is the exact defect class the neighbouring stamps
in this banner were added to prevent, committed inside one of them.

**Owner: the `coordinator`** (the writer of `global.md`, jointly with `strat-data-steward`).

## Observations

Non-gating.

- **The persisted earlier gate report should STAND unedited**, ruled here rather than left open.
  `Tools/architect/gate_reports/2026-09-06-unit-damage-alert-gate.md:232` cites the old clause
  name. That report is dated evidence of what a gate read in a tree that existed at the time;
  editing it would falsify the record of what that gate actually saw, and the standing rule that a
  gate report must be producible and quotable depends on its immutability. The name is correct AS
  HISTORY. Nothing else in the tree sends a reader looking for the old clause, so no live
  reference is broken.
- **`global.md` and `engine.md` enumerate the six sites differently, and neither list is exact.**
  `engine.md:93-99` names the opening block, the `StratDecideDamagedUnits` `.cpp` block, the
  `StratSoundCues.h` block, the clamp rule, the `StratClampVolume` banner paragraph, and
  `StratUnitActor.h`'s cross-file citation. `global.md` instead names three plus *"a second
  self-asserted re-check in `StratClampVolume`'s banner and a prior `engine.md` entry quoting the
  claim as true"* — five, called six, substituting the `engine.md` entry for `StratUnitActor.h`.
  In fact SEVEN sites were corrected: the six in source plus `engine.md:1600-1601`, which carries
  a bold RETRACTED 2026-09-06 marker immediately BEFORE the false quotation and a bold
  function-locally-only marker inline. Every one of the seven announces itself at or before the
  false words and never below them. The bookkeeping disagreement is a tidiness matter rather than
  a false claim about the tree — but it is how finding 1 stayed invisible: two lists that do not
  agree cannot be checked against each other.
- **`StratDecideDamagedUnits` holds the property its prose claims.** The predicate is
  `Unit.Hp < Before.Hp` and the output is `OutUnitIds.Add(Unit.UnitId)`; there is no subtraction
  anywhere in the function, so the no-magnitude claim survives the very addition most likely to
  have broken it. The join is by `UnitId` through a `TMap`, not by position.
- **`CancelDamageFlash` remains public with no production caller** — carried unchanged from the
  earlier gate's observation (d), recorded there rather than changed. Still non-gating: it is the
  retirement path made reachable from `Tests/` in place of a `Tick` advance seam, and that trade
  was already ruled.
- **The `GATE-AUDIO` clause records its own weaker-form debt honestly.** Both the clause block and
  `tests.md:145-151` state that a `static_assert` against a `Count` sentinel is strictly stronger,
  that it is production code this lane may not write, and that no sentinel was added and none
  should be read as proposed-and-rejected. Open, routed to `strat-gameplay-engineer`.
- **`global.md`'s honesty section (task check 7) is otherwise accurate and unsoftened.** It states
  plainly that the feature is INVISIBLE IN PIE and NO HUMAN HAS SEEN IT; that no clause pins
  amplitude, frequency, X/Y ratio, decay shape, smoothness or redness because `Tick` never runs
  headless; that the marker re-parent is UNMEASURED against `BP_StratUnit` because the editor was
  never opened, and that a green build is not evidence about reinstancing; that the tour path is
  defender-only because no `FStratAiPlaybackStep` field reports counter-damage; and that the
  `static_assert` is not written. Each of those was checked against the tree and each is correct.
  The 435 -> 445 arithmetic is correct where it is stated as the live figure; finding 2 is about
  one stale pointer inside the same banner, not about this section.

VERDICT: BLOCK
