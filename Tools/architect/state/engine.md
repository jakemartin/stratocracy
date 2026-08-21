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
  checkable the moment the wave closes.
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
