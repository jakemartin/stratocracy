# Gate: shipped-`Difficulty` pin, RE-GATE 3 (banner repair) — strat-integration-reviewer

## Scope

Working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base commit
`9d705ca6d7b639312ce761379786237e2d381cd3`, UNCOMMITTED. Verified with an absolute path:

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
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
```

Three modified files, three untracked gate reports, nothing staged. No worktree under
`E:/MultiAgent/Strat-wt/` or `.claude/worktrees/` was read or gated; every measurement below was
taken with `git -C E:/MultiAgent/Stratocracy` or from an absolute path under it.

**The change since RE-GATE 2 is `global.md` only, and that is measured rather than accepted.**
mtimes, local:

```
Tools/architect/gate_reports/...-shipped-difficulty-pin.md           2026-09-04 00:29:09
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp           2026-09-04 00:44:16
Saved/AutomationReport/index.json                                    2026-09-04 00:47:20
Tools/architect/state/tests.md                                       2026-09-04 00:49:03
Tools/architect/gate_reports/...-shipped-difficulty-pin-regate.md    2026-09-04 01:14:22
Tools/architect/gate_reports/...-shipped-difficulty-pin-regate-2.md  2026-09-04 01:31:02
Tools/architect/state/global.md                                      2026-09-04 01:32:02
```

`global.md` is the only file whose mtime post-dates RE-GATE 2's report. The `.cpp`, `tests.md`
and `index.json` all pre-date it, so no code changed, no test changed and no suite ran. The three
prior reports were read and NOT edited; each one's mtime pre-dates the creation of its successor,
so no predecessor was rewritten.

Prior verdicts, read from the last line of each file:

```
$ tail -n 1 <each>
...-shipped-difficulty-pin.md            VERDICT: PASS
...-shipped-difficulty-pin-regate.md     VERDICT: BLOCK
...-shipped-difficulty-pin-regate-2.md   VERDICT: BLOCK
```

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` printed nothing for both paths |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass — zero candidates, judged rather than merely counted |
| 3 | No vendored header in front of UHT | pass — the one grep hit is `StratBridge.h` itself, and every `UCLASS`/`USTRUCT`/`UENUM` token in it is inside a comment |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — two `TPimplPtr<FStratBridge>` members; the only `TUniquePtr<FStratBridge>` string in the tree is a comment recording the measured `C4150` |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 field names, derived not typed; the 16th row confirmed guarded by reading the branch |
| 6 | New modules registered, unless no module object | pass — no new module; `StratRules` correctly absent from `Modules` and carries no `IMPLEMENT_MODULE` |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` printed nothing |
| 8 | No `/Game/` literal in C++ outside the exceptions | pass — one hit outside `Tests/`, the import commandlet, a named standing exception |
| 9 | No widget-side arithmetic | pass — no file under `Source/StratUI/` is in the diff |
| 10 | Lane discipline | pass — this pass touched `global.md` only, by the `coordinator`, on its own file |
| 11 | Prose blocks present | pass — no new file; the changed `.cpp` carries its why-first block, unchanged this pass |
| 12 | Nothing staged or committed | pass — three ` M` and three `??`, no index entries |
| R1 | The repair: contradiction resolved, new claims TRUE, `has not been re-gated` surviving only as a quotation | **BLOCK** — the quotation test passes and every named report checks out, but the repair introduced an exhaustive count that this gate falsifies. FINDING 2 |
| R2 | Exhaustive sweep of the live banner, claim by claim | **BLOCK** — FINDING 1, a live false claim about what the first gate raised |
| R3 | `strat_banner_sweep.py`, full output, real exit code, not through a pipe | pass — `SWEEP CLEAN`, exit 0, and PROVED NON-VACUOUS on this file by a positive control |
| R4 | `index.json` read `utf-8-sig`; `reportCreatedOn` unmoved; NEW name present, OLD absent | pass — `2026.09.04-04.47.20`, unchanged; 388 / 0 / 0 / 0; NEW present, OLD absent |
| R5 | Macro set difference HEAD to worktree, one collector, symmetric file-set rule stated | pass — +1 added, 0 removed |
| R6 | Rename completeness and clause-body identity | pass — zero old-name occurrences under `Source/`; class, registered path and cross-reference all renamed |
| R7 | Record checks: sole live count and verdict, inline separate attribution, old name only where deliberate, `[STAMPED` distances MEASURED | pass on all four |

## Findings

### FINDING 1 — BLOCK. The banner says the first gate "raised one observation". It raised FOUR, and the file that proves it is named in the same paragraph.

`Tools/architect/state/global.md:58-60`, live prose, unstamped:

```
THE CLAUSE WAS RENAMED AFTER THE GATE PASSED IT, ON A USER RULING, AND THE REASON IS THE ONE
GENERAL LESSON THIS PASS PRODUCED. `strat-integration-reviewer` gated the pass **PASS with zero
findings** and raised one observation: the clause was named `...ShippedGameModeAuthorsEasyDifficulty`,
```

Measured against the artifact that same paragraph goes on to name at `:77`:

```
$ awk '/^## Observations/,0' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md \
    | grep -n "^[0-9]\+\."
5:1. **THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP. ...
20:2. **The `autocrlf` warning `tests.md` reports is real and is pre-existing.** ...
24:3. **The liveness control compares STRINGS, not arrays.** ...
30:4. **Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable ...
$ tail -n 1 Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
  VERDICT: PASS      <- quoted output of that command, indented so this line is not a verdict token
```

"zero findings" is correct. "one observation" is wrong by three. The construction is a precise
count standing beside another precise count, introduced by a colon that enumerates the set as a
single member — it is not a loose "raised an observation". This record's own convention for saying
it exactly already exists at `tests.md:3679`, "with zero findings and five observations".

**This is a live claim about what was gated, which is the class this brief ordered swept, and it
is false against a file in this tree.** The reader consequence is real and is this project's
recorded "stale status line runs both ways" hazard: the other three observations — the
string-versus-array liveness comparison, the pre-existing `autocrlf` warning, and the
unfalsifiability of the in-session clause's obligation (1) — are recorded in NO state file, and
this sentence tells a future reader there was nothing else to look for.

**Severity note, stated plainly because I am changing my own call on a fact I have already
reported twice.** I raised this as a non-gating Observation on RE-GATE 1 (Observation 1) and again
on RE-GATE 2 (Observation 2), on the ground that it was imprecision rather than self-contradiction.
This brief widens the mandate — "adjudicate every live assertion", "every claim about what was
gated and when" — and it is the third pass over the same paragraph with the sentence untouched. I
am not reporting a new fact; I am reporting the same fact at the severity this brief assigns it,
and I would rather say so than let a third Observation read as a third pass.

Owner: the `coordinator`, in lane on its own file. The fix is one number, or the `tests.md:3679`
phrasing, plus a decision on whether the other three observations are recorded anywhere.

### FINDING 2 — BLOCK. The repair replaced a sentence that a later event falsified with an exhaustive COUNT that this gate falsifies on landing.

`Tools/architect/state/global.md:81-88`, live prose, unstamped:

```
                                                     **THE FIRST GATE'S `VERDICT: PASS`
DESCRIBES A TREE THIS ONE IS NO LONGER**, since a verdict describes a tree and not a phase and
the rename touched a compiled identifier. IT WAS
THEREFORE RE-GATED, TWICE, AND BOTH RE-GATES BLOCKED. Reports:
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md` and
`...-regate-2.md`, each a new file leaving its predecessors byte-identical.
```

**Every clause of it is TRUE against the tree as I audited it, and I verified each half rather
than assuming.** Both files exist; both end `VERDICT: BLOCK`; the byte-identical-predecessors
claim holds on mtimes. The contradiction RE-GATE 2 found is genuinely gone, and the retired
sentence now survives only as a quotation — measured below.

**The defect is the shape, not the content.** `TWICE`, `BOTH`, and a closed two-item report list
are an exhaustive enumeration of a set, written INSIDE the document that grows the set, by an
author who had already dispatched the set's third member. This report is mandated by the same
brief, at a path in the same directory. The moment it lands, the tree carries a banner claiming
two re-gates beside three re-gate reports, and a claim that BOTH re-gates blocked beside a
directory in which that is no longer a complete description. **A `PASS` here releases a commit, so
the committed artifact would ship the contradiction rather than merely risk it.**

This is this project's own recorded rule — never write a count of a growing thing inside it — and
it is the third instance in one pass of the class the brief sent me to sweep. The first was a
retired timestamp, the second a sentence denying a gate had run, and this one is a census of the
gates that have run. Each repair moved the defect one sentence along and kept it countable.

**The honest limit on this finding, so it is not read as stronger than it is.** The falsifying
event is the existence of this report, which I am instructed to write; had I written nothing the
sentence would have stayed true. I am reporting a foreseeable falsehood rather than a measured
one, and I say so instead of dressing it as a measurement. FINDING 1 stands on its own and would
carry this verdict without FINDING 2.

The fix is a count-free formulation that survives an arbitrary number of further gates: name the
directory and the filename prefix instead of the cardinality, or move the count into this file's
own `STAMPED>` form as an as-of claim rather than a live one. Owner: the `coordinator`, in lane on
its own file.

## The repair, audited on its own terms

**(a) Is the contradiction resolved, or restated in new words?** Resolved. The only surviving
occurrence of the retired string in the record is a quotation, and I checked WHICH it is rather
than counting hits:

```
$ grep -rn "has not been re-gated|not been re-gated|re-gated since" Tools/ Source/ .agents/
...regate-2.md:82:  ### FINDING 1 - BLOCK. The banner asserts that this tree "has not been re-gated" ...
...regate-2.md:101: been re-gated since.
...regate-2.md:104: "the tree has not been re-gated since" was TRUE when written and is FALSE now. ...
...regate-2.md:216: | new finding paragraph (:47) vs "not been re-gated since" (:83) | CONTRADICTION ...
...regate.md:312:   been re-gated since.
Tools/architect/state/global.md:87: *"the tree has not been re-gated since"* -- true when written and false the moment the first
```

`global.md:86-88` reads: "the second found THIS SENTENCE, which until now still read *"the tree
has not been re-gated since"* -- true when written and false the moment the first re-gate ran."
The string sits inside italic quotation marks, governed by "still read", in the past tense. That
is MENTION, not use. The five other occurrences are in the two prior gate reports, which are the
record of the finding and must keep it. Zero live assertions of it remain anywhere.

**(c) The `has not been re-gated` string is a quotation, not a live assertion.** Confirmed above.

**(b) Are the new claims checkable and TRUE?** Each one, measured:

| Claim, `global.md` | Verified against | Result |
|---|---|---|
| `:83` "RE-GATED, TWICE" | the two `...-regate*.md` files in `gate_reports/` | true as audited; see FINDING 2 |
| `:83` "BOTH RE-GATES BLOCKED" | last line of each file | true — `VERDICT: BLOCK` twice |
| `:84-85` the two report paths | filesystem | true — both exist, both named exactly |
| `:85` "each a new file leaving its predecessors byte-identical" | mtimes; each predecessor older than its successor's creation | true |
| `:85-86` "The first found the stale bare citation repaired above" | `...-regate.md:52`, FINDING 1 | true |
| `:95` "`strat_banner_sweep.py` was CLEAN across both" | a search for SWEEP CLEAN in all three reports | true — CLEAN at `pin.md:371`, `regate.md:570`, `regate-2.md:590` |
| `:45-46` "twelve lines above the block that stamps that very citation" | `...-regate.md:52`, which says twelve | true — a faithful restatement of the earlier measurement, not a fresh guess |
| `:77-78` `pin.md` "names the OLD identifier four times" | occurrence count, not matching-line count | true — 4 |
| `:51-52` `_CITED_REPORT_STAMP_RE` requires the literal token | the sweep source, read | true, and the fix is still unwritten |
| `:21-23` both slot branches merged; seven worktrees stand | git branch --merged master, git worktree list | true — `feat/w8-path-preview` and `feat/w8-transient-receipts` both merged; seven worktrees besides the main checkout |
| `:127-131` the authorship reader "is not written" | a search of `Source/` for FProperty::Identical and GetSuperClass | true — the two hits are unrelated (a comment in `StratGuidanceRouteClauses.cpp`, a name print in `StratGuidanceStripClauses.cpp`); no CDO-versus-super comparison exists |

The sweep-source read behind row 9, unedited:

```
_CITED_REPORT_STAMP_RE = re.compile(
    r"reportCreatedOn\s+(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2})")
```

The token is still required, so the recorded debt is accurately described as open. The whitespace
class matches a NEWLINE, which matters here and was measured rather than assumed: `global.md:41-42`
wraps between `reportCreatedOn` and its stamp, and the regex still sees it —

```
segment lines 34-46, matches of _CITED_REPORT_STAMP_RE:
    'reportCreatedOn 2026.09.04-04.47.20'
    'reportCreatedOn\n2026.09.04-04.47.20'
```

so BOTH live citations of the live figure are gated, and the banner's own instruction at `:56-57`
-- every citation in this file must carry the `reportCreatedOn` token or it is ungated, and the
repaired sentence now does -- is satisfied by both, not by one.

## The exhaustive sweep of the class — every live assertion in the current banner

`global.md:14-131`, in order. "Live" excludes the `[STAMPED` and `STAMPED>` block at `:68-75`.

| Lines | Live claim | Verdict |
|---|---|---|
| 14-19 | written by the `coordinator`, in lane, under no exception clause; the clause and test prose are `strat-test-author`'s and in `tests.md`, which that lane wrote itself; "not a transcription of anything" | TRUE — `tests.md`'s new entry opens `strat-test-author` (ACTING and WRITING; IN LANE ...); no transcription attribution anywhere |
| 20-27 | the route was a user choice; both slot branches merged; seven worktrees stand; dispatch chosen | the checkable halves are TRUE (merge state, worktree count); the choice itself is unfalsifiable from a checkout, as `CLAUDE.md` declares of its own obligation (1) |
| 28-29 | suite 388/388, every entry Success, zero failed, notRun, succeededWithWarnings | TRUE — measured below |
| 30-34 | 387 to 388, +1 added and 0 removed, one collector over both sides; NEW name present BY NAME, OLD absent BY NAME | TRUE — measured below |
| 35 | the live figure's report is `reportCreatedOn 2026.09.04-04.47.20` | TRUE, and gated by the sweep |
| 36-40 | run made by the lane in this tree after the last source change; this seat read the exported report rather than the log | consistent with mtimes — `index.json` 00:47:20 post-dates the `.cpp` at 00:44:16; the seat's own act of reading is unfalsifiable from a checkout |
| 41-43 | "the report timestamp runs ahead of local time ... which is why this entry is dated 2026-09-04" | the DATE is correct; the stated reason is vacuous on this pass — Observation 2 |
| 44-49 | the previous sentence was a blocking finding, citing a retired stamp twelve lines above its own stamp block, found by the reviewer on the re-gate and not by the sweep | TRUE — matches `...-regate.md:52` and the three CLEAN sweeps |
| 50-57 | the sweep cannot see a bare stamp; the regex requires the token; the fix is the steward's and is NOT written | TRUE — source read above, regex unchanged |
| 58-60 | the first gate "raised one observation" | **FALSE — FINDING 1** |
| 60-67 | a clause name travels alone; the user ruled rename; it is now `...RunsAtEasy` | TRUE — the rename is complete in `Source/`, measured below |
| 76-79 | `pin.md` names the old identifier four times; editing it would falsify what was gated | TRUE |
| 79-81 | "`tests.md` carries the one surviving stamp of the old name under `Tools/architect/`" | IMPRECISE, not gating — Observation 1, third consecutive pass |
| 81-82 | the first gate's PASS describes a tree this one is no longer | TRUE |
| 82-85 | re-gated TWICE, BOTH BLOCKED, two named reports, predecessors byte-identical | true as audited; **FINDING 2** on the shape |
| 85-88 | what each re-gate found; the retired sentence quoted | TRUE, and correctly a quotation |
| 89-96 | a correction that only adds leaves both claims live; neither instance was visible to any instrument; the sweep was CLEAN across both; both found by a dispatched gate | TRUE — three CLEAN sweeps on record, and both findings came from a dispatched gate |
| 97-107 | the debt is CLOSED for the value and OPEN for the authorship; `Difficulty` initialises to `Easy`, `Easy` is 0, the 2026-09-03 export found it absent; no premise block, deliberately | TRUE against the `.cpp` — the reasoning is at `:327-331` and `:353` of `StratShippedGameModeMatchConfig.cpp`, and the clause carries no premise block |
| 108-111 | it catches a re-authored `Hard`; it cannot catch a CLEARED override | TRUE by construction — the assertion is `ShippedTier == EStratDifficulty::Easy` |
| 112-118 | the liveness control is on a DIFFERENT field, `BP_StratGameMode_AiVsAi`, `(1)` against `(0,1)` | TRUE — `:421-435` resolves the AiVsAi CDO and compares the two `AiSides` descriptions with `TestNotEqual` |
| 119-126 | two mutants run, the asset never touched, built IN PLACE, probes exported to separate paths, pristine restored and the full suite re-run | consistent with `tests.md` and with the surviving `index.json`; the runs are historical and not re-executable from this checkout, and are attributed to the lane rather than claimed by this seat |
| 127-131 | closing authorship needs a super-CDO or `FProperty::Identical` reader; it is the engineer's; it is NOT written | TRUE — no such reader exists in `Source/` |

**No live sentence in the banner was found that a later edit in THIS pass falsified.** The two
findings are of different kinds: one was false when written (FINDING 1), and one is falsified by a
mandated later artifact (FINDING 2).

## Instruments, inline

### `strat_banner_sweep.py` — full output, REAL exit code, NOT through a pipe

Run with a redirection rather than a pipe, so the status read is the interpreter's own and not a
pipeline's last stage — this project has a recorded phantom-exit-zero defect from exactly that.

```
$ python Tools/architect/strat_banner_sweep.py > <scratch>/sweep.txt 2>&1
$ echo "REAL_EXIT=$?"
REAL_EXIT=0
$ cat <scratch>/sweep.txt
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

One live suite claim across all six record files, and it is in `global.md`. That is the RECORD
OWNERSHIP rule holding.

**A CLEAN sweep is silence, and silence proves nothing until the instrument is shown able to speak
on THIS file. Control and mutant, both run:**

```
CONTROL -- a byte-identical copy of global.md, swept alone
$ python Tools/architect/strat_banner_sweep.py <scratch>/global_control.md ; echo EXIT=$?
  suite claims found: 58 (1 live, 57 stamped)
SWEEP CLEAN -- no self-contradiction found
EXIT=0

MUTANT -- the same file with the LIVE report stamp advanced by ONE SECOND
          (2026.09.04-04.47.20 -> ...21), two occurrences replaced, nothing else
$ python Tools/architect/strat_banner_sweep.py <scratch>/global_mutant.md ; echo EXIT=$?
  [**REPORT PROVENANCE**] global.md:35: cites `reportCreatedOn 2026.09.04-04.47.21`, but the
  report this sweep actually opened is `reportCreatedOn 2026.09.04-04.47.20` -- an unstamped,
  unquoted citation of a DIFFERENT run is a stale claim about which report backs a live figure.
  [**REPORT PROVENANCE**] global.md:41: cites `reportCreatedOn 2026.09.04-04.47.21`, but the
  report this sweep actually opened is `reportCreatedOn 2026.09.04-04.47.20` -- ...
SWEEP FAILED -- the record contradicts itself or the tree
EXIT=1
```

The mutant fires at BOTH citation sites, including the wrapped one at `:41`, and exits 1. The
CLEAN result above is therefore a real pass and not an inert instrument. Neither copy was written
inside the repository; both live in the session scratchpad, and `global.md` itself was never opened
for writing by this gate.

### `Saved/AutomationReport/index.json`, opened `utf-8-sig`

```
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388  succeededWithWarnings: 0  failed: 0  notRun: 0
entry count: 388
state histogram: Counter({'Success': 388})
NEW name present: True
OLD name present: False
duplicate paths: []
mtime epoch: 1788497240
```

NEW is `Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy` and OLD is
`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty`; both were compared against
the list of `fullTestPath` values BY NAME, not inferred from the delta. The OLD constant is written
out in full here for the reason the brief's own correction gives — a report that checks for a
retired name must contain it, and this report's four-report family is a deliberate survival site.

**`reportCreatedOn` is still `2026.09.04-04.47.20`. Nothing ran that the brief did not know about.**
The mtime epoch `1788497240` is 2026-09-04 00:47:20 local, matching the sweep's own REPORT IDENTITY
line and the file listing in Scope.

### Macro set difference, HEAD to worktree, ONE collector, symmetric file-set rule stated

The file set is the UNION of every path matching `Source/.*/Tests/.*[.]cpp$` in HEAD
(`git ls-tree -r --name-only HEAD`) and in the worktree
(`git ls-files --cached --others --exclude-standard`). A file present on one side only is collected
as EMPTY on the other, never skipped, so the difference measures clauses rather than the collector's
reach. One regex, one collect function, both sides.

```
file-set size (union): 81 | HEAD-only: [] | WT-only: []
HEAD clauses: 387  WORKTREE clauses: 388
ADDED (1):
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
     [('Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp',
       'FStratShippedGameModeRunsAtEasyTest')]
REMOVED (0):
duplicate clause paths in worktree: {}
```

+1 added, 0 removed, as claimed. 388 agrees with the report's entry count and with the sweep's own
macro census. The pattern is matched multiline, because these macros wrap across lines and a
single-line grep returns ZERO here.

## Standing checks, re-verified rather than inherited

### Module arrows — DERIVED from the three fields, 16 rows

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

All five modules appear, so the extraction did not silently fail — an absent module would be a
finding by its own absence, not a module with no arrows. The 16th row is
`StratBridge PrivateDependencyModuleNames StratRules`, and the scan is TEXTUAL and cannot show that
it is conditional, so `StratBridge.Build.cs` was READ for the branch:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
...
PublicIncludePaths.AddRange(new string[] { "StratBridge", "StratRules" });
```

The link edge is INSIDE the guard — monolithic targets only, deliberate, and NOT a finding.
`StratRules` does not appear in `PublicDependencyModuleNames`, and does not appear
UNCONDITIONALLY in `PrivateDependencyModuleNames`. The modular branch still compiles the vendored
sources in: `Source/StratBridge/Vendored/` holds ten `*.strat.cpp` files, each opening with a
`#error` guard on `STRAT_VENDORED_RULES_IN_BRIDGE` and then compiling under
`#if STRAT_VENDORED_RULES_IN_BRIDGE`, driven by the ONE expression above. `StratUI` has NOT gained
a `Stratocracy` dependency. `StratBridge -> Stratocracy` is the deliberate row and is not reported.
No cycle.

### Field census — 10 names

```
BRIDGE=
PCHUsage =
PrivateDefinitions.Add
PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
--- FIELD COUNT --- 10
```

Ten, matching. `BRIDGE=` is the known false positive — the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` inside `PrivateDefinitions.Add`, confirmed by reading the line
rather than assumed from the name. No eleventh name: no `DynamicallyLoadedModuleNames`, no
`PrivateIncludePaths`, no `PublicSystemLibraries`.

### `.uproject` registration

```
['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']
```

`IMPLEMENT_MODULE` search per source directory:

```
StratBridge      Source/StratBridge/StratBridge.cpp
StratPlay        Source/StratPlay/StratPlay.cpp
StratRules       (none)
StratUI          Source/StratUI/StratUI.cpp
Stratocracy      Source/Stratocracy/Stratocracy.cpp
```

`StratRules` carries no module object and is correctly ABSENT from `Modules`. Flagging it would be
the recorded false positive that costs a startup abort to discover.

### `strat::` — judged, not counted

```
$ grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*(" Source/ \
    | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
(no output)
```

Zero candidates outside the two permitted directories, so there was nothing to judge this pass. The
lowercase-initial pattern is what separates a call from a type; `strat::UiSnapshot`,
`strat::SaveCommand` and `strat::GameState` appearing elsewhere are TYPES and are legal, and are not
what this pattern matches.

### Vendored bytes, and the authority for their commits

```
$ git diff --stat -- Source/StratRules Data
(no output)
$ git diff --stat -- Source/Stratocracy
(no output)
```

Nothing to gate. The hash authority remains the manifests themselves —
`Source/StratRules/StratRules.manifest.json` and `Data/StratData.manifest.json` — rather than any
commit written into prose.

### `/Game/` literals

Every hit under `Source/` is inside a `Tests/` directory except one:

```
Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54:
    const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);
```

Automation-test fixtures and the import commandlet are the two standing exceptions. No gameplay
`ConstructorHelpers` lookup exists.

### Rename completeness and clause-body identity

Old-name census, counted by OCCURRENCE and not by matching line:

```
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md           4
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md    1
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md  1
Tools/architect/state/global.md                                            2
Tools/architect/state/tests.md                                             1
Source/  (whole tree)                                                      0
Config/ and .agents/                                                       0
```

**Zero under `Source/`.** Every survival is deliberate, and every site was READ rather than counted:
`pin.md`'s four are the record of a gate on the pre-rename tree; `regate.md:609` and
`regate-2.md:632` are the OLD constant line of the inlined report-reading instrument — which is
exactly the standing brief correction about my own reports, and which this report reproduces for
the same reason; `global.md:60` is narrative and `:73` is a `STAMPED>` re-quote; `tests.md:86` is
the one full-form stamp. This report adds one more deliberate occurrence, in its own instrument.

Clause body, `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`:

```
:381  IMPLEMENT_SIMPLE_AUTOMATION_TEST(
:382     FStratShippedGameModeRunsAtEasyTest,
:383     "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
:386  bool FStratShippedGameModeRunsAtEasyTest::RunTest(...)
:396     const EStratDifficulty ShippedTier = Shipped->MatchConfig.Difficulty;
:398,:412  TestEqual(..., ShippedTier, EStratDifficulty::Easy);
:421     const AStratGameMode* const AiVsAi = ResolveCdoOrFail(*this, kAiVsAiGameModeClassPath);
:427,:435  TestNotEqual(..., Describe(Shipped->...AiSides), Describe(AiVsAi->...AiSides));
:77   the sibling clause's header cross-reference, renamed to ...RunsAtEasy
```

Class name, registered path and the in-file cross-reference all carry the new name, and the body
matches what the banner and `tests.md` say about it: the `Easy` value assertion, the
different-field liveness control against `BP_StratGameMode_AiVsAi`, and no premise block.

### `[STAMPED` distances — MEASURED in characters, not reasoned

The rule this file states about itself at `:234-242` is a CHARACTER distance, because `is_stamped`
scans `_PROVENANCE_CITATION_WINDOW` (400 characters) either side of a citation and a `[STAMPED`
bracket inside that window EXCUSES the citation. Measured:

```
line 35   reportCreatedOn 2026.09.04-04.47.20   nearest [STAMPED line 68   distance 2766 chars  outside window
line 70   reportCreatedOn 2026.09.04-04.09.26   nearest [STAMPED line 68   distance  205 chars  inside window
line 152  reportCreatedOn 2026.09.04-03.04.52   nearest [STAMPED line 151  distance   96 chars  inside window
line 223  reportCreatedOn 2026.09.03-19.47.45   nearest [STAMPED line 213  distance  784 chars  outside window
line 264  reportCreatedOn 2026.09.02-23.16.13   nearest [STAMPED line 264  distance   28 chars  inside window
line 349  reportCreatedOn 2026.09.02-21.01.14   nearest [STAMPED line 330  distance 1544 chars  outside window
```

The LIVE certifying citation at `:35` is 2766 characters from the nearest `[STAMPED` — far outside
the window — so the provenance guard is NOT excused on the one claim it exists to police. That is
confirmed independently by the mutant above, which fired on `:35`. The retired citations at `:70`
and `:152` sit correctly inside their own stamps' windows. `:223`, `:264` and `:349` are
pre-existing history, unchanged this pass, and the sweep is CLEAN on all of them.

### Record checks

- **Sole live count or verdict.** The sweep found 61 suite claims across six record files, exactly
  ONE live, and it is in `global.md`. RECORD OWNERSHIP passes.
- **Inline separate attribution in both new entries.** `global.md:16-19` — "Written by the
  `coordinator`, whose file this is, IN LANE AND UNDER NO EXCEPTION CLAUSE ... This entry is not a
  transcription of anything." `tests.md`'s new bullet opens "2026-09-04 (local),
  `strat-test-author` (ACTING and WRITING; IN LANE -- one file under `Source/*/Tests/`, plus this
  file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit `9d705ca`,
  UNCOMMITTED)". Both name acting and writing INLINE, in the entry, where a reader arriving by
  citation lands. No transcription clause and no in-session clause is invoked, and neither is needed
  — the seat wrote only its own file and the lane wrote only its own.
- **Old clause name only where deliberate.** Measured above; zero under `Source/`.
- **Nothing staged.** Confirmed in Scope.
- **Lane discipline.** This pass changed `global.md` only, whose writers are the `coordinator` and
  the steward. The `.cpp` is under `Source/*/Tests/` and `tests.md` is the test-author's; neither
  moved this pass, which is measured on mtimes rather than assumed from the brief.

## Observations

Non-gating.

1. **`global.md:79-81` still claims `tests.md` carries "the one surviving stamp of the old name
   under `Tools/architect/`", and this pass moved it further from true.** `global.md:73` is itself a
   `STAMPED>` re-quote of the old name, six lines above the sentence, and there are now THREE gate
   reports carrying it rather than the one at `:74-75` ("in the gate report", singular). The narrow
   reading — that `tests.md` holds the one `[STAMPED ...]`-bracketed stamp — remains defensible,
   which is why this stays an Observation for the third consecutive pass rather than becoming a
   finding. The distribution itself is correct and the `Source/` count is ZERO. Owner if pursued:
   the `coordinator`.

2. **`global.md:41-43`'s stated reason for its own date does not apply to this pass.** "The report
   timestamp runs ahead of local time -- `reportCreatedOn 2026.09.04-04.47.20` by the report against
   2026-09-04 locally -- which is why this entry is dated 2026-09-04." The report is 04:47 UTC and
   the local time is 00:47 on the SAME date, so nothing about the offset determines the date here;
   the sentence is inherited from the 2026-09-03 banner, where it did real work (report 09-04, local
   09-03). The date is CORRECT either way, so this is not a finding — but as written it reads as
   though the entry were dated OFF the report, which is the inverse of this project's recorded rule
   that record dates are local and logs are UTC. One clause.

3. **The debt paragraph still plants bare retired stamps that the fix it asks for will fire on.**
   `global.md:45` and `:52` both write `2026.09.04-04.09.26` without the `reportCreatedOn` token —
   deliberately, as the exemplar of the class. When `strat-data-steward` extends the citation regex
   to match a bare timestamp, those two become the new check's first false positives. Raised on
   RE-GATE 2 and unrepaired; worth carrying into the debt entry so the steward meets it in the
   design rather than in a red sweep.

4. **`global.md` and `tests.md` are CRLF in the worktree and LF in HEAD.** Measured with
   `tr -cd` and a byte count, because grep counts ZERO carriage returns on this box: `global.md`
   7528 in the worktree against 0 in HEAD, `tests.md` 5920 against 0. `git diff --numstat` does not
   see it, so this is `autocrlf` and not a content change; `tests.md` shows it too and was not
   touched this pass. Worth a glance at staging time.

5. **The `autocrlf` warning on the `.cpp` persists.** `git diff` still emits "in the working copy of
   'Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp', LF will be replaced by CRLF the
   next time Git touches it". Pre-existing, and correctly reported by the lane as not introduced by
   this diff.

6. **The three prior gate reports are untracked (`??`) and were read, not edited.** Their mtimes are
   unchanged relative to one another and each pre-dates its successor's creation. This report is a
   new fourth file beside them; none of the four is staged.

7. **`.agents/ue-project-context.md` needed no change this pass.** Its module table, its account of
   the conditional `StratRules` link edge and its `IMPLEMENT_MODULE` registration rule were checked
   against the derivation above and agree with it — including the instruction at `:40` not to derive
   that row for one link type and report the other as a violation.

8. **Three consecutive findings, all in the same paragraph, none of them visible to any instrument.**
   A retired timestamp; then a sentence denying a gate had run; now a census of the gates that have
   run. The common cause is not any of the three subjects — it is that each repair edited the
   sentence it was sent to fix and left the claims around it to rot. Two cheap conventions would
   cover it better than a regex: a repair pass re-reads the WHOLE current banner rather than the
   sentence it was dispatched for, and the banner states no cardinality for any set that later
   passes add to — name the directory and the filename prefix instead of the count.

VERDICT: BLOCK
