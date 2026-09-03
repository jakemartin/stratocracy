# Gate: unit movement + existence hold, FINAL (third re-gate) — strat-integration-reviewer

## Scope

Working tree of `E:/MultiAgent/Stratocracy`, confirmed by `git rev-parse --show-toplevel`.
No worktree. Base commit `283d711`, nothing staged, nothing committed.
**27 entries** in `git status --porcelain` — 20 modified, 7 untracked (4 new `Tests/` files,
3 prior gate reports). 16 files carry a `git diff` of **5985 insertions / 125 deletions**.

**Nothing inherited.** The twelve-item checklist, the module graph, the field census, the
`.uasset` name-table instrument, the banner probes and the automation report were re-derived
and re-executed in this tree in this pass. The three prior reports were read only to know
which findings were claimed repaired, never as evidence that anything is clean.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — empty output |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass — 0 hits outside; control: 143 hits inside, so the instrument speaks. Type names outside (`strat::GameState`, `strat::Hex`) judged and NOT reported |
| 3 | No vendored header in front of UHT | pass — the only non-`StratRules` header including the vendored headers is `StratBridge.h`, which declares no UCLASS/USTRUCT/UENUM |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — both live members (`StratMatchSubsystem.h:2448`, `StratScoreboardHUD.h:1235`) are `TPimplPtr` |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 field names, both matching. Sixteenth row confirmed guarded |
| 6 | New modules registered | pass — no new module directory; `IMPLEMENT_MODULE` in exactly 3 files, all registered; `StratRules` correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — empty output |
| 8 | No `/Game/` path literal in C++ | pass — every hit outside `Tests/` is a comment except `ImportStratDataCommandlet.cpp:54`, the standing commandlet exception |
| 9 | No widget-side arithmetic | pass — `StratUI` has no file in the working-tree diff at all |
| 10 | Lane discipline | pass — production C++ in the engineer's lane, tests only in `Tests/`, no production file references a test double, one asset in the content lane, each record file written by its own owner |
| 11 | Prose blocks present, measurements recorded as measurements | **BLOCK** — F7, F8 |
| 12 | Nothing staged or committed | pass — `git diff --cached` empty |

### Module graph, derived (not quoted)

    StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
    StratBridge  PrivateDependencyModuleNames   (none)
    StratBridge  PrivateDependencyModuleNames   StratRules      <-- GUARDED
    StratBridge  PublicIncludePaths             StratBridge,StratRules
    StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
    StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
    StratPlay    PublicIncludePaths             StratPlay
    StratRules   PublicDependencyModuleNames    Core
    StratRules   PrivateDependencyModuleNames   (none)
    StratRules   PublicIncludePaths             StratRules
    StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
    StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
    StratUI      PublicIncludePaths             StratUI
    Stratocracy  PublicDependencyModuleNames    Core,...,StratRules
    Stratocracy  PrivateDependencyModuleNames   (none)
    Stratocracy  PublicIncludePaths             Stratocracy,...

Sixteen rows. The `StratRules` link edge is inside the `if (!bCompileVendoredRulesHere)` block at
`Source/StratBridge/StratBridge.Build.cs:135-138`, monolithic targets only — READ, not inferred
from the extraction, which cannot show a guard. Correct and not reported.
No cycle. `StratUI` has gained no `Stratocracy` dependency.

Field census printed exactly the documented ten names, `BRIDGE=` included — the string-literal
tail inside `PrivateDefinitions.Add`, not an eleventh field. No eleventh name.

**One instrument note that is not a tree fact, recorded so the next gate does not chase it.**
Run through `tee /dev/stderr`, both commands rendered garbled — `atBridge` for `StratBridge` and
`DGE=` for `BRIDGE=` — from stream interleaving, not from the tree. Re-run to a file and read
back, the output is exact. Do not tee these two commands; a garbled census is one character away
from looking like an eleventh name.

## The code, asset and module-graph pass — derived from scratch

This section exists because the user asked for it explicitly and because nine findings across
three rounds have all been in prose. It is reported whether or not it found anything.

**The bridge.** `StratRosterObservation` in `StratBridge.cpp` brackets the single
`strat::applyCommand` call. Read `Source/StratBridge/StratBridge.cpp:795-880` line by line:
the delta is cleared on the refusal path BEFORE the `Fail` return (`:820-821`) and filled only
after acceptance (`:847`), so a rejected command cannot inherit its predecessor's delta;
`RosterDeltaOfLastCommand` empties its out-params BEFORE the `bSeeded` guard (`:859-865`), so a
failed call leaves no stale list. `CaptureAfter` returns early with both lists cleared when either
side is unseeded. No path leaves a half-written pair. Correct.

