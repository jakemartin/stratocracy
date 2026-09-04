# Gate: `Difficulty` shipped-asset pin — RE-GATE 8 — strat-integration-reviewer

## Scope

The **UNCOMMITTED working tree** at `E:/MultiAgent/Stratocracy`, branch `master`, over base
commit `9d705ca`. Not a worktree — verified by absolute path:

```
$ git -C E:/MultiAgent/Stratocracy rev-parse HEAD
9d705ca6d7b639312ce761379786237e2d381cd3
$ git -C E:/MultiAgent/Stratocracy rev-parse --abbrev-ref HEAD
master
$ git -C E:/MultiAgent/Stratocracy worktree list
E:/MultiAgent/Stratocracy                                              9d705ca [master]
E:/MultiAgent/Strat-wt/slot-1                                          4f5e09e [feat/w8-path-preview]
E:/MultiAgent/Strat-wt/slot-2                                          0a697c0 [feat/w8-transient-receipts]
E:/MultiAgent/Stratocracy/.claude/worktrees/agitated-mirzakhani-22890b 63433e7 [...]
E:/MultiAgent/Stratocracy/.claude/worktrees/cranky-nobel-af4493        95e169b [...]
E:/MultiAgent/Stratocracy/.claude/worktrees/quizzical-yonath-9b009a    283d711 [...]
E:/MultiAgent/Stratocracy/.claude/worktrees/sweet-saha-aeb2d6          9402452 [...]
E:/MultiAgent/Stratocracy/.claude/worktrees/vigorous-thompson-e11d00   0b17685 [...]
```

Three modified files, eight untracked gate reports (this one is the eighth), nothing staged:

```
$ git status --porcelain
 M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-5.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-6.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-7.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
$ git diff --cached --stat
(empty)
$ git diff --numstat
146  2  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
163  7  Tools/architect/state/global.md
184  1  Tools/architect/state/tests.md
```

**The brief's account of what moved since RE-GATE 7 is CONFIRMED against the tree by mtime**,
which matters because the brief says only `global.md` changed:

```
2026-09-04 00:44:16  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
2026-09-04 00:47:20  Saved/AutomationReport/index.json
2026-09-04 02:31:26  Tools/architect/state/tests.md
2026-09-04 03:04:09  Tools/architect/gate_reports/...-regate-7.md
2026-09-04 03:05:01  Tools/architect/state/global.md   <-- the ONLY file after RE-GATE 7
```

No rebuild, no suite run, no `tests.md` edit: `index.json` is untouched at 00:47:20 and its
`reportCreatedOn` is still `2026.09.04-04.47.20`, as the brief states.

This report is a NEW file. It edits nothing.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct (16 rows, 10 fields) | pass |
| 6 | New modules registered; `StratRules` correctly absent | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal outside the two standing exceptions | pass |
| 9 | No widget-side arithmetic | pass — no `StratUI` file in the diff |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| R1 | RECORD — the narrowed claim (`:106-108`) is TRUE | pass |
| R2 | RECORD — the concession (`:109-112`) is accurate AND not a closed list | pass |
| R3 | RECORD — the repaired sentence is grammatical; no splice residue | pass |
| R4 | RECORD — this report's arrival falsifies nothing in either file | pass |
| R5 | RECORD — the CLOSED-artifact claim | pass (by mtime; limit stated) |
| R6 | RECORD — cross-file sweep; `global.md` sole holder of live count/verdict | pass |
| I1 | `strat_banner_sweep.py` — healthy path, real exit code, TWO mutants | pass, non-vacuity proven |
| I2 | `index.json` read `utf-8-sig`; stamp unmoved; both names asserted | pass |
| I3 | Macro set difference `HEAD` -> worktree | pass — +1 added, 0 removed |

## Findings

**None.**

The claim the previous seven gates kept falsifying is now narrow enough to be true, and I
checked it directly rather than accepting the narrowing on its face. The adjudication is
below, item by item, with the search that produced each answer.

## Adjudication — 1. THE REPAIR

### R1 — the narrowed claim at `global.md:106-108`. **TRUE**, on three separate searches.

