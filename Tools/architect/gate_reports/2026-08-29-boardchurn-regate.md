# Re-gate: hover-flicker / board no-churn early-out — strat-integration-reviewer

Second gate on the same tree. The first is
`Tools/architect/gate_reports/2026-08-29-boardchurn-hover-flicker.md`, VERDICT: BLOCK, four
findings, and it is NOT overwritten — it is the record of what was blocked.

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`, HEAD
`1a3520b1899bb3f3928a5114cab263a03b909a0f`, nothing staged. `git rev-parse --show-toplevel`
printed `E:/MultiAgent/Stratocracy`; no `cwd` echo was trusted, and every command ran from
that absolute path.

**THE CHANGED-SINCE-THE-FIRST-GATE SET WAS DERIVED HERE, NOT ACCEPTED FROM THE DISPATCH.** My
first report was written at `18:23:59`. A `find -newermt` sweep over the tree, minus build
output, returns exactly five files under audit:

| File | mtime |
|---|---|
| `Source/StratPlay/StratBoardActor.cpp` | 18:26:58 |
| `Source/StratPlay/StratBoardActor.h` | 18:27:24 |
| `Tools/architect/state/engine.md` | 18:28:16 |
| `Tools/architect/state/global.md` | 18:32:41 |
| `Tools/architect/state/content.md` | 18:25:17 |

**The dispatch's claim list is confirmed with one correction that matters.**
`Tools/architect/state/tests.md` (18:06:56) and `Source/StratPlay/Tests/StratBoardChurnClauses.cpp`
(17:47:46) both PREDATE my first report and are **not** part of the remediation. They carry
forward unchanged from the first gate, where they were cleared. `git status --porcelain` still
shows 6 modified + 2 untracked (the second untracked file is my own first report).

**What I did NOT measure, stated before the findings.**

- **I did not build.** No `Build.bat`, no `vcvars`. On this box a C++ gate SKIPs and exits 2
  without vcvars, and a skip is not a pass. The `Result: Succeeded` in `engine.md` is unverified
  by me.
- **I did not run the suite.** The `286/286` is corroborated three ways that are all artifact
  reads or static censuses, none of them an execution: the persisted report, the banner sweep's
  parse of it, and my own macro set-difference. Not one is me running a test.
- **I did not reproduce the falsifiability proof.** `E:/MultiAgent/Strat-mutant` is still gone,
  so the claim that every clause was proved RED against a built mutant remains **UNVERIFIED**
  from this checkout. Unchanged from the first gate.
- **I did not open the editor or look at a viewport.** Nothing here says the flicker is gone to
  a human eye.
- **I could not verify the `content.md` tool-surface control.** A `select:` lookup is a property
  of the writer's session, not of this tree. I verify that the control is STATED with a positive
  half; I cannot re-measure it.
- **I could not verify the two superseded run identities.** The record says `22.09.14` and an
  intermediate engineer re-run also read 286/0/0. Only `22.32.28` is on disk; the earlier reports
  are overwritten. That claim is unfalsifiable from this checkout.
- I did not open `.agents/ue-project-context.md`; the module graph below is derived from the five
  `.Build.cs` files, so nothing here depends on that note.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing, and `git status --porcelain` on those paths printed nothing, so no untracked plants either. Manifests derived rather than quoted: `rulesCommit 96d93ea9…`, `dataCommit c2f58608…`. |
| 2 | No `strat::` call outside StratBridge/StratRules | pass, **judged** — the call-shaped grep over `Source/` returned zero hits outside the two permitted directories. Type names such as `strat::UiSnapshot` and `strat::GameState` appear widely and are legal; none is reported as a violation. No new `strat::` token entered the diff. |
| 3 | No vendored header in front of UHT | pass, **judged** — the only header including a vendored header is `Source/StratBridge/StratBridge.h`, and an anchored grep for a line-initial `UCLASS`/`USTRUCT`/`UENUM` over it returns **nothing**; its four textual mentions are prose explaining why it declares none. The two changed headers add no include. |
| 4 | `TPimplPtr`, not `TUniquePtr` | pass — `StratMatchSubsystem.h:1973` and `StratScoreboardHUD.h:1093` are both `TPimplPtr<FStratBridge>`. The sole `TUniquePtr<FStratBridge>` token is `StratViewModel.h:47`, prose recording the measured `C4150`. `StratBoardActor` holds no bridge at all. |
| 5 | Module arrows unchanged and correct | pass — **derived, not trusted.** The extraction block was extracted from the checklist and executed; it printed **15 rows, three per module, five modules**, so no module fell out silently. Every row matches the expected graph. `StratRules` appears in the `PublicIncludePaths` of `StratBridge` **only**, and in neither dependency array — the correct state, not reported as drift. `StratUI` has gained no `Stratocracy` dependency. No cycle. All **12** vendored `*.strat.cpp` files are still present and still compiled in `StratBridge`. No `.Build.cs` is in the diff at all. The field census printed exactly **six** names — the three graph fields plus `PCHUsage`, `bUseUnity`, `ShadowVariableWarningLevel`. No seventh. |
| 6 | New modules registered | pass, vacuously — no new module directory; the five are the five known ones. `IMPLEMENT_MODULE` appears in four `.cpp` files and **not** in `Source/StratRules/`, which remains correctly absent from the `.uproject` `Modules` array. |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` printed nothing and no untracked file sits there. |
| 8 | No `/Game/` literal in gameplay C++ | pass, **judged** — one non-test hit, `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, which is the import commandlet: one of the two standing exceptions, pre-existing, and not in the diff. No `ConstructorHelpers` lookup in any changed file. |
| 9 | No widget-side arithmetic | pass, vacuously — `Source/StratUI/` is not in the changed set. Not re-audited beyond that. |
| 10 | Lane discipline | pass — the remediation is lane-clean. `strat-gameplay-engineer` touched only the two `StratBoardActor` files (production C++ outside `Tests/`) and its own `engine.md`; the `coordinator` touched only `global.md` (its own file) and `content.md` under that file's fallback clause, now cited. No test code outside `Tests/`, no production code inside it, no asset in a source-only phase. |
| 11 | Prose blocks present | **BLOCK** — see F5. The blocks are present and the four remediated ones are now true of the code; a NEW false claim about the code entered `global.md` in the same pass. |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` printed nothing; HEAD is still `1a3520b`. |

