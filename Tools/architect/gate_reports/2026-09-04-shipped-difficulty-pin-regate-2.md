# Gate: the shipped-GameMode `Difficulty` pin, RE-GATE 2 after the banner repair — strat-integration-reviewer

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
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
```

THREE modified files plus the two prior gate reports as untracked. NOTHING STAGED. Seven
worktrees exist under `E:/MultiAgent/Strat-wt/` and `.claude/worktrees/`; none was entered and
none was audited.

Both prior reports were READ and NOT EDITED. Neither is carried forward as evidence: every check
below was re-run against this tree.

### The brief's "only `global.md` changed" claim — CORROBORATED, not accepted

The brief asserts one file moved since the last gate. I did not take it; two independent
measurements agree, and I state the limit of each.

```
$ git diff --numstat
146     2       Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
127     7       Tools/architect/state/global.md
125     1       Tools/architect/state/tests.md
```

The prior report recorded `146/2`, `112/7`, `125/1`. Two of the three are IDENTICAL; `global.md`
grew by 15 insertions with its deletion count UNCHANGED at 7 — the shape of a pure addition plus
an in-place edit of a line that was already an addition, which is what the described repair is.

```
mtime                 local time            sha256[:16]        path
1788497056.05  2026-09-04 00:44:16  6ebf03a9578f8ff5  Source/.../StratShippedGameModeMatchConfig.cpp
1788498933.37  2026-09-04 01:15:33  1c3f828f589d53a7  Tools/architect/state/global.md
1788497343.12  2026-09-04 00:49:03  e53d27b73f2338bc  Tools/architect/state/tests.md
1788498862.59  2026-09-04 01:14:22  6965e11cc58cc171  .../2026-09-04-shipped-difficulty-pin-regate.md
1788497240.21  2026-09-04 00:47:20  bb0f2ebec2600157  Saved/AutomationReport/index.json
```

The prior gate report was written at 01:14:22. Exactly ONE file in the tree has an mtime after it:
`global.md` at 01:15:33. The `.cpp` mtime `1788497056.05` is the same value the prior report
independently recorded as "newest source". **LIMIT: an mtime is falsifiable by a rewrite that
restores it, so this corroborates rather than proves.** It is not load-bearing — the finding below
was re-derived from the file, not inherited.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing, and `git status --porcelain` over the same paths printed nothing. Both manifests untouched; `rulesCommit`/`dataCommit` read from the manifests as the authority |
| 2 | No `strat::` call outside `StratBridge/`+`StratRules/` | pass — zero hits; instrument A |
| 3 | No vendored header in front of UHT | pass — the ONLY header matching the include set is `Source/StratBridge/StratBridge.h`, which declares NO `UCLASS`/`USTRUCT`/`UENUM` and includes no `.generated.h`. Judged, not forwarded: that file IS the bridge header, and the constraint governs UHT-declaring headers |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — both members are `TPimplPtr` (`StratMatchSubsystem.h:2496`, `StratScoreboardHUD.h:1235`); the sole `TUniquePtr<FStratBridge>` occurrence (`StratViewModel.h:47`) is prose recording the measured `C4150`. Untouched by this diff |
| 5 | Module arrows unchanged and correct | pass — DERIVED, not trusted: 16 rows, 5 modules, 10 field names, guard read for the conditional row. No `.Build.cs` in the diff; instrument B |
| 6 | New modules registered | pass — no new module; `.uproject` lists exactly `Stratocracy, StratBridge, StratUI, StratPlay`, and those are exactly the four `IMPLEMENT_MODULE` sites. `Source/StratRules/` carries none and is correctly ABSENT |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — `git diff -U0 -- Source/` filtered for added `/Game/` lines is empty |
| 9 | No widget-side arithmetic | pass — `Source/StratUI/` untouched by this diff (`git diff --stat -- Source/StratUI` empty) |
| 10 | Lane discipline | pass — one file under `Source/*/Tests/` (test-author), `tests.md` (its own record), `global.md` (the coordinator's own file). No `Content/`, no `Config/`, no production C++ |
| 11 | Prose blocks present | pass — the two added paragraphs are why-first, name their owner, and record the guard blind spot as a measurement rather than an opinion |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` empty; HEAD still `9d705ca` |

**A gate is not only its twelve.** The brief's repair-specific, generalisation and record checks
follow, and one of them is a `BLOCK`. See Findings.

## Findings

### FINDING 1 — BLOCK. The banner asserts that this tree "has not been re-gated" thirty-six lines below its own account of the re-gate that blocked it.

**Constraint violated:** the same one FINDING 1 of the last gate violated — a correction that only
ADDS leaves the prior claim live, and `global.md` must not carry two answers to one question. The
repair fixed the citation it was sent to fix and did not revisit the sentence its own new
paragraph falsified.

The repair added, at `Tools/architect/state/global.md:47-48`:

```
Found by `strat-integration-reviewer` on the 2026-09-04 RE-GATE, not by the sweep, and the
reason the sweep could not find it is itself the debt below.
```

The banner already said, and still says, at `Tools/architect/state/global.md:81-83`:

```
trail. **THE GATE'S `VERDICT: PASS` THEREFORE DESCRIBES A TREE THIS ONE IS NO LONGER.** A verdict
describes a tree, not a phase; the rename touched a compiled identifier, and the tree has not
been re-gated since.
```

"the tree has not been re-gated since" was TRUE when written and is FALSE now. Two re-gates have
run over the post-rename tree, and one of them produced the blocking finding that this very repair
answers. The banner therefore asserts both "a re-gate found this" (`:47`) and "there has been no
re-gate" (`:83`).

**THE SENTENCE IS LIVE AND UNSTAMPED — measured, not assumed.** The only `[STAMPED` marker in the
current banner opens at `:68`, and its scope is the six `STAMPED>`-prefixed lines `:70-75`,
verified with `cat -A`. Line `:76` onward carries no prefix:

```
STAMPED>  deliberately in `tests.md`'s own stamp and in the gate report, see below.
THE RENAME POST-DATES THE GATE, AND THE GATE REPORT WAS DELIBERATELY NOT EDITED.
```

So `:81-83` is ordinary live narrative inside the current banner, not history.

**AND NO INSTRUMENT IN THIS TREE CAN SEE IT.** `strat_banner_sweep.py` governs suite figures,
report-stamp citations, provenance paths and record ownership. A prose claim about whether a gate
has run is none of those. The sweep is CLEAN over this tree (exit 0, verified below) and this
contradiction sits inside the region it just swept — the same relationship the repair's own new
debt paragraph describes for bare timestamps, in a second and unrelated dimension.

**Why this gates rather than sitting under Observations.** The sentence's function is to tell a
reader what is still owed. A reader arriving at `:83` — and a reader arriving by citation lands on
the sentence, not on the paragraph thirty-six lines above it — concludes that a gate is outstanding
on a tree that has now been gated twice. The brief states that a PASS here releases a commit and a
push; this sentence would be pushed asserting the opposite of the record beside it.

**Owner:** the `coordinator` — `global.md` is its own file, in lane, no exception clause needed.
The fix is one clause: stamp `:81-83` in the file's own idiom, or amend the final clause to record
the two re-gates and their verdicts. **I do not apply it.**

## 1. The repair itself — re-derived, not assumed to have landed

### 1a. Is the stale claim gone, or merely moved?

The prior finding's subject was `global.md:41`, which read:

```
was not used. The report timestamp runs ahead of local time, 2026.09.04-04.09.26 by the report
against 2026-09-04 locally, which is why this entry is dated 2026-09-04.
```

That sentence now reads, at `:41-43`:

```
was not used. The report timestamp runs ahead of local time -- `reportCreatedOn
2026.09.04-04.47.20` by the report against 2026-09-04 locally -- which is why this entry is
dated 2026-09-04.
```

The value is now the LIVE report and the `reportCreatedOn` token is present. I did not stop at
reading it: I re-ran the census that produced the finding (instrument H) over every
report-stamp-shaped string in all six state files, so a claim that had MOVED rather than
disappeared would have surfaced somewhere else. It did not. The two remaining occurrences of
`2026.09.04-04.09.26` outside the stamp block are adjudicated in section 2.

### 1b. Is the repaired citation genuinely visible to the check? — PROVED BY MUTANT

This is the check that matters, because adding the token is not the same as being gated by it, and
because the repaired sentence WRAPS between `reportCreatedOn` and the timestamp — this project has
a recorded trap where wrapped prose is invisible to a search a reader would expect to find it.

Derived from the tree rather than from my prior report, `strat_banner_sweep.py:442`:

```python
_CITED_REPORT_STAMP_RE = re.compile(
    r"reportCreatedOn\s+(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2})")
```

The `\s+` tolerates the newline. MEASURED rather than reasoned — the raw match at line 41 carries
the wrap inside it:

```
  line 35 stamp 2026.09.04-04.47.20 | matched text repr: 'reportCreatedOn 2026.09.04-04.47.20'
  line 41 stamp 2026.09.04-04.47.20 | matched text repr: 'reportCreatedOn\n2026.09.04-04.47.20'
  line 70 stamp 2026.09.04-04.09.26 | matched text repr: 'reportCreatedOn 2026.09.04-04.09.26'
```

A correct live citation is SKIPPED by part (a) by design (`if stamp == result.report_created_on_raw:
continue`, `strat_banner_sweep.py:1352`), so its silence proves nothing on its own. **An absence
needs a control.** Two in-memory mutants, no file written:

```
REAL    : [(70, '2026.09.04-04.09.26', 'quoted=False', 'windowStamped=True', 'FIRES=False')]

MUTANT3 (wrong ts in repaired sentence):
          [(41, '2026.09.04-01.11.11', 'quoted=False', 'windowStamped=False', 'FIRES=True'),
           (70, '2026.09.04-04.09.26', 'quoted=False', 'windowStamped=True',  'FIRES=False')]

MUTANT4 (token removed, wrong ts):
          [(70, '2026.09.04-04.09.26', 'quoted=False', 'windowStamped=True',  'FIRES=False')]
```

**MUTANT3 changes ONLY the timestamp in the repaired sentence to a wrong value, and the check
FIRES.** The citation is genuinely gated — the token was not merely added, it took effect.
**MUTANT4 is the negative control:** the same wrong timestamp with the token removed again does
NOT fire, reproducing the original blind spot on demand and confirming the debt paragraph's claim
is true rather than plausible.

### 1c. Does the banner contradict itself anywhere else? — every pair checked

| Pair | Result |
|---|---|
| live figure `388/388` (`:28`) vs the report | agree — `succeeded=388`, `len(tests)=388`, histogram `{Success: 388}` |
| live figure vs macro census | agree — sweep census 388; my independent set difference 388 |
| live report `:35` vs live report `:41` | agree — both `2026.09.04-04.47.20`, both live tense. **This is the repaired pair** |
| live report `:35`/`:41` vs STAMPED retired report `:70` | no contradiction — `:70` is inside the stamp block and reads as history |
| live clause name `:34` vs STAMPED retired name `:73` | no contradiction — `:73` is prefixed `STAMPED>` and labelled "before the rename" |
| `+1 added, 0 removed` (`:30-31`) vs the set difference | agree — `+1 / 0`, and the added name is the NEW one |
| new finding paragraph (`:44-49`) vs the repaired sentence | consistent — it describes the defect the repair removed |
| new debt paragraph (`:50-57`) vs the tree | TRUE — its closing claim that the repaired sentence "now does" carry the token is confirmed by the raw regex hit at line 41 |
| new finding paragraph (`:47`) vs "not been re-gated since" (`:83`) | **CONTRADICTION — FINDING 1** |
| `PASS with zero findings ... one observation` (`:59-60`) vs `:44-49` | no contradiction — these describe two different gates and the text distinguishes them |

### 1d. The stamp-marker distances — MEASURED, not reasoned (instrument C)

`global.md` holds 85 `[STAMPED` markers. The current banner is one paragraph, file offset 850,
length 8991 characters, containing exactly one.

| Citation | line | offset | nearest `[STAMPED` BEFORE | AFTER |
|---|---|---|---|---|
| LIVE `reportCreatedOn 2026.09.04-04.47.20` | 35 | 2685 | **none** | 2766 |
| LIVE `reportCreatedOn` + wrap + `04.47.20` (REPAIRED) | 41 | 3243 | **none** | 2208 |
| STAMPED `reportCreatedOn 2026.09.04-04.09.26` | 70 | 5656 | **205** | 4919 |
| bare `2026.09.04-04.09.26` (finding recount) | 45 | 3455 | none | 1996 |
| bare `2026.09.04-04.09.26` (debt exemplar) | 52 | 4028 | none | 1423 |

Both live citations sit with NO `[STAMPED` marker within `_PROVENANCE_CITATION_WINDOW` (400,
`strat_banner_sweep.py:313`) in either direction, so neither is laundered by a marker nor stamped
itself — correct for a live claim. The retired citation sits **205** characters after its marker,
inside the window — correct for history. **NOTE, so the two numbers are not confused:** `is_stamped`
counts a bare `reportCreatedOn <date>` as a marker in its own right, which is why `:45` scores
`windowStamped=True` against the repaired citation 212 characters above it while showing no literal
`[STAMPED` within 400. Both measurements are true, of different marker sets.

## 2. THE GENERALISATION — every report-stamp-shaped string in the six state files, classified

The finding's class: **a report stamp written WITHOUT the `reportCreatedOn` token is ungated
everywhere it appears.** Shape swept: a `YYYY.MM.DD-HH.MM.SS` group, over all six files, with no
exclusions. The instrument is inlined in section 4.

```
TOTAL report-stamp-shaped strings across the six files: 169

  file            total   gated(token)   ungated
  content.md          2          0            2
  data.md            10          5            5
  decisions.md        1          1            0
  engine.md          27         14           13
  global.md          83         65           18
  tests.md           46         28           18
  ------------------------------------------------
  TOTAL             169        113           56
```

**113 gated, 56 ungated.** The brief's "~20 bare timestamps in `global.md`" is close: the measured
figure is 18. The 56 ungated strings classify as follows.

| Class | Count | Verdict |
|---|---|---|
| STAMPED-CONTEXT — inside a `[STAMPED`/`STAMPED>` line or a stamped window | 12 | deliberately historical, correct |
| SUPERSEDED BANNER — in `global.md` below `current_banner_end` (line 120) | 8 | history by position; part (a) scopes itself out of these by design |
| DATED HISTORICAL ENTRY — non-`global.md`, per-entry run provenance | 34 | correct idiom, adjudicated below |
| **UNGATED LIVE CLAIM** | **0** | — |
| flagged by the classifier, adjudicated NOT a claim (use/mention) | 2 | see below |

**The 34 dated-entry stamps, adjudicated rather than counted.** These are the established
per-entry-provenance idiom: a dated entry naming the run that backed IT. Read in place, they are
uniformly of the shape `- **2026-08-29 (local; the run that backs this entry stamps UTC
2026.08.30-03.48.37 ...` (`tests.md:2552`) or `Report 2026.09.02-23.00.35.` (`engine.md:1009`, a
mutant probe's own report path inside a dated entry). None is tensed as current. The one that most
needed checking, `engine.md:2455`, is the strongest case AGAINST a finding — it explicitly refuses
to restate the figure:

```
**The suite figure is `global.md`'s to state and is not restated here**; the run this lane
read was exported to `Saved/AutomationReport/index.json` (`utf-8-sig`) with
`reportCreatedOn` **`2026.09.01-03.15.35`**
```

Two of the 34 are not report stamps at all: `content.md:1846-1847` cite Unreal LOG-LINE stamps
(`[2026.08.24-15.57.47:040]`), a different artifact with a different shape that the census matched
on shape alone. Counted here for honesty about what the instrument matched; neither is a report
citation.

**The two the classifier flagged, and why neither is a finding — USE versus MENTION.**

- `global.md:45`: "`2026.09.04-04.09.26` -- retired by the rename re-run -- LIVE and UNSTAMPED".
  The stamp is the SUBJECT of a claim that it is retired. It asserts the opposite of a live
  citation.
- `global.md:52`: "so `2026.09.04-04.09.26` written on its own is invisible to REPORT PROVENANCE".
  The stamp is quoted as the EXEMPLAR of the defect class. The predicate is a property of the
  string, not a claim about which report backs the live figure.

Neither uses the definite article to name the backing report, which is exactly what `:41` did and
what made it a finding. **The distinction is not a courtesy: the test I applied is whether the
sentence would become FALSE if the report on disk changed. Both of these stay true; the old `:41`
became false.** Not findings.

**So: zero fresh instances of the class.** The one instance found last pass was the only one, and
the debt is correctly recorded as OPEN rather than closed.

## 3. Everything a gate covers regardless — re-verified, not inherited

### 3a. Module arrows — DERIVED (instrument B)

16 rows across 5 modules, matching the required graph row for row:

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

All five modules are present, so the extraction did not fail silently — an empty result is the one
thing that shape cannot distinguish from a clean one. **The sixteenth row is the conditional one
and the extraction cannot show that**, so the guard was read directly,
`Source/StratBridge/StratBridge.Build.cs:133-137`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
```

One expression drives both halves, so the dependency and the compile cannot drift apart. The name
appears in `PrivateDependencyModuleNames` ONLY inside the guard and NOT in
`PublicDependencyModuleNames`, which is the exact shape the checklist permits. **No drift, and no
finding.** `StratUI` has gained no `Stratocracy` dependency; no cycle. `git diff --name-only --
'Source/*/*.Build.cs'` is empty, so nothing here moved this pass.

