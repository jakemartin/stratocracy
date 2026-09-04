# Gate: shipped-`Difficulty` pin — RE-GATE 4 — strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`, over base commit
`9d705ca`. Verified by absolute path, not inferred:

```
$ git -C E:/MultiAgent/Stratocracy rev-parse HEAD
9d705ca6d7b639312ce761379786237e2d381cd3

$ git -C E:/MultiAgent/Stratocracy branch --show-current
master
```

No worktree under `E:/MultiAgent/Strat-wt/` or `.claude/worktrees/` was read or audited. `git
worktree list` shows seven trees; all work below is in the main one.

Three tracked files modified, four untracked gate reports present, nothing staged:

```
$ git status --porcelain
 M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md

$ git diff --stat
 .../Tests/StratShippedGameModeMatchConfig.cpp      | 148 +++++++++++++++++-
 Tools/architect/state/global.md                    | 167 ++++++++++++++++++++-
 Tools/architect/state/tests.md                     | 126 +++++++++++++++-
 3 files changed, 431 insertions(+), 10 deletions(-)
```

**The brief's claim that `global.md` is the only change since the last gate is CONFIRMED against
the tree, by mtime, and it is load-bearing for FINDING 1 below.** RE-GATE 3 finished at 01:50:28.
Only one file has been touched since:

```
$ ls -l --time-style=full-iso <subjects>
2026-09-04 00:47:20  Saved/AutomationReport/index.json
2026-09-04 00:44:16  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
2026-09-04 00:29:09  Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
2026-09-04 01:14:22  Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
2026-09-04 01:31:02  Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
2026-09-04 01:50:28  Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
2026-09-04 00:49:03  Tools/architect/state/tests.md
2026-09-04 01:51:36  Tools/architect/state/global.md      <- the only edit after RE-GATE 3
```

Two consequences, both measured rather than reasoned. (a) The four predecessor gate reports are
byte-unmodified since they were written — the banner's "each a new file leaving its predecessors
byte-identical" holds. (b) `tests.md` was last written at 00:49:03, BEFORE the first gate report
was even filed, so nothing in it has been revisited across three re-gates. That is why FINDING 1
is where it is.

Also verified: the `.cpp` at 00:44:16 pre-dates the exported report at 00:47:20, so the cited run
post-dates the last source change. Its current content hash, recorded so a future pass can prove
the clause body did not move under a rename claim:

```
$ sha256sum Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
6ebf03a9578f8ff50946f893d55919ec04953ea06424f17b8241c2fa0ad03a90
```

