# Stratocracy state — decisions

> **Sole writer: `strat-data-steward (append-only)`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Director rulings, upstream filings against `stratocracy-crew`, and scope calls. **Never rewritten** — an entry that has moved is stamped in place.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## DECISIONS

- iteration 1: selected `scoreboard_host` at score 35; runner-up production_widget was blocked on buildlist_query

## NEXT

- **RULED, 2026-08-19 — the `chooseBuild` buildlist question is answered, and the answer is a
  per-type population cap.** Every sentence above and below that calls this question "awaiting a
  Director ruling" or "not discharged" was true when written; none is rewritten. The ruling: a side
  may not have more than N units of a type on the board at once; at the cap that type is ineligible,
  `buildPriorityLess` orders whatever remains, and when nothing eligible is affordable `chooseBuild`
  returns -1 exactly as it does today, so the side accrues Fame until the cap frees or the dearer
  unit becomes affordable. **Saving is emergent, not a second mechanism** — `nextCommand`'s economy
  block already reads `if (defIndex < 0) continue;`. Determinism is untouched (no RNG, no cursor, no
  new persisted state — population is board state `AiState::units` already carries), and Q9's ruled
  priority is untouched (the cap filters eligibility, the comparator still orders). Written up as
  §6 of `Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md`, including a change
  request drafted in `spec/ai_spec.md`'s own idiom, ready to file. **Three things this does NOT
  close.** (1) Nothing is implemented — the mechanism is in vendored bytes under
  `Source/StratRules/`; this repo may not edit them and no UE-side work discharges it. (2) **CORRECTED
  same day — the request IS now filed**, at crew `85995b8`, as change request 3 in
  `spec/ai_spec.md`; this clause was true for about an hour and is left rather than rewritten.
  (3) Two sub-questions go up with it rather than being decided here: where the cap numbers live
  (buildlist multiplicity — free, but silently re-reads every existing authored list as a quota
  table — versus an explicit parallel cap vector, recommended), and the requirement that the cap
  count `economy.pending` alongside alive units, without which a multi-factory board overshoots by
  one per extra factory in a single turn. Interim posture is unchanged and still Option A: author an
  Infantry-only buildlist, because a list naming Tank still reads as if a Tank might be built.