**The tween.** `AStratUnitActor::Tick`, `StratUnitActor.cpp:797-891`. I checked every division
for a zero denominator and every array index for a bound:

- `Duration` cannot be zero: `MoveTweenSeconds <= 0` returns at `:805` and a two-entry minimum on
  `TweenOffsets` is enforced at `:815`.
- A non-positive total arc length returns at `:816`, so the multiply at `:868` is safe.
- The segment scan at `:875` terminates with `Segment <= Num-2`, so `Segment + 1` at `:880` and
  `:890` is always valid. Alpha strictly below 1 is guaranteed by the `:838` return, so the scan
  cannot run off the end.
- A zero-length segment sits at its own start rather than dividing (`:885-887`).

**I verified the trapezoid closed form algebraically** rather than accepting its comment
(`StratUnitActor.cpp:213-233`). With plateau speed `V = 1/(1-E)`: `f(0) = 0`; `f(1) = 1`; at the
first knot `A = E` both branches give `V*E/2`; at the second knot `A = 1-E` the middle branch gives
`V*(1 - 1.5E)` and the decel branch gives `1 - V*E/2`, and multiplying through by `(1-E)` makes
both equal `1 - 1.5E`. The curve is continuous at both knots and exact at both ends for every `E`
in `[0, 0.5]`, and `E` is clamped into that range at the read site (`:862`). The `E <= 0` guard
returns `A`, which is both the divide guard and the correct rectangle-profile answer. The
comment's claims are true.

**The existence hold.** `BuildTourExistenceHolds` and `ApplyTourExistenceAtCursor`,
`StratMatchSubsystem.cpp:1729-1820`. The hoist runs above `RefreshPresentation` in
`RunAiTurnsNow`, which is the only instant `UnitActors` is still the pre-hand-over roster — I
confirmed the call order at `:1686` against the refresh at `:1690`. Visibility is recomputed from
the cursor every time and never applied as a delta, so a skip that jumps the cursor cannot
desynchronise. The permissive defaults make "in neither map" and "in both" fall out with no
special arm. I tested the both-maps case for reachability: an id in the reveal map did not exist
before the hand-over, so it cannot be in `UnitActors`, so the `:1759` guard already excludes it
from the hide map — the explicit drop loop at `:1773-1780` is genuinely redundant, and the comment
says so and says why it is kept anyway. Correct.

**Assets.** One asset changed, `Content/StratPlay/BP_StratUnit.uasset`, real bytes and not an LFS
pointer. Within the content lane.

**Module graph.** Above. No arrow changed.

### Is the clean code pass a property of the wave or a blind spot in the audit?

**Mostly genuine, with one bounded qualification that I would rather state than let the clean
rows imply.**

Genuine, because the four constraints that carry real link-time and startup risk here — no
cross-module `strat::` call, no vendored header in front of UHT, `TPimplPtr` not `TUniquePtr`,
no `/Game/` literal — are mechanically decidable, and I decided them with controls rather than by
absence. And because the C++ itself is written defensively past the point of habit: every divide
in `Tick` has a guard placed specifically so that `Tick` does not depend on another function
having been careful, and `FinishTween` is the single retirement point so there is one answer to
"how does it get back to zero" rather than one per exit. That is a codebase in which a reading
pass finds little because there is little to find.

**What my pass would have had to miss for it to be the second, stated concretely:**

1. **It is a STATIC pass by charter.** I did not build, run the suite, or open PIE. Every defect
   whose only symptom is behavioural is outside my reach. `ArmNextPlaybackStep` computes its
   interval as the step seconds plus the last armed slide seconds; a one-frame accumulation drift
   across 150 steps is not readable and I did not measure it.
2. **A plausible-but-wrong constant reads exactly like a right one.** I proved the trapezoid is
   mathematically exact; I cannot tell you that an ease fraction of `0.25f` looks right on screen.
   Only the user at the keyboard has answered that class of question in this wave, and did.
3. **I read the diff, not the tree.** A defect in unchanged code that new callers newly REACH
   appears in no hunk. `BuildTourExistenceHolds` reaching `UnitActors` at a new point in
   `RunAiTurnsNow` is exactly that shape; I judged it by call order and prose, not by execution.