```
102  WHAT THIS BANNER DOES AND DOES NOT RESTATE, SAID EXACTLY, BECAUSE A LOOSER VERSION OF THIS
103  SENTENCE WAS ITSELF A FINDING. It restates the FIRST gate -- its `VERDICT: PASS`, its zero
104  findings, and its four observations in substance -- because that gate is CLOSED: it is a
105  finished artifact that nothing further will add to, its first observation is why the rename
106  happened, and the other three are live caveats a reader of this pass needs. **It NEITHER
107  ENUMERATES NOR COUNTS the re-gates** -- no number of them, no list of them, no closed
108  statement of their verdicts -- because that set was still growing while this was written.
```

The claim has three conjuncts and each was searched for on its own.

**(a) "no number of them."** The brief's rule was applied as written — a count in words counts
as a number — so I swept the banner (lines 14-154) for every cardinality word, not just digits.
The sweep matched: both, each, every, all, twice, three, four, five, six, seven, eight, first,
second, third, fourth, fifth, sixth, seventh, one, two, several, multiple. It returned **50
hits**, and I read all fifty and checked the SUBJECT of each.

Every one takes a subject other than the re-gates: worktrees (`:23` "seven worktrees"), routes
(`:24` "Both routes"), suite entries (`:28` "every entry"), the FIRST gate's observations (`:59-60`
"FOUR observations", `:70` "THE OTHER THREE"), disclaimers (`:66-67` "five correct disclaimers"),
stamped claims (`:82` "the two claims below"), mutants (`:142` "TWO MUTANTS"), packages, sides,
halves. **Not one gives a cardinality of the re-gates.**

A second, narrower sweep for the specific shapes a re-gate count would take — the alternation
`twice`, `both blocked`, `re-gated twice/three/two`, `<number> re-gates`, `the re-gates are/were`
— returned **exit=1, nothing**. This one matters because RE-GATE 4 recorded that an earlier draft
carried *"re-gated TWICE, and BOTH BLOCKED"* inside quotation marks. That text is GONE from the
file, not merely quoted: the word `twice` does not appear in the banner at all.

**The one numeric contact with the re-gates, judged rather than forwarded.** `:71` reads *"a
live miscount found on the third re-gate"*. That is an ORDINAL identifying ONE member, not a
cardinality of the set: it says nothing about how many re-gates exist, and it stays true however
many more are written. The banner draws exactly this distinction two lines later — *"Referring
to one is safe; counting them is not"* (`:111`) — and applies it consistently. It is not a
violation of "no number of them", and I am not reporting it as one. **RE-GATE 7 independently
confirmed the ordinal is ACCURATE** (`-regate-3.md` is the third, and its FINDING 1 is the
miscount); I re-confirmed the naming order this pass.

**(b) "no list of them."** No enumeration of re-gates appears anywhere in the banner. Where the
set is referred to at all, it is by PATTERN and not by roster:

```
99   therefore re-gated. **The gate reports are the files matching
100  `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md`. Each is a new file leaving
101  its predecessors byte-identical and carries its own `VERDICT:` line and its own findings.**
```

`Each` there quantifies a PROPERTY over the set, not its size, and the property is true (see R5).

**(c) "no closed statement of their verdicts."** The only verdict-class statement about a re-gate
in the banner is `:44`, *"THAT SENTENCE WAS A BLOCKING FINDING"* — a statement about ONE finding,
not a terminated statement over the set. A grep for a live `VERDICT:` token in the banner returns
`:97` and `:103`, both of which name **the FIRST gate's** `VERDICT: PASS`, and `:97` says in terms
that it *"DESCRIBES A TREE THIS ONE IS NO LONGER"*. No re-gate's verdict token is stated, and no
statement covers the set.

**Why this is a PASS and not a rephrasing I happen to prefer.** The old sentence said *"not their
findings"* and three passages of its own banner falsified it — that was FINDING 1 of RE-GATE 7 and
it was correct. The new sentence does not say that. It makes three narrower claims, all three of
which survive a direct search of the same 141 lines. The narrowing is exactly the one RE-GATE 7
named as available at no cost, and it was taken without weakening anything a reader needs.

### R2 — the concession at `global.md:109-112`. **ACCURATE, and NOT a closed list.**

