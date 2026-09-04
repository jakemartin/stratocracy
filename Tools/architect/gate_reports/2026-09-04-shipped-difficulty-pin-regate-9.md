# Gate: shipped `Difficulty` asset-pin, RE-GATE 9 (final) — strat-integration-reviewer

## Scope

Working-tree audit of the UNCOMMITTED tree at the absolute path `E:\MultiAgent\Stratocracy`,
branch `master`, base commit `9d705ca`. No worktree was entered.

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
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-5.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-6.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-7.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-8.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
$ git diff --stat
 .../Tests/StratShippedGameModeMatchConfig.cpp      | 148 ++++++++++++++++-
 Tools/architect/state/global.md                    | 170 ++++++++++++++++++-
 Tools/architect/state/tests.md                     | 185 ++++++++++++++++++++-
 3 files changed, 493 insertions(+), 10 deletions(-)
```

Three modified files, nine untracked gate reports, nothing staged. The audited change since
RE-GATE 8 is one sentence in `Tools/architect/state/global.md`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` printed nothing on both paths |
| 2 | No `strat::` call outside `Source/StratBridge/` and `Source/StratRules/` | pass — the call-shaped grep returned zero rows outside the two permitted directories |
| 3 | No vendored header in front of UHT | pass — the only header including the vendored headers is `Source/StratBridge/StratBridge.h`, which declares no reflected type; every macro occurrence in it is prose |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — two live `TPimplPtr<FStratBridge>` members; the only `TUniquePtr<FStratBridge>` string in the tree is a comment recording the measured `C4150` |
| 5 | Module arrows unchanged and correct | pass — DERIVED, not typed: 16 rows, 10 field names, the 16th row confirmed guarded by reading the branch |
| 6 | New modules registered, unless no module object | pass — no new module; `StratRules` correctly absent from `Modules` and carries no `IMPLEMENT_MODULE` |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in C++ outside the exceptions | pass — exactly one hit outside `Tests/`, the import commandlet, a named standing exception |
| 9 | No widget-side arithmetic | pass — no file under `Source/StratUI/` is in the diff |
| 10 | Lane discipline | pass — the only write since RE-GATE 8 is `global.md`, the `coordinator`'s own file |
| 11 | Prose blocks present | pass — no new file; the changed `.cpp` carries its why-first block and did not move this pass |
| 12 | Nothing staged or committed | pass — three ` M`, nine `??`, no index entries |
| R1 | The one sentence: replacement TRUE on both halves | pass — verified against `...-regate-3.md` and `...-difficulty-pin.md` |
| R2 | The edit is genuinely SUBTRACTIVE — no new count, no closure over a growing set, no uncheckable locational claim | pass |
| R3 | The diff is that one sentence and nothing else in either record file | pass — measured; the limit on the measurement is stated in Observation 1 |
| R4 | `strat_banner_sweep.py`, full output, real exit code, not through a pipe, with a mutant | pass — `SWEEP CLEAN`, exit 0; mutant fires, exit 1 |
| R5 | `index.json` read `utf-8-sig`; `reportCreatedOn` unmoved; NEW name present, OLD absent | pass — `2026.09.04-04.47.20`; 388 / 0 / 0 / 0; 388 entries, all `Success` |
| R6 | Macro set difference HEAD to worktree, one collector, symmetric file-set rule stated | pass — +1 added, 0 removed, 0 renamed in place |
| R7 | Rename completeness and clause-body identity | pass — zero old-name occurrences under `Source/`; class, registered path and cross-reference all renamed |
| R8 | `global.md` the only file with a live count or verdict; inline separate attribution; `[STAMPED` distances MEASURED | pass on all three |
| R9 | Cross-file consistency of both newest entries | pass — same clause name, same report stamp, no live figure in `tests.md` |
| R10 | This report's own arrival falsifies nothing in either record file | pass |

---

## 1. THE ONE SENTENCE

### 1a. What moved

RE-GATE 8's Observation 2 quoted the live text at `global.md:71` as *"two lines above the file
that disproved it"* and measured the real distance at 29 lines. That phrase is gone from the
record:

```
$ grep -rn "two lines above" Tools/architect/state/
Tools/architect/state/engine.md:5631:    **BOTH SENTENCES ARE RETRACTED AND NOT ONLY THE CONCLUSION.** The premise sat two lines above
```

The single surviving hit is in `engine.md`, is about an unrelated premise, and is not in this
diff — `engine.md` is not among the three modified files. The phrase is absent from `global.md`.

