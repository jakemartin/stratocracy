# Phase 6 evidence — 7-turn hot-seat playtest

Assembled 2026-08-13. Source session: human-driven PIE session in a standalone window,
seeded `14:42:11`, last accepted command `14:43:43`. **Rescued from a log rotation** — an
editor restart moved the 27 `STRAT-CMD accepted` lines out of `Saved/Logs/Stratocracy.log`
and into `Saved/Logs/Stratocracy-backup-2026.08.13-15.07.27.log` before this task started;
that backup (and this whole session) is one more rotation away from being overwritten by a
future automation run, so it has been copied byte-identical into this directory
(`Stratocracy-backup-2026.08.13-15.07.27.log`, md5 `baa918eade6b1061e9346c47a3b82d54`,
verified equal to the copy still under `Saved/Logs/`).

## Contents of this directory

| File | What |
|---|---|
| `Stratocracy-backup-2026.08.13-15.07.27.log` | The full rescued log, byte-identical to the `Saved/Logs/` copy at time of rescue. |
| `command_log.md` | All 27 `STRAT-CMD accepted` lines verbatim with line numbers/timestamps, the six `Selection:` refusals, and the measured shape (kind counts, refusal count, hash uniqueness, side-flip count). |
| `assertions.md` | The eight `assert_log_contains` clauses, independently re-verified against the preserved log by direct grep, plus the two expected-absent checks. |
| `wait_witness.md` | The `STRAT-WAIT` sentinel from the *current* log and why it is deliberately a disjoint family from `STRAT-CMD`. |
| `ScreenShot00038_full.png` / `_cropped.png` | Turn-1 baseline. Cropped to the viewport region (left 40%, top 55% of the 2538×1352 editor frame, i.e. 1015×744) containing the scoreboard HUD: `TURN 1/20`, `Destroyed 0/0`, `Objectives 1/8`, `Unit HP 60/60`. |
| `ScreenShot00041_full.png` / `_cropped.png` | Post-game. Cropped **wider than the brief's rough guidance** — 1320×1320, not 1015×744 — because the rough crop only caught the scoreboard and left the Output Log panel (which sits directly below the viewport, not inside the stated 40%/55% box) out of frame. The wider crop was verified by inspection to show both in one image: `TURN 7/20`, `Destroyed 0/50`, `Objectives 1/8`, `Unit HP 50/51`, and the tail of the `STRAT-CMD accepted` lines visible in the docked Output Log panel beneath the viewport. |

## What is proven, and how

- **The command log is bytes, not narration.** Every line in `command_log.md` is a direct
  quote from the preserved log file, with a line number a reader can `grep` for themselves.
  Kind counts (Move ×4, Attack ×10, EndTurn ×13), zero refusals, and 27 distinct hashes were
  all re-derived from the file in this session, not copied from the task description.
- **One correction to the task's own framing, stated per "the tree wins," not silently
  applied**: walking the 27 `side=` values in order gives **13** transitions, not the stated
  12. See `command_log.md`'s "Measured shape" section for the full walk. This does not change
  any PASS/FAIL result — it is a count correction, made because a measured 13 outranks a
  stated 12.
- **The eight assertions were independently re-run as greps against the preserved log file**,
  not merely copied from the interactive session's reported PASS. All eight still PASS; see
  `assertions.md`.
- **The first attack lands where the earlier move put its target**: `3473` (`Attack unit=7
  hex=4,4`) fires on the hex `3470` (`Move unit=3 hex=4,4`) had moved side 0's unit 3 to one
  line earlier — cross-referenced directly in the log, not asserted from memory.
- **Screenshots are full-editor captures, cropped, with both crop images preserved
  alongside the crop actually used** so a reviewer can check the crop choice against the
  source rather than trust it.

## Limits — read this before citing anything above as a gate

- **These assertions document a stored artifact; they do not gate a repeatable run.**
  NeoStack input injection still does not reach `UPlayerInput` (see `Tools/architect/state.md`,
  Phase 6, "The symptom, stated precisely" — the break is upstream of `UPlayerInput`, in
  `UGameViewportClient::InputKey`'s dispatch to the local player). Nothing automated can
  reproduce this session. **Re-running `assert_log_contains` against a fresh log fails every
  one of the eight clauses in `assertions.md`**, because a fresh session's log holds no
  `STRAT-CMD` lines at all. "All eight PASS" describes this one artifact, not a working gate —
  say so plainly rather than letting it read as proof of a repeatable pipeline.
- **"A red unit won the encounter" is an on-screen observation by the user, not
  log-backed** — no combat-outcome line, no destruction line, and no `Destroyed` count change
  exists anywhere in the preserved log; the narrative stays visual, not log-backed, if a later
  phase wants it in `LogStratPlay` that is a `FStratBridge`-routed line, not a widget one. It
  now has independent corroboration from the scoreboard, though: `Destroyed 0 / 50` in
  `ScreenShot00041` is side 1's combat Fame (see the correction above, not a unit count), and
  side 1 holding combat Fame while side 0 holds none is consistent with the user's
  observation. The `Unit HP 50 / 51` asymmetry is a second, independent trace pointing the
  same way.
- **Correction, struck rather than carried forward: the `Destroyed` row reading `50` for
  side 1 is not a scoreboard defect.** This directory's own earlier text flagged it as a
  probable legibility bug on the premise that the maximum is 5 units. That premise is false.
  The `Destroyed` row does not count units at all — it binds `strat::UiSideView::fameCombat`
  (Fame from kills, GDD §2.11.4), not a unit tally, per
  `Source/StratUI/StratScoreboardWidget.cpp:214` (tooltip: *"Fame from kills. Factory income
  does not count at the cap."*) and pinned by
  `Stratocracy.StratUI.T-UI-03.DestroyedBindsCombatFame`
  (`Source/StratUI/Tests/StratScoreboardParity.cpp:376-424`, which explicitly asserts
  `TestFalse(… "Destroyed shows a bare value, not X of N")`). So `Destroyed 0 / 50` in
  `ScreenShot00041.png` means side 1 held 50 combat Fame and side 0 held none — **independent
  corroboration of the user's otherwise-unlogged "a red unit won the encounter"**, not a
  contradiction of it. The bug was in the reading, not the widget: treating a bare-value Fame
  row as a bounded unit count is what produced the phantom defect. No UI fix is owed.
- **A wait leaves no trace in `RecordedLog()`, and cannot.** See `wait_witness.md`. A replay
  reproduces state hashes but not the player's experience — a unit that waited replays as one
  that never acted. This is a property of the vendored save format (`Wait` was never in
  `strat::SaveCommandKind`), not a defect introduced by anything in this session.
- **Injection reaches `UGameViewportClient::InputKey`** — the in-game console provably
  toggles under simulated input — **but not `UPlayerInput`.** Hypothesis, not measured: a
  controller/input-device-id mismatch on the per-player route. This is NeoStack-plugin
  territory, not something fixable in this repository. See `Tools/architect/state.md` Phase 6
  for the full elimination history (input-mode filtering, `IgnoreInput()`, focus, all ruled
  out by measurement) before re-opening this investigation.
