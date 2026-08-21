# Stratocracy state — global

> **Sole writer: `coordinator + strat-data-steward`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Milestone status, the banner, the candidate backlog, and the cross-lane carried-debt ledger. **The only file that may state a suite count or a phase verdict.**
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

_Last run 2026-08-20 (TOOLING ONLY -- no C++, no editor, and no suite run: three pass-on-failure
sites in this project's own pre-commit hook, committed at `95d955b` and `ebbe20d`. Nothing in that
pass re-measures the suite, so every suite figure in this banner still describes `185e88f` and is
unchanged. See "Pre-commit hook" at the end of this file. The 2026-08-19 entry that follows is
preserved verbatim: post-milestone work: the `chooseBuild` cap RULED and FILED upstream at
crew `85995b8`; the content-independence SCENARIO axis FILED upstream against the same crew HEAD
as a change request in `spec/scenario_spec.md` — filing is not closing, and that axis is STILL
OPEN; ten rotted line-number citations replaced by function-and-branch citations; the
content-independence corpus DISCHARGED on two of three axes at `185e88f`, gated `VERDICT: BLOCK`
then `VERDICT: PASS` after this banner itself was found contradicting the entry below it — the
defect that produced `strat_banner_sweep.py`. Suite 107 -> 108. The milestone narrative that
follows is unchanged and still describes 2026-08-14: log-backed combat outcome milestone: phase 1 CLOSED — the `STRAT-COMBAT`
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
the CONTENT-independence half is NOT and is carried forward as its own future work **[BANNER
CORRECTED 2026-08-19: the content half is now discharged on two of its three axes at `185e88f` —
first side and buildlist moved, a second corpus graded 42/42 with zero mismatches; the SCENARIO
axis alone remains open. This clause described the state at phase 4 and is corrected here rather
than deleted]**; phase 5
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
guard" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the `STRAT-CMD refused`
grep-contract residual that the previous item opened is now DISCHARGED — two new clauses,
`T-SAVE-05.StratCmdRefusedLineShape` and `T-SAVE-05.GrepContractRejectsARefusedCaseVariant`,
close it the same way the `accepted` side was closed; suite is now **106/106** (was 104/104).
See "STRAT-CMD refused grep-contract residual" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane:** the fixture-verdict binding debt (open since phase 3, carried
through the pre-sliced zero-event guard entry and the grep-contract residual entry above, both
of which left it untouched) is now DISCHARGED. `Tools/architect/strat_fixture_verdict_binding.py`
(new) re-runnably binds all 11 checked-in `.log` fixtures under `evidence/08-combat-pairing-gate/
fixtures/` to their expected verdict AND their exact invocation, and is shown falsifiable by two
deliberate breaks, both reverted. `fixtures/README.md` (new) documents the `--test-path` /
`--pre-sliced` split in the fixtures directory itself, closing the second, smaller item. This
pass also found and fixed staleness in two checked-in artifacts the debt predicted would go
stale silently: `gate_self_test_output.txt` (10 entries, missing
`fail_pre_sliced_zero_events.log` — confirmed by `grep -c "^== "` against `git ls-files`'s 11)
and `self_test_internal_fixtures_output.txt` (11 entries against the in-script `--self-test`'s
actual 14), both regenerated from a real run; no Python or C++ suite count changes (Python-and-
doc-only work). See "Fixture-verdict binding" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane — three corrections, all found by the wave-1 reviewer gate as
non-gating observations:** (1) this header's own suite count was stale at 106/106; re-derived
directly against `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
2026.08.14-21.47.35`, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107
entries, zero non-Success) and corroborated by an independent macro census
(`IMPLEMENT_SIMPLE_AUTOMATION_TEST` sums to 107, `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS`
sums to 0) — suite was **107/107** at that pass (was 106/106) and **is now 108/108** as of
`185e88f`, the content-independence clause — re-derived both ways, `succeeded 108 / failed 0 /
notRun 0` in `Saved/AutomationReport/index.json` and an independent macro census summing to 108
with COMPLEX at 0. (2) A correction bullet this steward wrote
in the prior pass asserted `T-INT-05.AlreadyActedGuardFiresOnAForeignModel` was still open; it is
not — `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` (untracked) ran green in the
same report above, having landed mid-wave, after that pass. Corrections appended in place at all
three prior mentions (the "Four items carried out of..." NEXT bullet, the "STRAT-CMD refused
grep-contract residual" section, and the "Fixture-verdict binding" section) rather than rewritten
— each was true when written. The other two items those same sentences named,
`chooseBuild`'s buildlist-ratio question and the content-independence corpus, have moved
differently and are kept distinct: `chooseBuild` is now investigated and written up at
`Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md` (a request draft awaiting a
Director ruling, **not discharged**); content-independence remains genuinely open. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]** (3) The
fixture-count derivation command recorded in this file and in
`evidence/08-combat-pairing-gate/blackboard.md` — `git ls-files fixtures/ | wc -l` — measures
tracked files in the directory, not `.log` fixtures; it read 11 only because this pass's own
`fixtures/README.md` was still untracked, and would read 12 the moment that file is committed
while the `.log` count stays 11. Both prose copies corrected to `git ls-files
'.../fixtures/*.log' | wc -l`, which reads **11** both now and after a hypothetical commit of the
README (verified: `git ls-files --cached --others --exclude-standard '.../fixtures/*.log'` — the
proxy for post-commit tracked state — still returns 11, while the same query without the `*.log`
glob returns 12). See "Fixture-verdict binding" below for the full account. The prior
entry (AI-opponent milestone, phase D CLOSED, COMPLETE) is preserved under "AI-opponent milestone"
further down. **ALSO 2026-08-20, TOOLING ONLY, THIS STEWARD'S OWN LANE -- and it is the
reason this banner now lives in a different file:** the 3,524-line `state.md` is SPLIT into
`Tools/architect/state/`, one file per owning agent, and `state.md` is frozen as the closed-phase
history. The sweep that polices the record now DERIVES its file list from that directory instead of
naming `state.md`, compares live claims ACROSS the set rather than file by file, and enforces that
only this file may carry a live suite count. Two defects were fixed in it and both were shown
falsifiable against a control running HEAD's own bytes: a suite claim ending in a full stop was
invisible to its regex, and the pre-commit hook's typed path meant a commit touching only
`state/tests.md` drew ZERO bytes of output and exit 0. `install.sh` no longer dies in a linked
worktree, and the `strat-hotseat` skill's claim that a new worktree is unprotected is corrected --
it is protected, through the shared common directory. `.claude/skills/strat-parallel/SKILL.md` is
new and carries the worktree topology, the seven concurrency rules, per-tree command templates that
derive every path from that tree's own `git rev-parse --show-toplevel`, and the merge protocol.
**NO SUITE RUN AND NO C++ IN THIS PASS** -- every suite figure above still describes `185e88f` and
is unchanged; the only build was a re-verification that the `slot-1` worktree still compiles
(`Result: Succeeded`, `REAL_EXIT=0`, 82 of 82 actions, editor closed).)_