The live text now reads, at `:70-72`:

```
70: THE OTHER THREE OBSERVATIONS ARE RECORDED HERE BECAUSE THE SENTENCE ABOVE ONCE SAID "ONE" AND
71: THEREFORE DROPPED THEM -- a live miscount found on the third re-gate, against the first gate
72: report's own `## Observations` section. (2) The `autocrlf` warning on the `.cpp` is REAL and PRE-EXISTING: the file
```

### 1b. Is the replacement TRUE? Both halves verified against the named artifacts.

**Half one — "found on the third re-gate."** The third re-gate is
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md`. Its `## Findings`
opens with exactly this defect, at BLOCK severity:

```
### FINDING 1 — BLOCK. The banner says the first gate "raised one observation". It raised FOUR,
and the file that proves it is named in the same paragraph.
```

**Half two — "against the first gate report's own `## Observations` section."** That same finding
states its instrument, and the instrument is that section:

```
Measured against the artifact that same paragraph goes on to name at `:77`:

$ awk '/^## Observations/,0' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md | grep -n "^[0-9]+\."
5:1. **THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP. ...
20:2. **The `autocrlf` warning `tests.md` reports is real and is pre-existing.** ...
24:3. **The liveness control compares STRINGS, not arrays.** ...
30:4. **Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable ...
```

I did not inherit that. I re-ran the two premises it rests on, in this tree, this pass:

```
$ F=Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
$ grep -n '^## ' "$F"
3:## Scope
28:## Checklist
45:## Findings
50:## 1-2. The code, audited first per the standing gate-priority ruling
130:## 3. The record
206:## 4. The instruments — re-run here, INLINED so a checkout can re-execute them
504:## Observations

$ awk '/^## Observations/,0' "$F" | grep -nE '^[0-9]+\.'
5:1. **THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP. This is the
20:2. **The `autocrlf` warning `tests.md` reports is real and is pre-existing.** `git diff` on the
24:3. **The liveness control compares STRINGS, not arrays.** `TestNotEqual(..., Describe(A), Describe(B))`
30:4. **Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable from

$ tail -n 1 "$F"
    VERDICT: PASS      <- quoted output, indented so this line is not a verdict token

$ ls -1t Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md | tail -1
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
```

So: `...-difficulty-pin.md` IS the first gate report of this pass (oldest of the series by
mtime), it DOES carry a `## Observations` section, and that section DOES hold four numbered
items — which is the set the banner's "ONE" undercounted, and the set the banner now enumerates
as (2), (3) and (4) in the lines immediately following. Each of those three matches its numbered
counterpart in the report: `autocrlf` real and pre-existing, the liveness control comparing
rendered strings rather than arrays, and obligation (1) being unfalsifiable from a checkout.

**Both halves are TRUE.**

### 1c. Is it genuinely SUBTRACTIVE?

The false number is gone (1a). What replaces it is examined against the brief's own bar:

- **No new count.** The replacement contains no cardinality. "the first gate report" is an
  ordinal identifying one artifact, not a count of a set.
- **No closure over a growing set.** Its subject is a single, closed file — a finished gate
  report that nothing further will add to. Contrast the shapes that failed rounds three through
  seven, every one of which quantified over `Tools/architect/gate_reports/`, a directory that
  grows each time a gate runs. This sentence is immune to that by construction: it names one
  file and one section of it.
- **No uncheckable locational claim.** This is the exact defect being removed — "two lines above"
  asserted a layout no reader could confirm and that measured at 29. The replacement asserts a
  containment relation instead of a distance, and containment is settled by `awk` in one command,
  which I ran above. It survives any future re-wrapping of `global.md`, which the removed phrase
  did not.

The edit removed a false measurement and put in its place a claim that is both true and cheaper
to check than the one it replaced. It is subtractive in effect as well as intent.

The intermediate wording the brief says was discarded — *"in the same paragraph as the report
that disproved it"* — was correctly discarded, and I record why so the reasoning is not lost: the
banner is a single unbroken block with no blank line between `:14` and `:82`, so "the same
paragraph" would have been trivially true in a way that told a reader nothing, while reading as a
locational measurement. The sentence that landed does not have that property.

### 1d. Is the diff that one sentence and nothing else?

**`tests.md` and the `.cpp` — decisive.** Both have mtimes predating RE-GATE 8's own report, so
neither can have been written since that gate read them:

```
$ stat -c '%y %n' Tools/architect/state/global.md Tools/architect/state/tests.md Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-8.md
2026-09-04 03:20:48.319299500 -0400 Tools/architect/state/global.md
2026-09-04 02:31:26.766882300 -0400 Tools/architect/state/tests.md
2026-09-04 00:44:16.049383500 -0400 Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
2026-09-04 03:19:47.727744000 -0400 Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-8.md
```

RE-GATE 8's report was written at 03:19:47. `tests.md` last moved at 02:31 and the `.cpp` at
00:44, both BEFORE it. `global.md` moved at 03:20:48, 61 seconds AFTER it. **`global.md` is the
only file written since RE-GATE 8**, which answers the brief's "I believe nothing did, but I
want it measured" for the other two.

**`global.md` — bounded, and the bound is tight.** No snapshot of the RE-GATE 8 state exists to
diff against (the file is uncommitted and there is one copy of it), so I bounded the edit by
re-resolving every line number RE-GATE 8 cited. If the edit had added or removed a line anywhere
at or above `:110`, these would all have slipped:

| RE-GATE 8 cited | What that line resolves to now | Match |
|---|---|---|
| `:45-46` "twelve lines" | `:45-46` — *"`2026.09.04-04.09.26` -- retired by the rename re-run -- LIVE and UNSTAMPED, twelve lines / above the block that stamps that very citation"* | yes |
| `:50-57` sweep bare-stamp debt | `:50-57` — *"CARRIED DEBT, INSTRUMENT, OPEN: `strat_banner_sweep.py` cannot see a BARE report timestamp."* … through *"the sentence repaired above now does."* | yes |
| `:71` the miscount sentence | `:71` — the sentence under audit | yes, with the new wording |
| `:90-91` "Gate reports name the OLD identifier" | `:90-91` — *"THE RENAME POST-DATES THE FIRST GATE, AND NO GATE REPORT WAS EDITED. / Gate reports name the OLD identifier…"* | yes |
| `:100` the report glob | `:100` — ``Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md``… | yes |
| `:110` the "a miscount name" garden-path | `:110` — *"and was a finding for it: the paragraphs repairing a stale citation and a miscount name where"* | yes |

All six resolve to the same content at the same line numbers. The edit therefore added and
removed zero lines, and is confined to a within-line splice at `:71-72`. Every other live claim
in the file was additionally re-derived from scratch in section 2 below rather than inherited.
The honest limit on this: line-number stability plus independent re-derivation is very strong
evidence and is not a byte-diff. I say which it is rather than overstate it — see Observation 1.

**No other file moved.** `git diff --stat` lists three paths; two are excluded by mtime above;
`git status --porcelain` shows no fourth modification and no staged content.

---

## 2. THE STANDING CHECKS — re-derived, not inherited

### 2a. Vendored bytes, and the game module

```
$ git diff --stat -- Source/StratRules Data
$ git diff --stat -- Source/Stratocracy
```

Both printed nothing. Per this gate's own rule the authority for the vendored commits is the
manifests, not any figure quoted in a brief; no vendored path is in the diff, so no re-derivation
of the manifest commits was required and none is claimed.

### 2b. No `strat::` call outside the two permitted directories

```
$ grep -rnE 'strat::[a-z][A-Za-z0-9_]*\s*\(' Source/ --include=*.cpp --include=*.h | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'
(rc=1)
```

Zero rows. The lowercase-initial pattern is what separates a call from a type, so the many legal
`strat::UiSnapshot` / `strat::GameState` type mentions elsewhere are correctly not matched and are
not reported here.

### 2c. No vendored header in front of UHT

```
$ for h in $(grep -rlE 'UCLASS|USTRUCT|UENUM' Source/ --include=*.h); do
    if grep -qE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' "$h"; then
      echo "HIT: $h"; grep -nE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' "$h"; fi
  done
HIT: Source/StratBridge/StratBridge.h
53:#include "Data.h"
55:#include "Replay.h"
56:#include "Save.h"
57:#include "Scenario.h"
58:#include "Ui.h"
```

**Judged, not forwarded.** The one hit is the bridge header itself, and it is a hit only because
the file *discusses* the reflection macros in prose. It declares none:

```
$ grep -nE '^\s*(UCLASS|USTRUCT|UENUM)\s*\(' Source/StratBridge/StratBridge.h
(rc=1)
$ grep -nE 'UCLASS|USTRUCT|UENUM' Source/StratBridge/StratBridge.h | head -6
30:// back a `strat::UiSnapshot` by reference parameter rather than a USTRUCT: a
119:// PLAIN AND NOT REFLECTED, for `FStratResult`'s reason exactly: a `USTRUCT` here would
262:// struct's reason: this header must declare no `UCLASS`/`USTRUCT`/`UENUM`, because
264:// includes go in front of the header tool. A `UENUM` here would cost that, and no
323:// this header must declare no `UCLASS`/`USTRUCT`/`UENUM`, or UHT parses it and the
486:// must declare no `UCLASS`/`USTRUCT`/`UENUM` or UHT parses it and the vendored `strat`
```

Every occurrence is a comment stating the rule this check enforces. Not a violation.

### 2d. `TPimplPtr`, not `TUniquePtr`

```
$ grep -rn "TUniquePtr<FStratBridge>\|TPimplPtr<FStratBridge>" Source/
Source/StratPlay/StratMatchSubsystem.h:776:   * ... each holding a `TPimplPtr<FStratBridge>`
Source/StratPlay/StratMatchSubsystem.h:2496:  TPimplPtr<FStratBridge> Bridge;
Source/StratUI/StratScoreboardHUD.cpp:14:// ... `OwnedBridge` is a `TPimplPtr<FStratBridge>`, which
Source/StratUI/StratScoreboardHUD.cpp:184:// here -- it is not. `OwnedBridge` is a `TPimplPtr<FStratBridge>`, which type-erases its
Source/StratUI/StratScoreboardHUD.h:73:// held as `TPimplPtr<FStratBridge>`, and included ONLY from the .cpp.
Source/StratUI/StratScoreboardHUD.h:1235:  TPimplPtr<FStratBridge> OwnedBridge;
Source/StratUI/StratViewModel.h:47:// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in
Source/StratUI/Tests/StratScoreboardHUDSeam.cpp:6:// ... `TPimplPtr<FStratBridge> OwnedBridge`, which this
```

Two declarations, both `TPimplPtr`. The single `TUniquePtr<FStratBridge>` string is a comment at
`StratViewModel.h:47` recording the measured `C4150`.

### 2e. Module arrows — DERIVED, and the field set censused

```
$ for f in Source/*/*.Build.cs; do
    m=$(basename "$f" .Build.cs)
    sed 's#//.*##' "$f" | tr '\n' ' ' |
    grep -oE '(Public|Private)(DependencyModuleNames|IncludePaths)\.(AddRange|Add)\([^)]*\)' |
    while read -r line; do
      vals=$(printf '%s' "$line" | grep -oE '"[A-Za-z0-9_/]+"' | tr -d '"' | paste -sd, -)
      printf '%-12s %-30s %s\n' "$m" "${line%%.Add*}" "${vals:-(none)}"
    done
  done
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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,AIModule,NavigationSystem,StateTreeModule,GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
  -> 16 rows
```

**16 rows, all five modules present** — no module missing, which would itself be a finding since
an empty extraction is indistinguishable from a clean one. Row by row the graph matches the
carried block: `StratRules -> Core`; `Stratocracy -> ... StratRules`; `StratBridge -> Core,
CoreUObject, Engine, Stratocracy` with `PublicIncludePaths "StratRules"`; `StratUI -> StratBridge`
plus private UMG/Slate/SlateCore and **no `Stratocracy` dependency**; `StratPlay -> StratUI` plus
private `StratBridge`, UMG, EnhancedInput. No cycle. `StratBridge -> Stratocracy` is the
deliberate row-struct edge and is not reported as a layering violation.

**The 16th row is the conditional one, and the extraction cannot show that. I read the branch:**

```
$ grep -n "bCompileVendoredRulesHere\|Monolithic\|PrivateDependencyModuleNames\|PrivateDefinitions" Source/StratBridge/StratBridge.Build.cs
75:  PrivateDependencyModuleNames.AddRange(new string[] { });
133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
135:  if (!bCompileVendoredRulesHere)
137:      PrivateDependencyModuleNames.Add("StratRules");
```

The `.Add("StratRules")` at `:137` sits inside `if (!bCompileVendoredRulesHere)` at `:135`, and
`bCompileVendoredRulesHere` at `:133` is the single expression `Target.LinkType !=
TargetLinkType.Monolithic`. So the link edge exists on monolithic targets only, the compile half
and the dependency half come from that one expression and cannot drift, and `StratRules` appears
in `PublicDependencyModuleNames` nowhere and in `PrivateDependencyModuleNames` only under the
guard. **This is the correct, deliberate shape. Not reported.**

Field-set census:

