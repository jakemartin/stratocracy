# Gate: Difficulty-ownership pin -- RE-GATE after the OBSERVATION 1 stamp -- strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`. Second round.
The first round's report is `Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md`
and is left BYTE-IDENTICAL by this pass -- this is a new file, not an edit.

Base verified by this seat rather than accepted from the brief:

    $ git log --oneline -1
    15bd1b0 A line count in the banner became a quoted-words pointer, ...

    $ git status --porcelain --untracked-files=all
     M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md
    ?? Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md

Four paths. The fourth is the first round's own report, untracked, left by the reviewer seat and
not by the lane. Nothing staged.

    $ git diff --numstat
    300  10  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    109  11  Tools/architect/state/global.md
    119   1  Tools/architect/state/tests.md

Round 1 recorded `3 files, 518 insertions, 22 deletions`. The total is now 528/22. The whole
delta is `+10` insertions on `tests.md` and nothing else: 300+109+119 = 528, and 300+109+109 =
518 is the only decomposition consistent with round 1's figure. One file moved, as briefed.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered (unless no module object) | pass -- no new module |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in C++ | pass -- fixture exception, unchanged bytes |
| 9 | No widget-side arithmetic | pass -- no `StratUI` change in the diff |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |

### 1 -- vendored bytes

    $ git diff --stat -- Source/StratRules Data
    (no output)

    $ git status --porcelain -- Source/ Data/
     M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp

Nothing under `Source/StratRules/` or `Data/`; neither manifest is in the diff.

### 2 -- strat:: containment

    $ grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ --include=*.cpp --include=*.h \
        | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'
    (no hits)

No candidates to judge -- neither a call nor a type name outside the two permitted directories.

### 3 -- vendored headers in front of UHT

    $ for f in $(grep -rlE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)[[:space:]]*\(' Source/ --include=*.h); do
        grep -lE '#include[[:space:]]*"(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' "$f"; done
    (no hits)

Nothing to judge this pass. Note this run anchors the macro pattern at line start, which is why
`StratBridge.h` -- whose six hits round 1 judged and dismissed as its own prose -- does not even
enter the candidate set here. Same conclusion, reached with a tighter instrument.

### 4 -- pimpl

    $ grep -rn 'TUniquePtr<[[:space:]]*FStratBridge' Source/
    Source/StratUI/StratViewModel.h:47:// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in

JUDGED, NOT FORWARDED: a comment recording the measurement, on a `//` line. Not a declaration.
Unchanged from round 1; no member changed in this diff.

### 5 -- module arrows, DERIVED FROM THE TREE

No `.Build.cs` is in the diff. The graph was derived anyway, both call forms, comments stripped:

     1  StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
     2  StratBridge  PrivateDependencyModuleNames   (none)
     3  StratBridge  PrivateDependencyModuleNames   StratRules
     4  StratBridge  PublicIncludePaths             StratBridge,StratRules
     5  StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
     6  StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
     7  StratPlay    PublicIncludePaths             StratPlay
     8  StratRules   PublicDependencyModuleNames    Core
     9  StratRules   PrivateDependencyModuleNames   (none)
    10  StratRules   PublicIncludePaths             StratRules
    11  StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
    12  StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
    13  StratUI      PublicIncludePaths             StratUI
    14  Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                                   AIModule,NavigationSystem,StateTreeModule,
                                                   GameplayStateTreeModule,Niagara,UMG,Slate,Json,
                                                   StratRules
    15  Stratocracy  PrivateDependencyModuleNames   (none)
    16  Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...

SIXTEEN rows, five modules, none missing -- so the extraction did not silently fail. Every row
matches the expected graph. `StratUI` has gained no `Stratocracy` dependency; no cycle.

Row 3 is confirmed GUARDED by READING the file, which the textual extraction cannot show --
`Source/StratBridge/StratBridge.Build.cs:133-138`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

Monolithic-only, from the one expression that also decides the compile-in. Not unconditional and
not in `PublicDependencyModuleNames`. Correct and deliberate; not a finding.

Field census, the dumb instrument, unmodified:

    BRIDGE=                             PrivateDependencyModuleNames.AddRange
    PCHUsage =                          PublicDependencyModuleNames.AddRange
    PrivateDefinitions.Add              PublicIncludePaths.AddRange
    PrivateDependencyModuleNames.Add    ShadowVariableWarningLevel =
    bCompileVendoredRulesHere =         bUseUnity =

TEN names, exactly the documented ten. `BRIDGE=` is the known string-literal tail of
`"STRAT_VENDORED_RULES_IN_BRIDGE="`, not a field. No eleventh name.

### 6, 7, 8, 9

No new `Source/<Module>/` directory; `Stratocracy.uproject` not in the diff; `Source/StratRules/`
correctly still absent from `Modules`. `git diff --stat -- Source/Stratocracy` prints nothing.
The two `/Game/` literals in the changed file are the pre-existing automation-fixture constants
and are byte-unchanged (see section A). No `StratUI` file in the diff.

### 10 -- lanes

The only file that moved since round 1 is `Tools/architect/state/tests.md`, which is
`strat-test-author`'s own record file and the one place that lane reaches outside
`Source/*/Tests/`. The entry it edits opens, inline:

> 2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file under
> `Source/StratPlay/Tests/`, plus this file -- ... base commit `15bd1b0`, UNCOMMITTED)

