# Gate: unit movement, route slides and the tour existence hold -- RE-GATE -- strat-integration-reviewer

## Scope

Working tree of `E:/MultiAgent/Stratocracy`, confirmed by
`git -C E:/MultiAgent/Stratocracy rev-parse --show-toplevel` -> `E:/MultiAgent/Stratocracy`.
Branch `master`, HEAD `283d7114c82c792639f28c05454a4e3e4cf974a9`, uncommitted.
`git status --porcelain` prints **25 entries** -- 20 modified, 5 untracked -- matching the brief.
The twenty-fifth is the prior gate report itself, which was untracked-new since that gate ran.

This is a RE-GATE. I inherited nothing from
`Tools/architect/gate_reports/2026-09-03-unit-movement-and-existence-hold.md` (`VERDICT: BLOCK`),
including its clean code pass. Every checklist item below was re-run against this tree.

Audited: the full working-tree diff against `283d711`, the five untracked files, the module graph
derived from every `Source/*/*.Build.cs`, `Stratocracy.uproject`, the three shipped `.uasset`
files by raw bytes, the exported automation report, `strat_banner_sweep.py` and four runs of it
over mutated copies of the records, and the six `Tools/architect/state/` records.

Not run by me, by role: the build and the suite. Suite figures below are re-measured from the
exported report and from an independent macro census, not from a run I made.

## Code, assets and module graph -- the priority pass

**This pass is CLEAN. No finding in game code, in the assets, or in the module graph.**
Three source files moved since the prior gate (`Tests/StratAiPlaybackClauses.cpp` 15:45:58,
`Tests/StratTourExistenceHoldClauses.cpp` 15:41:07 -- both in `Tests/`); everything below was
re-measured over the current bytes regardless.

- **Vendored bytes untouched, with a control.** `git diff --stat -- Source/StratRules Data`
  prints nothing and `git status --porcelain` for those paths prints nothing, so there are no
  untracked additions either. The same two instruments were shown able to speak on
  `Source/StratPlay` (12 files, 5747 insertions). Both directories are non-empty (26 and 6
  files). Manifests read `rulesCommit 96d93ea9...` and `dataCommit c2f58608...`, which is the
  authority, and both match.
- **No strat call escaped its two directories.** Both the lowercase-initial call form and the
  nested-namespace form return **zero** hits outside `Source/StratBridge/` and
  `Source/StratRules/`. The instrument speaks: the identical pattern returns **144** hits inside
  `Source/StratBridge/`. 85 files outside those two directories name the namespace at all; every
  one is a TYPE, and a type name is not a call.
- **No vendored header in front of UHT.** `Source/StratBridge/StratBridge.h` is the only header
  outside `Source/StratRules/` that includes `Ai.h`/`Data.h`/`Move.h`/`Replay.h`/`Save.h`/
  `Scenario.h`/`Ui.h`. It contains six occurrences of the tokens UCLASS/USTRUCT/UENUM and **all
  six are prose in comments explaining why it declares none** (lines 30, 119, 262, 264, 323,
  486); it includes no `.generated.h`. No header anywhere in `Source/` includes `StratBridge.h`;
  the control is that **71 `.cpp` files do**.
- **TPimplPtr, never TUniquePtr.** Both live `FStratBridge` members are `TPimplPtr` --
  `StratMatchSubsystem.h:2448` and `StratScoreboardHUD.h:1235`. The only `TUniquePtr<FStratBridge>`
  occurrence in the tree is the C4150 measurement quoted in prose at `StratViewModel.h:47`.
- **Module arrows derive correctly and none moved.** The extraction printed **16 rows** across
  all five modules and the field census **10 names** -- the recorded shape exactly, including
  `BRIDGE=`, which is the tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="` and not
  a field. No module is missing from the output. The sixteenth row
  (`StratBridge PrivateDependencyModuleNames StratRules`) was read at its source:
  `StratBridge.Build.cs:135-138` guards it with `if (!bCompileVendoredRulesHere)`, where
  `bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic` (line 133) -- so it
  is monolithic-only, deliberate, and not a finding. `PublicIncludePaths` (line 144) carries
  `StratRules` unconditionally in every target, which is an include edge and not an arrow.
  `git diff -- Source/*/*.Build.cs` and `git status` for those paths are both empty, so no arrow
  moved this wave. `StratUI` gained no `Stratocracy` dependency. No cycle.