```
$ for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done | grep -oE '[A-Za-z]+[[:space:]]*(\.AddRange|\.Add|=)' | sed 's/[[:space:]]*$//' | sort -u
DGE=
PCHUsage =
PrivateDefinitions.Add
PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
  -> 10 names
```

**Ten, as the block records.** No eleventh name; no `DynamicallyLoadedModuleNames`,
`PrivateIncludePaths` or `PublicSystemLibraries` has appeared. `DGE=` is the known false positive
— the tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="` inside `PrivateDefinitions.Add`
at `:134`, which the comment-strip cannot reach inside. It is left unfiltered deliberately:
narrowing the census to spare a known false positive is how it would stop seeing the next real
field.

### 2f. Module registration

```
$ python -c "import json,io;d=json.load(io.open('Stratocracy.uproject',encoding='utf-8-sig'));print([m['Name'] for m in d['Modules']])"
['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']
$ for d in Source/*/; do n=$(basename $d); c=$(grep -rl "IMPLEMENT_MODULE\|IMPLEMENT_PRIMARY_GAME_MODULE" $d | head -1); echo "$n -> ${c:-NONE}"; done
StratBridge -> Source/StratBridge/StratBridge.cpp
StratPlay   -> Source/StratPlay/StratPlay.cpp
StratRules  -> NONE
StratUI     -> Source/StratUI/StratUI.cpp
Stratocracy -> Source/Stratocracy/Stratocracy.cpp
```

Four modules carry a module object and all four are registered. `StratRules` carries none and is
correctly absent — flagging it would be the false positive that costs a startup abort to discover.
No new `Source/<Module>/` directory in the diff.

### 2g. No `/Game/` literal outside the exceptions

```
$ grep -rn '"/Game/' Source/ --include=*.cpp --include=*.h | grep -v '/Tests/'
Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54:
    const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);
```

Exactly one hit outside `Tests/`: the import commandlet, one of the two named standing exceptions.
Every other occurrence — including the two in the changed `.cpp`, at `:113` and `:117`, naming
`BP_StratGameMode_C` and `BP_StratGameMode_AiVsAi_C` — is inside a `Tests/` directory, the
automation-fixture exception.

### 2h. Widget-side arithmetic, and lane discipline

No file under `Source/StratUI/` appears in the diff, so no widget number moved. The only write
since RE-GATE 8 is `Tools/architect/state/global.md`, which is the `coordinator`'s own file — in
lane. `tests.md` and the `.cpp` were `strat-test-author`'s and were written by that lane, in lane,
and are unchanged since 02:31 and 00:44 respectively. Test code is under `Tests/`; no production
code is under `Tests/`; no asset changed.

### 2i. Rename completeness and clause-body identity

```
$ grep -rn "ShippedGameModeAuthorsEasyDifficulty" Source/
(rc=1)
$ grep -rn "ShippedGameModeRunsAtEasy" Source/
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:77://  this file, `T-FAME-02.ShippedGameModeRunsAtEasy`, and that clause is a
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:382:  FStratShippedGameModeRunsAtEasyTest,
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:383:  "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:386:bool FStratShippedGameModeRunsAtEasyTest::RunTest(const FString& /*Parameters*/)
```

Zero old-name occurrences under `Source/` — which is the one property of the old name's
distribution that a checkout can re-execute, and the one the banner correctly claims. The class,
the registered path, the `RunTest` definition and the in-file cross-reference are all renamed
consistently. The clause body is otherwise unchanged and carries no premise block, as both
records state.

### 2j. `[STAMPED` distances — MEASURED, not reasoned

Every `reportCreatedOn <stamp>` citation in `global.md`, measured in characters against the
nearest `[STAMPED` / `[CORRECTED` / `RETRACTED>` marker, using the sweep's own 400-character
`_PROVENANCE_CITATION_WINDOW`:

```
$ python -    # measures byte offsets of every citation and every marker in global.md
citations of a report stamp in global.md: 65
  line 35   2026.09.04-04.47.20   nearest marker = 4004 chars -> LIVE
  line 41   2026.09.04-04.47.20   nearest marker = 3446 chars -> LIVE
  line 84   2026.09.04-04.09.26   nearest marker =  205 chars -> in-window (stamped)
  line 175  2026.09.04-03.04.52   nearest marker =   96 chars -> in-window (stamped)
  line 287  2026.09.02-23.16.13   nearest marker =   28 chars -> in-window (stamped)
  ... 60 older citations, all either in-window or historical ...
```