4. **Anything resting on an authored asset value is invisible to me** — which is not hypothetical
   here. It is the open question this whole wave keeps failing to close, and it is where F7 lives.

So the finding rate is low in code because code has a compiler, a linker and 386 clauses checking
it, and high in prose because prose has nothing checking it but a gate. That is the honest shape
of it. But rows 1-10 certify a reading, not an execution, and the suite verdict is
the test lane's, not mine.

## Findings

### F7 — BLOCK. A measurement claim in `engine.md` that is false of this tree, in the bullet that characterises the instrument the wave's rulings depend on.

`Tools/architect/state/engine.md:220-225`:

> `BP_StratUnit` behaves identically: every editable property returns exactly one, **including
> both `GuidedMarkerMesh`, which this record says WAS assigned on 2026-08-24, and
> `FlagMarkerMesh`, which the file says is unset.** The same instrument returns the same answer
> for a property known set and a property claimed unset, so **on the actor Blueprints it
> discriminates nothing** and no conclusion may be drawn from a hit in either direction.

**"every editable property returns exactly one" is false.** Re-run with the record's own stated
instrument, `grep -aoF` over the raw bytes of `Content/StratPlay/BP_StratUnit.uasset`, with a
fabricated name as the negative control and two known-present names as positive controls:

    MoveTweenEaseFraction    0     <-- EditDefaultsOnly, StratUnitActor.h:996-997
    FlagMarkerOffset         0     <-- EditDefaultsOnly, StratUnitActor.h:862-863
    UnactedPipOffset         0     <-- EditDefaultsOnly, StratUnitActor.h:867-868
    GuidedMarkerZOffset      0     <-- EditDefaultsOnly, StratUnitActor.h:706-707
    MoveTweenSeconds         1
    FlagMarkerMesh           1
    GuidedMarkerMesh         1
    ZZZFabricatedName        0     <-- negative control

Four of the fourteen `EditDefaultsOnly` properties on that class return ZERO. The dispatching seat
observed one of these; there are four.

**Why this is a finding and not a nitpick.** The false sentence is the sole warrant for the
conclusion drawn from it, and the conclusion is over-broad in a way that DESTROYS USABLE EVIDENCE.
The correct characterisation is the one the four zeros force:

- **Presence proves nothing on this asset** — `GuidedMarkerMesh` (known set) and `FlagMarkerMesh`
  (claimed unset) both return 1. This half of the finding is real and survives intact.
- **Absence IS decisive** — the four zeros are the positive control the instrument was previously
  missing on this asset, and they establish that those four properties carry NO Blueprint
  override, so the C++ defaults are the shipped values.

The record currently says "no conclusion may be drawn from a hit **in either direction**," which
writes off the half that works. That has an immediate cost: `MoveTweenEaseFraction` reading absent
settles, from the bytes alone, that the trapezoid the user just watched runs at the C++ `0.25f` —
a question the record hands to the content lane as unanswerable.

**Owner: `strat-gameplay-engineer`**, whose file and whose bullet it is. The repair is prose in one
file: correct the count, correct the conclusion to "presence proves nothing here, absence is
decisive", and re-check whether any of the five UNRULED sites is settled by an absence. No
production byte, no rebuild, no suite run.

**Note on what does NOT change.** The refusal to rule the five *"unset is the state this ships in"*
sites (`engine.md:226-236`) is **CORRECT and I endorse it**, and F7 does not disturb it. All five
concern properties that read PRESENT, which is the direction that proves nothing. Ruling them
defects on a hit that proves nothing would have fabricated findings; ruling them correct would have
been equally unwarranted. Declaring them unmeasurable-from-that-lane, naming them so they are
greppable, and handing them to the lane that can read the asset is the right call. The
`BP_StratGameMode` half of the control also stands: I did not re-measure it, but the asymmetry the
engineer reports — `bRecordCompletionOnMatchEnd` present in one GameMode and absent from the other
— is a genuine within-class discrimination. I did re-measure `BP_StratBoard`, where all nine
editable properties return 1 and the two zeros are non-editable runtime members, matching the
record.

### F8 — BLOCK. The superseded warrant is corrected in a new bullet and left standing, unstamped, in the old one.

`Tools/architect/state/engine.md:35-36`:

> from all three. So the instrument discriminates per asset AND per property in both directions.
> **A NAME-TABLE HIT PROVES THE PROPERTY IS SERIALISED, NOT WHAT IT HOLDS**, ...

