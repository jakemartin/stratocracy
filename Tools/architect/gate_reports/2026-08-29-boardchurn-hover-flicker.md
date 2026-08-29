# Gate: hover-flicker / board no-churn early-out — strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`, HEAD
`1a3520b1899bb3f3928a5114cab263a03b909a0f`. `git rev-parse --show-toplevel` printed
`E:/MultiAgent/Stratocracy`, so the tree audited is the one named in the dispatch and not a
worktree; every command was run from that absolute path.

`git status --short` matched the expected list exactly — 6 modified, 1 untracked, nothing
staged. **7 files: 2 production C++, 1 new test file, 4 record files.**

**What I did NOT measure, stated before the findings.**

- **I did not build.** No `Build.bat`, no `vcvars`. The engineer's `Result: Succeeded` is
  unverified by me. A C++ gate on this box SKIPs and exits 2 without vcvars, and a skip is
  not a pass.
- **I did not run the suite.** The `286/286` below is corroborated two ways that are both
  artifact reads, not executions: `strat_banner_sweep.py` parsed the persisted automation
  report and reported *286 entries, all Success (2026.08.29-22.09.14)*, and my own macro
  census over `^Source/**/*.cpp` counted 286 (279 tracked at HEAD, +7 in the new file).
  Neither is me running the suite.
- **I did not reproduce the falsifiability proof.** The disposable mutant tree
  `E:/MultiAgent/Strat-mutant` is deleted, so the RED-against-five-mutants claim in
  `tests.md` is **not reproducible from this checkout**. UNVERIFIED. I read the clause
  bodies and satisfied myself the instrument is sound in shape, but that is a reading.
- I did not open the editor and did not look at a viewport. Nothing here speaks to whether
  the flicker is gone to a human eye.
- I did not open `.agents/ue-project-context.md`; the module graph below was derived from the
  five `.Build.cs` files, so nothing here depends on that note — but I cannot say whether it
  has drifted.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing; `git status --porcelain` on those paths printed nothing, so no untracked plants either. Manifests derived, not quoted: `rulesCommit 96d93ea9…`, `dataCommit c2f58608…`. |
| 2 | No `strat::` call outside StratBridge/StratRules | pass, **judged** — the call-shaped grep returned zero hits outside the two directories. A looser `strat::[a-z]` sweep returned ~30 hits in `StratPlay`/`Stratocracy` and I read every one: all are inside comment prose. Type names (`strat::UiSnapshot` ×69, `strat::GameState` ×31) are legal and are not reported. `strat::uiResolveForGate` is called only in `Source/StratBridge/Tests/StratCombatOutcomeParity.cpp` — permitted directory, still un-bridged, still the independent oracle. |
| 3 | No vendored header in front of UHT | pass — the only header including `Data.h`/`Replay.h`/`Save.h`/`Scenario.h`/`Ui.h` is `Source/StratBridge/StratBridge.h:53-57`, which declares no `UCLASS`/`USTRUCT`/`UENUM` (anchored grep; its four hits are prose). The new test file includes `StratBridge.h` at line 99 — a `.cpp`, the required shape. |
| 4 | `TPimplPtr`, not `TUniquePtr` | pass — `StratMatchSubsystem.h:1973` and `StratScoreboardHUD.h:1093` are both `TPimplPtr<FStratBridge>`. The only `TUniquePtr<FStratBridge>` token in the tree is `StratViewModel.h:47`, recording the measured `C4150` in prose. |
| 5 | Module arrows unchanged and correct | pass — **derived, not trusted.** The extraction block was copied out of the checklist and executed; it printed **15 rows, three per module, five modules**, so no module is silently missing. Every row matches the expected graph. `StratRules` appears in `StratBridge`'s `PublicIncludePaths` only and in neither dependency array — the correct state, not reported. All 12 `Source/StratBridge/Vendored/*.strat.cpp` still present and still compiled there. `StratUI` has gained no `Stratocracy` dependency. No cycle. No `.Build.cs` is in the diff. The field census printed exactly **six** names — the three graph fields plus `PCHUsage`, `bUseUnity`, `ShadowVariableWarningLevel`. No seventh. |
| 6 | New modules registered | pass, vacuously — no new `Source/<Module>/`. The new `.cpp` is in `Source/StratPlay/Tests/`, inside an existing registered module. `Source/StratRules/` remains correctly absent from the `Modules` array and correctly carries no `IMPLEMENT_MODULE`. |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` printed nothing; no untracked file there. |
| 8 | No `/Game/` literal in gameplay C++ | pass — every `"/Game/` literal in `Source/` is inside a `Tests/` directory. The new file's two (`StratBoardChurnClauses.cpp:135-136`) are fixture table loads matching every sibling fixture. No `ConstructorHelpers` in the diff. |
| 9 | No widget-side arithmetic | pass, vacuously — `Source/StratUI/` is not in the diff. Not re-audited beyond that. |
| 10 | Lane discipline | **BLOCK** — see F3, F4. The code lanes are clean: engineer touched only `StratBoardActor.{h,cpp}` + `engine.md`; test-author only `Tests/StratBoardChurnClauses.cpp` + `tests.md`; no production code in `Tests/`, no test code outside it, no asset. The violations are in the record files. |
| 11 | Prose blocks present | **BLOCK** — see F1. Blocks are present and unusually good; one records a property the code does not have, and this pass propagated it into the engineer's record. |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` printed nothing; HEAD is still `1a3520b`. |