---

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No `strat::` call outside `StratBridge/`, `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct (16 rows, 10 fields) | pass |
| 6 | New modules registered / `StratRules` deliberately absent | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal outside the automation-fixture exception | pass |
| 9 | No widget-side arithmetic | pass — no `StratUI` file in the diff |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| R1 | THE REPAIR — observation count now correct and FAITHFUL | pass |
| R2 | THE REPAIR — count-free formulation genuinely count-free | **BLOCK** — FINDING 2 |
| R3 | THE REPAIR — the glob resolves to the re-gate reports | pass |
| R4 | Nothing in the current banner falsified by THIS report landing | **BLOCK** — FINDING 2 |
| R5 | Banner swept claim by claim, live assertions only | **BLOCK** — FINDING 2 |
| R6 | `tests.md` newest entry, live claims | **BLOCK** — FINDING 1 |
| R7 | `global.md` / `tests.md` cross-file contradiction | **BLOCK** — FINDING 1 |
| R8 | Rename completeness; zero old-name residue under `Source/` | pass |
| R9 | `global.md` the only file with a live count or verdict | pass |
| R10 | Inline separate attribution | pass |
| R11 | `[STAMPED` distances measured | pass |
| I1 | `strat_banner_sweep.py` — full output, real exit code, mutants | pass — CLEAN, exit 0, non-vacuous |
| I2 | `index.json` (`utf-8-sig`) — figures, stamp, NEW present / OLD absent | pass |
| I3 | Macro set difference `HEAD` -> worktree | pass — +1 added, 0 removed |

---

## Findings

### FINDING 1 — BLOCK. The miscount was repaired in `global.md` and left standing in `tests.md`, where it is now a live falsehood AND a direct cross-file contradiction of the file that repaired it.

**Constraint violated:** a live claim about what a gate found must be true, and two record files
must not carry two answers to one question.

`Tools/architect/state/tests.md:93-94`, inside the NEWEST entry, live prose, unstamped:

```
    - **Where the rename came from.** `strat-integration-reviewer` gated the pass PASS with
      zero findings but raised one observation: the verb `Authors` sat beside
```

`Tools/architect/state/global.md:59-60`, the repaired sentence, live prose, unstamped:

```
GENERAL LESSON THIS PASS PRODUCED. `strat-integration-reviewer` gated the pass **PASS with zero
findings and FOUR observations**, all four in that report under `## Observations`. THE FIRST
```

Measured against the artifact both sentences describe (instrument: an `awk` range from the
`## Observations` heading to end of file, piped to a numbered-item `grep`, over
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md`):

```
5:1. **THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP. ...
20:2. **The `autocrlf` warning `tests.md` reports is real and is pre-existing.** ...
24:3. **The liveness control compares STRINGS, not arrays.** ...
30:4. **Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable ...
```

Four. `global.md` now says four. `tests.md` still says one. **"zero findings" is right in both;
"one observation" is wrong by three, and it is wrong in the same construction RE-GATE 3 blocked
on** — a precise count standing beside another precise count, introduced by a colon that
enumerates the set as a single member.

**This is not the same finding re-filed. It is a different file, a different owner, and a defect
that did not exist in this form until 01:51:36.** Before the repair, `global.md` and `tests.md`
agreed and were both wrong; the repair fixed one of the two and thereby created a contradiction
between them. A reader who follows the trail the banner itself lays down — `global.md:80-81` sends
readers to "`tests.md`'s own stamp", and `tests.md:93` sits nine lines below that stamp — lands on
"one observation" immediately after having read "FOUR observations".

**The mtime is what makes this decisive rather than a judgement call.** `tests.md` was last
written at 00:49:03, before the first gate report existed. It has never been revisited. The
sentence is not stale-by-drift; it has simply never been checked against the artifact it
describes.

**The reader consequence is the recorded "stale status line runs both ways" hazard.** The other
three observations — the pre-existing `autocrlf` condition on this lane's own file, the
string-versus-array liveness comparison in this lane's own clause, and the unfalsifiability of the
in-session clause's obligation (1) — are the test lane's business, and `tests.md`'s own sentence
tells a future test-author there was nothing else raised.

**No instrument can see it.** `strat_banner_sweep.py` governs suite figures, report-stamp
citations, provenance paths, item states, banner dates and record ownership. A count of
observations in a gate report is none of those, and the sweep is CLEAN over this tree (exit 0,
inlined below) with this sentence inside the region it swept.

**Owner: `strat-test-author`** — `tests.md` is its file and its newest entry. The `coordinator`
may NOT write it under the transcription clause: that clause is triggered by A MERGE, and must
name one, and there has been no merge here (base `9d705ca`, uncommitted, no wave in flight). The
fix is one number, or the phrasing this record already owns at `tests.md:3679` ("with zero
findings and five observations"), plus a decision on whether the three dropped observations are
recorded in this lane's file at all. **I do not apply it.**

---

### FINDING 2 — BLOCK. The count was removed and an EXHAUSTIVE ENUMERATION of the same growing set was left one sentence below the rule forbidding it. This report falsifies it on landing — which is the defect the paragraph exists to prevent, committed by the paragraph itself.

**Constraint violated:** the one the paragraph states in its own words at `global.md:103-104` —
"NEVER WRITE AN EXHAUSTIVE COUNT OF A GROWING SET INSIDE THE THING THAT GROWS IT. Name the pattern
the set is enumerated by and let a reader count."

The repair does that correctly for the re-gate FILES. `Tools/architect/state/global.md:100-104`:

```
NO COUNT OF THEM IS WRITTEN HERE, DELIBERATELY, AND THAT IS THE THIRD LESSON OF THIS PASS: a
sentence saying "re-gated TWICE, and BOTH BLOCKED" with a closed two-item list stood here and
was true of the tree it was written in -- and the very re-gate report that read it was, by
existing, about to falsify it. **NEVER WRITE AN EXHAUSTIVE COUNT OF A GROWING SET INSIDE THE
THING THAT GROWS IT.** Name the pattern the set is enumerated by and let a reader count.
```

And then, immediately, at `Tools/architect/state/global.md:105-108`:

```
What each re-gate FOUND, which is the durable part: the stale bare citation repaired above;
then THIS SENTENCE's predecessor, which still read *"the tree has not been re-gated since"* --
true when written and false the moment the first re-gate ran; then the miscount of the first
gate's observations, repaired above, and the count defect this paragraph now exists to prevent.
```

**"What each re-gate FOUND" is a universal quantifier over the same growing set, and the colon
closes the list.** The four items map exactly onto RE-GATE 1 FINDING 1, RE-GATE 2 FINDING 1, and
RE-GATE 3 FINDINGS 1 and 2 — I verified that mapping against the three reports and it is faithful,
which is precisely what makes it read as complete. It is not a cardinality; it is the same closure
expressed as content. A reader arriving at `:105` by citation — and RE-GATE 2 established, and
this banner accepts, that a reader arriving by citation lands on the sentence and not on the
paragraph above it — concludes that this is what the re-gates found.

**It is already false, and it is false independently of my raising it.** FINDING 1 above is a
finding by a re-gate and is not in that list. The sentence would be pushed asserting a complete
account of the re-gates' findings that omits the one still-unrepaired defect in the tree it
describes. **This is the third instance in one pass of "a correction that only adds leaves both
claims live" — the banner names two at `:109`; this is the third, and it is inside the sentence
written to close the second.**

**The counter-reading, stated because the owner is entitled to judge it.** "which is the durable
part" can be read as a SELECTION of lessons rather than an inventory of findings, and the
paragraph does route readers to the primary artifacts two lines above ("read their own `VERDICT:`
lines"). Under that reading this is imprecision, not self-contradiction. I am not taking that
reading, for the reason RE-GATE 3 gave when it changed its own severity call on a neighbouring
sentence: the construction is a colon enumerating a closed set inside a document that grows the
set, the brief for this pass ordered every live assertion about what was gated adjudicated, and
this is the fourth pass over this paragraph. **The verdict does not hinge on it: FINDING 1 blocks
on its own.**

**No instrument can see it either.** A prose claim about what gates found is not a suite figure, a
stamp citation, a provenance path, an item state, a banner date or a record-ownership claim. The
sweep is CLEAN with this sentence inside the swept region.

**Owner: the `coordinator`** — `global.md` is its own file, in lane, no exception clause needed.
The fix is to apply the paragraph's own rule to its own next sentence: name the pattern (the
findings are in the re-gate reports' own `## Findings` sections, reachable by the glob already
written two lines above) and stop enumerating them here, or keep one or two as explicitly
illustrative rather than as "what each re-gate FOUND". **I do not apply it.**

