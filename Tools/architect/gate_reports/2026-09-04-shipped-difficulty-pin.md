# Gate: the shipped-GameMode `Difficulty` asset pin — strat-integration-reviewer

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
```

THREE files, all modified, none added, none deleted, NOTHING STAGED (the first porcelain column is
a space on all three; `git diff --cached --stat` is empty). 291 insertions, 10 deletions. Seven
worktrees exist under `E:/MultiAgent/Strat-wt/` and `.claude/worktrees/`; none was audited and none
was entered.

The brief's account of the change matched the tree in every particular I checked. No sentence of it
was found wrong.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing, and the two manifests are likewise untouched |
| 2 | No `strat::` call outside `StratBridge/`+`StratRules/` | pass — zero hits; see instrument A |
| 3 | No vendored header in front of UHT | pass — the only file including `Data.h`/`Replay.h`/`Save.h`/`Scenario.h`/`Ui.h` is `Source/StratBridge/StratBridge.h`, which declares NO `UCLASS`/`USTRUCT`/`UENUM`. No new include was added by this diff |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — both members (`StratMatchSubsystem.h:2496`, `StratScoreboardHUD.h:1235`) are `TPimplPtr`; the only occurrences of `TUniquePtr<FStratBridge>` are prose recording the measured `C4150`. Unchanged by this diff |
| 5 | Module arrows unchanged and correct | pass — DERIVED, not trusted; 16 rows, 5 modules, 10 field names. See instrument B |
| 6 | New modules registered | pass — no new module. The `.uproject` lists `Stratocracy, StratBridge, StratUI, StratPlay`; `Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly ABSENT |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — the diff introduces NO new literal; the clause reuses the two existing constants. See instrument A |
| 9 | No widget-side arithmetic | pass — `Source/StratUI/` is untouched by this diff |
| 10 | Lane discipline | pass — one file under `Source/*/Tests/` (test-author's lane), `tests.md` (its own record), `global.md` (the coordinator's own file). No production C++, no `Content/`, no `Config/` |
| 11 | Prose blocks present | pass — the new clause carries a why-first block citing GDD Sec 2.11.6 and Sec 2.9 and acceptance ID T-FAME-02, and records its measurements as measurements; `DescribeDifficulty` carries its own block |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` empty; HEAD is still `9d705ca` |

## Findings

**None.** No constraint in the twelve is violated, and the adversarial premise the brief asked to
be tested holds against the tree in both halves.

## 1-2. The code, audited first per the standing gate-priority ruling

### 2a. The load-bearing premise — BOTH HALVES TRUE

The clause is asserted throughout to be a pin on the VALUE and not the AUTHORSHIP, because
`FStratMatchConfig::Difficulty` initialises to `EStratDifficulty::Easy` and `Easy` is the enum's
first value. Both halves measured against the tree, not taken from the brief:

- `Source/StratPlay/StratMatchSubsystem.h:436` — `EStratDifficulty Difficulty = EStratDifficulty::Easy;`
- `Source/StratPlay/StratMatchSubsystem.h:212-222` — `enum class EStratDifficulty : uint8 { Easy, Normal, Hard }`, no explicit initialisers, so `Easy` is 0.

The premise is CORRECT. It follows that the clause's self-description — and every record entry
resting on it — is sound, and that the DELIBERATE ABSENCE of a premise block is right rather than
an omission: the sibling clause's "the C++ default cannot supply the expectation" is FALSE on this
field and, if copied, would have produced a clause that fails on a healthy tree.

### 2b. The GDD quote — verbatim

Grepped, never `cat`-ed:

```
$ grep -o "The first match runs on the one shipped scenario at [^.]*\." \
    E:/MultiAgent/stratocracy-content/Stratocracy_Prototype_GDD.md
