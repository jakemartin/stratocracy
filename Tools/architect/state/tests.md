# Stratocracy state — tests

> **Sole writer: `strat-test-author`, except that the `coordinator` transcribes
> lane-authored entries after a merge, attributing acting and writing separately (user ruling,
> 2026-08-29; the clause and its limits are stated in `CLAUDE.md`, which is the authority).** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Clause inventory, what each clause actually pins, known flakes, and instrument caveats — the places where a test asserts rather than measures.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything below this block is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did. (This sentence was truncated mid-clause when the
> file was split; completed 2026-08-22, no meaning changed.)

> **[OUT-OF-LANE WRITE, 2026-08-29, AND THE SECOND ONE THIS FILE HAS TAKEN IN A SINGLE DAY. THE
> TWO ENTRIES DIRECTLY BELOW WERE WRITTEN INTO THIS FILE BY THE `coordinator`, WHOSE FILE THIS IS
> NOT.** This file's header names `strat-test-author` its sole writer, and that rule is not
> weakened by what follows.
> **THE ACTING AND THE WRITING ARE ATTRIBUTED SEPARATELY, because they were done by different
> agents and naming one while leaving the other unnamed is itself a finding.** The clauses, the
> fixtures, the builds, the suite runs, the falsifiability probes and the prose of both entries
> are `strat-test-author`'s, produced in two lane worktrees and delivered as draft record entries
> in its lane reports; the `coordinator` transcribed them here and wrote this block. The user
> directed it in those terms on 2026-08-29, after the merge, having been offered the alternative
> of four lane dispatches writing into two shared files.
> **THE REASON THE DRAFTS EXISTED AT ALL is a lane-concurrency constraint:** two test-author lanes
> ran at once against one `tests.md`, so both were instructed NOT to write this file in their
> trees, because two lanes editing one record file conflict on rebase. The merge protocol already
> puts the record entry after the merge, on the integration tree, so the instruction cost nothing.
> **WHAT THIS BLOCK DOES NOT DO:** it licenses this transcription and nothing else. It is not a
> standing fallback writer for this file. **AND THE REPETITION IS THE POINT.** An earlier
> out-of-lane write into this file, on the morning of the same day, produced a `VERDICT: BLOCK`
> for the single reason that its markers did not say so -- the finding was that **the file that
> RECEIVES an out-of-lane write is where the exception has to be readable**, not only in the
> coordinator's own file. This block exists because of that finding. Two exceptions in one day in
> one file, and a third in `engine.md` beside it, is a clause forming rather than three
> coincidences; whether `Tools/architect/state/` should say plainly that the `coordinator`
> transcribes lane-authored entries after a merge was the user's to rule.
> **[RULED 2026-08-29 BY THE USER, LATER THE SAME DAY. THE QUESTION THIS BLOCK LEFT OPEN IS NOW
> CLOSED AND THE SENTENCE ABOVE IS STAMPED RATHER THAN DELETED.** This file's sole-writer line
> now carries the exception in the user's own words -- the `coordinator` transcribes
> lane-authored entries after a merge, attributing acting and writing separately -- so a future
> transcription no longer needs a block of its own, and this one stands as the account of how
> the clause came to exist. **THE COST THE USER NAMED IN RULING IT:** it removes the drift risk
> this pass measured, and costs a little of the strictness that makes ownership legible. The
> separate-attribution requirement is what buys that legibility back, and it is not optional.
> `CLAUDE.md` is the authority on the clause's limits.]**

- **2026-08-29 (local; the runs that back this entry stamp UTC `2026.08.29-18.15.33` and its
  confirming re-run `2026.08.29-18.16.21`, both in the LANE tree) -- W5'S SEC 2.11.2 AI TURN
  PLAYBACK IS PINNED BY NINE CLAUSES IN ONE NEW FILE, AND TWO OF THEM EXIST BECAUSE THE WAVE
  SHIPPED THE SAME DEFECT TWICE.** `strat-test-author` ACTING; ENTRY TRANSCRIBED by the `coordinator` under the block
  above -- the clauses, the fixtures, the runs and this entry's prose are the test author's,
  produced in the lane worktree
  `E:/MultiAgent/Strat-wt/slot-1` on branch `feat/ai-turn-playback`, against the engineer's
  uncommitted C++; landed on `master` in merge commit `69d0029`. One new file inside `Tests/`:
  `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` (9). The clause-name set grew by exactly
  nine over the wave and NOTHING was removed or renamed, measured by MULTILINE SET DIFFERENCE on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` -- never by a single-line grep, and never by an
  acceptance-ID grep, which once undercounted 8 as 5. The live suite figure is `global.md`'s and
  is not restated here.
  - **THE DEFECT THIS WAVE HAD TWICE, because the clauses are only legible against it, and
    because the second instance is the more interesting one.**
    - **INSTANCE 1 -- THE SHIPPED DEFAULT.** `RunAiTurnsNow` fills `AiPlaybackReel` on EVERY
      hand-over while only the TIMER was gated on `AiPlaybackStepSeconds`. At the default of zero
      the cursor sat at 0 over a non-empty reel: `IsAiPlaybackRunning()` true, `SkipAiPlayback()`
      true, and the first click or Esc after every AI hand-over consumed. **FOUND BY A CLAUSE THAT
      PASSED WHILE MEASURING IT** -- `SkipIsReachableAfterTheMatchConcludes` had reached "a reel
      playing" THROUGH the defect, so it was green and the state it was green about was wrong.
    - **INSTANCE 2 -- THE RESEED, AND THE PROSE ASSERTED THE PROPERTY THE CODE LACKED.**
      `BeginAiPlayback`'s invariant covered every reason not to ARM and said nothing about the
      reasons to STOP. `StopAiPlaybackTimer` cleared the clock and left the cursor;
      `TearDownPresentation` and `Deinitialize` both ended tours that way, and `StartMatchInternal`
      calls the former unconditionally with `LoadMatchFromSlot` reaching it through the same
      function. So a new match or slot load taken mid-tour began with a stranded cursor over the
      PREVIOUS match's step list, and swallowed the new match's first input. Found by
      `strat-integration-reviewer`, not by a clause. Fixed by making the verb `EndAiPlaybackTour`,
      which clears and retires atomically.
    - **THE LESSON THIS LANE SHOULD KEEP: an invariant stated on the function that establishes
      half of it reads as covering all of it. Grep the CALL SITES of the verb, not the prose.**
  - **WHAT EACH CLAUSE PINS, AND WHAT IT DOES NOT.**
    - `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState` reaches `T-TURN-09`'s
      STATE-EQUALITY half ONLY. It re-asserts nothing about Sec 2.8's result tier or which side
      won. The equality is cheap BY CONSTRUCTION -- the board is reconciled to the final view
      model before a tour begins -- and the clause's value is that the cheapness is STRUCTURAL:
      `FStratAiPlaybackReel` holds no `FStratBridge`, and the clause reddens the day something
      gives it one. **DELIBERATELY NOT rewritten onto `AdvanceAiPlaybackOneStep`** when that seam
      appeared: it needs no clock, no world and no subsystem today, and routing it through one
      would make it MORE contingent, not less.
    - `GATE-AITURN` is a LOCAL GATE NAME. It mints no acceptance ID and claims none. `T-AI-06` was
      REFUSED for this wave -- its subject is the TIE-BREAK RULE and a playback wrapper asserts
      nothing about tie-breaking -- and `T-INT-05` never became owed, because `FStratViewModel`
      gained no member.
    - `GATE-AITURN.ReelDoesNotChangeTheTurn` is THE KEYSTONE: without it,
      `StratAiTurnRunner.h`'s amended PACING bullet ("delete the parameter and every turn plays
      identically") is unfalsifiable prose. **Its comparators are PROVED able to say no** by a
      control arm that genuinely differs -- the report's own entries carry `'bOk differs'` and
      `'lengths differ: 8 vs 2'` -- so a comparator that always answered "equal" fails the clause
      before it reaches the claim. **BUT THE MUTANTS THAT WOULD REDDEN IT WERE NEVER EXECUTED:**
      each is an edit to `StratAiTurnRunner.cpp`, outside this lane. They are named at the clause
      and flagged there as unexecuted.
    - `GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder` pins one step per accepted command in
      submission order, the closing `EndTurn` included with `bHasHex == false`, and NOTHING
      appended on the refusal arm or the bound arm. Its expectation comes from an INDEPENDENT
      recorder in the scripted port, appended one stack frame below the reel's own append.
    - `GATE-AITURN.SkipReturnsTrueOnlyWhilePlaying` pins the FALSE arm on both quiet states, and
      pins `Num()` as the ONLY discriminator between a skipped tour and a reel never filled -- the
      two agree on `IsPlaying()`, on `GetCursor() == Num()` and on `Peek()`. A `SkipToEnd`
      rewritten to `Steps.Reset()` passes every other assertion in that clause and fails that one.
      **THAT SURVIVAL GUARANTEE IS SCOPED TO A TOUR ENDING, NOT A MATCH BOUNDARY** --
      `TearDownPresentation` zeroes the list on purpose, and `ReseedMidTourLeavesNoStuckTour` pins
      the zero. The halves are split because forgetting to retire SWALLOWS A CLICK while forgetting
      to clear only misleads a readout.
    - `GATE-AITURN.SkipIsReachableAfterTheMatchConcludes` pins a PLACEMENT, not a behaviour, and is
      the only clause that can see it. It drives `HandleSelectionEvent` and NOT `SkipAiPlayback`,
      because the subsystem method does not consult the match's conclusion and would pass with the
      gate moved anywhere. Its negative control -- the second identical Esc, refused by the
      concluded-match gate -- is what makes the first `true` a statement about the gate's position.
    - `GATE-AITURN.DefaultConfigConsumesNoInput` and `SkipIsReachableAfterTheMatchConcludes` are A
      MATCHED PAIR: identical fixtures but for `AiPlaybackStepSeconds`, with OPPOSITE expectations
      of the same `HandleSelectionEvent(Cancel, ...)` call. Neither is meaningful alone, and a
      change making both true, or both false, fails one of them.
    - `GATE-AITURN.NoWorldLeavesNoStuckTour` is what distinguishes the instance-1 fix from the
      REJECTED alternative (gating `SkipAiPlayback` on the config), which repairs the zero case and
      leaves a positive interval with no world reporting a phantom tour. It asserts
      `GetWorld() == nullptr` as its own control BEFORE reading anything into the quiet. **The gate
      confirmed it separates the two repairs only on the third exit** -- neither it nor its pair
      covers the reseed path, which is instance 2's.
    - `GATE-AITURN.StepFocusesAndStopsOnTheLast` is the FIRST thing to execute the camera-stepping
      half at all. It asserts the TIMER's behaviour rather than a copy of it, because
      `OnAiPlaybackTimer` is one line calling the same method. It made 155 calls over a 156-step
      reel; the loop is bounded by the recorded count, so a method returning `true` forever ends
      the clause with a failure rather than a hang.
    - `GATE-AITURN.ReseedMidTourLeavesNoStuckTour` is instance 2's regression clause and FAILS ON
      THE PRE-FIX CODE at all three of its assertions.
  - **A FALSE-PASS TRAP THAT WOULD HAVE MADE THE RESEED CLAUSE GREEN ON THE BUG, AND THE SHARPEST
    THING IN THIS ENTRY.** `BeginAiPlayback` shows the FIRST step IMMEDIATELY rather than one
    interval later, so a hand-over whose reel holds ONE action -- and a turn that only ends gives
    exactly one, since the closing `EndTurn` is itself recorded -- SELF-COMPLETES BEFORE
    `RunAiTurnsNow` RETURNS. A reseed clause fixtured that way reseeds an ALREADY-RETIRED reel and
    asserts nothing. `Num() >= 2` is mandatory, and the positive control -- `IsAiPlaybackRunning()`
    required TRUE immediately before the reseed, which fired at `1 of 156` -- is what tells "the
    reseed retired a live tour" from "no tour ever started". **Every assertion after a reseed is an
    ABSENCE, and an absence proves nothing until the instrument is shown able to speak.** Both
    guards are HARD `return false` early exits in the body, not soft assertions that would let the
    clause continue and report green on the rest.
  - **A CORRECTION THIS LANE MADE AGAINST ITSELF, RECORDED BECAUSE THE ERROR IS REUSABLE.** After
    instance 1 was fixed, this lane reported that "any fix to the swallow reddens
    `SkipIsReachableAfterTheMatchConcludes`, so the fix and a re-ruling of the clause's subject
    belong in one pass". **That was wrong, and the error was SUBJECT-VS-SCOPE committed against
    its own clause:** what was named as the clause's SUBJECT was the FIXTURE'S ROUTE to a playing
    reel. The subject is the placement of the skip gate ahead of `StratMatchAcceptsPlayerCommands`,
    which the 2026-08-29 ruling ordered and which the fix did not touch --
    `HandleSelectionEvent`'s control flow is byte-identical across it. Only the fixture moved, to
    a positive interval in a world, and the clause is STRONGER for it: it now tests the placement
    against a tour that genuinely armed instead of the phantom the defect was handing it. The
    2026-08-29 ruling stands unamended. The concession is written into the clause's own comment in
    the tree, not only into a lane report.
  - **NOT COVERED, AND IT IS A HOLE RATHER THAN A CHOICE.** (1) **"No timer handle is armed after
    the last step" is UNOBSERVABLE from `Tests/`:** every `FTimerManager` query is keyed on an
    `FTimerHandle` and `UStratMatchSubsystem::AiPlaybackTimer` is private. It is also without
    behavioural consequence -- the null-`Peek` arm ends the tour too -- so the clause asserts the
    observable consequences and says this at its head. A `bool IsAiPlaybackTimerArmed() const` seam
    would close it and NONE WAS ADDED. (2) **`FocusPlaybackStep` is now EXECUTED but exits at the
    camera cast every time.** The no-board arm, the no-player-controller arm, the WRONG-PAWN arm
    and the successful arm each need a possessed pawn of a specific class in a fixture level, and
    the successful arm additionally needs `AStratBoardActor::WorldLocationOfHex` on a board with a
    real tile set. That is a level-and-possession fixture, not a headless one. Stated as a debt,
    not reached for.
  - **A HARNESS TRAP THAT COST A RED RUN AND IS WORTH THE LINE.**
    `AddExpectedMessagePlain(..., Occurrences 0)` means **"fail if there were NO occurrences"**
    (`AutomationTest.h`, the `EAutomationExpectedMessageFlags` overload), NOT "any number including
    zero". A speculative declaration is itself an assertion and reddens a clean clause. Declare
    only what a MEASURED run attributed to that clause -- this lane ran first, read the exact
    warnings the report attributed to each clause, then declared only those.
  - **KNOWN FLAKES.** None observed. Every figure in this pass was confirmed by a second run on
    identical bytes.

- **2026-08-29 (local; the run that backs this entry stamps UTC `2026.08.29-17.27.23` in the LANE
  tree) -- W4'S TWO SEC 2.11.2 ON-MAP MARKERS ARE PINNED BY THREE CLAUSES, AND THE LANE PROVED
  FALSIFIABILITY BY THE ONLY ROUTE OPEN TO IT.** `strat-test-author` ACTING; ENTRY TRANSCRIBED by the `coordinator` under the
  block above -- the clauses, the fixtures, the runs and this entry's prose are the test
  author's, produced in the lane worktree `E:/MultiAgent/Strat-wt/slot-2` on branch `feat/map-markers`, against the engineer's
  uncommitted C++; landed on `master` in merge commit `4084df6`. One new file inside `Tests/`:
  `Source/StratPlay/Tests/StratMapMarkerClauses.cpp` (3), CRLF and no BOM. **NOT "matching its neighbours" --
  that gloss was written and is withdrawn here before it landed: `Source/StratPlay/Tests/`
  splits SIXTEEN CRLF to THIRTEEN LF-only and `StratMatchReconcile.cpp` next door is LF-only,
  so this file matches a MAJORITY and not a convention.** The two measured facts stand; the
  claim about the directory did not. The clause-name set grew by exactly three and NOTHING was removed, by multiline set
  difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`. The live suite figure is `global.md`'s and is
  not restated here.
  - **WHAT EACH CLAUSE PINS.**
    - `Stratocracy.StratPlay.T-INT-05.MarkersRebuildFromTheViewModelAlone` -- the two marker
      components are a function of the LAST applied model alone. Four passes: pass 1 records, pass
      2 inverts `bIsFlag`/`bDone` on every unit, pass 3 re-applies pass 1's model and every actor
      must answer identically, pass 4 leaves nothing lit. **Pass 3's expectation is PASS 1'S OWN
      RECORDED READING, per unit id, so no marker rule is re-derived anywhere in the clause**, and
      pass 4 kills a latch of the `if (X) Set(true);`-with-no-else shape. **THIS IS A STRETCH OF
      `T-INT-05` ONTO NEW SURFACE, NOT A DISCHARGE OF IT.** The ID's claim is about the WHOLE
      screen; this is one actor's two components' share. W4 leaves the ID exactly as open as it
      found it, one surface narrower.
    - `Stratocracy.StratPlay.GATE-MAPMARKERS.FlagMarkerShowsForBothSidesAlways` -- Sec 2.11.2's
      "both sides, always visible" forecloses a `ViewingSide` filter. **The decisive control is the
      scenario's side-1 flag unit viewed from side 0** -- the `bIsFlag && Side != ViewingSide` pair
      -- produced by the rules module and decorated by nothing, and its evaluation is COUNTED
      (`SawEnemyFlag == 1`) so the assertion cannot be green-and-empty. `GATE-MAPMARKERS` is a
      LOCAL GATE NAME: it mints no acceptance ID and claims none.
    - `Stratocracy.StratPlay.GATE-MAPMARKERS.UnactedPipIsOwnSideAndTheDoneBit` -- units B and C
      carry **identical `bHasMoved`/`bHasActed` and opposite `bDone`**, so no function of the turn
      flags can answer both; C is additionally `bLockedThisTurn` while lit, so an added lock term
      reddens it; E is the side term's control.
  - **WHERE THE EXPECTATIONS COME FROM, AND THE ONE PLACE A TYPED EXPECTATION APPEARS.** `bIsFlag`
    is MODULE-SIDE and is read, never written -- `StratBuildViewModel` takes it from the snapshot's
    `UiUnitView::isFlag`. **`bDone` has no module-side value BY RULING** (the rules module has no
    DONE bit and no snapshot field mirrors it), so in the pip clause it is the INPUT and the
    required outcome is Sec 2.11.2's own words. That is the file's one typed expectation and its
    header says so in terms. **No assertion restates `!bDone && Side == ViewingSide` as an
    expression.**
  - **W4 MINTS NO MIRROR CLAUSE AND THAT IS A GOOD OUTCOME RATHER THAN A GAP.** `bIsFlag` and
    `bIsGuidedMarked` are already asserted against the module by name in
    `Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField`. An unstated "already covered"
    reads to the next author exactly like an oversight. This file asserts a COMPONENT against the
    model and deliberately does not re-assert the model against the snapshot.
  - **FALSIFIABILITY WAS MEASURED, NOT ARGUED -- AND THE LIMIT OF WHAT THAT MEASURED IS STATED.**
    The lane may not mutate `StratUnitActor.cpp`, so "what one-line edit reddens this?" was
    unanswerable from inside it. The probe ran the OTHER way: five assertions inverted inside this
    file alone, rebuilt, three clauses red with the expected messages, then the file restored
    BYTE-FOR-BYTE and re-run. It established that the enemy flag unit really reads visible, that
    the own-side and other-side pip readings differ, that pass 1's recording is real, and -- worth
    keeping -- that **`TestNotEqual` on two `bool`s does fail when they are equal**, so the B/C
    premise guard is not inert. **THE GATE NARROWED WHAT THIS PROVES AND THE NARROWING IS RIGHT:**
    inverting an assertion proves the line EXECUTES and its subject holds a determinate value --
    that the clause is not VACUOUS. It does NOT prove the clause is sensitive to the code under
    test; only a mutation of `ApplyUnitView` would. What carries the mutation-sensitivity here is
    STRUCTURAL rather than executed: the counted controls (`FlagsMoved > 0`, `PipsMoved > 0`,
    `SawEnemyFlag == 1`, `SawOwnFlag == 1`) and the identical-turn-flags pair, which make the
    decisive cases provably reached.
  - **WHAT THESE CLAUSES DO NOT PIN.** (1) **Pixels.** `IsFlagMarkerVisible` and
    `IsUnactedPipVisible` read `USceneComponent::IsVisible`, which NEVER consults the static mesh
    -- the limit `IsGuidedMarkerVisible` already carried. Both answer TRUE for a marker with no
    mesh, drawing nothing, and in this headless fixture every marker is meshless, so all three
    clauses run entirely on that flag. **Green here does not mean a player sees a marker, and no
    headless clause can mean that.** (2) **Placement.** Nothing reads a transform;
    `FlagMarkerOffset` and `UnactedPipOffset` stay unpinned, as their own declarations say. (3)
    **The flag marker's hide path.** That it stops drawing by ACTOR DESTRUCTION on the flag unit's
    death depends on `UStratMatchSubsystem::ApplyView`'s destruction loop, which lives in a
    different file; if that loop were weakened, no clause in this file would notice.
  - **AN INSTRUMENT FINDING, NEW AND TREE-WIDE.** A non-ASCII character inside a `TEXT()` message
    literal does NOT round-trip into `Saved/AutomationReport/index.json` -- a section sign came
    back as `U+FFFD`. Harmless to a verdict, but **it corrupts the exact message a reader quotes
    off a RED run**. Message literals in this file are ASCII and its comments keep the sign. Other
    test files have the same exposure and were not touched.
  - **KNOWN FLAKES.** None observed; this file's clauses were green on two independent full runs.

- **2026-08-28 (local; this pass's runs stamp UTC `2026.08.28-22.17.49`, `-22.19.51` and the
  FINAL `2026.08.28-22.22.24`. THE LAST ONE IS THE REPORT THAT BACKS THIS ENTRY -- the first
  described a clause carrying an undeclared Warning, the second the declaration that removed it,
  and the third the same clauses after both new files were converted from LF to CRLF to match
  every sibling in their directories. A whitespace conversion is still different bytes, and
  inference from a diff is not a run) -- W7'S SEC 2.9 DIFFICULTY HANDICAP IS PINNED BY EIGHT
  CLAUSES ACROSS THREE EXISTING IDS, AND THE ONE THAT MATTERS MOST IS THE ONE THAT NEEDED A
  NON-ZERO `fameCombat` TO BE ABLE TO FAIL AT ALL.** Written in the MAIN tree,
  `E:/MultiAgent/Stratocracy`, on `master`, against the engineer's uncommitted C++. Two new
  files, both inside `Tests/`: `Source/StratBridge/Tests/StratDifficultyHandicapClauses.cpp` (4)
  and `Source/StratPlay/Tests/StratDifficultyHandicapMatchClauses.cpp` (4). The clause-name set
  grew by exactly eight and NOTHING was removed or renamed, measured by MULTILINE SET DIFFERENCE
  on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` between `HEAD`'s git objects and the worktree, both sides
  guarded as non-empty -- never by a single-line grep, which returns zero here because the
  macro's argument sits on the next line, and never by an acceptance-ID grep. The live suite
  figure is `global.md`'s and is not restated here.
  - **THE IDS ARE THE USER'S THREE AND NOTHING WAS MINTED**, per `global.md`'s `## NEXT` ruling
    of the same day: `T-FAME-02` for the arithmetic, the tier map, the inertness and the
    refusals; `T-FAME-01` for the one `fameCombat` clause; `T-SAVE-06` for the one cross-tier
    load clause. `T-AI-07` is named in both new files' header blocks as a trap and is not
    reached for.
  - **WHICH CLAUSE PINS WHICH BEHAVIOUR, AND WHERE ITS EXPECTATION COMES FROM.**
    - `Stratocracy.StratBridge.T-FAME-02.HandicapMovesTheScenariosConfiguredFameByTheDelta` --
      the opening purse is `FStratBridge::ScenarioData().startingFame[Side]`, read per side off
      the parsed scenario, and the handicap adds the delta to THAT. **No clause in this wave
      writes 200, 350 or 100**, which is `T-FAME-02`'s own instruction to a gate. It also pins
      the zero arm as inert BY the canonical state hash (unchanged across a zero delta, moved
      across a non-zero one -- the pair is what stops a do-nothing method passing), and pins
      `RecordedCommandCount()` unchanged, which is what makes the `T-SAVE-06` clause reachable.
    - `Stratocracy.StratBridge.T-FAME-01.HandicapLeavesANonZeroFameCombatUnmoved` -- **the
      correctness clause of the wave.** At the seeded opening both counters are 0, and 0 is also
      what an uninitialised read, a wrong-member read and a zeroing bug look like, so the clause
      replays `Data/parity_fixture.save` until the MODULE reports a non-zero counter and only
      then applies the handicap. The prefix is DISCOVERED at run time, not authored -- a
      re-emitted fixture moves the index and not the clause -- and the clause fails loudly with
      its own message if no prefix reaches one. Its control is that the same call moved
      `fameTotal` by the delta it was handed.
    - `.HandicapClampsAtZeroAndReportsZero` -- the overshoot is `-(SideFameTotal + 1)`, derived
      from the live purse, so it clamps by exactly one Fame whatever the scenario configured.
      The half with teeth is `bOk == true` AND `OutFameTotalAfter == 0`: `INDEX_NONE` is the
      refusal path's sentinel, and a clamp reporting it would make `StratBridge.h`'s "reported
      rather than refused" sentence false while every arithmetic clause stayed green.
    - `.HandicapRefusesAnUnseededBridgeAndAnOutOfRangeSide` -- the out-parameter is PRE-LOADED
      with a value the method cannot produce and required to have been overwritten with
      `INDEX_NONE`, which is the property `StratBridge.h` claims for it. The out-of-range side is
      `strat::SIDE_COUNT`, the module's own count. The prose is pinned only for what it
      DISTINGUISHES -- the two refusals must differ from each other, case-SENSITIVELY, and the
      out-of-range one must name the side this clause handed it -- because a reworded message
      with identical behaviour is not a regression.
    - `Stratocracy.StratPlay.T-FAME-02.DifficultyTierDeltasAreSection29sThreeNumbers` -- **the
      one place in this wave where the expectation is a specification literal, and it is
      labelled as one in the clause's own block.** `EStratDifficulty` carries no payload by
      design, so there is nothing module-side to read a tier number from; +150 / 0 / -100 come
      from Sec 2.9 and are cited. It also pins the two DIRECTIONS, which is what survives a
      transcription error, and Sec 2.11.6's shipped default read off a default-constructed
      `FStratMatchConfig`.
    - `.HandicapIsInertWithoutASinglePlayerOpponent` -- the `coordinator`'s single-player ruling,
      asserted on `StratHandicappedSide` directly rather than inferred from a purse, plus a live
      hot seat at **Easy** (not Normal: a zero delta cannot tell inert from applied). Its control
      is the fourth arm -- `AiSides` naming the OTHER side returns `ViewingSide` -- without which
      the three `INDEX_NONE`s would all pass on a deleted handicap.
    - `.HandicapMovesThePlayersOpeningFameAtEveryTier` -- baseline MEASURED per side from the
      inert hot seat, then `player == baseline + StratDifficultyFameDelta(tier)` and `AI ==
      baseline` at all three tiers. **The two StratPlay clauses cross-check each other on
      purpose:** if the handicap ever applied to a hot seat, the baseline would shift by the same
      delta and this clause would go RED rather than absorbing the bug into its own baseline.
    - `Stratocracy.StratPlay.T-SAVE-06.ASlotWrittenAtOneTierIsRefusedAtAnother` -- written at
      Easy, loaded at Hard through `SaveMatchToSlot` / `LoadMatchFromSlot`, so it is a real
      cross-TIER load and not two deltas standing in for tiers. Its control is the identical
      route at the SAME tier, required to succeed and to reach the hash the save was taken at,
      so the refusal is attributable to the tier and not to the restart. It pins the id
      case-sensitively and pins the word `handicap` in the reason -- the one token the engineer's
      message change would lose if reverted -- and nothing else about that sentence.
  - **WHAT THESE CLAUSES DO NOT PIN, stated rather than left to be discovered.**
    - **Sec 2.9's OTHER claim -- "the baseline routine is identical at every tier; only the
      economy shifts" -- IS UNPINNED.** `StratMatchSubsystem.h` states that no branch of
      `FStratAiTurnRunner` may read `EStratDifficulty`, and that is a property of code, not of
      an observable value: an AI that consulted the tier would still open on the same purse and
      no economy assertion could catch it. Pinning it needs either a static check that the enum
      is unreachable from the runner's translation unit or a determinism clause comparing two
      tiers' command sequences at an equal purse. **Neither was written and neither is claimed.**
    - **The `fameCombat` arm inside `HandicapMovesThePlayersOpeningFameAtEveryTier` is the
      WEAKER reading and says so in its own message.** At the opening both counters are zero, so
      that arm cannot tell "left alone" from "zeroed". The clause that can is the StratBridge
      `T-FAME-01` one above; a reader chasing that property must not stop at the StratPlay file.
    - The StratBridge clauses hand the method deltas that are deliberately NOT Sec 2.9's three
      numbers, so nothing in `StratBridge/Tests/` pins the tier table -- that is the StratPlay
      clause's alone, and the two halves only meet in a running match.
  - **AN INSTRUMENT DEFECT WORTH KEEPING, because it fabricated a finding and self-corrected in
    one step.** The first run of the clause-name set-difference reported EIGHT of this pass's
    clauses plus FIVE `Stratocracy.StratData.*` ones as "added", which would have read as a
    lane violation. The cause was asymmetric filters in the measuring script: the HEAD side
    matched `Source/<one-segment>/Tests/` while the worktree side walked for ANY directory named
    `Tests`, so `Source/Stratocracy/StratData/Tests/` existed on one side of the comparison and
    not the other. **A set difference between two differently-collected sets measures the
    collectors.** Both sides now use the same rule and the answer is eight.
  - **EVERYTHING THE ENGINEER'S REPORT CLAIMED WAS CHECKED AGAINST THE TREE AND HELD**, including
    the call site's position between seeding and the restore and the refusal joining `Complaints`
    rather than tearing the match down. One wording nuance for a future greps: the refusal
    message spells the section as `Sec 2.9`, not `§2.9`.
  - **NO CODE OUTSIDE `Tests/` WAS TOUCHED AND NO MUTATION WAS RUN.** Proving a pin by reverting
    what it pins would have meant editing the engineer's C++, which is not this lane's to edit
    even temporarily. What stands in for it is that every clause carries a CONTROL that is
    required to move in the same run, named in each clause's own block.

- **2026-08-28 (local; this pass's runs stamp UTC `2026.08.28-00.00.07` and `-00.02.17` for the
  first nine clauses and their mutation, then `-00.18.43` and `-00.20.00` after the gate's
  finding added the tenth, and the FINAL run `-00.32.37` after the re-gate's two comment
  observations. THAT LAST ONE IS THE REPORT THAT BACKS THIS ENTRY; every earlier stamp is quoted
  as history and describes different bytes, a comment edit included) — WAVE 2's SEC 2.11.2 INFO PANEL IS
  PINNED BY TEN CLAUSES, ALL UNDER `GATE-INFOPANEL`, AND THREE OF THEM EXIST BECAUSE THE OBVIOUS
  VERSION OF THE CLAUSE CANNOT FAIL.** Written in the lane tree `E:/MultiAgent/Strat-wt/slot-1` on
  `feat/info-panel`. Three new files, all inside `Tests/`:
  `Source/StratUI/Tests/StratInfoPanelSourceParity.cpp` (2),
  `Source/StratUI/Tests/StratInfoPanelClauses.cpp` (6), and
  `Source/StratPlay/Tests/StratInfoPanelRouting.cpp` (2). The clause-name set grew by exactly ten
  and NOTHING was removed or renamed, measured by MULTILINE SET DIFFERENCE on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` between `master`'s git objects and the worktree, with both
  sides guarded as non-empty — never by an acceptance-ID grep, and never by a single-line one,
  which returns zero here because the macro's argument sits on the next line. The live suite
  figure is `global.md`'s and is not restated here.
  - **THE ID IS `GATE-INFOPANEL` AND IT MINTS NOTHING, and the two IDs that were refused are
    recorded because both were nearly taken.** `T-UI-03`'s own GDD sentence is about the LIVE
    STANDINGS SCOREBOARD and its four side-level readouts; the info panel is a different surface,
    and a clause riding an ID whose own text excludes its subject is unfalsifiable by
    construction. `T-UI-05` asks whether the snapshot tells the truth about the module's state —
    and the four snapshot mirrors this wave finally gives a reader (`Hp`, `HpMax`, `bIsFlag`,
    `Owner`) are ALREADY asserted against the module in `StratViewModelParity.cpp`, verified in
    this pass at the assertion sites, so a second clause would be duplication and not coverage.
    `GATE-BUILDMENU` is the precedent for the `GATE-` form and each new file states the reasoning
    in its own header block.
  - **THE TREE DID NOT CARRY THE RULING THE DISPATCH CITED, and the dispatch was followed anyway
    because `GATE-` claims nothing.** The brief said the user had ruled W2's acceptance IDs and
    that `global.md`'s topmost `## NEXT` entry carried the ruling. It does not: the topmost entry
    is W3's, and the ID ruling inside it closes with `STILL UNRULED: W2, W4, W5, W6 and W8`.
    Recorded because the discrepancy is the kind that later reads as a ruling that was made.
  - **WHICH CLAUSE PINS WHICH BEHAVIOUR.**
    - `Stratocracy.StratUI.GATE-INFOPANEL.HexTerrainFactsAreTheTerrainRowAtTheSnapshotsIndex` —
      every hex's `TerrainMoveCost`, `TerrainDefensePct`, `bTerrainCapturable` and
      `TerrainIncomeFame` equal `strat::TerrainDef`'s at the row the snapshot names, read through
      `FStratBridge::Tables()`. None of the four has a snapshot source, so the mirror walk in
      `StratViewModelParity.cpp` cannot see them and never could. Its guards are table reads too:
      the board must exercise more than one cost, bonus and rate, both capturable states, a
      NEGATIVE bonus and a ZERO cost.
    - `Stratocracy.StratUI.GATE-INFOPANEL.UnitStatsAreTheUnitRowAtItsDefIndex` — the five §2.4
      stats against `strat::UnitDef` at that unit's own `DefIndex`, plus the guard that the
      projection contains a unit whose band is NOT a point.
    - `Stratocracy.StratUI.GATE-INFOPANEL.PanelDescribesTheHoveredHexsOwnRow` — two hexes of
      different terrain, and the panels differ.
    - `.PanelIsWrittenUnconditionallyAndIsEmptyOffTheBoard` — a model carrying a REAL stale panel
      (produced by the shipped decorator a line earlier) is cleared when the hover stops and when
      the hover is off-board. The off-board hex is derived from the board's own extent and then
      ASSERTED absent.
    - `.CapturabilityIsTheTerrainFlagAndNotOwnership` — see the trap note below.
    - `.ImpassableIsTheZeroCostSentinelAndTheCostSurvives` — both arms, and the raw cost still on
      the panel in both.
    - `.UnitRowIsTheUnitStandingOnTheHoveredHex` — every unit field is that unit's own, plus the
      empty-hex arm where `bHasHex` is true and the whole unit half is default.
    - `Stratocracy.StratPlay.GATE-INFOPANEL.PanelReachesTheModelThroughTheControllersDecorator` —
      the ROUTE. All six StratUI clauses stay green over a `DecorateForPresentation` that never
      calls the decorator at all, which is this project's recorded shape for a correct,
      clause-pinned, unreachable verb.
  - **THE THREE TRAPS, AND WHY A NAIVE CLAUSE PASSES EACH.**
    1. **Capturability is not ownership.** `Ui.h` scopes `owner` to capturable hexes, so a NEUTRAL
       Factory and a Plains hex both project `INDEX_NONE`. A clause that only ever hovered an
       OWNED capturable hex certifies a `HexOwner != INDEX_NONE` shortcut. The clause shows the
       neutral-capturable / non-capturable PAIR at the same owner, which is the one observation
       the shortcut cannot produce.
    2. **The range band needs Artillery.** Infantry, Tank and Recon are all `1..1` in the shipped
       table; only Artillery is `2..3`. Over any point-band unit a collapsed band is invisible.
       Both range clauses find their subject by SEARCHING the projection for `rangeMin` unequal to
       `rangeMax` rather than by naming a unit.
    3. **The DONE bit is neither turn flag nor any pair of them.** Driving only the waited unit
       (`bDone` true, `bHasActed` false) is satisfied by `!bHasActed`, which is a real wrong
       answer. The clause's decisive pair is two units with IDENTICAL turn flags and OPPOSITE
       `bDone`, over which no function of the turn flags can be right twice.
  - **WHAT THESE CLAUSES DO NOT PIN, stated rather than left to be discovered.**
    - Any widget, any rendered text, and the four snapshot mirrors as fidelity.
  - **WHERE A CLAUSE ASSERTS RATHER THAN MEASURES, named because this record exists for it.**
    `DoneIsThePresentationBitAndNotATurnFlag` writes the three bits it then expects. There is no
    module-side value to read: `Ui.h`'s presentation block has no module-side counterpart and no
    derivation from one — the same ruling that keeps those members out of T-UI-05's subject — and
    `StratBuildViewModel` leaves all three at their defaults by design. So what that clause pins
    is the SELECTION and nothing about what the bit means. Every other clause in the three files
    compares against a module-side or model-side value read beside the assertion; no terrain name,
    unit name, cost, stat, hex coordinate or board dimension is typed in any of them.
  - **[CORRECTED 2026-08-28, SAME PASS, AFTER A `VERDICT: BLOCK` — AND THE STRUCK CLAIM IS THIS
    LANE'S OWN.** This entry first said the wave was pinned by NINE clauses and that the panel's
    ordering against `FStratSelectionMachine::DecorateViewModel` could not be pinned at all. THE
    COUNT IS TEN AND THE ORDERING IS PINNED. The struck sentence and the reason it was wrong are
    below, stated flat and never nested, because this record has had a false sentence live
    directly above its own retraction.]**
  - **WHAT THE GAP BULLET USED TO SAY, STRUCK AND KEPT.** It read:
    STRUCK> "THAT THE PANEL RUNS AFTER `FStratSelectionMachine::DecorateViewModel`. The
    STRUCK> constraint is real — `bUnitDone` comes from the machine — and it is NOT OBSERVABLE
    STRUCK> from any fixture this wave has. With no live match the machine writes `bDone` false
    STRUCK> on every unit, so a panel decorated before it and one decorated after it are
    STRUCK> byte-identical, and an assertion over that pair would be green under both orders.
    STRUCK> Closing the remaining half needs a live `UStratMatchSubsystem` fixture with a unit
    STRUCK> actually marked done."
    THAT IS WITHDRAWN IN FULL. **The premise was false and the refutation is one line of the
    tree:** `FStratSelectionMachine::DecorateViewModel` writes `U.bDone =
    DoneUnits.Contains(U.UnitId)` in a plain loop over every unit with NO GUARD — an
    unconditional, destructive assignment, read at its site in
    `Source/StratPlay/StratSelectionMachine.cpp` in this pass and not taken from the gate's
    paste. So the two orders agree only when the INCOMING model already carries `bDone` false,
    and a fixture that owns its model by value controls exactly that. "The machine writes false
    in this fixture" was true, and was mistaken for "the machine writes nothing observable" —
    which is a different sentence, and the whole of the error.
  - **THE TENTH CLAUSE.**
    `Stratocracy.StratPlay.GATE-INFOPANEL.PanelReadsTheDoneBitTheSelectionMachineWroteThisFrame`
    seeds `bDone` TRUE on the hovered unit, hands the model to `DecorateForPresentation`, and
    asserts the machine destroyed the seed and the panel carries the MACHINE'S OWN OUTPUT — the
    post-decoration `Units[i].bDone`, never a literal — so the clause takes no position on which
    units the machine considers done. **It runs its own control:** the same seeded model, hovered
    and handed straight to `StratDecorateInfoPanel`, which is what a panel decorated before the
    machine would produce, and the clause asserts the two disagree. No live match, no
    `UStratMatchSubsystem`, no new seam.
  - **THE OTHER HALF OF THE OLD BULLET STANDS AND IS NOT WITHDRAWN WITH IT.** That the panel
    reads `bDone` and not a turn flag is genuinely pinned by
    `Stratocracy.StratUI.GATE-INFOPANEL.DoneIsThePresentationBitAndNotATurnFlag`, headlessly.
    The routing file pins the two ORDERINGS; that clause pins which bit is read.
  - **AND A COUNT DEFECT INSIDE A COMMENT, FIXED BY RE-DERIVING RATHER THAN BY PATCHING.**
    `StratInfoPanelSourceParity.cpp`'s header said the new fields were "seven more of that kind",
    which is impossible beside its own "nine". Seven survived from `FStratInfoPanelView`'s
    narrower FIRST shape. The figure was re-derived from `git diff master -- StratViewModel.h`
    — four reflected fields added to `FStratHexView` and five to `FStratUnitView`, every other
    added field belonging to the new struct — and the comment now states the SPLIT that produces
    the total rather than a bare number a later field would silently falsify. The gate supplied
    a figure and it was deliberately not used as the source.
  - **[AMENDED 2026-08-28, AFTER THE RE-GATE'S `VERDICT: PASS`, ON TWO NON-GATING COMMENT
    OBSERVATIONS — AND THE FIRST IS THIS WAVE'S DEFECT FOR THE THIRD TIME.**
    `StratInfoPanelRouting.cpp` carried a FILE-SCOPE sentence, `The hover is driven through the
    controller's own public SetHoveredHex / ClearHoveredHex seam, never by writing Model.Hover`.
    It was true when the file held one clause. The tenth clause's CONTROL leg writes
    `Model.Hover` directly — disclosed at its own line, with the reason that this lane may not
    edit the controller — which made the file-scope sentence false, and nothing re-read it. It is
    now stated PER CLAUSE, with the old wording quoted under `NARROWED>` rather than deleted.
    **Three instances of one species in one wave:** a struck ordering claim, a stale field count,
    and this. The rule that catches it is to re-read every unqualified file-scope sentence when a
    file gains a second clause — not to write more carefully.]**
  - **AND ONE OVERCLAIM THIS LANE INTRODUCED WHILE FIXING THE SECOND OBSERVATION, caught by
    re-reading its own new sentence against the code it cited.** The narrowed scope note first
    said the fixture's premise `is therefore asserted false a few lines below, so that premise
    fails at its own line`. **That is wrong:** the `TestFalse` it pointed at guards
    `StratBuildViewModel` leaving `bDone` false, and would keep PASSING if the machine ever began
    reporting the subject unit done, because the builder runs before the machine and the machine
    never touches that model. The comment now says the premise is NOT separately guarded, tells a
    reader to check the machine's answer before concluding the ordering broke, and names
    `StratSelectionMachineParity.cpp` as where guarding it would belong. Recorded because a
    correction that cites the wrong line reads exactly as authoritative as one that does not.
  - **FALSIFIABILITY FOR THE TENTH, instrument-side like the other nine.** The reordering was
    simulated in the test — the panel value a controller that decorated before the machine would
    leave on the model, driven through both of the clause's ordering assertions — and the red set
    was EXACTLY that one clause, with both assertions firing: `the panel's bUnitDone is the bDone
    the machine left on the model this frame ...: The two values are not equal` and `the shipped
    order and the reordered control disagree ...: The two values are equal`. Restored by `sha256`
    against a copy held outside the repository, rebuilt, re-run green. The limit is the same one
    stated for the first nine: instrument-side mutation proves each assertion reachable and
    sensitive to the quantity it names, not that the clause catches an arbitrary defect.
  - **FALSIFIABILITY, AND THE PROOF STATES ITS OWN LIMIT.** Nine simultaneous mutations, one per
    clause, gave the red set EXACTLY the nine added names with no pre-existing clause moving; the
    restore was verified by `sha256` against copies held outside the repository and the tree
    re-run green. **EVERY MUTATION WAS INSTRUMENT-SIDE**, because this lane may not edit the code
    under test even temporarily. Three of the nine implement the WRONG RULE over the clause's own
    fixture rather than merely breaking an assertion, which is the stronger form: the ownership
    shortcut evaluated over the neutral capturable hex printed `Expected 'the neutral capturable
    hex (q 3, r 1) reports bHexCapturable' to be true`; reading `bHasActed` in place of `bDone`
    reddened two of the four fixture units; and collapsing `StatRangeMin` onto `rangeMax` reddened
    ONLY the two units at definition row 2, which is the measurement that says Artillery is
    load-bearing. That proves each assertion reachable and sensitive to the quantity it names; it
    does NOT prove the clause would catch an arbitrary defect in the shipped function.
  - **THE FAILING RUN'S EXIT CODE WAS 255 AND THE GREEN RUNS' WAS 0**, captured on the line after
    the redirect and never through a pipe. Recorded as a measurement of this harness on this box,
    not as a rule.

- **2026-08-27 (local; this pass's runs stamp UTC `2026.08.27-20.56.24`, `-20.57.47`,
  `-20.59.19` and the final restored-tree run `-21.02.15`) — WAVE 3's PERSISTENT-HUD MODEL SURFACE IS PINNED BY FIVE CLAUSES, ACROSS TWO
  EXISTING ACCEPTANCE IDS, AND TWO SURFACES OF THAT SAME WAVE ARE DELIBERATELY LEFT UNPINNED.**
  Three new files, all inside `Tests/`:
  `Source/StratUI/Tests/StratProductionPurseClauses.cpp` (2),
  `Source/StratUI/Tests/StratViewingSideSelectorClauses.cpp` (2), and
  `Source/StratPlay/Tests/StratProductionPurseSeamClauses.cpp` (1). The clause-name set grew by
  exactly five and NOTHING was removed or renamed, measured by SET DIFFERENCE on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` between `HEAD` and the worktree — never by an acceptance-ID
  grep, which is useless here by construction since the five share two IDs. The live suite figure
  is `global.md`'s and is not restated here.
  - **WHICH CLAUSE PINS WHICH BEHAVIOUR.**
    - `Stratocracy.StratUI.T-UI-04.ReturnedPurseIsTheOneEveryShortfallWasComputedAgainst` —
      on ONE call of `StratBuildProductionMenu`'s six-argument overload, every unaffordable row's
      `Shortfall` equals `CostFame - OutPurseFame`, and `OutPurseFame` equals
      `strat::UiSnapshot::side[Side].fameTotal` read off the same bridge in the same frame. The
      §2.11.5 defect it forbids — a header printing `Fame: 250` above a 275-cost row reading
      `need 50` — is thereby structural rather than asserted.
    - `Stratocracy.StratUI.T-UI-04.TheFiveArgumentFormIsTheSixArgumentFormMinusThePurse` —
      the one-line forward agrees with the implementation on rows, on order, on every field
      (`Id` and `Reason` compared CASE-SENSITIVELY), on refusing, and on the refusal sentence.
    - `Stratocracy.StratPlay.T-UI-04.PublishedHeaderPurseIsTheOpenMenusPurseAndZeroWhenClosed` —
      `UStratMatchSubsystem::ProductionMenuFameTotal` across four stations of one cycle: never
      opened, open, AFTER A BUILD (the panel stays open and the header must have moved), and
      closed. The build station is the one a write-once member fails.
    - `Stratocracy.StratUI.T-INT-05.SideSelectorReturnsTheModelsRowAndRefusesOutOfRange` —
      **[RENAMED 2026-08-29. This clause and the one below it were written as
      `Stratocracy.StratUI.T-UI-05.<same clause>` and are recorded here under the names they
      carry NOW, because a record that names a clause that no longer exists sends the next
      reader to a grep that returns nothing. Only the ID moved: no assertion, expectation or
      fixture changed, and the suite count did not move. The ruling is `global.md`'s — `T-UI-05`
      stops at the snapshot, and a clause whose comparison crosses the bridge is `T-INT-05`'s.
      Neither this entry's account of what the two clauses assert, nor wave 3's, is otherwise
      affected.]** **[AND THE ACTOR IS NAMED HERE RATHER THAN ONLY IN `global.md`: this file
      was edited by the `coordinator`, NOT by `strat-test-author`, on the user's direct
      instruction. This file's header declares a sole writer and that writer did not make this
      edit; the user instruction is the only thing licensing it, so the file that RECEIVED the
      write is where it has to be readable. The same applies to the two other markers dated
      2026-08-29 below.]**
      `GetSideView` yields `Model.Sides[i]`; an out-of-range side, a negative side, a model whose
      own `ViewingSide` is out of range and an empty model all REFUSE and leave `OutSide`
      default-constructed. `GetViewingSideView` agrees with `GetSideView(Model.ViewingSide, ...)`
      on both the answer and the refusal.
    - `Stratocracy.StratUI.T-INT-05.ViewingSideSelectorTracksTheViewerAndNotTheSideToMove` —
      on a model rendered for side 1 while side 0 is to move, the selector returns the VIEWER's
      row and provably not the side-to-move's. This is the clause that protects the `+X/turn`
      line, because `FStratSideView::IncomePerTurn` had ZERO readers before this wave and this
      selector is now its only route to a widget.
  - **WHAT THESE CLAUSES DO NOT PIN, STATED SO NOBODY READS THEM WIDER THAN THEY ARE.** No widget,
    no Slate, no binding: wave 3 is model-side and T-UI-04's own BINDING is still blocked on a
    human-driven playtest, exactly as `StratProductionMenuRouting.cpp`'s header already records.
    The menu's ROUTING (rows are the bridge's rows, in the bridge's order, byte for byte) remains
    GATE-BUILDMENU's and is not restated. That `FStratSideView` mirrors `strat::UiSideView` field
    for field remains `StratViewModelParity.cpp`'s.
  - **TWO WAVE-3 SURFACES ARE UNPINNED ON PURPOSE, AND THE SILENCE IS DELIBERATE RATHER THAN AN
    OVERSIGHT.** `AStratPlayerController::RequestEndTurn` and
    `UStratViewModelLibrary::CountUnitsAbleToAct` / `CountViewingSideUnitsAbleToAct` got NO clause
    from this lane. Per the user ruling of this date, the GDD's UI acceptance set is
    `T-UI-01`..`T-UI-05` and neither surface falls under any of them; the idle count's subject is
    `bDone` and `bLockedThisTurn`, which `StratViewModel.h`'s PRESENTATION BLOCK rules OUT of
    T-UI-05's subject in terms — they have no module-side counterpart for a fidelity clause to
    compare against. An upstream mint is being filed. **No ID was stretched to reach them and none
    was invented.** Each of the two new files whose subject neighbours those surfaces names its
    own omission in its header block, so the gap is legible from the code as well as from here.
  - **A FIXTURE HAD TO DRIVE THE TWO SIDES APART BEFORE THE VIEWER CLAUSE COULD FAIL AT ALL, AND
    THIS IS THE MOST REUSABLE THING THIS PASS LEARNED.** `Data/ferrum_crossing.json` seeds
    `startingFame` at 200/200 on a symmetric board, so at turn 1 `Sides[0]` and `Sides[1]` can
    carry identical rows — and a clause asking "did the selector return the VIEWING side's row"
    against two identical rows CANNOT FAIL whichever row it returns. The fixture therefore has
    side 0 spend Fame on a real build first, and the clause then ASSERTS the divergence (both the
    index divergence and the purse divergence) before relying on it, bailing loudly if either is
    ever untrue. Any future clause distinguishing two sides on this scenario needs the same step.
  - **A REFUSAL AND AN ANSWERED NO ARE DIFFERENT THINGS HERE, AND THE FIRST DRAFT GOT IT WRONG —
    MEASURED, NOT REASONED.** `StratBuildProductionMenu` at a hex that is NO FACTORY does not
    refuse; it answers with rows whose `bAvailable` is false, which
    `StratProductionMenuRouting.cpp`'s `AnAnsweredNoIsNotARefusal` already pins. The forward
    clause's refusal arm was consequently EMPTY and the clause went RED on its own vacuity guard
    on the first run. The refusal is reserved for a MALFORMED QUESTION — a side outside the match.
    The vacuity guards on both arms are what turned a silently half-tested clause into a red one;
    they are load-bearing and must not be removed.
  - **HOW EACH CLAUSE WAS PROVED FALSIFIABLE, AND THE HONEST LIMIT OF THAT PROOF.** All five were
    reddened in ONE differential run with five simultaneous, independent mutations, then restored
    and re-run. Exactly five went red and no other clause moved, which is itself the no-collateral
    evidence. **EVERY MUTATION WAS INSTRUMENT-SIDE**, because this lane may not edit the code under
    test even temporarily; four of the five are shaped like the defect the clause forbids (a
    transposed `q`/`r` on the forward's hex; an off-by-one range bound; reading the PRE-BUILD purse
    where the published one belongs; comparing against `Sides[SideToMove]` instead of
    `Sides[ViewingSide]`), and the fifth — perturbing the returned purse by one before the
    assertions read it — is a plain sensitivity probe and is the weakest of the five. **What this
    proves is that each assertion is reachable and sensitive to the quantity it names. It is NOT a
    proof that the clause would catch an arbitrary defect in the shipped function**, which would
    have required mutating `Source/StratUI/StratViewModel.cpp` or `StratMatchSubsystem.cpp` —
    outside this lane, and not done.
  - **A `.pristine` COPY WAS TAKEN BEFORE THE MUTATIONS AND THE RESTORE WAS VERIFIED BY SHA-256
    AGAINST IT**, not by re-reading the files and not by a diff against `HEAD` — all three files
    are UNTRACKED, so `HEAD` carries no blob to compare against and a `numstat` check would have
    been silently vacuous. A grep for the mutation markers returning zero is a second, independent
    check and was also run.
  - **THE INSTRUMENT CAVEATS THIS PASS HONOURED, EACH ALREADY THIS RECORD'S.** `FString` `==`,
    `Contains`, `FName` comparison and `TestEqual` are all CASE-INSENSITIVE here, so the forward
    clause compares `Id` and `Reason` through `FString::Equals(..., ESearchCase::CaseSensitive)`.
    The out-parameter is PRE-POISONED before every refusing call, because `GetSideView` resets
    `OutSide` on entry and without a poison "left default-constructed" reads identically on a
    function that never touched it. The refusal expectation is a freshly default-constructed
    `FStratSideView()` — the module-side default asked for, never five zeroes typed out — so the
    clause moves if that struct ever grows a non-zero default. `IsProductionMenuOpen()` is asked
    of the subsystem at every station rather than inferred from the preceding call.
  - **NO REGRESSION.** The mutant run's green set is exactly the clause set that was green before
    this pass, so the failures were the five mutations and nothing else.
- **2026-08-27 (local; this pass's own build and suite run stamp UTC `2026.08.27-21.06.06`) —
  W1'S FORECAST CARD IS PINNED AT THIRTEEN CLAUSES ACROSS THREE MODULES, AND THE ONE DEBT THE
  ENGINEER CALLED SHARPEST IS DISCHARGED.** Three new files:
  `Source/StratBridge/Tests/StratForecastCardParity.cpp` (5 clauses),
  `Source/StratUI/Tests/StratForecastViewClauses.cpp` (5), and
  `Source/StratPlay/Tests/StratForecastDecoratorClauses.cpp` (3). The clause-name set grew by
  exactly thirteen and NOTHING was removed or renamed, measured by SET DIFFERENCE on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` between `git grep HEAD` and the worktree. The live suite
  figure is `global.md`'s and is not restated here.
  - **EVERY CLAUSE RIDES `T-UI-01` EXCEPT ONE, WHICH RIDES `T-FAME-01`, AND NO ID WAS MINTED.**
    The engineer's report proposed `T-UI-03` for the compose clauses and `T-INT-05` for the
    decorator ones; the coordinator ruled both out and this lane wrote neither. `T-UI-03`'s
    subject is the standings scoreboard and its no-widget-arithmetic sentence is a design rule
    cited across the GDD, not a licence to name forecast clauses under it. The single
    `T-FAME-01` clause is the kill award, which is that ID's own subject.
  - **THE STRETCH IS DECLARED IN EVERY CLAUSE THAT TAKES IT.** `T-UI-01`'s own sentence is that
    the forecast EQUALS the resolution. Exactly one clause asserts that equality
    (`ForecastCardHpAfterIsTheOracles`); the other twelve assert things the forecast card NEEDS
    — a mapping, a table read, a visibility rule, an ordering constraint — and each says so in
    its own comment rather than writing itself as though the ID's claim covered it.
  - **DEBT 1 IS DISCHARGED AND IT IS THE ENTRY WORTH REMEMBERING.** The two HP-after clamps in
    `FStratBridge::AttackForecast` were the only numbers on §2.11.3's card that no test compared
    against anything. `ForecastCardHpAfterIsTheOracles` calls `strat::uiResolveForGate` from
    `Source/StratBridge/Tests/` — which a TEST may do and PRODUCTION may not, on
    `StratCombatOutcomeParity.cpp`'s own construction — over three positions: a fixture attack
    with a firing counter, a fixture kill, and a SEARCHED counter-kill at prefix 20, which is the
    only position in this project where the attacker's clamp reaches its floor. One candidate was
    found at that prefix this pass; if it ever reaches zero the clause FAILS loudly rather than
    passing having proved nothing.
  - **WHAT THE COUNTER-REASON CLAUSE PINS, AND WHY IT IS NOT A COPY OF ITS OWN SUBJECT.**
    `StratComposeForecastView` chooses `EStratCounterReason` out of exactly two booleans, so a
    clause asserting the enum against those two booleans would stay green over any rearrangement
    of them and over both lie-vectors the engineer named.
    `Stratocracy.StratUI.T-UI-01.CounterReasonMatchesTheDefendersBand` witnesses each arm with a
    fact the mapping never consulted: `DefenderDestroyed` against the bridge's own
    `DefenderHpAfter` clamp, and `Number` / `OutOfRange` against the DEFENDER'S OWN
    `[rangeMin, rangeMax]` read off the projection. `out of range` is asserted only where the
    distance genuinely is outside that band. If `Combat.h::defenderCanCounter` ever gains a
    second refusal reason, this clause reddens — which is precisely what the engineer said no
    numeric parity gate could see.
  - **THE MODULE A CLAUSE LIVES IN WAS DECIDED BY THE LINKER AND ONE OF THE DECISIONS WAS
    MEASURED THIS PASS.** A first revision of the StratUI file drove its board off
    `Data/parity_fixture.save`; `strat::parseSave` is a vendored free function and the build
    returned **1 x `LNK2019`** on `UnrealEditor-StratUI.dll` naming `?parseSave@strat@@...`,
    then `LNK1120`. **A StratUI or StratPlay test cannot read the fixture at all.** The fix was
    to drive the board with §2.9's own AI instead — `FStratBridge::NextAiCommand` plus the
    engine-typed `SubmitMoveToHex` / `SubmitAttackAtHex` / `SubmitBuildAtHex` / `SubmitEndTurn`
    — which reaches mid-match boards from any module and has BETTER provenance than a fixture
    for a UI clause, because every position is the rules module's choice. 64 AI commands, 9
    sampled boards, all three counter arms reached. Note also that `Board::terrainAt` is an
    out-of-line member of a vendored struct and links no better than a free function.
  - **WHAT EACH CLAUSE DOES NOT PIN, stated where the gap is.**
    - `ForecastKillAwardIsTheModulesAward` pins `KillAwardFame == strat::killAward(victim, isFlag)`
      over 64 lethal and 426 survivable forecasts, and proves the `isFlag` argument load-bearing
      by asserting `killAward(def,true) != killAward(def,false)` on the loaded tables. It does
      **NOT** exercise the flat-500 arm through `AttackForecast`: **no lethal attack on a flag is
      reachable from any fixture position swept** (0 of 64 kills), and the clause reports that in
      an `AddInfo` rather than implying otherwise. The 500 is never typed anywhere.
    - The three decorator clauses pin WHEN there is a card, WHICH hex it is about, and that the
      model's field is written on all five no-card paths. They pin nothing about
      `AStratPlayerController::DecorateForPresentation`'s call site — that the forecast decorator
      is placed after the hover one in that method is asserted by no clause. What IS pinned is
      that the ordering CONSTRAINT is real: run before the hover, the decorator demonstrably
      composes last frame's hex.
    - `ForecastLegalityIsTheLitTargetSet` overlaps the pre-existing
      `T-UI-01.AttackTargetsAreForecastLegal` and is strictly stronger (set equality in BOTH
      directions, over every hex on the board, on `AttackForecast` rather than on `Forecast`).
      Neither supersedes the other; the older one names its failures more narrowly.
  - **THE INSTRUMENTS THAT MAKE THESE FALSIFIABLE, because three of them are the whole clause.**
    (1) The decorator's unconditional-write clause pre-populates `Model.Forecast` with a card
    loud on all nineteen fields, because an unconditional writer called with a default payload
    reads identically to a writer never called. (2) The spent-unit clause asserts THE QUERY WAS
    NOT ASKED — a clause that only observed "no card" would stay green with the gate deleted.
    (3) Every "should be empty" is a field-by-field comparison against a default-constructed
    `FStratForecastView`, never a typed zero. Also: `FString` comparison is case-INSENSITIVE
    here, so the one reason-string assertion uses `ESearchCase::CaseSensitive` explicitly, and
    the `FName` comparisons use `TestTrue` on `==` rather than `TestEqual` on two `FString`s.
  - **A `TestEqual` OVERLOAD FOR `FIntPoint` DOES NOT EXIST.** The hover-ordering clause compares
    hexes componentwise for that reason, which also makes an offset/axial confusion legible in
    the failure text instead of hidden by an even row.
  - **`Source/Stratocracy/StratData/Tests/` IS A FOURTH TESTS DIRECTORY** and a glob of
    `Source/*/Tests/*.cpp` does not reach it. A first set-difference this pass used that glob on
    one side and `git grep HEAD -- 'Source/*/Tests/*.cpp'` on the other, and reported five
    `StratData` clauses as REMOVED that were never touched. The instrument must be identical on
    both sides; re-measured with `Source/**/Tests/*.cpp` on both, the removed set is empty.
  - **ALL THIRTEEN WERE PROVED TO GO RED, BY SUBJECT-SIDE MUTATION OF THE SHIPPED FUNCTIONS.**
    Escalated first and authorised in writing by the coordinator, who took independent copies of
    the three production files outside the repo BEFORE any edit — the restore route was a copy
    from that directory and never `git checkout --`, which in this tree would have destroyed the
    engineer's uncommitted W1 work AND rewritten the line endings on the way in. Thirteen
    mutations applied at once, one build, one suite run: **231 green, 13 red, and the thirteen
    red names were exactly the thirteen added names. Not one pre-existing clause moved.** Every
    clause failed on its OWN intended assertion, not on a crash or a neighbour's — the
    counter-reason clause failed with `the card says 'out of range' at distance 1, which is
    INSIDE unit 10's own band [1, 1]`, which is precisely the lie-vector it was written to catch
    and which no numeric parity gate could have seen. Restored, rebuilt, re-run to green;
    the restore was verified **by content**, `sha256` against the pre-mutation digests
    (`ec5be618…`, `4f2e01fb…`, `1ab4fde6…`) with CR==LF at 1757/635/107, never by re-reading the
    files. **THE MUTATIONS WERE THE ONLY EDIT MADE OUTSIDE `Tests/`, and nothing else in those
    three files was touched** — no fix, no tidy, and no defect was found to report.
  - **WHAT THE COMBINED RUN PROVES, AND WHAT IT DOES NOT.** It proves each of the thirteen
    assertions is REACHABLE and SENSITIVE to its subject. It does **NOT** establish a one-to-one
    mutation→clause mapping, because thirteen mutations were live at once and two clauses are
    over-determined by construction: `ForecastCardRepeatsTheModulesForecast` is reddened by BOTH
    the HpBefore transposition and the legality suppression, and
    `ForecastDecoratorWritesUnconditionally` by BOTH the dropped up-front reset and the deleted
    act gate. For those two the claim is "red under the mutant set", not "red under its own
    mutation". The other eleven have a single plausible cause and their failure messages name it,
    but the run itself cannot separate the cases. **These are SUBJECT-side mutations in the
    shipped functions, which is the stronger kind** — W3's lane this session could reach only
    instrument-side mutations because it could not reach the code under test. That difference is
    worth keeping and worth not overstating: it makes these proofs about the production code,
    not about the fixtures.

- **2026-08-27 (local; this pass's own runs stamp UTC `2026.08.27-19.18.01`, `-19.20.22` and
  `-19.22.02`) — THE HOVER'S INPUT ROUTE IS PINNED FOR THE FIRST TIME, BY ONE CLAUSE, AND THE
  CLAUSE WAS PROVED TO FAIL BEFORE IT WAS BELIEVED.** One clause added,
  `Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath`, into the existing
  `Source/StratPlay/Tests/StratHoverInputClauses.cpp`. It rides `T-UI-01` by the same user ruling
  that covers wave 0's five; NO NEW ACCEPTANCE ID WAS MINTED. The clause-name set grew by exactly
  one and nothing was removed or renamed, measured by SET DIFFERENCE on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `git grep HEAD` versus the worktree — never by an
  acceptance-ID grep, which is useless here by construction since all six wave-0/hover clauses
  share one ID. The live suite figure is `global.md`'s and is not restated here.
  - **WHY THIS ONE MATTERS MORE THAN ITS SIZE.** Wave 0's hover rode an `Axis2D` `UInputAction`
    bound `ETriggerEvent::Triggered`. The binding was correct, live, warning-free and green, and
    the engine delivered essentially nothing through it — the handler ran ONCE in three and a half
    minutes, on a focus transition. **NO CLAUSE COULD HAVE CAUGHT THAT AND NONE DID**, because
    nothing in this project's automation reaches `UPlayerInput`; the record said so at the time
    and was right. The route is now `AStratPlayerController::Tick` calling
    `UpdateHoverFromCursor`, and a tick is something a fixture can deliver. That is the whole
    change in testability: the hover's input route went from unpinnable-by-construction to pinned.
  - **WHAT THE CLAUSE PINS.** A live spawned `AStratPlayerController` is given a hover through the
    shipped public `SetHoveredHex` seam, the plant is asserted STANDING (on the controller and on
    a model its own `DecorateForPresentation` wrote), the controller is TICKED, and it then
    reports no hovered hex on both readers. A second tick does not put it back.
  - **WHAT IT DOES NOT PIN, AND THIS IS THE HONEST LIMIT.** It cannot distinguish `Tick` calling
    `UpdateHoverFromCursor` from `Tick` calling `ClearHoveredHex()` directly. With no seeded match
    `GetBoard()` is null, so `HexUnderCursor` returns false before it would need a viewport, and
    both routes end at the identical `ApplyHoverChange(Hover.ClearHoveredHex())` with no other
    observable. **Only the OFF-BOARD branch is reachable headlessly.** The on-board branch — that
    a cursor over a hex produces THAT hex — needs `GetHitResultUnderCursor` and therefore a
    viewport; it was witnessed by a human across 25 distinct hexes and is `content.md`'s, not a
    clause's. Pinning the callee would need a seam that does not exist, and this lane did not
    create one.
  - **THE PLANT IS WHAT MAKES THE ASSERTION FALSIFIABLE AT ALL.** Without it, "not hovering after
    a tick" is indistinguishable from a controller that was never hovering — this record's
    own recorded shape where a default read cannot tell silence from an empty call. The
    pre-condition legs are therefore load-bearing, not decoration, and the clause bails with an
    `AddError` if the plant does not stand rather than reading a default and calling it a pass.
  - **NO COPY OF THE SUBJECT IS HELD.** The clause constructs no `FStratHoverState`; every value
    read comes back out of the spawned controller. The only default-constructed object is an
    `FStratHoverView`, and that is the MODULE-SIDE expectation for "cleared", asked rather than
    typed. Every hex is enumerated off a `StratBuildViewModel` built from the shipped scenario —
    no hex literal. Every boolean asserted is a shipped function's own return value.
  - **`Tick` IS `protected`, AND THE HANDLE TYPE IS THE FIX.** `Controller->Tick(dt)` through an
    `AStratPlayerController*` does not compile. `AActor::Tick` is `public` and `APlayerController`
    does not re-declare it, so an `AActor*` handle compiles — access is checked on the STATIC type
    while dispatch stays virtual. **VERIFIED BY COMPILING IT, not taken from the brief**, which is
    worth noting because a previous handoff got this wrong and a gate caught it.
  - **FALSIFIABILITY: MEASURED IN BOTH DIRECTIONS, ON A CONTENT-ADDRESSED MUTATION.**
    `AStratPlayerController::Tick`'s call was commented out; build green; the suite ran with
    **exactly one failed entry and it was this clause**, on six assertion legs including the
    headline one. The file was then restored BY COPYING A PRISTINE BACKUP rather than by re-typing
    the line, and `sha256sum` confirmed byte identity with the pre-mutation file
    (`dff277b2e4f3a4fb117034cdc067500f2f9dfe1e411fd2ace48a1b7875404037`), with `git status` clean
    of the controller and a zero-count census of the mutant token. Rebuild green, suite green,
    clause Success. **The restore was verified by content and not by a count**, on this project's
    recorded finding that a count-based integrity check cannot see a corruption that changes the
    count and changes it back.
  - **THE MUTATION TOUCHED A FILE OUTSIDE THIS LANE, TEMPORARILY AND UNDER EXPLICIT INSTRUCTION,
    AND THAT IS DECLARED RATHER THAN LEFT TO BE NOTICED.** The dispatching brief both mandated the
    probe ("the falsifiability step is the deliverable as much as the clause") and, in its scope
    section, forbade editing `StratPlayerController.cpp`. The two directions contradict; the probe
    was run because it was the specific, detailed instruction and because this file's own
    precedent is that a hover clause is proved by reverting what it pins. The tree ends byte-equal
    to where it started outside `Tests/`. **A future pass should not read this as a standing
    licence** — it was a single reverted probe with a hash check on both ends.
  - **ONE FURTHER THING THE MUTANT RUN PROVED THAT WAS NOT ASKED FOR.** EVERY OTHER CLAUSE IN THE
    SUITE stayed green with the hover's tick call deleted — no figure is written here, deliberately;
    `global.md` owns the count and the delta above is a delta, not a total.
    That is the measurement of how blind the rest of the
    suite is to this route, and it is the same shape as the phase-6 `bCanEverTick` finding: the
    defect that the whole suite is silent about is exactly the one worth a clause.
  - **AMENDED, NOT ADDED — `Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches`.** Its
    assertion is unchanged and was correct throughout; it protected TWO mechanisms and named ONE.
    Since the hover is polled on `Tick`, `bCanEverTick == false` now stops the hover dead as well
    as Enhanced Input, with the same total silence. The rationale and the failure message now
    state both, and the two clauses are cross-referenced: **T-UI-02 pins that the engine WILL call
    `Tick`; T-UI-01.TickDrivesTheHoverPath pins that `Tick` moves the hover when called. Neither
    alone is the route, and neither re-asserts the other's subject** — the new clause calls the
    override directly and reads no tick flag, which is why it stays green with the flag off. That
    is deliberate on both sides and is stated in both files.
  - **PROSE CORRECTED IN `StratHoverInputClauses.cpp`, IN PLACE AND AS HISTORY.** Two sites
    described a `HoverAction` BINDING that no longer exists — "deliberately out of scope" and
    "THAT `HoverAction` IS BOUND" — which read as LIVE SCOPE STATEMENTS rather than as history: a
    reader deciding what was safe to remove would have taken them to mean the binding exists
    somewhere and is merely untested. Both are quoted verbatim and struck rather than deleted,
    because they record why those clauses were scoped as they were. Three further counts-of-a-
    growing-set were amended in the same pass and are named because each was a live defect and not
    tidying: the file header's "THESE FIVE CLAUSES", an ORDINAL ("the fifth clause") into a file
    that grows, and `StratPlayerControllerTick.cpp`'s "all 66 other tests in this suite" — a stale
    suite count sitting in a source file, replaced with "every other test" and no figure.

- **2026-08-27 (local; the run's own reportCreatedOn is UTC 2026.08.27-03.59.15) — WAVE 0's HOVER INPUT, FIVE CLAUSES, AND TWO LEGS THAT COULD NOT FAIL.** New file
  `Source/StratPlay/Tests/StratHoverInputClauses.cpp`; the clause-name set grew by exactly five,
  measured by set difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` and not by an acceptance-ID
  grep — every wave-0 clause shares `T-UI-01` by the user's ruling, so an ID grep is useless here
  by construction. Nothing was removed. The live suite figure is in `global.md` and is not
  restated here.
  - **THE FILING IS `T-UI-01` AND NOT ONE OF THESE CLAUSES ASSERTS `T-UI-01`'s OWN SENTENCE.**
    That sentence is *the forecast equals the resolution*, and it lives in
    `Stratocracy.StratBridge.T-UI-01.CombatOutcomeAgreesWithForecast`. Wave 0 has no forecast at
    all. What the five pin is the NECESSARY CONDITION underneath it — that a hovered hex reaches
    `FStratViewModel::Hover`, and WHICH hex reaches it — because a card drawn for the wrong tile
    is wrong before `FStratBridge::Forecast` is ever called. The file's own header block says this
    in as many words so that a future reader cannot mistake the id for a claim about coverage.
  - **WHAT THE FIVE PIN.** `HoveredHexReachesTheViewModel` — hex identity on the model, two board
    hexes through one `FStratHoverState`, so a decorator writing a constant is caught.
    `ClearedHoverLeavesNoStaleHex` — a cleared model is a complete statement, both fields.
    `HoverChangeIsReportedOnlyWhenItMoves` — the setters' own return value, including the
    clear-then-same-hex leg. `AnUndecoratedModelIsNotHovering` — `StratBuildViewModel` has no
    opinion about the cursor, with a positive control. `ControllerHoverRouteReachesTheModel` —
    the live `AStratPlayerController` route, `SetHoveredHex` → `GetHoveredHex` →
    `DecorateForPresentation`.
  - **THE FIRST DEFECT WAS MINE AND IT IS THE ENTRY'S POINT. A CLEAR THAT ALSO WIPES THE
    COORDINATE MAKES THE RE-ENTRY LEG UNFALSIFIABLE AT EVERY HEX BUT ONE.**
    `FStratHoverState::SetHoveredHex`'s change test is `!bHasHoveredHex || HoveredHex != Hex`, and
    the whole worth of the third clause was the left disjunct — the drag across a unit's mesh and
    back onto the tile under it. But `FStratHoverState::ClearHoveredHex` resets the coordinate as
    well as the flag, so after a clear the held coordinate is the default and re-entering ANY
    other hex answers true on the right disjunct alone. The first version re-entered an arbitrary
    board hex and stayed **green** over a mutant that deleted `!bHasHoveredHex ||` outright. The
    leg now re-enters the board hex that IS the default, found by `BoardHexEqualToTheDefault`,
    and the same mutant reddens it. *A leg aimed at one disjunct must be run at a value where the
    other disjunct is false; nothing about the test's prose reveals that it is not.*
  - **THE SECOND IS A PROPERTY OF THE SHIPPED CODE, AND IT LIMITS WHAT A GREEN HERE MEANS.**
    "No stale hex" is defended in THREE places — the reset in `ClearHoveredHex`, and the
    `bHasHoveredHex ? ... : ...` masks in `FStratHoverState::DecorateViewModel` and
    `FStratHoverState::GetHoveredHex` — and any two cover for the third. Deleting the reset ALONE
    left all five clauses green; only deleting the reset and both masks reddens
    `ClearedHoverLeavesNoStaleHex`. **So a green there is not evidence that the reset line is
    live.** It is evidence that no stale coordinate is READABLE through either public reader,
    which is the property a consumer depends on and the strongest one reachable without a seam
    into the private field. Recorded on the clause itself as well as here.
  - **WHAT NO CLAUSE HERE PINS**, stated so a later wave does not assume it: that `HoverAction` is
    BOUND, or bound to `ETriggerEvent::Triggered` — **[STAMPED 2026-08-27, LATER THE SAME DAY.
    THE REASON GIVEN HERE HAS GONE STALE IN BOTH DIRECTIONS AND THE ITEM ITSELF NO LONGER HAS A
    SUBJECT. The wording is kept verbatim rather than deleted: `the asset is null in C++ by the
    project's own rule 4 and is authored on a Blueprint in a separate editor batch, so there is
    nothing yet to name`. FIRST it went stale by UNDER-claiming: that batch landed, `IA_Hover`,
    its `IMC_Selection` row AND the Blueprint default were all authored and byte-verified, so
    there was an asset to name. THEN the whole subject was removed: the `Axis2D` binding was
    measured dead on a human playtest and `HoverAction`, `OnHover`, the binding and both assets
    are gone. So this item is not an outstanding gap to be closed later — IT HAS NO SUBJECT. A
    reader who meets it looking for the unpinned half of the hover should read the topmost entry
    in this file instead. Left standing, it would have sent that reader to build a clause for a
    property that does not exist, which is the under-claiming failure this record has recorded
    before.]** — that a mouse move produces a hover — nothing in this suite drives `UPlayerInput`; that
    `AStratPlayerController::UpdateHoverFromCursor` resolves anything — it calls
    `HexUnderCursor`, which needs a viewport, a local player and a cursor no automation test has;
    and that `AStratPlayerController::ApplyHoverChange` REFRESHES — `RefreshFromMachine` refuses
    in a fixture with no live match and the shipped code returns true anyway, deliberately.
  - **EVERY EXPECTATION IS MODULE-SIDE.** Every hex is enumerated off a model
    `StratBuildViewModel` built from the shipped scenario; there is not one hex literal and no
    hex arithmetic in the file. "Not hovering" is a default-constructed `FStratHoverView`, asked
    rather than typed, so the assertion moves if the struct's default ever does. No string is
    compared anywhere in the file — wave 0 produces no text, which is the cleanest available
    handling of this project's case-insensitive-`FString` hazard.

- **2026-08-25 (second pass that day), WRITTEN BY THE `coordinator`, NOT BY `strat-test-author` --
  declared, same as the entry below. THE LATCH IS A PIN, AND THE PROPERTY IT PINS IS NOT THE ONE
  THE LATCH CLAIMED.** **NO CLAUSE WAS ADDED OR REMOVED** — the name set is byte-identical to
  `04437fe`'s, 225 both sides, set-difference empty in both directions. One existing clause,
  `EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion`, got teeth. The live suite figure is in
  `global.md` and is not restated here.
  - **THE OLD BLOCK CLAIMED AN UNTESTABLE PROPERTY, AND THAT IS WORTH MORE THAN THE FIX.** It said
    it pinned that the builder CALLS `StratScoreCriterionForKey` rather than keeping a private
    copy. **No black-box clause can assert that**: a duplicate switch that is CORRECT is
    indistinguishable from a call, and one that is WRONG is caught by the output. The clause now
    asserts the OUTPUT — the composed model's tag is right for every one of §2.8's keys — which is
    strictly stronger and is what actually protects the screen. *When a clause cannot be armed,
    check whether the property was even expressible before extracting a seam to reach it.*
  - **HOW THE UNREACHABLE KEYS ARE REACHED.** `StratComposeMatchResultModel` takes values, so the
    clause HAND-AUTHORS an `FStratMatchResultView` for each of keys 0, 1, 2 and 3 and drives the
    real composition. Keys 2 and 3 are states no bridge this suite can build reports. The
    expectation is still `StratBuildScoreboardModel`'s own `Rows[N - 1].Criterion` on the seeded
    bridge, never a typed table. Key 0 is asserted against a DEFAULT-CONSTRUCTED
    `FStratMatchResultModel`'s tag rather than a named enumerator, so "left at the default" is
    read off the struct instead of restated.
  - **THE BRIDGE-TAKING BUILDER IS PINNED TO THE COMPOSER BY IDENTITY — OVER EVERY *FIELD*, FOR
    THE ONE *STATE* A BRIDGE CAN PRODUCE.** [The qualifier was added 2026-08-25 on
    `strat-integration-reviewer`'s ruling 4; this bullet first said flatly "so the composer cannot
    be right and BYPASSED", which is over-broad by exactly that qualifier.] Via
    `UScriptStruct::CompareScriptStruct` over `FStratMatchResultModel` — UE's own reflection walk,
    on the `ResultScreenRowsAreTheScoreboardModelByIdentity` precedent — so a field added to that
    struct tomorrow is covered on compile. But both models are built from the seeded opening, where
    `DecidedByKey == 0`, so a bypassing builder that agreed there and diverged at key 2 would pass.
    **That residue is irreducible by black-box means** — driving the BUILDER at key 2 needs the
    bridge to report key 2 — and it is the same inexpressibility the bullet above describes.
  - **AND THE COMPARISON IS SHOWN ABLE TO FAIL, WHICH IT WAS NOT WHEN FIRST WRITTEN.** The gate's
    headline observation: the first mutation control planted the duplicate switch INSIDE the
    composer, which makes both sides of the identity wrong *identically*, so that block stayed
    green and nothing had ever demonstrated the walk could report `false`. Two things fixed it, on
    `Source/StratPlay/Tests/StratSelectionMachineParity.cpp`'s precedent — a `TestFalse` over a
    copy perturbed in the criterion tag, **and** the MIRROR mutation the gate asked for: a private
    duplicate planted in `StratBuildMatchResultModel` instead, which reddened the identity
    assertion alone and left the key loop green. 25 succeeded / 1 failed, the mirror image of the
    first run.
  - **KEY 1'S CRITERION COMPARISON ALONE IS THE WEAK ARM AND IS LABELLED AS SUCH IN THE CLAUSE.**
    `Rows[0].Criterion` is `CombatFame`, which is also the enum's zero and the model's default, so
    a composer writing NO criterion passes that one line; the `bHasDecidedBy` assertion in the same
    iteration is what catches it. Keys 2 and 3 have no such coincidence.
  - **FALSIFIABILITY MEASURED, AND THE MUTANT WAS THE DEFECT THE OLD LATCH NAMED.** A PRIVATE
    DUPLICATE SWITCH was planted inside the composer, swapped on keys 2 and 3, **with the seam left
    correct**. `Automation RunTests Stratocracy.StratUI.T-UI-03`: **25 succeeded, 1 failed**, both
    errors from the composed-model block naming keys 2 and 3. **The seam's own key loop stayed
    GREEN** — correctly, the seam being untouched — which is what shows the two blocks grade
    different things rather than one grading twice. Reverted, blob `e5d6e3a` before and after.

- **2026-08-25, WRITTEN BY THE `coordinator`, NOT BY `strat-test-author`, AND THE DEPARTURE IS
  DECLARED RATHER THAN QUIET.** No subagent ran this pass; the user asked for the fix directly and
  the two halves of it straddle the test lane and the engine lane, so one writer made both edits
  and both record entries. **§2.8's TIEBREAK KEYS 2 AND 3 ARE PINNED. The hole this file recorded
  in the WAVE 6 entry below is DISCHARGED**, and that bullet is stamped where it stands.
  - **ONE NEW CLAUSE**, `Stratocracy.StratUI.T-UI-03.EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion`,
    appended to the existing `Source/StratUI/Tests/StratMatchResultModelClauses.cpp`. **Clause
    delta +1**, by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` --
    224 names at `a808e0b`, 225 in this tree, exactly one added and none removed or renamed. Never
    by an acceptance-ID grep, which has undercounted 8 as 5 in this repo. **No id minted**; it
    joins T-UI-03 beside the clause it completes. The live suite figure is in `global.md` and is
    not restated here.
  - **THE MODULE-SIDE SEAM THIS FILE ASKED FOR NOW EXISTS**: `STRATUI_API bool
    StratScoreCriterionForKey(int32 Key, EStratScoreCriterion& OutCriterion)`, the switch that was
    inline in `StratBuildMatchResultModel`, moved out unchanged. See `engine.md` for that half.
    The clause was NOT WRITABLE BEFORE IT -- keys 2 and 3 need a capped match in which both sides
    fought to an EQUAL combat Fame, and no bridge this suite can build produces one. The
    unreachability is unchanged; what changed is that the mapping is now callable.
  - **WHERE THE EXPECTATION COMES FROM, WHICH IS STILL THE COLUMN THAT MATTERS.**
    `StratBuildScoreboardModel`'s OWN rows on the seeded bridge: §2.11.4 orders them in §2.8's
    tiebreak order and §2.8 numbers its keys from one, so key N must name `Rows[N - 1].Criterion`.
    The clause supplies only "§2.8 counts from one". A typed table of three pairs would have been
    this file transcribing the switch it grades, and would have agreed with a swapped mapping the
    moment somebody swapped both copies.
  - **THE NON-KEYS ARE PINNED IN BOTH DIRECTIONS.** `false` alone is half the contract: the
    function must also leave `OutCriterion` UNTOUCHED, which is what lets key 0 -- the common
    case, a flag kill having evaluated no key -- keep the caller's default instead of naming a
    row. Every call pre-loads the out-parameter with a value the true answer is not, over all
    three criteria, so the write and the non-write are both measured. Non-keys walked: 0, -1, 4,
    `INT32_MAX`, `INT32_MIN`. `INDEX_NONE` is not listed separately because it IS -1.
  - **THE BUILDER IS *NOT* PINNED AS THE SEAM'S CALLER, AND THE FIRST VERSION OF THIS ENTRY SAID
    IT WAS.** [Corrected 2026-08-25, same pass, on `strat-integration-reviewer`'s observation 1 --
    the gate returned `VERDICT: PASS` with zero findings and five observations, of which this was
    the first, and it was right.] The clause's third block builds the model over the seeded opening, where
    `DecidedByKey == 0`; the seam then returns false and leaves the comparison value as seeded, so
    both assertions reduce to `x == x` and `false == false`. **A builder keeping a private second
    switch would pass them.** The block is KEPT as a latch -- it arms itself with no edit the
    moment a non-zero key becomes reachable -- and its comment now says so, with an `AddInfo` that
    prints the inertness into every report. **This is the same unreached-arm-reads-as-covered
    shape the clause exists to close, reappearing one layer up in my own prose**, which is the
    part worth carrying forward.
  - **AND "UNREACHABLE" WOULD HAVE BEEN THE NEXT DEFECT, CAUGHT ON THE RE-GATE.** The
    `coordinator` told the reviewer it "could find no way" to make that block bite. That is an
    unmeasured limitation asserted as a rule, and it is wrong: `FStratMatchResultView` is a plain
    `USTRUCT` with a public defaulted `int32 DecidedByKey`, so a clause can hand-author key 2 with
    NO bridge. The only obstacle is that `StratBuildMatchResultModel` reaches for its result
    through the bridge; extracting the key-to-tag step to take a `const FStratMatchResultView&`
    makes the caller property fully falsifiable. **Not a wall -- a cost that was weighed and
    declined this pass**, and filed in `global.md`'s backlog rather than forgotten.
  - **FALSIFIABILITY WAS MEASURED, NOT INFERRED, AND THE CONTROL IS THE FINDING.** Keys 2 and 3
    were SWAPPED in the shipped function, the target rebuilt (`Result: Succeeded`), and
    `Automation RunTests Stratocracy.StratUI.T-UI-03` run: **25 succeeded, 1 failed** -- the new
    clause the only red one, naming both swapped keys with the criterion each should have been.
    **`DecidedByKeyReachesTheScreenAsACriterionAndNotAsArithmetic` STAYED GREEN over that mutant**,
    which is the hole demonstrated rather than argued. Reverted, blob re-measured identical by
    `git hash-object` (`68a842d` before and after), rebuilt, full suite re-run.
  - **THE TEMPORARY EDIT WAS TO A NON-`Tests/` FILE AND THAT IS ONLY ADMISSIBLE BECAUSE NO LANE
    AGENT MADE IT.** The WAVE 6 entry below records `strat-test-author` REFUSING exactly this
    mutation and being right to; that rule is unchanged and this is not a precedent for it. A
    lane agent asked to do this must still refuse and notify the coordinator.
  - **THE END-TO-END CLAUSE IS NOT MADE REDUNDANT.**
    `DecidedByKeyReachesTheScreenAsACriterionAndNotAsArithmetic` is still the only place the key
    travels from the rules module, through `StratBuildMatchResultModel`, onto the model a widget
    reads. The two clauses are the reachable half and the callable half of one property.

- **WAVE 6: §2.11.4's END-OF-MATCH SCREEN. SIX CLAUSES, AND ONE OF THE TWO ACCEPTANCE IDs THE
  DISPATCH PROPOSED DID NOT SURVIVE READING THE DOCUMENT.** Three new clause-bearing files:
  `Source/StratUI/Tests/StratMatchResultModelClauses.cpp` (five clauses, `T-UI-03`) and
  `Source/StratPlay/Tests/StratMatchResultTrigger.cpp` (one clause, `T-INT-05`), plus the UMG-free
  seam pair `Source/StratUI/Tests/StratMatchResultHostDouble.h` / `StratMatchResultHostProbe.h` /
  `.cpp`, which declare no clause. **Clause delta +6**, by set-difference on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` — never by an acceptance-ID grep, which
  has undercounted 8 as 5 in this repo. **No id minted.** Unstaged; staging is the user's call.
  The live suite figure is in `Tools/architect/state/global.md` and is not restated here.
  - **`T-UI-03` HELD; `T-TURN-04` DID NOT, AND THE MISS WAS IN THE DOCUMENT RATHER THAN IN THE
    CODE.** The engineer proposed `T-TURN-04` for the show-once / cleared-on-restart clause.
    `T-TURN-04` is the GDD Invariants row "at the turn cap, the attrition tiebreak resolves in the
    exact §2.8 order" — a rules-module property, already pinned in
    `Source/StratBridge/Tests/StratMatchResultClauses.cpp`, saying nothing about a screen's
    lifetime. Filing there would have made a grep for that row's coverage return a widget test.
    It went to `T-INT-05` instead, beside `StratMatchConclusion.cpp`'s five clauses on the same
    transition. `T-UI-03` was kept for the five model clauses on the precedent
    `StratMatchResultRouting.cpp` already set for §2.11.5 — **not** on an assumption that §2.11.4
    has an id of its own. It has none, and the whole 85-id set was walked to check.
  - **WHAT EACH CLAUSE PINS. THE COLUMN THAT MATTERS IS WHERE THE EXPECTATION CAME FROM.**
    - `ResultScreenRowsAreTheScoreboardModelByIdentity` — IDENTITY via
      `UScriptStruct::CompareScriptStruct` over `FStratScoreboardModel`'s reflection walk, **not**
      a hand-listed field comparison. That choice is the clause: a field added to that struct
      tomorrow is covered the moment it compiles, where a hand list would go green on a builder
      that composed the new field itself. The walk covered **8** reflected properties at this
      pass. Both viewing sides, plus a negative control that the two sides' models DIFFER —
      without it the identity holds equally over a builder that ignored `ViewingSide`.
    - `ResultLinesAreTheGddSamplesVerbatimAndInsideTheVoiceBudget` — the six strings are **parsed
      out of `Tools/architect/gdd_snapshot/Stratocracy_Prototype_GDD.md` at run time**, keyed by
      §2.11.4's own bullet labels, and the ≤ 30-word budget is parsed out of the same paragraph.
      A missing label FAILS rather than yielding a shorter list. The snapshot was measured
      byte-identical (`git hash-object`, blob `de88390`) to the live document in
      `stratocracy-content` at this pass.
    - `ResultVoiceFollowsTheWinnerAndNotTheViewer` — needs a real winner and PLAYS for one:
      §2.9's AI drives both sides on the bridge until §2.8 answers. Concluded at **156 commands,
      Decisive / FlagDestroyed / winner 1** at this pass; deterministic by T-TURN-09.
    - `ResultLineMappingIsTotalOverEveryTierCauseAndWinner` — **72** triples off `StaticEnum`'s
      own walks, **42** of them with a line, and every non-empty answer is required to be one of
      the GDD's six. Without that closure the totality clause is satisfied by returning "x".
    - `DecidedByKeyReachesTheScreenAsACriterionAndNotAsArithmetic` — see the hole below, now
      DISCHARGED, and its companion clause in this file's topmost entry.
    - `MatchResultScreenIsAskedForOncePerMatchAndClearedOnRestart` — ONE clause on purpose. The
      subject is the COMBINATION; either half alone passes against a broken implementation, and a
      trigger unobservable except by combination is the §2.11.6-B beat-2 defect class.
  - **WHAT THE KEY CLAUSE DOES NOT PIN, AND IT WAS A REAL HOLE. [DISCHARGED 2026-08-25 — see the
    entry at the top of this file. The bullet is kept unshortened because it is what named the
    seam, and the closing sentence below was true only until that seam landed.]** §2.8's keys
    **2 and 3** are unreachable from any bridge this suite can build — they need a capped match in
    which both sides fought to an EQUAL combat Fame. Every game §2.9's AI plays on the shipped
    scenario ends `Decisive / FlagDestroyed` with `decidedByKey == 0`, so **this** clause measures
    the **key-0 arm only** and states the shift as a relation (`Rows[DecidedByKey - 1].Criterion`)
    that fires only if a key is ever non-zero. **Discharged by a module-side seam this lane may
    not add:** a `STRATUI_API` free function exposing the key→criterion switch that then lived
    inline in `StratBuildMatchResultModel`. That function is `StratScoreCriterionForKey`, it
    exists, and `EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion` calls it — so the sentence
    *"until then a wrong mapping for keys 2 and 3 ships green"* NO LONGER HOLDS. It did hold for
    every pass before this one.
  - **THE MUTATION WITNESS IS IN THE CLAUSE, NOT IN A REPORT, AND THE LANE RULE IS WHY.** The
    dispatch asked for a two-directional differential over a temporary edit to
    `Source/StratUI/StratMatchResultWidget.cpp`. That file is outside this lane and the rule
    admits no temporary edit, so the falsifiability proof was built **inside** the clause:
    `StratResultLineFor(Tier, Cause, ViewingSide)` and `SideFaction(ViewingSide)` ARE the mutant's
    two outputs — the substitution is one argument and the builder does nothing else to those
    values — and the clause asserts they DIFFER from the shipped answer on this board. Measured at
    this pass: the mutant would show seat 0 the Directorate's decisive line while the shipped code
    shows the Vanguard's from both seats. **This is stronger than the temporary edit would have
    been**: it is permanent, and it is the one line that would speak up if the two spellings ever
    stopped being distinguishable — the only way those three assertions could go vacuous while
    staying green.
  - **INSTRUMENT CAVEATS, BOTH LOAD-BEARING.**
    - `FTriggerLogCapture` filters on `STRAT-` **or** `No end-of-match screen this match`.
      `ConcludeMatchIfEnded`'s screen refusal carries **no `STRAT-` prefix** — deliberately, that
      prefix being reserved for fixed-field gate lines — so a capture copied unchanged from
      `StratMatchConclusion.cpp` would have been silent about its own subject.
    - The refresh loop **does not assert on `RefreshPresentation`'s return value.** With no
      `ScoreboardWidgetClass` that function returns FALSE on every call even though `ApplyView`
      ran every time, because the scoreboard step it refuses at runs AFTER `ApplyView`. Asserting
      on the bool would have made five real refreshes look like five refusals, and "still exactly
      one" would then have been satisfied by refreshes that never happened. The clause reads the
      refusal REASON for the word `scoreboard` instead, which is what places the refusal
      downstream of the trigger.
  - **[DISCHARGED 2026-08-26 — THE VENDORING THIS BULLET NAMED AS PENDING HAS HAPPENED. EVERY
    CLAUSE OF IT IS NOW FALSE: THE WORDS ARE NOT A LITERAL, THE FILE IS IN THIS REPOSITORY, AND
    THE WORK IT ASSIGNS IS DONE. Kept rather than deleted, on this record's standing practice
    that a superseded claim survives as a quotation under a stamp naming what discharged it.
    It read:]**
    *"THE NINE BANNED WORDS ARE THE ONE STATED LITERAL IN THIS SUITE'S §2.11.4 COVERAGE, cited
    to `kb/setting.md`'s 'Banned register' bullet. `kb/setting.md` is not in this repository —
    it lives only in `E:\MultiAgent\stratocracy-content\kb\setting.md` — and a headless run may not
    reach outside the tree. Discharged by vendoring it beside the GDD snapshot, which is
    `strat-data-steward`'s call and not this lane's."*
    NOTE WHICH DIRECTION THIS ONE WENT STALE IN. It does not over-claim; it **under-claims**, and
    an under-claiming status line is the more expensive kind, because a reader who believed it
    would go and re-do a vendoring that already exists and re-open a lane question already
    settled. That is why it is stamped and not simply left standing.
    WHAT ACTUALLY HAPPENED. `strat-data-steward` vendored the setting document to
    `Tools/architect/kb_snapshot/setting.md`, and
    `Stratocracy.StratUI.T-UI-03.ResultLinesAreTheGddSamplesVerbatimAndInsideTheVoiceBudget` in
    `Source/StratUI/Tests/StratMatchResultModelClauses.cpp` now **PARSES** the nine words out of
    that file's own `- **Banned register:**` bullet at run time. Nothing about the register is
    typed in the suite any more, which closes the failure this bullet was filed against: a guard
    that types its own reference data goes inert on its own subject silently, and that reference
    data can be WRONG rather than merely stale.
    THE BULLET WRAPS, AND THAT IS THE PART A LATER READER WILL TRIP ON. The document writes the
    list across **two lines** — `*destiny, glory,` ends line 23 and `honor, … doom*` is line 24 —
    so a line-at-a-time reader sees two of the nine and reports success over the other seven.
    `ReadBannedRegister` joins the continuation lines before cutting the italic span out, and its
    anchors are deliberately **ASCII** (`words like *`), beginning AFTER the bullet's em dash, so
    no assumption about how UE decodes a non-ASCII byte can quietly change what the clause pins.
    WHAT THIS CLAUSE DOES **NOT** PIN. `Tools/architect/kb_snapshot/` is a **SNAPSHOT AND NOT A
    SYNC** — its `MANIFEST.md` records that nothing hash-gates it against `stratocracy-content`,
    that no `sync_*.py` stands behind it and that no acceptance ID asserts over it, exactly as
    `gdd_snapshot/` is ungated and unlike `Source/StratRules/` and `Data/`. So the clause pins
    the six result lines against the banned register **AS VENDORED HERE**. It does **not**
    certify that the vendored copy still matches the document upstream; if `kb/setting.md` moves
    on, this copy silently does not, and no clause in this suite will say so.
    THE FALSIFIABILITY WAS MEASURED, NOT INFERRED, AND THE INSTRUCTION IT WAS RUN UNDER WAS
    WRONG. The dispatch asked for a perturbation changing one banned word to something **absent
    from the six result lines**. That cannot move the verdict **by construction**: the assertion
    reads "no result line contains this word", and a word absent from all six satisfies it
    exactly as `doom` did. Run anyway (`doom` → `quixotic`), the clause stayed green while the
    info line reported `… sacred, quixotic` — so it is a **liveness control**, evidence the read
    reaches the file, and it is **not** a falsifiability proof. Writing that distinction down is
    the point: a probe the guard cannot fail passes exactly like an inert guard. The proof is the
    second run, `doom` → `record` (a word present in four of the six lines), which flipped the
    clause **Success → Fail** with four named errors. `setting.md` was restored byte-identical
    after every perturbation — `git hash-object` `d8d2d0590f30f5d9153c8a12f77ed4763cae6c03`
    before and after each, not "looks the same".
    AND THE REFUSAL PATH FIRES, IN THREE SHAPES, EACH MEASURED RED. An unreadable input is not a
    clean input, and a parser that yielded an empty set would make the whole assertion vacuous —
    it would then pass over any result line at all. (i) The file moved aside: *"the vendored
    kb/setting.md snapshot is unreadable at …"*. (ii) The italic span left unclosed (`doom*.`
    → `doom.`): *"… italic span is not closed, so the list cannot be read whole"*, and that
    error quotes the **joined** bullet, which is independent evidence the wrap handling ran.
    (iii) A word dropped from the list: *"Expected … still lists nine words to be 9, but it was
    8."* The count is asserted **before** the words are used, precisely so a short parse reddens
    here rather than silently narrowing what the six lines are checked against.
    WHY THE NUMBER NINE IS STILL TYPED, AND WHY THAT IS NOT THE OLD DEFECT. The nine *words* are
    read; the *count* is a tripwire on the parse, not the reference data. A register the document
    later grows will redden this clause deliberately and send a human back to `kb/setting.md`,
    which is the intended behaviour and not a regression to a typed list.
    No suite figure is restated here; the live one lives in `Tools/architect/state/global.md`.
    The pass that carried this change is the one whose automation report is stamped
    `2026.08.26-15.21.53`, and the clause-name set was verified identical before and after it —
    none added, none removed, none renamed — by set-difference over
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, never an acceptance-ID grep.
  - **[RETRACTED 2026-08-26 — THE MANIFEST WAS NEVER OUT OF DATE; THIS LANE'S READING OF THE FILE
    SIZE WAS THE THING THAT WAS WRONG. Kept rather than deleted, on this record's standing
    practice that a superseded claim survives as a quotation under a stamp naming what
    discharged it. It read:]**
    *"`Tools/architect/gdd_snapshot/MANIFEST.md` IS STALE, AND IT IS NOT MINE TO FIX. It records
    `Bytes 446,133` and a sha256 taken 2026-08-11; the tracked file is 449,498 bytes and hashes
    identical to the live document. The manifest's own text says nothing gates it. Flagged for
    `strat-data-steward` because two clauses now read that file."*
    BOTH HALVES ARE WITHDRAWN, AND WHAT REPLACES THEM IS THE MEASUREMENT RATHER THAN THE
    CONCLUSION. Re-derived in `E:\MultiAgent\Stratocracy` on 2026-08-26, not taken from a brief:
    the vendored `Stratocracy_Prototype_GDD.md` is **446,133 bytes** by `wc -c`, by `len()` over
    the raw bytes in python, and by `git cat-file -s` on its `HEAD` blob — three readings that
    share no code path. Its sha256 is
    `bfa4f713ab1415cff4e0e0ef09d94f751ca727f7893bc0d44900d81248ed954c`, which is EXACTLY the hash
    `MANIFEST.md` records, and the live document at
    `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md` hashes to that same value,
    so the two are byte identical. The file carries **3,365 lines and ZERO carriage returns**,
    and 446,133 + 3,365 = 449,498 — one added byte per line, which is precisely what a
    **CRLF-applying reader** reports for a file of this shape. 449,498 was never the size of
    anything on disk.
    THE CONTROL, and it was run, because a count of zero proves nothing until the instrument is
    shown able to speak: `printf 'a\r\nb\r\n' | tr -cd '\r' | wc -c` returns **2**, so the same
    reading CAN see a carriage return; its silence over the snapshot is a reading, not a failure.
    THE SECOND HALF GOES WITH THE FIRST: nothing is owed to `strat-data-steward` here, and a
    reader who had acted on "not mine to fix" would have edited a correct manifest to match a
    figure no file ever had. That is why this is stamped rather than left standing — a status
    line that under-claims sends the next reader to redo settled work. `MANIFEST.md` now carries
    this trap and the measuring command in its own text, so the next byte check has its control
    inside the file it is checking. No suite figure is restated here; the live one lives in
    `Tools/architect/state/global.md`.

- **WAVE 5: §2.11.5's BOXED-IN FOOTER FACT NOW HAS A GATE, AND FOUR OF THE FIVE CLAUSES ARE
  ABOUT THE CLOCK IT RIDES RATHER THAN THE VALUE IT CARRIES.** Five clauses, no new files: four
  appended to `Source/StratPlay/Tests/StratProductionMenuSeam.cpp` under `GATE-BUILDMENU`, one
  appended to `Source/StratBridge/Tests/StratSpawnBlockedClauses.cpp` under `T-UI-04`. Both ids
  pre-existing; **none minted**. **Clause delta +5**, by set-difference on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` (never by an acceptance-ID grep,
  which has undercounted 8 as 5 in this repo): exactly five names added, none renamed, none
  removed. Both files LF before and after, `git diff --numstat` **+958 / -0** and **+233 / -0**,
  additions only. Unstaged; staging is the user's call. The live suite figure is in
  `Tools/architect/state/global.md` and is not restated here.
  - **THE SUBJECT.** `UStratMatchSubsystem::IsOpenMenuFactorySpawnBlocked()`, the private
    `bProductionMenuSpawnBlocked` that `RefreshProductionMenu` publishes beside the rows and
    `CloseProductionMenu` clears beside them, and `FStratBridge::FactorySpawnBlockedAt`
    underneath both.
  - **BOTH ACCEPTANCE IDs WERE MOVED OFF THE ONES PROPOSED, AND BOTH MOVES ARE THE SAME
    ARGUMENT: DO NOT SPLIT ONE SUBJECT ACROSS TWO ROWS.** The dispatch proposed `T-UI-03` for
    all five.
    - **THE FOUR SEAM CLAUSES ARE `GATE-BUILDMENU`'s.** They are about a member written in the
      SAME STATEMENT GROUP as `ProductionMenu` and `ProductionMenuHex`, and those two are that
      file's subject under `GATE-BUILDMENU` in ten existing clauses. The file's own header
      block already argues the rest and needed no help: `T-UI-04` is an IN-EDITOR claim about a
      widget BINDING, nothing in the file constructs a widget or touches Slate, and
      `GATE-BUILDMENU` is this project's id for §2.11.5 TRANSPORT. **No clause in that file may
      be read as moving `T-UI-04`'s ledger, and these four do not.**
    - **THE BRIDGE CLAUSE IS `T-UI-04`'s**, beside
      `T-UI-04.ABoxedInFactoryReportsSpawnBlockedAndStillAcceptsTheBuild` at the top of the same
      file. Filing the REFUSAL arm of one method under a second id splits one question in half.
  - **WHAT EACH PINS, AND THEY ARE NOT FIVE READINGS OF ONE BOOL.**
    - `GATE-BUILDMENU.OpenMenuSpawnBlockedMatchesTheBridge` — **TWO FACTORIES, TWO ANSWERS,
      ONE FIXTURE.** A menu at side 0's boxed-in factory must read TRUE and a menu at a factory
      side 0 does NOT hold must read FALSE, each against `FactorySpawnBlockedAt` asked freshly
      for `ProductionMenuHex` in the same frame. **The expectation is handed the PUBLISHED hex,
      not the hex the clause asked for**, so a refresh pairing one factory's rows with another
      factory's footer reddens here instead of surviving as two separately-correct members. The
      third assertion — that the two menus reported DIFFERENT facts — is what no constant
      accessor can satisfy.
    - `GATE-BUILDMENU.SpawnBlockedFalseWhileNoMenuIsOpen` — three observations over **two
      routes to "closed"**: never-refreshed, `CloseProductionMenu`, and a reseed (which runs
      `TearDownPresentation`). Every false is preceded by a TRUE on the same subsystem, asserted
      fatally, so none of them is a mute instrument reading zero.
    - `GATE-BUILDMENU.SpawnBlockedFalseOnANonBuildPointMenu` — **A REFUSAL IS NOT A YES.** The
      menu is opened at the boxed-in factory FIRST so the published bool is true, and only then
      moved to a hex no factory claims; a refresh that published the footer only on the query's
      success branch leaves the previous TRUE standing and reddens. The refusal itself is
      asserted, not assumed.
    - `GATE-BUILDMENU.SpawnBlockedRidesTheRowsClockAcrossABuild` — **THE ONE THAT CATCHES AN
      `AppliedModel` RE-SOURCE, and it can only catch it because of an ORDERING.**
      `SubmitProductionChoice` calls `RefreshProductionMenu` BEFORE `RefreshPresentation`, so at
      the instant the footer is published the applied model still describes the PRE-BUILD board
      — where this factory is not boxed in. That pre-build value is captured, printed, and
      quoted in the terminal assertion's own message, so the failure names the defect rather
      than reporting a bool. If that ordering is ever reversed, **this clause stops being able
      to see the defect while staying green**, and that is the sentence to re-read before
      touching `SubmitProductionChoice`.
    - `T-UI-04.FactorySpawnBlockedRefusesANonFactoryHex` — **TWO SUBJECTS, AND ONLY THE SECOND
      DISCRIMINATES.** The obvious wrong implementation is a fall-through to
      `strat::spawnHexesBlocked`, which answers about ANY hex and is occupancy-only. On an
      ordinary empty hex it reports `false`, **which is also what a correct refusal leaves in
      the out-parameter** — so a clause asking only about an empty corner passes on the defect
      and on the shipped code alike. The second subject is a NON-FACTORY hex on which
      `spawnHexesBlocked` reads TRUE; there the two diverge in both channels. **That hex is
      FOUND, not written down** — every hex of the projection is asked until one answers true,
      and the run reports it as `axial(-2,4)=offset[0,4]`. If the arrangement ever stops
      producing one, the clause fails at that premise instead of degrading into the version that
      could not fail. A positive control on the real factory is in the same clause, because
      "refuses a non-factory hex" is satisfied perfectly by a method that refuses everything.
  - **THE HALF THAT COULD NOT BE SEPARATED, AND IT IS A FINDING RATHER THAN AN OMISSION.** The
    closed-panel answer is false TWICE OVER — `CloseProductionMenu` resets the bool and
    `IsOpenMenuFactorySpawnBlocked` ANDs `IsProductionMenuOpen()` on top. The dispatch asked for
    arrangements telling which half is load-bearing. **MEASURED: no such arrangement exists from
    `Tests/`.** Both routes to "closed" run the reset (`StartMatch` reaches it through
    `TearDownPresentation`), so **there is no reachable state in which the bool is stale-true
    with the rows empty** — deleting either half reddens nothing. The clause therefore pins the
    GUARANTEE and says in its own prose that it does not pin which half delivers it, so no
    reader can over-read it. Separating them needs a seam this lane may not add; see BLOCKED
    below.
  - **WHERE EVERY EXPECTATION CAME FROM.** All five read the module on both sides. The subject
    is always the accessor (or the bridge method); the expectation is always
    `FStratBridge::FactorySpawnBlockedAt` on the SAME bridge in the SAME frame, or
    `strat::spawnHexesBlocked` over `FStratBridge::MakeUiWorld()`. **No bool is written down,
    no hex is named and no neighbour is walked in any of the five.** Which hexes are factories,
    which side holds them and which hex is not a factory are read off `FStratViewModel` and
    `strat::UiSnapshot`.
  - **THE SHIPPED BOARD MAKES EVERY ONE OF THESE VACUOUS, WHICH IS WHY FOUR OF THEM COST A
    SCENARIO VARIANT.** `Data/ferrum_crossing.json` leaves side 0's factory hex FREE, so
    `spawnBlocked` is false there and a clause written against the shipped board measures
    `false == false`. Each clause writes a variant under `Saved/StratTests/` — never under
    `Data/`, which is vendored and hash-gated — and deletes it on the way out. Two boards:
    **boxed in** (three placements moved: side 0's Infantry onto its own factory hex, side 1's
    Artillery and Recon onto the two remaining free neighbours) and **nearly boxed in** (the
    same minus the Artillery, leaving exactly ONE free hex for the build to take). The shipped
    scenario already surrounds that factory with five of side 0's own units, which is the only
    reason a two-line and a three-line edit are enough.
    - **THE PLACEMENT STRINGS ARE NOW IN TWO FILES IN TWO MODULES.** They were
      `StratSpawnBlockedClauses.cpp`'s; the StratPlay file restates them rather than sharing a
      header, because a shared header between two test files in two modules is a third thing to
      keep true. **Both copies carry the T-SCN-07 and T-SCN-11 reasons for why those particular
      units move** — a scenario variant is validated as hard as the shipped file, and the first
      arrangement that used a side-1 INFANTRY was refused outright.
  - **FALSIFIABILITY: TWO ROUNDS, SIX MUTATIONS, ALL INSIDE `Tests/`.** Production source is not
    this lane's to touch even temporarily; each broken design was simulated by substituting the
    instrument, the technique `StratMatchConclusion.cpp` and `StratGuidedOpeningVisuals.cpp`
    both record.
    - **ROUND 1** — the accessor reports openness alone (clause 1); a STICKY footer with neither
      the reset nor the conjunct (clauses 2 and 3); the footer taken from the pre-build applied
      model (clause 5); `FactorySpawnBlockedAt` replaced by a fall-through to
      `strat::spawnHexesBlocked` (clause 4). Result: **exactly the five new clauses reddened and
      no others** (`reportCreatedOn 2026.08.25-18.56.51`), **each at its terminal assertion and
      none at a guard** — the distinction that made an earlier attempt on another file
      worthless. Messages included *"the footer for the unheld factory equals the bridge's own
      answer (accessor 1, bridge 0)"*, *"and the footer fact goes with it"*, *"a non-factory hex
      whose neighbourhood is FULL, axial(-2,4)=offset[0,4], is still REFUSED"*, and — the one
      that shows the mutation reached the defect this pass exists for — *"the footer fact moved
      false -> true inside the SAME SubmitProductionChoice … would have published the pre-build
      value (0) here"*.
    - **ROUND 2** — clause 1's other direction only, the accessor a constant `false`.
      **Exactly clause 1 reddened** (`2026.08.25-18.58.54`), on the assertions round 1 left
      green: *"(accessor 0, bridge 1)"* and *"on this board that shared answer is TRUE"*. **The
      two rounds redden disjoint assertion sets inside that clause**, sharing only the
      "two menus reported DIFFERENT facts" line — which is what says it tells the two failure
      modes apart rather than merely noticing something is wrong.
    - Shipped bytes restored from copies held OUTSIDE the repo and proved identical by
      `git hash-object` — `228afdc03f7a0002fd1d39f1c142090222a23b54` (seam) and
      `0f75bbac9382c9b2bd544f64dd5f7813f858b109` (spawn-blocked), before and after both rounds —
      never `git checkout --`, which rewrites LF to CRLF under `core.autocrlf=true`.
  - **WHAT THESE FIVE DO NOT PIN, and it is a real list rather than a hedge.**
    - **THAT ANYTHING IS ON SCREEN.** `WBP_ProductionMenu` binds `IsOpenMenuFactorySpawnBlocked`
      and nothing here constructs a widget or touches Slate. The footer's actual appearance is
      the content lane's and has **no gate under it from here**.
    - **WHICH HEX THE BUILT UNIT LANDED ON**, in clause 5. That needs an adjacency answer,
      `strat::neighbors` is `LNK2019` in `StratPlay`, and
      `T-UI-05.ADisplacedSpawnLandsOnANeighbourAndSpawnBlockedStaysFalse` owns it.
    - **THE DISPLACEMENT CASE, STILL.** The entry above records it and it is unchanged by this
      pass: `spawnHexesBlocked` is occupancy-only and is FALSE under displacement, so a footer
      bound to this accessor still says nothing about the case that produced the original
      complaint. **These five clauses gate the footer; they do not widen it.**
    - **WHICH HALF KEEPS THE CLOSED-PANEL ANSWER FALSE**, above.
    - **THAT `SubmitProductionChoice` KEEPS ITS ORDERING.** Clause 5's teeth depend on
      `RefreshProductionMenu` running before `RefreshPresentation`, and **nothing asserts that
      ordering directly** — reverse it and the clause goes quiet rather than red.
  - **BLOCKED, AND NOT WORKED AROUND.** Making the reset in `CloseProductionMenu` independently
    observable would need either a non-reflected read of `bProductionMenuSpawnBlocked` on
    `UStratMatchSubsystem`, or a route that empties the rows without calling
    `CloseProductionMenu`. Both are production-source changes and this lane made neither.

- **WAVE 2's TEST HALF: WHO WON, THE SPAWN-BLOCKED ASYMMETRY, THE CAPTURE VERB, AND THE
  HAND-OVER DESYNC.** Twelve clauses across five new files, all under acceptance IDs the GDD
  already owns — **none minted**. **Clause delta +12**, by set-difference on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` (never by an acceptance-ID grep,
  which has undercounted 8 as 5 in this repo): exactly twelve names added, none renamed, none
  removed. Unstaged; staging is the user's call. The live suite figure is in
  `Tools/architect/state/global.md` and is not restated here.
  - **THE FILES.** `Source/StratBridge/Tests/StratMatchResultClauses.cpp` (T-TURN-02, -04,
    -05), `Source/StratBridge/Tests/StratSpawnBlockedClauses.cpp` (T-UI-04, T-UI-05),
    `Source/StratBridge/Tests/StratCaptureCommandClauses.cpp` (T-FAME-05),
    `Source/StratUI/Tests/StratMatchResultRouting.cpp` (two T-INT-05 — **[RENAMED
    2026-08-29 from T-UI-05 by `global.md`'s ruling; the twelve-name delta this bullet reports
    is unaffected, since a rename adds and removes nothing. Edited by the `coordinator` and not
    by this file's declared sole writer, on the user's direct instruction -- same marker, same
    instruction, same author as the one in the wave-3 clause list above]** — two T-UI-03),
    `Source/StratPlay/Tests/StratViewingSideHandoverClauses.cpp` (two T-UI-03).
  - **THE ONE CLAUSE THE WHOLE OF ITEM 1 TURNS ON, AND WHAT IT ARRANGES.**
    `T-TURN-04.CappedMatchNamesTheKeyThatDiffered` is the only clause in this suite where
    **`winner != sideToMove`**. It is arranged rather than lucky: the cap resolves at a ROUND
    BOUNDARY, so `activeSide` is whichever side CLOSED the round — the last side in the
    alternation, never `firstSide` — and the arrangement makes side 0 (`kFirstSide`) lead
    Sec 2.8's comparison. Both numbers are then read from the module on one world in one frame
    — the winner from `uiMatchResult`, the side to move from `buildUiSnapshot` — and
    asserted unequal. **Without it every who-won clause passes against a `sideToMove`
    derivation**, and the projection loss item 1 exists to close stays open under a green
    suite. A flag kill cannot supply it: there `sideToMove` IS the winner, because the killer
    was to move.
  - **TWO LEVELS, AND THE SPLIT IS A LIMIT WORTH KNOWING RATHER THAN A CONVENIENCE.**
    - `T-TURN-05.MutualPassivityIsADrawWithNoWinner` runs the **whole engine chain on a bridge
      that genuinely concluded** — a `turnCap: 1` variant of the shipped scenario written
      under `Saved/StratTests/`, two `SubmitEndTurn` calls, then `FStratBridge::MatchResult` on
      the state those commands produced. No `TurnState` is touched by the test on that path.
    - **`T-TURN-02` AND `T-TURN-04` DO NOT EXERCISE `FStratBridge::MatchResult`'s OWN BODY**,
      and that is stated in the file rather than implied. `FStratBridge` exposes no writer for
      `strat::GameState`, this lane may not add one, and neither a flag kill nor a pre-cap KILL
      is reachable in the two commands a headless clause can afford. So they take the bridge's
      live `TurnState` through `MakeUiWorld()`, COPY it, hand the copy to the module's own
      graders (`checkImmediate` / `beginTurn` / `endTurn`) with a `BoardSnapshot` built from
      the bridge's own projection, and read the result back through `strat::uiMatchResult`
      — the exact function the bridge method calls. `StratMatchResultRouting.cpp` covers
      the bridge method and the T-TURN-05 clause drives it on a concluded match, so the chain
      is covered end to end across three clauses and by no single one.
  - **WHY `T-FAME-05` AND NOT THE PROPOSED `T-SAVE-05` FOR THE CAPTURE CLAUSE.** Both IDs exist
    in the GDD; the proposed one is about a different thing. T-SAVE-05's GDD text is "no
    partial load: a log with an illegal command at index k is refused whole; the pre-load state
    survives". T-FAME-05's is the capture line — "completes after N turns of HOLDING (N = 1
    on the shipped scenario, per-scenario data); progress is tile-held and RESETS TO ZERO..."
    — and `turnsHeld` is the counter this clause is about. **Read the SITE, not the hit
    count.**
  - **AND THE CAPTURE CLAUSE COULD NOT BE WRITTEN AT THE BRIDGE, WHICH IS ITSELF THE FINDING.**
    `captureTurns` is hardcoded to **1** by the seeding path (`seedFromScenario`, and the
    reseed in `Replay.good.cpp`), and the scenario schema carries **no key** for it. At N = 1
    the FIRST tick completes the capture, flips the owner and CLEARS the progress record, so
    **`turnsHeld` never reads 2 on any board this engine can seed** — the masking the
    filing predicted, measured. So the clause is in two halves: the non-idempotence itself at
    the module, on a hand-built `strat::EconomyState` with `captureTurns` above 1 and a
    ONE-TICK CONTROL beside the two-tick state (without which "2 after two ticks" is satisfied
    by a counter that jumps to 2 on the first); and, through the shipped bridge, that **two
    `SubmitCapture` calls in one turn are both accepted and both recorded** — the Capture
    arm consults neither T-TURN-10's per-factory allowance nor T-TURN-01's act flag. Either
    half alone is quotable and misleading.
  - **THE SPAWN-BLOCKED CLAUSES LIVE IN `StratBridge/Tests` AND NOT BESIDE THE SEAM CLAUSES,
    and the reason is the expectation rather than the subject.** "The unit landed on a
    NEIGHBOUR" needs an adjacency answer. `strat::neighbors(factoryHex, bounds, out)` IS that
    answer — the same walk `spawnHexesBlocked` and `resolveBuilds` both do — and it is
    a `strat::` free function, `LNK2019` in any other module. Written in `StratPlay` the clause
    would have had to carry a hex formula, which is the one thing this suite refuses.
    - **WHAT THEY PIN THAT NOTHING PINNED BEFORE.**
      `FStratProductionMenuBuildReconcilesTheBoardTest` asserts actor-id SET EQUALITY plus one,
      **which a neighbour spawn satisfies exactly as well as a factory-hex spawn**, so
      displacement was invisible to it. It does NOT pass silently on a boxed-in build — its
      COVERAGE guard bails out — so it was left alone.
    - **THE ASYMMETRY, NOW A MEASUREMENT.** `strat::spawnHexesBlocked` is OCCUPANCY-ONLY and is
      true only when the factory hex AND every in-bounds neighbour are occupied. Under
      displacement it is **false**, and
      `T-UI-05.ADisplacedSpawnLandsOnANeighbourAndSpawnBlockedStaysFalse` asserts it stays
      false across the build. **A Sec 2.11.5 footer bound to `bSpawnBlocked` therefore says
      nothing at all about the case that produced the original complaint.** Recorded here so no
      later pass re-derives it and no later widget is built against the wrong belief.
    - **TWO SCENARIO-VALIDATOR RULES BIT THE FIXTURES AND BOTH ARE WRITTEN INTO THE FILES.**
      Handing side 0 a second FACTORY is refused outright — "side 0 owns 2 factories at
      start; exactly one home factory is required (Sec 2.7)" — so the T-TURN-05 variant
      hands over a neutral TOWN instead. And moving a side-1 **Infantry** near the middle of
      the board contested side 0's guided lane under T-SCN-11 ("(1,5) -> (5,7) is contested: 5
      against 5"), so the boxing units are side 1's **Artillery and Recon**, which Sec 2.11.6-B's
      lane rule cannot see. **A scenario variant is validated as hard as the shipped file.**
  - **THE HAND-OVER CLAUSES, AND WHY THE EXISTING T-UI-03 CLAUSE COULD NOT HAVE CAUGHT THE
    DESYNC.** `Stratocracy.StratUI.T-UI-03.SetViewingSideRefusesOutOfRange` reads the HUD's
    member alone, and **the HUD's half was never wrong** — it range-checked before
    assigning throughout the life of the bug. That clause was green the whole time.
    `T-UI-03.ARefusedViewingSideLeavesBothMembersUnchanged` reads **both**, and the second read
    is the entire difference.
    - **BOTH FAILURE MODES RETURN FALSE AND THE ONLY OBSERVABLE THAT TELLS THEM APART IS
      WHETHER THE MEMBERS MOVED.** A headless fixture can have no scoreboard WIDGET
      (`UStratScoreboardWidget` is `Abstract`), so `RefreshScoreboard` refuses and an IN-RANGE
      hand-over returns false while both members HAVE moved — the deliberate no-rollback on
      a failed rebuild. An out-of-range one returns false with neither member moved. The clause
      drives both in sequence and that ordering is load-bearing, not stylistic.
    - **THE NULL-HUD BRANCH HAD NO AUTHORITY AND NO TEST AT ALL BEFORE THIS PASS.**
      `T-UI-03.ARefusedViewingSideWithNoHudLeavesTheSubsystemUnchanged` covers the engineer's
      new discarded trial build through `StratBuildViewModel` at the candidate side, in BOTH
      directions — an in-range side is still accepted, without which "refuses" is satisfied
      by a method that refuses everything, which would be a worse regression than the one it
      closes.
  - **TWO PRE-EXISTING CLAUSES WERE ALREADY RED ON THE TREE THIS LANE RECEIVED, BOTH CAUSED BY
    THE ENGINE HALF, AND BOTH WERE CORRECTED AT THEIR PREMISE RATHER THAN WEAKENED.** Neither
    was caused by anything in this pass — the five new files are additions.
    1. `Stratocracy.StratPlay.T-UI-03.SetViewingSideMutatesNoState` (`StratMatchLifecycle.cpp`)
       asserted that a refused OUT-OF-RANGE hand-over left the subsystem holding the refused
       side, quoting the class's no-rollback sentence. **That sentence is about a failed REBUILD
       and the clause was applying it to a failed RANGE CHECK** — the same conflation the
       code made and the fix removed. The clause was pinning the defect. Its own comment had
       asked in writing for "a conversation rather than a surprise" if the decision moved; this
       is that conversation. The surviving half moved to the new hand-over clause, which spawns
       a HUD and can drive it; this one deliberately does not.
    2. `Stratocracy.StratPlay.GATE-BUILDMENU.ARefusedRefreshLeavesTheOpenMenuAndItsHexIntact`
       lost its **LEVER, not its subject**. It made `RefreshProductionMenu` refuse by putting
       an out-of-range side into a live match through `SetViewingSide`, which now refuses
       before assigning. **The replacement lever is `FStratMatchConfig::ViewingSide`, which
       `StartMatch` assigns WITHOUT a range check** — deliberately, per that class's own
       comment — so the state is still reachable from a Blueprint default. The clause now
       builds its rows on a correct match, restarts on a skewed config, and puts the builder's
       own rows back where the reseed cleared them. **Nothing in it was relaxed.**
       - **THERE IS NOW NO OTHER REFUSAL CHANNEL INTO `RefreshProductionMenu` FROM A LIVE,
         SEEDED MATCH.** `FStratBridge::BuildOptions` refuses only on definitions, seeding and
         the side; a non-factory hex is a full menu with every row unavailable, not a refusal.
         If `StartMatch` ever range-checks `Config.ViewingSide`, this clause becomes unwritable
         and that is a finding to raise rather than a clause to delete.
  - **AN INSTRUMENT TRAP, MEASURED THIS PASS AND WORTH THE NEXT AUTHOR'S TIME.** A SECOND
    `AddExpectedMessagePlain(..., Contains, Occurrences 0)` for a line the clause already
    declared **fails the clause** with "Expected suppressed log message ... did not occur":
    `Occurrences 0` means "at least one" and the first declaration consumes every match, so the
    second is left waiting for an occurrence that will never come. One declaration per line per
    clause.
  - **WHERE EVERY EXPECTATION CAME FROM, AND THE ONE PLACE IT DID NOT.** All twelve read the
    module on both sides of every comparison, except
    `T-UI-03.UnaffordableRowNamesTheExactShortfall`, which computes `CostFame - FameTotal`.
    **That is the one acceptable case and the reason is structural: no module-side value
    exists.** `strat::UiBuildOption` carries `costFame` and `affordable` and NO shortfall
    — which is precisely why `FStratBuildOptionView::Shortfall` was added, and its
    declaration already records the debt as **discharged when** upstream adds `shortfallFame`.
    Both operands are still the module's; what this lane supplies is one minus sign, and it is
    named in the file's header. Both shortfall clauses are keyed off **`bAffordable`, never off
    `Shortfall > 0`** — asking the derived field to decide which rows to check would make
    the clause agree with itself by construction.

- **THE MARKER NOW HAS A GATE ON THE DIRECTION IT LATCHED IN, AND THE EXISTING CLAUSE THAT WENT
  RED WAS REPAIRED AT ITS PREMISE RATHER THAN WEAKENED.** Three clauses appended to
  `Source/StratPlay/Tests/StratGuidedOpeningVisuals.cpp`, all under the pre-existing `T-UI-02`,
  none minted here: `GuidedMarkerClearsWhenGuidanceDeactivates`,
  `GuidedMarkerAndObjectiveRingClearInTheSameFrame`, `GuidedMarkerGoesOutAfterSkipGuidance`.
  **Clause delta +3**, by macro census set-difference over
  `^IMPLEMENT_SIMPLE_AUTOMATION_TEST` — exactly those three names added, none renamed, none
  removed. Unstaged; staging is the user's call. The live suite figure is in
  `Tools/architect/state/global.md` and is not restated here.
  - **THE HOLE, AND IT IS THE ONE THIS LANE MOST NEEDS TO GENERALISE FROM.** Every marker
    assertion in that file ran inside a frame where guidance was ACTIVE. `ApplyUnitView` set the
    marker from `View.bIsGuidedMarked && View.Side == ViewingSide`, and **both operands are
    match-constant** — `bIsGuidedMarked` is the rules module's derivation off `placement` and is
    deliberately, permanently so, since beat 1a's content is that the marked Infantry MOVES. So
    the conjunction could never go false once true, the marker latched on, and a human found it
    at the keyboard while the suite stayed green.
    - **THE PREVIOUS CLAUSE DID ASSERT BOTH DIRECTIONS AND THE WRITER DID WRITE BOTH DIRECTIONS.
      Both were true while the defect was live.** What was missing was not an `else` and not a
      negative subject: it was **a frame in which the writer's own input could be false**.
      **BOTH DIRECTIONS OVER AN OPERAND SET THAT CONTAINS NO FALSE IS ONE DIRECTION**, and that
      sentence generalises past this bug — the same shape as the "unobservable by combination"
      finding, arrived at from the other side.
  - **WHAT EACH OF THE THREE PINS, AND THEY ARE NOT THREE READINGS OF ONE THING.**
    - `GuidedMarkerClearsWhenGuidanceDeactivates` — **WHICH FIELD.** Two frames of one model
      separated by exactly one field (`Guidance.bActive`), and **in the dark frame the two OLD
      operands are asserted STILL TRUE**: unit 3 still carries `bIsGuidedMarked` and is still on
      the viewing side. That is what makes this clause red on precisely the tree that shipped
      rather than on any tree where the marker happens to be dark. Nothing else in the suite has
      that property. The lit frame is the control: a marker that never lit reads dark for free.
    - `GuidedMarkerAndObjectiveRingClearInTheSameFrame` — **TWO SURFACES OUT OF ONE `ApplyView`.**
      §2.11.6's *"the objective ring … and the turn-1a unit marker clear in the same frame as the
      strip"* was prose in three files and a gate in none. Both surfaces are lit first — the
      ring's control forced by the mute-instrument trap, `GiveTheBoardAnOverlayMesh` — then one
      call, then both read. A marker that outlives the ring **by one refresh** is exactly what the
      playtest reported, and only the conjunction catches it.
    - `GuidedMarkerGoesOutAfterSkipGuidance` — **THE PRODUCTION ROUTE, writing no model field at
      all.** `FStratGuidedOpening::SkipGuidance` claimed this mechanism in a comment for a tree in
      which nothing implemented it (that comment now carries a `[STAMPED 2026-08-24]` retraction
      by the engine lane). It also asserts `bIsGuidedMarked` **survives** the skip, so a green
      cannot mean the mark was collaterally dropped — a rules-side regression would otherwise
      wear this clause's green as cover.
  - **A NAME WAS MOVED OFF THE ONE PROPOSED.** `GuidedMarkerReturnsNothingAfterSkipGuidance` →
    `GuidedMarkerGoesOutAfterSkipGuidance`. "Returns nothing" reads as a claim about a return
    value and `SkipGuidance` returns `void`. The id did not move.
  - **THE ONE PLACE THESE COMPUTE AN INPUT, AND WHY THAT IS NOT A COMPUTED EXPECTATION.** The
    first two make the dark frame by copying the applied model and assigning
    `Guidance.bActive = false`. That is an **input**, on the standing frame three's `ViewingSide`
    edit already set. The **expectation** — no marker anywhere, the overlay empty — is read off
    the actors and off the board and is nowhere typed. **The third clause does not even do that:**
    it drives `SkipGuidance` and asserts the module dropped the bit itself, which is what stops
    all three from silently measuring a bool this lane wrote.
  - **A REGRESSION IN AN EXISTING CLAUSE, DIAGNOSED AS A MISSING PREMISE AND REPAIRED WITHOUT
    MOVING AN ASSERTION.** `T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex` failed on six
    assertions at `reportCreatedOn 2026.08.24-14.34.09` — *"exactly one marker is lit on the whole
    board, on unit 3' to be 1, but it was 0"*. **Re-run on identical bytes first and it failed
    identically**, so not the known 1-in-4 flake. Cause: that clause builds its model with
    `BuildViewModel` and **never armed a guided opening**, so `Model.Guidance` was
    default-constructed and `ApplyUnitView`'s new first operand was false in every frame. **The
    code was right and the clause's fixture was incomplete.** Repaired by arming a real
    `FStratGuidedOpening` on the subsystem's own bridge, asserting `bActive` true as an explicit
    fatal premise, and re-decorating the model rebuilt after the move (`BuildViewModel`
    default-constructs the block, so a rebuild silently drops it). `Observe` is deliberately not
    re-run on the rebuilt model — it advances beats and that clause is not about a beat machine.
    **Not one assertion was removed, relaxed or renamed; three properties (A), (B), (C) are
    untouched.**
    - **THE SHAPE WORTH KEEPING: a clause can depend on a premise it never states, and the day
      that premise stops being true by default the clause reddens on a correct tree.** This one
      depended on "guidance is running" for a year without saying so, because until 2026-08-24
      nothing read that bit. The premise is now asserted, so a `DecorateViewModel` that stopped
      publishing it reddens the clause **at the premise** instead of emptying it silently.
  - **FALSIFIABILITY: ONE MUTATION, THE PRE-FIX PREDICATE EXACTLY, ALL SIX READ SITES AT ONCE.**
    Production source is not this lane's to touch even temporarily — the brief asked for the real
    subject to be broken and **that is refused as out-of-lane**, recorded below under what is
    blocked. `MutantPreFixLitMarkerIds` answered as an actor driven by
    `bIsGuidedMarked && Side == ViewingSide` over the model last applied, which is byte-for-byte
    what `ApplyUnitView` did that morning. Result: **exactly the three new clauses reddened and no
    others** (`reportCreatedOn 2026.08.24-14.39.44`), and **each reddened at its terminal
    assertion rather than at a guard clause** — the distinction that made an earlier MF1 attempt
    on this file worthless. Messages: *"with guidance no longer running NO turn-1a marker is
    showing anywhere on the board … reads 3 here"*, *"after `Skip guidance` … (still lit: 3)"*,
    and — the one that shows the mutation reached past both controls —
    *"BOTH SURFACES CLEARED IN THE SAME FRAME: ring 1 -> 0, markers lit 1 -> 1"*, i.e. the ring's
    control read 1 and cleared correctly while the marker survived, which is the human's report
    reproduced in a clause. **The repaired existing clause stayed GREEN under the mutation**,
    because it reads `IsGuidedMarkerVisible()` directly rather than through the mutated helper —
    an unplanned control on the mutation's own blast radius. Shipped bytes restored from a copy
    held OUTSIDE the repo and proved identical by `git hash-object` —
    `b3f9eda156150861c58a63c2df6916d7b5f3a8eb`, before and after — never `git checkout --`, which
    rewrites LF to CRLF under `core.autocrlf=true`.
  - **A RECORD-VS-TREE DISAGREEMENT, CORRECTED HERE.** The entry further down this file states
    *"The new `StratGuidedOpeningVisuals.cpp` is CRLF in the worktree, matching its neighbours in
    that directory."* **It is LF** — measured `tr -cd '\r' | wc -c` → 0 over 1767 lines, and
    `git diff --numstat` reported **+690 / -0** across this pass, additions only, which is what
    says no whole-file conversion happened. **[STAMPED 2026-08-24]** The older sentence stays as
    written for its own pass; this is the correction. Note `grep` cannot see a CR on this box, so
    `tr`/`wc` is the instrument.
  - **WHAT THESE THREE DO NOT PIN.**
    - **THAT ANYTHING IS ON SCREEN.** `IsGuidedMarkerVisible` returns `GuidedMarker->IsVisible()`
      and `USceneComponent::IsVisible` consults `bHiddenInGame` and the visible flag and **not**
      the static mesh — so it answers TRUE for a marked unit whose marker has no mesh and draws
      nothing. **All three clauses are about the FLAG falling, and their prose says so in as many
      words** so a future reader cannot over-read them. `SM_GuidedMarker` and
      `MI_Marker_Guided` are the content lane's and still have no gate under them from here.
    - **THE STRIP.** `FStratGuidanceView` reaches a UMG widget `StratPlay` cannot construct
      headlessly, so "the same frame as the strip" is pinned for the two surfaces named beside
      the strip and not for the strip itself.
    - **THE OTHER TWO ROUTES OUT OF THE WINDOW** — the turn-4 close and the all-beats-retired
      branch. Both set `bActive = false` in the same statement group, so the consequence is
      covered for any route; only `SkipGuidance`'s **trigger** is pinned.
    - **THE RING IS NOT ASKED ABOUT IN THE SKIP CLAUSE.** It assigns no `OverlayMesh`, so every
      overlay accessor there is MUTE and would read 0 whatever happened. Saying so is cheaper
      than an assertion that cannot fail.

- **THE MARKER CLAUSE WAS RESHAPED TO PIN THE 2026-08-23 USER RULING, AND IT GAINED A THIRD
  FRAME THAT PINS SOMETHING NOTHING IN THE TREE COULD SEE BEFORE.** One clause updated, none
  added: `Stratocracy.StratPlay.T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex` in
  `Source/StratPlay/Tests/StratGuidedOpeningVisuals.cpp`. **Clause delta +0** — the macro census
  and the clause-name set are byte-identical to the previous pass, nothing renamed, nothing
  removed. Unstaged; staging is the user's call. The live suite figure is in
  `Tools/architect/state/global.md` and is not restated here.
  - **IT WENT RED ON PURPOSE AND THAT WAS THE SYSTEM WORKING.** The entry above records the
    finding — two units carry `bIsGuidedMarked` on the shipped scenario, one per authored
    `guidedOpening` seat, and `ApplyUnitView` passed the bit through unfiltered, so the enemy
    seat's Infantry wore the turn-1a marker on the player's screen. That entry predicted in
    writing that "a fix that filters by viewing side turns it red on purpose, and that redness
    would be a decision arriving rather than a regression". The user ruled, the engineer
    implemented `View.bIsGuidedMarked && View.Side == ViewingSide`, and the prediction held:
    **exactly one clause failed and its two failing assertions were both about unit 7 (side 1)
    and neither about unit 3 (the viewing seat)** — the predicted singleton, not a superset.
    Worth keeping because the tripwire this lane wrote in `StratAiMatchClauses.cpp` did NOT trip
    when its subject changed; this one did, and the difference is that it asserted a VALUE a
    fix would move rather than the presence of a log line.
  - **WHAT THE RESHAPED CLAUSE PINS. THREE PROPERTIES, INDEPENDENT.**
    - **(A) THE RULING.** Three populations are enumerated off the applied model by
      `FMarkerCensus` — marked-and-on-the-viewing-side, marked-and-NOT, and unmarked — and
      **each is asserted non-empty**, so no case can go vacuous in silence. The first two
      **differ in exactly one field**, and the clause asserts that too: both carry
      `bIsGuidedMarked`, and their `Side` values differ. A positive and a negative subject
      separated by one field is what makes this strictly stronger than the pass-through clause
      it replaces, which had **no marked negative subject at all**. Both frames also assert
      **exactly one marker is lit board-wide**, which is the count a removed filter reads as 2.
    - **(B) THE MARK IS NOT KEYED TO A HEX.** Kept intact and unweakened: the marked Infantry is
      moved **through the rules** (`FStratBridge::ReachableHexes` supplies the destination,
      `SubmitMoveToHex` performs it), the model is rebuilt, `bIsGuidedMarked` is asserted still
      true by the RULES MODULE on a unit that really left `guidedOpening.infantry`, and the
      marker is required to still be showing. This is still the only thing in the tree that
      catches a hex-keyed re-derivation.
    - **(C) THE VIEWING SIDE IS READ OFF THE MODEL AND NOT OFF THE SUBSYSTEM — NEW, AND THE
      HIGHEST-VALUE PART OF THIS PASS.** A third frame applies the **same model** with
      `FStratViewModel::ViewingSide` alone changed to the other seat, and requires the marker to
      **move** to that seat's Infantry while the first unit goes dark. Every unit, every hex and
      every `bIsGuidedMarked` bit is identical across frames two and three; one field moved and
      the marker moved with it. **`UStratMatchSubsystem`'s own member is untouched by that edit,
      so this is the only place in the tree where the two sources disagree** — on every other
      path they hold the same number, and an actor reading the member would keep the marker on
      the wrong unit here with a green build everywhere else. `AStratUnitActor::GuidedMarker`'s
      block forbids the member by name for T-INT-05's reason; this frame is that sentence turned
      into a measurement.
      - **`SetViewingSide` IS DELIBERATELY NOT USED.** It rebuilds and re-applies through the
        subsystem, moving the member and the model together — which would destroy precisely the
        divergence the frame exists to create. The model is edited and applied directly, which
        is fixture construction on `StratMatchReconcile.cpp`'s standing.
  - **THE ONE PLACE THIS CLAUSE COMPUTES ITS EXPECTATION, AND WHY THAT IS THE HONEST SHAPE HERE.**
    `FMarkerCensus` classifies by `bIsGuidedMarked && Side == ViewingSide`, which mirrors the
    production predicate. **No module-side value produces "should this actor's marker be lit"** —
    the ruling IS the conjunction — and the only cached answer available is
    `AStratUnitActor::LastAppliedView`, which is exactly what `IsGuidedMarkerVisible`'s
    declaration refuses to answer from, because a clause reading it would report what the actor
    was TOLD and pass whether or not anything reached the component. So both operands are read
    off the applied model, neither is derived, neither is a literal, and **the clause earns its
    keep through the three-population coverage and frame three rather than through the
    conjunction itself.** Same standing `T-INT-05.ConclusionIsReadFromHasResultAndNotFrom-
    ResultTier` records for its planted pair: when the INDEPENDENCE of two fields is the
    subject, the pairing is what isolates which field the code consults.
  - **NO MESH IS ASSIGNED, AND THAT IS NOW SETTLED RATHER THAN A JUDGEMENT CALL.**
    `AStratUnitActor::IsGuidedMarkerVisible`'s "FALSE WITH NO MARKER MESH ASSIGNED" sentence
    **has been retracted at its source** with this lane's measurement attached — the header now
    carries it under `RETRACTED>` and records that it "propagated off this file into a dispatch
    brief and from there into a test author's instructions". `USceneComponent::IsVisible`
    consults `bHiddenInGame`, the visible flag and the cached level collection, and **not** the
    static mesh. The discrimination is available on unconfigured actors and is measured (a true
    and a false in the same frame) rather than assumed. **THE LIMIT THAT SURVIVES: the accessor
    reports a FLAG, not pixels. Nothing headless gates "the marker is on screen" and no clause
    here tries to build one.**
  - **FALSIFIABILITY MEASURED IN THE TWO DIRECTIONS THAT MATTER, BOTH INSIDE `Tests/`.**
    Production source is not this lane's to touch even temporarily, so each broken predicate was
    simulated by **substituting the instrument**: the clause reads `bLit` from a local
    `MutantMarker` that answers as an actor driven by that predicate would, instead of from the
    real actor. Same technique `StratMatchConclusion.cpp`'s `MutantAlwaysAccepts` records.
    - **MF1 — THE FILTER REMOVED** (`bIsGuidedMarked` alone, the pre-ruling pass-through
      exactly). **Exactly this clause reddened**, on six assertions: the enemy `TestFalse` in
      frames one and two, both "exactly one marker is lit" counts reading **2**, and frame
      three's dark assertion — *"THE 2026-08-23 RULING: unit 7 carries `bIsGuidedMarked` and is
      NOT on the viewing side 0, so it must NOT wear the marker"*, *"exactly one marker is lit
      on the whole board, on unit 3' to be 1, but it was 2"*.
    - **MF2 — THE FILTER WIDENED THE WRONG WAY**, hiding a marked unit that IS on the viewing
      seat (`bIsGuidedMarked && Side != ViewingSide`). **Exactly this clause reddened**, on six
      assertions **including the load-bearing after-move step** — *"AFTER THE MOVE: the marked
      Infantry, unit 3, has left axial X=-1 Y=5 for axial X=2 Y=2 and its turn-1a marker is
      STILL SHOWING …"* — and frame three fired the other way, *"VIEWED FROM SEAT 1: unit 7 …
      wears the marker … to be true"*.
    - **The two mutations redden DISJOINT-ENOUGH assertion sets to tell them apart**, which is
      what says the clause distinguishes the two failure modes rather than merely noticing that
      something is wrong. Shipped bytes restored from a copy held OUTSIDE the repo and proved
      identical by `git hash-object` — `61a601c1360e1d765bc063fed75a3bed64e8534e`, before and
      after, on both rounds — never `git checkout --`, which rewrites LF to CRLF under
      `core.autocrlf=true`. `StratGuidedOpeningClauses.cpp` was not touched in this pass and is
      still `ba3513a8950940197e87dbe9dbc85bd5d1ca7efd`.
  - **A MUTATION THAT PROVED TOO LITTLE, AND WHY IT WAS THROWN AWAY RATHER THAN REPORTED.** The
    first attempt at MF1 mutated `FMarkerCensus`'s classifier. It reddened the clause — but at
    the census's own *"the viewing seat has exactly one marked Infantry … to be 1, but it was
    2"* premise, which is **fatal and returns before any actor is read**. That demonstrates the
    premise is live and demonstrates **nothing at all** about whether the marker assertions can
    see an unfiltered actor. **A mutation that reddens a clause at a guard clause has not tested
    the clause**, and the distinction is easy to miss because the report looks identical: one
    clause, red, on the predicted name. Substituting the instrument instead of the classifier is
    what reached the assertions, and adding frame three removed the need for the argument
    entirely — the ruling is now measured in both directions on real actors in the green run.
  - **WHAT THIS CLAUSE STILL DOES NOT PIN.** That anything is on screen (above). That the
    *enemy* seat's guidance is suppressed anywhere other than the marker — the strip, the ring
    and the lock set are not asked about here. And `ApplyUnitView`'s new third parameter is
    **not defaulted**, which is what stops a future caller silently getting an unfiltered
    marker; **no file under `Tests/` calls that method directly**, so nothing in this lane was
    forced to move by the signature and nothing here pins the non-defaulting either.

- **§2.11.6-B's BEAT 2 NOW HAS A GATE UNDER THE ARM THE SHIPPED SCENARIO CAN ACTUALLY FIRE, AND
  ITS TWO ON-SCREEN SURFACES HAVE ONE AT ALL.** Four clauses: two appended to
  `Source/StratPlay/Tests/StratGuidedOpeningClauses.cpp` under `T-UI-03`, two in one new file
  `Source/StratPlay/Tests/StratGuidedOpeningVisuals.cpp` under `T-UI-02`. Both ids pre-existing;
  neither minted here. Untracked / unstaged at the time of writing; staging is the user's call.
  The live suite figure for this pass is in `Tools/architect/state/global.md` and is not
  restated here.

  - **THE DEFECT THE FIRST TWO ARE ABOUT, and it is the reason one of them is written the way
    it is.** `Data/ferrum_crossing.json` sets `captureTurns` to 1, so `strat::captureTick`
    pushes a `CaptureProgress` with `turnsHeld` 1, flips the objective's owner and calls
    `clearProgress` — all inside ONE call. The projection's `progressForUnit` then finds
    nothing, so `FStratUnitView::CaptureProgress` reads **0 in every snapshot that can exist**;
    the pip lives only in that function's own stack frame. Beat 2's only arm was
    `FStratGuidedOpening::HasCapturePipLanded`, whose first conjunct is `CaptureProgress > 0`,
    so on this scenario beat 2 **could not retire at all** and rule 1 re-issued its directive on
    turns 3 and 4 over a Factory the player already held. The fix ORs a second observable,
    `IsRingedObjectiveHeldByGuidedSide`, reading `FStratHexView::Owner` at the hex
    `FStratBridge::GuidedOpeningHexes` answers.
  - **THE TRAP, AND IT IS THE WHOLE VALUE OF THE FIRST CLAUSE: IT PLANTS NO PIP.** A clause that
    planted one would drive `HasCapturePipLanded`, which was never broken, and would go **green
    on exactly the tree that failed**. `FGuidanceHarness::NoUnitShowsACapturePip` is asserted on
    BOTH sides of the retirement — before the flip and again after it — so "the pip arm cannot
    have fired" is a measurement taken in the run rather than a property of how the fixture
    happens to be written.
  - **WHAT EACH CLAUSE PINS.**
    - `T-UI-03.Beat2RetiresWhenTheRingedObjectiveBecomesTheGuidedSides` — differential, on the
      shape its pip-arm sibling already uses. One fixture, two observations at the SAME turn,
      and between them exactly one field of one hex changes: the ringed hex's `Owner`. The
      unheld half is the control; without it, "outstanding, then not" would be
      indistinguishable from a machine that retires beat 2 on any observation at all. Its two
      premises — the ring does not start out held, and no unit shows a pip — are asserted first
      and are fatal.
    - `T-UI-03.Beat2StaysOutstandingWhenAnotherObjectiveFlips` — the 2026-08-21 user ruling,
      inherited by the second arm. **The ruling was about beat 2's SUBJECT, not about which
      observable reports it**, so the new arm inherits it and needs its own gate: without this
      clause the one above passes on an arm that scans every hex with the `H.Hex == Objective`
      test dropped, which is one tidy-up away. **Its control is inside itself**: after the wrong
      hex is flipped and beat 2 must stay outstanding, the RINGED hex is flipped in the same
      fixture and beat 2 must retire — so an arm that was DELETED reddens the second half and an
      arm that was WIDENED reddens the first, and there is no state of the tree in which both
      pass for the wrong reason.
    - `T-UI-02.ObjectiveRingLightsExactlyTheGuidanceHex` — three things asserted together:
      `GetObjectiveOverlayCount() == 1`; `GetTargetOverlayCount() == 0`, because a ring drawn on
      §2.6's attack overlay is the most plausible wrong implementation AND would light
      `T-UI-02.AttackIsClosedForTheMarkedInfantry`'s subject at exactly the moment that clause
      requires it empty; and **the drawn instance's world position**, compared in XY against
      `AStratBoardActor::WorldLocationOfHex(Model.Guidance.ObjectiveHex)`. That third one is the
      one worth having — a count of one says something was drawn, not where. Z is deliberately
      not compared: `OverlayZOffset` is a phase-5 property this lane must not guess.
    - `T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex` — the last step is the clause and
      the rest is its setup. The marked Infantry is **moved through the rules**
      (`FStratBridge::ReachableHexes` supplies the destination, `SubmitMoveToHex` performs it),
      the model is rebuilt, and the RULES MODULE'S OWN `bIsGuidedMarked` is asserted still true
      on a unit that really has left `guidedOpening.infantry` — then the marker is required to
      still be showing. Nothing in the tree measured that before. A hex-keyed re-derivation
      anywhere in the chain unmarks the unit at the exact moment beat 1a's directive needs it.

  - **THE CLAUSE THE RUN REFUTED, AND WHAT IT TAUGHT: `bIsGuidedMarked` IS NOT ONE UNIT, IT IS
    ONE PER SEAT.** The marker clause first asserted "true on the guided seat's unit and false
    on every other actor" and FAILED with *"before the move: unit 7's turn-1a marker follows
    `bIsGuidedMarked` (the marked unit is 3): The two values are not equal."*
    `Data/ferrum_crossing.json` authors a `guidedOpening` entry for side 0 **and** for side 1,
    and `Ui.h` declares `isGuidedMarked` true "on the placement that the scenario file's
    `guidedOpening.infantry` names for this unit's SEAT", so TWO units carry the bit. The clause
    was wrong and the code was right — `AStratUnitActor::GuidedMarker`'s own declaration says "A
    PASS-THROUGH OF ONE PUBLISHED FIELD" — so the clause was corrected to compare each actor
    against **its own** unit's published bit, which is the pass-through and is a stronger
    statement than the side policy nobody wrote.
    - **AND THE FINDING THAT FALLS OUT OF IT, WHICH IS NOT THIS LANE'S TO FIX.**
      `UStratMatchSubsystem::ApplyView` hands every unit's view to its actor with no viewing-side
      filter, so **the ENEMY seat's guided Infantry wears the turn-1a marker on the player's
      screen too.** Measured, not inferred: unit 3 (side 0) and unit 7 (side 1) both read
      `IsGuidedMarkerVisible() == true` in the same frame. Whether that is correct is a
      presentation ruling; §2.11.6-B's marker is described as the thing that makes "Select the
      marked Infantry" readable, which reads as a one-seat visual. **The clause pins the
      pass-through and deliberately does NOT pin a side policy**, so a fix that filters by
      viewing side turns it red on purpose, and that redness would be a decision arriving rather
      than a regression. **[STAMPED 2026-08-23] That is exactly what happened, within the same
      day: the user ruled, `ApplyUnitView` gained the side filter, the clause went red as
      predicted and has been reshaped to pin the ruling. See the entry above; the sentences here
      stay as written for their own pass.**

  - **A HEADER CLAIM MEASURED FALSE, AND IT IS THE PREMISE THE BRIEF ASKED THIS CLAUSE TO BE
    BUILT ON.** `AStratUnitActor::IsGuidedMarkerVisible`'s declaration states *"FALSE WITH NO
    MARKER MESH ASSIGNED, which is the state this ships in"* and concludes that a clause telling
    "not marked" from "marked but unconfigured" apart *"must assign `GuidedMarkerMesh` on the
    spawned actor first"*. **The engine does not do that.** `IsGuidedMarkerVisible` returns
    `GuidedMarker->IsVisible()`, and `USceneComponent::IsVisible` consults `bHiddenInGame`, the
    visible flag and the cached level collection — and **not** the static mesh. Confirmed in the
    run: this fixture assigns no marker mesh and the marked units read TRUE. So the clause
    assigns nothing and instead MEASURES the discrimination (a true and a false in the same
    frame on the same unconfigured actors), which needed no reach into a protected property.
    - **WHAT THIS COSTS, AND IT IS REAL.** The accessor reports the visible FLAG, not pixels. It
      cannot tell anyone whether a marker mesh was ever assigned, so **"the turn-1a marker is on
      screen" has no gate under it and cannot get one headlessly** — the same shape as the
      overlay accessors, which is why the ring clause has to assign `OverlayMesh` before it can
      speak at all. The engineer's declaration should be corrected; this lane does not edit it.

  - **AN INSTRUMENT THAT IS MUTE READS AS PASSING, AND THE RING CLAUSE IS SHAPED AROUND IT.**
    `AStratBoardActor::FillOverlay` returns early — silently, by design — when the overlay
    component has no static mesh. On a fixture with no Blueprint defaults every overlay accessor
    reads 0 for every input forever, so "the ring is dark" could never go red.
    `GiveTheBoardAnOverlayMesh` (lifted from `StratGuidanceInputGates.cpp`, which measured it
    first) is what gives the accessor a voice, and **the lit half of the clause is the dark
    half's control** — having shown 1 on the same board in the same clause is what gives the 0
    its meaning.
  - **THE RING COMPONENT IS READ BY NAME THROUGH REFLECTION AND NOT BY A COMPONENT SCAN.**
    `ObjectiveOverlay` is `protected`, and `AStratBoardActor.h` records as of 2026-08-23 that
    with a THIRD overlay in existence a `GetComponents` scan for "the one that is not the reach
    overlay" returns whichever of two the iteration order reached first. The public
    `GetObjectiveOverlayCount()` is asserted equal to that component's own instance count, which
    turns the accessor's "off the component, not a cached number" sentence into a measurement.

  - **THE ACCEPTANCE IDs WERE MOVED OFF THE ONES PROPOSED, AND THE SPLIT IS THE POINT.** The
    engineer proposed all four under `T-UI-02`. The two beat clauses are `T-UI-03`'s: every
    beat-machine clause in `StratGuidedOpeningClauses.cpp` is T-UI-03's, including
    `Beat2RetiresOnlyOnAPipAtTheRingedObjective`, which is the OR-partner of the first new one —
    filing beat 2's second arm elsewhere would split ONE beat's retirement across two ids, which
    is the thing an id exists to prevent. The two visual clauses stay `T-UI-02`, which is
    `StratPlay`'s board-and-highlight id (`BoardHexRoundTrip`, `ReachOverlayIsNotComputedHere`,
    `AttackIsClosedForTheMarkedInfantry`). Both ids verified present in the tree before use.

  - **THREE PROSE SITES CORRECTED IN PLACE AND STAMPED, ALL IN ONE FILE, ALL COMMENT TEXT — NOT
    ONE ASSERTION MOVED.** In `StratGuidedOpeningClauses.cpp`:
    1. `FGuidanceHarness::CapturePipHasLanded`'s block, *"beat 2's retirement trigger"* — the
       definite article now survives only as the name of what that helper plants.
    2. The header block of `T-UI-03.Beat2RetiresOnlyOnAPipAtTheRingedObjective`, *"Beat 2 retires
       ONLY on a capture pip at the RINGED objective"* — flatly false since the OR landed. What
       the clause pins is unchanged and is worth exactly what it was worth: that the PIP arm is
       qualified by the ring. Read it as "a capture pip retires beat 2 only at the ringed
       objective".
    3. That same clause's *"OFF THE RING: beat 2 must stay OUTSTANDING"* bullet — true only
       because the ringed hex has not changed hands, a premise that clause **does not assert**.
       Both new clauses assert it explicitly and first.
    - **THE CLAUSE NAME `Beat2RetiresOnlyOnAPipAtTheRingedObjective` NOW READS AS THE WIDER,
      FALSE CLAIM AND WAS LEFT STANDING.** A rename is a change to the suite's clause set and
      this pass was scoped to prose; it is filed here so the next pass can make it. This is the
      one place in this entry where the record disagrees with the tree on purpose.
    - **THE SWEEP WAS BY CLAIM SHAPE OVER COMMENT PROSE ONLY, subject set derived from the tree
      (`find Source -type d -name Tests`) and never typed.** Twenty-nine patterns across two
      passes, case-insensitive, anchored to `^\s*(//|\*|/\*)`: `only on a pip`, `only a pip`,
      `a pip is the only`, `pip.{0,40}retires beat 2`, `beat 2.{0,60}pip`,
      `retires when.{0,40}pip`, `HasCapturePipLanded`, `capture pip`, `ringed objective`,
      `ringed Factory`, `unqualified reading`, `a pip anywhere`, `no other way.{0,40}beat 2`,
      `beat 2.{0,40}outstanding`, `iff`, `if and only if`, `only when`, `and nothing else`,
      `the only .{0,30}(trigger|arm|way|route)`, `retirement trigger`, `four retirement`,
      `no other`, `exactly one .{0,30}(trigger|arm)`, `beat 2`, `Beat2`, `objective ring`,
      `objective is held|held by`, `Owner`, `captureTurns`. **REPORTING THE ZEROES IS PART OF
      THE METHOD**: `only on a pip`, `only a pip`, `a pip is the only`, `retires when.*pip`,
      `no other way.*beat 2`, `if and only if`, `four retirement`,
      `exactly one .*(trigger|arm)` and `captureTurns` all returned **nothing anywhere in the
      tree** — that is what says the shape was looked for rather than assumed absent. **No
      fourth copy exists**: every hit outside `StratGuidedOpeningClauses.cpp` was either a
      different subject or historically-scoped text that is still true. The three sites above
      are all of them, and `Source/StratBridge/Tests/StratGuidedOpeningScenarioParity.cpp` — the
      file the ring arm is load-bearing on — carries no beat-2 claim at all.

  - **FALSIFIABILITY MEASURED BY FOUR SIMULTANEOUS MUTATIONS, ALL INSIDE `Tests/`.** Production
    source is not this lane's to touch even temporarily, so each broken design was simulated in
    the clause itself. M1: the ringed hex never changes hands, which is the PRE-FIX TREE exactly
    — no pip is possible on this scenario and the held arm has nothing to fire on. M2: the
    "other" hex flipped IS the ring, simulating an arm widened to scan every hex. M3a: the
    second `ApplyView` is never made, which is precisely what a show arm with no `else`
    produces. M3b: the position expectation pointed at a different hex of the same model. M4:
    the after-move expectation re-derived FROM THE HEX, which is the broken derivation
    `StratGuidedOpening.h` warns about by name. Result: **exactly the four targeted clauses
    reddened and no others**, and the messages are the ones each mutation was designed to
    produce — *"…beat 2 RETIRES once the ringed objective at axial X=2 Y=7 reads owner 0 … to be
    false"*, *"…leaves beat 2 OUTSTANDING … to be true"* with the line correctly reported as
    beat 3, *"…leaves the objective overlay EMPTY…"*, *"WorldLocationOfHex(axial X=2 Y=7) is
    (0.0, 0.0), the instance is at (550.0, 606.2)"*, and *"after the move: unit 3's turn-1a
    marker still equals ITS OWN published `bIsGuidedMarked`"*. Shipped bytes restored from
    copies held OUTSIDE the repo and proved identical by `git hash-object` —
    `ba3513a8950940197e87dbe9dbc85bd5d1ca7efd` (clauses) and
    `5ae5b066451b799e2f8b96eb7cebb471ba1385a7` (visuals), before and after — never
    `git checkout --`, which rewrites LF to CRLF under `core.autocrlf=true`.

  - **THE HEX LITERAL TRAP PAID AGAIN, AND IT IS WHY NO COORDINATE APPEARS IN EITHER FILE.** The
    mutation reported the ringed objective as axial `(2, 7)`; `Data/ferrum_crossing.json` writes
    `"objective": [5, 7]`, because the model carries AXIAL and the data file is authored in
    OFFSET. A clause that had transcribed the hex from the data file would have been wrong and
    would have looked right. Every hex in both files is read — from
    `FStratBridge::GuidedOpeningHexes`, from `FStratViewModel::Factories`, from
    `FStratBridge::ReachableHexes` — and none is chosen.

  - **WHAT THESE FOUR DO NOT PIN, and it is a real list rather than a hedge.**
    - **THAT ANYTHING IS ON SCREEN.** Both visual clauses read a component's state, not pixels.
      The ring clause needs a stand-in `OverlayMesh` to read non-zero at all, and the marker
      clause reads a visibility FLAG on a component with no mesh. The shipped `ObjectiveMaterial`
      and `GuidedMarkerMesh`/`GuidedMarkerMaterial` Blueprint defaults are the CONTENT lane's and
      have **no gate under them from here**. `T-UI-02.TheShippedHudNamesAGuidanceWidgetClass`'s
      shape — an asset-CDO clause in StratUI — is the precedent for one that could be written
      against `BP_StratBoardActor` and `BP_StratUnitActor` once those defaults are authored.
    - **THE VIEWING-SIDE QUESTION FOR THE MARKER**, above. Pinned as a pass-through; not pinned
      as a policy.
    - **`AStratUnitActor::BeginPlay`'s MARKER SETUP.** `GuidedMarkerMesh`,
      `GuidedMarkerMaterial` and `GuidedMarkerZOffset` are applied there, and unit actors are
      spawned by `ApplyView` into a world that has not begun play, so that function does not run
      in either clause. The "has no GuidedMarkerMesh set" log line is consequently never emitted
      and is deliberately NOT declared expected — `Occurrences 0` means *at least one*, and
      declaring a line that cannot fire is how three clauses in `StratGuidanceInputGates.cpp`
      failed once already.
    - **`AStratBoardActor::BeginPlay`'s `ObjectiveMaterial` ARM.** The ring clause dispatches
      BeginPlay with `OverlayMesh` set and `ObjectiveMaterial` unset, so the `!= nullptr` branch
      is never taken. What is pinned is that an unset material is silent, by the run producing no
      undeclared log; the material's *assignment* is unpinned.
    - **WHICH ARM RETIRED BEAT 2, AS THE LOG REPORTS IT.** `RetireWhatTheModelRetires` logs
      `(pip=…, held=…)` precisely so a session log distinguishes the two, and no clause reads
      that line. The clauses distinguish the arms by CONSTRUCTION instead — zero pips throughout
      — which is stronger for the property and says nothing about the log's accuracy.
    - **THE `ObjectiveOverlay` COLLISION SETTINGS.** The constructor clears collision on all
      three overlays for the reason `AStratUnitActor` states; nothing here asks whether a ring
      can block the cursor trace, and `HexUnderCursor` returns false in every headless state
      (`GATE-BUILDMENU`'s fact 1), so it cannot be asked here.

  - **LINE ENDINGS, AGAIN, AND THEY DID NOT COST A ROUND THIS TIME BECAUSE THEY WERE CHECKED
    FIRST.** `StratGuidedOpeningClauses.cpp` is **CRLF**; `StratMatchReconcile.cpp` is LF. Every
    edit to the CRLF file was made ending-aware, asserted its anchor matched **exactly once**
    before writing, and was verified by `git diff --numstat` showing **+473 / -0** — additions
    only, which is what says no silent whole-file conversion happened. The new
    `StratGuidedOpeningVisuals.cpp` is CRLF in the worktree, matching its neighbours in that
    directory.

- **§2.8's END-OF-MATCH TRANSITION IS GATED, AND THE ONE-SHOT LATCH IS NOW A COUNT RATHER THAN AN
  ARGUMENT.** Five clauses in one new file, `Source/StratPlay/Tests/StratMatchConclusion.cpp` —
  three under `T-INT-05`, two under `T-AI-01`, both ids pre-existing and neither minted here.
  Untracked at the time of writing; staging is the user's call. The live suite figure for this
  pass is in `Tools/architect/state/global.md` and is not restated here.
  - **THE DEFECT THEY ARE ABOUT.** A match that reached a §2.8 result did not end the game: the
    rules module was right, `UStratMatchSubsystem` persisted the completion three milliseconds
    later, and no end-of-match transition existed in `StratPlay` or `StratUI` at all. The AI's
    closing `EndTurn` was refused with `[T-SAVE-05] no match is running`, so the turn and the
    side to move never moved and the HUMAN inherited the AI's side and kept issuing accepted
    commands. `global.md`'s `## NEXT` entry carries the measured chain; nothing here restates it.
  - **WHAT EACH CLAUSE PINS.**
    - `T-INT-05.AConcludedModelRefusesPlayerCommands` — `StratMatchAcceptsPlayerCommands` answers
      BOTH ways over two planted models differing in EXACTLY ONE FIELD, with the accepting
      direction IN THE SAME CLAUSE. Its refusal is compared against
      `StratMatchConcludedRefusalText()` byte for byte through `ESearchCase::CaseSensitive`; the
      sentence is nowhere transcribed, because a clause that typed it would BE the fourth site
      the function exists to prevent. The out-parameter is asserted empty on the accepting path,
      which is the declared contract.
    - `T-INT-05.ConclusionIsReadFromHasResultAndNotFromResultTier` — both directions, over a
      REFLECTION WALK of `EStratResultTier` rather than a typed tier list, so a fifth tier is
      REQUIRED to be handled instead of silently unmentioned. The null is
      `FStratMatchView().ResultTier`, the module's own default, not the enumerator named in
      source. It carries a coverage assertion (`DecidedTiersSeen > 0`) so a walk that found
      nothing cannot pass having asserted nothing. **This is the one clause whose planted pair
      is IMPOSSIBLE by construction** — `StratBuildViewModel` never emits `bHasResult == false`
      beside a decided tier — and that is deliberate: the independence of the two fields IS the
      subject, so no module-side value exists to read and the pairing is what isolates which
      field the predicate consults.
    - `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` — the clause that would have caught the
      defect. **It plants no result.** Both sides AI, one synchronous `RunAiTurnsNow`, a real
      §2.8 result, and FOUR facts asserted together: the match reached a result, the call
      returned TRUE, its `OutFailureReason` is EMPTY, and a `STRAT-AI refused` line actually
      appeared. That fourth one is not decoration — without it the clause goes green over a game
      that ended tidily at a turn boundary, where the post-refusal reclassification arm is never
      entered at all.
    - `T-AI-01.AGenuineAiRefusalIsStillAFault` — the negative control, and the clause that a
      correct-but-over-broad fix fails. **The refusal is manufactured through a PRODUCTION seam
      and NOT a scripted port**: `RunAiTurnsNow` constructs its own `FStratBridgeAiTurnPort` and
      takes no port argument, so there is no injection point and adding one is outside this
      lane. `FStratMatchConfig::AiMaxCommandsPerTurn = 1` makes the runner exceed its own bound
      and report `phase=bound` while the match is one command old. The reason is asserted
      NON-EMPTY and never compared for content — the bound's sentence is
      `StratAiTurnRunner.cpp`'s and `T-AI-01.LoopBoundIsAReportedFault` already owns it.
      The clause's PREMISE (`!After.Match.bHasResult`) is checked first and is fatal.
    - `T-INT-05.ConcludingLogsOnceAndOnlyOnce` — **this is the entry with the most value per
      line.** Before it, `bMatchConclusionAnnounced` was CORROBORATED by a magnitude argument
      over a PIE log — six `STRAT-MATCH concluded` lines against 967 `STRAT-AI applied` — with
      nothing ever enumerating how many matches concluded, so the ratio was suggestive and not a
      measurement. Here the denominator is known: ONE match, then five counted
      `RefreshPresentation` calls, and `STRAT-MATCH concluded` is asserted to still read exactly
      one. The five refreshes are required to have RETURNED TRUE, which is what stops "still
      one" being satisfied by refreshes that never reached `ApplyView`.
  - **THE CAPTURE IS THE SIXTH IN THIS SUITE AND CARRIES `CanBeUsedOnMultipleThreads() = true`.**
    Without it the 1-in-4 flake returns. **Its filter is deliberately CASE-INSENSITIVE and only
    the counting is `ESearchCase::CaseSensitive`** — a strict filter would drop a `strat-match`
    line, and the "exactly one" assertion would then pass at ZERO by being blind to the very
    defect it is aimed at. Instruments loose, claims strict.
  - **`RefreshPresentation` RETURNS TRUE IN THIS FIXTURE, unlike in `StratGuidanceRouteClauses.cpp`'s**,
    and the difference is worth knowing before reusing either. `FindScoreboardHUD()` finds
    nothing here, so the scoreboard step — the only refusable one — is skipped entirely and the
    return is a real signal. In the guidance-route fixture a HUD exists without a scoreboard
    widget, so the same call always refuses at that last step and its return is asserted nowhere.
  - **FALSIFIABILITY MEASURED BY FIVE SIMULTANEOUS MUTATIONS, ALL INSIDE `Tests/`.** Production
    source is not this lane's to touch even temporarily, so each pre-fix or broken design was
    simulated in the clause itself: a local `MutantAlwaysAccepts` for the gate that never
    existed; a local `MutantTierIsConcluded` reading `ResultTier` for the second-author defect;
    the two `RunAiTurnsNow` assertions inverted; and the latch's expected count set to the
    unlatched prediction. Result: **exactly the five targeted clauses reddened and no others.**
    The messages are the ones each mutation was designed to produce — *"got '', expected 'the
    match is over; no further commands are accepted'"*, *"tier 'Draw' … to be false"* three
    times over (which is the reflection walk proving itself live), *"RunAiTurnsNow returned true
    (it returned true, reason '')"*, *"…to be 6, but it was 1"*. Shipped bytes restored from a
    copy held outside the repo and proved identical by `git hash-object`
    (`4e4c460fa7b29d1e426e0f21e00fac9aa1626fd6` before and after) — never `git checkout --`,
    which rewrites LF to CRLF under `core.autocrlf=true`.
  - **THE HONEST LIMIT OF THE LATCH CLAUSE.** The four other mutations simulate a real broken
    implementation; the latch one does not, because the latch is production state this lane
    cannot remove. What that mutation proves is that the clause DISCRIMINATES 1 from 6 on live
    data — it is not asserting a constant it also computed. It does not prove that deleting
    `bMatchConclusionAnnounced` would redden it, though the arithmetic is not in doubt.
  - **WHAT THESE FIVE DO NOT PIN, and it is a real list rather than a hedge.**
    - **WHO WON.** `strat::UiMatchView` carries no winning side and `strat::MatchResult::winner`
      reaches no projection, so nothing here can assert one. A victory SCREEN has no gate under
      it and cannot get one until that value is projected — `engine.md` carries the debt.
      - **RETIRED 2026-08-25 BY THE ENGINE HALF OF WAVE 2, AND STAMPED RATHER THAN DELETED.
        HALF the premise above is now false and half of it survives, which is why the bullet
        stays.** Upstream `96d93ea` — the `rulesCommit` now recorded in
        `Source/StratRules/StratRules.manifest.json` — added `strat::uiMatchResult` as a
        FOURTH `ui*` QUERY beside `uiReachable`, `uiForecast` and `uiBuildOptions`, and NOT as
        a `UiSnapshot` field. Verified against the tree rather than taken from the handoff:
        `git diff` of `Source/StratRules/Ui.h` and `Ui.good.cpp` is **additions only**, and no
        `kUi*FieldCount` constant, no `uiFieldContract()` row and no `uiEnumerateSnapshot`
        emit line moved. `FStratBridge::MatchResult` routes it, `FStratMatchResultView`
        reflects it with `EStratResultCause` ordinal-pinned to `strat::ResultCause` exactly as
        `EStratResultTier` is pinned to `strat::ResultTier`, and
        `UStratMatchSubsystem::GetMatchResult` is this engine's reader.
        - **THE HALF THAT SURVIVES UNCHANGED IS THE SNAPSHOT HALF.** `strat::UiMatchView`
          still carries `turn`, `turnCap`, `sideToMove`, `hasResult`, `resultTier` and NO
          winning side, and `FStratMatchView` still faithfully mirrors that. Only the claim
          that *nothing downstream can name a winner* is withdrawn.
        - **WHAT THIS FILE OWED IS NOW PAID, and the sentence is updated rather than left
          saying work is outstanding that has been done.** The debt was clauses against
          T-TURN-02, T-TURN-04 and T-TURN-05, with at least one arranging
          `winner != sideToMove`. All three landed in
          `Source/StratBridge/Tests/StratMatchResultClauses.cpp`, and
          `T-TURN-04.CappedMatchNamesTheKeyThatDiffered` is the one that arranges the
          inequality — see the pass entry at the top of this file for how, and for what the
          three do and do not reach.
        - **`FStratMatchResultView` IS DELIBERATELY OUTSIDE `StratViewModelParity.cpp`'s WALK**
          because it mirrors nothing IN the snapshot and is not a field of `FStratViewModel`.
          There is therefore no parity clause to write and its absence is not a gap. What
          needed pinning was the ROUTING — one bridge, one frame, all four fields — and
          `Stratocracy.StratUI.T-INT-05.MatchResultViewRoutesTheBridgeAnswer` is it. **[RENAMED
          2026-08-29 from `T-UI-05` by `global.md`'s ruling. The sentence's point is untouched:
          the routing is what needed pinning and this clause is still the one that pins it.
          Edited by the `coordinator` and not by this file's declared sole writer, on the user's
          direct instruction -- same marker, same instruction, same author as the two above.]**
    - **THE CONTROLLER-SIDE GATES.** `AStratPlayerController::HandleSelectionEvent`,
      `ToggleProductionMenu`'s open branch and `SubmitProductionChoice` all now call
      `StratMatchAcceptsPlayerCommands`, and **not one of those three CALL SITES is pinned
      here.** What is pinned is the predicate they call. A controller that stopped calling it
      passes all five of these clauses. That is the same shape as `BeginPlay`'s unpinned
      `SetViewDecorator` line recorded further down this file, and it has the same cause: a
      transient world does not begin play, so no spawned controller runs `BeginPlay` and the
      input path is not reachable headlessly.
    - **THE PACING-TIMER CLEAR** inside `ConcludeMatchIfEnded`. `AiTurnTimer` is private with no
      accessor and the delayed path needs a ticking world; the transition's OTHER side effect
      (the one log line) is what carries the latch clause.
    - **`GetConcludedMatchView()`'s CONTENTS.** Only `bHasResult` is read off it. It is
      `AppliedModel.Match` — what was drawn, not a fresh query — and nothing here distinguishes
      those two, because on this path they agree.
  - **A STALE-PROSE FINDING IN AN EXISTING CLAUSE, CORRECTED IN PLACE AND STAMPED RATHER THAN
    DELETED.** `T-INT-05.BothSidesAiReachesAResultWithinTheBound` in `StratAiMatchClauses.cpp`
    carried two paragraphs stating as present-tense fact that a completed AI-vs-AI game "comes
    back FALSE" and that this was "a production-side finding, filed rather than fixed". The
    filing was acted on and both sentences stopped describing the tree. **Comment text only; not
    one assertion in that clause moved**, and the clause is deliberately left NOT asserting the
    return value, because `AMatchWonMidTurnIsNotReportedAsAnAiFault` now owns it and two clauses
    asserting one value is one too many.
    - **AND THE THING THAT FINDING TAUGHT: the tripwire did not trip.** That block predicted in
      writing that if the behaviour were corrected, its
      `AddExpectedMessagePlain(TEXT("STRAT-AI refused"), …, Occurrences 0)` "THIS LINE FAILS".
      The behaviour was corrected and the line did not fail. The prediction was wrong, not the
      fix incomplete: the expectation is aimed at the RUNNER'S LOG LINE, and the fix is a
      reclassification one layer above the runner that leaves that line exactly where it was.
      **A tripwire on a log line cannot see a change in a return value.** Worth generalising
      before the next one is written.

  - **THE GATE FOUND A SECOND COPY AND MY OWN SWEEP THEN FOUND A THIRD. THE CLAIM EXISTED IN
    THREE PLACES AND THE FIRST CORRECTION REACHED ONE.** All three now stamped in place;
    comment text only, and not one assertion in any of the three files moved.
    1. `StratAiMatchClauses.cpp`, `T-INT-05.BothSidesAiReachesAResultWithinTheBound` — corrected
       in the landing pass.
    2. `StratMatchCompletionRecording.cpp`'s header block — **the gate's BLOCK finding**, and it
       blocked rather than being an observation for a reason worth keeping: its sentence reads
       *"for the reason that same clause states"*, and that same clause is copy 1. So after the
       first correction the cross-reference POINTED AT CORRECTED TEXT WHILE RESTATING THE
       UNCORRECTED VERSION LOCALLY — and a reader trusts the nearer sentence. A duplicated claim
       with a pointer between the copies is strictly worse than two independent copies, because
       the pointer manufactures corroboration.
    3. `StratAiMatchClauses.cpp`, `T-INT-05.BothSidesAiReachesAResultOnDifferentContent` —
       **found by my own sweep, known to nobody.** It says *"a correctly finished game currently
       comes back false"*, where the other two say *"returns FALSE"*. **A phrase sweep for the
       two known wordings would have missed it.** It also carried a SECOND copy of the wrong
       tripwire prediction, likewise stamped.
    - **THE TECHNIQUE THAT FOUND IT, and it is the reusable part.** Sweep by CLAIM SHAPE over
      COMMENT PROSE ONLY, with the subject set derived from the tree (`find Source -type d -name
      Tests`) rather than typed. Fourteen varied patterns, case-insensitive: `returns FALSE`,
      `comes back false|come back false|came back false`, `currently returns|currently
      comes|currently reports|currently answers`, `refused closing|closing EndTurn`, `no match is
      running`, `asserting .?true`, `RunAiTurnsNow`, `for a game that finished|finished
      correctly`, `production-side finding|filed rather than fixed`, `deleted from .?ApplyView`,
      `NoteMatchResultIfEnded`, `does not exist yet|that does not exist`, `no end-of-match|no
      transition`, `reported as a fault|reads as a fault`. **Restricting to comment lines is not
      cosmetic** — an unrestricted `returns? FALSE` returned 1235 hits, almost all of them
      `return false;`, which is a result nobody reads. Restricted to prose it returned 27, of
      which the three that mattered were visible at a glance. `reported as a fault` returned
      ZERO, and reporting a zero is part of the method: it is what says the shape was looked for
      rather than assumed absent.
  - **A STALE CITATION, CORRECTED WITHOUT WEAKENING THE ARGUMENT IT SERVES.**
    `StratMatchCompletionRecording.cpp`'s falsifiability list cited "the
    `NoteMatchResultIfEnded(Model)` line deleted from `ApplyView`". `ApplyView` no longer holds
    that line — it calls `ConcludeMatchIfEnded`, which calls `NoteMatchResultIfEnded` itself,
    first and unlatched. **Only the CITATION was stale; the REASONING survives the indirection**,
    because deleting the link at either end of the `ApplyView` -> `ConcludeMatchIfEnded` ->
    `NoteMatchResultIfEnded` chain still reddens `T-UI-03.AMatchReachingAResultRecordsCompletion-
    OnTheSlot`. The bullet is re-pointed at the chain and the old wording kept beneath it. This
    is the reason this record cites by SYMBOL rather than by `file:NNN` — a symbol cite degrades
    into "follow one more link", a line cite degrades into silence.
  - **TWO NON-GATING CITES TAKEN WHILE THE FILES WERE OPEN, because stamping them before the
    commit is cheaper than after.** `StratProductionMenuSeam.cpp` and
    `StratProductionMenuRouting.cpp` both described T-UI-04's widget as "a widget that does not
    exist yet"; `Content/UI/WBP_ProductionMenu.uasset` is in the tree (451 KB, untracked,
    verified rather than taken from the brief). **Only those five words were stale and the id
    does not move** — T-UI-04 is still an in-editor BINDING claim, both files still construct no
    widget and touch no Slate, and that is precisely what keeps their clauses under
    `GATE-BUILDMENU`. What changed is what T-UI-04 is BLOCKED ON: a human-driven playtest rather
    than a missing asset.
  - **LINE ENDINGS VARY WITHIN THIS ONE LANE AND IT COST A ROUND.**
    `StratMatchCompletionRecording.cpp` is CRLF; `StratAiMatchClauses.cpp`,
    `StratMatchConclusion.cpp`, `StratProductionMenuSeam.cpp` and
    `StratProductionMenuRouting.cpp` are all LF. A patch written with `
` endings silently
    matched ZERO in the CRLF file — which fails safe only because the edit was written to assert
    its match count first. Every edit above was made ending-aware and verified by
    `git diff --numstat` showing ADDITIONS ONLY (+52/-1, +40/-0, +9/-0, +5/-0) rather than
    whole-file churn, which is what a silent conversion looks like.

- **§2.11.5's BUILD AFFORDANCE is gated on the paths a headless run can reach, and THREE of the
  seven proposed clauses were refused as unfalsifiable.** Eight clauses, all under
  `GATE-BUILDMENU`, across two new clause files plus a UMG-free probe pair:
  `Source/StratUI/Tests/StratProductionMenuHostClauses.cpp` (4),
  `Source/StratPlay/Tests/StratProductionMenuAffordance.cpp` (4),
  `Source/StratUI/Tests/StratProductionMenuHostProbe.{h,cpp}` and
  `Source/StratUI/Tests/StratProductionMenuHostDouble.h`. Untracked at the time of writing;
  staging is the user's call. The live suite figure for this pass is in
  `Tools/architect/state/global.md` and is not restated here.

  - **THE CONDITION THAT SHAPES EVERY CLAUSE AND EVERY REFUSAL, measured in the engine rather
    than assumed.** Two headless facts, and between them they decide what the affordance can
    and cannot be gated on:
    1. `APlayerController::GetHitResultUnderCursor` needs a `ULocalPlayer` with a viewport
       client. A transient `UWorld` has neither, so `AStratPlayerController::HexUnderCursor`
       returns **false in every state a headless clause can reach**.
    2. `UUserWidget::AddToViewport` routes to `UGameViewportSubsystem::AddToScreen`, which
       returns early — no widget added — when the world is not a game world **or** when that
       game world has no `UGameViewportClient`. So no widget in the test lane is ever in a
       viewport, and `AStratScoreboardHUD::IsProductionMenuWidgetOpen()` is false throughout.

  - **What the eight clauses pin.**
    - `StratUI.GATE-BUILDMENU.MenuOpenReadsTheViewportAndNotThePointer` — the one clause a
      pointer-only `IsProductionMenuWidgetOpen` fails. Fact 2 above is what makes it a gate
      rather than a tautology: the pointer is non-null and the viewport is empty, which is the
      only state the two readers disagree in. **Both halves are asserted**, so a green cannot
      mean "there was no widget"; and the clause `AddError`s and fails rather than passing if a
      viewport ever does appear, because the discrimination would be gone.
    - `StratUI.GATE-BUILDMENU.CloseDropsThePanelUnconditionallyAndIsSafeWithNoneUp` — the
      ABSENCE of an `IsInViewport()` guard in `CloseProductionMenuWidget` is falsifiable
      precisely because of fact 2: an implementation that acquired one would leave the pointer
      set headlessly.
    - `StratUI.GATE-BUILDMENU.AnUnsetMenuClassRefusesTheOpenInItsOwnWords` — the refusal names
      the property, and is pinned as NOT the `CreateWidget returned null` sentence.
    - `StratUI.GATE-BUILDMENU.AnUnsetMenuClassIsReportedAtBeginPlayWithoutRaisingAFailure` —
      the fifth `GLog` capture in this suite; it carries `CanBeUsedOnMultipleThreads() = true`
      and would reintroduce the 1-in-4 flake without it. The instrument is given a control (the
      guidance and scoreboard sibling lines) before its report about the menu line is trusted.
      Reaching `BeginPlay`'s production-menu block at all needs an **adopted seeded bridge** — a
      bare HUD returns at `SeedBridge` long before it.
    - `StratPlay.GATE-BUILDMENU.AFreshControllerLatchesNoTargetHex`
    - `StratPlay.GATE-BUILDMENU.AToggleWithNoScoreboardHudRefusesNamingTheHost`
    - `StratPlay.GATE-BUILDMENU.AToggleWithABoardButNoCursorRefusesAndLatchesNothing` — starts a
      real match so `GetBoard()` is non-null, which is what makes the refusal the CURSOR one by
      elimination rather than the no-board one. The two refusal clauses assert the two sentences
      DIFFER, which is what stops them collapsing into one message a player cannot act on.
    - `StratPlay.GATE-BUILDMENU.TheHudCloseTakesThePanelDownAndLeavesTheRowsToItsCaller` — the
      load-bearing half is the NEGATIVE: the HUD does not clear `UStratMatchSubsystem::
      ProductionMenu`. A "helpful" HUD that did passes every other clause and fails this one.

  - **WHAT THESE CLAUSES DO NOT PIN. A third of the affordance has no gate under it, and this
    is the entry a future reader most needs.**
    - **`GetProductionTargetHex` has NO POSITIVE CONTROL.** By fact 1, `bHasProductionTargetHex`
      is never set headlessly, so the accessor answers false in every reachable state and an
      implementation that ALWAYS answered false passes. The latch-at-the-keypress design — the
      thing the whole affordance turns on — is unmeasured.
    - **The UNWIND branch is unreachable and is deliberately ungated.** `ToggleProductionMenu`
      returns at "the cursor is not on the board" *before* the latch is set and *before*
      `OpenProductionMenuWidget` is called, so `ARefusedOpenLeavesNoLatch` as proposed would
      assert `false == false` over a branch that never executed. The engineer's own judgement
      that this is the branch "most likely to rot" stands, with nothing under it.
    - **The CLOSE ARM of the toggle, and the ORDER of its two acts, are unreachable** (fact 2
      gates the arm). What is pinned is that the two acts belong to two different objects — the
      property the order is *about* — not the order itself.
    - **The "a menu is already open" refusal is unreachable** for the same reason; the
      stale-pointer branch below it is taken instead.
    - **`OpenProductionMenuAction`'s binding at `ETriggerEvent::Started` is not pinned.** The
      property is `protected`, `SetupInputComponent` never runs on a transient-world controller,
      and the action asset is the editor lane's. Asset-and-PIE gate, not a headless one.
    - **`AStratScoreboardHUD::CreateProductionMenuWidget`'s SUCCESS path is unreachable.**
      `UUserWidget::CreateWidgetInstance` refuses a controller that is not a *local* player
      controller, so `CreateWidget` returns null headlessly. `ProductionMenuWidgetClass` is also
      `protected`, so a clause cannot set it without a HUD subclass; none was written, because
      the path behind it does not run anyway.

  - **REFUSED: `SelectionMachineHasNoBuildArm`, and the code change that would make it
    writable.** The negative is TRUE — verified against the tree, not taken from the brief: the
    working-tree diff to `StratSelectionMachine.h` is **comment text only** (19 insertions, all
    inside the deferral block), `EStratSelectionCommand` still enumerates exactly
    `None, Move, Attack, EndTurn`, and `StratSubmitSelectionCommand` gained no arm. It is not
    WRITABLE. `EStratSelectionCommand` is a plain `enum class` and **not** a `UENUM`, so there
    is no reflected enumerator list to read and compare; the only assertions available are over
    the enumerators' integer values, which a `Build` **appended after `EndTurn`** would not
    disturb. A clause named for the absence while measuring only the ordering carries a subject
    broader than what it pins. **The change that would discharge this: mark
    `EStratSelectionCommand` `UENUM()`**, after which
    `StaticEnum<EStratSelectionCommand>()->GetIndexByNameString(TEXT("Build")) == INDEX_NONE`
    is a real measurement with `Move`/`Attack`/`EndTurn` as its positive control. This lane does
    not make that change.

  - **A NEW TEST-ONLY `UCLASS` AND A NEW UMG-FREE PROBE PAIR, on `StratGuidanceRouteDouble.h`'s
    exact precedent.** `UStratProductionMenuHostDouble` exists because
    `AStratScoreboardHUD::ProductionMenu` is `TObjectPtr<UUserWidget>` and `UUserWidget` is
    `UCLASS(Abstract)` — a clause cannot construct one, and the shipped occupant is an asset no
    headless run may depend on. It adds and overrides nothing, so every clause reads the HUD's
    own property. `StratProductionMenuHostProbe.h` is the UMG-free surface, and it is a module
    boundary and not a convenience: `UMG` is PRIVATE to StratUI, so a StratPlay clause can
    neither construct nor dereference a `UUserWidget` — which is why the affordance clauses live
    in StratPlay and the host clauses in StratUI, and why either set in the other module would
    be `LNK2019` rather than a test.

  - **INSTRUMENT NOTE.** `ProductionMenu` (the pointer) and `LastFailureReason` are public on
    `AStratScoreboardHUD`; `ProductionMenuWidgetClass`, `ProductionMenuZOrder` and
    `CreateProductionMenuWidget` are `protected`. `ToggleProductionMenu`,
    `GetProductionTargetHex` and `HexUnderCursor` are public on `AStratPlayerController`;
    `OpenProductionMenuAction`, `OnToggleProductionMenu` and `GetMatch` are not. Any future
    clause list should be checked against that split before it is agreed to.

- **§2.11.5's production-menu SEAM on `UStratMatchSubsystem` is gated, and writing the
  clauses measured the seam's own documentation to be WRONG about a refusal.** Ten clauses,
  all under `GATE-BUILDMENU`, in one new file:
  `Source/StratPlay/Tests/StratProductionMenuSeam.cpp`. Untracked at the time of writing;
  staging is the user's call. The live suite figure for this pass is in
  `Tools/architect/state/global.md` and is not restated here.
  - **Why `GATE-BUILDMENU` and why a THIRD file under it.** `T-UI-04` asserts the menu
    *binds*, which is in-editor over a widget that does not exist; nothing here constructs a
    widget. The id already covers §2.11.5 TRANSPORT in `StratBuildOptionRouting.cpp`
    (StratBridge) and `StratProductionMenuRouting.cpp` (StratUI). This file is the third and
    the split is forced, not chosen: `StratPlay` is the only module that can see
    `UStratMatchSubsystem`, `AStratUnitActor` and `FStratBridge` at once, which is what a
    seam whose read is state, whose write is a rules command, and whose acceptance must reach
    the BOARD actually needs. In either other module these are `LNK2019`, not tests.
  - **THE FINDING THAT CHANGED THE CLAUSE LIST, and it has since been ACTED ON at the source.
    `UStratMatchSubsystem::RefreshProductionMenu`'s declaration said "A hex that is not a
    factory is REFUSED, in the bridge's own words". It was not.** `FStratBridge::BuildOptions`
    sends "whether it is an objective at all, whether this side holds it, whether it is a
    build point" out on `available`, never on the refusal channel -- its own comment says so
    and StratBridge's `GATE-BUILDMENU.AnswersRideTheOkChannel` already pinned it one layer
    down. So a non-factory hex OPENS a menu and `IsProductionMenuOpen()` goes TRUE for a hex
    that is not a factory. The proposed clause "a non-factory hex is refused" was therefore
    REFUSED as unwritable-as-specified, and
    `ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` pins what the code does instead.
    A change that starts refusing turns it red on purpose; that redness is a decision
    arriving, not a regression.
    - **The disagreement is CLOSED, in favour of the code, and the clause did not move.**
      `strat-gameplay-engineer` corrected `RefreshProductionMenu`'s block -- it now says a
      non-factory hex opens a menu, and carries the withdrawn sentence beneath it under
      `RETRACTED> ` rather than deleting it -- and corrected
      `UStratMatchSubsystem::SubmitProductionChoice`'s `Bridge.Get() == nullptr` arm in the
      same pass. The claim's ORIGIN was withdrawn in `Tools/architect/state/global.md`, from
      which it had propagated into the kickoff note and the dispatch briefs. **The clause and
      every assertion in it are byte-unchanged**: it was written against the CODE, and it is
      the documentation that moved to meet it. Both prose sites in
      `Source/StratPlay/Tests/StratProductionMenuSeam.cpp` -- the file header block and
      `ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused`'s own block -- were re-pointed to
      the past tense on 2026-08-22, because a caveat that goes stale by UNDER-claiming sends
      the next reader to a site that agrees with the test and reads as if the correction never
      happened. The sentences above stay as written for their own pass.
      **[STAMPED 2026-08-22]**
  - **The consequence for all-or-nothing, and it is why that clause looks strange.** Because
    of the above, NO hex can produce a refused refresh on a live match. The only reachable
    refusal is a viewing side outside the snapshot's sides, so
    `ARefusedRefreshLeavesTheOpenMenuAndItsHexIntact` manufactures it with
    `SetViewingSide(Model.Sides.Num())` -- deliberately leaning on that method's documented
    non-range-check and documented non-rollback. The side count is read off the model; no
    side count is written down. What the clause adds over StratUI's
    `ARefusalLeavesTheCallersMenuIntact` is the HEX: the builder owns the array, the
    subsystem owns `ProductionMenuHex` beside it, and only this file can see both.
  - **What the other clauses pin.** `RefreshPublishesTheBridgesRowsInOrderAtTheHexAsked` --
    field-for-field including ORDER against `StratBuildProductionMenu` on the same bridge in
    the same frame, plus the published hex. `AnUnofferedDefIndexIsRefusedWithoutReachingThe-
    Rules` and `SubmitWithNoMenuOpenIsRefusedWithoutReachingTheRules` -- both with
    `FStratBridge::RecordedCommandCount()` as the control, because a bare `false` is the same
    observable whether the seam declined to ask or asked and was told no, and the declaration
    claims the first. `CloseEmptiesTheRowsForgetsTheFactoryAndSubmitsNothing`.
    `AMenuDoesNotSurviveAReseed`. `AnAcceptedBuildReachesTheBoardWithNoManualRefresh` -- the
    clause calls NOTHING after the submit, and the world's actor id set must equal the
    rebuilt model's unit id set exactly.
  - **The highest-value clause, and what it measures INSTEAD of the obvious thing.**
    `AnAcceptedBuildLandsAtTheOpenMenusFactoryAndNoOther` does NOT check the new unit's hex:
    §2.7 spawns at the factory hex OR AN ADJACENT ONE, so "standing on the factory" is not a
    property that holds and measuring it would need an adjacency computation -- arithmetic
    the gate would then be asserting against itself. It reads
    `FStratFactoryView::bHasBuiltThisTurn`, the rules module's own per-factory record of
    T-TURN-10's allowance keyed by hex, and requires that EXACTLY ONE factory's flag moved
    and that it is `ProductionMenuHex`.
  - **THE HONEST LIMIT OF THAT CLAUSE, and it is a `Data/` limit rather than a fixture one.**
    On `ferrum_crossing.json` side 0 holds exactly ONE factory, so the fixture cannot exhibit
    "built at the wrong factory the caller could legitimately have named". Measured, not
    argued: the mutation below reddened it with *"factory is not held by this side"*, i.e.
    via the rules refusing a hex this side does not hold, NOT via a unit appearing in the
    wrong place. Distinguishing the two needs a scenario giving one side two factories -- a
    `Data/` addition, vendored and hash-gated, writable from no lane in this repo.
  - **The AI-turn refusal is reachable from outside the class, through a PRODUCTION seam.**
    `bAiTurnRunning` is a private `TGuardValue` with no setter, but `RunAiTurnsNow` ends with
    `RefreshPresentation()` STILL INSIDE the guard's scope, and that builds through
    `BuildViewModelForPresentation`, which runs the registered `FStratViewDecorator`. So
    `ASubmitDuringAnAiTurnIsRefusedAndRecordsNothing` submits from inside a decorator. Three
    things make it non-vacuous: the decorator's FIRING is itself asserted (a removed seam
    would otherwise leave it green having measured nothing); the menu is open and the index
    is one the menu OFFERS, so only the AI arm can account for the refusal; and the count is
    read on both sides of the call INSIDE the decorator, because the AI records into the same
    bridge and a count taken outside would move for the AI's own reasons. Its own instrument
    control is that the AI turn as a whole DID move the count.
  - **Falsifiability MEASURED by two mutations inside `Tests/` only** -- production source is
    not this lane's to touch even temporarily, so the pre-guard designs were simulated by
    calling `FStratBridge::SubmitBuildAtHex` directly in place of `SubmitProductionChoice`.
    M1 (a caller-supplied hex, at another factory) and M2 (submit with no reconcile) reddened
    **exactly the two targeted clauses and no others**, M2 reproducing the defect verbatim:
    *"actors [1..10] vs model [1..11]"* and *"the new unit 11 has an actor on the board ...
    to be not null"*. Shipped bytes restored from a copy held outside the repo and proved
    identical by `git hash-object` (`b1df955da5f95eb1f154a2dc4b1608ecf4975ed7` before and
    after) -- never `git checkout --`, which rewrites LF to CRLF under `core.autocrlf=true`.
  - **Technique reused and worth reusing again: enumerate the case list from the MODEL, never
    pick a hex.** No hex literal appears anywhere in the file; the held factory, the other
    factories and the first non-factory hex are all read out of `FStratViewModel`. This paid
    immediately: the mutation reported the neighbouring factory as `(5, 2)` where the
    scenario file writes `[6, 2]`, because `Factories` carries AXIAL coordinates and
    `Data/ferrum_crossing.json` is authored in OFFSET. A clause that had transcribed a hex
    from the data file would have been wrong and would have looked right.
  - **Coverage assertions, so no clause can go vacuous in silence.** The non-factory clause
    requires the SAME side in the SAME frame to produce an AVAILABLE menu at its held factory
    (an absence and its control belong in one clause). The build clauses require an
    affordable-AND-available row to exist, require the target factory's flag to be FALSE
    before, and require the build to have actually spawned -- §2.7 HOLDS a build whose
    factory is boxed in, so a build that spawned nothing would satisfy set equality for free.
  - **What these ten clauses do NOT pin.** (a) Any rules-module buildlist behaviour -- that is
    the crew repo's `GATE-BUILDLIST`, and a second opinion engine-side about a rule this tree
    does not own is worse than none. (b) The `bAffordable`/`bAvailable` independence, already
    pinned by StratUI's `AvailabilityAndAffordabilityAreIndependent`; nothing here re-asserts
    it, and no clause anywhere asserts that "neither is the AND of anything" -- that phrasing
    names no observable state and was declined. (c) That the forgotten `ProductionMenuHex` is
    specifically the origin -- the close clause asserts only that it is no longer the factory
    it was showing, because `FIntPoint(0, 0)` is a real hex on this board and writing it down
    would make the clause a statement about a literal. A close that moved the hex to some
    other non-opened value passes. (d) Anything about a menu WIDGET; no Slate, no
    `UUserWidget`, all ten run headless under `-nullrhi`.
  - **Slot hygiene held.** The one clause that plays a real AI turn names a slot of its own
    (`StratocracyAutomation_ProductionMenuSeam`), deletes it on both ends, and leaves
    `FStratMatchConfig::bRecordCompletionOnMatchEnd` at its C++ default rather than writing
    the value down. `Saved/SaveGames/` was empty before the run and empty after it.

- **§2.11.5's production-menu ROUTING is gated; the module's buildlist behaviour is not gated
  here and must not be.** Eleven clauses under a new id `GATE-BUILDMENU`, split across two files
  by what each can link against — `Source/StratBridge/Tests/StratBuildOptionRouting.cpp` (six)
  and `Source/StratUI/Tests/StratProductionMenuRouting.cpp` (five). The live count is in
  `Tools/architect/state/global.md` and nowhere else.
  - **Why `GATE-BUILDMENU` and NOT `T-UI-04`.** T-UI-04 asserts the production menu *binds*,
    which is in-editor over a widget that does not exist yet. Nothing in these eleven clauses
    constructs a widget or touches Slate, so §4.11 row 8's ledger row is unmoved by them.
    `GATE-BRIDGE-DEFS` and `GATE-SAVE-PARSE` are the precedent for a gate that has no acceptance
    row of its own.
  - **What the split of files is forced by, not chosen.** The bridge file compares
    `FStratBridge::BuildOptions` against `strat::uiBuildOptions` over the bridge's own
    `MakeUiWorld()`, so it must CALL both — only a `.cpp` inside `StratBridge` links the
    vendored symbol. The StratUI file compares `StratBuildProductionMenu` against
    `FStratBridge::BuildOptions` on the same bridge in the same frame, which needs no
    `strat::` call at all. The same bridge-side file written in StratUI is not a weaker test,
    it is `LNK2019`.
  - **What each clause pins.** Bridge: full row-and-field parity with the module including
    ORDER (`OptionsMatchTheModuleQuery`); the two channels — an unheld factory, a spent
    allowance, a pending build and a non-build-point hex all arrive as `Ok()` with `available`
    false, never as refusals (`AnswersRideTheOkChannel`); one row per loaded §2.4 row and never
    empty, on the unavailable cases too (`SuccessIsOneRowPerUnitTableRow`); the side as the one
    malformed-question case (`SideOutsideTheMatchIsRefused`); the module's own invalid-side
    answer shown to EXIST and then shown not to reach a caller
    (`TheModulesInvalidSideAnswerIsUnreachable`); and the two distinct pre-match refusals with
    the out-parameter cleared (`RefusesBeforeThereIsAMatch`). StratUI: field-for-field mirror
    including order (`MenuMirrorsTheBridgeRowForRow`); `bAffordable` copied and never recomputed
    (`AffordabilityIsCopiedNotRecomputed`); the two flags independent in BOTH directions
    (`AvailabilityAndAffordabilityAreIndependent`); an answered no returned as a SUCCESSFUL full
    menu (`AnAnsweredNoIsNotARefusal`); and all-or-nothing on refusal
    (`ARefusalLeavesTheCallersMenuIntact`).
  - **What these clauses do NOT pin, and where it IS pinned.** None of the rules-module
    behaviour: all four rows returned, the affordability split itself, the T-TURN-10 and
    `buildWaiting` gates, Q31's boxed-in factory staying available, availability never varying
    by row. That is the crew repo's `GATE-BUILDLIST`, 14 clauses in `test_ui.cpp`, vendored at
    `cae01e3`. Duplicating it engine-side would give this tree a second opinion about a rule it
    does not own. These eleven are about TRANSPORT only.
  - **Technique worth reusing: enumerate the case list from the board, never pick a hex.** Every
    clause asks every (factory, side) pair plus the first hex no factory claims, all read out of
    the snapshot / view model. A pre-check that refuses exactly the enemy-held factory cannot
    then hide behind a case the file happened not to choose, and a scenario edit moves the case
    list with it. Measured: mutating `BuildOptions` to fold an answered no onto the refusal
    channel reddened four of the six bridge clauses at once.
  - **Technique worth reusing: a coverage assertion, so a clause cannot go vacuous in silence.**
    `AnswersRideTheOkChannel` would be satisfied by a board on which every question answered
    yes, and `AffordabilityIsCopiedNotRecomputed` is BLIND to the `affordable && available` fold
    on any row where `available` is true. Both therefore assert that the board supplied the case
    they need — an answered no, and an unavailable-yet-affordable row — and fail loudly naming
    the missing case if a scenario edit ever stops producing it.
  - **The case-insensitivity trap, and what was done about it.** `FString`'s `==`, `Contains`
    and `TestEqual` are all case-insensitive, and so is `FName` comparison, so a reason-string
    clause written the obvious way cannot fail on a casing defect. Both string comparisons in
    the StratUI file go through `FString::Equals(..., ESearchCase::CaseSensitive)` against the
    module's own `std::string` bytes. The bridge file compares `std::string` to `std::string`
    directly, which is case-sensitive by construction. **No refusal sentence is transcribed
    anywhere in either file** — "the module's invalid-side answer is unreachable" is pinned as
    *zero rows reach the caller*, which needs no string and cannot be defeated by casing.
  - **Two things deliberately NOT asserted, because the assertion could not fail.** (a) That
    `isFlag` never appears in this query's output: it is a `Scenario.h` PLACEMENT field, the
    buildlist declares no such member, and `data/units.csv` carries no flag row, so the check
    passes on an empty implementation and a wrong one alike. (b) Any equality against a
    transcribed refusal sentence — see above.
  - **A default cannot signal "unset", and both sides default to -1.** `FStratBuildOptionView::DefIndex`
    is `INDEX_NONE` and `UiBuildOption::defIndex` is `-1`, so a bare parity check could not tell
    "copied" from "both untouched". Both files establish that the module's `defIndex` is a real
    §2.4 row index BEFORE comparing, which is what gives the equality teeth. The all-or-nothing
    clause plants a sentinel row (`DefIndex 4242`, `Id NotAUnit`) for the same reason and checks
    it is still there after the refusal, then gone after the success.
  - **Falsifiability, measured rather than argued.** Mutant A (`BuildOptions` folding an
    answered no onto the refusal channel) → `AnswersRideTheOkChannel`,
    `OptionsMatchTheModuleQuery`, `SuccessIsOneRowPerUnitTableRow` and
    `SideOutsideTheMatchIsRefused` all red. Mutant B (`bAffordable = affordable && available` in
    `StratBuildProductionMenu`) → `AffordabilityIsCopiedNotRecomputed` and
    `AvailabilityAndAffordabilityAreIndependent` red, and `MenuMirrorsTheBridgeRowForRow`
    correctly STAYS green, since that clause deliberately leaves `bAffordable` to the clause
    named after it. Both restored from a byte copy held outside the repo and verified by
    `git hash-object` — never `git checkout --`, which rewrites LF to CRLF under this repo's
    `core.autocrlf=true`.

- **The two guided-opening delivery defects now have clauses; they shipped green because nothing
  asked either question.** Nine clauses added to
  `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp`, all under `T-INT-05` — see "Why not
  T-UI-03" below. The live count is in `Tools/architect/state/global.md` and nowhere else.
  - **DEFECT 1 — the strip did not exist when the only decorated push arrived.**
    `StripCreatedAfterAPushStillCarriesIt` drives the production ordering rather than a
    synthesised push: recorder registered, then `StartMatch`, whose own reconcile pushes at a HUD
    with **no** strip; the strip is installed afterwards and `DeliverLatestGuidance()` is called
    by the clause, because `StratTestInstallGuidanceStripDouble` assigns `GuidanceStrip`
    directly and never runs `CreateGuidanceWidget`. Supported by
    `DeliverLatestGuidanceIsSilentBeforeAnyPush` (both falses — no strip, and nothing pushed),
    `DeliverLatestGuidanceIsIdempotent` (a replay, not a consumable latch: the strip is
    overwritten through the widget's own push and the third delivery restores the cache), and
    `DeliverLatestGuidanceReplaysOnlyTheLatestPush` (latest, never a backlog).
  - **DEFECT 2 — every subsystem-side reconcile cleared the strip.**
    `SetViewingSideDoesNotClearAnActiveDirective` is the measured reproduction and the clause
    whose absence let it ship. `RefreshPresentationDecoratesBeforeApplying`,
    `StartMatchAppliesADecoratedModel`, `UndecoratedBuildStaysUndecorated` and
    `RefreshFromMachineDecoratesWithNoRegistration` pin the `FStratViewDecorator` seam from both
    ends.
  - **Where every expectation comes from, without exception: `FStratGuidedOpening`.** A test-only
    `FRecordingDecorator` (namespace `StratGuidanceRoute`) runs the real module-side author as
    the registered decorator, records the block it wrote onto the model, and each clause compares
    the STRIP against that recording through `SameGuidance` — `CompareScriptStruct` over the
    struct's own reflection data, which is case-SENSITIVE where `TestEqual` on an `FString` in
    this project is not. **Not one guidance field is typed out in the new section.** The recorder
    arms LAZILY, mirroring `AStratPlayerController::TryArmGuidedOpening`, because one clause must
    register before `StartMatch` and there is no seeded bridge to call `Begin` with until then.
  - **What these clauses do NOT pin, and it is a real gap rather than a hedge.**
    `AStratPlayerController::BeginPlay`'s `SetViewDecorator` call is **not covered**: a transient
    world built with `InitializeActorsForPlay` does not begin play, so no spawned controller runs
    `BeginPlay` and no clause here observes that line. What IS pinned is the seam it registers
    with. The registration line itself rests solely on the PIE session of 2026-08-21. Likewise
    `EndPlay`'s `ClearViewDecorator` is unpinned, and its own comment records that it clears by
    POSITION and not by identity — a second controller in one world would drop the wrong binding.
  - **Falsifiability was MEASURED, not argued, and by three mutations inside `Tests/` only.**
    Production source is not this lane's to touch even temporarily, so the pre-fix code paths
    were simulated in the clauses themselves: M1 removed the `DeliverLatestGuidance()` call; M2
    and M3 replaced `SetViewingSide` / `RefreshPresentation` with `BuildViewModel` +
    `ApplyView`, which is exactly what those two did before the fix. Result: **exactly the three
    targeted clauses reddened and no others**, and the headline one reproduced the PIE symptom
    verbatim — `strip: bActive=0 beat=0 directive=''` against
    `decorator: bActive=1 beat=1 directive='Select the marked Infantry. …'`. Bytes restored and
    proved: `git hash-object` `310f9262af8831160b018fe7452d5d29c0859fd9` before and after.
    **[STAMPED 2026-08-22]**
  - **Why `T-INT-05` and not the `T-UI-03` the engineer proposed.** `T-UI-03` is the no-widget-side-
    arithmetic clause; nothing here is about arithmetic. `T-INT-05` is "rebuild the screen from
    the view model alone", which is precisely what a HUD-side CACHE threatens — and
    `StratGuidanceRouteClauses.cpp`'s header already files every clause about
    `AStratScoreboardHUD::PushGuidance` under that ID. All nine live in `StratPlay` for the
    module-boundary reason that file records: `StratPlay` is the only module that can see both
    `UStratMatchSubsystem` and (through the `STRATUI_API` probe) the strip. Put in `StratUI`,
    the decorator clauses would be `LNK2019`, not tests.
  - **Two fixture limitations, measured the hard way on the first run, and worth reusing.**
    (1) `AddExpectedMessagePlain(..., Occurrences 0)` means **at least one**, not "any number":
    the one new clause that starts no match failed with *"Expected suppressed ('Warning') level
    log message or higher matching 'no tile mesh for terrain' did not occur"* purely for
    declaring the fixture warning it could never emit. (2) **`RefreshPresentation`,
    `SetViewingSide` and `RefreshFromMachine` all return FALSE in this fixture** — the harness
    deliberately does not dispatch the HUD's `BeginPlay`, so it holds no scoreboard widget and
    `RefreshScoreboard` refuses as the LAST step, long after `ApplyView` has already run and
    already pushed. Their returns are reported into the failure messages, never asserted;
    asserting them would be asserting the presence of a scoreboard asset, which is the editor
    lane's subject. `RefreshAndReport` in `StratGuidanceRoute` carries the reasoning.
  - **`FRouteHarness::Arm` was split into `SpawnWorldAndActors` / `StartTheMatch` /
    `InstallStrip`** rather than a second fixture being written, because the ORDER of those three
    is the subject of two clauses — a strip installed before the match start receives its push
    and one installed after does not. `Arm(bWithStrip, …)` is unchanged in effect, so the seven
    clauses written against it drive the fixture they always drove. It gained a
    `bStratController` switch for the one clause that needs `AStratPlayerController`.

- **A helper inside my own lane had gone inert, and the fix is now itself falsifiable by
  measurement rather than by claim.** `AlterOneField` in
  `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp` (namespace `StratGuidanceRoute`) took
  any `FStructProperty` and reinterpreted its bytes as an `FIntPoint`, while the same block's
  trailing `OutError` promised to refuse "a field of a kind this helper cannot vary". It was
  harmless only because `FIntPoint` happens to be the one struct kind `FStratGuidanceView`
  declares. The moment a second struct field arrived, `T-INT-05.GuidanceComparisonDistinguishesViews` would have written eight bytes through a foreign offset, seen the comparator answer
  "different" for the wrong reason, and stayed GREEN while proving nothing about the new field —
  the going-inert failure the property-walk design exists to prevent, relocated one level down
  into the helper. The gate is now on the struct's identity
  (`AsStruct->Struct == TBaseStructure<FIntPoint>::Get()`, factored into `CanVaryStructField`),
  and anything else falls through to the existing refusal, which names the field.
  - **New clause: `Stratocracy.StratPlay.T-INT-05.GuidanceFieldVaryingHelperGatesOnStructIdentity`.**
    ACCEPT direction: it walks `FStratGuidanceView`'s own property list and requires *every*
    struct field it declares to be one the helper can vary — deliberately a walk and not a
    lookup of `ObjectiveHex` by name, so a second struct field on the view is *required* to be
    handled rather than silently unmentioned. REFUSE direction: a real `FStructProperty` of
    another kind (`FStratViewModel::Guidance`) is refused by the predicate, refused by
    `AlterOneField`, named in the refusal string, and leaves the view untouched.
  - **What this clause does NOT pin:** any struct kind other than `FIntPoint` is *unsupported*,
    not *supported and tested*. The clause pins that the helper says so out loud instead of
    guessing. It also says nothing about whether the FIntPoint mutation is the *right* mutation,
    only that it happens and that the comparator notices.
  - **Technique worth reusing: ask the predicate, do not perform the write.** The refuse
    direction cannot be proved by calling the writing path on a foreign struct field — that
    write goes through the foreign property's own offset, so a probe built that way relies on
    the very gate it is testing in order not to smash the stack. Splitting the predicate out
    (`CanVaryStructField`) removes the circularity and makes both directions free to assert.
  - **The fix was MEASURED falsifiable in two directions, not asserted.** Two mutation builds,
    each run through `UnrealEditor-Cmd.exe` against the named clauses, then the shipped bytes
    restored and verified identical by `git hash-object`
    (`c0b8693dbb7d2d6874b7b59264ba83fa9b75a3e4` before and after):
    - *Gate re-pointed at `TBaseStructure<FVector>::Get()`* (safe: no write becomes reachable) —
      the ACCEPT direction went red, and so did the pre-existing
      `T-INT-05.GuidanceComparisonDistinguishesViews`, with
      `Error : field 'ObjectiveHex' is of a kind this helper cannot vary`. That second failure
      is the load-bearing evidence: it is exactly the message a newly-added struct field will
      produce, and it proves the control clause is now coupled to the gate.
    - *Gate widened back to `return AsStruct != nullptr;` with the write neutralised in the same
      mutation* (the neutralising is what keeps the widened gate from scribbling through a
      foreign offset) — the REFUSE direction went red on all three of its assertions,
      `Expected 'the field-varying helper REFUSES a non-FIntPoint struct field ('Guidance', a
      'StratGuidanceView')' ... to be false`.
    Recorded because a one-directional mutation would have proved half of it, and because the
    *reason* the second mutation had to neutralise the write is the same reason the predicate
    was split out at all.

- **Two shipped-asset facts that lived in `.uasset` bytes and in nothing else are now clauses,
  filed under `T-UI-02`.** Both in `Source/StratUI/Tests/StratGuidanceStripClauses.cpp`,
  namespace `StratGuidanceStripAssets`. `T-UI-06` was *not* minted for them: it is not defined
  in the shipped GDD, and `T-UI-02` is the recorded partial fit.
  - `Stratocracy.StratUI.T-UI-02.TheDirectiveStripAssetIsAGuidanceWidget` — the generated class
    of `Content/UI/WBP_DirectiveStrip.uasset` has `UStratGuidanceWidget` as an ancestor. Nothing
    else in the build says so: re-parenting a Widget Blueprint compiles, saves and cooks
    silently, and the only symptom is a strip that never draws.
  - `Stratocracy.StratUI.T-UI-02.TheShippedHudNamesAGuidanceWidgetClass` — the
    `GuidanceWidgetClass` class default on `Content/UI/BP_StratScoreboardHUD.uasset`'s CDO is
    non-null *and* derives from `UStratGuidanceWidget`. `AStratScoreboardHUD::CreateGuidanceWidget` treats an unset class as a legitimate configuration and logs it at Log verbosity — by
    design, since a build shipping without a guided opening is a real build — so a `None` there
    produces no warning, no error and no red test. One went unnoticed for a whole phase.
  - **Where the expectations come from:** `UStratGuidanceWidget::StaticClass()` on both, read
    from reflection, never a typed class-name string. The only literals are the two asset
    *paths*, which are subjects and not expectations, and which were verified against
    `Content/UI/` in the tree. Loading a `/Game/` path in a fixture is one of the two standing
    exceptions to the no-path-literals rule; `Source/StratBridge/Tests/StratBridgeParity.cpp`
    does the same with `DT_Units`/`DT_Terrain`.
  - **Trap avoided, worth reusing: do NOT pass the asserted base class to `StaticLoadClass`.**
    It filters, returning null when the loaded class fails the filter — so filtering on
    `UStratGuidanceWidget` would turn "the asset was re-parented", the defect the clause exists
    to catch, into "the asset did not load", and the failure message would blame the wrong
    thing. `LoadGeneratedClass` passes `UObject::StaticClass()` and asserts kinship afterwards.
  - **Both clauses check that the class they resolved is Blueprint-generated and not native**
    before reading anything off it, and the HUD clause checks the PREMISE that the C++ default
    is null first — same shape as `T-UI-03.TheShippedGameModeOptsIn`. Without the premise the
    clause could pass on an asset that overrode nothing while still wearing its own name.
  - **What these clauses do NOT pin:** anything about the strip's *appearance*, its widget tree,
    or whether it is on screen. Both read reflection/CDO data only, so both run headless under
    `-nullrhi` — chosen deliberately over a clause needing a real widget tree.
  - **Not a flake, but a real finding if it ever changes:** `/Game/` packages loaded fine in the
    `-nullrhi` automation environment on this pass. If either clause ever goes red on *loading*
    rather than on kinship, that is a harness finding to report, never something to soften into
    a skip — every failure path in both clauses is red for that reason.

- **§2.11.6's guidance route is pinned by six clauses under `T-INT-05`, and writing them
  measured a module boundary the brief had not accounted for.** New files, all in `Tests/`:
  `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp` (six macros — five clauses plus one
  instrument control) and `Source/StratUI/Tests/StratGuidanceStripClauses.cpp` (one), with
  `Source/StratUI/Tests/StratGuidanceRouteDouble.h` and `StratGuidanceRouteProbe.h/.cpp` as the
  fixture. Untracked at time of writing; staging is the user's call. The live suite figure for
  this pass is in `Tools/architect/state/global.md` and is not restated here.
  - **THE MEASUREMENT THAT SHAPED EVERY FILE: `UMG` is a PRIVATE dependency of StratUI**
    (`StratUI.Build.cs`). A `UUserWidget` subclass therefore cannot be declared in StratPlay,
    and StratPlay cannot name `UStratGuidanceWidget` at all — not by include (the path to
    `Blueprint/UserWidget.h` is not exposed) and not by link. Measured as ~60 `LNK2019`s in
    `Module.StratPlay.gen.cpp.obj` naming `UUserWidget::NativeTick`, `::Initialize`,
    `::RebuildWidget` and friends, from a first attempt that put the test double in
    `Source/StratPlay/Tests/`. `AStratScoreboardHUD.h` already records the same boundary from
    the other side: it FORWARD DECLARES the widget and holds a `TObjectPtr`, precisely so
    StratPlay can include the HUD without acquiring UMG. **The consequence for this lane is
    permanent: any future clause that must touch a StratUI widget AND a StratPlay type needs a
    UMG-free probe in `Source/StratUI/Tests/`, not a shared header.** That probe is
    `StratGuidanceRouteProbe.h` — every signature in it is expressible in types StratPlay
    already has (`AStratScoreboardHUD*`, `FStratGuidanceView&`).
  - **What each clause pins.** `ApplyViewPushesTheModelsGuidance` — the value reaching the strip
    is the ARGUMENT's `Model.Guidance`, and it deliberately also asserts the strip does *not*
    match a fresh undecorated `BuildViewModel`, which is what a re-query would have produced.
    `UndecoratedRebuildClearsTheStrip` — the sharpest one: an active view applied, then a model
    with no decorate step applied over it; the strip must agree with the LAST model and not the
    union. That is the falsification test for the widget's copy becoming an independent source
    of truth. `ReconcileCompletesWithNoStripConfigured` — the board and units still reconcile by
    set difference with no strip, and no strip is invented; needs no asset.
    `StripHoldsWhatItWasPushed` — `UStratGuidanceWidget::PushGuidance` assigns unconditionally,
    including an inactive push over an active one, reached by a probe that bypasses the HUD so
    a regression names the widget rather than the HUD. `HudForwardsEveryGuidanceShapeUnchanged`
    — see the caveat below. `Stratocracy.StratUI.T-INT-05.GuidanceHasNoBlueprintWriter` —
    neither `PushGuidance` is reflected, `Guidance` is `BlueprintReadOnly` and is exactly
    `FStratGuidanceView`, `GuidanceStrip` is read-only, `GuidanceWidgetClass` is
    `EditDefaultsOnly`.
  - **WHAT `HudForwardsEveryGuidanceShapeUnchanged` DOES NOT PIN, and this is the honest
    limit.** The property asked for was "no `FStratGuidanceView` field is read inside
    `AStratScoreboardHUD::PushGuidance`". An automation test cannot read a function body, and a
    clause that scanned the `.cpp` text would fail on a comment edit and pass on a branch
    written in an unexpected form — a flake, not a gate. What is measured instead is the
    observable consequence: one distinguishable shape per field the struct declares, each
    required to arrive at the strip unaltered. **A branch that reads a field and changes
    nothing survives this clause.** That is not a defect, but a reader must not take the green
    tick as proof the method's source is field-blind.
  - **Technique worth reusing: compare through the struct's own reflection data, never a typed
    field list.** `SameGuidance` is `FStratGuidanceView::StaticStruct()->CompareScriptStruct`.
    A hand-written field-by-field comparison would go silently incomplete the day the struct
    grows an eleventh field, and the clause it feeds would still pass — the exact way a parity
    test stops testing. Same reason `AlterOneField` picks its target by walking
    `TFieldIterator<FProperty>` and REFUSES (with a reason the caller reports) on a property
    kind it cannot vary, rather than skipping it. Side benefit: no `FString`/`FText` comparison
    appears in any assertion, so this file is immune to the case-insensitivity trap;
    `CompareScriptStruct` uses `FProperty::Identical`, which is not case-folding.
  - **The comparator has its own control, `GuidanceComparisonDistinguishesViews`, and it is one
    of the six macros.** It shows `CompareScriptStruct` answering NO once per declared field
    before any clause relies on it answering YES. Without it every clause in the file would be
    green under a comparator that returned true unconditionally.
  - **Fixture caveat: `AStratScoreboardHUD::BeginPlay` is NOT dispatched by these clauses**,
    unlike `StratMatchLifecycle.cpp`'s fixture. `CreateGuidanceWidget` ends in `AddToViewport`,
    which a headless `-nullrhi` run has no viewport for; the strip is installed directly onto
    `GuidanceStrip` — the same member that method assigns. **So nothing here pins
    `CreateGuidanceWidget` itself, nor the `GuidanceZOrder` layering.** Those are the editor
    lane's, and need `WBP_DirectiveStrip` reparented onto `UStratGuidanceWidget` first.
  - **`UStratGuidanceWidget` is `UCLASS(Abstract)` and that ruling was not weakened.** The
    concrete class is `UStratGuidanceRouteDouble` in `Source/StratUI/Tests/`, which adds and
    overrides *nothing* — no `PushCount`, no captured-argument member. A double that recorded
    its own copy would let a clause pass by comparing the double's bookkeeping against itself
    while the inherited `Guidance` property, the one a WBP binds to, was never written.

- **The completion hook now has an OPT-IN, the fixtures declare it in writing, and the trap that
  made it necessary is gated rather than remembered.** The reviewer blocked the previous pass on
  a measured defect: `FStratMatchConfig::SaveSlotName` defaults to `TEXT("StratocracyMatch")` —
  the player's slot — and is never empty, so `NoteMatchResultIfEnded`'s empty-name guard
  protected nothing, and any clause that played a match to a §2.8 result wrote a 2096-byte
  `StratocracyMatch.sav` into a developer's `Saved/SaveGames/` and suppressed §2.11.6's
  onboarding on that machine forever. `StratAiMatchClauses.cpp` did it, and it is not a
  save-system file. The engineer fixed the PREDICATE, not the fixture:
  `FStratMatchConfig::bRecordCompletionOnMatchEnd`, false in C++, gating
  `NoteMatchResultIfEnded` alone — `RecordMatchCompletionOnSave` and `SaveMatchToSlot` stay
  ungated, because a caller that named a slot has already chosen.
  - **What this lane changed.** `StratMatchCompletionRecording.cpp`'s namespace-local
    `MakeConfig` now sets `Out.bRecordCompletionOnMatchEnd = true` beside its slot name, with
    the reason written where a future fixture author will read it: a slot name answers WHERE
    and cannot answer WHETHER, so naming a slot is not consent to be written to.
  - **A clause of this file's own was OVER-DETERMINED and the opt-in line is what fixed it.**
    `T-UI-03.AMatchWithNoResultRecordsNothingOnTheSlot` is the negative arm of the writer gate,
    and between the engineer's fix and this pass it passed for TWO reasons — no result AND no
    opt-in — so it isolated nothing. Confirmed restored rather than assumed: the clause builds
    its config through `MakeConfig` (opt-in now ON) and hands it to `StartMatch`, which copies
    it into `ActiveConfig`; of `NoteMatchResultIfEnded`'s three early returns, the opt-in arm is
    demonstrably not taken and the empty-slot arm is ruled out in-clause by `SlotNameResolves`,
    leaving `!Model.Match.bHasResult` as the single remaining cause of the silence.
  - **`T-UI-03.TheCompletionHookIsSilentWithoutTheOptIn` is the clause that makes the trap
    non-re-armable, and it is about fixtures nobody has written yet.** Until it existed, the
    only thing keeping the defect fixed was a human running `ls Saved/SaveGames/`; no gate
    re-ran that. It plays an AI-vs-AI game to a §2.8 result with the opt-in set to
    `FStratMatchConfig().bRecordCompletionOnMatchEnd` — **read off the module, never written
    down** — and requires that NO slot is created. A later "convenience" flip of the default
    therefore does not leave the clause quietly testing `false`; it turns it red.
  - **Its absence carries its control IN THE SAME CLAUSE, and that placement is the point.**
    "No slot was created" is satisfied for free by a fixture that never reached a result, by a
    scenario that failed to load, by an AI that refused its first turn. Phase A rules out the
    first by requiring `FStratMatchView::bHasResult` — the module's own answer — before it
    interprets anything. Phase B then plays a SECOND AI game on the same subsystem, the same
    slot and the same scenario with the opt-in ON, and requires the slot to appear. The opt-in
    clause at the top of the file is *not* a sufficient control for it: different world,
    different slot, different moment in the process. An absence and its control belong in one
    clause or the control is an assumption about another clause's health. Two AI games in one
    clause is the cost, paid deliberately.
  - **`T-SAVE-06.TheOptInDefaultsOffInCpp` is one line and it is the one that names the value.**
    `FStratMatchConfig().bRecordCompletionOnMatchEnd` is false. Not a duplicate of the clause
    above: that one goes red on a flipped default saying "a slot appeared", which reads as a
    hook defect; this one says what actually changed. **No module-side value exists to compare
    against — the default IS the subject** — so the literal is the reviewer's ruling, recorded
    here as its provenance.
  - The clause count in `StratMatchCompletionRecording.cpp` is now six; the entry below saying
    "four new clauses" was true at its own pass. **[STAMPED 2026-08-21]**
  - Slot hygiene, now a standing rule for this whole suite and not just this file: **any fixture
    that reaches a §2.8 result is a potential writer.** The new clause uses
    `StratocracyAutomation_OptInGate`, its own and shared with nothing, deleted on both ends via
    `FNamedSlotScope`. `Saved/SaveGames/` was empty before the run and empty after it —
    `StratocracyMatch.sav` absent, both automation slots gone.

- **THE OWED CLAUSE IS WRITTEN. `Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn` now
  exists, in its own file: `Source/StratPlay/Tests/StratShippedGameModeOptIn.cpp`.** Its
  prerequisite landed — `Content/StratPlay/BP_StratGameMode.uasset` now carries a `MatchConfig`
  override setting `bRecordCompletionOnMatchEnd` true (blob f1b65f3 -> e45d38a, uncommitted at
  the time of writing), so the debt recorded in the entry below is discharged.
  - **NOT COMPILED AND NOT RUN BY THIS LANE.** The editor was open on the integration tree when
    the clause was written, which locks `UnrealEditor-Strat*.dll` against a build. The clause
    was verified by reading only. Its first execution is somebody else's, and until then its
    status is unmeasured rather than green.
  - **It reads a CDO, not `.uasset` bytes, and that is the whole design.** `StaticLoadClass` on
    `/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C`, then
    `Cdo->MatchConfig.bRecordCompletionOnMatchEnd` required TRUE. The read is EFFECTIVE: an
    asset with no override hands back the C++ default, which is FALSE, which is the defect, so
    it fails. The trap the entry below warned about — "not found, therefore unchanged, therefore
    fine" — is structurally unreachable in this shape.
  - **Every resolution failure is RED, deliberately.** Package does not load, generated class
    unresolved, CDO null, class not an `AStratGameMode` — each fails and returns. A clause about
    a shipped default that cannot find the shipped default has observed nothing, and the state
    it would be hiding is the defect itself. Nothing in it degrades to a skip.
  - **Its own PREMISE is checked first and is fatal:** `FStratMatchConfig()`'s
    `bRecordCompletionOnMatchEnd` must be FALSE. If the member initialiser ever flips, this
    clause would go green on an asset that overrode nothing while still wearing the name. It
    shares a subject with `T-SAVE-06.TheOptInDefaultsOffInCpp` and the duplication is intended:
    that clause reports the flip, this one reports that the flip made this clause blind.
  - **What it does NOT pin.** (a) `BP_StratGameMode_AiVsAi`, which deliberately stays FALSE and
    was byte-identical through this pass — pinning that asymmetry needs a SECOND clause
    requiring FALSE there, never a widening of this one. (b) `SaveSlotName` and
    `AiMaxCommandsPerTurn` on that asset: both measured INHERITED, not overridden, so a clause
    reading them there would silently be a clause about `StratMatchSubsystem.h`. (c) That any
    map's World Settings or `Config/DefaultEngine.ini` actually SELECT this GameMode — it says
    only that IF this GameMode runs the match, the match is recorded. (d) The writer, which is
    `StratMatchCompletionRecording.cpp`'s.
  - **It touches no save slot.** No config is handed to a subsystem, no match is started, no
    §2.8 result is reachable, `UGameplayStatics` is not included. The standing slot-hygiene rule
    for this suite does not apply because there is no path to a writer.
  - **It carries a `/Game/` literal, under the standing automation-fixture exception**, and it
    has to: the asset path is not incidental to the clause, it is the subject. No property or
    bridge call can name "the Blueprint the shipped game points at".

- **OWED CLAUSE — `Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn`. NOT WRITTEN IN THIS
  PASS, DELIBERATELY, AND IT MUST NOT EVAPORATE.** *(Discharged 2026-08-21 — see the entry
  above. **[STAMPED 2026-08-21]**)* The engineer named it and it is his stated
  debt. It would read `bRecordCompletionOnMatchEnd` off `AStratGameMode`'s CDO — through
  `BP_StratGameMode`'s `MatchConfig` default — and require it TRUE.
  - **It would be RED today and correctly so. This is a real shipping defect right now:** the
    shipped game does NOT opt in, because `BP_StratGameMode`'s `MatchConfig` has no such
    checkbox yet, so in the packaged game nothing is ever recorded and §2.11.6's guided opening
    **re-arms for every player, every match, forever**. The C++ default is right; the shipped
    Blueprint default is missing.
  - **UNBLOCKED BY:** the `Content/` default landing — `BP_StratGameMode`'s `MatchConfig`
    carrying `bRecordCompletionOnMatchEnd = true`. That is `strat-editor-builder`'s lane, needs
    the live editor on the integration tree, and cannot happen until this C++ merges. Writing
    the clause now would block its own prerequisite.
  - **THE `.uasset` TRAP, FOR WHOEVER WRITES IT.** A property **absent** from the asset proves
    it equals the C++ default — i.e. **false**, i.e. the defect. So a clause that extracts the
    asset and looks for the property must treat "not found" as a FAILURE and never as
    "unchanged, therefore fine". Reading it off the CDO rather than off the file avoids this
    entirely and is the preferred shape; if anyone reads the `.uasset` bytes instead, note the
    4-character extraction trap recorded in the crew's memory on Blueprint default absence.

- **§2.11.6's WRITER is now falsifiable, and the two clauses that touched the field before this
  pass were not gates on it.** `Stratocracy.StratPlay.T-UI-03.GuidanceIsSuppressedByACompleted-
  MatchOnTheSave` and `.T-SAVE-06.SaveOverASlotPreservesOnboardingState` both PLANT
  `UStratSaveGame::bHasCompletedAMatch` by hand, so both pass identically whether
  `UStratMatchSubsystem::RecordMatchCompletionOnSave` exists, is deleted, or no-ops. Planted
  input is the right shape for a *reader* clause and it is not a gate on a *writer*. Four new
  clauses in `Source/StratPlay/Tests/StratMatchCompletionRecording.cpp`, and **nothing in that
  file plants the field**:
  - `T-UI-03.AMatchReachingAResultRecordsCompletionOnTheSlot` — the rules module's own AI plays
    both sides to a §2.8 result (`RunAiTurnsNow`, exactly as
    `T-INT-05.BothSidesAiReachesAResultWithinTheBound` does), and the slot afterwards reports a
    completed match. The clause never calls the writer. It goes red on the hook being deleted
    from `ApplyView`, on the hook moving to a path this clause does not take, and on the writer
    no-opping.
  - `T-UI-03.AMatchWithNoResultRecordsNothingOnTheSlot` — THE CONTROL, and without it the clause
    above proves only that *something* wrote a true. A hook with its `bHasResult` arm dropped
    would pass the first clause exactly and would suppress onboarding on the first frame of every
    player's first match, which is a worse defect than the missing writer was.
  - `T-SAVE-06.RecordingCompletionPreservesTheSavedMatchText` — the read-modify-write property
    from the writer's side: a `SaveMatchToSlot` text captured off the slot before the game must
    come back byte-identical after the completion write. Red the moment the writer
    default-constructs its payload, which would turn a guidance bug into a save-erasing one.
  - `T-SAVE-06.ASaveOverPreservesACompletionTheWriterRecorded` — the same carry-forward the
    existing save-over clause pins, but on a value the *subsystem* produced rather than one the
    test planted.
- **The anti-vacuity check that makes those four clauses mean anything is about a STRING, and it
  runs first.** `NoteMatchResultIfEnded` returns early **and silently** when
  `ResolveSaveSlotName(FString())` is empty — deliberately, and its own comment gives the reason.
  A clause that forgot to configure `FStratMatchConfig::SaveSlotName` would assert an effect
  against a subsystem that never attempted the write, and would report the writer's absence as
  the writer's silence. So every clause asks the subsystem what an empty name resolves to,
  requires it non-empty **and** requires it to be the test's own slot, as its own check, before
  any effect is asked about. `SlotNameResolves()` in that file is the one copy.
- **What those clauses do NOT pin, said plainly.** (a) That a *human* hot-seat match reaching a
  result records it — the path is the same `ApplyView` and the same model field, but the game
  that reaches a result is played by the AI because that is the only way this suite reaches a
  §2.8 result inside one test; the hook observes a MODEL, not a mover. (b) They were **not** run
  against a build with the hook deleted. What was run, and it is a real two-directional
  measurement inside one clause: `HasCompletedAMatchOnSave` is asserted FALSE after `ApplyView`
  has already run twice with `bHasResult` false, and TRUE after the game reaches a result — same
  subsystem, same slot, same process. "Red before, green after" is **not** claimed for the hook.
- **A control that plays a real match can silently disarm a whole file, now that the writer
  exists.** `StratGuidanceInputGates.cpp` depends on `kAbsentSlotName` NOT existing so the guided
  opening arms; the new AttackTargetHexes control plays an AI-vs-AI match in that file's own
  `MakeConfig`, and a game that reached a result would have had the §2.11.6 hook write
  `bHasCompletedAMatch` into exactly that slot — disarming every clause in the file, on that run
  and every run after. The control now uses `kControlSlotName` and deletes it on both ends.
  **The general rule this is an instance of: any fixture that plays a match to a result is now a
  WRITER to whatever slot its config names.** Every test config in the tree wants checking
  against that.
- **The controller attack arm's unreachability is a MEASUREMENT as of 2026-08-21, and it settled
  in the direction that keeps the attribution gap.**
  `Stratocracy.StratPlay.T-UI-02.TheMarkedInfantryHasNoAttackTargetAtDeployment` calls
  `FStratBridge::AttackTargetHexes` for the marked Infantry on the seeded board: `bOk` **true**
  (asserted separately from emptiness — a refusal also leaves the out-param empty, and reading
  only `Num() == 0` would call "the bridge does not know this unit" a fact about the board) and
  **zero** hexes. The same run reports 0 of 10 answered units with any target at deployment.
  What this does NOT buy: it does not make the overlay clause's zero attributable to the gate —
  knowing the target set is empty anyway means `BuildOverlays` would return empty with or without
  the gate. The prose in `StratGuidanceInputGates.cpp` was rewritten to say exactly that, with
  the withdrawn "THAT BELIEF IS AN INFERENCE" passage kept under `RETRACTED> `.
- **The control for that empty answer could not be taken on the opening position, and how it was
  taken instead is worth reusing.** `Data/ferrum_crossing.json` deploys the two sides at opposite
  ends of the map, so no unit anywhere has a target at deployment and there is no in-fixture
  control. `FindABoardPositionWithALegalAttack` therefore plays a PRIVATE both-sides-AI match in
  its own world with `AiMaxConsecutiveTurns` set to 2, and scans every unit after each batch —
  a single unbounded `RunAiTurnsNow` returns only at the finished position, where nothing is
  legal any more and the control would be mute. Measured on the pass that introduced it: the
  first batch already produced one (`unit 5 of side 0 at 3,4`, one legal target at `4,4`). Every
  batch that ends on the bound logs `STRAT-AI refused phase=handover`, so a clause using this
  helper MUST declare that warning expected — that is the bound reporting itself, not a defect.
- **`FAutomationTestBase::TestEqualSensitive` already exists; do not write a local one.** A
  file-local `TestEqualSensitive(Test, What, A, B)` is *shadowed* by the member inside `RunTest`
  and fails to compile (`C2661: no overloaded function takes 4 arguments`) — which is the lucky
  outcome. Use the engine's three-argument member.

- **A doc block goes stale in vocabulary the sweep does not know, so sweep by CLAIM SHAPE and by
  DIFF, never by topic.** The 2026-08-21 gate caught three live false claims in
  `StratGuidedOpeningClauses.cpp`'s `CapturePipHasLanded` block that my own retraction sweep had
  walked straight past. The reason is worth keeping: **that block shared no vocabulary with the
  phrases I was grepping for.** A phrase sweep is sound only for claims you already know about.
  **The method that replaced it, in four passes, each re-runnable by anyone:**
  - **Pass A — diff-derived topic sweep.** Take the identifier set off the *changed lines*
    (`git diff -U0 <changed files> | grep -oE '\b[A-Za-z_][A-Za-z0-9_]{4,}\b' | sort -u`),
    intersect it with the identifiers my own test files mention, and read every prose hit. This
    is complete with respect to *that* change, and it is derived rather than remembered — the
    version built from memory is the one that misses things.
  - **Pass B — topic-INDEPENDENT claim-shape sweep.** Grep comment lines for negative-existence
    and temporal-state vocabulary rather than for subject matter: `today|currently|no longer|
    there is no|has no|exposes no|carries no|no clause|cannot be|not observable|unreachable|
    is red|is green|WHAT WOULD UNBLOCK|DOES NOT PIN|does not exist|is protected`. This is the
    pass that catches a claim about a topic nobody thought to look for.
  - **Pass C — run-history claims.** `is green|is red|fail before and pass after|was shown to|
    demonstrated|N/N`. A comment must never assert a suite colour.
  - **Pass D — the named claims, confirmed absent as live text**, with `| grep -v 'RETRACTED> '`.
    That filter was UNANCHORED, and is now known to hide live lines that merely mention the
    token — see the 2026-08-21 entry below. The pass as run is recorded here as it was run.
    **[STAMPED 2026-08-21]**
  **What Pass A and B found that the gate had not named:** three more stale claims, listed below.
- **A comment must never assert a suite colour, and one of mine did.** The retraction I wrote
  said the converse clause "is at the bottom of this file **and it is green**" — a claim about a
  suite run, which the next commit falsifies without touching the file. Replaced with the
  clause's PRESENCE, which is checkable by reading the file, plus an explicit note that the
  colour is deliberately not asserted and lives in `global.md`. Same rule as the record files:
  only `global.md` may carry a live figure, and that rule binds prose in `Tests/` too.
- **A fix can plant a stale claim two paragraphs from itself.** When I removed the never-firing
  `"is unset"` expected-message declaration from `StratGuidanceInputGates.cpp`, I left the file
  header saying "no tile meshes AND no input assets, and **both** produce warnings that are
  declared expected in every clause." Half of that became false in the same edit, and the
  helper twenty lines below now contradicted it explicitly. **Lesson: after removing a
  behaviour, grep the file for prose describing it** — the contradiction was inside one file and
  survived a review pass anyway. Corrected rather than marked, because it was never a claim
  worth preserving; it was simply wrong about the log.
- **"The marked Infantry has no legal attack target at its deployment hex" was asserted as fact
  and never measured.** It is load-bearing — the whole unreachability of the controller's attack
  arm and the whole "cannot attribute the zero" caveat rest on it — and nothing in
  `StratGuidanceInputGates.cpp` calls `FStratBridge::AttackTargetHexes`, so no run has ever asked
  the rules module. Now marked as an INFERENCE with its basis stated (§2.13.1's authored lane,
  priced by §2.11.6 at five movement points of Plains to a NEUTRAL factory). **What would settle
  it:** one call to `AttackTargetHexes` on the seeded board, asserted empty — which would also
  turn the attack arm's unreachability from an inference into a measurement. Cheap, in this lane,
  and not done in the comment-only pass it was found in.
- **The `RETRACTED> ` convention's third hole, found the hard way.** Already recorded: an
  executable string is outside its reach, and prose that PARAPHRASES a retracted claim greps like
  one. The third: **a stale claim can be about a subject the convention was never pointed at.**
  The convention makes withdrawn text mechanically separable from asserted text; it does nothing
  to find text that should be withdrawn. Passes A–D above are that second job, and they are a
  different tool.

- **`TestTrue` is a claim, not a question — never use one as a probe.** Cost a red suite on
  2026-08-21, twice in one file, from the same shape: `if (!TestTrue(<precondition>)) { make it
  true; }`. `FStratGuidedOpening` arms lazily inside the first `HandleSelectionEvent`, so before
  any event the gate is legitimately open — the probe recorded a failure and the clause went red
  while every assertion it actually cared about passed. **The rule:** anything that must be true
  first is *done* first, unconditionally, and only then asserted. Recorded on
  `StratGuidanceInputGates.cpp`'s two arming sites.
- **A fixture board spawned by `UStratMatchSubsystem` has a MUTE target overlay until you give
  it one.** `AStratBoardActor::FillOverlay` returns early when the component has no static mesh
  — "Nothing to instance", silent by design — so `GetTargetOverlayCount()` reads 0 for every
  input, forever, on a board with no Blueprint defaults. A clause asserting "the overlay is
  empty" against that is unfalsifiable. `OverlayMesh` is applied at `BeginPlay`, not in the
  constructor, and the subsystem spawns into a world that has not begun play — so the fixture
  sets the property **on the instance** (never the CDO, which would leak into every other
  board-spawning clause) and calls `DispatchBeginPlay`, guarded on `HasActorBegunPlay`. Recorded
  on `StratGuidanceInputGates.cpp`'s `GiveTheBoardAnOverlayMesh`.
- **Beat 2's ring-only retirement is pinned, and the clause is differential on purpose.**
  `T-UI-03.Beat2RetiresOnlyOnAPipAtTheRingedObjective` uses one fixture, one pipped unit and two
  observations between which **exactly one field changes** — that unit's `Hex`. Off the ring,
  beat 2 must stay outstanding *and* keep the line; on the ring, the same pip must retire it.
  **What the form buys, which is the part worth stating:** the two halves fail in opposite
  directions, so a regression to the unqualified "a pip anywhere" reading reddens the off-ring
  half while a fixture that never planted a pip at all reddens the on-ring half. Neither can
  pass by accident. **What is NOT claimed:** no before/after run against the pre-narrowing
  predicate was ever executed by anyone — the off-ring half being red-by-construction against it
  is an inference from a one-line diff. **[CORRECTED 2026-08-21]** — this bullet used to end
  "and neither the clause's comment nor any report says otherwise." That was true of the new
  clause's own header block and FALSE of `CapturePipHasLanded`'s doc block in the same file,
  which still read "where it can be shown to fail before and pass after" — an assertion, in a
  tree where the narrowing had landed, that the demonstration was performed. It was not. The
  gate caught it; all four stale claims in that block are retracted in place now.
  *(Supersedes the earlier entry on this page that recorded the clause as owed.)*
- **`T-UI-02.AttackIsClosedForTheMarkedInfantry` now pins both restrictions, and the limit of
  the second is on the record.** §2.11.6-B names two things — nothing submitted, and targets not
  lit — and the clause used to pin only the first, which looks complete. The not-lit half now
  measures `AStratBoardActor::GetTargetOverlayCount()`. **What it pins:** a non-zero overlay is
  PLANTED and read back (the control — an absence needs one), then required to be zero after the
  gated refresh, so the refresh demonstrably **clears** the overlay rather than never writing to
  it. **What it still cannot attribute:** on `ferrum_crossing.json` the marked Infantry has no
  legal attack target at its deployment hex, so `BuildOverlays` would hand back an empty set with
  or without the gate. The zero is the right observable; it is not evidence the gate caused it.
  Same root cause as the unreachable attack arm below, and unblocking that unblocks this.
- **The controller's attack ARM remains unreachable on the shipped scenario, and that is a
  property of the arm rather than a gap in any fixture.** It fires only when `HandleEvent`
  returns `Attack` for the marked unit *while beat 1a is outstanding* — and beat 1a retires on
  that unit's own move, so it is reachable only from a position where the marked Infantry has a
  legal target **at its deployment hex**. **Needs:** a scenario whose `guidedOpening.infantry`
  deploys adjacent to an enemy — a `Data/` addition, vendored and hash-gated, writable from no
  lane in this repo. *(The overlay half of this item was discharged by the engineer's
  `GetTargetOverlayCount()` wave on 2026-08-21; only this half stands.)*
- **`StratBoardPicking.cpp` no longer identifies the target overlay by elimination.** It used to
  take "the first component that is not the reach overlay and whose count moved" — which took the
  first match without ever asserting there was only one, so a third overlay component would have
  made it silently measure the wrong thing. **Converted 2026-08-21 to
  `AStratBoardActor::GetTargetOverlayCount()`.** What was verified before converting, rather than
  assumed: (1) the accessor returns `TargetOverlay->GetInstanceCount()` and 0 on null — the same
  quantity off the same object, cached nowhere; (2) the STRUCTURAL claim the elimination stood in
  for is not lost but is now asserted **positively** — exactly one component's count moves under
  `ShowTargets`, and it is not the reach overlay, which is strictly stronger than "at least one
  non-reach component moved"; (3) the reach and target sets are deliberately different sizes (3
  vs 1), so a board that had collapsed them into one component could not satisfy both counts.
  The clause is `Stratocracy.StratPlay.T-UI-02.ReachOverlayIsNotComputedHere`.
- **The `RETRACTED> ` sweep found TWO passages in `StratGuidanceInputGates.cpp`, and reading the
  likeliest clause would have found one.** The stale claim ("the target overlay is not
  observable") lived in the file header block's *what this does not pin* list **and** in
  `AttackIsClosedForTheMarkedInfantry`'s own block. This is the second time in two days that a
  claim of one kind occupied more passages than expected — the first was three in
  `StratSelectionMachineParity.cpp`. **The method that works: grep the whole file for the claim's
  distinctive words, then subtract `RETRACTED> `; do not go to the clause that seems most likely
  to hold it.** A third quotation, in `StratBoardPicking.cpp`, was caught the same way, in prose
  paraphrasing the code it had just replaced.
- **`Occurrences = 0` on `AddExpectedMessagePlain` means "any number, but AT LEAST ONE" — it
  does NOT mean "zero or more".** Measured 2026-08-21: three declarations added defensively, for
  warning lines that turned out never to fire, failed four clauses outright with *"Expected
  suppressed ('Warning') level log message or higher matching 'X' did not occur."* So an
  expected-message declaration is a **claim that the line appears** and must be justified like
  any other assertion; a spare one turns a green clause red. The two that did not fire were
  `LoadMatchFromSlot`'s `"Load of slot ... reported:"` (it fires only when the load returns
  false, and the SECOND `StartMatchInternal` in one world returns true) and
  `AStratPlayerController`'s five `"<Action> is unset"` warnings (`SetupInputComponent` never
  runs on an unpossessed controller in a world that has not begun play).
- **`HandleSelectionEvent` returning `true` does NOT mean a command was submitted, and a test
  helper that believes it will silently do nothing.** `EStratSelectionCommand::None` is an
  ordinary outcome — a selection, a cancel and a click on empty ground all produce it — so the
  bool cannot tell "moved" from "selected". Compounding it: a `HexPrimary` on the hex of the
  ALREADY-SELECTED unit **drops** the selection rather than re-selecting. Together those two
  cost three red clauses in one run: a helper that had been handed an already-selected unit
  deselected it, clicked reachable hexes at nothing, and reported success. The rule this lane
  now follows: a helper that claims to submit a command **measures
  `FStratBridge::RecordedCommandCount()`**, and issues a `Cancel` first so its starting state is
  the same however the caller arrived. Both are recorded on
  `StratGuidanceInputGates.cpp`'s `FGateHarness::MoveTheMarkedInfantry`.
- **The `RETRACTED> ` marker convention, and a later sweep may rely on it.** Introduced in
  `Source/StratPlay/Tests/StratSelectionMachineParity.cpp` on 2026-08-21, now also in use in
  `StratGuidanceInputGates.cpp` and `StratBoardPicking.cpp`. Every comment line that QUOTES a
  claim the file no longer makes begins, after its comment leader (`//` or `*`), with the exact
  token `RETRACTED> `; no line carrying it IN THAT POSITION asserts anything. A stale-phrase
  sweep therefore subtracts withdrawn quotations mechanically with an ANCHORED filter,
  `grep -n '<phrase>' <file> | grep -vE '^[0-9]+:[[:space:]]*(//|\*)[[:space:]]*RETRACTED> '`.
  **Why it was needed:**
  six header retractions in this tree quote their own old wording, so a grep for the phrase
  returns mostly correct withdrawals — which is exactly how one LIVE false assertion survived the
  header sweep and had to be caught by the reviewer. It is **per-line, not per-paragraph**,
  because a paragraph marker leaves a sweep guessing where the paragraph ends. **Its one hole,
  stated:** an EXECUTABLE string — an `AddInfo` literal — is outside a comment convention's reach
  and must be deleted or rewritten, not marked; the deleted text is then quoted under the marker
  beside the replacement. **Its second, softer hole:** prose that PARAPHRASES a retracted claim
  is not a quotation but greps like one, so such prose either carries the marker too or is
  written to avoid the claim's distinctive words.
- **The convention's THIRD hole: the published filter was unanchored.** Found and fixed
  2026-08-21 on `feat/retracted-anchor`. The recipe the convention published,
  `grep -n '<phrase>' <file> | grep -v 'RETRACTED> '`, is a SUBSTRING match: it drops every line
  containing the token anywhere, including the prose that DECLARES the convention. Measured at
  `682d17f`, before the fix: **2** live lines hidden in `StratSelectionMachineParity.cpp`
  (the sentence defining the token, and the recipe line itself), **2** in
  `StratGuidanceInputGates.cpp`, **1** in `StratGuidedOpeningClauses.cpp` — **5 across
  `Tests/`**, plus 7 in this file. The task brief that sent me said "five in
  `StratSelectionMachineParity.cpp`"; the tree said two, and the tree wins — five is the
  `Tests/`-wide figure.
  - **The anchored replacement, and the `[0-9]+:` that is easy to drop.** The filter runs
    downstream of `grep -n`, whose output starts with a line number, so an anchor written
    `^[[:space:]]*(//|\*)…` matches NOTHING in the pipeline — measured: 11 of 11 lines survived
    it. In a pipe the anchor is `^[0-9]+:[[:space:]]*(//|\*)[[:space:]]*RETRACTED> `; run
    directly over a file it is the same without the `[0-9]+:`.
  - **The `*` branch is load-bearing, not defensive.** 22 of the 65 marker lines in `Source/`
    sat in block comments behind a `*` leader. **[STAMPED 2026-08-21 — measured at `682d17f`,
    before `f7b934a` added markers to `StratGuidanceInputGates.cpp`.]** Re-derived on
    `feat/retracted-anchor` in slot-1, with that branch's own retractions applied: of the
    **108** lines under `Source/` (`*.cpp`, `*.h`) that mention the token at all, **101** are
    properly marked — **59** behind a `//` leader and **42** behind a `*` leader. Dropping
    `\*` from the alternation moves whole-tree survivors from **7** to **49**, so the `*`
    branch carries 42 lines. The single-phrase control still reads exactly as first recorded:
    on the header `Source/StratPlay/StratBoardActor.h` the phrase `reach overlay` has 3 raw
    hits, **2** survivors under the anchored filter and **3** with `\*` dropped. That phrase
    was not named when this was first written; naming it is what makes the control
    reproducible, and re-running it is how I confirmed the 2-back-to-3 figure rather than
    taking it on trust.
  - **Two rules the declaration now carries that it lacked.** (a) A quotation must occupy WHOLE
    LINES — a line-oriented filter can only subtract a whole line, so a mid-line marker either
    swallows the live half of the line or leaves the quotation live. Measured both ways: with
    the marker mid-line the anchored filter correctly surfaces the line, while the old
    unanchored one silently swallowed it. (b) Sweep phrases must be SUBJECT-BEARING — grep for
    how a claim is PHRASED, not what it is ABOUT; lift the words off the withdrawn lines
    themselves.
  - **What this pass did NOT do.** No automation clause was added, and none should be: the
    subject is a shell recipe published in comment prose, and there is no module-side value for
    a `TestEqual` to read. An assertion here would be a claim about grep, not about the game.
    The evidence for the fix is greps and diffs, run in a drafting worktree that has never been
    built — no suite was run for it, and none is implied. Suite figures live in `global.md`.
  - **The healthy path was run too, because the last guard in this project was tested only for
    its failures.** Over the unmodified tree the anchored filter subtracted 65 of 65 properly
    marked lines, false positives 0. **[STAMPED 2026-08-21 — measured at `682d17f`.]**
    Re-derived in slot-1 on `feat/retracted-anchor`: it subtracts **101 of 101** properly
    marked lines, false positives **0**, and the **7** lines it leaves standing are exactly the
    prose that DECLARES the convention — three in `StratGuidanceInputGates.cpp`, four in
    `StratSelectionMachineParity.cpp`. Every one of those seven asserts something, which is the
    whole reason the filter is anchored; the unanchored form would have swallowed all seven.
- **A comment can argue its own necessity and be WRONG about it — `kControlSlotName`'s did.**
  Found and withdrawn 2026-08-21 on `feat/retracted-anchor` in slot-1.
  `StratGuidanceInputGates.cpp` gives its AI-vs-AI control a save slot of its own, and two
  blocks — the doc comment on `kControlSlotName`, and the block immediately above
  `BothAi.SaveSlotName = kControlSlotName;` in `FindABoardPositionWithALegalAttack` — justified
  that at length: a match reaching a result "now WRITES §2.11.6's completion flag through
  `ApplyView`'s hook", so sharing `kAbsentSlotName` would disarm the guided opening for every
  other clause in the file. **That stopped being true when the opt-in landed.**
  `NoteMatchResultIfEnded` returns early unless `FStratMatchConfig::bRecordCompletionOnMatchEnd`
  is true; the field is declared `= false` in `StratMatchSubsystem.h`, and `MakeConfig` in
  `StratGuidanceInputGates.cpp` never sets it, so `Base` and its `BothAi` copy both inherit
  false and the hook writes NOTHING on that control whatever the slot name is. Measured by
  grepping every occurrence of the identifier under `Source/`: the only assignments of `true`
  are in `StratMatchCompletionRecording.cpp`. Both blocks are now withdrawn under `RETRACTED> `
  rather than smoothed away — they argued in their own voice, so a reader who remembers them has
  to see them retracted rather than find them quietly absent. `kControlSlotName` and
  `FControlSlotScope` STAY: still correct, still cheap, and defence in depth against a future
  fixture in that file that DOES opt in.
  - **What this changes in the inventory:** no clause in `StratGuidanceInputGates.cpp` is
    evidence that the completion hook respects slot separation. It cannot be — the hook never
    fires there. `StratMatchCompletionRecording.cpp` is the only file that opts in and the only
    one that pins it.
  - **The general shape.** A comment explaining why a guard is NECESSARY is a claim about the
    production code, and it goes stale the moment that code gains a gate — silently, because the
    guard it defends keeps working either way and no clause reddens. A claim-shape sweep for
    this one's own wording (`onto whatever`, `hook writes`, `silently disarm`,
    `reaches a result now`, `resolves to`) across all of `Source/` found no other copies; the
    only near-miss, "silently disarm every AI mid-milestone" in `StratAiBridgeParity.cpp`, has a
    different subject and is still true.
- **What the §2.11.6 clause set pins, and what it does NOT.** Written 2026-08-21.
  - The three schedule-table clauses (`CommonCase` / `Wandered` / `FastLane`
    `ReproducesTheScheduleTable`) assert the **rule as well as the beat**, via
    `FStratGuidedOpening::RuleThatSelectedTheLine`. The cells that carry the weight are the
    common case's turn 3 vs turn 4 (same beat, rule 1 then rule 2) and the wandered case's turn
    4 (rule 2 must pick the LOWEST-numbered outstanding beat — beat 2, not the incumbent beat 3).
  - **The schedule table is a literal in the test, and deliberately.** No module-side value
    produces it; `FStratGuidedOpening` IS its implementation, so reading the expectation off the
    machine under test would be self-testing. This is the one shape in which a literal is right.
  - `WindowEndTagRendersOnTurnFourOnly` gets its whole value from the **false** case — the fast
    lane's turn-3 rule-2 last call, shown untagged. A clause checking only turn 4 cannot fail on
    a tag wrongly keyed to `LineRule == 2`.
  - **NOT pinned: the key to `HandleSelectionEvent` binding.** The gate clauses call the method
    directly on an unpossessed controller. `StratPlayerControllerTick.cpp` pins the tick flag
    that silently disables the whole input pipeline; the bindings themselves remain unpinned.
- **No clause asserts on the two log sentences the engineer's 2026-08-21 wave reworded.**
  Verified by grep across every `Tests/` file, not inferred: nothing asserts on beat 2's
  retirement line (now naming the objective hex) and nothing asserts on
  `"slot '%s' carries no §4.10 text"` (the bare-citation fix). Recorded so the next reword does
  not have to re-derive it — and so the next agent knows the check was a grep and not a guess.
- **`T-SAVE-04.LoadRefusesAWrongSavedDataVersion` pins that the previous match SURVIVES, which
  is not what was specified.** The brief asked for `IsMatchLive()` false. The code says
  otherwise and is right: `LoadMatchFromSlot`'s version gate returns **before**
  `StartMatchInternal`, so `TearDownPresentation` never runs. `StratMatchSubsystem.h` records
  "ON A REFUSAL THE PREVIOUS MATCH IS GONE" as a limitation of the refusals INSIDE the sequence;
  this is one of the ones outside it. The clause therefore pins survival — live, same state
  hash, same recorded log — so a refactor that moved the check inside the sequence would turn a
  harmless refusal into a destructive one and say so.
- **`T-SCN-07`, not `T-SCN-02`, is the guided opening's acceptance ID.** The clause on
  `FStratBridge::GuidedOpeningHexes` was proposed as `T-SCN-02` ("structural validity: every hex
  reference is in bounds"), which is a property of the file and already the loader's. `T-SCN-07`
  is "opening-capture naming", and `Source/StratRules/Scenario.good.cpp` refuses every
  guided-opening violation under that id and no other. Reassigned, not minted.
- **The save-slot clauses must never use the configured default slot name.** They write to
  `StratocracyAutomation_*` and delete it on entry AND exit. `FStratMatchConfig::SaveSlotName`
  is `StratocracyMatch` — the player's slot — and two distinct hazards follow from touching it:
  a suite run would destroy a developer's in-progress game, and the input-gate clauses would go
  **silently vacuous** on any machine whose real slot had `bHasCompletedAMatch` true, because
  guidance would be suppressed, every gate would be open, and every clause would pass.
- **A variant scenario for a "different scenario" clause goes in `Saved/`, and drops the
  declared hash rather than authoring one.** `T-SAVE-04.RestoreSourcesScenarioHashFromThisBridge`
  needs a bridge seeded from a scenario other than `ferrum_crossing.json`, and `Data/` is
  vendored and hash-gated. The recipe, in `StratBridgeRestoreParity.cpp`: copy the file, delete
  the whole `"scenarioHash"` LINE — `Scenario.h` reading 4 makes that field optional and DERIVED,
  so the loader recomputes it — and change `"turnCap"`, which `strat::scenarioHash`'s preimage
  carries as `cap=`. Nothing computes or asserts the new hash. The file is deleted on the way out.

- **Phase D's acceptance must not be written against seeing a Tank built.** `strat::chooseBuild`
  (it collects every buildlist entry whose `costFame <= fameTotal`, then reduces that set with
  `buildPriorityLess`) always buys the cheapest *affordable* buildlist entry; with Infantry
  (100 Fame) and Tank (300 Fame) both in `BP_StratGameMode`'s authored `AiBuildlistUnitIds`, the
  Tank entry is unreachable at the rules layer regardless of how many times it repeats — §2.9's
  "an occasional Tank" is not an observable outcome. **Corrected in phase 5's doc pass:**
  `SetBuildlistByIds`'s own doc block in `Source/StratBridge/StratBridge.h` (name it, not a line
  number — the citation had already rotted once) now states this plainly rather than claiming
  duplicates in the buildlist express a ratio; that older claim was true of what the bridge
  preserves and false of what the rules layer does with it. This is a vendored-behaviour observation for
  `E:\MultiAgent\stratocracy-crew`, not a task for this repo. See "Phase C — CLOSED" below for the
  full account.
- **`UStratMatchSubsystem::RunAiTurnsNow`'s return value is an untested production contract —
  phase D's gate must not be built on it.** A §2.8 result reached mid-turn ends with the rules
  module correctly refusing the winning side's closing EndTurn
  (`STRAT-AI refused phase=apply kind=EndTurn ... reason=[T-SAVE-05] no match is running`), so
  `FStratAiTurnRunner::RunTurn` returns `bOk = false` and `RunAiTurnsNow` returns `false` for a
  game that finished *correctly* — result, turn count, recorded log, replay parity and handover
  silence are all right; only the return value misreports. `strat-test-author` asserted neither
  `true` nor `false` (either would be wrong or would demand the defect) and instead left a
  tripwire, `AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ..., Occurrences 0)` at
  `StratAiMatchClauses.cpp:610`, so a future fix to this fails loudly rather than silently.
  Carried into phase D's brief per the phase B gate's explicit instruction — see "Phase B —
  CLOSED" below for the full account.
- **The two clauses owed since phase 6 are now written and green — discharged.**
  `Stratocracy.StratPlay.T-INT-05.WaitIsDistinguishableFromAttack` and
  `.T-INT-05.WaitWithNothingSelectedIsANoOp`, in a new file
  `Source/StratPlay/Tests/StratSelectionWaitClauses.cpp` (untracked at time of writing — staging
  is the user's call). Suite was **69/69** at that pass, `succeeded 69 /
  succeededWithWarnings 0 / failed 0 / notRun 0`, `reportCreatedOn 2026.08.13-16.47.36`
  (`Saved/AutomationReport/index.json`, read `utf-8-sig`); both entries `state: "Success"`.
  **[STAMPED 2026-08-21]** — that figure is history, not a live count; the live count lives in
  `Tools/architect/state/global.md` and nowhere else. All 67 pre-existing tests still pass, none
  downgraded. `WaitIsDistinguishableFromAttack` pins that a wait and an accepted attack differ
  in the machine's *returned value* (`Command == None` naming no unit vs `Command == Attack`
  carrying `UnitId`/`Hex`), that both units end `bDone == true`, that both re-clicks refuse
  identically, and that `FStratBridge::StateHash()` is unmoved by the wait but moved by the
  attack — expectations taken from `FStratSelectionOutcome`, `AttackTargetHexes`'s own first
  element, and `StateHash()` against its own earlier reading; no hex literal, no predicted hash.
  `WaitWithNothingSelectedIsANoOp` pins the guard at `StratSelectionMachine.cpp:156-160`:
  `Command == None`, selection stays `INDEX_NONE`, a refusal reason is set, nothing joins
  `DoneUnits` (every `bDone` false on the decorated model), state hash unmoved, no `STRAT-WAIT
  spent` line emitted. Neither clause asserts the log line's *existence* — only its absence on
  the no-op path and its presence as a positive control on a real wait driven through the same
  capture in the same run; the line was never the property worth pinning, the wait/attack
  distinguishability was.
  - **Technique worth reusing: pinning a refusal string with no module-side accessor.** The
    guard's `TEXT("nothing is selected")` at `StratSelectionMachine.cpp:158` is an inline
    literal exposed by neither `FStratSelectionMachine` nor `FStratSelectionOutcome`. The clause
    does not hardcode a copy — that would fail on a harmless wording edit while still passing if
    the guard were replaced by a different arm producing a different sentence. Instead it
    asserts non-emptiness plus inequality against the machine's own done-set refusal, produced
    by the same machine in the same run.
  - **Technique worth reusing: a log-silence assertion needs a positive control.** Asserting "no
    `STRAT-WAIT spent` line appeared" is worthless if the capture cannot speak, so the clause
    first drives a real wait through the same live `GLog` capture and requires the count to rise
    before reading the silence on the guarded path — the general remedy for the failure mode
    that cost phase 6 six rounds: measuring an absence without first proving the instrument can
    register a presence.
- Standing debt, ruled out of any future phase's scope by the phase-6 reviewer: NeoStack input
  injection reaches `UGameViewportClient::InputKey` but never `UPlayerInput`, so no playtest is
  machine-repeatable. This is a NeoStack plugin issue outside this repository.
- The **owned-bridge-path leak on `AStratScoreboardHUD` is not pinned.** Proving it needs an
  allocation counter behind `FStratBridge` — offered by `strat-gameplay-engineer` during phase 2
  and **declined as new production surface beyond phase 2's exit criterion**; offered again in
  phase 3 and **declined a second time**. The tests prove the *adopted* bridge is not freed
  (`T-UI-03.EndPlayLeavesAnAdoptedBridgeUsable`); `T-INT-05.SubsystemSeedsBeforeHandover` proves
  the *consequence* that the HUD reads the subsystem's bridge and not one it allocated, but
  neither proves the owned path itself. Stated honestly in `StratScoreboardHUDSeam.cpp:31-37`.
  Out of scope for the hot-seat milestone.
- **`ETriggerEvent::Started` on all four input actions is asserted, not measured** — no input
  asset exists yet to test against. Discharged the first time the controller runs in PIE with
  real assets, i.e. no earlier than phase 5.
