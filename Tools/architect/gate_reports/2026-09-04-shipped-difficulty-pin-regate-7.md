# Gate: `Difficulty` shipped-asset pin — RE-GATE 7 — strat-integration-reviewer

## Scope

The **UNCOMMITTED working tree** at `E:/MultiAgent/Stratocracy`, branch `master`, over base commit
`9d705ca`. Not a worktree — verified by absolute path:

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

Three modified files, seven untracked gate reports, nothing staged:

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
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
$ git diff --cached --stat
(empty)
```

**The brief's account of what changed since RE-GATE 6 is CONFIRMED against the tree by mtime**,
which matters because the brief says only `global.md` moved:

```
2026-09-04 00:44:16  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
2026-09-04 00:47:20  Saved/AutomationReport/index.json
2026-09-04 02:31:26  Tools/architect/state/tests.md
2026-09-04 02:49:32  Tools/architect/gate_reports/...-regate-6.md
2026-09-04 02:50:26  Tools/architect/state/global.md      <-- the ONLY file after RE-GATE 6
```

No rebuild and no suite run: `index.json` is untouched at 00:47:20 and its `reportCreatedOn` is
still `2026.09.04-04.47.20`, as the brief states.

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
| 9 | No widget-side arithmetic | pass — no `StratUI` change in the diff |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| R1 | RECORD — FINDING 1's repair (`:94-96`) true against the tree | pass |
| R2 | RECORD — FINDING 2's repair (`:102-108`), FIRST-gate half | pass |
| R3 | RECORD — FINDING 2's repair, "restates nothing from the re-gates" | **BLOCK** |
| R4 | RECORD — the "CLOSED artifact" justification | pass (by mtime; limit stated) |
| R5 | RECORD — this report's arrival falsifies nothing in either file | pass |
| R6 | RECORD — cross-file sweep; `global.md` sole holder of live count/verdict | pass |
| I1 | `strat_banner_sweep.py` — healthy path + mutant | pass, non-vacuity proven |
| I2 | `index.json` read `utf-8-sig`, stamp unmoved, names by name | pass |
| I3 | Macro set difference HEAD to worktree | pass — +1 added, 0 removed |

## Findings

### FINDING 1 — BLOCK. The repaired sentence at `global.md:106-108` asserts a universal that three passages of the SAME banner falsify. Two are re-gate FINDINGS restated in substance; one names a re-gate by ordinal.

**The claim, `Tools/architect/state/global.md:102-108`:**

```
102  WHAT THIS BANNER DOES AND DOES NOT RESTATE, SAID EXACTLY, BECAUSE A LOOSER VERSION OF THIS
103  SENTENCE WAS ITSELF A FINDING. It restates the FIRST gate -- its `VERDICT: PASS`, its zero
104  findings, and its four observations in substance -- because that gate is CLOSED: it is a
105  finished artifact that nothing further will add to, its first observation is why the rename
106  happened, and the other three are live caveats a reader of this pass needs. **It restates
107  nothing from the re-gates** -- not their verdicts, not their number, not their findings --
108  because that set was still growing while this was written.
```

The enumeration is explicit: **not their verdicts, not their number, not their findings.**
Adjudicated directly, as the brief asked, by searching the banner (lines 14-155, the 2026-09-04
`_Last run` entry) for any restatement of any re-gate's verdict, count or findings. **Three exist,
all inside the same banner, all ABOVE the sentence that denies them.**

**(a) `global.md:44-49` — a re-gate FINDING restated, and characterised by its verdict class.**

```
44  THAT SENTENCE WAS A BLOCKING FINDING AND IS FIXED HERE RATHER THAN QUIETLY. It cited
45  `2026.09.04-04.09.26` -- retired by the rename re-run -- LIVE and UNSTAMPED, twelve lines
46  above the block that stamps that very citation as overwritten, so this banner asserted both.
47  Found by `strat-integration-reviewer` on the 2026-09-04 RE-GATE, not by the sweep, and the
48  reason the sweep could not find it is itself the debt below. Owner: the `coordinator`, in
49  lane on its own file.
```

This is not an allusion. It states WHAT the finding was, WHO raised it, WHERE (`the 2026-09-04
RE-GATE`), and that it was **BLOCKING** — a restatement of that re-gate's verdict class in all but
the token. Measured: the finding belongs to
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md`, which carries
`VERDICT: BLOCK` and 16 occurrences of `04.09.26`.