```
109  IT DOES REFER TO INDIVIDUAL RE-GATE FINDINGS ABOVE, and the sentence here formerly denied that
110  and was a finding for it: the paragraphs repairing a stale citation and a miscount name where
111  each came from. Referring to one is safe; counting them is not. That is the whole distinction,
112  and it is what "does not narrate the process" means in practice.
```

**Are the two named ones the right ones?** Yes, and each does what the concession says — names
where it came from:

```
44  THAT SENTENCE WAS A BLOCKING FINDING AND IS FIXED HERE RATHER THAN QUIETLY. It cited
47  ... Found by `strat-integration-reviewer` on the 2026-09-04 RE-GATE, not by the sweep ...
                                              ^ the stale-citation repair NAMES its source

70  THE OTHER THREE OBSERVATIONS ARE RECORDED HERE BECAUSE THE SENTENCE ABOVE ONCE SAID "ONE" AND
71  THEREFORE DROPPED THEM -- a live miscount found on the third re-gate, ...
                                                    ^ the miscount repair NAMES its source
```

**Are they the ONLY ones? No — and this is the question the brief was right to press, because
naming two while a third existed would be the enumeration defect reappearing inside the
concession itself.** I enumerated every passage above `:109` that refers to an individual
re-gate finding. There are FOUR, in two subject-groups plus a self-reference:

| Passage | What it refers to | Covered by the concession? |
|---|---|---|
| `:44-49` | the stale-citation finding, and who found it | YES — "a stale citation" |
| `:50-57` | the same finding's instrument: *"The reviewer proved it with an in-memory mutant"* | YES — same subject; "the paragraphs" is PLURAL |
| `:70-72` | the miscount finding, and which re-gate found it | YES — "a miscount" |
| `:102-103` | *"A LOOSER VERSION OF THIS SENTENCE WAS ITSELF A FINDING"* | YES — disclosed by the concession's own clause *"the sentence here formerly denied that and was a finding for it"* |

The fourth is the interesting one, and it is not an omission: the concession's own middle clause
states it explicitly about itself. So every reference in the banner is either named or
self-disclosed in the same sentence.

**Is the wording open enough to survive another?** Yes. The sentence contains **no closure
token** — no "only", no "both", no "the two", no count. Read literally it asserts that the banner
DOES refer to individual re-gate findings, and that two named paragraphs name their sources.
Neither assertion excludes a further one. A future paragraph referring to another re-gate finding
would leave both sentences true. That is precisely the property the old universal lacked, and it
is why this is a pass rather than the same defect one layer in.

### R3 — grammar and readability of the repaired sentence. **Clean. No splice residue.**

```
$ grep -n "restates neither" Tools/architect/state/global.md
exit=1     <-- the broken splice "It restates neither ENUMERATES NOR COUNTS" is gone
```

Read as a whole, `:102-108` is one well-formed sentence pair: *"It restates the FIRST gate — …
— because that gate is CLOSED: … . **It NEITHER ENUMERATES NOR COUNTS the re-gates** — no number
of them, no list of them, no closed statement of their verdicts — because that set was still
growing while this was written."* Subject, correlative conjunction, verb pair, object; the
em-dash gloss is appositive to the whole predicate and the `because` clause attaches to the main
verb. The parallel with the preceding sentence (*It restates … because …* against *It NEITHER …
NOR … because …*) is now intact, which it was not before the repair.

One readability note, recorded under Observations rather than here: `:110`'s *"the paragraphs
repairing a stale citation and a miscount name where each came from"* garden-paths briefly on
*"a miscount name"* before the reader recovers the real subject/verb split. It is grammatical
and it is true; it is not a finding.

### R4 — this report's arrival falsifies nothing in either record file. Verified, claim by claim.

- `global.md:99-101` — *the gate reports are the files matching
  `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md`; each is a new file leaving
  its predecessors byte-identical and carries its own `VERDICT:` line and its own findings.*
  This file is `2026-09-04-shipped-difficulty-pin-regate-8.md`: it MATCHES that glob, it is NEW,
  it edits no predecessor, and it carries a `VERDICT:` line and a `## Findings` section. Preserved.