## Findings

### F1 — BLOCK. Production prose asserts a property the board does not have, and this pass propagated the claim into the record.

**Constraint:** measurements are recorded as measurements; a comment block may not assert a
property the code lacks.

`Source/StratPlay/StratBoardActor.cpp:230-231`, inside `AStratBoardActor::DrawsExactlyTheseHexes`:

>     // off the end of an empty array here and the rebuild runs -- which is what makes a
>     // mesh assigned after the fact take effect on the very next refresh.

**Confirmed false of the board.** `AStratBoardActor::LayerFor` (`StratBoardActor.cpp:132-140`)
returns an existing layer by `TerrainId` *before* reaching the `SetStaticMesh` calls at
`:166-173`, so a layer created during an unmeshed apply keeps a null mesh forever. The early-out
does correctly return `false` on the second apply and the rebuild does run — and then
`ApplyHexes` at `:296` sees `GetStaticMesh() == nullptr` again and `continue`s past every hex.
The sentence is true of the *early-out* and false of the *board*.

**The sibling, which is new in this pass and was not in the brief.**
`Tools/architect/state/engine.md:74` carries the same claim as a measured fact in the
engineer's own record:

>     assigned later take effect on the next refresh instead of never. A layer still holding

It sits under the heading **"THE EARLY-OUT IS CONSERVATIVE BY CONSTRUCTION AND THE THREE WAYS
IT COULD HAVE LIED ARE CLOSED IN THE COMPARISON ITSELF."** The record now states as closed a
way the board still lies, while `tests.md` states in the same change set that it is open. Two
files written this pass disagree about the same fact, and the record is the one that is wrong.