- **New modules.** None. Five module directories, four IMPLEMENT_MODULE sites, four entries in
  `Stratocracy.uproject`. `StratRules` carries no module object and is correctly absent from the
  array. The new untracked file `Source/StratBridge/Tests/StratRosterDeltaClauses.cpp` is inside
  an existing module. `StratBridge`, `StratPlay`, `StratUI` and `StratRules` all set NoPCHs and
  `bUseUnity = false`.
- **`Source/Stratocracy/` untouched.** `git diff --stat` and `git status --porcelain` both empty.
- **No `/Game/` literal in gameplay C++.** 158 hits exist across 70 files; every one is under a
  `Tests/` directory or in `ImportStratDataCommandlet.cpp` -- the two standing exceptions.
- **No widget-side arithmetic.** No `Source/StratUI/` file is touched by this diff at all.
- **The assets, measured by raw bytes with two controls.** `AiSides`, `AiPlaybackStepSeconds`
  and `AiBuildlistUnitIds` are each serialised once in BOTH `BP_StratGameMode.uasset` and
  `BP_StratGameMode_AiVsAi.uasset` and absent from `BP_StratUnit.uasset`; `MoveTweenSeconds`
  gives the exact mirror-image result; a fabricated property name is absent from all three. The
  instrument discriminates per asset and per property. Only `BP_StratUnit.uasset` is modified by
  this diff, and `content.md` records its authorship in lane.
- **The suite artifacts agree three ways.** The exported report reads
  `reportCreatedOn 2026.09.03-19.47.45`, 386 succeeded / 0 failed / 0 notRun, and its `tests`
  array holds 386 entries every one of which is Success. An independent macro census of `Source/`
  counts **386** automation-test macros, 386 unique names, by a multiline regex. All five
  existence-hold clause names are present in the report BY NAME with state Success. No `Source/`
  file is newer than the report (newest is `Tests/StratAiPlaybackClauses.cpp` at 15:45:58 local;
  the report is 15:47:45 local, which is the 19.47.45 UTC stamp).

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No strat:: call outside StratBridge/StratRules | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | TPimplPtr and not TUniquePtr for `FStratBridge` | pass |
| 5 | Module arrows unchanged and correct (16 rows, 10 fields) | pass |
| 6 | New modules registered (none added; StratRules correctly absent) | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass |
| 9 | No widget-side arithmetic (`Source/StratUI/` untouched) | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed (`git diff --cached` empty) | pass |
| -- | Record accuracy (prose) | **BLOCK** -- one finding below |

## Verification of the three repairs

### F1 -- REPAIRED, and the widening is correct. Verified, not taken.

`Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp` now reads, at the clause's own header,
"THE FIXTURE IS THE C++ FIELD DEFAULT, AND THAT IS A PATH THE SHIPPED GAME NEVER TAKES" and
"THIS CLAUSE COVERS NO SHIPPED PATH", with the byte-level measurement and both controls stated
beside it. The file header's note at lines 42-48 says the same. The clause is intact and NOT
weakened: `FStratGateIsNeverRaisedWithoutATourTest` /
"Stratocracy.StratPlay.T-TURN-09.TheGateIsNeverRaisedWithoutATour" is unchanged, its assertions
are unchanged, and the report carries that name at Success. The fixture now takes 0.0f off a
default-constructed `FStratMatchConfig` rather than writing the literal, which is strictly better
than what the prior gate found.

The widened repair in `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` is correct. A sweep of
the claim shape across the whole tree now returns no instance of a C++ field default being called
the shipped value, except the one declared open below. `StratRosterDeltaClauses.cpp` and
`StratRouteTweenUnitDouble.{h,cpp}` are clean; the unit double gets it explicitly right.