**(b) `global.md:53-55` — the same re-gate's EVIDENCE restated.**

```
53  is exactly how a stale citation survived a CLEAN sweep and needed a human-dispatched gate to
54  catch. The reviewer proved it with an in-memory mutant that adds ONLY that token and makes
55  the check fire. The fix -- matching a bare `YYYY.MM.DD-HH.MM.SS` stamp too -- is in
```

The instrument a re-gate built and what it showed — that finding's warrant, restated.

**(c) `global.md:70-72` — a re-gate finding restated AND the re-gate identified by ORDINAL.**

```
70  THE OTHER THREE OBSERVATIONS ARE RECORDED HERE BECAUSE THE SENTENCE ABOVE ONCE SAID "ONE" AND
71  THEREFORE DROPPED THEM -- a live miscount found on the third re-gate, two lines above the file
72  that disproved it. (2) The `autocrlf` warning on the `.cpp` is REAL and PRE-EXISTING: the file
```

`a live miscount found on the third re-gate` is a re-gate's finding, attributed to a specific
re-gate by position in the sequence. **And it is ACCURATE**, which is what makes it a restatement
rather than a mistake:

```
$ sed -n '/^## Findings/,/^## Observations/p' ...-shipped-difficulty-pin-regate-3.md
### FINDING 1 - BLOCK. The banner says the first gate "raised one observation". It raised FOUR,
    and the file that proves it is named in the same paragraph.
$ grep -n "^VERDICT" ...-shipped-difficulty-pin-regate-3.md
623:VERDICT: BLOCK
```

Naming order confirmed: `-regate.md` is RE-GATE 1, `-regate-2.md` is 2, `-regate-3.md` is 3. The
ordinal is right. The claim that nothing from the re-gates is restated is wrong.

**Per-report cross-check, so the attributions above are not taken from the banner being audited:**

```
2026-09-04-shipped-difficulty-pin-regate.md   : VERDICT: BLOCK : 04.09.26 hits=16 : miscount hits=0
2026-09-04-shipped-difficulty-pin-regate-2.md : VERDICT: BLOCK : 04.09.26 hits=14 : miscount hits=0
2026-09-04-shipped-difficulty-pin-regate-3.md : VERDICT: BLOCK : 04.09.26 hits=2  : miscount hits=0
2026-09-04-shipped-difficulty-pin-regate-4.md : VERDICT: BLOCK : 04.09.26 hits=6  : miscount hits=4
2026-09-04-shipped-difficulty-pin-regate-5.md : VERDICT: BLOCK : 04.09.26 hits=0  : miscount hits=7
2026-09-04-shipped-difficulty-pin-regate-6.md : VERDICT: BLOCK : 04.09.26 hits=4  : miscount hits=6
```

**Why this is a finding under the brief's own bar, and not the shape flagged reflexively.** The brief
warned that a universal negative which is TRUE and checkable is not automatically a defect. I agree,
and I applied it: **I did not block on the universal's SHAPE. I blocked because I checked it and it
is FALSE.** The falsifying evidence is not a future re-gate and not this report — it is three
passages of the paragraph's own banner, sitting 34, 52 and 62 lines above the sentence that denies
them. This is the failure mode the banner itself diagnoses two sentences later at `:109-115` — *"a
count, then an enumeration with the count merely implied, then a universal"* — with the universal now
falsified not by the next gate but by the file it was written into.

**The narrower true statement is available and costs nothing.** What the sentence appears to MEAN is
that the banner does not narrate the re-gate sequence as a set — no total, no roster, no terminated
list. That is true: no count of the re-gates and no enumeration of them appears anywhere in the
banner (verified; `:86-88` correctly points at the glob instead). Saying that, instead of "restates
nothing from the re-gates ... not their findings", would be both true and checkable, and the repairs
at `:44`, `:53` and `:70` would stand as what they are — fixes this banner made, each naming its
cause.