## Verification of the four original findings

All four are remediated at their own sites. Each was checked against the CODE, not against the
remediation's own account of the code.

**F1 — FIXED, and fixed TRUE rather than merely softened.** This was the item most at risk of a
softer-but-still-wrong rewrite, so the underlying facts were re-derived from source:

- `LayerFor` returns an existing layer by `TerrainId` **before** reaching either `SetStaticMesh`
  call. Those two calls, at `StratBoardActor.cpp:168` and `:172`, are the **only** `SetStaticMesh`
  sites for a terrain layer in the file — the other three, at `:76-78`, are the overlays. So a
  mesh assigned after a layer exists never reaches that component. TRUE.
- `TerrainLayers` is **append-only**: across both files the only mutation is the `Add` at `:179`.
  No `Empty`, no `Reset`, no `RemoveAt`, and no code outside `StratBoardActor` names it. So the
  retraction's "keeps a null mesh **for the life of the actor**" is TRUE and not an overstatement.
- The null-mesh arm of `ApplyHexes` adds the id to `Unmeshed` and continues, and the method later
  writes `OutFailureReason` and returns `false`. So "THE REBUILD THEN RUNS AND STILL DRAWS
  NOTHING … reporting the id again" is TRUE in both halves.

The retraction at `StratBoardActor.cpp:228-243` quotes the old sentence behind `RETRACTED>`
markers, states plainly that it "is true of the EARLY-OUT and false of the BOARD", and closes
with "what this block must not do is deny it". The `engine.md` heading is corrected to
"…**TWO ARE CLOSED IN THE COMPARISON AND THE THIRD IS CLOSED ONLY AS FAR AS THE COMPARISON
REACHES**", with its own `RETRACTED>` block. The sibling at `StratBoardActor.h:194` is tightened
to "REPORTS, and does not necessarily draw", which is exactly what the code does. **No residue of
the false claim remains in either the code or `engine.md`.**

**F2 — FIXED.** The `[STAMPED AGAIN]` block in `global.md` no longer cites `engine.md:293`. It
cites the condition by its own words — the sentence beginning "DISCHARGED WHEN an editor batch
sets the four slots on `BP_StratUnit`" — and that sentence **does** resolve in `engine.md` today.
It states in terms why: a citation into a file that grows above it is stale by the next entry. It
also states that the earlier "named here rather than edited" claim "is false of this tree and is
corrected rather than left standing", which closes the second half. The block is flat, not nested.

**F3 — FIXED.** The discharge stamp in `engine.md` now opens "**WRITTEN BY
`strat-gameplay-engineer`, WHOSE FILE THIS IS**", names the gate report and finding that caused
the attribution, and explains why an entry whose FACTS are content-lane still belongs to the
engine lane. It is consistent with F2's correction, which no longer claims the coordinator left
the file unedited.

