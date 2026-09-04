# Gate: difficulty-ownership pin, RE-GATE (round 3) -- strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base
`15bd1b030407c0404e6c02a05c646532c993e5ed`. Three modified paths, two untracked, nothing staged:

    $ git status --porcelain --untracked-files=all
     M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md
    ?? Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin-regate.md
    ?? Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md

Rounds 1 and 2 of this gate are those two untracked files; both returned `VERDICT: PASS`. The
only thing that has moved since round 2 is `Tools/architect/state/global.md`. This round runs the
full twelve and answers four questions the brief put to me directly. I did not take the brief's
framing as established: section A proves the delta by byte accounting rather than accepting it.

Digests recorded so a fourth round has something to compare against:

    $ sha1sum <the record files and both prior reports>
    926a200f38f12923f92baad3f30f44ba37452c1c  Tools/architect/state/tests.md
    7aa69c841332e76109b98f060ff7d3330f6cb749  Tools/architect/state/global.md
    83fcacc37ef97b6c7b2d50f14fb624aea665735e  Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md
    0a2544aa1ec16f2de05c0186bca40c08c5a8b369  Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin-regate.md

    $ git hash-object Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    76384ce060e231b5d7b04f23aacda9485ba08711

That last digest is EQUAL to the one round 2 recorded in its section A. `Source/` is
byte-identical to what was gated, read off a digest rather than argued from mtime -- which is
exactly what round 2's observation 3 asked the next round to be able to do.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass -- zero hits outside the two permitted directories |
| 3 | No vendored header in front of UHT | pass -- one candidate, judged and dismissed below |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` | pass -- every member site is `TPimplPtr`; the one `TUniquePtr` hit is a comment recording the measured `C4150` |
| 5 | Module arrows unchanged and correct | pass -- 16 rows, 10 field names, derived this round |
| 6 | New modules registered | pass -- no new module directory; `StratRules` correctly absent from the uproject |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in C++ | pass -- all hits are automation fixtures under `Tests/` plus the import commandlet, both standing exceptions |
| 9 | No widget-side arithmetic | pass -- no `StratUI` file is in the diff |
| 10 | Lane discipline | pass -- three files, three owners, each in its own lane |
| 11 | Prose blocks present | pass -- the added banner text is why-first house style and names its owner and its lane |
| 12 | Nothing staged or committed | pass -- `git diff --cached --stat` empty; HEAD still `15bd1b0` |

### 1 -- vendored

    $ git diff --stat -- Source/StratRules Data
    (no output)

### 2 -- cross-module `strat::` calls

    $ grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*[(]' Source/ --include=*.cpp --include=*.h |
        grep -vE '^Source/(StratBridge|StratRules)/'
    (no output; exit 1)

Zero candidates this round, so no judging was required. `Source/` is byte-identical to the tree
round 2 ran the equivalent grep over.

### 3 -- vendored headers in front of UHT

The file-level grep returns exactly one candidate, and it is a false positive:

    $ grep -rlnE 'UCLASS|USTRUCT|UENUM' Source/ --include=*.h |
        xargs grep -lE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)[.]h"'
    Source/StratBridge/StratBridge.h

    $ grep -nE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)[[:space:]]*[(]' Source/StratBridge/StratBridge.h
    (no output; exit 1)

`StratBridge.h` mentions those macro names in its prose and DECLARES none of them. It is the
bridge header itself and is precisely the file that is supposed to include the vendored headers.
The constraint governs a header that declares a reflected type; this one does not. Reported as a
judged candidate rather than as a finding.

### 4 -- bridge ownership

    $ grep -rnE 'TUniquePtr<[[:space:]]*FStratBridge' Source/
    Source/StratUI/StratViewModel.h:47:// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in

One hit, and it is a comment recording the measured defect, not a declaration. The two real
members are `StratMatchSubsystem.h:2496` and `StratScoreboardHUD.h:1235`, both `TPimplPtr`.

### 5 -- module arrows, derived this round

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

    ROWCOUNT = 16

Sixteen rows, five modules, none missing -- so the extraction spoke rather than failing silently.
Row three is the CONDITIONAL one and the extraction cannot show that it is, so it was read from
the file:

    Source/StratBridge/StratBridge.Build.cs:133  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    Source/StratBridge/StratBridge.Build.cs:134  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    Source/StratBridge/StratBridge.Build.cs:135  if (!bCompileVendoredRulesHere)
    Source/StratBridge/StratBridge.Build.cs:137      PrivateDependencyModuleNames.Add("StratRules");

The `.Add` sits inside the guard, so the link edge is monolithic-only, the compile-in is the
modular branch, and both come from the one expression. `StratRules` appears in
`PublicIncludePaths` -- a header search path, not an arrow -- and NOWHERE in
`PublicDependencyModuleNames`, and its `PrivateDependencyModuleNames` appearance is guarded. That
is the required shape; it is not drift and it is not a finding.

Field census -- TEN names, matching the pinned figure:

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

`BRIDGE=` is the known false positive -- the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` inside `PrivateDefinitions.Add`. There is no eleventh name.