Both sentences are refuted by the within-class control recorded 165 lines below at `:201-236`, and
neither is stamped. A hit does NOT prove the property is serialised on the actor assets — that is
precisely what the new bullet measured — and "in both directions" is the exact over-claim it
withdraws.

This is the project's own named failure mode, *a correction that only adds leaves both claims
live*, and it is the same shape as F4 and F6 in the two prior rounds: a reader arriving at
`engine.md:35` by citation lands on an unstamped sentence asserting the withdrawn warrant, with
nothing on the bullet pointing down to the correction. Inline stamping is required here for the
reason this record states about itself elsewhere — a citation lands on the sentence, not on any
header and not on any later bullet.

**Owner: `strat-gameplay-engineer`**, same file, same pass as F7. Prose only.

**A near-miss checked and NOT raised.** `tests.md:29-32` carries a similar sentence, but its
conclusion is scoped to *"rather than answering PRESENT to everything"*, which the four zeros
confirm rather than refute. The test lane's wording survives the new control. I re-read it
specifically to see whether F8 propagated, and it does not.

## Observations

Non-gating.

**1. The F6 discharge condition IS now sufficient, and I am ruling on it explicitly because that
was asked.** `engine.md:162-164` reads *"DISCHARGED WHEN the pass that rewrites the enumerated
`AiPlaybackStepSeconds` blocks ALSO rewrites these three,"* with a `[SCOPE AMENDED]` pointer at
`:123-127` on the other bullet so a reader landing on the original condition is sent to the widened
one. Test: can the condition be satisfied today while a known site of the same class stands? No —
the three sites are named, and the widened sweep's partition shows the defect-class set is exactly
the six now enumerated. The claim at `:164` is also correctly scoped: it says the condition "can no
longer be satisfied while they stand, which is the only thing F6 asked for", and does not
over-claim that the condition became derivation-shaped. **The residual, which is not a finding:**
the condition is still an ENUMERATION, and the discharging pass is by definition a pass that
changes this module's code, so a site added between now and discharge would not be covered.
Cheapest close is to add "and the widened sweep re-runs clean" to the condition.

**2. The widened sweep's partition audits correctly.** I checked the rulings rather than their
existence. The axis correction is right and worth having been recorded: the discarded first run
added a live/actual/in-practice vocabulary, which widens the VOCABULARY, whereas F6 was about the
claim not needing the word `default` — dropping that co-token requirement is the correct axis, and
393 unrulable hits is the right reason to discard the other. The four "correct as written" groups
are sound classes, and I spot-checked the fourth: `StratUnitActor.h`'s *"the shipped duration goes
on `BP_StratUnit`"* does name the Blueprint as the authority and is the pattern the deferred
repairs move toward, as claimed.

**3. The sweep's declared non-`Tests/` scope leaves one live claim of the same class unswept.**
`Source/StratPlay/Tests/StratRouteTweenUnitDouble.h:5-6`, new this wave: *"the shipped values live
on `BP_StratUnit`"*, of `MoveTweenSeconds` and `BodyZOffset`. For `MoveTweenSeconds` this is
corroborated independently — the tween path is inert at a non-positive duration per
`StratUnitActor.h`, and the user confirmed at the keyboard that units slide. For `BodyZOffset` it
rests on byte-presence alone, which F7 shows proves nothing on that asset. Not a finding: the
sweep's scope is honestly declared as non-`Tests/`, and the file's load-bearing claim — that the
C++ defaults are zero, so a subclass is the only route from inside `Tests/` — is true and verified.
Worth settling in the same content-lane reading.

**4. The `MoveTweenSeconds` warrant the dispatching seat self-reported is confirmed, and I found no
third instance.** I swept all four record files and the three prior gate reports for name-table
warrants. The GameMode-asset claims — `AiSides` serialised in both — sit on the assets where the
instrument demonstrably discriminates and are sound. The only unstamped actor-asset warrants are
`engine.md:35-36`, which is F8. The retraction of the `MoveTweenSeconds` byte-presence warrant is
correct, and that conclusion does survive on the independent evidence it names.

**5. All four standing open items are still correctly declared, none quietly closed.** The content
lane's six-site asset reading (`engine.md:139` and `:226-236`); `StratAiMatchClauses.cpp:380`
(`engine.md:138`); the `const` accessor discharge for
`AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep` (`engine.md:417` and `global.md:51-58`); and the
two undischarged mutants on `GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder`
(`tests.md:282-296`, which states in terms that neither was built and neither may be cited as
discharged).