Acting and writing both named, inline, base commit named. No transcription clause and no
in-session clause is invoked, and neither is needed. No existing gate report was modified:

    $ git status --porcelain -- Tools/architect/gate_reports/
    ?? Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md

The one entry is the first round's report, untracked and unmodified. Every committed gate report
in that directory is byte-identical.

### 12 -- staging

    $ git diff --cached --stat
    (empty)

---

## A -- IS `Source/` BYTE-IDENTICAL TO WHAT WAS GATED?

Yes, and the pin used is stronger than a comparison against the gate.

    $ ls -la --time-style=full-iso <the four paths>
    40630  2026-09-04 12:28:46.903378400 -0400  Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    681507 2026-09-04 12:35:59.363131000 -0400  Tools/architect/state/global.md
    524266 2026-09-04 12:48:24.351962100 -0400  Tools/architect/state/tests.md
    20395  2026-09-04 12:45:55.114102600 -0400  Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md

    $ git hash-object Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    76384ce060e231b5d7b04f23aacda9485ba08711

Three independent lines, and they agree:

1. **The `.cpp` predates the suite run it is cited against.** The exported report's own
   `reportCreatedOn` is `2026.09.04-16.30.20`, and the file's mtime confirms it --
   `mtime=1788539420` = `2026-09-04 12:30:20 -0400`. The `.cpp` was last written at `12:28:46`
   local, **94 seconds before the report was produced**. So the bytes on disk now ARE the bytes
   the suite ran over. This is the pin that matters, and it does not route through round 1 at all.
2. **The `.cpp` predates round 1's report** (`12:28:46` vs `12:45:55`), so it is also the file
   round 1 read.
3. **The numstat is unchanged at 300/10**, and `git status -- Source/` returns that one path and
   nothing else. `Source/` carries no untracked file.

CONSEQUENCE: `reportCreatedOn 2026.09.04-16.30.20` and the 389 figure remain the live artifact,
and every citation of them still holds. Round 1's observations 2 and 3 -- both statements about
that `.cpp` -- are unaffected; see section E.

GAP, stated rather than glossed: round 1 recorded no blob hash for the `.cpp`, so byte-identity
with round 1 specifically rests on mtime plus numstat, not on a recorded digest. The
report-precedes-source pin in (1) is unaffected by that gap, and it is the one the live-artifact
claim actually needs. The hash is recorded above so the NEXT round has a digest to compare.

## B -- DOES THE STAMP WORK AS A STAMP?

**It is FLAT.** `Tools/architect/state/tests.md:109-118`, in full:

    109  **[STAMPED 2026-09-04: this bullet originally opened "Working tree at the end of the pass:
    110  STAMPED> `git status --porcelain --untracked-files=all` lists exactly one path,
    111  STAMPED> `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` (modified)".
    112  That was a working-tree self-measurement inside the tree it measured, and it moved its own
    113  subject: this entry was the second path, `global.md` the third, and the gate report the
    114  fourth, so a checkout read it as false. `strat-integration-reviewer` raised it as
    115  OBSERVATION 1 in `Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin.md`.
    116  The retired clause is NOT replaced with a corrected count, because any count is false again
    117  on the next write to any record file; only the half that stays true -- the named artifacts
    118  cleaned up, nothing staged, nothing committed -- is carried forward above.]**

It opens and closes at one bracket level, sits BELOW the live replacement text rather than inside
it, and is not enclosed by any other `[STAMPED` block. The second stamp added by this edit -- the
`[STAMPED 2026-09-04, SECOND TIME ...]` block retiring `reportCreatedOn 2026.09.04-04.47.20` --
is likewise a SIBLING of the stamp above it, opening after that block's closing bracket, not
nested inside it. Both satisfy "restate the correction flat".