- `global.md:90-91` — *Gate reports name the OLD identifier.* This report names
  `ShippedGameModeAuthorsEasyDifficulty` repeatedly. Preserved. (It remains a universal over a
  growing set; see Observation 1, carried forward from RE-GATE 7 and still non-gating.)
- `global.md:106-108` — my arrival GROWS the re-gate set, which is what the sentence's own
  `because` clause anticipates. Nothing in it is about this report.
- `global.md:109-112` — a claim about the banner's own paragraphs. Untouched by an external file.
- `tests.md:154-158` — *Later re-gates of this same tree are on disk and are NOT enumerated here …
  count them yourself.* It enumerates none, so an eighth cannot falsify it. My file also matches
  that entry's narrower glob ending `-regate*.md`. Preserved.

### R5 — the CLOSED-artifact claim (`:104-105`). Holds on the available evidence; the limit is stated.

The claim is that the first gate report is a finished artifact nothing further will add to.
Measured by mtime across all eight reports:

```
00:29:09  ...-shipped-difficulty-pin.md
01:14:22  ...-regate.md
01:31:02  ...-regate-2.md
01:50:28  ...-regate-3.md
02:09:16  ...-regate-4.md
02:27:37  ...-regate-5.md
02:49:32  ...-regate-6.md
03:04:09  ...-regate-7.md
```

Strictly increasing in report order; no report has an mtime later than any successor's, so no
later pass modified an earlier report, and the first has not been touched since 00:29:09 — before
every re-gate. **The honest limit, restated because it has not changed: these files are untracked,
so there is no committed blob to diff against and mtime is the only instrument a checkout has
here.** An edit that preserved mtime would be invisible to it. I report what was measured and
what it cannot see.

## Adjudication — 2. THE CROSS-FILE SWEEP

Both files' newest entries adjudicated against each other and against the tree. `tests.md` has not
moved since RE-GATE 7 (mtime 02:31:26) but is RE-DERIVED here, not inherited.

- **What the clause pins and does not pin.** `global.md:120-134` and `tests.md:35-49` both say it
  pins the VALUE and not the AUTHORSHIP, resting on two facts. Both re-verified in the tree:
  `Source/StratPlay/StratMatchSubsystem.h` initialises the field to `EStratDifficulty::Easy`, and
  `enum class EStratDifficulty : uint8 { Easy, Normal, Hard }` carries no explicit initialisers, so
  `Easy` is 0. The `.cpp` agrees at `:396` (`Shipped->MatchConfig.Difficulty`) and its assertion
  message ends *"Do not cite this clause as evidence that a designer set the tier"* at `:408`. All
  three artifacts agree. TRUE.
- **The deliberate absence of a premise block.** Stated in both files with the same reason; the
  `.cpp` carries none. Consistent. TRUE.
- **The rename and why.** `global.md:58-69` and `tests.md:84-128` both attribute it to the first
  gate's observation 1 and to the user's ruling, and both say the lane did not choose the name.
  The `.cpp:368` header reads *"WHY THE NAME SAYS `RunsAt` AND NOT `Authors`. Renamed 2026-09-04 on
  the user's ruling…"*. TRUE.
- **Lane attribution, inline and separate.** `global.md:16-19` — *"Written by the `coordinator`,
  whose file this is, IN LANE AND UNDER NO EXCEPTION CLAUSE… This entry is not a transcription of
  anything."* `tests.md:17-19` — *"`strat-test-author` (ACTING and WRITING; IN LANE… base commit
  `9d705ca`, UNCOMMITTED)"*. Both inline, both name acting and writing, neither claims a
  transcription, and the base commit matches `HEAD`. TRUE.
- **The report and its stamp.** Both cite `reportCreatedOn 2026.09.04-04.47.20`, both carry the
  literal `reportCreatedOn` token the open sweep debt requires, and both stamp the retired
  `2026.09.04-04.09.26` rather than leaving it live (`global.md:84`, `tests.md:24-29`). Re-measured
  against the artifact: the live stamp is unchanged. TRUE.