**RULING ON THE ONE LEFT OPEN -- `Source/StratPlay/Tests/StratAiMatchClauses.cpp:380`
("// ---- THE GUARDED PATH: the shipped defaults ----"): LEAVING IT OPEN IS ACCEPTABLE AND IS
NOT A FINDING OF THIS GATE.** Three reasons, and the first is decisive. (1) The file is not in
this wave's diff -- `git status --porcelain` for it is empty -- and a reviewer audits the diff;
widening a single-finding repair into an untouched file is scope creep this crew is right to
refuse. (2) The premise is nonetheless TRUE and I re-measured it independently: `AiSides` is
serialised in both GameMode Blueprints, with the mirror-image and fabricated-name controls above.
So it IS the same conflation. (3) It is a comment, it reaches no report, and it is declared with
its measurement and its owner at `Tools/architect/state/tests.md:78-84`, which makes it greppable
rather than lost. A declared instance with a named owner is a debt, not a defect at large. It
should be repaired in the next pass that touches that file, and it should not be repaired now.

### F2 -- REPAIRED. Both refusals are correct; I uphold both.

All four stamps are present in `engine.md`'s topmost live entry and each sits on the bullet it
corrects rather than only in the header, which is the right shape for a reader arriving by
citation. The "THE TREE DOES NOT COMPILE" bullet carries "DISCHARGED, AND EVERY BUILD FACT IN
THIS BULLET IS HISTORY ... NOTHING IN THIS BULLET MAY BE CITED AS A LIVE BUILD STATUS". The
"NOT WRITTEN AND NOT BUILT" list carries "ALL FIVE ARE NOW WRITTEN, BUILT AND GREEN". The inert
`BeginAiPlayback` mutation carries "THE MUTATION DIRECTLY ABOVE IS INERT AND MUST NOT BE RUN AS
THIS CLAUSE'S PIN", kept rather than deleted, with the mutation that DOES pin it -- deleting
`ApplyTourExistenceAtCursor();` from `UStratMatchSubsystem::AdvanceAiPlaybackOneStep` -- named
beside it and "Cite THAT one, never the one above". I read the call chain against the tree and
the inertness argument holds.

**RULING ON REFUSAL 1 -- declining to write the const accessor: CORRECT.** It is a new public
surface on a subsystem, and landing it inside a RECORD REPAIR over a tree under `VERDICT: BLOCK`
would carry an unpinned change in on the back of a prose fix. The discharge condition written
instead is falsifiable and specific -- an accessor reporting whether a given unit id is currently
held concealed, AND a clause reading it at a point interleaved with `BeginAiPlayback`'s own
apply, at which moment the inert mutation becomes a real pin and the stamp retires. The gap is
declared at four sites, and no file claims the clause pins the `BeginAiPlayback` call. That is a
declared gap with a named closing condition, which is the discipline working.

**RULING ON REFUSAL 2 -- declining tests.md's "hoist the apply" alternative: CORRECT, and it is
the more important of the two.** That call's placement is argued line by line -- below all three
guards, after the reconcile has spawned the actors, before `OnAiPlaybackTimer` shows step one.
Moving a production call to make it observable trades a declared gap for a live regression risk,
and the written reason says exactly that. An accessor adds a read and moves nothing. The refusal
is recorded on the engineer's side, and `tests.md` still carries its own proposal, so both halves
of the disagreement are legible from either file.

### F3 -- REPAIRED, and the sweep repair verified by four runs I made myself.

The count sentence now reads: ALL SIX **[STAMPED 2026-09-03 -- "SIX" IS THE UNIT-MOVEMENT WAVE'S
OWN COUNT ... IT IS NOT THE +5 THIS BANNER NOW CARRIES LIVE ...]** NEW CLAUSES ARE DISCHARGED BY
MEASUREMENT. The stamp is INLINE at the sentence, which re-scopes both the count and the
discharge claim to the wave they were written for. A reader landing at that sentence by citation
now sees the correction. The unqualified-discharge half of the prior finding is resolved by the
same stamp, and the inert-mutation paragraph sits above it in the same block.

**THE PROVENANCE REASONING IS SOUND AND I RE-DERIVED IT INDEPENDENTLY.** `is_stamped` is called
with `_PARAGRAPH_STAMP_MARKERS`, which includes a bare `reportCreatedOn <date>`, over a window of
`_PROVENANCE_CITATION_WINDOW = 400` characters around the citation being judged; the matched span
is masked but every other marker in the window is not; and neither part of check 5 uses
`in_code_span`, so a backticked STAMPED bracket counts. The whole banner is ONE paragraph of
10,915 characters, so the window -- not the paragraph -- is what decides. Measured against the
current file: the live citation is at `global.md:26`, and the nearest STAMPED marker to it is at
**+1122 characters**, comfortably outside 400. The next-nearest markers cluster around the
`2026.09.02-23.16.13` citation at line 65 (at -28 and -480), which is correctly stamped history.

