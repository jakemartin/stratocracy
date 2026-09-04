# Gate: shipped-`Difficulty` asset pin — RE-GATE 5 — strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`9d705ca`. Verified by absolute path rather than by cwd, because a subagent cwd echo has been
measured lying in this project:

```
$ git -C E:/MultiAgent/Stratocracy rev-parse HEAD
9d705ca6d7b639312ce761379786237e2d381cd3
$ git -C E:/MultiAgent/Stratocracy rev-parse --abbrev-ref HEAD
master
$ git -C E:/MultiAgent/Stratocracy status --porcelain
 M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
```

Three tracked files modified, zero staged, five untracked gate reports (this report is the sixth
artifact in that directory for this pass and does not touch the other five). No worktree under
`E:\MultiAgent\Strat-wt\` or `.claude\worktrees\` was read or audited; `git worktree list`
confirms this checkout is the `master` one at `9d705ca`.

Since RE-GATE 4 the diff moved in exactly two files, `Tools/architect/state/tests.md` and
`Tools/architect/state/global.md`. No code, no clause, no rebuild, no suite run — `reportCreatedOn`
is unchanged at `2026.09.04-04.47.20`, measured below.

```
$ git -C E:/MultiAgent/Stratocracy diff --numstat
146     2       Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
168     7       Tools/architect/state/global.md
173     1       Tools/architect/state/tests.md
```

---

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` prints nothing for both paths |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass — lowercase-initial call grep over `Source/` returns zero hits outside the two permitted directories |
| 3 | No vendored header in front of UHT | pass — the one file matching both patterns is `Source/StratBridge/StratBridge.h`, which declares no `UCLASS`/`USTRUCT`/`UENUM`; its six hits are prose explaining why it must not |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — the only `TUniquePtr<FStratBridge>` string in the tree is a comment at `Source/StratUI/StratViewModel.h:47` recording the measured `C4150` |
| 5 | Module arrows unchanged and correct | pass — derived, 16 rows, 10 census names; the 16th row is the guarded `.Add`, read in `StratBridge.Build.cs:133-138` |
| 6 | New modules registered; `StratRules` deliberately absent | pass — no new module; the uproject lists four, and the four `IMPLEMENT_MODULE` sites are exactly those four |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` prints nothing |
| 8 | No `/Game/` path literal in C++ | pass — every hit is under a `Tests/` directory except `ImportStratDataCommandlet.cpp:54`, which is the standing import-commandlet exception |
| 9 | No widget-side arithmetic | pass — no file under `Source/StratUI/` is in the diff |
| 10 | Lane discipline | pass — one file under `Source/StratPlay/Tests/` (test author), `tests.md` (test author), `global.md` (coordinator); nothing outside those lanes; nothing staged |
| 11 | Prose blocks present | pass — the new clause carries a why-first block citing GDD Sec 2.11.6 and Sec 2.9 and recording its two mutants as measurements; no new file and no new `.Build.cs` entry |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` is empty; HEAD is still `9d705ca` |
| — | THE TWO REPAIRS AND THE CROSS-FILE SWEEP | **BLOCK** — four findings, all in live prose in the two edited record files |

---

## Findings

### FINDING 1 — BLOCK. The replacement smuggled closure back in, in the shape its own rule names, and the universal it is built on is FALSE against this tree.

**Constraint violated:** a live claim in the record must be true, and a paragraph must not break
the rule it states two sentences above itself — the defect RE-GATE 3 and RE-GATE 4 both blocked on.

`Tools/architect/state/global.md:108-116`, live prose, unstamped. The rule, then the sentence that
breaks it, in one quotation so the adjacency is visible:

```
                                                 **The rule is not about the word "twice"; it is
about CLOSURE.** Any construction a reader can read as complete -- a number, "both", "each", a
terminated list -- is the defect. **What the re-gates found is in the re-gate reports, each under
its own findings section, and is not restated here.** The durable part is not the list anyway: it
is that EVERY defect this pass produced in this file was a live prose claim that a later edit in
the same pass falsified, that NO instrument in this tree could see any of them -- a claim about
gating history is not a suite figure, a stamp citation, a provenance path or an ownership claim,
so `strat_banner_sweep.py` was CLEAN across all of them -- and that each was caught only by a
dispatched gate reading for meaning.
```