- **Open debts.** `global.md:50-57` records the sweep's bare-stamp blindness as OPEN in
  `strat-data-steward`'s lane; `global.md:150-154` records the authorship pin as OPEN in
  `strat-gameplay-engineer`'s. Neither is written in this tree — confirmed: the sweep's
  `_CITED_REPORT_STAMP_RE` still requires the token, and no override-comparing reader exists under
  `Source/`. `tests.md:50-59` restates the authorship limit without claiming closure. TRUE.
- **The `Source/`-absence property.** Stated in `global.md:94-96` and `tests.md:92-95`, phrased
  differently, and RE-EXECUTED here with a positive control, because an absence read off a silent
  instrument is not a measurement:

```
$ grep -rl "ShippedGameModeAuthorsEasyDifficulty" Source/
  exit=1                                                  <-- OLD name absent
$ grep -rl "ShippedGameModeRunsAtEasy" Source/
  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
  exit=0                                                  <-- CONTROL: the instrument speaks
```

  And the companion claim — *"No count of where else it survives is written, here or in
  `tests.md`"* — re-checked by reading every occurrence in both files:

```
$ grep -n "ShippedGameModeAuthorsEasyDifficulty" global.md tests.md
global.md:62   (rename narrative -- states no distribution)
global.md:87   (STAMPED block -- states a KIND, "in record-file stamps and in gate reports")
tests.md:86    (stamped, and says in terms that where else it survives is NOT stated here)
```

  No number and no terminated list of locations in either file. TRUE.
- **The observation count.** Both files say FOUR and both carry all four in substance
  (`global.md:59-81`; `tests.md:96-153`). Cross-checked against
  `2026-09-04-shipped-difficulty-pin.md`, whose `## Observations` section carries four numbered
  items and whose `## Findings` reads "**None.**". TRUE.
- **`global.md` is the only file with a live count or verdict.** The sweep's RECORD OWNERSHIP check
  passes on the real tree, and I proved that check non-vacuous with a planted mutant (instrument 2b
  below) rather than accepting its silence. `tests.md`'s newest entry cites the run by stamp and
  never by number, and says so in terms at `:21-23`. A grep for `VERDICT:` tokens across the state
  files returns hits in `content.md`, `data.md`, `decisions.md`, `engine.md` and `tests.md`, but
  every one is a NARRATION OF A PAST PHASE's verdict inside a historical entry — none is a live
  phase verdict for this pass, and none sits in this pass's newest entry in any file but
  `global.md`. TRUE.

## Instruments — run this pass, inline, and controlled

### 1. `strat_banner_sweep.py` — healthy path, REAL exit code, NOT through a pipe

```
$ python Tools/architect/strat_banner_sweep.py > <scratchpad>/sweep.txt 2>&1 ; echo REAL_EXIT
REAL_EXIT=0
$ cat <scratchpad>/sweep.txt
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn 2026.09.04-04.47.20
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

The exit code is taken from the process directly, with output redirected to a file — never read
after a pipe, which would report the tail of the pipeline instead of the sweep.

### 2. Two mutants, on two different checks, with a control on the harness itself

The state files were copied to a scratchpad and mutated THERE; the real record was never touched.
The harness loads the real `strat_banner_sweep.py` and calls `run_sweep()` on the copied paths.

**Control first, because a harness that cannot fail is not an instrument:** the UNMUTATED copy run
through the same harness prints `SWEEP CLEAN`. So neither mutant result below is an artefact of
copying.

**2a — SUITE COUNT AGREEMENT. Live 388/388 changed to 389/389 in the copy of `global.md`:**

```
  suite claims found: 61 (1 live, 60 stamped)
  [**SUITE COUNT AGREEMENT**] live suite claims disagree with the tree's total (388): global.md:28: 389/389
  [**SUITE COUNT AGREEMENT**] live suite claims disagree with the report's own succeeded count (388): global.md:28: 389/389

SWEEP FAILED -- the record contradicts itself or the tree
```

**2b — RECORD OWNERSHIP. A live suite count planted into the copy of `tests.md`.** This is the
check that guards the brief's own requirement that `global.md` be the only file with a live count,
so its silence on the real tree is worth nothing until it is shown able to speak:

```
  suite claims found: 62 (2 live, 60 stamped)
  [**RECORD OWNERSHIP**] only global.md may carry a LIVE suite count, but one appears in:
      tests.md:30: 388/388 -- link to the figure in global.md instead of restating it, or stamp it as history