Field census — **TEN names, as required, with no eleventh:**

```
BRIDGE=                              <- known FALSE POSITIVE: the tail of the string literal
PCHUsage =                              "STRAT_VENDORED_RULES_IN_BRIDGE=" inside PrivateDefinitions.Add.
PrivateDefinitions.Add                  Not chased, and not filtered away either -- narrowing the
PrivateDependencyModuleNames.Add        census to spare it is how it would stop seeing the next
PrivateDependencyModuleNames.AddRange   real field.
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
```

### 3b. Module registration

`Stratocracy.uproject` `Modules` is `['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']`, and
the `IMPLEMENT_MODULE`/`IMPLEMENT_PRIMARY_GAME_MODULE` sites are exactly those four files.
`Source/StratRules/` carries none and is correctly ABSENT — flagging it would cost a startup abort
to discover.

### 3c. The rename's completeness — spot-checked, undisturbed

```
grep -rn "AuthorsEasyDifficulty" Source/ Config/ Content/ .agents/ .github/   -> (nothing)
grep -rn "AuthorsEasyDifficulty" Source/ | wc -l                             -> 0

grep -rn "RunsAtEasy" Source/
StratShippedGameModeMatchConfig.cpp:77  : header bullet handing `Difficulty` over
StratShippedGameModeMatchConfig.cpp:382 : FStratShippedGameModeRunsAtEasyTest,
StratShippedGameModeMatchConfig.cpp:383 : "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
StratShippedGameModeMatchConfig.cpp:386 : bool FStratShippedGameModeRunsAtEasyTest::RunTest(...)
```