**The retired count cannot be read as a live claim.** Every line of the quoted retired text
carries a marker: line 109 carries the `[STAMPED` bracket itself and the opening quote, and lines
110 and 111 carry the `STAMPED>` prefix. There is exactly one occurrence of the retired phrase
anywhere in the record and it is prefixed:

    $ grep -n 'lists exactly one path' Tools/architect/state/*.md
    Tools/architect/state/tests.md:110:    STAMPED> `git status --porcelain --untracked-files=all` lists exactly one path,

So a `grep -v STAMPED>` sweep, or a reader arriving mid-block by citation, lands on a marked line
rather than on a bare claim. `STAMPED>` is the established convention here -- 44 occurrences in
`global.md`, now 2 in `tests.md`.

**The sweep window -- and the honest limit on what the sweep's exit 0 vouches for.** Ran with NO
pipe; the code is the sweep's own and not a tail status:

    $ python Tools/architect/strat_banner_sweep.py > out.txt 2>&1 ; SWEEPRC=$?
    Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
      automation report: 389 entries, all Success (2026.09.04-16.30.20)
      macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
      report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
      suite claims found: 62 (1 live, 61 stamped)
      tracked items declared: 2

    SWEEP CLEAN -- no self-contradiction found
    EXITCODE=0

Of the two stamps this edit added, ONLY ONE is a shape the sweep can see, and the distinction is
worth stating because "the sweep is clean" would otherwise be read as vouching for both.

- The `[STAMPED ... SECOND TIME ...]` block IS covered. It carries a bare
  `reportCreatedOn 2026.09.04-04.47.20`, which is exactly the shape the REPORT PROVENANCE check
  collects, and an unstamped one not equal to `2026.09.04-16.30.20` would redden. It sits inside
  a `[STAMPED` bracket roughly 65 characters from the marker, far inside `_STAMP_WINDOW = 220`
  (`strat_banner_sweep.py:294-297`), so the stamp lands in the window with room to spare. The 61
  stamped / 1 live split, the live one being `global.md`'s banner, is unchanged.
- The OBSERVATION 1 stamp is NOT covered, and cannot be. The text it retires is a `git status`
  path count. It carries no `N/N`, no `reportCreatedOn`, and no tracked-item shape, so no sweep
  check ever collects it -- `SWEEP CLEAN` is silent about it in both directions. Its stamping is
  sound on the evidence above (flatness, and a marker on every line of the retired quote), which
  is a grep-checkable property, but it is NOT machine-enforced. Recorded as OBSERVATION 1 below
  rather than left to be discovered.

## C -- IS THE REPLACEMENT BULLET TRUE OF A CHECKOUT, AND DID IT TRADE ONE SELF-MEASUREMENT FOR ANOTHER?

The bullet, `tests.md:104-108`:

> **Housekeeping at the end of the pass, stated as artifacts rather than as a path count:**
> the probe file was deleted, the `Saved/AutomationReportMutant` directory was deleted, the
> mutated `.cpp` was restored (`grep -n MUTANT` returns nothing), and `Saved/` is ignored via
> `.gitignore:68`. **No untracked strays were left by this pass, and nothing was staged and
> nothing was committed.**

Each named artifact, checked:

    $ grep -n 'MUTANT' Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    (no lines; exit 1)

    $ ls -d Saved/AutomationReportMutant
    ls: cannot access 'Saved/AutomationReportMutant': No such file or directory

    $ ls Source/StratPlay/Tests/
    (43 files; no probe file, and `git status -- Source/` shows no untracked path)

    $ sed -n '68p' .gitignore
    Saved/*
    $ git check-ignore -v Saved/AutomationReport/index.json
    .gitignore:68:Saved/*   Saved/AutomationReport/index.json

    $ git diff --cached --stat
    (empty)

All four artifacts verify, and the ignore citation was checked by resolution rather than by
eyeballing the line -- `check-ignore -v` names `.gitignore:68` itself as the matching rule.

**It did NOT trade one moment-scoped self-measurement for another.** The distinction is real and
is the whole point of the rewrite. The retired sentence asserted a property of the tree AT READ
TIME (`git status` "lists exactly one path"), which the act of writing the sentence falsified.
Every replacement clause is either (a) a claim about what THE PASS did -- "the probe file was
deleted", "the directory was deleted", "was restored" -- which no later write to any record file
can turn false; or (b) a property of a path that is not itself a record file (`grep -n MUTANT`,
`.gitignore:68`), which no record write moves. Nothing in the bullet counts a growing thing from
inside it.

The one clause that reaches toward tree state, "No untracked strays were left by this pass", is
SCOPED TO THE PASS and is therefore not falsified by the untracked file that now exists -- this
seat's own round-1 gate report, left by the reviewer and not by the lane. That scoping is doing
real work and it is correct as written; a reader who drops the "by this pass" would misread it.

## D -- THE RECORD CHECKS, RE-RUN

**`tests.md` carries no live `N/N` and no phase verdict.** Every `N/N`-shaped hit, judged:

    :180   +150 / 0 / -100            a Fame delta triple, not a count
    :1281  [1/4] Compile [x64]        a UBT progress line, quoted
    :1331  5 / 0 / 0 / 5 / 0 / 0 / 5  per-side unit figures
    :1530  hp 1/12, :1531 1->4/12     hit points
    :2463  22/1 split by ID           a clause split, not a suite figure
    :3171  +150 / 0 / -100            the Fame triple again
    :3461  200/200                    a startingFame value
    :3601  1757/635/107               CR/LF byte counts
    :6053  Suite was **69/69** at that pass -- past tense, historical entry

Not one is a live suite claim, and the sweep's own classifier agrees independently: 62 suite
claims tree-wide, 1 live, and the live one is the `global.md` banner. Five `VERDICT` occurrences
in `tests.md` (`:1755, :2700, :3305, :3349, :3856`) are all either quotations of a past gate's
token or prose about one; none asserts a phase verdict for this pass.

**The `global.md` live banner, RE-MEASURED by this seat from the exported report with
`utf-8-sig`; the log was not used:**

    reportCreatedOn = 2026.09.04-16.30.20
    succeeded=389 failed=0 notRun=0 succeededWithWarnings=0
    entries: 389   states: {"Success": 389}
    mtime: 1788539420  ->  2026-09-04 12:30:20 -0400
    new clause in report BY NAME: True

The banner states `389/389`, `reportCreatedOn 2026.09.04-16.30.20`, and `+1 added, 0 removed`.
The delta was re-derived by set difference on the test MACRO with one collector run over BOTH
sides -- the worktree and the `HEAD` blobs via `git show` -- rather than accepted:

    HEAD macro names: 388
    worktree macro names: 389
    added  : Stratocracy.StratPlay.T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp
    removed: (empty)
    report names: 389
    in tree not in report: (empty)
    in report not in tree: (empty)

Name sets equal in BOTH directions between tree and report. Every figure in the banner
reproduces. `global.md` is byte-unchanged since round 1 (numstat 109/11, mtime 12:35:59).

**Sweep exit code, measured without a pipe:** `EXITCODE=0`, captured from the shell status
immediately after a redirect to a file. Full output in section B.

## E -- THE OTHER TWO OBSERVATIONS FROM ROUND 1

Both were raised against `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp`, which is
byte-unchanged this round (section A). **Neither has changed status, and neither has been
addressed -- by design, as briefed.**

- OBSERVATION 2, *the archetype is derived, not read* -- `Super->GetDefaultObject<AStratGameMode>()`
  where the prose says "ITS ARCHETYPE". Still non-gating, still sound, still worth a line only so
  a later reader does not expect a `GetArchetype()` call.
- OBSERVATION 3, *`TestEqual` on `FString` is case-insensitive on this engine*, affecting the
  reflected-versus-typed consistency guard. Still non-gating; nothing rests on it.

OBSERVATION 1 is CLOSED, on the evidence in sections B and C.

## Findings

None.

## Observations

Non-gating, recorded rather than acted on.

1. **The OBSERVATION 1 stamp is correct but not machine-enforced, and `SWEEP CLEAN` does not
   vouch for it.** A retired `git status` path count is not a shape `strat_banner_sweep.py`
   collects -- no `N/N`, no `reportCreatedOn`, no tracked item -- so the sweep would print
   `SWEEP CLEAN` whether that block were stamped, half-stamped, or absent. Its soundness here
   rests on two grep-checkable properties this report measured directly: the flat bracket at
   `tests.md:109-118`, and a marker on every line of the retired quote with one hit tree-wide.
   Worth a line because a future stamp of the same non-suite shape has no instrument behind it
   either, and the reader of a clean sweep is entitled to know which stamps it actually covered.
2. **`.gitignore:68` is a line-number citation into a file a future edit will shift.** The
   citation resolves correctly today -- `check-ignore -v` names that exact rule -- but the
   record's own standing preference is a citation a diff cannot invalidate. Quoting the rule
   `Saved/*` alongside the line would survive an insertion above it. Minor; the claim is true
   and independently re-derivable by `check-ignore`, so nothing rests on the number.
3. **Round 1 recorded no blob hash for the source file it gated.** Byte-identity across the two
   rounds therefore had to be argued from mtime and numstat rather than read off a digest. It
   happens not to matter, because the stronger pin -- the mtime of the `.cpp` preceding the
   exported report by 94 seconds -- establishes the live-artifact claim without reference to
   round 1 at all. The `git hash-object` digest `76384ce060e231b5d7b04f23aacda9485ba08711` is
   recorded in section A so a third round has something to compare against.
4. **The untracked gate report is a standing feature of re-gating this tree.** `git status` will
   show N untracked reviewer reports on the Nth round, and any record sentence that counts paths
   will be wrong by exactly that number. This is the general form of the defect the stamp just
   retired; the "by this pass" scoping in the replacement bullet is the right shape for it.

VERDICT: PASS