---

## The repair, audited item by item

### R1. Is the observation count now correct, and are the summaries FAITHFUL rather than merely present?

The count is correct — four, verified above against the artifact. The three added summaries at
`global.md:70-81` were checked against what I actually wrote, claim by claim, not skimmed for
presence.

**Observation (2), `autocrlf`.** Banner: "the file is LF in the worktree and LF in the `HEAD` blob
and both passes preserved that; watch it at staging time." My text: "the file is LF in the
worktree and LF in the HEAD blob, and this pass preserved that. Worth watching at staging time;
not a change this diff made." The banner widens "this pass" to "both passes" — a claim I did not
make, because the rename pass had not happened when I wrote it. **I measured the widened claim
rather than accepting or rejecting it, and it holds:**

```
$ tr -cd '\r' < Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp | wc -c   -> 0
$ tr -cd '\n' < Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp | wc -c   -> 438
$ git show HEAD:<same path> | tr -cd '\r' | wc -c                                    -> 0
$ git show HEAD:<same path> | tr -cd '\n' | wc -c                                    -> 294
```

Zero CR on both sides after both passes. (`grep` cannot count CR on this box; `tr -cd` plus
`wc -c` is the instrument that can.) Faithful, and the widening is true.

**Observation (3), the liveness control.** Banner: "compares the RENDERED STRINGS `(1)` and
`(0,1)` rather than the two `TArray<int32>`s -- correct here because digits and commas cannot be
bitten by `FString`'s case-insensitive comparison, recorded so a future edit rendering a
case-bearing field through the same helper knows the property is not free." That is my claim, my
reason and my forward-looking caveat, with nothing added and nothing dropped. Faithful.

**Observation (4), obligation (1).** Banner: "the gate verified that this banner SAYS both routes
were offered and the user chose dispatch, in the required inline shape, and records that it cannot
verify that it HAPPENED." Mine: "I verified that the entry SAYS it, in the required inline shape.
I cannot verify that it HAPPENED, and I record which of the two I did." The banner preserves the
SAYS/HAPPENED split, which is the entire content of the observation. Faithful.

**The framing sentence is also faithful.** "None of the three is a defect and none is closed by
anything in this pass" — my section was headed "Non-gating", and I closed none of them. Correct.

**One thing the banner adds that I did not write, and it is right to:** "a live miscount found on
the third re-gate, two lines above the file that disproved it." That is a true statement about
RE-GATE 3.

### R2. Is the count-free formulation genuinely count-free?

For the re-gate FILES: yes, and I swept the whole paragraph for smuggled cardinality rather than
checking the headline sentence. Sentence by sentence, `global.md:82-111`:

- `:84-86` — "The re-gates are the files matching `...-regate*.md`, each a new file leaving its
  predecessors byte-identical; read their own `VERDICT:` lines". No count. "each" here quantifies
  a PROPERTY over the set, not its size, and the property is true (measured by mtime, above).
- `:87` — "NO COUNT OF THEM IS WRITTEN HERE, DELIBERATELY". Self-describing; true of the files.
- `:87` — "THE THIRD LESSON OF THIS PASS" — an ordinal over LESSONS, not over re-gates. Not the
  prohibited count, and not a miscount either: the paragraphs above label the citation repair and
  the clause-name lesson ahead of it.
- `:88` — "re-gated TWICE, and BOTH BLOCKED" — inside quotation marks, quoting the retired
  sentence in order to name the defect. Not an assertion.
- `:105-108` — **FINDING 2.** No cardinality, but a closed enumeration of the same set's contents.
- `:109-112` — "THE SAME DEFECT TWICE IN ONE PASS", "CLEAN across both", "Both were found by a
  dispatched gate". These are counts, but of a CLOSED historical set — two instances of "a
  correction that only adds", both already in the past, neither able to grow. Not findings. The
  "sweep was CLEAN across both" claim is true: RE-GATE 2 and RE-GATE 3 each recorded a clean
  sweep, and I reproduce a clean sweep below.
- No "the second found...", no "each of the two", no "both re-gates" applied to the growing set,
  anywhere in the banner. Searched, not assumed.

### R3. Does the glob resolve to the re-gate reports and nothing else?

Run, not reasoned:

```
$ ls -1 Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate*.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md

$ grep -H '^VERDICT' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate*.md
  ...-regate-2.md:VERDICT: BLOCK
  ...-regate-3.md:VERDICT: BLOCK
  ...-regate.md:VERDICT: BLOCK
```