SWEEP FAILED -- the record contradicts itself or the tree
```

Restoring the copy to pristine returns `SWEEP CLEAN` again. Both checks fire on a planted defect
and stay quiet on the healthy path. Non-vacuity discharged on both.

**The standing limit, restated because it is still open:** the sweep has no check for whether a
banner's prose claims ABOUT the gate reports are true. It reads suite counts, stamps, provenance
and ownership. `global.md:106-112` is invisible to it — which is why the last five findings on this
tree all needed a human-dispatched gate, and why a CLEAN sweep here is not evidence that R1 and R2
hold. They hold because I searched for them.

### 3. `Saved/AutomationReport/index.json`, opened `utf-8-sig`

```
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388   succeededWithWarnings: 0   failed: 0   notRun: 0
entry count: 388
states: Counter({'Success': 388})
NEW present: ['Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy']
OLD present: []
```

**`reportCreatedOn` is still `2026.09.04-04.47.20`.** Nothing ran that the brief did not know
about. The new name is present BY NAME and `Success`; the old name is absent BY NAME. Both
directions asserted; neither inferred from the delta.

### 4. Macro-based multiline set difference, `HEAD` to worktree — ONE collector, symmetric rule

**The rule, stated before the numbers and applied identically to both sides:** a path counts if and
only if it starts with `Source/`, ends with `.cpp`, and contains a `/Tests/` component. Both sides
take their file list from git — `git ls-tree -r HEAD` for the `HEAD` side, and `git ls-files` with
`--cached --others --exclude-standard` for the worktree side — so an untracked new test file would
be seen, and the committed fossil under `Tools/architect/evidence/…` that `tests.md:180-188`
records as having broken an earlier census is excluded by the SAME predicate on both sides rather
than by an ad-hoc filter on one. A single `collect()` function reads both sides.

```
HEAD files: 81   worktree files: 81
HEAD clauses: 387   worktree clauses: 388
ADDED (1):
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy ['Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp']
REMOVED (0):
duplicate clause names in worktree: none
```

**+1 added, 0 removed**, as the brief expects, and the file sets are equal at 81/81 — the asymmetry
`tests.md` records as caught mid-check does not recur under this rule.

## The twelve, re-verified rather than inherited

**1. Vendored bytes.** `git diff --stat -- Source/StratRules Data` prints nothing. The manifests are
the authority and were read, not quoted from prose:
`"rulesCommit": "96d93ea9e490c8cc33261f5d9671389a9c76ecfd"` in
`Source/StratRules/StratRules.manifest.json`, and
`"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"` in `Data/StratData.manifest.json`. pass.

**2. `strat::` calls.** A grep for the lowercase-initial CALL pattern across `Source/`, excluding
`Source/StratBridge/` and `Source/StratRules/`, returns nothing (`exit=1`). No hit to judge this
pass — no type name misreported as a call, because there was no hit at all. pass.

**3. Vendored header before UHT.** Refined to real include DIRECTIVES rather than prose mentions,
which is what makes this check produce judgements instead of noise:

```
$ grep -rnE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay).h"' Source/ --include=*.h
Source/StratBridge/StratBridge.h:53,55,56,57,58   (Data.h, Replay.h, Save.h, Scenario.h, Ui.h)
```

`StratBridge.h` is the only non-vendored header including them, and it declares no reflected type:
a line-anchored grep for `UCLASS`, `USTRUCT` and `UENUM` returns nothing (`exit=1`), and it has no
`.generated.h` include (`exit=1`). pass.

**4. `TPimplPtr`.** The only `TUniquePtr<FStratBridge` string in the tree is a comment at
`Source/StratUI/StratViewModel.h:47` recording the measured `C4150`. The live members are
`Source/StratPlay/StratMatchSubsystem.h:2496` and `Source/StratUI/StratScoreboardHUD.h:1235`, both
`TPimplPtr<FStratBridge>`. pass.

**5. Module arrows — DERIVED, not trusted.**

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
--- ROW COUNT --- 16
```