The first match runs on the one shipped scenario at **Easy** by default (player +150 opening Fame, §2.
```

The clause quotes this verbatim apart from rendering the GDD's section sign as `Sec`, which is this
tree's standing ASCII transliteration (non-ASCII in these files has been measured coming back as
U+FFFD). Not a misquote.

### 2c. Does ANYTHING read as claiming an authorship pin?

Every mention of the clause across `Source/` and `Tools/`:

```
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:77   (header bullet handing Difficulty over)
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:369,370,373  (the macro and RunTest)
Tools/architect/state/global.md:33, 139
Tools/architect/state/tests.md:24
```

Every prose mention sits inside, or immediately adjacent to, an explicit disclaimer.
`global.md:139` claims only that the re-authoring hazard "is now caught ... goes red on a shipped
`Hard`" — a VALUE claim and a true one. NO prose in any of the three files cites this clause as
evidence that a designer set the tier. The conflation the 2026-09-03 pass existed to repair has NOT
been re-introduced in prose. One residual NAMING hazard is recorded under Observations.

### 2d. The liveness control's limitation — STATED IN EVERY PLACE THE CONTROL IS DESCRIBED

The brief asked whether the "measured on a different field" caveat is everywhere or only in some
places. It is in ALL FIVE places, including the one a reader reaches without opening a file:

| Site | Form |
|---|---|
| `StratShippedGameModeMatchConfig.cpp:357` | "THE LIVENESS EVIDENCE THEREFORE COMES FROM A DIFFERENT FIELD THAN THE ONE BEING PINNED" |
| `StratShippedGameModeMatchConfig.cpp:401` | "THE CONTROL: the reader is alive -- MEASURED ON A DIFFERENT FIELD" |
| `StratShippedGameModeMatchConfig.cpp:416` | inside the ASSERTION MESSAGE: "LIVENESS CONTROL, ON A DIFFERENT FIELD" |
| `Tools/architect/state/tests.md:44` | "measured on a DIFFERENT FIELD, and that is the caveat to carry forward" |
| `Tools/architect/state/global.md:57` | "MEASURED ON A DIFFERENT FIELD, WHICH IS A WEAKER WARRANT AND IS LABELLED AS ONE IN EVERY PLACE IT APPEARS" |

That last sentence is itself a claim about coverage, and it is TRUE as measured.

### 2e. Every failure path is RED and none skips

`ResolveCdoOrFail` (`StratShippedGameModeMatchConfig.cpp:157-209`) records a failure through `Test`
on each of its four exits — class did not load, class is native, class is not an `AStratGameMode`,
CDO null — and returns `nullptr` only after the test is already red. The new `RunTest` calls it
TWICE and does `return false;` on each null. There is no warn-and-continue, no early `return true`,
and no skip anywhere in the clause. Correct, and it honours the discipline the file's header
declares.

### 2f. `DescribeDifficulty` is display-only

`StratShippedGameModeMatchConfig.cpp:141-148`. Its return value reaches exactly one place — the
`FString::Printf` that builds the assertion's MESSAGE. Nothing is asserted about the reflected
string; the assertion's subject is `ShippedTier` against `EStratDifficulty::Easy`, an enum
comparison that does not involve the name at all. Its null-`StaticEnum` branch degrades to
`<unreflected tier N>` in the message rather than crashing or changing the verdict. Correct, and
correctly reasoned in its own comment block.

## 3. The record

### 3a. THE >400-CHARACTER MEASUREMENT — the brief's blocking question

**Measured, not reasoned: the nearest `[STAMPED` marker to the live citation is 4425 characters
away, and there is NO `[STAMPED` marker anywhere inside the paragraph that contains it.**

Instrument C (inlined below) reports, against `Tools/architect/state/global.md` in this tree:

```
live citation char offset in file: 2605
occurrences of live citation: 1
total [STAMPED markers in file: 84
nearest [STAMPED BEFORE: offset None distance None
nearest [STAMPED AFTER : offset 7030 distance 4425
--- containing paragraph: file offset 850 len 5446
citation offset within paragraph: 1755
[STAMPED markers inside this paragraph: []
nearest in-paragraph BEFORE dist: None
nearest in-paragraph AFTER  dist: None
```

4425 is more than ten times the 400-character window. The provenance check is NOT inert on this
banner, so the sweep's CLEAN verdict on it is worth what it says. TWO further facts make that
robust rather than lucky:

1. The sweep's window is taken WITHIN a paragraph (`strat_banner_sweep.py:1372-1374`:
   `ws = max(0, m.start() - _PROVENANCE_CITATION_WINDOW)` sliced from `para`, not from the file),
   and the containing paragraph — the whole new banner, 5446 characters with no blank line —
   contains ZERO `[STAMPED` markers. Nothing could leak in across the paragraph boundary even at a
   much larger window.
2. Part (a) skips a citation outright when it EQUALS the report actually opened
   (`strat_banner_sweep.py:1352-1353`, `if stamp == result.report_created_on_raw: continue`).
   `2026.09.04-04.09.26` is exactly what the report says, so this citation is not the kind the
   window governs in the first place. Both routes give the same answer.

### 3b. The three stamps

| Stamped claim | Where | Formed correctly? |
|---|---|---|
| "The suite is 387/387 ..." | `global.md:87-91` | yes — `[STAMPED 2026-09-04 -- superseded ...]` followed by the old text re-quoted under `STAMPED>` prefixes. No live `387/387` survives: both remaining occurrences (`:89`, `:159`) are inside a `STAMPED>` quote or inside a stamp bracket |
| "THE LIVE FIGURE'S REPORT IS reportCreatedOn 2026.09.04-03.04.52" | `global.md:95-98` | yes — stamped "ONE REPORT PATH, ONE ARTIFACT" and re-quoted under `STAMPED>`, correctly stating the artifact was overwritten and is no longer re-readable |
| "WHAT IS STILL UNPINNED ... Difficulty" | `global.md:135-147` | yes, and it is the best of the three: it stamps ADDRESSED IN PART, says what the debt got WRONG (that it read as closable by a clause at all), and carries the still-open half forward rather than closing it |

No stamped claim still reads as live. The one remaining bare mention of the superseded stamp,
`global.md:105` ("the run is 2026.09.04-03.04.52 by the report and 2026-09-03 locally"), is a
date-reconciliation sentence inside the already-superseded banner; it asserts no live figure and it
sits outside the current banner segment, which is where part (a) scopes itself. The same stale
citation in `tests.md:90` carries its own inline `[STAMPED 2026-09-04 ...]` block.

### 3c. Ownership of the figure

`global.md` carries the only live count: `388/388` at `:28`. `tests.md`'s new entry carries NO
number and cites `reportCreatedOn 2026.09.04-04.09.26` instead, exactly as the convention requires,
and repeats the rule explicitly ("never by a number -- the figure lives in
`Tools/architect/state/global.md` and nowhere else"). No non-`global.md` record file carries a live
`N/N` suite figure or a phase verdict. The sweep independently agrees:
`suite claims found: 61 (1 live, 60 stamped)`.

### 3d. Attribution

- `tests.md:17` — "`strat-test-author` (ACTING and WRITING; IN LANE ... base commit `9d705ca`,
  UNCOMMITTED)". Acting and writing named INLINE in the entry; base commit named; NO exception
  clause claimed, and none was needed.
- `global.md:14-19` — written by the `coordinator`, "whose file this is, IN LANE AND UNDER NO
  EXCEPTION CLAUSE -- this seat wrote no code and no clause", and states explicitly "This entry is
  not a transcription of anything." That is the correct posture: `global.md`'s writer is already
  the coordinator, so limit (4) of the transcription clause puts it outside that clause's reach
  entirely, and the entry does not reach for it.
- The banner additionally records that BOTH routes were offered and the user CHOSE DISPATCH, with
  each route's cost named. That is obligation (1) of the in-session clause discharged in the one
  form a checkout can hold — and, as `CLAUDE.md` itself declares, obligation (1) is unfalsifiable
  from a checkout. I record that I verified the entry SAYS it, not that it happened.

No file restates a fact `global.md` owns.

## 4. The instruments — re-run here, INLINED so a checkout can re-execute them

A persisted report that cites a scratchpad path is unfalsifiable, so every instrument below is
present as text rather than as a path.

### Instrument A — the standing architectural greps

```bash
cd E:/MultiAgent/Stratocracy
grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ --include=*.cpp --include=*.h \
  | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'      # -> empty
for h in $(grep -rlE 'UCLASS|USTRUCT|UENUM' Source/ --include=*.h); do
  grep -qE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' "$h" && echo "HIT: $h"
done                            # -> StratBridge.h only, which itself declares no reflection macro
git diff -U0 -- Source/ | grep '^+.*/Game/'                            # -> empty: no NEW literal
```

The `/Game/` literals the clause reuses (`kShippedGameModeClassPath`, `kAiVsAiGameModeClassPath`,
at `StratShippedGameModeMatchConfig.cpp:112-117`) live in `Source/StratPlay/Tests/` and are covered
by `CLAUDE.md`'s automation-test-fixture exception, which the file's own header at `:29-33` reasons
correctly. The ONLY `/Game/` literal anywhere in `Source/` outside a `Tests/` directory is
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the import commandlet — the other
standing exception. Neither is touched by this diff.

### Instrument B — the module-arrow derivation and the field census

Both extracted from the reviewer checklist and EXECUTED against this tree, not read.

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
done
```

Output — 16 rows, all five modules present (a module missing would itself be a finding):

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

Row 3 is the CONDITIONAL sixteenth row, and the extraction is textual and cannot show that it is
conditional, so the file was read for the branch:

```
StratBridge.Build.cs:133  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
StratBridge.Build.cs:134  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (...));
StratBridge.Build.cs:135  if (!bCompileVendoredRulesHere)
StratBridge.Build.cs:137      PrivateDependencyModuleNames.Add("StratRules");
```

Guarded, monolithic-only, from the ONE expression — so the dependency half and the compile half
cannot drift apart. `StratRules` appears in `PublicDependencyModuleNames` NOWHERE, and in
`PrivateDependencyModuleNames` ONLY inside the guard. That is the permitted shape and is NOT a
finding. `StratUI` has not gained a `Stratocracy` dependency. No cycle. `StratBridge -> Stratocracy`
is the deliberate, correct edge. NOTHING in this diff touched any `.Build.cs`.

`.agents/ue-project-context.md:40` and `:113` describe this conditional edge accurately, naming the
`Target.LinkType != TargetLinkType.Monolithic` expression. NO DRIFT to report in the context
document this pass.

The field census, which exists because the three graph fields are themselves a typed subject list:

```bash
for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done |
  grep -oE '[A-Za-z]+[[:space:]]*(\.AddRange|\.Add|=)' | sed 's/[[:space:]]*$//' | sort -u
```

TEN names, exactly the documented set, and no eleventh:

```
BRIDGE=                                 <- the KNOWN FALSE POSITIVE: the tail of the string
PCHUsage =                                 literal "STRAT_VENDORED_RULES_IN_BRIDGE=" inside
PrivateDefinitions.Add                     PrivateDefinitions.Add. Not a field. Not chased,
PrivateDependencyModuleNames.Add           and deliberately not sed-ed away either.
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
```

### Instrument C — the >400-character provenance measurement

```python
import re, io
p = r"E:\MultiAgent\Stratocracy\Tools\architect\state\global.md"
text = io.open(p, encoding="utf-8").read()
LIVE = "reportCreatedOn 2026.09.04-04.09.26"
i = text.find(LIVE)
print("live citation char offset in file:", i)
print("occurrences of live citation:", text.count(LIVE))
marks = [m.start() for m in re.finditer(re.escape("[STAMPED"), text)]
print("total [STAMPED markers in file:", len(marks))
before = [m for m in marks if m < i]
after  = [m for m in marks if m > i]
print("nearest [STAMPED BEFORE: offset", before[-1] if before else None,
      "distance", (i - before[-1]) if before else None)
print("nearest [STAMPED AFTER : offset", after[0] if after else None,
      "distance", (after[0] - i) if after else None)
paras = re.split(r"\n\s*\n", text)
off = 0
for para in paras:
    j = text.find(para, off)
    if j <= i < j + len(para):
        k = i - j
        print("--- containing paragraph: file offset", j, "len", len(para))
        print("citation offset within paragraph:", k)
        pm = [m.start() for m in re.finditer(re.escape("[STAMPED"), para)]
        print("[STAMPED markers inside this paragraph:", pm)
        pb = [m for m in pm if m < k]; pa = [m for m in pm if m > k]
        print("nearest in-paragraph BEFORE dist:", (k-pb[-1]) if pb else None)
        print("nearest in-paragraph AFTER  dist:", (pa[0]-k) if pa else None)
        break
    off = j + len(para)
```

Its output is quoted in full in section 3a above.

### Instrument D — the banner sweep, with a REAL exit code and a LIVENESS CONTROL

The exit code was captured into a variable BEFORE anything read the output, because `$?` after a
pipe reports the last stage and a false "exit 0" has been recorded in this project before:

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
  automation report: 388 entries, all Success (2026.09.04-04.09.26)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788494967, reportCreatedOn='2026.09.04-04.09.26'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

**A CLEAN RESULT IS AN ABSENCE, AND AN ABSENCE NEEDS A CONTROL.** The same binary was shown able to
speak, again with the exit code captured before any pipe:

```bash
python Tools/architect/strat_banner_sweep.py --self-test > st.out 2>&1
rc=$?; echo "SELF-TEST REAL EXIT CODE: $rc"    # -> 0
tail -1 st.out                                 # -> SELF-TEST: ALL FIXTURES CORRECT
```

59 lines, every fixture `[OK]`, and TWO of them bear directly on the question this gate was asked:

- `[OK] a wrong citation buried in a long paragraph, far from two UNRELATED stamp markers elsewhere
  in it, still FAILS: expected FAIL, got FAIL -- REPORT PROVENANCE` — the fixture that pins the
  400-character windowing itself.
- `[OK] the pre-fix whole-paragraph is_stamped call launders the buried citation (True) while the
  shipped windowed call does not (False), on the identical paragraph and match` — a regression pin
  asserted against the SHIPPED module objects, so it cannot go on naming a fix no longer present.

So the CLEAN above is a measurement and not a silence.

### Instrument E — the automation report, read with `utf-8-sig`

The log undercounts by exactly one every run and was NOT used. The exported report was opened
directly:

```python
import json, io, collections
p = r"E:\MultiAgent\Stratocracy\Saved\AutomationReport\index.json"
d = json.load(io.open(p, encoding="utf-8-sig"))
for k in ("reportCreatedOn","succeeded","succeededWithWarnings","failed","notRun","totalDuration"):
    print(f"{k:24} {d.get(k)}")
tests = d.get("tests", [])
print("entry count (len(tests))", len(tests))
print("state histogram:", dict(collections.Counter(t.get("state") for t in tests)))
NAME = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty"
print("new clause present BY NAME:", any(t.get("fullTestPath")==NAME for t in tests))
```

```
reportCreatedOn          2026.09.04-04.09.26
succeeded                388
succeededWithWarnings    0
failed                   0
notRun                   0
totalDuration            4.621513366699219
entry count (len(tests)) 388
state histogram: {'Success': 388}
new clause present BY NAME: True
  fullTestPath: Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty
  state       : Success
  warnings    : 0 errors: 0
sibling present: True
```

`succeeded` 388 equals `len(tests)` 388 equals the state histogram's only bucket. The banner's
`388/388` and its `reportCreatedOn` citation are both correct against the artifact.

**The artifact is also pinned in time, independently of the sweep's own check 4:**

```
report mtime      : 1788494966.98  (2026-09-04 00:09:26 local)
newest source     : 1788494861.12  (2026-09-04 00:07:41 local)
  path            : Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
REPORT NEWER THAN SOURCE: True  delta(s): 105.9
```

The report POSTDATES the last source edit by 106 seconds, so it describes THIS tree and not a
previous one.

### Instrument F — my own macro-based set difference, ONE collector over BOTH sides

A single-line grep on the macro returns zero because the macro wraps across lines, so the pattern
is `re.S` and spans them. One function collects both sides, so the difference measures the TREES
and not two collectors.

```python
import re, subprocess, io, os
REPO = r"E:\MultiAgent\Stratocracy"
MACRO = re.compile(
    r"IMPLEMENT_(?:SIMPLE|COMPLEX)_AUTOMATION_TEST\s*\(\s*[A-Za-z_][A-Za-z0-9_]*\s*,\s*"
    r'(?:TEXT\s*\(\s*)?"([^"]+)"', re.S)

def collect(get_text, paths):                  # ONE collector, used on BOTH sides
    out = set()
    for p in paths:
        t = get_text(p)
        if t is None: continue
        out |= set(MACRO.findall(t))
    return out

def git(*a):
    return subprocess.run(["git","-C",REPO,*a],capture_output=True,text=True,check=True).stdout

head_paths = [p for p in git("ls-tree","-r","--name-only","HEAD").splitlines()
              if p.endswith(".cpp") and p.startswith("Source/")]
def head_text(p):
    r = subprocess.run(["git","-C",REPO,"show",f"HEAD:{p}"],capture_output=True,text=True)
    return r.stdout if r.returncode==0 else None

wt_paths = []
for root,_,files in os.walk(os.path.join(REPO,"Source")):
    for f in files:
        if f.endswith(".cpp"):
            wt_paths.append(os.path.relpath(os.path.join(root,f),REPO).replace("\\","/"))
def wt_text(p):
    try: return io.open(os.path.join(REPO,p),encoding="utf-8",errors="replace").read()
    except OSError: return None

H = collect(head_text, head_paths); W = collect(wt_text, wt_paths)
print("HEAD clause count      :", len(H))
print("WORKTREE clause count  :", len(W))
print("ADDED   (+):", len(W-H)); [print("   +", n) for n in sorted(W-H)]
print("REMOVED (-):", len(H-W)); [print("   -", n) for n in sorted(H-W)]
print("collector non-vacuity: found >0 on both sides:", len(H)>0 and len(W)>0)
```

```
HEAD clause count      : 387
WORKTREE clause count  : 388
ADDED   (+): 1
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty
REMOVED (-): 0
collector non-vacuity: found >0 on both sides: True
```

**+1 added, 0 removed, 387 -> 388, and the added name is the one the banner names.** This agrees
with the report (`succeeded` 388), with the sweep's independent macro census
(`IMPLEMENT_SIMPLE_AUTOMATION_TEST=388`), and with `global.md`'s claim — by four routes that do not
share an implementation.

## Observations

Non-gating.

1. **THE CLAUSE'S NAME SAYS `Authors`, AND THE CLAUSE EXPLICITLY CANNOT PIN AUTHORSHIP. This is the
   sharpest thing in the pass, and I am recording deliberately that it is NOT a block.**
   `ShippedGameModeAuthorsEasyDifficulty` sits beside `ShippedGameModeAuthorsOneAiSide`, where the
   same verb IS earned — that clause asserts, first and fatally, a value the C++ default cannot
   supply. On `Difficulty` it is not earned, and every prose block in the tree spends paragraphs
   saying so. The NAME is the one artifact that carries none of that context: it is what appears in
   `index.json`, in a CI summary, in a grep, and in the "+1 added" line of two record files. A
   reader who sees only the name concludes exactly the thing five separate disclaimers exist to
   prevent — the same conflation, one field over, that the 2026-09-03 pass was run to repair.
   It is not a block because (a) no prose anywhere asserts the false claim, (b) the file's own
   header at `:76-84` intercepts a reader arriving by the name within a few lines, and (c) the
   assertion message itself ends "Do not cite this clause as evidence that a designer set the
   tier." A rename — `ShippedGameModeRunsAtEasy` or similar — would also move the suite's
   clause-name set and every citation of it, so it is a lane decision, not a gate fix.
   Owner if pursued: `strat-test-author`.
2. **The `autocrlf` warning `tests.md` reports is real and is pre-existing.** `git diff` on the
   `.cpp` emits "warning: in the working copy of ... LF will be replaced by CRLF the next time Git
   touches it". The lane's account is correct: the file is LF in the worktree and LF in the HEAD
   blob, and this pass preserved that. Worth watching at staging time; not a change this diff made.
3. **The liveness control compares STRINGS, not arrays.** `TestNotEqual(..., Describe(A), Describe(B))`
   compares the rendered `(1)` against `(0,1)` rather than the two `TArray<int32>`s. It is correct
   here — the values are digits and commas, so the case-insensitivity of `FString` comparison in
   this engine cannot bite — and the string form is what the failure message needs anyway. Recorded
   only so that a future edit rendering a case-bearing field through the same helper knows the
   property is not free.
4. **Obligation (1) of the in-session clause is, as `CLAUDE.md` itself declares, unfalsifiable from
   a checkout.** `global.md`'s banner states both routes were offered with their costs named and
   that the user chose dispatch. I verified that the entry SAYS it, in the required inline shape. I
   cannot verify that it HAPPENED, and I record which of the two I did.

VERDICT: PASS