All four sites still move together. **ZERO under `Source/`**, as the clause header's own claim
requires.

Old-name distribution, measured:

```
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md        : 4
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md : 4
Tools/architect/state/tests.md                                           : 1   (inside a [STAMPED ...] block)
Tools/architect/state/global.md                                          : 2   (narrative + STAMPED> re-quote)
Source/, Config/, Content/, .agents/, .github/                           : 0
```

**THE BRIEF'S ENUMERATION IS INCOMPLETE AND THE TREE WINS.** The brief names "4 in the first gate
report, 1 stamp in `tests.md`, stamped references in `global.md`, zero under `Source/`". It omits
the FOUR in my own prior regate report, which is untracked in this tree and is a legitimate record
of a gate on the post-rename tree, exactly as the first report is. Naming the sentence as
instructed; the omission is in the brief, not a defect in the tree. The distribution is otherwise
exactly as intended, and the `Source/` zero holds.

### 3d. The clause body — identity spot-checked

The clause still holds exactly one `TestEqual` (`ShippedTier` against `EStratDifficulty::Easy`),
exactly one `TestNotEqual` (the `AiSides` liveness control), and two `ResolveCdoOrFail` calls each
guarded by `return false;` on null. `Source/StratPlay/Tests/` is the only `Source/` path in the
diff, its `numstat` is unchanged from the prior gate at `146/2`, and its content hash is recorded
in Scope. No assertion, control, expectation, message or failure path moved this pass.