- **DISCHARGED 2026-08-19 -- the content-independence corpus, the last item carried out of the
  log-backed combat outcome milestone.** Phase D asked that the pairing invariant not rest on one
  corpus; phase 4 discharged the HOST half (same content, PIE instead of headless, byte-identical
  hashes) and the CONTENT half has been carried untouched through every pass since. It is now
  closed for two of the three axes the precedent named, and the third is stated as still open
  rather than folded in.
  - **What was built.** One clause,
    `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultOnDifferentContent`, appended to
    `Source/StratPlay/Tests/StratAiMatchClauses.cpp` so it reuses that file's world scope, config
    helper and `GLog` capture instead of duplicating them. Suite **107 -> 108** (macro census:
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST` in that file 4 -> 5), `succeeded 108 /
    succeededWithWarnings 0 / failed 0 / notRun 0`, 108 entries, zero non-Success,
    `reportCreatedOn 2026.08.19-18.40.01`.
  - **Both moved axes are derived, not typed.** First side comes from the projection's own side
    list, taking the side phase 3 did not start (`1` vs `kFirstSide`'s `0`), and the clause
    `TestNotEqual`s it so the axis cannot silently fail to move. The buildlist is the unit
    table's LAST row by its own row order, handed in alone (`Recon`), with a size assertion
    against the full table. No row name and no side number is written into the clause.
  - **Why the buildlist had to shrink to one entry, and the first LIVE evidence for the
    `chooseBuild` finding.** Cheapest-affordable means any multi-entry list fields the same army,
    so nothing short of exclusion changes the board. Measured on the phase-3 corpus's own
    checked-in slice: **22 builds, every one `def=0`** (Infantry). That request was filed upstream
    at crew `85995b8` argued purely from source; this is the first corpus in the project that
    shows it happening. Corroboration, not closure -- the cap is still unimplemented.
  - **The result, and the control, in one session.** New corpus: 42 `applied kind=Attack` / 42
    `STRAT-COMBAT resolved` / **0 pairing mismatches**, PASS, `EXIT=0`. Phase-3 corpus re-graded
    minutes later in the same session: 68/68, 0 mismatches, PASS -- identical to the figures
    phase 3 and phase 4 recorded on a different day, a determinism reading this pass got for
    free. The two games differ by 26 attacks, 3 turns (11 vs 8), 12 builds and the whole army.
  - **The gate can still FAIL on the new content, measured not assumed.** First two
    `applied kind=Attack` lines swapped in a scratch copy, nothing else touched: same 42/42
    totals, `pairing mismatches: 2`, both named, `FAIL`, `EXIT=1`. Ordered identity, not counts.
    The scramble was deliberately NOT added to `08-combat-pairing-gate/fixtures/` -- 
    `strat_fixture_verdict_binding.py` asserts that directory in both directions, so a 12th
    `.log` would have failed the binding rather than extended it.
  - **Evidence:** `Tools/architect/evidence/09-content-independence/` -- `blackboard.md`,
    `different-content-slice.log` (201 lines, cut at the test's own `Test Started.`/`Test
    Completed.` markers, both present as first and last line), `gate_output.txt` (all four runs
    verbatim with exit codes).
  - **STILL OPEN, and not closed by this: the SCENARIO axis.**
    `Data/ferrum_crossing.json` is the only scenario in the tree and `Data/` is vendored
    certified bytes, so a different map was not available without authoring one upstream. Two of
    the precedent's three axes moved; the third did not. Also unchanged: two corpora are not
    many, the `!bSeeded` silent path fired zero times here as in phase 3 (a measurement of these
    runs, not a structural guarantee), and `ReplayLog` still does not route through `Submit`.
  - **No reviewer verdict.** This is post-milestone work and has not been gated.
- **FILED UPSTREAM 2026-08-19 — the SCENARIO axis, which is STILL OPEN and is not closed by
  filing.** The axis above cannot move in this repo at all: `Data/` is vendored certified bytes
  and holds one scenario, and the crew repo holds one too — measured by git blob id rather than a
  worktree diff (CRLF vs LF), `ferrum_crossing.json` `64e3e3a8` on both sides, with
  `git diff --stat c2f5860 HEAD -- data/` empty upstream, so this is missing content and not a
  stale vendor. Filed against crew HEAD `85995b8` and landed there at crew `3b5ef9d`, as a new
  "Change requests for the Director" section in `spec/scenario_spec.md`, placed before Acceptance
  where `ai_spec.md` keeps its own. Spec-only upstream: no scenario authored, no source file
  touched, `sync_stratdata.py` unchanged, no acceptance ID moved.
  Investigation: `Tools/architect/evidence/upstream-second-scenario.md`.
  - **It is a ruling that is being asked for, not a task.** `spec/scenario_spec.md`'s own scope
    ruling forbids authoring *Longwater March* (§2.13.5) and *The Causeway* (§2.13.6) "not even as
    test fixtures" — decided on row 7's costs, before a downstream consumer wanting a second board
    for an unrelated reason existed. §2.13.7's four-clause stretch condition is quoted rather than
    argued around; whether week 4 is consumed by balance is not a fact this repo holds.
  - **The consumer side needs no code, and that is measured.**
    `UStratMatchSubsystem::ScenarioFile` is already a configured project-relative path and every
    clause that opens a scenario sets that string. What is missing is the datum.
    `ferrum_crossing_variant.json`, written under `Saved/` by `StratSelectionFacadeParity` arm 6,
    is the shipped board with the id relabelled and the hash line dropped — it is named in the
    filing precisely so it is not later mistaken for the axis having moved.
  - **§2.13.5 transcribes but for ONE value, checked by script and not by eye.** 13 × 8 = 104;
    glyph census (Plains 86 · Woods 4 · Mountains 4 · Towns 4 · Factories 6, no Water, no Bridge)
    identical to the section's own prose; every named coordinate on the right glyph; ρ-symmetric
    at all 104 hexes under ρ(c,r) = (12−c, 7−r), not only at the nine listed pairs; ten starting
    positions all on Plains, distinct, ρ-imaged, home factory hexes free; no terrain kind absent
    from `terrain.csv`. The missing value is **`turnCap`** — §2.13.5 gives an estimated match
    length and §2.8 makes the cap per-scenario data, so under the spec's own "author nothing" rule
    it is the Director's to state.
  - **Three costs are stated in the filing rather than folded in:** T-SCN-09's asserting branch,
    dropped by the scope ruling, would run for the first time (`rot180` on an even row count); row
    7's `*pending*` ledger posture would need restating rather than inheriting; and
    `sync_stratdata.py`'s `SCENARIOS` list is declared rather than globbed, so vendoring is an
    upstream edit plus a manifest entry under `GATE-DATA-VENDOR`.
  - **What was NOT done:** no scenario was authored, no loader was run, no path cost was priced,
    no ownership list was transcribed, and nothing was staged or committed in either repo.

- **RULED 2026-08-20 by the Director -- the T-UI-04 buildlist is a QUERY, not a snapshot field.**
  `Source/StratRules/Ui.h` states the gap in its own words and refuses to guess: *"There is
  deliberately no third query: T-UI-04's buildlist has no stated shape -- field or query -- and
  inventing one here would pre-empt a Director ruling."* The ruling is: a third `ui*` function
  beside `uiReachable` and `uiForecast`, delegating to the unit table and the side's `fameTotal`,
  answering "what can this side afford at this factory right now".
  - **Why, in the Director's terms:** it matches the two existing queries, and it keeps
    `UiSnapshot` fixed. 27 snapshot fields are already gate-pinned, so a
    `std::vector<UiBuildOption>` alongside `factories` would have moved the hash and parity
    clauses and made every consumer pay for a field only the production menu reads. The query
    charges the cost to the one caller.
  - **`UiFactoryView` already carries `buildWaiting` and `spawnBlocked`, and `UiSideView` carries
    `fameTotal`**, so the missing piece really is only the affordability answer -- not a new
    concept in the contract.
  - **THIS IS A CHANGE TO VENDORED CERTIFIED BYTES AND IS NOT AUTHORED HERE.** Non-negotiable 1:
    it is written in `E:\MultiAgent\stratocracy-crew`, certified there, and re-vendored into
    `Source/StratRules/`. `Data/StratData.manifest.json`'s `rulesCommit` moves with it and
    T-INT-01 hash-matches the result. **No filing has been made yet** -- this entry records the
    ruling, not its landing.
  - **Lane A (production menu, GDD SS2.11.5, the SS4.5 MVP blocker) stays BLOCKED until the
    re-vendor lands.** It is the one remaining feature on the hard MVP line.

- **FILED UPSTREAM 2026-08-20 -- the `uiBuildOptions` ruling now has a home in the crew repo.**
  The entry above closes with "**No filing has been made yet** -- this entry records the ruling,
  not its landing." That sentence was true when written and is superseded here rather than
  edited: the filing exists, **uncommitted**, in `E:\MultiAgent\stratocracy-crew` against crew
  `HEAD 1d5a42f`. 123 insertions, `spec/ui_spec.md` only. Staging and the commit are the user's,
  in that repo exactly as in this one.
  - **The filing shape was copied from the two precedents rather than invented.** `85995b8`
    (the `chooseBuild` cap) and `3b5ef9d` (the scenario axis) are both **spec-only** -- neither
    touched a header. The `## Change requests for the Director` section goes before
    `## Acceptance`, where `ai_spec.md` and `scenario_spec.md` both keep theirs. Whoever
    implements the ruling changes `cpp_reference/Ui.h`; this repo then re-vendors and
    `rulesCommit` moves off `cb8e12b`.
  - **CHECKED FIRST, BECAUSE THE RULES REPO IS OFTEN AHEAD:** the `Queries (§4.7 Stub 8)`
    comment block above `uiReachable` in `Ui.h` at crew HEAD still carries the refusal verbatim,
    **[CORRECTED 2026-08-20, caught by a reviewer gate: this cited `Ui.h:326`, a line number into
    a vendored header -- the one citation shape this project bans, written into the one file
    class no guard scans. It was also off-target: the refusal spans three lines and 326 carries
    only its middle fragment. The bullet two sentences below predicts the very re-vendor that
    would have moved it.]** and no `uiBuildOptions` or `UiBuildOption` symbol exists
    anywhere in `cpp_reference/`. This was not already built. Our vendored `cb8e12b` IS an
    ancestor of crew HEAD (14 commits behind), and `cpp_reference/` has moved in that span --
    `test_balance.cpp`, `test_replay.cpp`, `test_save.cpp` -- so a re-vendor will carry more
    than the buildlist query when it happens.
  - **THREE RESIDUAL DECISIONS THE RULING DOES NOT SETTLE, and they are the reason the filing is
    worth more than the one-word answer.** All three were found by reading the module, not by
    reasoning from the brief.
    1. **Affordable-only, or all four rows with a flag?** `T-UI-03` forbids widget-side
       arithmetic. If the query returns only what the side can afford, a menu that greys out the
       rest must decide affordability itself -- the exact arithmetic that clause forbids -- and
       must invent the omitted rows besides. Costs make it concrete: Infantry 100, Recon 150,
       Artillery 200, Tank 300, so at 150 Fame an affordable-only list is ONE row long.
       **Recommended in the filing:** return all four with a module-computed `affordable`.
    2. **What factory state does to the answer -- already unruled in `Ui.h` itself.**
       `spawnBlocked`'s comment records that **Q31** (may a player queue into a boxed-in
       factory?) is unruled and that `buildWaiting` is the field such a ruling would bind to. A
       per-factory query cannot dodge it.
    3. **Does the per-type population cap bind the PLAYER?** `85995b8` reads "a side may not have
       more than N units of a type on the board at once" -- **a side**, not the AI -- and it is
       ruled but NOT implemented (no cap symbol in `cpp_reference/` at `1d5a42f`). If it is a
       game rule the menu must reflect it; if AI policy, it must not. Two implementers would
       otherwise each guess, differently.
  - **TWO TRAPS RECORDED FOR WHOEVER IMPLEMENTS IT.**
    - **The third query does NOT delegate, and `Ui.h` says both queries do.** `uiReachable`
      forwards to `Move.h::reachable` and `uiForecast` to `Combat.h::resolveDamage`; nothing
      answers "what can this side afford". **`chooseBuild` is not it** -- it takes `AiState`,
      not a `UiWorld`, and returns the ONE cheapest affordable entry rather than the set, so a
      menu built on it offers the player a single choice. The filing says to update that comment
      in the same commit, and names `standingIncomeRate` and `spawnHexesBlocked` as the existing
      precedent for a module-side derivation that is not a delegation.
    - **T-UI-04's flag clause would be a CLAUSE THAT CANNOT FAIL if asserted against this
      query.** T-UI-04 says "the flag never appears". But `isFlag` is a `Scenario.h` PLACEMENT
      field valid only on a Tank, and `data/units.csv` has exactly four rows -- Infantry, Tank,
      Artillery, Recon -- with no flag row. A query enumerating table rows therefore *cannot*
      emit a flag, so a clause asserting its absence passes on an empty implementation and a
      wrong one alike. This is the shape this project has paid for before. The filing says: write
      no filter for a case that cannot arise, and gate the clause where a flag could actually
      reach a buildlist -- placement and ledger level -- if the Director wants it to bite.
  - **Every quotation in the filing was verified verbatim against its source** rather than
    retyped from memory: `Ui.h`'s no-third-query refusal, `Ui.h`'s Q31 sentence, and
    `ai_spec.md`'s cap wording all compare equal after whitespace normalisation.
  - **Lane A is still BLOCKED.** Filing is not landing, and landing is not vendoring. The
    production menu (GDD SS2.11.5) stays the last feature on SS4.5's hard MVP line until the
    ruling is implemented upstream and re-vendored here.