**16 rows, all five modules present** — no module missing, so the extraction did not fail silently
and return an empty result that a clean tree cannot be told from. Every row matches the expected
graph. The sixteenth row was READ FOR ITS GUARD rather than taken as an unconditional arrow,
because the extraction is textual and cannot show a branch:

```
Source/StratBridge/StratBridge.Build.cs
133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
135:  if (!bCompileVendoredRulesHere)
137:      PrivateDependencyModuleNames.Add("StratRules");
144:  PublicIncludePaths.AddRange(new string[] {
```

The link edge sits inside the `if (!bCompileVendoredRulesHere)` guard — monolithic targets only,
from the one expression that also governs the compile half. Correct and deliberate; **NOT a
finding.** `StratRules` appears in no `PublicDependencyModuleNames` and in no unconditional
`PrivateDependencyModuleNames`. `StratUI` has not gained a `Stratocracy` dependency. No cycle.

**Field census — 10 names, matching the recorded figure exactly:**

```
BRIDGE=                                 <-- the KNOWN false positive: the tail of the string
PCHUsage =                                  literal "STRAT_VENDORED_RULES_IN_BRIDGE=" inside
PrivateDefinitions.Add                      PrivateDefinitions.Add. Not chased, and not
PrivateDependencyModuleNames.Add            filtered away either -- the census is
PrivateDependencyModuleNames.AddRange       deliberately a dumb instrument.
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
--- FIELD COUNT --- 10
```

No eleventh name. No `DynamicallyLoadedModuleNames`, `PrivateIncludePaths` or
`PublicSystemLibraries` has appeared. pass.

**6. Module registration.** `Stratocracy.uproject` lists `Stratocracy`, `StratBridge`, `StratUI`
and `StratPlay`, plus plugin modules. Every `Source/*` directory was CHECKED for `IMPLEMENT_MODULE`
rather than assumed:

```
Source/StratBridge/   -> Source/StratBridge/StratBridge.cpp
Source/StratPlay/     -> Source/StratPlay/StratPlay.cpp
Source/StratRules/    -> (none)
Source/StratUI/       -> Source/StratUI/StratUI.cpp
Source/Stratocracy/   -> Source/Stratocracy/Stratocracy.cpp
```

`Source/StratRules/` carries no module object and is correctly ABSENT from the array; flagging it
would be the false positive that costs a startup abort to discover. No new module in this diff.
pass.

**7. `Source/Stratocracy/`.** `git diff --stat -- Source/Stratocracy` prints nothing. pass.

**8. `/Game/` literals.** Every one of the roughly 150 hits is either under a `Tests/` directory or
in `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp` — the automation-fixture and
import-commandlet exceptions, exactly the two that are standing. No gameplay C++ hit. The two
literals in the changed file (`StratShippedGameModeMatchConfig.cpp:113,117`) are fixture path
constants in a test, are pre-existing, and that file's own header at `:29` explains why they are
not a violation. pass.

**9. Widget arithmetic.** No `StratUI` file appears in `git diff --name-only` (`exit=1`). Nothing
added draws a number. pass.