**Checked and cleared, not siblings.** `StratBoardActor.h:485` ("assigned afterwards never take
effect -- so the miss case resets these to empty") is about the OVERLAY cache and is **earned**:
`FillOverlay` reads `GetStaticMesh()` live on every call and resets `DrawnHexes` on the miss,
and clause 7 covers it. `StratBoardActor.h:194` ("the rebuild runs and reports again the moment
a mesh arrives") is also true as written — the rebuild does run and does re-report.

**Owner: `strat-gameplay-engineer`**, for both the `.cpp` comment and `engine.md:74`. The
underlying `LayerFor` defect is the user's deferral and is NOT what this blocks on; the prose
that denies it is.

### F2 — BLOCK. `global.md` asserts a state of `engine.md` that is false of this tree, through a citation the same change set invalidated.

**Constraint:** the record may not carry a false claim about the tree, and a citation must resolve.

`Tools/architect/state/global.md:2796`, inside the new `[STAMPED AGAIN 2026-08-29 …]` block:

>     retracts it. `engine.md:293` still STATES the condition in the future tense and is the
>     `strat-gameplay-engineer`'s file, not this writer's, so it is named here rather than edited.

Both halves are false of the working tree.

1. **The citation does not resolve.** `engine.md:293` is now mid-paragraph in the W4
   map-markers entry ("…in the lane worktree `E:/MultiAgent/Strat-wt/slot-2`…"). The
   `DISCHARGED WHEN` condition moved to `engine.md:385-387`, because the new 92-line entry was
   appended *above* it in this same change set. This is the recorded "citation into an
   append-above document" failure, with the citation and the edit that broke it landing together.
2. **It has been edited.** `engine.md:388-397` carries a `[DISCHARGED 2026-08-29, BOTH HALVES…]`
   stamp discharging exactly that condition. `global.md` says the file was "named here rather
   than edited"; it was named here *and* edited.

**Owner: `coordinator`** (`global.md` is its file). Fix: restate the block flat — never nested —
citing the condition by its text or by the entry it sits in rather than by a line number, and
record that the stamp landed.

### F3 — BLOCK. The `engine.md` discharge stamp names no writer.

**Constraint:** CLAUDE.md's separate-attribution requirement, inline in the entry; and "an
out-of-lane write is attributed where it landed."

`Tools/architect/state/engine.md:388`:

>     **[DISCHARGED 2026-08-29, BOTH HALVES, STAMPED HERE RATHER THAN DELETED.** The editor batch

Every other stamp across these four files names its writer ("BY THE `coordinator`"). This one
names none — and the fact it records, a human confirming marker placement at the keyboard, is
content-lane and coordinator-facing rather than engine-lane. Combined with F2, the tree
affirmatively asserts that the `coordinator` did *not* edit this file while an unattributed edit
sits in it. **Authorship is unresolvable from this checkout**, which is exactly the legibility
the transcription clause's limit (3) exists to protect, and the same axis a gate blocked on
earlier today.

**Owner: `coordinator`** to resolve, since it is the writer F2 puts in question. If the engineer
wrote it, say so; if the coordinator did, name the clause that licenses it.

### F4 — BLOCK. The `content.md` stamp cites no licence and shows no control.

**Constraint:** `content.md`'s own header — sole writer `strat-editor-builder`, fallback writer
`coordinator` under one condition, with **two** obligations.

`Tools/architect/state/content.md:170-180`:

>     **[STAMPED 2026-08-29, LATER THE SAME DAY, BY THE `coordinator`: THE CONDITION IS NOW
>     MET AND THE DOUBT THIS BULLET RAISED WAS NOT BORNE OUT.**

Obligation (2) — naming the coordinator as writer — **is met**. Obligation (1) — *"measure the
absence with a control before invoking it … show the same lookup returning the tools that ARE
served"* — **is not**, and the entry cites no authority for the write at all. It is not a
transcription either: it names no merge commit, and it authors new content, since it *retracts*
the record's own crowding reservation rather than carrying across what a lane wrote. The file's
header says in terms that an entry muddling its authorities is a finding.

**Owner: `coordinator`.** Cheapest fix: add the control measurement, or state that the
fallback / editor-driver condition was measured in `1a3520b` and cite that entry.

## Observations

Non-gating.

**1. Can the board's instance state and `InstanceHexes` diverge with the early-out vouching for
it? I found no such path — the reasoning, not the reassurance.** The early-out **writes
nothing**: `DrawsExactlyTheseHexes` is `const`, and `ApplyHexes` returns at `:264` before the
first `ClearInstances` at `:280`. So it cannot *create* a divergence; it can only decline to
*correct* a pre-existing one. A pre-existing one would have to survive the `checkf` at
`StratBoardActor.cpp:316` asserting `InstanceIndex == InstanceHexes.Num()` on every add, and
then be introduced from outside the class. I grepped every `ClearInstances` / `AddInstance` /
`RemoveInstance` / `UpdateInstanceTransform` in `Source/`: **the only production site is
`StratBoardActor.cpp` itself.** The single external site is the new test fixture, deliberately
and documented. The per-layer `Tiles->GetInstanceCount() != InstanceHexes.Num()` guard at `:200`
catches any external clear-or-add, and `HexAtInstance:358` still bounds-checks the index.
`GetInstanceCount()` reads `PerInstanceSMData` synchronously and not the async cluster tree, so
it cannot lag behind an add. **Picking is sound as far as the source can show.**

**2. A model `DrawsExactlyTheseHexes` would wrongly call "already drawn."** I found exactly one,
and it is unreachable in production today. The procedure compares the **hex sequence per layer**
and nothing else, but the drawn transform is `LocalLocationOfHex(Hex.Hex, 0.0) = f(HexSize, Hex)`.
So a change to `HexSize` (`StratBoardActor.h:415`) with an unchanged hex list leaves the board
drawing at the old spacing while the early-out reports it already drawn — a case the old
always-rebuild code corrected for free. Same class: `OverlayZOffset` (`:458`) against
`FillOverlay`'s cache. Both are `EditDefaultsOnly`, so no shipping path mutates them mid-run and
only a fixture could reach it; recording rather than reporting. **If `HexSize` ever becomes
runtime-settable, that setter must invalidate — this is the property that stopped being free
today.** Beyond that I could not construct a false positive: a cross-layer interleaving change
produces an identical drawing and the early-out is *right* to fire on it (the test-author found
the same thing and made "same-terrain" load-bearing in clause 3); a dropped terrain kind fails
the end-of-layer cursor check; an unmeshed layer fails on an empty array; a new terrain kind
fails the layer lookup.

**3. The falsifiability claim is UNVERIFIED, not disproved.** `E:/MultiAgent/Strat-mutant` is
gone, I could not re-establish it, and I did not build. What the checkout does show is that the
clauses are *shaped* to be capable of going red: the instrument is a transform the board
demonstrably never reads, so it cannot steer the decision it measures, and each survival clause
is paired with a control demanding the marker die. `tests.md` also self-reports the M1
instrument trap — a copied tree carrying `Intermediate/` compiled the MAIN tree's sources and
returned a false green, caught by DLL timestamps — which is the kind of admission that raises
rather than lowers confidence in the rest.

**4. `GATE-BOARDCHURN.AnUnmeshedBoardIsNeverRememberedAsDrawn` cannot go red on the `LayerFor`
defect, by design and by its own admission** (`StratBoardChurnClauses.cpp:846-854`, `:927-938`).
It is an `if/else` over the branch the board took, and both branches assert. That is the right
call — asserting "0 drawn" would freeze the gap into the suite — but it means the suite contains
**no clause that will go red the day `LayerFor` is fixed, or the day it regresses further**.

**5. `FillOverlay`'s two guards are individually redundant**, per mutant M5 in `tests.md`:
`Overlay->GetInstanceCount() == DrawnHexes.Num()` and `DrawnHexes == Hexes` each defeat the
request-caching bug alone, so `AnOverlayMeshArrivingLateStillDraws` pins the conjunction and
neither half. Deleting either as redundant will not redden the suite. Self-reported by the lane
that found it, which is the correct handling; recorded here so the gate record carries it too.

**6. `global.md` now has 22 LF-only lines in an otherwise-CRLF file**, and `git diff --check`
warns accordingly. Lines 1-12 were already LF at HEAD, so this is pre-existing mixing that the
new banner extends rather than a new condition. The new `StratBoardChurnClauses.cpp` is all-LF,
matching `StratGuidedOpeningVisuals.cpp` (all-LF) and not `StratBoardPicking.cpp` (all-CRLF) —
this repo's line endings vary per file and the new file is within existing practice.

**7. The two `reportCreatedOn` stamps differ and that is correct, not drift:** `tests.md` cites
`2026.08.29-22.04.45` (the lane's run) and `global.md` cites `2026.08.29-22.09.14` (the
coordinator's own re-run in this tree), which is the rule that a count needs a run in its own
tree. `global.md`'s paragraph explaining why the `reportCreatedOn` citation sits away from the
figure is correct and load-bearing: I re-ran the sweep with the exit code captured **directly
and not through a pipe** and it returned **exit 0, `SWEEP CLEAN`, 42 suite claims (1 live, 41
stamped), macro census 286, report identity `2026.08.29-22.09.14`**.

**8. On the collector that produced the census.** My first clause count returned 59 — it was
counting matching *files*, not occurrences, because `tr -d '\0'` collapsed each file to one
line. Corrected with `grep -o | wc -l` and controlled against the new file, which is known to
hold 7 and reported 7. The corrected totals are 279 tracked at HEAD and 286 in the working tree.
The broken figure is recorded here because an uncontrolled collector is how this project has
manufactured findings before.

## Verdict rationale

The code is the strongest part of this change and I found nothing in it to block on: the
early-out writes nothing, cannot corrupt the picking map, and I could not construct a
production-reachable model it would wrongly call already drawn. Every blocking finding is a
claim in prose — one in a comment, three in the record — that is not true of the tree it
describes.

VERDICT: BLOCK