- **LANDED UPSTREAM 2026-08-20 -- the `uiBuildOptions` filing is committed at crew `4d36a16`.**
  The entry above says the filing "exists, **uncommitted**, against crew `HEAD 1d5a42f`". That
  was true when written and is superseded here rather than edited, which is this file's
  convention. It is now a commit: *"The buildlist gets a query, and T-UI-04's flag clause cannot
  fail"*, one file, `spec/ui_spec.md`, 123 insertions and no deletions, against that same parent.
  - **Spec-only, verified rather than asserted:** `git diff-tree --no-commit-id --name-only -r`
    on that commit lists `spec/ui_spec.md` and nothing else. No header is touched, no source file
    is written, and no acceptance ID moves -- the same shape as the two precedents it was
    modelled on, `85995b8` and `3b5ef9d`.
  - **THIS REPOSITORY IS UNCHANGED BY IT.** `Source/StratRules/` is byte-identical and
    `StratRules.manifest.json`'s `rulesCommit` still reads `cb8e12b`. A filing that has landed
    upstream has not been implemented, and an implementation has not been vendored. Those are
    three separate events and only the first has happened.
  - **Lane A remains BLOCKED.** The production menu is still the last feature on the GDD's hard
    MVP line, and it stays blocked until someone writes the query into `cpp_reference/Ui.h`
    upstream and this repository re-vendors the result. Filing is not landing; landing is not
    building; building is not vendoring.