**F4 — FIXED.** The `content.md` stamp now cites "**THIS FILE'S OWN FALLBACK CONDITION**" as its
licence, gives the reason a licence must be cited at all, and discharges obligation (1)
explicitly and "NOT INHERITED FROM THE ENTRY ABOVE": one `select:` lookup naming both
`execute_script` variants and `unreal_status` returned `unreal_status` and neither
`execute_script` — a positive control beside the absence, which is what the obligation asks.
Obligation (2) was already met. The entry is a FILE WRITE recording a user confirmation, with no
asset work, so the file-write fallback is the correct clause and the editor-driver clause is not
needed; the two are not conflated.

**Question 3 — the `LayerFor` deferral. The reasoning is SOUND and the debt is recorded
accurately.** I re-derived both premises rather than accepting them:

- The clause body at `StratBoardChurnClauses.cpp:927-938` is an `if/else` over
  "second apply reported OK and the reason was empty", and it asserts on **both** arms. Fixing
  `LayerFor` flips execution to the first arm, whose assertion — drawn hex count equals the
  model's hex count — would then **pass**. So the suite indeed contains no clause that reddens
  the day `LayerFor` is fixed, and `engine.md` represents my Observation 4 correctly.
- Both `TerrainMeshes` (`StratBoardActor.h:431`) and `FallbackTerrainMesh` (`:436`) are
  `UPROPERTY(EditDefaultsOnly)`, so "not reachable on any shipping path … only a fixture or a
  runtime editor assignment can reach it" holds.
- The `DISCHARGED WHEN` condition is well formed: it demands a clause that is **red over today's
  `LayerFor`**, landing together with the three-line fix. That is a real, checkable trigger and
  not a restatement of intent.

Deferring an unpinned behaviour change out of a BLOCK remediation, with the fix specified and a
falsifiable discharge condition recorded, is the correct call and is not what this gate blocks on.

**Question 4 — the restamped report citation RESOLVES.** `Saved/AutomationReport/index.json`
carries `reportCreatedOn 2026.08.29-22.32.28`, matching `global.md` exactly. Read directly:
**286 succeeded, 0 failed, 0 notRun, 0 succeededWithWarnings, 286 test entries.** My own
independent set-difference census over the `IMPLEMENT_SIMPLE_AUTOMATION_TEST` macro, anchored at
`Source/` and collected by ONE function on both sides, gives HEAD 279 to worktree 286, and the
**seven** new clause names are all `GATE-BOARDCHURN.*` in the one new file — so "279 to 286,
seven new clauses, all `GATE-BOARDCHURN`" is TRUE. I re-ran `strat_banner_sweep.py` with the exit
code captured **directly and not through a pipe**: **exit 0**, `SWEEP CLEAN`, 286 entries all
Success, macro census 286, **42 suite claims (1 live, 41 stamped)**. The banner's account of why
the citation sits away from the figure — that a `reportCreatedOn` inside the stamp window reads
as an explicit stamp and would empty the sweep — is consistent with the sweep's own behaviour and
with this project's recorded measurement of it.

**Instrument control, recorded because an uncontrolled collector has manufactured findings here
before.** My first read of `index.json` raised a JSON decode error at char 0, which reads exactly
like a corrupt report. It is not: the file is UTF-8 **with a BOM**, and it parses cleanly as
`utf-8-sig`. No integrity claim was made before the instrument was controlled.

## Findings

### F5 — BLOCK. The new `global.md` banner re-asserts, in the present tense, the exact claim F1 required retracted — and was written six minutes AFTER the retraction landed.

**Constraint:** the record may not carry a false claim about the tree, and measurements are
recorded as measurements. This is the same constraint F1 blocked on, in a different file.

`Tools/architect/state/global.md:47-48`:

>     TWO THINGS REMAIN OPEN AND ARE NAMED IN `## NEXT` RATHER THAN CLOSED HERE: a late-assigned
>     terrain mesh still never draws, and it is `LayerFor` and not the new early-out that prevents
>     it -- while `DrawsExactlyTheseHexes`'s own comment claims the stronger property.

**The final clause is false of this tree.** The comment on `DrawsExactlyTheseHexes` does not claim
the stronger property; it **retracts** it, by name, in the block this gate has just verified —
`StratBoardActor.cpp:232-243`:

>     // RETRACTED>  "-- which is what makes a mesh assigned after the fact take effect on
>     // RETRACTED>   the very next refresh."
>     // That is true of the EARLY-OUT and false of the BOARD. ... The defect is `LayerFor`'s
>     // and it is OLDER THAN THIS EARLY-OUT; what this block must not do is deny it.