**(a) It is the banned shape by the paragraph's own definition.** Line 109-110 types the ban as
*a number, "both", "each", a terminated list*. Line 112 opens with EVERY and line 115 with each,
about the same growing set — the defects this pass has produced in this file, a set this very
report is about to grow. I state the counter-reading, because it is a real one: a universal
generalisation is not an enumeration, and it does not go stale merely because the set grows — only
if a new member violates the predicate. Under that reading the sentence is legitimate and the
RULE at 109-110 is the thing that is over-broad. **Either way the file contains a rule it
contradicts six lines later, which is exactly what RE-GATE 4 FINDING 2 blocked on, one sentence
further down the same paragraph.** The owner must fix one or the other; this gate does not choose
which.

**(b) And it does not survive the counter-reading either, because the universal is UNTRUE.** The
predicate fails on a member of the set that is named in this same file. The miscount defect —
RE-GATE 3 FINDING 1, against `global.md:58-60`, "gated the pass PASS with zero findings and
raised one observation" — was **not** "a live prose claim that a later edit in the same pass
falsified". It was false at the instant it was written, against an artifact that already existed:

- `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md` carries four numbered items
  under its Observations heading and has an mtime of `2026-09-04 00:29:09` (inlined below).
- A sentence that reports that gate's verdict cannot predate that gate's report — the report IS
  the gate's output. So the miscount was written after the disproving artifact existed.
- **This file says so itself, forty lines above the universal.** `global.md:70-72`: "THE OTHER
  THREE OBSERVATIONS ARE RECORDED HERE BECAUSE THE SENTENCE ABOVE ONCE SAID ONE AND THEREFORE
  DROPPED THEM -- a live miscount found on the third re-gate, two lines above the file that
  disproved it." The file that disproved it is named in the same paragraph and pre-existed the
  claim. So line 112 and lines 70-72 are two answers to one question inside one banner.

**Why this is not a quibble.** The universal is offered as "the durable part" — the lesson a
future reader is meant to carry. It names one hazard (a claim a later edit falsifies) and thereby
tells that reader the OTHER hazard did not occur here. The other hazard is the one that actually
recurred: a precise claim written from a brief without being checked against an artifact that was
already on disk — twice, in two files, across three gates (RE-GATE 3 FINDING 1 in `global.md`,
RE-GATE 4 FINDING 1 in `tests.md`, and FINDING 4 below makes a third). A durable lesson that
misnames the recurring failure is worse than no lesson.

**And this report falsifies it on landing, independently of my raising it.** FINDING 3 and FINDING
4 below are defects produced by this pass, in these files, that no later edit falsified — they were
untrue when written.

**Owner: the `coordinator`**, in lane on its own file, no exception clause needed. The fix is to
say what is actually durable without quantifying over the set — for example that the defects this
pass produced in this file were live prose claims invisible to every instrument, found only by a
dispatched gate — and to drop the mechanism claim, or to state the mechanism per instance under a
STAMPED line as an as-of claim. **I do not apply it.**

---

### FINDING 2 — BLOCK. A typed count of the same growing set stands thirteen lines below the rule forbidding it, and a report already in this tree contradicts it.

**Constraint violated:** the one the file states at 103-104 — "NEVER WRITE AN EXHAUSTIVE COUNT
OF A GROWING SET INSIDE THE THING THAT GROWS IT."

`Tools/architect/state/global.md:117-124`, live prose, unstamped:

```
THAT IS THE SAME DEFECT TWICE IN ONE PASS AND IS WORTH MORE THAN THE SENTENCE IT COST. **A
CORRECTION THAT ONLY ADDS LEAVES BOTH CLAIMS LIVE.** The repair above added a paragraph saying
the finding came from a re-gate and did not go back to stamp the sentence that denied one had
happened, so this banner asserted both -- exactly the shape of the finding it was repairing, one
paragraph later. Neither instance was visible to any instrument in this tree: a prose claim about
gating history is not a suite figure, a stamp citation, a provenance path or a record-ownership
claim, so `strat_banner_sweep.py` was CLEAN across both. Both were found by a dispatched gate,
which is the only reader in this project that reads for meaning.
```