- **RE-MEASURED 2026-08-21 — Lane A's §2.11.5 production-menu blocker still stands, re-derived
  fresh rather than inherited from the entries above and below it.**
  - `git -C E:/MultiAgent/stratocracy-crew status --porcelain` -> empty; `git -C
    E:/MultiAgent/stratocracy-crew rev-parse HEAD` -> `4d36a160ff18f203a19918e5c30274c4a160412d`,
    clean.
  - `git -C E:/MultiAgent/stratocracy-crew grep -n "uiBuildOptions"` -> two hits, both in
    `spec/ui_spec.md` (lines 268, 298); `git -C E:/MultiAgent/stratocracy-crew grep -n
    "uiBuildOptions" -- cpp_reference/` -> zero hits, non-zero exit. The spec entry has landed;
    the header change has not.
  - `git -C E:/MultiAgent/stratocracy-crew log -1 --format='%s%n%n%b'` on `4d36a16` confirms its
    own text verbatim: *"Spec-only. No header is touched, no source file is written, no
    acceptance ID moves."*
  - `Source/StratRules/StratRules.manifest.json` pins `"rulesCommit":
    "cb8e12b3a897c7329497ced4d1c6207630f37101"`. `git -C E:/MultiAgent/stratocracy-crew
    rev-list --count cb8e12b..HEAD` -> **15**, agreeing with the entry below rather than the 14
    this task's own dispatch prompt stated -- the tree wins, and this exact figure was already
    corrected once, at the `FILED UPSTREAM 2026-08-21` entry below.
  - **Nothing here is new.** Every figure matches what the entries above and below it already
    record; this entry exists because it was independently re-derived, not copied, and none of
    the re-derivations disagreed.
  - **Lane A remains BLOCKED**, unchanged. Filing is not landing, and landing is not vendoring.

- **RULED, 2026-08-21, by the user — beat 2 of the guided opening retires ONLY on a capture pip
  at `guidedOpening.objective`.** GDD §2.11.6 stays AMBIGUOUS on this point; nothing in the
  document itself settles it, and this entry records that the code follows a ruling, not a
  reading. §2.11.6-B's own turn/beat table (`Turn 2 | beat 2 (rule 1) — retires on the pip`) and
  the "giving up the line is not retiring" prose (the ring and the marked Infantry are the same
  instruction in spatial form) are consistent with the ruling but do not, on their own, choose it
  over some other trigger — that is why it took a ruling and not a re-read.
  - **Why this is load-bearing rather than cosmetic.**
    `Stratocracy.StratPlay.T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile` is now the clause
    that would go red if beat 2's implementation ever regressed to a nearest-factory heuristic
    instead of reading `guidedOpening.objective` off the scenario file: a nearest-factory guess
    would retire the beat on the WRONG TILE, silently, with a green build everywhere else — no
    other clause in the suite pins the objective hex to the authored field rather than to "some
    plausible factory". The suite count and phase verdict for the pass that landed this are
    recorded once, in `state/global.md`'s banner (2026-08-21, LANE B LANDED); this entry does not
    restate them.
  - **Two acceptance-ID reassignments, both CONFIRMED by the coordinator on 2026-08-21, neither
    minting a new ID.**
    1. **The guided-opening lookup moves from `T-SCN-02` to `T-SCN-07`.** `T-SCN-02` is §4.7's
       structural-validity gate — every hex reference in bounds, no two placements sharing a hex
       — a property of the *file*, not of the two authored `guidedOpening` fields. `T-SCN-07` is
       GDD 4.7's "opening-capture naming", whose subject is exactly `guidedOpening.infantry` and
       `guidedOpening.objective`. **Re-verified here, not inherited:**
       `grep -n 'T-SCN-0[27]' Source/StratRules/Scenario.good.cpp` shows every `guidedOpening`
       refusal (the entry count per side, the infantry/objective hex existence and ownership
       checks, the shared-objective check) carrying `"T-SCN-07"` — lines 769–805 of that file —
       and `T-SCN-02` appearing only in the unrelated structural block above it (lines 584–620:
       bounds, terrain Id, ownership hex, placement hex, unit Id, duplicate-hex checks). No
       `guidedOpening` refusal carries `T-SCN-02`.
    2. **The Q27 input gates (End Turn inert during beat 1a until the marked Infantry has moved;
       Wait/`Space` inert on the same footing; Attack closed because the SELECTED → attack
       transition never lights) are filed under `T-UI-02`.** This is the precedent this tree
       already set at `870c611`: `T-UI-02.ControllerTicksSoInputDispatches` and
       `T-UI-02.BoardHexRoundTrip` both pre-date this ruling and neither is about the reachable-
       hex set either — `T-UI-02` is in practice this tree's ID for the StratPlay input surface.
       **RECORDED AS A PARTIAL FIT, because it is one.** `T-UI-02`'s written invariant, GDD 4.11
       (`§4.11` queries/invariants block): *"the reachable-hex highlight displays exactly the
       T-MOVE-01 set — the UI queries the module and never recomputes movement (§2.5)"* —
       verified verbatim at `Stratocracy_Prototype_GDD.md:2505`. That sentence is not about input
       gating at all. **The GDD ships no acceptance ID for §2.11.6-B's one input-gating
       constraint.** GDD 4.7 states it in prose only, at the beat-1a directive row: *"adopted
       under **Q27** (§4.7), ruled — it was registered rather than assumed because it gates an
       input"* — no `T-` id accompanies that sentence. The full family list was enumerated over
       the GDD, not recalled: `grep -oE 'T-(AI|CAP|COMBAT|DATA|FAME|HEX|INT|MOVE|REPAIR|SAVE|SCN|
       TURN|UI)-01'` returns exactly those thirteen families and no onboarding family exists among
       them. Because a test may not mint an ID, the gates are filed under the nearest precedent
       (`T-UI-02`) and the gap itself is FILED UPSTREAM — see
       `Tools/architect/evidence/upstream-onboarding-input-gating-acceptance-id.md`. Filing is not
       landing, and this reassignment does not become a clean fit until the GDD ships its own ID.