**Owner: the `coordinator`, in lane on its own file.** No code, test or asset change is implied.

## Adjudication of the checks the brief asked for, item by item

### R1 — FINDING 1's repair (`global.md:94-96`). TRUE. Both new claims verified independently.

```
94  follow it forward to the live name. **It does not occur under `Source/` at all** -- the one
95  property of its distribution that a checkout can RE-EXECUTE, and the one that stays true as
96  the reports accumulate. No count of where else it survives is written, here or in `tests.md`.
```

**Claim A — `Source/`-absence is re-executable. RUN, with a positive control, because an absence
measured by a silent instrument is not a measurement:**

```
$ grep -rl "ShippedGameModeAuthorsEasyDifficulty" Source/
grep exit=1                                   <-- nothing; the OLD name is absent
$ grep -rl "ShippedGameModeRunsAtEasy" Source/
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
grep exit=0                                   <-- CONTROL: the same search over the same tree speaks
```

The instrument is shown able to return a hit from `Source/` before its silence on the old name is
read as absence. Claim A holds, and it holds by RE-EXECUTION rather than by citation.

**Claim B — no count of where else it survives is written in `global.md` or `tests.md`. TRUE.** This
is a claim about two files and it is checkable, so I read every occurrence of the retired identifier
in both:

```
$ grep -n "ShippedGameModeAuthorsEasyDifficulty" Tools/architect/state/global.md Tools/architect/state/tests.md
global.md:62: `...ShippedGameModeAuthorsEasyDifficulty`,
global.md:87: STAMPED> "`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`"
tests.md:86:      `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`
```

`global.md:62` is the rename narrative and states no distribution. `global.md:88-89` says *"it
survives deliberately in record-file stamps and in gate reports"* — a statement of KIND, not a count
and not a number. `tests.md:86-93` says in terms that where else it survives is deliberately NOT
stated there, in a number or a list or a "the one", and then gives the same re-executable property
with its own control. **No number, no terminated list of locations appears in either file.**
Claim B holds.

### R2 — FINDING 2's repair, the FIRST-gate half. TRUE, and accurate item by item.

Verified against `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md` directly:

```
$ grep -n "^VERDICT" ...-shipped-difficulty-pin.md
538:VERDICT: PASS                                          <-- "its VERDICT: PASS"      OK

$ sed -n '/^## Findings/,/^## Observations/p' ...
## Findings

**None.** No constraint in the twelve is violated, ...     <-- "its zero findings"      OK

(numbered items under ## Observations)
1. THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP ...
2. The `autocrlf` warning `tests.md` reports is real and is pre-existing. ...
3. The liveness control compares STRINGS, not arrays. ...
4. Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable ...
                                                           <-- "its four observations"  OK
```

"In substance" is earned rather than asserted — the banner carries all four, not just the count:
observation 1 is the rename paragraph at `:58-69`; observation 2 is `:72-74` (autocrlf, real and
pre-existing); observation 3 is `:74-77` (rendered strings `(1)` vs `(0,1)`, not the two
`TArray<int32>`s); observation 4 is `:77-81` (obligation (1) unfalsifiable from a checkout). Each
matches its source observation's content.

### R3 — FINDING 2's repair, "restates nothing from the re-gates". **FALSE.** See FINDING 1.

### R4 — The "CLOSED artifact" justification. Holds on the evidence available; the limit is stated.

The claim is that the first gate report is finished and nothing further will add to it. Measured by
mtime across all seven reports:

```
00:29:09  ...-shipped-difficulty-pin.md
01:14:22  ...-regate.md
01:31:02  ...-regate-2.md
01:50:28  ...-regate-3.md
02:09:16  ...-regate-4.md
02:27:37  ...-regate-5.md
02:49:32  ...-regate-6.md
```

Strictly increasing in report order; **no report has an mtime later than any successor's**, so no
later pass modified an earlier report. The first gate report has not been touched since 00:29:09,
before every re-gate. **The honest limit: these files are untracked, so there is no committed blob to
diff against and mtime is the only instrument a checkout has here.** An edit that preserved mtime
would be invisible to it. I report what was measured and what it cannot see.

### R5 — This report's arrival falsifies nothing in either file. Verified.