**6. `global.md` re-verified adversarially, probes re-run rather than cited.** Copied the six state
files to a scratch directory and ran `strat_banner_sweep.py` against the copies.
Control, unmutated copies: `SWEEP CLEAN`, exit 0.

- **Stale live citation.** Live `reportCreatedOn 2026.09.03-19.47.45` changed to the dead
  `2026.09.03-14.05.36`: `SWEEP FAILED`, **exit 1**. Correct.
- **Wrong live count.** `386/386` changed to `387/387`: `SWEEP FAILED`, **exit 1**. Correct.
- **The 400-character margin is load-bearing and is the ONLY thing preventing inertness.** Same
  stale citation as the first probe, with a `[STAMPED` marker placed immediately before it:
  `SWEEP CLEAN`, **exit 0** — the stale live citation is excused. `global.md:33-42` describes this
  exactly and it reproduces exactly. Nothing may be inserted between that citation and the stamped
  block below it.

**7. Independent re-measurement of every figure in the dispatch brief. All four hold.** Exported
report `Saved/AutomationReport/index.json`, read with `utf-8-sig`: **386 succeeded, 0 failed,
0 notRun**, 386 test entries, `reportCreatedOn 2026.09.03-19.47.45`. Macro census by independent
count: **386**. `strat_banner_sweep.py` run without a pipe: `SWEEP CLEAN`, **exit 0**, 59 suite
claims of which 1 live and 58 stamped. `git status --porcelain`: **27 entries**, nothing staged.

## Handover

**What a reader picking this tree up cold must know.**

1. **The tree is UNCOMMITTED at base `283d711`, 27 entries, nothing staged.** The suite is 386/386
   at `reportCreatedOn 2026.09.03-19.47.45`, and that report is the only artifact behind the
   figure — one report path, so re-running the suite invalidates every citation of it.
2. **The code is clean by this gate's reading and has been for four rounds.** Every finding in this
   wave, all eleven now, has been in prose. Rows 1-10 of the checklist certify a reading, not an
   execution.
3. **The `.uasset` name-table instrument is HALF good on actor Blueprints, and this is the single
   most useful thing learned this wave.** On `BP_StratGameMode` and `BP_StratGameMode_AiVsAi` it
   discriminates fully. On `BP_StratUnit` and `BP_StratBoard`, **presence proves nothing, absence
   is decisive.** Do not read the current `engine.md:224` sentence, which says neither direction
   works — that is F7.
4. **The 400-character margin in `global.md` is not stylistic.** Inserting a `[STAMPED` marker near
   the live citation silently disarms the sweep's provenance check. Measured again today.

**Blocking versus declared debt.**

**BLOCKING before the user commits — F7 and F8.** Both are prose in one file, `engine.md`, owned by
`strat-gameplay-engineer`, requiring no code change, no rebuild and no suite run. I am calling them
blocking rather than debt for one reason: a debt is a known-unfinished thing correctly declared,
and these are two false statements about a measurement. F7 in particular does not merely
under-deliver — it instructs the next reader that a working instrument does not work, and the next
reader is the content lane, which is the lane holding six open questions this instrument can now
partially answer. Carried forward it costs another round to re-derive; fixed, it is a few
sentences.

**Declared-and-scoped debt, correctly recorded, NOT blocking:**

- The six content-lane asset sites — `AiSides` on `BP_StratGameMode`; `FlagMarkerMesh`,
  `UnactedPipMesh` and the markers-art bullet on `BP_StratUnit`; `BuildPulseMaterial` in
  `StratBoardActor.h` and in `BeginPlay`. Note that F7's repair may settle some of these from the
  bytes alone.
- The five UNRULED shipping-state sites — correctly refused, correctly named.
- The `const` accessor that would properly pin
  `AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep`; the clause is currently pinned by a different,
  working mutation and the inert one is declared inert.
- The two undischarged mutants on `GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder`, both named
  line by line, both correctly declared not-cited-as-discharged.
- The F6 discharge condition's enumeration shape (Observation 1).
- `StratRouteTweenUnitDouble.h`'s `BodyZOffset` claim (Observation 3).

**Reviewer note.** This report is a reading of an uncommitted tree by a read-only gate. I ran no
build and no suite; the 386/386 figure is the test lane's and `global.md`'s, re-measured here from
the exported artifact but not re-executed.

VERDICT: BLOCK