### 3e. Record ownership

`global.md:28` carries the only live count, `**388/388**`. The sweep's own RECORD OWNERSHIP check
agrees independently: `suite claims found: 61 (1 live, 60 stamped)` — unchanged from the prior
gate, so the two added paragraphs introduced no suite claim. No non-`global.md` record file carries
a live suite figure or a phase verdict.

### 3f. Attribution

The two added paragraphs are additions to an EXISTING banner entry, not new entries. That entry's
attribution stands at `:16-19`: "Written by the `coordinator`, whose file this is, IN LANE AND
UNDER NO EXCEPTION CLAUSE -- this seat wrote no code and no clause." `global.md`'s writer is the
`coordinator` by the file's own header, so limit (4) of the transcription clause puts this write
outside that clause entirely and no separate acting/writing split is owed. The new paragraphs name
their owners for the FIX — the `coordinator` at `:48-49`, `strat-data-steward` at `:56` — which is
ownership rather than attribution, and is correct. **I verify that the entry SAYS it; whether both
routes were genuinely offered is unfalsifiable from a checkout, as `CLAUDE.md` itself declares.**

### 3g. The architecture note

`.agents/ue-project-context.md` was read and checked against the derived graph. Line 40 states the
`StratBridge` row with both branches and the include-versus-link distinction; lines 109-113 reason
the `LNK2005` measurement and name `Target.LinkType != TargetLinkType.Monolithic`. **No drift.**
Not touched by this diff.