Three files, all three re-gates, nothing else. Critically it does NOT sweep in
`2026-09-04-shipped-difficulty-pin.md` (the first gate, which is not a re-gate) — the `-regate`
infix excludes it. And **this report, `...-regate-4.md`, matches the same glob and will be found
by the same reader**, which is the property the count-free formulation was written to have. The
instruction "read their own `VERDICT:` lines" resolves correctly: each file carries exactly one
line beginning with that token at column 0.

### R4. Is anything in the current banner falsified by THIS report landing?

The check RE-GATE 3 established. The current banner is `global.md:14-152` (next `_Last run` at
`:153`, measured). I walked every claim in it that could be sensitive to a fourth report existing:

- "each a new file leaving its predecessors byte-identical" — I create a new file and modify
  nothing. Verified by mtime that the four predecessors are unmodified since they were written.
  **Not falsified.**
- The glob at `:85` — matches my filename. **Not falsified; strengthened.**
- "NO COUNT OF THEM IS WRITTEN HERE" — still true. **Not falsified.**
- "What each re-gate FOUND: ..." at `:105-108` — **FALSIFIED ON LANDING.** FINDING 2.
- "THE FIRST GATE'S `VERDICT: PASS` DESCRIBES A TREE THIS ONE IS NO LONGER ... IT WAS THEREFORE
  RE-GATED" — still true. **Not falsified.**
- "`strat_banner_sweep.py` was CLEAN across both" — about two past instances; my clean sweep does
  not disturb it. **Not falsified.**
- "PASS with zero findings and FOUR observations" — describes the FIRST gate report, a different
  artifact. My verdict does not touch it. **Not falsified.**
- The suite figure, the report stamp, the falsifiability account and the open debts — none
  reference the gate history. **Not falsified.**

### R5. The banner swept claim by claim — live assertions only

Stamp geometry MEASURED, not reasoned. Within `:14-152` there is exactly one `[STAMPED` marker:

```
82: **[STAMPED 2026-09-04 -- the two claims below were true of this banner as first written, b
84: STAMPED> "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.04-04.09.26`."
85: STAMPED>  -- ONE REPORT PATH, ONE ARTIFACT: the rename's rebuild-and-re-run overwrote it.
86: STAMPED>  figure did NOT move (388 both sides, as a rename must); only the artifact did.
87: STAMPED> "`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`"
88: STAMPED>  -- the clause's name before the rename. Retired from `Source/`; it survives
89: STAMPED>  deliberately in `tests.md`'s own stamp and in the gate report, see below.
```

Marker opens at `:82`; its scope is the six prefixed lines `:84-89`. Line `:90` carries no prefix
and is live. **Therefore everything the repair wrote — the FOUR-observations sentence at `:59-60`,
the three observation summaries at `:70-81`, and the count-free paragraph at `:100-111` — is LIVE,
unstamped prose, correctly so.** Nothing was accidentally buried inside a stamp block, and nothing
retired was left outside one.

Adjudicated live claims, all verified true unless noted:

- `:29-30` "The suite is 388/388, every entry Success, zero failed, zero notRun, zero
  succeededWithWarnings" — **true**, instrument I2.
- `:31-34` "+1 added, 0 removed, 0 state changes ... set difference on the test MACRO with one
  collector over both sides" — **true**, instrument I3, reproduced with my own collector.
- `:35` "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.04-04.47.20`" — **true**.
- `:36-44` the provenance paragraph, and the repaired sentence at `:41-43` now carrying the
  `reportCreatedOn` token — **true**; the token is present, so the citation is inside the sweep's
  reach, which MUTANT B below proves is a real governance and not a nominal one.
- `:38-43` "CARRIED DEBT, INSTRUMENT, OPEN: the sweep cannot see a BARE report timestamp ... The
  fix ... is in `strat-data-steward`'s lane and is NOT written" — **still true**; the regex still
  requires the literal token, and no steward change is in the diff.
- `:45-69` the rename paragraph and the four-observation account — **true**, R1 above, except that
  its counterpart in `tests.md` contradicts it (FINDING 1).
- `:77-81` "the gate report ... names the OLD identifier four times ... editing it would falsify
  what was actually gated" — **true**; the old name appears at `:408`, `:422`, `:494` and `:510`
  of `2026-09-04-shipped-difficulty-pin.md`, exactly four, and that file is unmodified (mtime).
- `:82-89` the stamp block — correctly scoped, contents accurate.
- `:112-135` the pass's substantive account (what is closed for the value and open for the
  authorship, the weaker-warrant liveness control, the two mutants, the restore-and-re-run) —
  every claim cross-checks against the `tests.md` entry and against the clause body I read;
  nothing in it is contradicted by the tree.
- `:136-140` "STILL OPEN ... it is `strat-gameplay-engineer`'s. It is not written." — **true**; no
  such library code exists in the diff or in `Source/`.
- Lane attribution at `:16-20`: "Written by the `coordinator`, whose file this is, IN LANE AND
  UNDER NO EXCEPTION CLAUSE ... that lane was DISPATCHED and wrote its own entry, in lane, itself.
  This entry is not a transcription of anything." — **consistent with the tree**: the `.cpp` and
  `tests.md` were written at 00:44 and 00:49, `global.md` at 01:51, and the `tests.md` entry
  self-attributes to `strat-test-author` as ACTING and WRITING, IN LANE. No transcription clause
  is invoked and none is needed; correctly, no merge is named, because there is none.
