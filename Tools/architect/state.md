# Architect state

_Last run 2026-08-14 (log-backed combat outcome milestone: phase 1 CLOSED — the `STRAT-COMBAT`
emitter on `FStratBridge::Submit`, gated three times, zero findings, plus an unplanned 1-in-4 test
flake in a `GLog` capture found and fixed on source-level proof, not a probe; phase 2 CLOSED,
`VERDICT: PASS`, zero findings — six new clauses in `StratCombatOutcomeParity.cpp`, suite 87 → 93,
including the first-ever execution of the `adied=1` counter-kill arm; phase 3 CLOSED
MET-AS-CORRECTED, gated THREE TIMES: first two `VERDICT: BLOCK` (a fixture-count miscount in this
file; then this file's OWN mid-file summary line still claiming "no reviewer verdict yet" after
this same file's phase-3 entry already recorded the first `BLOCK` — the other half of the exact
mistake phase D's entry already documents paying for once), both fixed in place; third
`VERDICT: PASS`, zero findings, 2026-08-14 — the brief's `STRAT-CMD accepted` ↔ `STRAT-COMBAT
resolved` pairing is unreachable in an
AI-vs-AI match (measured 0), the real pairing is `STRAT-AI applied kind=Attack` ↔ `STRAT-COMBAT
resolved` by ordered identity, proven 68/68 with zero mismatches by
`Tools/architect/strat_combat_pairing_gate.py`, suite 93 → 103; phase 4 CLOSED MET, gated once,
`VERDICT: PASS`, zero findings, 2026-08-14 — the only phase this milestone that did not need a
re-gate — the live PIE path (`Saved/Logs/Stratocracy_2.log`, `--pre-sliced` mode added to the
gate for its first non-automation corpus), same gate PASS, 68/68 again, and all eleven
`STRAT-AI turn-ended` hashes byte-identical to phase 3's headless run — the phase-D "avoid
one-corpus proof" precedent is now PARTLY discharged: the HOST-independence half IS discharged,
the CONTENT-independence half is NOT and is carried forward as its own future work; phase 5
CLOSED (editor closed throughout), the doc pass, gated three times, all `VERDICT: PASS`, zero
findings each — 9 files, 231 insertions/23 deletions vs `HEAD` `ae2f22a`, proven comment-only
(254 changed lines, zero fail the comment-body shape test; `/*`/`*/` counts identical HEAD-vs-
worktree in all nine files), suite unmoved at 103/103, and the standing finding that
`strat::chooseBuild`'s cheapest-affordable selection makes a buildlist's repeated entries
inert — a vendored-behaviour question for `stratocracy-crew`, not a defect here. **THE
LOG-BACKED COMBAT OUTCOME MILESTONE IS NOW COMPLETE — five phases closed, no phase 6 was ever
planned for it.** See "Log-backed combat outcome milestone" below. **POST-MILESTONE, 2026-08-14,
NOT A PHASE:** `a2d370a` discharged the deferred `ESearchCase::CaseSensitive` tightening on the
T-SAVE-05 grep-contract clause and opened a narrower `STRAT-CMD refused`-shape residual in its
place; suite is now **104/104** (was 103/103 as of phase 5, above). See "Grep-contract
case-sensitivity tightening" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the
`--pre-sliced` zero-event gate debt (phase 5's item 4, carried in NEXT) is DISCHARGED —
`Tools/architect/strat_combat_pairing_gate.py` now refuses a zero-applied/zero-resolved
`--pre-sliced` corpus by default and accepts an explicit `--expect-min-pairs N` floor (any N,
including 0, as an opt-out of the default); a 14th in-script self-test case proves the guard is
falsifiable, and a new 11th checked-in fixture (`fixtures/fail_pre_sliced_zero_events.log`, empty)
demonstrates it against the actual defect. This is a Python-only change — no `IMPLEMENT_SIMPLE_
AUTOMATION_TEST` clause, so the 104/104 C++ suite count is unmoved. See "Pre-sliced zero-event
guard" below. The prior entry (AI-opponent milestone, phase D CLOSED,
COMPLETE) is preserved under "AI-opponent milestone" further down.)_

## BUILT

- `Source/StratUI/StratScoreboardHUD.h` — applied (iteration 1, step `host_h`, live)
- `Source/StratUI/StratScoreboardHUD.cpp` — applied (iteration 1, step `host_cpp`, live)
- `FStratBridge::Reachable` — landed at `e0cc53d` with zero tests; its five clauses are now
  covered (`StratBridgeQueryParity.cpp`, T-UI-02, phase 1, 2026-08-12). Debt discharged.

## DECISIONS

- iteration 1: selected `scoreboard_host` at score 35; runner-up production_widget was blocked on buildlist_query

## NEXT

- **A claim in a document is checkable against an artifact, and the artifact wins — this has now
  cost six corrections across four separate occasions in this milestone alone.** (Corrected count:
  the earlier "four separate corrections" undercounted its own tally by conflating occasions with
  corrections — the fourth occasion below is itself three miscounts across two files plus one
  propagation into this file, so six corrections total, on four occasions.) Phase D's gate-half
  produced three prose errors caught by the reviewer's gate on the first occasion: two technical
  (misreading UE 5.8's `AddExpectedMessagePlain(Occurrences=0)` semantics; overstating what the
  `:610` tripwire's substring match actually pins down), one pure bookkeeping (this file and the
  phase evidence blackboard both claiming "no reviewer verdict exists yet" a hundred-plus lines
  after a `BLOCK` verdict on the same piece was already recorded in the same file) — caught
  respectively by checking the code against the claim and by reading the document's own history. A
  fourth occasion, same class, different artifact: `pie-session/narrative.md` and
  `pie-screenshots/narrative.md` hand-transcribed `STRAT-AI applied` command counts (a turn-3 span
  and count, a turn-5 Build/Move/Attack breakdown, a session-2 Move count sourced from "the brief"
  rather than the log printed three lines above the claim) that a `grep -c` against the checked-in
  slice — the artifact sitting in the same directory — falsifies; propagated once into `state.md`
  before the reviewer's re-gate caught it (2026-08-13, `pie-session`/`pie-screenshots`
  `narrative.md` BLOCK findings) — three miscounts plus the one propagation, six corrections in
  total across the four occasions. Where a narrative states a count a one-line command counts
  exactly, derive it and name the command, rather than reading it off a log by eye or trusting a
  dispatching agent's number over the artifact in hand — this applies to counts, to "has a verdict
  run yet" bookkeeping, and to slice boundaries alike; a document is never its own authority over
  the tree or the log it describes.
- **Slice a growing log by content markers, never by EOF.** "To EOF" of a file a running editor is
  still appending to means "to whenever the cut happened to run," not a fixed boundary, and a
  line-number range into a rotating log is a fragile name even when stable at cut time (a
  neighbouring session's content can ride along under a boundary chosen for a different reason).
  Bound evidence slices by a session's own open/shutdown line, a turn boundary, or a result line;
  name the file for what it *is*, not for where it sat in the log when cut. Applied to both PIE
  session slices in `evidence/07-ai-opponent/` (`pie-session/session1-completed-game.log`,
  `pie-screenshots/session2-screenshot-run.log`); see `evidence/07-ai-opponent/blackboard.md:267-275`
  for the original derivation.
- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)
- **Hot-seat milestone is COMPLETE (phase 6 closed 2026-08-13); see "Hot-seat milestone —
  COMPLETE" below.** The AI-opponent milestone is now current (phase C closed 2026-08-13; phase D
  — PIE playtest and a machine-repeatable AI-vs-AI gate — is next; see "## AI-opponent milestone"
  below). Remaining out of scope, unchanged: production menu (§2.11.5), guided opening (§2.11.6),
  info panel, toasts, save-slot UI, move-undo.
- **Phase D's acceptance must not be written against seeing a Tank built.** `strat::chooseBuild`
  (`Ai.good.cpp:275-289`) always buys the cheapest *affordable* buildlist entry; with Infantry
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
  is the user's call). Suite **69/69**, `succeeded 69 / succeededWithWarnings 0 / failed 0 /
  notRun 0`, `reportCreatedOn 2026.08.13-16.47.36` (`Saved/AutomationReport/index.json`, read
  `utf-8-sig`); both entries `state: "Success"`. All 67 pre-existing tests still pass, none
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
  not cover it.** (Phase 4 deferral, carried forward.)
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
- **`ETriggerEvent::Started` on all four input actions is asserted, not measured** — no input
  asset exists yet to test against. Discharged the first time the controller runs in PIE with
  real assets, i.e. no earlier than phase 5.
- **`Saved/AutomationReport/index.json` is UTF-8-with-BOM, not UTF-16.** First bytes are
  `EF BB BF`; all three UTF-16 codecs fail to parse it and `utf-8-sig` succeeds. Two separate
  gate passes reported UTF-16 for this file and were both wrong. **A phase-6 gate that
  hardcodes UTF-16 to parse the report will read zero tests and may not say so** — the same
  failure shape as the bare-`Stratocracy.uproject` test command that exits in ~1s having run
  nothing. Recorded here so a phase-6 reader hits it before writing that parser.
- **CORRECTED, 2026-08-14 — this bullet's own claim had itself gone stale and was repeated
  uncritically.** It previously asserted "`.agents/ue-project-context.md:195` reads '51/51,
  hot-seat phase 3'", true only at phase 4. The line has been rewritten by later phases since; a
  direct read on 2026-08-14, at the start of the combat-outcome milestone, found it instead reading
  "Last observed here: 86/86, AI-opponent phase B, 2026-08-13" — not the phase-4 text this bullet
  claimed — and that number was itself carried into a dispatch prompt before being checked, the
  same class of error this bullet exists to warn against. The `EnhancedInput` module-arrow gap this
  bullet also raised has not been independently re-checked and is left as an open question for
  `.agents/`'s owner. That file remains outside this steward's lane
  (`.agents/` is not `Config/` or `Tools/architect/`); the line-195 count is now current — see the
  new milestone's header above and `.agents/ue-project-context.md:195` itself, which as of
  2026-08-14 reads "93/93, combat-outcome phase 2, 2026-08-14" (updated by the user, not this
  steward).
- **Phase 6 risk, and the most consequential open item: the `STRAT-CMD` click-to-command gate
  is unclosed.** No `playtest_key` or `playtest_click` produced any `LogStratPlay` output.
  **Correction to this record, made in phase 6:** the original claim here — "total input
  silence, reproduced on Epic's own shipped TopDown template with known-good assets" — had a
  confounded control and is **not established**. The "TopDown template" the control actually
  drove was `Lvl_FerrumCrossing` launched under the TopDown template's GameMode (a consequence
  of the stale-config bug found in phase 6 — see below), and `Lvl_FerrumCrossing`'s `Floor`
  actor had been deleted in phase 5 (recorded above, "The template's `Floor` actor was deleted
  from `Lvl_FerrumCrossing`"). A click on that map hits nothing under the cursor, so
  `CharMoveComp.Velocity` legitimately stayed zero for a reason having nothing to do with
  whether input reaches the game. `Escape` still ending PIE is real and unaffected by this
  correction. The underlying question — does simulated input reach `StratPlayerController` at
  all — is **still open**, restated accurately with new, still-confounded-a-different-way
  measurements under Phase 6 below. Phase 4's standing debt — "`ETriggerEvent::Started` on all
  four input actions is asserted, not measured" — remains open, and phase 6's
  `assert_log_contains` gate still depends on solving it. **Do not action the phase-6 escalation
  that asks to repoint `Lvl_FerrumCrossing`'s GameMode or `GlobalDefaultGameMode` again** — see
  Phase 6 below: the running editor was serving a stale, pre-flip config, and the on-disk
  `Config/DefaultEngine.ini` needs no change.
- **`AWorldSettings` is unreachable through the NeoStack Lua API**, so no level's GameMode
  override can be authored that way. Exact failures: `configure("actor","WorldSettings")`,
  `select_actor`, `get_actor_properties`, `open_asset` on the sub-object path, and
  `invoke({actor_label="WorldSettings"})` all failed; `configure` accepts only
  `actor|landscape`. (Worked around this phase for the `Config` question by reading the two
  levels' binary `.umap` bytes directly instead — see Phase 5 below.)
- **`array_count("DefaultKeyMappings.Mappings")` → `property not found`** — dot-path into a
  struct's array is unsupported; write the whole struct instead. **Generalises to struct members,
  not just arrays** — phase C (AI-opponent milestone) hit the same failure on
  `get`/`set("self", "MatchConfig.AiSides")`, a plain (non-array) struct field, and had to read
  and rewrite the whole `MatchConfig` struct. Whole-struct read/write is the standing workaround
  for both cases.
- **`FKey` `ImportText` takes the bare name.** `Key=(KeyName="X")` silently produced `Key=()`;
  `Key=LeftMouseButton` succeeded. A silent empty-key write is exactly the kind of failure that
  reads as an input bug later.
- Three findings for `strat-gameplay-engineer`, none blocking: (1) `AStratCameraPawn` sets
  `Arm->bInheritYaw = false` at `StratCameraPawn.cpp:59`, pinning the camera to world yaw 0 so
  spawn rotation cannot turn the view — measured, `PlayerStart` yaw `-90` and `0` give
  pixel-identical framing; Ferrum Crossing's long axis (2800×1385, long axis on world X) is
  stuck on the screen's short axis. An `ArmYaw` property or `bInheritYaw = true` fixes it; the
  builder compensated with `DefaultArmLength = 3600`. (2) `AStratBoardActor` has
  `TerrainMeshes` but no `TerrainMaterials` map, which is the sole reason there are seven
  near-identical `SM_HexTile_*` assets instead of one mesh and seven materials. (3)
  `MI_Terrain_Water.Color` reads back `(0.02, 0.09, 0.30)` yet renders pale blue, and darkening
  every terrain colour ~55% produced no visible change — the harvested `M_Mono` does not
  respond linearly to `Color`; a purpose-built opaque `M_StratTile` would remove the guesswork.
- **Scope note:** `BP_StratCamera` is a sixth Blueprint beyond the four the brief named. The
  builder judged it inside "the Blueprints" because `AStratCameraPawn`'s C++ default
  `DefaultArmLength = 1200` shows about a third of the board. It holds no logic — four float
  defaults only. Recorded so a gate can evaluate the judgement rather than rediscover it.
- **The map→GameMode binding is a single unguarded line.**
  `Config/DefaultEngine.ini:4` (`GlobalDefaultGameMode`) is now the *only*
  thing binding `Lvl_FerrumCrossing` to `BP_StratGameMode_C`, because
  `AWorldSettings` is unreachable through the NeoStack Lua API and the level
  carries no World Settings override — unlike `Lvl_TopDown`, which does (see
  Phase 5's `Config` flip evidence above). This fact is not recorded in
  `.agents/ue-project-context.md`, which is where a future config edit would
  most plausibly be checked against. Owed to a future doc pass. **No crew
  agent owns `.agents/ue-project-context.md`** — this is a flag, not a task
  the steward takes.
- **Four items carried out of the log-backed combat outcome milestone's phase 5 (now CLOSED,
  milestone COMPLETE) — none owned by this steward, none built in that doc pass by design:**
  1. `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` — pin that
     `FStratSelectionMachine` refuses an enemy click when the selected unit has
     `bHasActed == true` while the machine's own `DoneUnits` is empty (a machine built against a
     match already in progress). Proposed by `strat-gameplay-engineer`, correctly not written in
     a doc pass. Owner: `strat-test-author`.
  2. **DISCHARGED, `a2d370a`, 2026-08-14 — see "Grep-contract case-sensitivity tightening"
     below.** Tighten `Source/StratPlay/Tests/StratHotSeatReplayParity.cpp`'s shape `TestTrue`
     (`Line.StartsWith(TEXT("STRAT-CMD accepted "))`) to `ESearchCase::CaseSensitive`. Residual
     as originally recorded: a case-only change to the `STRAT-CMD accepted` emitter would slip
     past today — confirmed by re-reading the assertion directly; it currently relies on a
     whole-tree case-variant census (also re-confirmed this phase, exactly two lowercase hits,
     both illustrative, in the test files themselves) rather than on its own shape. Deliberately
     not done in phase 5: retightening an assertion is a code change, not a doc change. Owner: a
     code phase. **A new, narrower residual on the `refused` shape replaces this one — see NEXT
     below and the new section.**
  3. A line-number citation living inside a `TestEqual` message string in
     `Source/StratPlay/Tests/StratSelectionWaitClauses.cpp`
     (`StratSelectionMachine.cpp:156-160`). Currently accurate, but unreachable by a doc pass
     because it sits inside an assertion string rather than a comment. Same owner class as item 2.
  4. **DISCHARGED, 2026-08-14 — see "Pre-sliced zero-event guard" below.** `--pre-sliced` gate
     debt on `Tools/architect/strat_combat_pairing_gate.py`, confirmed untouched by phase 5
     (`git diff --stat ae2f22a -- Tools/architect/` empty at the time). It returned `PASS` and
     exit 0 on an empty or wrong pre-sliced corpus. Shipped both halves of the owed shape — a
     structural default refusal on a zero-event `--pre-sliced` corpus, AND a caller-supplied
     `--expect-min-pairs N` floor, the latter opting out of the former when passed explicitly
     (including `N=0`) — plus a 14th self-test case proving the guard can itself fail.
  Also unchanged and still open, carried again: the content-independence half of the phase-D
  "avoid one-corpus proof" precedent (a different scenario/buildlist/first side, genuinely
  different game content) — the host-independence half was discharged in this milestone's
  phase 4.
- **New residual, opened `a2d370a`, 2026-08-14 — replaces discharged item 2 above: a case-only
  change to the `STRAT-CMD refused` shape is caught by nothing.** Cannot be closed with an
  expected-message gate — verified against `UE_5.8\Engine\Source\Runtime\Core\Public\Misc\
  AutomationTest.h`: `FAutomationExpectedMessage::Matches` uses `FString::Contains` at its
  `IgnoreCase` default; `EAutomationExpectedMessageFlags::Exact` adds only a length equality,
  and a case variant has identical length; both constructors build the pattern with
  `ERegexPatternFlags::CaseInsensitive` hardcoded, not parameterized. Closing it needs a clause
  asserting the `refused` spelling the way `T-SAVE-05.GrepContractRejectsACaseVariant` now
  asserts `accepted`. Stated at measured width only: ICU (which backs `FRegexPattern`) honours
  inline mode modifiers such as `(?-i)` in general, but whether that route is actually usable
  here was **not established either way** — the bundled ICU ships `include` and `lib` only, no
  regex compiler source, so the test-author correctly declined to assert it in either direction.
  Owner: `strat-test-author`.

## Hot-seat milestone

### Phase 0 — row order proven, tests green

- **Completed:** 2026-08-12
- **Exit criterion:** "row order proven; 18/18 tests green; git status clean"
- **Met.** Units and terrain close the criterion by different mechanisms, and
  that asymmetry is deliberate, not a gap:
  - **Units — proven directly.**
    `Stratocracy.StratBridge.GATE-BRIDGE-DEFS.MappedDefsMatchLoaderOrder`
    (`Source/StratBridge/Tests/StratBridgeParity.cpp:166-231`) compares
    `FStratBridge::UnitDefs()[i]` (built from the live `.uasset` via
    `UDataTable::GetRowNames()`, `StratBridge.cpp:53-76`) against
    `strat::loadUnits` over the same vendored `units.csv`, positionally, field
    by field. This exists *because* `SaveCommandKind::Build` carries a raw
    `defIndex` in its `unitId` field (`Save.h:64`) that `applyCommand` uses as
    a direct array index with no name lookup (`Replay.good.cpp:483-487`,
    `t.units->size()` bounds check) — an order divergence there resolves the
    same log to a different unit type, silently. Passed 18/18 at HEAD
    `0a7296e` (report `Saved/AutomationReport/index.json`, reportCreatedOn
    2026.08.12-13.55.04).
  - **Terrain — ruled not load-bearing, on evidence, not a gap.** I checked a
    proposed transitive proof through T-INT-02 (Bridge.StateHash() vs the
    fixture's stateHash) and it does not hold. `Bridge.StateHash()` calls
    `strat::canonicalStateHash(GameState)`, defined in `Replay.good.cpp:224`
    over `canonicalStateBytes` (`Replay.good.cpp:149-222`) — a different
    function from `Driver.good.cpp:485`'s `stateHash(Session)`, which
    `Save.h:12-16` explicitly disclaims: *"It is NOT the `stateHash` in
    Driver.h, which is the debug driver's own digest (GATE-DRV-06) and a
    different thing."* `canonicalStateBytes` folds in turn/side, per-side fame,
    objective hex+owner, per-unit id/side/hex/hp/flags, capture progress,
    `builtThisTurn`, and pending-build hex/side/defIndex — **no terrain field
    or terrain index appears anywhere in it.** Nor is there a transitive path:
    scenario seeding resolves each hex's terrain by NAME, scanning
    `t.terrain` for `.id` match (`Replay.good.cpp:299-308`, mirrored in
    `Driver.good.cpp:336-343`), so the semantic properties attached to a hex
    (moveCost, defensePct, capturable, …) are correct regardless of the
    table's row order in that build — unlike units, no `SaveCommand` field
    carries a raw terrain-table index the way Build's `unitId` carries
    `defIndex` (`Save.h:59-68` lists all five command kinds' fields: `unitId`,
    `hex`, nothing terrain-shaped). So a `DT_Terrain` row-order divergence
    changes no semantic value, reaches no hash byte, and is not merely
    untested — it is untestable via T-INT-02 by construction, because the
    format never externalizes a terrain index the way it externalizes
    `defIndex`. `T-DATA-05.TerrainTableMatchesCsv`'s name-keyed `FindRow`
    comparison is therefore the correct and sufficient check for terrain; no
    GATE-BRIDGE-DEFS-shaped terrain test is needed. Suite is 18/18 and git
    status is clean apart from this steward's own edits.

### Phase 1

- **Completed:** 2026-08-12
- **Exit criterion:** "`FStratBridge` gains `Forecast`, the five `Submit*` façade
  methods, `RecordedLog`, `SerializeRecordedSave`; build green; parity tests
  green for every one of those methods including `Reachable`".
- **Met.** Evidence:
  - Engineer added, additions only (349 insertions, 0 deletions, confirmed by
    the gate against `HEAD` `b0a55c5`), to `Source/StratBridge/StratBridge.h`/
    `.cpp`: `FStratSaveIdentity`, `SubmitMove`, `SubmitAttack`, `SubmitBuild`,
    `SubmitCapture`, `SubmitEndTurn`, `RecordedLog`, `SerializeRecordedSave`,
    `Forecast`, private `SubmitStamped` + `Recorded`.
  - The five command kinds are `Move, Attack, Build, Capture, EndTurn`
    (`Source/StratRules/Save.h:54`) — **there is no `Wait`**. The orchestration
    brief said `Wait` and was wrong; recorded here so the next reader does not
    re-introduce it. `Wait` is a UI-level concept the save format cannot
    carry.
  - Suite **31/31** (was 18; +13), report
    `Saved/AutomationReport/index.json`, `reportCreatedOn
    2026.08.12-16.31.54`, `succeeded 31 / failed 0 / notRun 0`.
  - `Reachable`'s five clauses are now green in
    `Source/StratBridge/Tests/StratBridgeQueryParity.cpp`, closing the
    zero-coverage debt from `e0cc53d`: `T-UI-02.MatchesModuleQuery`
    (`:245-311`), `.NotHexDistance` (`:336-474`), `.RefusesUnknownUnit`
    (`:490-551`), `.SuccessIsNeverEmpty` (`:564-621`), `.RefusesUnseeded`
    (`:637-704`). `NotHexDistance` measured **122 divergent hexes across 10 of
    10 units**, from three causes: occupancy (a friendly unit blocks a
    distance-1 hex), impassable Water at moveCost 0, and weighted
    Woods=2/Mountains=3 cost.
  - Plus `T-UI-01.*` (3 clauses) and `T-SAVE-06.*` (5 clauses) in
    `StratBridgeQueryParity.cpp` / `StratBridgeSaveRecording.cpp`.

### Phase 2

- **Completed:** 2026-08-12
- **Exit criterion:** "`StratViewModel.h/.cpp` in `StratUI`; `AdoptBridge` +
  `SetViewingSide` on `AStratScoreboardHUD`; build green; all existing tests
  still green".
- **Met.** Evidence, all verified by the gate in source rather than taken
  from builder reports:
  - `Source/StratUI/StratViewModel.h`/`.cpp` — a fully reflected view model
    that **names no `strat::` type at all**: `FIntPoint` hexes (X=q, Y=r),
    `FName` terrain/unit ids, `EStratResultTier`. `StratViewModel.h` includes
    only `CoreMinimal.h` and its own `.generated.h`, and forward-declares
    `class FStratBridge`. Built by one `STRATUI_API` free function
    `StratBuildViewModel(const FStratBridge&, int32 ViewingSide,
    FStratViewModel&, FString&)`, deliberately a free function so it is
    assertable with no actor and no Slate in existence.
  - `AdoptBridge`, `SetViewingSide`, and `GetViewingSide` on
    `AStratScoreboardHUD`. **Ownership is represented by which of two members
    is non-null, with no `bOwns` bool**: `TPimplPtr<FStratBridge> OwnedBridge`
    (freed in `EndPlay`) vs raw `FStratBridge* AdoptedBridge` (**never freed
    here**; `EndPlay` nulls it and destroys nothing). `AdoptBridge` does
    `OwnedBridge.Reset(); AdoptedBridge = &InBridge;` in that order, so no
    interval exists with both set. `BeginPlay` still calls `SeedBridge` when
    `AdoptedBridge == nullptr`, keeping the owned path alive for the
    pre-existing tests.
  - Suite **44/44** (was 31; +13), report `Saved/AutomationReport/index.json`,
    `reportCreatedOn 2026.08.12-18.06.16`, `succeeded 44 / failed 0 / notRun
    0`, every entry `Success`. All 31 pre-existing tests present and passing.
  - New clauses: six `T-INT-05.*` on the builder
    (`Source/StratUI/Tests/StratViewModelParity.cpp`) and seven `T-UI-03.*` on
    the HUD seam (`Source/StratUI/Tests/StratScoreboardHUDSeam.cpp`).
- **Two contracts phase 3 inherits — recorded here explicitly, they are the
  reason the phase was worth gating:**
  - **`AdoptBridge` refuses an unseeded bridge.** So
    `UStratMatchSubsystem` **must seed before handing over**. That ordering
    is now a contract, not a convention.
  - **`FStratUnitView::bDone` and `bLockedThisTurn` are declared and left
    false, with no producer.** `Ui.h:186-197` puts the presentation block in
    the view model precisely so T-INT-05 can rebuild from the model alone.
    Phase 3's selection machine must write them **onto the built model, never
    keep its own copy in an actor** — if it does, T-INT-05 silently becomes
    false with no compiler diagnostic. `T-INT-05.PresentationBitsAreDefaulted`
    pins the negative; nothing can pin the positive obligation until the
    owner exists.

### Phase 3

- **Completed:** 2026-08-12
- **Exit criterion:** "`StratPlay` module created and registered in
  `Stratocracy.uproject`; board actor, unit actor, match subsystem, camera
  pawn, game mode; build green; hex-layout round-trip and view-reconcile
  tests green."
- **Met.** Gate passed with **zero findings**. Evidence:
  - New module `StratPlay`, registered in `Stratocracy.uproject` (+5 lines) as
    `Runtime`/`Default`, **and** carrying `IMPLEMENT_MODULE(FDefaultModuleImpl,
    StratPlay)` at `StratPlay.cpp:14`. The gate verified both, specifically
    because `Source/StratRules/` is the local precedent for a module with a
    `Build.cs` and no `.uproject` entry — copying that shape would have
    produced a green build and a module that never loads.
  - Classes: `AStratBoardActor : AActor`, `AStratUnitActor : AActor`,
    `UStratMatchSubsystem : UWorldSubsystem`, `AStratCameraPawn : APawn`,
    `AStratGameMode : AGameModeBase`. Reflected structs `FStratTerrainLayer`,
    `FStratMatchConfig`.
  - Suite **51/51** (was 44; +7), `Saved/AutomationReport/index.json`,
    `reportCreatedOn 2026.08.12-19.11.44`, `succeeded 51 / failed 0 / notRun
    0 / succeededWithWarnings 0`. All 44 baseline tests still plain
    `Success`, none downgraded. Read from the report by the gate, not taken
    from a builder.
  - The seven new clauses, by full name:
    `Stratocracy.StratPlay.T-UI-02.BoardHexRoundTrip`,
    `.T-UI-02.ReachOverlayIsNotComputedHere` (both
    `Source/StratPlay/Tests/StratBoardPicking.cpp`),
    `.T-INT-05.ApplyViewSpawnsMovesAndDestroys`,
    `.T-INT-05.NoActorHoldsPresentationBits` (both
    `Tests/StratMatchReconcile.cpp`), `.T-INT-05.SubsystemSeedsBeforeHandover`,
    `.T-INT-05.StartMatchRefusalLeavesNoBridge`,
    `.T-UI-03.SetViewingSideMutatesNoState` (all
    `Tests/StratMatchLifecycle.cpp`).

  Decisions worth preserving because they foreclose alternatives:
  - **Tile HISM components are created from data, keyed by `FName`
    `TerrainId`, not seven `CreateDefaultSubobject` calls.** The
    declared-in-C++ alternative was rejected because it makes
    `StratBoardActor` a second author of the §4.8 terrain table, and because
    phase 0 ruled `DT_Terrain` row order *not* load-bearing — so a positional
    array would draw Water as Woods after a table edit, silently. Cost: tiles
    do not appear individually in the Blueprint component tree; phase 5
    styles them through the keyed `TerrainMeshes` map.
  - **There is deliberately no `HexAtWorldLocation`.** Picking is an
    instance-index lookup only. A rounding inverse of `WorldLocationOfHex`
    would disagree with the forward map at tile edges, and disagreeing about
    which hex the cursor is on is how a player attacks the wrong unit.
  - **`UWorldSubsystem`, not GameInstance and not the GameMode.** GameInstance
    outlives the map while the seeded state is per-map; the GameMode is an
    actor, and putting `strat::GameState` on an actor reinstates the §4.1
    debt this phase discharges. The GameMode holds configuration only.
  - **`AGameModeBase`, not `AGameMode`** — `AGameMode`'s `MatchState` machine
    would be a second, engine-side answer to "is the match over" beside
    `strat::UiMatchView`.
  - **`StartMatch` is all-or-nothing on the rules side, deliberately not on
    the presentation side.** A failed load resets the bridge to null so
    `GetBridge()` can never hand out a half-loaded one; a missing tile mesh
    returns `false` with the match live. `T-INT-05.StartMatchRefusalLeavesNoBridge`
    pins the first across four refusal paths — including the one where
    `LoadDefinitions` succeeded and a half-loaded bridge genuinely existed.

  **The measured link fact — the phase's most transferable finding:**
  `StratPlay.Build.cs` must name `StratBridge` **directly**; relying on
  `StratUI`'s public `StratBridge` dependency produced 4 × `LNK2019`. The
  missing symbols were exactly the out-of-line `STRATBRIDGE_API` ones —
  `FStratBridge::LoadDefinitions`, `LoadScenarioFromFile`, the constructor,
  and the destructor via `TPimplPtr`'s `DeleterFunc<FStratBridge>` — while
  `IsSeeded()` and `GetBridge()`, being header-inline, never appeared. **A
  smaller caller would not have surfaced this.** A transitive public
  dependency propagates include paths but **not the import library**. This is
  categorically **not** the `strat::`-across-a-module error measured 8× on
  this project: those symbols arrive *unadorned* in `namespace strat`,
  whereas these arrived `__declspec(dllimport)`, proving the header and the
  `_API` macro both resolved.

  Deferred, recorded because a positive obligation is now owed to phase 4:
  - **`HexSize` on `AStratBoardActor`** is centre-to-centre spacing for a
    pointy-top layout and is the only axial→world constant in the project.
    It must be matched to whatever tile mesh phase 5 picks.
  - **`EnhancedInput` is absent from `StratPlay.Build.cs` by design**; phase 4
    adds it. Anyone finding it missing should not "fix" it early.
  - **A `bDone`/`bLockedThisTurn` producer does not exist yet.** Phase 4's
    selection machine must write those onto the built view model via the
    `ApplyView(model)` seam, **never** into an actor. Nothing can pin that
    positive obligation until the owner exists;
    `T-INT-05.NoActorHoldsPresentationBits` currently pins the negative.
  - **Deferred a second time, and say so explicitly:** proving
    `AStratScoreboardHUD` does not *also* allocate an owned bridge needs a
    production-side reader (`bool OwnsItsBridge() const`) or the allocation
    counter offered and declined in phase 2. `T-INT-05.SubsystemSeedsBeforeHandover`
    proves the *consequence* — the HUD reads the subsystem's bridge, that
    bridge is not the one it allocated, and an `EndTurn` on the subsystem's
    bridge moves the HUD's `StateHash` and lengthens its log — but not the
    member. This is the second phase running in which the direct check was
    offered and declined as production surface beyond the exit criterion.
    Record that it has now been deferred twice.

  Two non-gating observations from the gate, for a later phase and not this
  milestone:
  - `StratPlay.Build.cs:94` lists `StratBridge` under
    `PublicDependencyModuleNames`; nothing in the module's headers names
    `FStratBridge` beyond a forward declaration, so `Private` would be the
    tighter statement and would stop the arrow being re-exported to phase 4's
    dependents.
  - `AStratBoardActor` contains **three** copies of the axial→world
    expressions (`WorldLocationOfHex`, `ApplyHexes` at
    `StratBoardActor.cpp:205-210`, `FillOverlay` at `:295-300`), differing
    only in Z, while its header block claims to be "the only axial → world
    conversion in the project". They cannot disagree today because they read
    the same `HexSize`, but the prose is stronger than the code. Follow-up
    for `strat-gameplay-engineer`.

### Phase 4

- **Completed:** 2026-08-12
- **Exit criterion:** "PlayerController with Enhanced Input, the selection
  state machine as a plain testable struct, the `STRAT-CMD` log line; build
  green; hot-seat replay-parity test green."
- **Met, but only after a `BLOCK` on the first gate.** The re-gate cause is
  the most transferable finding in this phase — record why it failed before
  what landed.

  **The first gate — `VERDICT: BLOCK`, three findings.**
  - **Finding 1 — a split clause left the recording joint unpinned.** The
    chain the criterion needs is *clicks → outcomes → submissions →
    `RecordedLog()` → replay → equal hash*. Two clauses covered the ends;
    **the joint "that `StratSubmitSelectionCommand` calls a recording entry
    point at all" was pinned by nothing.** The StratPlay clause was
    self-referential — it drove both bridges through the same submission
    function, so any path, recording or not, yielded equal hashes, and its
    count assertion counted `STRAT-CMD accepted` lines emitted by that same
    function, making it both subject and witness. Routing submission through
    a non-recording apply path would have left both clauses green with
    `RecordedLog()` empty after a full hot-seat session — exactly what
    phase 6's PIE gate leans on. Closed by two new engine-typed methods,
    `int32 RecordedCommandCount() const` and
    `FStratResult ReplayRecordedLogOnto(FStratBridge& Fresh) const`, and
    repointing the gating clause onto them. **The general lesson: a clause
    whose expectation is produced by the code under test pins nothing, and
    splitting one clause across two modules can drop the joint between them.**
  - **Finding 2 — `EStratSelectionCommand::Attack` was executed by no test.**
    All four references were inside `StratSelectionMachine.cpp`;
    `SubmitAttackAtHex` had zero test callers. A q/r transposition at
    `StratBridge.cpp:739-742` would have refused every attack in the game
    with the suite green at 62/62. Move was implicitly protected because its
    destination is read out of `ReachableHexes` inside a clause asserting
    acceptance; Attack had no such clause. The conversion turned out to be
    **correct** — `Hex.h:11-14` declares `q` then `r` — but correct-and-untested
    was still a `BLOCK`. Closed by three clauses; the transposition check
    takes its coordinate from the vendored fixture's own `strat::Hex{3,4}`
    (entry 17) rather than from `AttackTargetHexes`, **specifically so a
    double transposition cannot cancel**, asserts `q != r` first, and submits
    the transposed `{4,3}` as a foil observed refused.
  - **Finding 3** was prose stating `strat::saveCommandName` is "file-local"
    when it has external linkage and merely carries no `_API` macro — the
    conclusion held, the stated reason did not, and a wrong reason tells the
    next reader no pin is possible anywhere.

  **What landed, once the re-gate passed with zero findings:**
  - `Source/StratPlay/StratSelectionMachine.h`/`.cpp`,
    `StratPlayerController.h`/`.cpp` (new); `Source/StratBridge/StratBridge.h`/
    `.cpp`, `StratPlay.Build.cs`, `StratGameMode.h`/`.cpp`,
    `StratBoardActor.h`/`.cpp` (modified). No new module —
    `Stratocracy.uproject` unchanged, confirmed by the gate rather than
    assumed.
  - `FStratSelectionMachine` is a **plain non-reflected struct**, drivable
    with no actor, no world, no PIE, no Slate. `AStratPlayerController` holds
    it by value as a non-`UPROPERTY`.
  - Enhanced Input: five `EditDefaultsOnly TObjectPtr` properties
    (`SelectionMappingContext` + `SelectAction`, `CancelAction`,
    `WaitAction`, `EndTurnAction`), **null by default and null-safe at every
    use site**, because phase 5 authors the assets. `EnhancedInput` added to
    `StratPlay.Build.cs` (Private); `StratBridge` moved Public → Private
    there.
  - The `STRAT-CMD` line:
    `STRAT-CMD accepted kind=%s unit=%d hex=%d,%d turn=%d side=%d hash=%s`,
    sole call site `StratSubmitSelectionCommand`. `kind` spells
    `Move`/`Attack`/`EndTurn` to match the save format's own words so a gate
    can compare a line to a `commandLog` entry with no translation table.
    `unit`/`hex` are **always numeric**, carrying `-1`/`-1,-1` where the kind
    has no such field, so the line's shape never depends on its content.
    `turn`/`side` are read **before** submission, `hash` is `StateHash()`
    **after**. Refusals log `STRAT-CMD refused …` — a deliberately different
    phrase, so `grep "STRAT-CMD accepted"` counts only commands that applied.
  - Suite **66/66** (was 51; +15), `succeeded 66 / failed 0 / notRun 0`,
    `reportCreatedOn 2026.08.12-21.47.20`, read from the report by the gate.

  Decisions that foreclose alternatives — recorded with their reasons:
  - **`StratPlay` still names no `strat::` type; the translation went into
    the bridge.** Six new engine-typed `STRATBRIDGE_API` methods: `Turn`,
    `SideToMove`, `ReachableHexes`, `AttackTargetHexes`, `SubmitMoveToHex`,
    `SubmitAttackAtHex`. The rejected alternative was a `strat::Hex`-naming
    helper inside a `StratPlay` `.cpp` — legal by the letter of the linker
    rule, since naming is not calling — killed because it would put a second
    spelling of the axial coordinate in the one module that is supposed to
    have none.
  - **`AttackTargetHexes` is an enumeration, not a range check.** `Ui.h`
    declares no target-enumeration counterpart to `uiReachable`, so it walks
    the live unit list and asks `uiForecast` per enemy, keeping the module's
    `legal`. No distance is compared and no `UnitDef` range is read. If such
    a function is ever vendored, the body becomes a forward and no caller
    changes.
  - **The machine holds no mirror of the rules state.** `bHasMoved`/
    `bHasActed` are read off the view model every call. The rejected
    alternative — optimistic state advanced and rolled back on refusal — was
    killed because the rollback path is the one no test exercises. This is
    why a refused command cannot desynchronise the machine.
  - **The view model is rebuilt on every event, not read from
    `GetViewModel()`** ("a record, not a source"); otherwise what a click
    means would depend on when `ApplyView` last ran.
  - **`bDone`/`bLockedThisTurn` reach the model through
    `FStratSelectionMachine::DecorateViewModel`**, called between
    `BuildViewModel` and `ApplyView` — the seam phase 3 left owing. The bits
    live in the machine's `TSet`s; no actor holds them, neither is a
    `UPROPERTY` on an actor. `bDone` is set on a Wait and on an accepted
    Attack, **never** derived from `bHasMoved && bHasActed`.
  - **`GameMode` sets `PlayerControllerClass` but still not `HUDClass`** — a
    bare controller with null input assets is inert and says so once; a bare
    scoreboard HUD refuses every refresh and reads as a bridge bug.
  - **An empty recorded log is a refusal, not a successful no-op**
    (`ReplayRecordedLogOnto` arm 5). A deliberate departure from
    `RecordedLog()`'s "empty is an ordinary answer" posture, because as a
    *replay input* empty returns success and equal hashes and proves
    nothing. Scenario identity is compared by `strat::scenarioHash` over the
    seeded bytes, not by the `scenarioId` label a file chose.
  - Both phase 3 non-gating findings were taken and confirmed mechanical:
    `StratBridge` → `PrivateDependencyModuleNames`, and the three
    axial→world copies in `AStratBoardActor` collapsed into one private
    `LocalLocationOfHex`.

  **T-UI-02 foil weaker than phase 1's, by construction, and the gate
  verified the argument rather than waving it through:** 69 divergent hexes
  across 5 of 5 active units (occupancy, impassable Water, weighted
  Woods/Mountains) vs phase 1's 122 across 10 of 10, because `StratPlay`
  cannot see a unit's `move` and so the disc radius is the largest distance
  the real reachable set itself contains. Every reachable hex is reached in
  ≤ `move` steps and axial distance never exceeds step count, so the
  phase-4 disc is a strict subset of the classic one — conservative, the
  only direction a self-computed value may lean.

  Two corrections to carry forward as measurements — the numbered
  UTF-8-with-BOM and `.agents/ue-project-context.md:195` staleness items
  live under NEXT above, next to the other deferred debt.

**Phases 0-5 are closed**, including the `Config` map-defaults flip (see Phase
5 above). Phase 6 is next; its `assert_log_contains` gate depends on closing
the unresolved click-to-command input gate recorded under NEXT — that item is
this milestone's one standing blocker. Out-of-scope list unchanged:
production menu (§2.11.5), guided opening (§2.11.6), info panel, toasts,
save-slot UI, AI opponent, move-undo.

Note for phase 5's builder, from the engineer: the five input assets must be
authored and set on a `BP_` subclass of `AStratPlayerController`, all
`UInputAction`s Digital/bool; the GameMode Blueprint's `PlayerControllerClass`
must point at that `BP_` subclass (the C++ default is the floor, not the
answer); and **tile meshes must be traceable on `ECC_Visibility` or picking
returns nothing.**

### Phase 5

- **Completed:** 2026-08-12
- **Exit criterion:** "hex mesh + terrain material instances, Input Mapping
  Context + Input Actions, the Blueprints and Widget Blueprints,
  `Lvl_FerrumCrossing`, with `Config` map defaults flipped last."
- **Met**, with one item carried forward unresolved (the click-to-command
  input gate — see NEXT) that is explicitly outside this exit criterion's
  wording. Evidence:
  - Assets: 13 material instances under `/Game/StratArt/Materials/`, 9 hex
    meshes under `/Game/StratArt/Meshes/`, 5 Enhanced Input assets under
    `/Game/StratInput/`, 5 Blueprints under `/Game/StratPlay/`, and
    `/Game/StratMaps/Lvl_FerrumCrossing`.
  - `HexSize = 200.0`, derived from `SM_Tile_Hex`'s across-flats width along X
    (2 × 100.0). The collision hulls read out as an exact regular hexagonal
    prism — vertices `(±99.9998, 0)`, `(0, ±115.4694)`,
    `(±99.9998, ±57.7347)` — not a bounding approximation. Neighbours abut
    with zero overlap.
  - `SM_Hex` was rejected for tiles because its collision is a single box of
    half-extent `(87.68, 101.24)` which at a 151.9 row pitch overlaps
    neighbours by ~50 units, producing coplanar ambiguous hits and wrong-hex
    picking near row edges. Used only as `OverlayMesh`, where the C++
    disables collision.
  - The template's `Floor` actor was deleted from `Lvl_FerrumCrossing`. It
    sits at `Z=0`, coplanar with the tile tops, and would have contested
    every `GetHitResultUnderCursor(ECC_Visibility)` — the one asset-side way
    picking could have silently failed.
  - Terrain keys `Plains, Woods, Mountains, Water, Town, Bridge, Factory`
    read from the `Id` column of `Data/terrain.csv`; unit keys
    `Infantry, Tank, Artillery, Recon` from `Data/units.csv`.
  - PIE confirms the live match: `LogStratPlay: Match live: seeded from
    '.../Data/ferrum_crossing.json' (first side 0), drawn for side 0, 99
    hexes and 10 units on screen.` `BP_StratPlayerController_C_0`
    possessing, mapping context added, no missing-context warning.
  - `IMC_Selection.DefaultKeyMappings`: `IA_Select`→`LeftMouseButton`;
    `IA_Cancel`→`RightMouseButton`, `Escape`; `IA_Wait`→`W`;
    `IA_EndTurn`→`Enter`, `SpaceBar`. Storage location verified against all
    11 pre-existing IMCs in the project rather than guessed.
  - `Lvl_TopDown`, `BP_TopDownGameMode` untouched by the builder.

  **The `Config` flip — measured before flipping, not taken from the builder
  or the brief.** The builder's escalation argued `GlobalDefaultGameMode` is
  load-bearing for `Lvl_TopDown`'s existing scoreboard evidence, because it
  could not author a World Settings GameMode override on `Lvl_FerrumCrossing`
  (`AWorldSettings` unreachable through NeoStack — see NEXT) and inferred
  `Lvl_TopDown` must therefore also rely on the global default, making a flip
  unsafe. That inference rested on an untested premise, which I checked by
  measurement rather than accepting or overriding on argument alone:
  - **`Lvl_TopDown` already carries its own World Settings GameMode
    Override, as a hard package reference.** Extracted printable ASCII
    strings directly from the binary `Content/TopDown/Lvl_TopDown.umap`:
    it contains `/Game/TopDown/Blueprints/BP_TopDownGameMode`,
    `BP_TopDownGameMode_C`, and the literal property name
    `DefaultGameMode`. `DefaultGameMode` is `AWorldSettings`'s own override
    property — confirmed against the engine header,
    `WorldSettings.h:631-634`: `UPROPERTY(EditAnywhere, BlueprintReadOnly,
    Category=GameMode, meta=(DisplayName="GameMode Override"))
    TSubclassOf<class AGameModeBase> DefaultGameMode;`. The same extraction
    against `Content/StratMaps/Lvl_FerrumCrossing.umap` finds `WorldSettings`
    / `WorldSettings1` (the actor exists) but **no GameMode string of any
    kind** — consistent with the builder's report that it could not write
    one. A level's own World Settings override always takes precedence over
    the config global, so flipping `GlobalDefaultGameMode` cannot strip
    `Lvl_TopDown`'s GameMode; the dilemma the builder raised does not apply
    to it.
  - **The existing scoreboard/match PIE evidence was never produced via the
    global default in the first place.** `Saved/Logs/Stratocracy.log:4911`:
    `LogGlobalStatus: UEngine::Browse Started Browse:
    "/Game/StratMaps/Lvl_FerrumCrossing?game=/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C"`
    — an explicit `?game=` override, followed at `:4926` by `LogLoad: Game
    class is 'BP_StratGameMode_C'` and at `:4930` by `LogStratPlay: Match
    live: seeded from '.../ferrum_crossing.json' ...`. The reproduction path
    names its map and GameMode explicitly and does not read
    `GlobalDefaultGameMode` at all.
  - Both checks say the flip is safe on evidence, not inference, so **all
    three lines were flipped together** — see Files changed below. No
    outstanding `Config` item remains from this job.

  Decisions worth preserving because they foreclose alternatives:
  - **`GameDefaultMap`/`EditorStartupMap`/`GlobalDefaultGameMode` were
    treated as one decision, not three**, once both measurements confirmed
    no conflict — the task brief allowed treating the GameMode line
    independently if a real conflict existed; none did, so splitting them
    would have been unmotivated caution.
  - **A binary `.umap` cannot be diffed for row order or struct content**,
    but a targeted ASCII-string extraction over its raw bytes is a legitimate
    measurement for "does this package reference symbol X at all" — it does
    not require the editor. Used here in place of the escalation the brief
    anticipated; recorded so a future steward reaches for it before assuming
    "binary, therefore unknowable" for this narrower class of question.

  **Committed as `d310aa1`** ("Phase 5: the global default was never
  load-bearing, and the floor would have eaten every click"), 35 files
  changed, pushed to `origin/master`. Parent `ed27d5a` (phase 4).

  **It took two gate passes — the block is worth recording, same reasoning
  as phase 4's.**
  - **First gate: `VERDICT: BLOCK`, one finding.** All eleven substantive
    checks passed; the block was the index. 28 asset files were staged
    (`A`/`AM`) while `Config/DefaultEngine.ini` was unstaged and all five
    `Content/StratPlay/` Blueprints were **untracked** (`??`, no ignore rule —
    simply never added). A commit from that index would have landed
    `Lvl_FerrumCrossing` referencing five Blueprints absent from the
    repository, and without the `GlobalDefaultGameMode` line that is the
    level's only map→GameMode binding. Owner was **the user**, not a crew
    agent, per the "agents do not commit" non-negotiable.
  - **A second, narrower trap found while clearing it, worth recording as a
    measurement:** most assets showed `AM` — staged, then modified again by
    the editor afterwards. Committing in that state captures **stale asset
    bytes** while the working tree holds newer ones: a commit that is a
    snapshot of a state nobody ever ran. Resolved by re-running `git add -A`
    to collapse every `AM` → `A` before committing. Likely cause is Unreal's
    Git revision-control plugin auto-staging assets on save — meaning **the
    index goes stale again the moment the editor touches a file.** Expect
    this on every asset phase, i.e. phase 6.
  - **Re-gate: `VERDICT: PASS`, zero findings**, audited against `d310aa1`
    rather than the working tree. The stale-bytes risk was confirmed clean:
    `git diff HEAD` empty, `git lfs fsck` OK, and four asset blobs
    (`BP_StratGameMode.uasset`, `Lvl_FerrumCrossing.umap`,
    `SM_HexTile_Plains.uasset`, `IMC_Selection.uasset`) hashing identically on
    both sides. The committed snapshot is the state measured working in PIE.
  - The gate also confirmed `.agents/ue-project-context.md` shows **no**
    drift from the tree at HEAD — module table (lines 37-41), the
    `StratRules`-deliberately-absent note (lines 60-64), and the vendored-path
    row (line 173, `rulesCommit cb8e12b`) all match measurement. Nothing in
    `d310aa1` invalidated it. Recorded so the existing staleness flag for that
    file (phase 4, `:195`) is not over-read as still-live everywhere.

**Phase 6 mid-flight checkpoint, 2026-08-12, by the phase-6 builder.** Folded under
"Phase 6 — CLOSED" below, whose gate is the phase's actual authority; this record is
kept for its measurements, not for its status line.

- **Status (2026-08-12, SUPERSEDED — Phase 6 closed 2026-08-13, `VERDICT: PASS`; see
  "Phase 6 — CLOSED" below):** ~~in progress, not closed. No gate has run.~~ This was a
  checkpoint so a fresh session could resume without re-measuring.
- **Exit criterion (as of 2026-08-12, not yet met at that time; MET by
  2026-08-13 close — see "Phase 6 — CLOSED" below):** the `assert_log_contains`
  clause over `STRAT-CMD` closing the click-to-command input gate, plus
  on-disk evidence in `Tools/architect/evidence/`. Neither existed yet — see
  "Not yet produced" below.

  **Finding 1 — the running editor was serving a stale config; the on-disk
  `Config/DefaultEngine.ini` needed no change.** `Config/DefaultEngine.ini:4`
  reads `GlobalDefaultGameMode=/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C`,
  flipped in `d310aa1` (phase 5). The running `UnrealEditor` process
  (PID 34576) started **2026-08-12 18:11:07**; `DefaultEngine.ini` was last
  written **2026-08-12 19:01:37**. The editor read its config at startup and
  held the **pre-flip** `GlobalDefaultGameMode` in memory for the rest of that
  process's life. Consequence, measured: every `playtest_start({map=
  '/Game/StratMaps/Lvl_FerrumCrossing'})` in that process launched the map
  under the **TopDown template GameMode** — the PIE world contained
  `BP_TopDownController_C_0`/`BP_TopDownCharacter_C_1`, no
  `AStratPlayerController`, no `AStratCameraPawn`, no `AStratBoardActor`, and
  `playtest_observe` showed pure sky. **`Lvl_FerrumCrossing` does not need a
  World Settings GameMode Override and `GlobalDefaultGameMode` does not need
  repointing** — the phase-6 builder's gating escalation asked for one of
  those two, and the ask rested on the premise that the on-disk config is
  wrong. Measurement refutes that premise. **The fix is to restart the
  editor**, not to touch `Config/`. Generalised: **a long-lived editor
  silently serves the config it started with**, so any `Config/` change
  measured only on disk is unproven until a process that started after the
  write reads it. Phase 5's own PIE evidence never exercised the global
  default in the first place — `Saved/Logs/Stratocracy.log:4911` used an
  explicit `?game=` override — which is why the staleness went unseen until
  now.

  **Finding 2 — correction owed to phase 5's own record.** See the corrected
  NEXT bullet above ("Phase 6 risk, and the most consequential open item").
  Phase 5's "total input silence, reproduced on Epic's own shipped TopDown
  template with known-good assets" claim had a confounded control: the
  "TopDown template" it actually drove was `Lvl_FerrumCrossing` under the
  TopDown GameMode (a consequence of Finding 1's stale-config bug), and that
  map's `Floor` actor had already been deleted in phase 5, so a click hit
  nothing and `CharMoveComp.Velocity` legitimately stayed zero. That specific
  claim is retracted; the underlying input question is not — see Finding 3.

  **Finding 3 — superseded below.** The original text here reported the
  input question as "still open, and itself confounded," pending a clean-PIE
  re-test after an editor restart. That re-test has since run (full editor
  restart, standalone-window PIE via a human-pressed Play button) and the
  question is **not** resolved — it is narrower and more precisely located.
  See "The symptom, stated precisely" and "Ruled out" below, which correct
  this finding in place rather than being appended beside it.

  **Ruled out, each with its measurement:**
  - **Stale editor config** — fixed (Finding 1 stands). Cold PIE now comes up
    on `BP_StratGameMode_C` with the match seeded (`99 hexes and 10 units on
    screen`). Not the input cause.
  - **`WBP_Scoreboard` blocking mouse input** — root `VerticalBox_35` and all
    five panels are `SelfHitTestInvisible`; only ten small top-left
    TextBlocks are `Visible`. `DesiredFocusWidget = none`.
  - **Editor-process state / input-device→local-player mapping** — a **full
    editor restart** was tried (fresh process, fresh `LocalPlayer`, PIE via
    the Play button, standalone window) and a real user click on a cyan unit
    still produced no feedback and no `STRAT-CMD`.
  - **UE 5.8 input-mode filtering** — ruled out **by measurement**.
    `UEnhancedInputDeveloperSettings`: `bEnableInputModeFiltering=True`,
    `DefaultInputMode=(GameplayTags=((TagName="EnhancedInput.Modes.Default")))`,
    `DefaultMappingContextInputModeQuery` with
    `QueryTokenStream=(0,1,2,1,0)` decoding to **`AllTagsMatch{EnhancedInput.
    Modes.Default}`** — which matches the player's mode exactly.
    `ShouldFilterMappingByInputMode()`/`GetInputModeQuery()` have exactly
    **one** call site in the whole plugin,
    `EnhancedInputSubsystemInterface.cpp:978-986`, inside
    `RebuildControlMappings` — **not** at evaluation time. A PIE restart with
    `LogEnhancedInput` verified at `VeryVerbose` produced **zero**
    `"Not applying mappings from IMC"` lines while other
    `LogEnhancedInput: Verbose:` lines from the same startup printed
    normally.
  - **`UGameViewportClient::IgnoreInput()`** — ruled out by enumerating all
    four `SetIgnoreInput` call sites engine-wide; only
    `FInputModeUIOnly::ApplyInputMode` (`PlayerController.cpp:6384`) and an
    editor-module SceneState file set `true`, and `SetInputMode` appears
    **zero** times in `Source/` with zero matching nodes in any of the seven
    Strat Blueprint/Widget graphs.
  - **Input-gating calls in the project** — grep across all of `Source/` for
    `SetInputMode`, `FInputModeUIOnly/GameAndUI/GameOnly`, `DisableInput`,
    `EnableInput`, `bBlockInput`, `SetIgnoreMoveInput/LookInput`,
    `Push/PopInputComponent`, `SetPause`: **zero hits**.

  **Established correct, and should not be re-measured:**
  - All six key→action mappings are live on the player:
    `GetAll EnhancedPlayerInput EnhancedActionMappings` shows
    `BP_StratPlayerController_C_0.EnhancedPlayerInput_0.EnhancedActionMappings`
    = `IA_Select`←`LeftMouseButton`, `IA_Cancel`←`RightMouseButton`/`Escape`,
    `IA_Wait`←`W`, `IA_EndTurn`←`Enter`/`SpaceBar`.
  - `IMC_Selection` applied at `Priority: 0`; `Input Mode:
    EnhancedInput.Modes.Default`; `STATE Playing`; controller possessing
    `BP_StratCamera_C_0`; `DefaultPlayerInputClass`/
    `DefaultInputComponentClass` are `EnhancedPlayerInput`/
    `EnhancedInputComponent` in config **and** live on the actor.
  - The four `IA_*` assets are `ValueType=Boolean`, empty
    `Triggers`/`Modifiers`, `bConsumeInput=True`.

  **The symptom, stated precisely.** In a **standalone PIE window** (verified
  1286×760, title `Stratocracy Preview [NetMode: Standalone 0]`, not the
  2538×1352 editor frame), two independent devices were held for ~50 s each —
  `W` and the left mouse button — and **every** action stayed
  `None - 0.000s (false)` on the `showdebug enhancedinput` overlay, with
  `LogInput`, `LogEnhancedInput` and `LogStratPlay` silent at verified
  `VeryVerbose`. `STRAT-CMD` has **never** appeared in any log file. The
  break is upstream of `UPlayerInput`, in `UGameViewportClient::InputKey`'s
  dispatch to the local player.

  **Premises retired — recorded as corrections, they cost this phase hours:**
  1. **"The console opens, therefore keys reach the viewport and the
     game."** False as an exclusion. `GameViewportClient.cpp:767-770`: when
     `IgnoreInput()` is true the console still receives every key while the
     PlayerController receives none. The observation never had exclusionary
     power.
  2. **"`consumed=true`/`consumed=false` indicates delivery."** It does not.
     `consumed` means some Slate widget claimed the event; it was measured
     unstable between sessions for the same key.
  3. **"Simulated input bypasses Slate focus."** False for keys — Slate
     routes keys to the focused widget, so `playtest_key` returns
     `consumed=false` in an unfocused viewport and does not escape the focus
     variable.
  4. **This section's own earlier "total input silence reproduced on
     known-good assets"** — already retracted in Finding 2; its control was
     confounded.
  5. **The `showdebug enhancedinput` overlay proves the mapping context is
     applied, NOT that any handler is bound.** It is populated by the
     subsystem, independent of `BindAction`.

  **New observations, recorded as deferrals — not this milestone:**
  - **`AStratPlayerController::SetupInputComponent`
    (`StratPlayerController.cpp:123-142`) guards each `BindAction` with
    `if (Action != nullptr)` and logs nothing on the null branch.** A null
    action binds silently; only the mapping context has a warning. So "the
    four action defaults are correct" is **un-witnessed** — the log cannot
    distinguish four bindings from zero. Same half-pinned shape as the
    phase-4 findings. Owner: `strat-gameplay-engineer`.
  - **`AStratPlayerController::OnSelect` returns silently on a trace miss**
    (`StratPlayerController.cpp:198-204`), making "input never arrived" and
    "picking failed" indistinguishable in the log. This unobservability cost
    six sessions of ambiguity. Owner: `strat-gameplay-engineer`.
  - **`LogUIActionRouter: Error: Using CommonUI without a
    CommonGameViewportClient derived game viewport client. CommonUI Input
    routing will not function correctly.`** appears in every session, with
    `Found 0 derived classes`. CommonUI is a template leftover — no
    `CommonUI`/`UCommon`/`CommonActivatableWidget` match anywhere in
    `Source/`, and the three `CommonUI.*` cvars in `DefaultGame.ini:10-12`
    are leftovers too. Probably inert, but it is an `Error` on the input
    path that nobody has investigated.
  - **`LogStratPlay: Verbose: BP_StratPlayerController_C_0 could not paint an
    initial screen …: scoreboard refresh refused: there is no scoreboard
    widget to refresh`** alongside `LogStratUI: Scoreboard live on an
    adopted bridge` — the HUD has a scoreboard widget, the PlayerController
    does not. This is the resolution of the phase-6 "Finding 5" unchased
    observation below: not a screenshot-compositing artifact but a real
    widget-ownership mismatch, still unchased for cause.
  - **The screenshot pipeline excludes the UI layer**: `playtest_observe`
    and plain `HighResShot` do not composite the canvas layer; only
    `Shot showui` does. **No playtest may use a screenshot to prove a widget
    drew.** `Shot showui` captures the whole editor window in docked
    sessions and needs cropping for evidence.
  - **`playtest_start` overrides the editor's Play dropdown** and forces
    in-viewport PIE, silently ignoring `PlayMode_InEditorFloating`. Only a
    human-pressed Play button produces a standalone window. This is why the
    focus confound could not be removed by any agent-initiated session.
  - **`Escape` is the PIE-stop key** in the preview window — sending it via
    `playtest_key` ends the session.
  - **NeoStack `execute_script` discards Lua `return` values**; `print()` is
    the only output channel. `read_log('output', …)` reports the
    live-locked `Stratocracy.log` as empty while `Grep`/`Read` read it fine.
    `playtest_key` rejects `BackQuote`; `Tilde` is accepted.

  **Not yet produced:** no `assert_log_contains` clause, no on-disk evidence
  in `Tools/architect/evidence/`, and no `STRAT-CMD` line. The blocker is
  unresolved and is the milestone's one standing item.

  **Status line.** The decided next step is a **C++ diagnostic probe** —
  `UE_LOG` in an `InputKey` override plus `GetGameViewport()->IgnoreInput()`
  logged from `BeginPlay` — which is `strat-gameplay-engineer`'s lane and
  requires the editor **closed** for a build, reversing phase 6's
  editor-open invariant. That decision is the user's and has not been taken.
  Keep the scope fence: none of the deferrals above enters this milestone.

  **Update — the probe ran, found the cause, and has been stripped.**

  **The cause and fix, verified against source.**
  `AStratPlayerController` had `PrimaryActorTick.bCanEverTick = false`.
  `APlayerController::TickActor` → `TickPlayerInput` →
  `UPlayerInput::ProcessInputStack` is the *only* evaluator of Enhanced Input
  trigger state machines — the one place bound `BindAction` delegates fire.
  With the tick off, every key was still received and buffered at
  `InputKey`, every `BindAction` call still ran and logged no warning, the
  mapping context was still applied, and the console — handled upstream at
  the viewport-client layer, never through `ProcessInputStack` — still
  worked. Nothing observable pointed at the tick flag. Now `true`, set
  explicitly in the constructor rather than left to
  `APlayerController`'s own default, with the reasoning written in place
  (`Source/StratPlay/StratPlayerController.cpp:37-70`) specifically so a
  future reader who re-derives "this controller polls nothing, so it need
  not tick" — the exact reasoning that produced the bug in phase 4 — hits
  the correction before re-introducing it. **The general mechanism, not just
  the line: a `UCLASS` on the input path that disables its own tick still
  receives input and dispatches none of it, and the failure presents as a
  green build with every binding correctly configured.**
  Confirmed by reading the constructor and `SetupInputComponent` directly
  (`StratPlayerController.cpp:37-200`) rather than taken from a report.

  **The diagnostic probe is fully stripped.** `grep "STRAT-PROBE"` across
  `Source/` returns one hit, and it is inside the constructor's explanatory
  comment, not code; no `InputKey` override exists anywhere under
  `Source/StratPlay/`. The four `else` Warning branches on
  `SetupInputComponent`'s null `BindAction` guards (`StratPlayerController.cpp:165-199`)
  are confirmed **permanent** in the source's own comment block
  (`:147-154`, "THIS PART IS PERMANENT... not to be stripped with them") and
  stay.

  **Acceptance ID — confirmed, not reassigned.**
  `strat-test-author`'s `Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches`
  (`Source/StratPlay/Tests/StratPlayerControllerTick.cpp`) is **confirmed**
  over `strat-gameplay-engineer`'s proposed `T-PLAY-01`, on the same two
  grounds the test's own header states, independently re-verified here:
  `grep -c "T-PLAY"` against the GDD returns **0** (own measurement, not
  copied), and the GDD's actual `T-UI-02`
  (`Stratocracy_Prototype_GDD.md:2505`, "the reachable-hex highlight
  displays exactly the T-MOVE-01 set") is the acceptance ID StratPlay's
  existing click-to-hex-to-reach chain already uses
  (`BoardHexRoundTrip`, `ReachOverlayIsNotComputedHere`,
  `SelectionMachineUsesTheQueryNotDistance`) — the controller's tick is the
  first link in that same chain, since no click reaches the highlight
  without it. No test in `Source/StratPlay/Tests/` mints an ID, and minting
  one is not this steward's lane either; `T-UI-02` is the correct reuse.

  **Suite count, read from the report, not asserted.**
  `Saved/AutomationReport/index.json`: `reportCreatedOn 2026.08.13-15.05.30`,
  `succeeded 66 / failed 0 / notRun 0`. `ControllerTicksSoInputDispatches`
  does **not** appear in that report's test list — confirmed by direct
  search of the JSON — so the suite stands at **66/66 with the 67th clause
  compiled but not yet run**, matching the phase-6 status exactly.
  `strat-test-author` re-runs it after this steward.

  **Two further clauses owed, not built** (per the engineer's proposal,
  recorded here as owed rather than authored — writing tests is
  `strat-test-author`'s lane, not this one's):
  - `T-INT-05.WaitIsDistinguishableFromAttack` — pin that a wait yields
    `Command == None` while an accepted attack yields `Command == Attack`
    with unit and hex, read off the outcome struct (no `UE_LOG` can satisfy
    this; it is a struct-field expectation).
  - `T-INT-05.WaitWithNothingSelectedIsANoOp`.

  **Harness facts, recorded for the next agent to hit them before
  re-discovering them:**
  - `playtest_log_marker`'s `since=` filter is off by one — the marker's own
    line is excluded from what it returns.
  - The in-game console cycles **small → full → closed**, and a full-screen
    console silently swallows every keystroke.
  - `playtest_start` forces in-viewport PIE and ignores the Play dropdown;
    only a human-pressed Play button produces a standalone window.
  - `Escape` is the PIE-stop key in the preview window.

  Phase 6 evidence assembled under
  `Tools/architect/evidence/06-hotseat-playtest/` — see that directory's
  own `blackboard.md` for the full account, including a correction this
  steward made to the task's own framing (measured **13** side flips across
  the 27 `STRAT-CMD accepted` lines, not the stated 12).

### Phase 6 — CLOSED

- **Completed:** 2026-08-13. `strat-integration-reviewer` returned
  `VERDICT: PASS`, zero findings, all twelve standing constraints clean, and
  the probe strip verified complete (no `InputKey` override, no `BeginPlay`
  dump, no live `STRAT-PROBE` emitter; the single occurrence is inside an
  explanatory comment at `StratPlayerController.cpp:56`). The tick fix and
  all four `else` Warnings survived the strip. Nothing staged or committed;
  `HEAD` stayed `d310aa1` throughout — staging/committing is the user's call.
- **Exit criterion met:** "PIE playtest screenshots plus every
  `assert_log_contains` passing; full suite green; evidence assembled under
  `Tools/architect/evidence/`."
  - Suite **67/67**, `succeeded 67 / succeededWithWarnings 0 / failed 0 /
    notRun 0`, `reportCreatedOn 2026.08.13-15.29.04`, with
    `Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches` present
    **by name** and `Success` (it had compiled but not yet run as of the
    2026.08.13-15.05.30 report cited above).
  - Eight assertions pass, re-derived independently by the reviewer from the
    preserved log (`md5 baa918eade6b1061e9346c47a3b82d54`,
    `grep -c "STRAT-CMD accepted"` = 27, `refused` = 0, `Selection:` = 6).
  - Evidence under `Tools/architect/evidence/06-hotseat-playtest/`.
- **A false positive struck, not carried forward — the reviewer's reasoning
  is the keeper, not this steward's original flag.** This record previously
  flagged `ScreenShot00041.png`'s `Destroyed` row rendering `50` for side 1
  as a probable scoreboard defect, reasoning from a false premise: that the
  row is a unit count capped at 5. **It is not a count.** `Destroyed` binds
  `strat::UiSideView::fameCombat` — Fame from kills, GDD §2.11.4 — per
  `Source/StratUI/StratScoreboardWidget.cpp:214` (tooltip: *"Fame from
  kills. Factory income does not count at the cap."*), pinned by
  `Stratocracy.StratUI.T-UI-03.DestroyedBindsCombatFame`
  (`Source/StratUI/Tests/StratScoreboardParity.cpp:406-424`, including
  `TestFalse(… "Destroyed shows a bare value, not X of N")`). So
  `Destroyed 0 / 50` means side 1 held 50 combat Fame and side 0 held none —
  **independent corroboration of the user's otherwise-unlogged "a red unit
  won the encounter,"** not a contradiction of it. The misreading itself is
  the lesson: reading a bare-value row as a bounded count is what produced
  the phantom defect. Both this steward and the reviewer made that error
  once; the pinning test is what settled it. `blackboard.md` under the
  evidence directory has been corrected in place rather than superseded.
- **Cause and fix, generalised for reuse:** a `UCLASS` on the input path
  that disables its own tick still receives input and dispatches none of
  it, and the failure presents as a green build with every binding
  correctly configured. `AStratPlayerController::PrimaryActorTick.bCanEverTick`
  `false` → `true`, set explicitly in the constructor with the reasoning
  written in place (`StratPlayerController.cpp:37-70`).
- **Standing debt, left open by the reviewer's judgement — do not spend
  another phase on it.** NeoStack input injection reaches
  `UGameViewportClient::InputKey` (the in-game console provably toggles
  under simulated input) but never reaches `UPlayerInput`, so the playtest
  is **not machine-repeatable**: the eight assertions document one stored
  artifact rather than defend a repeatable gate. This is judged a NeoStack
  plugin issue outside this repository. The phase's actual regression — the
  tick flag — *is* netted repeatably, by
  `T-UI-02.ControllerTicksSoInputDispatches`.
- **Combat-outcome gap, unresolved and not this phase's job to close:** no
  destruction or encounter-result line exists in `LogStratPlay`, so the
  encounter narrative stays visual. If a later phase wants a log-backed
  outcome, it is a `FStratBridge`-routed line, not a widget one.
- **Two clauses were writable, owed to the next test-author phase** — they
  were not writable before phase 6 because the `STRAT-WAIT spent` line did
  not yet exist: `T-INT-05.WaitIsDistinguishableFromAttack` and
  `T-INT-05.WaitWithNothingSelectedIsANoOp`. **Discharged 2026-08-13** — see
  the record under NEXT above.
- **A wait leaves no trace in `RecordedLog()` and cannot** — the vendored
  format has no `Wait` kind; a replay reproduces state hashes but not the
  player experience.
- **`.agents/ue-project-context.md:195` drift is already fixed** — it now
  reads 67/67, hot-seat phase 6, 2026-08-13 (confirmed above, this
  steward's own edit, dated 2026-08-13). No action owed from a future
  reader; recorded here so it is not re-flagged.

## Hot-seat milestone — COMPLETE

With phase 6 closed, the hot-seat milestone is complete. Ferrum Crossing
renders, units spawn from the scenario, click-to-select → move → attack
with the deterministic forecast, wait, end turn advances sides, and the
scoreboard follows the active side.

Out-of-scope list at the time this milestone closed: production menu (§2.11.5),
guided opening (§2.11.6), info panel, toasts, save-slot UI, AI opponent,
move-undo. **The AI opponent has since become its own milestone — see
"## AI-opponent milestone" below.** The remaining out-of-scope items are
unchanged and stay under NEXT: production menu (§2.11.5), guided opening
(§2.11.6), info panel, toasts, save-slot UI, move-undo.

## AI-opponent milestone

The GDD's §2.9 AI is already vendored and certified — this milestone authors
no AI and re-tests none of its decision quality. `Source/StratRules/Ai.h:84`
declares `strat::nextCommand(const AiState&, int side)`, a pure function with
no RNG and no clock (`Ai.h:2`, confirmed by direct read: *"Zero engine
dependencies. Pure function of state; no RNG, no clock, no I/O."*). The GDD
§3 ledger already records `T-AI-01..06 (6/6) + GATE-AI-SMOKE` verified
upstream against `cpp_reference/Ai.good.cpp`. `T-AI-02/03/04/05` are someone
else's proof, not this milestone's to redo — this milestone's job is
integration: get the engine to call `nextCommand`, apply what it returns
through the same path a player's command takes, and drive a turn loop and a
gate around that.

Two structural facts, worth preserving because they foreclose alternatives
future phases might otherwise re-derive:
- `Driver.h:120`'s `aiStateOf` takes the driver's `Session`, which the bridge
  does not have. `MakeAiState` is the fifth composition, on the precedent
  `StratBridge.h:248-256` already documents for `MakeUiWorld`.
- `Ai.h:62-66` omits Capture deliberately; `openTurn` runs `captureTick`
  (`Replay.good.cpp:264-270`) from the EndTurn arm (`:529-535`), so **the AI
  captures without ever emitting Capture**. `SubmitCapture`'s missing
  affordance stays out of scope.

### Phase A — CLOSED

- **Completed:** 2026-08-13.
- **Exit criterion:** bridge AI surface — build green, parity tests green.
- **Met.** `Source/StratBridge/StratBridge.h` (+198 lines) and `.cpp` (+219
  lines), additions only, confirmed by `git diff --stat` against `HEAD`
  `f04f5a4`; nothing staged. New untracked test file
  `Source/StratBridge/Tests/StratAiBridgeParity.cpp` (1577 lines).
  - Surface: plain `EStratAiCommandKind`/`FStratAiCommand`, `NextAiCommand`,
    `SubmitBuildAtHex`, `SetBuildlistByIds`, `BuildlistDefIndexes`, private
    `MakeAiState`.
  - Suite **78/78**, `succeeded 78 / succeededWithWarnings 0 / failed 0 /
    notRun 0`, `reportCreatedOn 2026.08.13-19.03.16`
    (`Saved/AutomationReport/index.json`, read `utf-8-sig`) — read directly
    from the report by this steward, not taken from a builder number. **The
    pre-phase baseline was 69, not 67**; verified against the phase-6-close
    NEXT entry recording 69/69 after the two wait clauses. 78 − 69 = 9 new
    clauses, all `state: "Success"`: `Stratocracy.StratBridge.T-AI-01.`
    `CommandsSubmitUnchangedAndRecord`, `.AiBoardAgreesWithRulesQueries`,
    `.BuiltThisTurnReachesTheAi`, `.AttackTargetIdResolvesToItsHex`,
    `.RefusalsAreDistinguishableFromEndTurn`, `.SubmitBuildAtHexMatchesSubmitBuild`,
    `T-AI-06.SameStateYieldsSameCommandSequence`,
    `GATE-BRIDGE-DEFS.BuildlistResolvesInLoaderOrder`,
    `.BuildlistRefusalsLeavePriorListIntact` — all found by name in the
    report's `tests` array by this steward.
  - Each self-play game ran 156 commands (22 Build, 55 Move, 68 Attack, 11
    EndTurn) to a terminal §2.8 result, 59 of 68 attacks foil-eligible off
    the `q==r` diagonal. No hex, unit id, defIndex or turn number is written
    down in the test file.
  - `Ai.h:1-2` read directly for this record: "headless baseline opponent AI
    (GDD §4.7 Stub 6, §4.11 row 6)... Pure function of state; no RNG, no
    clock, no I/O."

  **Decisions and debts recorded from the phase:**
  - **`CheckAiStateComposition` was proposed by `strat-test-author` and
    declined.** `strat::isFlagUnit` (`Replay.good.cpp:142-144`) is
    token-for-token the expression `aiStateOf` inlines at
    `Driver.good.cpp:424`, and it is folded into `canonicalStateBytes`
    (`Replay.good.cpp:183`) → `canonicalStateHash` →
    `FStratBridge::StateHash()`, already gated against the headless fixture
    by `T-INT-02.ReplayParityWithHeadless`. So `isFlag` is witnessed through
    another gate and is not unpinned — a different call from the
    twice-declined phase-2/3 allocation-counter debt, which had no witness
    at all. `economy` needs no separate witness because `A.economy =
    GameState.economy` is a whole-struct copy with no per-field
    transcription for an omission to hide in.
  - **Owed to phase B's test-author, deliberately deferred to avoid a second
    full suite run:** three case-insensitive comparisons in
    `StratAiBridgeParity.cpp`, verified by direct read of those exact lines
    — `1133-1135` (`TestEqual` on the `GATE-BRIDGE-DEFS` positional
    comparison), `1029-1034` (pairwise `TestNotEqual` on refusal reasons),
    `1240` (`FString::Contains`, case-insensitive by default in UE 5.8).
    None is live today — both sides of each comparison are byte-identical by
    construction — but the shape is unsafe. Fix with `TestEqualSensitive` /
    `ESearchCase::CaseSensitive`.
  - **General measurement worth reusing:** `FString::operator!=`,
    `FString::Contains`, and `TestEqual(FString)` are all case-INSENSITIVE in
    UE 5.8 (`TestEqualSensitive` is the strict variant). Same family as
    phase 6's "an absence needs a control" — a clause that could not fail.
  - **Hazard for phase B:** `FStratAiCommand`'s default-constructed value
    reads as a genuine `EndTurn`, so `FStratResult::bOk` is the only thing
    separating "refused to answer" from "ended its turn." A turn loop that
    ignores `bOk` gets a clean, instant, empty turn with no fault.
  - **Engineer decisions preserved:** kind mapping is a refusing `switch`,
    not a `static_cast`, because `Ai.h` lives in another repository and a
    re-vendored enumerator would otherwise silently produce a value the
    engine enum lacks. Buildlist ids match on exact bytes because `FName`
    compares case-insensitively and would widen the §4.8 id space in engine
    code rather than data. An unresolvable attack target refuses rather than
    defaulting `Hex`, because `FIntPoint(0,0)` is a real hex on this board.
    Duplicates in the buildlist are preserved — that is how §2.9's build mix
    is expressed. `LoadDefinitions` clears the buildlist (defIndexes into a
    vector a reload moves); `LoadScenarioFromFile` does not.
  - **Vendoring was already done, confirmed by direct read:**
    `Source/StratBridge/Vendored/Ai.strat.cpp` already existed (since
    `0897cb5`) and already includes `../../StratRules/Ai.good.cpp`, so
    `strat::nextCommand` was in the DLL with no `.Build.cs` change needed.

### Phase B — CLOSED

- **Completed:** 2026-08-13. Gate returned `VERDICT: PASS` twice — an initial pass, then a
  narrow re-gate after a post-pass production change (see "The post-gate fix" below).
- **Exit criterion (literal):** "AI turn runner as a plain testable struct in `StratPlay`, match
  wiring, a `STRAT-AI` log line; build green; tests green." **Met.**
- **Suite, read directly from the report (`Saved/AutomationReport/index.json`, `utf-8-sig`),
  not taken from a builder number:** `reportCreatedOn 2026.08.13-20.22.26`, `succeeded 86 /
  succeededWithWarnings 0 / failed 0 / notRun 0`, 86 entries in `tests`, all `state: "Success"`.
  Baseline entering the phase was 78 (phase A's close). 86 − 78 = 8, and the eight new
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` sites are found directly in the two new test files, not
  guessed from a name pattern: in `StratAiTurnRunnerClauses.cpp` —
  `Stratocracy.StratPlay.T-AI-01.RefusalIsNotATurnEnd`, `.T-AI-01.SubmitRefusalStopsTheTurn`,
  `.T-AI-01.LoopBoundIsAReportedFault`, `.T-AI-01.EveryKindRoutesToARecordingSubmit`,
  `.T-AI-06.SameStateYieldsSameAiTurn`; in `StratAiMatchClauses.cpp` —
  `Stratocracy.StratPlay.T-INT-05.EmptyAiSidesRunsNoAiTurn`,
  `.T-INT-05.BuildlistRefusalLeavesTheMatchLive`,
  `.T-INT-05.BothSidesAiReachesAResultWithinTheBound`. (An earlier name-pattern search over the
  report missed the three `T-INT-05` clauses — they don't contain the substring `T-AI` — and
  wrongly implied only 5 new tests; re-derived correctly by grepping the test files themselves,
  which reconciles exactly to 8.) The seven `T-AI-01`/`T-AI-06` `StratBridge` clauses from phase
  A (`AiBoardAgreesWithRulesQueries`, `AttackTargetIdResolvesToItsHex`, `BuiltThisTurnReachesTheAi`,
  `CommandsSubmitUnchangedAndRecord`, `RefusalsAreDistinguishableFromEndTurn`,
  `SubmitBuildAtHexMatchesSubmitBuild`, `SameStateYieldsSameCommandSequence`) are still present in
  the 86 and still `Success`, unchanged carryover from the 78 baseline. Report file mtime
  **16:22:26** local — confirmed directly via `os.stat`, 4 hours behind the `reportCreatedOn`
  field's `20:22:26`, consistent with that field being UTC; both point at the same report, no
  discrepancy in substance.
- **What landed** (verified against `git status --porcelain=v1 -uall`, not taken from a builder
  report): new and untracked — `Source/StratPlay/StratAiTurnRunner.h`/`.cpp`
  (`IStratAiTurnPort`, production adapter `FStratBridgeAiTurnPort`, `FStratAiTurnOutcome`,
  `FStratAiTurnRunner`, all plain non-reflected structs), `Source/StratPlay/Tests/StratAiTurnRunnerClauses.cpp`,
  `Source/StratPlay/Tests/StratAiMatchClauses.cpp`. Modified — `StratMatchSubsystem.h`/`.cpp`
  (five `EditAnywhere` AI fields on `FStratMatchConfig`; `IsSideAi`, `IsAiTurnDue`,
  `RunAiTurnsIfDue`, `RunAiTurnsNow`), `StratGameMode.cpp` and `StratPlayerController.cpp` call
  sites, and `Source/StratBridge/Tests/StratAiBridgeParity.cpp` (the three case-insensitivity
  fixes carried as debt from phase A — see below). No `.Build.cs` change, no `.uproject` change,
  no new module. Nothing staged, nothing committed.

  **Decisions that foreclose alternatives — recorded with the measurement behind each:**
  - **The runner's rules side is an `IStratAiTurnPort` interface, not `FStratBridge&`.** The
    shipping AI never refuses, never fails a submission, and never loops, so all three fault arms
    of `RunTurn` are unreachable through a real bridge and would ship unexecuted — the exact state
    phase A's second gate finding blocked on. A scripted port is the only way a test can make the
    AI refuse.
  - **`bOk` is branched on before `Kind` is ever named**, and a refusal does not print the
    command's fields, because `FStratAiCommand`'s default value is a plausible-looking `EndTurn`
    nobody decided.
  - **AI commands bypass `StratSubmitSelectionCommand`** (it takes a click-shaped
    `FStratSelectionOutcome` with no Build kind) but still go out through the four recording entry
    points. Consequence a gate can rely on: `grep -c "STRAT-CMD accepted"` counts human commands,
    `grep -c "STRAT-AI applied"` counts AI commands, no overlap, `RecordedCommandCount()` is the
    sum.
  - **Loop bound `MaxCommandsPerTurn` default 256; `<= 0` refuses rather than meaning
    unbounded.** Phase A measured a whole self-play game at 156 commands across 11 turns
    (~14/turn), so 256 is an order of magnitude above anything observed. Unbounded is the one
    configuration where a looping AI hangs PIE and the suite, reachable from a Blueprint default
    with no compiler diagnostic. Outer bound `AiMaxConsecutiveTurns` default 64.
  - **Pacing is before the turn, not between commands** (`AiTurnDelaySeconds`, default `0.0` =
    synchronous). A per-command delay would make the runner an incremental machine with resumable
    mid-turn state — a mirror of the rules state by another name. Zero default is what makes the
    AI path drivable from a test with no ticking world.
  - **`SideToMove()` is read once at entry**, so a side changing underneath the loop ends it; the
    successful exit is an applied EndTurn, never "the side changed."
  - **The runner holds no mirror** — turn, side and hash are read off the port at the instant they
    are printed; the AI is re-asked after every applied command. Same discipline as
    `FStratSelectionMachine`.

  **The `STRAT-AI` format strings** (`LogStratPlay`; `applied`/`turn-ended` at `Log`, `refused` at
  `Warning`) — recorded verbatim, phase D builds a parser on them:
  ```
  STRAT-AI applied kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d hash=%s
  STRAT-AI refused phase=%s kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d reason=%s
  STRAT-AI turn-ended side=%d turn=%d commands=%d hash=%s
  ```
  `phase` ∈ `decide` | `apply` | `bound` | `handover`. `def` and `target` are this line's two
  additions over `STRAT-CMD` (a `defIndex` is phase 0's load-bearing raw index; `TargetId` names
  the unit the AI chose, which the resolved hex does not tell you). `turn`/`side` read before
  submission, `hash` after.

  **The post-gate fix, recorded because the reasoning generalises.** The first gate's Observation
  2 found `StratAiTurnRunner.h:231` already false in its own diff — it claimed the runner emits
  every `STRAT-AI` line, while `RunAiTurnsNow` emitted a hand-written **two-field** `phase=handover`
  line not following the declared nine-field format, which phase D's parser would have tripped on.
  Closed immediately rather than deferred, because the editor was closed at that moment and phase
  C is editor-open, so deferring cost a full close→build→reopen cycle. The engineer declined the
  naive remedy (copy the format string into `StratMatchSubsystem.cpp`) because that satisfies the
  letter while recreating the cause — two nine-field `UE_LOG` sites free to drift apart again,
  which is what had just happened inside a single diff. Instead it exported
  `STRATPLAY_API StratLogAiTurnRefusal(...)` so `StratAiTurnRunner.cpp` remains the **sole holder**
  of the format string, verified here directly: `grep -c "STRAT-AI refused"` finds 3 occurrences in
  `StratAiTurnRunner.cpp` (the format literal at `:79` plus its two other call sites) and 2 in
  `StratMatchSubsystem.cpp` (both calls into `StratLogAiTurnRefusal`, no literal), and
  `grep -n "STRAT-AI refused phase"` — the actual format-string text — returns exactly one hit,
  `StratAiTurnRunner.cpp:79` (a second match at `:209` is a comment, not code). **Generalisable:
  when two sites must agree on a format, route one into the other rather than copying — a copy
  makes the prose true today and false again on the next edit.**

  **The deferred finding — the gate was explicit this must be written down before the phase
  closes.** An AI-vs-AI game that reaches a §2.8 result **mid-turn** ends with the rules module
  refusing the winning side's closing EndTurn:
  ```
  STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=6 side=1 reason=[T-SAVE-05] no match is running
  ```
  So `FStratAiTurnRunner::RunTurn` returns `bOk = false` and `UStratMatchSubsystem::RunAiTurnsNow`
  returns **false for a game that finished correctly**, while the result, turn count, recorded
  log, replay parity and handover silence are all right. **Finishing is reported as a fault**,
  distinguishable from a genuine mid-game refusal only by reading the refusal text. It is loud,
  not silent — `StratGameMode.cpp:126` and `StratPlayerController.cpp:383` both complain — which
  is why the gate ruled it non-blocking for an exit criterion about the runner, the wiring and the
  log line. `strat-test-author` asserted the return value **neither** way: asserting `true` fails
  on a correct game today, asserting `false` would make the clause demand the defect. It left
  `AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ..., Occurrences 0)` at
  `StratAiMatchClauses.cpp:610` as a tripwire, so a future fix fails the clause loudly and gets
  revisited deliberately. **Consequence recorded in NEXT above, in terms a phase-D reader cannot
  miss:** `RunAiTurnsNow`'s return value is now an untested production contract, and phase D's
  gate must not be built on it.

  **Carried-forward debt, also recorded because it must reach phase D's brief rather than rest on
  a gate observation:**
  - **`Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields`** — proposed by the
    engineer, ruled by the re-gate as correctly deferred to phase D, provided the debt is carried
    into phase D's brief rather than resting on a gate observation. That is what this entry does.
    Today `StratAiMatchClauses.cpp:730` asserts only the handover line's *absence*, so nothing
    executes its content; the shared formatter is already covered by three executed refusal arms
    (`decide`, `apply`, `bound`), leaving only `StratLogAiTurnRefusal`'s forward and its single call
    site dark. Phase D owns the field parser this clause would assert against — writing it earlier
    means writing the expected shape twice.
  - **The paced path (`AiTurnDelaySeconds > 0`) needs a ticking world** and is not covered by the
    synchronous tests; discharged by phase D's PIE playtest.
  - **`StratGameMode.cpp:98-104`** moved an early `return` inside an `else` so the opening AI turn
    is reached on the warning path too — correct and reasoned, but it changes control flow of an
    existing method that **no clause in this diff exercises**; the AI-first-side-with-missing-tile-mesh
    path is argued, not measured.
  - **`StratAiMatchClauses.cpp:560-562`** prose is now cosmetically stale (says the handover line
    is "emitted by `RunAiTurnsNow` — not by the runner"; still true of the call site, but now reads
    as if the format lives there). `strat-test-author`'s lane; the re-gate ruled it cosmetic, not
    misleading — worth a one-line touch when that file is next opened, not worth a cycle.
  - **Phase A's three case-insensitivity debts are DISCHARGED** — `StratAiBridgeParity.cpp` at
    1029-1039, 1138-1145, 1250-1256, now `TestNotEqualSensitive` / `TestEqualSensitive` /
    `ESearchCase::CaseSensitive`, both tests still passing. The sharpest was the `GATE-BRIDGE-DEFS`
    positional comparison: it pins that `SetBuildlistByIds` matches on **bytes** rather than
    `FName`'s case-insensitive operator, so a case-insensitive assertion there would have agreed
    with the very widening the clause forbids. Line 1244's `Contains(FString::FromInt(...))` was
    deliberately left alone — digits have no case.

  **Phase C handoff — recorded here so phase C's builder reads it.** Phase C must set these on
  `BP_StratGameMode`'s `MatchConfig`; none has a hardcoded value and none is an asset path:
  `AiSides` (e.g. `[1]`; empty today, which is why the AI is off and why the 78 baseline held),
  `AiBuildlistUnitIds` (`FName` ids matching `Data/units.csv` byte for byte — the bridge compares
  exact UTF-8, not `FName`'s case-insensitive operator; duplicates express §2.9's build mix and
  are preserved), and `AiTurnDelaySeconds` for pacing. `AiMaxCommandsPerTurn` (256) and
  `AiMaxConsecutiveTurns` (64) have safe defaults phase C need not touch.

  **A file outside this steward's lane, mentioned so a future phase commit does not drop it:**
  `.agents/ue-project-context.md:195` carried a stale `78/78, AI-opponent phase A` count. It has
  been corrected here (no crew agent owns that file) to `86/86, AI-opponent phase B` and the
  re-gate confirmed the edit correct against the report it read. It is unstaged.

### Phase C — CLOSED

- **Completed:** 2026-08-13. Gate returned `VERDICT: PASS`, zero findings. Editor OPEN,
  `strat-editor-builder` sole builder, no build, no test run — correctly out of scope for an
  asset-only phase, so phase B's 86/86 still describes the tree.
- **Exit criterion:** `BP_StratGameMode`'s `MatchConfig` carries the AI opponent's authored
  data — `AiSides`, `AiBuildlistUnitIds`, `AiTurnDelaySeconds` — set as Blueprint defaults.
  **Met.**
- **Exactly one changed path, confirmed by the gate and re-confirmed here against `HEAD`
  `3d864aa`:** `Content/StratPlay/BP_StratGameMode.uasset`, unstaged (`git status
  --porcelain=v1 -uall` shows only ` M Content/StratPlay/BP_StratGameMode.uasset`). LFS pointer
  `git diff` re-measured directly: `oid sha256:4ba9df4ea0…` → `d5129a45c…`, `size 22635` →
  `22882`. `HEAD` unmoved.
- **Values set, per the builder's post-save readback — re-measured here by printable-ASCII
  extraction over the smudged 22882-byte working-tree binary (min run length lowered to 3 to
  catch the 4-letter "Tank", which a 5-char threshold silently drops):**
  ```
  AiSides=(1)
  AiBuildlistUnitIds=("Infantry","Infantry","Infantry","Tank")
  AiTurnDelaySeconds=0.500000
  AiMaxCommandsPerTurn=256   (untouched)
  AiMaxConsecutiveTurns=64   (untouched)
  ```
  `AiSides`, `AiBuildlistUnitIds`, `AiTurnDelaySeconds` each appear as exact-spelling name-table
  strings; `AiMaxCommandsPerTurn`/`AiMaxConsecutiveTurns` appear **zero** times. `Infantry` and
  `Tank` each appear exactly once, byte-identical to `Data/units.csv`'s `Id` column — no
  `Artillery`, no `Recon`, no case or whitespace variant. `UnitTable`, `TerrainTable`,
  `ScenarioFile`, `BoardActorClass`, `UnitActorClass` all preserved across the rewrite (present,
  unchanged shape).

  **What extraction proves and what it cannot.** String presence, non-appearance, and exact id
  spelling are measured facts, above. `AiSides == (1)`, the buildlist's arity of 4, and
  `AiTurnDelaySeconds == 0.5` **cannot** be proven by string extraction — those rest on the
  builder's readback alone. **UE delta-serializes only properties differing from the archetype**,
  so for a Blueprint default the *absence* of `AiMaxCommandsPerTurn`/`AiMaxConsecutiveTurns` in
  the package is the positive evidence those equal their C++ defaults (`= 256` at
  `StratMatchSubsystem.h:235`, `= 64` at `:247`, both re-read directly for this record) — and
  symmetrically, the three properties that *do* appear are thereby proven non-default. This is
  the delta-serialization counterpart to the ASCII-string-extraction technique phase 5 used for
  "does this package reference symbol X at all." Phase D's PIE evidence is the natural place to
  close the value gap: an AI that takes a turn at all confirms `AiSides`, and a 0.5 s delay is
  visible.

- **Finding — the buildlist cannot express a ratio, and a bridge comment says it can. This
  reaches phase D's brief, not just a gate observation.** Re-read directly:
  `strat::chooseBuild` (`Source/StratRules/Ai.good.cpp:275-289`) collects every *affordable*
  buildlist entry and takes the minimum under `buildPriorityLess`, which is ascending
  `costFame` (`Ai.good.cpp:224-231`, comment: *"§2.9: it spends and replaces losses instead of
  hoarding, so the cheapest affordable buildlist unit is bought rather than saving for a dearer
  one."*). **Duplicates carry no weight at the rules layer.** `Data/units.csv` re-read directly:
  `Infantry,...,100,...` / `Tank,...,300,...` — any economy that can afford a Tank can also
  afford an Infantry and will always choose the Infantry. With Infantry in the list at all, **the
  Tank entry is not rare, it is unreachable**, and §2.9's "an occasional Tank" is not observable
  in phase D by any amount of playing.

  `Source/StratBridge/StratBridge.h:592-600` (re-read directly) states the opposite of that
  effect: *"DUPLICATES ARE LEGAL AND ARE THE POINT. §2.9 describes 'mostly Infantry, an
  occasional Tank' and gives no ratio, so `Ai.h:49-53` makes the list caller-supplied DATA;
  repetition is how a ratio is expressed in it. Deduplicating here would silently flatten every
  mix to 1:1."* That prose is correct about the **bridge** — `SetBuildlistByIds`
  (`StratBridge.cpp:910-930`) does preserve duplicates and compares exact UTF-8 bytes, and
  `GATE-BRIDGE-DEFS.BuildlistResolvesInLoaderOrder` pins it — and wrong about the **effect**: the
  rules layer never reads the repetition. **Filed as an upstream observation, not a task; not
  edited.** Fix, if ever wanted, is upstream in `E:\MultiAgent\stratocracy-crew`, in
  `Ai.good.cpp`'s `chooseBuild`.

  **Phase D consequence:** phase D's acceptance must not be written against seeing a Tank built —
  that is an outcome the rules cannot produce with this buildlist.

- **The buildlist mix is `strat-editor-builder`'s judgement, not the GDD's — recorded as such.**
  §2.9 (re-read directly, `Stratocracy_Prototype_GDD.md:436`): *"(§2.7) — from a default
  buildlist (mostly Infantry, an occasional Tank),"* no ratio given. `Ai.h:49-52` (re-read
  directly) independently confirms the omission is deliberate: *"§2.9 describes it as 'mostly
  Infantry, an occasional Tank' and gives no ratio or rule, so the list is DATA the caller
  supplies; inventing a ratio here would be a rule the GDD does not have."* The builder authored
  `Infantry, Infantry, Infantry, Tank` (3:1) as the smallest list reading as "mostly Infantry, an
  occasional Tank," and excluded Artillery and Recon on the reasoning that §2.9's buildlist
  clause names only Infantry and Tank, while the four-way `Infantry > Recon > Artillery > Tank`
  tie-break at GDD line 438 (re-read directly, confirmed the exact string) orders whatever the
  list contains rather than stating the list's contents. Recorded as the builder's judgement where
  the GDD is silent, same posture already taken on `buildlist_query` under NEXT. Worth noting: the
  finding above means the 3:1 ratio has no runtime effect regardless.

- **Latent hazard, not live today.** `AiBuildlistUnitIds` is `TArray<FName>`
  (`StratMatchSubsystem.h:228`, re-read directly), and in a non-case-preserving build
  `FName::ToString()` returns the first-interned casing. Benign now because `DT_Units` interns
  the same ids from the same CSV, but it is a path by which an exact-byte comparison could one
  day see a casing it did not author.

- **NeoStack: struct sub-paths fail, generalising the existing array note.** `get`/`set` on
  `MatchConfig.AiSides` failed with `property not found`; the whole `MatchConfig` struct had to
  be read and rewritten. Folded into the existing `array_count("DefaultKeyMappings.Mappings")`
  entry under NEXT rather than a second, separate one — whole-struct read/write is the standing
  workaround for both arrays and struct members.

- **Nothing needed flipping this phase; confirmed untouched, not merely stated.** `Config/`:
  `git status` shows no `Config/` path modified. `Lvl_TopDown`, `BP_TopDownGameMode`,
  `GlobalDefaultGameMode`: no other path appears in `git status --porcelain=v1 -uall` beyond
  `BP_StratGameMode.uasset`, so all three were untouched by construction.

**Phase D opened, 2026-08-13 (editor OPEN; piece (a) below touched neither editor nor build).**
Folded under "Phase D — CLOSED" below — this is a dated sub-record of the phase's opening state,
not a current status; matches the single-heading convention used by phases A/B/C.

PIE playtest plus a machine-repeatable AI-vs-AI gate, evidence under
`Tools/architect/evidence/07-ai-opponent/`.

**Piece (a) done, 2026-08-13, by `strat-data-steward`: the `STRAT-AI` parser and gate.**
`Tools/architect/strat_ai_log_gate.py` parses all three `STRAT-AI` shapes into structured
dataclasses (`StratAiApplied`, `StratAiRefused`, `StratAiTurnEnded`; an unparseable line is a
fourth, `StratAiParseFailure`, reported, never skipped) and a CLI gate (`run_gate`/`main`) that
exits non-zero on any parse failure, any non-terminal refusal, or (by default) a log with zero
`STRAT-AI` lines. Literals re-read directly from `Source/StratPlay/StratAiTurnRunner.cpp`
(lines 78-89, 208-210, 318-336), not transcribed from this file. The one refusal shape the gate
must let through — `phase=apply kind=EndTurn reason=[T-SAVE-05] no match is running`, the
terminal "match already finished correctly" case this milestone's phase B recorded — is isolated
in `is_terminal_handover_refusal()`, checked on all four of phase/kind/reason-prefix/reason-
substring together so no unrelated refusal is waved through by accident. Proven against four
hand-authored fixture logs under `Tools/architect/evidence/07-ai-opponent/fixtures/` (no real AI
log exists yet — this dispatch touched neither the editor nor a build), captured verbatim in
`Tools/architect/evidence/07-ai-opponent/gate_self_test_output.txt`; see that directory's
`blackboard.md` for the full account, including why fixtures stand in for a real log at this
stage. **`Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields`
(`strat-test-author`'s lane) can now be written against this parser's field contract** — see the
dispatch report for the exact dataclass field names and types. **Still open in phase D:** the PIE
playtest itself, and gating a real AI-vs-AI log (headless or PIE-produced) rather than synthetic
fixtures.

**Piece (a), continued, 2026-08-13, same day, by `strat-data-steward`: the owed clause landed, and
the gate ran against a real log for the first time. (Dated record: at the moment this bullet was
written, the PIE half and the reviewer gate were both outstanding; superseded by "Piece (b)" and
"Phase D — CLOSED" below, which report both complete.)**

- `Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields` now exists
  (`Source/StratPlay/Tests/StratAiMatchClauses.cpp:922`), asserting against the parser's ten-field
  contract. Suite **87 succeeded / 0 failed / 0 notRun, 87 entries**
  (`Tools/architect/evidence/07-ai-opponent/suite-87/index.json`, `reportCreatedOn
  2026.08.13-21.38.06`); macro set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` went 86 → 87,
  delta +1, agreeing exactly.
- **Correction, checked and found not to require any edit here:** `phase=handover` is not one of
  `FStratAiTurnRunner`'s fault arms and is not reachable through `IStratAiTurnPort`. Read directly,
  `StratMatchSubsystem.cpp:706-724`: it is `UStratMatchSubsystem::RunAiTurnsNow`'s own outer bound
  (`StopReason.IsEmpty() && TurnsRun >= MaxTurns`), emitted by that function via
  `StratLogAiTurnRefusal` after a run of turns that all *succeeded* — the source comment at
  `:712-713` states this outright. Neither this section nor the gate's code attributed
  `phase=handover` to the runner, so nothing needed correcting; recorded so a future reader does
  not re-introduce that misattribution.
- **A full-suite log is the wrong corpus for this gate.** Gated against `Saved/Logs/` full-suite
  output (249 `STRAT-AI` lines): exit 1, 5 parse failures + 6 blocking refusals, **all 11 correct
  behaviour being misreported** — the suite deliberately manufactures refusals as fixtures
  (scripted-port refusals, `MaxCommandsPerTurn is 0`, `definitions are not loaded`, two
  handover-adjacent lines). The gate is built for one isolated real match, not a synchronous test
  suite's log; pointing it at the latter will always trip it by design.
- **Real corpus, and independent corroboration of correctness.** Ran
  `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` alone
  (`-ReportExportPath=...\Saved\AutomationReportIsolated`, keeping the 87/87 artifacts
  unclobbered), captured at `Tools/architect/evidence/07-ai-opponent/real-game/isolated-run.log`
  (= `Saved/Logs/Stratocracy.log` at that point, confirmed by grepping its own `Cmd: Automation
  RunTests ...` and `Test Completed. Result={Success}` lines). Pre-fix gate output: **156 applied,
  11 turn-ended, 1 terminal refusal, 0 blocking, 1 parse failure.** 156 commands across 11 turns
  is exactly what phase A measured for a self-play game — independent confirmation the gate reads
  a real, complete, deterministic match, and that its terminal-refusal branch fires correctly on a
  real finish.
- **The one real defect, found and fixed here.** The single parse failure was line 3290, not a
  production line at all:
  `LogAutomationController: Suppressed expected ('Warning') level log message or higher matching
  'STRAT-AI refused' 1 times.` — versus a genuine line two entries earlier,
  `LogStratPlay: STRAT-AI turn-ended side=0 turn=1 commands=8 hash=02cd799951334a5f`.
  `find_strat_ai_payload` matched the bare substring `STRAT-AI` anywhere in the line, so the
  automation harness's own commentary about a suppressed expected-error match was parsed as if it
  were the runner's own output. **Fixed** by anchoring on the `LogStratPlay:` category — the only
  category `StratAiTurnRunner.cpp` logs through — and searching for the `STRAT-AI ` payload only
  after that marker; an allowlist of one known-good category, not a blocklist of
  `LogAutomationController` specifically. Confirmed the fix does not weaken the drift check:
  `fixtures/fail_format_drift.log` (three genuine, malformed `LogStratPlay:` lines) still reports
  **3 parse failures**, unchanged. **Pinned with a new fixture**,
  `fixtures/pass_harness_echo_ignored.log` (two genuine lines plus the exact harness-echo string
  copied from the real log). Re-run against `real-game/isolated-run.log` post-fix: **exit 0,
  clean** (`real-game/gate_output.txt`).
- **Lesson worth generalising:** every fixture built for this gate before this update was
  hand-authored *in production shape* only — a parser tested exclusively against inputs shaped
  like its happy path cannot discover a defect that only a genuinely adversarial, differently-
  shaped input (here: the test harness's own commentary, sharing a substring with the target but
  not its category) will trigger. The self-test could not have caught this on its own; it took a
  real log.
- Full account, all six log excerpts, and the suite-log caveat stated in full:
  `Tools/architect/evidence/07-ai-opponent/blackboard.md`.
- **Reviewer gate on this piece: `VERDICT: PASS`, after an initial `BLOCK` and a fix.** The
  reviewer's first pass returned `BLOCK` on a prose finding in the evidence blackboard (the
  `Occurrences = 0` / T-SAVE-05 passage misread UE 5.8's expected-message semantics — see that
  file's "Limits" section, CORRECTED). The passage was fixed in place; a narrow re-gate then
  returned `VERDICT: PASS`, zero gating findings, scope confirmed by file mtime since no
  intermediate commit exists to diff against. That re-gate also surfaced five non-gating
  observations (prose staleness and precision gaps), fixed in the same steward pass that recorded
  this — see the blackboard's "Reviewer gate on this piece" section for the full sequence.
- **Still open, and phase D is NOT closed by this update:** the PIE playtest itself has not been
  run — the user drives it in a later sitting. The gate half's reviewer gate is now `PASS`; the PIE
  half's has not run and cannot until PIE runs. **(This bullet is a piece-(a)-era record, written
  before the PIE playtest existed; superseded by "Piece (b)" immediately below, which reports the
  playtest having run. Left in place rather than deleted, because it is true of the moment it
  describes — see the phase-close entry below for the current, final status.)**

**Piece (b), 2026-08-13, same day, by `strat-data-steward`: the PIE playtest, driven by the user,
assembled here. Phase D's exit criterion is now MET. This piece's own narrow re-gate has since run
— an initial `BLOCK` on three miscounted claims, corrected, then `VERDICT: PASS` on the fourth
gate — see "Phase D — CLOSED" below for the full sequence.**

- **Two separate PIE windows exist in one log, `Saved/Logs/Stratocracy.log` — kept
  distinct, not merged.** Both open with the identical `LogWorld: Bringing World
  /Game/StratMaps/UEDPIE_0_Lvl_FerrumCrossing.Lvl_FerrumCrossing up for play` line. **Re-cut
  2026-08-13, later still, once the "3481 vs 3480" line count below was traced to a live, growing
  log rather than a slicing error — both slices are now bounded by their own open/shutdown markers,
  never by EOF or by a neighbouring session's start; see `blackboard.md`'s "Re-cut" section for the
  full account.**
  - **Session 1 — the completed game.** Bounded by full-log lines 3249 (open) to 3384 (this
    session's own `Destroying online subsystem :Context_1`), sliced to
    `Tools/architect/evidence/07-ai-opponent/pie-session/session1-completed-game.log`. Gate run
    directly against the slice: **exit 0, PASS — 74 applied / 4 turn-ended / 1 terminal refusal /
    0 blocking / 0 parse failures**, unchanged from the pre-re-cut result, reproducing the
    dispatching agent's reported numbers exactly.
  - **Session 2 — the screenshot run, a separate short session.** Bounded by full-log lines 3443
    (open) to 3480 (this session's own `Destroying online subsystem :Context_2`), sliced to
    `.../pie-screenshots/session2-screenshot-run.log`. Gate: **exit 0, PASS — 8
    applied / 1 turn-ended / 0 terminal / 0 blocking / 0 parse failures**, unchanged from the
    pre-re-cut result, also reproducing exactly.
  - Both gate outputs captured verbatim in each directory's `gate_output.txt`.
- **The paced path is discharged with a number, read from session 1's own timestamps.** Delay from
  each human `STRAT-CMD accepted kind=EndTurn` to the next `STRAT-AI applied`, all five turns:
  **0.507, 0.500, 0.501, 0.499, 0.500 s**, against `AiTurnDelaySeconds = 0.500000`. A sixth data
  point from session 2's single turn: 0.501 s. This debt had no synchronous test coverage (needs a
  ticking world); PIE is the only thing in the project that exercises it. Full table with line
  numbers: `pie-session/narrative.md`.
- **Pacing is per-turn, not per-command — confirmed independently.** Inter-command gaps within an
  AI turn are 0.000 s (every `applied` line in one AI turn shares the same millisecond timestamp).
  Observed at runtime, not re-argued from source — phase B's design decision.
- **`AiSides=(1)` confirmed at runtime, in both sessions independently.** Every `STRAT-AI` line in
  both slices carries `side=1`; every `STRAT-CMD` line carries `side=0`.
- **The terminal-refusal case observed in the wild for the first time**, session 1 line 3371 (full
  log): `STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=5 side=1
  reason=[T-SAVE-05] no match is running`. The human lost all units; the match reached a §2.8
  result mid-AI-turn; the rules module correctly refused the winner's own closing `EndTurn`; the
  gate classified it terminal and passed. Previously this shape existed only in a hand-authored
  fixture and one headless run — this is its first live-PIE observation.
- **Phase C's Tank prediction is now empirically confirmed.** All **12** `STRAT-AI applied
  kind=Build` lines across both sessions (11 in session 1, 1 in session 2) carry `def=0`.
  `Data/units.csv` row order (proven phase 0) is `Infantry, Tank, Artillery, Recon`, so `def=0` is
  Infantry and Tank (`def=1`) never appears — 12 opportunities, zero Tanks, despite Tank being a
  quarter of the authored buildlist. Phase C predicted this by reading `chooseBuild`; it is now
  measured, twice.
- **The gate handles PIE corpus shape — the open limit is now closed.** Both slices gate with
  **0 parse failures**; the `LogStratPlay:` anchor, proven previously only against a headless
  automation-test log, now parses PIE output correctly. `blackboard.md`'s "What remains open"
  section is updated to record this closed.
- **Command-channel disjointness held in a real session.** Session 1: 23 `STRAT-CMD accepted`
  (12 Move, 6 Attack, 5 EndTurn) vs 74 `STRAT-AI applied` (11 Build, 37 Move, 22 Attack, 4
  EndTurn) — no overlap, counted directly.
- **The phase C value gap is CLOSED.** `strat-editor-builder` read `BP_StratGameMode`'s
  `MatchConfig` live off the running CDO and the dispatching agent independently re-read it in the
  same session: `AiSides=(1)`, `AiBuildlistUnitIds=("Infantry","Infantry","Infantry","Tank")`,
  `AiTurnDelaySeconds=0.500000`, `AiMaxCommandsPerTurn=256`, `AiMaxConsecutiveTurns=64` — every
  phase C expectation confirmed, no discrepancy. That CDO reading is not repeated by this steward
  pass — `execute_script` was disconnected this session (editor opened after the session started,
  the known NeoStack connector latch) — but three of the five values are independently
  corroborated *behaviourally* by this PIE log itself: `AiSides=(1)` (runtime), `AiTurnDelaySeconds
  = 0.500000` (the five-plus-one delay measurements), and the Infantry/Infantry/Infantry/Tank
  buildlist's Tank entry never being reached. `AiMaxCommandsPerTurn=256` and
  `AiMaxConsecutiveTurns=64` are not falsifiable from a 5-turn, ≤22-command-per-turn game and are
  taken on the prior CDO reading, not re-measured here. Note the CDO read shows 256/64
  **positively**, where phase C's ASCII extraction could only infer them from absence; the two
  techniques agree, and phase C's "What extraction proves and what it cannot" paragraph (see
  "Phase C — CLOSED" below) should be read alongside this as its open gap now closed.
- **Screenshots.** `ScreenShot00042.png` (20:37:00, `TURN 1 / 20`, `Unit HP 60/60`) and
  `ScreenShot00043.png` (20:37:08, `TURN 2 / 20`, `Unit HP 60/70`) both fall inside **session 2's**
  window (opened 20:36:56) — copied to `pie-screenshots/`. Viewed directly by this steward: both
  are full-editor-window captures, not viewport-only; `00043`'s legible Output Log panel matches
  `pie-screenshots/session2-screenshot-run.log` line for line (STRAT-CMD accepted EndTurn turn=1
  side=0, AI's Build, six Moves, EndTurn, turn-ended side=1 turn=1 commands=8 — `grep -c
  "kind=Move" session2-screenshot-run.log` = 6). **These
  screenshots depict session 2's short run, not session 1's completed game — stated explicitly in
  `pie-screenshots/narrative.md` so this is never conflated.**
- **CORRECTED — the "3481 vs 3480" line originally recorded here as a discrepancy is not one.**
  `Saved/Logs/Stratocracy.log` is a live log a running editor keeps appending to (`[UsageMonitor]`
  lines land roughly every 90 seconds, plus unrelated engine noise); its line count is not a fixed
  fact about the file. The brief's 3480, this steward's original 3481, and a later re-measurement
  of 3518 were each correct at the instant taken — there is nothing to reconcile beyond "the file
  was still growing." Named slice boundaries (3249, 3442, 3443) landed exactly where the brief
  specified and every gate count reproduced exactly; the "to EOF" boundary itself was the actual
  problem, fixed by the re-cut above. **Generalisable lesson, for any future PIE or headless
  capture: slicing a log against EOF produces a non-reproducible artifact whenever the source is
  still being written — bound evidence slices by content markers, never by end-of-file.**
- Evidence: `Tools/architect/evidence/07-ai-opponent/pie-session/` (session 1: sliced log,
  `gate_output.txt`, `narrative.md`) and `.../pie-screenshots/` (session 2: sliced log,
  `gate_output.txt`, `narrative.md`, both PNGs). Full account and the "Update" section:
  `blackboard.md`.
- **Phase D's exit criterion — "PIE playtest done, machine-repeatable AI-vs-AI gate built and
  proven, evidence assembled" — is MET.** This piece's own narrow re-gate has since run and closed
  the phase — see "Phase D — CLOSED" immediately below.

### Phase D — CLOSED

- **Completed:** 2026-08-13. `strat-integration-reviewer` gated both halves of this phase, each with
  its own `BLOCK`-then-`PASS` sequence, honestly recorded here rather than only the final verdict:
  - **Gate half** (piece (a), the `STRAT-AI` parser/gate): first gate `VERDICT: BLOCK` on one prose
    finding (the `Occurrences = 0` / T-SAVE-05 passage misreading UE 5.8's expected-message
    semantics — see `evidence/07-ai-opponent/blackboard.md`'s "Limits" section, CORRECTED); fixed in
    place; narrow re-gate `VERDICT: PASS`, zero gating findings.
  - **PIE half** (piece (b), the PIE playtest evidence): first gate `VERDICT: BLOCK` on three
    miscounted claims in `pie-session/narrative.md` and `pie-screenshots/narrative.md` — a turn-3
    span/count, a turn-5 Build/Move/Attack breakdown, and a session-2 Move count sourced from "the
    brief" rather than the log printed three lines above the claim (the fourth occasion recorded
    under NEXT above, "six corrections across four occasions"); fixed in place, each replaced by a
    `grep -c` derivation run and confirmed against the checked-in slice; **`VERDICT: PASS` on the
    fourth gate** (the second gate on the PIE half, the fourth gate this phase in total counting
    both halves' first-and-re-gate pairs), zero findings.
  - This closing pass additionally fixed the reviewer's five non-gating observations from that
    fourth gate: the tally-noun imprecision under NEXT (corrected above to "six corrections across
    four occasions"), the three now-stale "has not run" sites (`state.md:3`, `:1550-1551` [that
    bullet's own text, dated and marked superseded rather than deleted], `:1554-1555`, `:1650`, all
    rewritten to state the actual `BLOCK`→fix→`PASS` sequence), the piece-(a)-era "PIE playtest has
    not been run" bullet immediately above the piece (b) section (left in place, dated, and marked
    superseded rather than deleted, since it was true of the moment it describes), the NEXT-block
    merge (checked — nothing lost), and the two narrative-derivation commands
    (`pie-session/narrative.md`'s pacing-count derivation and `pie-screenshots/narrative.md`'s Move
    count) — both now state their intent explicitly (channel- and kind-qualified `grep -c`) rather
    than returning the right number for a reason the command itself does not say.
- **Exit criterion met:** PIE playtest run, plus a machine-repeatable AI-vs-AI gate proven, evidence
  assembled under `Tools/architect/evidence/07-ai-opponent/`.
- **Measured, not reported — the record a future phase needs:**
  - Suite **87/87**, 0 failed / 0 notRun; macro set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
    went 86 → 87.
  - **The owed clause is discharged:**
    `Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields`
    (`StratAiMatchClauses.cpp:922`), executing the previously-dark `handover` arm. `phase=handover`
    is `RunAiTurnsNow`'s own outer bound (`StratMatchSubsystem.cpp:724`, reached after turns that all
    *succeeded*), **not** a `FStratAiTurnRunner` fault arm, and needs no scripted port — an earlier
    brief in this phase claimed otherwise and was wrong; corrected in place, not repeated.
  - **The gate** (`Tools/architect/strat_ai_log_gate.py`) is proven against three real corpora: the
    headless isolated run, PIE session 1, PIE session 2 — plus five fixtures. Exit 0 on all three
    real logs.
  - **Paced path discharged with numbers:** 0.507 / 0.500 / 0.501 / 0.499 / 0.500 s across session 1,
    plus 0.501 s in session 2, against `AiTurnDelaySeconds = 0.500000`. Inter-command gaps within a
    turn 0.000 s, confirming phase B's per-turn-not-per-command decision at runtime.
  - **`AiSides=(1)` confirmed at runtime, and phase C's value gap is closed** by the live CDO read
    (`AiSides=(1)`, buildlist arity 4, `AiTurnDelaySeconds=0.500000`, `AiMaxCommandsPerTurn=256`,
    `AiMaxConsecutiveTurns=64`, no discrepancy). Phase C's "What extraction proves and what it
    cannot" paragraph (see "Phase C — CLOSED" above) should be read with this: its open gap is now
    closed, and the CDO read shows 256/64 positively where extraction could only infer them from
    absence.
  - **Phase C's Tank prediction is empirically confirmed:** 12 of 12 `Build` lines carry `def=0`
    (Infantry); Tank (`def=1`) never appears.
  - **The terminal-refusal case was observed in a real game for the first time** (session 1, full-log
    3371) and the gate classified it correctly.
- **Gates unchanged by this closing pass's prose edits** — re-run directly, not assumed: session 1 →
  `74 applied / 4 turn-ended / 1 terminal / 0 blocking / 0 parse failures`, exit 0; session 2 →
  `8 applied / 1 turn-ended / 0 terminal / 0 blocking / 0 parse failures`, exit 0.

**The AI-opponent milestone is now COMPLETE**, phases A–D closed. Out-of-scope list unchanged:
production menu (§2.11.5), guided opening (§2.11.6), info panel, toasts, save-slot UI, move-undo.

## Log-backed combat outcome milestone

A combat outcome carried on a log line routed through `FStratBridge`, not computed in a widget.
**The design constraint that shapes it:** `applyCommand`'s Attack case returns `okResult(1)` and
nothing else (`Source/StratRules/Replay.good.cpp:413-470`) — the rules layer hands back no damage,
death, counter, or fame. The outcome is instead assembled from `strat::uiForecast` captured
**before** the submit plus the measured state delta **after**, both on one log line with an
agreement flag. **MILESTONE COMPLETE — all five planned phases CLOSED, no phase 6 was ever
scoped for it.** Phase 3 MET-AS-CORRECTED, gated three times — first two `VERDICT: BLOCK`, both
fixed in place, third `VERDICT: PASS` with zero findings — see "Phase 3" below for the full
history; phase 4 MET, gated once, `VERDICT: PASS`, zero findings, first gate — see "Phase 4"
below; phase 5 MET, the doc pass, gated three times (the phase itself, a narrow re-gate after
round-two corrections, a micro re-gate after a final four-word correction), all `VERDICT: PASS`,
zero findings — see "Phase 5" below. What the milestone delivered: the `STRAT-COMBAT` log
family mounted on `FStratBridge::Submit`; parity clauses proving the forecast agrees with the
measured delta, including the first-ever exercise of the `adied=1` counter-kill arm; an
AI-vs-AI headless pairing gate (`strat_combat_pairing_gate.py`) proving `STRAT-AI applied
kind=Attack` pairs 1:1 with `STRAT-COMBAT resolved` by ordered identity, with ten checked-in
fixtures proving it can fail; the same gate re-run against a live PIE corpus with `--pre-sliced`,
byte-identical to the headless run (host-independence discharged; content-independence carried
to NEXT); and a doc pass that corrected four stale citations in this file and in
`.agents/ue-project-context.md`, discharged the unexercised-guard comment debt, and recorded
that `strat::chooseBuild` makes buildlist repetition inert at the rules layer — a vendored-rules
observation for `stratocracy-crew`, not a defect here.

### Phase 1 — CLOSED (editor closed)

- **Completed:** 2026-08-13/14.
- **Exit criterion:** a `STRAT-COMBAT` log family exists, mounted where a future raw Attack that
  applies is guaranteed to be observed, with zero cross-contamination against `STRAT-CMD`/`STRAT-AI`.
  **Met**, gated three times (the phase itself, a comment-only correction pass, a capture-repair
  pass), all `VERDICT: PASS`, zero findings.
- New `Source/StratBridge/StratCombatLog.h` — `LogStratBridge` (`StratBridge`'s first `UE_LOG`
  category; the module carried zero `UE_LOG` calls before this), the plain non-`USTRUCT`
  `FStratCombatOutcome`, `EStratCombatDivergence` — plus `DEFINE_LOG_CATEGORY` and a
  `StratCombatObservation` block in `StratBridge.cpp`.
- **The emitter is on `FStratBridge::Submit`, not `SubmitAttack`** — recorded as the decision worth
  preserving because it forecloses the alternative. `SubmitAttackAtHex → SubmitAttack →
  SubmitStamped → Submit`, and `Submit` has exactly three non-declaration callers; two
  (`Tests/StratBridgeParity.cpp:340`, `Tests/StratBridgeSaveRecording.cpp:466`) hand it a **raw
  `strat::SaveCommand`** that bypasses both typed arms. Corrected framing, preserved over the
  first one: both raw sites submit deliberately illegal commands that refuse, so **no attack in
  the tree today applies without passing a typed arm** — `Submit` is the right mount because the
  guarantee concerns a *future* raw Attack that does apply, and observing in both places would
  double-count.
- Three log phrases — `resolved` / `refused` / `divergence` — with `divergence` emitted **beside**
  `resolved` so `grep -c "STRAT-COMBAT resolved"` stays a count of resolutions. Zero
  cross-contamination with `STRAT-CMD`/`STRAT-AI` (the families diverge at character 7). `refused`
  sits at `Log`, not `Warning`: the first suite run came back **85/0/0/2** because the automation
  framework charges warnings to the provoking test, and two existing clauses deliberately submit
  illegal attacks. `divergence` is deliberately at `Error` so a forecast disagreeing with its own
  resolution fails any suite observing it.
- **Two documented holes, both deliberate, both with their consequence recorded in the code:**
  `ReplayLog` does not pass through `Submit`, so replayed attacks emit nothing (in-process round
  trip is fine; **a log loaded from disk in a fresh process is a real gap**); an attack refused by
  the `!bSeeded` guard emits **no line at all**, so **`resolved + refused` is not the count of
  attacks submitted** — a trap left explicitly for the phase-3 pairing gate.

### Unplanned: a 1-in-4 test flake, found and fixed

`Stratocracy.StratPlay.T-UI-01.ClickedAttackIsAcceptedAndRecorded` failed **1 run in 4 on
byte-identical code** (`02.28.50` 87/0, `02.47.30` **86/1**, `02.52.27` 87/0, `02.53.40` 87/0),
passing in isolation: `Expected 'one accepted command emits exactly one line' to be 1, but it was
4`.

- **Cause:** `FOutputDeviceRedirector` queues lines it cannot broadcast on the primary-thread fast
  path, and `FlushBufferedItems` drains that queue to whichever devices sit in
  `BufferedOutputDevices` **at drain time** — a capture registering mid-stream inherits the
  queue's tail, and `GLog->Flush()` inside the capture's own `Settle()` triggered the drain that
  leaked into it. Verified in UE 5.8 source: `OutputDeviceRedirector.cpp:440-447` routes
  `CanBeUsedOnMultipleThreads()` devices to `UnbufferedOutputDevices`, `:553`/`:560` drain the
  buffered queue **only** to `BufferedOutputDevices`, `:905` is the synchronous in-call broadcast.
  Engine precedent: `AutomationTest.h:1345`, `:1396`.
- **Fix:** `virtual bool CanBeUsedOnMultipleThreads() const override { return true; }` plus an
  `FCriticalSection`, on **all four** captures — `FStratCmdCapture`
  (`StratHotSeatReplayParity.cpp`), both `FStratAiCapture`s (`StratAiMatchClauses.cpp`,
  `StratAiTurnRunnerClauses.cpp`), `FStratWaitCapture` (`StratSelectionWaitClauses.cpp`). **None of
  the four was sound**: the two AI captures bounded by *index* watermarks against a failure mode
  that bounds by *time*, and the `STRAT-WAIT` silence clause's hard `== 0` was the most exposed — a
  stray late line would have failed it with nothing wrong in the module. Those AI captures back the
  `STRAT-AI` clauses phase D closed on; nothing there is known wrong, but the instrument under them
  was less trustworthy than the record claimed. **The fix is strictly strengthening** — under the
  buffered path an in-window line could be drained after `RemoveOutputDevice` and be *missed*.
- **Evidence:** eight consecutive full-suite greens at 87/0/0/0 across distinct timestamps
  (`03.18.51` through `03.28.30`), plus the source-level proof, which is the load-bearing part —
  the flake was never made deterministic and the author said so plainly rather than dressing up a
  probe that measured concurrency instead of leakage.
- **Residual debt, non-gating:** the `FCriticalSection` covers the single mutation and **none of
  the ~20 reads**, safe only because every `STRAT-*` emitter is on the game thread — a property of
  the emitters that nothing pins. Two of the four files disclose this; two add the lock silently.
  Also, two of four filters are case-insensitive and two explicitly `CaseSensitive`.

### Phase 2 — CLOSED (gated 2026-08-14, `VERDICT: PASS`, zero findings)

- **Completed:** 2026-08-14. Gated by `strat-integration-reviewer`, **`VERDICT: PASS`, zero
  findings**, 2026-08-14. The withholding in the previous entry was correct while no verdict
  existed; a verdict now exists and this heading records it landed, not pending.
- **Exit criterion:** parity clauses proving the combat-outcome forecast agrees with the measured
  state delta, including the previously-unexercised counter-kill arm. **Met** — six clauses, all in
  new `Source/StratBridge/Tests/StratCombatOutcomeParity.cpp`. Suite **87 → 93**, three consecutive
  runs at 93/0/0/0, 93 entries, none downgraded. The gate independently confirmed each of the six
  clauses can go red.
- Names and properties: `T-UI-01.CombatOutcomeAgreesWithForecast` (measured HP == forecast HP read
  three ways, oracle `strat::uiResolveForGate`), `.CounterKillLeavesTheAttackerOffTheRoster` (**the
  `adied=1` arm, which had never executed in this project's history**), `T-FAME-01.AttackerFameMovesOnAKill`
  (against `strat::killAward`), `T-SAVE-06.RawAttackThroughSubmitIsObserved` (a raw Attack that
  **applies**, which the pre-existing raw site could not prove since it refuses — **and the only
  thing in the tree defending the emitter's placement on `Submit`; if this clause is ever deleted,
  the emitter can migrate to a typed arm with the suite green**), `T-UI-01.RefusedAttackCarriesTheDistinctPhrase`,
  `.UnseededAttackIsObservedByNothing` (both with positive controls before the absence read).
- **The open item that matters most, and the gate's most transferable result: the
  `divergence`/`agree=0` and `LegalityDisagrees` arms are genuinely unreachable — confirmed by the
  gate against the vendored sources rather than accepted from the author.**
  `FStratBridge::CombatUnitOf` (`StratBridge.cpp:655-672`) is field-for-field `strat::combatUnit`
  (`Replay.good.cpp:43-51`); `uiForecast` and `applyCommand` call the same
  `resolveDamage`/`defenderCanCounter` over identical stat blocks and the same terrain source; all
  five `uiForecast` illegalities are also refused by `applyCommand`, which refuses on two further
  counts besides, so it is strictly more refusing. A forecast-illegal attack that applies is not
  constructible (74 Attacks measured, `illegalForecastApplied` = 0). **Writing no foil was ruled
  correct — record this so a future reader does not mistake the missing clause for an oversight and
  "fix" it with something unfalsifiable.** Proposed seam for the engineer, unchanged: promote
  `StratLossAgrees` and `StratDivergenceMaskOf` out of the file-local `StratCombatObservation`
  namespace into `StratCombatLog.h` as engine-typed free functions, so a clause can hand-build an
  `FStratCombatOutcome` with a wrong measured HP and assert the mask directly. Separately,
  `agree=-1` (all three of `CaptureAfter`'s early returns) is **dead code today** and judged not
  worth a production seam.
- **Three debts recorded by the gate, none gating, all owed to `strat-test-author`'s lane:**
  - A case-insensitive comparison survives at `StratCombatOutcomeParity.cpp:1312` —
    `TestEqual(..., Unobserved.Reason, FString(TEXT("no scenario is loaded")))`. `TestEqual` on
    `FString` is ignore-case in UE 5.8 (the standing project-wide gotcha), so a guard re-spelled
    `"No Scenario Is Loaded"` would slip through. Not that clause's teeth — the absence assertions
    below it are all `ESearchCase::CaseSensitive` — but it is the one place in the file the standing
    rule is unapplied.
  - `hex=` is matched as a prefix at `:1039-1041` — `Contains("hex=3,4")` is also satisfied by
    `hex=3,45`. Both coordinates come from the fixture so the risk is theoretical; a trailing-space
    match closes it.
  - The `fdmg`/`fcdmg` comparisons at `:508-511` are weaker than the prose suggests — both sides
    originate in `uiForecast` over equivalent state, so they pin field *transposition* in the
    emitter rather than the damage figure. The clause's real independence is at `:476-479`. The code
    is right; the wording overstates it.

### Phase 3 — CLOSED (editor closed), MET-AS-CORRECTED

- **Completed:** 2026-08-14.
- **Exit criterion as WRITTEN in the brief:** "AI-vs-AI headless proof that every
  `STRAT-CMD accepted kind=Attack` pairs 1:1 with a `STRAT-COMBAT resolved`, counts derived by a
  named command, and it must account for the `!bSeeded` silent path." **That criterion names the
  wrong pairing and cannot be met as written** — `STRAT-CMD accepted` is the HUMAN selection
  applier's line (`StratSelectionMachine.cpp`); the AI path never emits it
  (`StratAiTurnRunner.cpp:242-247`: routing AI commands through `StratSubmitSelectionCommand`
  would mean inventing a click). Measured over the AI-vs-AI slice:
  `grep -c "STRAT-CMD accepted kind=Attack" slice.log` → **0**. No gate can pair a line the
  corpus never contains.
- **MET-AS-CORRECTED.** The real pairing — `STRAT-AI applied kind=Attack` ↔ `STRAT-COMBAT
  resolved` — is proven 1:1 by **ordered identity** on `(unit, hex)` vs `(attacker, hex)`, not
  merely by matching totals, by `Tools/architect/strat_combat_pairing_gate.py`.
- **Gated THREE times, 2026-08-14. `VERDICT: PASS`, zero findings, on the third — PHASE 3 IS
  CLOSED.** Full history, honestly kept rather than only the final verdict:
  - **First gate: `VERDICT: BLOCK`, one finding.** The gate and fixtures were correct; the finding
    was a prose miscount in this file — "eight distinct FAIL fixtures plus two PASS controls,"
    naming a checked-in fixture that does not exist and omitting `pass_terminal_refusal_ok.log`
    from the PASS list. Fixed in place, corrected below (see "Ten checked-in fixtures...").
    No code, fixture, or gate-script change was implied or made.
  - **Second gate, on the fix for the first: `VERDICT: BLOCK`, one finding, in this file again.**
    The reviewer independently re-derived every number in the corrected passage (7 fail / 3 pass,
    7×`EXIT=1` / 3×`EXIT=0`, 11 in-script assertions with "unmatched resolved line" as case 4) and
    confirmed all of it correct, then found that this file's OWN mid-file summary line still read
    "phase 3 MET-AS-CORRECTED, no reviewer verdict yet" after this same file's own phase-3 entry
    had already recorded the first `BLOCK`. **This is the exact mistake `state.md` already
    recorded paying for once, in the phase-D entry above** ("this file and the phase evidence
    blackboard both claiming 'no reviewer verdict exists yet' a hundred-plus lines after a
    `BLOCK` verdict on the same piece was already recorded in the same file") — the same
    bookkeeping class the NEXT section's lesson names explicitly ("this applies to counts, to
    'has a verdict run yet' bookkeeping, and to slice boundaries alike"), repeated here despite
    being written down as a thing to derive rather than carry forward. Fixed in place. No code,
    fixture, or gate-script change was implied or made by this finding either.
  - **Third gate: `VERDICT: PASS`, zero findings.** The reviewer re-ran the whole-file
    verdict-status grep itself across 28 sites, confirmed no phase-3 `PASS` claim existed anywhere
    before this gate, that no true statement had been deleted by either fix, and that only the two
    documents in this steward's lane had moved (file-mtime scan). This entry is that closing
    record.
  - **The general lesson, restated because it recurred once even after being written down twice:**
    a "has a gate run yet" / "what did it say" claim is exactly as checkable against the file's
    own history as a fixture count is against a directory. Before writing ANY verdict-status
    sentence in this file, run:
    ```
    grep -n "gated\|VERDICT\|verdict" Tools/architect/state.md
    ```
    and make every site consistent — not just the one nearest the edit. This is the literal
    command that caught the second finding; run it, do not re-derive that it should be run.
- **Seam extraction, landed by `strat-gameplay-engineer` ahead of this piece (piece (a)):**
  `Source/StratBridge/StratCombatLog.h` gained `StratLossAgrees` and `StratDivergenceMaskOf` as
  two engine-typed, undecorated, in-module-only free functions; `CaptureAfter` now calls them
  instead of holding a second copy of the rule. **Correction to this file's own prior wording:**
  the phase-2 entry above said "promote `StratLossAgrees` and `StratDivergenceMaskOf`" as if both
  already existed as named functions — only `StratLossAgrees` did; `StratDivergenceMaskOf`'s
  logic was seventeen inline lines inside `CaptureAfter`, so its half of this work was an
  EXTRACTION, not a promotion of an existing name. Recorded here so a future reader does not
  repeat the same imprecision this file itself carried forward once already.
  `StratDivergenceMaskOf` is deliberately PARTIAL — its precondition is that the outcome is
  already known measurable, and the measurability guard itself stays in `CaptureAfter` rather
  than folding into the mask function, precisely so `ForecastAgrees`'s three-state result
  (agree / disagree / unmeasurable) cannot collapse to two. Moving the guard into the promoted
  function would make it unable to distinguish "agreed" from "could not be measured."
- **Test suite, landed ahead of this piece (piece (b)):** 10 new clauses in a new
  `Source/StratBridge/Tests/StratDivergenceRuleClauses.cpp`, each demonstrated red under a
  mutated expectation and green again on restored bytes. Suite **93 → 103**,
  `succeeded=103 failed=0 notRun=0` (`Saved/AutomationReport/index.json`, read `utf-8-sig`).
  Count method: set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, per the standing project
  rule for counting new clauses.
- **The gate itself (piece (c), this entry):** `Tools/architect/strat_combat_pairing_gate.py`.
  Parses both the `STRAT-COMBAT` family (anchored on the `LogStratBridge:` category, three
  shapes: `resolved` / `refused` / `divergence`, plus a `StratCombatParseFailure` for anything
  else) and reuses `strat_ai_log_gate`'s `STRAT-AI` parser — one copy of that parsing rule, not a
  second one. Slices a log to one test's own `Test Started.` / `Test Completed.` markers, keyed
  on the test's full `Path={...}`; a missing or unterminated marker pair is a **hard failure**,
  never a silent scan to EOF (this log has no `Log file closed` line at all — it ends `**** TEST
  COMPLETE. EXIT CODE: 0 ****`). Asserts the pairing by **ordered identity**, not count-equality:
  position *i*'s `STRAT-AI applied kind=Attack (unit, hex)` must equal position *i*'s
  `STRAT-COMBAT resolved (attacker, hex)`, and a length mismatch between the two lists is itself
  a failure — the structural net under the `!bSeeded` silent path and the `ReplayLog` gap, rather
  than a floor check that would wave a real gap through as a harmless miscount.
- **Ten checked-in fixtures under `Tools/architect/evidence/08-combat-pairing-gate/fixtures/`
  prove the gate can genuinely FAIL, each via the real CLI, not a mocked function.** Split derived
  directly from the directory, not counted by eye: `ls .../fixtures/ | grep -c '^fail_'` → **7**,
  `ls .../fixtures/ | grep -c '^pass_'` → **3** (10 total, matching `gate_self_test_output.txt`'s
  seven `EXIT=1` blocks and three `EXIT=0` blocks). The seven FAIL fixtures: an ordering-scrambled
  corpus with an unchanged multiset (the case a count-only or set-only check would pass, `fail_ordering_scramble.log`),
  an unmatched applied attack — the silent-path shape (`fail_silent_path_gap.log`), a malformed
  `STRAT-COMBAT` line (`fail_format_drift.log`), a `divergence` line (`fail_divergence_present.log`),
  a non-terminal `STRAT-AI refused` line (`fail_blocking_ai_refusal.log`), no markers at all
  (`fail_missing_markers.log`), and a truncated (`Started` with no `Completed`) log
  (`fail_truncated_no_completed.log`). The three PASS controls, named in full: `pass_clean_pairing.log`,
  `pass_terminal_refusal_ok.log` (the lone `[T-SAVE-05]` terminal handover refusal alone does not
  fail the gate), and `pass_commentary_echo_ignored.log` (a `LogAutomationController:`-echoed
  `STRAT-COMBAT` line proven ignored by the category anchor). **Correction to this file's own
  prior wording:** an earlier pass of this entry said "eight distinct FAIL fixtures plus two PASS
  controls," naming an "unmatched resolved line" fixture that is not checked in — that scenario
  exists only as case 4 of the in-script `check_self_test()`, over an inline corpus built with
  `tempfile`, not among the checked-in `.log` files this sentence was describing — and omitting
  `pass_terminal_refusal_ok.log` from the PASS list. The two errors summed to the right total (10)
  by cancelling, which is how it passed unnoticed the first time; caught by the reviewer's gate on
  phase 3, `VERDICT: BLOCK`, one finding, 2026-08-14. **The in-script `check_self_test()` corpora
  and the checked-in CLI fixtures are two distinct sets and must not be totalled together again** —
  `check_self_test()` exercises 11 assertions (including the unmatched-resolved-line case that has
  no checked-in counterpart), the fixtures directory holds 10 files, and neither number is the
  other. `gate_self_test_output.txt` is every CHECKED-IN fixture's real, unedited CLI output and
  exit code; `self_test_internal_fixtures_output.txt` is the separate `--self-test` run.
- **Measured against the real log, verbatim commands recorded in
  `evidence/08-combat-pairing-gate/blackboard.md`:**
  `Saved/Logs/Stratocracy.log`, 5169 lines, session opened `08/14/26 10:16:47`. Slice bounded by
  `Test Started.`/`Test Completed.` for
  `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound`, found at lines
  **4179..4474** by `grep -n ... | grep -E "Test Started|Test Completed"`.
  `STRAT-AI applied kind=Attack` = **68**, `STRAT-COMBAT resolved` = **68**, `STRAT-AI refused`
  = **1** (kind=EndTurn, `[T-SAVE-05] no match is running` — the known terminal handover
  refusal), `STRAT-COMBAT divergence` = **0**, `STRAT-COMBAT refused` = **0**. Kind census on
  `STRAT-AI applied`: 68 Attack / 55 Move / 22 Build / 11 EndTurn. Whole-log (unsliced)
  `STRAT-COMBAT resolved` count is **493** — the other 425 are bridge/fixture tests that submit
  directly and emit no `STRAT-AI` line, confirming slicing is mandatory and not optional
  ergonomics. The gate against the full log and against a standalone checked-in copy of just the
  slice (`real-run/t-int-05-both-sides-ai-slice.log`) report **identical figures and PASS**,
  confirming the checked-in slice is a faithful, self-contained copy of what was graded.
- **The `!bSeeded` silent path fired zero times in this corpus** — a measurement, not a
  guarantee — and is handled structurally by TWO independent nets, not one:
  1. The pairing's own length-mismatch arm: `applied_attack_count == combat_resolved_count`
     (measured 68 == 68) is what a future firing of that path would break, and the gate's
     length-mismatch check (proven able to fire by `fail_silent_path_gap.log`) reports a real gap
     with the cause named in the message rather than a silent miscount.
  2. An unseeded `Submit` still produces an observable line at the AI layer even though it
     produces none at the combat layer: `Port.Submit` returning false makes
     `StratAiTurnRunner.cpp` emit `STRAT-AI refused phase=apply kind=Attack ...`, and
     `is_terminal_handover_refusal` will NOT whitelist that shape (it matches only
     `phase=apply kind=EndTurn` with the `[T-SAVE-05]` reason) — so it surfaces as a blocking
     `STRAT-AI` refusal, proven able to fire by `fail_blocking_ai_refusal.log`.
- **The corrected pairing, confirmed by THREE independent derivations, not one:** the source read
  of `StratAiTurnRunner.cpp:242-247` (AI commands structurally cannot emit `STRAT-CMD`), the
  `grep -c` counts on the real slice (0 / 68 / 68 against a 493 whole-log figure), and the gate's
  own pass/fail behaviour across ten checked-in fixtures — 7 fail_ / 3 pass_, per
  `ls .../fixtures/ | grep -c '^fail_'` / `'^pass_'` (correctly PASS on the clean and
  terminal-refusal-only corpora, correctly FAIL on the seven fault shapes, including one — the
  ordering scramble — that a count-only check would have missed).
- **Carried forward, unchanged:** `ReplayLog` does not route through `Submit` — a combat log
  loaded from disk in a fresh process emits no `STRAT-COMBAT` line at all; this gate reads a
  live-session log only and says nothing about a replayed one. Still open.
- **Handed to phase 4, unchanged from the milestone's original scoping:** phase 4 (editor OPEN)
  and phase 5 (the doc pass) remain not started. Phase 4's carried-forward constraint: drive it
  with `RunAiTurnsNow` so no simulated input is needed, routing around the standing NeoStack
  injection blocker, and slice evidence by session markers — never to EOF, the same discipline
  this phase's own gate enforces on `Test Started.`/`Test Completed.` markers.
- **Deferred debt, two items, neither gating, both recorded rather than fixed now:**
  - **`StratDivergenceRuleClauses.cpp:12` cites 483 resolutions, restated without a run anchor.**
    That figure is phase 1's (`483 resolutions, 49 kills, agreement on every one`); the current
    session log this phase measured against carries 493 `STRAT-COMBAT resolved` lines
    (`grep -c "STRAT-COMBAT resolved" Saved/Logs/Stratocracy.log`). The two numbers are not in
    conflict — different runs measure different totals as the suite grows — but the comment
    presents 483 as if it were this run's count with nothing tying it to the run it actually came
    from. Owner: `strat-test-author`'s lane (`Source/StratBridge/Tests/`).
  - **The ten checked-in fixtures under `evidence/08-combat-pairing-gate/fixtures/` have no
    re-runnable assertion binding each `.log` to its expected verdict.** `gate_self_test_output.txt`
    records what each one returned at the time it was captured; nothing re-checks that a future
    parser edit still produces the same verdict on the same bytes, so a regression there could
    leave the recorded table silently stale. Relatedly, running any of the ten fixtures without
    `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` fails it for the wrong reason (no marker
    found for the default `T-INT-05` path) with nothing in the fixtures directory itself saying
    so — a reader has to already know to pass that flag. Owner: this steward's own lane
    (`Tools/architect/`) — a future pass should either add a small assertion harness over the
    fixtures directory or a `README` in it stating the required flag.
- **Findings outside this steward's lane, recorded here as flags, not tasks:** the gate also
  found a stale `LegalityDisagrees` doc arm in `Source/StratBridge/StratCombatLog.h:171` (owed to
  `strat-gameplay-engineer`'s phase-5 doc pass) and `.agents/ue-project-context.md:195` still
  reading "93/93" when the suite is now 103/103 (no crew agent owns that file — see the
  standing flag under NEXT above). **Correction to this bullet's own prior wording:** it named the
  path `Source/StratCombatLog.h` — no such file (`ls Source/StratCombatLog.h` → "No such file or
  directory"). The file is `Source/StratBridge/StratCombatLog.h`; verified with
  `ls Source/StratBridge/StratCombatLog.h` and `grep -n "LegalityDisagrees"
  Source/StratBridge/StratCombatLog.h` → `171: LegalityDisagrees = 1 << 2,`, confirming both the
  path and the line the arm actually sits on before writing this correction. Caught by the
  fourth gate; not introduced by the third gate's fix, and pre-existing in this entry since it
  was first written.

### Phase 4 — CLOSED (editor OPEN), MET

- **Completed:** 2026-08-14.
- **Exit criterion:** the live PIE path, so no simulated input is needed, routing around the
  standing NeoStack injection blocker. **MET.** **Correction to this line's own wording:**
  nothing external calls `RunAiTurnsNow` — it has no `UFUNCTION` and is unreachable from Lua. The
  real chain is `AStratGameMode::BeginPlay` → `RunAiTurnsIfDue` → the paced timer →
  `RunAiTurnsNow` internally, once a turn is due. The limits section below already states this
  correctly (the entry was self-correcting); this line is fixed to match rather than invite a
  reader to think an agent invoked `RunAiTurnsNow` directly.
- **Gated 2026-08-14, `VERDICT: PASS`, zero findings, FIRST GATE — the only phase in this
  milestone that did not need a re-gate.** The reviewer re-derived everything independently as a
  third derivation: the slice diffs identical to `sed -n '3462,3727p'`, contamination clean
  (first `STRAT-` line is 3481, after the 3462 cut), counts 68/68/0/0/0 plus the one
  `[T-SAVE-05]` refusal, 68× `agree=1 diverge=0`, the 11-hash diff at exit 0, and
  `fameBefore=200 fameAfter=700` verbatim — plus confirmed no screenshot substitution anywhere
  and that `--pre-sliced` leaves the pairing loop, both parsers, and `GateResult.passed`
  byte-unchanged. Before writing this line, `grep -n "gated\|VERDICT\|verdict"
  Tools/architect/state.md` was run (per the standing lesson, now paid for three times in this
  file's own phase-3 history) and the top-of-file banner and mid-file summary were checked
  alongside this entry — the exact site that produced a `BLOCK` last time — and updated to match.
- **The corpus:** `Saved/Logs/Stratocracy_2.log` — a NEW file after a Claude Code restart. No
  line number was carried forward from `Stratocracy.log`; everything below was re-derived
  directly against `Stratocracy_2.log`.
- **Markers, verified rather than trusted, all four checked with `awk`/`grep` before use:**
  open `3462: LogPlayLevel: Creating play world package: .../Lvl_FerrumCrossing`,
  `3469: LogLoad: Game class is 'BP_StratGameMode_AiVsAi_C'`; close
  `3724: LogWorld: BeginTearingDown for .../Lvl_FerrumCrossing`,
  `3727: LogPlayLevel: Display: Shutting down PIE online subsystems`. **Cut `3462 -> 3727`**
  (the builder's recommendation): this puts the `Game class is 'BP_StratGameMode_AiVsAi_C'` line
  INSIDE the slice, so the corpus identifies its own GameMode rather than relying on a claim in a
  narrative.
- **The contamination check, verified rather than assumed.** `grep -n "Game class is"
  Saved/Logs/Stratocracy_2.log` finds it twice — an earlier, pre-travel PIE session under the
  shipping GameMode (`BP_StratGameMode_C` at line 3427, travel to `Lvl_FerrumCrossing` again at
  3462 under `BP_StratGameMode_AiVsAi_C`). `grep -n "STRAT-" Saved/Logs/Stratocracy_2.log | awk
  -F: '$1<3462' | wc -l` → **0**. No `STRAT-*` line exists before the slice boundary, so the
  earlier session produced none and there is no contamination — measured, not inferred, because
  this is exactly the class of boundary claim this project has been wrong about before.
- **Slice checked in:** `Tools/architect/evidence/08-combat-pairing-gate/pie-run/ai-vs-ai-pie-session-slice.log`,
  cut by `sed -n '3462,3727p'`, 266 lines, named for what it is (one PIE session's own AI-vs-AI
  match). First and last lines are the open/close markers themselves.
- **Counts, all re-derived, all matching the builder's report exactly:**
  `STRAT-AI applied kind=Attack` = **68**, `STRAT-COMBAT resolved` = **68**,
  `STRAT-COMBAT refused` = **0**, `STRAT-COMBAT divergence` = **0**, `STRAT-CMD accepted` (any
  kind) = **0**, `STRAT-AI refused` = **1** (kind=EndTurn, `[T-SAVE-05] no match is running`).
  Every one of the 68 `resolved` lines reads `agree=1 diverge=0`. Kind census: 68 Attack / 55
  Move / 22 Build / 11 EndTurn. The three zeros carry a positive control from the same
  instrument on the same file: `resolved`=68 proves the grep can see `STRAT-COMBAT` lines, so a
  zero elsewhere is a real absence, not a blind grep. `STRAT-COMBAT resolved` confirmed emitted
  BEFORE its `STRAT-AI applied` partner throughout this corpus (checked directly, not assumed
  from phase 3's) — the bridge logs during `Submit`, the runner after.
- **The gate's first PIE corpus.** `strat_combat_pairing_gate.py`'s slicer was built for
  headless automation logs and hard-requires `Test Started.`/`Test Completed.` markers, which a
  PIE session log carries none of. Added `--pre-sliced` (mirrors `strat_ai_log_gate.py`'s own
  established posture of "point this at an ALREADY isolated log"): the entire given file is
  treated as the slice and the marker search is skipped; the pairing logic, parsers, and every
  failure mode are unchanged. Proven backward-compatible (the unaffected phase-3 corpus,
  relocated by log rotation to `Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log`, still
  gates clean; all ten checked-in fixtures still return the same seven `EXIT=1`/three `EXIT=0`)
  and proven able to fail in the new mode too — self-test grew from 11 to **13** fixtures, the
  two new ones exercising `--pre-sliced` specifically (a clean corpus passes with no automation
  markers at all; an ordering-scramble corpus still fails).
- **Gate run against the checked-in slice, gate output verbatim:**
  ```
  STRAT-COMBAT pairing gate: .../pie-run/ai-vs-ai-pie-session-slice.log
    test:                       (pre-sliced input, no automation markers)
    slice (1-based lines):      1..266
    STRAT-AI applied kind=Attack:  68
    STRAT-COMBAT resolved:         68
    STRAT-COMBAT refused:          0
    STRAT-COMBAT divergence:       0
    STRAT-AI terminal refusals:    1
    STRAT-AI blocking refusals:    0
    STRAT-COMBAT parse failures:   0
    STRAT-AI parse failures:       0
    pairing mismatches:            0
  PASS
  ```
  Full output in `evidence/08-combat-pairing-gate/pie-run/gate_output.txt`.
- **Host independence, confirmed by hash diff — NOT content independence, and this run does not
  claim to be.** 68/68 is EXACTLY phase 3's headless number; with deterministic rules, a
  deterministic AI, and the same scenario/buildlist/first side, that is expected — the same game
  replayed through a different host, not a new game. Settled by measurement: all eleven
  `STRAT-AI turn-ended` hashes (turn/side/commands/hash, four fields each) diffed between this
  slice and phase 3's headless corpus (`evidence/08-combat-pairing-gate/real-run/t-int-05-both-sides-ai-slice.log`)
  are **byte-identical** — `diff pie_hashes.txt headless_hashes.txt` produces no output, exit
  code 0. Full listing in `evidence/08-combat-pairing-gate/pie-run/hash_diff.txt`. **This
  confirms the same live-PIE path with no simulated input reaches the same result as headless
  automation. Coordinator's ruling, recorded as written: the phase-D "avoid one-corpus proof"
  precedent is PARTLY discharged, not fully and not "still fully open."** The HOST-independence
  half — the same game surviving a different host (PIE vs. headless automation) — IS discharged
  by this run's identical-hash result. The CONTENT-independence half — a different scenario,
  buildlist, or first side, genuinely different game content rather than a different way of
  driving the same one — is NOT discharged and is out of phase 4's scope; it is carried to NEXT
  as its own future work, not folded into this phase's closure. The identical-hash result makes
  that remaining limit concrete rather than theoretical: this project now has two runs of the
  exact same game through two different hosts, not two different games.
- **Two honest limits, self-reported by the builder, recorded rather than smoothed over:**
  1. **`AiTurnDelaySeconds` does not pace per turn.** The whole game ran in ~4 ms
     (`16.15.29:141` → `:145`, confirmed directly on the slice). The delay paces *entry into*
     `RunAiTurnsIfDue`; once inside, `RunAiTurnsNow` — "plays every consecutive AI turn that is
     due, synchronously, and reconciles after," per its own doc comment in
     `StratMatchSubsystem.h` — plays every remaining AI turn in one call. **Cited by the
     function's doc comment, not a line number** (the standing "cite functions, not line numbers"
     lesson: a line-number citation is accurate today and rots on the next edit to that file).
     **Consequence: there are no distinct seed / mid-progression / result screenshots of this
     match** — confirmed: no screenshot in `Saved/Screenshots/WindowsEditor/` postdates phase D's
     captures (`ScreenShot00042.png`/`00043.png`, both dated Aug 13 20:37, already checked into
     `evidence/07-ai-opponent/pie-screenshots/`). **The builder explicitly refused to present the
     earlier shipping-GameMode session's pristine seed image as this match's seed — recorded here
     approvingly; that is the correct call and the opposite of what this project has been burned
     by** (a confounded control standing in for the real one — the hot-seat phase-6 precedent).
     This also corrects phase D's "0.5 s pacing" reading as it applies to this run: the pacing is
     real, but it is not per-turn once a paced entry fires.
     **A committed message this correction cannot reach:** `eda3b4b` ("An AI-vs-AI GameMode, so
     the corpus does not come from a config that exists nowhere") carries the premise
     "`AiTurnDelaySeconds` stays 0.5 so turns pace as phase D measured and mid-progression
     screenshots mean something." This run falsifies that premise — there is no per-turn pacing
     once `RunAiTurnsIfDue` fires, so no mid-progression screenshot from this path means what
     that message says it means. The commit message is immutable; this paragraph is the
     correction, written down plainly enough that a reader of `git log` alone, without this file,
     is still misled by that one message and needs to find this correction to know it.
  2. **No result tier is logged.** No `STRAT-*` line carries one, and the match subsystem is
     unreachable from Lua. The observable finish evidence is the turn count plus the
     `[T-SAVE-05] no match is running` signature at whole-file lines 3716-3717 — which is what a
     CORRECT §2.8 finish looks like, not a fault (the same terminal-refusal shape phases 1 and 3
     already established). Reached turn 6; eleven `STRAT-AI turn-ended` lines; the killing blow
     at whole-file line 3714 took side 1's fame `fameBefore=200` → `fameAfter=700`, confirmed
     verbatim by `sed -n '3714,3717p' Saved/Logs/Stratocracy_2.log`.
- **Log-rotation note, not a finding:** `Saved/Logs/Stratocracy.log` (no `_2`) was rotated
  between phase 3 and phase 4 by the editor restart — it is now a short, unrelated 3492-line
  session with zero occurrences of `BothSidesAiReachesAResultWithinTheBound`. Not data loss: the
  byte-identical phase-3 content is preserved at
  `Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log`, confirmed still gate-clean. Nothing in
  phase 3's evidence or `state.md` entry depends on the live `Stratocracy.log` path remaining
  stable.
- **Evidence:** `Tools/architect/evidence/08-combat-pairing-gate/pie-run/` — the checked-in
  slice, `gate_output.txt`, `hash_diff.txt`, and `blackboard.md` recording all of the above with
  the commands run beside every number.
- **`.agents/ue-project-context.md` drift, unowned by any crew agent, flag only:** `:195` still
  reads "93/93" against an actual 103; `:198`'s "Measured in phase 4" refers to the HOT-SEAT
  milestone's phase 4 and now collides in name with this combat-outcome milestone's own phase 4 —
  worth qualifying by milestone name whenever that file is next touched. No crew agent owns
  `.agents/`; this is a flag, not a task this steward takes.
- **Deferred, out of phase 4's scope, its own future work:** a content-independent corpus — a
  different scenario, buildlist, or first side — to discharge the content-independence half of
  the phase-D "avoid one-corpus proof" precedent (the host-independence half is now discharged —
  see the hash-diff bullet above). This phase's identical-hash result shows that remaining half
  is concretely, not merely theoretically, open.
- **Gate debt, real and non-gating, NOT to be built now (scope fence stands; phase 5 is the doc
  pass) — recorded so it is discharged before this gate is pointed at a corpus anyone has not
  eyeballed.** `--pre-sliced` returns `PASS` and exit 0 on an EMPTY or WRONG corpus. Measured
  directly: an empty file through `--pre-sliced` prints `slice (1-based lines): 1..0`, every
  count zero, `PASS`, `EXIT=0`. The marker-slicing path hard-fails on a wrong file via
  `SliceFailure`; `--pre-sliced` has no equivalent self-identification check, and
  `GateResult.passed` carries no non-zero floor — zero applied attacks and zero resolved lines
  currently reads as a trivially-satisfied pairing rather than "nothing was actually checked."
  **This does not touch phase 4's result** — that slice is content-bounded, carries both boundary
  markers as its own first and last lines, and has 68 real pairs — but the flag trades a
  structural corpus-identity guard for caller discipline, and neither this file nor the pie-run
  blackboard previously recorded that trade. This project has already shipped a commit titled "a
  gate whose own fixtures could not fail it"; a known silent-pass in the instrument this
  milestone's results rest on is exactly that class of debt. **Owed shape, per the reviewer:** a
  `--expect-min-pairs N` argument, or an outright refusal on a zero-event pre-sliced corpus, plus
  a 14th self-test fixture proving the new guard can itself fail. Owner: this steward's own lane
  (`Tools/architect/`), next time the gate script is touched — not this phase.

### Phase 5 — CLOSED (editor closed throughout), MET

- **Completed:** 2026-08-14. Editor closed for the whole phase; it is still closed.
- **Exit criterion:** the doc pass — correct stale citations this file and `.agents/` carried
  from earlier phases, discharge named comment debts, keep the diff comment-only. **Met.**
- **Gated three times, all `VERDICT: PASS`, zero findings each: the phase itself, a narrow
  re-gate after round-two corrections, and a micro re-gate after a final four-word correction.**

**Context carried into the phase, for the record — this is what phase 5 was scoped against,
not what it produced (the closure evidence follows below):**

- **`Combat.h::resolveDamage`/`defenderCanCounter` is the sole producer of `uiForecast`
  (`Ui.h:345-350`), which is what makes "the forecast is what resolves" structural rather than
  incidental; phase 1 is the first time that claim has been measured in-engine rather than
  asserted — 483 resolutions, 49 kills, agreement on every one.
- **Phase-3 seam, carried forward from phase 2 — DISCHARGED in phase 3.** `StratLossAgrees` /
  `StratDivergenceMaskOf` are now engine-typed free functions in `StratCombatLog.h`
  (`strat-gameplay-engineer`, piece (a)), and 10 clauses in `StratDivergenceRuleClauses.cpp`
  (`strat-test-author`, piece (b)) hand-build a foil for the `divergence` mask rather than
  leaving it permanently unfalsifiable. See "Phase 3 — CLOSED" above.
- **Phase-3 pairing-gate trap, carried forward from phase 1 — DISCHARGED in phase 3, AND
  RESTATED.** The original wording assumed the pairing was `STRAT-CMD accepted kind=Attack` ↔
  `STRAT-COMBAT resolved`; that pairing is unreachable in an AI-vs-AI match (`STRAT-CMD accepted`
  is human-only, measured 0 occurrences — see "Phase 3 — CLOSED" above). The gate that actually
  ships accounts for the `!bSeeded` silent path structurally, against the real pairing
  (`STRAT-AI applied kind=Attack` ↔ `STRAT-COMBAT resolved`): a length mismatch between the two
  event lists is itself a failure, proven able to fire by `fail_silent_path_gap.log` in
  `evidence/08-combat-pairing-gate/fixtures/`, and measured zero hits in the real corpus.
- **Phase-1 hole carried forward:** `ReplayLog` does not route through `Submit`, so a combat log
  loaded from disk in a fresh process emits nothing — in-process replay is unaffected.
- **The `GLog`-capture requirement (`CanBeUsedOnMultipleThreads`) now lives in
  `.agents/ue-project-context.md`**, not only in this file's "Unplanned" section above — the
  coordinator added a paragraph there recording the measurement and the caveat that the lock
  guards the append only, not the ~20 reads. Recorded here so the next test author authoring a
  `GLog` capture finds the constraint where they would actually look, rather than only in a
  milestone history entry.

**Closure evidence — re-derived independently by this steward, not taken from any dispatch
prompt:**

- **Diff, verified against `HEAD` `ae2f22a`:** `git diff --shortstat ae2f22a` →
  `9 files changed, 231 insertions(+), 23 deletions(-)`; `git status --porcelain` shows all nine
  as unstaged `M`, nothing staged, no untracked files; `git rev-parse HEAD` still `ae2f22a`.
  Files: engineer's lane (`Source/` outside `Tests/`) — `StratBridge/StratBridge.h`,
  `StratBridge/StratCombatLog.h`, `StratPlay/StratPlayerController.h`,
  `StratPlay/StratSelectionMachine.cpp`, `StratUI/StratViewModel.h`; test-author's lane
  (`Tests/` only) — `StratBridge/Tests/StratDivergenceRuleClauses.cpp`,
  `StratPlay/Tests/StratAiMatchClauses.cpp`, `StratPlay/Tests/StratHotSeatReplayParity.cpp`,
  `StratPlay/Tests/StratSelectionWaitClauses.cpp`.
- **Provably comment-only, by two independent measurements, both re-run here:** per-file
  `/*`/`*/` counts (`grep -o` on both markers) are identical HEAD-vs-worktree in all nine files
  — no hunk opened or closed a block comment. Separately, every `+`/`-` line across all nine
  files' diffs against `ae2f22a` (254 lines total, counted by `git diff | grep -cE
  '^[+-][^+-]'` per file, summing to 254) was tested against a comment-body shape
  (`^\s*(//|/\*|\*|$)` after stripping the diff marker): **zero** lines fail that test.
- **Suite: 103 succeeded / 0 succeededWithWarnings / 0 failed / 0 notRun**, read directly from
  `Saved/AutomationReport/index.json` as `utf-8-sig` (first bytes `EF BB BF`, per the standing
  project note). `reportCreatedOn 2026.08.14-17.44.15`. Walking every leaf test entry gives 103
  entries, all `state: "Success"` — the full census, not just the top-line count, so a state
  flip under an unmoved total would have shown.
  **The pre-phase baseline, corrected to MEASURED after an initial wrong refusal.** A first pass
  of this record could not find `reportCreatedOn 2026.08.14-14.23.36` on any
  `Saved/AutomationReport*/index.json` on disk and declined to repeat it — a reasonable
  instinct, but wrong here, because `Saved/AutomationReport/index.json` is a single file each
  headless run overwrites, not an archive; two later runs (`17.27.57`, then `17.44.15`) had
  already overwritten the `14.23.36` report by the time of that search, so its absence from the
  live path proves nothing about whether it was ever real. **The control that settles it, run
  independently rather than taken on the coordinator's word: the rotated per-run log survives
  overwriting even though the JSON report does not, and each run's own log carries its own
  `LogAutomationController` lines.** `Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log`
  itself contains, at its own tail, `[2026.08.14-14.23.36:262]...LogAutomationController: Writing
  reports to E:\MultiAgent\Stratocracy\Saved\AutomationReport...` followed immediately by
  `Exported report to '...\Saved\AutomationReport' in 0.00 Seconds` — the run that wrote the
  now-overwritten `14.23.36` report is the same run whose rotated log carries that filename,
  confirmed by the log's own internal timestamp, not merely the filename. `grep -c
  "Test Completed. Result={Success}"` against that same log returns **103**, and `grep -c
  "Test Completed."` (any result) also returns **103** — no failure, no warning, no `notRun`,
  independently re-deriving the exact figures the coordinator read from the JSON before it was
  overwritten, from a different surviving artifact. The positive control the coordinator offered
  (`Stratocracy-backup-2026.08.14-17.27.57.log` pairing with the test-author's mid-phase
  `reportCreatedOn 17.27.57`) checks out the same way: that log's own tail carries
  `[2026.08.14-17.27.57:428]...LogAutomationController: Writing reports to
  ...\AutomationReport...` — the same mechanism, a second time. **So: pre-phase baseline is
  `succeeded 103 / succeededWithWarnings 0 / failed 0 / notRun 0` at `reportCreatedOn
  2026.08.14-14.23.36`, MEASURED — first read by the coordinator directly from the JSON at
  session start (before any run in this phase, while the report still existed at that path), and
  independently re-confirmed here from the surviving rotated log rather than taken on trust.**
  **Why it will not be re-derivable from `Saved/AutomationReport/index.json` again:** that path
  is overwritten by every subsequent headless run, so a reader repeating the original search
  after any further test run will correctly find nothing there — that absence is not a defect
  and should not reopen this question; check the matching `Saved/Logs/*-backup-<timestamp>.log`
  instead, which is what settled it here.
  **Method, worth recording because this project has paid for the shape once already:** the
  first pass applied correct procedure — searched, found nothing, declined to assert — but did
  so against an instrument already known to have been overwritten, so the absence it found
  proved nothing. The fix was a control (the surviving rotated log, independently re-grep'd),
  not a louder restatement of either side's claim. This is a second instance of this milestone's
  own standing lesson, "an absence needs a control": an absence proves nothing until the
  instrument is shown able to find the thing when it is present — here, until it is shown that
  the *reporting* instrument (the JSON) is what got overwritten, while a *different* instrument
  (the per-run log) survived and can still answer.
- **The substantive finding, verified against the vendored (read-only) source, not merely
  accepted:** `Source/StratRules/Ai.good.cpp`'s `chooseBuild` (`:275-289`) builds an
  `affordable` list from `s.buildlist` and picks the single `buildPriorityLess`-least entry
  (`:224-231`, ascending `costFame`); a buildlist `{Infantry, Infantry, Tank}` and `{Infantry,
  Tank}` produce an `affordable` set differing only by a duplicate Infantry entry, and the
  minimum-cost pick is identical either way — **Tank is never chosen while Infantry is
  affordable, regardless of repetition.** `Source/StratBridge/StratBridge.h`'s
  `SetBuildlistByIds` doc block, re-read directly at its current location, now states this
  correctly (the "duplicates express a ratio" claim is gone). This is a vendored-behaviour
  observation for `E:\MultiAgent\stratocracy-crew`; no rules change was requested or made.
- **The two stale-citation corrections in this file's own prior entries, verified rather than
  copied:** (1) `.agents/ue-project-context.md` — `grep -n "93/93\|103/103"
  .agents/ue-project-context.md` returns exactly one hit, line 195, reading "103/103,
  combat-outcome phase 3, 2026-08-14"; there is no `93/93` anywhere in the file, and `:198`'s
  "Measured in phase 4" already reads "Measured in the hot-seat milestone's phase 4 (not the
  combat-outcome milestone's...)" — both flags this file raised were already discharged before
  this phase started. Both flags are corrected in place above (not deleted) so a reader sees they
  were raised and then discharged, per the brief's instruction; **note that a prior
  `strat-gameplay-engineer` report restated the "93/93" flag as though it had confirmed it by
  reading the file — it had not; a restated flag is not a measurement**, and this steward's own
  correction above is grounded in the `grep` shown, not in that report. (2) The
  `StratSelectionMachine.cpp:258-263` and `StratBridge.h:592-600` citations are corrected above
  to name the branch/function rather than a line number — the guard is now at a different line
  (`if (Selected->bHasActed)` inside the enemy-click arm of `HandleEvent`, currently near `:315`,
  read directly), the fourth instance of line-number rot in this milestone.
- **The bHasActed-guard comment debt is discharged, verified by direct read of
  `Source/StratPlay/StratSelectionMachine.cpp`:** the branch now carries a comment block stating
  why no click sequence reaches it, what it actually guards (rules-side `bHasActed` vs.
  engine-side `DoneUnits`), and that its cost is zero clause coverage — landed by
  `strat-gameplay-engineer` this phase.
- **The two gate-rationale corrections, both re-verified directly against
  `Source/StratBridge/StratBridge.cpp`/`.h`:** `MakeUiSnapshot`'s guards are exactly
  `bDefinitionsLoaded`, `bSeeded`, and a per-unit `defIndex` bounds scan (`StratBridge.cpp:758-776`)
  — no null-table guard anywhere in it. `MakeUiWorld` (`:697-708`) sets `W.unitDefs = &Units` and
  `W.terrain = &Terrain`, and `StratBridge.h:801-802` declares both as by-value
  `std::vector` members — addresses of a by-value member cannot be null, which is the pin that
  actually survives reordering the guards. Separately, `grep -rn` for lowercase
  `strat-cmd`/`strat-combat`/`strat-ai`/`strat-wait` across `Source/` returns exactly two hits,
  both inside the new comments in `StratHotSeatReplayParity.cpp` and
  `StratSelectionWaitClauses.cpp` themselves — confirming the case-variant census the corrected
  rationale rests on.
- **`--pre-sliced` gate debt confirmed untouched this phase**, as scoped: `git diff --stat
  ae2f22a -- Tools/architect/` is empty and `git status --porcelain Tools/architect/` is empty.
  Still open; owed shape recorded under NEXT.

**Milestone status: COMPLETE.** Five phases planned, five phases CLOSED; no phase 6 was ever
scoped for this milestone (unlike the hot-seat and AI-opponent milestones, both of which ran to
6/D respectively — this one's own header always said "five phases," never open-ended). What it
delivered: a `STRAT-COMBAT` log family mounted on `FStratBridge::Submit`; parity clauses proving
the pre-submit forecast agrees with the post-submit measured delta, including the first-ever
exercise of the `adied=1` counter-kill arm; a headless AI-vs-AI pairing gate
(`Tools/architect/strat_combat_pairing_gate.py`) proving `STRAT-AI applied kind=Attack` pairs
1:1 with `STRAT-COMBAT resolved` by ordered identity, backed by ten checked-in fixtures proving
it can fail; the same gate re-run against a live PIE corpus in `--pre-sliced` mode, with all
eleven per-turn hashes byte-identical to the headless run (host-independence discharged;
content-independence carried to NEXT as its own future work, unchanged by this phase); and a
doc pass that corrected the two stale citations and one stale flag-pair this file and
`.agents/ue-project-context.md` were carrying, discharged the unexercised-guard comment debt,
and recorded — against the vendored source, not on argument — that `strat::chooseBuild` makes
buildlist repetition inert at the rules layer.

## Grep-contract case-sensitivity tightening (not a phase)

- **Landed:** `a2d370a`, "A grep contract a case-only change would have slipped past, and a
  build command nobody could run", 2026-08-14. Editor closed throughout; tree clean at `a2d370a`,
  pushed to `origin/master`. **This is not a phase and does not reopen the log-backed combat
  outcome milestone (CLOSED, COMPLETE above).** It discharges NEXT item 2 carried out of that
  milestone's phase 5 (the deferred `ESearchCase::CaseSensitive` tightening) and opens a new,
  narrower residual — see NEXT.
- **4 files, 316 insertions, 39 deletions** (`git show --shortstat a2d370a`, re-verified
  directly, matches exactly): `Tests/StratHotSeatReplayParity.cpp`,
  `Tests/StratSelectionWaitClauses.cpp` (both `strat-test-author`'s lane), plus
  `.agents/ue-project-context.md` and `.claude/skills/strat-hotseat/SKILL.md`.
- **Suite 103 → 104.** `Saved/AutomationReport/index.json`, read as `utf-8-sig`:
  `reportCreatedOn 2026.08.14-18.44.32`, `succeeded 104 / succeededWithWarnings 0 / failed 0 /
  notRun 0`, 104 test entries, every one `state: "Success"` (all re-derived directly, not taken
  from a report). Independent corroboration: `IMPLEMENT_SIMPLE_AUTOMATION_TEST` across `Source/`
  totals **104** across 20 files, `IMPLEMENT_COMPLEX_AUTOMATION_TEST` totals **0** — the
  count-new-clauses-by-macro method this project prefers over an acceptance-ID grep, re-run
  directly rather than trusted from the brief.
- **One new clause:** `Stratocracy.StratPlay.T-SAVE-05.GrepContractRejectsACaseVariant`, in the
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` block of that name in `StratHotSeatReplayParity.cpp`
  (re-read directly).
- **The change.** `FString::StartsWith` defaults to `ESearchCase::IgnoreCase` in UE 5.8, so the
  T-SAVE-05 clause asserting the `STRAT-CMD accepted ` line shape (via the new shared predicate
  function `IsGrepContractAcceptedLine`) accepted any casing while the phase-6 grep it stands
  in for would not. Only that assertion was tightened, to `ESearchCase::CaseSensitive`.
- **Design ruling, preserved with its reason — instruments loose, claims strict.** The capture
  filter in `FStratCmdCapture::Serialize` and the `CountStartingWith` member function stay
  `IgnoreCase` deliberately. Tighten the filter instead and a case-mangled line never reaches
  the strict claim: `Lines.Num()` reads 0, the arity clause goes red first and returns early,
  and the failure surfaces as "no line at all" rather than "the line is spelled wrong" — the
  instrument must be able to SEE the defect the claim is unable to ACCEPT. The pre-existing
  fail-safe count argument (the `CountStartingWith` calls counting `"STRAT-CMD accepted"` and
  `"STRAT-CMD refused"`) was re-derived rather than inherited this pass and survives in its
  original shape *because* the filter stayed loose. A future reader is the most likely one to
  "fix" this without understanding it — recorded here with the reason so that doesn't happen.
- **What the new clause actually proves.** Its load-bearing assertion is that the bare
  `StartsWith` expression the pass deleted still ACCEPTS the same case variant the new predicate
  refuses — the defect named rather than described, not merely an absence asserted. Reverting
  `CaseSensitive` turns the clause red. It also pins its own fixture with
  `Equals(..., ESearchCase::CaseSensitive)` false and `Equals(..., ESearchCase::IgnoreCase)` true,
  inside that same clause, because a bare `TestEqual` on those two strings would call them equal
  and make the whole fixture meaningless — the same case-insensitivity trap already on record in
  this project's history (`ue-fstring-comparison-is-case-insensitive`).
- **The new residual, carried to NEXT above:** a case-only change to the `STRAT-CMD refused`
  shape is caught by nothing, and closing it needs an equivalent clause for that shape. Stated
  at measured width, including the one thing the brief's draft overclaimed and the
  test-author correctly declined to assert either way (ICU inline mode modifiers,
  unverifiable from this tree's bundled headers-and-libs-only ICU) — see the NEXT entry for the
  exact citations (`AutomationTest.h`'s `Matches`, `Exact`, and both constructors' hardcoded
  `ERegexPatternFlags::CaseInsensitive`).
- **Gating, recorded honestly, including the exception.** Two full gates (`VERDICT: PASS`, zero
  findings each) and one micro re-gate (`VERDICT: PASS`, zero findings). **The final
  comment-only edit was coordinator-verified, not gated** — the reviewer had itself authored and
  verified the replacement sentence, so gating text it dictated would have been circular. The
  mechanical comment-only proof was run independently: the non-comment changed lines in both
  `Tests/` files are exactly the previously-gated code (the predicate, one call-site
  substitution, the new clause), `StratSelectionWaitClauses.cpp` contributes zero non-comment
  lines, and the macro census was unchanged at 104. This one edit did not pass the gate, and
  that is recorded here rather than left to be implied by the rest of the entry.
- **`Build.bat`, also fixed this pass, not flagged.** `.agents/ue-project-context.md` and
  `.claude/skills/strat-hotseat/SKILL.md` both carried `Build.bat` as a bare command in a
  "verbatim, never reconstructed" block. It is **not on `PATH`**
  (`E:\MultiAgent\Stratocracy` shell: `command -v Build.bat` fails) — it lives at
  `C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat`, confirmed present at
  that path directly. Every agent handed the bare form had worked around it silently and never
  written it down. Both files now carry the full path plus the PowerShell call-operator caveat
  (`& "C:\…\Build.bat" …` needed from PowerShell, not from the Bash tool), verified to run
  verbatim from both shells. This file has previously carried standing flags about `.agents/`
  being unowned — this is one of those items fixed, not flagged again.
- **A method lesson worth recording.** The same defect shape — a universal quantifier standing
  in for the argument's actual scope — recurred a fifth time in this milestone-adjacent pass: an
  early draft of the `refused`-residual paragraph claimed "there is NO way to spell a
  case-sensitive expected message in this engine version," a universal over *every possible
  regex pattern* derived from a measurement of *what two constructors accept as parameters* —
  different SUBJECT from what was measured, which is why an "enumerate the set" habit does not
  catch it. The working check: ask whether the sentence's subject is the thing you actually
  measured. Corrected to the narrower, true claim before landing.

## Pre-sliced zero-event guard (not a phase)

- **Landed:** 2026-08-14, this steward's own lane only (`Tools/architect/`). Discharges the
  `--pre-sliced` gate debt carried out of the log-backed combat outcome milestone's phase 5 (NEXT
  item 4, and the phase-4 section's original "Owed shape, per the reviewer" paragraph). **Not a
  phase, does not reopen the milestone.** Tree clean at `c778f7c` before this pass; nothing
  staged or committed by this pass — that is the user's call.
- **The defect, reproduced directly before any edit, not quoted from the brief:** an empty file
  through `python Tools/architect/strat_combat_pairing_gate.py <file> --pre-sliced` printed
  `slice (1-based lines): 1..0`, every count `0`, `pairing mismatches: 0`, `PASS`, `EXIT=0`. Zero
  applied attacks paired vacuously true against zero resolved lines — an ordered-identity loop
  over two empty lists asserts nothing and disagrees with nothing.
- **Design chosen: BOTH halves of the reviewer's stated shape, not one.** A structural default
  refusal (`GuardRefusal`, no flag needed, cannot be forgotten) fires when `--pre-sliced` yields
  zero `STRAT-AI applied kind=Attack` AND zero `STRAT-COMBAT resolved` lines. `--expect-min-pairs
  N` is additionally offered as a strictly stronger, caller-opt-in floor (`min(applied, resolved)
  >= N`) that catches a non-empty but short corpus (e.g. 68 expected, 3 present) the structural
  check alone would miss. Passing `--expect-min-pairs` at all — including `N=0` — is read as the
  caller's explicit claim about the corpus and REPLACES the structural default rather than
  stacking under it, so a genuinely combat-free `--pre-sliced` session (if one is ever a real
  corpus) has a stated, auditable way to pass rather than needing the guard disabled. Rejected:
  shipping `--expect-min-pairs` alone, because a forgotten flag reproduces the exact defect this
  debt exists to close; shipping the structural refusal alone, because it cannot catch a
  short-but-nonzero corpus and the reviewer's brief asked for the stronger option too.
- **Scope: `--pre-sliced` only, deliberately, not the marker-sliced path.** A marker-sliced
  corpus (the default mode) can legitimately contain zero attacks — a real automation test that
  never attacks, sliced correctly by its own `Test Started.`/`Test Completed.` pair, is a boring,
  correct `PASS`, and a blanket zero-event refusal there would fire on a good run. Marker slicing
  already carries its own structural corpus-identity guard (`SliceFailure` on a missing or
  mismatched marker pair) that `--pre-sliced` was built specifically to trade away for caller
  convenience on a PIE session log with no automation markers at all; the new guard restores an
  equivalent for the mode that gave that guard up, rather than duplicating a check the
  marker-sliced path does not need. `--expect-min-pairs` is NOT scope-restricted — it is a plain,
  useful floor in either mode — but only `--pre-sliced`'s empty corpus gets an unconditional
  default.
- **Exit code and channel: matched the script's own existing convention, not invented.**
  `SliceFailure` and a `PairingMismatch` failure already share one exit code
  (`main`'s `return 0 if result.passed else 1` has never branched on failure kind) and are
  distinguished by RENDERED TEXT alone — `SliceFailure` prints its own `"FAIL: slicing failed --
  {reason}"` branch instead of the normal count table. The new `GuardRefusal` follows the same
  precedent: a `GUARD REFUSED: {reason}` line appended to the normal, full count table (unlike
  `SliceFailure`, the guard fires only after parsing succeeds, so the counts are worth showing),
  same terminal `PASS`/`FAIL` word, same `EXIT=1`. No new exit value was introduced.
- **Falsifiability — the non-negotiable.** Self-test case 14 (`check_self_test`, 14th `record()`
  call) asserts, in one case: an empty `--pre-sliced` corpus is refused by default
  (`r_default.passed is False` AND `r_default.guard_refusal is not None`), AND the same corpus
  with `--expect-min-pairs 0` passes (`r_overridden.passed is True`) — proving both that the
  guard actually fires and that the override is not itself dead code. Delete the zero-event
  branch in `run_gate` and this case regresses to `passed=True` on the default call, failing the
  `record()`. Verified directly: `--self-test` prints `[OK] --pre-sliced empty corpus is refused
  by default, but --expect-min-pairs 0 overrides it -- default: passed=False guard_refusal=...;
  overridden: passed=True`.
- **Two counts, kept distinct, per the brief's own warning about conflating them.** In-script
  `--self-test` cases: **13 → 14** (one new `record()` call). Checked-in `.log` fixtures in
  `evidence/08-combat-pairing-gate/fixtures/`: **10 → 11** — a new empty file,
  `fail_pre_sliced_zero_events.log`, added because the defect this debt closes was itself
  demonstrated with an empty file, and a fixture proving it belongs beside the other ten. It is
  run with `--pre-sliced` (not `--test-path`) — `--pre-sliced` ignores `--test-path` entirely, so
  it carries none of the other ten fixtures' `--test-path Stratocracy.Fixture.FIX-01.FixtureTest`
  trap. Noted so a future reader does not conflate the 14 and the 11.
- **Regression evidence, all re-run after the change, not assumed:**
  - `--self-test`: all 14 cases `[OK]`, `SELF-TEST: ALL FIXTURES CORRECT`, `EXIT=0`.
  - All 10 pre-existing `.log` fixtures, each run with `--test-path
    Stratocracy.Fixture.FIX-01.FixtureTest` (the documented trap, applied correctly): the 7
    `fail_*` fixtures each print `FAIL`/`EXIT=1`, the 3 `pass_*` fixtures each print
    `PASS`/`EXIT=0` — identical to `gate_self_test_output.txt`'s recorded table, unchanged by
    this pass.
  - The real phase-4 PIE corpus, `evidence/08-combat-pairing-gate/pie-run/ai-vs-ai-pie-session-
    slice.log` (266 lines), run with `--pre-sliced`: unchanged — `STRAT-AI applied kind=Attack:
    68`, `STRAT-COMBAT resolved: 68`, `pairing mismatches: 0`, `PASS`, `EXIT=0`. The guard did
    not fire because the corpus is non-empty; this is the control proving the guard admits a good
    corpus, not merely that it rejects a bad one.
  - The new fixture itself, `fail_pre_sliced_zero_events.log` (0 bytes), through `--pre-sliced`
    with no `--expect-min-pairs`: `GUARD REFUSED: --pre-sliced corpus produced ZERO ... EXIT=1` —
    the exact defect this debt closes, now caught.
- **Files changed:** `Tools/architect/strat_combat_pairing_gate.py` (edited — new `GuardRefusal`
  dataclass, `guard_refusal` field on `GateResult`, the guard computation in `run_gate`, the
  `--expect-min-pairs` CLI flag, self-test case 14, plus docstring updates at the module banner
  and `run_gate`'s own docstring); `Tools/architect/evidence/08-combat-pairing-gate/fixtures/
  fail_pre_sliced_zero_events.log` (new, empty). `git diff --stat`: one file changed, 134
  insertions, 3 deletions in the `.py`; the fixture is untracked. Nothing staged, nothing
  committed — this steward does not commit.
- **Out of scope, recorded and carried forward, not built:** the separate debt that the 10
  checked-in `.log` fixtures have no re-runnable assertion binding each to its expected verdict
  in `gate_self_test_output.txt` (so a future parser edit could leave that table silently stale),
  and that `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` is undocumented in the fixtures
  directory itself. Touched only insofar as the new fixture's own note above states its mode;
  the rest is unchanged and not this task.