### 6 -- module registration

    $ python -c "import json; print([m['Name'] for m in json.load(open('Stratocracy.uproject'))['Modules']])"
    ['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']

    $ grep -rlE 'IMPLEMENT_MODULE|IMPLEMENT_PRIMARY_GAME_MODULE' Source/
    Source/StratBridge/StratBridge.cpp
    Source/Stratocracy/Stratocracy.cpp
    Source/StratPlay/StratPlay.cpp
    Source/StratUI/StratUI.cpp

Four modules carry a module object and all four are registered. `Source/StratRules/` carries none
and is correctly absent from the array. No new module directory appears in the diff.

### 8 -- `/Game/` literals

    $ grep -rn '"/Game/' Source/ --include=*.cpp --include=*.h | grep -v '/Tests/'
    Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54:  ... TEXT("/Game/StratData/%s") ...

The single non-test hit is the import commandlet, one of the two standing exceptions. Every other
hit is under `Source/*/Tests/`, the other exception. All of them are byte-unchanged from `HEAD`
except the two fixture constants in the one modified test file, which round 1 already judged and
which have not moved since.

### 10 -- lanes

Three modified files, three owners:

- `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` -- `strat-test-author`'s lane,
  byte-identical to the digest round 2 recorded.
- `Tools/architect/state/tests.md` -- `strat-test-author`'s own record file, byte-identical to
  round 2 (`524266` bytes, mtime `12:48:24.351962100`, both equal to round 2's section A).
- `Tools/architect/state/global.md` -- the `coordinator`'s own file, whose declared writers are
  the coordinator and the steward. In lane. No exception clause is invoked in the added text and
  none is needed: it is not a transcription, it is not an in-session code write, and it does not
  reach `Content/`.

No test code outside `Tests/`, no production code inside it, no asset touched. No existing gate
report was modified; see section B.

### 12 -- staging

    $ git diff --cached --stat
    (empty)
    $ git rev-parse HEAD
    15bd1b030407c0404e6c02a05c646532c993e5ed

---

## A -- IS THE DELTA SINCE ROUND 2 REALLY ONLY THOSE TWO PARAGRAPHS?

Yes, and it is proved by byte accounting, because `git diff` against `HEAD` shows the whole
uncommitted banner and cannot isolate what moved since round 2.

Round 2's section A recorded `Tools/architect/state/global.md` at `681507` bytes. It is now
`683061`, a delta of `+1554`. The added text is lines 92-108. Measured with python, in binary:

    total 683061
    nlines 7675
    seg bytes 1555          # lines 92..108 inclusive, line endings kept
    rest   681506           # total - seg
    line endings: 7675 CRLF, 0 LF-only
    last line of seg ends:  "... mutant that unstamps it must redden.)" + CR LF
    line 91 now ends:       "... the tail's status and not the sweep's." + CR LF

The accounting closes exactly. Before the edit the banner block's closing parenthesis sat at the
end of line 91; the edit moved it to the end of the inserted text. So the pre-edit file was
`rest + 1` = `681506 + 1` = **681507**, which is precisely the size round 2 measured. There is no
residue. Nothing else in `global.md` changed -- no stamp was re-opened, no retired quote altered,
no live figure touched, no byte elsewhere in 683061 moved.

The same instrument records why the obvious measurement is wrong here. `sed -n '92,108p' | wc -c`
returns `1538`, seventeen bytes short, one per line, because this file is 100% CRLF and `sed` on
this box drops the CR. Trusting it would have produced a 17-byte phantom discrepancy somewhere
else in the file and sent me hunting an edit that does not exist.

## B -- IS THE GATE-REPORT REFERENCE TRUE AND RE-EXECUTABLE?

The paragraph, `Tools/architect/state/global.md:92-97`:

> THIS PASS WAS GATED, AND THE GATE REPORTS ARE THE FILES MATCHING
> `Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin*.md`. Each is a new file that
> leaves its predecessors byte-identical and carries its own `VERDICT:` line, its own findings and
> its own observations. No number of them is written here and none is enumerated -- that set was
> still growing while this was written, and a closed statement about a moving subject is false on
> landing, which is the lesson the banner below was re-gated into learning.

**The glob resolves, and to exactly the intended set.**

    $ ls -1 Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin*.md
    Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin-regate.md
    Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md

It does not collide with the nine `2026-09-04-shipped-difficulty-pin*` reports or the two
`2026-09-04-bare-stamp-sweep-fix*` reports sitting in the same directory: the
`difficulty-ownership-pin` stem is distinct from `shipped-difficulty-pin`. This third report is
named `2026-09-04-difficulty-ownership-pin-regate-2.md`, collides with neither existing name, and
joins the same glob.

**Verdict line, findings, observations -- each member checked, not assumed.**

    $ grep -n '^VERDICT:' <the glob>
    ...-regate.md:403:VERDICT: PASS
    ...-pin.md:389:VERDICT: PASS

    $ grep -nE '^## (Scope|Checklist|Findings|Observations)' <the glob>
    ...-pin.md:      3 Scope   23 Checklist   356 Findings   360 Observations
    ...-regate.md:   3 Scope   32 Checklist   371 Findings   375 Observations

Both carry the verdict as their last line and both carry their own findings and observations
sections. True as written.

**"leaves its predecessors byte-identical" -- I am the source, and I confirm it for the set as it
stood.** Round 1's report is `20395` bytes at mtime `12:45:55.114102600`: the identical size and
identical sub-second mtime that round 2 recorded while reading it. Round 2's report was created at
`12:57:39` and has not been rewritten. `global.md` was edited at `13:07:04`, ten minutes after the
later report was written, and `git status` lists both reports as `??` with no modification flag.
Section A independently shows that edit touched nothing but `global.md`. I preserve the property:
this report is a NEW file and neither predecessor was opened for writing.

Two limits on that confirmation, stated rather than glossed:

1. Both predecessors are UNTRACKED, so "byte-identical" rests on size plus sub-second mtime plus
   `git status`, not on a git blob comparison. The sha1s in the Scope section exist so that stops
   being true of the next round.
2. The sentence is a UNIVERSAL over a set that is still growing, so unlike a count it cannot be
   made safe by scoping -- a fourth round that edited a predecessor would falsify it
   retroactively. It survives its own arrival here, and the protocol keeping it true is the
   reviewer's own. Recorded as Observation 1, not as a finding.

**Re-executable from a checkout: not yet, and that is the real hazard.** All three reports are
untracked. A checkout of `15bd1b0` -- or any future commit that lands `global.md` without
`git add`-ing this directory -- resolves that glob to ZERO files, and the sentence becomes an
assertion about an empty set. Nothing is staged and staging is the user's call, so no constraint
is violated today; it is a commit-time hazard that no instrument in this tree checks.
Observation 2.

## C -- DOES EITHER NEW PARAGRAPH MAKE A CLOSED STATEMENT ABOUT A MOVING SUBJECT?

This is the axis the previous pass was re-gated over repeatedly, so it is checked in the three
shapes that caused those blocks: a count, an enumeration with the count implied, and a universal.

**No count.** The paragraph names no number of reports. The only digits in lines 92-97 are the
date inside the glob itself. It says this of itself, and the self-description is true.

**No enumeration.** Neither report is named. A glob is a pattern, not a list, so it admits members
that did not exist when it was written -- including this one.

**The universal is present and it survives.** "Each is a new file that leaves its predecessors
byte-identical and carries its own `VERDICT:` line, its own findings and its own observations"
quantifies over the whole growing set. This report is a new file, leaves both predecessors
byte-identical, and carries a verdict line plus its own findings and observations. **The paragraph
survives its own arrival.** That was the question, and the answer is measured rather than
asserted, because I am the member that arrived.

**No verdict is claimed, and that restraint is load-bearing.** The paragraph says "THIS PASS WAS
GATED" and stops. It does not say the pass PASSED. Had it written a verdict, this round could have
falsified it -- a verdict is exactly a closed statement about a subject that was still moving.
Grepping the added lines for `PASS` or `BLOCK` returns nothing.

**The sweep's own counts confirm no new collectable claim was added.** Run with NO pipe:

    $ python Tools/architect/strat_banner_sweep.py > <file> 2>&1 ; SWEEPRC=$?
    Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
      automation report: 389 entries, all Success (2026.09.04-16.30.20)
      macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
      report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
      suite claims found: 62 (1 live, 61 stamped)
      tracked items declared: 2

    SWEEP CLEAN -- no self-contradiction found
    EXITCODE=0

`62 (1 live, 61 stamped)` and `tracked items declared: 2` are IDENTICAL to round 2's run, so the
two added paragraphs introduced no new suite claim and no new tracked item. Notably the bare token
`reportCreatedOn` at `global.md:99`, which the carried-debt paragraph quotes as a SHAPE it is
describing, was not collected as a claim -- a census that quotes its own search token is a known
way to manufacture a finding, and the unchanged split is the evidence that this one did not.

The exit code was measured without a pipe. A control that the instrument can speak non-zero: an
earlier attempt this session redirected into an unset `TMPDIR`, and the shell returned
`EXITCODE=1` from the failed redirect with the sweep never running. So the `0` above is the
sweep's own status and not a tail's.

## D -- IS THE CARRIED-DEBT PARAGRAPH A FAIR STATEMENT OF WHAT I OBSERVED?

I am the source. Round 2's Observation 1 read:

> **The OBSERVATION 1 stamp is correct but not machine-enforced, and `SWEEP CLEAN` does not vouch
> for it.** A retired `git status` path count is not a shape `strat_banner_sweep.py` collects --
> no `N/N`, no `reportCreatedOn`, no tracked item -- so the sweep would print `SWEEP CLEAN`
> whether that block were stamped, half-stamped, or absent. Its soundness here rests on two
> grep-checkable properties this report measured directly [...]

The banner's paragraph at `global.md:98-108`, clause by clause:

- "collects claims BY SHAPE -- an `N/N`, a `reportCreatedOn`, a tracked-item line" -- FAITHFUL.
  Those are the three collector families, and the sweep's own output names them.
- "a retired claim of any other shape is never collected at all, and the sweep prints CLEAN
  whether that stamp is sound, half-written or absent" -- FAITHFUL. My sentence with
  "half-stamped" rendered "half-written"; the same claim.
- "The live example is this pass's own: the `git status` path count stamped in `tests.md`" --
  CORRECT. It is `tests.md:109-118`, the block round 1 raised and round 2 verified flat.
- "the stamp rests on a grep a reviewer ran by hand rather than on the instrument" -- FAITHFUL,
  and it is the honest half. It does not claim the stamp is unsound; it says what vouches for it,
  which is exactly the distinction I drew.
- "Raised by `strat-integration-reviewer` as a non-gating observation" -- correct attribution and
  correct severity. It does not promote my observation into a finding.
- "It is the same shape as the bare-report-stamp blind spot the banner below records as
  discharged" -- TRUE and checkable. That paragraph is at `global.md:156-171` and is stamped
  `the debt is discharged`. Both defects are "the collector's pattern does not reach this claim
  form"; calling them the same shape is fair.
- "and it is NOT fixed" -- TRUE. Nothing in this pass touched `strat_banner_sweep.py`.

**No overclaim and no understatement.** It does not say the sweep is broken, does not say the
stamp is unsound, and does not convert a non-gating observation into a finding. The one element
that is NOT mine is the closing "Whoever picks it up" sentence proposing the check to add and the
control it would need; I made no such proposal in round 2. It is not misattributed -- the
attribution sentence covers only the raising, and the proposal is offered in the coordinator's own
voice in the coordinator's own file -- so it is not a finding. Flagged as Observation 3 only so
that a later reader cannot cite it back as a reviewer recommendation it never was.

## E -- DOES THE BANNER NOW CARRY A NEW LIVE CLAIM THAT IS FALSE OR UNSTAMPED?

The record checks the brief named, re-run:

- **`389/389` live, once.** `global.md:32` carries the live figure. The only other `389/389` in
  the file is line 111, inside the `[STAMPED ... superseded ...]` block heading the retired banner.
- **`reportCreatedOn 2026.09.04-16.30.20`** at `global.md:40`, matching the sweep's own read of
  the exported report (`mtime=1788539420`). Every other bare stamp in the file sits inside a
  `[STAMPED` block or behind a `STAMPED>` prefix, and the sweep's `1 live / 61 stamped` split is
  unchanged from round 2.
- **`+1 added, 0 removed`** at `global.md:33-34`, with the new clause named in full. The sweep's
  macro census reads `IMPLEMENT_SIMPLE_AUTOMATION_TEST=389`, equal to the report's entry count,
  so the addition claim is consistent with both instruments.
- **`tests.md` carries no live `N/N` and no phase verdict.** Its three `N/N`-shaped hits are
  `200/200` (a `startingFame` value), `1757/635/107` (byte counts), and `69/69` at line 6053
  explicitly scoped "at that pass" with its own retired `reportCreatedOn` beside it. Its five
  `VERDICT` hits are all narrative references to past gate reports, not a verdict this file
  issues. Both unchanged, and the file is byte-identical to round 2.
- **Nothing new about gating, verdicts or this pass's own history is false or unstamped.** The two
  added paragraphs are the only new live claims in the file, and sections B, C and D measure every
  one of them.

Per my own round-2 observation, `SWEEP CLEAN` at exit 0 is treated here as covering less than it
appears to: it vouches for the `N/N`, the report stamp and the tracked-item lines, and it is
silent in BOTH directions about the `git status` path-count stamp in `tests.md` and about
everything in the two new paragraphs. Those were checked by hand, above.

## Findings

None.

## Observations

Non-gating.

1. **The "each ... leaves its predecessors byte-identical" clause is a universal over a growing
   set, and scoping cannot make it safe.** Unlike a count it cannot be qualified into permanence;
   it stays true only for as long as every future round honours it. It survives this round's
   arrival, which was the test put to it. Worth a line because the banner's own reasoning for
   avoiding a count does not transfer to a universal, and a later author could read the paragraph
   as having solved the moving-subject problem in general when it has solved one instance of it.
2. **The glob resolves to zero files in any checkout, because all three reports are untracked.**
   The sentence at `global.md:92-93` is true of this working tree and would become an assertion
   about an empty set if `global.md` were committed without this directory. Nothing is staged and
   staging is the user's call, so no constraint is violated today. The mitigation is a
   `git add Tools/architect/gate_reports/` in the same commit as the record; the hazard is that
   nothing checks the pairing.
3. **The carried-debt paragraph's closing proposal is the coordinator's, not the reviewer's.**
   "the check to add collects a stamped quotation and asks whether what it retires is greppable as
   live anywhere else, and the control it needs is a stamp of a shape the sweep already covers"
   was not part of round 2's Observation 1, which named the gap and stopped. The paragraph does
   not attribute it to me and is not misleading as written; recorded so a later citation cannot
   turn it into a reviewer recommendation.
4. **Byte-accounting a record file needs a CR-aware instrument on this box.** `sed` and `grep`
   here silently drop CR, so the naive line-slice under-counted the added region by exactly one
   byte per line and manufactured a 17-byte phantom discrepancy elsewhere in the file. Section A's
   python read is the form that closes. Recorded because the next round will want to do the same
   arithmetic against the digests in the Scope section, and the wrong tool yields a plausible
   false finding rather than an obvious error.
5. **Round 2's observations 1, 2 and 4 remain open and are unaffected by this edit.** Observation
   3 -- no blob hash recorded for the gated source -- is DISCHARGED: this round read
   `76384ce060e231b5d7b04f23aacda9485ba08711` off `git hash-object` and compared it directly to
   the digest round 2 recorded.

VERDICT: PASS