- `:20-28` the route paragraph (both routes offered, costs named, user chose dispatch) — **SAYS
  it, in the required inline shape. Whether it HAPPENED is unfalsifiable from a checkout**, as
  `CLAUDE.md` declares and as the banner now itself records at `:78-81`. I record which of the two
  I did, again.

### R6 / R7. The `tests.md` newest entry, and cross-file contradiction

The newest entry is `tests.md:17-116`. Live claims checked:

- Attribution: "`strat-test-author` (ACTING and WRITING; IN LANE -- one file under
  `Source/*/Tests/`, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`,
  base commit `9d705ca`, UNCOMMITTED)" — **inline, both roles named, base commit named. Correct**,
  and it matches the tree.
- "Cite this pass by its exported `reportCreatedOn 2026.09.04-04.47.20`" — **true**.
- "never by a number -- the figure lives in `Tools/architect/state/global.md` and nowhere else" —
  **true**; a grep for the figure over `tests.md`, `engine.md`, `content.md`, `data.md` and
  `decisions.md` returns nothing, and the sweep record-ownership check is clean.
- The `[STAMPED 2026-09-04]` block retiring `2026.09.04-04.09.26` — correct and correctly scoped.
- "`FStratMatchConfig::Difficulty` initialises to `EStratDifficulty::Easy` ... `Easy` is the FIRST
  value ... and is therefore 0" — consistent with the clause body and with the header prose.
- "The macro-based multiline set-difference against `HEAD` (`9d705ca`, which predates the whole
  clause) showed exactly ONE added and ZERO removed" — **independently reproduced**, I3.
- The collector-asymmetry account (82 test `.cpp` files on the `HEAD` side to the worktree side 81,
  caused by a committed fossil copy of `StratScoreboardParity.cpp` under
  `Tools/architect/evidence/02-scoreboard-widget/generated/Source/StratUI/Tests/`) — **premise
  verified**; that path is present in `git ls-tree -r --name-only HEAD`. The remedy it prescribes,
  scoping BOTH sides to `Source/` explicitly, is what I did, giving 81/81 and a proven-symmetric
  file set.
- "Reported to the coordinator, NOT fixed here (out of lane)" on the `autocrlf` warning —
  **correct**, and the LF measurement above confirms the substance.
- **`tests.md:93-94` "raised one observation" — FALSE, and it contradicts `global.md:59-60`.
  FINDING 1.**

I found no OTHER contradiction between the two files. The suite figure appears in exactly one of
them; the report stamp agrees in both; the live clause name agrees in both; the old name is
stamped in both and live under `Source/` in neither; the falsifiability accounts agree; the open
authorship debt is attributed to `strat-gameplay-engineer` in both.

### R8. Rename completeness and old-name residue

A census of the retired identifier, scoped and inlined:

```
$ grep -rn AuthorsEasyDifficulty Source/
(nothing)