`global.md:86-88` states that the gate reports are the files matching
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md`, that each is a new file leaving
its predecessors byte-identical, and that each carries its own `VERDICT:` line and its own findings.
This file is named `2026-09-04-shipped-difficulty-pin-regate-7.md`: it matches that glob and
`tests.md`'s narrower `...-regate*.md` glob; it is a new file; it edits no predecessor; it carries a
`VERDICT:` line and a `## Findings` section. `global.md:90-91` says gate reports name the OLD
identifier — this report does, repeatedly, so its arrival preserves that sentence rather than
falsifying it. `tests.md`'s "Later re-gates ... are NOT enumerated here" is untouched, because it
enumerates none.

### R6 — Cross-file sweep of the two newest entries. No live assertion found false.

- **What the clause pins and does not pin.** `global.md:116-126` and `tests.md`'s bullets both say
  the clause pins the VALUE and not the AUTHORSHIP, resting on `FStratMatchConfig::Difficulty`
  initialising to `Easy` and `Easy` being the enum's zero. Both halves re-verified in the tree at
  `Source/StratPlay/StratMatchSubsystem.h` — the field initialiser is `= EStratDifficulty::Easy`, and
  `enum class EStratDifficulty : uint8 { Easy, Normal, Hard }` carries no explicit initialisers. The
  two files agree with each other, with the `.cpp` header, and with the assertion message. TRUE.
- **The deliberate absence of a premise block.** Both files state it with the same reason; the `.cpp`
  carries none. Consistent. TRUE.
- **The rename and why.** `global.md:58-69` and `tests.md`'s rename bullet both attribute it to the
  first gate's observation 1 and to the user's ruling, and both say the lane did not choose the name.
  The `.cpp` comment "WHY THE NAME SAYS `RunsAt` AND NOT `Authors`" agrees. TRUE.
- **Lane attribution, inline and separate.** `global.md` opens "Written by the `coordinator`, whose
  file this is, IN LANE AND UNDER NO EXCEPTION CLAUSE -- this seat wrote no code and no clause" and
  records the route as a user choice with both costs named. `tests.md` opens "`strat-test-author`
  (ACTING and WRITING; IN LANE ... base commit `9d705ca`, UNCOMMITTED)". Both inline, both name
  acting and writing, neither claims a transcription. The base commit matches `HEAD`. TRUE.
- **The report and its stamp.** Both cite `reportCreatedOn 2026.09.04-04.47.20`, both carry the
  `reportCreatedOn` token that the open instrument debt requires, and both stamp the retired
  `2026.09.04-04.09.26` rather than leaving it live. Re-measured: the live stamp is unchanged. TRUE.
- **Open debts.** `global.md` records the sweep's bare-stamp blindness as OPEN in
  `strat-data-steward`'s lane and the authorship pin as OPEN in `strat-gameplay-engineer`'s. Neither
  is written in this tree; both correctly read as open. `tests.md` restates the authorship limit
  without claiming closure. TRUE.
- **The `Source/`-absence property.** Stated in both files, phrased differently, both true (R1), and
  the two do not contradict.
- **The observation count.** Both files say FOUR and both carry all four in substance (R2). TRUE.
- **Sole holder of a live count or verdict.** The sweep's RECORD OWNERSHIP check passes;
  `tests.md`'s newest entry cites the run by stamp and never by number, and says so in terms.

## Instruments — run, inline, and controlled

### 1. `strat_banner_sweep.py` — healthy path, REAL exit code, NOT through a pipe

```
$ python Tools/architect/strat_banner_sweep.py > <scratchpad>/sweep.txt 2>&1 ; echo "REAL_EXIT=$?"
REAL_EXIT=0
$ cat <scratchpad>/sweep.txt
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

The exit code is captured from the process into a variable, not read from `$?` after a pipe.

### 2. The mutant — because a CLEAN sweep is silence, and silence proves nothing

The state files were copied to a scratchpad and the ONE live suite claim mutated there. The real
record was never touched.

```
MUTANT PLANTED: live 388/388 -> 389/389 in a COPY of global.md
$ python <runner> <scratchpad>/mutstate
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the tree's total (388): global.md:28: 389/389
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the report's own succeeded count (388): global.md:28: 389/389