The result that matters for this pass: **the only two LIVE citations in the current banner are
both `2026.09.04-04.47.20`, at `:35` and `:41`, and both match the report on disk.** The two
retired stamps — `04.09.26` at `:84` and `03.04.52` at `:175` — sit 205 and 96 characters from
their `[STAMPED` markers, well inside the window, so the sweep gates them as stamped rather than
live. This is the check RE-GATE 1 found broken; it is measured here and is intact.

The bare stamps at `:45` and `:52` (no `reportCreatedOn` token) remain the OPEN instrument debt
recorded at `:50-57`. Both are mentions of a retired stamp inside the prose that explains its
retirement, not live citations; the record correctly names the gap as open and in
`strat-data-steward`'s lane. Unchanged this pass. Not a finding.

### 2k. `global.md` the only file with a live count or verdict

The sweep's own accounting, run over all six record files (section 3a), reports **`suite claims
found: 61 (1 live, 60 stamped)`** — exactly one live suite claim in the whole record. Run over
`global.md` alone it reports `58 (1 live, 57 stamped)`, locating that one live claim in
`global.md`. An independent grep of the other five record files for an N/N figure or a verdict
token returned only historical, stamped, or quoted occurrences — `data.md`'s 346/347 guard
narrative, `decisions.md`'s and `engine.md`'s CORRECTED stamps, `tests.md:3738`'s quotation of a
past gate. No second live figure and no live verdict.

### 2l. Inline separate attribution, and the two newest entries agreeing

`global.md:14-19`, the banner's opening, attributes inline:

```
Written by the `coordinator`, whose file this is, IN LANE AND UNDER NO EXCEPTION
CLAUSE -- this seat wrote no code and no clause. The clause and the test prose are
`strat-test-author`'s and are in `tests.md`; that lane was DISPATCHED and wrote its own entry, in
lane, itself. This entry is not a transcription of anything.
```

`tests.md`'s newest entry attributes inline in the required shape and names its base commit:

```
- **2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- one file under
  `Source/*/Tests/`, plus this file -- on `master` in the main tree `E:/MultiAgent/Stratocracy`,
  base commit `9d705ca`, UNCOMMITTED)
```

Neither is a transcription and neither invokes an exception clause, so the transcription clause's
limits and the in-session clause's four obligations are not engaged by either. The two entries
agree on every shared fact I can check: the clause name
`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy`; the report stamp
`reportCreatedOn 2026.09.04-04.47.20`; the +1/0 delta; the value-not-authorship scope; the
different-field liveness control; the two mutants and their separate probe report paths.
`tests.md` states no figure and no verdict, deferring to `global.md` explicitly at `:23`.

The banner's routing claim — *"`git worktree list` still shows seven worktrees"* — is true as
measured this pass: the command prints eight rows, the main tree plus seven others.

---

## 3. THE INSTRUMENTS — re-run here, inlined so a checkout can re-execute them

### 3a. `strat_banner_sweep.py` — full output, real exit code, NOT through a pipe

```
$ cd E:/MultiAgent/Stratocracy && python Tools/architect/strat_banner_sweep.py
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
EXITCODE=0
```

The exit code is read from the sweep itself with nothing downstream of it — the recorded
exit-code-after-a-pipe hazard is avoided by construction.

### 3b. The sweep proved NON-VACUOUS on this file's bytes, with a control

A clean sweep is indistinguishable from an inert one, so both arms were run. The mutant was made
on a copy in the session scratchpad; **no repository file was written by this control.**

```
$ SP=<session scratchpad>
$ cp Tools/architect/state/global.md "$SP/global_control.md"
$ cp Tools/architect/state/global.md "$SP/global_mutant.md"
$ python -  "$SP/global_mutant.md"      # 388/388 -> 999/999, asserting exactly 1 occurrence
mutant written: 388/388 -> 999/999 (1 occurrence)