**THE FOUR RUNS.** Executed against copies of the six record files in a scratch directory, using
the SHIPPED script, without a pipe, reading the exit code directly.

| Probe | Mutation | Exit | Result |
|---|---|---|---|
| P0 control | none | 0 | SWEEP CLEAN, 59 claims (1 live, 58 stamped), 2 tracked -- identical to the live tree |
| P1 | live citation 19.47.45 -> 11.11.11 | **1** | SWEEP FAILED, `[**REPORT PROVENANCE**] global.md:25: cites reportCreatedOn 2026.09.03-11.11.11, but the report this sweep actually opened is 2026.09.03-19.47.45` |
| P2 | live figure 386/386 -> 387/387 | **1** | SWEEP FAILED, two SUITE COUNT AGREEMENT findings, against both the tree total and the report's own succeeded count |
| P3 | P1's mutation PLUS an ordinary `**[STAMPED 2026-09-03 -- an ordinary supersession note.]**` inserted ~120 characters below the live citation | 0 | SWEEP CLEAN, **zero** REPORT PROVENANCE findings |

P1 and P2 discharge the two probes the brief asked for: a stale live citation fails, and a wrong
live count fails. **P3 is mine, and it confirms the in-file note is exactly right and not
stylistic.** One ordinary stamp bracket, of the kind this record adds every time it corrects
something, placed anywhere in the 400 characters below the live citation, silently disarms check
5(a) on the banner's own certifying citation -- the sweep then prints SWEEP CLEAN at exit 0 over
a demonstrably stale live stamp. Nothing else was disarmed: the live run and P0 agree on claim
counts and tracked items, RECORD OWNERSHIP still holds (no live N/N outside `global.md`), and P2
shows the count check is live on this banner. The unmutated tree's own run is exit 0, SWEEP
CLEAN, 386 entries, macro census 386.

**I disagree with nothing about leaving the prior gate report untouched.** It cites
`reportCreatedOn 2026.09.03-18.43.07`, which no longer exists because there is one report path
and F1's re-run overwrote it. A gate report is a verdict about a tree at a moment; editing it to
match a later artifact would destroy the only evidence of what was actually measured. Leaving it
is correct. This report supersedes it; that one is not retracted.

## Findings

### F4 -- engine.md's LIVE entry still calls the C++ field default "the shipped default", in the same bullet list the F2 pass stamped. BLOCK.

`Tools/architect/state/engine.md:262-265`, inside the topmost `## NEXT` entry, in the
"WHAT THE TEST LANE IS OWED" list -- unstamped:

    - `T-TURN-09.TheGateIsNeverRaisedWithoutATour` -- with `AiPlaybackStepSeconds` at the shipped
      default, `RunAiTurnsNow` leaves no actor retained and no actor hidden. **MUTATION: in
      `RunAiTurnsNow`, replace `if (WillAiPlaybackRun())` with `if (true)`** ...

This is the identical claim the prior gate blocked as F1, in a different file, and it is now the
only surviving instance in a live record. `AiPlaybackStepSeconds` is 0.0f in C++ and IS
overridden on both shipped GameMode Blueprints -- I re-measured the serialisation this pass with
both controls. So the fixture runs at the C++ FIELD default, and the record's own live
instruction sheet calls it the shipped default.

It is load-bearing for three reasons, which is why this is a BLOCK and not an observation.
(1) It directly contradicts the repaired clause it describes: the tree now says
"THIS CLAUSE COVERS NO SHIPPED PATH" in capitals, and the tree wins, so the record is what is
wrong. (2) It sits in the LIVE topmost entry, two bullets below stamps this very repair pass
added -- so the correction did not reach the handoff sentence, which is a shape this project has
already paid for. (3) The false belief it encodes has a measured cost in this repo already:
`Source/StratPlay/StratPlayerController.cpp:930-932` records a 2026-08-29 regression caused by
believing AI playback ships inert on the zero default.

