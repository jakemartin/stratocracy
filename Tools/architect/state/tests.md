# Stratocracy state — tests

> **Sole writer: `strat-test-author`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Clause inventory, what each clause actually pins, known flakes, and instrument caveats — the places where a test asserts rather than measures.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT

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

- **OWED CLAUSE — `Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn`. NOT WRITTEN IN THIS
  PASS, DELIBERATELY, AND IT MUST NOT EVAPORATE.** The engineer named it and it is his stated
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