## BUILT

- `Source/StratUI/StratScoreboardHUD.h` — applied (iteration 1, step `host_h`, live)
- `Source/StratUI/StratScoreboardHUD.cpp` — applied (iteration 1, step `host_cpp`, live)
- `FStratBridge::Reachable` — landed at `e0cc53d` with zero tests; its five clauses are now
  covered (`StratBridgeQueryParity.cpp`, T-UI-02, phase 1, 2026-08-12). Debt discharged.
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
- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)
- **Hot-seat milestone is COMPLETE (phase 6 closed 2026-08-13); see "Hot-seat milestone —
  COMPLETE" below.** The AI-opponent milestone is now current (phase C closed 2026-08-13; phase D
  — PIE playtest and a machine-repeatable AI-vs-AI gate — is next; see "## AI-opponent milestone"
  below). Remaining out of scope, unchanged: production menu (§2.11.5), guided opening (§2.11.6),
  info panel, toasts, save-slot UI, move-undo.
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
- **Four items carried out of the log-backed combat outcome milestone's phase 5 (now CLOSED,
  milestone COMPLETE) — none owned by this steward, none built in that doc pass by design:**
  1. `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` — pin that
     `FStratSelectionMachine` refuses an enemy click when the selected unit has
     `bHasActed == true` while the machine's own `DoneUnits` is empty (a machine built against a
     match already in progress). Proposed by `strat-gameplay-engineer`, correctly not written in
     a doc pass. Owner: `strat-test-author`.
     - **CORRECTION, 2026-08-14, this steward's own lane, later pass — DISCHARGED, landed mid-wave
       after this entry was written.** This item was correctly open when written. It is not open
       now: `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` is in the tree
       (untracked at time of writing — `git status --porcelain` lists it `??`), and
       `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` reads `state:
       "Success"` in `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
       2026.08.14-21.47.35`). Suite is now **107/107** (`succeeded 107 / succeededWithWarnings 0 /
       failed 0 / notRun 0`, 107 entries, zero non-Success — re-derived directly, not taken from a
       report or a dispatch number), corroborated independently by
       `grep -rc IMPLEMENT_SIMPLE_AUTOMATION_TEST Source --include=*.cpp` summing to **107** and
       `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS` summing to **0**. The other three items this
       same numbered list names — the `chooseBuild` buildlist-ratio question and the
       content-independence corpus — have NOT moved the same way: `chooseBuild` is now
       investigated and written up at
       `Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md`, a request draft
       awaiting a Director ruling, **not discharged**; content-independence remains genuinely open,
       untouched. These three items are three different states, not one word. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]**
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
     - **DISCHARGED 2026-08-19 — and the file held SEVEN citations, not one, one of which had
       already rotted.** The item named the two in-string ones; a grep for
       `StratSelectionMachine.cpp:` in that file returned seven sites (two inside assertion
       message strings, five in comments), all replaced with function-and-branch citations.
       **`:390`, cited twice as `NotifyCommandApplied`'s `DoneUnits.Add`, was already wrong when
       this pass opened it** — that add is at `411`, and `390` now lands on a `break;` in
       `HandleEvent`'s Move arm, a different function. Re-derived, not assumed:
       `grep -n "DoneUnits.Add(Outcome.UnitId)"` → 411; `sed -n '390p'` → `break;`. The other
       five (`:156-160`, `:158`, `:168`) were still accurate and were replaced anyway, which is
       the point — accuracy today is not the property, unreachability by the next diff is.
       Rebuilt (`Result: Succeeded`, one TU recompiled) and re-run headless: suite **107/107**,
       `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries, zero
       non-Success, `reportCreatedOn 2026.08.19-17.49.48`; both wait clauses `Success`. Count
       unmoved, as expected for a comment-and-message-string change.
     - **NOT closed by this, and measured rather than guessed: four more line-number citations
       survive in two other files**, outside the file this item named. `StratBridge.cpp:448`
       cites `StratSelectionMachine.cpp:571-575` — checked, still accurate (the submit `switch`).
       `StratHotSeatReplayParity.cpp` carries three, and **two of them have already rotted**:
       `:255-287` (cited twice, once INSIDE an assertion message string at its ATTACK clause)
       lands on the selection arm, not the attack branch — the real enemy-click arm runs from
       roughly the `bHasActed` guard to the `Command = Attack` write; and `:350-356`, cited for
       "an attack ends the unit's turn", lands on the Move arm's "already moved this turn"
       refusal -- that one is `NotifyCommandApplied`'s `Attack` arm, the same add the wait file
       cites. A fifth, `StratSelectionMachine.cpp:519`'s `Save.good.cpp:294-300`, cites a
       VENDORED file by line and is a different problem again.
     - **The three `StratHotSeatReplayParity.cpp` citations were then fixed too, on the user's
       instruction, in the same pass** -- both rotted ones and the one in-string site among them,
       all replaced with the branch names above. `grep -rn "StratSelectionMachine.cpp:"
       Source/StratPlay/Tests/` now returns nothing: the Tests lane carries no line-number
       citation into the machine at all. Rebuilt and re-run: `Result: Succeeded`, suite
       **107/107**, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries,
       zero non-Success, `reportCreatedOn 2026.08.19-18.05.58`; `T-UI-01.ClickedAttackIsAccepted
       AndRecorded` -- the clause whose assertion message was edited -- `Success`, as are
       `T-SAVE-05.HotSeatReplayParity` and `T-INT-02.ReplayParityWithHeadless`.
     - **A build trap worth the record: the editor blocks the build, and the first build of this
       pass slipped through anyway.** The second build failed `Result: Failed
       (OtherCompilationError)` on `Unable to build while Live Coding is active` with the editor
       running (PID 51424, started 13:54, i.e. ALREADY RUNNING during the first build, which
       succeeded). So "the editor was open" does not predict the failure on its own -- Live
       Coding has to have engaged -- and an agent that treats a green build as proof the editor
       was closed has it backwards. Verification here waited for the user to close it rather than
       committing on the strength of the edit being "the same class" as one already proven, which
       is not a measurement.
     - **The vendored citation is now fixed too, on the user's instruction -- and measuring it
       turned up 48 more.** `StratSelectionMachine.cpp`'s `CommandKindName` doc block carried
       three line citations, all three now by symbol: `Save.good.cpp:294-300` ->
       `strat::saveCommandName`'s switch; `Save.h:135` / `Save.good.cpp:292` -> declared in
       `Save.h`, defined in `Save.good.cpp`; and `.agents/ue-project-context.md:189-191` -> that
       file's read-only-territory heading. The block now says WHY in as many words: vendored
       bytes move on a re-vendor and nothing in this repository would catch a stale number.
       **`.agents/…:189-191` had already rotted, and this steward rotted it** -- the map ->
       GameMode section committed at `0b70a18` was inserted above it, so the citation now lands
       on that new prose rather than the sharper-clause text it names. Same class as the §2
       staleness this steward's own filing caused in the `chooseBuild` document, on the same day.
       Verified: `Result: Succeeded`; suite **107/107**, zero non-Success, `reportCreatedOn
       2026.08.19-18.28.07`; and the diff proven comment-only, not asserted to be -- every added
       and removed line matches a comment-body shape, non-comment changed lines **0**.
     - **THE REAL SIZE OF THIS, measured rather than estimated: 46 line citations into vendored
       headers survive across 16 files** -- 48 before this pass, less the two the block above
       just fixed. **Both numbers are stated because the command below returns the SECOND one,
       and a note whose figure its own command contradicts is the exact defect this file keeps
       paying for.** Caught by running it after writing the number, not before. Kept on ONE
       line, unwrapped, because a wrapped command is a command nobody can run:

       ```
       grep -rEn "(Save|Ai|Combat|Economy|Move|Turn|Ui|Data|Hex|Driver|Rules)\.(h|good\.cpp|buggy\.cpp):[0-9]" Source/StratPlay Source/StratBridge Source/StratUI Source/Stratocracy | wc -l
       ```

       Run over the four non-vendored `Source/` modules. Concentrated in
       `StratBridge.cpp` (11), `StratUI/StratViewModel.h` (8), `StratBridge.h` (7),
       `StratCombatOutcomeParity.cpp` (4). These are the WORST class of the whole family: a
       re-vendor moves them all at once, silently, and no clause in this repository reads a
       vendored line number to notice. Spot-checked while here: `Save.h:54`'s
       `SaveCommandKind` enum is still accurate. The other 47 were NOT checked one by one and
       are not claimed either way. Unowned -- `StratBridge` and `StratUI` are
       `strat-gameplay-engineer`'s lane, not this steward's, and this is a phase-sized sweep
       rather than a fix to slip into another pass.
     - **Still not fixed, and still measured: one citation outside the Tests lane.**
       `StratBridge.cpp:448`'s `StratSelectionMachine.cpp:571-575` -- accurate today (it names
       the submit `switch`); production code, `strat-gameplay-engineer`'s lane.
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
  phase 4. **[STAMPED 2026-08-19 — DISCHARGED on two of three axes at `185e88f` (first side and buildlist moved; the scenario axis is still open because `Data/` is vendored and holds one scenario). True when written; see the NEXT bullet dated 2026-08-19.]**
- **DISCHARGED, this pass, 2026-08-14 — see "STRAT-CMD refused grep-contract residual" below.**
  The residual as originally opened by `a2d370a`: a case-only change to the `STRAT-CMD refused`
  shape was caught by nothing. Cannot be closed with an expected-message gate — verified against
  `UE_5.8\Engine\Source\Runtime\Core\Public\Misc\AutomationTest.h`:
  `FAutomationExpectedMessage::Matches` uses `FString::Contains` at its `IgnoreCase` default;
  `EAutomationExpectedMessageFlags::Exact` adds only a length equality, and a case variant has
  identical length; both constructors build the pattern with `ERegexPatternFlags::CaseInsensitive`
  hardcoded, not parameterized. Closed instead with `IsGrepContractRefusedLine`, a dedicated
  `ESearchCase::CaseSensitive` predicate, and two new clauses — see the new section for the full
  account, including the falsifiability measurement's stated limit (it exercised the predicate,
  not an edited emitter, so the closure is a sound inference rather than a directly observed one).