The mutation itself is correct and must not change. The fix is the four words.

Owner: **`strat-gameplay-engineer`** (`engine.md` is its file).

## Observations

Non-gating.

- **`strat_banner_sweep.py`'s REPORT PROVENANCE 5(a) can be disarmed by an ordinary editorial
  act, and only prose currently prevents it.** P3 above is the measurement. The repair that made
  the sweep live again is a CHARACTER DISTANCE in a document whose own convention is to grow by
  stamping in place -- exactly the failure mode the check's own docstring diagnoses for the
  paragraph-wide scope it replaced, reproduced one level up at the window. The margin today is
  1122 against a threshold of 400; any future stamp written above the note, or any shortening of
  the note, closes it silently and the sweep keeps printing SWEEP CLEAN. Note also that the note
  is itself the buffer AND contains the trigger token, so editing the note edits the guard. The
  note is accurate, is correctly labelled load-bearing, and is the honest disclosure this gate
  wants -- but a guard whose precondition it cannot itself check is a guard resting on a reader's
  care. Two candidate directions, neither proposed as a diff: exempt the LIVE banner's own
  citation from the bare-reportCreatedOn marker the way part (b) already narrows its marker set
  via `_PROVENANCE_TREE_STAMP_MARKERS`, or require the live citation to be stamped only by an
  annotation that NAMES it. Owner: **`strat-data-steward`** (`Tools/architect/` is its lane
  outside the record files). Recorded here rather than blocked on, because nothing in the record
  is currently false by it and the disclosure is present and correct.
- **`tests.md:62-64` over-claims where the repaired messages land.** It says three of the repaired
  sites "were TEST MESSAGES, which reach `Saved/AutomationReport/index.json` -- a false claim that
  ships is the reason this was a BLOCK". Measured this pass: a Success entry in that report
  carries only Info-type events (AddInfo output and suppressed-warning notices);
  TestTrue/TestEqual message text is emitted only on FAILURE. Probing the report blob for the old
  and the new strings returns 0 for both. The messages WOULD ship if the clause reddened, so the
  repair was worth making and the severity call was defensible -- but as written the sentence
  asserts something false of a green suite. Subject versus scope. Not gating; worth a
  half-sentence next time that file is touched.
- **`AdvanceOneStepStillArmsNoTimer` still greps like a defined clause name and is not one.**
  Zero occurrences in `Source/`, one at `Tools/architect/state/engine.md:639`. The surrounding
  prose is clear that it is proposed and unpinnable, so this is not a fabricated citation -- but
  the prior gate asked for a marker and it did not get one. Unchanged since that gate.
- **Attribution is clean everywhere I checked.** All three repair entries name acting and writing
  inline with the base commit `283d711` and the tree path: `strat-test-author` ("ACTING and
  WRITING; IN LANE -- Source/*/Tests/ only, plus this file"), `strat-gameplay-engineer` ("ACTING
  and WRITING; IN LANE"), and `global.md`'s banner naming the `coordinator` as its own file's
  writer "IN LANE AND UNDER NO EXCEPTION CLAUSE", explicitly disclaiming code and clauses. No
  entry names one actor where two differ. No transcription clause and no in-session clause was
  invoked, and none was needed.
- **Prose blocks present on all five untracked files.** Each opens why-first with GDD sections and
  acceptance IDs, and `StratRouteTweenUnitDouble.h` states in its first six lines that the shipped
  values live on `BP_StratUnit` and its own numbers are fixture input -- the correct distinction,
  gotten right before the F1 sweep asked for it.
- **Lane discipline holds across the repair passes.** Only `Source/*/Tests/` files and record
  files moved after the prior gate (15:19:18); no production `.cpp` or `.h` was touched
  (`StratBridge.cpp` and `StratMatchSubsystem.cpp` are unchanged at 14:34:59). Each record file
  was written by its own sole writer.
- Git reports LF/CRLF conversion warnings on `Source/StratBridge/StratCombatLog.h`,
  `Source/StratPlay/Tests/StratAiTurnRunnerClauses.cpp` and
  `Source/StratPlay/Tests/StratMatchReconcile.cpp`. Known repo condition -- line endings vary per
  file here. Not a finding.

VERDICT: BLOCK
