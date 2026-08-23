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
- **The hot-seat hand-over key is deliberately unbound.** `SetViewingSide` on a keypress would
  let either player see the other's board at any time; the confirmation screen is UI work no
  phase owns. (Phase 4 deferral, carried forward.)
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
