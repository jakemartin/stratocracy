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

- **2026-09-06 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- two comment blocks in
  `Source/StratPlay/Tests/StratShellRouteClauses.cpp` and
  `Source/StratPlay/Tests/StratMatchCompletionRecording.cpp`, plus this file -- on `master` in the
  main tree `E:/MultiAgent/Stratocracy`, base commit `c69e519`, and this pass is UNCOMMITTED)** --
  **THE FIRST SWEEP THIS LANE HAS EVER HAD, AND THE HEADLINE IS NOT THE FINDING IT WAS DISPATCHED
  FOR: A `DOES NOT PIN` BLOCK CAN BE PARTLY DISCHARGED, AND "DISCHARGED" IS THE OVER-STAMP THAT
  WOULD HAVE MADE IT WORSE THAN LEAVING IT ALONE.** No exception clause is cited and none applies:
  every file touched is a `Tests/` file or this lane's own record. Cite this pass by its exported
  `reportCreatedOn 2026.09.06-04.29.34`; the pass/fail figure lives in
  `Tools/architect/state/global.md` and nowhere else. When this lane re-ran
  `python Tools/architect/strat_banner_sweep.py` over base `c69e519` on 2026-09-06 it printed
  `SWEEP CLEAN -- no self-contradiction found`; what the sweep says on any later tree is what
  running it says, and this entry does not predict it.
  - **THE DISPATCHED FINDING, RE-DERIVED RATHER THAN ACCEPTED.**
    `StratShellRouteClauses.cpp`'s `WHAT IT DOES **NOT** PIN` block closed *"That discharges on a
    clause reading both Blueprint CDOs, and those assets do not exist yet."* Both assets are
    tracked -- `Content/StratPlay/BP_StratGameMode.uasset` (`d310aa1`) and
    `Content/StratPlay/BP_StratShellGameMode.uasset` (`e4a21b0`) -- and the clause exists,
    `Stratocracy.StratPlay.GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot`. The
    sentence was TRUE at `a6ebbe2` (2026-08-30 19:59) and false four hours later at `e4a21b0`
    (2026-08-31 00:03).
  - **THE PATH IS `Content/StratPlay/`, NOT `Content/Blueprints/`, AND THAT IS WHY THE CONTROL IS
    IN THE STAMP.** `git ls-files --error-unmatch` against the wrong directory returns exactly
    what a genuinely missing asset returns. `Content/StratPlay/BP_ThisDoesNotExist.uasset` was run
    as the negative control before any ABSENT was read as an absence.
  - **PARTLY, NOT FULLY -- AND THIS IS THE ENTRY'S REASON FOR EXISTING.** The stale sentence
    covers *"a Blueprint that OVERRIDES **either** property"*, and `either` has two readings. ONE
    asset overridden and not the other: the two CDOs disagree and the parity clause reddens --
    COVERED. BOTH overridden to the same new string: the two CDOs agree, the parity clause stays
    green, and the C++ derivation `TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn` pins has
    stopped deciding what ships -- NOT COVERED, and deliberately, because
    `StratShellBlueprintSlotParity.cpp` states in its own `does not pin` list that it must permit
    a slot renamed correctly on both assets. `BP_StratGameMode_AiVsAi` is a third asset of the
    first class and is read by neither clause. Stamping this `DISCHARGED` would have replaced a
    stale sentence with a false one, in a file whose whole subject is which residue is left.
  - **TWO MORE TRUE POSITIVES, FOUND BY THE SWEEP AND NOT DISPATCHED, BOTH IN
    `StratMatchCompletionRecording.cpp`, AND ONE OF THEM IS A FALSE CLAIM ABOUT WHAT THE PACKAGED
    GAME DOES.** At the `TheCompletionHookIsSilentWithoutTheOptIn` block: *"It does not, today --
    `BP_StratGameMode`'s `MatchConfig` has no such checkbox yet, so the packaged game's guided
    opening re-arms every match. That clause is owed."* All three sentences are false.
    `Source/StratPlay/Tests/StratShippedGameModeOptIn.cpp` asserts
    `Cdo->MatchConfig.bRecordCompletionOnMatchEnd` TRUE off `BP_StratGameMode_C`'s CDO, and
    `Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn` landed at `7d8185f` on 2026-08-21 --
    **a fortnight before this sweep, and the two files sit in one directory.** At the
    `TheOptInDefaultsOffInCpp` block the heading is still true and only the pointer went stale.
  - **THE CITATION RAN ONE WAY, WHICH IS THE SHAPE WORTH REMEMBERING.**
    `StratShippedGameModeOptIn.cpp` opens by naming `StratMatchCompletionRecording.cpp` and
    explaining why the clause is not in it, and cross-cites `T-SAVE-06.TheOptInDefaultsOffInCpp`,
    which lives there. The discharging file cited its creditor; the creditor never cited back. The
    dispatched finding has the identical shape -- `StratShellBlueprintSlotParity.cpp:13` says
    *"W6's asset tail created `BP_StratShellGameMode` and the debt became payable. This file pays
    it."* while `StratShellRouteClauses.cpp` still said the asset did not exist. **A debt note
    should be read as pointing at a file that may already have been written, and the cheap check
    is `ls` on the neighbouring `Tests/` directory, not a re-derivation.**
  - **WHAT THE SWEEP DID NOT FIND, RECORDED SO A NULL RESULT IS A MEASUREMENT.** 107 tracked files
    under any `Tests/` directory, 83,277 lines. Five shapes, each with a control showing the
    instrument able to speak (an impossible token returned 0; `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
    returned 91 files). Shape 1/5 phrase sweep: 81 candidate lines. Shape 4 bare-quantifier sweep:
    267 raw, 61 after excluding `clause`. Shape 3: 103 candidate lines, of which 48 are
    `*"..."*` verbatim quotes machine-checked against every tracked file in `Source/`,
    `Tools/architect/state/`, `.agents/` and `CLAUDE.md` -- **3 unmatched, all 3 false positives of
    my own instrument** (a case-only difference, a backtick inside the quoted span, and a `*"`
    inside a C++ string literal). **Zero stale verbatim citations.**
  - **AND THE MOST INSTRUCTIVE NULLS ARE THE FOUR CITATIONS THAT *DID* GO STALE THIS PASS AND STILL
    NEEDED NOTHING.** `strat-gameplay-engineer`'s sweep inserted `[READ: THREE]` and
    `[CONDITION MET]` **inside sentences this lane quotes verbatim** --
    `StratShellGameMode.h:165`, `:199`, `:271` and `StratMatchSubsystem.h:368`. Each quoting site
    (`StratShellBlueprintSlotParity.cpp`, `StratShippedOptionsWidgetClassParity.cpp`,
    `StratShippedSoundBankParity.cpp`) already reconciles the discrepancy **in its own next
    sentence** -- *"Both headers say TWO, and they are right about CLASSES and understate the
    ASSETS"*, *"THREE ASSETS AND NOT TWO"*, *"the debt became payable. This file pays it."*
    Stamping them would have been pure over-stamping. Likewise the two
    `**[THE WIDGET NOW EXISTS, 2026-08-23 ... (untracked at the time of writing)]**` stamps in
    `StratProductionMenuSeam.cpp:11` and `StratProductionMenuRouting.cpp:24`: `WBP_ProductionMenu`
    IS tracked now, so a shape-2 sweep flags them -- and *"at the time of writing"*, on a dated
    stamp, is an anchored claim and is still true. **The anchor rule is what tells a real shape-2
    instance from this one, and it is the only thing that does.**
  - **THE JUDGEMENT ASKED FOR, ANSWERED AGAINST THIS LANE'S INTEREST: THE ANCHOR RULE AND THE
    INSTRUMENT RULE TOGETHER COVER FOUR OF FIVE, NOT FIVE.** The engineer's rule -- *a discharge
    condition must name an instrument a reader can run* -- catches shapes 1, 3 and 5, and the
    anchor rule adds shape 2, because a stamp that anchors to its own date cannot itself rot (both
    `WBP_ProductionMenu` stamps above are the worked example). **Neither rule touches shape 4.** A
    bare *"the two GameMode Blueprints"* names no instrument and asserts no moment; it is a
    quantifier with a silent scope, and the only thing that catches it is a reader who counts.
    `StratShellBlueprintSlotParity.cpp` survives its own shape-4 hit purely because it happens to
    declare the exclusion at `:77` -- **that is a house-style habit, not a rule either of us
    stated**, and nothing in this tree would have caught it if the habit had lapsed. Four of five
    is the honest figure; a rule for shape 4 is still owed and I do not have one.
  - **INSTRUMENT NOTE, AND IT IS THE TRAP THE DISPATCH WARNED ABOUT.** The comment-only proof is a
    string/char-literal-aware C++ stripper over `git show HEAD:<file>` versus the worktree,
    remainders diffed: 957 -> 957 and 694 -> 694 code lines, both diffs empty. **A stripper that
    crashes emits nothing on BOTH sides and empty == empty reads as "identical" while a mutation
    control still says the instrument speaks**, so this one RAISES on zero lines rather than
    returning, and that refusal was itself tested against an all-comment input. Two mutation
    controls per file: `return true;` -> `return false;` IS visible, and a comment-only edit is
    NOT.
  - **ONE THING THIS SWEEP COULD NOT SETTLE, AND IT IS NOT A NULL.**
    `StratTitleMusicLifecycle.cpp:132-137` records two debts conditioned on the asset pass --
    *"after the asset pass sets it"* and *"once both assets carry it"* -- for
    `BP_StratShellGameMode`'s `TitleMusic` and its sound-class pairing. `grep -a` finds `TitleMusic`
    in the asset's bytes (control: `SoundBank` 2, an impossible token 0), but **presence in a
    `.uasset` proves nothing about an override** -- `StratShellBlueprintSlotParity.cpp` states the
    rule this lane relies on, that ABSENCE decides and presence does not -- so grep cannot tell a
    set property from a name-table entry. Settling it needs a CDO read, which is a new clause and
    was out of scope for a comment-only pass. **The conditions may already have fired; nothing here
    claims they have not.**
  - **AND THE ELEVENTH INSTANCE OF THE DEFECT THIS ENTRY REPORTS THE SWEEP FOR WAS IN THIS ENTRY,
    THREE PARAGRAPHS UP, ABOUT THAT SWEEP.** The paragraph above read *"`strat_banner_sweep.py`
    exits 1 on this tree with one finding, `REPORT PROVENANCE` on `global.md:33`"*. That was true
    when written and false within the hour: the `coordinator` moved the `global.md` citation, and
    the sweep, re-run here over base `c69e519`, printed
    `SWEEP CLEAN -- no self-contradiction found`. The sentence named no moment -- the exact defect
    the anchor rule four bullets up was written to catch. **This lane's earlier discipline did not
    catch it because that discipline is phrased about FILES THIS LANE DOES NOT OWN, and this
    sentence described a SCRIPT'S OUTPUT, which no lane owns.** The correction is that the anchor
    rule covers both: a claim about a tool's current output is exactly as perishable as a claim
    about a file's current contents, because the tool reads those files. **State what your own run
    did at a named moment and name the command for everything else** -- the durable form the
    2026-09-05 entry below already established, restated here because it was stated about files
    and had to be re-learned about a command. Raised as the second finding of
    `Tools/architect/gate_reports/2026-09-06-regate-stamp-density-round-4.md`. Anything read from
    the file's own line numbers is deliberately absent: the repair moved them.

- **2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one comment block in
  `Source/StratUI/Tests/StratOptionsModelClauses.cpp`, plus this file -- on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `c69e519`, and this pass is UNCOMMITTED)** --
  **THE SAME DEFECT AS THE ENTRY BELOW, ONE FILE OVER, AND THIS TIME THE STALE PART WAS THE
  CITATION AND NOT ONLY THE CLAIM.** No exception clause is cited and none applies: this is a
  `Tests/` file and this lane's own record. Raised as finding 3 of
  `Tools/architect/gate_reports/2026-09-05-content-md-stamp-density.md`; both halves of its
  evidence were re-derived here rather than accepted. No assertion, clause name, fixture or
  include moved -- `git show HEAD:<file> | grep -v '^\s*//'` against the same strip of the
  worktree copy diffs empty, which is the reviewer's own instrument and also refuses a line
  commented out or uncommented. Cite this pass by its exported `reportCreatedOn
  2026.09.06-03.28.13`; the pass/fail figure lives in `Tools/architect/state/global.md` and
  nowhere else. (The report's date is one day ahead of this entry's because the export stamps UTC
  and this record is local -- the already-recorded `reportCreatedOn`-after-20:00 effect, not a
  discrepancy.)
  - **WHAT WENT FALSE.** The `WHAT THIS FILE DOES NOT PIN` block said *"No `WBP_` asset derives
    from `UStratOptionsWidget` yet"* and *"The binder from this delegate to
    `UStratSoundDirector::CommitVolumes` does not exist yet"*. Both are false of the tree.
    `Content/UI/WBP_Options.uasset` carries `/Script/StratUI.StratOptionsWidget` 4x and
    `Default__StratOptionsWidget` once; `Source/StratPlay/StratOptionsPresenter.cpp:189` binds
    `OnAudioOptionsCommitted` to `HandleAudioOptionsCommitted` and `:308` calls
    `Director->CommitVolumes(...)`.
  - **THE ASSET MEASUREMENT NEEDED A CONTROL AND THE REASON IS A TOOL THAT IS NOT HERE.**
    `strings` is not on this box (`command -v strings` prints nothing), so a `strings`-based
    check returns zero lines on a 39 KB `.uasset` and that zero reads exactly like "not found".
    `grep -a` was used instead, with `Content/UI/WBP_TitleMenu.uasset` as the negative control:
    0 for `StratOptionsWidget`, 16 for `Widget`. The control is what separates an absence from a
    mute instrument. Also worth keeping: `grep -a -c` says 2 and `grep -a -o | uniq -c` totals 6
    on the SAME file, because `-c` counts LINES in a binary -- neither figure is wrong and a
    reader comparing them without knowing that will think one of them is.
  - **WHAT THIS PINS AND WHAT IT DOES NOT, WHICH IS WHY ONLY THE PARENTHESES WERE STAMPED.** Both
    bullet HEADINGS are still true and were left untouched: this file constructs no Slate, so it
    pins no pixel, slider or layout, and no clause in it observes a commit reaching a slot. Only
    the appended asides about what does not exist were stamped. The two things that DO pin the
    now-existing halves are named in the stamp so a reader is not left with a bare correction:
    `Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass` for
    the asset, and
    `Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.ThePresenterSeedsThePanelWithoutForwardingACommitAndForwardsARealOne`
    for the near side of the join.
  - **"YET" IS NOT A TIME QUALIFIER, AND THIS WAS THE ONE JUDGEMENT CALL.** Over-stamping is a
    finding in its own right, so the reading was taken deliberately: a bare *"not ... yet"*
    asserts the absence NOW and adds only an expectation of change, with no anchor. The sentence
    `StratOptionsWidget.h`'s own stamp explicitly SPARES -- *"As of this file's first commit ...
    no WBP derives from this class"* -- anchors to a named moment and is still true today. Same
    subject, opposite outcome, and the anchor is the entire difference. **The general form worth
    carrying: a qualifier saves a sentence only if it names a MOMENT; a word that merely signals
    an expectation of future change saves nothing.**
  - **THE NEW LESSON, WHICH THE ENTRY BELOW DID NOT REACH: A CITATION AGES LIKE A CLAIM.** Both
    false sentences rested on the same authority -- *"the widget header says so itself"*, *"again
    the widget header's own statement"* -- and that authority had ALREADY been corrected.
    `git log --reverse -S "WHAT IS TRUE NOW, each part measured rather than asserted" --
    Source/StratUI/StratOptionsWidget.h` returns `c69e519`, and this test file carried no stamp of
    any kind at that commit. So the correction landed at the cited file and did not propagate to
    the site citing it, and a reader who trusted the citation instead of following it got the
    pre-correction answer. **So the CITATION is stamped in the file, not only the claim.** The
    entry below told this lane to describe a condition rather than a status; this pass adds the
    other half -- when a `DO NOT PIN` block cites another file's prose as its authority, that
    citation is itself a present-tense claim about that file's current text and goes stale on the
    same schedule. Neither compiles, no clause asserts either, and `strat_banner_sweep.py` has no
    shape for either, so nothing between the two files will ever raise it.
  - **THE RE-RUN WAS OWED FOR THE MECHANISM THE ENTRY BELOW RECORDS**: a `.cpp` carrying
    `IMPLEMENT_*_AUTOMATION_TEST` moves `newest_test_mtime` and stales the report even when only
    comments moved. Built (`Result: Succeeded`, `Compile [x64] StratOptionsModelClauses.cpp`) and
    re-ran the full headless suite; zero failed, zero notRun, zero succeededWithWarnings, and no
    previously-green clause moved.

- **2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one comment block in
  `Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp`, plus this file -- on `master` in the
  main tree `E:/MultiAgent/Stratocracy`, base commit `c5d2c65`, tree already carrying four other
  lanes' uncommitted files, and this pass is UNCOMMITTED)** -- **A CLAUSE FILE'S "WHAT THESE
  CLAUSES DO NOT PIN" BLOCK CARRIED A CLAIM ABOUT ANOTHER FILE, AND THE OTHER FILE MOVED.** No
  assertion, no clause name, no fixture and no include moved; `git diff -- Source/StratPlay/Tests/`
  is comment lines only. Cite this pass by its exported `reportCreatedOn 2026.09.06-02.19.18`; the
  figure lives in `Tools/architect/state/global.md` and nowhere else.
  - **WHAT WENT STALE, AND IT WAS ONE WORD.** The block said a human at the keyboard is the only
    instrument for whether the options screen is legible or operable, *"which
    `StratOptionsPresenter.h` already records as an **outstanding** discharge for the in-match
    input mode."* Earlier the same day `strat-gameplay-engineer` closed that item; the header now
    reads `DISCHARGED 2026-09-05, IN THE DIRECTION THAT LEAVES THE CODE ALONE`. Both halves were
    verified against the tree rather than taken from the brief or from the gate report: the
    header's discharge text was read in place, and the enclosing function of the mouse-axis
    rejection was resolved by scanning FORWARD from the comment block to the next definition,
    `void AStratPlayerController::Tick(float DeltaSeconds)`, rather than backward -- a backward
    scan from that block lands on `OnToggleProductionMenu` and would have named the wrong function
    in the replacement text.
  - **THE LESSON, WHICH IS THIS LANE'S AND NOT THE ENGINEER'S.** A "what this does not pin" block
    is the most useful prose in a clause file and it is also the only prose in it that makes
    PRESENT-TENSE CLAIMS ABOUT FILES THIS LANE DOES NOT OWN. Nothing compiles it, no clause
    asserts it, and the sweep is blind to it -- the sweep collects suite counts and phase verdicts
    by shape and has no shape for "the header records X as outstanding". So a debt discharged in
    `Source/StratPlay/` falsified a sentence in `Source/StratPlay/Tests/` with no instrument
    between them. **When a clause file describes another file's state, describe the CONDITION and
    not its status** -- "the header names a human report as its discharge condition" would not
    have gone stale; "records it as outstanding" did.
  - **WHAT THE REPLACEMENT ASSERTS, AND THE TWO THINGS IT REFUSES TO.** It keeps the main clause
    intact, because the main clause never went stale: a human at the keyboard is still the only
    instrument for legibility and operability, and that is exactly what these clauses do not pin.
    It then records the discharge as a REPORT and not a measurement -- one human report, no clause
    in this tree can observe an input mode, reversible if a later playtest contradicts it. And it
    says in terms that the discharge settles NOTHING about the separate mouse-axis rejection
    argued above `AStratPlayerController::Tick`, which stands on `bShowMouseCursor = true` being
    load-bearing and is untouched.
  - **THE NEAR-IDENTICAL SENTENCE IN THIS FILE WAS EXAMINED AND LEFT ALONE, and the reason is
    exactly the lesson above.** It reads *"Whether the screen is legible or operable remains a
    human at the keyboard, as `StratOptionsPresenter.h` already records for the in-match input
    mode."* It says the header RECORDS the point; it does not say the point is owed. A header that
    records a discharge still records the point. So it survived the same event that falsified the
    `.cpp`, by one adjective's worth of restraint, which is the whole finding in miniature.
  - **THE RE-RUN WAS OWED EVEN THOUGH ONLY A COMMENT MOVED, and this is the mechanism.**
    `strat_banner_sweep.py`'s REPORT IDENTITY check populates `newest_test_mtime` from `.cpp`
    files carrying an `IMPLEMENT_*_AUTOMATION_TEST` macro and fires when that mtime is newer than
    the report's. It does not read the diff and cannot know the edit was a comment. Touching any
    clause file at all therefore stales the report. Build then suite then the EXPORTED
    `Saved/AutomationReport/index.json` read with `utf-8-sig` -- the log undercounts by one.
  - **A MEASURED CORRECTION TO A STANDING CLAIM ABOUT THE SWEEP'S EXIT CODE.** The brief for this
    pass stated that the sweep exits 0 even when it prints `SWEEP FAILED`. Measured here on the
    run that printed `SWEEP FAILED -- the record contradicts itself or the tree`:
    `python Tools/architect/strat_banner_sweep.py; echo "EXITCODE=$?"` printed `EXITCODE=1`. The
    printed line is still the verdict to READ, and the sweep is not this lane's file to change --
    but "the exit code is 0 on failure" is false as measured on 2026-09-05, and an agent that
    gated on the exit code expecting 0 would have been wrong in the safe direction only by luck.
  - **WHAT THIS PASS DID NOT TOUCH -- STATED AS AN ACT OF THIS LANE AND NOT AS A STATE OF ANOTHER
    LANE'S FILES, AND THAT DISTINCTION IS THE WHOLE POINT OF THE REWRITE.** This pass wrote
    exactly two files: `Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` (comment lines
    only) and this one. It did not write `Source/StratPlay/StratOptionsPresenter.h`,
    `Source/StratUI/StratOptionsWidget.h`, `content.md`, `engine.md`, `global.md`, `data.md` or
    `decisions.md`. That is a fact about this lane's own actions, and no other lane can falsify
    it. Whether those files have since MOVED is not this bullet's to say -- `git diff --stat` says
    it, at the moment it is asked.
  - **AND THE FIRST DRAFT OF THE BULLET ABOVE COMMITTED, IN THIS FILE, THE EXACT DEFECT THE ENTRY
    ABOVE IT DIAGNOSES.** It read that four other-lane files "are byte-identical", that
    `global.md:33` cited a report this pass had staled, and that the sweep carried one remaining
    `REPORT PROVENANCE` finding naming it. **All three were true when written and all three were
    false within the hour**, each falsified by a different lane still working: the four files had
    moved and a fifth (`Source/StratUI/StratOptionsWidget.h`) had joined the pass; the coordinator
    had moved `global.md:33`'s citation forward; and the sweep, re-run here, printed
    `SWEEP CLEAN -- no self-contradiction found` with no findings at all. The entry above says a
    "does not pin" block is the only prose in a clause file that makes present-tense claims about
    files this lane does not own. **The same sentence is true of a RECORD entry, and this lane
    proved it by writing one paragraph later.**
  - **A TIME QUALIFIER WOULD NOT HAVE BEEN A FIX, WHICH IS WHY THE REPAIR IS A DIFFERENT SHAPE.**
    "As of 2026-09-05 they were byte-identical" is unfalsifiable rather than durable: in a session
    where lanes run concurrently the qualifier's window can be ZERO, so the sentence buys its
    truth by describing an instant no reader can return to. The durable form asserts only what
    this lane DID, and points at the instrument for everything else. **A record entry should name
    the command that answers a question about another lane's files, never the answer.**

- **2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- six new files under
  `Source/*/Tests/`, two of them test-only headers, plus one existing test-only header extended,
  plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `374398a` (clean and pushed), and this pass is UNCOMMITTED) -- **THE OPTIONS SCREEN AND ITS TWO
  BUTTONS, AND THE THING WORTH READING IS THAT A FIXTURE PREMISE AND A CLAUSE DEFECT PRINTED THE
  SAME SENTENCE.** Cite this pass by its exported `reportCreatedOn 2026.09.05-22.19.02`; the
  figure lives in `Tools/architect/state/global.md` and nowhere else. That run is the FINAL one
  of the pass and followed the last rebuild; every earlier run of this pass was overwritten and
  is not citable.
  - **THIRTEEN CLAUSES, COUNTED BY MACRO AND NOT BY NAME.**
    `Source/StratUI/Tests/StratOptionsWidgetBindingClauses.cpp` (3, `GATE-AUDIO-SETTINGS`),
    `Source/StratUI/Tests/StratCommandBarOptionsClauses.cpp` (2, `GATE-TITLEMENU`),
    `Source/StratPlay/Tests/StratOptionsPanelDelegateClauses.cpp` (1, `GATE-TITLEMENU`),
    `Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` (3 -- 2 `GATE-TITLEMENU`, 1
    `GATE-AUDIO-SETTINGS`), `Source/StratPlay/Tests/StratTitleMenuOptionsRowClauses.cpp` (1,
    `GATE-TITLEMENU`), `Source/StratPlay/Tests/StratCommandBarOptionsBindClauses.cpp` (2,
    `GATE-TITLEMENU`), `Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp` (1,
    `GATE-TITLEMENU`). Non-clause: the new `Source/StratPlay/Tests/StratOptionsPanelDoubles.h`
    and `Source/StratUI/Tests/StratCommandBarOptionsDouble.h`, and seven planted sub-widgets plus
    a dismiss tally added to the existing `Source/StratUI/Tests/StratOptionsWidgetDouble.h`.
  - **THE IDs FOLLOW THE STEWARD'S THIRD 2026-09-05 RULING AND NOT THE ENGINEER'S PROPOSAL.**
    That proposal was `T-UI-05.CommandBarOptions*`, which would have filed a route-request button
    under the ID that owns the `+X/turn` INCOME LINE -- `decisions.md`'s 2026-08-27 W3 mint-request
    entry says in terms, *"`T-UI-05` covers the `+X/turn` income line"*. `T-UI-03` was refused
    for the same six subjects on the argument this lane had already made for the options model:
    `OptionsButton` draws no value and reads no `strat::UiSnapshot` field, so there is no
    snapshot-fidelity fact for that ID to own. This lane read the ruling against the headers and
    agrees with all six subjects, including the deliberate SPLIT of `UStratOptionsPresenter`
    across both names by facet -- screen-visibility reconciliation and settings-model plumbing
    are two quantities in one class, and `StratOptionsPresenterClauses.cpp` carries clauses under
    both IDs for that reason.
  - **THE FINDING OF THE PASS: A FIXTURE PREMISE AND THE DEFECT IT SITS ABOVE PRINT THE SAME
    SENTENCE, AND ONLY THE PREMISE TELLS THEM APART.** Both command-bar bind clauses failed on
    their first run with *"Expected ... to be 1, but it was 0"* -- which is verbatim the message
    written for the defect they exist to catch, "the bind runs once at startup and the in-match
    Options control is dead on every real match". IT WAS NOT THAT DEFECT. `FBarScope` did not
    call `UWorld::InitializeActorsForPlay`, so a controller spawned into it was never added to
    the world's `PlayerControllerList`, so `UWorld::GetFirstPlayerController` returned null and
    `UStratMatchSubsystem::FindScoreboardHUD` -- whose first step that is -- returned null before
    reaching the binding at all. **The subject under test was never executed and the clause
    reported the exact wording of a real defect.** What separated them was adding two FIXTURE
    premises that assert the subsystem can REACH the bar the way it does in a match (that the
    world's first player controller is the fixture's own, and that its `MyHUD` is the fixture's
    HUD); those are in both clauses now and named FIXTURE so a future red run reads as plumbing
    rather than as a finding. The general form, for the next author: **a clause whose subject sits
    behind two or three null-guarded lookups will report a silent early return in the words of
    the defect, and no message wording fixes that -- only a premise that measures the lookups.**
  - **WHAT EACH CLAUSE PINS, AND WHERE ITS EXPECTATION COMES FROM.** No clause in this pass
    computes an expectation a module-side value could have supplied. The delegate clause reads
    `GetOptionsPanelRequestCount()` and `IsOptionsPanelOpen()` from INSIDE the handler and
    compares them against the same subsystem's reading afterwards -- which is the only place
    `RequestOptionsPanel`'s stated write-then-broadcast ordering is observable. The presenter's
    seed clause compares the panel's `Model` against `UStratSoundDirector::GetAudioSettings()`
    and the forwarded commit against the widget's own `Model`; nothing calls
    `StratBuildAudioOptionsModel` to predict a value. The title-menu clause compares the drawn
    caption against `GetMenuModel()`'s own row found BY PREDICATE, never by index. The shipped
    parity clause is three independent CDO reads and a relation. The only literals anywhere are
    broadcast/show COUNTS, which are the properties themselves and are asserted as deltas on a
    reading taken first.
  - **WHAT THIS PASS DOES NOT PIN, RECORDED BECAUSE THE CLAUSE NAMES CANNOT SAY IT.**
    - **`UStratOptionsWidget::bSyncingBoundWidgets`'s RE-ENTRANT PATH IS NOT REACHABLE AND IS NOT
      PINNED.** The guard exists so a `USlider::SetValue` that broadcast `OnValueChanged` could
      not turn a sync into a commit. Measured on this tree: UMG's `SetValue` does NOT broadcast
      -- Slate fires that delegate on user interaction only -- so the early return cannot be
      driven headlessly, and no plant makes it reachable, since the plant would have to be a
      broadcasting `USlider` subclass this project does not have. **A guard deleted outright
      stays green.** What IS pinned is the property it protects: a sync commits nothing, and a
      setter commits exactly once. Stated in the file too.
      **[SUPERSEDED 2026-09-05 -- THE "NOT REACHABLE" HALF IS FALSE; SEE THE MUTANT-BATTERY ENTRY
      BELOW. The claim stays here because the way it was wrong is the useful part: it reasoned
      about `SSlider` and the widget under test is the `U`.]**
    - **NO CLAUSE PROVES A PIXEL.** `AddToViewport` in a `-nullrhi` run reaches no game viewport
      and says so at Warning; that warning is now DECLARED with `AddExpectedMessage(...,
      Occurrences 0)` in both presenter clauses rather than tolerated, which also makes it
      evidence that `ShowPanel` reached its last observable step. Whether the screen is legible
      or operable remains a human at the keyboard, as `StratOptionsPresenter.h` already records
      for the in-match input mode.
    - **`HandleCommandBarOptionsRequested` GOING THROUGH `AStratPlayerController::RequestOptionsScreen`
      RATHER THAN STRAIGHT TO `ExecuteRoute` IS NOT PINNED.** Both routes move the same counter,
      so the bind clauses are green under either. That choice is argued on the click cue in the
      handler's own declaration and nothing in this suite holds it.
    - **THE `bEnabled` ANSWER IS NOT PINNED, ONLY THE COPY.** `RefreshOptionsRow` copies
      `Row->bEnabled` across without a branch; WHAT that answer should be is `IsRoutePermitted`'s
      and `StratShellRouteClauses.cpp`'s.
    - **NO ASSET-SIDE CHILD NAME IS PINNED.** `OptionsButton` and `OptionsLabel` on
      `UStratShellMenuWidget`, and `OptionsButton` on `UStratCommandBarWidget`, are all
      `BindWidgetOptional`, so nothing enforces them at Blueprint compile time. Only the three
      GameModes' `OptionsWidgetClass` is checked as an asset fact.
  - **AND THE `nullptr` HALF OF THE PARITY CLAUSE IS THE HALF THAT MATTERS.**
    `OptionsWidgetClass`'s C++ default is `nullptr`, so THREE UNSET GAMEMODES AGREE PERFECTLY --
    an agreement-only clause would have gone green on the state this milestone started in. Each
    of the three is therefore asserted non-null in its own right, which
    `StratShellBlueprintSlotParity.cpp`'s precedent did not need because `SaveSlotName`'s C++
    default is a correct value. Its header also records the 2026-09-05 ENSURE HAZARD: loading a
    GameMode CDO compiles the widget Blueprints its defaults reference, and an uncompiled saved
    widget ensures on load -- an automation ERROR, attributed to whichever clause loaded the
    asset first, which is how a sound-bank clause went red for a widget reason.

- **2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- four files under
  `Source/*/Tests/` and one test-only header, plus this file -- on `master` in the main tree
  `E:/MultiAgent/Stratocracy`, base commit `089c79c`, over the UNCOMMITTED audio-milestone work,
  and this pass is UNCOMMITTED too) -- **THE TITLE-MUSIC AND VOLUME-SCREEN CLAUSES, AND THE
  THING WORTH READING IS THAT AN `AddExpectedMessagePlain` IS NOT A POSITIVE CONTROL THAT CODE
  RAN.** Cite this pass by its exported `reportCreatedOn 2026.09.05-15.21.15`; the figure lives
  in `Tools/architect/state/global.md` and nowhere else. That run is the FINAL one of the pass
  and followed the last rebuild; every earlier run of this pass was overwritten and is not
  citable.
  - **THE FILES, counted BY MACRO.** `Source/StratPlay/Tests/StratShellOptionsRouteClauses.cpp`
    (2 `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, `GATE-TITLEMENU`),
    `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` (1, `GATE-TITLEMENU`),
    `Source/StratPlay/Tests/StratAudioVolumeClauses.cpp` (4, `GATE-AUDIO-SETTINGS`),
    `Source/StratUI/Tests/StratOptionsModelClauses.cpp` (2, `GATE-AUDIO-SETTINGS`), plus the
    non-clause double `Source/StratUI/Tests/StratOptionsWidgetDouble.h`.
  - **THE ID SPLIT FOLLOWS THE STEWARD'S 2026-09-05 RULING AND NOT THE ENGINEER'S HANDOFF,
    which proposed `T-UI-03` for four of these clauses.** `T-UI-03`'s defining text is *"no
    widget-side arithmetic; every number a widget draws equals exactly one `strat::UiSnapshot`
    field"*. `FStratAudioOptionsModel` is built from three floats and NO `UiSnapshot` -- a
    player's gain is not a rules fact -- and `StratBuildAudioOptionsModel` multiplies by a
    hundred and rounds. Filing these under `T-UI-03` would have put clauses under an ID whose
    own sentence their subject visibly breaks, and would have quietly widened that ID from a
    statement about the rules/widget JOIN into a general rule about widgets. The steward's split
    is adopted unchanged and this lane agrees with it.
  - **WHAT EACH CLAUSE PINS.**
    - `GATE-TITLEMENU.TheOptionsRouteDoesNotExitTheProcess` -- a PARTITION over every route the
      reflected `EStratShellRoute` declares: exactly one of `RouteTravels` /
      `RouteExitsProcess` / `RouteOpensOptions` holds for each, the exiting one is `QuitGame`
      and the opening one is `Options`. It is shaped as a partition because the single
      assertion it replaces (`RouteExitsProcess(Options) == false`) is green against a
      predicate that returns false for everything, which would leave the Quit button inert.
      NOTE WHAT IT CANNOT SAY: `UKismetSystemLibrary::QuitGame` is unobservable under
      `-nullrhi`, so no clause pins that the process does not exit -- only that `Options` is
      not classified as the exiting route.
    - `GATE-TITLEMENU.ExecuteRouteOptionsRequestsThePanelOncePerCall` -- the first granted arm
      of `ExecuteRoute` any clause has executed end to end. Counted as a DELTA on
      `GetOptionsPanelRequestCount()` twice, with a stale `ArmPendingLoadSlot` proving the arm
      is local and not a travel.
    - `GATE-TITLEMENU.TheTitleTrackIsReleasedWhenTheShellMapEnds` -- `BeginPlay` spawns no
      handle with no track and a non-self-destroying handle playing the configured track with
      one; `EndPlay`, reached by destroying the actor, stops it, destroys it and nulls the
      member.
    - `GATE-AUDIO-SETTINGS.ApplyVolumesRecordsEveryDisposition` -- one record per call on all
      five dispositions, gains through the module's own `StratClampVolume`, and
      `ChannelsApplied` bracketed at 3 / 1 / 0 by three fixture banks so a constant is visible.
      THIS IS THE CLAUSE THAT KEEPS THE OTHERS NON-VACUOUS: a single early `return` at the top
      of `ApplyVolumes` empties them all at once.
    - `GATE-AUDIO-SETTINGS.AMissingSettingsSlotAppliesUnityAndStillApplies` -- unity gains AND
      a non-zero `ChannelsApplied` AND `bSettingsCameFromSlot == false`. The channel count is
      what separates "applied unity" from "did nothing"; the flag is what separates "chose
      unity" from "never chose".
    - `GATE-AUDIO-SETTINGS.CommittedVolumesRoundTripThroughASlot` -- a SECOND, cache-free
      director reads the slot the first one wrote. Every expectation is the writing director's
      own settings object; the only literal is the out-of-range input handed to the clamp.
    - `GATE-AUDIO-SETTINGS.AbsentAudioSlotLoadsWithoutTouchingTheFile` -- the regression that
      made 109 clauses log `Failed to read file` before the `DoesSaveGameExist` guard landed.
    - `GATE-AUDIO-SETTINGS.OptionsModelTextRendersItsOwnClampedField` and
      `.SeedingTheOptionsScreenCommitsNothing` (`StratUI`) -- the text equals what the module
      renders from the model's OWN stored field, with the rounding proven to be a display
      decision that does not move the saved gain; and `PushAudioOptions` writes the model and
      broadcasts nothing while each of the three setters broadcasts exactly once.
  - **AN `AddExpectedMessagePlain` IS NOT EVIDENCE THAT THE CODE RAN, MEASURED. [INSTRUMENT
    CAVEAT]** The title-music clause's first shape inferred "`BeginPlay` ran" from an
    `AddExpectedMessagePlain(..., Occurrences = 1)` over the refusal `BeginPlay` logs, reasoning
    that an unfulfilled expectation fails the clause. It does not, in this engine: the run
    reported the three assertion failures and NO unfulfilled-expectation error, with the
    declaration never matched. Any clause in this tree using an expected-message declaration as
    proof that a path executed is resting on nothing. `HasActorBegunPlay()` is the instrument
    that cannot be satisfied by silence, and is what the shipped clause asserts.
  - **AND THE DIAGNOSIS THAT CAVEAT PRODUCED WAS ITSELF WRONG ONCE.** The first failure was
    diagnosed as "the headless run has no audio device". Measured instead:
    `DIAG UseSound=1 allowAudio=1 worldBegunPlay=0 actorBegunPlay=0 audioDevice=1` -- the audio
    device was there the whole time; `FinishSpawningActor` dispatches `BeginPlay` only when the
    WORLD has begun play, and a world built by `UWorld::CreateWorld` + `InitializeActorsForPlay`
    has not. `AActor::DispatchBeginPlay()` is the fix, and
    `Source/StratPlay/Tests/StratBuildPulseDrawClauses.cpp` was already using it.
  - **A FINDING IN `StratShellGameMode.h`'s OWN RATIONALE, MEASURED AND NOT ARGUED.** That
    header justifies the `SpawnSound2D` shape by saying the component is *"owned by the audio
    device rather than by this actor's component list, so `EndPlay` must stop it"*. Measured:
    `DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1` --
    outer AND owner are the spawning actor, and it is that actor's only owned component. Actor
    teardown WOULD reach it, so `EndPlay`'s `DestroyComponent()` is belt-and-braces rather than
    the only thing preventing menu music under a match. THE BEHAVIOUR IS UNAFFECTED and the
    clause is green; it is the header's stated reason that is false, and the clause asserts the
    measurement rather than the claim, so the day `SpawnSound2D` changes, a reader is told
    there. Prose in `Source/StratPlay/StratShellGameMode.h` is the engineer's lane and is
    untouched by this pass.
  - **NO MUTANT WAS RUN, AND THAT IS A LANE RULE RATHER THAN AN OMISSION.** The dispatching
    task asked for each clause's named mutant to be run IN PLACE in this tree. Every one of
    them requires editing a file outside `Source/*/Tests/`, which the user's 2026-08-14 ruling
    forbids this lane *"for any reason ... explicitly including temporary edits the agent
    intends to revert"* -- a ruling made about this exact practice by this exact agent, and
    reinforced here by `git worktree list` showing seven other worktrees on this repository, so
    a revert's safety would again rest on facts this lane does not control. The routes that
    ruling names are a structural control inside the clause, a copy of the tree, or a
    serialized phase with the lane owner. EVERY CLAUSE ABOVE CARRIES ITS STRUCTURAL CONTROL --
    named in each clause's own block -- and the exact one-line mutant for each was handed back
    to the dispatching seat rather than executed here. UNTIL THOSE MUTANTS RUN, NO CLAUSE ABOVE
    IS RECORDED AS PROVEN FALSIFIABLE; a structural control shows the fixture reached a
    meaningful state, which is not the same claim.
  - **ONE CLAUSE'S CONTROL IS A REAL DIFFERENTIAL AND IS WORTH REUSING.**
    `AbsentAudioSlotLoadsWithoutTouchingTheFile` runs the PRE-GUARD CODE ITSELF as its positive
    control: an unguarded `UGameplayStatics::LoadGameFromSlot` on the same absent slot, inside
    the same live `GLog` capture, must produce at least one line naming the slot, and the
    director's guarded path must produce zero. The mutation and the subject run in the same
    clause in the same process, so no edit to production code is needed to show the instrument
    can speak. The capture overrides `CanBeUsedOnMultipleThreads()` -> true; without it the
    project's measured 1-in-4 flake returns.
  - **FIXTURE SLOTS ARE NAMED AND DELETED.** Every director in these clauses is handed
    `UseAudioSettingsSlot("StratAudio_Fixture_*")` before anything can call `GetAudioSettings`,
    because an unset slot name resolves to the REAL `StratocracyAudio` file. `Saved/SaveGames/`
    held only the pre-existing `StratocracyMatch.sav` after the final run: no audio slot was
    created by this pass, developer's or fixture's.
  - **WHAT THESE CLAUSES DO NOT PIN.** That the shipped `DA_StratSoundBank` carries `BaseMix`
    or the three sound classes -- unset today, so that clause belongs after the asset pass, and
    every bank here is a fixture built inside the test file. That the mix TREE above
    `SCL_Strat_SFX` is shaped as intended: no clause reads a parent class. That the title
    track's Sound Class is the bank's `MusicSoundClass`. That `OnAudioOptionsRefreshed` fires --
    it is a `BlueprintImplementableEvent` and C++ cannot observe one; what is observed is the
    model the widget writes before firing it. That any options surface is drawn: nothing binds
    `OnAudioOptionsCommitted` and no `WBP_` derives from `UStratOptionsWidget` yet.
  - **NO CLAUSE WAS WRITTEN FOR `PresentButUnparseableSlotStillAppliesUnity`, ON THE ENGINEER'S
    OWN MEASUREMENT.** A 14-byte non-savegame file reads fine and the header parse inside
    `LoadGameFromMemory` returns null without a word, so the loud case is an UNREADABLE file and
    not unparseable bytes. A clause there would go red the day that silence is discharged, which
    is the right time for it to speak and not before.

  - **PROSE CORRECTION, 2026-09-05 (later the same day), AFTER THE ENGINEER RAN THE NINE
    MUTANTS. Comments only -- NO COMPILED BYTE CHANGED in either file, so no suite run was made
    for this correction and none is owed; a rebuild would have produced an identical binary.**
    Two false statements were reported against this lane and a third was found while checking
    whether they had spread. All three are RETRACTED IN PLACE rather than reworded, on the
    engineer's own reasoning: each had travelled, so a reader holding one must land on why it is
    wrong.
    - **(1) THE OWNERSHIP PREMISE, in `StratTitleMusicLifecycle.cpp`'s header.** It stated as
      fact that `SpawnSound2D` hands the actor a component *"the AUDIO DEVICE owns rather than
      this actor's component list ... not in `OwnedComponents`"*, which this file's OWN assertion
      falsifies (`DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1
      ownedCount=1`). Three sites in that one file carried it -- the header, a second copy in the
      comment immediately above the assertion that disproves it, and a quotation of
      `StratShellGameMode.h`'s now-`RETRACTED>` sentence that did not say it had been retracted.
      All three fixed. **HOW FAR IT SPREAD, measured rather than assumed:** `grep` over all of
      `Source/` for `OwnedComponents`, `owned by the audio device` and `audio device rather than`
      returns exactly three hits, all now retraction blocks -- the header's, and this file's two.
      It reached no other test file.
    - **(2) THE EXPECTED-MESSAGE EXPLANATION in `StratAudioVolumeClauses.cpp` WAS CORRECT AND IS
      NOT WEAKENED. THE FILE THAT DISPUTED IT WAS THE ONE IN ERROR.** That block says an
      `AddExpectedMessagePlain(..., Occurrences = 1)` *"is itself the assertion that the control
      fired"*. Checked against UE 5.8's `Runtime/Core/Private/Misc/AutomationTest.cpp` rather
      than against either file's prose: `HasMetExpectedMessages` raises the unfulfilled error on
      `ExpectedNumberOfOccurrences > 0 && Expected != Actual`, so the declaration IS an
      assertion. What was missing was its precondition, now stated in both files.
    - **(3) THE ACTUAL FALSE CLAIM WAS THIS LANE'S OWN GENERALISATION**, in
      `StratTitleMusicLifecycle.cpp`: *"an `AddExpectedMessagePlain` is not a positive control
      that the line was emitted in this engine, whatever its `Occurrences` argument says."* It
      generalised from ONE red run, and **two other clauses in this same directory had already
      measured the opposite** -- `StratGuidanceRouteClauses.cpp` records a clause turned red by
      an unfulfilled declaration, and `StratHotSeatReplayParity.cpp` records the same. Retracted
      in place, and the mechanism that reconciles all three is now recorded in both files:
      `FAutomationTestFramework::InternalStopTest` computes `bTestSuccessful = bTestSuccessful
      && !HasAnyErrors() && HasMetExpectedMessages()`, and **`&&` SHORT-CIRCUITS**, so on a
      clause that is ALREADY FAILING the only site that raises the error is never called.
    - **THE RULE THIS LANE NOW HOLDS, at the width it was measured at: an
      `AddExpectedMessagePlain` with `Occurrences >= 1` (and `Occurrences 0`, which is
      "at least one") IS an assertion that the line fired -- ON A CLAUSE THAT WOULD OTHERWISE
      PASS. It is NOT a diagnostic you may read on a run that is already red.** That is the one
      state in which the engine does not check it, and it is exactly the state the title-music
      clause's first shape was in when it inferred `BeginPlay` had run. The remedy stands
      unchanged: that clause asserts `HasActorBegunPlay()`, which holds on a red run too.
    - **WHAT DOES NOT NEED A FIX, checked rather than assumed.** No clause's CORRECTNESS rested
      on either false premise -- only prose did. The title-music clause asserts the MEASUREMENT
      (`GetComponents().Contains(Component)` is true), not the retracted premise; the volume
      clause's control is double-bound and the binding mutant 7 reddened is `ControlHits > 0`,
      read straight off the capture and independent of the declaration. Five other files
      (`StratSoundDirectorCallSite.cpp`, `StratAiPlaybackClauses.cpp`, `StratAiMatchClauses.cpp`,
      `StratGuidanceRouteClauses.cpp`, `StratHotSeatReplayParity.cpp`) reason about
      `Occurrences 0` correctly and were left untouched; they carry the same unstated
      precondition, which is now stated in the two files above and is not false anywhere.
    - **AND THE LESSON THAT IS NOT ABOUT THIS ENGINE.** A measurement taken on a RED run
      measures the failure path, and the failure path in this framework is not the same code as
      the passing path. Before generalising from one, grep this directory for a clause that
      already measured the same mechanism -- two had, and either would have caught it.
    - **FOURTH-ROUND ADDENDUM, 2026-09-05 (later still), `strat-test-author` (ACTING and
      WRITING; IN LANE -- `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` and this file
      only, on `master` in `E:/MultiAgent/Stratocracy`, base commit `089c79c`, uncommitted).
      **THE PARAGRAPH ABOVE SAYS "THREE SITES IN THAT ONE FILE CARRIED IT ... ALL THREE FIXED".
      THERE WERE FOUR, AND THE FOURTH WAS THE FILE'S OPENING PARAGRAPH.** The re-gate
      (`Tools/architect/gate_reports/2026-09-05-audio-milestone-final-gate-options-and-title-music-regate.md`,
      `VERDICT: BLOCK`) charged it: the header still read *"nothing frees the component on its
      own"*, the VERBATIM claim charged in `StratShellGameMode.h` the round before and fixed
      there, sitting in the same header block, above this file's own `RETRACTED>` block that
      measures it false.
      - **WHY IT SURVIVED THREE ROUNDS IS THE PART WORTH CARRYING.** Not that no instrument saw
        it. The engineer's own sweep shape --
        `nothing (else )?(will |would |can )?(free|frees|release|releases|destroy|destroys)` --
        MATCHES THAT LINE WITH BOTH OPTIONAL GROUPS EMPTY. The instrument spoke and a summary
        collapsed it. Separately, the round before that treated the compiled assertion string as
        *"its one loose string"*, having read the assertion and not the file header.
      - **AND THIS LANE'S RETRACTION BLOCK UNDER-COUNTED ITS OWN SPREAD.** It enumerated three
        destinations -- `StratShellGameMode.h`, this file's assertion below, a dispatch brief --
        and omitted the paragraph directly above itself. **An enumeration of where a false
        premise travelled is itself a claim, and a low one hides work from every reader who
        audits the spread against the list and stops.** Corrected to four in place.
      - **THE FIX'S SHAPE: TRUE READ IN ISOLATION, because a reader landing on the header never
        reaches the retraction.** The paragraph now says what `bAutoDestroy=false` actually
        removes (the ENGINE'S own release, and only that one), says plainly that this is NOT the
        claim that nothing else could free the component, states the measured answer -- the
        actor's teardown WOULD reach it -- and hands the reader to the block below. ~~The file
        still has exactly ONE `RETRACTED>` block; a second would just be another retraction a
        reader must reach to learn the truth.~~
        - **THAT STRUCK SENTENCE WAS FALSE WHEN WRITTEN, AND IT IS STRUCK RATHER THAN REWORDED
          BECAUSE IT WAS QUOTED OUT OF THIS FILE** -- into the re-gate report and into a dispatch
          brief -- and a reader arriving by either quotation must land on why it is wrong.
          **`StratTitleMusicLifecycle.cpp` HAS TWO `RETRACTED>` BLOCKS, AND THIS LANE WROTE BOTH,
          so the number can be checked against a list instead of taken on faith:**
          - `:30-32` -- the OWNERSHIP PREMISE (*"hands this actor a component the AUDIO DEVICE
            owns rather than this actor's component list: it is not in `OwnedComponents`, so
            actor teardown does not touch it"*), falsified by this file's own `DIAG
            outer=... owner=... registered=1 ownedCount=1`.
          - `:73-75` -- the `AddExpectedMessagePlain` GENERALISATION (*"is not a positive control
            that the line was emitted ... whatever its `Occurrences` argument says"*), which the
            third-round bullet **four bullets above, in this same entry** already records as
            *"Retracted in place"*. The sentence contradicted a fact its own entry states.
          - Measured by `grep -n 'RETRACTED>'` over that file: NINE hits -- six are those two
            three-line blocks; the other three (`:38`, `:318`, `:338`) are prose references to a
            block rather than blocks.
          **WHAT THE FOURTH-ROUND FIX ACTUALLY DID is the true claim the struck sentence was
          reaching for: it added NO NEW BLOCK.** The header paragraph was rewritten to be true
          read in isolation instead of being retracted, so the count did not move -- from two to
          two, not from one to one. **A count is checkable, and this one was wrong in the
          countable direction: the file holds MORE of the thing than the sentence said, which is
          the direction that hides work from anyone auditing the file against it.** It is the
          same defect one scale down as the enumeration two bullets above, which this entry had
          already named and then committed again in its own summary.
      - **THE COMPILED ASSERTION STRING IS DELIBERATELY UNTOUCHED AND STAYS AN OBSERVATION.**
        `TestFalse(TEXT("the component is not self-destroying, so EndPlay is the only thing that
        can release it"), Component->bAutoDestroy)` is frozen on this lane's stated grounds: it
        is a compiled byte, and freezing it is what lets a prose pass move nothing UHT or the
        linker sees. It is an observation of `bAutoDestroy`, which is what the clause asserts;
        the sole-custody gloss in its message is retracted in the prose above it and in the
        header, not edited into the string.
      - **SWEEP COVERAGE, not hits: 97 files (every `.cpp` and `.h` in every `Tests/` directory
        anywhere under `Source/`), seven regex shapes, run after the edit.** *(The gloss here
        read "under `Source/*/Tests/`" until 2026-09-05; that glob resolves to 96 -- 18 + 48 + 30
        -- and misses the 97th, `Source/Stratocracy/StratData/Tests/`, which is one level deeper.
        The FIGURE was right and its description was narrower than it; corrected rather than
        struck, since no reader could have audited fewer files by following it.)* The engineer's
        shape (a) returns **ZERO**
        across all 97. Six further shapes -- `no/nothing ... free|release|destroy|reach|touch
        ... component|handle|it`, `only|sole|one place ... EndPlay|release|free|destroy`,
        `audio device ... own(s|ed)`, `not in|is not one of|outside ... OwnedComponents|component
        list`, `teardown ... does not|cannot|never`, and `bAutoDestroy` -- return no unretracted
        site anywhere in the lane. Every surviving hit in this file is inside a `RETRACTED>`
        block, is the frozen assertion string above, is the true narrow claim that the member
        going null is a thing only `EndPlay` does, **or is one of the rewritten-true sites and
        flagged quotations this pass produced** -- the header paragraph at `:9`, `:40` and `:43`,
        the comment above the assertion at `:302`, and the two places (`:311`, `:315`) that quote
        a retracted sentence while saying in the same breath that it is retracted. *(This
        enumeration named only the first three categories until 2026-09-05 and so did not account
        for every hit it claimed to; the SWEEP VERDICT -- no unretracted site -- is unchanged, and
        each added site was re-read to confirm it.)* **`audio device ... owns` is zero
        lane-wide.**
        **NO FIFTH SITE EXISTS IN THIS LANE.** `StratShellGameMode.h` is the engineer's file and
        was not read for edits here.
      - **COMMENT-ONLY, AND A SUITE RUN WAS OWED ANYWAY -- THE OPPOSITE OF THE ENTRY ABOVE.**
        Every changed line begins `//`; both edits replaced comment-only spans inside the header
        block, so no compiled byte moved. But `strat_banner_sweep.py`'s REPORT IDENTITY check
        derives `newest_test_mtime` from `.cpp` files carrying an `IMPLEMENT_*_AUTOMATION_TEST`
        macro, and this file is one -- so touching it reddens the sweep until the report is
        newer. Rebuilt (`Result: Succeeded`, 9 actions) and re-ran the suite;
        `GATE-TITLEMENU.TheTitleTrackIsReleasedWhenTheShellMapEnds` reports `state: "Success"`.
        The live figure and the new `reportCreatedOn` are in
        `Tools/architect/state/global.md`, which is the only file that may state them.

      - **FIFTH-ROUND ADDENDUM, 2026-09-05 (later still), `strat-test-author` (ACTING and
        WRITING; IN LANE -- THIS FILE ONLY, on `master` in `E:/MultiAgent/Stratocracy`, base
        commit `089c79c`, uncommitted). RECORD CORRECTION ONLY: NO SOURCE FILE WAS TOUCHED, SO
        NO COMPILED BYTE MOVED, AND NO BUILD AND NO SUITE RUN WERE MADE OR ARE OWED.** This is a
        `.md`, which `strat_banner_sweep.py`'s REPORT IDENTITY check does not consider at all --
        it derives `newest_test_mtime` from `.cpp` files carrying an `IMPLEMENT_*_AUTOMATION_TEST`
        macro -- so the live figure at `reportCreatedOn 2026.09.05-16.43.13` in
        `Tools/architect/state/global.md` is unaffected. **CONTRAST THE ROUND ABOVE, which owed a
        run for exactly that reason.** The finding was the struck `RETRACTED>` count; while
        fixing it, **EVERY OTHER COUNT AND ENUMERATION IN THIS ENTRY WAS RE-MEASURED AGAINST THE
        TREE AS IT NOW STANDS**, rather than against what was measured before the fourth round's
        edit -- an unchecked count that happens to be right is indistinguishable from a lucky one.
        - **RIGHT, AND CHECKED.** Macros per file, 2 / 1 / 4 / 2 in the four files named at the
          head of this entry. `EStratVolumeDisposition` has FIVE members (`Applied`,
          `NoSettings`, `NoBank`, `NoMix`, `NoWorld`), so *"all five dispositions"* is exact, as
          is the 3 / 1 / 0 `ChannelsApplied` bracket. Seven sweep shapes = shape (a) plus the six
          named. Shape (a) returns ZERO across all 97 and `audio device ... own(s|ed)` is ZERO
          lane-wide. The three-token `grep` over all of `Source/` still returns EXACTLY THREE
          hits. `git worktree list` still shows this tree plus SEVEN others.
        - **RIGHT IN NUMBER, LOOSE IN DESCRIPTION -- both corrected above.** The 97 (its glob
          gloss covered 96) and *"every surviving hit"* (three categories for five).
        - **ONE MORE, NARROWER THAN ITS WORDING, corrected here rather than in the third-round
          bullet that made it.** That bullet says the three surviving hits are *"all now
          retraction blocks"*. Two are (`StratShellGameMode.h:61`,
          `StratTitleMusicLifecycle.cpp:32`); the third,
          `StratTitleMusicLifecycle.cpp:315`, is a QUOTATION of the retracted sentence that says
          in its own paragraph that the sentence is retracted at source. No unretracted site
          exists, which is the claim that matters; *"retraction block"* is not the right name for
          all three.
        - **AND ONE CLAIM IS BROADER THAN ITS EVIDENCE, LEFT STANDING BUT NOW BOUNDED.** The
          third-round bullet names FIVE other files that reason about `Occurrences 0` correctly
          and concludes the unstated precondition *"is not false anywhere"*. Measured now: **28
          files under `Source/*/Tests/` pass `Occurrences 0`**, and only those five were read.
          The five were chosen because they REASON ABOUT the mechanism in prose rather than
          merely using it, which is where a false premise can hide -- but a lane-wide negative
          resting on five of twenty-eight is a sample, not a sweep. **READ IT AS: no false
          statement of the precondition was found in the five files that discuss it.** A full
          pass over the other 23 is not owed by any clause and is not claimed here.

- **2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file under
  `Source/StratPlay/Tests/`, plus this file -- on `master` in the main tree
  `E:/MultiAgent/Stratocracy`, base commit `089c79c`, over the UNCOMMITTED phase A'/A/B/C audio
  work, and this pass is UNCOMMITTED too) -- **THE AUDIO MILESTONE'S PHASE D: `GATE-AUDIO`'S
  THIRD FILE, AND THE THING WORTH READING IS THAT PHASE C SHIPPED SEVEN SOUND ASSETS, A BANK
  AND THREE BLUEPRINT DEFAULTS WITHOUT MOVING THE SUITE BY ONE TEST** -- because every phase-B
  fixture builds its own `FStratMatchConfig` with no `SoundBank`, so every emission in the tree
  recorded `NoBank` and NOT ONE CLAUSE HAD EVER READ A SHIPPED SOUND ASSET. Cite this pass by
  its exported `reportCreatedOn 2026.09.05-13.03.00` -- never by a number; the figure lives in
  `Tools/architect/state/global.md` and nowhere else. That run is the FINAL one of the pass: it
  followed the last mutant's revert AND a rebuild, and it overwrote every earlier run of this
  pass, which are therefore not citable.
  - **THE FILE.** `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp`, five clauses,
    counted BY MACRO: 5 `IMPLEMENT_SIMPLE_AUTOMATION_TEST`. The ID is `GATE-AUDIO`, and this is
    the third and last of the three files the 2026-09-04 ruling in `global.md` authorized for
    it; **no acceptance ID is minted or claimed.** It lives in `StratPlay` because that is the
    module that can see `AStratGameMode`, `AStratShellGameMode`, `UStratSoundBank` and
    `UStratSoundDirector` at once.
  - **THE THREE ARMS THAT WERE DEAD CODE TO THE SUITE UNTIL THIS PASS, and this is a
    measurement, not an impression.** Before this file, `EStratSoundDisposition::Played`,
    `::NoSoundConfigured` and `::SuppressedByCooldown` appeared nowhere in `Source/` outside a
    display-only `switch` in a test helper. `EmitCue`'s entire play arm -- the `SoundFor`
    lookup, the cooldown comparison, the `PlaySound2D` call, the `LastPlayedAtSeconds` write and
    the `Played` record -- could have been deleted with the suite staying green.
    `EveryCueInTheShippedBankReachesTheEngine` is the clause that runs it, and its mutants
    exercised `NoSoundConfigured` for the first time as well. **`SuppressedByCooldown` IS STILL
    UNPINNED AND CANNOT BE PINNED BY THIS SHAPE** -- it compares against
    `UWorld::GetTimeSeconds`, these fixture worlds are created with `bInformEngineOfWorld=false`
    and never ticked, so the clock does not advance and a second emission would measure the
    FIXTURE rather than the bank. Each cue is therefore emitted exactly ONCE, which is the only
    shape whose answer does not depend on a clock. Closing that arm needs a seam letting a
    clause supply the "now"; it is not a test-lane change.
  - **THE TWO DEBTS DISCHARGED.** `StratMatchSubsystem.h` on `FStratMatchConfig::SoundBank` and
    `StratShellGameMode.h` on `AStratShellGameMode::SoundBank` each recorded, in terms, that an
    asset pointer has no C++ author to reconcile against and that the discharge was *"phase D's
    and is not this file's to write"*. `EveryShippedGameModeNamesTheSameSoundBank` is it.
    **BOTH HEADERS SAY "TWO", AND THAT IS RIGHT ABOUT CLASSES AND SHORT ABOUT ASSETS:** two C++
    classes carry a `SoundBank`, and THREE shipped Blueprints set one, because
    `BP_StratGameMode_AiVsAi` is a second asset of the first class. The clause compares three
    pointers and is named `Every...` rather than the `Both...` the phase brief proposed -- a
    name asserting over three while saying "Both" is the overclaiming-name defect this record
    already carries a stamped entry about.
  - **WHAT EACH CLAUSE PINS AND WHAT IT DOES NOT, where the distinction bites.**
    - `EveryShippedGameModeNamesTheSameSoundBank` pins that the three assets name ONE bank and
      that each pointer is non-null. It holds **no expectation at all** -- no asset path, no
      literal -- so re-authoring the bank keeps it green as long as all three move together.
      **It does not pin that the bank is the RIGHT bank**; nothing module-side could say which
      asset is right, and the other four clauses pin what is IN it instead.
    - `ShippedGameModesAuthorTheirSoundBank` is the MIRROR of
      `T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp`: same archetype-comparison
      instrument, opposite answer. That one asserts IDENTICAL and concludes inheritance; this
      one asserts NON-IDENTICAL and concludes authorship. **WHY IT IS NOT REDUNDANT WITH THE
      NON-NULL ASSERTIONS ABOVE:** those rule out a pure C++ answer only because
      `TObjectPtr<UStratSoundBank>` happens to initialise null on both classes today. Give
      either a non-null C++ default and "all three equal and non-null" would be satisfied by
      objects no asset ever set, while the Blueprints had been cleared -- this record's "a
      default cannot signal unset" in a new costume. This clause cannot be fooled that way
      because it reads a DELTA and not a value. Note the asymmetry with the clause it mirrors:
      asserting IDENTICAL cannot separate "never touched" from "set to the parent's value";
      asserting NON-IDENTICAL has no such gap, since a delta cannot exist unwritten.
    - `EveryShippedCueSoundCarriesTheSfxClass` carries **the file's ONE asset-path literal**,
      `SCL_Strat_SFX`, and it is the legitimate shape: nothing in `Source/` names a sound class
      at all, by design, so there is no module-side value to read and the named asset IS the
      expectation. It is loaded as an OBJECT and compared by identity, never by string -- which
      also sidesteps `FString`'s case-insensitive compare. Half the claim needs no literal: all
      cues must carry the SAME class as each other, read off the first. **It does not pin the
      mix tree ABOVE that class** -- not the parent `SCL_Strat_Master`, not `SBM_Strat_SFX` as a
      default submix, not `SMX_Strat_Base`. Those are separate claims on separate assets.
    - `EveryCueInTheShippedBankHasASound` is the cheapest clause and names the fault most
      precisely. **`UStratSoundBank`'s header is right that a null slot is a configuration and
      not a fault -- about the CLASS.** It is not right about the SHIPPED bank, and nothing
      could tell the difference before this clause: the director records `NoSoundConfigured`,
      logs nothing, refuses nothing, and exactly one cue is silent forever. **It is NOT
      redundant with `...ReachesTheEngine`, which also reddens on a null slot:** this one runs
      with no world, no subsystem and no engine call, so it is decided by the DATA alone, while
      the other can be reddened by a fixture problem that has nothing to do with the bank. When
      both fail, this is the one that says what to fix.
    - The cue list in the last two clauses is **read off `StaticEnum<EStratSoundCue>()` and not
      written**, so an eighth cue is covered the day it is added. A hand-written seven would
      have reintroduced, one level down, the exact gap this whole file closes.
  - **THE FIVE NAMED MUTANTS, EACH AN ASSET OR BLUEPRINT EDIT AND NOT A C++ ONE**, run in place
    through a headless `-run=pythonscript` commandlet, each exported to its own report path so
    the live citation was never overwritten, each reverted from a byte backup and verified by
    sha256. Every mutation was confirmed by the file's sha256 CHANGING, never by an API return
    value -- this record's "a `set()` return is not evidence".
    - Clear `DA_StratSoundBank.UnitMoved` -> reddens `EveryCueInTheShippedBankHasASound` naming
      `UnitMoved`, `EveryCueInTheShippedBankReachesTheEngine` reading back
      `NoSoundConfigured`, and `EveryShippedCueSoundCarriesTheSfxClass`'s null guard. Three red.
    - Point `MS_Strat_UnitMove.SoundClassObject` at `SCL_Strat_Music` -> reddens
      `EveryShippedCueSoundCarriesTheSfxClass` ALONE, on all three of its lines: the claim, the
      set-difference control, and the literal-free parity half. One red.
    - Clear `BP_StratGameMode`'s `MatchConfig.SoundBank` -> five red, which is the whole file
      seeing the shipped pointer.
    - Clear `BP_StratShellGameMode`'s `SoundBank` -> reddens
      `EveryShippedGameModeNamesTheSameSoundBank` and `ShippedGameModesAuthorTheirSoundBank`'s
      TITLE-MAP claim, confirming part two is a separate measurement on a separate `FProperty`
      of a separate `UClass` and not a repetition of part one. Two red.
    - Duplicate the bank and point `BP_StratShellGameMode` at the copy -> reddens the EQUALITY
      claim in `EveryShippedGameModeNamesTheSameSoundBank` and **nothing else**, with both
      pointers non-null and the two banks structurally identical. That is the strongest form of
      this mutant and the only one reaching the equality line rather than a null guard. The
      duplicate was deleted from disk and its absence verified by listing the directory, not by
      an API return.
  - **A CLAUSE OF MINE WAS MEASURED WRONG BY ITS OWN MUTANT AND REWRITTEN, which is the reason
    mutants are RUN and not merely reasoned about.** `ShippedGameModesAuthorTheirSoundBank`'s
    CONTROL 1a was first written fatal -- `if (!TestFalse(...)) { return false; }`. On the
    mutant that matters most, clearing the shipped bank, it fired FIRST and stopped the clause
    with *"the copy is not faithful"*, **a false diagnosis of a true failure**, and THE CLAIM --
    whose message names the actual fault and the two lanes that could own the fix -- never ran.
    Made non-fatal, the same input leaves control 1b green (a copy cleared to the archetype's
    own null does compare identical) and reddens THE CLAIM with its own message. **A control
    whose failure message misdiagnoses the most likely cause is worse than no message.**
  - **CONTROL 1b READS THE ARCHETYPE'S OWN VALUE RATHER THAN WRITING `nullptr`**, so it stays
    valid on the day a C++ default stops being null -- which is precisely the day the first
    clause's non-null assertions stop being able to infer authorship on their own. The two
    clauses were written to fail over to each other on that change.
  - **THE SOURCE-CONTROL PROVIDER AUTO-STAGED THREE ASSETS DURING THE MUTANT RUNS, AND A CLEAN
    WORKTREE DID NOT MEAN A CLEAN INDEX.** `DA_StratSoundBank.uasset`, `MS_Strat_UnitMove.uasset`
    and the temporary `DA_MUTANT_Bank.uasset` were added to the index the moment the commandlet
    saved them, while all four mutated files verified byte-identical to their backups by sha256.
    **A sha256 check over the worktree cannot see an index that moved.** The index was restored
    with `git rm --cached` and the restoration verified by DIFFING `git status --porcelain`
    against a capture taken BEFORE the first mutation -- which is the check that catches this,
    and only because the capture existed beforehand.
  - **WHAT THIS FILE DOES NOT PIN, beyond the per-clause notes above.** Not that anything is
    AUDIBLE: `Played` means `PlaySound2D` was called and returned, and under `-nullrhi` there is
    no device. That the seven MetaSound graphs produce waveforms a human would recognise is not
    observable by any instrument in this tree and needs a human at the keyboard. Not
    `SC_StratCues`' concurrency behaviour, which nothing here reads. Not `MinSecondsBetween`.
    Not that any of these Blueprints is its map's GameMode -- that binding is still the single
    unguarded `GlobalDefaultGameMode` line in `Config/DefaultEngine.ini`.

- **2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- two files under
  `Source/*/Tests/`, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`,
  base commit `089c79c`, over phase A's UNCOMMITTED audio C++, and this pass is UNCOMMITTED
  too) -- **THE AUDIO MILESTONE'S PHASE B: `GATE-AUDIO` IS PINNED, AND THE THING WORTH READING
  IS THAT ONE OF THE NINETEEN CLAUSES WAS MEASURED WRONG BY ITS OWN NAMED MUTANT AND HAD TO BE
  REBUILT -- its "the tour is not silent" control was being satisfied through a DIFFERENT CALL
  SITE than the one under test.** Cite this pass by its exported
  `reportCreatedOn 2026.09.05-04.55.48` -- never by a number; the figure lives in
  `Tools/architect/state/global.md` and nowhere else. That run is the FINAL one of the pass: it
  followed the last mutant's revert AND a rebuild, and it overwrote every earlier run of this
  pass, which are therefore not citable.
  - **THE TWO FILES.** `Source/StratUI/Tests/StratSoundCueClauses.cpp` (ten clauses, world-free,
    over `StratSoundCues.h`'s pure deciders) and
    `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp` (nine clauses, live, on
    `StratTransientReceiptCallSite.cpp`'s fixture shape). Counted BY MACRO, not by name: 10 + 9
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, and 19 `GATE-AUDIO` names present in the report. The
    ID is `GATE-AUDIO`, authorized for exactly these two files and a third not yet written by
    the 2026-09-04 ruling in `global.md`; **no acceptance ID is minted or claimed.** The split
    between the two files is `StratTransientReceipt*`'s exactly: `StratUI` is below `StratPlay`,
    so anything naming a subsystem, a controller or a world lives in the second file.
  - **THE ONE CLAUSE THAT KEEPS THE OTHER EIGHTEEN NON-VACUOUS, and it is why it is written and
    placed first.** `Stratocracy.StratPlay.GATE-AUDIO.EmitCueRecordsWithNoBank`. No
    `UStratSoundBank` asset exists in this project, so **every cue this suite will ever observe
    is recorded with `EStratSoundDisposition::NoBank`.** `UStratSoundDirector::EmitCue` records
    first and plays second with no early return on any path, and that ordering IS the feature.
    MEASURED: `if (SoundBank == nullptr) { return; }` inserted at the top of `EmitCue` reddens
    **six** clauses at once. A future reader tidying that function must read this bullet first.
  - **WHAT EACH CLAUSE PINS AND WHAT IT DOES NOT, for the ones where the distinction bites.**
    - `AUnitAbsentFromTheModelEmitsDestroyed` and `AUnitAbsentFromTheMarkEmitsBuilt` are the two
      set differences and **NEITHER IS REDUNDANT -- MEASURED, NOT ARGUED.** Inverting the ARRIVAL
      arm alone reddens only the second; inverting the DEPARTURE arm alone reddens only the
      first (plus `ADestroyedUnitCarriesItsSide`). Do not delete one as a duplicate of the other;
      each is the other's only net.
    - `ADestroyedUnitCarriesItsSide` is what makes `FStratUnitSoundMark::Side` a feature rather
      than a dead field. That header calls it "stored and never COMPARED", which is true and is
      not the same as never READ -- it is read out into the death cue's `Side`, and the model
      cannot supply it because the unit is gone. The clause wipes a WHOLE side so the model has
      no copy to fall back on.
    - `TheTurnEdgeIsThePairAndNotEitherHalf` drives BOTH halves in one clause because either
      alone is green over a wrong decider -- this record's "an invariant covers only the half it
      was stated on" class.
    - `TheMarkDiesWithTheAppliedModel` pins ONE of the two reset sites: the
      `TearDownPresentation` copy, reached through a second `StartMatch`. **`Deinitialize`'s copy
      is NOT pinned and cannot be by this lane** -- it runs as the world dies, and the director
      that holds the record dies with the same world, so there is nothing left to observe with.
      Identical gap, identical line, as `StratTransientReceiptCallSite.cpp` records for the
      receipts' mark. Closing it needs a probe outliving the world.
    - `AStepVoicesItsOwnEventAndTheTourIsNotSilent` **does not pin the per-step KIND MAPPING.**
      It pins the tour as a whole: at least one command cue, never a non-diegetic cue, at most
      one command cue per step, at least one step with none (which is what pins §4.10 `EndTurn`
      being silent), and -- from a roster DIFFERENCE, not a list the test composed -- that a
      hand-over which gained units voiced a build cue and one that lost units voiced an attack
      cue. **A mutant swapping the Move and Attack arms is invisible to it.**
  - **THE SEAM THAT WOULD CLOSE THAT LAST GAP, NAMED SO IT IS NOT REDERIVED.**
    `UStratMatchSubsystem::AiPlaybackReel` is private and this module publishes no accessor for a
    step's CONTENTS -- `GetAiPlaybackStepCount` and `GetAiPlaybackCursor` are counts. A
    `const FStratAiPlaybackStep* GetAiPlaybackStepAt(int32) const` would let a clause assert
    Move/Attack/Build/EndTurn => 1/1/1/0 per step directly. **That is engine-lane work outside
    `Source/*/Tests/` and this lane did not do it.**
  - **NINETEEN CLAUSES, EIGHTEEN NAMED MUTANTS, EACH RUN IN PLACE AND EACH RESTORED.** Every
    mutant was applied to the real source, built, run against the FULL suite exported to a
    SEPARATE report path so the citable run was never overwritten, then reverted; all four
    touched files were restored from byte copies and their sha256 confirmed equal to the
    pre-mutant values. Each mutant reddened its named clause and nothing outside `GATE-AUDIO`
    ever went red -- **the collateral is itself evidence, and the widest was the null-bank early
    return at six.**
  - **AND ONE MUTANT SURVIVED ON THE FIRST PASS, WHICH IS THE ENTRY'S HEADLINE.**
    `AStepVoicesItsOwnEventAndTheTourIsNotSilent` was written asserting
    `GetEmissions().Num() > 0` after stepping the tour, and the mutant it names -- gating the
    per-step cues on `PlayMoveSlideForStep`'s return -- **came back with the suite fully green,
    zero red.** The cause: `AdvanceAiPlaybackOneStep` emits from TWO call sites, the command
    switch and `ApplyTourExistenceAtCursor`'s death cue, and only the first is behind the gate.
    `AStratUnitActor::MoveTweenSeconds <= 0` is the C++ default, so the mutant silenced every
    command cue -- and the DEATH cues alone kept the control true. **The control was satisfied
    through a channel other than the one under test**, which is a fresh instance of a class this
    record already carries. Repaired by counting the three command cues separately; the mutant
    was re-run against the repaired clause and now reddens it. The clause's own header states
    all of this, including the survived run, rather than reading as if it had always worked.
  - **AND ONE DECLARATION WAS AN ASSERTION THAT FAILED.** The first run of
    `MatchEndedFiresOnceAndFromTheLatch` was RED, on
    *"Expected suppressed ('Warning') level log message or higher matching 'No end-of-match
    screen this match' did not occur"* -- declared on `StratMatchResultTrigger.cpp`'s precedent
    and wrong for this fixture, which spawns no `AStratScoreboardHUD` and so never reaches the
    line that refuses. `AddExpectedMessagePlain` with `Occurrences 0` **is an assertion**; the
    declaration was removed rather than the fixture changed, and the clause's header records why.
  - **WHERE THE EXPECTATIONS COME FROM.** Every expectation is a module-side read: a field off a
    real `FStratViewModel` projected from the shipped tables and scenario, a field off the mark
    the module's own `StratSoundMarkFromView` produced, or a count off
    `UStratSoundDirector::GetEmissions` / `GetEmitCallCount` / `GetApplyViewObservationCount`.
    **No unit id, side index, hex, HP figure, turn number or unit count is written in either
    file.** Plants are module values too -- a "moved" unit gets ANOTHER unit's hex off the same
    model, a "built" unit is a real unit removed from a copy. **The single arithmetic operation
    across both files is on a PLANT and is declared in the header that carries it:** `Hp - 1`,
    used to make an ORDERED pair out of one reading so `AnHpRiseIsNotAnAttack` can drive both
    directions of a `<` over the same two numbers. No clause asserts an HP figure.
  - **SILENCE IS NEVER ASSERTED ALONE IN THE LIVE FILE**, which is that file's whole addition
    over the world-free one. An empty emission list has two causes that look identical -- every
    cue suppressed, or the seam dead -- so every silence clause carries a positive control:
    `GetApplyViewObservationCount()` where `ApplyView` is the subject, a demonstrably destroyed
    actor population where the tour release is, and a return value where a verb is.
  - **NO FLAKE OBSERVED.** No clause in either file captures `GLog`, so the
    `CanBeUsedOnMultipleThreads()` hazard does not arise here. Both files were run twenty times
    over in the mutant pass and no clause ever failed except by its own mutant.

- **2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file under
  `Source/StratPlay/Tests/`, plus this file -- on `master` in the main tree
  `E:/MultiAgent/Stratocracy`, base commit `15bd1b0`, UNCOMMITTED) -- **WHO OWNS THE SHIPPED
  DIFFICULTY TIER IS NOW A MEASURED FACT AND NOT AN OPEN QUESTION, AND THE THING WORTH READING
  IS THAT THE ANSWER IS "NOBODY IN CONTENT": the Blueprint contributes no value for that field
  at all, so the tier the shipped game runs at is the C++ initialiser's, reaching the package by
  inheritance.** Cite this pass by its exported `reportCreatedOn 2026.09.04-16.30.20` -- never by
  a number; the figure lives in `Tools/architect/state/global.md` and nowhere else. That run is
  the FINAL one of the pass: it followed the mutant's revert and the rebuild, and it overwrote
  the earlier `2026.09.04-16.26.11` run of identical source, which is therefore not citable.
  - **The clause.** `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp`,
    the THIRD clause in `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`, reusing
    that file's `ResolveCdoOrFail`, `Describe` and `DescribeDifficulty`. **It is not another
    reading of the same instrument the two clauses beside it use.** Those read an EFFECTIVE value
    off a CDO -- the number a spawned GameMode would start from -- and an effective read cannot
    say where the number came from. This one reads no value against an expectation at all: it
    takes the reflected `FProperty` for `FStratMatchConfig::Difficulty` and asks
    `Identical_InContainer` whether the shipped `BP_StratGameMode_C` CDO differs from its
    ARCHETYPE, the native `Default__StratGameMode`, on that one member.
  - **WHAT IT ESTABLISHES, POSITIVELY.** `BP_StratGameMode` contributes NO delta on
    `Difficulty`; the value is inherited from `FStratMatchConfig::Difficulty`'s C++ initialiser.
    The operational consequence, and the reason it earns a clause: **change that initialiser and
    the shipped game moves with it.** Had the asset authored a tier, it would not have. This is
    strictly more than the "cannot tell" the sibling clause was left with, and it is the reason
    that clause's header now carries a `<SUPERSEDED 2026-09-04>` block instead of its old
    closing sentence that no reader of overrides could exist.
  - **WHAT IT DOES NOT PIN, AND THE FIRST ONE IS THE REAL LIMIT OF THE WHOLE APPROACH.**
    (a) **It does not pin that a designer never touched the tier.** A property set in the editor
    to the value its parent already holds contributes no delta either, so "never set" and "set to
    Easy, which is what the parent said anyway" are the SAME reading here and this clause does not
    separate them. It does not need to -- the claim is about where the RUNTIME value comes from,
    which is answered either way -- and the clause is worded "inherits", never "the designer left
    it alone", for exactly that reason. The failure message says this in its own words.
    (b) **It does not pin the VALUE.** That the inherited tier is `Easy` and not `Hard` remains
    `T-FAME-02.ShippedGameModeRunsAtEasy`'s, immediately above it. This clause would stay GREEN if
    the C++ initialiser moved to `Normal` and the Blueprint still said nothing. **The two clauses
    are a pair and neither is redundant: together they pin the tier and its owner; separately,
    neither does.** Do not delete one as duplicative of the other.
    (c) It pins no behaviour, and it pins no other field. `AiSides` appears in it ONLY as a
    control; `ShippedGameModeAuthorsOneAiSide` is what pins `AiSides`.
  - **THE INSTRUMENT, MEASURED BEFORE IT WAS TRUSTED.** A throwaway probe clause was built and
    run first, and deleted before the deliverable; what it reported, on the shipped package:
    super class `StratGameMode` and native, archetype `Default__StratGameMode`, `Difficulty`
    identical with both sides exporting `Easy`, `AiSides` NOT identical with `(1)` against the
    C++ default's empty, and the whole `MatchConfig` struct NOT identical. On
    `BP_StratGameMode_AiVsAi_C`, the same shape with `(0,1)`. **The hypothesis the brief offered
    was confirmed rather than assumed, and the probe was the thing that confirmed it.**
  - **GRANULARITY IS LOAD-BEARING AND IS ASSERTED, NOT COMMENTED.** The WHOLE `MatchConfig`
    property compares NON-identical between those two objects, because `AiSides` differs. So a
    future "simplification" to a struct-level compare would conclude that the Blueprint authors
    the tier and would be WRONG. The clause asserts the struct-level non-identity precisely so
    that the granularity the answer depends on is pinned by a test rather than by prose.
  - **THE LIVENESS CONTROL RUNS ON THE FIELD IN QUESTION, WHICH IS THE ONE THING THE SIBLING
    CLAUSE CANNOT DO.** `ShippedGameModeRunsAtEasy`'s liveness rides `AiSides`, a different field.
    Here, control 1 copies the shipped CDO's own `FStratMatchConfig`, asserts the copy still
    compares IDENTICAL to the archetype on `Difficulty` (so the copy is faithful), then moves
    ONLY `Difficulty` on the copy and asserts the SAME call against the SAME archetype now
    reports a DIFFERENCE. The tier it moves to is chosen relative to the one read, so the control
    survives a future change of shipped tier. Control 2 is the older shape -- `AiSides` on the
    CDO/archetype pair must differ -- and its job is narrower than it looks: it kills the case
    where the clause has accidentally compared an object to ITSELF, which would read as a clean
    inheritance for the worst possible reason.
  - **Falsifiability: ONE mutant, RUN IN PLACE, and the reason it has that shape.** The honest
    mutant -- authoring a tier on the asset, or moving the C++ default -- touches
    `Content/` or `Source/StratPlay/` outside `Tests/`, and this lane may do neither. So the
    subject was simulated at the exact call the claim rests on: a local copy of the shipped
    `MatchConfig` with `Difficulty` moved to `Hard`, substituted for the shipped container in
    THE CLAIM's `Identical_InContainer` only. Built in place (a copied tree's cached
    `Intermediate/Build` makes a mutant a silent no-op) and run: the clause went **RED at THE
    CLAIM**, with its own message printing `(both read 'Easy')`, and **only that clause** --
    the other five `Stratocracy.StratPlay.T-FAME-02.*` clauses stayed green in the same run,
    so the mutant is specific and not a blast. The mutant run was exported to
    `Saved/AutomationReportMutant` so the full-suite `index.json` was not overwritten by it;
    that directory was then deleted, the file restored, the editor target rebuilt, and the FULL
    suite re-run. `grep -n MUTANT` over the file returns nothing.
    **What this mutant does NOT prove, said plainly: it demonstrates that the claim's comparison
    reports a difference when the shipped side's `Difficulty` moves. It does not exercise a real
    re-authored asset, because this lane cannot produce one.** The standing in-suite equivalent
    is control 1b, which makes that same demonstration on every run rather than once.
  - **The build is not optional before the run, and it is why the mutant was rebuilt twice.** A
    clause name is compiled into the binary; a stale `UnrealEditor-StratPlay.dll` reports the OLD
    name green and the new clause simply absent. Presence was therefore confirmed BY NAME in the
    exported report and not inferred from a count delta.
  - **The `autocrlf` warning on this file's `.cpp` is PRE-EXISTING and this pass preserved it.**
    `git diff` emits "warning: in the working copy of ... LF will be replaced by CRLF the next
    time Git touches it"; the file is LF in the worktree and LF in the HEAD blob. Same condition
    the entry below reports; not a change this diff made.
  - **Housekeeping at the end of the pass, stated as artifacts rather than as a path count:**
    the probe file was deleted, the `Saved/AutomationReportMutant` directory was deleted, the
    mutated `.cpp` was restored (`grep -n MUTANT` returns nothing), and `Saved/` is ignored via
    `.gitignore:68`. **No untracked strays were left by this pass, and nothing was staged and
    nothing was committed.**
    **[STAMPED 2026-09-04: this bullet originally opened "Working tree at the end of the pass:
    STAMPED> `git status --porcelain --untracked-files=all` lists exactly one path,
    STAMPED> `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` (modified)".
    That was a working-tree self-measurement inside the tree it measured, and it moved its own
    subject: this entry was the second path, `global.md` the third, and the gate report the
    fourth, so a checkout read it as false. `strat-integration-reviewer` raised it as
    OBSERVATION 1 in `Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md`.
    The retired clause is NOT replaced with a corrected count, because any count is false again
    on the next write to any record file; only the half that stays true -- the named artifacts
    cleaned up, nothing staged, nothing committed -- is carried forward above.]**

- **2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file under
  `Source/*/Tests/`, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`,
  base commit `9d705ca`, UNCOMMITTED) -- **`Difficulty` ON THE SHIPPED ASSET IS PINNED, AND THE
  THING WORTH READING IS THAT THIS CLAUSE IS DELIBERATELY WEAKER THAN THE ONE BESIDE IT AND
  SAYS SO IN ITS OWN ASSERTION MESSAGE.** Cite this pass by its exported
  `reportCreatedOn 2026.09.04-04.47.20` (UTC in the report; local date 2026-09-04), never by a
  number -- the figure lives in `Tools/architect/state/global.md` and nowhere else.
  **[STAMPED 2026-09-04: this entry originally cited `reportCreatedOn 2026.09.04-04.09.26`.
  That run is gone -- `Saved/AutomationReport/index.json` is a SINGLE PATH every run
  overwrites, and the rename recorded in the bullet below rebuilt and re-ran the full suite
  over it. The old stamp is kept as the historical name of the run this entry first
  described; it is no longer re-readable from a checkout, and every citation of it
  elsewhere is a citation of a report that no longer exists.]**
  **[STAMPED 2026-09-04, SECOND TIME, AND FOR THE SAME MECHANISM: `reportCreatedOn
  2026.09.04-04.47.20` IS NOW ALSO GONE. The pass recorded at the top of this file rebuilt and
  re-ran the full suite over that same single path. The stamp above predicted this in general
  terms; this one records that it happened, so that a reader holding either figure knows both
  name runs a checkout can no longer produce. The live report is whichever
  `reportCreatedOn` the topmost entry cites.]**
  - **The clause.** `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy`, in
    `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`, beside
    `T-FAME-02.ShippedGameModeAuthorsOneAiSide` and reusing that file's `ResolveCdoOrFail` and
    both path constants. It reads `BP_StratGameMode_C`'s CDO and requires
    `MatchConfig.Difficulty == EStratDifficulty::Easy`.
  - **WHAT IT PINS: THE VALUE. WHAT IT DOES NOT PIN: THE AUTHORSHIP.** This is the `ViewingSide`
    situation in that same file, NOT the `AiSides` situation, and the distinction is the whole
    reason the clause needed a design rather than a copy. `FStratMatchConfig::Difficulty`
    initialises to `EStratDifficulty::Easy` in `Source/StratPlay/StratMatchSubsystem.h`; `Easy`
    is the FIRST value of `UENUM EStratDifficulty` and is therefore 0; and the 2026-09-03
    live-editor measurement found `Difficulty` ABSENT from both the native and the Blueprint
    export, which is what a zero-valued field at its C++ default looks like. So a CDO read
    cannot distinguish an authored `Easy` from an untouched one, and **nobody may cite this
    clause as evidence that a designer set the tier.** The assertion's own failure message says
    so, so a reader who lands on a red without reading any header still learns it.
  - **THERE IS NO PREMISE BLOCK, AND ITS ABSENCE IS DELIBERATE.**
    `ShippedGameModeAuthorsOneAiSide` opens by asserting, first and fatally, that the C++ default
    cannot supply its expectation. On `Difficulty` that premise is FALSE -- the C++ default IS
    the expectation -- so copying the block would have produced a clause that fails on a healthy
    tree. Do not "restore" it.
  - **The liveness control is measured on a DIFFERENT FIELD, and that is the caveat to carry
    forward.** A reader that had silently stopped seeing asset overrides answers the C++ default
    to everything, and here the C++ default is the expectation, so the `Difficulty` assertion
    cannot be its own liveness check. The clause runs the same `ResolveCdoOrFail` over
    `BP_StratGameMode_AiVsAi_C` and requires the two packages to disagree on `AiSides` (`(1)` vs
    `(0,1)`). That proves the reader reads ASSETS. **It does not prove that this field's answer
    came from the asset rather than from the C++ default underneath it, and no arrangement of
    assertions can while `Easy` is also the C++ default.** Closing it needs either a different
    C++ default or a reader that reports whether a property was overridden; neither is this
    lane's to build. **[STAMPED 2026-09-04: the last eight words are RETIRED. The second of
    those two exits WAS this lane's to build and was built the same day -- see the entry at the
    top of this file, whose clause
    `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp` compares the
    shipped CDO against its archetype through the reflected `FProperty` and reports whether the
    field carries a delta, needing no asset edit and no C++ edit. The rest of this bullet stands
    unchanged and is still the correct account of THIS clause: it reads an effective value, its
    liveness rides `AiSides`, and it still cannot separate an authored `Easy` from an untouched
    one on its own. What changed is that the file now answers the question elsewhere, not that
    this clause got stronger.]**
  - **Why the value is worth pinning at that weaker warrant.** GDD Sec 2.11.6, quoted from the
    document: "The first match runs on the one shipped scenario at **Easy** by default (player
    +150 opening Fame, Sec 2.9)". `StratDifficultyFameDelta` gives +150 / 0 / -100, so Easy is
    the only tier whose delta is non-zero in the player's favour. A shipped `Normal` would make
    Sec 2.9's handicap ARITHMETICALLY unobservable while every existing handicap clause stayed
    green, because all of them construct their own tier in C++ and read no asset.
  - **Falsifiability: two mutants RUN, and the honest limit on both.** The subject is an asset
    this lane may not edit, so the asset itself was never mutated. (a) Expectation `Easy` ->
    `Normal`, built in place and run: RED, and its message printed `(read: Easy)` -- so the
    assertion is reached and the value genuinely came back from a CDO read at runtime, though
    that still cannot separate asset from C++ default. (b) Control package pointed at the
    shipped package so both sides name one asset: RED --
    `Expected '...' to differ from "(1)", but it was "(1)"` -- which proves the liveness control
    is a real discriminator and not an assertion that always passes. Both mutants were built IN
    PLACE (a copied tree's cached `Intermediate/Build` makes a mutant a silent no-op) and both
    probe runs were exported to `Saved/ProbeReportA` and `Saved/ProbeReportB` so the full-suite
    `index.json` cited above was not overwritten by them. The pristine file was restored,
    rebuilt, and the FULL suite re-run afterwards; the citation above is that final run.
  - **A stale header claim was repaired in the same pass.** That file's "WHAT THIS CLAUSE DOES
    NOT PIN" bullet read *"IT DOES NOT PIN `Difficulty`, `SaveSlotName`, `AiPlaybackStepSeconds`
    OR ANY OTHER FIELD"* -- true until this clause landed and false the moment it did. It is now
    two bullets: one narrowed to the fields still unpinned, and one that hands `Difficulty` to
    the new clause and states in the same breath that the new clause is the weaker instrument
    and why.
  - **THE CLAUSE WAS RENAMED AFTER THE GATE, AND THE REASON IS THE LESSON: A CLAUSE NAME IS
    READ WITHOUT ITS HEADER.** This clause first landed as
    `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`
    **[STAMPED 2026-09-04: that name is retired. It is written out here in full so that a
    reader who arrives holding it can follow the trail forward to the live name. WHERE ELSE IT
    SURVIVES IS DELIBERATELY NOT STATED HERE, in a number or a list or a "the one": the set of
    files carrying it is still growing, so any such construction in this file is false by the
    next write, and three attempts to state it were falsified exactly that way. The one
    property about its distribution that a checkout can re-execute, and that stays true as that
    set grows, is that IT DOES NOT OCCUR UNDER `Source/` AT ALL -- `grep -rl` over `Source/`
    returns nothing for it while returning the live name, which is the control showing the
    search can see that tree.]** The live name is `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy`.
    - **Where the rename came from, and HOW MANY OBSERVATIONS THAT GATE ACTUALLY RAISED.**
      `strat-integration-reviewer` gated the pass PASS with zero findings -- its `## Findings`
      section reads "**None.**" -- and raised **FOUR observations**, all marked "Non-gating."
      **[STAMPED 2026-09-04: this sentence originally read "gated the pass PASS with zero
      findings but raised **one observation**". THAT WAS FALSE BY THREE, AND IT WAS FALSE THE
      INSTANT IT WAS WRITTEN -- not merely falsified later. An earlier stamp in this slot
      explained it as *"written before that report existed to be checked against"*; THAT
      EXPLANATION IS ITSELF FALSE and is retired together with the claim it excused, because a
      stamp that retires a claim for a wrong reason teaches the wrong lesson to the next
      reader. What was measured: `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md`
      is the source both of the rename this bullet describes and of the "zero findings" wording
      standing beside the miscount, so the miscount was written with that report in hand; and
      `git show HEAD:Tools/architect/state/tests.md` finds neither the miscount nor this bullet
      in the committed file, so both were authored in the same uncommitted pass, which began
      after that report was on disk. Counted in the report's own `## Observations` section the
      numbered items are four; only the first is the one this bullet goes on to describe, and
      the other three are recorded in the sibling bullet below rather than merely counted. The
      retired claim is kept visible here per this file's convention. The lesson is not about
      timing: a miscount of a document you are quoting while you quote it is an uncounted
      count, and the repair is to count it.]** The FIRST observation is
      the one that produced the rename: the verb `Authors` sat beside
      `ShippedGameModeAuthorsOneAiSide`, where that verb is genuinely earned by a premise
      block that proves the C++ default cannot supply the expectation -- and here it is not
      earned at all, since the bullet two above says in terms that this clause CANNOT
      distinguish an authored `Easy` from an untouched one. The report calls this "the
      sharpest thing in the pass" and records deliberately that it is NOT a block, for three
      stated reasons: no prose anywhere asserts the false claim, the file's own header
      intercepts a reader arriving by the name within a few lines, and the assertion message
      itself ends "Do not cite this clause as evidence that a designer set the tier." It
      offered `ShippedGameModeRunsAtEasy` "or similar" as a rename, called it a lane decision
      rather than a gate fix because it would move the suite's clause-name set and every
      citation of it, and named this lane as owner if pursued. The user ruled on the
      observation and named the replacement. This lane did not choose it.
    - **THE OTHER THREE OBSERVATIONS, IN SUBSTANCE -- so this entry is not merely counted
      right while still dropping what was raised.** All three are non-gating; none is a
      finding.
      - **(2) The `autocrlf` warning this file reports is real and is PRE-EXISTING.** The
        gate confirms the lane's account: `git diff` on the `.cpp` emits "warning: in the
        working copy of ... LF will be replaced by CRLF the next time Git touches it"; the
        file is LF in the worktree and LF in the HEAD blob, and this pass preserved that.
        Worth watching at staging time; not a change this diff made. It is the same condition
        the last bullet of this entry reports from the lane side.
      - **(3) The liveness control compares STRINGS, not arrays -- and that property is not
        free.** `TestNotEqual(..., Describe(A), Describe(B))` compares the rendered `(1)`
        against `(0,1)` rather than the two `TArray<int32>`s. The gate judges it correct
        HERE, because the values are digits and commas, so the case-insensitivity of
        `FString` comparison in this engine cannot bite, and because the string form is what
        the failure message needs anyway. It is recorded only so that a future edit rendering
        a CASE-BEARING field through the same helper knows the property is not free. **That
        is a live caveat on this lane's own instrument and is exactly what this file exists to
        hold: the day someone routes a name, a tag or a tier label through the same describe
        helper, this control stops discriminating case, and it will not say so.**
      - **(4) Obligation (1) of the in-session clause is unfalsifiable from a checkout, as
        `CLAUDE.md` itself declares.** The gate verified that `global.md`'s banner SAYS both
        routes were offered with their costs named, and which route the user chose, in the
        required inline shape; it records that it could NOT verify that the offer happened,
        and states which of the two it did. It concerns no clause and no test, and the fact
        it is about belongs to `global.md`, not to this file.
    - **Later re-gates of this same tree are on disk and are NOT enumerated here.** Read them
      by the glob `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate*.md`
      and count them yourself. A number, a "both", or a terminated list of them written into
      this entry would be a count of a set that this very edit can grow -- the defect that
      produced the correction stamped above, one layer down.
    - **Why five correct disclaimers did not make the old name acceptable.** They did not
      travel with it. The name alone reaches `Saved/AutomationReport/index.json`, a CI
      summary line, a `grep` hit, and the "+1 added" line of this file and `global.md`; the
      header, the assertion message and the four caveats above all stay behind. A reader who
      meets only the name concludes exactly the authorship the disclaimers exist to prevent.
      **Generalise it: an overclaiming clause name is a defect in its own right, not a
      cosmetic one, and no amount of correct surrounding prose repairs it.** Judge a name by
      what it asserts to someone who will never read the file.
    - **What the rename did NOT change.** Not one assertion, not the liveness control, not
      the expectation, not the warrant. Everything the bullets above record about what this
      clause pins and fails to pin still holds verbatim under the new name; only the label
      moved. The two mutants recorded below were run against the OLD name and were not
      re-run, because a rename cannot alter what they proved -- but note that as a limit,
      not as a re-measurement.
    - **How the rename was verified, since a rename is the one edit a stale binary reports as
      a success.** The editor target was REBUILT first (a clause name is compiled; an unbuilt
      tree would have reported the old name green and the new one absent). The full suite was
      then re-run and read from the exported `index.json` opened `utf-8-sig`, never from the
      log, which undercounts by exactly one. Both directions were asserted by name against
      the report: the old name ABSENT, the new name PRESENT and `Success`. The macro-based
      multiline set-difference against `HEAD` (`9d705ca`, which predates the whole clause)
      showed exactly ONE added and ZERO removed. **A collector asymmetry was caught in the
      middle of that check and is worth carrying forward:** the `HEAD` side of the difference
      initially swept 82 test `.cpp` files to the worktree side's 81, because a fossil copy
      of `StratScoreboardParity.cpp` is committed under
      `Tools/architect/evidence/02-scoreboard-widget/generated/Source/StratUI/Tests/` and a
      `git ls-tree` path filter reaches it while an `os.walk` of `Source/` cannot. It changed
      no verdict here -- its clause names duplicate the real file's -- but a set difference
      whose two sides enumerate different file sets is not measuring what it claims, and any
      future clause census must scope BOTH sides to `Source/` explicitly.
  - **Reported to the coordinator, NOT fixed here (out of lane).** `git diff` on that file emits
    `warning: ... LF will be replaced by CRLF the next time Git touches it`. The file is LF in
    the worktree and LF in the HEAD blob and this pass preserved that exactly; the warning is a
    pre-existing repo-wide `autocrlf` condition, not a change this pass introduced. Worth
    watching at staging time.

- **2026-09-03 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `0b17685`, UNCOMMITTED) -- **THE ASSET-PIN CLAUSE EXISTS, THE RETRACTION MARKERS ARE ANCHORED,
  AND THE THING WORTH READING IS THAT THE MUTANT'S OWN FAILURE MESSAGE IS WHAT PROVED THE
  READER WAS READING THE ASSET -- it printed the value it had read, `(1)`, while asserting the
  mutated `0`, so one line of output discharged falsifiability AND non-vacuity together.** Cite
  this pass's run by its exported `reportCreatedOn 2026.09.04-03.04.52`
  **[STAMPED 2026-09-04: that report no longer exists.
  `Saved/AutomationReport/index.json` is a SINGLE PATH that every run overwrites, and the
  2026-09-04 pass recorded above this entry re-ran the full suite over it. The citation is
  kept as the historical name of the run this entry describes; it is no longer re-readable
  from a checkout.]** (UTC in the report;
  local date 2026-09-03), never by a number -- the figure lives in
  `Tools/architect/state/global.md` and nowhere else. Follows the pass recorded immediately
  below, which the `coordinator` and the user reopened on three counts.

  - **THE CLAUSE.** `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide`, in the
    new file `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`. It loads
    `/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C` and reads the CDO's
    `MatchConfig.AiSides` and `MatchConfig.ViewingSide`. Counted by MACRO with a multiline
    set-difference against `HEAD` (`0b17685`) -- **exactly one clause name added, none removed,
    none renamed.** A single-line name grep returns zero here; the macro spans three lines.

  - **WHERE THE EXPECTATION COMES FROM.** A literal, `AiSides == {1}` and `ViewingSide == 0`,
    and it is legitimate as one because the module is not the author of the value -- the ASSET
    is, and a test that asked the asset and then asserted the answer would assert nothing. The
    reading is the `coordinator`'s off the live editor, 2026-09-03, with its three controls;
    the user ruled it intended. Same posture and same justification as
    `StratShippedGameModeOptIn.cpp`'s `true`, which the file's header cites by name.

  - **HOW IT FAILS WHEN IT CANNOT TELL, WHICH IS THE PART THAT MATTERS.** Four separate
    explicit failures, none of them a skip: the package failing to load, the path resolving to
    a NATIVE class rather than a Blueprint-generated one, the class not deriving from
    `AStratGameMode`, and a null CDO. Each names what to fix. A clause that goes quiet when the
    asset moves reads green while pinning nothing, which is worse than no clause -- it also
    occupies the space a real gate would go in.

  - **TWO CONTROLS, BOTH ASSERTIONS RATHER THAN REMARKS, BECAUSE A CDO READ IS ASYMMETRIC
    EVIDENCE.** A CDO reports the asset's override if there is one and the C++ default if there
    is not, and the two are indistinguishable at the call site. (1) A FATAL PREMISE asserts
    `FStratMatchConfig::AiSides` is EMPTY in C++, so the value sought is one the C++ default
    cannot supply and a reader stuck on the C++ default fails rather than passes. (2) The SAME
    reader is run against `BP_StratGameMode_AiVsAi` and must come back `(0,1)`, with an explicit
    `TestNotEqual` between the two packages' answers -- three distinct answers counting the C++
    default's empty, which is what makes it a reader OF AN ASSET rather than a constant.

  - **THE MUTANT, RUN AND SHOWN RED.** Expectation mutated in my own clause -- not in the
    `.uasset`, which is LFS and not this lane's -- from `ShippedAiSides[0], 1` to
    `..., 0`; rebuilt; run alone with its report exported OUTSIDE the repo so it could not
    clobber the suite report this entry cites. Result `Fail`, and the framework's own message:
    `Expected '§2.7: the AI seat BP_StratGameMode authors is side 1, the Vanguard (read: (1)).
    Side 0 is the Directorate and is the human's.' to be 0, but it was 1.` **The `read: (1)` in
    that message is the evidence, not the failure itself** -- it is the value taken off the CDO,
    printed by the clause, so the run proves the read reaches the asset and is not returning a
    constant or the C++ default. Then restored (verified byte-identical to the pre-mutant copy),
    rebuilt, and the full suite re-run green.

  - **WHAT THE NEW CLAUSE DOES NOT PIN, and its own header says each one.** It does NOT pin
    `ViewingSide` as an OVERRIDE: `FStratMatchConfig::ViewingSide` initialises to 0 and the
    asset reads 0, so on that one field the effective read cannot tell an authored value from an
    untouched one. The assertion still earns its place -- a shipped `ViewingSide` of 1 would put
    the human on the AI's seat, `StratHandicappedSide`'s second arm, and make §2.9's handicap
    silently inert in the shipped game -- but it pins the VALUE, not the authorship, and the
    assertion message says exactly that so a reader cannot mistake the one for the other. It
    also pins no BEHAVIOUR, no other field of the same struct, and not that `BP_StratGameMode`
    is the map's GameMode (that binding is a single unguarded line in `Config/DefaultEngine.ini`).

  - **THE MARKERS ARE RE-ANCHORED, AND THE COORDINATOR'S BRIEF WAS THE CAUSE, NOT A LANE
    ERROR.** I was told to match the engineer's inline `[CORRECTED ...; this comment read
    RETRACTED> "..."]` form; the tree already declared a different one and the declared one is
    the authority -- `StratSelectionMachineParity.cpp`'s `THE CONVENTION, and it is per-LINE and
    not per-paragraph` block: the marker's meaning comes from its POSITION at the head of the
    comment line, never from its presence, and *"a partial-line quotation is not retractable and
    must not be written."* All four of my markers were mid-line. Restructured so every withdrawn
    quotation occupies whole lines. **MEASURED, before and after, with the BEFORE state
    reconstructed by reverse-applying the patch to the current files rather than remembered:
    4 marker instances / 0 line-anchored, becoming 8 marker LINES / 8 line-anchored.** Withdrawn
    text surviving the declared anchored filter on the shipped-claim axis: **2 -> 0**. **AND THE
    CONTROL, because a zero survivor count is an absence and proves nothing until the instrument
    is shown able to speak: the same anchored filter still returns LIVE claims on the same axis
    -- 3 of them, including this file-pair's `SINGLE-PLAYER IS THE SHIPPED CONFIGURATION`
    paragraph.** It returned 4 before; the one that went away is the false `TestEqual` message
    corrected in the same pass, so the drop is a repair and not the instrument going quiet.

  - **WHY THE BEFORE FIGURE IS 2 AND NOT 4, STATED BECAUSE IT LOOKS LIKE AN UNDERCOUNT.** The
    axis regex is per-line, and two of the four withdrawn quotations had their claim phrase
    WRAPPED across a line break (`... the shipped` / `defaults ...`), so no single-line pattern
    could see them. They were equally unretracted; they were simply invisible to that instrument
    as well as to the filter. **The decisive figure is therefore the marker census -- 0 of 4
    anchored -- not the phrase survivor count**, and this is the same wrapped-claim blind spot
    this record has been bitten by before.

  - **THE `:360` MESSAGE, AND THE TWO BESIDE IT.** `T-FAME-02.HandicapIsInertWithoutASingle`
    `PlayerOpponent`'s first `TestEqual` message lost its false tail; it now reads *"a hot seat
    is handicapped on NO side -- ARM ONE, reached on FStratMatchConfig's C++ default and NOT on
    the shipped configuration"*. **The expectation, the value and the call are untouched.** The
    coordinator asked me to check the two messages below it in the same block: the `ScreenIsAi`
    message (*"the screen opening on an AI seat ... either both sides are AI or the
    configuration is inverted"*) and the `BothAi` message (*"phase D's AI-vs-AI configuration is
    handicapped on NO side"*) **carry no shipped claim and are both true** -- `BothAi` describes
    `BP_StratGameMode_AiVsAi`, whose `(0,1)` genuinely contains its `ViewingSide`. Neither was
    changed.

  - **WHAT IS STILL NOT PINNED AFTER ALL OF THIS.** `Difficulty` on the shipped asset. The
    Easy-tier clause reads it off a default-constructed config, and the C++ default and the
    asset agree at `Easy`, so nothing in the tree would go red if the asset's tier were
    re-authored to `Hard` -- the handicap would silently invert from +150 to -100 on the human's
    side and every clause would stay green. It is the same gap class this pass just closed for
    `AiSides`, on the field next to it, and the new file's header names it as unpinned. **Not
    written: outside what the user approved.**

- **2026-09-03 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `0b17685`, UNCOMMITTED) -- **A USER RULING FALSIFIED PROSE IN TWO TEST FILES AND THE THING
  WORTH READING IS THAT THE RULING MOVED NO EXPECTATION AND EXPOSED NO GAP: THE ARM THE SHIPPED
  GAME TAKES WAS ALREADY PINNED, TWICE, UNDER A CLAUSE NAME THAT DID NOT SAY SO.** Comment-only.
  No clause added, removed, renamed or weakened; no expectation, no fixture setup and no
  assertion argument changed -- `git diff -U0` over `Source/StratPlay/Tests/` yields ZERO
  changed lines that are not comment lines, which is the measurement, not the intent.
  **I did not run the suite** -- the coordinator is sequencing the rebuild-and-restamp after
  both lanes land -- so this entry cites no run and no figure. The figure lives in
  `Tools/architect/state/global.md` and nowhere else.

  - **THE RULING, AND WHOSE MEASUREMENT IT RESTS ON -- WHICH IS NOT MINE.**
    `BP_StratGameMode`'s authored `MatchConfig` carries `AiSides=(1)` with `ViewingSide` 0 and
    `Difficulty` Easy. **Read off the LIVE EDITOR by the `coordinator` on 2026-09-03**, with
    three controls: `BP_StratGameMode_AiVsAi` reads `(0,1)`,
    `struct_properties("StratMatchConfig")` gives the C++ default as empty, and
    `BP_StratShellGameMode` carries no `MatchConfig` at all -- so the instrument discriminates
    per asset rather than answering the same thing to everything. Neither this lane nor
    `strat-gameplay-engineer` took that reading, and every site I corrected attributes it to
    the `coordinator` in its own text rather than to the file it sits in. USER RULING the same
    day, BOTH halves: (a) the shipped game is human-versus-AI and is NOT a hot seat, and (b)
    Easy's +150 landing on the HUMAN side is intended, because Easy is meant to help the player.

  - **WHAT WAS FALSE, AND THE NARROWER THING THAT WAS NOT.** The false claim class is calling an
    EMPTY `AiSides` "the shipped configuration" / "the shipped default" / "the shipped hot seat".
    An empty `AiSides` is still `FStratMatchConfig`'s C++ default and is still what every fixture
    that leaves the field alone builds -- **both facts stand and were re-attributed rather than
    deleted**, because deleting them would have removed the reason those fixtures are correct.
    Every correction is stamped in place with this tree's `RETRACTED>` token quoting the old
    words, so the old claim is greppable and neither claim is left live.

  - **THE SITES, BY ENCLOSING SYMBOL AND CLAUSE, NOT BY LINE.** In
    `Source/StratPlay/Tests/StratAiMatchClauses.cpp`: the file-header property bullet 1 ("THE
    DEFAULT IS INERT ... the ordinary state of a hot-seat game"), and the guarded-path banner
    plus its `Base` provenance comment inside
    `FStratAiEmptyAiSidesRunsNoAiTurnTest::RunTest` (`T-INT-05.EmptyAiSidesRunsNoAiTurn`). In
    `Source/StratPlay/Tests/StratDifficultyHandicapMatchClauses.cpp`: the header block of
    `T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent` ("the shipped hot seat is the
    default configuration") and that clause's live-half fixture comment ("the shipped
    configuration §2.11.6 names"); the Easy-tier provenance comment in
    `T-FAME-02.DifficultyTierDeltasAreSection29sThreeNumbers`, which was NOT retracted and now
    says why; and a new paragraph on the header of
    `T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier`.

  - **THE ONE SITE THAT WAS NOT FALSE, AND IS NOW LABELLED AS SUCH ON PURPOSE.**
    `T-FAME-02.DifficultyTierDeltasAreSection29sThreeNumbers` reads `Difficulty` off a
    default-constructed config and its message says "§2.11.6's shipped default tier is Easy".
    **On THIS field the C++ default and the shipped asset agree** -- `BP_StratGameMode` also
    authors Easy -- so the message is true of both and nothing was retracted there. It is
    called out in the file because it sits three lines from claims about `AiSides` that were
    NOT true of both, and an unlabelled survivor beside four corrections reads as an oversight.

  - **THE "ARM THREE IS UNPINNED" GAP IS NOT REAL, AND THIS IS THE FINDING OF THE PASS.** The
    handoff asked me to confirm whether any clause pins the arm the shipped game actually takes
    -- `StratHandicappedSide` returning `ViewingSide` on `AiSides` non-empty and not containing
    `ViewingSide`. **Two do, and both build EXACTLY the shipped field values.** In
    `StratDifficultyHandicapMatchClauses.cpp`, `kViewingSide` is `0` and `kAiSide` is `1`; the
    CONTROL arm of `T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent` constructs
    `ViewingSide = 0, AiSides = {1}` and asserts `StratHandicappedSide` returns `0`, and
    `T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier` runs a LIVE match on that same
    configuration and pins the player's opening Fame moving by every tier's delta, Easy's +150
    included, while the AI side stays on the scenario's configured value. **What was missing was
    a NAME, not a clause**: both call the configuration "single-player", the ruling's news is
    that single-player IS the shipped game, and the file's old prose pointed a reader at the
    empty-`AiSides` clause instead. That is now said in the second clause's header. **No clause
    is owed on this account and none was written this pass.**

  - **WHAT THESE CLAUSES STILL DO NOT PIN, STATED SO IT IS NOT INFERRED FROM THE ABOVE.** Nothing
    in `Source/*/Tests/` reads `BP_StratGameMode`'s authored `AiSides` and compares it to
    anything. Every clause named here constructs its own `FStratMatchConfig` in C++, so the
    coverage is of `StratHandicappedSide`'s arms and of the subsystem's behaviour on a given
    config -- **not of the ASSET being authored `(1)`**. If a content pass re-authored
    `BP_StratGameMode` to an empty `AiSides` tomorrow, every clause in both files would stay
    green and the corrected prose above would silently become false again.
    `StratShippedGameModeOptIn.cpp` is the shape that would close it -- it loads
    `/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C` and reads a CDO field, failing RED when
    it cannot resolve rather than passing. **Not written this pass: the user scoped it to prose.**
    **[STAMPED 2026-09-03 -- WRITTEN later the same day, on the user's approval, as
    `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide` in
    `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`, in exactly the shape this
    bullet names. See the entry above.]**

  - **[STAMPED 2026-09-03 -- CLOSED later the same day; see the entry above. The message now
    reads "ARM ONE, reached on FStratMatchConfig's C++ default and NOT on the shipped
    configuration". The coordinator lifted the off-limits instruction for this one string on
    the user's ruling.]** ONE FALSE CLAIM IS STILL LIVE IN THIS TREE AND I WAS FORBIDDEN TO
    TOUCH IT. In
    `T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent`, the first `TestEqual`'s message
    argument reads `"T-FAME-02: a hot seat is handicapped on NO side -- the shipped
    configuration"`. Its expectation is CORRECT and unchanged -- an empty `AiSides` is
    `INDEX_NONE` -- but the trailing four words are the exact falsified claim, and **an assertion
    message reaches the exported automation report**, which is the same defect class a gate
    blocked on 2026-09-03 for `"an AI-vs-AI config assembles at the shipped playback default"`.
    The dispatching brief forbade changing any assertion argument, so it stands. **Declared open
    here rather than fixed.** The neighbouring messages on the same call site are fine: the
    `ScreenIsAi`, `BothAi` and CONTROL messages make no shipped claim.

  - **[STAMPED 2026-09-03 -- CLOSED later the same day. The coordinator confirmed its brief
    was wrong, the engineer re-anchored its three markers and this lane re-anchored its four;
    see the entry above for the before/after measurement and its control. The finding below
    was correct and is kept for the reasoning.]** TWO `RETRACTED>` CONVENTIONS NOW COEXIST IN
    `Source/StratPlay/Tests/` AND THE ANCHORED FILTER ONLY UNDERSTANDS ONE. `StratSelectionMachineParity.cpp`'s declaration is explicit
    that the marker's meaning comes from its POSITION -- head of the comment line, after the
    leader -- and that **"a partial-line quotation is not retractable and must not be written"**,
    because a line-oriented filter can only subtract whole lines. The form
    `strat-gameplay-engineer` introduced on 2026-09-03 in `StratMatchSubsystem.cpp`, and which
    this pass was told to match, is INLINE: `[CORRECTED <date>; this comment read RETRACTED>
    "..."]`. **Consequence, measured by reading the declared recipe against my own output:** run
    `grep -vE '^[[:space:]]*(//|\*)[[:space:]]*RETRACTED> '` over either file I edited and the
    withdrawn sentences SURVIVE the filter, because their marker is mid-line. They are honestly
    labelled to a human reader and invisible-as-withdrawn to the sweep. I did not resolve this
    -- picking a convention across two lanes' files is not this pass's scope -- but a sweep that
    trusts the anchored filter will read six withdrawn quotations as live prose.

- **2026-09-03 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `283d711`, UNCOMMITTED) -- **GATE FINDING F1 IS REPAIRED, AND THE THING WORTH READING IS THAT
  THE FINDING WAS NARROWER THAN THE DEFECT: THE REVIEWER NAMED THREE SITES IN ONE FILE, AND A
  CLAIM-SHAPE SWEEP FOUND TEN IN TWO, PLUS ONE STILL-OPEN INSTANCE ON A DIFFERENT PROPERTY IN A
  FILE THIS WAVE NEVER TOUCHED.** No clause was added, removed, renamed or weakened; nothing but
  comments and `TestTrue`/`TestFalse` message text changed. Cite the run of this pass by its
  exported `reportCreatedOn 2026.09.03-19.47.45`, never by a number; the figure lives in
  `Tools/architect/state/global.md` and nowhere else.

  - **THE MEASUREMENT THAT MAKES THE OLD PROSE FALSE, RE-TAKEN RATHER THAN INHERITED.**
    `AiPlaybackStepSeconds` is serialised in the raw bytes of
    `Content/StratPlay/BP_StratGameMode.uasset` and `Content/StratPlay/BP_StratGameMode_AiVsAi.uasset`
    and absent from `Content/StratPlay/BP_StratUnit.uasset`; `MoveTweenSeconds` gives the exact
    mirror-image result and a fabricated property name is absent from all three, so the
    instrument discriminates per asset and per property rather than answering PRESENT to
    everything. Instrument is `grep -aF` against the asset bytes. **`strings` is NOT usable for
    this and the coordinator measured it failing its own positive control** -- it reported every
    property absent including one known to be set. The override was authored in `1a3520b`
    (2026-08-29). **What I did NOT re-measure: the value 0.5.** The name-table hit proves the
    property is serialised, not what it holds; the float sits behind a name index, and the 0.5
    is `global.md`'s figure, cited as such in both files rather than claimed as mine.
    So `WillAiPlaybackRun()` is TRUE in every shipped GameMode, and `0.0f` is a C++ FIELD
    default that no real match runs at.

  - **WHAT `T-TURN-09.TheGateIsNeverRaisedWithoutATour` PINS, AND WHAT IT DOES NOT.** It pins
    that the existence hold cannot be raised at a NON-POSITIVE playback interval -- worth
    pinning, because the stranding it prevents has no recovery verb in the class. **It does NOT
    cover any path the shipped game currently takes**, and that is now stated in the clause's own
    header instead of the opposite. The prose used to read "this clause pins the behaviour on the
    path the game actually takes"; it now reads "THIS CLAUSE COVERS NO SHIPPED PATH", with the
    measurement above beside it. Its matched pair -- the three clauses running the identical
    fixture at a positive interval -- is unchanged and still carries the shipped-configuration
    coverage.

  - **THE SITES, BY ENCLOSING SYMBOL, NOT BY LINE.** In
    `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp`: the file header's
    `AiPlaybackStepSeconds` note; the positive-interval fixture control message shared by the
    three tour clauses; and inside `FStratGateIsNeverRaisedWithoutATourTest::RunTest`, its
    header block, its fixture-provenance comment, its config-assembly message and its
    per-departed-unit assertion message. In
    `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp`: the header of
    `T-TURN-09.PlaybackDispositionsAgreeOnState`, and in
    `FStratDefaultConfigConsumesNoInputTest` (`GATE-AITURN.DefaultConfigConsumesNoInput`) its
    title line, its two historical-narrative sentences, its fixture comment and three assertion
    messages; plus the positive-interval fixture note in the reseed clause below it. **Three of
    those were TEST MESSAGES, which reach `Saved/AutomationReport/index.json` -- a false claim
    that ships is the reason this was a BLOCK and not an observation.**

  - **THE HISTORICAL HALF WAS REWORDED, NOT RE-DIAGNOSED.** `DefaultConfigConsumesNoInput`'s
    header narrates a real 2026-08-29 regression. I did not touch the diagnosis; I replaced
    "at the shipped default of zero" and "in the shipped configuration" with "at a zero
    interval" and "in a zero-interval configuration". That makes the sentences true without
    asserting anything new about content, which is the correct move for a lane that may not
    read the engineer's account back to it.

  - **THE SWEEP, AND ITS ONE OPEN RESULT.** Shape swept, not phrase: prose asserting that a C++
    field default is what the shipped game runs, with comment leaders stripped and lines joined
    so a claim wrapped across two `//` lines is visible. Run across ALL of `Source/*/Tests/`, not
    just the two new files. **`Source/StratBridge/Tests/StratRosterDeltaClauses.cpp` and
    `Source/StratPlay/Tests/StratRouteTweenUnitDouble.{h,cpp}` are CLEAN** -- the unit double in
    fact gets it explicitly right, recording that `BP_StratUnit` carries the shipped values and
    its own `0.25f`/`0.0f` are fixture input and never an expectation. **STILL OPEN, NOT MINE TO
    FIX THIS PASS: `Source/StratPlay/Tests/StratAiMatchClauses.cpp` calls an empty `AiSides` "the
    shipped defaults", and `AiSides` IS serialised in BOTH GameMode Blueprints** (measured in the
    same pass, same instrument, same controls). Same conflation, different property, in a file
    this wave does not touch, so I reported it rather than widening the diff under a
    single-finding repair. It is a comment and reaches no report.
    **Measured NOT to be instances, so nobody re-opens them:** `Difficulty` is ABSENT from both
    GameMode Blueprints, so `StratDifficultyHandicapMatchClauses.cpp`'s "§2.11.6's shipped
    default tier is Easy" is TRUE as written; and `StratShellMenuTimingClauses.cpp`,
    `StratShellBlueprintSlotParity.cpp` and `StratShippedGameModeOptIn.cpp` say "shipped default"
    about a value they READ FROM THE CDO, which is the correct use of the phrase.

  - **THE BUILD WAS RUN BEFORE THE SUITE, BOTH TIMES.** A clause name and a message are compiled;
    a stale binary would have reported the old text green. `Result: Succeeded` on both, and the
    loaded `Binaries/Win64/UnrealEditor-StratPlay.dll` was checked to carry the NEW message
    strings as UTF-16 and not the old one. The count did not move, which is what a
    comments-and-messages change should do.

- **2026-09-03 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `283d711`, UNCOMMITTED, over the UNCOMMITTED `strat-gameplay-engineer` existence hold in the
  same tree; **its `reportCreatedOn` citation below is STAMPED -- `Saved/AutomationReport/` has
  one report path and this pass overwrote it, so that stamp no longer resolves. [STAMPED
  2026-09-03]**) -- **THE FIVE CLAUSES FOR SECTION 2.11.2'S EXISTENCE HOLD ARE WRITTEN, AND THE
  THING WORTH READING IS THAT ONE OF THE FIVE BRIEFED MUTATIONS DOES NOT REDDEN ITS CLAUSE AND
  CANNOT -- THE LINE IT DELETES IS OVERWRITTEN BY THE NEXT STATEMENT BEFORE ANY OBSERVER
  EXISTS.** Two new files, five new macros, no existing clause touched. Cite the run of this
  pass by its exported `reportCreatedOn 2026.09.03-18.37.03`, never by a number; the figure
  lives in `Tools/architect/state/global.md` and nowhere else.

  - **WHAT EACH CLAUSE PINS, AND WHERE ITS EXPECTATION COMES FROM.** Not one unit id, hex, step
    index or population size is written in either file. **Both populations are a difference
    between two readings of `UStratMatchSubsystem::GetViewModel()`** -- the applied model before
    the hand-over and after it. "Departed" is the first less the second, "appeared" the second
    less the first. That is deliberately the SAME authority `EndAiPlaybackTour`'s destroy loop
    consults, and it is not a circularity: what is NOT shared is the private half.
    `RevealAfterStep`, `HideAfterStep` and `bTourExistenceHeld` are never read by a clause and
    cannot be, so the maps the feature builds are compared against a roster difference derived
    independently of them. A hold built from the wrong ids reddens all four.
    | Clause | Pins |
    |---|---|
    | `T-TURN-09.AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep` | a unit built during the hand-over is concealed when the tour begins, is revealed AT A STEP rather than at the release, and is visible after |
    | `T-TURN-09.AUnitKilledDuringTheHandoverSurvivesUntilItsStep` | its actor survives `RefreshPresentation`, and is gone once the tour is STEPPED to its end |
    | `T-TURN-09.EndingATourDestroysEveryRetainedCorpse` | `SkipAiPlayback` destroys the retained actors ITSELF, with no model applied afterwards; and the survivors are untouched |
    | `T-TURN-09.TheGateIsNeverRaisedWithoutATour` | at the shipped `AiPlaybackStepSeconds` default, nothing is retained and nothing is hidden |
    | `T-INT-05.ARefusedCommandLeavesNoRosterDelta` | `RosterDeltaOfLastCommand` is empty after a refused submit that follows an accepted one that was not |

  - **THE ONE THAT COULD NOT BE PINNED, AND IT IS A PROPERTY OF THE CODE AND NOT OF THE
    FIXTURE.** The brief's mutation for the first clause was *"in `BeginAiPlayback`, delete the
    `ApplyTourExistenceAtCursor();` call"*. **APPLIED IN PLACE AND REBUILT, THE CLAUSE STAYED
    GREEN**, and so did the other four. The cause is structural: `BeginAiPlayback` ends with
    `OnAiPlaybackTimer()`, which reaches `AdvanceAiPlaybackOneStep`, which calls the SAME PURE
    FUNCTION again at cursor 1 -- all inside one synchronous call, before `BeginAiPlayback`
    returns. So the deleted call's only effect is on the board at cursor 0, in a window the
    subsystem's own comment says no frame is rendered in and no headless clause can reach. **The
    deleted call is not dead: under the supplementary mutation below, unit 11 -- which appears
    at step index 0 and is therefore legitimately visible at cursor 1 -- becomes CONCEALED,
    which is exactly the cursor-0 hide that the unmutated `Advance` immediately undoes.** So the
    line does something; nothing can observe it. **WHAT WOULD CLOSE IT:** a `const` accessor on
    `UStratMatchSubsystem` reporting whether a given unit id is currently held concealed --
    or simply hoisting the existence apply to a point a caller can interleave with. **I did not
    make either change and must not.**
  - **SO THE CLAUSE WAS PROVED FALSIFIABLE BY A DIFFERENT MUTATION RATHER THAN LEFT UNPROVEN,
    BECAUSE NON-VACUITY IS NOT FALSIFIABILITY AND ITS OWN CONTROLS ALL PASS EITHER WAY.**
    Deleting `ApplyTourExistenceAtCursor();` from **`AdvanceAiPlaybackOneStep`** -- the call the
    reveal half actually rests on -- reddens it on all 19 appeared ids at once. That is recorded
    as the mutation this clause is pinned by; the briefed one is recorded as measured-inert.
  - **THE OTHER FOUR REDDENED UNDER EXACTLY THE BRIEFED ONE-LINE MUTATION**, each applied IN
    PLACE in the main tree, rebuilt, run, and reverted to byte-identical
    (`sha256` of `StratMatchSubsystem.cpp` and `StratBridge.cpp` equal to the pre-pass copies,
    and the `git diff` over every non-`Tests/` path unchanged at
    `b7016f268b4e843fb4b9e1d9d400ed18a8a3b0929937dbf135f0510d1ecd7609` after every one of the
    six mutant passes). **A diff was not accepted as a run: every red and every green below was
    executed.**

  - **THE FIXTURE'S OWN MEASURED SHAPE, WHICH A LATER CLAUSE MAY RELY ON AND A LATER READER
    SHOULD NOT RE-DERIVE.** One AI-vs-AI hand-over on Ferrum Crossing, 156 steps: **19 unit ids
    appear and survive, 4 depart having been in the opening roster, and all 4 are retained at
    the instant before the skip.** Of the 19, **18 are concealed at cursor 1** -- unit 11 is not,
    because it appears at step index 0 and the tour's first step is shown inside
    `BeginAiPlayback`. **That is why the first clause asserts "at least one is concealed" and not
    "all are": asserting "all" would be asserting a fixture property, and it would redden on the
    day the AI's first command changed.** The reveals are spread from cursor 9 to cursor 144,
    which is what makes "revealed at a step, not at the release" a real distinction here rather
    than a formality.
  - **AND THE THIRD CLAUSE'S SHAPE IS DECIDED BY THE ALTERNATIVE IT RULES OUT, NOT BY THE
    BEHAVIOUR IT ASSERTS -- WHICH IS WHY THE ABSENCE OF A `RefreshPresentation` CALL IN IT IS
    LOAD-BEARING AND MUST NOT BE TIDIED.** `EndAiPlaybackTour` destroys the corpses itself; the
    tempting refactor is to drop the gate and let the next `ApplyView` do it. A clause that
    refreshed anything between the skip and its read would **go green over that refactor
    forever**, and the refactor is the defect -- after a tour ends the next `ApplyView` may never
    come at all. The clause therefore reads the board with nothing applied since the skip, and
    says so in its own block.
  - **WHAT THESE FIVE DO NOT PIN, STATED SO NOBODY INFERS COVERAGE FROM THE COUNT.** (a) That
    the roster delta is RIGHT on the accepted path -- that the ids `RosterDeltaOfLastCommand`
    names are the ids `applyCommand` actually spawned and erased. The bridge clause reads the
    accepted delta only to establish one exists; the four `T-TURN-09` clauses reach the
    identities end to end, but through the view model rather than against `applyCommand`.
    (b) `ReplayLog`, which reaches `strat::replayLog` directly and never passes through
    `Submit`, so a log loaded in a fresh process leaves the delta untouched -- the same gap
    `Submit`'s own block already records for the `STRAT-COMBAT` family. (c) A COUNTER-KILL: the
    engineer's own measurement says no AI-vs-AI hand-over in this suite produces an attacker
    death, so the departed set here is always defenders. (d) A unit BUILT AND KILLED inside one
    hand-over, which is out of the feature's scope by construction and is not contradicted here.
    (e) That a retained corpse stands at the right HEX -- the engineer names that limit and no
    clause of mine reaches it.
  - **NO NEW FLAKE OBSERVED.** No clause here registers an `FOutputDevice` on `GLog`, so the
    `CanBeUsedOnMultipleThreads` hazard this file records four times does not apply to either
    new file. Both `AddExpectedMessagePlain` declarations were taken from a measured run of the
    same fixture shape, not added defensively -- `Occurrences 0` FAILS a clause where the
    message never fires, so a speculative declaration is itself an assertion.

- **2026-09-02 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `283d711`, UNCOMMITTED, over the UNCOMMITTED `strat-gameplay-engineer` widening of
  `IStratAiTurnPort::Submit` to `FStratAiCommandEffect` in the same tree) -- **THE SECOND
  SIGNATURE WIDENING IN ONE DAY BROKE THE SAME FOUR SITES, AND
  `FStratAiPlaybackStep::AppearedUnitIds` / `DepartedUnitIds` ARE NOW PINNED BY THE SAME
  ORDERING CLAUSE THAT WOULD OTHERWISE HAVE SHIPPED THEM UNASSERTED.** The thing worth reading
  is **THE RATIO CONTROL**: a clause that merely said *"some step carries a roster delta"* is
  passed by the obvious WRONG mechanism -- one that reports a departure on every accepted
  Attack, since `FStratAiCommand::TargetId` sits right there and looks like a death identity.
  The clause's own measured run says **68 Attacks and 7 departures**, so what it asserts is that
  departures are a small MINORITY of attacks and not merely fewer than them. Built and run; the
  figure lives in `Tools/architect/state/global.md` and nowhere else. Cite the last run of this
  pass by its exported `reportCreatedOn 2026.09.03-01.26.54`, never by a number.
  - **NO NEW CLAUSE WAS MINTED, AND THAT IS CHECKABLE RATHER THAN ASSERTED.** Nothing here adds
    an `IMPLEMENT_SIMPLE_AUTOMATION_TEST`; the work is four signature repairs plus assertions
    inside one existing clause. Set difference on the test MACRO (multiline, one collector over
    both sides) against `283d711` is **+11, −0**, byte-for-byte the same eleven names the entry
    below reports -- so a total that MOVED in this pass would be somebody else's change.
  - **THE FOUR REPAIR SITES, AND A FIFTH THE COMPILER FOUND THAT THE BRIEF DID NOT.** The
    dispatch brief named four sites from the engineer's own compiler output and all four were
    right: `FStratRecordingAiPort::Submit` (`Tests/StratAiPlaybackClauses.cpp`),
    `FStratFaultInjectingAiPort::Submit` and `FStratNeverEndingAiPort::Submit`
    (`Tests/StratAiTurnRunnerClauses.cpp`), and the local in
    `Stratocracy.StratPlay.T-AI-01.EveryKindRoutesToARecordingSubmit`. **THE FIRST BUILD AFTER
    THE REPAIR STILL FAILED, ON A FACT NO GREP OF THE FOUR SITES COULD HAVE PRODUCED:**
    `StratAiTurnRunner.h` only FORWARD-DECLARES `FStratAiCommandEffect` -- it takes it by
    reference and touches no member -- so a file that CONSTRUCTS one or calls `Reset()` on it
    needs the definition from `StratAiPlayback.h`. Measured: `C2027 use of undefined type` at
    `FStratNeverEndingAiPort::Submit`, `C2079` on the local in
    `EveryKindRoutesToARecordingSubmit`, and a `C2664` cascading from the `C2079`, each citing
    the header's own forward declaration as the note. `Tests/StratAiTurnRunnerClauses.cpp` grew
    the include and says why at the include. `Tests/StratAiPlaybackClauses.cpp` needed nothing
    -- it already includes that header for the reel. **A parameter widening that changes a type
    from "passed through" to "constructed" changes which files need the definition, and that is
    invisible at every call site.**
  - **WHO CLEARS THE EFFECT IS UNCHANGED IN SUBSTANCE AND WIDER IN SCOPE, WHICH IS WHY THE
    STRUCT IS THE RIGHT SHAPE.** `FStratRecordingAiPort` and `FStratFaultInjectingAiPort` still
    forward straight through and add NO second `Reset()`: the inner port is an implementation of
    the interface and the contract puts the clear at entry of every implementation.
    `FStratNeverEndingAiPort` still clears for itself and is still the only double that must --
    it accepts without applying and has no inner port -- but **it now clears THREE things rather
    than one**, and `FStratAiCommandEffect::Reset()` is one verb over all three precisely so
    that site cannot clear the route, forget the roster delta, and attribute command N's
    appearances to command N+1. **That omission would not fail to compile.** Every one of those
    prose blocks was corrected in place rather than left naming `OutMoveRoute`, a parameter that
    no longer exists under that name; the only two surviving mentions are the two `[WIDENED
    2026-09-02 ... FROM]` citations, which are history and read as history.
  - **WHAT THE NEW ASSERTIONS PIN, AND WHERE THE EXPECTATION COMES FROM.**
    `Stratocracy.StratPlay.GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder` gained a fourth
    arm. `FStratRecordingAiPort` now keeps `AcceptedAppeared` and `AcceptedDeparted` beside
    `AcceptedRoutes`, **appended in the same statement group as `Accepted`**, fed by the widened
    `Submit`. The expectation is READ from what the production port wrote into `OutEffect` at
    the instant the rules module accepted the command; the bridge is never asked for a roster
    after the turn, because a roster read afterwards is a roster of a board the turn has already
    moved -- the same trap the route arm's own comment names. The non-circularity property is
    preserved verbatim: the reel's append is one stack frame ABOVE and in a different file, so
    the four lists are still built by two writers at the same instant from the same value.
    Comparison is element-wise AND IN ORDER, with no sort and no set semantics, because
    `AppearedUnitIds`' own block promises the reel copies the observation verbatim with no
    de-duplication and no reordering -- a comparison that sorted first would go green over a
    reel that reordered what it was handed.
  - **THE CONTROLS, AND THE SECOND ONE IS THE POINT OF THE WHOLE ARM.** (1) The count control,
    on this clause's own measured lesson from earlier the same day: element-wise equality over
    two EMPTY arrays passed every step with no error and only a count reddened, so at least one
    step must add a unit and at least one must remove one. (2) **THE RATIO CONTROL, WHICH A
    "SOME STEP HAS A DELTA" CLAUSE WOULD NOT HAVE.** A mechanism that reported a departure on
    every accepted Attack satisfies (1) completely. It cannot be right -- `AppearedUnitIds`'
    block records that the Attack arm erases a unit only when `defHpAfter <= 0` or when the
    counter kills the attacker -- so the clause requires `DepartingSteps * 2 < AttackSteps`. Its
    own printed measurement on the green run: *"over 11 AI turns and 156 recorded steps: 22
    steps added a unit, 7 removed one; 22 Builds and 68 Attacks were accepted"*. The populations
    are taken from the PORT's accepted `Kind`, so "how many Attacks were there" is the rules
    module's answer and not the reel's.
  - **THE ROSTER ARM PLAYS ITS OWN HAND-OVER AND THAT IS THE CONTROL, NOT TIDINESS.** The route
    arm stops the moment the AI accepts one Move, which on Ferrum Crossing is the first turn --
    8 steps, no combat, no deaths. A DEPARTURE needs a kill, and the ratio needs a real
    population of Attacks, so the new arm plays until §2.8 ends the match or a bound of 40
    turns. It ended at turn 11 with the rules module refusing the closing EndTurn
    (*"[T-SAVE-05] no match is running"*), which `IStratAiTurnPort`'s own amendment classifies
    as an ordinary end. It reads the reel through `FStratAiPlaybackReel::StepAt` rather than
    `Peek`, so it neither depends on nor disturbs a cursor.
  - **THE THREE CONTROLS THE BRIEF NAMED ALL HELD, UNADJUSTED, IN `reportCreatedOn
    2026.09.03-01.26.54`.** `T-TURN-09.PlaybackDispositionsAgreeOnState` (no world, no
    subsystem), `T-INT-05.ApplyViewSpawnsMovesAndDestroys`, and the A-then-B clause of that same
    shape in `Tests/StratGuidanceRouteClauses.cpp` -- which is
    `T-INT-05.ReconcileCompletesWithNoStripConfigured`, named here because the brief cited it by
    line number and a line number written during a diff is invalidated by that diff. All three
    drive `ApplyView` with no reel and all three report `Success`, so the tour's existence gate
    is down when no tour is running. `T-AI-01.EveryKindRoutesToARecordingSubmit`, whose local
    was the fourth repair site, is green too.
  - **FALSIFIABILITY: UNDISCHARGED, AND THE MUTATION IS NAMED.** In
    `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` this lane may build a mutant IN THE
    DOUBLE, and did not, because the sharper mutations are in production code this lane may not
    touch. **The one-line mutation that must redden the new arm:** in
    `FStratAiTurnRunner::RunTurn` (`Source/StratPlay/StratAiTurnRunner.cpp`), pass a
    default-constructed `FStratAiCommandEffect{}` to `FStratAiPlaybackReel::Record` instead of
    the effect the port filled -- the reel then carries empty sets on every step, **every
    element-wise equality above stays green**, and only the two count controls redden. **A
    second, sharper one for the ratio control:** in `FStratBridgeAiTurnPort::Submit`
    (`Source/StratPlay/StratAiTurnRunner.cpp`), append `Command.TargetId` to
    `OutEffect.DepartedUnitIds` on every accepted Attack rather than on an observed erasure --
    the wrong mechanism the ratio exists to catch. It leaves both count controls green and must
    fail on `7 * 2 < 68` becoming `68 * 2 < 68`. **Neither was built and neither may be cited as
    discharged**; the engineer runs them.
  - **TWO THINGS OUT OF REACH, RECORDED SO NOBODY WRITES A CLAUSE FOR THEM.** (1) A unit BUILT
    AND KILLED inside one hand-over has **no actor at all** -- it is in neither the pre-hand-over
    roster nor the final model -- and three of this run's seven deaths are such units. They are
    deliberately shown neither being built nor dying, and **no clause here asserts they are
    shown.** (2) A retained corpse stands where the last reconcile put it, which is wrong for a
    unit that moved before dying. That is a named debt of the engineer's and **nothing in
    `Tests/` pins it as correct.**

- **2026-09-02 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `283d711`, UNCOMMITTED, over the UNCOMMITTED `strat-gameplay-engineer` entry of the same day
  that made AI moves slide) -- **`AStratUnitActor::PlayRouteSlide`'S PARK IS PINNED BY FIVE
  `T-INT-05` CLAUSES AND A SKIP BY ONE `T-TURN-09` CLAUSE; A SEVENTH WAS WRITTEN, BUILT, RUN,
  FAILED ON THE SHIPPED TREE AND WAS DELETED.** The thing worth reading is that seventh:
  **`T-TURN-09.TheLastStepsSlideSurvivesTheTourEnding` was not wrong about the code -- it was
  right about the code and the SCENARIO could not reach its precondition**, and the measurement
  that says so is the only reason the arm-last ordering is now recorded as a gap instead of as
  coverage. Built and run; the figure lives in `Tools/architect/state/global.md` and nowhere
  else. Cite the last run of this pass by its exported `reportCreatedOn 2026.09.02-22.53.33`,
  never by a number. The failing run of the deleted clause is `reportCreatedOn
  2026.09.02-22.50.13`, which no longer exists on disk -- there is one report path and this
  pass's own final run overwrote it.
  - **THE SIX CLAUSES AND WHAT EACH PINS. Read this before citing any of them as coverage.**
    All five `T-INT-05` clauses are appended to `Source/StratPlay/Tests/StratMatchReconcile.cpp`
    under their own banner, reusing `StratRouteTween::Arrange` and its board-discovered route;
    the `T-TURN-09` clause is in `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp`, which is
    the only file with an AI-vs-AI harness and therefore the only place `SkipAiPlayback` can be
    reached at all. **NO EXISTING CLAUSE WAS EDITED OR WEAKENED.**
    - `T-INT-05.RouteSlideParksOverTheRoutesLastHex` -- the RE-SCOPED invariant, and the sibling
      `TweenWaypointsFollowTheNotedRoute` is now scoped against. That clause asserts the terminal
      waypoint is EXACTLY zero because `ApplyUnitView` APPENDS it; `PlayRouteSlide` COMPUTES its
      terminal and it is zero only when the route ends on the actor's own hex. This one takes the
      other case -- a leg that stops one hex short of the anchor, which is every step but a
      unit's last in a real tour -- and requires the terminal to be NON-ZERO and to stand over
      the route's last hex. Expectation is `AStratBoardActor::WorldLocationOfHex`, never
      computed. Printed on the green run: *"unit 1 slid over 3 hexes anchored on (1,3) and parked
      100.0 uu away, on a 100.0 uu tile"*.
    - `T-INT-05.AFirstRouteSlideArmsANonDegeneratePolyline` -- **the defect the engineer caught
      by READING and that no other clause in the tree can see.** The dispatch brief for that pass
      required `TweenOffsets[0]` to be overwritten unconditionally with the drawn position; at a
      tour's start every picture is at exact relative zero, so that expression is ZERO while the
      route's start is a hex away, and a ONE-HEX move -- two waypoints, the commonest reel entry
      -- becomes `{0, 0}`, arc length zero, `FinishTween`, and **nothing animates with a green
      build**. The clause uses the two-waypoint case and asserts the duration, the waypoint count
      and the polyline's own length, because the arc-length guard retires the tween outright and
      each of the three sees the collapse at a different point.
    - `T-INT-05.RouteSlideWaypointsSitAtTheDrawnUnitsHeight` -- the anchor's `BodyZOffset`
      cancellation on the NEW path, mirroring `TweenWaypointsSitAtTheDrawnUnitsHeight` on the
      old one. The lift is READ BACK off the drawn actor (`GetActorLocation().Z` minus the
      board's Z for the anchor hex) and only asserted non-zero, so no number is named. Printed:
      *"the drawn unit sits 37.0 uu above the tile plane"*.
    - `T-INT-05.AParkedPictureSurvivesAnApplyWithIdenticalPositions` -- **the hover hazard, and
      it is the half a naive clause omits.** `AStratPlayerController::Tick` reaches `ApplyView`
      whenever the cursor crosses a hex boundary, so `ApplyUnitView` fires DURING a tour; an
      unconditional park clear would pop every parked unit on the next mouse move. The clause
      applies the identical model twice with a park created in between and compares the waypoints
      element by element AND the picture's own offset, which is required NON-ZERO before and
      after -- an equality between two absences would report a destroyed park as intact.
    - `T-INT-05.AnActorThatMovedWithNoRouteClearsItsPark` -- the other half of the same `if`:
      the actor genuinely moved, nothing armed, so the park is HARD-CUT. `IsZero()` with no
      tolerance immediately after the apply is what distinguishes a hard cut from the "gentler"
      shape the branch's own comment records as wrong, which would walk the polyline to the park
      first. `Tick` never runs here, so a picture merely SENT home would still be parked.
    - `T-TURN-09.SkipLeavesNoPictureStranded` -- §2.11.2's *"any click or Esc skips to the end
      state"*, extended to the pictures. It drives a real AI-vs-AI tour by hand until some unit
      is genuinely parked, skips, and requires EVERY unit actor to report zero waypoints and a
      picture at exact relative zero. Printed: *"at step 2 of 156, unit 4 is mid-slide over 4
      waypoints with its picture 360.6 uu from its transform"*.
  - **THE ARM-LAST ORDERING IS PINNED BY NOTHING, AND THIS IS A MEASUREMENT AND NOT A JUDGEMENT
    CALL.** `UStratMatchSubsystem::AdvanceAiPlaybackOneStep` arms the slide LAST -- after
    `FocusPlaybackStep`, after `Advance()`, and after the last-step `EndAiPlaybackTour()`, which
    cancels every unit's slide. A picture still in flight once the tour has ended could only
    have been armed after that call, so a clause was written on exactly that observation, built,
    and run. **IT FAILED ON THE SHIPPED, UNMUTATED TREE** (`reportCreatedOn
    2026.09.02-22.50.13`, its own message: *"Expected 'the tour's final step armed a slide that
    outlived the tour ...' to be not null"*): the AI-vs-AI hand-over on Ferrum Crossing records
    **156 steps and not one picture is in flight when the last of them has been shown.** Since no
    production file was mutated, the arming cannot have been moved, so the cause is the other one
    the clause itself named -- **this scenario's hand-over does not end on a `Move` that arms**,
    and `UStratMatchSubsystem::PlayMoveSlideForStep` refuses six ordinary ways. **THE CLAUSE WAS
    DELETED RATHER THAN SOFTENED INTO "ASSERT ONLY IF THE LAST STEP HAPPENED TO BE A MOVE"**,
    because a clause that decides what to assert from the data it is looking at reports a defect
    and a quiet scenario identically. **NO CLAUSE MAY BE CITED AS COVERING THAT ORDERING, AND
    MOVING THE ARM ABOVE THE `EndAiPlaybackTour()` CALL YIELDS A GREEN SUITE.** Closing it needs
    a reel whose final step is a surviving unit's `Move` -- a fixture scenario authored for it,
    or a seam that lets a clause place a step on the reel. Neither exists and this lane asked for
    neither.
  - **AND `AdvanceAiPlaybackOneStep` STILL ARMING NO TIMER IS UNOBSERVABLE BY COMBINATION, WHICH
    IS A SHARPER FINDING THAN "NO SEAM".** `UStratMatchSubsystem::AiPlaybackTimer` is private, so
    no `FTimerManager` query reaches it from `Tests/` -- that much
    `GATE-AITURN.StepFocusesAndStopsOnTheLast`'s banner already recorded for its own half. **A
    SEAM WOULD NOT FIX IT.** While a tour plays, a one-shot on that handle is ALWAYS already
    pending (`BeginAiPlayback` arms one through `ArmNextPlaybackStep` before any clause can
    look), and `FTimerManager::SetTimer` on a pending handle REPLACES it -- so relocating the
    re-arm into `AdvanceAiPlaybackOneStep` changes no observable state in a fixture that never
    ticks, and in one that did it would change only the INTERVAL, which is a pace and
    `AiPlaybackStepSeconds`' own declaration rules that no test may assert one. Derived by
    reading before any clause was written for it; no clause was written.
  - **THE DOUBLE GREW TWO CONST READS AND ITS HEADER'S "NO BEHAVIOUR IS OBSERVED HERE" IS
    RETRACTED IN PLACE.** `AStratRouteTweenUnitDouble::HasBody` and
    `AStratRouteTweenUnitDouble::BodyRelativeLocation` were added, because
    `AStratUnitActor::Body` is protected and **`GetTweenWaypointCount() == 0` REPORTS "THE
    PICTURE IS HOME" AND "THE PICTURE IS STRANDED OVER AN INTERMEDIATE HEX" IDENTICALLY** -- both
    have an empty polyline, and the only distinguishing fact is `Body`'s relative location.
    Without them `SkipLeavesNoPictureStranded` and `AnActorThatMovedWithNoRouteClearsItsPark`
    would both pass over an implementation that emptied the arrays and left the picture parked
    forever. `HasBody` is a SEPARATE question on purpose: a null `Body` answers zero, which is
    the same answer a picture at rest gives, so a clause asserting "exactly zero" would otherwise
    be satisfied by an actor with no picture. **No virtual is replaced and no component is
    added**; `BodyRelativeLocation` returns `USceneComponent::GetRelativeLocation()` verbatim and
    derives nothing.
  - **THE ANCHOR IS THE PRODUCTION ANCHOR, AND GETTING IT WRONG WOULD HAVE MADE THE
    NON-DEGENERACY CLAUSE INERT -- DERIVED BEFORE WRITING, NOT AFTER.**
    `UStratMatchSubsystem::PlayMoveSlideForStep` anchors on `AppliedModel`'s entry for the unit,
    and `RunAiTurnsNow` has ALREADY reconciled the board to the FINAL state before a tour begins
    -- so the anchor is the unit's DESTINATION and the actor is already standing on it. Every
    clause therefore applies `Moved` first, through `StratRouteSlide::ArrangeAtDestination`,
    which asserts the picture is at EXACT relative zero before anything is armed. **Anchoring on
    the actor's STARTING hex instead would have made waypoint 0 exactly zero on the shipped code
    too, at which point the mutation the clause names produces the identical polyline and the
    clause pins nothing** -- the same equivalent-mutant shape that made two clauses inert in the
    entry below.
  - **FALSIFIABILITY: UNDISCHARGED, ALL SIX, AND THE ENTRY BELOW IS WHY THAT IS SAID PLAINLY.**
    Each clause names its exact one-line mutation in its own banner -- all of them in
    `Source/StratPlay/StratUnitActor.cpp` or `Source/StratPlay/StratMatchSubsystem.cpp`, which
    this lane may not edit even temporarily -- and **NOT ONE WAS BUILT.** In the entry below,
    two of five clauses passed their own named mutation on the first pass with every vacuity
    assertion present and green. **A NAMED MUTATION IS A PROMISE AND NOT A MEASUREMENT**, and
    nothing here may be cited as discharged until `strat-gameplay-engineer` has run it. The six
    mutations, in one place: the offset loop's `Point - AnchorWorldPoint` changed to
    `Point - RouteWorldPoints.Last()` (parks); the same changed to `Point - GetActorLocation()`
    (height); deleting `PlayRouteSlide`'s `if (bSlideInFlight)` guard (non-degeneracy); dropping
    `bActorMoved &&` from `ApplyUnitView`'s park-clear `else if` (hover); deleting that branch's
    `CancelRouteSlide();` (hard cut); and deleting `EndAiPlaybackTour`'s per-actor
    `CancelRouteSlide` loop (skip). **The height mutant must redden the height clause ALONE** --
    every other assertion in these five is deliberately blind in Z -- and **the hover mutant and
    the hard-cut mutant must each redden one of that pair and leave the other green.**
  - **THE CONTROL HELD, UNTOUCHED.**
    `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState` -- whose fixture drives
    the runner and reel with no world and no subsystem, and which is what reddens the day
    somebody gives the reel a bridge -- reports `Success` in `reportCreatedOn
    2026.09.02-22.53.33`, with no adjustment. So do
    `GATE-AITURN.StepFocusesAndStopsOnTheLast` and
    `GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder`, the two clauses nearest this work.
  - **WHAT NONE OF THIS PINS, AND THE LIST IS AS LOAD-BEARING AS THE ONE ABOVE.** Both fixtures
    create their worlds with `bInformEngineOfWorld = false` and **NOTHING TICKS THEM**, so
    `AStratUnitActor::Tick` never runs and no timer ever fires. Nothing here observes a unit
    MOVING, the eased position at any instant, the trapezoidal profile, the wall-clock duration,
    that the tour WAITS for a slide, or the retirement of a completed slide. The seconds
    `PlayRouteSlide` returns are asserted only to be POSITIVE and never as a number, for
    `AiPlaybackStepSeconds`' own recorded reason that a pace has one home and a test is not it.
    **A green run says a polyline is the right shape and that a park is created, survived and
    cleared on the right occasions. It says nothing whatever about animation.** No test-only
    advance seam was asked for or added; a clause that wants the curve needs a PIE fixture.

- **2026-09-02 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit
  `283d711`, UNCOMMITTED, over the UNCOMMITTED `strat-gameplay-engineer` widening of
  `IStratAiTurnPort::Submit` in the same tree) -- THE TREE WAS UNCOMPILABLE AND FOUR TEST SITES
  WERE THE WHOLE OF IT; `FStratAiPlaybackStep::RouteHexes` IS NOW PINNED BY THE ORDERING CLAUSE
  THAT WOULD OTHERWISE HAVE SHIPPED IT UNASSERTED. **The thing worth reading is MUTANT B: with
  the route cleared on BOTH sides of the comparison, the element-wise equality passed over all
  eight steps with no error, and the ONLY assertion that reddened was the non-emptiness count.**
  An empty route and a forgotten route are the same bytes, and no equality can tell them apart.
  Built and run; the figure lives in `Tools/architect/state/global.md` and nowhere else. Cite
  the last run of this pass by its exported `reportCreatedOn 2026.09.02-22.09.31`, never by a
  number. That run was made from a rebuilt binary AFTER the mutants were removed and the file
  was verified byte-identical to its pre-mutant state -- clause names are compiled, and a suite
  run against a mutant binary is a different tree's result.
  - **THE FOUR REPAIR SITES, AND THE COMPILER'S OWN LIST AGREED WITH THE GREP EXACTLY.** The
    dispatch brief supplied four sites from a grep and asked whether the compiler found more.
    It found no more. Because the repair was written before the first build, the error set was
    measured by REVERTING both files and building again -- the honest way to answer, since a
    green build after a complete repair is silent about what would have broken. That build
    reported 20 errors in exactly two files and at exactly four distinct causes:
    `C3668`/`C2660` at `StratAiPlaybackClauses::FStratRecordingAiPort::Submit`, the same pair
    at `StratAiTurnRunnerClauses::FStratFaultInjectingAiPort::Submit`, `C3668` at
    `StratAiTurnRunnerClauses::FStratNeverEndingAiPort::Submit`, and `C2660` on the direct
    `FStratBridgeAiTurnPort::Submit` call inside
    `Stratocracy.StratPlay.T-AI-01.EveryKindRoutesToARecordingSubmit`. Everything else was
    `C2259 cannot instantiate abstract class` cascading from those three doubles -- eight
    instantiations of `FStratRecordingAiPort` alone. **`FStratAiPlaybackReel::Record`'s new
    required fourth parameter broke NOTHING in `Tests/`, and that is worth knowing rather than
    assuming: there is exactly one caller of `Record` in this tree and it is
    `FStratAiTurnRunner::RunTurn`.** No test constructs a reel's contents directly.
  - **WHO CLEARS `OutMoveRoute`, WHICH IS THE ONE PLACE THE THREE DOUBLES DIFFER.**
    `FStratRecordingAiPort::Submit` and `FStratFaultInjectingAiPort::Submit` forward straight
    through and add NO second `Reset()`: the inner port is an implementation of the interface
    and the contract puts the clear at entry of every implementation, so a clear in the
    decorator would be a second owner of one obligation.
    `FStratNeverEndingAiPort::Submit` DOES clear it itself, and is the only double that must --
    it accepts without applying and has no inner port, so nothing in its call chain would ever
    touch the array and it would silently inherit whatever the caller passed in. That omission
    would NOT fail to compile. The line says so at the line.
  - **WHAT THE NEW ASSERTION PINS, AND WHERE ITS EXPECTATION COMES FROM.**
    `Stratocracy.StratPlay.GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder` compares reel
    steps against `FStratRecordingAiPort::Accepted`, and `FStratAiCommand` carries no route
    counterpart, so `RouteHexes` had no independent value to be compared against at all. The
    double now keeps `FStratRecordingAiPort::AcceptedRoutes`, appended IN THE SAME STATEMENT
    GROUP as `Accepted`, fed by the widened `Submit`. **That preserves the clause's
    non-circularity property verbatim and does not weaken it**: the reel's own append is one
    stack frame ABOVE this one and in a different file, so the two lists are still built by two
    writers at the same instant from the same value. The expectation is READ from the
    production port's `OutMoveRoute`, never computed. `FStratBridge::MovePathToHex` is
    deliberately NOT re-asked after the turn -- it answers about the board as it stands and the
    board has moved, which is the trap the player path's own comment names by hand.
  - **WHAT IT DOES NOT PIN.** It does not pin that the route is the SHORTEST or the CHEAPEST
    path, nor that it agrees with what `MovePathToHex` would answer at any other instant; it
    pins that the reel carries, verbatim, whatever the production port handed out at the moment
    the rules module accepted that command. Nor does it pin any DRAWING -- the polyline the
    tween walks is the entry below's five `T-INT-05` clauses, and these two sets of clauses do
    not overlap.
  - **THE CONTROL, AND MUTANT B IS THE REASON IT IS NOT DECORATION.** Two mutants were planted
    IN LANE (in the double, not in production code), built IN PLACE, and run:
    - MUTANT A -- `OutMoveRoute.Reset()` AFTER `AcceptedRoutes.Add`, so the reel gets an empty
      route and the recorder keeps the real one. **Red**, naming the divergence:
      *"step 1 (Move) does not carry the route the port was handed: reel '<empty>' vs port
      '(-1,3) -> (0,2) -> (1,1) -> (2,1)'"*.
    - MUTANT B -- the same `Reset()` moved BEFORE both appends, so both sides are empty.
      **The element-wise comparison reported NOTHING.** The clause's own info line read
      *"over 1 AI turns and 8 recorded steps: 0 accepted Moves carried a route, 2 non-Move
      steps carried none"*, and the single error was the non-emptiness assertion. **A clause
      with the equality and without the count would have been green over a reel that carried
      no routes whatsoever.**
    The second half of the control -- every non-Move step's route is empty -- is guarded by its
    own count for the same reason, and the classification is taken from the PORT's accepted
    `Kind` rather than from the reel, so what may carry a route is the rules module's answer.
  - **THE MOVE-SEEKING LOOP IS HARDENING AND WAS NOT NEEDED ON THIS BOARD, WHICH IS STATED SO
    NOBODY DELETES IT AS DEAD.** The route block plays AI turns until the port has seen a Move
    ACCEPTED, bounded at 8, and fails loudly if it never does. On Ferrum Crossing the first
    turn already contains six accepted Moves, so the loop exits after one turn today. It exists
    because nothing guarantees §2.9's opening turn contains a Move -- an all-Build opening is a
    perfectly ordinary answer -- and a vacuity control that could be defeated by which command
    the AI happened to pick first is not a control. A turn that stops short is recorded with
    `AddInfo` and breaks the loop rather than erroring, because §2.8 can end the match inside a
    turn and `IStratAiTurnPort`'s own amendment classifies that refusal as an ordinary end.
  - **THE CONTROL CLAUSE HELD.** `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState`
    drives the runner and reel with no world and no subsystem and was required to stay green
    without adjustment -- it is what reddens the day somebody gives the reel a bridge. It is
    green in `reportCreatedOn 2026.09.02-22.09.31`, untouched.
  - **NO NEW CLAUSE WAS MINTED.** Nothing here adds an `IMPLEMENT_SIMPLE_AUTOMATION_TEST`; the
    work is assertions inside one existing clause plus three signature repairs. The clause
    inventory is unchanged by this pass, so a count that moved would be somebody else's change.

- **2026-09-02 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`,
  UNCOMMITTED, over the two UNCOMMITTED `strat-gameplay-engineer` entries of the same day in
  `Tools/architect/state/engine.md`) -- THE ROUTE TWEEN'S POLYLINE IS PINNED BY FIVE CLAUSES
  UNDER `T-INT-05` AND ONE NEW TEST-ONLY ACTOR DOUBLE -- **ALL FIVE NOW PROVEN ABLE TO FAIL BY
  A BUILT AND RUN MUTANT** -- AND THE THING WORTH READING IS THAT **TWO OF THE FIVE COULD NOT
  FAIL WHEN FIRST WRITTEN, WHICH ONLY THE MUTANTS FOUND: EVERY VACUITY ASSERTION I HAD WRITTEN
  WAS PRESENT AND GREEN IN BOTH.** Built and run; the figure lives in
  `Tools/architect/state/global.md` and nowhere else. Cite the last run of this pass by its
  exported `reportCreatedOn 2026.09.02-20.57.45`, never by a number.
  **[STAMPED 2026-09-02: that stamp IS NO LONGER ON DISK. There is one report path, and the
  route-widening pass recorded in the entry ABOVE deleted and rewrote it -- twice, once at
  `2026.09.02-22.04.03` and finally at `2026.09.02-22.09.31`. The clauses this entry describes
  were all green in that final run. The claim STANDS; only the citation is unre-executable, and
  it stays written rather than being edited to a stamp this entry's own pass did not produce.]**
  - **WHAT WAS GATED, AND IT IS THE THING THE ENGINEER'S OWN ENTRY NAMED AS UNGATED.** That
    entry closes on "the polyline is unpinned by any clause, so green here does not mean the
    unit walks the right hexes ... until one exists the only gate is a human watching a unit
    move." `AStratUnitActor::GetTweenWaypointCount` and `AStratUnitActor::GetTweenWaypoint`
    were added FOR that clause and say so in their own declarations. These are those clauses.
  - **WHERE THEY LIVE AND WHY THERE.** All five are appended to
    `Source/StratPlay/Tests/StratMatchReconcile.cpp`, under their own banner, below the two
    clauses that were already there. **A second file would have meant a second copy of
    `StratMatchReconcile::FTestWorldScope` and `StratMatchReconcile::MakeConfig`**, which are
    that translation unit's and are the exact fixture these need. Neither existing clause is
    edited: `Stratocracy.StratPlay.T-INT-05.ApplyViewSpawnsMovesAndDestroys` and
    `...NoActorHoldsPresentationBits` are byte-identical to `283d711` (verified by md5 over the
    first clause's whole body), and the new clauses configure a DIFFERENT `UnitActorClass` of
    their own AFTER calling `MakeConfig`, so nothing above them changes shape.
  - **WHAT EACH CLAUSE PINS AND WHERE ITS EXPECTATION COMES FROM. Read this before citing any
    of them as coverage.**
    - `StratPlay.T-INT-05.TweenWaypointsFollowTheNotedRoute` -- one waypoint per noted hex,
      each standing over `AStratBoardActor::WorldLocationOfHex` of the hex at the same index.
      **Asserted on the INTERMEDIATE waypoints, because a chord and a route agree at both ends
      and an endpoint-only assertion is passed by the two-point fallback this feature
      deleted.** The route is chosen to BEND and the bend is asserted twice -- on the hexes when
      the fixture is built, and on the waypoints AS DRAWN, where "every intermediate waypoint
      leaves the chord" is what a straight-line implementation fails. Measured on the green run
      and printed by the clause: *"unit 1 routed (-2,4) -> (-1,4) -> (0,4) -> (1,3); off-chord
      32.7 uu of a 100.0 uu tile"*. The terminal waypoint is asserted EXACTLY zero (`IsZero()`,
      no tolerance) because `AStratUnitActor::TweenOffsets`' own block says it is APPENDED
      rather than computed so the claim rests on construction.
    - `StratPlay.T-INT-05.TweenWaypointsSitAtTheDrawnUnitsHeight` -- the Z half, separately,
      because it is a separate defect and is invisible in XY. **This is the clause for the
      arithmetic the engineer caught while writing the comment**: differencing against the
      actor's location instead of the route's own last point sinks every intermediate waypoint
      by `BodyZOffset`. The lift is READ BACK off the drawn actor as `GetActorLocation().Z`
      minus the board's Z for the hex it stands on -- `AStratUnitActor::ApplyUnitView`'s own
      location expression inverted -- and only asserted NON-ZERO, so no number is named.
      Measured and printed: *"the drawn unit sits 37.0 uu above the tile plane"*.
    - `StratPlay.T-INT-05.NoNotedRouteArmsNoTween` -- an apply that MOVES a unit with nothing
      noted arms nothing. This is the structural form of the user's decision that AI moves
      snap: `UStratMatchSubsystem::NotePendingMoveRoute` has no caller but an accepted player
      command, so the map's emptiness IS the signal and no AI detection exists. The clause
      asserts the actor REALLY MOVED, without which it would also pass for clause 4's reason.
    - `StratPlay.T-INT-05.SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` -- the
      `&& bActorMoved` re-entrancy guard. The waypoint list is compared ELEMENT BY ELEMENT AND
      EXACTLY. **REWRITTEN THIS SAME PASS AFTER THE MUTANT PROVED THE FIRST VERSION INERT; ITS
      SUBJECT WAS UNPINNED UNTIL THIS PASS AND ANY EARLIER CITATION OF IT AS COVERAGE IS
      FALSE.** The rewrite is now discharged: the identical mutation that reddened nothing
      against the first version reddens this one. See the ledger below.
    - `StratPlay.T-INT-05.ASpawnedUnitDoesNotSlideEvenWithARouteNoted` -- **NEW THIS PASS, AND
      IT REPLACED `PendingRouteIsEmptiedByEveryApply`, WHICH IS RETIRED.** A unit leaves the
      model (its actor is destroyed), a route is noted that ends EXACTLY where it is about to
      reappear so `ApplyView`'s last-hex equality accepts it, and the unit comes back: it must
      arrive STANDING STILL. It pins two sentences nothing pinned before --
      `AStratUnitActor::SnapToWorldLocation`'s "a unit that just appeared must not slide in
      from wherever it was standing", and `UStratMatchSubsystem::ApplyView`'s spawn branch on
      what happens without it: "every unit on a fresh board would ease in from the world
      origin."
  - **THE MUTANTS WERE BUILT -- BY `strat-gameplay-engineer`, IN PLACE, IN THE FILES THIS LANE
    MAY NOT TOUCH, AND THAT DIVISION IS THE WHOLE REASON THE FINDING EXISTS.** I named five
    one-line mutations and could build none of them; the lane that owns
    `Source/StratPlay/StratUnitActor.cpp` and `Source/StratPlay/StratMatchSubsystem.cpp` built
    each, rebuilt, re-ran with the exported report deleted first, and restored both files
    byte-identically.
    - **DISCHARGED, THREE, each quoting the message its own clause produced.**
      `TweenWaypointsFollowTheNotedRoute` -- red, *"expected 4 waypoints, was 2"*.
      `TweenWaypointsSitAtTheDrawnUnitsHeight` -- red **ALONE**, *"waypoint 1 draws at the
      unit's own height over hex (-1, 4): Z 0.00, expected 37.00"*, **with clause 1 staying
      GREEN under the same mutation**, which is exactly what the pair was written to do.
      `NoNotedRouteArmsNoTween` -- red, *"expected 0, was 2"*.
    - **INERT, TWO -- ON THE FIRST PASS. THIS IS THE ENTRY'S FINDING AND IT IS ABOUT MY OWN
      WORK.** Both clauses passed on the shipped code, passed every precondition they asserted,
      and passed under the mutation they each named. **NON-VACUITY IS NOT FALSIFIABILITY** -- I
      had written the vacuity assertions (a tween armed at all, the route bends, the lift is
      non-zero, the actor really moved) precisely as the guard against a clause that measures
      nothing, and they were all live and all green in both inert clauses. A clause that names a
      mutation is making a PROMISE about that mutation, and only running it collects.
    - **AND THEN THE REWRITES WERE MUTATED IN TURN, WHICH IS THE HALF THAT MAKES THE FINDING
      ACTIONABLE RATHER THAN MERELY EMBARRASSING.** `strat-gameplay-engineer` ran two more
      mutants against the rewritten clauses, same protocol, both source files restored
      byte-identically on hash and md5 afterwards:
      - **Dropping `&& bActorMoved` in `AStratUnitActor::ApplyUnitView` --
        `reportCreatedOn 2026.09.02-20.47.31` -- reddened TWO clauses.**
        `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone`: *"Expected 'the second apply
        leaves the waypoint count untouched -- 4, not the chord's 2' to be 4, but it was 2."*
        **The identical mutation reddened NOTHING against the previous version**, so the
        discharge is attributable to the rewrite; and the count fell to the CHORD'S TWO, the
        predicted MECHANISM and not merely the predicted direction.
        `ASpawnedUnitDoesNotSlideEvenWithARouteNoted`: *"Expected 'a freshly spawned actor arms
        no tween, though a 4-hex route ending on its own hex was noted' to be 0, but it was 4."*
        -- the second, independent net that clause claimed.
      - **Deleting `Spawned->SnapToWorldLocation(Where);` from
        `UStratMatchSubsystem::ApplyView`'s spawn branch -- `reportCreatedOn
        2026.09.02-20.49.51` -- reddened `ASpawnedUnitDoesNotSlideEvenWithARouteNoted` AND THAT
        CLAUSE ALONE.** `T-INT-05.ApplyViewSpawnsMovesAndDestroys` stayed GREEN, and the reason
        is worth more than the discharge: `AStratUnitActor::ApplyUnitView`'s own
        `SetActorLocation` writes the destination microseconds later, so **the spawn snap is
        unobservable in the final POSITION and observable only in whether a tween ARMS**. No
        clause that reads a position after `ApplyView` can see that line at all; this one is the
        only thing in the tree that can.
      - Engineer's restored run after the mutants: `reportCreatedOn 2026.09.02-20.52.04`,
        non-success empty. **All three mutant runs are that lane's measurements and not this
        lane's** -- this lane owns no file it could mutate, which is the whole reason the
        division produced the finding.
  - **CLAUSE 4: WHY IT WAS INERT, AND WHY THE OBVIOUS REPAIR WOULD NOT HAVE FIXED IT EITHER.**
    It applied the same model twice and noted a route ONCE, and `UStratMatchSubsystem::ApplyView`
    empties `PendingMoveRoutes` on every apply -- so the second apply carried an EMPTY route and
    `AStratUnitActor::ApplyUnitView` refused at `RouteWorldPoints.Num() > 0`, several tokens
    EARLIER than the `&& bActorMoved` guard the clause claimed to be about. **THE REPAIR I WAS
    HANDED WAS "NOTE THE ROUTE AGAIN BEFORE THE SECOND APPLY", AND THAT ALONE LEAVES THE MUTANT
    EQUIVALENT** -- derived from the code before writing it, and stated here so nobody
    re-derives it: on the second apply `Drawn` is `Body->GetComponentLocation()` and `Body` is
    sitting at `TweenOffsets[0]`, so `Drawn - Destination` reproduces `TweenOffsets[0]` exactly,
    and every other waypoint is `RouteWorldPoints[i] - RouteEnd` over the same route -- **the
    rebuilt array is IDENTICAL ELEMENT BY ELEMENT, and a mutant that rebuilds the same numbers
    is invisible to any assertion about the numbers.** `TweenElapsed` restarting is the real
    cost of a re-arm and there is no accessor for it, nor did I ask for one. **SO THE SECOND
    APPLY NOW CARRIES A DIFFERENT AND SHORTER ROUTE -- THE CHORD.** Both earlier conditions are
    then satisfied and the guard is the only thing that can refuse the re-arm; without it the
    polyline collapses to two points, which is the failure `ApplyUnitView`'s own block names
    verbatim.
  - **CLAUSE 5: RETIRED AS AN EQUIVALENT-MUTANT CLAUSE, AND WHAT IT CLAIMED IS RETRACTED RATHER
    THAN QUIETLY DROPPED.** It said:
    RETRACTED> "`PendingRouteIsEmptiedByEveryApply` -- a route noted for a unit ABSENT from the
    RETRACTED>  next model must not reach a later apply ... WHAT WOULD REDDEN IT: replace the
    RETRACTED>  unconditional `PendingMoveRoutes.Reset()` with a removal of only the ids the
    RETRACTED>  unit loop consumed."
    It stayed green under that mutation and under two further diagnostics (dropping
    `&& bActorMoved`; dropping the last-hex equality). Its own middle step re-applied a model
    that CONTAINED the unit, which put the id back in the seen set, so the narrowed reset
    removed the route there anyway. **AND THE DEEPER REASON MATTERS MORE THAN THE CLAUSE DID:
    the hazard is foreclosed TWICE OVER without the map -- a unit absent from a model has its
    ACTOR DESTROYED, so on any later apply a stranded route could attach to, the actor is
    freshly spawned, snapped, and the actor-moved guard is false.** **SO NOTHING IN THIS TREE
    PINS `UStratMatchSubsystem::PendingMoveRoutes`' UNCONDITIONAL `Reset()`, NO CLAUSE MAY BE
    CITED AS DOING SO, AND NO CLAUSE SHOULD BE WRITTEN FOR IT -- EVERY MUTATION OF THAT LINE IS
    AN EQUIVALENT MUTANT AND A SUITE THAT DELETES IT IS GREEN.** That is a finding and not a gap
    to be closed. `strat-gameplay-engineer` is keeping the line as correct, direct defence in
    depth behind `bActorMoved`, has recorded in `engine.md` that deleting it yields a green
    suite, and that is a decision about the code rather than a coverage claim.
  - **WHAT THESE CLAUSES DO NOT PIN, AND THIS LIST IS AS LOAD-BEARING AS THE ONE ABOVE.**
    `StratMatchReconcile::FTestWorldScope` never begins play and never ticks, so
    `AStratUnitActor::Tick` NEVER RUNS in any of them. Nothing here observes, and nothing here
    could observe: the eased position at any moment between the ends; the trapezoidal velocity
    profile, its plateau or `AStratUnitActor::MoveTweenEaseFraction` at all; the wall-clock
    duration or that it is `AStratUnitActor::MoveTweenSeconds` PER HEX STEP; the zero-velocity
    ends that make a tween replaced mid-flight seamless; or the retirement of a completed tween,
    which only `Tick` performs. **A green run over these five says the POLYLINE IS THE RIGHT
    SHAPE AND IS ARMED ON THE RIGHT OCCASIONS, and says nothing whatever about the motion along
    it.** `Tick` is protected and stays so; no test-only advance entry point was asked for or
    added, and a future clause that wants the curve needs a PIE fixture rather than a seam.
  - **A DOUBLE WAS REQUIRED AND IS NOT A CONVENIENCE, WHICH IS WORTH SAYING BECAUSE THE
    ALTERNATIVE PASSES.** `Source/StratPlay/Tests/StratRouteTweenUnitDouble.h/.cpp` declares
    `AStratRouteTweenUnitDouble`, an `AStratUnitActor` subclass whose constructor writes
    `MoveTweenSeconds = 0.25f` and `BodyZOffset = 37.0f`. Both members are `protected`, which a
    subclass may write and a clause may not. Against the shipped C++ defaults
    `MoveTweenSeconds` is `0.0f`, so `ApplyUnitView` takes its documented no-animation path and
    **NO TWEEN IS EVER ARMED** -- all five clauses would assert over an empty polyline and pass
    over a route implementation, a chord implementation and no implementation alike; and
    `BodyZOffset` is `0.0f`, at which the height clause is a TAUTOLOGY. **The two values are
    fixture input, never an expectation, and are deliberately NOT the shipped Blueprint's**
    (`content.md` records `BP_StratUnit` at `BodyZOffset = 50.0` and `MoveTweenSeconds = 0.2`)
    -- copying those would invite a later reader to treat a `Tests/` file as a second,
    unhash-gated statement of the authored defaults. The double **overrides nothing and observes
    nothing**, on `StratProductionMenuHostDouble.h`'s stated rule: it can make a clause
    REACHABLE and cannot make one pass.
  - **NO COORDINATE, NO ADJACENCY RULE AND NO AXIAL -> WORLD FORMULA IS WRITTEN IN THESE
    CLAUSES.** Every expected point is `AStratBoardActor::WorldLocationOfHex`, off the board the
    subsystem spawned. **The route is DISCOVERED from the board rather than typed**: neighbours
    are the hexes at the minimum positive world distance from a given hex, and the tile spacing
    is measured the same way, so no hex arithmetic is restated -- which matters here more than
    usual, because `strat::` hex math is unreachable from `StratPlay` by the `LNK2019` rule and
    is exactly why the production path asks `FStratBridge::MovePathToHex` instead of walking
    neighbours itself. A four-hex simple path is chosen to MAXIMISE how far BOTH its
    intermediate hexes sit off the chord (the score is the MINIMUM of the two distances, so one
    collinear step scores as collinear), and the fixture refuses to proceed below a quarter of a
    tile.
  - **THE FALSIFIABILITY LEDGER AS IT STANDS, WHICH IS THE COLUMN TO READ BEFORE TRUSTING ANY
    OF THIS. [RESTATED FLAT ON THE SAME DAY, NOT APPENDED TO, BECAUSE A CORRECTION THAT ONLY
    ADDS LEAVES BOTH CLAIMS LIVE.]** It said:
    RETRACTED> "**Still owed, and owed by the lane that owns the code:** clause 4 against
    RETRACTED>  dropping `&& bActorMoved` ... **that argument has not been run**; and clause 5
    RETRACTED>  against either deleting `Spawned->SnapToWorldLocation(Where)` ... **Until those
    RETRACTED>  run, clauses 4 and 5 are arguments, not measurements.**"
    **NOTHING IS OWED. ALL FIVE ARE DISCHARGED BY A BUILT, RUN AND REVERTED MUTANT**, each
    quoted with its own failure message and its run's `reportCreatedOn` in the clause's own
    banner in `Source/StratPlay/Tests/StratMatchReconcile.cpp`. Two of them are SHARPER than a
    bare discharge: clause 2 reddened ALONE while clause 1 stayed green under the same mutation,
    and clause 5 reddened ALONE while `ApplyViewSpawnsMovesAndDestroys` stayed green under the
    snap deletion. Every one of those runs is `strat-gameplay-engineer`'s, in the files this
    lane may not touch; this lane wrote the clauses and named the mutations and could build
    none of them.
  - **[DISCHARGED THE SAME DAY. RESTATED FLAT RATHER THAN DELETED, because a reader arriving by
    a citation to "unbuilt and unrun" must land on what replaced it.]** It said:
    RETRACTED> "**THE CLAUSES ARE WRITTEN AND ARE UNBUILT AND UNRUN -- AN EDITOR HELD THE LIVE
    RETRACTED>  CODING MUTEX AND `Build.bat` REFUSED BEFORE COMPILING ANYTHING.**"
    The editor was closed and `Build.bat` returned `Result: Succeeded`, compiling
    `StratMatchReconcile.cpp` and relinking `UnrealEditor-StratPlay.dll`; the full headless
    suite then ran over that binary with `Saved/AutomationReport/index.json` DELETED FIRST, and
    all five clause names were verified PRESENT IN THE EXPORTED REPORT rather than inferred from
    a count delta -- a delta is consistent with a stale binary. **The retired name is verified
    ABSENT from the same report.** Set difference on the test MACRO (multiline, one collector
    over both sides) against `283d711`: **+5, none removed**; the retirement and its replacement
    net to zero within this pass.
    **AND THE COMMENT-ONLY PASS THAT RECORDED THE DISCHARGES WAS REBUILT AND RE-RUN RATHER THAN
    ASSUMED SAFE**, on the same reasoning `engine.md` records for its own comment edit:
    `StratMatchReconcile.cpp` is excluded from `StratPlay`'s unity file, so it has its own
    translation unit and a real compile action ran -- `[1/4] Compile [x64]
    StratMatchReconcile.cpp`, then a relink of `UnrealEditor-StratPlay.dll`. The report deleted
    before that run carried `reportCreatedOn 2026.09.02-20.52.04` (the engineer's restored run);
    the run that replaced it is `reportCreatedOn 2026.09.02-20.57.45`, non-success empty, with
    all five names verified present and the retired name absent.
    **AND ONE INSTRUMENT NOTE THAT COST NOTHING HERE AND WOULD HAVE COST A CITATION.** The
    report I deleted carried `reportCreatedOn 2026.09.02-20.30.33`, NOT the
    `2026.09.02-20.09.41` I had been handed as the run that gated these clauses -- the mutant
    passes had overwritten `index.json` IN PLACE in between. Recording the old stamp before
    deleting is what made that visible; a citation to the earlier stamp was already dead when it
    was written down.

- **2026-09-02 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base `fcf64d3`, UNCOMMITTED
  at the time of writing) -- §2.11.2'S ON-SCREEN COMMAND BAR IS GATED: THE BUILD BUTTON, THE
  END TURN HIGHLIGHT, THE FOCUS LATCH AND THE TWO NEW CONTROLLER VERBS. EIGHT CLAUSES IN TWO
  NEW FILES, PLUS ONE NEW TEST-ONLY WIDGET DOUBLE. THE LIVE COUNT LIVES IN
  `Tools/architect/state/global.md` AND NOWHERE ELSE.**
  - **THE ACCEPTANCE ID WAS RULED BY THE USER, NOT DERIVED: every clause rides `T-UI-03` and
    states its own stretch in its own name**, on `T-SCN-07`'s precedent for a shared ID. It is
    also the ID the subject earns: `T-UI-03` is the no-widget-side-arithmetic clause, and the
    whole reason `FStratCommandBarView` exists is that the two controls' conditions were folded
    in C++ so no graph spells `count == 0` or `suggested AND NOT gated`.
  - **WHERE THE CLAUSES LIVE, AND WHY EACH IS IN THE MODULE IT IS IN.** The split is a linker
    fact and was decided before anything was written:
    - `Source/StratUI/Tests/StratCommandBarClauses.cpp` -- four clauses over
      `StratDecorateCommandBar`, a free function on `FStratViewModel`. No widget, no actor, no
      world, no PIE.
    - `Source/StratPlay/Tests/StratBuildAffordanceClauses.cpp` -- four clauses over
      `FStratBuildAffordance` and the controller verbs. **The hand-over clause calls BOTH the
      latch (StratPlay) and `StratDecorateCommandBar` (StratUI), so StratPlay is the only
      directory that can host it; in StratUI it is `LNK2019`.**
  - **WHAT EACH CLAUSE PINS, AND WHERE ITS EXPECTATION COMES FROM. Read this column before
    citing any of them as coverage.**
    - `StratUI.T-UI-03.CommandBarBuildButtonFollowsFactoryOwnershipOnEveryDecorate` -- the
      design's load-bearing property. **ONE latch, unchanged across seven decorates, and the
      MODEL is mutated between them**: the factory changes hands, goes neutral, the seat moves,
      the factory leaves `Factories` entirely, everything is restored, and finally the bool is
      dropped with the real hex still passed in. The answers follow the model every time, which
      is what makes staleness structurally impossible rather than dependent on a clear-point
      list. Expectation: the contract stated on `bShowBuildButton` ("TRUE MEANS ...") over a
      factory and side indices ENUMERATED off the built model -- no coordinate is named.
    - `StratUI.T-UI-03.CommandBarGateBeatsSuggestionWhenBothHold` -- the `&&` both
      `bEndTurnSuggested`'s block and the decorator call "MUST NOT BE DROPPED AS REDUNDANT".
      **On the shipped scenario the pair is unreachable** (beat 1a locks every unit but the
      marked Infantry and locked units do not count), so the clause hand-builds the combination
      and pins the answer -- which is what turns a fact about `PublishLocks` into a structural
      one. Carries its own positive control (same model, ungated, MUST suggest).
    - `StratUI.T-UI-03.CommandBarSuggestionAgreesWithCountViewingSideUnitsAbleToAct` -- SINGLE
      AUTHORING over a seven-model corpus the selector's four clauses disagree on. **Measured
      on the green run: 5 / 0 / 0 / 5 / 0 / 0 / 5 able to act, three suggested and four not**,
      and the clause asserts the corpus is non-vacuous in both directions so it cannot go quiet
      if it ever drifts to one answer.
    - `StratUI.T-UI-03.BuildViewModelLeavesCommandBarDefaulted` -- sibling to
      `BuildViewModelLeavesGuidanceDefaulted`, `CompareScriptStruct` over the whole block so a
      field added tomorrow is covered without an edit, with the decorated-model negative
      control that stops the comparison being one that cannot fail.
    - `StratPlay.T-UI-03.BuildAffordanceClearsOnEveryStatedClearPoint` -- **one leg per row of
      `StratBuildAffordance.h`'s clear-point table, which IS this pass's specification**,
      including the row stated as a NON-clear: a production menu close does NOT clear, measured
      through `AStratPlayerController::CloseProductionMenu` on a real controller. A FRESH
      affordance per leg, so no leg's precondition is a previous leg's outcome.
    - `StratPlay.T-UI-03.OpenProductionMenuAtFocusedFactoryNeedsNoCursor` -- see the gap note
      below.
    - `StratPlay.T-UI-03.CloseProductionMenuTakesThePanelDownBeforeClearingTheRows` -- see the
      order note below.
    - `StratPlay.T-UI-03.CommandBarSurvivesNoStaleFocusAcrossAHandover` -- **both halves, in
      order, and the order is the subject.** (1) The RECOMPUTE alone: a latch left deliberately
      stale across a hand-over draws nothing, with `HasFocus()` asserted STILL TRUE at that
      moment so the answer is attributable to the decorator. (2) Only then the CLEAR. A clause
      measuring only (2) would report the design's guarantee as resting on a clear-point list,
      which the header says it does not rest on. The factory never changes hands anywhere in
      it; only the seat moves.
  - **A GAP `StratProductionMenuAffordance.cpp` DECLARES IN ITS OWN HEADER IS NOW TWO-THIRDS
    CLOSED, AND THE FILE'S OWN TEXT IS THE CITATION.** That header lists three things a
    headless run cannot reach, all because `HexUnderCursor` needs a `ULocalPlayer`:
    - **THE OPEN PATH IS NOW REACHABLE, and by contrast in ONE FRAME.** With a live match and a
      board asserted non-null, `ToggleProductionMenu` is refused AT THE CURSOR while
      `OpenProductionMenuAtFocusedFactory`, on the same controller in the same frame, resolves
      its hex from the latch and reaches the HUD -- its refusal becomes the HUD's own
      `no ProductionMenuWidgetClass ...`. **The three refusals are asserted to be three
      different sentences, case-sensitively.** Measured focus: hex `(-1, 4)` on the live board.
    - **THE UNWIND BRANCH, which that header calls UNREACHABLE, is now reached and asserted**:
      a refused open leaves `GetProductionTargetHex` false and the meaningless hex.
    - **THE CLOSE ARM OF `ToggleProductionMenu` IS STILL UNREACHABLE** -- it is gated on
      `IsProductionMenuWidgetOpen()`, which is false headlessly because `AddToViewport` is a
      no-op with no game viewport. **No clause claims it.** What is pinned instead is
      `CloseProductionMenu`, the verb that arm now forwards to, called directly.
  - **AN ORDER IS NOT VISIBLE IN THE STATE THAT FOLLOWS IT, AND THAT IS WHY THIS PASS ADDED A
    SECOND WIDGET DOUBLE.** `CloseProductionMenu`'s declaration calls the order the contract --
    panel down, THEN rows cleared -- but both orders leave the identical aftermath (no widget,
    no rows). A clause named for the order while measuring only the conjunction would carry a
    subject broader than what it pins.
    - `Source/StratUI/Tests/StratProductionMenuOrderDouble.h/.cpp` overrides
      `UWidget::RemoveFromParent`, the one virtual `CloseProductionMenuWidget` calls, and fires
      a hook the clause supplies. **The hook reads `UStratMatchSubsystem::IsProductionMenuOpen()`
      and `ProductionMenu.Num()` -- the SUBSYSTEM's own answers -- at that instant**, so the
      double supplies a MOMENT and never an expectation.
    - **`StratProductionMenuHostDouble.h`'s "ADDS NOTHING AND OVERRIDES NOTHING" RULE IS NOT
      WEAKENED AND THAT FILE IS NOT EDITED.** Its stated hazard is a clause going green
      comparing a double against itself; nothing here compares against a recording the double
      made. The two doubles are separate so that rule keeps holding for every clause that uses
      the first one.
    - **MEASURED ON THE GREEN RUN, printed by the clause via `AddInfo` so it is checkable from
      a checkout:** *"panel-down fired 1 time(s); at that instant the subsystem reported the
      menu open = 1, with 4 row(s)"*. Idempotence is asserted on top: a second exit succeeds,
      takes no second panel down, and leaves the rows cleared.
    - **THE HOOK IS CAPTURED WEAKLY (`TWeakObjectPtr`) AND BY `TSharedRef`, AND CLEARED
      EXPLICITLY BEFORE TEARDOWN.** `AStratScoreboardHUD::EndPlay` calls
      `CloseProductionMenuWidget` unguarded, so a live hook could otherwise run clause code on
      a destroyed subsystem. Belt and braces, and both are deliberate.
  - **FALSIFIABILITY, MEASURED, AND WHAT KIND OF MEASUREMENT IT IS. NO PRODUCTION MUTANT WAS
    BUILT** -- the lane forbids editing a file outside `Tests/` even temporarily -- so all four
    probes were planted in the CLAUSES' OWN FILES, built in place, run, and reverted. Each
    therefore proves the comparison is LIVE ON BOTH SIDES against the shipped code, and none
    proves anything about a hypothetical rewrite of the code under test. **One planting pass,
    four inverted assertions, and exactly the four intended clauses went Fail with the other
    four new ones still green:**
    - the ordering observation inverted -- Fail: *"Expected 'T-UI-03: at the instant the PANEL
      came down, the subsystem still held its ROWS ...' to be true."*
    - the enemy-captured case of the ownership clause inverted -- Fail: *"Expected 'T-UI-03:
      the SAME latch darkens the BUILD control once the factory changes hands ...' to be
      false."*
    - the gate's suppression inverted -- Fail: *"Expected 'T-UI-03: §2.11.6-B's gate beats
      §2.11.2's highlight ...' to be false."*
    - the button path's refusal required to name the CURSOR instead of the HUD -- Fail:
      *"Expected 'T-UI-03: with a factory focused, the BUILD verb resolves its hex WITHOUT A
      CURSOR and reaches the HUD ...' to be true."* **That one is the sharpest of the four: it
      is a measurement that the refusal reaching the caller really is the HUD's sentence and
      really is not the cursor's, which is the whole claim in the clause's name.**
    - **THE REVERT SCRIPT REFUSED ITS OWN FOURTH EDIT AND WAS RIGHT TO.** The mutant made
      `Focused.Contains(TEXT("cursor") ...)` appear TWICE in the file, so the anchor count was
      2 and the script raised rather than replacing. Recorded because the refusal is the
      feature: a script that had taken the first match would have silently rewritten the
      wrong assertion. Reverted with a two-line anchor including the preceding comment's own
      indentation.
  - **WHAT THESE CLAUSES DO NOT PIN. Read this before citing any of them as coverage.**
    - **THAT THE LATCHED HEX IS THE FOCUSED ONE.** `OpenProductionMenuAtFocusedFactory` cannot
      SUCCEED headlessly -- `CreateWidget` refuses a controller that is not a LOCAL player
      controller, measured and recorded in `StratProductionMenuHostProbe.cpp` -- and the
      refusal unwinds the latch, so `GetProductionTargetHex` is false whether the hex was
      written or never written at all. **What is pinned is that hex RESOLUTION succeeded with
      no cursor; WHICH hex it resolved to is owed by a PIE clause.**
    - **NO WIDGET, NO BINDING, NO PIXEL.** Whether `WBP_CommandBar` binds visibility to one
      bool and makes no conjunction, whether the BUILD button is on screen, whether the exit
      button is wired to `CloseProductionMenu` -- all asset properties and the editor lane's.
      `OpenProductionMenuAction`'s binding at `ETriggerEvent::Started` is likewise unpinned,
      for `StratProductionMenuAffordance.cpp`'s stated reasons.
    - **WHETHER `CountViewingSideUnitsAbleToAct` IS RIGHT.** The agreement clause calls it on
      both sides of its comparison, so a defect INSIDE the selector moves both together and is
      invisible there. Its four-clause derivation is gated where it is authored. What the
      clause pins is SINGLE AUTHORING -- that the bar grew no second walk of `Model.Units`.
    - **THE DECORATOR ORDERING CONSTRAINT.** `StratDecorateCommandBar` must run AFTER
      `FStratGuidedOpening::DecorateViewModel` -- it reads `Guidance.bEndTurnGated` and writes
      neither -- and nothing below pins the call ORDER inside
      `AStratPlayerController::RefreshFromMachine`. What IS pinned is that the two copies track
      the guidance block in both directions. **A clause over the call order would need a seam
      the controller does not have; see the open item below.**
  - **TWO HAZARDS THE ENGINEER DOCUMENTED WERE OFFERED AS CANDIDATE CLAUSES AND ONE WAS
    REFUSED. Both refusals are the same rule: PIN THE REQUIREMENT, NOT THE HAZARD.**
    - **REFUSED: the neutral-factory latch.** `Owner == ViewingSide` latches a NEUTRAL factory
      (`Owner == INDEX_NONE`) when `ViewingSide` is ALSO `INDEX_NONE`, on a model
      `StratBuildViewModel` refuses to produce. A clause asserting that behaviour would GO RED
      ON ITS OWN FIX the day someone tightens the test, which is the wrong thing for a gate to
      do. **What was written instead is the requirement**: a neutral factory does NOT latch and
      does NOT show the button on a model with a real viewing side -- a leg of the clears
      clause and case (c) of the ownership clause.
    - **ACCEPTED, BUT AS A CASE AND NOT AS A CLAUSE: the vacuous zero-unit reading.** A model
      with no units reads as suggested. That is a CONSEQUENCE of `count == 0` rather than a
      rule of its own, so it sits in the agreement corpus where the equality is measured over
      it; a clause named after it would advertise a subject the decorator does not have.
  - **SLOT HYGIENE, AND IT IS NOT OPTIONAL.** `Source/StratPlay/Tests/StratBuildAffordance-
    Clauses.cpp` names its own slot (`StratocracyAutomation_BuildAffordance`) on both clauses
    that start a match. **An unset `FStratMatchConfig::SaveSlotName` resolves to the PLAYER'S
    REAL SAVE SLOT**, which stays an open finding elsewhere in this record; the two new
    match-starting clauses are not part of it.
  - **NO `FOutputDevice` IS CONSTRUCTED IN EITHER NEW FILE**, so the
    `CanBeUsedOnMultipleThreads()` override that the four captures in `Source/StratPlay/Tests/`
    carry is not in play here. Recorded so a future pass adding a capture to either file knows
    it must add the override with it.
  - **OPEN, AND EACH NAMES THE CODE CHANGE IT WOULD NEED. This lane does not make them.**
    - **THE DECORATOR CALL ORDER inside `RefreshFromMachine`.** `BuildAffordance.Observe` then
      `DecorateViewModel`, after the guidance layer -- unobservable from outside, because the
      method returns one finished model and every intermediate ordering produces the same one
      on any model where the gate is stable across the frame. It would need either a seam that
      reports the decoration sequence, or a public split of `RefreshFromMachine` into its
      decoration steps.
    - **THE AFFIRMATIVE HALF OF `GetProductionTargetHex`** -- that it can ever answer TRUE with
      the FOCUSED hex. Still owed by a PIE clause, for the reason above.
    - **`FStratSelectionMachine` HAS NO BUILD ARM.** Still not writable as a clause, for the
      reason `StratProductionMenuAffordance.cpp`'s header already records: `EStratSelection-
      Command` is a plain `enum class` and not a `UENUM`, so there is no reflected enumerator
      list to read and compare. Unchanged by this pass and restated only because
      `StratBuildAffordance.h` newly rules that the machine stays untouched, which makes the
      absence load-bearing where it used to be incidental.

- **2026-09-01 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base `f079b9f`, UNCOMMITTED
  at the time of writing) -- W8 ITEMS (3) AND (4) ARE GATED: THE §2.7 REPAIR RECEIPT AND THE
  §2.11.5 BUILD-PULSE DRAW. FIFTEEN CLAUSES IN THREE NEW FILES. THE LIVE COUNT LIVES IN
  `Tools/architect/state/global.md` AND NOWHERE ELSE.**
  - **THE ACCEPTANCE IDS WERE DECIDED FROM §4.11'S OWN TEXT, BECAUSE THE TWO PROPOSED LISTS
    DISAGREED WITH EACH OTHER.** The bridge repair clauses were offered once as `T-UI-02` and
    once as `T-TURN-09`, and the receipt clauses as `T-UI-03`. All three were refused and none
    was minted:
    - **`T-TURN-08` for the bridge repair measurement.** §4.11 states it as *"repair fires at
      the start of the unit's turn exactly when the verified repairAmount says so ... this gate
      asserts the turn loop calls it at the right moment with the right board facts, nothing
      more."* That sentence is exactly what
      `Source/StratBridge/Tests/StratRepairReceiptParity.cpp` does. `T-TURN-09` is DETERMINISM,
      `T-UI-02` is the reachable-hex highlight and `T-UI-03` is the scoreboard's 1:1 binding;
      none of them is repair. **Reassigned on `T-SCN-07`'s precedent, and this is `T-TURN-08`'s
      first use in this repository.**
    - **`GATE-TRANSIENT` for the receipt decider clauses**, which is what all thirteen
      pre-existing transient-receipt clauses ride and what a corpus census settled rather than
      a preference. It mints no acceptance ID.
    - **`T-UI-02` for the pulse draw**, which is what the board-overlay clauses in
      `Source/StratPlay/Tests/` already ride -- and is the name
      `AStratBoardActor::GetBuildPulseOverlayCount`'s own doc block asked for by full string.
      `GATE-BOARDCHURN` refused `T-UI-02` for ITS clauses on the ground that they are about
      whether the renderer was TOUCHED rather than about what is DRAWN; every clause here has
      what is drawn as its subject, so that refusal does not reach them.
  - **`FStratBridge::RepairsAtTurnOpen`'s "(T-TURN-09)" CITATION IS HALF OF A REAL ONE, NOT AN
    ERROR.** `strat::applyStartOfTurnRepair`'s own doc line in `Source/StratRules/Turn.h` cites
    BOTH T-TURN-08 and T-TURN-09 for the ascending-id guarantee. Recorded so nobody re-derives
    it as drift.
  - **THE PARITY FIXTURE REACHES NO REPAIR AT ALL, AND THAT IS THE MOST USEFUL MEASUREMENT IN
    THIS PASS.** `Data/parity_fixture.save` replayed one `FStratBridge::Submit` at a time
    crosses **12 turn openings across its 169 commands and heals NOBODY at any of them.**
    §2.7 needs a damaged unit standing on an objective its own side holds with no enemy
    adjacent, and the fixture's damaged units are the ones in contact while its units on owned
    objectives are the ones capturing away from the fighting. A first draft of the file walked
    the fixture looking for a repairing opening and went red on all five clauses saying so;
    that red run is what produced this figure.
    - **SO THE POSITION IS REACHED RATHER THAN FOUND, AND EVERY STEP OF REACHING IT IS STILL A
      MODULE ANSWER.** `DriveToARepairingTurnOpen` replays the fixture (which supplies the
      DAMAGE -- not one attack is chosen in the test), then at a turn opening asks the
      projection which of the active side's units is below `hpMax`, asks
      `FStratBridge::Reachable` -- the rules module's own query, never a distance filter --
      where each may stand, and keeps only hexes where `strat::UiHexView::owner == side` and no
      enemy is at `hexDistance == 1`. It moves every such unit, recomputing the projection and
      the reach set between movers, then ends two turns with the opponent passing WITHOUT A
      SINGLE COMMAND so the adjacency it read is the adjacency §2.7 sees. **A refused move is
      fatal, not skipped:** the destination came out of the module's own reach set, so a
      refusal would mean two module answers disagree. Going off the fixture's script is this
      directory's existing posture -- `StratCombatOutcomeParity.cpp`'s counter-kill clause does
      the same and says so.
    - **WHAT IT ACTUALLY REACHES, printed by the clauses themselves via `AddInfo` so it is
      checkable from a checkout rather than taken on trust:** *"fixture position 20 (3 openings
      in), side 0: unit 5 at hp 1/12 on (-1,4)"*, and the measured answer
      `{unit=5 side=0 hp 1->4/12 amount=3}`.
  - **THE ORACLE CLAUSE, AND IT IS THE ONE THE WHOLE BRIDGE FILE EXISTS FOR.**
    `T-TURN-08.RepairAmountIsTheModulesOwnRepairAmount` computes `strat::repairAmount` in the
    test from the same board facts and requires the bridge's `Amount` to equal it. **That is
    the one place in this pass where an expectation is computed rather than read, and the
    reason is that no module-side repair figure exists anywhere in this tree:**
    `strat::openTurn` builds the `RepairApplied` vector, adds each amount to `GameUnit::hp` and
    DISCARDS it, returning `void`; `Driver.good.cpp`'s `openActiveTurn` keeps it but is the
    debug driver over `Session` and the bridge never runs it. So the only independent oracle
    available is the rules function itself, asked the same question. The two board facts are
    read off the module's own projection (`UiHexView::owner` for the objective,
    `strat::hexDistance == 1` for adjacency) and `hp`/`hpMax` off `strat::UiUnitView`;
    **nothing in the test re-derives the +25% table, the min-1 floor or the never-overheal
    clamp**, which are `repairAmount`'s and are gated upstream as T-REPAIR-01..07.
    - **IT ASKS THE ORACLE ABOUT EVERY UNIT, NOT ONLY THE REPORTED ONES.** A unit the oracle
      says was owed nothing must carry NO entry; a unit it says was owed something must carry
      one for exactly that amount; and the two lists must be the same size. A one-sided version
      would be green over a bridge reporting one unit out of four.
    - **THE CLAUSE MUST LIVE IN `Source/StratBridge/Tests/`.** It calls a `strat::` free
      function, which is `LNK2019` in any other module -- measured 8x.
  - **FALSIFIABILITY, MEASURED, AND WHAT KIND OF MEASUREMENT IT IS.** **No production mutant
    was built.** The lane forbids editing a file outside `Tests/` even temporarily, so both
    probes were planted in the CLAUSE'S OWN FILE, built in place, run, and reverted; each
    therefore proves that the comparison is LIVE ON BOTH SIDES against the shipped code, and
    neither proves anything about a hypothetical rewrite of the code under test.
    - **The oracle clause.** `strat::repairAmount(...)` became `strat::repairAmount(...) + 1` in
      the test. Result **Fail**, on four distinct assertion families at once: *"unit 5's
      measured amount IS strat::repairAmount's answer' to be 4, but it was 3"*, *"unit 5's
      HpAfter is HpBefore plus the module's amount' to be 5, but it was 4"*, five separate
      *"strat::repairAmount owes unit N 1 HP at this opening, so the bridge must report it ...
      to be not null"*, and *"the bridge reports exactly the units the oracle owed ... to be 6,
      but it was 1"*.
    - **The copy-not-subtracted clause.** Expectation swapped from `Repair.Amount` to `Delta` --
      i.e. asserting what a RE-SUBTRACTING decider would have produced. Result **Fail**:
      *"the receipt's Amount is the BRIDGE's Amount, copied' to be 100, but it was 200."* **That
      is a measurement of the shipped decider**: on a row whose `Amount` is 200 and whose
      `HpAfter - HpBefore` is 100, it emits 200. An implementation that re-subtracted would emit
      100 and this clause would redden on it. The three numbers are printed by the clause itself
      via `AddInfo` on every green run.
  - **WHY THE COPY CLAUSE'S FIXTURE DISAGREES WITH ITSELF ON PURPOSE.** On any consistent row --
    the only kind the bridge produces -- `Amount` and `HpAfter - HpBefore` are the same number,
    so `Receipt.Amount == Repair.Amount` is true of a copy AND of a re-subtraction and pins
    nothing. The planted row makes them 200 and 100: both nonzero, so a wrong answer is a wrong
    VALUE rather than a zero that could be confused with the arm not firing; and every one of
    the three planted numbers is a module reading off the live projection (`FStratUnitView::Hp`,
    `FStratSideView::IncomePerTurn`, `FStratSideView::FameTotal`), whose PROVENANCE is the
    fixture and none of which is an expectation. **All three inequalities are asserted as
    preconditions before anything is decided**, so the clause cannot go inert quietly.
  - **WHAT THESE CLAUSES DO NOT PIN. Read this before citing any of them as coverage.**
    - **A BLOCKED REPAIR, AND NOTHING DRAWN OFF AN ABSENCE.** The record is positive-case only:
      an absent entry cannot be told apart from full HP, from a unit off an owned objective, or
      from §2.7's anti-fortress lock firing. §2.11.6's `enemy adjacent` one-shot needs precisely
      that third reading and **no clause anywhere asserts it**. A clause that did would be
      asserting a discrimination the code deliberately does not have.
    - **THE FIRST TURN OF A MATCH.** `strat::seedFromScenario` opens it, not `Submit`, so there
      is no bracket to measure.
    - **`ReplayLog`'s TURN BOUNDARIES.** It reaches `strat::replayLog` directly and never passes
      through `Submit`; the clause pins that it leaves the record EMPTY, not that it observes.
    - **THE ASCENT, ON A ONE-ENTRY ANSWER.** `EveryRepairEntryIsAPositiveRiseAscendingByUnitId`
      asserts strict ascent by unit id, and the reachable position yields **one** entry today,
      so on this fixture **that assertion pins nothing**. `DriveToARepairingTurnOpen` plants
      every mover it can rather than the first, precisely so the width can grow; the clause
      reports the width it got via `AddInfo` instead of demanding one it cannot honestly
      require. **The `std::sort` in `StratRepairObservation::CaptureAfter` is therefore
      UNPINNED as of this pass.**
    - **`LoadDefinitions` CLEARING THE RECORD IS UNOBSERVABLE FROM OUTSIDE THE BRIDGE.** That
      function sets `bSeeded = false` in the same body that calls
      `RepairsAtLastTurnOpen.clear()`, so `RepairsAtTurnOpen` refuses afterwards and the
      emptiness hides behind the refusal: **deleting that `clear()` leaves
      `ReplayAndReseedEachLeaveTheRecordEmpty` green.** What the clause does pin is what a
      caller experiences -- the answer goes away and the caller's array is emptied -- and the
      route's own comment block in the test says so rather than implying more. Making it
      observable needs a production change, so it is listed under the blocked items below and
      not fixed here.
    - **PIXELS, MATERIALS AND COPY.** Nothing asserts an appearance. `BuildPulseMaterial` ships
      UNSET, so a lit factory currently draws in `OverlayMesh`'s own material; every pulse
      clause reads counts and instance POSITIONS, both unaffected by which material a slot
      holds. The GDD's `+[N] HP — repaired` string is likewise unasserted -- it is a UMG
      binding, and `FString` comparison is case-insensitive in this engine, which has already
      produced a clause in this project that could not fail.
  - **THE PULSE CLAUSES, AND THE ONE FACT THEY ARE BUILT AROUND.** `bBuildPulse` inherits
    `side == activeSide` from `strat::canBuildAt`, so **on the opponent's hot-seat turn every
    factory is dark board-wide, and that is correct.**
    `T-UI-02.BuildPulseClearsAndIsDarkOnTheOpponentsTurn` asserts that darkness **as a
    requirement**, and produces it through the rules module -- `SubmitEndTurn` on the bridge,
    the model rebuilt for a viewing side that is no longer to move -- rather than by editing a
    flag. Its LIT half is its own positive control: on a board with no overlay mesh both halves
    read zero and the clause would look green while measuring nothing, which is why
    `GiveTheBoardAnOverlayMesh` is not optional in any of these fixtures.
    - `BuildPulseOverlayIsDrawnFromTheModelAlone` checks POSITIONS and not only the count: a
      subsystem collecting the right NUMBER of hexes off the wrong predicate (`Owner ==
      ViewingSide`, `!bHasBuiltThisTurn`) lands on the same count on this board. Every drawn
      instance is matched to a hex the model marked through
      `AStratBoardActor::WorldLocationOfHex` -- the board's own axial-to-world map, read and
      never repeated -- and every marked hex must be found among the drawn ones.
    - It also reads the other three overlays, because *"the pulse was drawn"* and *"something
      was drawn on the reach overlay instead"* are different facts and only the second reading
      tells them apart.
    - **`GetBuildPulseOverlayCount` HAD NO CALLER, AND NOW HAS ITS DECLARED ONE.** Its doc block
      names `Stratocracy.StratPlay.T-UI-02.BuildPulseOverlayIsDrawnFromTheModelAlone` by full
      string; that is the clause written. Its two siblings are likewise called only from
      `Tests/`. The clause asserts the accessor agrees with the component it claims to read,
      which turns *"off the component, not a cached number"* from a sentence into a measurement.
  - **INSTRUMENT CAVEAT WORTH REUSING: THE THREE PULSE CLAUSES DECLARE THE BOARD'S
    UNMESHED-TERRAIN WARNING.** A fixture world has no Blueprint defaults, so
    `AStratBoardActor` logs *"no tile mesh for terrain ..."* at Warning on every `ApplyView`.
    The first full run left three tests in `succeededWithWarnings`;
    `AddExpectedMessagePlain(..., Occurrences 0)` on `StratAiMatchClauses.cpp`'s precedent
    removed them. **A gate that leaves warnings in a green run trains the next reader to skim
    warnings**, and that is the whole reason it was fixed rather than tolerated.
  - **PROJECT-PROSE DRIFT FOUND WHILE WRITING THESE, NOT FIXED -- `Source/` OUTSIDE `Tests/` IS
    NOT THIS LANE.** `FStratReceiptView::UnitId`'s tooltip says it keys into
    `FStratViewModel::Units` by **`FStratUnitView::Id`**; that field is spelled `UnitId`, and
    `StratViewModel.h` records at the declaration that it is spelled that way ON PURPOSE.
    One-word drift, no behaviour, for `strat-gameplay-engineer`.
  - **THE RUN.** Editor CLOSED and verified (`tasklist`: no `UnrealEditor` and no
    `LiveCodingConsole` process) before every build; `Build.bat` green each time, no
    `-NoHotReloadFromIDE`. The figures are in `Tools/architect/state/global.md`, which is the
    only file that may carry them; what is recorded here instead is the SHAPE of the result,
    which this file owns: **zero failures, zero warnings, the fifteen clauses above are
    additions and not replacements, and a name-by-name diff of the new report against the
    previous canonical one shows no test removed and no pre-existing test changed state.**
    `git status --porcelain` after the run shows the three new `Tests/` files as `??` and
    nothing else that is mine.
    - **A CLAIM STOOD HERE FOR ABOUT FOUR MINUTES AND IS KEPT RATHER THAN DELETED, BECAUSE WHAT
      IT RECORDS IS THE REASONING THAT WAS WRONG:**

        STRUCK> "Every run in this pass went to a scratch `-ReportExportPath`, so
        STRUCK>  `Saved/AutomationReport/index.json` STILL READS `reportCreatedOn
        STRUCK>  2026.09.01-23.31.22` and every live citation of that stamp elsewhere remains
        STRUCK>  valid."

      **WITHDRAWN, AND THE PRESERVATION WAS THE DEFECT.** Leaving the canonical report alone
      protects the citation in `global.md` and breaks something worse:
      `strat_banner_sweep.py` then reported **TREE DISAGREES WITH ITSELF** -- report 347,
      macro census 362 -- plus **REPORT IDENTITY**, *"predates a test-defining source file
      modified 2026-09-01 20:32:47 ... this report is evidence about a PAST tree."* A stale
      canonical report is a finding in its own right, and a valid citation OF a stale report is
      a citation of the wrong tree. So the suite was re-run to the canonical path.
    - **`Saved/AutomationReport/index.json` NOW READS `reportCreatedOn 2026.09.02-00.38.55`,
      AND TWO LINES IN `global.md` ARE STALE BECAUSE OF IT.** They are not this lane's to
      touch and the sweep named both, identified here by their CONTENT because `global.md` is
      prepended to and any line number into it rots: its banner line carrying the live suite
      figure, which no longer matched the tree, and the line citing `reportCreatedOn
      2026.09.01-23.31.22`, which was by then a DIFFERENT run from the one the sweep opens.
      **[This is an account of the tree as it stood on 2026-09-02; both lines have since been
      corrected. STAMPED 2026-09-05 -- the original wording named those two lines by NUMBER,
      which is the rotting shape `strat_doc_citation_gate.py` refuses, and it is restated here
      by content instead.]** Handed to the `coordinator`/steward
      with the new stamp above. **After those two lines are corrected the sweep has nothing
      else to say about this pass: no finding it raised names `tests.md`.**

- **2026-09-01 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base `cada741`, UNCOMMITTED
  at the time of writing) -- THE FIVE `T-UI-02` VISUAL CLAUSES IN
  `StratGuidedOpeningVisuals.cpp` ARE ARMED, NOT VACUOUS. THE SAVE CANNOT REACH THEM. THE SLOT
  NAME ADDED IS HYGIENE AND NOT A REPAIR.**
  - **THE QUESTION.** The entry below found that `FStratMatchConfig::SaveSlotName` defaults to
    the PLAYER'S slot and that a completed match there suppresses the whole §2.11.6 opening.
    `StratGuidedOpeningVisuals.cpp` also names no slot and is dense with guidance machinery, so
    the worry was the SILENT shape of that hazard: five clauses about what is DRAWN, passing
    because nothing was armed.
  - **THE ANSWER, AND WHY BOTH HALVES OF IT WERE NEEDED.** The five clauses ran green with
    `Saved/SaveGames/StratocracyMatch.sav` PRESENT and green again with it ABSENT, on identical
    binaries (`Build.bat` reported `Target is up to date` before the first), reports to a
    scratch `-ReportExportPath`. **That pair is ambiguous on its own** -- it is equally the
    signature of insensitivity and of vacuity -- so three temporary probes decided it, in a
    file created and deleted inside this pass
    (`Source/StratPlay/Tests/ZZTempProbeGuidedVisualsFalsifiability.cpp`, named outside the
    `Stratocracy.` namespace so it could never join the suite):
    1. `ZZTempProbe.SaveIsVisibleThroughTheVisualsFixtureConfig` -- **Success**. Built with
       `StratGuidedOpeningVisuals.cpp`'s own config, `HasCompletedAMatchOnSave(FString())`
       answered TRUE. The suppressing save IS reachable from that exact config, so the green
       above is not an instrument that cannot speak.
    2. `ZZTempProbe.SuppressedArmingReddensThePremise` -- **Fail**, *"Expected 'the shipped
       scenario arms a guided opening for the seat to move' to be true."* That is verbatim the
       premise all five clauses open with.
    3. `ZZTempProbe.SuppressedOpeningCannotSatisfyTheLitControl` -- **Fail**, *"Expected 'with
       guidance SUPPRESSED, the lit control the real clauses assert first (exactly one marker
       showing)' to be 1, but it was 0."* The arming premise was REMOVED in this probe so the
       fixture could not stop early; the clauses' own LIT CONTROL still could not be satisfied.
    So a suppressed opening turns this file **RED** and cannot make it pass quietly. The
    coverage that has been shipping green is real coverage.
  - **WHY IT IS IMMUNE, WHICH IS A PROPERTY OF THE ROUTE AND NOT OF THE FIXTURE'S CARE.** The
    save is consulted at exactly one site, `AStratPlayerController::TryArmGuidedOpening`, which
    COMPUTES `bSuppressed`. Every clause in this file bypasses the controller and passes the
    literal `false` to `FStratGuidedOpening::Begin`. `StratGuidanceRouteClauses.cpp` went red
    for the opposite reason: it drives the controller.
  - **WHAT CHANGED, AND IT MOVED NO EXPECTATION.** `kAbsentSlotName` and one
    `Out.SaveSlotName` line in `MakeConfig`, with the measurement above recorded at the
    constant. It buys one thing: a clause added here LATER and routed through the controller
    would suppress silently on a developer's box and pass on a clean one. **It repaired
    nothing** -- all five were green before it and after it, in both save states.
  - **WHAT THESE FIVE DO NOT PIN, restated because this pass could be misread as widening
    them.** Not pixels (`IsGuidedMarkerVisible` reports a visible FLAG). And **not the
    production suppression route**: no clause here reaches `TryArmGuidedOpening`, so "a
    completed match on the save skips guidance" is pinned by `StratGuidanceInputGates.cpp` and
    `StratGuidanceRouteClauses.cpp` and NOT by this file. The slot name added here does not
    change that and must not be cited as coverage of it.
  - **THE OTHER TEN UNSLOTTED FIXTURE FILES: NONE PROVEN SENSITIVE, SO NONE EDITED.** The whole
    suite was run twice on identical binaries, save present and save absent, and **no test
    changed state between the two runs**; `Saved/SaveGames/` was still empty after the absent
    run, so nothing in the suite recreates the player's slot either. By route: every file that
    consults a save (`StratMatchCompletionRecording`, `StratGuidanceInputGates`,
    `StratGuidanceRouteClauses`, `StratDifficultyHandicapMatchClauses`,
    `StratProductionMenuSeam`) already names its own slot; of the ten unslotted ones, none
    calls a save API, none constructs an `FStratGuidedOpening`, and the three that could WRITE
    a slot (`StratMatchConclusion`, `StratMatchResultTrigger`,
    `StratViewingSideHandoverClauses`) already ASSERT `bRecordCompletionOnMatchEnd` false off
    `FStratMatchConfig()`. **LATENT, AND WORTH NAMING RATHER THAN EDITING:**
    `StratPathPreviewDecoratorClauses.cpp` and `StratAiPlaybackClauses.cpp` already spawn an
    `AStratPlayerController` and call `DecorateForPresentation` / `HandleSelectionEvent`
    directly. Neither reaches `RefreshFromMachine`, so neither is sensitive today; **one added
    refresh call in either would make it sensitive in one line**, and that is the day each
    needs a slot name.
  - **THE DEVELOPER'S SAVE WAS MOVED TWICE AND RESTORED EACH TIME BY CONTENT AND BY MTIME.**
    sha256 `c78b5af5f8c8dd890da4f57a0248900eccc6ed0042674b15a9a034f9bdb0cb9e`, 2076 bytes,
    mtime `2026-09-01 17:00:14.502327500 -0400`, identical before and after both moves --
    `mv` in both directions, never a copy-over. **Its mtime is 17:00 and the entry below dates
    the same file at 09:49**; that clock moved between the two passes and not in this one.
  - **NO CANONICAL REPORT WAS OVERWRITTEN.** Every run in this pass went to a scratch
    `-ReportExportPath`; `Saved/AutomationReport` still reads `reportCreatedOn
    2026.09.01-21.16.17`, which is the stamp `global.md`'s banner cites. The suite's figure is
    `global.md`'s to state and is not restated here.

- **2026-09-01 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base `cada741`, UNCOMMITTED
  at the time of writing) -- `T-INT-05.RefreshFromMachineDecoratesWithNoRegistration` WAS
  READING A `.sav` FILE IT NEVER NAMED. THE CLAUSE WAS WRONG; THE CONTROLLER WAS RIGHT.**
  - **THE VERDICT, AND WHICH SIDE IT ACQUITS.** `AStratPlayerController::RefreshFromMachine`
    did not stop decorating. `TryArmGuidedOpening` runs inside `DecorateForPresentation` and
    suppresses the whole §2.11.6 opening when `HasCompletedAMatchOnSave(FString())` is true --
    which is §2.11.6 in terms, *"any completed match on the save skips all guidance
    automatically"*. `ResolveSaveSlotName(FString())` returns `ActiveConfig.SaveSlotName`, and
    this file's `MakeConfig` never set one, so it resolved to the C++ default
    `StratocracyMatch` -- **THE PLAYER'S SLOT**. `Saved/SaveGames/StratocracyMatch.sav` exists
    on this box (2076 bytes, mtime 2026-09-01 09:49 local) and its only serialised property is
    `bHasCompletedAMatch : BoolProperty`. The opening was therefore correctly suppressed, the
    decoration correctly wrote an inactive block, and the clause asserted an active one.
  - **THE MEASUREMENT, WHICH IS A REVERSAL AND NOT A SINGLE OBSERVATION.** Clause run alone,
    three times, byte-identical source and binaries (`Build.bat` reported `Target is up to
    date` before the first): save present -> **Fail**; the same file moved aside and nothing
    else touched -> **Success**; the same bytes restored (sha256
    `c78b5af5f8c8dd890da4f57a0248900eccc6ed0042674b15a9a034f9bdb0cb9e`, verified before and
    after) -> **Fail**. All three went to a scratch `-ReportExportPath`; the canonical
    `Saved/AutomationReport` was NOT overwritten by any run in this pass and still holds
    `reportCreatedOn 2026.09.01-19.41.17`, the stamp `engine.md` and `global.md` cite.
  - **WHAT THIS EXPLAINS THAT NOTHING ELSE DID.** The morning-green / evening-red pair over
    identical source had no tested cause. A file appearing in `Saved/SaveGames/` between two
    runs is a cause of exactly that shape. **It is not proof of what wrote it:** the full suite
    run with the directory empty left it empty, so the suite does not write that slot --
    `FStratMatchConfig::bMayRecordCompletionOnSave` is doing its job. A PIE or packaged-build
    playtest is the remaining candidate and **is UNVERIFIED**; I did not attribute the file.
  - **WHAT THE CLAUSE'S OWN COMMENT GOT RIGHT, STATED BECAUSE IT WAS UNDER SUSPICION.** The
    block saying the scoreboard refusal happens *"long after `ApplyView` has already run and
    already pushed"* is **NOT stale**. The reported refusal was the scoreboard one, which is
    the LAST step of `RefreshFromMachine`, so `ApplyView` provably ran; the strip was unset
    because what was pushed was legitimately inactive. Read that comment as still true.
  - **THE FIX, ENTIRELY INSIDE `Tests/`.** `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp`,
    **40 insertions, 0 deletions** -- no existing clause byte moved. (1) A file-own
    `kAbsentSlotName` constant, matching `StratGuidanceInputGates.cpp`'s established pattern;
    (2) `MakeConfig` now sets `Out.SaveSlotName`; (3) the clause **asserts** the suppression
    premise via `H.Subsystem->HasCompletedAMatchOnSave(FString())` instead of inheriting it
    from whatever disk it runs on. Nothing was relaxed: the strip-parity assertion and the
    field-for-field comparison are untouched.
  - **THE FALSIFIABILITY PROOF, BECAUSE A NEW ASSERTION IS WORTHLESS UNTIL IT IS SEEN RED.**
    A valid `UStratSaveGame` payload carrying `bHasCompletedAMatch` was copied to
    `Saved/SaveGames/StratocracyAutomation_NoSuchSlot_GuidanceRoute.sav` -- the fixture's own
    slot -- and the clause failed **on the new line by name**: *"Expected 'the fixture's own
    save slot records no completed match, so §2.11.6 does not suppress the opening this clause
    is about' to be false."* The plant was removed and the clause returned to Success. That is
    the diagnosis the old clause could not give: it reported an unset strip and named no cause.
  - **WHAT THIS CLAUSE PINS, AND WHAT IT STILL DOES NOT.** It pins that `RefreshFromMachine`
    decorates through `DecorateForPresentation` directly, with NO decorator registered on the
    subsystem (`HasViewDecorator()` false) -- i.e. that decoration does not depend on
    `BeginPlay` having run. It does **NOT** pin suppression behaviour: `bSuppressed` is now
    false by construction on both sides, and the clause's module-side expectation still passes
    `false` to `FStratGuidedOpening::Begin` literally. **A clause that pins §2.11.6's
    suppression through the CONTROLLER's own arming path does not exist** -- the closest is
    `StratGuidanceInputGates.cpp`, which arms through the production path but with an absent
    slot in both arms. Writing one needs a fixture that plants and removes a `.sav`, which is
    a filesystem side effect no clause in this corpus currently takes; it is a real gap and it
    is stated rather than closed here.
  - **THE HAZARD CLASS, AND THIS FILE ALREADY KNEW ABOUT IT.** `StratGuidanceRouteClauses.cpp`
    carried prose naming *"treating a real default as an unset marker, in
    `FStratMatchConfig::SaveSlotName`"* and still did not point its own `MakeConfig` at a slot.
    **Knowing a defect class in prose is not applying the mitigation.** Nine other test files
    set `Out.SaveSlotName`; this one was the omission. **Any future fixture that starts a match
    must set it**, in BOTH directions of the failure: unnamed, a suppressed opening makes
    guidance clauses fail (here) or pass vacuously (`StratGuidanceInputGates.cpp`'s own block
    warns of the second), and neither symptom is about the code under test.
  - **No suite figure, count move or verdict is stated in this file;** `global.md` owns all
    three. **Note for whoever updates it:** the figure it currently carries was measured over
    the canonical report above, i.e. BEFORE this fix, and this entry deliberately does not
    restate or update it.

- **2026-08-31 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- `Source/*/Tests/`
  only -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base `2592276`, UNCOMMITTED
  at the time of writing) -- THE WIN64 **GAME** TARGET AND WHAT `Tests/` COULD AND COULD NOT FIX.
  ONE FILE GUARDED; THE REMAINING GAME-TARGET FAILURE IS NOT IN THIS LANE.**
  - **WHAT WAS CHANGED, AND THE PROOF IT WEAKENED NOTHING.**
    `Source/StratUI/Tests/StratMatchResultModelClauses.cpp` is now wrapped `#if WITH_EDITOR` /
    `#endif`, from just after its last `#include` to the last line. `git diff --stat` on that
    file reads **30 insertions and 0 deletions** -- the guard and its prose block and nothing
    else -- so no clause byte moved. All six clause names the file declares
    (`Stratocracy.StratUI.T-UI-03.*`) were confirmed present and `Success` in the exported
    report after the change, by set-membership against `Saved/AutomationReport/index.json`
    rather than by eye. **No suite figure, count move or verdict is stated in this file;**
    `global.md` owns all three and this entry links to it rather than restating any.
  - **WHY `WITH_EDITOR` AND NOT `WITH_METADATA` ON THE TWO CALL SITES.** The failing construct
    is `UEnum::HasMetaData` inside the file-local `AllTiers()` and `AllCauses()`, which filter
    `Hidden` enumerators off `StaticEnum<>()`. Guarding just those calls needs an `#else` arm
    that derives the enumerator set a second way -- a second implementation of the filter,
    living in the one target where no clause here runs and where therefore no gate covers it.
    `WITH_EDITOR` is the predicate `EAutomationTestFlags::EditorContext` already asserts, and it
    is 1 for `StratocracyEditor`, so the guard is inert on the editor suite BY CONSTRUCTION and
    not merely by inspection. **What the guard does NOT pin:** nothing. It is not a clause and
    it certifies no behaviour; it removes a translation unit from a target that could never have
    run it.
  - **WHY THE GUARD WAS NOT APPLIED TO THE OTHER SIXTY-NINE TEST FILES, AND THE COST OF THAT.**
    A blanket guard looks like a uniform no-op and is not one: six headers under
    `Source/*/Tests/` (`StratGuidanceRouteDouble.h`, `StratGuidanceRouteProbe.h`,
    `StratMatchResultHostDouble.h`, `StratMatchResultHostProbe.h`,
    `StratProductionMenuHostDouble.h`, `StratProductionMenuHostProbe.h`) declare `UCLASS`
    doubles that UHT parses and that other modules include and link across a module boundary --
    `StratPlay/Tests/StratMatchResultTrigger.cpp` includes `Tests/StratMatchResultHostProbe.h`,
    and two more do the same for the guidance-route and production-menu probes. **The cost is
    stated rather than left to be discovered: the next editor-only engine API used in ANY other
    test file reddens the Game target again, and nothing in this tree gates against that.**
    Whether any CI job builds the Game target at all is **UNVERIFIED** -- I did not read the
    workflow files -- so the sentence above is a statement about this tree's C++ gates and not
    about the runner.
  - **THE GAME TARGET STILL DOES NOT LINK, AND THE CAUSE IS OUTSIDE `Tests/` ENTIRELY. THIS IS
    THE MOST REUSABLE THING HERE.** With the guard in place the compile step passes and the link
    fails with **110 x `LNK2005`** and then `LNK1169`, each of the form
    `Ai.strat.cpp.obj : error LNK2005: "... strat::buildPriorityLess(...)" already defined in
    Ai.good.cpp.obj`. The twelve vendored rules translation units exist TWICE in this tree by
    design -- `Source/StratRules/*.good.cpp` compiled as the `StratRules` module, and
    `Source/StratBridge/Vendored/*.strat.cpp` compiled into `StratBridge` -- and the second copy
    is the recorded fix for the 8 x `LNK2019` that `.agents/ue-project-context.md` describes.
    **The editor target is MODULAR, so the two copies land in two different DLLs and the
    duplication is invisible; the Game target is MONOLITHIC, so both sets of objects land in one
    `Stratocracy.exe` and every `strat::` symbol is defined twice.** The architecture note's
    "measured 8 x LNK2019" reasoning is stated only over the modular case, and this is the other
    half of it. Fixing it needs a `.Build.cs` or module-registration change, which is not this
    lane's and not this task's blast radius, so it was left standing and reported.
  - **AN INSTRUMENT CAVEAT WORTH THE LINE.** `Build.bat` exits **6** on both failures above --
    the `C2039` compile failure and the `LNK1169` link failure -- and **0** on the editor
    success, so the exit code separates green from red but says nothing about which failure you
    are looking at. Read `Result: Succeeded` / `Result: Failed (OtherCompilationError)` and the
    linker's own lines. Both Game builds reported `Result: Failed (OtherCompilationError)`,
    including the one whose only error was a LINK error.

- **2026-08-31 (local), `strat-test-author` (ACTING and WRITING; IN LANE, in worktree
  `E:/MultiAgent/Strat-wt/slot-2` on `feat/w8-transient-receipts`, base `c754342`, lane commit
  `0a697c0`, MERGED TO `master` AS `ed09973`) -- W8 ITEMS (5) AND (6): THIRTEEN
  `GATE-TRANSIENT` CLAUSES FOR THE INCOME AND KILL RECEIPTS AND THE TURN BANNER, TWO OF WHICH
  PASSED THEIR FIRST MUTANT ROUND WHILE BEING INCAPABLE OF FAILING.**
  - **WHOSE WRITE THIS IS, SAID FIRST.** The clause bytes are this lane's, written by this lane,
    in this lane's own worktree, under a standing wave topology; this entry is this lane's too,
    **written directly rather than drafted for transcription**. Neither the transcription clause
    nor the in-session clause is invoked or needed, and the direct write is the stronger form for
    the reason the transcription clause states about its own limit (1): nothing in a checkout
    distinguishes a real transcription from an invented one. **The commits are a separate actor
    and are named as such:** the `coordinator` ran `0a697c0` and `ed09973` at the user's explicit
    instruction in session -- the user did not run git -- and the git author recorded on both is
    the user's identity. `engine.md`'s entry for this same lane makes the identical distinction;
    **that is one wave's convention recorded twice and not two independent findings**, and the
    W8 path-preview lane's own entry (lane commit `4f5e09e`, merged as `1576e40`) records it a
    third time for the same wave and the same reason. **No suite figure, count move or verdict is
    stated in this file**; `global.md`'s banner owns all three and this entry links to it rather
    than restating any.
  - **THE THIRTEEN, DERIVED BY SET DIFFERENCE ON THE MACRO AND NOT BY AN ACCEPTANCE-ID GREP.**
    Multiline regex over `IMPLEMENT_SIMPLE_AUTOMATION_TEST` across `Source/**/*.cpp`, both sides
    collected by ONE function -- `c754342` and `0a697c0` each read through `git show` -- which
    returns exactly these names, thirteen added and none removed. Collecting the two sides with
    one function is the point rather than a detail: two corpora gathered by different filters
    fabricate findings. Eleven in `Source/StratUI/Tests/StratTransientReceiptClauses.cpp`:
    `UnseededMarkEmitsNoReceiptsAndStillBanners`, `KillReceiptCarriesTheFameCombatRise`,
    `FameCombatFallOrHoldIsSilent`, `SideAbsentFromTheMarkIsSkipped`,
    `IncomeReceiptIsTheRateVerbatim`, `IncomeFiresOnlyOnTheTurnSidePair`,
    `KillReceiptsPrecedeTheIncomeReceipt`, `BannerFollowsSideToMoveAgainstTheSeat`,
    `ResultSilencesTheBanner`, `BannerChangeIsRelativeToTheMarksOwnInputs` and
    `ReceiptsAreOverwrittenNotAccumulated`. Two in
    `Source/StratPlay/Tests/StratTransientReceiptCallSite.cpp`: `ApplyViewRemarksAfterDeciding`
    and `MarkResetsWithTheAppliedModel` -- the two halves a world-free clause cannot reach,
    because their subject is `UStratMatchSubsystem` and `StratUI` sits below `StratPlay`. **All
    thirteen ride `GATE-TRANSIENT` and mint no acceptance ID**, per the 2026-08-29 W8 ruling.
    `T-FAME-02`, `T-FAME-07` and `T-TURN-01` are REFUSED for these items and appear in this lane
    only inside the prose that refuses them: a toast asserts that a receipt fired with the
    module's number and not that an award was computed correctly, and a banner asserts nothing
    about strict alternation, it follows a mirror of it.
  - **THE TWO CLAUSES THAT PASSED THEIR FIRST MUTANT ROUND WHILE BEING INCAPABLE OF FAILING, AND
    THE SPECIES THEY SHARE. THIS IS THE MOST REUSABLE THING IN THE WAVE.** Both are the same
    defect wearing different clothes: **a fixture in which the two candidate expressions the
    clause is meant to separate evaluate to THE SAME NUMBER**, so the assertion is satisfied by
    either and the clause certifies nothing while looking green.
    - `KillReceiptCarriesTheFameCombatRise` asserted that `Amount` is the RISE in
      `FStratSideView::FameCombat`. Its first fixture took `Before` from the seeded opening,
      where `FameCombat` is **zero** -- so `After - Before` and `After` are the same number, and
      a mutant carrying the absolute reading instead of the rise ran the whole suite green.
      Repaired by planting `Before` at the side's standing income rate (a module field, nonzero),
      pushing `FameTotal` down to that same value so the two fields stay distinguishable, and
      asserting that the two expressions DISAGREE before asserting the amount.
    - `BannerChangeIsRelativeToTheMarksOwnInputs` asserted that the previous banner is recomputed
      from the MARK's inputs. Its concluded-match case took the mark **over the already-concluded
      model**, so the mark's result flag and the model's could never disagree -- and a mutant
      recomputing the previous banner with the CURRENT result flag ran the whole suite green.
      Repaired by adding the case in which they DO differ: the mark taken while the match is
      live, a concluded model applied against it -- the refresh on which the match ends, which is
      the one transition the naive fix hides.
    - **THE GENERALISATION, WHICH IS WHAT A LATER READER WANTS.** A fixture must be checked for
      the coincidence BEFORE the mutant is written, by asking of each assertion: what is the
      other expression a wrong implementation would plausibly produce here, and does this fixture
      make it a different number? A zero, an identity and a value copied from the same source are
      the three ways it silently does not. The income arm's fixture was built to avoid exactly
      this and says so in its own comment; the same reasoning was simply not carried across to
      the other two clauses in the same file, which is how both survived review, a first mutant
      round, and the author's own reading.
  - **WHERE THE EXPECTATIONS COME FROM.** Every clause seeds a real `FStratBridge` from the
    shipped tables and scenario and builds a real `FStratViewModel` from it; no fame figure,
    income figure, side count or turn number is typed. The income amount is asserted against
    **`strat::UiSideView::incomePerTurn` off the snapshot the same bridge projected** -- one
    layer BELOW the view model -- and the turn edge that fires it is a real
    `FStratBridge::SubmitEndTurn` with the pair change asserted first. The banner clauses use two
    REAL models built for two seats off one bridge, so `ViewingSide` is the builder's answer.
    **One expectation in the pass is computed and it is declared:** the kill `Amount`, as the
    difference of two module-side reads, because no delta is published anywhere in this tree --
    `accrueIncome`'s return is unpublished, the bridge does not retain `FStratCombatOutcome`
    (its delivery is deferred by name to the `bridge_event_list` ruling), and no `UiSideView`
    field carries "fame gained since". Both operands are separately pinned to their own sources
    before the subtraction is named.
  - **THE MUTANT MATRIX, AND TWO CITATION DEFECTS IN MY OWN REPORT OF IT.** Fifteen mutants, each
    built IN PLACE in the lane tree -- never in a copy, whose cached `Intermediate/Build`
    resolves the original sources and reports success for a build that compiled nothing -- one at
    a time, whole suite each, reverted and sha256-verified. Every clause has at least one
    witness. Two defects in how it was REPORTED, both the species this lane spent the pass
    correcting elsewhere:
    - **A quoted artifact came from a different run than the one it was attached to.** The report
      said "every mutant restored: `RESTORED, hashes match: True`". The first matrix **never
      printed that line** -- `grep -c` returns zero on its output -- because it terminated early
      on an anchor failure and its closing restore never ran. The line quoted came from the
      SECOND run. Restoration was true and independently verified three ways (the loop-top
      restore runs before the patch, so the anchor failure wrote nothing; the tracked diff stayed
      additions-only; the CR count was unchanged), but the evidence cited was produced by another
      execution. **The anchor failure had its own cause worth keeping:** the multi-line anchor
      was written with `\n` while `StratMatchSubsystem.cpp` is CRLF, so it matched ZERO times in
      that file while matching fine in the LF files beside it. Line endings vary per file in this
      tree and a patch driver must try both spellings.
    - **The matrix was presented as uniform when eleven of its rows predated the two fixture
      fixes.** Those rows were measured against clause bytes that later changed. Closed by
      re-running the three rows that named an edited clause over the DELIVERED bytes, with
      identical results, so every row is now a measurement over bytes that are in the tree.
  - **SIX PROSE SITES WHERE THE COMMENTS STILL TOLD THE PRE-FIX STORY, TWO OF WHICH NO GATE
    REPORTED.** The reviewer flagged one paragraph; a sweep of this lane's own prose against its
    own delivered fixtures found six. All six were withdrawn in `STRUCK>` form with the
    superseded text KEPT, never edited to be true, because what the old text records is the
    defect that was caught. The two nobody reported are the ones worth naming:
    - **The kill block's FIRST paragraph, which the fixture moved under.** It said `DurableBefore`
      is asserted against the mark row "which `StratMarkFromView` wrote off the real projection".
      The repaired fixture OVERWRITES that row with the plant, so for the risen side the asserted
      value is the plant and not the projection. It stays true of every other side, which is what
      the per-side parity loop actually pins.
    - **The banner-change header still asserting a coverage this lane's own M10 mutant had
      DISPROVED.** It claimed the concluded-match case pins the result-flag defect. That mutant
      had already shown it does not. The body gained the new case during the fix and the header
      was left telling the superseded two-case story -- **the fix reached the code and not the
      prose that describes it**, in a block whose whole subject is a claim that was wrong.
    The other four: the file header's copy of the same wrong-field claim; an in-body comment
    **written during the fix itself** that was also wrong and also unmeasured; a cross-reference
    in `FameCombatFallOrHoldIsSilent` that died when the clause it pointed at was rebuilt; and
    the `StratPlay` file header carrying both the stale cross-reference and the same unmeasured
    mechanism. **A prose sweep after a fixture change is now part of this lane's own checklist**,
    because four of the six were introduced or falsified by the repair rather than by the
    original write.
  - **THE WRONG-FIELD MECHANISM WAS MEASURED, NOT REASONED -- AND THE REVIEWER'S OWN ACCOUNT OF
    IT WAS WRONG WHILE ITS CONCLUSION WAS RIGHT.** The struck prose claimed a decider watching
    `FameTotal` instead of `FameCombat` "would see no rise at all and the clause would go red on
    an empty list". The reviewer, correcting it, said such a decider sees a **fall**. Neither is
    what happens. Two wrong-field deciders were built in place against the delivered fixture:
    - **Comparing `FameTotal` while the mark still writes `FameCombat`: the list is never empty.**
      Side 0 HOLDS -- its mark row is the plant and its `FameTotal` was pushed down to match --
      but SIDE 1, whose mark row is untouched at zero, rises and fires. The clause reddens on the
      receipt's `Side`, on `DurableBefore`, on `Amount` and on the guard that `Amount` is not the
      reading itself.
    - **Swapping the field in both the mark and the comparison** reddens the per-side parity loop
      FIRST and only then on an empty list. So the empty list is real in exactly one of the two
      variants, and it is not the sharpest thing that fails; the parity loop is.
    **A reviewer can be right that a claim is false and wrong about why**, and this pass is the
    instance: the finding was correct, narrower than the defect, and its proposed mechanism did
    not survive being run. The lesson is not about the reviewer -- it is that a mechanism nobody
    executed is a guess whoever writes it, and the correction for an unmeasured claim is a
    measurement rather than a better-sounding claim.
  - **WHAT THESE THIRTEEN DELIBERATELY DO NOT PIN, AND WHY ASSERTING IT WOULD BE WRONG.** Victim
    identity, per-kill granularity, toast duration, stacking, animation and copy are all absent
    from the module under test ON PURPOSE, and the reasons are in
    `Source/StratUI/StratTransientReceipts.h`. **No clause asserts their absence**, because a
    clause over a known limitation goes RED the day the limitation is repaired -- the day the
    `bridge_event_list` ruling lands and the kill arm grows a `VictimUnitId`, a clause requiring
    no victim identity would fail for the feature being built. Ask of every clause what it does
    the day the thing it describes is fixed. Also unpinned: **`UStratMatchSubsystem::Deinitialize`'s
    copy of the mark reset**. `TearDownPresentation`'s copy IS pinned, reached through a second
    `StartMatch`; `Deinitialize` runs as the world dies, after which nothing survives to observe
    the mark, and pinning it needs a seam this lane may not add. **That is a stated gap, not an
    oversight.** And the kill rise is PLANTED rather than played: the only deterministic
    mid-match position in this tree is `Data/parity_fixture.save`, which needs
    `strat::parseSave` -- a vendored free function, `LNK2019` outside `StratBridge` and
    `StratRules`. Reaching a rules-produced kill needs either a bridge method that loads a save
    log, or the clause moved to `StratBridge/Tests`, where `StratBuildViewModel` is not visible.
  - **INSTRUMENT CAVEATS FROM THIS PASS, FOR THE NEXT ONE.** (a) `TestNotEqual`'s overload set is
    ambiguous over the bare `INDEX_NONE` macro while `TestEqual`'s is not -- measured, one
    `C2668`; spell it into an `int32` local. (b) `FString` comparison is case-insensitive here
    for `==`, `Contains` and `TestEqual` alike, and nothing in these thirteen compares two
    `FString`s for that reason; every value asserted is an integer or an enumerator, and
    enumerators go through `static_cast<int32>` so a failure prints the ordinal. (c) The suite
    report must be read from the EXPORTED report and not the log, which undercounts by exactly
    one every run. (d) **This lane's own three verification stamps from the lane tree --
    `-22.46.13`, `-23.06.21` and `-23.27.38` UTC -- are SUPERSEDED and must not be cited as
    current.** Each run overwrote the one before it, which is the standing hazard: one report
    path means every run falsifies every unstamped citation of the previous one, anywhere in the
    record. The live report identity and the count belong to `global.md`'s banner; this entry
    states neither. **[STAMPED 2026-08-31]**

- **2026-08-31 (local), `strat-test-author` (ACTING and WRITING; IN LANE, in worktree
  `E:/MultiAgent/Strat-wt/slot-1` on `feat/w8-path-preview`, base `c754342`, lane commit
  `4f5e09e`, MERGED TO `master` AS `1576e40`) -- W8 ITEM (1): THIRTEEN `T-UI-02` CLAUSES FOR THE
  PATH PREVIEW, AND EVERY ONE OF THE FOUR GATE ROUNDS AFTER THE ASSERTIONS WERE SETTLED WAS
  SPENT ON THE PROSE AROUND THEM.**
  - **WHOSE WRITE THIS IS, SAID FIRST.** The clause bytes are this lane's, written by this lane,
    in this lane's own worktree, under a standing wave topology; this entry is this lane's too,
    written directly rather than drafted for transcription. **Neither the transcription clause
    nor the in-session clause is invoked or needed**, and the direct write is the stronger form
    for the reason the transcription clause states about its own limit (1): nothing in a
    checkout distinguishes a real transcription from an invented one. **The commits are a
    separate actor and are named as such:** the `coordinator` ran `4f5e09e` and `1576e40` at the
    user's explicit instruction in session -- the user did not run git -- and the git author
    recorded on both is the user's identity. `engine.md`'s entry for this same lane makes the
    identical distinction; **that is one wave's convention recorded twice and not two
    independent findings.** No suite figure, count move or verdict is stated in this file;
    `global.md`'s banner owns all three and this entry links to it rather than restating any.
  - **THE THIRTEEN, DERIVED BY SET DIFFERENCE ON THE MACRO AND NOT BY AN ACCEPTANCE-ID GREP.**
    Multiline regex over `IMPLEMENT_SIMPLE_AUTOMATION_TEST` across `Source/**/*.cpp`, both sides
    collected by ONE function -- `c754342` and `1576e40` each read through `git show` -- which
    returns exactly these names with nothing removed. An ID grep is the wrong instrument here
    and has undercounted on this project before, because a name can carry an ID in prose without
    defining a clause and a clause can be defined without its ID appearing on the macro line.
    Six in `Source/StratBridge/Tests/StratPathPreviewParity.cpp`:
    `MovePathRouteIsFindPathsRoute`, `MovePathTicksAreReachableCosts`,
    `MovePathAnswersExactlyTheReachableSet`, `MovePathIncludesBothEndpoints`,
    `MovePathRefusesUnseeded`, `MovePathRefusesUnknownUnitId` -- plus a seventh added on the
    reviewer's finding, `MovePathArraysAreTheSameLength`. Six in
    `Source/StratPlay/Tests/StratPathPreviewDecoratorClauses.cpp`:
    `PathPreviewWritesUnconditionally`, `PathPreviewGoalIsTheHoveredHex`,
    `PathPreviewGateIsHasMovedNotDone`, `PathPreviewCopiesTheQueryAnswer`,
    `PathQueryForwardsTheBridgesAnswer` and `DecorateForPresentationPublishesThePreview`. **All
    thirteen ride `T-UI-02` and mint no acceptance ID**, per the 2026-08-29 W8 ruling.
  - **WHERE THE EXPECTATIONS COME FROM, WHICH IS THE ONLY QUESTION WORTH ASKING OF A PARITY
    FILE.** The bridge seven take theirs from `strat::findPath` and `strat::uiReachable` over the
    bridge's own `MakeUiWorld()`, and the refusal wordings from `FStratBridge::Reachable`'s own
    `Reason` on the same object in the same run -- no refusal literal is typed. The play six take
    theirs from a recording double's own payload, except `PathQueryForwardsTheBridgesAnswer` and
    `DecorateForPresentationPublishesThePreview`, which take theirs from
    `FStratBridge::MovePathToHex` on **the same bridge pointer** the production code reached.
    **One number in the pass is computed and it is declared a FOIL**, not an expectation: a
    fixture-richness control that subtracts two ticks to establish that some route under test
    enters a hex costing more than 1, so a board of uniform plains cannot satisfy the pass
    silently.
  - **WHAT THEY DO NOT PIN, AND THE MECHANISM, BECAUSE A READER WHO CANNOT RE-DERIVE THIS WILL
    FILE IT AS MISSING COVERAGE.** Two claims in `MovePathToHex`'s header are TRUE OF THE SOURCE
    and unobservable in every output it can produce. (1) **Which expression produced the
    ticks.** `strat::reachable`'s cost for a hex IS the summed `moveCost` along a cheapest path
    to that hex; `findPath`'s route is a cheapest path; and every prefix of a cheapest path is
    itself a cheapest path to its own last hex, because move costs are non-negative and a
    cheaper prefix could otherwise be spliced in to beat a minimal total. **So "look the cost up
    per hex" and "sum `moveCost` while walking the route" are the same number on every board** --
    not merely on *Ferrum Crossing*, and not merely on boards these fixtures build. A clause
    asserting a difference would be asserting against an identity.
    `MovePathTicksAreReachableCosts` therefore pins that each tick EQUALS the module's per-hex
    answer, which is real, and cannot pin which body produced it. (2) **Which expression produced
    `OutTotalCost`**, and the cause is the method's own cross-check rather than a weak fixture:
    it REFUSES every board on which the last tick and `findPath`'s `outCost` differ, so on every
    board that answers at all the two are equal by construction and the assignment is invisible
    from outside. (3) **The bridge's two internal refusal arms** -- a route hex absent from the
    reach set, and the cost disagreement itself -- are unreachable **by the same argument**: the
    check is what makes the disagreement impossible, so its own arm cannot fire. That is a proof
    and not a sample. **What would flip any of this:** a `uiPath` entry point vendored beside
    `uiReachable`, or the cross-check removed in favour of reporting both numbers -- either makes
    the assignment observable and the clauses buildable.
  - **FALSIFIABILITY WAS PROVED IN TWO HALVES BY TWO LANES, AND NEITHER HALF SUBSTITUTES FOR THE
    OTHER.** This lane built **thirteen test-side liveness perturbations** -- one expectation per
    clause, all built IN PLACE in the lane tree and never in a copy, eleven in one batch and two
    in a second after the last clauses landed. Each reddened **exactly its own clause** and left
    every other test in the tree green; all were reverted from byte backups -- the first batch
    verified by grepping for perturbation fragments, the second by `git hash-object` against
    the backup -- before the final run. That proves each assertion is live and wired to its
    subject -- the "assertion that could not fail" defect this file already records from the
    2026-08-31 input-claim pass -- and it proves nothing about discrimination of a real defect.
    **The five code mutants are `strat-gameplay-engineer`'s and are recorded in `engine.md`**:
    three died and two survived. **This lane REFUSED to build them**, because `Source/` outside
    `Tests/` is not its lane and the standing rule covers a temporary edit intended to be
    reverted; it specified them instead, and the engineer's measurement confirmed the two
    survivals this entry's mechanism paragraph explains. **A lane that had run its own code
    mutants would have discovered the same identity and had no clean record of who was entitled
    to write those bytes.**
  - **THE DEFECT THAT COST FOUR GATE ROUNDS WAS PROSE, AND IT RECURRED TWICE AFTER BEING FIXED
    ONCE -- WHICH IS THE FINDING, NOT THE FIRST INSTANCE.** The block over
    `MovePathTicksAreReachableCosts` claimed the clause forbade the summing body and named a
    board on which the two numbers differ. **`strat-integration-reviewer` returned BLOCK**; the
    claim was false in exactly the direction the engineer's surviving mutant had measured, and
    no such board exists. It was retracted in the house `STRUCK>` form -- the false sentences
    quoted, then the mechanism as the measurement that refutes them -- **and the opening sentence
    was struck too**, because retracting an argument while leaving its conclusion standing is the
    same shape as the original defect. Then the identical gap was found in the same file's *WHAT
    THIS FILE DOES NOT PIN* list, which named the cross-check arm but not the ticks. Then **two
    assertion failure strings** were found still claiming the discrimination the prose now
    disclaimed. **Each round was caught by a different reader and none by the suite**, which was
    green throughout: no instrument in this tree reads a comment, and a clause whose prose
    overclaims is green by construction.
  - **AND THE LOAD-BEARING PART: AN ASSERTION MESSAGE STRING IS COMPILED TEXT, NOT A COMMENT.**
    Both this lane and the `coordinator` described a pass as "comment-only" when it had altered
    an assertion's message. Behaviourally inert -- no predicate, no early return, no clause name
    moved -- but it is in the binary, a stale build reports the OLD string, and the distinction
    matters most at the moment a message renders: **a failure string is read when a reader is
    furthest from the prose and most likely to act on the line alone.** The two rewritten strings
    had been pointing a debugger at the summing hypothesis the retraction proves impossible and
    away from the causes the clause can actually produce. **The check that separates the two
    cases is a diff filtered for non-comment non-blank added lines**, which is cheap and was not
    run the first time. A clause NAME is likewise compiled; rebuild before the suite or a stale
    binary reports the old set green.
  - **A FLAKE, MEASURED AND NOT DIAGNOSED.** One verification run in the lane tree came back with
    **five StratBridge clauses red at once** -- `T-SAVE-06.RestoreFromSaveTextReachesTheSavedHash`,
    `T-SAVE-06.SerializeRefusesUnseeded`, `T-TURN-04.CappedMatchNamesTheKeyThatDiffered`,
    `T-UI-01.CombatOutcomeAgreesWithForecast` and
    `T-UI-01.ForecastIllegalAloneSetsOnlyLegalityDisagrees`. None was this lane's and none was
    under perturbation. Byte identity with the preceding green state was confirmed by
    `git hash-object` and **the identical bytes re-ran clean**. Recorded so the next reader does
    not attribute it to a change: five clauses reddening together in one run is the shape, and
    the correct first move is a re-run, not a diagnosis.
  - **TWO FIGURES CORRECTED IN THE MERGED TREE, BOTH MEASURED HERE.** (1) *"`MovePathToHex` is the
    tree's only `strat::findPath` call site"* -- stated unqualified in `4f5e09e`'s commit message,
    which is merged and not amendable -- **is FALSE as written and TRUE scoped to production
    code.** `grep -rn "strat::findPath(" Source/ | grep -v "^Source/StratRules/"` returns TWO:
    `Source/StratBridge/StratBridge.cpp:1446`, the production call, and
    `Source/StratBridge/Tests/StratPathPreviewParity.cpp:303`, this lane's own INDEPENDENT
    ORACLE. The second is legal precisely because that file is inside `StratBridge`, where the
    vendored symbols link, and it is what makes the route clause a parity test rather than a
    self-comparison. **The scoped form is the claim to carry forward.** (2) The
    `DecorateForPresentation` seam is now **eleven call sites across three test files**, not the
    nine across two it was before this lane, because `DecorateForPresentationPublishesThePreview`
    used that route.
  - **INSTRUMENT CAVEATS FROM THIS PASS, EACH ONE A THING THAT WOULD HAVE MADE A CLAUSE UNABLE TO
    FAIL.** `TestEqual` on `FString` is case-INSENSITIVE in this engine, as are `==` and
    `Contains`; the two refusal clauses use **`TestEqualSensitive`** (`AutomationTest.h`) so a
    re-cased wording cannot pass. **`TestNotEqual` has no `int32` overload** here though
    `TestEqual` does -- it is declared only for the string-ish types, so a numeric non-equality
    control must be written `TestTrue(A != B)`. **`bool` has no `TestEqual` overload to rely on**
    either; those comparisons are explicit. On the fixture side: **"unmoved" is not "selectable"**
    on *Ferrum Crossing*, because the guided opening locks every unit but the one it marks, so
    `DecorateForPresentationPublishesThePreview` ASKS the selection machine which unit it will
    accept rather than restating the guided opening's rule -- restating it would have been the
    very substitution `T-UI-02` exists to catch. **`HandleSelectionEvent` is stateful** and a
    second `HexPrimary` on a selected unit's own hex DESELECTS it, which left the machine at
    `-1` on a unit it had just selected; every selection is now preceded by `Cancel`. And a
    fixture that spawns the board actor emits `no tile mesh for terrain`, **declared** with
    `AddExpectedMessagePlain` on `StratAiMatchClauses.cpp`'s precedent rather than suppressed.
  - **WHAT THIS LANE DID NOT PIN AND WHY, beyond the unfalsifiable pair above.** A selection
    naming a unit absent from `Model.Units` -- the decorator returns false there and the
    declaration folds it into "nothing is selected". Any drawing: no spline, arrowhead, tick
    widget or overlay exists yet, and nothing here is on screen. And `Move.h`'s tie-break among
    equal-cost routes, which is the rules crew's gate; these clauses assert the bridge returns
    whatever `findPath` returned, which is the only half the bridge can get wrong.

- **2026-08-31 (local), in the INTEGRATION tree, base `edda819`, committed as `810c366`, editor
  closed for the build -- THE TITLE SCREEN'S INPUT MODE IS PINNED BY THREE CLAUSES, AND THE FIRST
  MUTANT ROUND FOUND THE DEFECT IN THE CLAUSES RATHER THAN IN THE CODE.** **ACTING: the
  `coordinator`, OUT OF LANE, on the user's explicit instruction in session -- it wrote the clause
  bytes, and `Tests/` is `strat-test-author`'s lane. WRITING THIS ENTRY: `strat-test-author`, in
  lane, in its own file.** Two actors, both named, because an entry naming one where two acted is
  a false claim about authorship. **This is NOT the transcription clause and is deliberately not
  dressed as one:** that clause is triggered by a MERGE and licenses only carrying a lane's draft
  across, and there was no worktree, no branch, no merge and no draft here. No suite figure, no
  count move and no verdict is stated in this file; `global.md`'s topmost banner owns all three
  and this entry links to it rather than restating any of them.
  - **THE THREE CLAUSES.** All ride `T-UI-02` -- the same acceptance ID as
    `StratPlayerControllerTick.cpp`, for the reason that file gives: T-UI-02 is the
    click-to-hex-to-reach chain, and this is the link before its first one. They mint no new ID
    and live in the new file `Source/StratPlay/Tests/StratInputClaimClauses.cpp`:
    `TheInputClaimClearsAViewportsIgnoreInput`,
    `TheInputClaimRestoresTheProjectsOwnMouseModes` and
    `BeginPlayMakesTheInputClaim`. Derived here by set difference on
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/**/*.cpp`, multiline, both sides collected by
    one function -- `edda819` read through `git show`, `810c366` likewise -- and it returns
    exactly those three names with nothing removed.
  - **WHAT EACH ONE PINS, AND THEY DO NOT RE-ASSERT EACH OTHER'S SUBJECT.** (1) the repair itself:
    `AStratPlayerController::RestoreProjectInputState` clears `bIgnoreInput` UNCONDITIONALLY,
    which is the field the reported defect is about. (2) that the restore reads the PROJECT'S
    `UInputSettings` rather than imposing an `FInputMode*`'s opinion, and that its
    `FApp::CanEverRender()` arm mirrors `UGameViewportClient::Init`'s headless rule instead of
    inventing one. (3) the ROUTE -- that `BeginPlay` calls `ClaimGameInput()` at all. (3) exists
    because this project has already paid for the mirror-image defect once: a correct mechanism
    with no caller reads as built from every other instrument.
  - **WHERE THE EXPECTATIONS COME FROM.** Clause (2) takes BOTH expectations from
    `GetDefault<UInputSettings>()`, the same object `Init` reads, so a project that changes its
    capture mode moves the clause with it rather than reddening it; nothing retypes
    `Config/DefaultInput.ini`. Its planted values are DERIVED to differ from the expected ones --
    if the project ever adopted `NoCapture` as its own default, planting `NoCapture` would make
    the restore arm pass by coincidence. Clause (1)'s `false` is a literal and is the shape in
    which a literal is correct: no Stratocracy value produces it, the engine's input pipeline
    imposes it.
  - **THE FINDING WORTH CARRYING: AN ASSERTION THAT COULD NOT FAIL, CAUGHT BY ITS OWN MUTANT.**
    The trailing `bIgnoreInput` assertion in clause (2) originally read a bare
    `UGameViewportClient` whose flag was already false from its own constructor, so it passed
    regardless of what the restore did -- the `SetIgnoreInput` mutant reddened clause (1) and left
    clause (2) GREEN. It now plants the flag before the call. **An assertion that cannot fail is
    worse than an absent one, because it reads as cover** -- and note that the clause looked
    complete on inspection and only a mutant separated the two.
  - **THE FALSIFIABILITY, MEASURED.** Three mutants, each built IN PLACE in this tree and never in
    a copy -- a copied tree's cached `Intermediate/Build` resolves the ORIGINAL sources and
    reports `Result: Succeeded` for a build that compiled nothing. Dropping `SetIgnoreInput(false)`
    reddened (1) and (2); defeating the `CanEverRender` guard reddened (2) alone; deleting the
    `ClaimGameInput()` call from `BeginPlay` reddened (3) alone.
  - **WHY (3) NEEDS THE UNREFLECTED `EStratInputClaim LastInputClaim`.** A headless world has no
    `UGameViewportClient`, so the claim can only take its no-viewport arm and leaves no other mark
    on any object a clause could reach. `NotAttempted` and `NoViewport` are distinct values on
    purpose and the clause reads the difference; without that member the deleted call site would
    redden nothing. Nothing on screen reads it and it is not reflected, so it is not the
    presentation bit T-INT-05 forbids.
  - **THE BOUNDARY, STATED HONESTLY: NO INSTRUMENT IN THIS TREE CAN SEE THE END-TO-END BEHAVIOUR.**
    `bIgnoreInput` is not a `UPROPERTY`, so `GetAll` cannot read it out of a live PIE session and
    there is no `ke` getter for it. These clauses pin the DECISION headlessly and nothing more.
    What confirmed that clicking works again after travel was **the user's own human playtest**,
    which is not re-executable from a checkout and is therefore not a gate. Also unpinned: that
    `FInputModeUIOnly` is what set the flag in the first place (the clauses plant it at its
    setter, not through Slate); the viewport client surviving `OpenLevel`; and any assertion that
    a click reaches `UPlayerInput` or a hex.

- **2026-08-31 (local), in the INTEGRATION tree, base `da06812`, editor closed -- THE CALL-SITE
  GAP IS CLOSED, AND THE CLAIM THAT IT NEEDED PIE WAS MY OWN, WAS NEVER MEASURED, AND WAS
  HALF WRONG.** ACTING AND WRITING are both the `coordinator`, OUT OF LANE, on the user's
  explicit instruction in session; not a transcription, and not dressed as one -- that clause
  needs a merge and a lane's draft, and there was neither. No suite figure and no verdict is
  stated in this file; `global.md` owns both.
  - **THE THREE CLAUSES**, in the new file `Source/StratPlay/Tests/StratShellHudCallSiteClauses.cpp`,
    all riding `GATE-TITLEMENU`:
    `AnUnconfiguredShellMakesTheSpawnedHudWaitInsteadOfDrawing`,
    `AConfiguredShellMakesTheSpawnedHudAskForItsMenuAtOnce` and
    `AHudWithNoMenuClassConsultsTheShellNotAtAll`.
  - **WHAT WAS WRONG WITH THE OLD CLAIM.** The entry below says reaching `ResolveMenuTiming`
    needs "a PIE-shaped fixture and not a headless one". That was inferred from the fact that
    the arm ends in `CreateWidget`, never measured, and it conflated the DECISION with the
    DRAWING. A probe measured them apart: a transient world, a `UGameInstance::
    InitializeStandalone` whose subsystem collection IS live, a spawned `APlayerController`
    assigned to `AHUD::PlayerOwner`, and `AActor::DispatchBeginPlay` reach the call site for
    real, with no PIE, no `InitializeActorsForPlay` and no `UWorld::BeginPlay`.
  - **THE DISCRIMINATOR IS `ConfigurationTicksWaited`** -- 1 when the wait arm ran, 0 when the
    create arm ran. That single integer separates the two paths through the call site, and it
    is why the property exists on the class. The unconfigured clause additionally asserts
    `LastFailureReason` is EMPTY, which is the assertion that kills the unconditional-create
    rewrite: entering the create arm in a viewport-less run always leaves a reason behind, so
    an empty one is positive evidence the arm was never entered.
  - **THREE ROUTES TO A GENUINELY SUCCESSFUL CREATE WERE MEASURED AND ALL THREE REJECTED**, and
    they are recorded so nobody re-walks them. (1) A local player via
    `UGameInstance::CreateInitialPlayer` works and `CreateWidget` on the shipped
    `WBP_TitleMenu_C` then SUCCEEDS -- but the call trips a HANDLED ENSURE on
    `IsDedicatedServerInstance()`, and a fixture that trips an engine ensure is not worth what
    it buys. (2) A concrete native widget class declared in the test file, to dodge the
    engine's refusal to construct `UUserWidget` itself ("Abstract, Deprecated or Replaced
    classes are not allowed"): UHT does not parse `.cpp` files, measured as `fatal error C1083`
    on a `.generated.h` that cannot exist. (3) The shipped `WBP_TitleMenu_C` without a local
    player: loads from its package fine, fails to create exactly as the native class does. **So
    the fixture needs no `/Game/` path, and declines the `Tests/` exception it is entitled to.**
  - **TWO EXPECTED-MESSAGE DECLARATIONS, AND THEY ARE DIFFERENT KINDS OF THING.** Mine, at
    EXACTLY ONE occurrence, is an ASSERTION -- zero means the create arm was never entered, two
    means it created twice, and both redden. The engine's "Only Local Player Controllers can be
    assigned to widgets" is a SUPPRESSION at any count, labelled as one, because pinning an
    engine message would make the clause fail on a reword and that finding would be about
    nothing.
  - **FALSIFIABILITY, AND MUTANT D IS THE ONE THAT MATTERS.** Three mutants, each built in place
    in this tree with its own unit confirmed recompiled. (D) `ResolveMenuTiming` stops
    consulting `DecideMenuTiming` and always creates -> the unconfigured clause alone went red.
    (E) the call site always waits -> the configured clause alone went red. (F) `BeginPlay`
    loses its early return on an unset menu class -> the no-menu-class clause alone went red.
    **IN ALL THREE ROUNDS THE FOUR OLD DECIDER CLAUSES STAYED GREEN** -- and under (D) that is
    not a control, it is the point: (D) is the exact rewrite those four were documented as
    blind to, and it is now caught. Reverted, rebuilt, re-run green.
  - **WHAT IS STILL UNREACHED.** The menu ever APPEARING -- no viewport exists, so `IsMenuLive()`
    is false on every path here and no clause asserts otherwise. The re-arming timer FIRING; the
    world never ticks. `ApplyMenuInputMode`, which runs only after a create that cannot succeed.
    Only a live PIE session sees a menu, and a screenshot is not a test.

- **2026-08-31 (local), in the INTEGRATION tree, base `ee4acf5`, editor closed -- THE SHELL
  HUD'S DECIDER IS PINNED BY FOUR CLAUSES, AND THE MUTANT RUN FALSIFIED A CLAIM IN MY OWN
  COMMENT BEFORE IT PROVED ANYTHING ABOUT THE CODE.** This entry's ACTING AND WRITING are both
  the `coordinator`, OUT OF LANE, on the user's explicit instruction in session. **It is NOT a
  transcription and is deliberately not dressed as one:** that clause is triggered by a MERGE
  and licenses only carrying across a draft `strat-test-author` already wrote, and there was no
  lane, no worktree, no branch, no merge and no draft here. No suite figure and no verdict is
  stated in this file; `global.md` owns both and this entry links to it rather than restating
  it.
  - **THE FOUR CLAUSES.** All ride `GATE-TITLEMENU`, mint no acceptance ID -- there is no
    acceptance ID for this screen and inventing one would put a string in a test name that
    greps exactly like a defined one -- and live in the new file
    `Source/StratPlay/Tests/StratShellMenuTimingClauses.cpp`:
    `AConfiguredShellIsDrawnFromAtEveryPointInTheWaitBudget`,
    `AnUnconfiguredShellIsWaitedForUntilItsShippedBudgetIsSpent`,
    `AZeroWaitBudgetDrawsTheMenuRatherThanWaitingForATick` and
    `TheShellsConfiguredFlagRecordsTheCallAndNotItsArguments`.
  - **WHAT THEY PIN, AND WHY THE OBVIOUS CLAUSE IS STILL REFUSED.** The 2026-08-31 entry below
    records this lane refusing to pin `BP_StratShellGameMode`'s one-tick hop, because a clause
    asserting the CURRENT ordering pins a hazard as a requirement and reddens on its own
    repair. That refusal stands. `ee4acf5` made the honest clause writable by building
    `AStratShellHUD`, which asks whether the shell HAS BEEN CONFIGURED rather than assuming an
    ordering; what is pinned is that a configured shell is drawn from at once and an
    unconfigured one is waited for to a budget. Moving `ConfigureMatchDestination` ahead of
    `Super::BeginPlay()` keeps every clause green, which is the property that makes them worth
    having.
  - **WHERE THE EXPECTATIONS COME FROM.** The wait budget is READ FROM `AStratShellHUD`'s class
    default object, never typed, so this file is not a second author of a shipped number. The
    three arms are compared as ENUM VALUES and never as text -- `FString` comparison in this
    engine is case-insensitive for `==`, `Contains` and `TestEqual` alike, which once shipped a
    clause here that could not fail -- and arm names are reflected out of `StaticEnum` for the
    FAILURE MESSAGE only, where being wrong costs readability and not a verdict.
  - **THE VACUITY GUARD IS LOAD-BEARING IN THE BOUNDARY CLAUSE.** It loops over `[0, Budget)`.
    At a budget of zero that body would never run and the clause would pass having observed
    NOTHING, which is this project's recorded "a clause can pass while measuring the defect"
    shape. The budget is therefore asserted positive BEFORE the loop, so a zero budget reddens
    the clause instead of silencing it -- and the zero case is covered by its own clause.
  - **THE FALSIFIABILITY, MEASURED.** Three mutants, each edited INTO THIS TREE and rebuilt in
    place. Never in a copied tree: a cached `Intermediate/Build` there resolves the ORIGINAL
    sources and `Result: Succeeded` stops being evidence that anything was compiled, so each
    round asserted its own mutated translation unit was named in its own build log before the
    run was trusted. (A) budget consulted BEFORE the configured flag -> the first clause alone
    went red. (B) the subsystem flag tidied into `return !MatchLevel.IsNull();` -> the fourth
    clause alone went red. (C) `<=` for `<` -> the boundary and zero-budget clauses went red
    and no others. **In all three rounds the 24 pre-existing `GATE-TITLEMENU` clauses stayed
    green**, which is the control that the new clauses caught each defect rather than the suite
    reacting to a broken build. Reverted, rebuilt, re-run green, and `git diff` against
    `ee4acf5` empty.
  - **A CLAIM IN MY OWN COMMENT WAS FALSE AND MUTANT (C) IS WHAT FOUND IT.** The zero-budget
    clause's comment said no other clause in the file would see an off-by-one. The boundary
    clause sees it too, at `Waited == Budget`, for as long as the shipped budget is positive.
    That is an over-broad claim about a clause's subject. The comment was narrowed to what the
    clause is actually worth -- the ONE configuration the boundary clause cannot observe, a
    budget of zero, where its loop runs no iterations -- and the correction was made BEFORE the
    mutants were run for record rather than left to be discovered by a gate.
  - **WHAT NO CLAUSE HERE REACHES, AND IT IS THE BIGGEST ONE.** **Nothing pins that
    `AStratShellHUD::ResolveMenuTiming` ACTUALLY CONSULTS `DecideMenuTiming`.** These clauses
    pin the decider, not the call site; a `ResolveMenuTiming` rewritten to create the menu
    unconditionally leaves all four GREEN -- re-measured since as mutant D, which did exactly
    that and left all four green.
    **[STAMPED 2026-08-31 -- THE NEXT SENTENCE WAS FALSE. It is quoted rather than deleted,
    and the entry ABOVE this one carries the measurement that falsified it.]**
    RETRACTED>  "Reaching it needs a spawned HUD with an owning player controller and a live
    RETRACTED>   game instance carrying the subsystem, which is a PIE-shaped fixture and not a
    RETRACTED>   headless one."
    It was inferred rather than measured, and it was half wrong: the DECISION is reachable
    headlessly and only the DRAWING is not. `StratShellHudCallSiteClauses.cpp` closes this gap
    without PIE. Still unreached, and now the whole of it: the menu ever APPEARING, the created
    widget and its Z-order, the UI-only input mode's effect on the controller, and the
    re-arming next-tick timer FIRING.

- **2026-08-31 (local), in the INTEGRATION tree `E:/MultiAgent/Stratocracy` on `master`, base
  `e4a21b0`, editor closed -- W6'S ASSET TAIL EARNED TWO CLAUSES AND ONLY ONE OF THEM IS
  HONEST.** `strat-test-author`, acting and writing. No worktree, no merge and no commit, so no
  exception block applies and none is claimed -- this is an ordinary in-lane write to this lane's
  own file. **Clause delta +1, REMOVED 0, derived by me in this tree** by set difference on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/**/*.cpp`, collected by ONE function on both
  sides -- base `e4a21b0` read through `git show`, the working tree read from disk. **The suite
  figure and the verdict are `global.md`'s topmost banner and are not restated here**; that banner
  moves in the same commit that brings this clause in.

  **THE CLAUSE THE ENGINEER RECORDED AS OWED IS PAID.**
  `Stratocracy.StratPlay.GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot`, in the new
  file `Source/StratPlay/Tests/StratShellBlueprintSlotParity.cpp`. It loads both
  Blueprint-generated classes, reads `SaveSlotName` off the title GameMode's CDO and
  `MatchConfig.SaveSlotName` off the match GameMode's, and asserts the two strings equal
  CASE-SENSITIVELY. Neither side is authored in the test: it pins a RELATION between two
  independent module-side reads and never states what the slot ought to be called, so a rename
  applied to BOTH assets stays green -- which is correct, because the shipped name is a designer's
  to choose and the subject is agreement, not identity with a literal. It discharges the debt
  `StratShellGameMode.h` states in terms under `SaveSlotName` -- *"The discharge is unchanged -- a
  clause reading both Blueprint CDOs and asserting the two strings equal, once the editor lane has
  created them"* -- and the same debt `GATE-TITLEMENU.TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn`
  names in its own "does not pin" block. Both effective values read `StratocracyMatch`, re-derived
  here from the mutant's own failure message rather than carried in from a dispatch brief.

  **WHAT IT DOES NOT PIN**, each stated because each has been mistaken for it: either value
  against `FStratMatchConfig`'s C++ default -- that is
  `…TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn`'s subject, and asserting it here would
  redden on a correct rename applied to both assets; `MatchLevel`, `TitleLevel` or any map's World
  Settings; `BP_StratGameMode_AiVsAi` or any third GameMode; and that anything ever WRITES to the
  agreed slot. Every lookup failure -- package, generated class, CDO, native-instead-of-Blueprint
  -- is fatal red and never a skip, on `StratShippedGameModeOptIn.cpp`'s precedent.

  **THE CASE-SENSITIVE COMPARISON IS LOAD-BEARING, AND THAT WAS MEASURED RATHER THAN ARGUED.** Two
  BUILT mutants, both inside this lane: the drift was injected at the READ of the shell CDO,
  because `Content/` is the editor lane and a `.uasset` edit was not mine to make. **(A)**
  uppercasing the shell read reddened the clause, and the framework's own message printed the two
  values it had read. **(B)** the SAME mutant with the comparison swapped to the naive `==` came
  back **Success**. So a `TestEqual`/`==` version of this clause would have been INERT over the
  exact drift it exists to catch -- this project's recorded case-insensitivity hazard, caught this
  time before it shipped. Both mutant builds recompiled the file and relinked
  `UnrealEditor-StratPlay.dll`, so neither was the silent no-op a copied tree produces. Reverted
  from a byte copy taken before mutation, rebuilt, and the reported run is of the reverted tree.

  **THE SECOND CLAUSE WAS REFUSED, AND THE REFUSAL IS THE FINDING.**
  `AStratShellGameMode::BeginPlay` calls `Super::BeginPlay()` FIRST -- which is what fires a
  Blueprint's `Event BeginPlay` -- and only afterwards calls `ConfigureMatchDestination`, so a
  widget built directly off that event reads an unconfigured shell and greys its travelling rows.
  `BP_StratShellGameMode`'s graph hops one tick past it. Three reasons not to pin it, heaviest
  first. **(1) A CLAUSE ASSERTING THAT ORDERING WOULD GO RED THE DAY THE ORDERING WAS FIXED.**
  Move `ConfigureMatchDestination` above `Super::BeginPlay()` and the defect is gone and the
  clause fails: it would pin a hazard as a requirement and block its own remedy, which is worse
  than no clause. **(2)** The CONSEQUENCE is already pinned twice --
  `GATE-TITLEMENU.EveryRowsEnablementAndReasonComeFromIsRoutePermitted` and
  `GATE-TITLEMENU.ContinueNamesTheMissingLevelAheadOfTheMissingSave`, the second taking the
  missing-level refusal FROM the function under test as its oracle. A third clause there is a name
  and no coverage. **(3)** The half that protects the player is the GRAPH's hop, and no clause in
  this suite has a viewport or a cursor. It rests on `ScreenShot00071`, `00072` and `00073` and a
  PIE positive control, recorded in `content.md`, and a screenshot is not a test.

  **WHAT WOULD MAKE IT WRITABLE, filed as an engineer-lane request rather than written weaker.**
  The shell HUD class with an `EditDefaultsOnly` widget-class slot that `content.md` already names
  as an engineer-lane item, creating the menu from C++ AFTER `ConfigureMatchDestination` has run.
  That turns the property into *"the shell is configured before the menu is asked for its model"*,
  which is a requirement rather than a hazard and reddens in the right direction. **Until that
  seam exists this property is UNPINNED**, and no clause name in this tree may be read as covering
  it.

- **2026-08-30 (local), in the LANE tree `E:/MultiAgent/Strat-wt/slot-1` on `feat/title-menu`, base
  `347c722` -- W6'S TITLE/MENU SHELL IS PINNED BY 23 CLAUSES, 22 OF THEM ON THE LOCAL GATE NAME
  `GATE-TITLEMENU` AND ONE ON `T-TURN-09`.** `strat-test-author`. **This entry is a RECORD-ONLY
  pass**: it was dispatched with no build, no suite run, no `Source/` edit and no commit, so every
  figure below that is not marked as my own measurement is somebody else's, and is attributed.
  **Clause delta +23, REMOVED 0, re-derived by me in this tree** by set difference on
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/**/*.cpp`, collected by ONE function on both
  sides -- base `347c722` read through `git show` versus the working tree read from disk. It agrees
  with the `coordinator`'s census exactly, including the 22/1 split by ID. **The wave's suite figure
  and its verdict are `global.md`'s topmost banner and are not restated here.** The gate rounds are
  `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md`, which is the authority on the wave.

  **WHERE THE CLAUSES LIVE.** `Source/StratPlay/Tests/StratShellRouteClauses.cpp` is new and holds
  22. `Source/StratPlay/Tests/StratSaveSlotClauses.cpp` gains ONE --
  `GATE-TITLEMENU.TheLoaderRefusesInTheRestorabilityPredicatesOwnWords` -- because it needs a
  configured `UStratMatchSubsystem` and that file owns the world/slot/config fixtures.
  `Source/StratPlay/Tests/StratMatchCompletionRecording.cpp` gains **no clause**: its ten added
  lines are a comment saying that an existing assertion's MESSAGE names the loader while its
  SUBJECT is the writer, and naming the two new clauses that now back the sentence.

  **WHY THERE IS NO MODULE-SIDE ORACLE HERE, AND WHAT THESE FILES DO INSTEAD.** `strat::` knows
  nothing about screens and the GDD states no route set, so the ordinary parity shape has no module
  to ask. The answer taken is to pin RELATIONS BETWEEN TWO INDEPENDENTLY-WRITTEN FUNCTIONS instead
  of retyping a switch as an expectation: order and length against `StaticEnum<EStratShellRoute>()`;
  every row's enablement and reason against `UStratShellSubsystem::IsRoutePermitted` under the same
  facts; every refusal sentence taken FROM the function under test under a fact set where only one
  refusal is possible, then asserted to appear where both are; `ResolveDestination` against
  `IsRoutePermitted` rather than against a route-to-level list. **No label, refusal or precondition
  string is typed anywhere in either file.**

  **THE INVENTORY, BY WHAT EACH GROUP HOLDS DOWN.**
    - **The model's shape and its one authority (3).** `MenuCarriesEveryDeclaredRouteInReflectedOrder`
      -- one row per declared route in the enum's own declaration order, in all sixteen fact
      combinations, which is what entitles every other clause to address a row BY INDEX rather than
      by matching label text on a case-insensitive engine.
      `EveryRowsEnablementAndReasonComeFromIsRoutePermitted` -- `BuildMenuModel` carries
      `IsRoutePermitted`'s answers rather than a second copy of the conditions.
      `ADisabledReasonIsEmptyExactlyWhenTheRowIsEnabled` -- both directions, with the sweep COUNTING
      what it saw and failing if it saw only enabled rows or only disabled ones, because a guard
      goes inert on its own subject. **It pins consistency, NOT correctness** -- the file says so
      itself so the pair is not mistaken for coverage of the model.
    - **Refusal precedence, in opposite orders (2).** `ContinueNamesTheMissingLevelAheadOfTheMissingSave`
      and `ReturnToTitleNamesTheAbsentMatchAheadOfTheAbsentLevel`. Same oracle shape both ways, plus
      a VACUITY GUARD: the two sentences must DIFFER first, compared case-INsensitively, which is
      the stricter reading of difference on this engine. Neither can pass with the two checks
      swapped.
    - **The exit (1).** `QuitIsPermittedAndUnexplainedInEveryFactCombination` -- permitted and
      carrying no shortfall sentence in all sixteen combinations, on BOTH surfaces (the direct
      permission call and the drawn row).
    - **Destination resolution without loading a level (3).** `PermittedTravelAlwaysResolvesADestination`
      (the heaviest clause: every travelling route against every configuration of the two pointers,
      facts derived FROM those pointers), `NewMatchAndContinueResolveToTheOneMatchDestination` (the
      sameness is the load-bearing half -- two destinations for start and restore would be two
      answers to the question `T-TURN-09` keeps single), and
      `ResolveDestinationDecidesWithoutLoadingTheLevel`, which pins the `IsNull()`-and-not-`IsValid()`
      property: an implementation asking `IsValid()` would refuse every correctly configured
      destination in a shipped project while every fact-only clause stayed green.
    - **One label moves, and only one (1).** `OnlyTheLiveMatchLabelMovesWithTheFacts` --
      `bMatchIsLive` changes the `NewMatch` row's LABEL and nothing else about it, and no other
      row's label. No label text is typed.
    - **The pending-load-slot lifecycle (1).** `ThePendingLoadSlotSurvivesOnePeekAndOneConsumeOnly`
      -- `ArmPendingLoadSlot` writes, `PeekPendingLoadSlot` reads without clearing,
      `ConsumePendingLoadSlot` answers once and clears. A player who continues and then restarts
      must not be handed the old save on the second travel.
    - **The slot pairing, over the extracted static `UStratShellSubsystem::PendingSlotForRoute` (5).**
      `OnlyTheRestoringRouteCarriesASlotToArm` (routing only -- emptiness against
      `RouteLoadsSaveSlot`, no string compared), `TheArmedSlotIsTheConfiguredOneAndNotABakedInName`
      (the value -- the argument byte for byte), `AnUnconfiguredSlotArmsNothingOnEveryRoute` (no
      default, no fallback, no stand-in; kept separate on purpose so a red names one subject),
      `ARouteThatDoesNotRestoreClearsAStaleArmedSlot` (a behaviour the follow-up pass CREATED: the
      arming is now unconditional, so a `NewMatch` route overwrites a stale `Continue` request), and
      `ARefusedRouteArmsNothing` -- the one clause that calls `ExecuteRoute` for real, pinning the
      one ordering fact the refusal path makes observable: **the permission check precedes the
      arming.**
    - **Restorability -- the four clauses added after the shipped defect (4).**
      `ACompletionOnlyPayloadIsNotARestorableMatch` (a current header with no match text is not
      restorable, with the version equality ASSERTED as a premise so a later change would report
      that the subject moved rather than passing for a new reason),
      `TheRestorabilityRefusalIsEmptyExactlyWhenRestorable` (the out-parameter contract in both
      directions over states that actually produce both),
      `AFinishedButUnsavedMatchLeavesContinueGreyed` (end to end through a real slot file: the file
      EXISTS and the slot is NOT restorable, and that gap IS the defect -- the only menu clause in
      the file whose fact is read from the module rather than constructed, and therefore the only
      one that would have gone red on the shipped tree), and
      `TheSlotQuestionReturnsThePredicatesRefusalUnchanged`
      (`DoesSlotHoldARestorableMatch` hands back `IsPayloadRestorable`'s refusal UNCHANGED).
      Plus, in `StratSaveSlotClauses.cpp`, `TheLoaderRefusesInTheRestorabilityPredicatesOwnWords`:
      the loader's failure text CONTAINS the phrase the predicate returned FOR THE SAME PAYLOAD,
      derived not retyped, so a loader that went back to restating the conditions in its own
      perfectly correct words goes red.
    - **The derived default (1).** `TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn` --
      `AStratShellGameMode`'s `SaveSlotName` class default EQUALS `FStratMatchConfig`'s because the
      constructor derives it. It reddens the moment somebody reintroduces a literal, which is the
      shape that drifts silently.
    - **`T-TURN-09` (1).** `ARestartRouteRequestsNoSavedStateToSeedFrom`. **THE STRETCH IS STATED AT
      THE CLAUSE AND MUST NOT BE READ AS DISCHARGING THE ID.** `T-TURN-09` reads *"same scenario ->
      identical state, with an empty command prefix"*; this clause reaches ONLY the seeding half. It
      runs no two matches, compares no two state hashes and does not travel. Both directions are
      asserted (`ContinueMatch` must load a slot, `NewMatch` must not) so a `RouteLoadsSaveSlot`
      returning false for everything cannot satisfy it.

  **`T-SAVE-04` WAS PROPOSED FOR THE RESTORABILITY CLAUSES AND WAS REFUSED BY THIS LANE, AGAINST
  THAT ID'S OWN GDD SENTENCE. RECORD THIS OR IT WILL BE PROPOSED AGAIN.** The GDD defines
  `T-SAVE-04` as *"refusal: any header mismatch (version/rules/data/scenario hash)"* and says it
  *"refuses on the header alone and never applies a command"*. **A completion-only payload's header
  is VALID AND CURRENT -- that is the entire reason the defect shipped** -- and an empty body is not
  a header mismatch under either sentence. A clause riding an ID whose sentence excludes its subject
  is the unfalsifiable shape this project has already been caught by, so they ride `GATE-TITLEMENU`,
  which the user's 2026-08-29 ruling puts the screen and its routes on. The version arm's rule
  remains squarely `T-SAVE-04`'s and keeps its existing clause
  (`Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAWrongSavedDataVersion`); no new clause restates it.
  Stated precisely, because a looser sentence stood in the file earlier the same day:
  `TheRestorabilityRefusalIsEmptyExactlyWhenRestorable` DOES construct a wrong-version payload, as
  one of four states exercising the OUT-PARAMETER contract, and asserts nothing about the version
  rule.
    - **THE GATE REPORT SAYS "all four new clauses ride `GATE-TITLEMENU` instead"; THE TREE CARRIES
      FIVE UNDER THAT REASONING, AND THE TREE WINS.** The fifth is
      `TheLoaderRefusesInTheRestorabilityPredicatesOwnWords` in `StratSaveSlotClauses.cpp`, whose
      own comment restates the same refusal in its own words. The report's sentence sits in its
      round-3 section and is true of the four in the restorability section; it is not a count of
      everything the refusal covers. Nothing about the reasoning changes -- only the number of
      clauses it reaches.
    - **REPORTED, NOT FIXED, AND CORRECTLY NOT FIXED.** The pre-existing
      `Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAnUnconfiguredSubsystem` is likewise outside that
      sentence: it refuses BEFORE `LoadGameFromSlot` is called, so there is no header to mismatch.
      Renaming it is not this wave's change to make, and it is recorded here so the next reader
      meets a known item rather than a discovery.

  **WHAT IS STILL NOT REACHED, WITH THE DISTINCTION THE GATE BLOCKED TWICE ON.** The five slot
  clauses pin `UStratShellSubsystem::PendingSlotForRoute` -- **THE STATIC**. *Which slot
  `ExecuteRoute` arms* -- that it passes that static's answer, and not some other string, to
  `ArmPendingLoadSlot` -- is a **different subject that no clause reaches**, and so is *that the
  arming call is unconditional*: putting the `if` back around it reddens nothing.
  **THESE TWO ITEMS ARE THE SAME SINGLE LINE OF CODE, and a reader who closes one has not closed
  the other.** Neither does the record now swing to the opposite over-claim: the DECISION is pinned
  five ways where it was pinned none, which is strictly more than before and is worth what it cost.
  Also unreached: the permitted arm of `ExecuteRoute` in its entirety (reaching it means travelling,
  a viewport and a world); `GatherFacts` (it needs a `UGameInstance` with a world and a save file --
  `ARefusedRouteArmsNothing` causes it to RUN and asserts nothing about what it returns, and
  executing a function is not pinning it); and any ordering beyond "the permission check precedes
  the arming".

  **THE CALL-SITE DERIVATION THAT STANDS IN FOR THE MISSING CLAUSE, AND IT IS WORTH MORE THAN THE
  MUTANT IT REPLACED** because it says WHY nothing catches a change to that line rather than
  reporting THAT nothing did -- and because a checkout can re-execute a grep and cannot re-execute
  a mutant somebody ran and discarded. **Re-run from a checkout:**

        grep -rn "ExecuteRoute(" Source/ | grep -v "://" | grep -- "->ExecuteRoute("

  **I ran it in this tree and got ONE line**, the call inside `FStratShellRefusedRouteArmsNothingTest`,
  whose own fixture asserts the route is REFUSED -- so it returns before `ArmPendingLoadSlot`.
  **The unfiltered form returned SIX in the same tree, so the middle filter is doing real work**,
  and the naive form is the self-quoting-census trap this branch already fell into: the test file's
  own prose contains the token it tells you to search for.
  **THE TRIGGER THAT VOIDS THE DERIVATION ENTIRELY: a second `ExecuteRoute` caller appearing.**
  Whoever adds one owns rewriting it. Cited by symbol and never by line, because that call site
  moved 1421 to 1489 to 1521 inside a single session.

  **THE DEAD MUTANT INSTRUMENT, WHICH IS THE MOST TRANSFERABLE THING THIS WAVE PRODUCED.** Mutants
  are built in a disposable `robocopy`'d COPY of the tree, because a test author may not edit the
  code under test even temporarily. **A copied tree carries a copied `Intermediate/Build`, whose
  cached build graph still resolves paths in the tree it was generated in -- so UBT concludes there
  is nothing to do and says so successfully.** Every "mutant build" was a no-op printing
  `Result: Succeeded`, the first mutant run came back GREEN, and the obvious reading -- "these
  clauses are inert" -- was FALSE and one step from being reported. **Proved three ways rather than
  inferred:** (1) an unconditional `AddError` planted in the clause never fired and the suite still
  read all-passing, and a clause that RUNS cannot pass with an unconditional error in it; (2) `grep`
  on the built `UnrealEditor-StratPlay.dll` found the probe string ABSENT and the clause name
  PRESENT -- the state in which every name looks right and every behaviour is stale; (3) the DLL was
  deleted and the build re-run, which reported `Result: Succeeded` AND EMITTED NO DLL AT ALL.
  **Fixed by clearing `Intermediate/Build` in the copy, FIRST and once, at the cost of a full module
  rebuild.**
    - **`Result: Succeeded` IS NOT EVIDENCE THAT A BUILD HAPPENED. THE ARTIFACT IS.** Same species
      as this project's recorded *"exit code is not a verdict"*, arriving through a different door.
    - **THE ASYMMETRY, WHICH IS THE PART TO CARRY.** A RED mutant implies the mutation reached the
      binary -- a no-op build leaves the baseline in place and the baseline is green -- so **red
      implies a relink incorporating the change, ABSENT A FLAKE**. The qualifier is not decoration;
      see the flake note below. **A GREEN mutant implies NOTHING WHATEVER until the artifact is
      checked**: it is equally explained by an inert clause and by a build that never ran.
    - **AND WHEN YOU GREP THE BINARY, GREP FOR SOMETHING THAT MOVED.** An identifier that has been
      in the tree for several rounds proves only "some build since that round". The discriminating
      probe is a string that CHANGED THIS ROUND -- superseded wording zero in source and binary,
      current wording present in both. Note that only strings that COMPILE can be probed: a `.cpp`
      comment reaches no binary, so a comment-only round leaves nothing to grep for. A header doc
      comment above a `UPROPERTY`/`UFUNCTION` DOES survive, as UHT `Comment`/`ToolTip` metadata --
      which is why the deleted identifier `bSaveSlotExists` still greps in the DLL, as quoted
      history inside `bSaveSlotIsRestorable`'s tooltip. **A removed identifier found in a binary is
      alarming and here it meant freshness, not staleness. Check the sense of a hit before reporting
      it.**

  **THE HONEST LIMITS OF THIS ENTRY'S EVIDENCE, STATED RATHER THAN LEFT TO BE DISCOVERED.**
    - **The mutant sets M1-M10, N1-N5 and R1-R4 are agent self-reports that a checkout cannot
      re-execute.** They are not in the tree in any form, and the gate report says in terms that no
      gate round built the project or ran the suite. Citing that file for a per-clause mutant table
      would be the same defect one directory over.
    - **N6 WAS GREEN.** An earlier draft's *"M1-M10, N1-N6 and R1-R4 all produced reds"* was false,
      and three sites cited it, two of them saying *"measured ... not reasoned"* -- disclaiming the
      derivation that would have rescued them. The two clauses that leaned on a green mutant now
      rest on the call-site derivation above instead.
    - **The test file's claim that every clause in it is justified by a built mutant is a universal
      quantifier over an agent self-report**, and it was written in the same wave in which two of
      those runs turned out not to have happened. It is retracted in place and narrowed in the file;
      read it as narrowed. **What a reader CAN check is the census command, the clause bodies, and
      the derivations that replaced the two green mutants.**
    - **One prior state is not re-derivable by anyone from a checkout**: that the shell previously
      called `UGameplayStatics::DoesSaveGameExist` directly rests on gate readings taken while
      `StratShellSubsystem.cpp` was untracked.
    - **WHAT I DID NOT MEASURE IN THIS PASS.** I did not build, did not run the suite, did not run a
      mutant, and did not edit anything under `Source/`. The clause-name set difference and the two
      `ExecuteRoute` greps above are mine, taken in this tree; every other figure is attributed to
      the agent that took it.

  **KNOWN FLAKE, RESTATED BECAUSE THE MUTANT ASYMMETRY DEPENDS ON IT.** This project has recorded a
  roughly 1-in-4 red flake in the suite. That is why the inference is *red implies relinked, absent
  a flake* and never the absolute form. Across a mutant set whose every red is semantically matched
  to its own subject -- the clause that went red is the clause naming the broken property -- a flake
  is not a credible explanation, but that is the reader's judgement to make.

  **INSTRUMENT CAVEAT THAT APPLIES TO EVERY CLAUSE IN THESE FILES.** `FString` `==`, `Contains` and
  `TestEqual` are CASE-INSENSITIVE in this engine, measured in this project where it once produced a
  clause that could not fail. The two files use the comparisons in OPPOSITE directions on purpose:
  where the claim is SAME, `ESearchCase::CaseSensitive` (the stricter reading of sameness); where
  the claim is DIFFERENT, the default case-insensitive comparison (the stricter reading of
  difference, since two strings differing only in case are then reported equal and the clause goes
  red). **A clause asserting "different" case-sensitively, or "same" case-insensitively, would pass
  over a capitalisation-only mutation. Neither shape is present -- check that any clause added to
  these files keeps it.**

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

> **[OUT-OF-LANE WRITE, 2026-08-29, AND IT IS NOT A TRANSCRIPTION -- STATED FIRST BECAUSE THE
> CLAUSE THAT NOW SITS IN THIS FILE'S SOLE-WRITER LINE DOES NOT COVER IT. The entry directly
> below was ACTED AND WRITTEN BY THE `coordinator`, WHOSE FILE THIS IS NOT.** There was no lane
> agent, no worktree, no merge and no lane-authored draft: the user instructed the `coordinator`
> directly, in the main tree on `master`, to fix `LayerFor` and the clause that pins it. The
> transcription clause licenses carrying across what a lane already wrote, AFTER A MERGE; neither
> condition holds here, so this block and not that clause is what makes the write readable.
> **ACTING AND WRITING ARE ATTRIBUTED SEPARATELY EVEN THOUGH THEY ARE THE SAME AGENT**, because
> the point of the requirement is that a reader can tell, and "both were the `coordinator`" is
> the fact rather than an omission. **WHAT THIS BLOCK DOES NOT DO:** it licenses this write and
> nothing else, and it does not make the `coordinator` a fallback writer for this file.]**

- **2026-08-29 (local; the run that backs this entry stamps UTC `2026.08.30-03.48.37`, in the MAIN
  tree `E:/MultiAgent/Stratocracy` on `master`) -- THE CLAUSE THAT WAS OWED FOR `LayerFor` EXISTS
  NOW, AND IT WAS PROVED RED BY REBUILDING THE DEFECT IT PINS.** Acted and written by the
  `coordinator` (see the block above; there was no lane). One clause added to the existing
  `Source/StratPlay/Tests/StratBoardChurnClauses.cpp`:
  `Stratocracy.StratPlay.GATE-BOARDCHURN.AMeshAssignedAfterAnUnmeshedApplyDrawsOnTheNextApply`.
  **Clause delta +1**, by MULTILINE set difference on `IMPLEMENT_(SIMPLE|COMPLEX)_AUTOMATION_TEST`
  over `Source/**/*.cpp`, collected by ONE function on both sides -- HEAD `d3efe78` versus the
  working tree; nothing was removed or renamed. The live suite figure is `global.md`'s and is not
  restated here.
  - **TWO HALVES, AND THE SECOND ONE EXISTS BECAUSE `strat-integration-reviewer` BLOCKED THE
    FIRST SHAPE OF THE FIX** (gate report
    `Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md`). The find path was first
    written to re-read the mesh configuration ONLY when the component had none; the gate measured
    that justification as false and the remediation removed the guard. Widening it is a behaviour
    change, so it is PINNED and not merely argued: the second half changes `FallbackTerrainMesh`
    on a board that is already drawing and requires every tile layer to follow. It applies a
    CHANGED model to get there -- one hex popped, the idiom
    `AChangedHexListRebuildsAndPickingFollowsIt` already uses -- because a mesh reconfiguration is
    not a model change, so on an identical list `DrawsExactlyTheseHexes` would early-out and
    `LayerFor` would never be reached. **THAT IS A REAL REMAINING GAP AND THE CLAUSE IS SHAPED SO
    AS NOT TO ASSERT IT AWAY;** it is carried in `engine.md`, not here.
  - **WHAT THE FIRST HALF ASSERTS, AND WHY IT IS UNCONDITIONAL.** A board is applied with no tile mesh at all
    (so its layer components are created null-meshed -- asserted as a PRECONDITION, not assumed:
    the first apply must report the gap and must draw zero, or the fixture is not on the path the
    clause is about); `FallbackTerrainMesh` is then assigned by the same reflection helper the
    neighbouring clauses use; the IDENTICAL model is applied again and MUST draw. No `if/else`
    over the branch the board took -- that shape is exactly what
    `strat-integration-reviewer`'s Observation 4 measured as unpinnable, and it is what clause 4
    deliberately keeps for its own subject.
  - **IT READS THE COMPONENTS AND NOT ONLY THE COUNT.** `GetDrawnHexCount` sums `InstanceHexes`,
    the parallel array; a fix that populated that array while leaving every component meshless
    would satisfy it and leave the board blank. So the tile layers are gathered by the board's own
    `HexAtInstance` answer -- the identification rule this file already records -- and each one's
    `GetStaticMesh()` and `GetInstanceCount()` are read directly, with the instance total required
    to equal the model's hex count.
  - **THE FALSIFIABILITY PROOF IS TWO MUTANTS, ONE PER HALF, AND EACH REDDENS ITS OWN HALF AND
    NOTHING ELSE.** Both were built in the MAIN tree, `Result: Succeeded` each time, with the
    eight `GATE-BOARDCHURN` clauses re-run over each.
    **M1 -- the pre-fix find path** (`return Existing;` with no re-read): the clause went `Fail`
    on BOTH halves -- *"Expected 'and every hex of the model is drawn' to be 99, but it was 0"*,
    *"...to be "", but it was "no tile mesh for terrain Bridge, Factory, Mountains, Plains, Town,
    Water, Woods ..."*, and then *"Expected 'the changed model applies cleanly' to be true"*.
    **M2 -- the NARROWED guard the gate blocked** (`&& GetStaticMesh() == nullptr` restored): the
    clause went `Fail` on the SECOND HALF ONLY, seven times, once per tile layer, every one
    *"Expected 'every drawing tile layer now wears the CHANGED mesh' to be true"* -- and the first
    half stayed green, which is what proves the two halves are not measuring the same thing.
    **Under BOTH mutants exactly one of the eight clauses moved**, clause 4
    (`AnUnmeshedBoardIsNeverRememberedAsDrawn`) among the seven that stayed `Success`, confirming
    it holds on both sides of the fix exactly as it was written to. The fix was restored from a
    byte copy taken before each mutation, rebuilt, and the full suite re-run.
    **NOT REPRODUCIBLE FROM THIS CHECKOUT** -- all three mutants were built and discarded in
    place, and only this paragraph records that they existed.

- **2026-08-29 (local; the run that backs this entry stamps UTC `2026.08.29-22.04.45`, in the MAIN
  tree `E:/MultiAgent/Stratocracy` on `master`) -- THE BOARD'S NO-CHURN EARLY-OUT IS PINNED BY
  SEVEN CLAUSES, AND EVERY ONE OF THEM WAS PROVED RED BY BUILDING A MUTANT THAT BREAKS IT.**
  `strat-test-author`, in the main tree, against `strat-gameplay-engineer`'s UNCOMMITTED fix in
  `Source/StratPlay/StratBoardActor.{h,cpp}`. One new file inside `Tests/`:
  `Source/StratPlay/Tests/StratBoardChurnClauses.cpp` (7). **Clause delta +7**, by MULTILINE set
  difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `^Source/**/*.cpp` at `1a3520b` versus the
  working tree, collected by ONE function on both sides; nothing was removed or renamed. The live
  suite figure is `global.md`'s and is not restated here.

  **WHAT MADE THIS HARD, STATED FIRST BECAUSE IT IS THE WHOLE POINT.** The fix
  (`AStratBoardActor::DrawsExactlyTheseHexes`, asked before `ApplyHexes` clears anything, plus the
  same guard in `FillOverlay`) changes NO OUTCOME a headless assertion can read: same instance
  count, same `InstanceHexes`, same `HexAtInstance` answers, because a rebuild from the same model
  reproduces exactly what it produced. **A clause asserting the resulting count or the picking
  answers passes identically with the fix reverted and pins nothing** -- this record already
  carries that failure shape under "a pin that asserts a copy of its subject". The only
  distinguishing fact is WHETHER THE RENDERER WAS TOUCHED.

  **THE INSTRUMENT IS A MUTATION PLANTED BEHIND THE BOARD'S BACK.** Instance 0 of each component
  is moved to Z = 1000 through `UpdateInstanceTransform` -- a write the class never performs and,
  decisively, NEVER READS: `DrawsExactlyTheseHexes` compares `InstanceHexes` and instance COUNTS
  and never a transform, so the probe cannot steer the decision it measures. The same call is then
  made again; survival is possible only if `ClearInstances` was never called. Every survival clause
  is paired with a POSITIVE CONTROL that demands the marker die, because a probe that cannot see a
  real rebuild passes exactly like an inert clause.

  **THE ID IS `GATE-BOARDCHURN`, A LOCAL GATE NAME. It mints no acceptance ID and claims none**
  (`GATE-MAPMARKERS` / `GATE-INFOPANEL` precedent). **`T-UI-02` WAS PROPOSED BY THE ENGINEER AND
  IS REFUSED**, on the acceptance set's own text: §4.11 states T-UI-02 as *"the reachable-hex
  highlight displays exactly the T-MOVE-01 set"*, and `StratBoardPicking.cpp` stretches it to the
  axial->world map and its inverse. Both are claims about WHAT IS DRAWN. Nothing in the new file
  has what-is-drawn as its subject -- every clause is about whether the RENDERER WAS TOUCHED while
  what is drawn stayed identical, which no GDD acceptance ID covers. Filing it under T-UI-02 would
  make that ID's discharge depend on a property it was never written to carry. (The dispatch brief
  described T-UI-02 as "presentation-layer arithmetic"; that is `T-UI-03`. Neither fits.)

  **THE SEVEN CLAUSES, ALL `Stratocracy.StratPlay.GATE-BOARDCHURN.*`:**
    - `ReapplyingTheSameHexesTouchesNoTileInstance` -- the identical model applied twice leaves
      every tile layer's planted marker in place. THE CLAUSE THE FIX EXISTS FOR.
    - `AChangedHexListRebuildsAndPickingFollowsIt` -- control: one hex dropped, every marker dies,
      the drawn count is the SHORTENED MODEL'S OWN count, every instance sits at the board's own
      `WorldLocationOfHex`, and the dropped hex is named by no instance.
    - `AReorderedHexListRebuildsAndPickingFollowsIt` -- control: same length, two SAME-TERRAIN
      entries swapped, markers die and `HexAtInstance` walks the reordered order. Same-terrain is
      load-bearing: swapping two hexes of DIFFERENT kinds leaves each layer's own sequence
      untouched, so the drawing is identical and the early-out is RIGHT to fire.
    - `AnUnmeshedBoardIsNeverRememberedAsDrawn` -- the board never reports a clean draw it has not
      performed.
    - `ReshowingTheSameOverlayHexesTouchesNoInstance` -- the same property for `ShowReach`,
      `ShowTargets` and `ShowObjective`, all three markers surviving.
    - `AChangedOverlaySetRebuilds` -- control for the three overlays.
    - `AnOverlayMeshArrivingLateStillDraws` -- `FillOverlay` caches what was DRAWN, so an
      `OverlayMesh` arriving at `BeginPlay` after a request makes the identical request draw.

  **WHERE THE EXPECTATIONS COME FROM.** The hex list is `FStratViewModel::Hexes` off a bridge
  seeded with the shipped scenario; the two changed models are that model with one edit each, so
  the changed expectation IS the changed model. Every "the marker died" location is
  `Board->WorldLocationOfHex(...)` for the hex `HexAtInstance` names -- the axial->world formula is
  read, never repeated. Overlay counts are read BY NAME through `GetTargetOverlayCount` /
  `GetObjectiveOverlayCount`. Tile layers are identified by `HexAtInstance` answering for them and
  overlays by which single component's count MOVES under its own call -- positive identification,
  never by component name and never by elimination. **The one invented value is the marker Z**, and
  it is a FIXTURE value, not an expectation: it is planted and then looked for, and no module-side
  value exists for "a Z the board would never write".

  **FALSIFIABILITY, MEASURED AND NOT ARGUED.** Five mutants were built and run on a DISPOSABLE COPY
  of the tree at `E:/MultiAgent/Strat-mutant` (since deleted); no file under test was edited in
  this repository at any point. The pristine copy ran first as a control and was green there. Then,
  each mutant applied alone to `StratBoardActor.cpp`, rebuilt, and the gate re-run:
    - **M1**, the tile early-out call deleted -> `ReapplyingTheSameHexesTouchesNoTileInstance` RED
      on all 7 layers.
    - **M2**, the `FillOverlay` early-out deleted -> `ReshowingTheSameOverlayHexesTouchesNoInstance`
      RED on all 3 overlays.
    - **M3**, an early-out that treats an unmeshed layer as already satisfied ->
      `AnUnmeshedBoardIsNeverRememberedAsDrawn` RED: *"a board reporting a clean draw has drawn
      every hex of the model' to be 99, but it was 0"*.
    - **M4**, an early-out that fires for ANY model once the board has been drawn once (tiles and
      overlays) -> `AChangedHexListRebuildsAndPickingFollowsIt`,
      `AReorderedHexListRebuildsAndPickingFollowsIt` and `AChangedOverlaySetRebuilds` RED. The
      controls are live.
    - **M5b**, `FillOverlay` caching the REQUEST and its instance-count agreement dropped ->
      `AnOverlayMeshArrivingLateStillDraws` RED.

  **WHAT M5 FOUND, AND IT IS A FACT ABOUT THE CODE RATHER THAN THE TEST.** M5 -- the cache
  recording the request, with the count agreement LEFT IN -- turned NOTHING red. `FillOverlay`
  carries two independent protections and either one alone defeats that bug: the count agreement
  `Overlay->GetInstanceCount() == DrawnHexes.Num()` already refuses a cache that claims a highlight
  the component is not drawing. So `AnOverlayMeshArrivingLateStillDraws` pins the CONJUNCTION and
  not each half; removing either one alone leaves it green. Anyone deleting one of those two
  conditions as redundant should know the suite will not stop them.

  **[STAMPED 2026-08-29, LATER THE SAME DAY, BY THE `coordinator` (out-of-lane; see the block at
  the top of this file). THE PARAGRAPH DIRECTLY BELOW IS NOW HISTORY AND IS STAMPED WHERE IT SITS
  RATHER THAN DELETED, so a reader arriving by a citation lands on the correction.** Its account
  of what was true is accurate as of the pass it describes. What has changed: `LayerFor`'s find
  path now re-reads `TerrainMeshes`/`FallbackTerrainMesh`, so **THE BOARD DOES DRAW**; the code change the paragraph says "this lane may not make" was made by the
  `coordinator` on the user's direct instruction; and the `DrawsExactlyTheseHexes` sentence the
  paragraph quotes as "currently claim[ing] the stronger property" does not stand in the code and
  did not at `d3efe78` either -- it is a `RETRACTED>` line there, and the paragraph reads as
  though it were live prose. WHETHER IT WAS ALREADY RETRACTED WHEN THAT PARAGRAPH WAS WRITTEN IS
  NOT SETTLED HERE: the retraction was committed in `136f863` at 23:29 UTC and the run backing
  that entry stamps 22:04:45 UTC, but the entry says it worked against an UNCOMMITTED fix, so the
  commit time does not answer it and this stamp does not pretend otherwise. The clause that pins
  the draw is the topmost entry in this file.]**

  **WHAT `AnUnmeshedBoardIsNeverRememberedAsDrawn` DOES NOT PIN, AND WHY -- THIS IS A FINDING.**
  The brief asked for "apply with `FallbackTerrainMesh` unset, assign it, apply the same model, the
  board must draw". **IT DOES NOT DRAW, and the early-out is not the reason.**
  `AStratBoardActor::LayerFor` assigns a layer component's static mesh ONCE, at creation, so a
  layer created during an unmeshed apply keeps a null mesh forever; the second apply DOES rebuild
  (`DrawsExactlyTheseHexes` correctly returns false) and skips every hex again. The clause's own
  run output records which branch it took: *"the second apply reported the gap again"*. Asserting
  "0 drawn" would have frozen that gap into the suite and gone red the day it is fixed, so the
  clause asserts the invariant that holds on both sides of the fix. **Closing it needs a code
  change this lane may not make:** `LayerFor` (or `ApplyHexes`) must re-apply
  `TerrainMeshes`/`FallbackTerrainMesh` to an existing layer whose component has no mesh. The
  class's own `DrawsExactlyTheseHexes` block currently claims the stronger property -- *"which is
  what makes a mesh assigned after the fact take effect on the very next refresh"* -- and that
  sentence is true of the EARLY-OUT and false of the BOARD.

  **AN INSTRUMENT TRAP MEASURED HERE, WORTH MORE THAN THE CLAUSES.** The first M1 run came back
  GREEN ON EVERY CLAUSE with the fix deleted, and the mutant was real. A copied tree that carries
  its `Intermediate/` carries GENERATED BUILD RULES FULL OF ABSOLUTE PATHS TO THE TREE IT WAS
  COPIED FROM: the build launched with `-project=E:/MultiAgent/Strat-mutant/...` compiled the MAIN
  tree's sources and wrote the MAIN tree's `Binaries/`, so the run measured pristine code and said
  so convincingly. It was caught by the DLL timestamps -- the mutant tree's binary was older than
  the main tree's. **Delete `Intermediate/` and `Binaries/` in any copied tree before building
  it**, and check that the copy's own DLL is the one that moved.

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

---

## 2026-09-05, `strat-test-author` — the mutant battery's two findings against this lane, over base `374398a`

Fifteen mutants were run in place by `strat-gameplay-engineer` with a clean rebuild between each.
Thirteen killed exactly their clause. The two that did not are recorded here, with a third defect
of the same shape found by construction while correcting the first. **Suite figures for this pass
live in `Tools/architect/state/global.md`; nothing is restated here.**

- **A CLAUSE CAN PIN A REAL PROPERTY AND CARRY A FALSE PREDICTION ABOUT A SECOND ONE, AND THE
  PREDICTION IS THE MORE DANGEROUS ARTIFACT.**
  `Stratocracy.StratPlay.GATE-TITLEMENU.TheTitleMenuLabelsItsOptionsRowByRouteAndNotByIndex`
  genuinely pins **by-route-not-by-index** — the fixed-index mutant reddens it correctly. Its
  `MUTANTS:` block also predicted "Red" for a **hardcoded-caption** mutant
  (`OptionsLabel->SetText(FText::FromString(TEXT("Options")))`). **That mutant survived.**
  - **Why.** `BuildMenuModel`'s `EStratShellRoute::Options` arm labels the row with the literal
    `TEXT("Options")` and labels it *from no fact* — its own comment says the row "has no second
    spelling". So the model's string and the hardcoded string are byte-identical in every state a
    fixture can reach. **An equality clause cannot separate a value read from the model from the
    same value written beside it.** No wording, no premise and no extra control changes that.
  - **The general form, and it is the transferable part: a comparison against a CONSTANT
    module-side value is not evidence that the value was read.** "Never compute the expectation"
    is satisfied here — the expectation IS read from `GetMenuModel()` — and it still does not
    discriminate, because the module-side value has exactly one possible content. **When the
    module-side value cannot vary, reading it proves the number matches and not that the code
    consulted it.** Ask, before writing such a clause: *can any fixture make this value differ
    from what a hardcode would produce?* If not, say so in the file.
  - **A false annotation is worse than no annotation, which is why the line was corrected rather
    than deleted.** A reader takes "Red," as measured. The corrected block in
    `Source/StratPlay/Tests/StratTitleMenuOptionsRowClauses.cpp` now names each mutant as KILLED,
    SURVIVED-MEASURED, or SURVIVES-BY-CONSTRUCTION, and the file's `WHAT THIS DOES NOT PIN`
    block leads with label-from-model.
  - **THE THIRD MUTANT IN THAT SAME BLOCK IS ALSO GREEN, DERIVED FROM THE SOURCE AND NOT RUN.**
    `OptionsButton->SetIsEnabled(true)` unconditionally. `IsRoutePermitted`'s `Options` arm
    returns `true` with no branch on any fact, so `OptionsRow.bEnabled` is `true` in every
    reachable state and the enabled-copy assertion compares `true` against `true`. The clause is
    kept (it goes live the day that arm grows a refusal, which is where its own comment says a
    refusal belongs) and is now annotated at the assertion as non-discriminating. **One
    contradicted prediction in a block is a reason to re-derive every other line in it.**
- **IS LABEL-FROM-MODEL REACHABLE? NO, NOT WITHOUT A SEAM — AND THE SEAM IS NAMED RATHER THAN
  ASKED FOR.** `RefreshOptionsRow` calls `Shell->GetMenuModel()` itself, so no fixture can hand
  it a model whose Options label differs from the shipped literal, and `GetSubsystem<T>()`
  returns the exact class so a broadcasting/overriding subsystem subclass is not reachable
  either. The only change that would reach it: splitting the lookup from the write —
  `RefreshOptionsRow` keeping the `FindByPredicate` and delegating to an
  `ApplyOptionsRow(const FStratShellOption&)` a fixture could call with a planted row — in
  `Source/StratPlay/StratShellMenuWidget.h` / `.cpp`, which is not this lane's to make. **It is
  recorded as the change that would be needed, not requested.** The existing clause is green
  either way and loses nothing.
- **A MUTANT THAT CRASHES THE PROCESS ERASES ITS OWN EVIDENCE, AND THE COUNT CANNOT TELL A
  CRASHED MUTANT FROM A KILLED CLAUSE.**
  `Stratocracy.StratUI.GATE-TITLEMENU.ACommandBarWithNoOptionsButtonConstructsAndBroadcastsNothing`
  is a good clause and the guard it pins is real. Its mutant — dropping the
  `if (OptionsButton != nullptr)` guard in `NativeConstruct` — is a null dereference that takes
  `UnrealEditor-Cmd.exe` down with an access violation **before the report export step**, so
  `Saved/AutomationReport/index.json` **is never written**. A reader then opens the file this
  project treats as authoritative and reads **the previous run's** `failed: 0`.
  - **The check that separates them: compare `index.json`'s own `reportCreatedOn` against the run
    you just started. If the timestamp did not move, the run produced no report and every figure
    in it is stale.** This belongs beside the existing "exit code is not a verdict" and "the log
    undercounts by exactly one" — same family: **the instrument's silence reads as a good
    result.**
  - **The clause cannot do this from inside itself** and no rewrite makes it able to: a clause
    cannot report on a process that died before it could report. The caveat is the deliverable.
    Recorded at the clause in `Source/StratUI/Tests/StratCommandBarOptionsClauses.cpp`.
  - The engine LOG does name the running test alongside the access violation. The old annotation
    said only that — **true of the log, false of the report, and the report is what gets read.**
- **CORRECTED: `bSyncingBoundWidgets`'s re-entrant path IS reachable and IS already pinned.** The
  superseded bullet above claimed `USlider::SetValue` does not broadcast headlessly. It does:
  `Slider.cpp` calls `MySlider->SetValue` inside `if (MySlider.IsValid())` and then, in a
  **separate, unguarded** `if (Value != InValue)` block, assigns and calls
  `HandleOnValueChanged`, which broadcasts. **The old reasoning was right about `SSlider` and the
  widget under test is the `U`.** Deleting the three handler guards reddens three existing
  clauses; **no seam is needed and none is coming.** What is still NOT pinned is the *recursion
  brake* — the `if (bSyncingBoundWidgets) return;` at the top of `SyncBoundWidgetsToModel`, a
  different line — which stays green when deleted alone, because the handler guards stop the loop
  one level earlier. `Source/StratUI/Tests/StratOptionsWidgetBindingClauses.cpp` now carries the
  split.
  - **The lesson, and it is about this file as much as that clause: a premise recorded as
    "Measured on this tree" was not measured on this tree — it was reasoned from the wrong class
    in a two-class pair (`U*` wrapping `S*`) where both have a method of the same name.** The
    engineer measured the premise instead of accepting it, which is what caught it. **When a
    record says a path is unreachable, that claim is a mutant away from being checkable and
    should be checked before it is cited.**
