# Stratocracy state — engine

> **Sole writer: `strat-gameplay-engineer`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** The `StratBridge` / `StratPlay` / `StratUI` C++ record — module arrows, deliberate absences, deferred production surface, and the branches a citation must name.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

- **2026-08-27, `strat-gameplay-engineer` -- THE HOVER IS POLLED ON TICK. THE ENHANCED INPUT
  HOVER ROUTE WAS MEASURED DEAD AND HAS BEEN REMOVED, AND THE INSTRUMENT THAT MEASURED IT IS
  GONE WITH IT.** In the integration tree `E:/MultiAgent/Stratocracy` on `master`, from
  `5e0333f`, on top of the instrumentation entry below -- which this entry DISCHARGES. Two files
  modified, both mine: `Source/StratPlay/StratPlayerController.h` and
  `Source/StratPlay/StratPlayerController.cpp`. Nothing committed, nothing staged, no asset, no
  config, no test, nothing under `Tests/`. No suite figure is stated here; `global.md` owns that
  and owns any verdict.
  - **THE MEASUREMENT, WHICH IS THE ENTIRE BASIS FOR THE CHANGE AND IS PRESERVED HERE BECAUSE
    THE CODE THAT PRODUCED IT NO LONGER EXISTS.** A human playtest on the instrumented binary,
    on a seeded 99-hex board, over three and a half minutes. The complete set of `HOVER-DIAG`
    lines: the resolve path was entered four times, three of them forced from the console; the
    handler `OnHover` ran **exactly once**, at 17:48:59, seven seconds after the match seeded,
    on a focus transition, with the cursor off the board. Between two forced console reads at
    17:50:43 and 17:52:14 the human swept the cursor across the board for fifteen seconds and
    **the handler count did not move**. An independent earlier observation agrees: a planted
    hover cleared at the moment the human moved the cursor out of the window to type -- another
    focus transition, off-board, one event, one clear.
  - **THE CONTROL IS WHAT MAKES THAT AN ABSENCE RATHER THAN A SILENCE.** The forced call
    `ke StratPlayerController UpdateHoverFromCursor` printed, which proved in the same line
    that the log channel spoke AND that the editor had loaded the rebuilt DLL, while reading the
    handler's count out of the running game with no planted state. The two-counter shape was
    designed for exactly this and it did exactly this.
  - **WHAT THE MEASUREMENT PROVES, AND -- STATED FIRST, BECAUSE IT IS THE PART A LATER READER
    WILL OVER-CLAIM -- WHAT IT DOES NOT.** It proves that mouse movement produces no `Triggered`
    event for a mouse-axis action on this controller as configured. **IT DOES NOT PROVE WHY.**
    The standing hypothesis is that a visible, uncaptured cursor feeds no axis to `UPlayerInput`
    -- this class sets `bShowMouseCursor = true` and calls `SetInputMode` nowhere in
    `Source/StratPlay/` -- but that mechanism was never isolated and **this record does not
    assert it**. If the tick route works when a human next drives it, that is evidence the ROUTE
    was wrong; it is not proof of the engine mechanism behind it, and nothing downstream should
    cite it as one.
  - **THE ROUTE CHOSEN: `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` and
    discards the result.** Four lines of body. Every hover entry point still ends at the single
    `ApplyHoverChange` refresh decision, unchanged.
  - **WHAT WAS REJECTED, AND ON WHAT.** `SetInputMode` with capture, which is the more obvious
    repair. Rejected on two counts. (1) `bShowMouseCursor = true` is load-bearing -- a
    mouse-driven hex strategy whose player cannot see the cursor is not the game -- and every
    mode that reliably feeds mouse axis does so by capturing, which changes cursor behaviour at
    the viewport edge and how a click lands. A route that fixes the hover and makes selection
    feel different is worse than the bug. (2) It is unfalsifiable at our cost: no clause in this
    project reaches `UPlayerInput`, so an input-mode fix could only ever be re-confirmed by
    another human playtest, whereas a tick call is reachable by a clause. **This is not a ruling
    that input modes are wrong** -- if a later pass isolates the capture mechanism and finds a
    mode that feeds axis with a free visible cursor, it is a live alternative again, and it would
    still have to beat the testability.
  - **THE THREE CLAIMS THE TASK TOLD ME TO RE-DERIVE RATHER THAN ACCEPT, ALL RE-DERIVED AT
    SOURCE AND ALL HOLDING.** (a) The de-duplication is real: `FStratHoverState::SetHoveredHex`
    and `ClearHoveredHex` each compute and return `bChanged` (`StratHoverState.cpp`), so the
    per-frame cost is one trace and NOT a model rebuild. (b) `PrimaryActorTick.bCanEverTick =
    true` was already set in the constructor with the phase-6 measurement beside it -- the
    recorded `bCanEverTick = false` incident is closed in this tree and I did not re-open or
    re-derive it. (c) `HexUnderCursor` returns before tracing when there is no board, so a
    controller in an unseeded world pays a null check per frame and nothing else.
  - **WHAT WAS REMOVED, AND WHY REMOVAL RATHER THAN LEAVING IT UNBOUND.** The `HoverAction`
    `UPROPERTY`, the `OnHover` handler, the `ETriggerEvent::Triggered` binding in
    `SetupInputComponent`, its unset-property Warning, and the whole diagnostic -- both counters,
    both log sites, the powers-of-two throttle helper and both comment blocks. **NONE OF THE
    INSTRUMENT SURVIVES**; the debt the header declared is discharged in full, on the condition
    that header named. A wired-and-dead asset reference is worse than no reference: left in
    place, `HoverAction` would read to the next reader as the hover mechanism --
    `EditDefaultsOnly`, non-null on the shipping Blueprint, bound without a warning -- and send
    anyone debugging the hover to the trigger event instead of the tick.
  - **THE DIAGNOSIS IS RECONSTRUCTIBLE FROM THIS ENTRY ALONE, WHICH IS THE CONDITION THE TASK
    SET FOR DELETING ALL OF IT.** The verbatim timings, the counts, the control and the sweep
    are above. A summary of the measurement also sits on `Tick`'s definition in the `.cpp`, so a
    reader who reaches the code without the record still gets it.
  - **BUILD, GREEN, EDITOR CLOSED.** `Build.bat StratocracyEditor Win64 Development -waitmutex`
    -> `Result: Succeeded`, 15.38 s, 11 actions, zero warnings and zero errors. I confirmed the
    DLL lock was released with my own rename round-trip on
    `Binaries/Win64/UnrealEditor-StratPlay.dll` before building, rather than taking the
    editor-closed claim on trust.
  - **THE `Stratocracy.StratPlay` SUBSET RAN CLEAN AFTER THE CHANGE** -- zero failed, zero
    notRun, zero `succeededWithWarnings`. It is a regression result and NOTHING MORE. No count is
    written in this file; the live figure is `global.md`'s.
  - **[CORRECTED 2026-08-27, SAME DAY, ON A `strat-integration-reviewer` BLOCK. The sentence
    this bullet carried before it read: "That last figure is the one that matters here beyond
    regression: it is independent confirmation that the diagnostic `Warning` is gone from the
    tree." THAT WAS AN ABSENCE WITH NO CONTROL, and it is struck rather than deleted because the
    shape is worth keeping visible.** The conclusion is true -- the instrument is gone -- but the
    zero could not have witnessed it, and this same file refutes the reasoning twice over.
    (1) The design note further down states in terms that **no automation clause can reach
    `OnHover` at all**, so that log site could never have turned a clause
    `succeededWithWarnings` in ANY run, before or after. (2) The other log site,
    `UpdateHoverFromCursor`, was put at `Log` and not `Warning` **precisely so that the clauses
    which do call it would be unaffected** -- also stated in that note. Both halves of the
    instrument were designed to be invisible to this field. A field engineered not to move cannot
    testify that something moved. There was no pre-instrumentation-versus-post reading either:
    the instrument was never committed and no suite was run while it existed, so the nearest
    earlier report (`global.md`'s `05.17.45`) is not a "before" measurement of it but a run from
    a tree that never contained it.
    **WHAT ACTUALLY CONFIRMS THE REMOVAL, AND IT IS NOT THE SUITE.** Two things, both of them
    direct rather than inferential: a tree-wide grep --
    `grep -rn "HOVER-DIAG\|HoverHandlerCalls\|HoverResolveCalls\|OnHover(" Source/` returns
    **0 lines**, so neither counter, neither log site, the throttle helper nor the handler
    survives anywhere in `Source/`; and the build, which compiled the resulting tree with zero
    warnings. The subject grep was re-derived 2026-08-27 at `5e0333f` **with a control** --
    `grep -rn "UpdateHoverFromCursor\|SetHoveredHex" Source/` returns **36 lines** over the same
    tree, so the instrument is shown able to speak and the zero is a measured absence rather than
    a grep that could not match anything. The suite's role here is to show that removing it broke
    nothing else.
    **[A THIRD ITEM STOOD HERE AND WAS WITHDRAWN 2026-08-27, ON THE SECOND GATE OF THIS PASS.**
    It read: "the diff itself, `git diff --stat Source/StratPlay/` --> 160 insertions and 72
    deletions across the two files, which is where the removed lines are enumerable one by one."
    The figure was wrong when the gate re-derived it: the tree at `5e0333f` says **176
    insertions**, 72 deletions. It moved because of prose I wrote into this same header later in
    the same pass -- the identical defect to the stale header line numbers struck one bullet
    below, and a case of the standing rule against writing a count of a growing thing inside the
    thing that grows. **It is withdrawn rather than corrected, and the reason is not the rot.**
    Re-deriving 176 would fix the instance and leave the shape live: the next edit to either
    file, including one made to satisfy the next gate, falsifies the new number too. But the
    disqualifying fault is that **a diffstat was never evidence for this claim in the first
    place.** The claim is that a specific instrument is absent. A deletion count is a measure of
    total churn across the whole pass -- it cannot distinguish an instrument line from the
    surrounding refactor, and 72 deletions is equally consistent with the instrument surviving
    intact beside 72 unrelated removals. Absence is proved by absence. The grep above names the
    four tokens, returns zero, and carries a control; it is strictly stronger AND it does not
    rot, because no volume of later editing can make a token reappear that nothing writes. A
    figure that cannot survive its own document, and that does not bear on the proposition it
    was offered for, earns no place next to it.]
  - **WHAT IS NEWLY PINNABLE, FOR `strat-test-author`.** Before this change, "a mouse move
    produces a hover" was unreachable by any clause and the wave-0 hover file says so in terms.
    It is still unreachable -- nothing here reaches `UPlayerInput`. But the ROUTE is now
    reachable for the first time: `Tick` is virtual and callable on a spawned controller, and
    with no viewport `HexUnderCursor` returns false, so a tick CLEARS a hover planted through
    `SetHoveredHex`. That is a falsifiable pin on the tick-to-hover wiring --
    emptying the `Tick` body leaves the planted hover standing. Requested clause:
    `Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath`. **It should assert against the live
    spawned controller and hold no `FStratHoverState` of its own**, on the precedent the
    neighbouring hover clause already sets, and it should be proved by reverting what it pins.
  - **[CORRECTED 2026-08-27, SAME DAY, SAME BLOCK -- THE HANDOFF ABOVE SAID "`Tick` is public,
    virtual and callable on a spawned controller" AND `Tick` IS NOT PUBLIC HERE.** In
    `Source/StratPlay/StratPlayerController.h` the declaration
    `virtual void Tick(float DeltaSeconds) override;` sits between the file's `protected:` and
    `private:` labels -- cited by symbol and not by line on purpose, because the line numbers
    the gate quoted for it were already stale by the time this correction was written: the
    prose amendment in the same pass moved that declaration from 650 to 666 and the `private:`
    label from 691 to 707, in the very edit that answered the gate's third finding. So
    `Controller->Tick(dt)` through an `AStratPlayerController*` is an
    access error at compile time, and `strat-test-author` would have met the handoff as a build
    failure rather than as a clause. The word "public" is struck; "virtual and callable" is not,
    and the correction is written here rather than folded silently into the sentence above
    because the sentence above is what was handed over.
    **THE CLAUSE IS STILL WRITABLE, VIA THE BASE HANDLE, AND THAT IS THE FORK I TOOK.** C++
    access is checked on the STATIC type of the expression, while dispatch is virtual, so a
    `Controller` held as `APlayerController*` or `AActor*` -- where `Tick` is public in
    `AActor` -- calls this override legitimately and with no cast, no friendship and no change
    to this file. `SpawnActor<AStratPlayerController>` already returns something assignable to
    either. **THE OTHER FORK WAS TO MAKE `Tick` PUBLIC HERE, AND I REJECTED IT.** `BeginPlay`,
    `EndPlay`, `SetupInputComponent` and the rest of the framework overrides are all in that
    same `protected:` block, which is both this file's convention and Unreal's; widening one
    would be a change to shipping code made for a test's convenience, and it would leave the
    next reader unable to tell whether the access level meant anything. A pin should reach its
    subject through a handle the engine already hands out, not by relaxing the subject.]
  - **A HANDOFF INSIDE `Tests/`, WHICH I DID NOT EDIT.**
    `Source/StratPlay/Tests/StratPlayerControllerTick.cpp` pins the CDO's `bCanEverTick == true`
    and its prose gives the Enhanced Input trigger-evaluation reason for it. That clause now
    protects TWO mechanisms and names one: with this change, a `false` there would also stop the
    hover dead. The assertion is still correct and still green; only its rationale is now
    incomplete.

- **[HISTORY, 2026-08-27 -- SUPERSEDED BY THE ENTRY ABOVE, WHICH DISCHARGED THIS INSTRUMENT'S
  DECLARED DEBT AND DELETED EVERY LINE OF IT. The counters, log sites, token and throttle
  described below NO LONGER EXIST IN THE TREE. Kept because it records the design of the
  instrument that produced the measurement the fix rests on, and because the measurement itself
  is quoted in the entry above rather than here.]**
  **2026-08-27, `strat-gameplay-engineer` -- HOVER DIAGNOSTIC: `OnHover` IS NOW OBSERVABLE, AND
  THE INSTRUMENT CARRIES ITS OWN CONTROL. INSTRUMENTATION ONLY -- NO FIX, NO INPUT-ROUTE CHANGE.**
  In the integration tree `E:/MultiAgent/Stratocracy` on `master`, branched from `5e0333f`.
  Two files modified, both mine: `Source/StratPlay/StratPlayerController.h` (+44) and
  `Source/StratPlay/StratPlayerController.cpp` (+105), zero lines removed in either. Nothing
  committed, nothing staged, no asset, no config, no test. No suite was run and no suite figure
  is stated here; `global.md` owns that and any verdict.
  - **WHAT THE GAP WAS, AND IT IS AN INSTRUMENT GAP RATHER THAN A CODE ONE.** `content.md`'s
    playtest could not distinguish "`OnHover` never ran" from "`OnHover` ran and failed to
    resolve a hex". It reached the first only by planting a hover hex and watching it survive 28
    samples -- sound, but derived from a model field two frames downstream of the handler, and it
    needed a trick to get even that far. Nothing anywhere printed when the handler itself ran.
  - **THE SHAPE. TWO COUNTERS, TWO LOG SITES, ONE SCHEDULE.** `HoverHandlerCalls` counts entries
    to `OnHover`; `HoverResolveCalls` counts entries to `UpdateHoverFromCursor`. Both are plain
    `uint32` members, not `UPROPERTY`s, for `ProductionTargetHex`'s stated reason. **They are
    members and not function-local `static`s deliberately**: a `static` is process-wide and would
    carry a previous PIE session's count into the next one, making "never ran this session"
    indistinguishable from "ran last session". The controller is respawned per PIE session, so a
    member resets exactly when the question resets.
  - **THE SECOND SITE IS THE FIRST'S CONTROL, WHICH IS THE POINT OF HAVING TWO.** `OnHover` is
    private, unreflected and reachable only through Enhanced Input, so nobody can demonstrate its
    line is CAPABLE of printing -- and this record has been bitten before by an absence measured
    on an instrument never shown able to speak. `UpdateHoverFromCursor` IS console-reachable, and
    the playtest already drove it that way 30 times out of 30. So one
    `ke StratPlayerController UpdateHoverFromCursor` prints a line that both proves the channel
    works and reports `OnHover`'s count out of the running game. A forced call answering
    `OnHover has run 0 time(s)` after a minute of the cursor circling the board settles the
    hypothesis with no planted state at all.
  - **THE BRANCH IS VISIBLE AND NO SECOND CURSOR TRACE WAS ADDED.** `OnHover` reads the branch
    back out of `Hover` after the call -- latched hex means the cursor resolved, cleared means it
    took the off-board path. A struct read, not a re-trace: this file has ONE cursor-to-hex route
    and a diagnostic is not a reason to grow a second.
  - **THE VERBOSITY SPLIT IS OPPOSITE AT THE TWO SITES, AND EACH REASON FAILS TO APPLY AT THE
    OTHER.** `OnHover` logs at `Warning`, because `LogStratPlay`'s default runtime verbosity is
    `Log` and a `Verbose` line would print nothing until somebody typed `log LogStratPlay
    Verbose` -- which makes an absent line ambiguous between "the handler never ran" and "nobody
    raised the verbosity", the exact ambiguity this exists to remove. That is safe there because
    **no automation clause can reach `OnHover` at all**, so it cannot turn a clause
    `succeededWithWarnings`. `UpdateHoverFromCursor` logs at `Log`, because the wave-0 hover
    clauses DO call it directly and a `Warning` raised inside a clause is a record-visible change
    to the suite made by a diagnostic.
  - **THE TOKEN IS `HOVER-DIAG` AND DELIBERATELY NOT `STRAT-`.** Every `FOutputDevice` capture in
    `Source/StratPlay/Tests/` filters on `STRAT-CMD` or `STRAT-AI`, and several assert exact line
    counts over their window (`StratGuidanceInputGates.cpp`'s `Lines.Num(), 0` among them). A
    line carrying either prefix would enter those windows.
  - **AN OFF-BY-ONE THAT WOULD HAVE FAKED THE FAILING READING, CAUGHT BEFORE THE SECOND BUILD.**
    `UpdateHoverFromCursor` runs INSIDE `OnHover`, so its control line reports `OnHover`'s count
    from within the very call being counted. Incrementing after the call would have made a
    handler that had run exactly once report `OnHover has run 0 time(s)` -- the precise false
    zero this instrument exists to abolish, printed by the instrument itself. `OnHover` therefore
    increments FIRST and reports AFTER; the report has to be after, because the branch it names
    is read back from `Hover`.
  - **BOUNDED BY POWERS OF TWO, `(C & (C - 1)) == 0`, EVALUATED AFTER THE INCREMENT.** `OnHover`
    is bound `Triggered` on a 2D-axis action over mouse XY, so it fires at mouse-move rate. Call
    1 always prints, then 2, 4, 8, 16 ... -- about twenty lines for a whole session, 32 lines at
    `2^32` by construction. The alternatives were considered and rejected in the code comment: a
    first-call latch cannot separate "fired once" from "streaming"; a fixed every-Nth throttle is
    silent for the first N-1 calls and so reproduces the same false zero the playtest already
    hit; a time-based throttle makes the printed count depend on how fast the human moved.
  - **THE DEBT, WITH ITS DISCHARGE CONDITION, STATED IN THE HEADER AS WELL AS HERE.** This is
    instrumentation for one open question, not a feature. Both members, both log sites and both
    comment blocks come out once the hover route is proven working or replaced. If it is still
    here after that question closes it is dead weight in a hot path, and it should be deleted
    rather than downgraded.
  - **BUILD, GREEN, EDITOR CLOSED.** `Build.bat StratocracyEditor Win64 Development -waitmutex`
    -> `Result: Succeeded`, 15.31 s, 11 actions, zero warnings and zero errors printed;
    `[Adaptive Build] Excluded from StratPlay unity file: StratPlayerController.cpp` and both
    `UnrealEditor-StratPlay.lib` and `.dll` relinked.
  - **WHAT I DID NOT TOUCH, BECAUSE THE TASK SAID SO AND BECAUSE IT WOULD HAVE DESTROYED THE
    MEASUREMENT.** `bShowMouseCursor`, `SetupInputComponent`'s binding, the `IA_Hover` asset, the
    `IMC_Selection` row and the Blueprint default are all unchanged. Fixing the route in the same
    pass that instruments it would leave nobody able to say which change did it.
  - **MY READING OF THE HYPOTHESIS, OFFERED AS A READING AND NOT AS A FINDING.** The tree is
    consistent with it and contains nothing that contradicts it: `IA_Hover` is the project's only
    non-Boolean action and `Mouse2D` its only axis key, so the measured split -- seven key rows
    working, one axis row silent -- has exactly one structural difference behind it. **And the
    source of the `True` the playtest read off the live controller is IN THIS CLASS, which I had
    assumed it was not until I grepped:** `AStratPlayerController`'s constructor sets
    `bShowMouseCursor = true` with a comment saying a turn-based game is played with a cursor and
    that it is set in the constructor rather than `BeginPlay` so a Blueprint subclass can
    override it. No `SetInputMode` call exists anywhere in `Source/StratPlay/`. So if the
    hypothesis holds, the fix is a decision about input MODE rather than about the hover code:
    `FInputModeGameAndUI` with `SetHideCursorDuringCapture(false)`, or an explicit capture, or
    reading the cursor position on tick instead of via an axis action -- and the last of those
    would be a route change, which is why none of them is in this diff. The instrument above is
    what decides which question is even being answered.

- **2026-08-26, `strat-gameplay-engineer` -- WAVE 0: THE HOVER INPUT SURFACE. C++ ONLY, IN THE
  WORKTREE `E:/MultiAgent/Strat-wt/slot-1` ON `feat/hover-input`, BRANCHED FROM `69e75bb`.
  NOTHING IS COMMITTED, NOTHING IS STAGED, NO ASSET WAS TOUCHED AND NO TEST WAS WRITTEN.** No
  suite was run in this tree and no suite figure is stated here or moves on this work -- the live
  figure is `global.md`'s, which is the only file that may carry one. No phase verdict is stated;
  nothing here has been through a gate.
  - **WHAT THE GAP WAS.** The MVP-gap audit found §2.11.3's forecast card and §2.11.2's info
    panel both specified as hover-driven with NO HOVER INPUT ANYWHERE:
    at `69e75bb` `AStratPlayerController` bound no hover action of any kind -- dated here for
    the reason it is dated in `FStratHoverView`'s own block, an inconsistency the second W0 gate
    caught in this file after I had fixed it in the header. The hover STRINGS
    are shipped and are a different thing -- `FStratGuidanceView::EndTurnGateHover` and
    `LockedUnitHover` are tooltip TEXT for a widget that already knows what it is drawing, and
    neither answers "where is the cursor". Nothing in this wave reads or writes either.
  - **THE SHAPE.** A new plain struct `FStratHoverState` (`Source/StratPlay/StratHoverState.h`
    and its `.cpp`) holds the hex and the has-a-hex flag and produces `FStratHoverView`, a new
    `USTRUCT` on `FStratViewModel::Hover`. `AStratPlayerController` gains an
    `EditDefaultsOnly TObjectPtr<UInputAction> HoverAction`, an `OnHover` handler, three public
    entry points (`UpdateHoverFromCursor`, `SetHoveredHex`, `ClearHoveredHex`), a
    `BlueprintPure GetHoveredHex`, `GetHoverState`, and one private `ApplyHoverChange` that is
    the only place a hover decides to refresh. `DecorateForPresentation` calls
    `FStratHoverState::DecorateViewModel` beside the machine's and the guidance layer's.
  - **IT REUSES THE ONE CURSOR-TO-HEX ROUTE AND ADDS NO SECOND TRACE.**
    `UpdateHoverFromCursor` calls `AStratPlayerController::HexUnderCursor`, which is the same
    instance-index lookup `OnSelect` uses. No inverse of `WorldLocationOfHex` was written and
    none exists to call. **There is no hex arithmetic in any file this wave added or changed** --
    no distance, no neighbour, no addition of two coordinates; `FIntPoint::operator==` is the
    only spatial operator and it answers "did the hovered hex change", which is a question about
    two values the code was handed rather than about the board.
  - **NOT ONE `strat::` CALL AND NOT ONE `/Game/` LITERAL WAS ADDED.** This wave asks the rules
    module nothing: a hover is a statement about the cursor, and there is no rules question whose
    answer is which hex the mouse is over. `StratHoverState.cpp` does not include
    `StratBridge.h`. The Enhanced Input asset is the `EditDefaultsOnly` property and is authored
    in a separate editor batch.
  - **THE ONE `Triggered` BINDING IN THE FILE, AND THE REASON IS RECORDED AT THE BINDING.**
    Every other action binds `ETriggerEvent::Started` because each is a discrete decision.
    `Started` on a hover fires once when the mouse begins moving and not again, which would
    freeze the hovered hex at the first frame of a drag -- a symptom that reads as a card showing
    the wrong hex and sends the next reader to the forecast rather than to the trigger event.
  - **THE OBSERVABLE SEAM, STATED SO A CLAUSE CAN BE WRITTEN AGAINST IT.** `FStratHoverState` is
    drivable with no actor, no world, no PIE and no cursor: `SetHoveredHex` then
    `DecorateViewModel` over a default-constructed `FStratViewModel`, then read
    `Model.Hover.bHasHoveredHex` and `Model.Hover.HoveredHex`. That route reaches WHICH HEX and
    not merely THAT a hex arrived. The controller-level route
    (`AStratPlayerController::SetHoveredHex` -> `GetHoveredHex`) is the same path from the hex
    onward with only the trace removed, and needs a world but no viewport. **The split exists for
    exactly this reason and the header says so** -- a hovered hex whose only observable route was
    a live mouse over live Slate could not be pinned at all.
  - **STALE COUNTS AMENDED IN PLACE RATHER THAN LEFT -- AND THE FIRST DRAFT OF THIS BULLET WAS
    ITSELF A FALSE COMPLETENESS CLAIM, WHICH THE W0 GATE CAUGHT AND WHICH IS THE MORE USEFUL
    HALF OF THIS ENTRY.** [STAMPED 2026-08-27 on `strat-integration-reviewer`'s `VERDICT: BLOCK`,
    five findings, ALL FIVE IN COMMENT PROSE AND ALL FIVE MINE; the reviewer's own words on the
    behaviour were "the code itself is clean" and all twelve constraint checks passed. What the
    draft said: *"Four further sites said 'four' of a set that had grown twice. **Every one** is
    amended to name no number, each with an `[AMENDED, wave 0: ...]` note quoting what it said."*
    Both halves were false against the tree that sentence was written about. The census was
    wrong, and `StratPlayerController.cpp`'s input-handlers banner had been changed from "Four
    one-line calls." to "One line each." **silently** -- no quote, no stamp. A bullet about
    unstamped rewrites, containing an unstamped rewrite.]
  - **WHAT THE SUBJECT ACTUALLY WAS.** `StratPlayerController.h`'s header block stated the action
    set as both "THE FOUR ACTIONS" and "all five null" within one paragraph, and BOTH were
    already wrong before this wave -- `OpenProductionMenuAction` landed 2026-08-22 and neither
    count had moved. Beyond that: `SetupInputComponent` carried five counts of the same set
    inside ONE function ("three of the four assets", "ON ALL FOUR", "these four guards",
    "consistent with four bindings", "what these four lines state directly") while that same
    function now binds a hover action; `OnToggleProductionMenu`'s note called it "the only one of
    the five that does not go through `HandleSelectionEvent`" while wave 0's own added text a few
    lines above already named `OnHover` as a second; and the class doc had its count AND its
    tense rewritten with no quote.
  - **I DO NOT STATE A TOTAL HERE, AND THAT IS THE CORRECTION RATHER THAN AN OMISSION.** The
    previous draft typed one and was wrong by at least one within its own pass. **The census is
    the marker string, and THIS IS ITS ONE SPELLING IN THE PROJECT** -- the note in
    `AStratPlayerController`'s header block points here rather than restating it, because a
    second spelling is a second thing to go stale and, worse, a spelling *inside the searched
    tree* is a hit:

        grep -ro "\[AMENDED, wave 0" Source | wc -l

    **[STAMPED 2026-08-27 -- THE SECOND W0 GATE MEASURED THE PREVIOUS SPELLING AND IT WAS BROKEN
    TWICE OVER, WHICH IS WHY THE COMMAND ABOVE LOOKS OVER-BUILT.** It read
    `grep -c "AMENDED, wave 0" Source/StratPlay/`. Run verbatim it does not run at all:
    `grep: Source/StratPlay/: Is a directory`, printing `Source/StratPlay/:0` at **exit 2** --
    so a reader who trusts the output and not the exit code sees a census reporting **ZERO**
    notes, which is this record's own "exit code is not a verdict" shape. Add `-r` to make it
    run and it returns **16 against 15 real notes**, because the sixteenth hit was the header
    line quoting its own search token -- the recorded "a census that quotes its own search
    token" defect, and the whole of the gap between the 16 reported to the user and the 15 that
    existed. **Bracketing the token does not by itself fix this**, because the documenting line
    still contains the pattern; what fixes it is keeping the only spelling OUTSIDE the tree the
    command searches, which is why it lives in this file and not in a header.]**

    Why each part: `-r` because the argument is a directory; `-o | wc -l` rather than `-c`
    because `-c` counts matching LINES and would undercount two markers landing on one line;
    `Source` rather than `Source/StratPlay/` because notes now exist in `StratUI` too; and the
    marker stays wave-qualified because the tree already carries older `[AMENDED <date>:` notes
    from previous passes that must not be swept in -- measured, there are five of them.

    **PROVED, not asserted.** It returns **17**; demoting one real note to a non-marker returns
    **16**; restoring it returns **17** again, with `sha256sum` on the file before and after
    confirming the probe was byte-lossless.
  - **THE FIRST RUN OF THAT PROOF DAMAGED A SOURCE FILE, AND ONLY THE HASH GUARD CAUGHT IT.**
    The probe's mutation pattern had a fallback that SPANNED A LINE WRAP -- it matched
    `[AMENDED, wave 0, on the second W0 gate:` plus the newline, the `//` and the opening words
    of the quoted sentence -- so the "removal" ate `"Three entry points` and the "restore" put
    back only the short marker. **The census went 17 -> 16 -> 17 exactly as predicted while the
    file was silently broken**, because the marker count was restored and the sentence was not;
    the verdict line was clean and the tree was not. `sha256sum` before and after is the only
    reason it was found, and it is why the check is stated with the hash step rather than as
    three numbers. Repaired, re-verified, rebuilt. The re-run uses a SAME-LENGTH, SINGLE-LINE
    token swap that cannot span a wrap. This is the project's recorded "prove a restore on a
    disposable copy" lesson arriving the hard way: a probe is an edit, and an edit to prove a
    claim is still an edit. Every note quotes the sentence it replaced, so the
    number and the quotations are checkable against each other without trusting this file.
  - **THE COMPLETENESS CLAIM IS NOW DERIVED FROM `git`, MEASURED, AND FALSIFIABLE.** A throwaway
    script took every comment line at `69e75bb` carrying a count word, dropped those that survive
    verbatim, and checked that each remaining one is quoted inside some `[AMENDED, wave 0:` note
    -- subject set from `git show`, never a hand-kept list. Result: **COVERED**, and the claim
    is now true at LINE level rather than only at SITE level. **That distinction was a real
    caveat and it was closed rather than narrowed**: the reviewer found two fragments
    (`three actions rather than none` -> `those actions rather than none`, and
    `because the other three are statements` -> `because the others are`) whose blocks carried a
    stamped note but whose own words were nowhere quoted, so the site-level reading passed and
    the line-level reading did not. Both are now quoted verbatim in their block's note, which
    takes the base side to **0 uncovered** on the stricter reading.
    **It was proved able to fail, and the FIRST probe was mis-shaped** -- mutating a line wave 0
    had ADDED changed nothing, because the check's subject is lines that existed at `69e75bb`, so
    the probe routed around the guard rather than testing it. The correctly shaped probe strips
    the quote out of one note; the check then names `"Binds the four actions on the Enhanced
    Input component."` as uncovered. Both mutants were reverted and the revert proved
    byte-lossless by `sha256sum` on each file before and after. **Two instrument defects were
    found and fixed BEFORE the verdict was read**, both of which had produced false alarms: the
    note blob was flattened without stripping `//` and `*` prefixes, and the comparison was
    punctuation- and non-ASCII-sensitive, so `ON ALL FOUR.` never matched a quote ending
    `ON ALL FOUR"`. Three "findings" in the first run were pure encoding artifacts on lines
    `git diff` proves this wave never touched.
  - **WHICH ORDINALS WERE REMOVED AND WHICH WERE KEPT, on the coordinator's instruction to use
    judgement rather than delete every one.** REMOVED, because each is a CENSUS of a growing set
    and goes stale on the next property: "A SIXTH ACTION ... The five above", "exactly as the
    five above are", "the other five", "the sixth handler", "THE SIXTH BINDING", "THE FIFTH
    BINDING ... THE OTHER FOUR", "A FIFTH ACTION RATHER THAN A FIFTH MEANING", "Unlike the four
    above". KEPT, because it is the CONTRAST that is the actual point and it survives without a
    total: the hover binding is `ETriggerEvent::Triggered` where **the bindings above** are
    `Started`, and the production-menu binding is `Started` for a reason **the selection bindings
    above** do not have. Rewritten as "THE HOVER BINDING" and "THE PRODUCTION-MENU BINDING" --
    a binding named for what it does cannot go stale when a seventh arrives.
  - **HISTORICAL COUNTS SCOPED RATHER THAN DELETED.** Phase 6's measurement sentences ("all four
    `BindAction` calls proven to have run", "these four guards were silent on the null side") are
    a RECORD OF WHAT WAS MEASURED and deleting them would destroy evidence. They now read "EVERY
    `BindAction` call THAT EXISTED THEN" and "THE GUARDS THAT EXISTED THEN", each with a note
    saying the measurement is unchanged and only the implication that four is still the number is
    removed. Same treatment for "the four `IA_*` actions", which was accurate about phase 5 and
    which no reader could sort from the live counts around it.
  - **COUNTS I INTRODUCED IN THIS WAVE AND THEN HAD TO REMOVE. [STAMPED 2026-08-27: THIS BULLET
    SAID "THREE" AND SAID I HAD BROKEN THE POLICY "three times in the same pass". BOTH WERE
    FALSE -- the second W0 gate's R2 found a fourth, and a sweep that closed my check's blind
    spot then found a fifth and a sixth. A completeness claim about counts, carrying a count
    that was wrong: the F1 shape, third occurrence, and the reason no total is stated in this
    bullet either.]** What they were: `StratHoverState.cpp`'s block opened "FOUR SHORT
    FUNCTIONS"; `FStratHoverView`'s block said `AStratPlayerController` "bound five actions";
    both of my own `[AMENDED]` notes restated "the fifth"/"the sixth" while removing exactly
    those ordinals a clause earlier; `FStratHoverState::DecorateViewModel` claimed it was safe
    to call "in any order relative to the other two decorators" (**the gate's R2** -- true while
    `DecorateForPresentation` calls exactly two others, false the day W1 or W2 adds one); the
    hover section header in `StratPlayerController.cpp` said "Three entry points and one refresh
    decision, rather than three copies of the same two lines"; and `FStratHoverView`'s block
    said "The two hover STRINGS on `FStratGuidanceView`". Every one is gone, each replaced by
    the invariant the sentence was actually for -- the file's own functions are the census, the
    ordering claim does not depend on how many decorators there are, there is ONE refresh
    decision however many entry points call it, and the view-model block now DATES its claim
    (`at 69e75bb ... bound no hover action of any kind`) instead of counting. **Writing the
    policy did not stop me from breaking it repeatedly in the same pass**, which is the argument
    for the derived check below rather than for care.
  - **R2's REAL LESSON IS THE BLIND SPOT, NOT THE LINE, AND THE BLIND SPOT IS NOW CLOSED.** My
    first check's subject set was "comment lines present at `69e75bb`", so it was blind BY
    CONSTRUCTION to counts introduced on ADDED lines and in NEW FILES -- which is where R2 lived
    and why the check reported COVERED over it. That is the same defect as the first
    falsifiability probe, which mutated an added line and moved nothing: a subject set that
    excludes the wave's own new text will miss the next one identically. The rebuilt check
    sweeps BOTH sides -- 114 base count-lines and 45 added ones -- and it earned its keep
    immediately: it found the two survivors above that neither the gate nor the reviewer's own
    44-line sweep named. **The added side is a TRIAGE AID AND NOT A VERDICT**, because whether a
    count names a growing set or a fixed one is a judgement, not a match: it prints 13
    candidates that are all genuinely fixed pairs named in their own sentence ("the two unit
    bits", "addition of two coordinates", "TWO FIELDS AND NOTHING ELSE"), and a human adjudicates.
  - **ONE THING THE GATE DID NOT NAME AND I FIXED ANYWAY, plus one it did.** Not named: the three
    live census sentences in the `OpenProductionMenuAction` block, which my own second pass then
    rewrote WITHOUT quotes -- committing F1's shape a second time, inside the fix for F1. Caught
    by the derived check, now quoted. Named and fixed: the wrap artifact that left an orphaned
    "A / controller that" mid-sentence.
  - **DEBT TAKEN ON, WITH ITS DISCHARGE CONDITION, AND IT IS WRITTEN IN THE HEADERS TOO.**
    `FStratHoverView` carries a hex and a flag and nothing else -- no hovered unit id, no
    terrain, no forecast. The condition that discharges it is W1 and W2 arriving with their own
    structs and their own readers; a field landed ahead of its reader is a field written without
    its caller, and this record already describes what an unreachable-but-correct surface reads
    like.
  - **THE BUILD IS GREEN IN THIS WORKTREE**, with `-NoHotReloadFromIDE` because the editor is
    open on the integration tree and the Live Coding mutex is engine-keyed and machine-wide.
    `Result: Succeeded`, zero warnings. Built twice: once for the code
    (`StratHoverState.cpp` compiled, both module DLLs linked, 41.50 s) and once after the
    comment-only amendment pass, which still recompiles because two of the amended sites are
    inside function bodies -- `StratPlayerController.cpp` recompiled and
    `UnrealEditor-StratPlay.dll` relinked, 37.33 s. No suite was run in this tree; see the
    handoff about this slot's stale local automation report.

- **2026-08-25 (second pass that day), WRITTEN BY THE `coordinator`, NOT BY
  `strat-gameplay-engineer` -- declared, same as the entry below. THE COMPOSITION IS SPLIT OUT OF
  THE BRIDGE-TAKING BUILDER SO THAT A TAG NO TEST COULD REACH BECOMES ONE A TEST DRIVES.**
  Behaviour unchanged; this is a testability seam and is labelled as one in the header.
  - **WHAT MOVED.** `STRATUI_API void StratComposeMatchResultModel(const FStratMatchResultView&,
    const FStratScoreboardModel&, int32, FStratMatchResultModel&)` now holds every copy, every
    lookup and §2.8's key-to-criterion tag. `StratBuildMatchResultModel` is reduced to two bridge
    queries and one call: it asks `StratBuildMatchResult` for §2.8's result, asks
    `StratBuildScoreboardModel` for §2.11.4's rows into a LOCAL, and hands both to the composer.
  - **WHY IT IS A FUNCTION OVER VALUES.** The builder reaches its `FStratMatchResultView` THROUGH
    the bridge, and no bridge this suite can build reports `decidedByKey` 2 or 3 — they need a
    capped match in which both sides fought to an EQUAL combat Fame. `FStratMatchResultView` is a
    plain `USTRUCT` with a public defaulted `int32 DecidedByKey`, so a clause can hand-author one
    and drive the real composition. **That is the entire reason for the split** and the header
    says so, so nobody later "simplifies" it back.
  - **TOTAL AND INFALLIBLE.** `void`, no failure reason: every input is already a value somebody
    else refused or produced. It still assigns `OutModel` wholesale on its last line.
  - **ALL-OR-NOTHING SURVIVED AND IS MORE LEGIBLY SO — *NOT* STRONGER.** [Corrected 2026-08-25 on
    `strat-integration-reviewer`'s ruling 1; this bullet first said "STRONGER" and gave a mechanism
    that does not support it.] `Built.Scoreboard` **was already a local** in the pre-image, so a
    refusing build never touched `OutModel` there either and the guarantee is unchanged. What
    improved is legibility: the composer's signature makes the wholesale assignment structural
    instead of something a reader has to notice.
  - **ONE EXTRA COPY, ACCEPTED.** `FStratScoreboardModel` is copied into the model rather than
    built in place. Three rows, once per match, on a screen that appears at the end of one.
  - **BUILD.** `Result: Succeeded`, exit 0, twice — once for the extraction and once after the
    mutation control was reverted. Editor CLOSED throughout. No `Content/`, no
    `Source/StratRules/`, no `Data/`, no `.uproject`. No suite count and no verdict here;
    `global.md` owns both.

- **2026-08-25, WRITTEN BY THE `coordinator`, NOT BY `strat-gameplay-engineer`, AND THE DEPARTURE
  IS DECLARED RATHER THAN QUIET.** No subagent ran this pass. **A SEAM WAS ADDED SO THAT A
  CORRECT-BUT-UNTESTABLE MAPPING BECAME TESTABLE. Behaviour is unchanged -- deliberately, and
  that is the whole claim.**
  - **WHAT MOVED.** `StratBuildMatchResultModel`'s inline `switch` over
    `FStratMatchResultView::DecidedByKey` is now `STRATUI_API bool StratScoreCriterionForKey(int32
    Key, EStratScoreCriterion& OutCriterion)`, declared in `Source/StratUI/StratMatchResultWidget.h`
    beside `StratResultTierTextFor` and defined in the `.cpp` above the builder. The builder's
    seven-line switch became one line: `Built.bHasDecidedBy =
    StratScoreCriterionForKey(Result.DecidedByKey, Built.DecidedByCriterion);`.
  - **WHY IT IS A FUNCTION AND NOT A `Key - 1`.** T-UI-03's reason one layer down: the shift is
    arithmetic between two vocabularies that both already exist -- §2.8 numbers its keys 1/2/3 and
    `EStratScoreCriterion` numbers the same three criteria 0/1/2 -- and a cast off an int would
    turn every key §2.8 does not have into a real row of the scoreboard. The `switch` is total and
    every enumerator of the target names a row a display could accidentally mark.
  - **THE DISPOSITION OF `false` IS LOAD-BEARING AND IS UNCHANGED FROM THE INLINE SWITCH.**
    `OutCriterion` is written ONLY on true, so the caller's own default survives key 0 -- the
    common case, a flag kill having evaluated no key at all -- and `bHasDecidedBy` remains the
    thing that says whether to read the tag. A sentinel enumerator was not added, for the reason
    the header already gave.
  - **BUILD.** `Result: Succeeded`, exit 0, 13.18 s, green on the first attempt. Adaptive unity
    excluded `StratMatchResultWidget.cpp` and `StratMatchResultModelClauses.cpp` and rebuilt
    `UnrealEditor-StratUI.dll`. Editor CLOSED for the whole pass -- no `UnrealEditor` process --
    so no DLL write lock and no Live Coding mutex was in play. No `Content/`, no
    `Source/StratRules/`, no `Data/`, no `.uproject` change. No suite count and no verdict is
    stated here; `global.md` owns both.
  - **THE MAPPING WAS NEVER WRONG.** Key 1 -> `CombatFame`, key 2 -> `Objectives`, key 3 ->
    `SurvivingHp`, before and after, and the clause that now grades it derives that from
    `StratBuildScoreboardModel`'s own rows rather than from a table. What was wrong was that no
    test could reach two of the three arms. See `tests.md`.

- **2026-08-25, `strat-gameplay-engineer`: SEC 2.11.4'S END-OF-MATCH SCREEN HAS A C++ HALF,
  AND ITS THREE ROWS ARE THE LIVE SCOREBOARD'S OWN MODEL RATHER THAN A SECOND COPY OF SEC 2.8'S
  ORDER.** Built in worktree `E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen`,
  off `194de95`. Two new files in `Source/StratUI/`, plus wiring on `AStratScoreboardHUD` and one
  call site in `UStratMatchSubsystem`. No `Tests/`, no `Content/`, no `Source/StratRules/`, no
  `Data/`. No suite count and no verdict is stated here -- `global.md` owns both, and NO SUITE
  WAS RUN THIS PASS.
  - **THE GAP, MEASURED BEFORE IT WAS CLOSED.** `FStratMatchResultView` was projected by
    `StratBuildMatchResult`, routed by `UStratMatchSubsystem::GetMatchResult`, and READ BY
    NOTHING: Sec 2.8 had a winner and no surface could say who it was. That is the
    reflected-verb-with-no-caller shape again, one layer up -- the value was reachable and the
    SCREEN was not.
  - **THE LOAD-BEARING CALL: `FStratMatchResultModel` EMBEDS `FStratScoreboardModel` WHOLE.**
    Sec 2.11.4 asks for "the same three rows in the same order", and the two available shapes
    were (a) a private row array with its own labels and its own criteria-order walk, or (b) the
    live panel's own builder called a second time on the same bridge. (a) was killed by what
    Sec 2.11.4 exists for: revision Sec 1.5-#1 forbids the tiebreak being a hidden win condition,
    and a verdict screen spelling that order a SECOND way would reopen the hiding place one layer
    up. `StratBuildMatchResultModel` therefore calls `StratBuildScoreboardModel` and copies
    nothing of the rows itself. The chevron debt `StratScoreboardWidget.h` already records --
    delegate to the rules module when it exposes the comparison -- now covers this screen too,
    for free, which is the second reason (b) won.
  - **THE FACTION BINDING IS OURS AND IS LABELLED AS OURS.** Nothing in the GDD, `kb/setting.md`,
    the scenario JSON or the vendored rules joins a `strat` side index to a faction. `SideFaction`
    in `StratMatchResultWidget.h` pins side 0 -> Directorate, side 1 -> Vanguard on the user's
    2026-08-25 ruling, matching that file's Faction A / Faction B ordering, and its block says in
    as many words that it is a HARNESS PIN and not a rule -- `AStratScoreboardHUD::FirstSide`'s
    treatment of who-moves-first, applied to the same species of undecided thing. IF THE GDD EVER
    BINDS SIDES TO FACTIONS, THAT FUNCTION MUST DELEGATE AND NOT MERELY AGREE.
  - **THE VOICE IS THE WINNER'S AND THE COLUMNS ARE THE VIEWER'S, AND THE TWO READS ARE
    SEPARATED IN CODE.** `StratResultLineFor` takes `Winner` and never sees `ViewingSide`;
    `ViewingSide` reaches only `StratBuildScoreboardModel`'s YOU/ENEMY assignment. They agree in
    single-seat play and disagree on exactly the hot-seat turn where the loser is looking at the
    screen, which is the one case a Vanguard line under a Directorate camera would be produced by.
  - **THE SIX LINES ARE QUOTED, NOT COMPOSED, AND THE QUOTE WAS CHECKED MECHANICALLY.** A script
    extracted the six backticked samples from GDD lines 677-683 and the `TEXT(...)` literals of
    at least 20 characters from `StratMatchResultWidget.cpp`, and reported 6/6 exact matches with
    an EMPTY set of cpp strings not drawn from the GDD. Longest line 15 words against
    `kb/setting.md`'s <= 30 budget; zero hits for any of its nine banned-register words. The
    header's own "longest of the six is 15 words" sentence is that measurement and not an estimate.
  - **THE TRIGGER IS AN EDGE THAT ALREADY EXISTED.** `ConcludeMatchIfEnded` latches on
    `bMatchConclusionAnnounced` and is the only place in the class a finished match is crossed
    exactly once; `ShowMatchResult` hangs inside that latch, AFTER the `STRAT-MATCH concluded`
    log line so a gate reading the transition never depends on an asset existing. TWO THINGS
    WERE NEEDED FOR A RESTART AND NEITHER IMPLIES THE OTHER: `StartMatchInternal` already cleared
    the bool, which removes nothing from the viewport, so it now also calls
    `AStratScoreboardHUD::HideMatchResult`. The widget belongs to the HUD and outlives
    `TearDownPresentation`, so without that call the last match's verdict would sit on top of the
    new match's board.
  - **ONE DIVERGENCE FROM THE PRODUCTION-MENU PRECEDENT, PAID FOR ON PURPOSE.**
    `ProductionMenuWidgetClass` is a bare `TSubclassOf<UUserWidget>` to hold the
    `/Script/Module.Class` bake open. `MatchResultWidgetClass` is typed as
    `TSubclassOf<UStratMatchResultWidget>` and pays that bake now, because this class MUST be
    called -- `ShowMatchResult` pushes a finished model in, and a WBP free to compose its own
    verdict would be a second author of Sec 2.8's result. Recorded in both headers.
  - **BUILD.** `Build.bat StratocracyEditor Win64 Development -waitmutex -NoHotReloadFromIDE
    -MaxParallelActions=10` against the worktree's own `.uproject`; `Result: Succeeded`, process
    exit 0, first attempt. The only warnings in the log are the two pre-existing C4456/C4457 in
    vendored `Source/StratRules/Driver.good.cpp`; zero from the new or edited files.
  - **WHAT IS NOT BUILT AND WHO OWNS IT.** `WBP_MatchResult` does not exist -- no `Content/` was
    touched, which is `strat-editor-builder`'s lane, and until that asset exists and is set on the
    HUD Blueprint's `MatchResultWidgetClass`, `ConcludeMatchIfEnded` logs
    `No end-of-match screen this match: no MatchResultWidgetClass is set ...` at Warning and the
    match still concludes correctly. The screen is therefore ROUTED but not yet REACHABLE by a
    player, and that is the honest state of it.
  - **NO COPY WAS INVENTED FOR `DecidedByKey`.** The model carries the key as a raw number AND as
    an `EStratScoreCriterion` tag so the WBP can mark the deciding row with no arithmetic, and no
    sentence naming the criterion, because the GDD has not written one --
    `FStratMatchResultView::DecidedByKey`'s own refusal to name the keys, honoured rather than
    worked around.

- **2026-08-25, `strat-gameplay-engineer`: §2.11.5'S BOXED-IN FOOTER HAS A BLUEPRINT ROUTE, AND
  THE ROUTE WAS CHOSEN BY MEASURING TWO CLOCKS RATHER THAN BY PICKING A SIGNATURE.** Three new
  symbols across `StratBridge` and `StratPlay`, plus two prose corrections my own insertion made
  necessary. No `Tests/`, no `Content/`, no `Source/StratRules/`, no `Data/`. No suite count and
  no verdict is stated here -- `global.md` owns both, and NO SUITE WAS RUN THIS PASS.
  - **THE GAP, MEASURED BEFORE IT WAS CLOSED.** `grep -rn "bSpawnBlocked" Source/` outside
    `Tests/` returned exactly TWO sites: the declaration in `StratViewModel.h` and the single
    assignment in `StratViewModel.cpp`. Nothing read it. The field sits on `FStratFactoryView`,
    reachable only through `FStratViewModel::Factories`, and `UStratMatchSubsystem::GetViewModel`
    is deliberately not a `UFUNCTION`. So a correct, gated, T-UI-05-pinned field had no player
    behind it -- the same shape as `SkipGuidance` having no caller, and the check that finds it
    is the ROUTE and not the mechanism.
  - **THE TWO CLOCKS ARE REAL AND THEY DIVERGE ON THE ONE SEQUENCE THIS FOOTER IS ABOUT.**
    `RefreshProductionMenu` fills `ProductionMenu` from a LIVE bridge query and writes nothing
    else; `Factories` lives on `AppliedModel`, which `ApplyView` alone writes and whose own
    comment calls it "a record, never an input". They are independent by construction. Three
    findings, re-derived in this tree rather than reasoned from the shape:
    - **REACHABLE DIVERGENCE.** In `SubmitProductionChoice` an accepted build rebuilds the MENU
      first and the SCREEN second, and a failed `RefreshPresentation` returns with the rows
      fresh and `AppliedModel` untouched -- a case that function's own block already states
      ("A `false` RETURN AFTER AN ACCEPTED COMMAND IS POSSIBLE"). `RunAiTurnsNow` has the same
      shape. **THE BUILD THAT FILLS THE FACTORY'S LAST FREE HEX IS EXACTLY THE BUILD THAT FLIPS
      THIS BIT**, so the divergence is not on an exotic path, it is on the subject's own path.
    - **`AppliedModel` IS "WHAT WAS DRAWN" AND NOT "WHAT THE RULES SAY".** `ApplyView` is public
      and takes the model as its argument on purpose; a caller may apply a hand-built one, and
      `Source/StratPlay/Tests/` does. A footer sourced there would be a function of the last
      argument somebody passed.
    - **AND IT CANNOT ANSWER AT ALL ON A STATE THE MENU REACHES.** A refresh aimed at a hex that
      is not a build point SUCCEEDS -- `IsProductionMenuOpen`'s block says so -- and
      `Snapshot.factories` holds only factory objectives, so `AppliedModel.Factories` has no
      entry for `ProductionMenuHex` in that state. This one is decisive independently of the
      clocks and would have held even if they could not diverge.
  - **THE SHAPE THAT LANDED IS NEITHER OF THE TWO THE BRIEF OFFERED, AND THAT IS THE
    CONSEQUENTIAL CALL.** Not "read the applied model" (wrong clock) and not "re-query on every
    read" (right answer, wrong cost -- a `BlueprintPure` bound to a footer's visibility runs per
    frame). It is **re-query ON THE ROWS' OWN CLOCK and publish the answer beside them**:
    `RefreshProductionMenu` asks `FStratBridge::FactorySpawnBlockedAt` in the same call, from the
    same bridge, and assigns the result in the same statement group that assigns the rows and the
    hex; `CloseProductionMenu` clears all three together. **THE FOOTER AND THE ROWS ARE THEREFORE
    FRESH TOGETHER OR STALE TOGETHER AND CANNOT DISAGREE** -- structurally, not by discipline --
    which is the property the brief asked for, and the snapshot build is paid once per menu
    refresh instead of once per frame.
  - **IT IS NOT THE `bSeeded`-SHAPED MIRROR THIS CLASS REFUSES, AND THE TEST IS THE ONE THAT RULE
    STATES.** A mirror is refused when it CAN disagree with what it mirrors. There is no other
    source for this value in the subsystem, and the three members move as one -- which is
    `ProductionMenuHex`'s existing relationship to the rows, not `bSeeded`'s to the bridge. The
    member is deliberately **not** a `UPROPERTY` and not public: `IsOpenMenuFactorySpawnBlocked`
    ANDs `IsProductionMenuOpen()` in, so publishing the raw bool beside it would be two Blueprint
    surfaces for one fact and only one of them correct.
  - **`FStratBridge::FactorySpawnBlockedAt` PROJECTS A WHOLE SNAPSHOT TO READ ONE BOOL, AND THAT
    IS FORCED RATHER THAN CHOSEN.** `strat::spawnHexesBlocked` is declared in `Ui.h` and would
    answer the occupancy half in a bounded handful of lookups -- but it answers it about ANY hex,
    so alone it would report "boxed in" for a hex that is not a build point. The predicate that
    decides build-point-ness, `isFactoryObjective`, is in an **anonymous namespace** in
    `Ui.good.cpp` and is declared in no header (measured: zero hits over `Source/StratRules/*.h`),
    so `buildUiSnapshot`'s `factories` vector is the only sanctioned way to learn which hexes are
    factories. Re-deriving `capturable && isSpawnPoint` in the bridge was the other shape and was
    killed on one argument: it puts a second author on a rules predicate.
  - **"NOT A BUILD POINT" RIDES THE REFUSAL CHANNEL HERE WHERE IT RIDES THE ANSWER CHANNEL IN
    `BuildOptions`, AND THE ASYMMETRY IS THE POINT.** A buildlist for a non-factory hex is a real
    answer -- one row per §2.4 row, each `available` false with the module's own reason, which
    is what §2.11.5 draws. "Is this factory boxed in" has NO answer for a hex that is not a
    factory: `false` reads as "there is room", `true` as "it is full", and both are sentences
    about something that does not exist. The accessor therefore answers **false** there, and
    false when no menu is open, and both are stated as decisions rather than as fallbacks.
  - **THE FOOTER SWAPS AND THE BUILD BUTTONS STAY ENABLED, WHICH IS Q31 AND NOT THIS LANE'S
    TASTE.** Ruled 2026-08-22, restated at the field in `Ui.h`: a player MAY queue into a
    boxed-in factory, `buildWaiting` is the mechanism, and `uiBuildOptions` must not fold
    `spawnBlocked` into availability. The accessor's block says outright that ANDing this into a
    button's `bIsEnabled` would re-decide widget-side a question the rules module declined to
    fold in.
  - **A REFUSAL FROM THE NEW QUERY IS NOT LOGGED, AND THAT IS A DECISION.** Two refusals are
    reachable past a successful menu build: "not a build point", which is an ordinary keypress
    over a non-factory hex and would make a `Warning` per press out of a surface already
    explaining itself in the module's words; and the corrupt-`defIndex` case `MakeUiSnapshot`
    guards, which is a real fault and is already loud in a better place, because `BuildViewModel`
    goes through the same projection and the board has stopped rebuilding. Both fall back to
    `false`, the direction that claims nothing. Written down here because a swallowed refusal
    that is never declared is indistinguishable from one nobody thought about.
  - **TWO PROSE CORRECTIONS MY OWN INSERTION MADE NECESSARY, BOTH FOUND BEFORE THE FINAL BUILD.**
    - **A CARDINAL OF MINE THAT WAS A BOUND STATED AS AN EXACT COUNT.** "would answer the
      occupancy half in seven lookups" -- `spawnHexesBlocked` early-returns on the first free
      hex, so seven is a MAXIMUM. Reworded to a bound over a set the rules module defines
      (`HEX_DIRECTIONS`), which is the distinction this record already draws between a cardinal
      over a defined set and one over an undefined one.
    - **AND I LANDED A METHOD INSIDE A SECTION THAT THREE DATED READINGS RANGE OVER.**
      `StratBridge.h`'s façade banner recites SIX / SEVEN / SEVENTEEN as the evidence that the
      SCOPE, not the number, was the defect. `FactorySpawnBlockedAt` went into
      `// ---- Rules queries`, so the SIX reading now yields seven and the SEVEN reading eight;
      the SEVENTEEN reading did NOT move, because my signature names no `strat::` type.
      **STAMPED, NOT REFRESHED** -- refreshing them would turn a dated derivation back into the
      standing census that block exists to refuse, and the argument they support is unharmed by
      the tree moving. That the three readings moved by different amounts is itself the cleanest
      demonstration available that they were never counting one set.
  - **`BlueprintPure` WAS VERIFIED OFF THE GENERATED FLAGS AND NOT OFF THE SPECIFIER I TYPED.**
    `Intermediate/.../StratMatchSubsystem.gen.cpp` emits `IsOpenMenuFactorySpawnBlocked` with
    `EFunctionFlags 0x54020401`, which carries `FUNC_BlueprintPure` (`0x10000000`),
    `FUNC_BlueprintCallable` (`0x04000000`), `FUNC_Const` (`0x00020000`) and `FUNC_Native`. The
    exec thunk and the `NativeFunctionLookup` entry are both present, so the verb exists on the
    reflected class and not merely in a header UHT might have skipped.
  - **CRLF WAS MEASURED PER FILE AND PRESERVED, WITH A NUMSTAT CONTROL.** All four edited sources
    are CRLF; edits went in through a byte-level insert with a unique-anchor assertion, never
    `sed`. CR deltas equal added-line counts on every file (`StratBridge.h` 1092 -> 1165 with
    `git diff --numstat` reading `75 2`; `StratBridge.cpp` 1577 -> 1619 / `42 0`;
    `StratMatchSubsystem.h` 1574 -> 1670 / `96 0`; `StratMatchSubsystem.cpp` 1800 -> 1842 /
    `44 2`), so no file was rewritten whole. `grep` cannot see a CR on this box; counted with
    `tr -cd '\r' | wc -c`.
  - **BUILD: GREEN, MEASURED AFTER THE LAST EDIT.** `Build.bat StratocracyEditor Win64
    Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex` from the
    Bash tool in the integration tree -> `Result: Succeeded`, `REAL_EXIT=0`, 51 actions,
    235.13 s. `Module.StratPlay.gen.cpp` and `Module.StratBridge.gen.cpp` both compiled, so UHT
    parsed the changed headers. The editor was closed and re-verified absent in the same command
    (`tasklist | grep -ci "UnrealEditor.exe"` -> 0), with `explorer.exe` as the control that the
    instrument can speak. NO `-NoHotReloadFromIDE` was needed. THIS IS THE FINAL BUILD; an
    earlier run in this pass was 58 actions / 263.01 s and is superseded rather than deleted,
    because it is the run that first proved the new `UCLASS` surface parses.
  - **DEBT: NOTHING BINDS IT YET, AND NOTHING HEADLESS FAILS IF I DELETE IT TOMORROW.** The
    accessor exists, is reflected, and has no caller -- which is precisely the shape this record
    was corrected for on `SkipGuidance`, so it is declared rather than left to be found.
    **DISCHARGED WHEN** `WBP_ProductionMenu`'s footer binds to it (`strat-editor-builder`) and a
    clause pins it (`strat-test-author`). **OWNED: coordinator, to sequence.**
  - **DEBT: `FStratFactoryView::bBuildWaiting` AND `bHasBuiltThisTurn` STILL HAVE NO BLUEPRINT
    ROUTE, AND THIS PASS DELIBERATELY DID NOT GIVE THEM ONE.** §2.11.5's boxed-in footer is one
    sentence about one field; `bBuildWaiting` is a different sentence ("a build is queued here")
    and `Ui.h` names the case that separates them. Adding all three now would have been three
    accessors written for one measured requirement. **DISCHARGED WHEN** a GDD section names the
    surface that draws them.

- **2026-08-25, `strat-gameplay-engineer`: THE WINNER NOW REACHES ENGINE CODE, THE PRODUCTION
  MENU CAN NAME A SHORTFALL, AND THREE DEFERRALS WERE RETIRED BECAUSE THEIR PREMISES WERE
  FALSE.** Six changes across `StratBridge`, `StratUI` and `StratPlay`; one of them is a defect
  fix and the rest are new surface plus stamped retractions. No suite count and no verdict is
  stated here -- `global.md` owns both.
  - **`FStratBridge::MatchResult` ROUTES THE FOURTH `ui*` QUERY.** Upstream `96d93ea` added
    `strat::UiMatchResult` / `strat::uiMatchResult` to the vendored `Ui.h` as a QUERY beside
    `uiReachable`, `uiForecast` and `uiBuildOptions` -- not as a `UiSnapshot` field. So
    `kUiSnapshotFieldCount`, `kUiMirrorFieldCount`, `kUiDerivedFieldCount`, the transcribed
    `uiFieldContract()` table and `uiEnumerateSnapshot` are all UNMOVED, and no T-UI-05 consumer
    carries anything new. `MakeUiSnapshot` is untouched. The bridge method takes an out-parameter
    in `BuildOptions`'s shape, and it has ONE channel rather than two, because `uiMatchResult`
    holds no legality the rules could decline.
  - **IT REFUSES AN UNSEEDED BRIDGE WHERE UPSTREAM ANSWERS ONE, AND THAT IS THE ONLY DIVERGENCE.**
    `Ui.h` rules that a world with no `turn` reports InProgress / SIDE_NONE, because a missing
    input and an unfinished match are the same thing to a caller that cannot tell them apart.
    `FStratBridge` HAS `IsSeeded()` and can, so it refuses -- a result screen handed "no winner
    yet" by a bridge that never loaded a match would wait forever on a match that does not exist.
  - **`FStratMatchResultView` AND `EStratResultCause` IN `StratViewModel.h`.** The cause enum is
    ordinal-pinned to `strat::ResultCause` and mapped by an exhaustive switch (`ResultCauseOf`)
    beside `ResultTierOf`, with no `default:` label, so a cause added upstream is a compile
    failure in a file anybody may fix. `Winner` is `INDEX_NONE` on a draw and while in progress,
    and a fourth `static_assert` -- `strat::SIDE_NONE == INDEX_NONE` -- makes that a copy rather
    than a re-encoding. `StratBuildMatchResult` is the projection, and
    `UStratMatchSubsystem::GetMatchResult` is the reader beside `GetConcludedMatchView`.
  - **NO PARITY CLAUSE IS OWED FOR IT, AND THE ABSENCE IS THE POINT.** `StratViewModelParity.cpp`
    walks `FStratViewModel` against a snapshot the same bridge projected; this struct is outside
    that walk because it mirrors nothing IN the snapshot. What has to be pinned instead is the
    ROUTING -- the bridge asked and this asked, on one bridge in one frame.
  - **`FStratBuildOptionView::Shortfall` IS THE FIRST DERIVED NUMBER AT THAT BOUNDARY, AND THE
    PLAN'S PREMISE FOR IT WAS WRONG.** The plan said "the same boundary already computes derived
    values". It does not: `StratViewModel.h`'s header block claimed "NO ARITHMETIC, ANYWHERE" and
    `StratBuildViewModel` contains no `+`, `-`, `/` or `.size()` over a snapshot vector. That
    claim is now NARROWED IN PLACE in three headers rather than quietly falsified -- it still
    holds for `FStratViewModel` and everything reachable from it, and there is exactly one
    exception, in a struct that is not part of the model. `{S}2.11.5` requires `need N` on an
    unaffordable row and T-UI-03 forbids the widget subtracting, so it lands here.
    `UiBuildOption::affordable` remains the sole authority on WHETHER a row can be bought;
    `Shortfall` is forced to 0 whenever it is true and clamped at 0 otherwise, so the two cannot
    disagree about the greying even if they disagreed about the number. **DISCHARGED WHEN**
    upstream adds `shortfallFame` to `UiBuildOption`, at which point the field becomes an
    ordinary mirror and the subtraction is DELETED rather than moved.
  - **`UStratMatchSubsystem::SetViewingSide` HAD A REAL DESYNC AND IT IS FIXED BY THE ORDER OF THE
    LINES, NOT BY A COMMENT.** `AStratScoreboardHUD::SetViewingSide` range-checks BEFORE
    assigning; the subsystem assigned FIRST and then forwarded, returning `false` on the HUD's
    refusal without reaching `RefreshPresentation`. So a refused out-of-range hand-over left the
    subsystem holding a side the HUD had rejected, and every later refresh failed inside
    `StratBuildViewModel` naming the builder rather than the hand-over. Every refusal now sits
    ABOVE the assignment and the rebuild sits BELOW it, which is what distinguishes the two
    failure modes structurally. The deliberate no-rollback ON A FAILED REBUILD is unchanged and
    its reason is unchanged.
  - **THE NULL-HUD PATH WAS UNCHECKED AND IS NOW DECIDED EXPLICITLY.** With no HUD, nothing
    range-checked the side at all -- a HUD-less subsystem is a legitimate configuration and it
    used to adopt any `int32`. It now runs a TRIAL BUILD at the candidate side through
    `StratBuildViewModel` -- the same authority `RefreshPresentation` reaches, asked for the
    PROPOSED side rather than the held one -- and discards the model. No new constant and no
    third range check: the two existing authorities are still the only ones, and only WHEN they
    are consulted moved. With no HUD **and** no bridge there is no authority in existence to ask,
    so the assignment stands and `RefreshPresentation` reports "there is no bridge", which is a
    rebuild failure and not a refused side; that fall-through is stated in the code.
  - **THREE PROSE CLAIMS OF MY OWN WENT FALSE IN THE SAME PASS THAT WROTE THEM, FOUND BY THE
    WAVE 3 REVIEWER GATE AND CORRECTED IN PLACE 2026-08-25.** All three are the same class as
    the "NO ARITHMETIC, ANYWHERE" claim above -- a COUNT or an unqualified QUANTIFIER that the
    change itself invalidated -- which is the pattern worth carrying forward, not the three
    instances.
    - **A CENSUS THAT COUNTED THE WRONG THING.** `StratViewModel.cpp`'s header block said
      "The static_asserts -- FOUR since 2026-08-25". The file has THREE `static_assert(`
      statements (HEAD had two; I added `strat::SIDE_NONE == INDEX_NONE`). The block then
      listed four SUBJECTS, and the fourth -- the `ResultTier` / `ResultCause` enumerators --
      is real but is pinned by a DIFFERENT MECHANISM: an exhaustive switch with no `default:`
      label. So the sentence conflated a count of asserts with a count of pinned subjects. It
      now states three asserts and two switches, names each, and says why an enum's
      MEMBERSHIP cannot be `static_assert`ed the way a constant's VALUE can.
      `StratViewModel.h`'s narrowing paragraph disagreed with the .cpp about the same objects
      and now defers to the .cpp block as the authority.
    - **AND THE CORRECTION REINTRODUCED THE DEFECT ONE LEVEL UP -- A DIFFERENT MECHANISM,
      WORTH ITS OWN LINE.** The fixed sentence read "the number is stated because this list
      is the census and a reader will count `static_assert(` to check it". It named the exact
      token it told the reader to search for, so the sentence became a member of its own
      subject: `grep -c "static_assert(" Source/StratUI/StratViewModel.cpp` returned **4**
      against a stated THREE -- three statements plus the census sentence. Found by the Wave 3
      reviewer, measured before acting on it.
      **THE GENERAL SHAPE, AND IT IS NOT THE SAME LESSON AS THE BULLET ABOVE: A CENSUS THAT
      QUOTES ITS OWN SEARCH TOKEN JOINS THE SET IT IS COUNTING.** The bullet above is a count
      invalidated by the change that wrote it; this one is a count invalidated by the sentence
      that states it, and no amount of re-counting the subjects would have caught it. The
      repo's neighbouring instance is prefix nesting -- `BP_` inside `WBP_` inflating a census
      by 19 -- which is the same failure with two tokens instead of one.
      **THE FIX IS AN ANCHOR THAT PROSE STRUCTURALLY CANNOT SATISFY**, not a reworded
      sentence. The block now prescribes `grep -c '^[[:space:]]static_assert'`, which a
      comment line cannot match because a comment line begins with `/`. Two figures in that
      block are stated and both were measured on the file as it now stands: the anchored form
      returns **3** (equal to the stated count) and an unanchored search for the bare token
      returns **8**, because the prose names the token five more times -- which is exactly why
      the unanchored form is not a census at all.
      **THE ONLY CHECK THAT WOULD HAVE CAUGHT EITHER OF THESE IS RUNNING THE COUNT THE
      SENTENCE ITSELF PRESCRIBES, ON THE FILE AS EDITED, BEFORE REPORTING.** Reading the
      sentence and re-counting the subjects passed twice. That is the practice this entry is
      really recording.
    - **AND THE ANCHOR WAS ONE CHARACTER SHORT OF IMMUNE -- A THIRD MECHANISM, FOUND BY THE
      RE-GATE AND MEASURED BEFORE ACTING.** The prescribed check was
      `grep -c '^[[:space:]]static_assert'`, which matches EXACTLY ONE whitespace character.
      It returned 3, but only because all three statements happen to sit at namespace scope
      at one tab: one added inside a function body, a nested namespace or a class would sit
      deeper and would NOT have matched, and the census would have UNDER-reported silently.
      The check is now `grep -c '^[[:space:]]*static_assert'` -- still unable to match a
      comment line, because `/` is not whitespace, and now indifferent to depth. Verified on
      the file as edited: **returns 3**, equal to the stated count, matching the same three
      statements. Build after the change: `Result: Succeeded`, exit 0.
      **THE PATTERN ACROSS ALL THREE IS THE POINT.** (a) counted the wrong subject, (b)
      quoted its own search token, (c) over-constrained the anchor. Each fix was sound and
      each introduced the next, which is why "a census is correct" is not a property a
      single reading establishes -- only running the prescribed check on the edited file
      does, and that is now the third time it was the only thing that would have.

    - **HELD, NOT FIXED: `StratBridge.h`'s FACADE BANNER, BECAUSE MY COUNT DISAGREES WITH THE
      REVIEWER'S AND A CONFIDENTLY WRONG NUMBER IS WORSE THAN AN UNCHECKABLE ONE.** The
      re-gate asked me to replace the banner's cardinals with NAMED methods -- the right
      instruction -- but supplied a figure I cannot reproduce, and the brief's own rule is to
      settle it before editing. Re-derived against the header, not taken from the relay:
      - **AGREED, and it confirms the sentence is wrong today.** The facade section declares
        **SEVEN** methods -- `Turn`, `SideToMove`, `ReachableHexes`, `AttackTargetHexes`,
        `SubmitMoveToHex`, `SubmitAttackAtHex`, `SubmitBuildAtHex` -- of which **FIVE**
        forward to a typed method beside them. `Turn()` and `SideToMove()` forward to
        nothing: both read `GameState.turn` directly in the .cpp. So the banner's inherited
        "The six methods below" and its "each one forwards to the typed method beside it"
        cannot both describe the same set. **THAT AMBIGUITY IS PRE-EXISTING AND NOT MINE**;
        I preserved it when I corrected the other half of the sentence.
      - **DISPUTED.** The relay says the methods above the banner naming a `strat::` type
        number SIX. I count **SEVEN** in the two sections immediately above it (`// ---- View
        model` and `// ---- Rules queries`): `MakeUiWorld`, `MakeUiSnapshot`,
        `CheckSnapshotFidelity`, `Reachable`, `Forecast`, `BuildOptions`, `MatchResult`.
        `MakeUiWorld` returns `strat::UiWorld`, sits above the banner, and is public -- the
        class has one `public:` and one `private:` and every declaration in that band is on
        the public side. And under the OTHER available reading of "above the banner" -- the
        whole class -- the answer is neither six nor seven but **17**, because `Submit`,
        `SubmitMove`, `SubmitAttack`, `SubmitBuild`, `ReplayLog`, `RecordedLog`, `State`,
        `UnitDefs`, `Tables` and `ScenarioData` all name a `strat::` type too.
      **SO THE REAL DEFECT IS NOT THE CARDINAL, IT IS THAT "ABOVE" HAS NO DEFINED SCOPE** --
      which is exactly why two careful readers got two different numbers, and why guessing a
      band in order to name its members would bake my guess into the header. **WHAT IS OWED:**
      a ruling on which set the sentence means, then a rewrite that NAMES those methods and
      states no cardinal at all. The substantive half stands either way and is untouched:
      `MatchResult` needs no `int32`/`FIntPoint` mirror because its consumer is `StratUI`,
      which MAY name a `strat::` type, and `StratBuildMatchResult` hands `StratPlay` a struct
      naming nothing vendored. **OWNED: coordinator, to settle the scope.**
      - **SETTLED AND LANDED 2026-08-25. The coordinator re-derived rather than adjudicating
        between two reports, confirmed SEVEN, and cited the header's own prose -- "PRIVATE,
        WHERE `MakeUiWorld` IS PUBLIC" -- which settles the public/private half from inside
        the file.** THE RULING WENT PAST THE NUMBER: the sentence stops depending on "above"
        at all, because picking a band would have left the next reader re-deriving that
        choice from a sentence that still did not state it. It now describes THIS SECTION and
        its forwarding relation -- well-defined without a scope convention and checkable by a
        reader standing in one place -- and NAMES `Turn()` and `SideToMove()` as the two that
        mirror nothing, with no cardinal for either section.
      - **THE CORRECTION HAD TO REACH THREE SENTENCES, NOT ONE, AND I FOUND THE THIRD.** The
        coordinator named two: the banner, and "THEY ADD NO POLICY. Each one forwards to the
        typed method beside it..." -- the second mattering more because it states a GUARANTEE
        rather than a count, so a reader is likelier to rely on it, and a fix reaching only
        the banner leaves the false claim standing where it does work. `Turn()` and
        `SideToMove()` refute it the same way: they forward to nothing, convert nothing, and
        cannot refuse at all, so "every refusal is the typed method's" is VACUOUS for them
        rather than true. **THE THIRD WAS "OUT OF LINE, DELIBERATELY, all six"** -- the
        section declares seven, and every one of them is out of line, established by
        inspection (no declaration in the section carries a body) rather than by counting.
        Its stated reason -- an inline body would instantiate the caller's TU over
        `strat::Hex` -- does NOT reach `Turn()`/`SideToMove()`, which name no vendored type;
        that is now said, so a reader does not take the argument as covering a case it does
        not. Three cardinals in one block, all removed.
      - **TWO CARDINALS IN THE SECTION WERE CHECKED AND DELIBERATELY LEFT ALONE**, because
        both name a set the tree defines: "The five `Submit*` methods" (the section banner is
        "Typed commands (§4.9's five, and no others)", and `strat::SaveCommandKind` is pinned
        at five) and "§2.9's AI emits Build as one of its four kinds" (`strat::AiCommandKind`
        is `{Build, Move, Attack, EndTurn}`). A cardinal over a DEFINED set is checkable; the
        defect is a cardinal over an undefined one.

    - **MECHANISM (d), AND IT IS THE ONE THAT BREAKS THE PATTERN OF THE OTHER THREE: A
      QUANTIFIER OVER A SET THE SENTENCE NEVER DEFINES.** (a) counted the wrong subject,
      (b) quoted its own search token, (c) over-constrained the anchor -- and **each of those
      three was findable by ONE reader running ONE check.** (d) was not findable that way at
      all: every reader who ran a count got a self-consistent answer, and the answers differed
      only because each had silently supplied a different scope. It surfaced solely because
      two readers compared results and a third reading (17) showed the disagreement was not a
      tie to be broken. **SO THE PRACTICE THAT CATCHES (a)-(c) -- run the check the sentence
      prescribes -- CANNOT CATCH (d).** What catches (d) is refusing to write a cardinal whose
      set the sentence does not name, and, when one is disputed, holding the edit rather than
      picking the likelier number. Holding was the right call here on the coordinator's own
      verdict: a confidently wrong list baked into the header would have been worse than the
      uncheckable number it replaced.
    - **A BANNER OFF BY ONE BECAUSE OF MY OWN INSERTION.** `StratBridge.h`'s engine-typed
      facade read "The six methods below say exactly what the five above say"; `MatchResult`
      was inserted immediately above it, so a reader counting finds seven. Corrected, AND the
      asymmetry is now stated rather than left to look like an omission: `MatchResult` needs
      no `int32`/`FIntPoint` mirror because its consumer is `StratUI`, which MAY name a
      `strat::` type, and `StratBuildMatchResult` hands `StratPlay` a struct that names
      nothing vendored. A seventh mirror would be a second translation of one value.
    - **AN UNQUALIFIED "EXACTLY ONE WRITER" THAT AUTOMATION HAD ALREADY FALSIFIED.**
      `UStratMatchSubsystem::ProductionMenu`'s block said "there is exactly one writer,
      `RefreshProductionMenu`". `Source/StratPlay/Tests/StratProductionMenuSeam.cpp` assigns
      both `ProductionMenu` and `ProductionMenuHex` directly, to re-plant a menu a deliberate
      reseed cleared. Nothing is broken -- `BlueprintReadOnly` never held C++ in this module
      out, the plant is declared at its own site, and the REASON the block gives is about
      Blueprints and still holds. Stamped: shipping writers are `RefreshProductionMenu` and
      `CloseProductionMenu`, and the invariant a widget author may rely on is the one about
      Blueprints, not a total writer count. **CHECKED AND CLEAR WHILE I WAS THERE:** that
      file's out-of-range viewing side is arranged through `StartMatch` -- which assigns
      `Config.ViewingSide` without a range check, deliberately and by its own comment -- and
      NOT through `SetViewingSide`, so the 4b reordering does not touch it.
  - **BUILD: GREEN, MEASURED AFTER THE LAST EDIT.** `Build.bat StratocracyEditor Win64
    Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
    -NoHotReloadFromIDE` from the Bash tool in the integration tree -> `Result: Succeeded`,
    exit 0, 64 actions, 317.62 s. The editor was closed for it. THIS IS THE FINAL BUILD, run
    after the three prose corrections above; the earlier one in this pass was 59 actions /
    289.55 s and is superseded rather than deleted, because the action count moved for a
    reason that is not mine -- `strat-test-author` landed five new clause files in the same
    tree between the two runs, and a reader comparing the numbers should not read that as
    engine code growing. No suite figure is stated here; `global.md` owns it, and the clauses
    are `strat-test-author`'s.

- **2026-08-24, `strat-gameplay-engineer`: THREE COMMENTS NAMED BLUEPRINTS THAT DO NOT EXIST, IN
  THE SENTENCES THAT ASSIGN THE WORK TO THE CONTENT LANE. COMMENT-ONLY; NO DECLARATION, SIGNATURE
  OR EXECUTABLE LINE MOVED.** Four name instances across `StratBoardActor.h`, `StratUnitActor.h`
  and `StratGuidedOpening.h`: `BP_StratBoardActor` -> `BP_StratBoard`, `BP_StratUnitActor` ->
  `BP_StratUnit`. `Content/StratPlay/` holds six Blueprints and no `...Actor` variant of anything;
  the content lane's own record has said so since 2026-08-23. No suite count and no verdict is
  stated here -- `global.md` owns both, and no suite figure moves on a comment.
  - **WHY THIS IS NOT AN ORDINARY STALE COMMENT.** All three sentences are hand-off sentences:
    each one names the asset the CONTENT lane is being told to author or assign. A stale comment
    misinforms whoever reads it; a comment that names a non-existent asset in the sentence that
    ASSIGNS the work sends the receiving lane to search `/Game/StratPlay` for a name that has
    never been there, and the search comes back empty with nothing to indicate the brief was
    wrong rather than the asset missing. That asymmetry is the reason this was worth a pass.
  - **BUILD: 22 OF 25 ACTIONS COMPILED CLEAN; THE LINK WAS REFUSED BY THE OPEN EDITOR AND THAT
    WAS THE EXPECTED OUTCOME.** `Build.bat StratocracyEditor Win64 Development -waitmutex
    -NoHotReloadFromIDE`, 172.58 s. UHT re-ran (`Invalidating makefile ... working set of source
    files changed`) and `Module.StratPlay.gen.cpp` compiled, so the three edited headers parsed.
    Then `UbaSessionServer - ERROR opening file ...UnrealEditor-StratPlay.dll for write after
    retrying for 20 seconds`, and on the non-UBA retry `LINK : fatal error LNK1104: cannot open
    file 'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll'`. **For a
    comment-only change the compile IS the evidence** -- what a comment edit can break is a block
    comment or a line continuation, and that is a compile error, not a link error. The link adds
    nothing here and its refusal is not a finding.
  - **`sed` STRIPPED THE CARRIAGE RETURNS AND IT DID NOT ANNOUNCE ITSELF.** `StratBoardActor.h`
    and `StratGuidedOpening.h` are CRLF in this worktree (461 and 528 CRs); `StratUnitActor.h` is
    LF (0 CRs) -- measured per file, not assumed. An in-place `sed` rewrote both CRLF files whole
    and `git diff --numstat` would have read `461 461` instead of `1 1`. Restored before
    inspecting the diff. Measure endings per file before any scripted edit to a header.
  - **TWO NEIGHBOURING SENTENCES ARE NOW WRONG AS A CONSEQUENCE, AND ARE DELIBERATELY NOT FIXED
    IN THIS PASS.** Both say the assignment is still owed, and it is not: `BP_StratBoard`'s bytes
    carry `/Game/StratArt/Materials/MI_Overlay_Objective` against `ObjectiveMaterial`, and
    `BP_StratUnit`'s carry `/Game/StratArt/Meshes/SM_GuidedMarker` and
    `/Game/StratArt/Materials/MI_Marker_Guided` against `GuidedMarkerMesh` and
    `GuidedMarkerMaterial`. So `ObjectiveMaterial`'s "UNSET IS LEGITIMATE AND IS THE STATE THIS
    SHIPS IN", `GuidedMarkerMesh`'s identical clause, and `StratGuidedOpening.h`'s "What is still
    the content lane's is the asset half: the meshes, the material instances, and their
    assignment" all describe a tree that stopped existing when those defaults were committed.
    Kept out so this diff stays exactly what its message claims. **Discharged by** a follow-up
    pass that rewrites those three blocks to say the defaults are authored -- and note the ring
    needs no new mesh at all, it instances `OverlayMesh`, so "the meshes" is plural about one.
    **DISCHARGED 2026-08-24 by the entry immediately below.** (Its own lead says "TWO
    NEIGHBOURING SENTENCES" and then lists three; three is the count, and three were fixed.)

- **2026-08-24, WRITTEN BY THE `coordinator` INTO THE ENGINEER'S FILE, DECLARED: THE THREE
  BLOCKS THE ENTRY ABOVE DEFERRED NOW SAY THE DEFAULTS ARE AUTHORED. COMMENT-ONLY; NO
  DECLARATION, SIGNATURE OR EXECUTABLE LINE MOVED.** `strat-gameplay-engineer` is `engine.md`'s
  sole writer and was not dispatched for this pass; the user asked for the fix directly in the
  main session. Recorded here rather than nowhere, and named so the next reader knows whose hand
  it was. No suite count and no verdict is stated here -- `global.md` owns both, and no suite
  figure moves on a comment.
  - **WHAT EACH BLOCK NOW SAYS.** `AStratBoardActor::ObjectiveMaterial` and
    `AStratUnitActor::GuidedMarkerMesh` are rewritten in place: the assignment is stated as
    landed and the instance is named, "unset is legitimate" is kept but demoted to *any other
    Blueprint of this class* rather than the shipping state, and each block says outright that
    it used to claim otherwise. `StratGuidedOpening.h`'s block gets an APPENDED
    `[AMENDED 2026-08-24: ...]` instead of a rewrite -- the sentence under it is itself an
    amendment, and leaving it standing keeps the boundary it drew legible when only its tense
    is wrong.
  - **"THE MESHES" WAS PLURAL ABOUT ONE, AND THE AMENDMENT SAYS SO.** The ring instances
    `AStratBoardActor::OverlayMesh`; `SM_GuidedMarker` is the only mesh the content lane added.
  - **THE PREMISE WAS RE-MEASURED OFF THE PACKAGE BYTES, NOT CARRIED OVER FROM THE ENTRY
    ABOVE.** `grep -a` over the working-tree `.uasset`s: `BP_StratBoard.uasset` yields
    `/Game/StratArt/Materials/MI_Overlay_Objective` alongside the name `ObjectiveMaterial`;
    `BP_StratUnit.uasset` yields `/Game/StratArt/Meshes/SM_GuidedMarker` and
    `/Game/StratArt/Materials/MI_Marker_Guided` alongside `GuidedMarkerMesh` and
    `GuidedMarkerMaterial`. Working-tree bytes, not `git show` -- `.uasset` is LFS there. Both
    packages are clean in `git status`, so the defaults are committed, not pending.
  - **BUILD: 22 OF 25 ACTIONS COMPILED CLEAN; THE LINK WAS REFUSED BY THE OPEN EDITOR, THE SAME
    EXPECTED OUTCOME AS THE PASS ABOVE.** 169.34 s.
    `Module.StratPlay.gen.cpp`, `StratBoardActor.cpp`, `StratGuidedOpening.cpp` and
    `StratUnitActor.cpp` all compiled, so all three edited headers parsed. Then
    `UbaSessionServer - ERROR opening file ...UnrealEditor-StratPlay.dll for write` and on the
    non-UBA retry `LINK : fatal error LNK1104`. For a comment-only change the compile IS the
    evidence; the link adds nothing and its refusal is not a finding.
  - **THE CARRIAGE-RETURN TRAP FROM THE PASS ABOVE WAS AVOIDED BY NOT USING `sed`.** Edits made
    through the line-local editor; CRs counted before and after with `tr -cd '\r' | wc -c`
    (`grep` cannot see a CR on this box). `StratBoardActor.h` 461 -> 464, `StratGuidedOpening.h`
    528 -> 536, `StratUnitActor.h` 0 -> 0, each delta equal to that file's added-line count.
    `git diff --numstat` reads `7 4` / `9 1` / `7 3` -- line-local, not a whole-file rewrite.

- **2026-08-24, `strat-gameplay-engineer`: THE TURN-1a MARKER COULD NEVER CLEAR, BECAUSE EVERY
  OPERAND IT HAD WAS MATCH-CONSTANT. THE CODE COMPILES AND THE TREE LINKS.** Found in a human
  playtest, not by any clause. No suite count and no verdict is stated here; `global.md` owns
  both, and NO SUITE WAS RUN THIS PASS -- it is the test author's and is owed after this
  signature.
  **[HEADLINE AMENDED 2026-08-24, LATER THE SAME MORNING, BY THIS ENTRY'S OWN AUTHOR.** It read
  "THE CODE COMPILES AND THE TREE DOES NOT LINK", which was true of the tree it described: the
  editor held the DLL. The editor closed and the link completed. The two build bullets at the
  foot of this entry carry both runs and neither is deleted, because the compile-only measurement
  is what separated "the code is wrong" from "the editor is open".]**
  - **THE DIAGNOSIS IS CONFIRMED IN THIS TREE AND WAS RE-DERIVED RATHER THAN TAKEN ON REPORT.**
    `AStratUnitActor::ApplyUnitView` set the marker from
    `View.bIsGuidedMarked && View.Side == ViewingSide`. `bIsGuidedMarked` is the rules module's
    derivation off `placement` -- `StratViewModel.cpp` copies `Source.isGuidedMarked` and is its
    only producer -- and both sides are fixed for a match. So the conjunction HAD NO FALSE IN IT
    once true, and the marker latched on for the rest of the match. The player saw the objective
    ring clear with the Infantry's marker still lit.
  - **THE DURABLE FINDING IS NOT THE MISSING BIT. BOTH-DIRECTIONS IS A PROPERTY OF THE OPERAND
    SET AND NOT ONLY OF THE WRITER.** The comment directly above the defective line already named
    this exact failure mode -- "a writer that only ever SHOWS is a writer whose hide can be missed
    on one path -- and for this bit that means a permanent marker on a unit §2.11.6's window
    closed on" -- and the writer SATISFIED it: `SetVisibility` ran unconditionally, in both
    directions, on every one of the ~1700 calls a suite run makes. **A correctly-written
    unconditional writer produced a latch**, because the discipline was checked at the assignment
    and never at the inputs. That sentence is now in `GuidedMarker`'s own block.
  - **THE MISSING OPERAND IS `FStratViewModel::Guidance.bActive`, AND THE GDD PICKED IT RATHER
    THAN THIS LANE.** §2.11.6, quoted in `StratGuidedOpening.h`: the ring "and the turn-1a unit
    marker clear in the same frame as the strip". The strip IS `bActive`. **NOT "beat 1a
    retired"** -- that would clear the mark the moment the beat it belongs to succeeded, one beat
    early -- and **NOT a hex test**, which `StratGuidedOpening.h` records as the trap that unmarks
    the unit at the exact moment beat 1a needs it marked. `bIsGuidedMarked` WAS NOT TOUCHED and
    must not be; `StratViewModelParity` pins it and a second producer is the drift the view model
    header exists to refuse.
  - **THE RING'S BEHAVIOUR MAKES "SAME FRAME" STRUCTURAL AND THAT WAS MEASURED, NOT ASSUMED.**
    `FStratGuidedOpening` writes `bHasObjective = false` in exactly three places -- `SkipGuidance`,
    the turn-4 window close, the all-beats-retired branch -- and ALL THREE set `bActive = false`
    beside it; `DecorateViewModel` assigns `View.bActive = bActive` unconditionally and writes
    `bHasObjectiveRing` only inside `if (bActive)`. So a frame in which the ring is out is a frame
    in which `bActive` is out, and the marker now rides the same bool through the same
    `UStratMatchSubsystem::ApplyView` that drives the ring twenty lines below it.
  - **THE SIGNATURE GREW A FOURTH PARAMETER AND IT IS A `bool`, NOT THE `FStratGuidanceView`.**
    `AStratUnitActor::ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation,
    int32 ViewingSide, bool bGuidanceActive)`. Passing the whole struct was the other shape and
    was killed on one measurement rather than on taste: it would put `ObjectiveHex` within reach
    of the marker writer, which is the hex-keyed derivation `GuidedMarker`'s block forbids by
    name. **WHAT THIS CLASS CANNOT SEE, IT CANNOT USE**, and that is the same argument the
    `ViewingSide` parameter was shaped by on 2026-08-23. Not defaulted, for that parameter's
    recorded reason: a default lets a future caller get a latching marker silently.
  - **ONE CALL SITE, MEASURED AND NOT ASSUMED.** `grep` for `->ApplyUnitView(` over `Source/`
    returns exactly one line, in `UStratMatchSubsystem::ApplyView`, which already held the whole
    `FStratViewModel`. **NO FILE UNDER `Tests/` CALLS IT**, so no lane but this one had to move --
    corroborated at compile time, since `StratGuidedOpeningVisuals.cpp` and
    `StratMatchReconcile.cpp` both rebuilt clean against the new declaration.
  - **THE FALSE SENTENCE IN `FStratGuidedOpening::SkipGuidance` IS STAMPED, NOT DELETED.** It read
    "the marker clears because that same call publishes `bActive` false and the marker is drawn
    only while guidance runs" -- **an unimplemented mechanism asserted as fact for a day**, and it
    is exactly the shape this record has been corrected for before. It is left standing with a
    dated stamp because it is TRUE OF THIS TREE NOW and a reader should see that it described an
    intention before it described an implementation. **A SENTENCE IN ONE FILE ASSERTING WHAT
    ANOTHER FILE DOES IS A CLAIM AND NOT A SPECIFICATION.**
  - **A SECOND FALSE SENTENCE WAS FOUND BY FOLLOWING THE FIRST, AND THE BRIEF DID NOT NAME IT.**
    `FStratGuidedOpening::SkipGuidance`'s DECLARATION in `StratGuidedOpening.h` said "the marker
    reads `bIsGuidedMarked` on a model the very next `DecorateViewModel` writes with `bActive`
    false" -- which implies `bIsGuidedMarked` falls with `bActive`, and it does not and must not.
    Retracted in place with the `RETRACTED>` form. The `.cpp` and the `.h` carried the same wrong
    belief in different words, which is why fixing the site the brief named would not have
    finished the job.
  - **THE COUNT IN `GuidedMarker`'s BLOCK MOVES A SECOND TIME AND IS NOW RETIRED AS AN
    INVARIANT.** ONE until 2026-08-23, TWO until 2026-08-24, THREE today; both prior wordings are
    kept as `RETRACTED>` quotes. The "rendering of two fields" sentence further down is rewritten
    **COUNT-FREE** rather than re-numbered, on the precedent this file already set for the overlay
    count: the invariant was never the arithmetic, it is that every operand comes off ONE model
    and none is remembered. Re-numbering to three would rebuild the same trap for whichever
    ruling adds a fourth.
  - **[SUPERSEDED 2026-08-24, LATER THE SAME MORNING, BY THIS ENTRY'S OWN AUTHOR: THE EDITOR
    CLOSED AND THE TREE LINKED. The bullet below is KEPT rather than deleted -- it carries the
    twenty compiles and the zero diagnostics that the two-action link run does NOT contain, so it
    is the only record of this change ever being compiled. Do not read it as a live instruction to
    rebuild.]**
  - **THE BUILD COMPILED EVERYTHING AND LINKED NOTHING, AND THE EDITOR WAS OPEN BY ARRANGEMENT.**
    `Get-Process` returns `UnrealEditor` PID 88652, with `explorer.exe` PID 13508 in the same
    round as the control that shows the instrument can speak. `Build.bat` with the documented
    arguments PLUS `-NoHotReloadFromIDE`, twenty-three actions: **all twenty compiles succeeded**,
    `[1/23] Module.StratPlay.gen.cpp` -- so UHT parsed the changed header -- through
    `[20/23] StratUnitActor.cpp`, with all five changed/adjacent sources excluded from the unity
    file per `[Adaptive Build]`. **ZERO WARNINGS AND ZERO COMPILER DIAGNOSTICS.** Then
    `UbaSessionServer - ERROR opening file E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll for write after retrying for 20 seconds (The process cannot access the file because it is being used by another process. - C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe)`,
    then on the non-UBA retry `LINK : fatal error LNK1104: cannot open file 'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll'`,
    `Result: Failed (OtherCompilationError)`, `REAL_EXIT=6`, 158.47 s.
  - **THIS IS THE WRITE LOCK AND NOT THE LIVE CODING MUTEX, AND THE FLAG BEHAVED EXACTLY AS
    `.agents/ue-project-context.md` PREDICTS.** That file records that `-NoHotReloadFromIDE`
    defeats the engine-keyed mutex but NOT this tree's DLL write lock, and that the symptom is a
    full compile followed by a UBA write error and then `LNK1104`. Observed shape matches
    line-for-line, including the retry wording. The mutex form aborts ahead of the action graph
    with zero compiles; this ran twenty. **NOT WORKED AROUND, NOT RETRIED, AND NO DLL DELETED** --
    the fix is the editor closing.
  - **THE LINK COMPLETED ON A SECOND RUN WITH THE EDITOR CLOSED, AND IT WAS TWO ACTIONS, WHICH IS
    STATED PLAINLY RATHER THAN GLOSSED.** `Get-Process UnrealEditor` returns nothing, with
    `explorer.exe` PID 13508 in the same round as the control. Same command, same flags:
    `Using Unreal Build Accelerator local executor to run 2 action(s)`,
    `[1/2] Link [x64] UnrealEditor-StratPlay.dll`, `[2/2] WriteMetadata StratocracyEditor.target`,
    `Result: Succeeded`, `REAL_EXIT=0`, 2.44 s. **THE TWENTY COMPILES ARE THE EARLIER PASS'S AND
    NOT THIS RUN'S** -- this run compiled nothing at all. That is sound here and the soundness was
    measured rather than asserted: the objects were produced by a BYTE-IDENTICAL INVOCATION over
    BYTE-IDENTICAL SOURCES. All five changed sources carry mtimes of 10:10:36 - 10:12:38, every
    one of them BEFORE the 20-compile pass, and `git hash-object` over all six changed files
    returns the same digests after this run as before it. No file was touched between the compile
    and the link.
  - **A NEW DLL EXISTS AND THE FILE MOVED, WHICH IS THE ONLY THING THAT DISTINGUISHES A LINK FROM
    A CACHED `Succeeded`.** `Binaries/Win64/UnrealEditor-StratPlay.dll` went from **1431552 B /
    2026-08-23 19:46** to **1435136 B / 2026-08-24 10:22** -- 3584 bytes larger, stamped in the
    run's own minute. **THE FAILED PASS HAD ALREADY MOVED THE `.pdb` AND NOT THE `.dll`**
    (`UnrealEditor-StratPlay.pdb` stamped 10:15 while the DLL still read 19:46), which is worth
    knowing: on an `LNK1104` the debug symbols can advance while the binary does not, so a `.pdb`
    timestamp is NOT evidence that a link happened.
  - **[STAMPED 2026-08-24, LATER THE SAME DAY. THE FIGURE ABOVE WAS CORRECT FOR THIS LANE'S LINK
    AND NO LONGER DESCRIBES THE BINARY ON DISK, WHICH IS EXPECTED RATHER THAN A DRIFT.]** A reader
    who stats `UnrealEditor-StratPlay.dll` now gets **1460224 B / 2026-08-24 10:40:11**, not the
    **1435136 B / 10:22** recorded above. Nothing was rewritten and nothing was relinked here:
    `strat-test-author` added roughly 690 lines of clauses to `Source/StratPlay/Tests/` and
    relinked the same module eighteen minutes later, in its own lane. **THE ORDERING IS COHERENT
    AND IS THE POINT** -- this lane's link at 10:22 local, the test lane's relink at 10:40:11, and
    the suite at `reportCreatedOn 2026.08.24-14.41.49` UTC, which is 10:41 local, so the suite ran
    against the LATER binary and that binary contains this change. Re-measured here with `ls -la`
    over the whole directory rather than taken on report, with the four untouched sibling DLLs
    (`StratBridge`, `StratRules`, `Stratocracy` at 2026-08-22, `StratUI` at 2026-08-23) as the
    control that the listing reports real per-file stamps and not one blanket time.
    **THE ORIGINAL FIGURE IS KEPT AND MUST NOT BE REFRESHED TO MATCH THE DISK.** It is the only
    record that this lane's link produced a binary at all -- the two-action run compiled nothing,
    so the moved byte count is the whole of that evidence. A number that tracks the current file
    would prove nothing about any particular link, which is the property being protected. **A
    BUILD ARTEFACT FIGURE IN THIS RECORD DESCRIBES A RUN, NOT A FILE**, and a later in-lane relink
    is the normal way it stops matching. `strat-integration-reviewer` raised this as an
    observation and did not gate on it; its verdict and the lane its one finding belongs to are
    stated where they are owned, and are deliberately not restated here.
  - **AN INSTRUMENT LIED SILENTLY AND ONLY A CONTROL CAUGHT IT.** `Get-Item` on the DLL through
    the PowerShell tool printed NOTHING -- no object and no error -- for a file that exists, which
    reads exactly like "the binary was never produced" and would have contradicted the
    coordinator's baseline. `ls -la` over the directory, with a total-entry count as the control,
    returned the file at precisely the coordinator's figures. **A NULL RESULT FROM A STAT IS NOT AN
    ABSENCE UNTIL THE INSTRUMENT HAS BEEN SHOWN ABLE TO SPEAK**, and this project has paid for that
    shape before.
  - **WHAT IS THEREFORE PROVED AND WHAT IS NOT.** PROVED: these bytes compile, the reflected
    header parses, every dependent translation unit in `StratPlay` including the test lane's
    agrees with the new signature, and the module links and is on disk. NOT PROVED: **no suite was
    run -- deliberately, it is the test author's and it is owed after this signature lands** -- and
    nothing has been observed at runtime. The playtest observation that produced the defect has no
    clause behind it, which is why the suite was green with it live.
  - **WHAT WAS NOT DONE.** No `Tests/` file, no `Content/` asset, no `Source/StratRules/` and no
    `Data/` was touched, and no re-vendor request is filed -- the rules module was correct
    throughout and `bIsGuidedMarked` is doing exactly what it was designed to do. The stale
    `BP_StratBoardActor` / `BP_StratUnitActor` comment names and `GuidedMarkerZOffset`'s occlusion
    were left alone on the coordinator's instruction; both are known and filed.
  - **DEBT: NOBODY HAS SEEN THE MARKER CLEAR.** The tree links now, and that moves nothing about
    this debt -- the fix is still a flag change no eye has been on.
    **DISCHARGED WHEN** a human plays §2.11.6-B past the window's close with this in the tree and
    reports the marker going out with the ring -- injected input never reaches `UPlayerInput` on
    this project, so no agent can take that observation. OWNED: coordinator, to schedule.
  - **[DISCHARGED 2026-08-24. A HUMAN PLAYED IT AND THE MARKER WENT OUT WITH THE RING. THE DEBT
    TEXT ABOVE IS KEPT DELIBERATELY]** -- it is the record that this fix shipped unobserved, and
    of how long it stayed that way. The player ran the guided opening to completion and reports
    the marker clearing WITH the ring, and additionally that the guidance STRIP disappeared in the
    same frame: all three surfaces dark together, which is §2.11.6's sentence in full rather than
    the half this lane owed.
    **WHAT THE LOG PROVES AND WHAT THE EYE PROVES ARE DIFFERENT, AND ONLY ONE OF THEM IS AN
    INSTRUMENT.** `Saved/Logs/Stratocracy.log` carries `Guided opening complete on turn 3: every
    beat retired.` at `[2026.08.24-15.57.47:040]` and `[2026.08.24-15.58.38:000]`, re-measured
    here rather than restated -- and a THIRD at `[2026.08.24-15.23.54:477]`, also after this
    lane's 10:22 link and the test lane's 10:40 relink, so every one of them ran against a binary
    containing this change. That line is emitted on the ALL-BEATS-RETIRED branch, one of the three
    places `bActive` goes false, so the log proves the sessions REACHED the instant the fourth
    operand acts on. **IT DOES NOT PROVE WHAT WAS ON SCREEN.** The three surfaces going dark
    together is the player's observation and nothing in this tree can corroborate it:
    `IsGuidedMarkerVisible` reports the visible FLAG and not pixels, exactly as its own block says,
    so **THIS DEBT IS DISCHARGED BY A HUMAN AND NOT BY A CLAUSE**, and no clause can be written
    that would discharge it.
    **THIS MAKES NO CLAUSE STRONGER THAN IT WAS.** The three clauses covering the marker remain
    pinned against a test-side instrument rather than against the production writer, because
    breaking the real subject would have needed an out-of-lane edit into this lane's files. That
    limit is `tests.md`'s and is unchanged by anything here; a playtest is not a gate, and a
    discharged debt is not a regression net. **WHAT IS STILL TRUE: nothing headless fails if this
    fourth operand is deleted tomorrow.**
  - **[CORRECTED 2026-08-24, SAME DAY, AFTER A COORDINATOR MEASUREMENT. THE TWO SENTENCES THAT
    CLOSE THE BULLET ABOVE ARE FALSE, AND THE SECOND OF THEM IS THE ONE THIS LANE'S OWN REPORT
    ASKED THE REVIEWER TO LOOK AT.]** They said:
    RETRACTED> "The three clauses covering the marker remain pinned against a test-side
    RETRACTED>  instrument rather than against the production writer, because breaking the real
    RETRACTED>  subject would have needed an out-of-lane edit into this lane's files."
    RETRACTED> "WHAT IS STILL TRUE: nothing headless fails if this fourth operand is deleted
    RETRACTED>  tomorrow."
    **THE CLAUSES READ THE PRODUCTION WRITER, AND THE MEASUREMENT WAS RE-TAKEN IN THIS TREE
    RATHER THAN ACCEPTED ON REPORT.**
    `Stratocracy.StratPlay.T-UI-02.GuidedMarkerClearsWhenGuidanceDeactivates` calls
    `Match.Subsystem->ApplyView(Dark)` -- the REAL `UStratMatchSubsystem`, which is the sole
    caller of `AStratUnitActor::ApplyUnitView` -- and reads the result through the fixture's
    `LitMarkerIds`, whose whole body is `Actor->IsGuidedMarkerVisible()` over the live actors that
    call reconciled. Before asserting anything it pins as FATAL premises that in the dark frame
    `Marked->bIsGuidedMarked && Marked->Side == Dark.ViewingSide` is STILL TRUE, that
    `Guidance.bActive` is the only field that moved, and that the seat and the unit count did not.
    Then it asserts `StillLit.Num() == 0`. **So deleting the fourth operand leaves the two
    match-constant operands the clause has just asserted true, the marker stays lit, and that
    closing `TestEqual` goes RED.** It also lights the marker in frame one as a positive control,
    so it is not passing vacuously. **SOMETHING HEADLESS DOES FAIL. THE RETRACTED SENTENCE STATES
    THE OPPOSITE OF THE TREE.**
    **THE CONFLATION, NAMED, BECAUSE IT IS THE REUSABLE PART.** `strat-test-author` reported that
    its FALSIFIABILITY MUTATION lived in a test-side helper -- it reinstated the pre-fix predicate
    in the INSTRUMENT because breaking the real subject would have meant editing this lane's
    files, which its lane forbids. That is a fact about HOW IT PROVED THE CLAUSES CAN FAIL. This
    lane read it as a fact about WHAT THE CLAUSES READ. **A PROOF OF FALSIFIABILITY THAT ROUTES
    AROUND THE SUBJECT SAYS NOTHING ABOUT WHERE THE SUBJECT IS**, and merging the two understated
    this project's own coverage in the record -- the rarer direction, and no safer for it, because
    a future reader deletes an operand the record told them nothing guards.
    **BOTH SENTENCES ARE RETRACTED AND NOT ONLY THE CONCLUSION.** The premise sat two lines above
    the conclusion and would have been left standing by a fix aimed at the headline alone; this
    project has a recorded entry for exactly that half-done shape and this is not another one.
    **WHAT SURVIVES UNCHANGED, AND IT IS THE PART THAT WAS ACTUALLY MEASURED:** no clause gates
    THE MARKER REACHED THE SCREEN. `IsGuidedMarkerVisible` reports the visible FLAG and not
    pixels, so the pixel question is human-only and this debt really was discharged by a human
    rather than by a clause. **THE OVER-CLAIM WAS EXTENDING THAT FROM PIXELS TO THE OPERAND
    ITSELF.** The flag is gated; the pixels are not; those were never the same claim.

- **2026-08-23, `strat-gameplay-engineer`: THE OVERLAY-COUNT CLAIM IS SWEPT OUT OF THE TREE BY
  CLAIM SHAPE, AND THE SWEEP FOUND ONE THE REVIEWER DID NOT.** Comment-only; no signature, no
  behaviour and no `UPROPERTY` moved. Build green, suite green. `global.md` owns the count.
  - **THE GATING FINDING WAS MINE AND IT WAS A HALF-DONE RETRACTION.** Closing the "exactly
    two overlays" claim in `AStratBoardActor`'s header block, `ShowTargets` and `OverlayMesh`
    left it standing in two more places in the same file, both verified false against the
    tree: the "NOT IN THIS ROUND" bullet's "this class exposes the two overlays it drives"
    and `BoardRoot`'s "the two overlays are constructor subobjects and attach here too".
    `ObjectiveOverlay` is a third `CreateDefaultSubobject` and calls `SetupAttachment(BoardRoot)`
    beside the other two. **A RETRACTION THAT DOES NOT REACH EVERY SITE IS A RETRACTION THAT
    LEAVES THE CLAIM LOAD-BEARING SOMEWHERE**, and this is the second time this project has
    paid for that specific shape.
  - **A THIRD SITE WAS FOUND, AND ONLY THE SHAPE SWEEP COULD HAVE FOUND IT.** `FillOverlay`'s
    declaration read "The shared tail of `ShowReach` and `ShowTargets`, so the two cannot
    drift" -- **an overlay-count claim spelled as a CALLER LIST**, containing neither the word
    "overlays" nor any number a reader would think to grep. `ShowObjective` calls it too. A
    phrase sweep for "two overlays" returns this line zero times.
  - **WHAT WAS SEARCHED FOR AND WHAT CAME BACK, ZEROES INCLUDED**, because a sweep that
    reports only its hits cannot be told from a sweep that only ran one pattern. Over
    `StratBoardActor.h`: `two overlays` 4, `both overlays` 0, `the pair` 0, `two components`
    1, `there are exactly` 1, `second component` 2, `second material` 0, `one mesh, two` 0,
    `the other overlay` 0, `duo` 0, `these two` 0, `two of them` 1, `overlay component` 2,
    `\btwo\b` 15, `\bboth\b` 8, `\bexactly\b` 6, `\bpair\b` 1, `the two` 5. Every hit
    on the three broad shapes was read rather than counted; the survivors are unrelated
    counts (two containers, two copies of a formula, two materials on ONE component, two
    accessors easy to conflate, two readers of a clause, two adjacent hexes, two of three
    callers) and are listed here so the next sweeper does not re-open them.
  - **ONE "two overlays" HIT IS KEPT DELIBERATELY AND IS NAMED SO IT IS NOT MISTAKEN FOR A
    MISS.** `ShowObjective`'s own block says "there was nothing in C++ to bind to -- this
    class declared exactly two overlays". That is PAST TENSE inside a what-gap-this-closes
    narrative and is true of the tree before this change; rewriting it would delete the
    reason the component exists. The other two surviving hits are inside `RETRACTED>` quotes,
    which is what a retraction looks like when it works.
  - **THE CORRECTIONS ARE COUNT-FREE RATHER THAN RE-NUMBERED, WHICH IS THE ACTUAL FIX.**
    "the overlays it drives", "the overlays are constructor subobjects", "`ShowReach`,
    `ShowTargets` AND `ShowObjective`". Re-numbering to three would have rebuilt the same
    trap for whichever GDD section needs a fourth component. **The invariant is stated once**,
    in the header block: ONE MEANING PER COMPONENT. A count is a fact; that is a rule.
  - **ALL THREE NON-GATING SENTENCES WERE TAKEN, AND ONE WAS TAKEN AGAINST THE ARGUMENT FOR
    DECLINING IT.** `UStratMatchSubsystem::GetBoard`'s "the selection machine drives the two
    overlays through it" was TRUE -- the machine owns exactly two highlight sets and does not
    know the ring exists. It was amended anyway, because a reader arriving at it by grepping
    the count generalises it to the board, and an understatement that reads as a total is how
    a true sentence teaches a false thing. `AStratPlayerController::RefreshFromMachine`'s
    declaration and its `.cpp` counterpart were genuinely stale: that path repaints three
    overlays by two routes, this class calling `ShowReach` / `ShowTargets` itself while the
    ring rides the `ApplyView` step. Its step list is accurate for what that class does and
    was left alone; only the count moved.
  - **`StratGuidedOpening.h`'S LANE SENTENCE IS AMENDED AND THE BOUNDARY IS RESTATED WHERE IT
    ACTUALLY FELL.** It said "the ring mesh and the turn-1a marker are `strat-editor-builder`'s
    lane". The ring's COMPONENT and its show/clear are `AStratBoardActor::ShowObjective` /
    `ClearObjective`, and the marker's COMPONENT and its visibility are
    `AStratUnitActor::GuidedMarker` / `ApplyUnitView` -- C++, this lane. What remains the
    content lane's is the ASSET half: meshes, material instances, and their assignment on
    `BP_StratBoardActor` / `BP_StratUnitActor`. **Nothing about `FStratGuidedOpening` changed**
    -- it still draws nothing and names no component; the sentence described a boundary that
    moved underneath it.
  - **BUILD GREEN, AND A COMMENT-ONLY CHANGE WAS NOT ARGUED TO BE SAFE.** Plain `Build.bat`,
    no `-NoHotReloadFromIDE`, editor re-verified absent: `Result: Succeeded`, `REAL_EXIT=0`,
    136.70 s, twenty-five actions -- twenty-two compiles including `Module.StratPlay.gen.cpp`,
    then both links. Five headers changed, so UHT and every dependent TU rebuilt; that is why
    the run is bigger than the diff.
  - **SUITE GREEN AND THE FIGURE HELD, MEASURED RATHER THAN INFERRED.** Zero non-Success,
    `notRun` zero, `reportCreatedOn 2026.08.23-23.48.14` (UTC; 19:48:14 local). **CLAUSE DELTA
    ZERO BY MACRO SET-DIFFERENCE** -- `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/` reads
    identically before and after, correct because this agent writes none. **NO FIGURE IS
    WRITTEN HERE**; `global.md` owns it and it went to the coordinator. The clause that was
    red by design in the previous pass is green again, which is the test author's update
    landing and not anything this pass did.
  - **`Saved/SaveGames/` ENUMERATED ZERO BEFORE AND ZERO AFTER**, directory mtime moving
    19:09:36 -> 19:48:13, the run's own minute.
  - **WHAT WAS NOT DONE.** `global.md`, `content.md`, `decisions.md` and `tests.md` were not
    touched -- the record finding is the coordinator's and the steward's. No clause was
    written or edited. No `Content/` asset was touched, so **nothing has been seen on a
    screen** and this pass changes nothing about that.

- **2026-08-23, `strat-gameplay-engineer`: THE TURN-1a MARKER IS FILTERED TO THE VIEWING SIDE,
  AND A FALSE SENTENCE THIS LANE WROTE IS RETRACTED WHERE IT WAS BORN.** Build green; one
  clause red ON PURPOSE and named below. `global.md` owns the count and the verdict.
  - **THE PREMISE WAS RE-VERIFIED IN THIS TREE BEFORE ANY CODE MOVED.**
    `Data/ferrum_crossing.json` authors `guidedOpening` for BOTH seats -- side 0
    `infantry [1, 5]`, side 1 `infantry [9, 3]` -- so two units carry `bIsGuidedMarked` at
    once and the unfiltered pass-through marked the ENEMY seat's Infantry on the player's
    screen. **THE USER RULED IT OUT:** a marker that says "select this" pointing at a unit
    the player cannot select is confusing. That reasoning is carried in `GuidedMarker`'s own
    block, not only here.
  - **THE FILTER LANDED IN `AStratUnitActor::ApplyUnitView`, WHICH GREW A THIRD PARAMETER,
    AND THE VIEWING SIDE COMES OFF THE MODEL RATHER THAN OFF THE SUBSYSTEM.** The predicate
    is `View.bIsGuidedMarked && View.Side == ViewingSide` -- two published fields ANDed, no
    derivation, no lookup, no hex comparison, and `bIsGuidedMarked` still read off
    `placement` by the rules module and never recomputed here.
    `UStratMatchSubsystem::ApplyView` passes `Model.ViewingSide`.
  - **`UStratMatchSubsystem::GetViewingSide` EXISTS, WAS THE OTHER CANDIDATE, AND WAS
    REFUSED -- BY A SENTENCE ALREADY IN THE TREE.** `FStratViewModel::ViewingSide`'s own
    declaration block says it lives on the model "rather than in the actor that draws it so
    that 'what should be on screen' is a function of this value alone -- a viewing side held
    beside the model is a second input, and T-INT-05 would then be about two things". The
    subsystem's member is equal to the model's on every path that reaches `ApplyView` TODAY;
    reading it from the actor would make the marker a function of the model PLUS a member,
    and a stale member would put the mark on the wrong seat with a green build.
  - **THE THIRD PARAMETER IS NOT DEFAULTED, DELIBERATELY.** A default would have let a future
    caller get an unfiltered marker silently; making it a compile error is the cheaper
    failure. It cost nothing to require: `ApplyUnitView` has exactly ONE caller in the whole
    tree -- measured, `grep` for `ApplyUnitView(` outside its own declaration returns one
    line, in `UStratMatchSubsystem::ApplyView` -- and no file under `Tests/` calls it
    directly, so no lane but this one had to move.
  - **EXACTLY ONE CLAUSE WENT RED AND IT IS THE ONE THAT WAS SUPPOSED TO.**
    `Stratocracy.StratPlay.T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex`, with two
    assertions failing, both about unit 7 (side 1) and neither about unit 3 (side 0, the
    viewing seat): *"before the move: unit 7's turn-1a marker equals ITS OWN published
    `bIsGuidedMarked` -- a pass-through, not a side policy (the guided seat's marked unit is
    3; 2 units carry the bit): The two values are not equal."* and *"after the move: unit 7's
    turn-1a marker still equals ITS OWN published `bIsGuidedMarked`: The two values are not
    equal."* Its author wrote it against the unfiltered pass-through knowing a filter would
    turn it red. **NOT EDITED -- `Tests/` is not this lane** -- and NOT worked around. NO
    OTHER CLAUSE MOVED, which is the finding that matters: 197 Success, 1 Fail, 0 notRun, so
    the failure set is exactly the predicted singleton and not a superset.
  - **A FALSE SENTENCE THIS LANE WROTE IS RETRACTED AT ITS SOURCE, AND IT HAD ALREADY
    PROPAGATED TWO HOPS.** `AStratUnitActor::IsGuidedMarkerVisible`'s block claimed it
    answers "FALSE WITH NO MARKER MESH ASSIGNED" and concluded that a clause must "assign
    `GuidedMarkerMesh` on the spawned actor first". `USceneComponent::IsVisible` consults
    `bHiddenInGame`, the visible flag and the cached level collection -- NOT the static mesh
    -- so it answers TRUE for a marked unit whose marker draws nothing. Measured by
    `strat-test-author` in the suite; never measured by the author who wrote it. It reached
    a dispatch brief and from there a test author's instructions, which is why this is
    recorded as a propagation and not as a stray comment: **an unmeasured sentence in a
    declaration block is an instruction to everyone downstream.**
  - **THE CONSEQUENCE IS THE HONEST LIMIT OF THIS SEAM AND IT IS NOW IN THE HEADER.**
    `IsGuidedMarkerVisible` reports a FLAG, NOT PIXELS. It cannot say whether
    `GuidedMarkerMesh` was ever assigned and it cannot say a marker reached the screen, so
    **"the marker is actually on screen" has NO headless gate at all** and none is available
    from this class. The `BeginPlay` log line is the only place "unconfigured" is
    distinguishable from "not marked", and that path is still reached by no clause.
  - **FOUR COMMENTS THIS CHANGE FALSIFIED WERE AMENDED IN THE SAME CHANGE.**
    `GuidedMarker`'s "PASS-THROUGH OF ONE PUBLISHED FIELD" is retracted in place and now says
    two, with the ruling attached; `IsGuidedMarkerVisible`'s false sentence and the "assign
    the mesh first" instruction that rested on it are retracted together;
    `GuidedMarkerMesh`'s "`IsGuidedMarkerVisible` answers false" clause is corrected; and
    `UStratMatchSubsystem::ApplyView`'s "this function knowing nothing about either" is
    amended, because it now supplies an argument -- it still decides nothing.
  - **BUILD GREEN.** Plain `Build.bat`, no `-NoHotReloadFromIDE`, editor re-verified absent
    by `tasklist` with `explorer.exe` PID 13508 as the control: `Result: Succeeded`,
    `REAL_EXIT=0`, 94.50 s, seventeen actions including `Module.StratPlay.gen.cpp`. Zero
    warnings, zero errors.
  - **SUITE RAN IN THIS TREE, HEADLESS, EDITOR CLOSED.** `reportCreatedOn
    2026.08.23-22.51.00` (UTC; 18:51:00 local), `notRun` zero, one Fail as named above.
    **CLAUSE DELTA ZERO BY MACRO SET-DIFFERENCE** -- `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over
    `Source/` reads the same before and after, correct because this agent writes none. **NO
    FIGURE IS WRITTEN HERE**; `global.md` owns it and the figure went to the coordinator.
  - **THE MARKER WRITER IS OBSERVED EXECUTING AND ITS `BeginPlay` STILL IS NOT.**
    `ApplyUnitView` ran 1757 times in this run, counted off the pre-existing unconditional
    `No mesh assigned for unit definition` line. `Saved/SaveGames/` enumerated zero before
    and zero after, directory mtime moving 18:37:24 -> 18:51:00, the run's own minute.
  - **WHAT WAS NOT DONE.** The red clause was not edited and no clause was written --
    `Tests/` is not this lane; the update is owed to `strat-test-author` and the symbol names
    it needs are `AStratUnitActor::ApplyUnitView` (third parameter `ViewingSide`),
    `FStratViewModel::ViewingSide`, and `AStratUnitActor::IsGuidedMarkerVisible`. No
    `Content/` asset was touched, so **nothing draws yet and nobody has seen a marker on a
    screen** -- the filter is proved by a flag and by one clause going red in the predicted
    place, not by pixels.

- **2026-08-23, `strat-gameplay-engineer`: THE RING AND THE MARKER NOW HAVE C++ SEAMS. THE
  CONTENT LANE WAS BLOCKED BY AN ABSENCE IN THIS LANE AND IT IS NOT ANY MORE.** Build green,
  suite green in this tree; `global.md` owns the count and the verdict and neither is stated
  here.
  - **THE BLOCK WAS REAL AND IT WAS THIS LANE'S.** §2.11.6-B beat 1a says "Select the marked
    Infantry" and beat 2 says "the ringed Factory"; nothing on screen was marked or ringed, and
    the user learned the objective hex by reading the log. `strat-editor-builder` could not fix
    it from `Content/` because there was nothing declared to bind to: `AStratBoardActor` had
    exactly two overlay components and `AStratUnitActor` had one component and one `UFUNCTION`.
    Verified in this tree before building anything.
  - **RING: A THIRD OVERLAY, AND REUSING `TargetOverlay` WAS REFUSED FOR TWO REASONS, NOT ONE.**
    `AStratBoardActor::ObjectiveOverlay`, with `ShowObjective` / `ClearObjective` /
    `GetObjectiveOverlayCount` and an `EditDefaultsOnly` `ObjectiveMaterial` left UNSET. The
    semantic reason is that `TargetOverlay` is §2.6's ATTACK targets. The sharper one is
    clause-visible and would have failed loudly: `T-UI-02.AttackIsClosedForTheMarkedInfantry`
    asserts the attack overlay is dark during beat 1a, and beat 1a and the ring are on screen
    together - a ring drawn there would have lit the very component that clause says must be
    empty, so this was never a style question.
  - **`ShowObjective` TAKES ONE HEX AND NOT A `TArray`, AND THE SIGNATURE IS THE GUARD.**
    §4.7's `guidedOpening.objective` is one authored hex per seat. A set-shaped parameter would
    invite a caller to pass "every objective", which is the "nearest objective" heuristic
    §2.11.6 forbids by name arriving through a parameter list instead of through a function.
    Internally it goes through the same private `FillOverlay` the other two use, so the ring
    cannot drift from the highlights in how it clears or how it is Z-offset.
  - **[AMENDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR: THE MARKER IS NOW
    FILTERED TO THE VIEWING SIDE, ON A USER RULING, SO "ONE BOOL" AND "FROM NOTHING ELSE"
    BELOW ARE SUPERSEDED BY THE ENTRY ABOVE.** Kept rather than rewritten: the unfiltered
    pass-through is what a clause was written against, and the next reader needs to see that
    the second operand arrived by ruling rather than that it was always there.]** MARKER: A
    COMPONENT ON `AStratUnitActor`, DRIVEN OFF THE VIEW THAT ACTOR WAS ALREADY HANDED. `GuidedMarker`, a `VisibleAnywhere`
    component; plus THREE `EditDefaultsOnly` properties this change added to
    `AStratUnitActor` -- `GuidedMarkerMesh`, `GuidedMarkerMaterial` and
    `GuidedMarkerZOffset` -- of which TWO ship unset and `GuidedMarkerZOffset` carries a real
    default of `150.0f`; and `AStratUnitActor::IsGuidedMarkerVisible` as the clause seam.
    **THE SCOPE OF THAT COUNT IS THIS ACTOR AND THIS CHANGE, AND IT IS STATED BECAUSE STATING
    IT IS THE FIX.** `AStratUnitActor` carries SEVEN `EditDefaultsOnly` properties in total --
    `MeshByDefId`, `FallbackMesh`, `SideMaterials` and `BodyZOffset` predate this change -- so
    "the `EditDefaultsOnly` properties on this actor" would be a third wrong number. The
    feature's third unset asset reference is `AStratBoardActor::ObjectiveMaterial`, which is on
    the BOARD actor and is covered by the RING bullet above; it does not belong in this count.
    **[CORRECTED 2026-08-23, TWICE, AND BOTH ARE SHOWN BECAUSE THE SEQUENCE IS THE LESSON.**
    FIRST it read "all `EditDefaultsOnly` and all UNSET" -- false, because `GuidedMarkerZOffset`
    is `= 150.0f`. THEN the correction itself read "all four `EditDefaultsOnly`, of which THREE
    are unset" -- **false in two independent ways in one sentence**, and it CONTRADICTED THE STAMP
    IT WAS PART OF, which said in the same edit that `GuidedMarker` is `VisibleAnywhere` and not
    in the set. (That stamp's wording is superseded by this one; do not look for it below, it is
    not there. The true statement was written in the stamp and the false one in the sentence the
    stamp was correcting, four lines apart, by one author in one pass.) The denominator wrongly counted `GuidedMarker`, a `CreateDefaultSubobject`
    component that can never be unset; and the numerator, THREE, only balanced by importing
    `AStratBoardActor::ObjectiveMaterial` -- a property on a DIFFERENT ACTOR -- across a scope
    boundary into a bullet whose subject is one actor.
    **THE MEASUREMENT, RE-DERIVED FROM THE HEADERS FOR THIS BULLET'S ACTOR ONLY AND REUSING
    NEITHER PREVIOUS NUMBER:** `git diff` on `Source/StratPlay/StratUnitActor.h` adds exactly
    four `UPROPERTY`s -- `UPROPERTY(VisibleAnywhere)` on
    `TObjectPtr<UStaticMeshComponent> GuidedMarker`, and `UPROPERTY(EditDefaultsOnly)` on
    `TObjectPtr<UStaticMesh> GuidedMarkerMesh`, `TObjectPtr<UMaterialInterface>
    GuidedMarkerMaterial` and `float GuidedMarkerZOffset = 150.0f`. Three `EditDefaultsOnly`;
    two with no initialiser.
    **WHAT THE THREE ROUNDS HAVE IN COMMON, AND IT IS NOT CARELESSNESS WITH ARITHMETIC.** Round
    one named a set that was not the set measured. Round two counted across a scope the sentence
    did not own. NEITHER IS A COUNTING ERROR -- both are SCOPE errors that produced a wrong
    count as a symptom, which is why re-checking the number would not have caught either. The
    procedure that does catch them is the one the coordinator imposed on round three: **state
    the scope of the sentence in the sentence, then count only inside it.** The previous
    correction closed with "a recorded lesson is not a guard; only a measurement at the moment
    of writing is" -- and then got it wrong again while writing that very sentence, which is the
    second data point for its own claim and the reason it is left standing rather than
    softened.]**
    `AStratUnitActor::ApplyUnitView` sets visibility from `FStratUnitView::bIsGuidedMarked` and
    from nothing else - no hex comparison against `guidedOpening.infantry`, which
    `StratGuidedOpening.h` records as the trap that would unmark the unit at the exact moment
    beat 1a needs it marked, because that beat's entire content is that the unit MOVES.
  - **SET IN BOTH DIRECTIONS ON EVERY CALL, WHICH IS THE SAME RULE `PublishLocks` ALREADY
    CARRIES.** A writer that only ever SHOWS is a writer whose hide can be missed on one path,
    and for this bit that means a permanent marker on a unit the §2.11.6 window closed on. Same
    for the ring: the `ApplyView` call site has an `else` that clears, so "the ring and the
    marker clear in the same frame as the strip" - `FStratGuidedOpening::SkipGuidance`'s wording
    - is structural. The clear and the strip's push are a few lines apart in one function, on
    one value, and neither surface has a second driver.
  - **ONE CALL SITE FOR THE RING AND ZERO FOR THE MARKER, AND THE ASYMMETRY IS THE POINT.**
    `UStratMatchSubsystem::ApplyView` drives the ring because the board is not per-unit. The
    marker needs no call site there at all: `ApplyUnitView` is already called once per unit per
    refresh from that same function, so the subsystem knows nothing about the marker and the two
    visuals still ride one refresh. A second call site would have been a second driver.
  - **NEITHER VISUAL PERFORMS ARITHMETIC OR A DERIVATION.** The ring reads
    `FStratGuidanceView::bActive` and `bHasObjectiveRing` as a visibility condition and copies
    `ObjectiveHex`; the marker copies one bool (TWO as of the ruling in the entry above). `bHasObjectiveRing` is read because
    `FStratGuidanceView` declares that it alone qualifies `ObjectiveHex` - `FIntPoint(0, 0)` is a
    real hex and cannot signal its own absence, the trap this project has now paid for three
    times. **This is not T-UI-03's forbidden arithmetic**: no number is drawn and no widget
    renders the conjunction.
  - **NO `/Game/` LITERAL AND NO ASSET WAS TOUCHED.** THIS BULLET'S SCOPE IS THE WHOLE
    FEATURE -- both actors -- and it is named here so that it cannot be read as disagreeing
    with the MARKER bullet above, whose scope is `AStratUnitActor` alone and which therefore
    correctly says TWO. THREE of the four new configuration properties this change added
    across `AStratBoardActor` and `AStratUnitActor` ship unset -- `ObjectiveMaterial`,
    `GuidedMarkerMesh`, `GuidedMarkerMaterial`, which is every one that decides whether
    anything DRAWS; the meshes, the
    material instances and their assignment on `BP_StratBoardActor` / `BP_StratUnitActor` are
    the CONTENT lane's and are what actually makes either visual appear.
    **[CORRECTED 2026-08-23. THIS READ "All four new configuration properties ship UNSET" AND
    IT IS THREE.** `GuidedMarkerZOffset` is `= 150.0f` and has nothing to do with whether
    anything draws -- it is how high the marker floats once it does.
    **THE SUBJECT WAS WRONG, NOT THE SCOPE, WHICH IS WHY IT SURVIVED:** narrowing "all" to
    "most" would not have caught it, because the sentence named a DIFFERENT SET than the one
    that was measured. A quantifier check passes cleanly over a mis-named subject.
    **THIS FILE ALREADY CARRIED THE WARNING AND THE WARNING DID NOT STOP IT.** An entry
    further down says in as many words that "this project has already paid once for treating a
    real default as an unset marker" -- about `FIntPoint(0, 0)` and `ProductionMenuHex` -- and
    the same author, in the same file, then wrote a real `float` default into a set defined by
    being unset. A recorded lesson is not a guard; only a measurement taken at the moment of
    writing is.
    **THE FIGURE CAME IN FROM A DISPATCH AND WAS NOT INVENTED HERE, AND THAT IS THE LESS
    USEFUL HALF OF THE FINDING** -- it was reproduced here without being checked against the
    header this same agent had written minutes earlier, which no provenance excuses.]**
    **UNTIL THAT LANDS, NOTHING DRAWS** - the seam is necessary and is not sufficient, and this
    entry does not claim the section is unblocked on screen, only that the binding surface
    exists.
  - **THE TWO UNSET CASES ARE REPORTED DIFFERENTLY ON PURPOSE.** A missing `GuidedMarkerMesh` is
    logged once per actor at `BeginPlay`, because an unconfigured marker and an unmarked unit are
    indistinguishable on screen and have entirely different fixes. A missing `ObjectiveMaterial`
    is SILENT, because a ring with no material still draws in the mesh's own material - a
    visibly-wrong ring, which needs no log line to find. Only a missing `OverlayMesh`, which
    draws nothing at all, keeps the loud line.
  - **THE BOARD ACTOR'S HEADER SAID "THERE ARE EXACTLY TWO OF THEM" AND THAT IS RETRACTED IN
    PLACE.** The count was never the invariant; ONE MEANING PER COMPONENT was, and the retracted
    wording tied a structural rule to an arithmetic fact a new GDD section was always going to
    move. `GetTargetOverlayCount`'s block is stamped too: its "the day a third overlay lands"
    hypothetical is no longer hypothetical, so reading these counts BY NAME is now load-bearing
    rather than merely tidier. `AStratUnitActor`'s "NOT IN THIS ROUND" bullet about the
    DONE/locked visual is amended to draw the contrast rather than left to look inconsistent:
    `bIsGuidedMarked` names one unit for the whole match and its directive is unreadable without
    it, where `bDone` and `bLockedThisTurn` change several times a turn and §2.11 has not said
    what they look like. That bullet still stands for those two bits.
  - **BUILD GREEN.** `Build.bat` with the documented arguments and NO `-NoHotReloadFromIDE`,
    editor confirmed absent: `Result: Succeeded`, `REAL_EXIT=0`, 114.94 s, nineteen actions,
    including `Module.StratPlay.gen.cpp` - so UHT parsed the new `UFUNCTION`s and `UPROPERTY`s -
    and all four changed sources compiled as their own translation units per
    `[Adaptive Build] Excluded from StratPlay unity file`. Zero warnings, zero errors.
  - **SUITE GREEN IN THIS TREE.** Zero non-Success, `notRun` zero.
    `reportCreatedOn 2026.08.23-22.10.25` (UTC; 18:10:25 local). Clause delta ZERO by macro
    set-difference over `Source/`, unchanged across both passes. `Saved/SaveGames/` enumerated
    zero before and zero after, directory mtime moving 18:03:09 -> 18:10:24, the run's own
    minute. **NO FIGURE IS WRITTEN HERE**; `global.md` owns it.
  - **THE MARKER'S PER-REFRESH WRITER IS OBSERVED EXECUTING; NEITHER `BeginPlay` PATH IS.**
    `ApplyUnitView` ran 1697 times in this run - counted off the pre-existing
    `No mesh assigned for unit definition` line, which is unconditional on that path - so the
    `SetVisibility(View.bIsGuidedMarked)` line executed 1697 times without incident. But
    `Saved/Logs/Stratocracy.log` carries ZERO `GuidedMarkerMesh set` lines AND zero
    `has no OverlayMesh set` lines, and the second is PRE-EXISTING board behaviour. **So no
    clause in this suite reaches EITHER actor's `BeginPlay`**, the mesh/material assignment path
    is unobserved, and it will first execute in PIE. Stated because a green suite over this
    change proves the writer and not the configuration.
  - **WHAT WAS NOT DONE AND CANNOT BE CLAIMED.** No clause was written - `Tests/` is not this
    lane, and four are named in the handoff. No `Content/` asset was touched. **Nobody has SEEN
    a ring or a marker**: injected input never reaches `UPlayerInput` on this project, so that
    needs the content defaults plus a human at the keyboard, and it is the one thing that would
    close §2.11.6-B's visual half. OWNED: coordinator, to schedule.

- **2026-08-23, `strat-gameplay-engineer`: SEC 2.11.6-B'S BEAT 2 COULD NOT RETIRE ON THE SHIPPED
  SCENARIO. THE CODE IS WRITTEN AND NOTHING HAS COMPILED IT.** No suite count and no verdict is
  stated here; `global.md` owns both.
  - **THE DISPATCH BRIEF'S MECHANISM IS CONFIRMED, AND IT WAS RE-DERIVED FROM THE VENDORED
    SOURCES RATHER THAN TAKEN ON REPORT.** Four readings, each independently checkable:
    `strat::EconomyState::captureTurns` is declared `= 1` and NOTHING assigns it — the shipped
    `Data/ferrum_crossing.json` carries no `captureTurns` key and it is named in no loader, so
    the shipped match runs at 1. `strat::captureTick` pushes a `CaptureProgress` with
    `turnsHeld = 1`, tests `turnsHeld >= captureTurns` in the same iteration, flips
    `Objective::owner` and calls `strat::clearProgress`, which erases the entry — one call.
    That `push_back` is the ONLY write into `EconomyState::captures` anywhere in the vendored
    tree, so no other path can leave one standing. The projection's `progressForUnit` therefore
    finds nothing and returns 0, so `UiUnitView::captureProgress` and
    `FStratUnitView::CaptureProgress` read 0 in every snapshot that can ever be taken, and
    `FStratGuidedOpening::HasCapturePipLanded` is unreachable at `captureTurns = 1`.
  - **[RETRACTED 2026-08-23, SAME DAY, BY THIS ENTRY'S OWN AUTHOR AFTER A COORDINATOR
    CORRECTION. THE TWO HEXES ARE THE SAME HEX AND THERE WAS NEVER A DISCREPANCY.** The bullet
    below is KEPT rather than deleted because the reading was reasonable and the next person to
    diff a scenario file against a log needs to see why it was wrong.] It said:
    RETRACTED> "ONE FIGURE IN THE BRIEF IS REFUTED BY THE TREE, AND IT DOES NOT CHANGE THE
    RETRACTED>  DIAGNOSIS. The brief names the ringed objective as `(2, 7)`.
    RETRACTED>  `Data/ferrum_crossing.json`'s `guidedOpening` block authors `objective` as
    RETRACTED>  `[5, 7]` for side 0 and `[6, 2]` for side 1; `(2, 7)` is a capturable hex in
    RETRACTED>  the `ownership` block (`owner: -1`) and is NOT either seat's ring. Either the
    RETRACTED>  session captured a different factory than the one it was directed to — in which
    RETRACTED>  case beat 2 correctly did not retire, and the 2026-08-21 ruling was doing its
    RETRACTED>  job — or the hex was mis-transcribed. Nobody re-ran the session, so this is NOT
    RETRACTED>  settled."
  - **WHY THAT WAS WRONG: A SCENARIO AUTHORS ODD-R OFFSET AND THE RUNTIME SPEAKS AXIAL.** Two
    coordinate systems, one bracket notation, and nothing in the JSON says which it is.
    `Source/StratRules/Hex.h` states the conversion in its own comment — "odd-r offset <-> axial
    (§4.7): q = col - (row - (row & 1)) / 2, r = row" — and `Source/StratRules/Save.h` says
    "PARSED STATE HOLDS AXIAL ONLY. Hexes are authored odd-r `[col, row]`, converted at" parse.
    So authored `[5, 7]` is col 5, row 7, and q = 5 - (7 - (7 & 1)) / 2 = 5 - 3 = 2, r = 7 —
    **axial (2, 7)**, exactly what the log printed in two separate sessions. Re-derived here
    against those two headers rather than taken on report.
  - **WHAT THAT PUTS BACK, AND IT ALL RUNS THE FIX'S WAY.** The user stood on the RIGHT ringed
    objective; beat 2 genuinely failed to retire on the hex it was supposed to retire on; the
    observed symptom IS evidence of the defect and not a confounder; and
    `T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile` is green because it is CORRECT, not
    because it is weak — it compares like with like across the same conversion. Nothing about
    the fix or its reasoning changes; only the confidence it rests on, which goes UP.
  - **THE DURABLE LESSON, AND IT IS NOT ABOUT THIS HEX.** A bracketed integer pair in
    `Data/ferrum_crossing.json` and a bracketed integer pair in a `LogStratPlay` line are
    DIFFERENT COORDINATE SYSTEMS that print identically, and a straight comparison between them
    manufactures a discrepancy for roughly every hex with an odd row. Convert before comparing,
    or compare through `FStratBridge::GuidedOpeningHexes`, which has already converted.
  - **THE FIX IS A SECOND OBSERVABLE OR'd ONTO BEAT 2, AND THE SUBJECT IS UNCHANGED.**
    `FStratGuidedOpening::IsRingedObjectiveHeldByGuidedSide` is new: it reads
    `FStratHexView::Owner` at the SAME hex `HasCapturePipLanded` reads —
    `guidedOpening.objective` through `FStratBridge::GuidedOpeningHexes`. The 2026-08-21 user
    ruling is untouched, no "nearest objective" heuristic is introduced, no hex is derived, and
    there is no arithmetic: one equality against a field the rules module published.
    `Stratocracy.StratBridge.T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile` is load-bearing
    for the new arm exactly as it already was for the old one.
  - **THE PIP ARM IS KEPT RATHER THAN REPLACED, AND THAT WAS A CALL.** Deleting it would drop
    §2.11.6's own named trigger, and at `captureTurns >= 2` the pip is the EARLIER of the two —
    the arrival receipt, not the deed. Keeping both also means every existing clause in
    `Source/StratPlay/Tests/StratGuidedOpeningClauses.cpp` that plants `CaptureProgress` stays
    green, so this change is additive to the suite rather than a rewrite of another lane's file.
  - **`FStratHexView` AND NOT `FStratFactoryView`.** `guidedOpening.objective` is an
    `strat::Objective` and is not obliged to sit on Factory terrain; §2.11.6 says "ringed
    Factory" because that is what Ferrum Crossing authored. `FStratHexView::Owner` mirrors
    `UiHexView::owner` for every capturable hex, so the arm survives a scenario whose ring is a
    Town. A non-capturable hex reads `INDEX_NONE` (`== strat::OWNER_NEUTRAL`), which can never
    equal a side, so a mis-authored objective fails CLOSED.
  - **THE ARM IS A STATE AND NOT AN EVENT, DELIBERATELY.** A scenario seeding the ring already
    held by the guided seat retires beat 2 on the first observation. That is correct, not
    lenient: `strat::captureTick` short-circuits on `o.owner == side`, so no pip and no flip
    could ever follow there, and rule 1 would re-issue an impossible instruction forever — the
    same defect. Measured on the shipped data: both seats' objective hexes appear in `ownership`
    with `owner: -1`, so nothing retires early on Ferrum Crossing. The alternative shape — latch
    the ring's owner at the first `Observe` and retire on a CHANGE — was rejected for adding a
    second piece of remembered state beside `SeenFriendlyUnitIds` to make the machine WORSE in
    the only case the two differ on.
  - **THE RETIREMENT LOG NOW NAMES WHICH ARM FIRED** (`pip=`, `held=`), because the two mean
    different things to a reader of a session log and a single line would let this fix look like
    it worked for the reason nobody checked. The `Guided beat 2 retired on turn %d:` prefix is
    preserved. No test captures that string — the one existing reference is a `TestFalse` on a
    bool, not a log filter.
  - **TWO COMMENTS THIS CHANGE FALSIFIED WERE AMENDED IN THE SAME CHANGE.**
    `HasCapturePipLanded`'s block said "the ownership flip a turn later is a different event
    this beat deliberately does not wait for" — retracted in place, because on the shipped
    scenario there is no "a turn later": the flip and the pip's erasure are the same call. The
    header's four-retirement-triggers list and its "every fact this machine branches on" list
    both carry the new observable and the measurement behind it.
  - **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR. THE EDITOR
    CLOSED, THE TREE BUILT, AND THE SUITE RAN IN IT.** The bullet below reported a tree with no
    compiler output at all, and that was true of the tree it described. It is KEPT rather than
    deleted because its finding is durable and is a DIFFERENT finding from the `LNK1104` one
    this record already carries: the Live Coding mutex aborts ahead of the action graph, so a
    mutex block yields ZERO compile actions and zero diagnostics, where the DLL write lock
    yields a full compile and then a link failure. A reader who has only ever seen the second
    would misread the first as a code fault. **Do not read the bullet below as a live
    instruction to rebuild.**]**
  - **THE BUILD IS GREEN UNDER THE PLAIN DOCUMENTED COMMAND, WITH NO FLAG, AND THE EDITOR'S
    ABSENCE WAS RE-VERIFIED HERE RATHER THAN TAKEN ON REPORT.** `tasklist` returns `INFO: No
    tasks are running which match the specified criteria.` for `UnrealEditor.exe`, with
    `explorer.exe` PID 13508 listed in the same round as the control that shows the instrument
    can speak. `Build.bat` with the documented arguments and NO `-NoHotReloadFromIDE`:
    `Result: Succeeded`, `REAL_EXIT=0`, 56.29 s, fifteen actions. `[7/15] Compile [x64]
    StratGuidedOpening.cpp` is the first compile these bytes have ever had, and
    `[Adaptive Build] Excluded from StratPlay unity file: StratGuidedOpening.cpp` confirms the
    file was compiled as its own translation unit rather than folded into a cached unity blob.
    Zero warnings, zero errors, both links completed.
  - **THE SUITE RAN IN THIS TREE, HEADLESS, EDITOR CLOSED, AND IT IS GREEN.** Zero non-Success
    and `notRun` zero, every entry `Success`. `reportCreatedOn 2026.08.23-22.03.10` — UTC, which
    is 18:03:10 local; do not "correct" either stamp to match the other. **NO FIGURE IS WRITTEN
    HERE AND NONE MAY BE:** `global.md` owns the count, and the figure went to the coordinator.
    **THE CLAUSE DELTA IS ZERO AND IT WAS MEASURED BY MACRO SET-DIFFERENCE, NOT BY NAME** — the
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST` count over `Source/` is identical before and after, which
    is correct: this agent writes none.
  - **THE EXISTING PIP-DRIVEN CLAUSES STAYED GREEN, WHICH IS THE SPECIFIC THING THE OR ARM
    RISKED.** Four `Guided beat 2 retired` lines in `Saved/Logs/Stratocracy.log` from this run,
    every one reading `(pip=true, held=false)` — so the fixtures that plant `CaptureProgress`
    still retire beat 2 through the OLD arm and the new arm did not quietly take over. The new
    per-arm logging is what made that checkable at all; a single line would have hidden it.
  - **THE ODD-R CORRECTION IS CONFIRMED BY THE RUNTIME AND NOT ONLY BY ARITHMETIC.** Those same
    four lines print the ringed objective as `(2, 7)` — the machine's `Objective`, taken from
    `FStratBridge::GuidedOpeningHexes` off authored `[5, 7]`. The conversion is therefore
    observed, not merely computed, and the retracted discrepancy bullet above is settled.
  - **THE SAVE-SLOT CONTROL IS DISCHARGED ON THIS RUN AND NOT BORROWED.** `Saved/SaveGames/`
    enumerated ZERO entries before and ZERO after, and the directory mtime MOVED,
    17:26:41 -> 18:03:09, the run's own minute.
  - **THE BUILD WAS ATTEMPTED ONCE AND WAS BLOCKED BEFORE ANY COMPILE ACTION RAN.** `Build.bat`
    with the documented arguments and NO `-NoHotReloadFromIDE`, with `UnrealEditor.exe` open by
    prior arrangement: `Unable to build while Live Coding is active. Exit the editor and game,
    or press Ctrl+Alt+F11 if iterating on code in the editor or game`, then
    `Result: Failed (OtherCompilationError)`, 2.06 s, exit 6. **This is the Live Coding MUTEX
    and not the `LNK1104` DLL write lock** — it aborts ahead of the action graph, so ZERO
    compile actions ran and **there is no compiler output at all for these bytes**. Nothing
    below or above may be read as if the change had compiled. Discharged by the coordinator
    running the documented command with the editor closed.
  - **WHAT WAS NOT DONE.** No clause was written — `Tests/` is not this lane, and the clause
    this needs is named in the handoff. No `Content/` asset was touched; the editor-builder was
    live in that directory during this pass. `Source/StratRules/` and `Data/` were READ and not
    written, and no upstream re-vendor request is filed, because the fix needed neither — that
    is the reason option 3 (`captureTurns`) was not taken even though it would also have worked.

- **2026-08-23, `strat-gameplay-engineer`: SEC 2.8'S END-OF-MATCH TRANSITION IS BUILT. THE CODE
  COMPILES AND THE BUILD IS NOT GREEN, AND THOSE ARE TWO DIFFERENT SENTENCES.** Written against
  `global.md`'s topmost `## NEXT` entry ("A MATCH THAT ENDS DOES NOT END THE GAME"). No suite
  count and no verdict is stated here; neither could be measured this pass and `global.md` owns
  both anyway.
  - **THE UNVERIFIED LEAD IN THE BRIEF IS CONFIRMED, and it is confirmed by symbol rather than by
    line number.** `UStratMatchSubsystem::IsAiTurnDue` and `UStratMatchSubsystem::RunAiTurnsNow`
    are the two sites that read `FStratMatchView::bHasResult` in `StratPlay` outside `Tests/`, and
    both gate whether an AI turn should START. Neither gates the human, and neither survives the
    flag going down MID-`RunTurn`. So the subsystem could always SEE the result; it had nowhere to
    put the answer.
  - **ONE CORRECTION TO THE BRIEF'S ACCOUNT OF THE FIVE GREP HITS.** The brief describes the fifth
    as "a comment saying the thing does not exist", separately from the `IStratAiTurnPort` design
    note. Re-run in this tree, they are the SAME hit: the only non-`bRecordCompletionOnMatchEnd`
    match is `IStratAiTurnPort`'s own "no `IsMatchOver()`" sentence in
    `Source/StratPlay/StratAiTurnRunner.h`. Four flag hits plus that one is five. The conclusion is
    unaffected; the count was never five independent facts.
  - **WHAT THE FIX IS, IN THREE PIECES, CITED BY SYMBOL.**
    - `StratMatchIsConcluded` and `StratMatchAcceptsPlayerCommands` — new `STRATPLAY_API` free
      functions declared at file scope in `StratMatchSubsystem.h`, pure predicates over an
      `FStratViewModel`. `StratMatchConcludedRefusalText` is the single spelling of the refusal
      sentence, a function rather than a literal at three sites for the reason the `STRAT-AI`
      format string already taught this project.
    - `UStratMatchSubsystem::ConcludeMatchIfEnded` — the one-shot transition, called from
      `ApplyView` where `NoteMatchResultIfEnded` used to be called from directly. It calls
      `NoteMatchResultIfEnded` FIRST and OUTSIDE its own latch, so §2.11.6's completion writer
      keeps its own opt-in, its own latch and its own retry-on-failure; then it clears
      `AiTurnTimer` and logs `STRAT-MATCH concluded` once. `bMatchConclusionAnnounced` is the
      latch, cleared in `StartMatchInternal` beside `bMatchResultRecorded`.
      `UStratMatchSubsystem::IsMatchConcluded` and `UStratMatchSubsystem::GetConcludedMatchView`
      are the two reflected accessors.
    - `AStratPlayerController::HandleSelectionEvent` gates on the model it has ALREADY built,
      ahead of `TryArmGuidedOpening` and ahead of the machine;
      `AStratPlayerController::ToggleProductionMenu` gates only its OPEN branch; and
      `UStratMatchSubsystem::SubmitProductionChoice` gates the build path, because a gate on move
      / attack / wait / end-turn alone would have been a lockout with a build-shaped hole in it.
  - **THE RULES MODULE DOES NOT STOP THESE COMMANDS, WHICH IS WHY AN ENGINE GATE IS THE FIX AND
    NOT A BELT.** The 2026-08-23 log shows `STRAT-CMD accepted kind=Move unit=14 ... side=1` and
    `STRAT-CMD accepted kind=Attack unit=14 ... side=1` landing AFTER the flag fell; only `EndTurn`
    carries the `[T-SAVE-05] no match is running` check. "The rules will refuse it anyway" was
    available as an argument and is false.
  - **THE AI'S TURN NOW TERMINATES, AND THE PORT DID NOT CHANGE.** `RunAiTurnsNow` re-reads the
    view model after an `FStratAiTurnOutcome` with `bOk` false and, when the match has since
    concluded, ends its loop with an EMPTY stop reason rather than reporting a fault. It keys on
    the rebuilt model and never on the refusal's text, so a re-vendored wording cannot silently
    turn this arm off. **THE ALTERNATIVE WAS `IsMatchOver()` ON `IStratAiTurnPort` AND IT WAS
    KILLED TWICE:** it would make a runner able to decide, which that interface's own header
    forbids in as many words, and it would oblige every test double in `Tests/` — a lane this
    agent may not edit — to grow an arm before the tree would build at all.
  - **THREE COMMENTS THAT THIS CHANGE INVALIDATED WERE UPDATED IN THE SAME CHANGE**, which is the
    obligation `StratGuidedOpening.h` names: `IStratAiTurnPort`'s "decided before `RunTurn` is
    called" paragraph now carries the amendment and the measurement; `FStratAiTurnOutcome::bOk`
    records that a `false` is no longer always a fault at the caller; and
    `NoteMatchResultIfEnded`'s declaration records that its caller moved and its behaviour did not.
  - **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR. THE TREE NOW LINKS
    AND THE SUITE HAS RUN IN IT.** The two bullets that stood here reported a compiled-but-unlinked
    tree and a non-empty save slot, and both were true of the tree they described. Superseded
    rather than deleted, because the compile-only measurement is what separated "the code is wrong"
    from "the editor is open", and the next reader who hits `LNK1104` needs to see that the
    distinction was made rather than assumed.]** The editor was closed, and
    `Saved/SaveGames/StratocracyMatch.sav` was MOVED rather than deleted, on the user's decision,
    to `C:\Users\me\AppData\Local\Temp\claude\E--MultiAgent-Stratocracy\e142e851-499d-4869-9f61-e543568bbc02\scratchpad\StratocracyMatch.sav.2026-08-23-1114`. Both blockers were RE-VERIFIED HERE rather than taken on report: `tasklist` returns
    `INFO: No tasks are running which match the specified criteria.` for `UnrealEditor.exe`, with
    `explorer.exe` PID 13508 listed in the same round as the control that shows the instrument can
    speak, and `Saved/SaveGames/` enumerates zero entries.
  - **THE BUILD IS GREEN UNDER THE PLAIN DOCUMENTED COMMAND, WITH NO FLAG.** `Build.bat` with the
    documented arguments and NO `-NoHotReloadFromIDE`: `Result: Succeeded`, `REAL_EXIT=0`, all
    nineteen actions -- sixteen compiles including `Module.StratPlay.gen.cpp`, then
    `[17/19] Link [x64] UnrealEditor-StratPlay.dll`, `[18/19] Link [x64] UnrealEditor-StratPlay.lib`
    and `WriteMetadata StratocracyEditor.target`. Zero warnings and zero errors reported.
    **THE FIRST PLAIN RUN LINKED IN TWO ACTIONS OFF CACHED OBJECTS AND THAT WAS NOT ACCEPTED AS
    EVIDENCE**, because those objects were produced by the earlier `-NoHotReloadFromIDE` run. The
    four changed sources were `touch`ed to force a recompile under the plain invocation, with
    `git hash-object` taken before and after proving all four blobs BYTE-IDENTICAL across the
    touch, and the nineteen-action build above is that second run. A cached object is not a compile
    anybody watched.
  - **THE SUITE RAN IN THIS TREE, HEADLESS, WITH THE EDITOR CLOSED, AND IT IS GREEN.** Zero
    non-Success and `notRun` zero, every entry `Success`. `reportCreatedOn 2026.08.23-19.55.53` --
    UTC, which is 15:55:53 local on the same day; do not "correct" either stamp to match the other.
    **NO FIGURE IS WRITTEN HERE AND NONE MAY BE**: `global.md` owns the count, and the figure went
    to the coordinator instead. No clause was added or removed by this pass, which is correct --
    this agent writes none.
  - **THE REPORT IS THIS TREE'S OWN AND ITS IDENTITY WAS CHECKED, not assumed.** `index.json` is
    stamped 15:55:53 local against the four changed sources at 15:52:42 and the relinked
    `UnrealEditor-StratPlay.dll` at 15:54:21 -- so the report is newer than both the code it
    describes and the binary it ran against.
  - **THE SAVE-SLOT CONTROL IS DISCHARGED ON THIS RUN AND NOT BORROWED.** `Saved/SaveGames/`
    enumerated ZERO entries before and ZERO after, and the directory mtime MOVED, 15:51:20 ->
    15:55:53, the run's own minute. **NOTHING APPEARED DURING THE RUN**, which matters more than
    usual this pass: the change touches the completion-write path, and a file materialising there
    would have been a finding rather than something to tidy away.
  - **THE NEW TRANSITION EXECUTES AT RUNTIME, AND THE ABSENCE THAT FIRST LOOKED LIKE A DEFECT WAS
    THE INSTRUMENT.** `STRAT-MATCH concluded` greps to ZERO in `suite.log` -- and so does the bare
    string `LogStratPlay`, 0 hits, so that file carries no `LogStratPlay` output at all and its
    silence proves nothing about the line. The control moved the question to
    `Saved/Logs/Stratocracy.log`, which carries 2909 `LogStratPlay` lines from this run, and there
    `STRAT-MATCH concluded` fires SIX times, every one inside the run's own window, reading e.g.
    `STRAT-MATCH concluded turn=6 turnCap=20 sideToMove=1 tier=Decisive`. So
    `UStratMatchSubsystem::ConcludeMatchIfEnded` is reached by the EXISTING AI-vs-AI clauses with
    no clause written for it, and `StaticEnum<EStratResultTier>()` resolves the tier name at
    runtime rather than printing an integer.
  - **THE ONCE-PER-MATCH LATCH IS CORROBORATED AND IS NOT PROVED, and the difference is stated
    because this record has been corrected before for reaching further.** Six conclusion lines
    stand against 73 `STRAT-AI turn-ended` and 967 `STRAT-AI applied` in the same log, and a broken
    `bMatchConclusionAnnounced` would emit one line per `ApplyView` on a finished match -- orders
    of magnitude more than six. That is an argument from MAGNITUDE and not a count of how many
    matches actually concluded; nothing here enumerated them. The clause that would settle it is
    named in the handoff to `strat-test-author` and is not owed by this pass.
  - **THE TWO LATCHES ARE VISIBLY INDEPENDENT IN THAT SAME LOG**, which is the property the
    ordering inside `ConcludeMatchIfEnded` was written for: six `STRAT-MATCH concluded` against
    five `now records a completed match`. Sec 2.11.6's writer stayed gated on its own opt-in and
    its own slot while the transition ran regardless, so neither became the other's condition. And
    `Saved/SaveGames/` is still empty after all five, so none of them reached the player's slot.
  - **THE DEBT THIS ENTRY OPENED AS "NOTHING VERIFIES AT LINK OR AT RUNTIME" IS DISCHARGED HERE.**
    Its stated discharge condition was the editor closing and the build plus suite running; both
    happened, the tree links, the suite is green, and the new transition was observed executing.
    What is NOT discharged and is not claimed: no human has played a match to a result with this
    fix in the tree, so the player-facing half -- that a finished match stops accepting clicks on
    screen -- rests on the input gate's source and on clauses nobody has written yet, not on
    anything anybody has watched. Injected input never reaches `UPlayerInput` on this project, so
    that observation needs a human at the keyboard. OWNED: coordinator, to schedule with the user.
  - **A DEBT SHIPS WITH THIS AND IT IS THE ONE A VICTORY SCREEN NEEDS: NOBODY DOWNSTREAM CAN SAY
    WHO WON.** `strat::MatchResult` carries `winner`, `cause` and `decidedByKey` and lives on
    `TurnState`; `strat::UiMatchView` — the only thing `FStratMatchView` mirrors — carries
    `turn`, `turnCap`, `sideToMove`, `hasResult` and `resultTier` and NO winner. So
    `GetConcludedMatchView` can say *Decisive* and cannot say *for whom*. **DELIBERATELY NOT
    CLOSED HERE**, because both routes cost more than the defect did: either a new `FStratBridge`
    accessor over `TurnState::result` plus a new reflected struct in `StratViewModel.h`, which
    would put a field on the view model that mirrors nothing and would need its own parity clause;
    or an upstream `UiMatchView::winner`, which is a vendored, hash-gated change in
    `E:\MultiAgent\stratocracy-crew` and a re-vendor. **DISCHARGED WHEN** a victory surface is
    actually specified — until then a screen can say a match ended and its tier, which is the
    transition this pass owed.
  - **THAT DEBT IS DISCHARGED 2026-08-25, BY A ROUTE IT DID NOT CONSIDER, AND IS STAMPED IN PLACE
    RATHER THAN DELETED.** Both stated discharge conditions were met and neither named route was
    taken. (i) A victory surface IS specified: §2.11.4's end-of-match screen gives the tier, the
    three scoreboard rows in order, and a FACTION-VOICED result line -- and the faction is chosen
    by WHO WON, so the winner is load-bearing for the specified screen rather than decorative.
    (ii) The route taken is a THIRD one the filing did not weigh: upstream added
    `strat::uiMatchResult` as a fourth `ui*` QUERY, on the precedent `uiBuildOptions` set. It
    costs neither of the two prices the filing objected to -- no view-model field that mirrors
    nothing, and no move to `kUiSnapshotFieldCount` / `kUiMirrorFieldCount` /
    `kUiDerivedFieldCount` / `uiFieldContract()` / `uiEnumerateSnapshot`. `FStratBridge::
    MatchResult`, `FStratMatchResultView`, `StratBuildMatchResult` and
    `UStratMatchSubsystem::GetMatchResult` are the engine half; the clauses are
    `strat-test-author`'s and are named in the 2026-08-25 entry at the top of this file.
    **WHAT IS NOT DISCHARGED:** nothing engine-side ASSERTS a winner yet. Until T-TURN-02 /
    T-TURN-04 / T-TURN-05 clauses exist -- and until at least one of them arranges
    `winner != sideToMove` -- a wrong derivation would pass every clause that does exist.

- **`HexSize` on `AStratBoardActor`** is centre-to-centre spacing for a pointy-top layout, the
  only axial→world constant in the project; must be matched to whatever tile mesh phase 5 picks.
- **`EnhancedInput` is absent from `StratPlay.Build.cs` by design**; phase 4 adds it. Do not
  "fix" it early. — **Discharged in phase 4**: `EnhancedInput` is now Private in
  `StratPlay.Build.cs`; `StratBridge` moved Public → Private there in the same change.
- **A `bDone`/`bLockedThisTurn` producer does not exist yet.** Phase 4's selection machine must
  write those onto the built view model via `ApplyView(model)`, never into an actor.
  `T-INT-05.NoActorHoldsPresentationBits` pins the negative only. — **Discharged in phase 4**:
  `FStratSelectionMachine::DecorateViewModel`, called between `BuildViewModel` and `ApplyView`,
  writes both bits from the machine's own `TSet`s; no actor holds either.
- **`SubmitCapture` has no affordance and the machine never emits `Capture`.** The phase-4
  command list is literally move → attack, wait, end turn; which hex offers a capture and how
  the player is told is an unanswered UI question. The applier's `switch` will need one new arm.
  (Phase 4 deferral, carried forward.)
  - **RETIRED 2026-08-25: NOTHING IS OWED, AND THE LAST SENTENCE WAS WRONG.** The affordance
    question is not unanswered -- §2.11 answers it in the negative: "Capture and build need no
    extra verbs. Capture is by presence (§2.7 ... a progress pip appears, NO BUTTON)."
    `strat::AiCommandKind` is `{Build, Move, Attack, EndTurn}` for the same reason. And the
    applier's `switch` will NEVER need an arm: `FStratBridge::SubmitCapture` is §4.10
    SAVE-FORMAT TRANSPORT, kept because `strat::SaveCommandKind` is pinned at the five and
    `Source/StratBridge/Tests/StratBridgeRestoreParity.cpp` and `StratBridgeSaveRecording.cpp`
    dispatch it in log replay. The reason never to wire an affordance is stronger than "no phase
    owns it": `strat::captureTick` is NOT IDEMPOTENT -- `prog->turnsHeld += 1` per call, and
    `applyCommand`'s Capture arm builds occupants from the whole board and never reads
    `c.unitId`, so the command is not even unit-specific. N submissions in one turn advance a
    capture N turns' worth; on the shipped scenario (`captureTurns = 1`) that is masked. Stamped
    into `StratPlayerController.h`, `StratSelectionMachine.h` and `StratMatchSubsystem.h`.
    **WHAT MAKES THIS FALSIFIABLE RATHER THAN ASSERTED** is a clause nobody has written:
    `TwoCaptureCommandsInOneTurnAdvanceHoldingTwice`. Named for `strat-test-author`.
- **The hot-seat hand-over key is deliberately unbound.** `SetViewingSide` on a keypress would
  let either player see the other's board at any time; the confirmation screen is UI work no
  phase owns. (Phase 4 deferral, carried forward.)
  - **THE KEY STAYS UNBOUND AND THE SECOND CLAUSE IS RETRACTED 2026-08-25: THERE IS NO
    CONFIRMATION SCREEN AND NONE IS OWED.** The GDD specifies no hand-over screen. Measured
    against `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`: four
    case-insensitive hits for hot-seat in the whole document, all four in stretch-feature
    context (§1.5's resolved question 4 -- "2-player hotseat: in scope or cut? RESOLVED: stretch
    only, off the critical path (§2.10)" -- the §2.10 scope table's STRETCH row, and two pacing
    asides); ZERO hits for "hand-over", "handover" or "pass the device"; and §2.11.5 states the
    complete prototype screen list -- "title/menu, briefing, match, result". So "UI work no phase
    owns" described work that does not exist, and a later reader following the citation would
    have scheduled a screen nobody asked for.
    **THE HALF THAT SURVIVES IS THE WHOLE REASON THE KEY IS UNBOUND** and it needs no screen
    behind it: a key that silently flipped the viewing side would let either player see the
    other's board at any time. If hot-seat ever comes off the stretch list, whatever drives the
    swap must make it deliberate and visible; a bare keypress cannot. Stamped into
    `StratPlayerController.h`'s "NOT IN THIS ROUND" block.
    **A REAL DEFECT WAS SITTING BESIDE IT AND IS FIXED** -- the `ViewingSide` desync between this
    class and `AStratScoreboardHUD`. See the 2026-08-25 entry at the top of this file.
- **`SetLockedThisTurn` has a writer and no shipping caller** (§2.11.6 guided opening is out of
  milestone), so `bLockedThisTurn` is false in every running path. Its clause calls the setter
  itself and discloses in four places that it pins machine behaviour, not that any shipping path
  produces a lock. **When §2.11.6's producer lands it needs a clause of its own; this one will
  not cover it.** (Phase 4 deferral, carried forward.) — **DISCHARGED 2026-08-21 at `1d6f758`:**
  `FStratGuidedOpening::PublishLocks` is the first shipping caller. It writes the bit for every
  friendly unit on every observation, true and false both, rather than setting once and clearing
  once — a writer that only ever sets is a writer whose clear can be missed on one path, and for
  this bit that means a permanently undimmable unit and a player who cannot move. The lock is
  keyed on beat 1a being OUTSTANDING, not on its holding the line, so it clears mid-turn when the
  marked Infantry's move completes and not at the turn boundary. The clause this entry asked for
  exists; `tests.md` names it.
- **The attack branch's "already acted" refusal — cite the branch, not a line number, this is
  the fourth instance of line-number rot in this milestone** — is the `if (Selected->bHasActed)`
  test inside the enemy-click arm of `FStratSelectionMachine::HandleEvent`
  (`Source/StratPlay/StratSelectionMachine.cpp`), unreachable by any click sequence because
  `NotifyCommandApplied` marks an attacker DONE on the same event. Ruled an acceptable
  defensive guard, not dead code — it guards a disagreement between the model's `bHasActed`
  (rules-side) and the machine's `DoneUnits` (engine-side, per-session) that a loaded save, a
  replayed log, or phase 6's PIE can produce even though no scripted sequence in this suite can.
  **DISCHARGED in the combat-outcome milestone's phase 5:** the branch now carries a comment
  block stating exactly this — why no click sequence reaches it, what it actually guards, and
  that its cost is that no clause covers it — added by `strat-gameplay-engineer` and explicitly
  ruled correct by the reviewer ("labelling it is what stops the next reader deleting it, which
  is the failure mode the phase exists to prevent. Keep it.").
- **`ReplayRecordedLogOnto` does no save round trip** — no serialize, no parse, no
  `FStratSaveIdentity`. `SerializeRecordedSave` + `T-SAVE-06.SaveRoundTripsToEqualHash` already
  cover that path; a second entry point there would be a second policy over the same bytes.
  **Still true as written, and 2026-08-21 did not breach it:** `RestoreFromSaveText` is the READ
  side — it parses and replays §4.10 text *into* a bridge; `ReplayRecordedLogOnto` still never
  serializes. One policy over the bytes, one direction each.

---

### 2026-08-22 — the BUILD affordance for §2.11.5's production menu (input + widget host)

C++ only. No asset, no test — `WBP_ProductionMenu` and the clauses are other lanes'.

**THE BUILD IS RED AND THE CODE IS NOT THE REASON.**
`Build.bat StratocracyEditor Win64 Development -project=… -waitmutex -NoHotReloadFromIDE`
with the editor OPEN (the coordinator was driving it): **Result: Failed
(OtherCompilationError)**, 164.94 s. All 22 compile actions succeeded — including
`Module.StratUI.gen.cpp` and `Module.StratPlay.gen.cpp`, so UHT parsed every new reflected
member — and both LINK actions failed with
`LINK : fatal error LNK1104: cannot open file
'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratUI.dll'` and the same line for
`UnrealEditor-StratPlay.dll`, each preceded by `UbaSessionServer - ERROR opening file … for
write after retrying for 20 seconds (The process cannot access the file because it is being
used by another process. - …\UnrealEditor.exe)`. **A GREEN BUILD OF THIS CHANGE HAS NOT BEEN
MEASURED** and nothing below may be read as if one had; it needs one rerun with the editor
closed. **[SUPERSEDED 2026-08-22: THAT RERUN HAPPENED AND WAS GREEN.** The coordinator
closed the editor and rebuilt these exact bytes -- `Result: Succeeded`, exit 0, the two links
that had failed completing normally. For the suite run over these bytes see `global.md`'s
banner, which owns that fact class; this file does not restate it. The red
measurement above is KEPT rather than deleted because its finding is the durable half:
`-NoHotReloadFromIDE` does not cover the open-editor DLL write lock, which
`.agents/ue-project-context.md` now records. **Do not read the sentence before this bracket as a
live instruction to rebuild** -- doing so is the stale-status-line failure this project has paid
for, in its under-claiming direction. Stamped by the COORDINATOR and not by
`strat-gameplay-engineer`, whose file this is, under the same declared-deviation form as
`content.md`'s newest entry; `strat-integration-reviewer` raised the staleness as its one BLOCK
finding and named either owner as acceptable.]** For any suite figure or phase verdict see `global.md` — this file states neither.

- **`-NoHotReloadFromIDE` DOES NOT DEFEAT THE OPEN-EDITOR DLL LOCK, AND THE DISPATCH BRIEF
  SAID IT WOULD.** The memory it came from is about the Live Coding **mutex**, which is
  engine-keyed and global and which that switch does defeat — that is what lets a worktree
  build start at all while an editor is up. The **write lock on `Binaries\Win64\*.dll`** is a
  different mechanism: the running `UnrealEditor.exe` has the DLLs mapped, and no UBT switch
  unmaps them. Measured above; the two failures are 20 s apart in the same run, so the switch
  bought the compile phase and nothing after it. `.agents/ue-project-context.md` already
  states the rule correctly ("a link error naming `UnrealEditor-Strat*.dll` and 'cannot open
  for writing' means *close the editor*"); what was missing was that `-NoHotReloadFromIDE`
  is not an exemption from it.

- **THE RULING'S PREMISE WAS WRONG AND THE AFFORDANCE IS BUILT ON THE CURSOR INSTEAD.** The
  user's ruling — a dedicated action opens the menu "for the CURRENTLY SELECTED HEX" — names
  a thing this project does not have. `FStratSelectionMachine` holds `SelectedUnitId` and
  nothing spatial; its `HexPrimary` arm treats a click on empty ground with nothing selected
  as "an ordinary click and not a failure", so **a factory hex with no unit on it cannot be
  selected at all**, and one with a unit on it is not a hex you can build at. An accessor
  over the machine's state would have had to ADD a hex-selection concept to the machine —
  exactly what the ruling was shaped to avoid. `AStratPlayerController::HexUnderCursor` is
  used instead: the same source `OnSelect` already uses to decide which hex a click means.
  **The ruling's INTENT is intact and is what was implemented** — `HexPrimary`'s semantics
  are untouched, `FStratSelectionMachine` gained no BUILD arm, no
  `EStratSelectionCommand::Build` exists, and `StratSubmitSelectionCommand`'s `switch` gained
  no arm. Only the ruling's account of where the hex comes from moved.

- **THE HEX IS LATCHED AT THE KEYPRESS AND NEVER RE-READ, and the widget's own lifecycle is
  what forces that.** `AStratPlayerController::GetProductionTargetHex(FIntPoint& OutHex)`
  hands back what `ToggleProductionMenu` recorded, not a live cursor read. The asset's
  `Construct` runs INSIDE `AddToViewport`, so a live read would answer a different hex the
  moment the mouse moved off the factory between the keypress and the construct — a menu
  built for a factory the player never pointed at. Two channels rather than one because
  `FIntPoint(0, 0)` is a real hex and cannot signal its own absence, the same trap
  `IsProductionMenuOpen` records about `ProductionMenuHex`. **T-INT-05 does not reach this
  latch**: that clause forbids an actor holding `bDone` / `bLockedThisTurn`, which are fields
  OF THE VIEW MODEL and would drift if copied; this is an input intent, appears in no view
  model, and nothing on screen is drawn from it.

- **THE WIDGET IS CREATED BY `AStratScoreboardHUD` AND THE DEBT THAT FILE RECORDS GREW FROM
  SEVEN MEMBERS TO ELEVEN RATHER THAN A SECOND DEBT BEING OPENED.** The guidance block's
  argument transfers unchanged: `CreateWidget` + `AddToViewport` mean `UMG`, `Slate` and
  `SlateCore`, and `StratPlay.Build.cs` growing all three is a structural cost where widening
  the HUD is a prose one. The same condition discharges all eleven together — a §2.11 UI-layer
  owner existing. The **only** difference from the other two widgets is lifetime: this one is
  created ON DEMAND and destroyed on close, because §2.11.5's menu is about ONE factory chosen
  at the moment it is asked for, and a resident panel would need "no factory" as a drawable
  state nothing in the rules produces. An unset `ProductionMenuWidgetClass` is still reported
  once at `BeginPlay` beside the other two, so "not configured" and "failed to open" stay
  distinguishable.

- **`ProductionMenuWidgetClass` IS TYPED `TSubclassOf<UUserWidget>` AND THAT IS THE
  IRREVERSIBLE CALL BEING HELD OPEN RATHER THAN SPENT.** A Blueprint deriving from a C++ class
  bakes `/Script/Module.Class` into itself permanently. Typing this property as a base of ours
  would decide which module owns the menu widget before anybody needed to decide it — the same
  call the reflected seam declined a day earlier. **The cost, stated so the next reader does
  not assume it is free:** the HUD can call nothing on the widget beyond `UUserWidget`'s own
  surface, so it cannot refresh it. It does not want to; a caller that does must pay for a
  base class first. **The condition that discharges it:** somebody needing C++-side layout or
  a `BlueprintImplementableEvent` hook.

- **NOTHING IN C++ CALLS `RefreshMenu`, AND THE HUD COULD NOT IF IT WANTED TO.** `RefreshMenu`
  is a Blueprint custom event on the asset and the member is typed `UUserWidget`, so there is
  no C++ name to call — the constraint is structural, not a convention someone could forget.
  C++ creates and shows; the widget refreshes itself from `Construct` off
  `GetProductionTargetHex`. That keeps WHEN a menu's contents are decided in one place.
  `ToggleProductionMenu` likewise calls no `RefreshProductionMenu` and asks the rules module
  nothing at all.

- **THE ONE THING THE TOGGLE REFUSES IS A CURSOR OFF THE BOARD, AND IT IS NOT A RULES
  ANSWER.** It does not test whether the hex is a factory, whether the side holds it, or
  whether the side can pay. All three are rows on the menu with the module's own reasons
  attached, and `RefreshProductionMenu` is documented to SUCCEED on a hex that is not a build
  point — drawing a full menu of unavailable rows, which is what §2.11.5 draws. A pre-check
  here would replace the module's reason with this class's silence.

- **CLOSING IS TWO ACTS ON TWO OBJECTS IN A FIXED ORDER: panel down, then rows cleared.** The
  reverse leaves a live panel bound to an empty `ProductionMenu` array for however long the
  two lines are apart — a screen showing a menu the subsystem says is not open.
  `AStratScoreboardHUD::CloseProductionMenuWidget` deliberately does NOT reach into
  `UStratMatchSubsystem` to do the second act itself; that would be the first line of the HUD
  becoming a second thing that runs matches, which its header block forbids in those words.
  `AStratPlayerController::ToggleProductionMenu` is the one caller and does both.

- **`IsProductionMenuWidgetOpen()` READS THE WIDGET AND IS NOT A BOOL BESIDE IT**, on the rule
  `IsProductionMenuOpen` states about its own rows. A WBP that removes ITSELF — a Cancel button
  in the asset, a shape the asset is free to have — leaves the pointer non-null and the widget
  off the viewport; a cached bool would have answered true and the toggle would have needed two
  presses. It is **out of line rather than inline**, and that is a module arrow rather than a
  style choice: the body calls `UUserWidget::IsInViewport()`, and inlining it would push `UMG`
  onto every module including `StratScoreboardHUD.h`.

- **`CreateProductionMenuWidget` ASSIGNS THE MEMBER BEFORE `AddToViewport`, WHICH IS THE
  OPPOSITE ORDER TO `CreateGuidanceWidget`.** `AddToViewport` constructs the widget, so the
  asset's `Construct` runs inside that call — and this asset's `Construct` refreshes the menu,
  which can reach a Blueprint asking the HUD whether a menu is open. Assigning afterwards would
  have that question answered "no" from inside the act of opening one. The strip has no such
  graph and keeps the narrower rule.

- **TWO DEFERRAL BULLETS WERE RETRACTED IN PLACE, AND ONE HALF-BULLET WAS DELIBERATELY LEFT
  STANDING.** `AStratPlayerController.h`'s "CAPTURE and BUILD … neither has an answered
  affordance question" is retracted for BUILD only; `FStratSelectionMachine`'s "BUILD.
  §2.11.5's production menu is explicitly out of the hot-seat milestone" is retracted with the
  substance being that nothing in that struct changed. **`SubmitCapture` still has no
  affordance and no engine caller** — the standing `## NEXT` entry above is untouched, and the
  capture half of the controller's bullet stands unretracted.

### 2026-08-22 — the reflected seam for §2.11.5's production menu (`UStratMatchSubsystem`)

C++ only. No widget, no asset, no test — the WBP and the clauses are other lanes'. Build was
`Build.bat StratocracyEditor Win64 Development` with the editor confirmed closed
(`Get-Process UnrealEditor` empty): **Result: Succeeded**, 21 actions, 77.73 s, zero warnings.
For the suite figure and any phase verdict see `global.md` — this file states neither.

- **The seam went on `UStratMatchSubsystem` and not on `AStratScoreboardHUD`, and the WRITE
  half is what decided it.** The HUD was the closer prior art on the read half — it has a
  bridge, already hosts a second widget, and `RefreshScoreboard` is the exact shape
  `RefreshProductionMenu` needed. Two things killed it. First, **an accepted Build puts a unit
  on the board**, and the only thing in the project that can make the board agree with that is
  `ApplyView`, which lives on the subsystem; a submit routed through the HUD would refresh the
  scoreboard panel and leave the new unit unspawned until something else happened to reconcile
  — a screen carrying a change its view model did not. Second, **the HUD is shedding its
  bridge, not growing one**: `StratScoreboardHUD.h` states in advance that it must give up
  bridge ownership once a proper owner exists, the subsystem IS that owner, and hanging a
  MUTATING rules call off the borrower deepens the debt instead of discharging it. A
  `UBlueprintFunctionLibrary` was the third shape and is structurally impossible rather than
  merely worse: its statics would need an `FStratBridge&`, which is not a reflected type and
  must never become one, so no Blueprint graph could ever supply the argument.
- **No new `UCLASS`, and that is deliberate rather than incidental.** A WBP reaches the
  subsystem with `Get World Subsystem` and derives from nothing of ours, so no
  `/Script/Module.Class` parent is baked into an asset before anybody has decided who owns the
  menu widget — a choice that is irreversible the moment an asset makes it. **The condition
  that would discharge this into a `UStratProductionMenuWidget` base:** somebody deciding the
  menu needs C++-side layout or a `BlueprintImplementableEvent` hook, which no caller needs
  today.
- **The submit takes no hex, and that is the one guard the engine adds.** `DefIndex` is
  meaningful only as a row of the menu it was picked from, and `applyCommand` uses that field
  as a raw bounds-checked-only index with no name lookup — the same property that makes
  `DT_Units` row order load-bearing. A caller-supplied hex would let a widget pick row 2 of
  factory A and build it at factory B, and the rules module would accept it silently.
  `SubmitProductionChoice` reads the OPEN MENU's own hex, which removes the pairing rather than
  checking it.
- **On the SUBMIT path the engine refuses exactly one rules-shaped thing, and it is not a
  legality question.** (The other two arms there are not rules-shaped: no live match, and an
  AI turn in progress.) A `DefIndex` appearing in NO row of the open menu is refused locally
  — a value the module never OFFERED.
  `bAffordable` and `bAvailable` are **not read** on the submit path; whether the side can pay
  and whether the factory will take a build are the rules module's answers and the submit asks
  for them again from the authority that owns them. Nothing anywhere on this path ANDs the two
  bits, which is T-UI-03's forbidden arithmetic — they are separate answers because
  §2.11.5 shows them differently.
- **The READ path has TWO CHANNELS and this seam collapses neither — and the prose that
  shipped with this entry got the split backwards, so it is corrected here rather than
  quietly.** `FStratBridge::BuildOptions` states it in its own declaration block:
  `FStratResult` says whether the query could be ASKED; `bAvailable` and `Reason` on each row
  say what the rules ANSWERED.
  - **Riding the `FStratResult` refusal channel:** a bridge with no definitions loaded or no
    scenario seeded; a SIDE outside the match — the one malformed-question case, refused in
    the bridge rather than left to the module because `uiBuildOptions` spells "invalid side"
    the same way it spells "not a build point"; and an EMPTY SET, which is unreachable with a
    unit table loaded because the menu carries one row per §2.4 row.
  - **Riding each ROW's `bAvailable` / `Reason`:** a factory this side does not hold, one that
    has already taken its build this turn, one already holding a waiting build, and **a hex
    that is not a build point at all**. The factory hex is therefore NOT pre-checked — not in
    `FStratBridge::BuildOptions` and not in `UStratMatchSubsystem::RefreshProductionMenu` —
    the way `Forecast` leaves the defender to the module.
  **So a non-factory hex OPENS a menu and is not refused:** `IsProductionMenuOpen()` reads
  true, every row reads `bAvailable` false with the module's own reason, and §2.11.5 draws
  those reasons. A caller asking "is this hex a factory" must read the ROWS, never the return
  value. **The opposite claim — that `UStratMatchSubsystem::RefreshProductionMenu` REFUSES a
  hex that is not a factory — stood in this file and in that function's declaration block and
  was never measured;** it reached both from a handoff note by way of the dispatch brief.
  `strat-test-author` measured the tree independently, declined to write the clause asserting
  it, and wrote `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` instead.
  It is now **retracted in place** in that declaration block, and the `Bridge.Get() == nullptr`
  arm in `UStratMatchSubsystem::RefreshProductionMenu` — which drew the same contrast in other
  words, naming "that hex is not a factory" as a later refusal — is corrected beside it.
  Corrected 2026-08-22; **no behaviour changed and no signature moved** — comment text only,
  rebuilt to prove the headers still parse:
  `Build.bat StratocracyEditor Win64 Development -project=… -waitmutex` with the editor
  confirmed closed (`Get-Process UnrealEditor` empty), **Result: Succeeded**, 16 actions,
  82.52 s, zero warnings.
- **An empty array means "no menu open", and what makes that sound is one layer down.**
  `StratBuildProductionMenu` REFUSES rather than producing an empty menu, so an empty array is
  not a value any successful refresh can produce and is free to carry the marker.
  `ProductionMenuHex` could not have carried it — `FIntPoint(0, 0)` is a real hex, and this
  project has already paid once for treating a real default as an unset marker. **What would
  break it:** a rules change making an empty buildlist a legal answer; the cost if it broke is
  a refused submit, never a wrong build.
- **`false` after an accepted command is possible and is stated rather than designed away.**
  On acceptance the menu and then the screen are both rebuilt; the command is recorded and
  irreversible by then, so a rollback is not on offer. Both rebuilds are attempted before
  either is reported, and the reason is prefixed `build accepted, but …`.
  `FStratBridge::RecordedCommandCount` is the disambiguator. Same trade `SetViewingSide`
  documents about its own already-changed side.
- **The header's "NOT IN THIS ROUND — submitting commands" bullet is RETRACTED IN PLACE for
  Build and only for Build.** Its own stated condition — "a wrapper landed early is a wrapper
  written without its caller" — was met: §2.11.5's menu is the caller. Move, Attack and
  EndTurn still travel `FStratSelectionMachine` / `StratSubmitSelectionCommand` from
  `AStratPlayerController`, because that is where a CLICK ON THE BOARD belongs and a menu pick
  has no click and no selection to advance. **`SubmitCapture` still has no engine caller at
  all** — the standing `## NEXT` entry above is untouched by this change.
- **The menu is cleared in `TearDownPresentation`,** beside `AppliedModel`, because a buildlist
  describes one factory in one `strat::GameState` and a reseed and a world death both
  invalidate it.
---

### 2026-08-21 — save-slot I/O and the §2.11.6 guided opening (lane B, merged at `1d6f758`)

Built in the slot-1 worktree on `feat/save-and-guidance` off `870c611`, then rebuilt and re-run
on this tree. For the suite figure and the phase verdict see `global.md`'s banner — this file
does not restate either.

- **A restore is a replay that is verified before it is adopted.** `FStratBridge::RestoreFromSaveText`
  calls `strat::loadSave` for the header, then replays the parsed `commandLog` onto a **copy** of
  `GameState` and compares `strat::canonicalStateHash` against the save's `stateHash` before
  assigning anything. Cite the copy, not a line: the reason it is a copy is that `replayLog` is
  all-or-nothing *within the log* but the hash check sits downstream of it, and `replayLog` cannot
  roll back for a reason it never sees. Assigning `GameState`, `Recorded` and `OutCommandCount` in
  one step at the bottom is what makes the method all-or-nothing on *this* object. A mutated state
  hash is therefore REFUSED, not restored, and the refusal carries `T-SAVE-06` as its id. The
  freshness arm above it (`!Recorded.empty()`) refuses a bridge that has already applied commands,
  for `ReplayRecordedLogOnto`'s arm-4 reason.
- **`UStratSaveGame` (`Source/StratPlay/StratSaveGame.h`) carries the slot, and carries exactly
  what §4.10 cannot.** The slot overrides three config fields on load — `ScenarioFile`, `FirstSide`,
  `ViewingSide` — and nothing else. Tables and actor classes come from `ActiveConfig`, because a
  slot that pinned `DT_Units` would break the first time that asset was renamed, and the
  definitions are already checked harder by the replayed-hash comparison inside
  `RestoreFromSaveText`.
- **`StartMatchInternal` is an extraction, and the extraction is the point.** `SaveMatchToSlot`
  and `LoadMatchFromSlot` do not each own a start sequence; `LoadMatchFromSlot` runs `StartMatch`'s
  body with one optional extra step between `LoadScenarioFromFile` and the hand-over. There is
  exactly ONE `LoadDefinitions → LoadScenarioFromFile → (restore) → AdoptBridge` ordering in
  `StratMatchSubsystem.cpp`, which makes that ordering a contract rather than a habit. Two
  orderings side by side is the shape that produces two `strat::GameState`s for one map, and it
  drifts silently because both halves compile. The presentation consequence is free rather than
  arranged: the extracted body ends in `BuildViewModel → ApplyView → HandBridgeToScoreboard`, so
  `T-INT-05`'s "rebuild the screen from the view model alone" covers a loaded match without a
  second drawing path existing.
- **`FStratGuidedOpening` (`Source/StratPlay/StratGuidedOpening.h`/`.cpp`) is the guidance layer,
  held by value on the controller beside `FStratSelectionMachine` and not a `UCLASS`.** Nothing
  spawns it and nothing finds it by class; its whole visible surface reaches the screen through
  `FStratGuidanceView` on the view model. It names no `strat::` type, which is why
  `StratPlayerController.h` may include it. **It decides nothing:** every fact it branches on is a
  field read off `FStratViewModel` (`bIsGuidedMarked`, `bHasMoved`, `Turn`, `CaptureProgress`) or
  an answer `FStratBridge::GuidedOpeningHexes` gave it. In particular the marked unit comes from
  `bIsGuidedMarked`, which the rules module derives off `placement`, and is never inferred by
  comparing a hex against `guidedOpening.infantry` — that comparison would let beat 1a's own move
  unmark the unit the beat is about.
- **Beat 2 retires only on a capture pip at `guidedOpening.objective`, and that follows a ruling
  rather than a reading.** §2.11.6 is ambiguous; the 2026-08-21 user ruling narrowed
  `HasCapturePipLanded` from any pip to the ringed objective, looked up through
  `FStratBridge::GuidedOpeningHexes`. §2.11.6 forbids the alternative by name ("no 'nearest
  objective' heuristic is used"). **That makes `T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile`
  load-bearing rather than decorative:** without it a regression to a nearest-factory heuristic
  retires beat 2 on the wrong tile with a green build instead of failing visibly. `HasAnyUnitSpawned`
  is now the one remaining place that takes a broader observable than the fact it wanted, and says
  so in the open.
- **The Q27 input gates are three arms in `AStratPlayerController::HandleSelectionEvent`, and two
  of them return BEFORE the machine sees the event.** The End Turn arm and the `Wait` arm are
  pre-`HandleEvent` because `Wait` is the event that sets `bDone` without a rules command:
  intercepting it after the machine had seen it would leave the unit retired with beat 1a still
  outstanding — End Turn inert and no move left to satisfy it, which is precisely the deadlock the
  two closures exist to make unreachable. The third arm is the attack gate, and it can only sit
  after `HandleEvent` and before the submit, because a primary click on an enemy hex is `HexPrimary`
  and nothing earlier can tell it from a move; `NotifyCommandApplied` is not reached, so the DONE
  set does not move either. **All three refuse without touching `FStratBridge`** — an inert input
  is an input that was never asked about, not a command the rules module refused — and all three
  still call `RefreshFromMachine`, so the strip and the dimming show the player *why* nothing
  moved. The reason string is `FStratGuidedOpening::EndTurnGateHoverText`, so the log line, the
  hover and the strip cannot say three different things.
- **`AStratBoardActor::GetTargetOverlayCount` reads the component, not a cached number.** It
  returns `TargetOverlay->GetInstanceCount()`, so it cannot report a light the board is not
  actually showing — the failure mode a cached count has is agreeing with the code that set it
  while disagreeing with the screen. `BlueprintPure`, and deliberately not `GetDrawnHexCount`,
  which is the tile count and is the thing it is easiest to conflate it with.
- **The attack-target suppression clears the set rather than declining to compute it.**
  `RefreshFromMachine` calls `BuildOverlays` as always and then resets `TargetHexes` when
  `GuidedOpening.IsUnitInputGated` says so. Asking `FStratSelectionMachine::BuildOverlays` to know
  about beats would put a guidance rule inside a state machine that owns none. The cost is one
  `AttackTargetHexes` call whose answer the player will not see, and that is the price paid on
  purpose. The REACH overlay is left lit, because beat 1a's own directive is "Lit hexes are its
  true reach".

#### The completion writer's opt-in, 2026-08-21

- **`FStratMatchConfig::bRecordCompletionOnMatchEnd` gates the unprompted §2.11.6 write, and it
  exists because the guard it replaced protected nothing in the case it was written for.**
  `UStratMatchSubsystem::NoteMatchResultIfEnded` gated on
  `ResolveSaveSlotName(FString()).IsEmpty()` and its comment said that arm was for "an automation
  test that never configured a slot". `FStratMatchConfig::SaveSlotName` is declared
  `= TEXT("StratocracyMatch")` — the player's slot — so a caller that configured nothing resolved
  to the shipped slot, never to empty. Measured with a two-directional control: the integration
  tree ran the suite without the writer and `Saved/SaveGames/` ended with zero files; this
  worktree ran the same suite with the writer and produced `StratocracyMatch.sav`, 2096 bytes.
  The AI-vs-AI clauses in `Source/StratPlay/Tests/StratAiMatchClauses.cpp` name no slot and assert
  a result, so every suite run recorded a completed match on the player's slot and suppressed the
  guided opening permanently — the inverse of the change's purpose.
- **The root cause was a predicate, not a fixture.** Emptiness cannot distinguish `unset` from
  `chosen` when the default is non-empty, so patching one fixture would leave the trap armed for
  the next. `StratMatchLifecycle` and `StratMatchReconcile` were named latent by the gate, safe
  only because they never reach a result. The new field is a separate axis: a slot name answers
  WHERE, the flag answers WHETHER.
- **The alternative was a `TOptional<FString>` slot name and it cannot be built here.** `TOptional`
  is not a reflectable `UPROPERTY` type and `FStratMatchConfig` is `USTRUCT(BlueprintType)` reaching
  a designer through a details panel, so the unset/set distinction on the string itself would cost
  the group its editability — the one thing the struct's header block says it exists for.
- **The flag gates `NoteMatchResultIfEnded` ONLY.** `RecordMatchCompletionOnSave` and
  `SaveMatchToSlot` stay unconditional, because a caller that named a slot has already chosen.
  That partition is what keeps the two completion clauses that call the writer directly green
  while the two that reach it through the hook go red until they opt in.
- **The empty-slot early return survives, re-worded.** It is now reachable only by a deliberate
  clear — opted in with `SaveSlotName` emptied — and stays silent and unlatched for its original
  reason: `ApplyView` runs per refresh and a warning there is a warning per frame.
- **Misleading prose retracted in place** in `UStratMatchSubsystem::HasCompletedAMatchOnSave`'s
  block, which named "the slot name is empty" as the first thing to check in a "why does the strip
  keep coming back" report. That branch is unreachable without a deliberate clear. The first thing
  to check is now `bRecordCompletionOnMatchEnd` being false, which is the one a build can get wrong
  silently: false in C++, true only on the GameMode Blueprint's default.

#### Debts taken on 2026-08-21 (continued)

- **Shipping now depends on a Blueprint default that no C++ asserts.** `BP_StratGameMode`'s
  `MatchConfig.bRecordCompletionOnMatchEnd` must be true or §2.11.6's guided opening re-arms on
  every match in the shipped game — the original defect, reintroduced by an unset checkbox, with a
  green build and a green suite. This is the deliberate cost of defaulting to the safe direction:
  the failure mode moved from "corrupts the player's slot" to "forgets the player's history", and
  the second is the one §2.11.6 says to prefer. **Discharged by** a `Content/` default carrying the
  flag plus a clause that reads it off the GameMode's CDO rather than off a C++-built config — both
  outside this lane, and both filed in the handoffs for this change.

#### Debts taken on 2026-08-21

- **`UStratSaveGame::bHasCompletedAMatch` has a READER and NO WRITER, so the guided opening runs
  on every match rather than only the first.** Verified against this tree rather than taken on
  report: the only shipping read is the `return Payload != nullptr && Payload->bHasCompletedAMatch;`
  at the bottom of `UStratMatchSubsystem::HasCompletedAMatchOnSave`, and the only assignment
  anywhere in `Source/` is a test planting the value in `StratSaveSlotClauses.cpp`. `SaveMatchToSlot`
  carries the field forward by reading the existing slot before overwriting — so the debt is a
  MISSING writer and not a LOSSY one — and its declaration states in prose that it does not set the
  field and that nothing else does. **The condition that discharges it:** a caller that sets the
  field when a match reaches a result, i.e. a match-ended hook off `FStratMatchView::bHasResult`,
  which is the view model's own answer to "is this match over". It is deliberately not inferred
  inside `SaveMatchToSlot`, because a save taken on the final turn of a match the player then
  abandons is not a completed match. Until then `HasCompletedAMatchOnSave` answers false for every
  slot and guidance runs every match — the safe direction of the two, since guidance shown to a
  veteran is a strip they dismiss and guidance withheld from a first-timer is §2.11.6 silently not
  happening. The earlier note in `StratMatchSubsystem.h` that said wave B2 would write it is
  retracted in place: B2 landed the guided opening and did not, and a wave label stops being
  checkable the moment the wave closes. — **DISCHARGED 2026-08-21 on `feat/match-ended-hook`, in the
  working tree at the time of writing (the user commits, so no hash is cited here rather than a wrong
  one):** `UStratMatchSubsystem::RecordMatchCompletionOnSave` is the writer and
  `UStratMatchSubsystem::ApplyView` is the caller, through the private `NoteMatchResultIfEnded`, on
  `FStratMatchView::bHasResult`. The hook hangs off `ApplyView` and not off a command result because
  `ApplyView` is the one place every model reaches the screen through — `RefreshPresentation`,
  `AStratPlayerController::RefreshFromMachine` after decorating, and `StartMatchInternal` on a load
  all end there — and a hook on the submit path would miss §2.9's AI making the winning move, which
  is exactly how a first-session match ends. `SaveMatchToSlot` is unchanged and still carries the
  onboarding pair forward by not writing it; the new writer touches that one field and no other, so
  whichever of the two runs second keeps what the first left. The inference this debt forbade is
  still forbidden: nothing in `SaveMatchToSlot` derives the flag from match state.
- **The completion writer CREATES a slot that does not exist, so `DoesSaveSlotExist` can now answer
  true for a slot with no match in it.** Taken on deliberately: writing only into an existing slot
  would leave a first-time player who finished a match without ever pressing Save a first-time player
  forever, which is the likeliest path through a first session and therefore the path the defect
  would have survived on. The payload created that way carries an empty `SaveText`, which
  `LoadMatchFromSlot` already refuses by name ("carries no §4.10 text") rather than restoring an
  empty board — so the failure is a refusal and not a corrupt match. **Discharged by** a "Continue"
  affordance gating on a LOADABLE slot rather than on `DoesSaveSlotExist`; there is no such
  affordance in the tree today, and `DoesSaveSlotExist` has no shipping caller (its only caller is a
  test), which is why this is recorded as a hazard for the next reader rather than a live bug.
- **`LoadMatchFromSlot` tears down before it validates, so a refused load leaves NO match.** The
  sequence rebuilds; a slot that fails its header or its hash check leaves `IsMatchLive()` false
  and the caller must start a new one. Validating first would need the load checked against a
  bridge that does not exist yet — a second seeded bridge, which is the thing `UStratMatchSubsystem`
  exists to make impossible. **Discharged by** an `FStratBridge` that can be constructed, seeded
  and restored detached and then swapped in whole; `ReplayRecordedLogOnto` is already that shape on
  the write side, so the seam exists and nothing needs it yet.
- **User index 0 is hardcoded in the slot path.** §2.11 is hot-seat — two players, one machine, one
  logged-in user — and a per-side index would split one match's saves across two directories at the
  hand-over. **Discharged by** split-screen or online profiles arriving; the constant is one place.
- **The Q27 input gates are filed under `T-UI-02`, whose written invariant is the reachable-hex
  highlight.** No acceptance id was minted for them. That filing follows the precedent this tree
  already set with `ControllerTicksSoInputDispatches`, but it is a stretch and is recorded as one
  rather than papered over; the gap is filed upstream. **Discharged by** an id for input gating
  existing in the GDD. (The other reassignment in the same change is not a stretch: the
  guided-opening lookup moved from `T-SCN-02` to `T-SCN-07` because its subject is the two authored
  `guidedOpening` fields, which `Scenario.good.cpp` refuses violations of under `T-SCN-07` and under
  no other id.)
- **Two guided-opening DELIVERY defects, both closed on `feat/guidance-delivery` in the working tree
  at the time of writing (the user commits, so no hash is cited here rather than a wrong one).** The
  strip reached the screen and had never been seen to draw a directive; neither defect was in the
  beat machine or in the projection, and no clause covered either, which is how both survived a green
  suite. Cited by function and branch, never by line.
  - **Defect 1 — the session's only decorated push was dropped because the strip did not exist.**
    Measured across five fresh PIE sessions on 2026-08-21: `AStratPlayerController::BeginPlay`
    reached `UStratMatchSubsystem::ApplyView` with a decorated model before
    `AStratScoreboardHUD::BeginPlay` had run `CreateGuidanceWidget`, every time, so
    `AStratScoreboardHUD::PushGuidance`'s null check discarded it and nothing reconciles again
    without player input. **Closed by** `AStratScoreboardHUD::PushGuidance` recording every value it
    is handed in `LastPushedGuidance` / `bGuidanceEverPushed`, and `CreateGuidanceWidget` calling the
    new `AStratScoreboardHUD::DeliverLatestGuidance` after the strip is on screen. It asserts no
    `BeginPlay` order in either direction: a push before the strip is replayed, a push after it takes
    the ordinary route, and a session with no push leaves the strip on its defaults.
  - **Defect 2 — every subsystem-side reconcile cleared the strip.** Driven and watched on
    2026-08-21: a decorated refresh put a live directive on the widget, and `SetViewingSide` →
    `RefreshPresentation` → undecorated `ApplyView` → `PushGuidance` left it reading
    `bActive=False, Beat=None, DirectiveText=""` one second later. The same clearing on the first AI
    turn (`RunAiTurnsNow` → `RefreshPresentation`) is a **strong inference and not an observation** —
    taking a turn needs input, so it was never driven. **Closed at the build and not at the push**:
    `ApplyView`'s "UNCONDITIONAL, WITH NO BRANCH ON `bActive`" ruling is intact and untouched, and
    the fix is that models missing their guidance section no longer reach it.
    `UStratMatchSubsystem::BuildViewModelForPresentation` runs a registered `FStratViewDecorator`
    over every model this subsystem builds to draw from, and `AStratPlayerController::BeginPlay`
    registers `AStratPlayerController::DecorateForPresentation` as that decorator.
- **The undecorated `BuildViewModel` is still public and still has two callers that must not
  decorate.** `UStratMatchSubsystem::IsAiTurnDue` and `RunAiTurnsNow`'s loop read `sideToMove` and
  `bHasResult` off a model nobody draws; decorating there would run
  `FStratGuidedOpening::Observe` — "THE ONLY THING THAT ADVANCES A BEAT" — for a question with no
  screen behind it. The invariant to hold when editing that class is narrower than "always
  decorate": *no model this class builds reaches `ApplyView` without passing through
  `BuildViewModelForPresentation`.*
- **`AStratPlayerController::RefreshFromMachine` calls `DecorateForPresentation` DIRECTLY rather than
  going through `BuildViewModelForPresentation`, and the asymmetry is deliberate.** The registration
  is taken in `BeginPlay`, so a controller driven before or without `BeginPlay` — which is every
  fixture that spawns one into a world it built itself — would silently stop decorating, and this
  path's whole purpose would come to rest on an actor lifecycle step. It cannot double-decorate:
  `BuildViewModel` never runs the delegate and this path never calls the presentation build.
- **`FStratViewDecorator` is single, weak, and cleared BY POSITION rather than by identity.**
  `AStratPlayerController::EndPlay` calls `ClearViewDecorator()` unconditionally, so in a world where
  a SECOND controller had since registered it would drop that controller's binding. No such world
  exists this milestone — §2.11's hot seat is one controller holding two seats, and
  `FStratGuidedOpening::Begin` takes the guided side by argument for exactly that reason.
  **Discharged by** the day a second controller can exist: the seam grows an owner argument, or the
  delegate an identity check, and the `EndPlay` call passes `this`.
- **`AStratScoreboardHUD` now holds seven guidance members, not four, and one of them is a cache it
  had explicitly declined to hold.** `DeliverLatestGuidance`, `LastPushedGuidance` and
  `bGuidanceEverPushed` joined `GuidanceWidgetClass`, `GuidanceStrip`, `CreateGuidanceWidget` and
  `PushGuidance`. The header's discharge condition is unchanged and the count in it is restated
  rather than left to be recounted: when a §2.11 UI-layer owner exists, all seven move there
  unchanged. The cache does not make this class a second thing that runs matches — it never asks for
  guidance, touches no bridge and no view model, has one writer, and every later push overwrites it,
  so it changes WHEN a value is delivered and never WHICH.
- **`StratScoreboardHUD.h` now includes `StratViewModel.h`, where it forward declared
  `FStratGuidanceView`.** `LastPushedGuidance` is held by value, so the size is needed by UHT.
  Nothing about the constraint that governs that header is loosened: `StratBridge.h` remains
  forbidden there, forever, and `StratViewModel.h` is this module's own reflected header with nothing
  vendored behind it.