- **FILED UPSTREAM 2026-08-21 — GDD 4.11 has no acceptance ID for §2.11.6-B's Q27 input-gating
  constraint (End Turn / Wait / Attack during beat 1a).** Filed against
  `E:\MultiAgent\stratocracy-crew` at HEAD `4d36a16` (measured 15 commits ahead of this repo's
  vendored `rulesCommit cb8e12b`, not the 14 this task's brief stated — `git log --oneline
  cb8e12b..4d36a16 | wc -l` — the tree outranks the brief). Investigation and the drafted change
  request are at
  `Tools/architect/evidence/upstream-onboarding-input-gating-acceptance-id.md`. **This is a
  filing, not a landing** — no file in either repo was touched by this pass, nothing was staged,
  nothing was committed. Distinct from, and NOT to be conflated with, the still-open
  `uiBuildOptions` item above (Lane A / GDD §2.11.5) — that one is a header change awaiting
  implementation and a re-vendor; this one is a spec-only acceptance-ID gap.

- **FILED UPSTREAM 2026-08-21, on the user's explicit instruction — a scenario whose
  `guidedOpening.infantry` starts adjacent to an enemy unit.** Drafted against crew `main` HEAD
  `4d36a16` and landed the same day at crew **`a8c8cdb`**, as item 2 of the existing "Change
  requests for the Director" section in `spec/scenario_spec.md`, placed immediately before
  `## Acceptance` — the same placement and shape item 1 (the second-scenario request, `3b5ef9d`)
  already uses. **Independently re-verified against the crew repo, not taken on the dispatch
  prompt's word:** `git -C E:\MultiAgent\stratocracy-crew log --oneline -3 main` shows `a8c8cdb`
  on `main` with `4d36a16` as its immediate parent and the stated subject line; `git -C
  E:\MultiAgent\stratocracy-crew show --stat a8c8cdb` shows exactly one file touched,
  `spec/scenario_spec.md`, 61 insertions and 0 deletions — spec-only, `data/` and `cpp_reference/`
  untouched, no manifest or hash moved, `sync_stratdata.py` unchanged, no acceptance ID moved, all
  as claimed. Investigation and the filed change-request text (quoted from the landed diff, not
  the pre-filing draft) are at
  `Tools/architect/evidence/upstream-guided-opening-adjacency.md`, §4 and §8.
  - **This is a filing, not a landing, in the sense that matters for content.** A commit now
    exists carrying the request text (`a8c8cdb`), which is what distinguishes this from the still-
    drafted-only Q27 entry above. But no Director has ruled on either half of the question in §1,
    no scenario has been authored with the adjacency property (`data/ferrum_crossing.json` remains
    the only scenario, and neither *Longwater March* nor *The Causeway* has been drafted with it),
    and nothing in this repo's vendored `Data/` moved — `Data/StratData.manifest.json`'s
    `dataCommit` is unchanged. The evidence file's own §4 was stale in the OPPOSITE direction
    before this pass (it said "not filed" after filing had happened); that is corrected there, not
    here — this entry does not restate a suite count or a phase verdict, both of which stay in
    `state/global.md` only.
  - **Distinct from, and NOT to be conflated with,** the still-open SCENARIO axis entry above
    (2026-08-19, landed `3b5ef9d`) and the still-drafted-only Q27 acceptance-ID gap immediately
    above this entry. All three are separate crew-repo filings at different commits, on different
    subjects, in different states of landing.

- **FILED UPSTREAM 2026-08-21, on the user's explicit instruction — the Q27 acceptance-ID gap
  named two entries above is no longer draft-only; it is now filed at crew `3510bc2`.** The entry
  above it ("FILED UPSTREAM 2026-08-21 — GDD 4.11 has no acceptance ID...") described a filing
  that was, at the time it was written, drafted but not yet committed anywhere. That was true when
  written and is superseded here rather than edited, per this file's own convention. Drafted
  against crew `main` HEAD `4d36a16` and landed the same day at crew **`3510bc2`**, as item 2 of
  the existing "Change requests for the Director" section in `spec/ui_spec.md`, placed immediately
  before `## Acceptance` — the same placement item 1 (the `uiBuildOptions` ruling, `4d36a16`)
  already uses. **Independently re-verified against the crew repo, not taken on the dispatch
  prompt's word:** `git -C E:\MultiAgent\stratocracy-crew log --oneline -3 main` shows `3510bc2`
  on `main`, one commit ahead of `a8c8cdb` (the adjacency filing); `git -C
  E:\MultiAgent\stratocracy-crew show --stat 3510bc2` shows exactly one file touched,
  `spec/ui_spec.md`, **64 insertions, 4 deletions** — spec-only, `data/` and `cpp_reference/`
  untouched, no manifest or hash moved, `sync_stratdata.py` unchanged, no acceptance ID minted.
  Investigation and the filed change-request text (quoted from the landed diff, not the
  pre-filing draft) are at
  `Tools/architect/evidence/upstream-onboarding-input-gating-acceptance-id.md`, §7-§9.
  - **The 4 deletions are an intro rewrite, not a retraction, and it is exactly the failure this
    record keeps paying for if left unqualified.** `spec/ui_spec.md`'s "Change requests for the
    Director" section previously opened "There is one, and it is not a question" — true while
    the `uiBuildOptions` ruling was the section's only entry. Item 2 (this filing) falsifies that
    count, so the same commit rewrites the intro to "There are two, and they are different in
    kind" and states which is a ruling awaiting implementation and which is a naming question
    awaiting a ruling.
  - **One substantive correction the filing made to its own draft, carried into the record here
    because a Director asked to name "the Q27 constraint" needs to know which sentence ruled
    what.** The draft's §1 (in the evidence file) describes "the three player-input gates
    §2.11.6-B beat 1a specifies" in a way that reads as though Q27 ruled all three. Verified
    directly against `Stratocracy_Prototype_GDD.md` (grepped, never `cat`, 446 KB / ~100 KB
    lines): Q27's own ruling text (the `| **Q27** |` row) names **End Turn** only. The
    §2.11.6-B beat-1a row is what separately extends the constraint to **Space** ("inert for it
    on the same footing as End Turn and for the same reason") and closes the **Attack**
    transition (§2.11.1) to that unit. The filed request states this split explicitly; the
    evidence file's §1 does not, and is left as-is per this file's "never rewritten" convention —
    the correction is recorded in the evidence file's own §7 instead of retroactively edited into
    §1.
  - **"Filed" is not "granted", and this entry states it plainly rather than leaving it
    implied.** No Director has ruled on either branch of the request (mint an ID, e.g. `T-UI-06`,
    or rule that Q27 stays unassigned by design). `T-UI-06` does not exist in `cpp_reference/` or
    `spec/ui_spec.md` — it appears only as an example inside the quoted request text. The
    downstream clauses named in the evidence file's §1 stay filed under `T-UI-02` as a recorded
    partial fit, unchanged by this filing.
  - **Distinct from, and NOT to be conflated with, the two neighbouring filings this same day.**
    The `uiBuildOptions` item (Lane A / GDD §2.11.5) is a header change awaiting implementation
    and a re-vendor. The adjacency request (crew `a8c8cdb`, `spec/scenario_spec.md` item 1) is a
    board-content ruling. This one is an ID-assignment request, and both possible answers —
    minting an ID or ruling Q27 stays unassigned by design — are legitimate outcomes; the filing
    presumes neither.
  - **The commit gap re-derived fresh, not inherited.** `git -C E:/MultiAgent/stratocracy-crew
    rev-list --count cb8e12b..HEAD` against crew HEAD `3510bc2` on 2026-08-21 returns **17** —
    two more than the 15 recorded at the `RE-MEASURED 2026-08-21` entry above, reflecting the
    crew's two moves since then (`a8c8cdb`, then `3510bc2`). This entry does not restate a suite
    count or a phase verdict; both stay in `state/global.md` only.

- **RULED 2026-08-21, by `strat-integration-reviewer` — a retraction or repair to this record
  must derive its own blast radius, not enumerate it.** A fix applied only to the sentences a
  gate names, rather than derived over the whole passage it sits in, leaves the tail asserting
  what the head withdrew. This session's record-gate sequence on `global.md` produced this
  finding three times in three shapes, and the ruling is to write it down as a standing rule
  rather than leave it scattered through the bullet it happened in. Full blow-by-blow is in the
  commit messages of `68d56d1`, `de517f0`, `c21e7a5`, `d67471b`, `a55f924` and `8eef22b`, cited
  rather than retold here.
  - **The three shapes, each its own evidence for the rule.** (1) A correction retracted a claim
    where the evidence was discussed, and the same claim stood unretracted in the "what remains"
    sentence the next lane reads as its task. (2) A gate named three sites carrying a retracted
    claim; the fix marked those three rather than deriving the list from the retraction itself,
    and missed a fourth — the one carrying both the whole withdrawn argument and a dispatch
    instruction the correction forbids. (3) A numbered item was rewritten to open a closed
    disjunction, and its own closing sentence — untouched because it was not one of the named
    sentences — went on restating the closure affirmatively.
  - **The rule.** When a claim in `Tools/architect/state/` is retracted or repaired, derive the
    set of affected sites from what the retraction withdraws — not from a gate's or a reviewer's
    named list of sites, which is itself a sample — walk every derived site, and state explicitly
    which sites were checked and deliberately left alone, with the reason. Naming the sites left
    alone is what makes the sweep falsifiable: the same reasoning this project already holds for
    guard code, that a checker which types its own subject list stops covering that subject
    silently.
  - **Two measured facts that make this enforceable rather than aspirational, not merely
    advisory.** First: a phrase-level `grep` over this record MISSES a claim that straddles a
    line break — `grep "does not take"` returned only an unrelated hit while the claim still
    stood in `global.md`; `rg -U` (multiline) found it. The record wraps at roughly 100 columns,
    so any three-word phrase will eventually straddle a line. **Any claim-shape sweep over
    `Tools/architect/state/` must be multiline.** Second: the same habit shows up as whitespace —
    patching a sentence without re-flowing its paragraph left eight short fragment lines in one
    bullet. No guard reads it and it is cosmetic, but it is the same failure shape: a change
    scoped to the named span rather than derived over the passage it sits in.
  - **This entry does not state a suite count or a phase verdict** — both stay in
    `state/global.md` only; see that file for the live figure.

- **RECORDED 2026-08-23, ON THE COORDINATOR'S REQUEST AFTER `strat-integration-reviewer` DECLINED
  TO ASSIGN IT — the match-does-not-end entry at the top of `global.md`'s `## NEXT` took four
  `BLOCK`s to reach its `VERDICT: PASS`, and every one is the SAME DEFECT CLASS, not four
  unrelated mistakes.** The reviewer's own closing note said this belonged in `decisions.md` as a
  standing caution about that entry's provenance — written fast against a live log — and said
  assigning it was not its call. All four are re-derived below against
  `Saved/Logs/Stratocracy.log` and the current tree; the entry as it now stands is correct on all
  four, so nothing here corrects `global.md` — this bullet is the caution itself.
  - **ORDINAL.** A drafted version of the entry called the flag kill the AI's "eighth" applied
    command on turn 7. `grep -n "STRAT-AI applied ... turn=7 side=1" Saved/Logs/Stratocracy.log`
    returns exactly nine lines, and the killing `Attack unit=6` line is the ninth, not the eighth.
    The entry's own later sentence already quoted the runner's report of "9 applied command(s)"
    two bullets below the wrong ordinal — it disagreed with its own cited evidence in the same
    passage. The version now in the tree reads "NINTH AND LAST applied command" and matches.
  - **ATTRIBUTION.** A drafted version credited the single quoted line `Selection: unit 24 belongs
    to side 0 and side 1 is to move` with firing "four times". `grep -n "Selection: unit .*
    belongs to side .* and side .* is to move" Saved/Logs/Stratocracy.log` shows that exact line
    firing exactly TWICE (both for unit 24); the count of four belongs to the SHAPE across
    **[CORRECTED 2026-08-23, `strat-integration-reviewer` `VERDICT: BLOCK`, stamped in place per
    this file's own header ("Never rewritten -- an entry that has moved is stamped in place"):
    this sentence read "four distinct units -- 24, 24, 5 and 21". Distinct units are THREE (24, 5,
    21 -- 24 fires twice), not four. Four is the OCCURRENCE count, not the distinct-unit count --
    the fourth instance of this very entry's own defect class, found inside the bullet teaching
    it. Correct wording: four times across three distinct units -- 24, 24, 5 and 21.]** three
    distinct units — 24, 24, 5 and 21 — not to the one exemplar. The version now in the tree says
    so explicitly: "the count is the shape's, and no single one of those lines carries it."
  - **FABRICATED SYMBOL.** A drafted version attributed a verbatim quotation to `FStratAiTurnPort`.
    That name exists nowhere in the tree —
    `grep -rn "FStratAiTurnPort\b" Source/` returns zero hits. It fuses two real, distinct symbols
    in `Source/StratPlay/StratAiTurnRunner.h`: the interface `IStratAiTurnPort` (line 105), which
    owns the comment quoted ("deliberately no `IsMatchOver()` and no `IsSideAi()`...",
    **[CORRECTED 2026-08-23, `strat-integration-reviewer` `VERDICT: BLOCK`, stamped in place: this
    citation read "lines 90-93 and 105-107". The quotation is fully contained in lines 90-92; line
    93 is a different sentence not in the quote, and lines 105-107 (`struct STRATPLAY_API
    IStratAiTurnPort`, `{`, the destructor) are the struct's opening brace and body, not comment at
    all, and needlessly re-cite 105 which this same sentence already gives as the declaration line.
    Correct span: lines 90-92.]** lines 90-92), and its concrete implementer
    `FStratBridgeAiTurnPort` **[CORRECTED 2026-08-23, `strat-integration-reviewer`
    `VERDICT: BLOCK`, stamped in place -- this is the finding that matters most, because
    the bullet committed the exact defect it exists to name: this citation read "(named
    at line 97)". Line 97 (`* adapter (`FStratBridgeAiTurnPort`, below) becomes a named,
    separately assertable thing`) is a PROSE MENTION inside a doc comment -- exactly the
    kind of hit this same bullet's own closing sentence, three bullets below, tells the
    reader to reject. Re-derived: `grep -n "FStratBridgeAiTurnPort"
    Source/StratPlay/StratAiTurnRunner.h` returns four lines -- 48 and 97 are prose
    mentions, 150 is the declaration (`struct STRATPLAY_API FStratBridgeAiTurnPort final
    : public IStratAiTurnPort`), 152 is the constructor. Correct citation: declared at
    line 150, not "named at line 97".]** (declared at line 150).
    Under this project's cite-by-symbol convention a wrong symbol is UNGREPPABLE — the reader gets
    zero hits and cannot tell whether the evidence moved, was renamed, or never existed. It also
    silently weakens the argument the entry is making: the inference rests on a PORT delegating,
    and an `F`-prefixed name points at the adapter rather than the interface contract that
    actually states the omission. The version now in the tree names `IStratAiTurnPort` and says
    "the INTERFACE, not the concrete `FStratBridgeAiTurnPort`" — correct on both counts.
  - **UNREPRODUCIBLE EVIDENCE.** A drafted version cited the diagnostic regex
    `MatchOver|EndMatch|OnMatchEnd|MatchEnded|Victory|Defeat|GameOver` wrapped mid-alternation
    across a line break, so copying it out of the record reconstructed a broken pattern. The
    version now in the tree carries it on one unbroken line (`global.md`, the "AND THE REASON IS
    THAT NO END-OF-MATCH TRANSITION EXISTS TO MISFIRE" bullet).
  - **THE COMMON SHAPE, which is the point of this entry.** Every one of the four figures was
    REAL — nine applied commands, four refusal lines, a real interface, a real regex — and each
    had drifted onto the wrong SUBJECT: the wrong ordinal for a correctly-counted total, the wrong
    line for a correctly-counted shape, the wrong symbol for a correctly-quoted comment, the wrong
    line-wrap for a correctly-chosen pattern. None was a wrong measurement. All four survived a
    re-read and were caught only by RE-DERIVING against the log and the tree. This is the
    project's existing "subject, not scope" concern, but sharper — it recurred three times in one
    entry written in a single pass.
  - **A COUNT VERIFIED IN THE AGGREGATE IS NOT VERIFIED IN ITS ATTRIBUTION.** The reviewer
    volunteered this against its own first audit: it confirmed the four-line SHAPE count and
    reported "exactly four" without checking that the quoted exemplar line actually carried that
    count. Verifying a total is not verifying its subject.
  - **BEFORE WRITING ANY SYMBOL INTO THIS RECORD, GREP IT, AND REQUIRE A DEFINITION RATHER THAN A
    PROSE MENTION.** The fabricated name returned exactly one hit in the drafted text and it was
    its own sentence — no definition, no other reference. In UE, check the prefix against the
    declaration site — `I` interface, `F` struct, `U` UObject, `A` Actor — because adjacent real
    symbols blend into a plausible third that greps like a hit until you look for where it is
    DEFINED rather than where it is SAID.
  - **This entry does not state a suite count or a phase verdict** — both stay in
    `state/global.md` only; see that file for the live figure.

- **CORRECTED 2026-08-23, SAME DAY, ON THE COORDINATOR'S FLAG — the FABRICATED SYMBOL bullet
  above commits its own defect class while stating the rule against it.** **[SUPERSEDED 2026-08-23,
  SAME DAY, BY `strat-integration-reviewer` `VERDICT: BLOCK`: this entry's mechanism was wrong, not
  its content. It corrected the site by appending a new entry below rather than stamping the
  defect in place at the sentence that carried it, which does not satisfy this file's own header
  ("Never rewritten -- an entry that has moved is stamped in place"): a reader following the
  original bullet's line-97 citation got it as supporting evidence with no signal that a
  withdrawal existed ~40 lines below. The three findings this entry raised (span 90-93/105-107,
  the line-97 citation, and — caught only on a second gate — the "four distinct units" miscount)
  are now stamped in place, inline, at each site in the entry above, in the marker style
  `content.md` and `global.md` already use elsewhere in the tree today. This entry is kept rather
  than deleted, per append-only, and its own reasoning about keeping accurate line numbers and
  citing the symbol first (the sub-bullet below) still stands and was not superseded.]** It cites
  `FStratBridgeAiTurnPort` "(named at line 97)" as support for the fabricated-symbol finding. Line
  97 of `Source/StratPlay/StratAiTurnRunner.h` is `* adapter (`FStratBridgeAiTurnPort`, below)
  becomes a named, separately assertable thing` — a PROSE MENTION inside a doc comment, not the
  declaration. Re-derived directly rather than trusting the coordinator's message:
  `grep -n "FStratBridgeAiTurnPort" Source/StratPlay/StratAiTurnRunner.h` returns four lines — 48
  (a different prose mention, inside a separate comment), 97 (the one cited, also prose), 150 (`struct
  STRATPLAY_API FStratBridgeAiTurnPort final : public IStratAiTurnPort` — the actual declaration),
  and 152 (the constructor). The bullet's own closing sentence reads "REQUIRE A DEFINITION RATHER
  THAN A PROSE MENTION ... look for where it is DEFINED rather than where it is SAID," and its own
  supporting citation is exactly the kind of hit that sentence tells the reader to
  reject. **[CORRECTED 2026-08-23, `strat-integration-reviewer` `VERDICT: BLOCK`,
  non-gating, stamped for consistency: this sentence read "Left in place above rather
  than rewritten, per this file's append-only convention", describing the FABRICATED
  SYMBOL bullet as unedited. It no longer is -- the bullet is now stamped in place,
  inline, at each site this entry and the gate's later findings named. The substance
  this sentence went on to state (the bullet's core claim unaffected, only the line-97
  citation wrong) was and is correct; only the "left in place ... unedited" framing went
  stale once the in-place stamps landed.]** The bullet's substance (the fabricated
  symbol, its two real referents, the tree's current correct text) is unaffected; only
  the line-97 citation for `FStratBridgeAiTurnPort` was wrong. The correct citation for
  that symbol's declaration is line 150.
  - **ON CITING BY LINE NUMBER AT ALL, IN A BULLET ARGUING FOR CITE-BY-SYMBOL.** The coordinator
    raised this as a second, lower-stakes point and left the call to this file's writer. The
    project's standing finding (see the RULED 2026-08-21 blast-radius entry above, and
    `cite-functions-not-line-numbers` in project memory) is that a `file:NNN` written during a
    change is invalidated by that same change — measured three times in one milestone.
    Judgement, and its premises: **[CORRECTED 2026-08-23, `strat-integration-reviewer`
    `VERDICT: BLOCK`, stamped in place -- the SUPERSEDED marker above vouched for this
    sub-bullet and both of its premise clauses were false. This sentence read "the line
    numbers in the FABRICATED SYMBOL bullet are currently accurate ... and are left as
    written rather than retrofitted to symbol citations, because rewriting them would
    violate append-only for no correctness gain today." "Currently accurate" was false
    WHEN WRITTEN -- this same entry's headline finding is that the line-97 citation and
    the 90-93/105-107 span were both wrong; the sub-bullet asserted accuracy in the
    entry that disproves it. "Left as written rather than retrofitted" is false NOW --
    "declared at line 150" was retrofitted into the FABRICATED SYMBOL bullet this same
    pass. The CONCLUSION below is sound and stands unstamped: cite the symbol first,
    treat the line number as provisional, re-grep to re-find it if the file is ever
    edited.]** The risk the standing finding names is real and this bullet is exactly
    the kind of place it would recur, so the citation here is stated as a symbol first:
    `FStratBridgeAiTurnPort`'s declaration is the `struct STRATPLAY_API FStratBridgeAiTurnPort final : public
    IStratAiTurnPort` line in `Source/StratPlay/StratAiTurnRunner.h`, presently line 150 — grep the
    symbol to re-find the line if this file is ever edited, rather than trusting the number.
  - **This entry does not state a suite count or a phase verdict** — both stay in
    `state/global.md` only; see that file for the live figure.
