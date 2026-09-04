# Gate: the shipped-GameMode `Difficulty` pin, RE-GATE after the clause rename — strat-integration-reviewer

## Scope

The UNCOMMITTED working tree of the main repository at `E:/MultiAgent/Stratocracy`, branch
`master`, over base commit `9d705ca`. Verified by absolute path rather than by cwd echo, which is
known to misreport in this project:

```
$ git -C E:/MultiAgent/Stratocracy rev-parse HEAD
9d705ca6d7b639312ce761379786237e2d381cd3
$ git -C E:/MultiAgent/Stratocracy rev-parse --abbrev-ref HEAD
master
$ git -C E:/MultiAgent/Stratocracy status --porcelain
 M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
```

THREE modified files plus the prior gate report as untracked. NOTHING STAGED. 383 insertions,
10 deletions (`146/2`, `112/7`, `125/1`). Seven worktrees exist under `E:/MultiAgent/Strat-wt/`
and `.claude/worktrees/`; none was entered and none was audited.

This is a FRESH gate. The 2026-09-04 `VERDICT: PASS` at
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md` described a tree that no
longer exists — it was read, it was NOT edited, and it is NOT carried forward as evidence for
anything here. Every check below was re-run against this tree.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing; both manifests untouched, `rulesCommit 96d93ea…`, `dataCommit c2f5860…` read from the manifests as the authority |
| 2 | No `strat::` call outside `StratBridge/`+`StratRules/` | pass — zero hits; instrument A |
| 3 | No vendored header in front of UHT | pass — the ONLY header matching the include set is `Source/StratBridge/StratBridge.h`, which declares NO `UCLASS`/`USTRUCT`/`UENUM` and includes no `.generated.h`; instrument A |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — both members are `TPimplPtr` (`StratMatchSubsystem.h:2496`, `StratScoreboardHUD.h:1235`); the sole `TUniquePtr<FStratBridge>` occurrence is prose recording the measured `C4150`. Untouched by this diff |
| 5 | Module arrows unchanged and correct | pass — DERIVED, not trusted: 16 rows, 5 modules, 10 field names, guard read for the conditional row. No `.Build.cs` in the diff; instrument B |
| 6 | New modules registered | pass — no new module; `.uproject` lists exactly `Stratocracy, StratBridge, StratUI, StratPlay`, and those are exactly the four `IMPLEMENT_MODULE`/`IMPLEMENT_PRIMARY_GAME_MODULE` sites. `Source/StratRules/` carries none and is correctly ABSENT |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — `git diff -U0 -- Source/` filtered for added `/Game/` lines is empty; the rename introduced no literal |
| 9 | No widget-side arithmetic | pass — `Source/StratUI/` untouched by this diff |
| 10 | Lane discipline | pass — one file under `Source/*/Tests/` (test-author), `tests.md` (its own record), `global.md` (the coordinator's own file). No `Content/`, no `Config/`, no production C++ |
| 11 | Prose blocks present | pass — the renamed clause's header gained a why-first block for the rename itself, citing the ruling and the general lesson; GDD Sec 2.11.6 / Sec 2.9 and T-FAME-02 still cited |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` empty; HEAD still `9d705ca` |

**A gate is not only its twelve.** The brief's rename-specific and record-specific checks are
below, and one of them is a `BLOCK`. See Findings.

## Findings

### FINDING 1 — BLOCK. A retired report stamp is cited LIVE and UNSTAMPED in the current banner, twelve lines above the block that stamps that very citation as retired.

**Constraint violated:** `global.md` is the one file that may carry the live suite citation, and a
correction that only ADDS leaves both claims live. The current banner asserts two different
answers to "which report backs this entry".

`Tools/architect/state/global.md:41`:

```
was not used. The report timestamp runs ahead of local time, 2026.09.04-04.09.26 by the report
against 2026-09-04 locally, which is why this entry is dated 2026-09-04.
```

The report on disk is `reportCreatedOn 2026.09.04-04.47.20`, which this same banner states
correctly at `:35`. `2026.09.04-04.09.26` is the run the rename's rebuild-and-re-run OVERWROTE,
and this banner says so itself at `:53-55`:

```
**[STAMPED 2026-09-04 -- the two claims below were true of this banner as first written, before
the rename above. Kept, not deleted, per this file's own convention.]**
STAMPED> "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.04-04.09.26`."
```

So `:41` is an unstamped, live sentence naming, with the definite article, a report that the same
banner declares gone twelve lines later. The pass stamped the citation it went looking for and
left the other one standing.

**AND THE SWEEP CANNOT SEE IT — measured, not reasoned.** `_CITED_REPORT_STAMP_RE`
(`strat_banner_sweep.py:443`) requires the literal token `reportCreatedOn` immediately before the
timestamp. `:41` writes the bare timestamp, so part (a) never generates a match for it. Proved by
an in-memory mutant that adds ONLY that token (instrument D2): the check goes from one governed
citation to two, and the new one FIRES. This is a guard blind spot, not a judgement call — the
tooling will not catch it next pass either.

Of the two bare-timestamp occurrences in this file that lack the token, the other
(`global.md:132`, `2026.09.04-03.04.52`) sits inside the already-superseded banner below and was
recorded as acceptable by the prior gate for that reason. `:41` is in the CURRENT banner and is
not.

**Owner:** the `coordinator` — `global.md` is its own file, in lane, no exception clause needed.
The fix is one sentence: replace the stale stamp at `:41` with `2026.09.04-04.47.20`, or move that
sentence inside the stamp block. **I do not apply it.**

## The rename, audited as a change that can be wrong

### R1. Complete and consistent — and ZERO old-name residue under `Source/`

```
grep -rn "AuthorsEasyDifficulty" Source/          -> (nothing)
grep -rn "RunsAtEasy" Source/
StratShippedGameModeMatchConfig.cpp:77  : header bullet handing `Difficulty` over
StratShippedGameModeMatchConfig.cpp:382 : FStratShippedGameModeRunsAtEasyTest,   (macro class identifier)
StratShippedGameModeMatchConfig.cpp:383 : "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
StratShippedGameModeMatchConfig.cpp:386 : bool FStratShippedGameModeRunsAtEasyTest::RunTest(...)
```

All four sites the brief names — clause string, macro class identifier, `RunTest` definition, and
the in-file comment reference — moved together. There is no half-rename.

The old name's surviving distribution is EXACTLY the deliberate one:

```
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md : 4
Tools/architect/state/tests.md                                    : 1   (line 86, inside a [STAMPED ...] block)
Tools/architect/state/global.md                                   : 2   (line 45 narrative, line 58 STAMPED> re-quote)
Source/                                                           : 0
Config/, Content/, .agents/, .github/                             : 0
```

4 / 1 / 2 / 0. The clause header's own claim that the old name is "deliberately NOT repeated here"
so that a `Source/`-scoped census "must come back empty" is TRUE as measured.

### R2. Did anything but the name move? NO — and the line arithmetic proves it

The insertion is a single contiguous comment block,
`Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:368-379` ("WHY THE NAME SAYS `RunsAt`
AND NOT `Authors`" ... "a comment would be an indistinguishable hit"), documenting the rename.
Everything below it shifted by a uniform +13 and nothing above it moved at all. Checked against
the prior report's own recorded line numbers:

| Site | prior report | this tree | delta |
|---|---|---|---|
| header bullet handing `Difficulty` over | 77 | 77 | 0 |
| `DescribeDifficulty` | 141 | 141 | 0 |
| `ResolveCdoOrFail` definition | 157 | 157 | 0 |
| "LIVENESS EVIDENCE ... DIFFERENT FIELD" | 357 | 357 | 0 |
| macro / clause string / `RunTest` | 369 / 370 / 373 | 382 / 383 / 386 | +13 |
| "THE CONTROL ... MEASURED ON A DIFFERENT FIELD" | 401 | 414 | +13 |
| assertion-message "LIVENESS CONTROL, ON A DIFFERENT FIELD" | 416 | 429 | +13 |

A uniform shift below the insertion with a zero-shift region above it is what a pure insertion
looks like, and is not what an edited assertion looks like. The clause body still holds exactly
one `TestEqual` (`ShippedTier` against `EStratDifficulty::Easy`) and exactly one `TestNotEqual`
(the `AiSides` liveness control), two `ResolveCdoOrFail` calls each guarded by a `return false;`
on null, no warn-and-continue and no skip. **No assertion, control, expectation, message or
failure path changed.** The rename is a rename.

### R3. Does `RunsAtEasy` overclaim in its own way? NO — tested, not assumed

The test applied to `Authors` was: does the name assert something the clause does not establish,
to a reader who will never open the file? Applied to `RunsAtEasy`:

- **Subject.** The name's subject is `ShippedGameMode`, the asset the clause actually reads
  (`BP_StratGameMode_C`'s CDO). It is NOT "the shipped game", so it does not reach across the
  `Config/DefaultEngine.ini` map-to-GameMode binding that the file header at `:88-90` explicitly
  disclaims.
- **Predicate.** "runs at Easy" is a claim about the configured tier taking effect at runtime. I
  checked the ROUTE rather than assuming it: `AStratGameMode::MatchConfig` is handed to
  `StartMatch` (`StratGameMode.cpp:98`), which stores it (`StratMatchSubsystem.cpp:221`,
  `ActiveConfig = Config;`), and `Difficulty` is thereafter only READ
  (`StratMatchSubsystem.cpp:317`, `StratDifficultyFameDelta(ActiveConfig.Difficulty)`). There is
  NO runtime writer of `Difficulty` anywhere in `Source/` outside `Tests/`. So a GameMode whose
  CDO carries `Easy` does run at `Easy`.
- **What it does NOT say.** Nothing about who set the tier. That is the entire defect of `Authors`,
  and `RunsAt` does not reproduce it in another form.

The name is correct. The fix landed.

### R4. The five disclaimers — still present, still accurate, NOT contradicted by the new name

| Site | Form | State |
|---|---|---|
| `StratShippedGameModeMatchConfig.cpp:357` | "THE LIVENESS EVIDENCE THEREFORE COMES FROM A DIFFERENT FIELD THAN THE ONE BEING PINNED" | present, unchanged |
| `StratShippedGameModeMatchConfig.cpp:414` | "THE CONTROL: the reader is alive -- MEASURED ON A DIFFERENT FIELD" | present, unchanged |
| `StratShippedGameModeMatchConfig.cpp:429` | inside the ASSERTION MESSAGE: "LIVENESS CONTROL, ON A DIFFERENT FIELD" | present, unchanged |
| `Tools/architect/state/tests.md` | "measured on a DIFFERENT FIELD, and that is the caveat to carry forward" | present, re-stated in the new entry |
| `Tools/architect/state/global.md` | "MEASURED ON A DIFFERENT FIELD, WHICH IS A WEAKER WARRANT AND IS LABELLED AS ONE IN EVERY PLACE IT APPEARS" | present |

The assertion message still ends "Do not cite this clause as evidence that a designer set the
tier." NOTE: that sentence WRAPS across a C++ string-literal boundary, so a grep for the whole
phrase returns ZERO on a file that contains it.

The new name STRENGTHENS these rather than contradicting them: a `RunsAt` name beside a
"not authorship" disclaimer is consistent, where an `Authors` name beside the same disclaimer was
not. The load-bearing premise was re-measured against the tree rather than inherited:
`Source/StratPlay/StratMatchSubsystem.h:436` reads
`EStratDifficulty Difficulty = EStratDifficulty::Easy;`, and `EStratDifficulty` is
`{ Easy, Normal, Hard }` with no explicit initialisers, so `Easy` is 0. The deliberate ABSENCE of
a premise block remains correct.

## The adversarial question — the collector asymmetry, verified against the tree

**The lane's account is TRUE in every particular, and I re-derived each one rather than accepting
it.**

```
git ls-tree -r --name-only HEAD | grep -E 'Tests/.*[.]cpp$' | wc -l            -> 82
git ls-tree -r --name-only HEAD | grep -E '^Source/.*Tests/.*[.]cpp$' | wc -l  -> 81
find Source -path '*/Tests/*.cpp' | wc -l                                      -> 81

git ls-tree -r --name-only HEAD | grep -E 'Tests/.*[.]cpp$' | grep -v '^Source/'
Tools/architect/evidence/02-scoreboard-widget/generated/Source/StratUI/Tests/StratScoreboardParity.cpp
```

82 against 81, and the single extra path is exactly the fossil named. It EXISTS in `HEAD` at that
path and in the worktree. It is the ONLY macro-bearing `.cpp` anywhere in `HEAD` outside `Source/`
(77 macro-bearing `.cpp` in `HEAD`, 76 of them under `Source/`).

**Is the corrected collector genuinely symmetric?** I did not verify the lane's collector; I wrote
my own with ONE function over both sides and an explicitly stated file-set rule, and checked the
PATH SETS for equality BEFORE comparing clause names — because a set difference whose two sides
enumerate different files measures its collectors. Instrument G reports
`path-set symmetric?: True`, `only in HEAD: []`, `only in WT: []`, 159 files read on each side.

**Does the asymmetry contaminate any figure already in the record?** NO, and this is measured
rather than argued. The fossil declares 10 clause names, and ALL TEN are already declared by the
live `Source/StratUI/Tests/StratScoreboardParity.cpp`:

```
fossil clause names: 10
fossil names NOT in the Source/ set: []
=> admitting the fossil would have changed the union by: 0 names
```

Because the comparison is over a SET of names, admitting the fossil to one side adds nothing to
that side's union. So the recorded `387 -> 388` and `+1 added, 0 removed` in both `global.md` and
`tests.md` are unaffected, and so is the prior gate report's `387 -> 388`. **The asymmetry was a
real defect in the instrument and it changed no figure.** The lane's claim to that effect is
correct, and `tests.md`'s instruction that "any future clause census must scope BOTH sides to
`Source/` explicitly" is the right generalisation.

## The record

### The `[STAMPED` distances — MEASURED, not reasoned (instrument C)

`Tools/architect/state/global.md` holds 85 `[STAMPED` markers. The whole current banner is ONE
paragraph, file offset 850, length 7775 characters, containing exactly one `[STAMPED` marker.

| Citation | file offset | nearest `[STAMPED` BEFORE | nearest `[STAMPED` AFTER |
|---|---|---|---|
| LIVE `reportCreatedOn 2026.09.04-04.47.20` (`:35`) | 2685 | **none** — no marker precedes it in the file or the paragraph | **1550** |
| newly-stamped `reportCreatedOn 2026.09.04-04.09.26` (`:55`) | 4440 | **205** | 4919 |
| bare `2026.09.04-04.09.26`, FINDING 1 (`:41`) | 3240 | **none** | 995 |

The live citation has NO marker within 400 characters in either direction, so it is neither
laundered by one nor stamped itself. The newly-stamped citation sits **205** characters after its
marker, inside the 400-character window (`_PROVENANCE_CITATION_WINDOW = 400`,
`strat_banner_sweep.py:313`). The brief's ~420-to-~205 reordering is confirmed against the file.

### DOES THE REORDERING ACTUALLY REACH ITS SUBJECT? — proved by mutant, not by the silence

An absence needs a control, and "the sweep is quiet" has four possible causes here: the
`stamp == report_created_on_raw` skip, the BANNER / `current_banner_end` scope gate, the
`_QUOTED_FIGURE_RE` quoting exemption, or the stamp window. I traced part (a) in memory over the
real file (instrument D1) and got, for the current banner:

```
(line 55, '2026.09.04-04.09.26', quoted=False, windowStamped=True, FIRES=False)
```

`quoted=False` is the important half: the `STAMPED> "..."` re-quote form is NOT recognised by
`_QUOTED_FIGURE_RE`, so the quoting route is NOT what silences it. Disarming ONLY the marker
(`**[STAMPED 2026-09-04 --` to `**[NOTE 2026-09-04 --`, in memory, no file written) flips it:

```
(line 55, '2026.09.04-04.09.26', quoted=False, windowStamped=False, FIRES=True)
```

**The stamp at 205 characters is the sole reason the check is quiet.** The reordering reaches its
subject. That same trace is also what exposes FINDING 1: it shows only ONE governed citation in
the current banner, because `:41` carries no `reportCreatedOn` token to match.

### Ownership of the live figure

`global.md:28` carries the only live count, `**388/388**`. The new `tests.md` entry carries NO
number and cites `reportCreatedOn 2026.09.04-04.47.20` instead, stating the rule explicitly
("never by a number -- the figure lives in `Tools/architect/state/global.md` and nowhere else").
The only three `N/N`-shaped strings anywhere in `tests.md` are `69/69` (an old, dated, historical
entry), `200/200` (starting Fame) and `1757/635/107` (byte counts) — none live, none added by this
diff. No non-`global.md` record file carries a live suite figure or a phase verdict. The sweep
agrees independently: `suite claims found: 61 (1 live, 60 stamped)`.

### Stamped claims that still read as live

One: FINDING 1. The three stamp blocks the prior gate examined are all still correctly formed
(`387/387` superseded; `2026.09.04-03.04.52` under "ONE REPORT PATH, ONE ARTIFACT"; and the
`WHAT IS STILL UNPINNED` debt stamped ADDRESSED IN PART with the authorship half carried forward
rather than closed), and the two new blocks added this pass are formed the same way.

### Attribution — inline, in both new entries

- `tests.md`: "**2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file
  under `Source/*/Tests/`, plus this file -- on `master` in the main tree
  `E:/MultiAgent/Stratocracy`, base commit `9d705ca`, UNCOMMITTED)**". Acting and writing named
  inline, base commit named, no exception clause claimed and none needed.
- `global.md`: "Written by the `coordinator`, whose file this is, IN LANE AND UNDER NO EXCEPTION
  CLAUSE -- this seat wrote no code and no clause. ... This entry is not a transcription of
  anything." Correct posture: limit (4) of the transcription clause puts `global.md` outside that
  clause entirely, and the entry does not reach for it. The banner also records that both routes
  were offered with their costs named and that the user chose dispatch. I verify that the entry
  SAYS it, in the required inline shape; I record that obligation (1) is unfalsifiable from a
  checkout, as `CLAUDE.md` itself declares, and that I checked the saying and not the happening.

### "The prior gate's PASS describes a superseded tree"

Present and accurate, `global.md:66-68`:

```
**THE GATE'S `VERDICT: PASS` THEREFORE DESCRIBES A TREE THIS ONE IS NO LONGER.** A verdict
describes a tree, not a phase; the rename touched a compiled identifier, and the tree has not
been re-gated since.
```

True as written. The accompanying decision NOT to edit the prior gate report is also correct: it
is the record of a gate on a tree that no longer exists, and editing it would falsify what was
gated. It names the old identifier four times, as measured.

## The instruments — re-run here, INLINED so a checkout can re-execute them

A persisted report citing a scratchpad path is unfalsifiable, so every instrument is present as
text. All Python path literals use forward slashes deliberately.

### Instrument A — the standing architectural greps

```bash
cd E:/MultiAgent/Stratocracy
grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*[(]' Source/ --include=*.cpp --include=*.h \
  | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'        # -> empty
for h in $(grep -rlE 'UCLASS|USTRUCT|UENUM' Source/ --include=*.h); do
  grep -qE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)[.]h"' "$h" && echo "HIT: $h"