The sentence names the correct residual defect — `LayerFor` — and then attributes to the comment a
denial the comment now explicitly forbids itself. The two neighbouring open items are both TRUE:
the late-assigned mesh really does still never draw, and the two overlay-cache guard halves really
are individually unreddenable. That is what makes the false clause easy to read straight past.

**This is not stale drafting.** By mtime, `StratBoardActor.cpp` was written at **18:26:58** and
`global.md` at **18:32:41** — the banner was authored **five minutes and forty-three seconds
after** the retraction it contradicts, by the writer who had just read the gate that demanded it.
This is precisely the failure mode the dispatch asked me to look hardest for: a correction pass
introducing a fresh false claim about code, and a withdrawn claim being re-asserted as fact in a
neighbouring file.

**Why it gates rather than sitting under Observations.** It is load-bearing twice over. It sits in
the **live** banner — the sweep counts exactly 1 live claim, and this is that entry — and it is the
tree's top-level statement of what remains OPEN. A reader or agent arriving at it is told to go
fix a comment that has already been fixed, and is told that the code's own prose still denies a
defect which that prose now documents at length. That misdirects the next lane and re-opens the
authorship-of-truth question F1 was blocked to close.

**Owner: `coordinator`** — `global.md` is its file. Cheapest correct fix: end the clause at
"…and it is `LayerFor` and not the new early-out that prevents it", or restate it truthfully, since
the comment now RECORDS the limit rather than claiming past it. Do not add a nested stamp: this
banner is minutes old and is the coordinator's own file, so correct it in place rather than
stamping it, and nothing else in the paragraph needs to move.

## Observations

Non-gating.

**1. Every other new prose claim I could check against code is TRUE.** I tested the changed prose
against source rather than against itself: `AStratPlayerController::Tick` does call
`UpdateHoverFromCursor` unconditionally (`StratPlayerController.cpp:503-511`) with
`PrimaryActorTick.bCanEverTick = true` (`:93`), so the per-frame hover-poll chain is as described;
the AMENDED block on `FStratTerrainLayer` ("either rewritten in full or left exactly as it was
cannot acquire the off-by-one") is true of an early-out that writes nothing; the `FillOverlay`
declaration amendment correctly records that it is no longer `const` and takes its caller's cache;
and the miss-case `DrawnHexes.Reset()` at `StratBoardActor.cpp:409` really is on the null-mesh
path, so the cache records what was drawn and never what was requested. **F5 is the single false
one**, which is worth stating plainly: the remediation was accurate everywhere else.

**2. The `LayerFor` debt is now recorded in three places and they agree.** `StratBoardActor.cpp`
(the retraction), `engine.md` (the deferral, the three-line fix, the discharge condition) and
`tests.md` (unchanged, already recording it as open). The first gate blocked partly because
`engine.md` and `tests.md` contradicted each other on this fact inside one change set; they no
longer do. Except for F5's clause, `global.md` agrees with them.

**3. Limit (1) of the transcription clause remains unfalsifiable here, as CLAUDE.md itself
concedes.** The F3 stamp in `engine.md` asserts that `strat-gameplay-engineer` wrote it and that
the `coordinator` did not edit that file in this pass. Nothing in this checkout can confirm or
refute it — no lane draft is persisted. The attribution obligation is met **on its face**, which is
what F3 asked for and all a gate can check. Recorded so a future reader knows this rests on the
agent's honesty rather than on an instrument.

**4. Observations 1, 2, 5 and 8 of the first report stand unchanged.** The early-out still writes
nothing and cannot corrupt the picking map; `HexSize` and `OverlayZOffset` remain the one model
class the comparison would wrongly call "already drawn", still unreachable because both are
`EditDefaultsOnly`; the two `FillOverlay` guards remain individually redundant. None of the
remediation touched the code paths those describe. The `HexSize` note is worth keeping alive: if
it ever becomes runtime-settable, that setter must invalidate.

**5. `global.md` still mixes line endings** and `git diff --check` warns accordingly; the new
banner extends pre-existing LF-in-CRLF mixing rather than creating it. Unchanged from the first
gate, still non-gating.

## Verdict rationale

The remediation is good work and it is nearly complete: all four original findings are fixed at
their own sites, F1 is fixed TRUE of the code rather than merely made vaguer, the deferral of
`LayerFor` is sound and honestly recorded with a falsifiable discharge condition, and the
restamped report identity resolves to a report on disk that says what the record says it says.
The code remains the strongest part of this change, and I again found nothing in it to block on.

But the pass introduced one new false claim about the code, in the live banner, minutes after the
retraction it contradicts — the same class of defect, by the same reasoning, that the pass existed
to remove. One clause of one sentence is the whole of it.

VERDICT: BLOCK