$ grep -rn AuthorsEasyDifficulty Source/ Tools/ Config/ Content/ .agents/ CLAUDE.md
gate_reports/2026-09-04-shipped-difficulty-pin.md:408,422,494,510   (first gate, historical)
gate_reports/...-regate.md:100,609,612,635                          (my own instruments)
gate_reports/...-regate-2.md:378,379,632,635,636,657                (my own instruments)
gate_reports/...-regate-3.md:86,327                                 (my own instruments)
state/global.md:62    (live prose, truncated form, narrating the rename)
state/global.md:87    (inside the STAMPED block)
state/tests.md:86     (inside the STAMPED block)
```

**Zero under `Source/`**, which is the requirement. The gate-report occurrences are deliberate and
must stay: they are the record of gates on trees that no longer exist. The `global.md:62`
occurrence is a truncated narration of what the name WAS, in a sentence that ends by giving the
live name; it is not a citation of a live identifier.

The four rename sites in the `.cpp` are consistent — macro class identifier, clause string,
`RunTest` definition and the in-file header reference all read `RunsAtEasy` — and the clause body
is the `TestEqual(ShippedTier, EStratDifficulty::Easy)` plus the `TestNotEqual` liveness control
on `AiSides` that both record files describe. The `.cpp` has not been touched since 00:44:16, i.e.
since before RE-GATE 1 audited the rename; its hash is recorded in Scope so a future pass can
prove body identity without re-reading it.

### R9 / R10 / R11

- **R9, live count and verdict ownership.** `global.md` carries the only live suite figure; the
  sweep record-ownership and live-count checks are clean, and no other state file carries a suite
  count. `tests.md` does carry gate-verdict prose ("gated the pass PASS with zero findings ..."),
  which this record convention permits — `tests.md:3679` predates it and passed earlier gates —
  since the ban is on suite counts and phase verdicts, not on a lane narrating why a rename
  happened. Its falsity, not its presence, is FINDING 1.
- **R10, inline separate attribution.** Both entries name acting and writing INLINE in the entry
  rather than in a header. `global.md` names no exception clause and needs none; `tests.md` names
  ACTING and WRITING as the same agent, in lane, which is the ordinary case and needs none either.
- **R11, `[STAMPED` distances.** Measured with an `awk` pass over line numbers, inlined under R5:
  marker at `:82`, scope `:84-89`, first live line after it `:90`. No stamped block over-reaches,
  and no retired claim sits outside one — with the exception that the FINDING 2 sentence is live
  and should not survive the fix in its present form.

---

## Instruments — inlined in full

### I1. `strat_banner_sweep.py`, real exit code, not through a pipe

Redirected to a file so the exit status read is python's own and not a pipeline's:

```
$ python Tools/architect/strat_banner_sweep.py > /tmp/sweep.txt 2>&1; echo "REAL_EXIT=$?"; cat /tmp/sweep.txt
REAL_EXIT=0
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

All six record files swept — none missing, which is itself one of the checks.

**Non-vacuity, with a control.** A clean sweep is indistinguishable from an inert one, so I ran
mutants. The sweep exposes `run_sweep(paths=...)`, so the mutation is on COPIES in a scratch
directory; `Source/`, `Saved/` and `Tools/architect/state/` are untouched, and this seat writes
nothing in the repo but this report. The probe asserts the mutation actually applied before
running, so a no-op edit cannot masquerade as a firing check:

```python
# scratchpad/mutant.py  (executed; output below)
import io, os, shutil, sys, tempfile
sys.path.insert(0, r"E:/MultiAgent/Stratocracy/Tools/architect")
import strat_banner_sweep as S

SRC = S.STATE_DIR
def copy_to(d):
    out = []
    for f in sorted(os.listdir(SRC)):
        if f.endswith(".md"):
            shutil.copy2(os.path.join(SRC, f), os.path.join(d, f)); out.append(os.path.join(d, f))
    return out

def run(label, mutate):
    d = tempfile.mkdtemp(); paths = copy_to(d)
    g = os.path.join(d, "global.md")
    with io.open(g, encoding="utf-8") as fh: t = fh.read()
    t2 = mutate(t)
    assert t2 != t, "MUTATION DID NOT APPLY -- probe is inert"
    with io.open(g, "w", encoding="utf-8", newline="\n") as fh: fh.write(t2)
    r = S.run_sweep(paths)
    print(label, "passed=", r.passed, "findings=", len(r.findings))

# CONTROL FIRST: verbatim copies must stay clean, or the copying is the cause.
d = tempfile.mkdtemp(); r = S.run_sweep(copy_to(d))
print("CONTROL (verbatim copies) passed=", r.passed, "findings=", len(r.findings))

run("MUTANT A: live suite figure 388 -> 389",
    lambda t: t.replace("The suite is **388/388**", "The suite is **389/389**", 1))
run("MUTANT B: live reportCreatedOn 04.47.20 -> 04.09.26",
    lambda t: t.replace("REPORT IS `reportCreatedOn 2026.09.04-04.47.20`",
                        "REPORT IS `reportCreatedOn 2026.09.04-04.09.26`", 1))
```

```
--- CONTROL (verbatim copies): passed=True  findings=0
--- MUTANT A: live suite figure 388 -> 389: passed=False  findings=2
    live suite claim(s) disagree with the tree total (388): global.md:28: 389/389
    live suite claim(s) disagree with the report own succeeded count (388): global.md:28: 389/389
--- MUTANT B: live reportCreatedOn 04.47.20 -> 04.09.26: passed=False  findings=1
    global.md:35: cites reportCreatedOn 2026.09.04-04.09.26, but the report this sweep
    actually opened is reportCreatedOn 2026.09.04-04.47.20 -- an unstamped, unquoted
    citation of a DIFFERENT run [...]
```

The control passes the healthy path; both mutants fire. The sweep is live on the two dimensions
that matter to this banner. **It remains blind to everything in FINDING 1 and FINDING 2**, which
is the carried instrument debt the banner itself records.

### I2. `Saved/AutomationReport/index.json`, opened `utf-8-sig`

```
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388
succeededWithWarnings: 0
failed: 0
notRun: 0
entry count: 388
states: Counter({'Success': 388})
NEW present: True | state: ['Success']
OLD present: False
any name containing AuthorsEasyDifficulty: []
```

**`reportCreatedOn` is still `2026.09.04-04.47.20`** — unchanged, as the brief requires; no suite
has run since RE-GATE 3, consistent with the file mtime `2026-09-04 00:47:20` and with the fact
that no `Source/` file has been touched since 00:44:16. The NEW name is present BY NAME and
`Success`; the OLD name is absent BY NAME. Both were asserted directly against the report rather
than inferred from the delta, and the file was opened `utf-8-sig` because a plain `utf-8` open
fails on its BOM.

### I3. Macro-based multiline set difference, `HEAD` to worktree, one collector

One regex, one collection function, used on both sides. **Symmetric file-set rule, stated:** both
sides enumerate only paths that begin `Source/`, contain a `/Tests/` directory, and end `.cpp` —
which is precisely the scoping `tests.md` reports having got wrong once, when a `git ls-tree` path
filter reached a fossil copy under `Tools/architect/evidence/` that an `os.walk` of `Source/`
cannot. The file sets are compared and proved equal BEFORE any clause is counted, so a collector
asymmetry cannot hide inside the result.

```python
MACRO = re.compile(
    r'IMPLEMENT_(?:SIMPLE|COMPLEX)_AUTOMATION_TEST(?:_PRIVATE)?\s*\(\s*[^,]+,\s*"([^"]+)"', re.S)

def is_subject(p):
    p = p.replace(chr(92), "/")
    return p.startswith("Source/") and "/Tests/" in p and p.endswith(".cpp")

# HEAD side: git ls-tree -r --name-only HEAD, filtered by is_subject, read via git show HEAD:<p>
# worktree side: os.walk of Source/, filtered by the SAME is_subject, read from disk
```

```
HEAD subject files: 81   worktree subject files: 81
file-set symmetric: True
only in HEAD: []
only in worktree: []
HEAD clause count: 387   worktree clause count: 388
ADDED (1):
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
     ['Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp']
REMOVED (0):
duplicate names in worktree: {}
```

**+1 added, 0 removed**, as the record claims, and the added name is the live one. The multiline
flag matters: the macro name argument sits on its own line, and a single-line grep returns zero
here. 387 to 388 also reconciles with the report 388 and with the sweep macro census of 388 —
three independent counts agreeing.

### Standing checks — re-verified, not inherited

Check 1 and check 7, the two that must print nothing:

```
$ git diff --stat -- Source/StratRules Data
(nothing)

$ git diff --stat -- Source/Stratocracy
(nothing)
```

Vendored bytes untouched; the game module untouched. Nothing to escalate upstream to
`E:/MultiAgent/stratocracy-crew`, and no local edit to certified bytes.

Check 2, no cross-module `strat::` CALL. A grep for the call shape (namespace, lowercase initial,
optional space, open paren) across `Source/`, with the two permitted directories filtered out,
returns nothing. Note, because this grep produces legal hits every pass and forwarding them all
trains the reader to skim: the lowercase-initial pattern is what separates a function from a type.
`strat::UiSnapshot`, `strat::SaveCommand` and `strat::GameState` appear widely outside the two
permitted directories and are fine. There are ZERO hits of the CALL shape outside them.

Check 3, no vendored header in front of UHT. Every header containing the tokens `UCLASS`,
`USTRUCT` or `UENUM` was scanned for a direct vendored include. One file matched —
`Source/StratBridge/StratBridge.h`, which includes `Data.h`, `Replay.h`, `Save.h`, `Scenario.h`
and `Ui.h`. **It is a false positive, judged rather than forwarded.** An anchored grep for a real
macro declaration in that header returns nothing, a count of `GENERATED_BODY` in it returns 0, and
the unanchored hits are all comment prose:

```
30:  // back a `strat::UiSnapshot` by reference parameter rather than a USTRUCT: a
262: // struct reason: this header must declare no UCLASS/USTRUCT/UENUM, because
323: // this header must declare no UCLASS/USTRUCT/UENUM, or UHT parses it and the
```

The header explains, in prose, why it declares none. The constraint holds.

Check 4, the pimpl rule. A grep over headers for both spellings:

```
StratMatchSubsystem.h:2496:  TPimplPtr<FStratBridge> Bridge;
StratScoreboardHUD.h:1235:   TPimplPtr<FStratBridge> OwnedBridge;
(plus three comment references, one of which cites the measured C4150 on the wrong spelling)
```

Both real members are `TPimplPtr`. No `TUniquePtr<FStratBridge>` declaration exists.

Check 5, module arrows DERIVED from the three graph-bearing fields, comments stripped:

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
StratPlay    PublicIncludePaths             StratPlay
StratRules   PublicDependencyModuleNames    Core
StratRules   PrivateDependencyModuleNames   (none)
StratRules   PublicIncludePaths             StratRules
StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
StratUI      PublicIncludePaths             StratUI
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                            AIModule,NavigationSystem,StateTreeModule,
                                            GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
-- 16 rows, five modules, none missing.
```

Sixteen rows including the conditional one, matching the checklist row for row, and every module
represented — a module missing from that output would itself be a finding, because an empty
extraction is the one thing this shape cannot distinguish from a clean one.

**The sixteenth row is CONDITIONAL and a textual extraction cannot show that, so I read the
branch** — `Source/StratBridge/StratBridge.Build.cs:133-137`:

```
133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
135:  if (!bCompileVendoredRulesHere)
137:      PrivateDependencyModuleNames.Add("StratRules");
```

The `StratRules` link edge sits inside the guard — monolithic targets only, one expression driving
both halves, so the dependency half and the compile half cannot drift apart. That is the
deliberate design and NOT a finding: the name appears in `PrivateDependencyModuleNames` only
under the guard, never unconditionally, and never in `PublicDependencyModuleNames`. `StratUI` has
gained no `Stratocracy` dependency; no cycle exists. `StratBridge -> Stratocracy` is the
documented, correct edge and is not reported as a layering violation.

Field census, the guard against a graph-bearing field this table does not model:

```
BRIDGE=                                ShadowVariableWarningLevel =
PCHUsage =                             bCompileVendoredRulesHere =
PrivateDefinitions.Add                 bUseUnity =
PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
-- 10 names.
```

Exactly ten, matching the recorded 2026-09-01 measurement. No eleventh name; no
`DynamicallyLoadedModuleNames`, `PrivateIncludePaths` or `PublicSystemLibraries` has appeared.
`BRIDGE=` is the documented false positive — the tail of the string literal inside
`PrivateDefinitions.Add` at line 134 above, which the comment strip cannot reach inside a quoted
string. Not chased, and not filtered away either: narrowing the census to spare one known false
positive is how it would stop seeing the next real field.

Check 6, registration. The `Modules` array of `Stratocracy.uproject`, read with a JSON parser
rather than a grep, and the module objects that exist in the tree:

```
uproject Modules: ['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']

IMPLEMENT_MODULE / IMPLEMENT_PRIMARY_GAME_MODULE sites:
StratBridge/StratBridge.cpp:  IMPLEMENT_MODULE(FDefaultModuleImpl, StratBridge)
Stratocracy/Stratocracy.cpp:  IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Stratocracy, "Stratocracy")
StratPlay/StratPlay.cpp:      IMPLEMENT_MODULE(FDefaultModuleImpl, StratPlay)
StratUI/StratUI.cpp:          IMPLEMENT_MODULE(FDefaultModuleImpl, StratUI)
```

Four modules carry a module object; all four are registered. `Source/StratRules/` carries none and
is correctly ABSENT from the array — flagging it as unregistered is the false positive that costs
a startup abort to discover, and I checked for the macro before reporting anything. No new module
directory appears in the diff.

Check 8, `/Game/` literals. A grep across `Source/` headers and sources with `Tests/` paths
excluded returns nothing. Every `/Game/` literal in `Source/` sits inside a `Tests/` directory —
the standing automation-fixture exception — and there are zero in gameplay C++. The new clause
reaches its assets through the `kShippedGameModeClassPath` and `kAiVsAiGameModeClassPath`
constants already present in the same test file.

Checks 9 to 12. **9:** no `Source/StratUI/` file appears in the diff, so no widget-side arithmetic
can have been introduced. **10:** the `.cpp` is under `Source/StratPlay/Tests/` and belongs to the
test-author; `tests.md` is the test-author's; `global.md` is the coordinator's; the gate reports
are mine. No production code inside `Tests/`, no test code outside it, no `Content/` change in a
source-only pass. **11:** the new clause carries a why-first header citing GDD Sec 2.11.6 and Sec
2.9, naming `T-FAME-02`, recording its two mutants as measurements and stating in terms what it
does NOT pin; the `DescribeDifficulty` helper carries its own reasoned block explaining why the
tier name is asked of the `UENUM` rather than switched over locally; and the pre-existing header
bullet that this clause falsified was narrowed in the same pass rather than left standing.
**12:** nothing staged, nothing committed — the `git status --porcelain` in Scope shows three
unstaged modifications and four untracked reports, five once this one lands.

---

## Observations

Non-gating.

1. **`global.md:80-81` says "`tests.md` carries the one surviving stamp of the old name under
   `Tools/architect/`", and `global.md:87` is itself a `STAMPED>` line carrying that name in
   full.** Under a literal reading the claim is short by one, in the file making it. I am not
   raising it as a finding: the `:87` occurrence is a stamp of THIS BANNER's own prior claim
   rather than a standalone retirement of the identifier, the sentence's function — telling a
   reader where the trail is — is discharged correctly, and `:88-89` points explicitly at both
   `tests.md` and the gate report. Recorded so that whoever edits this paragraph for FINDING 2 can
   tighten it in the same pass rather than in a fifth one.

2. **The `autocrlf` condition is broader than the observation that named it, and the `.cpp` is the
   least-affected file in the diff.** Measured: the `.cpp` is LF in the worktree and LF in the
   `HEAD` blob, zero CR on both sides. `global.md` and `tests.md` are CRLF in the worktree (7548
   and 5920 CR respectively) and LF in their `HEAD` blobs. Git is normalising on the way in — both
   diffs show only added hunks, not whole-file rewrites — so a commit will store LF and this is
   not a finding. But the record's watch-it-at-staging note applies to the two record files rather
   more than to the `.cpp` it was written about.

3. **The carried instrument debt is unchanged and is now demonstrably load-bearing several times
   over.** `strat_banner_sweep.py` was CLEAN over a tree containing FINDING 1 and FINDING 2, as it
   was clean over the trees containing the four earlier findings. Six findings across four gates,
   none seen by any instrument. The bare-timestamp fix in `strat-data-steward`'s lane would close
   one narrow dimension; nothing in the tooling addresses prose claims about gate history or
   observation counts, and it is not obvious that anything could. That is an argument for the
   dispatched gate, not for more tooling, and it is recorded rather than proposed.

4. **The two blocking findings are one-sentence prose edits in two different lanes, and they must
   not be collapsed into one write.** FINDING 1 belongs to `strat-test-author` and FINDING 2 to
   the `coordinator`. There is no merge in this tree, so the transcription clause does not license
   the `coordinator` to write `tests.md`; dispatch is the route. If both are fixed, the tree needs
   a fifth gate, because a verdict describes a tree and not a phase.

5. **Nothing in this report contradicts the brief, and the one place where I had to prefer the
   tree is worth naming.** The brief states that `global.md` is the only change since the last
   gate. That is true of the WORKING TREE relative to RE-GATE 3, and I verified it by mtime — but
   `git status` also shows the `.cpp` and `tests.md` modified relative to `HEAD` `9d705ca`, which
   is a different question and is the diff this gate audits. Both statements are true and neither
   is the other; I read the brief as the former.

VERDICT: BLOCK