## 4. The instruments — re-run here, INLINED so a checkout can re-execute them

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
grep -nE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)[[:space:]]*[(]|GENERATED_BODY|generated[.]h' \
  Source/StratBridge/StratBridge.h                                         # -> empty
git diff -U0 -- Source/ | grep '^[+].*/Game/'                              # -> empty
grep -rnE 'TUniquePtr<FStratBridge>|TPimplPtr<FStratBridge>' Source/
git diff --stat -- Source/StratRules Data                                  # -> empty
git diff --stat -- Source/Stratocracy                                      # -> empty
git diff --stat -- Source/StratUI                                          # -> empty
git status --porcelain -- Source/StratRules Data Source/Stratocracy        # -> empty
```

### Instrument B — the module-arrow derivation and the field census

```bash
cd E:/MultiAgent/Stratocracy
for f in Source/*/*.Build.cs; do
  m=$(basename "$f" .Build.cs)
  sed 's#//.*##' "$f" | tr '\n' ' ' |
  grep -oE '(Public|Private)(DependencyModuleNames|IncludePaths)\.(AddRange|Add)\([^)]*\)' |
  while read -r line; do
    vals=$(printf '%s' "$line" | grep -oE '"[A-Za-z0-9_/]+"' | tr -d '"' | paste -sd, -)
    printf '%-12s %-30s %s\n' "$m" "${line%%.Add*}" "${vals:-(none)}"
  done
done | wc -l                                              # -> 16

for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done |
  grep -oE '[A-Za-z]+[[:space:]]*(\.AddRange|\.Add|=)' | sed 's/[[:space:]]*$//' | sort -u | wc -l
                                                          # -> 10
```

### Instrument C — the stamp-marker distances

```python
import re, io
p = "E:/MultiAgent/Stratocracy/Tools/architect/state/global.md"
text = io.open(p, encoding="utf-8").read()
marks = [m.start() for m in re.finditer(re.escape("[STAMPED"), text)]
print("total [STAMPED markers in file:", len(marks))

def report(label, needle):
    print("=== " + label + " : " + repr(needle))
    for i in [m.start() for m in re.finditer(re.escape(needle), text)]:
        ln = text[:i].count("\n") + 1
        b = [m for m in marks if m < i]; a = [m for m in marks if m > i]
        print("  line", ln, "offset", i,
              "| nearest [STAMPED BEFORE dist:", (i - b[-1]) if b else None,
              "| AFTER dist:", (a[0] - i) if a else None)

report("LIVE citation :35",                     "reportCreatedOn 2026.09.04-04.47.20")
report("LIVE citation :41 (REPAIRED, wrapped)", "reportCreatedOn\n2026.09.04-04.47.20")
report("STAMPED retired citation",              "reportCreatedOn 2026.09.04-04.09.26")
report("ALL bare 04.09.26",                     "2026.09.04-04.09.26")
```

Output is the table in section 1d.

### Instruments D3 and D4 — the part-(a) trace and its two mutants (NO FILE WAS WRITTEN)

```python
import sys, io
sys.path.insert(0, "E:/MultiAgent/Stratocracy/Tools/architect")
import strat_banner_sweep as S
text = io.open("E:/MultiAgent/Stratocracy/Tools/architect/state/global.md",
               encoding="utf-8").read()
LIVE = "2026.09.04-04.47.20"

print("banner end line:", S.current_banner_end(text))       # -> 120
print("--- RAW REGEX HITS: does the WRAPPED citation match at all? ---")
for m in S._CITED_REPORT_STAMP_RE.finditer(text):
    ln = text[:m.start()].count("\n") + 1
    print("  line", ln, "stamp", m.group(1), "| matched text repr:", repr(m.group(0)))

def partA(t, tag):
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
    print(tag, out)
    return out

partA(text, "REAL          :")

OLDSENT = ("The report timestamp runs ahead of local time -- `reportCreatedOn\n"
           "2026.09.04-04.47.20` by the report against 2026-09-04 locally")
assert text.count(OLDSENT) == 1, ("anchor count", text.count(OLDSENT))

# D3 -- change ONLY the timestamp of the REPAIRED sentence to a wrong value.
m3 = text.replace(OLDSENT, OLDSENT.replace("2026.09.04-04.47.20", "2026.09.04-01.11.11"), 1)
partA(m3, "MUTANT3 (wrong ts in repaired sentence):")

# D4 -- CONTROL: strip the reportCreatedOn token back out, restoring the OLD defect shape.
m4 = text.replace(OLDSENT,
                  "The report timestamp runs ahead of local time -- `\n"
                  "2026.09.04-01.11.11` by the report against 2026-09-04 locally", 1)
partA(m4, "MUTANT4 (token removed, wrong ts):")
```

The anchor `assert` refuses on anything but exactly one match, so a silently-missed replacement
cannot masquerade as a quiet result — a mutant the instrument failed to apply would otherwise pass
exactly like a citation that is correctly gated. Output is quoted in section 1b.

### Instrument E — the banner sweep, with a REAL exit code and a liveness control

The exit code was captured into a variable BEFORE anything read the output, because `$?` after a
pipe reports the last stage and this project has a recorded phantom exit-0 from exactly that.

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

59 lines, 55 `[OK]` fixtures, 0 `[FAIL]`, ending `SELF-TEST: ALL FIXTURES CORRECT`, including the
four that bear directly on this gate:

```
[OK] a wrong citation buried in a long paragraph, far from two UNRELATED stamp markers elsewhere
     in it, still FAILS: expected FAIL, got FAIL -- REPORT PROVENANCE
[OK] an honest account of a superseded citation, using the reporting verb 'cited', PASSES
[OK] the pre-fix whole-paragraph is_stamped call launders the buried citation (True) while the
     shipped windowed call does not (False), on the identical paragraph and match
[OK] the quoted-figure pattern matches the real 'cited reportCreatedOn ...' sentence while the
     pre-addition pattern does not, on the identical text
```

So the CLEAN is a measurement and not a silence. **It is also, per FINDING 1, a CLEAN over a
subject this instrument cannot see. A sweep-clean banner is not the same thing as a correct
banner, and this is the second consecutive pass on which that gap produced the only finding.**

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
print("any path containing AuthorsEasyDifficulty:",
      [x for x in paths if "AuthorsEasyDifficulty" in x])
st = os.stat(p)
newest = max((os.stat(os.path.join(r, f)).st_mtime, os.path.join(r, f))
             for r, _, fs in os.walk("E:/MultiAgent/Stratocracy/Source") for f in fs)
print("report mtime ", st.st_mtime)
print("newest source", newest)
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
any path containing AuthorsEasyDifficulty: []

T-FAME-02 entries (all 8, all Success):
   Stratocracy.StratBridge.T-FAME-02.HandicapClampsAtZeroAndReportsZero
   Stratocracy.StratBridge.T-FAME-02.HandicapMovesTheScenariosConfiguredFameByTheDelta
   Stratocracy.StratBridge.T-FAME-02.HandicapRefusesAnUnseededBridgeAndAnOutOfRangeSide
   Stratocracy.StratPlay.T-FAME-02.DifficultyTierDeltasAreSection29sThreeNumbers
   Stratocracy.StratPlay.T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent
   Stratocracy.StratPlay.T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide
   Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy

report mtime  : 1788497240.2102695
newest source : 1788497056.0493834 -> Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
REPORT NEWER THAN SOURCE: True   delta(s): 184.2
```

**`reportCreatedOn` is still `2026.09.04-04.47.20`, identical to the prior gate's read, and so is
every figure with it.** No suite ran since the last gate; the brief's conditional finding does not
trigger. The report still postdates the last source edit by 184 seconds, so it describes THIS tree
and not a previous one — the check that separately rules out a stale binary reporting old clause
names green. `succeeded` 388 equals `len(tests)` 388 equals the histogram's only bucket, and the
OLD name is absent BY NAME and BY SUBSTRING, so a partial rename cannot be hiding in a longer path.

### Instrument G — my own macro set difference, ONE collector over BOTH sides

**FILE-SET RULE, STATED EXPLICITLY AND IDENTICAL ON BOTH SIDES: a path is in scope iff it starts
with `Source/` and ends with `.cpp`.** The evidence fossil under `Tools/` is therefore EXCLUDED
from both sides. The path sets are compared for equality BEFORE any clause name is compared,
because a set difference whose two sides enumerate different files measures its collectors.

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
    r = subprocess.run(["git", "-C", REPO, "show", "HEAD:" + p], capture_output=True, text=True)
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
```

**+1 added, 0 removed, 387 -> 388, and the added name is the NEW one** — exactly what the brief
expects. `HEAD` (`9d705ca`) predates the whole clause, so a rename presents here as a single
addition rather than an add-plus-remove pair; the old name's absence from the exported report
(instrument F) is the separate evidence that the rename reached the binary.

### Instrument H — the report-stamp census across all six state files

```python
import sys, io, re
sys.path.insert(0, "E:/MultiAgent/Stratocracy/Tools/architect")
import strat_banner_sweep as S
D = "E:/MultiAgent/Stratocracy/Tools/architect/state/"
FILES = ["content.md", "data.md", "decisions.md", "engine.md", "global.md", "tests.md"]
STAMP = re.compile(r"(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2})")
TOKEN = re.compile(r"reportCreatedOn\s*$")   # the token IMMEDIATELY before the stamp

rows = []
for f in FILES:
    t = io.open(D + f, encoding="utf-8").read()
    lines = t.split("\n")
    banner_end = S.current_banner_end(t) if f == "global.md" else None
    for m in STAMP.finditer(t):
        ln = t[:m.start()].count("\n") + 1
        pre = t[max(0, m.start() - 40):m.start()]
        gated = bool(TOKEN.search(pre) or TOKEN.search(pre.rstrip("`").rstrip()))
        linetext = lines[ln - 1]
        rows.append(dict(file=f, line=ln, stamp=m.group(1), gated=gated,
                         stamped_line=(linetext.lstrip().startswith("STAMPED>")
                                       or "[STAMPED" in linetext),
                         cur_banner=(f == "global.md" and ln < banner_end),
                         text=linetext.strip()[:110]))

print("TOTAL report-stamp-shaped strings:", len(rows))
for f in FILES:
    r = [x for x in rows if x["file"] == f]
    print(f"  {f:14} {len(r):4}  gated={sum(1 for x in r if x['gated']):4}"
          f"  ungated={sum(1 for x in r if not x['gated']):4}")
for x in [x for x in rows if not x["gated"]]:
    print(f"  {x['file']}:{x['line']}  {x['stamp']}  cur_banner={x['cur_banner']}")
    print(f"       | {x['text']}")
```

The `gated` test deliberately requires the token IMMEDIATELY before the stamp, which is what the
sweep's own regex requires; a looser "token anywhere on the line" test would have scored the debt
paragraph as gated and hidden the very class this census exists to find. Every one of the 56
ungated hits was printed with its line text and read in place; the classification is section 2.

## Observations

Non-gating.

1. **`global.md:79-80` still claims `tests.md` carries "the one surviving stamp of the old name
   under `Tools/architect/`", and this pass made it further from true.** `global.md` itself carries
   two occurrences, and there are now TWO gate reports carrying four each. The narrow reading —
   that `tests.md` holds the one `[STAMPED ...]`-bracketed *stamp*, as opposed to a citation — is
   defensible, and `global.md:73` is a `STAMPED>` re-quote that arguably also qualifies. Raised as
   Observation 3 last pass and unrepaired. Owner if pursued: the `coordinator`.
2. **`global.md:59-60` still says the first gate "raised one observation". It raised FOUR**
   (`2026-09-04-shipped-difficulty-pin.md`, lines 508, 523, 527, 533). Raised as Observation 1 last
   pass and unrepaired; the other three are recorded nowhere in the state files.
3. **The new debt paragraph plants a bare retired stamp in the current banner, and the fix it asks
   for will fire on it.** `global.md:52` quotes `2026.09.04-04.09.26` as the exemplar of the class.
   When `strat-data-steward` extends the citation regex to match a bare `YYYY.MM.DD-HH.MM.SS`, this
   sentence — and `:45` — become the new check's first false positives. This is this project's
   recorded "a census that quotes its own search token" hazard. The fix should be designed with
   those two sites in hand: either stamp them, or give the bare-stamp rule a use/mention exemption
   and pin it with a fixture. Worth carrying into the debt entry so the steward does not meet it as
   a surprise.
4. **`global.md` and `tests.md` are CRLF in the worktree and LF in `HEAD`** (7515 and 5920 CRLF
   against 7395 and 5796 bare LF respectively). `git diff --numstat` does not see it, so the
   normalisation is `autocrlf`'s and not a content change; it affects `tests.md` too, which was not
   touched this pass, so the repair did not introduce it. This project has a recorded memory for
   exactly this. Worth a glance at staging time.
5. **The `autocrlf` warning on the `.cpp` persists** — `git diff` still emits "LF will be replaced
   by CRLF the next time Git touches it". Pre-existing, correctly reported by the lane as not
   introduced by this diff.
6. **The two prior gate reports are untracked (`??`) rather than modified.** Both were read and NOT
   edited, as instructed. This report is a new file beside them; none of the three is staged.
7. **`.agents/ue-project-context.md` needed no change this pass.** No drift found against the
   derived module graph, the conditional `StratRules` edge, or the `IMPLEMENT_MODULE` registration
   rule.
8. **FINDING 1 is the second consecutive finding that the sweep structurally cannot see.** Last
   pass it was a bare timestamp; this pass it is a prose claim about whether a gate has run. The
   common shape is a correction that only ADDS. If a mechanical guard is ever wanted for it, the
   subject is not timestamps — it is "the current banner asserts a fact about its own gating
   history", and that is likely cheaper to handle by convention (a repair pass re-reads the WHOLE
   current banner, not just the sentence it was sent to fix) than by regex. Recorded because two
   in a row is a pattern rather than two accidents.

VERDICT: BLOCK