SWEEP FAILED -- the record contradicts itself or the tree
```

The instrument fires on a planted defect and stays quiet on the healthy path. Non-vacuity discharged.

**The standing limit, restated because it is still open and is why FINDING 1 needed a human gate:**
the sweep has no check for whether a banner's prose claims ABOUT the gate reports are true. It reads
suite counts, stamps, provenance and ownership. `global.md:106-108` is invisible to it, exactly as
the bare report stamp was on RE-GATE 1.

### 3. `Saved/AutomationReport/index.json`, opened `utf-8-sig`

```
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388   succeededWithWarnings: 0   failed: 0   notRun: 0
entry count: 388
states: Counter({'Success': 388})
NEW present: ['Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy']
OLD present: []
```

**`reportCreatedOn` is still `2026.09.04-04.47.20`.** Nothing ran that the brief did not know about.
The new name is present BY NAME and `Success`; the old name is absent BY NAME. Both directions
asserted, neither inferred from the delta.

### 4. Macro-based multiline set difference, HEAD to worktree — ONE collector, symmetric file-set rule

**The rule, stated before the numbers and applied identically to both sides:** a path counts if and
only if it starts with `Source/`, ends with `.cpp`, and contains a `/Tests/` component. Both sides
take their file list from git — `git ls-tree -r HEAD` for the HEAD side, `git ls-files --cached
--others --exclude-standard` for the worktree side — so an untracked new test file would be seen, and
the committed fossil under `Tools/architect/evidence/...` that `tests.md` records as having broken an
earlier census is excluded by the SAME predicate on both sides rather than by an ad-hoc filter on
one. One `collect()` function reads both sides.

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
the authority and read `"rulesCommit": "96d93ea9e490c8cc33261f5d9671389a9c76ecfd"` and
`"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"` — derived here, not quoted from prose.
pass.

**2. `strat::` calls.** A grep for the lowercase-initial call pattern across `Source/`, excluding
`Source/StratBridge/` and `Source/StratRules/`, returns nothing. No hit to judge this pass. pass.

**3. Vendored header before UHT.** The naive candidate grep returns 16 headers, and every one is a
FALSE POSITIVE — they mention `UCLASS`/`USTRUCT`/`UENUM` inside prose comments explaining why they
must not declare one. Judged by refining to real include directives:

```
$ grep -rnE (include directive for StratBridge|Ui|Data|Save|Scenario|Replay .h) Source/ --include=*.h
Source/StratBridge/StratBridge.h:53,55,56,57,58   (Data.h, Replay.h, Save.h, Scenario.h, Ui.h)
Source/StratRules/*.h                             (vendored, internal)
```

`StratBridge.h` is the only non-vendored header including them, and it declares no reflected type: a
line-anchored grep for `UCLASS`/`USTRUCT`/`UENUM` on it returns nothing, and it has no `generated.h`
include. Its own comments at `:262-264` and `:323` state the constraint. pass.

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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,AIModule,
                                            NavigationSystem,StateTreeModule,GameplayStateTreeModule,
                                            Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
--- ROW COUNT --- 16
```

**16 rows, all five modules present** — no module missing, so the extraction did not fail silently.
Every row matches the expected graph. The sixteenth row was checked for its guard rather than read as
an unconditional arrow, because the extraction is textual and cannot show a branch:

```
Source/StratBridge/StratBridge.Build.cs
133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
135:  if (!bCompileVendoredRulesHere)
137:      PrivateDependencyModuleNames.Add("StratRules");
144:  PublicIncludePaths.AddRange(new string[] {
```

The link edge sits inside `if (!bCompileVendoredRulesHere)` — monolithic targets only, from the one
expression that also governs the compile half. Correct and deliberate; NOT a finding. `StratRules`
appears in no `PublicDependencyModuleNames` and in no unconditional `PrivateDependencyModuleNames`.
`StratUI` has not gained a `Stratocracy` dependency. No cycle.

**Field census — 10 names, matching the recorded figure exactly:**

```
BRIDGE=                          <-- the KNOWN false positive: the tail of the string literal
PCHUsage =                           "STRAT_VENDORED_RULES_IN_BRIDGE=" inside PrivateDefinitions.Add.
PrivateDefinitions.Add               Not chased, and not filtered away either.
PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
--- FIELD COUNT --- 10
```

No eleventh name. No `DynamicallyLoadedModuleNames`, `PrivateIncludePaths` or
`PublicSystemLibraries` has appeared. pass.

**6. Module registration.** `Stratocracy.uproject` lists `Stratocracy`, `StratBridge`, `StratUI`,
`StratPlay`. `Source/StratRules/` carries no `IMPLEMENT_MODULE` — checked, not assumed — and is
correctly absent; flagging it would be the false positive that costs a startup abort. Every other
`Source/*` directory has an `IMPLEMENT_MODULE` and is registered. No new module in this diff. pass.

**7. `Source/Stratocracy/`.** `git diff --stat` prints nothing. pass.

**8. `/Game/` literals.** Every hit is under a `Tests/` directory or in
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp` — the automation-fixture and
import-commandlet exceptions, exactly the two that are standing. No gameplay C++ hit. The two
literals in the changed file, `StratShippedGameModeMatchConfig.cpp:113,117`, are fixture path
constants in a test and are pre-existing. pass.

**9. Widget arithmetic.** No `StratUI` file is in the diff; nothing added draws a number. pass.

**10. Lane discipline.** The only source change is `Source/StratPlay/Tests/...` — inside `Tests/`,
the test-author's lane. `tests.md` is the test-author's file; `global.md` is the coordinator's. No
production code inside `Tests/`, no test code outside it, no asset touched. pass.

**11. Prose blocks.** The new clause carries a why-first header block citing GDD Sec 2.11.6 and
Sec 2.9, acceptance ID `T-FAME-02`, and records its two mutants as measurements. No `.Build.cs`
changed. pass.

**12. Nothing staged or committed.** `git diff --cached --stat` is empty; `HEAD` is still `9d705ca`.
pass.

## Observations

Non-gating.

1. **`global.md:90-91`, "Gate reports name the OLD identifier", is a universal over a set that is
   still growing.** It is TRUE right now — all seven existing reports contain the retired name
   (counts 4, 1, 1, 2, 1, 8, 6 in creation order), and this one does too, so my arrival preserves it.
   I am deliberately NOT raising it as a finding: unlike `:106-108` it is not false, and its evident
   subject is the reports that already exist and must not be edited. But it is one silent future
   report away from becoming false, and it is the same construction that has now produced findings on
   four rounds. Scoping it to the reports written before the rename would retire the hazard at no
   cost in meaning.

2. **A count of the old name's distribution DOES exist in the tree, in the `.cpp`, and it is
   correct.** That file's header sends a reader to `tests.md` "from that gate report's four citations
   of it". Measured: `grep -c` on `2026-09-04-shipped-difficulty-pin.md` returns exactly `4`. It is
   true, and it is safe only because that report is closed. FINDING 1's repaired sentence scopes its
   "no count" claim to `global.md` and `tests.md`, so this third file does not falsify it — but a
   reader should know the count exists.

3. **The `autocrlf` warning is still real and still pre-existing.** `git diff` on the changed `.cpp`
   emits the LF-will-be-replaced-by-CRLF warning. The file is LF in the worktree and LF in the HEAD
   blob and this pass preserved that. Watch it at staging time; it is not a change this diff made.

4. **The sweep's bare-stamp blindness remains OPEN**, in `strat-data-steward`'s lane, and is
   correctly recorded as open in `global.md:50-57`. Until it is fixed, every report-stamp citation in
   the record must carry the literal `reportCreatedOn` token. All current citations do — re-checked
   this pass.

5. **The instrument gap that produced FINDING 1 is structural, not incidental.**
   `strat_banner_sweep.py` validates counts, stamps, provenance and ownership. It has no check for a
   banner's prose claims ABOUT the gate reports, which is the class of claim that has now failed on
   rounds three, four, five, six and seven. A sweep check that flags any live universal or count
   whose subject is `Tools/architect/gate_reports/` would have caught every one of them from the
   record side. That is a candidate for `strat-data-steward`'s backlog, not a finding here.

VERDICT: BLOCK