--- CONTROL (unmutated copy of the live file) ---
$ python Tools/architect/strat_banner_sweep.py "$SP/global_control.md"
Banner sweep: global.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 58 (1 live, 57 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
CONTROL_EXIT=0

--- MUTANT ---
$ python Tools/architect/strat_banner_sweep.py "$SP/global_mutant.md"
Banner sweep: global.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 58 (1 live, 57 stamped)
  tracked items declared: 2

  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the tree's total (388): global.md:28: 999/999
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the report's own succeeded count (388): global.md:28: 999/999

SWEEP FAILED -- the record contradicts itself or the tree
MUTANT_EXIT=1
```

The control passes and the mutant fires, on the live file's own bytes, against the live report.
The instrument can speak, so its silence on the healthy path is a measurement rather than an
absence. Note the scope of that warrant: this is a control on the sweep's COUNT checks. The sweep
has no check for a banner's prose claims about the gate reports, which is precisely why the
sentence under audit needed a human gate — Observation 3.

### 3c. `Saved/AutomationReport/index.json`, opened `utf-8-sig`

```
$ python -     # json.load(io.open(path, encoding="utf-8-sig"))
reportCreatedOn: 2026.09.04-04.47.20
succeeded: 388 failed: 0 notRun: 0 succeededWithWarnings: 0
entry count: 388
NEW present: ['Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy']
OLD present: ABSENT
state counter: Counter({'Success': 388})
```

`reportCreatedOn` is still **`2026.09.04-04.47.20`**, as the brief requires and as the banner and
`tests.md` both cite. 388 succeeded, 0 failed, 0 notRun, 0 succeededWithWarnings; 388 entries, all
`Success`. The NEW clause is present BY NAME, not inferred from a delta; the OLD name is absent by
the same read. Nothing in this pass rebuilt or re-ran anything, and the artifact is unmoved.

### 3d. Macro set difference, HEAD to worktree, ONE collector, symmetric file set

```
$ python -
# MACRO matches IMPLEMENT_(SIMPLE|COMPLEX)_AUTOMATION_TEST(_PRIVATE)? followed by the class
#   name and the quoted registered path, compiled with re.S so it spans lines. A single-line
#   grep would return ZERO here.
# FILE-SET RULE, STATED: the UNION of every Source/**/*.cpp present in HEAD OR in the
#   worktree, probed on BOTH sides, so a file added or deleted on one side is never
#   silently dropped from the comparison.
# ONE COLLECTOR: collect(reader, paths) is used for both sides. Only the reader differs
#   (git show HEAD:<path> versus open(<path>)); the extraction does not, so the set
#   difference cannot end up measuring its own collectors.
file-set rule: UNION of every Source/**/*.cpp in HEAD and in the worktree
  HEAD .cpp: 159  worktree .cpp: 159  union: 159
  macro clauses HEAD: 387  worktree: 388
ADDED   (+1): ['Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy']
REMOVED (-0): []
CLASS-RENAMED IN PLACE (0): []
```

**+1 added, 0 removed**, exactly as both records claim, and the worktree total of 388 agrees with
the report's succeeded count of 388 and with the sweep's own macro census.

### 3e. This report's own arrival

This file lands at `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-9.md`
and matches the glob `global.md:99-101` points at. Checked claim by claim against what its
arrival does:

- *"The gate reports are the files matching `...2026-09-04-shipped-difficulty-pin*.md`"* — still
  true; this file matches the pattern. The claim is a pattern, not an enumeration, which is
  exactly what lets it survive a ninth report.
- *"Each is a new file leaving its predecessors byte-identical"* — preserved: this is a new file
  and I hold no write tool that could touch a predecessor. `git status --porcelain` on the
  directory shows the nine existing reports still untracked and unmodified.
- *"and carries its own `VERDICT:` line and its own findings"* — preserved: this report has both.
- `:90-91`, *"Gate reports name the OLD identifier"* — preserved: this report quotes
  `ShippedGameModeAuthorsEasyDifficulty` in section 2i, so my arrival keeps the universal true
  rather than breaking it. It remains one silent future report away from becoming false, which
  is Observation 2.
- `:106-108`, *"It NEITHER ENUMERATES NOR COUNTS the re-gates"* — preserved, and this is the
  claim most at risk from a ninth report. `global.md` states no number of re-gates, lists none,
  and makes no closed statement of their verdicts. Its two references to individual re-gates —
  *"the 2026-09-04 RE-GATE"* at `:47` and *"the third re-gate"* at `:71` — are singular
  references, which `:109-111` explicitly licenses and distinguishes from counting. My arrival
  makes neither false.
- `tests.md` contains no claim about the gate-report set that a ninth report could falsify.

**Nothing in either record file is falsified by this report's arrival.**

---

## Findings

None.

The one sentence that changed is true on both halves, verified against the two artifacts it
names. It removed a false measurement and introduced no count, no closure over a growing set,
and no locational claim that cannot be checked — I checked the one it does make. Every standing
check was re-derived from the tree rather than inherited from RE-GATE 8's verdict, and all twelve
pass. The instruments were re-run, the sweep with a control on both arms, and the report artifact
is unmoved at `reportCreatedOn 2026.09.04-04.47.20`.

## Observations

Non-gating. None of these gates the pass.

1. **The `global.md` diff bound is line-number stability plus independent re-derivation, not a
   byte-diff — and I would rather name the limit than let strong evidence read as decisive
   evidence.** The file is uncommitted and there is one copy of it, so no snapshot of the
   RE-GATE 8 state exists to diff against. What I measured: all six line numbers RE-GATE 8 cited
   still resolve to the same content, which confines the edit to a within-line splice at
   `:71-72`; plus every other live claim in the file re-derived from scratch this pass. The
   residual risk is a second within-line edit somewhere I did not independently re-derive.
   Section 2 covers the live counts, the citations, the attribution, the routing claim and the
   claims about the gate reports, which is where the falsifiable content is. **For `tests.md`
   and the `.cpp` the mtime evidence IS decisive** — both predate RE-GATE 8's own report — so
   the brief's request there is answered without qualification. Recorded so a reader knows which
   half rests on what.

2. **`global.md:90-91`, "Gate reports name the OLD identifier", is still a universal over a set
   that is still growing.** Carried forward from RE-GATE 7 and RE-GATE 8 unchanged, and it is
   still TRUE: all nine existing reports contain the retired name and this one does too, so my
   arrival preserves it. **Left as an observation deliberately, for the third time, on the
   brief's own bar** — it is neither false nor a closure over a growing set in the sense that
   matters, because its evident subject is the reports that already exist and must not be
   edited. Scoping it to the reports written before the rename would retire the hazard at no
   cost in meaning, and that remains the cheapest available repair if anyone wants it. Reporting
   a true sentence as a finding to justify a tenth round would be exactly the failure the brief
   warned against.

3. **The instrument gap that produced findings on rounds three through seven is still there, and
   it is what made this round necessary.** `strat_banner_sweep.py` validates counts, stamps,
   provenance and ownership; it has no check for a banner's prose claims ABOUT the gate reports.
   Section 3b proves it fires on a wrong COUNT and says nothing about a wrong locational phrase —
   the sweep was `SWEEP CLEAN` on the pre-edit text with "two lines" in it, and it is
   `SWEEP CLEAN` now. A sweep check flagging any live universal, cardinality, or distance claim
   whose subject is `Tools/architect/gate_reports/` would have caught every one of these from the
   record side. That is `strat-data-steward`'s backlog, not a finding here. **Said plainly now
   that the prose is finally clean: the sentence is true because it was narrowed to something a
   search can settle, not because the instrument improved.**

4. **`global.md:72` is over-long and was not re-wrapped after the splice.** The replacement text
   was substituted in place, leaving `:72` at roughly 105 characters against the file's habitual
   95-character wrap, with the next sentence's `(2)` starting mid-line. Purely cosmetic; it
   changes nothing a reader concludes and it is the kind of untidiness the brief explicitly ruled
   out as a finding. Recorded only so nobody mistakes it for evidence that something else moved.

5. **"found on the third re-gate" is defensible but is not the only reading, and I weighed it
   before passing it.** RE-GATE 3 is where the miscount was raised as a BLOCK finding; that
   report's own severity note says it had been raised as a non-gating Observation on RE-GATE 1
   and again on RE-GATE 2. So "found" reads naturally as "found as a finding", which is true of
   RE-GATE 3, and less naturally as "first noticed", which would point at RE-GATE 1. This half of
   the sentence is UNCHANGED by this pass — it stood in the text RE-GATE 5 and RE-GATE 8 both
   read — and it is not false under its natural reading. Not a finding. Noted so the next reader
   knows it was considered rather than skipped.

6. **The sweep's bare-stamp blindness remains OPEN**, correctly recorded at `global.md:50-57`, in
   `strat-data-steward`'s lane, unfixed. The two bare stamps at `:45` and `:52` are mentions of a
   retired stamp inside the prose explaining its retirement, not live citations; every actual
   citation in the file carries the `reportCreatedOn` token, re-measured this pass in section 2j.
   Unchanged since RE-GATE 8.

7. **The `autocrlf` warning is still real and still pre-existing.** `git diff` on the changed
   `.cpp` emits *LF will be replaced by CRLF the next time Git touches it*. The file is LF in the
   worktree and LF in the `HEAD` blob and this pass preserved that. Watch it at staging time; it
   is not a change this diff made, and `global.md:72-74` records it correctly.

VERDICT: PASS