done                                                                       # -> StratBridge.h only
grep -nE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)[[:space:]]*[(]' Source/StratBridge/StratBridge.h
grep -nE 'GENERATED_BODY|generated[.]h' Source/StratBridge/StratBridge.h    # -> both empty
git diff -U0 -- Source/ | grep '^[+].*/Game/'                              # -> empty
grep -rnE 'TUniquePtr<FStratBridge>|TPimplPtr<FStratBridge>' Source/
git diff --stat -- Source/StratRules Data                                  # -> empty
git diff --stat -- Source/Stratocracy                                      # -> empty
git diff --stat -- Source/StratUI                                          # -> empty
git status --porcelain -- Content/ Config/                                 # -> empty
git diff --cached --stat                                                   # -> empty
```

The `UCLASS|USTRUCT|UENUM` hits inside `StratBridge.h` (lines 262, 323, 486) are all PROSE
explaining why the header must declare none; there is no declaration and no `.generated.h`
include, so the check-3 "HIT" is correct-by-design and not a finding. The two `/Game/` literals
the clause reuses live in `Source/StratPlay/Tests/` under the automation-fixture exception; the
only `/Game/` literal outside a `Tests/` directory anywhere in `Source/` remains the import
commandlet. Neither is touched by this diff.

### Instrument B — the module-arrow derivation and the field census

```bash
cd E:/MultiAgent/Stratocracy
for f in Source/*/*.Build.cs; do
  m=$(basename "$f" .Build.cs)
  sed 's#//.*##' "$f" | tr '\n' ' ' |
  grep -oE '(Public|Private)(DependencyModuleNames|IncludePaths)[.](AddRange|Add)[(][^)]*[)]' |
  while read -r line; do
    vals=$(printf '%s' "$line" | grep -oE '"[A-Za-z0-9_/]+"' | tr -d '"' | paste -sd, -)
    printf '%-12s %-30s %s\n' "$m" "${line%%.Add*}" "${vals:-(none)}"
  done
done
```

16 rows, all five modules present — a module missing from this output would itself be a finding:

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
```

Row 3 is the CONDITIONAL sixteenth row. The extraction is TEXTUAL and cannot show that it is
conditional, so the file was read for the branch:

```
StratBridge.Build.cs:133  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
StratBridge.Build.cs:134  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (...));
StratBridge.Build.cs:135  if (!bCompileVendoredRulesHere)
StratBridge.Build.cs:137      PrivateDependencyModuleNames.Add("StratRules");
```

Guarded, monolithic-only, from the ONE expression, so the dependency half and the compile half
cannot drift apart. `StratRules` appears in `PublicDependencyModuleNames` NOWHERE and in
`PrivateDependencyModuleNames` ONLY inside the guard — the permitted shape, NOT a finding.
`StratUI` has not gained a `Stratocracy` dependency. No cycle. `StratBridge -> Stratocracy` is the
deliberate, correct edge. NOTHING in this diff touched any `.Build.cs`:
`git diff --stat -- 'Source/*/*.Build.cs'` is empty.

The field census, run because the three graph fields are themselves a typed subject list:

```bash
for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done |
  grep -oE '[A-Za-z]+[[:space:]]*([.]AddRange|[.]Add|=)' | sed 's/[[:space:]]*$//' | sort -u
```

TEN names, the documented set, no eleventh:

```
BRIDGE=                                <- KNOWN FALSE POSITIVE: the tail of the string literal
PCHUsage =                                "STRAT_VENDORED_RULES_IN_BRIDGE=" inside
PrivateDefinitions.Add                    PrivateDefinitions.Add. Not chased, and deliberately
PrivateDependencyModuleNames.Add          not sed-ed away either.
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
```

`PCHUsage = NoPCHs` and `bUseUnity = false` are present on `StratRules`, `StratBridge`, `StratUI`
and `StratPlay` — every module reaching the vendored headers. `Stratocracy` carries
`UseExplicitOrSharedPCHs` and does not reach them.

`.agents/ue-project-context.md` was read in full and checked against this derivation, the
conditional `StratRules` edge (its rows 40 and 113 name the
`Target.LinkType != TargetLinkType.Monolithic` expression accurately), the `IMPLEMENT_MODULE`
registration rule and the `utf-8-sig` report note. **NO drift to report in the context document
this pass.**

### Instrument C — the `[STAMPED` distance measurement

```python
import re, io
p = "E:/MultiAgent/Stratocracy/Tools/architect/state/global.md"
text = io.open(p, encoding="utf-8").read()
marks = [m.start() for m in re.finditer(re.escape("[STAMPED"), text)]
paras = re.split(r"\n\s*\n", text)
print("total [STAMPED markers in file:", len(marks))

def report(label, needle):
    print("=== " + label + " : " + needle)
    for i in [m.start() for m in re.finditer(re.escape(needle), text)]:
        b = [m for m in marks if m < i]
        a = [m for m in marks if m > i]
        print("  offset", i,
              "| nearest [STAMPED BEFORE dist:", (i - b[-1]) if b else None,
              "| AFTER dist:", (a[0] - i) if a else None)
        off = 0
        for para in paras:
            j = text.find(para, off)
            if j <= i < j + len(para):
                k = i - j
                pm = [m.start() for m in re.finditer(re.escape("[STAMPED"), para)]
                pb = [m for m in pm if m < k]
                pa = [m for m in pm if m > k]
                print("    paragraph offset", j, "len", len(para),
                      "| citation offset in para", k)
                print("    in-para [STAMPED count:", len(pm),
                      "| BEFORE dist:", (k - pb[-1]) if pb else None,
                      "| AFTER dist:", (pa[0] - k) if pa else None)
                break
            off = j + len(para)

report("LIVE citation",          "reportCreatedOn 2026.09.04-04.47.20")
report("NEWLY-STAMPED citation", "reportCreatedOn 2026.09.04-04.09.26")
report("BARE old stamp",         "2026.09.04-04.09.26")
```

```
total [STAMPED markers in file: 85
=== LIVE citation : reportCreatedOn 2026.09.04-04.47.20
  offset 2685 | nearest [STAMPED BEFORE dist: None | AFTER dist: 1550
    paragraph offset 850 len 7775 | citation offset in para 1835
    in-para [STAMPED count: 1 | BEFORE dist: None | AFTER dist: 1550
=== NEWLY-STAMPED citation : reportCreatedOn 2026.09.04-04.09.26
  offset 4440 | nearest [STAMPED BEFORE dist: 205 | AFTER dist: 4919
    paragraph offset 850 len 7775 | citation offset in para 3590
    in-para [STAMPED count: 1 | BEFORE dist: 205 | AFTER dist: None
=== BARE old stamp : 2026.09.04-04.09.26
  offset 3240 | nearest [STAMPED BEFORE dist: None | AFTER dist: 995    <- FINDING 1, line 41
  offset 4456 | nearest [STAMPED BEFORE dist: 221 | AFTER dist: 4903
```

### Instruments D1 and D2 — the part-(a) trace and its two mutants (NO FILE WAS WRITTEN)

```python
import sys, io
sys.path.insert(0, "E:/MultiAgent/Stratocracy/Tools/architect")
import strat_banner_sweep as S
text = io.open("E:/MultiAgent/Stratocracy/Tools/architect/state/global.md",
               encoding="utf-8").read()
LIVE = "2026.09.04-04.47.20"

def partA(t):
    out = []
    for start, para in S.paragraphs_with_lines(t):
        for m in S._CITED_REPORT_STAMP_RE.finditer(para):
            stamp = m.group(1)
            if stamp == LIVE:
                continue
            ln = start + para[:m.start()].count("\n")
            if not (S.section_of(t, ln) == "BANNER" and ln < S.current_banner_end(t)):
                continue
            quoted = bool(S._QUOTED_FIGURE_RE.search(S.quoting_window(para, m.start())))
            ws = max(0, m.start() - S._PROVENANCE_CITATION_WINDOW)
            we = min(len(para), m.end() + S._PROVENANCE_CITATION_WINDOW)
            window = S._mask_span(para[ws:we], m.start() - ws, m.end() - ws)
            stamped = S.is_stamped(window)
            out.append((ln, stamp, "quoted=" + str(quoted),
                        "windowStamped=" + str(stamped),
                        "FIRES=" + str(not quoted and not stamped)))
    return out

print("banner end line:", S.current_banner_end(text))
print("REAL   :", partA(text))

# D1 -- does the reordering do the work? Disarm ONLY the marker, in memory.
m1 = text.replace(
    "**[STAMPED 2026-09-04 -- the two claims below were true of this banner as first written, before",
    "**[NOTE 2026-09-04 -- the two claims below were true of this banner as first written, before", 1)
print("MUTANT1:", partA(m1))

# D2 -- is line 41 invisible only because the `reportCreatedOn` token is absent?
m2 = text.replace("runs ahead of local time, 2026.09.04-04.09.26 by the report",
                  "runs ahead of local time, reportCreatedOn 2026.09.04-04.09.26 by the report", 1)
print("MUTANT2:", partA(m2))
```

```
banner end line: 105
REAL   : [(55, '2026.09.04-04.09.26', 'quoted=False', 'windowStamped=True',  'FIRES=False')]
MUTANT1: [(55, '2026.09.04-04.09.26', 'quoted=False', 'windowStamped=False', 'FIRES=True')]
MUTANT2: [(41, '2026.09.04-04.09.26', 'FIRES=True'), (55, '2026.09.04-04.09.26', 'FIRES=False')]
```

MUTANT1 proves the stamp window is the sole cause of the silence at line 55 — the reordering
reaches its subject. MUTANT2 proves the silence at line 41 is caused by the missing
`reportCreatedOn` token and nothing else, which is FINDING 1.

### Instrument E — the banner sweep, with a REAL exit code and a liveness control

The exit code was captured into a variable BEFORE anything read the output, because `$?` after a
pipe reports the last stage and this project has a recorded phantom exit-0 from exactly that:

```bash
cd E:/MultiAgent/Stratocracy
python Tools/architect/strat_banner_sweep.py > sweep.out 2>&1
rc=$?          # captured HERE, before any pipe or read
echo "REAL EXIT CODE: $rc"
cat sweep.out
```

```
REAL EXIT CODE: 0
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

A CLEAN RESULT IS AN ABSENCE AND AN ABSENCE NEEDS A CONTROL:

```bash
python Tools/architect/strat_banner_sweep.py --self-test > st.out 2>&1
rc=$?; echo "SELF-TEST REAL EXIT CODE: $rc"    # -> 0
```

59 lines, 55 `[OK]` fixtures, ending `SELF-TEST: ALL FIXTURES CORRECT`, including the two that
bear directly on this gate:

```
[OK] a wrong citation buried in a long paragraph, far from two UNRELATED stamp markers elsewhere
     in it, still FAILS: expected FAIL, got FAIL -- REPORT PROVENANCE
[OK] the pre-fix whole-paragraph is_stamped call launders the buried citation (True) while the
     shipped windowed call does not (False), on the identical paragraph and match
```

So the CLEAN is a measurement and not a silence. **It is also, per FINDING 1, a CLEAN over a
subject this instrument cannot see; a sweep-clean banner is not the same thing as a correct
banner, and this pass is the case that separates them.**

### Instrument F — the automation report, opened `utf-8-sig`

The log undercounts by exactly one every run and was NOT used.

```python
import json, io, collections, os
p = "E:/MultiAgent/Stratocracy/Saved/AutomationReport/index.json"
d = json.load(io.open(p, encoding="utf-8-sig"))
for k in ("reportCreatedOn","succeeded","succeededWithWarnings","failed","notRun","totalDuration"):
    print(f"{k:24} {d.get(k)}")
tests = d.get("tests", [])
print("entry count (len(tests))", len(tests))
print("state histogram:", dict(collections.Counter(t.get("state") for t in tests)))
paths = [t.get("fullTestPath") for t in tests]
NEW = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy"
OLD = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty"
print("NEW name present:", NEW in paths)
print("OLD name present:", OLD in paths)
print("any path containing AuthorsEasyDifficulty:", [x for x in paths if "AuthorsEasyDifficulty" in x])
st = os.stat(p)
newest = max((os.stat(os.path.join(r, f)).st_mtime, os.path.join(r, f))
             for r, _, fs in os.walk("E:/MultiAgent/Stratocracy/Source") for f in fs)
print("report mtime", st.st_mtime, "| newest source", newest)
print("REPORT NEWER THAN SOURCE:", st.st_mtime > newest[0],
      "delta(s)", round(st.st_mtime - newest[0], 1))
```

```
reportCreatedOn          2026.09.04-04.47.20
succeeded                388
succeededWithWarnings    0
failed                   0
notRun                   0
totalDuration            4.527674674987793
entry count (len(tests)) 388
state histogram: {'Success': 388}
NEW name present: True
OLD name present: False
sibling present : True
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide | state Success | warnings 0 | errors 0
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy       | state Success | warnings 0 | errors 0
any path containing AuthorsEasyDifficulty: []
report mtime  : 1788497240.21
newest source : 1788497056.05 -> Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
REPORT NEWER THAN SOURCE: True   delta(s): 184.2
```

`succeeded` 388 equals `len(tests)` 388 equals the histogram's only bucket. The NEW name is
present BY NAME and `Success`; the OLD name is ABSENT BY NAME **and by substring**, so a partial
rename could not be hiding inside a longer path. The report postdates the last source edit by 184
seconds, so it describes THIS tree and not a previous one — which is the check that separately
rules out a stale binary reporting old clause names green.

### Instrument G — my own macro set difference, ONE collector over BOTH sides

**FILE-SET RULE, STATED EXPLICITLY AND IDENTICAL ON BOTH SIDES: a path is in scope iff it starts
with `Source/` and ends with `.cpp`. The evidence fossil under `Tools/` is therefore EXCLUDED from
both sides.** The path sets are compared for equality BEFORE any clause name is compared.

```python
import re, subprocess, io, os
REPO = "E:/MultiAgent/Stratocracy"
MACRO = re.compile(
    r"IMPLEMENT_(?:SIMPLE|COMPLEX)_AUTOMATION_TEST\s*\(\s*[A-Za-z_][A-Za-z0-9_]*\s*,\s*"
    r'(?:TEXT\s*\(\s*)?"([^"]+)"', re.S)          # re.S: the macro wraps across lines

def collect(get_text, paths):                      # ONE collector, used on BOTH sides
    out, seen = set(), 0
    for p in paths:
        t = get_text(p)
        if t is None:
            continue
        seen += 1
        out |= set(MACRO.findall(t))
    return out, seen

def git(*a):
    return subprocess.run(["git", "-C", REPO, *a],
                          capture_output=True, text=True, check=True).stdout

def in_scope(p):
    return p.startswith("Source/") and p.endswith(".cpp")

head_paths = [p for p in git("ls-tree", "-r", "--name-only", "HEAD").splitlines() if in_scope(p)]

def head_text(p):
    r = subprocess.run(["git", "-C", REPO, "show", "HEAD:" + p],
                       capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None

wt_paths = []
for root, _, files in os.walk(REPO + "/Source"):
    for f in files:
        rp = os.path.relpath(os.path.join(root, f), REPO).replace(os.sep, "/")
        if in_scope(rp):
            wt_paths.append(rp)

def wt_text(p):
    try:
        return io.open(os.path.join(REPO, p), encoding="utf-8", errors="replace").read()
    except OSError:
        return None

print("HEAD paths in scope     :", len(head_paths))
print("WORKTREE paths in scope :", len(wt_paths))
print("path-set symmetric?     :", set(head_paths) == set(wt_paths))
print("only in HEAD  :", sorted(set(head_paths) - set(wt_paths)))
print("only in WT    :", sorted(set(wt_paths) - set(head_paths)))
H, hs = collect(head_text, head_paths)
W, ws = collect(wt_text, wt_paths)
print("files read HEAD/WT      :", hs, ws)
print("HEAD clause count       :", len(H))
print("WORKTREE clause count   :", len(W))
print("ADDED   (+):", len(W - H)); [print("   +", n) for n in sorted(W - H)]
print("REMOVED (-):", len(H - W)); [print("   -", n) for n in sorted(H - W)]
print("non-vacuity (both sides > 0):", len(H) > 0 and len(W) > 0)

foss = io.open(REPO + "/Tools/architect/evidence/02-scoreboard-widget/generated"
                      "/Source/StratUI/Tests/StratScoreboardParity.cpp",
               encoding="utf-8", errors="replace").read()
F = set(MACRO.findall(foss))
print("fossil clause names:", len(F))
print("fossil names NOT in the Source/ set:", sorted(F - W))
```

```
HEAD paths in scope     : 159
WORKTREE paths in scope : 159
path-set symmetric?     : True
only in HEAD  : []
only in WT    : []
files read HEAD/WT      : 159 159
HEAD clause count       : 387
WORKTREE clause count   : 388
ADDED   (+): 1
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
REMOVED (-): 0
non-vacuity (both sides > 0): True
fossil clause names: 10
fossil names NOT in the Source/ set: []
```

**+1 added, 0 removed, 387 -> 388, and the added name is the NEW one.** `HEAD` (`9d705ca`)
predates the whole clause, so a rename presents here as a single addition rather than as an
add-plus-remove pair; the old name's absence from the exported report (instrument F) is the
separate evidence that the rename actually reached the binary. Four routes that do not share an
implementation agree on 388: this set difference, the exported report, the sweep's own macro
census, and `global.md`'s claim.

## Observations

Non-gating.

1. **`global.md:45` and `tests.md:94` both say the prior gate "raised one observation". It raised
   FOUR.** `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md` carries four
   numbered observations at lines 508, 523, 527 and 533; the naming one was number 1. The
   convention for stating this precisely already exists in these files — `tests.md:3679` reads
   "with zero findings and five observations". The other three (the `autocrlf` warning, the
   string-versus-array comparison in the liveness control, and the unfalsifiability of the
   in-session clause's obligation (1)) are now recorded nowhere in the state files, so a reader of
   the record would not know they were raised. Owner if pursued: the `coordinator` for
   `global.md`, `strat-test-author` for `tests.md`.
2. **`tests.md` says "The two mutants recorded below" and they are recorded ABOVE.** The
   "Falsifiability: two mutants RUN" bullet precedes the "THE CLAUSE WAS RENAMED" bullet whose
   sub-bullet refers back to it. A reader following "below" finds nothing. One word.
3. **`global.md` claims `tests.md` carries "the one surviving stamp of the old name under
   `Tools/architect/`".** `global.md` itself carries two occurrences of the old name — `:58` inside
   a `STAMPED>` re-quote, which is defensible, and `:45` in ordinary narrative prose, which is not
   a stamp at all. The claim is imprecise about its own file. The overall distribution is still
   exactly the intended 4 / 1 / 2 / 0 with ZERO under `Source/`.
4. **The assertion message says "the shipped game runs at Easy" where the clause name correctly
   says "ShippedGameMode".** The message is prefixed "Sec 2.11.6:" and is quoting the GDD's own
   sentence, and the file header at `:88-90` separately disclaims the map-to-GameMode binding, so
   this is attributed rather than asserted and is not a finding. Recorded only because the gap
   between "the shipped GameMode" and "the shipped game" is a single unguarded line in
   `Config/DefaultEngine.ini`, and the assertion message is the artifact a reader meets on a red.
5. **A phrase grep for "designer set the tier" returns ZERO on the file that contains that
   sentence,** because it wraps across a C++ string-literal boundary. Anyone verifying the five
   disclaimers by phrase grep will conclude one is missing. Same class as this project's recorded
   wrapped-prose trap; strip the literal boundaries before searching.
6. **The `autocrlf` warning on the `.cpp` persists** and is correctly reported by the lane as
   pre-existing rather than introduced. `git diff` still emits "LF will be replaced by CRLF the
   next time Git touches it". Worth watching at staging time; not a change this diff made.
7. **`.agents/ue-project-context.md` needed no change this pass.** Read in full and checked against
   the derived module graph, the conditional `StratRules` edge, the `IMPLEMENT_MODULE`
   registration rule and the `utf-8-sig` report note. No drift found.
8. **The prior gate report is untracked (`??`) rather than modified.** It was read and NOT edited,
   as instructed. This new report is a new file beside it; neither is staged.

VERDICT: BLOCK