**10. Lane discipline.** The three changed files are `Source/StratPlay/Tests/...` (inside `Tests/`,
the test-author's lane), `Tools/architect/state/tests.md` (the test-author's own record file) and
`Tools/architect/state/global.md` (the coordinator's own record file). No production code inside
`Tests/`, no test code outside it, no asset touched, no `Content/` change. pass.

**11. Prose blocks.** The changed file opens *"GDD Sec 2.7 / Sec 2.9 / Sec 2.11.6 -- what
`BP_StratGameMode` AUTHORS into its `MatchConfig`. T-FAME-02."* and the new clause carries a
why-first block quoting Sec 2.11.6 verbatim, naming its acceptance ID, explaining the deliberate
absence of a premise block, and recording its two mutants as measurements rather than intentions.
No `.Build.cs` changed. pass.

**12. Nothing staged or committed.** `git diff --cached --stat` is empty; `HEAD` is still
`9d705ca`. pass.

**Clause-body identity, re-derived:** `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`
declares its clause at `:381-383` under
`"Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy"`, reads
`Shipped->MatchConfig.Difficulty` at `:396`, quotes Sec 2.11.6 in its message at `:400-402`, prints
`(read: %s)`, ends *"Do not cite this clause as evidence that a designer set the tier"* at `:408`,
carries no premise block, and runs the `BP_StratGameMode_AiVsAi` control. Unchanged from RE-GATE 7
and matching every prose description of it in both record files.

## Observations

Non-gating. None of these gates the pass.

1. **`global.md:90-91`, "Gate reports name the OLD identifier", is still a universal over a set
   that is still growing.** It is TRUE right now — all eight existing reports contain the retired
   name, and this one does too, so my arrival preserves it. Carried forward unchanged from
   RE-GATE 7's Observation 1: unlike the sentence that was repaired, it is not false, and its
   evident subject is the reports that already exist and must not be edited. It remains one silent
   future report away from becoming false. Scoping it to the reports written before the rename
   would retire the hazard at no cost in meaning. **Left as an observation deliberately** —
   reporting a true sentence as a finding to keep a streak alive would be exactly the failure the
   brief warned against, and the bar it set was: is it FALSE, or does it assert closure over a
   growing set. This is neither.

2. **`global.md:71`'s "two lines above the file that disproved it" is still a rhetorical
   intensifier standing where a measurement appears to be.** RE-GATE 5 recorded this as a
   non-gating observation; I re-measured rather than inheriting it, because the lines have moved
   since. In the file as it stands the miscount sentence is at `:70-71` and the nearest naming of
   the disproving artifact is the glob at `:100` — **29 lines**, measured, not reasoned. No layout
   makes "two" true. It changes nothing a reader concludes about the tree, so it is not gated; it
   is recorded so it is not mistaken for a measurement by the next reader. **Contrast `:45-46`'s
   "twelve lines", which IS a faithful restatement of RE-GATE 1's own measurement of the PRE-repair
   layout (`...-regate.md:52`) and was verified as such by RE-GATE 3 — that one is not in this
   class.**

3. **`global.md:110` garden-paths on "a miscount name".** The phrase *"the paragraphs repairing a
   stale citation and a miscount name where each came from"* momentarily reads *"a miscount name"*
   as a noun phrase before the subject/verb split resolves. It is grammatical and it is true; a
   comma, or recasting to *"...and a miscount each name their source"*, would remove the stumble.
   Purely a readability note on a sentence a splice already passed through once.

4. **A count of the old name's distribution DOES exist in the tree, in the `.cpp`, and it is
   correct.** That file's header sends a reader to `tests.md` "from that gate report's four
   citations of it", and `grep -c` on `2026-09-04-shipped-difficulty-pin.md` returns exactly `4`.
   It is true, and it is safe only because that report is closed. The banner's "no count" claim is
   scoped to `global.md` and `tests.md`, so this third file does not falsify it — but a reader
   should know the count exists.

5. **The `autocrlf` warning is still real and still pre-existing.** `git diff` on the changed
   `.cpp` emits the LF-will-be-replaced-by-CRLF warning. The file is LF in the worktree and LF in
   the `HEAD` blob and this pass preserved that. Watch it at staging time; it is not a change this
   diff made.

6. **The sweep's bare-stamp blindness remains OPEN**, in `strat-data-steward`'s lane, and is
   correctly recorded as open at `global.md:50-57`. Until it is fixed, every report-stamp citation
   in the record must carry the literal `reportCreatedOn` token. All current citations do —
   re-checked this pass.

7. **The instrument gap that produced findings on rounds three through seven is structural and is
   still there.** `strat_banner_sweep.py` validates counts, stamps, provenance and ownership. It
   has no check for a banner's prose claims ABOUT the gate reports, which is the exact class of
   claim that failed five rounds running and passed on the sixth attempt only because a human gate
   read it. A sweep check that flags any live universal or cardinality whose subject is
   `Tools/architect/gate_reports/` would have caught every one of them from the record side. That
   is a candidate for `strat-data-steward`'s backlog, not a finding here. **Worth saying plainly
   now that the prose is finally true: the repair held because the claim was narrowed to something
   a search can settle, not because the instrument improved.**

VERDICT: PASS