TWICE, "Neither instance", "Both were found", "CLEAN across both" — four closed constructions
over the set "instances of a correction that only adds, in this pass", inside the document that
grows it.

**It is already false, and not because of me.**
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md`, in its FINDING 2,
states plainly:

```
**This is the third instance in one pass of "a correction that only adds leaves both
claims live" -- the banner names two at `:109`; this is the third, and it is inside the sentence
written to close the second.**
```

That third instance is the one RE-GATE 4 FINDING 1 filed: the observation miscount was repaired in
`global.md` and left standing in `tests.md`, so the tree carried FOUR and one simultaneously —
a correction that only reached one of the two files it needed to reach, leaving both claims live.
The report saying so was on disk at `2026-09-04 02:09:16`, before `global.md` was last written at
`2026-09-04 02:10:06`. **The count was not merely foreseeably falsified; it was falsified by an
artifact that existed one minute before the file was saved** — the same failure mode as FINDING 1's
counter-example, in the paragraph immediately below the one that claims that failure mode did not
occur.

**Owner: the `coordinator`**, in lane on its own file. The fix is the paragraph's own prescription
applied to itself: name the class and let a reader count, or move the cardinality under a STAMPED
line as an as-of claim. **I do not apply it.**

---

### FINDING 3 — BLOCK. Both edited files assert, in one pass and by two writers, that the retired clause name survives in exactly one place. It survives in two, and the second is six lines above the sentence that says so.

**Constraint violated:** a live exclusivity claim about the tree must be true, and two record files
must not carry a claim the tree falsifies. This is the cross-file class the brief sent me to sweep
and the class RE-GATE 4 FINDING 1 blocked on.

`Tools/architect/state/tests.md:86-88`, live prose inside the new entry's stamp:

```
    `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`
    **[STAMPED 2026-09-04: that name is retired. It is written out here in full, and only
    here, so that a reader arriving from the four citations of it in
```

`Tools/architect/state/global.md:87-89` and `:93-94`, agreeing with it:

```
STAMPED> "`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`"
STAMPED>  -- the clause's name before the rename. Retired from `Source/`; it survives
STAMPED>  deliberately in `tests.md`'s own stamp and in the gate report, see below.
...
and editing it would falsify what was actually gated. `tests.md` carries the one surviving stamp
of the old name under `Tools/architect/`, so a reader arriving from those four citations has a
trail.
```

Measured, not reasoned — every occurrence of the retired identifier in the tree:

```
$ grep -rn ShippedGameModeAuthorsEasyDifficulty Source/ Tools/ .agents/ Config/
Tools/architect/state/global.md:62:  `...ShippedGameModeAuthorsEasyDifficulty`,
Tools/architect/state/global.md:87:  STAMPED> "`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`"
Tools/architect/state/tests.md:86:   `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md            4 occurrences
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md     1
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md   1
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md   2
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md   1
$ grep -rc ShippedGameModeAuthorsEasyDifficulty Source/
(zero hits under Source/ -- that half of both claims is TRUE)
```

Three things are false and one is true:

- **FALSE:** `tests.md:87`, "written out here in full, and only here". `global.md:87` writes the
  same identifier out in full, with its full namespace, inside a STAMPED line.
- **FALSE:** `global.md:93-94`, "`tests.md` carries the one surviving stamp of the old name under
  `Tools/architect/`". `global.md:82-89` is itself a stamp block under `Tools/architect/`
  carrying the old name in full. The sentence is falsified by the six lines directly above it.
- **FALSE:** `global.md:89`, "and in the gate report", singular. Five files under `gate_reports/`
  carry it, nine occurrences in total.
- **TRUE and verified:** the name is left nowhere under `Source/`.

**The counter-reading, stated because the owner is entitled to judge it.** Each sentence can be
read as implicitly excluding its own file — "only here, besides where you already are". I am not
taking that reading, for the reason RE-GATE 3 gave when it changed its own severity call: this is
a precise exclusivity claim standing beside another precise claim, written to route a reader, and
it is the fifth pass over these two files. A reader who does the obvious thing — grep for the
retired name to see where the trail is — finds two record-file stamps and five gate reports, having
just been told there is one of each. **The verdict does not hinge on it: FINDINGS 1 and 2 block on
their own.**

**Owner: split, and that is the point.** `global.md:87-94` is the `coordinator`'s, in lane.
`tests.md:87-88` is `strat-test-author`'s, in lane. **Neither may be fixed by the other seat** —
the transcription clause is triggered by A MERGE and must name one, and there has been no merge
here (base `9d705ca`, uncommitted, no wave in flight). **I do not apply either.**

---

### FINDING 4 — BLOCK. The `tests.md` stamp that repairs the miscount explains it with a claim that is false, and the false explanation was inherited from a gate report rather than measured.

**Constraint violated:** a live claim in the record must be true. A stamp that retires a false
claim must not assert a new one.

`Tools/architect/state/tests.md:95-101`, live prose inside the repair:

```
      `strat-integration-reviewer` gated the pass PASS with zero findings -- its `## Findings`
      section reads "**None.**" -- and raised **FOUR observations**, all marked "Non-gating."
      **[STAMPED 2026-09-04: this sentence originally read "gated the pass PASS with zero
      findings but raised **one observation**". THAT WAS FALSE BY THREE. It was written
      before `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md` existed to
      be checked against, which is why it was written, and it stayed false after that report
      landed.
```

**"It was written before that report existed to be checked against" is false, and the ordering is
not close.** Two independent arguments, one of which needs no timestamps at all:

1. **Causal, and decisive on its own.** The sentence reports that gate's verdict and its
   observation, and sits inside a bullet about a rename that the gate's observation caused. A
   sentence reporting a gate's verdict cannot predate the gate's report, because the report is
   the gate's output. So the sentence was written after the pin report existed.
2. **Timestamps, second-hand but consistent.** The pin report has mtime `2026-09-04 00:29:09`.
   RE-GATE 4 recorded `tests.md`'s then-current write as `00:49:03` — twenty minutes later.
   `tests.md` has since been rewritten at `02:12:13`, so I cannot re-measure the earlier mtime
   myself; I say so rather than presenting it as my own measurement.

**And the false explanation appears to be INHERITED, which makes it worth more than a corrected
sentence.** RE-GATE 4 FINDING 1 wrote "`tests.md` was last written at 00:49:03, before the first
gate report existed" — a conclusion its own two numbers contradict, since `00:49:03` is after
`00:29:09`. The lane repaired the count faithfully and carried the reviewer's mis-derived
explanation across as a measurement. That is this project's recorded "an inherited prose phrase is
not a measurement" hazard, and its own rule is to fix the ORIGIN too — but the gate report is
history and must stay unedited, so the correction belongs here and in `tests.md`.

**Why it matters rather than being a tidy-up.** The stamp's job is to tell the next reader how the
error happened so it is not repeated. "Written before the artifact existed" is an excusable timing
accident that prescribes nothing. What actually happened is that a precise count was written from
a dispatch brief while the artifact that disproves it had been on disk for twenty minutes — which
prescribes exactly the rule this project already carries: **the brief is not ground truth; open the
artifact.** The wrong diagnosis retires the wrong lesson.

**Owner: `strat-test-author`**, in lane on its own file and its own newest entry. **I do not apply
it.**

---

## Observations

Non-gating.

1. **A measured-sounding distance in `global.md` does not match any measurement I can make.**
   `global.md:70-72` reads "a live miscount found on the third re-gate, two lines above the file
   that disproved it." In the file as it stands, that sentence is at 70-71 and the nearest naming
   of `2026-09-04-shipped-difficulty-pin.md` is at 91 — twenty lines below. In the tree RE-GATE 3
   audited, its FINDING 1 measured the miscount at 58-60 and the naming of the same file at 77 —
   seventeen lines. I could not find a layout under which "two lines" is true. It is a rhetorical
   intensifier standing where a measurement appears to be, which is the same class as FINDING 4 at
   lower cost. Not gated: it changes nothing a reader concludes about the tree.
2. **`git diff` still emits the autocrlf warning on the `.cpp`, and it remains pre-existing.**
   Measured this pass: the `.cpp` is bare-LF in the worktree (438 LF, 0 CRLF) and bare-LF in the
   HEAD blob (294 LF, 0 CRLF). Both record files are CRLF in the worktree and LF in their HEAD
   blobs, with zero mixed endings after the edit — the lane's "CRLF preserved" claim is correct.
3. **The clause-body identity is unchanged from RE-GATE 4 and matches every prose description of
   it.** `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` declares
   `FStratShippedGameModeRunsAtEasyTest` under
   "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy", reads
   `Shipped->MatchConfig.Difficulty`, prints "(read: ...)", ends its message "Do not cite this
   clause as evidence that a designer set the tier", carries no premise block, and runs the
   cross-package `TestNotEqual` liveness control. Nothing in the diff since RE-GATE 4 touches it.
4. **The three observation summaries in `tests.md` are faithful, not merely present.** I compared
   each against what I wrote in the pin report's Observations, claim by claim: (2) pre-existing
   autocrlf, LF both sides, watch at staging — faithful; (3) rendered strings not arrays, correct
   here because digits and commas cannot be bitten by the case-insensitivity of FString, recorded
   for a future case-bearing field — faithful, and the lane's added sentence about a name, tag or
   tier label going through the same helper extends my point rather than shifting it; (4) the gate
   verified the banner SAYS both routes were offered in the required inline shape and records that
   it could not verify that it HAPPENED — faithful, including "which of the two it did". No
   paraphrase shifts a claim. The count is FOUR, counted by me against the artifact and not
   inherited.
5. **Obligation (1) of the in-session clause remains unfalsifiable from a checkout**, as
   `CLAUDE.md` declares. `global.md:20-27` says both routes were offered with their costs named
   and that the user chose dispatch, inline and in the required shape. I verified it SAYS it. I
   cannot verify it HAPPENED, and I record which of the two I did. Unchanged from every prior pass.
6. **Nothing else in either file is falsified by this report's landing.** `global.md:97-99` and
   `tests.md:130-134` both name the re-gate glob and decline to count — both survive this file's
   creation, which is the repair working. `global.md:91`, "names the OLD identifier four times",
   is scoped to the pin report and is verified TRUE at four occurrences. The two claims this
   report does falsify are FINDING 1's universal and FINDING 2's count, and both were already
   false before it.

---

## Instruments, inlined

### I1. Banner sweep — real exit code, not through a pipe

```
$ python Tools/architect/strat_banner_sweep.py > sweep_out.txt 2>&1 ; echo EXITCODE=$?
EXITCODE=0
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn=2026.09.04-04.47.20
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

The exit code is the sweep's own: stdout and stderr were redirected to a FILE and the status read
immediately, never after a pipe — this project's recorded phantom-exit-zero hazard.

### I2. The sweep is NOT vacuous — mutant plus healthy-path control, same invocation

The mutant is a COPY in the scratchpad, outside the repository; nothing in the tree was modified.
The sweep resolves its paths from its own file location and accepts explicit record paths, so the
copy is swept against the real `Saved/AutomationReport/index.json`. The mutation is one digit: the
live suite claim 388/388 becomes 389/389, applied with sed to a copy.

```
$ python Tools/architect/strat_banner_sweep.py MUT/global.md > out.txt 2>&1 ; echo EXITCODE=$?
EXITCODE=1
Banner sweep: global.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn=2026.09.04-04.47.20
  suite claims found: 58 (1 live, 57 stamped)
  tracked items declared: 2

  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the tree total (388): global.md:28: 389/389
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the report succeeded count (388): global.md:28: 389/389

SWEEP FAILED -- the record contradicts itself or the tree
```

The control — the SAME single-file invocation against the real file, so the only difference between
the two runs is the mutated digit, not the argument shape:

```
$ python Tools/architect/strat_banner_sweep.py Tools/architect/state/global.md > ctl.txt 2>&1 ; echo EXITCODE=$?
EXITCODE=0
Banner sweep: global.md
  suite claims found: 58 (1 live, 57 stamped)
SWEEP CLEAN -- no self-contradiction found
```

Both directions measured: the guard fires on a planted defect and stays quiet on the healthy path.
The one live suite claim it governs is `global.md:28`. The four findings above are prose claims
about gating history, which the sweep does not model — which is why they needed a gate.

### I3. The automation report, opened utf-8-sig

```
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388
succeededWithWarnings: 0
failed: 0
notRun: 0
entry count: 388
NEW present: True     Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
OLD present: False    Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty
distinct states: Success only
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide -> Success
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy -> Success
```

`reportCreatedOn` is `2026.09.04-04.47.20`, unchanged from RE-GATE 4 and from the brief expectation.
Nothing ran that the user does not know about.

### I4. Macro-based multiline set difference, HEAD to worktree, ONE collector, both sides scoped to Source/

The symmetric file-set rule is stated and enforced: the HEAD side is `git ls-tree -r HEAD Source/`
filtered to `.cpp` under `Source/`; the worktree side is an os.walk of `Source/` filtered
identically. This is the asymmetry `tests.md` reports catching (a fossil copy of
`StratScoreboardParity.cpp` under `Tools/architect/evidence/`); scoping BOTH sides to `Source/`
excludes it from both.

```
HEAD .cpp under Source/: 159
worktree .cpp under Source/: 159
HEAD macro clauses: 387
worktree macro clauses: 388
ADDED (1):
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
     Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
     FStratShippedGameModeRunsAtEasyTest
REMOVED (0):
```

+1 added, 0 removed, both file sets equal at 159. The regex is multiline (the macro name, open
paren, test class name, comma, quoted clause path, matched with DOTALL), because a single-line
grep returns zero on this codebase wrapped macro invocations.

### I5. Module arrows, DERIVED

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
--- rows: 16
```

All five modules present, so the extraction did not silently fail. The sixteenth row is the
CONDITIONAL one, and the extraction cannot show that it is conditional — so I read the branch,
`Source/StratBridge/StratBridge.Build.cs:133-138`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
```

Guarded, monolithic-only, and both halves come from the one expression. `StratRules` appears in no
`PublicDependencyModuleNames` and in no UNCONDITIONAL `PrivateDependencyModuleNames`. Correct and
not a finding. `StratUI` has not gained a `Stratocracy` dependency; no cycle.

Field census — TEN names, matching the recorded figure:

```
BRIDGE=                            <- the known false positive: the tail of the string literal
PCHUsage =                            STRAT_VENDORED_RULES_IN_BRIDGE= inside PrivateDefinitions.Add
PrivateDefinitions.Add
PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
```

No eleventh name. No DynamicallyLoadedModuleNames, PrivateIncludePaths or PublicSystemLibraries.

### I6. Registration and line endings

```
uproject Modules: Stratocracy, StratBridge, StratUI, StratPlay
IMPLEMENT_MODULE sites: Source/StratBridge/StratBridge.cpp, Source/Stratocracy/Stratocracy.cpp,
                        Source/StratPlay/StratPlay.cpp, Source/StratUI/StratUI.cpp
```

Four modules, four module objects, exact match. `Source/StratRules/` carries no IMPLEMENT_MODULE
and is correctly absent from the array; flagging it would cost a startup abort to discover.

```
Tools/architect/state/tests.md    worktree CRLF=5968 bareLF=0   HEAD blob CRLF=0 bareLF=5796
Tools/architect/state/global.md   worktree CRLF=7556 bareLF=0   HEAD blob CRLF=0 bareLF=7395
StratShippedGameModeMatchConfig.cpp  worktree CRLF=0 bareLF=438  HEAD blob CRLF=0 bareLF=294

$ git diff --numstat -- Tools/architect/state/tests.md
173     1       Tools/architect/state/tests.md
```

The lane's `173 1` numstat verified, and its CRLF claim verified: `tests.md` is uniformly CRLF in
the worktree before and after, with zero mixed endings introduced.

### I7. Timestamps used in FINDING 2 and FINDING 4

```
2026-09-04 00:29:09  Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
2026-09-04 01:14:22  ...-regate.md
2026-09-04 01:31:02  ...-regate-2.md
2026-09-04 01:50:28  ...-regate-3.md
2026-09-04 02:09:16  ...-regate-4.md
2026-09-04 02:10:06  Tools/architect/state/global.md
2026-09-04 02:12:13  Tools/architect/state/tests.md
2026-09-04 00:44:16  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
2026-09-04 00:47:20  Saved/AutomationReport/index.json
```

An ascending gate sequence, with both record files written after the last gate report they answer.

---

VERDICT: BLOCK
