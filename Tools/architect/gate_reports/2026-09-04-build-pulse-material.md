# Gate: W8 build-pulse material (`M_OverlayPulse` + `MI_Overlay_BuildPulse` reparent) — strat-integration-reviewer

## Scope

Audited the **uncommitted working tree** of `E:/MultiAgent/Stratocracy`, branch `master`,
against base `4e4e2d1ca043ae5113dfcbf9789192206e589b17` (verified with `git rev-parse HEAD`).
All paths below are absolute or repo-relative to that tree; nothing in this report was read
from a worktree. `git worktree list` shows seven worktrees standing, none of them this one.

`git status --porcelain` — **four paths, one of them staged**:

```
 M Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
AM Content/StratArt/Materials/M_OverlayPulse.uasset
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
```

`git diff --stat 4e4e2d1`:

```
 .../Materials/MI_Overlay_BuildPulse.uasset         |  4 +-
 Content/StratArt/Materials/M_OverlayPulse.uasset   |  3 +
 Tools/architect/state/content.md                   | 99 ++++++++++++++++++++++
 Tools/architect/state/global.md                    | 15 ++++
 4 files changed, 119 insertions(+), 2 deletions(-)
```

Four files. No `Source/`, no `Config/`, no `Data/`, no `.uproject`, no Blueprint — the
coordinator's claim on that point is confirmed by the diff, not taken.

Read in full or in part, all under `E:/MultiAgent/Stratocracy/`:
`.agents/ue-project-context.md`, `Tools/architect/state/content.md` (header and lines 355-395),
`Tools/architect/state/global.md` (lines 1-60, 625-680, 4295-4345),
`Source/StratBridge/StratBridge.Build.cs`, `Stratocracy.uproject`,
`Source/StratRules/StratRules.manifest.json`, `Data/StratData.manifest.json`,
`Saved/Config/WindowsEditor/SourceControlSettings.ini`, and the raw bytes of every
`Content/StratArt/Materials/MI_*.uasset` plus `M_OverlayPulse.uasset`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered (unless no module object) | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass |
| 9 | No widget-side arithmetic | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | **BLOCK** |

### How each was run

**1.** `git diff --stat -- Source/StratRules Data` printed **nothing**. Also
`git diff --stat 4e4e2d1 -- Source/StratRules Data` — nothing. The manifests were read for the
authority rather than quoted from the checklist:
`Source/StratRules/StratRules.manifest.json` -> `rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`,
`Data/StratData.manifest.json` -> `dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`. Unmoved.

**2.** `grep -rnE "strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(" Source/ --include=*.cpp --include=*.h`
minus `Source/StratBridge/` and `Source/StratRules/` returned **zero lines**. No judgement calls
were needed this pass because there were no hits at all; `Source/` is byte-untouched by this diff.

**3, 4, 9.** No header, no `.cpp` and no widget changed in this diff (`git diff --stat` above lists
no `Source/` path). The nineteen headers that name `StratBridge.h` are unchanged from `4e4e2d1`,
which the last gate passed. Recorded as **pass on an unchanged subject**, not as a fresh
re-derivation of each file.

**5.** The derivation was run, not trusted. `Source/*/*.Build.cs` with comments stripped printed
**16 rows** — five modules times three fields, plus the conditional
`StratBridge PrivateDependencyModuleNames StratRules`:

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

All five modules present — an absent module would itself have been a finding. Row 3 was then
read at its source rather than treated as an unconditional arrow;
`Source/StratBridge/StratBridge.Build.cs:133-137`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
```

Guarded, one expression driving both halves, monolithic-only. Correct and not a finding.
`StratUI` has no `Stratocracy` dependency; no cycle. The field census printed **ten** names —
`BRIDGE=`, `PCHUsage =`, `PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`,
`PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`,
`bUseUnity =`. Ten, as expected; no eleventh name. `BRIDGE=` is the known false positive (the
tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="`) and was neither chased nor
filtered away.

**6.** `Stratocracy.uproject` `Modules` = `['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']`,
unchanged. `grep -rl IMPLEMENT_MODULE` over `Source/` returns exactly those four modules'
`.cpp` files; `Source/StratRules/` carries none and is correctly absent from the array. No new
`Source/<Module>/` directory in this diff.

**7.** `git diff --stat -- Source/Stratocracy` printed **nothing**.

**8.** `grep -rn "/Game/" Source/` returns hits only under `Source/StratBridge/Tests/` — e.g.
`Source/StratBridge/Tests/StratBridgeParity.cpp:89`,
`UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));` — which is the
standing automation-fixture exception. No gameplay-C++ literal, and no `Source/` file moved
this pass. The new `/Game/StratArt/Materials/M_OverlayPulse` path exists only inside the
`.uasset` bytes and the record prose, never in C++.

**10.** Content-lane asset work performed by the `coordinator`. `Tests/` untouched, no
production code in `Tests/`, no test code outside it. See Observation A on the route.

**11.** Both record files carry why-first prose in house style; `.agents/ue-project-context.md`
was checked for drift against the derived arrows and is **correct as written** — its
`StratBridge` row already states the include edge and the monolithic-only link edge and names
`Target.LinkType != TargetLinkType.Monolithic` as their common source, and its `Source/StratRules/**`
row already defers to the manifest rather than quoting a commit.

**12. BLOCK — see Finding 1.**

## Findings

### Finding 1 — item 12, and it is worse than "something is staged"

**Constraint violated:** *Agents do not commit. Unexpected staged content is a finding.*

`E:/MultiAgent/Stratocracy/Content/StratArt/Materials/M_OverlayPulse.uasset` is **staged**, and
what is staged is **not the material this pass authored**. It is the intermediate, pre-rewire
duplicate.

`git ls-files -s Content/StratArt/Materials/M_OverlayPulse.uasset`:

```
100644 c1c9d5454366139e06de3bbeff27d040a5e6786a 0	Content/StratArt/Materials/M_OverlayPulse.uasset
```

`git cat-file -p c1c9d54` — the staged LFS pointer:

```
version https://git-lfs.github.com/spec/v1
oid sha256:64ec50f34132607680e503454fc0108b8038c6b0e2baf922c4378c66d3ad18bf
size 6477
```

The working-tree file hashes to a different pointer —
`git hash-object --path <that file> <that file>` -> `0dc3a87ecda23bcd2f173a52af39892f001becf9`,
pointing at `oid 3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24`, `size 11384`.

The staged LFS object is present locally at
`E:/MultiAgent/Stratocracy/.git/lfs/objects/64/ec/64ec50f34132607680e503454fc0108b8038c6b0e2baf922c4378c66d3ad18bf`,
so it could be read directly rather than inferred from the size. Scanning its printable
strings for material-expression class names returns, in full:

```
MaterialExpressionCollection
MaterialExpressionEditorX
MaterialExpressionEditorY
MaterialExpressionGuid
MaterialExpressionVectorParameter
```

**No `MaterialExpressionTime`. No `MaterialExpressionSine`. No `MaterialExpressionMultiply`,
`Add` or `LinearInterpolate`. No `MaterialExpressionScalarParameter`, and no `PulseSpeed`,
`PulseMin` or `PulseMax`.** It is a bare duplicate of `M_Translucent` — one vector parameter —
captured by the editor's auto-add the instant `duplicate_asset` saved it, before the graph was
rewired.

The same scan over the **working-tree** file (11384 bytes) returns exactly what the entry
claims: `MaterialExpressionTime`, `MaterialExpressionSine`, `MaterialExpressionMultiply`,
`MaterialExpressionAdd`, `MaterialExpressionLinearInterpolate`,
`MaterialExpressionScalarParameter`, `MaterialExpressionVectorParameter`, `PulseSpeed`,
`PulseMin`, `PulseMax`, `BLEND_Translucent`.

`git lfs status` confirms which object is on the commit path:

```
Objects to be committed:

	Content/StratArt/Materials/M_OverlayPulse.uasset (LFS: 64ec50f)
```

**The operational consequence, which is why this gates.** A `git commit` run against this index
as it stands — without a further `git add` — commits a material that **does not pulse**, under
the name of a pass whose entire subject is that it pulses. The instance reparent is unstaged, so
a plain `git commit` would land nothing at all on `MI_Overlay_BuildPulse` and a static master
beside it: the shipped defect the pass exists to fix, wearing the fix's commit message.

`Tools/architect/state/content.md` **does** name the staging, in its last bullet, and names the
measured cause correctly — `Saved/Config/WindowsEditor/SourceControlSettings.ini` reads
`Provider=Git` (verified: that is the file's entire body under
`[SourceControl.SourceControlSettings]`), and the modified `MI_Overlay_BuildPulse` being unstaged
is a sound tell. But the bullet says only:

> `M_OverlayPulse.uasset` is STAGED and no `git add` was run.

It does not say that **the staged bytes are stale and are not the delivered asset**. A reader who
takes that bullet at face value would conclude the index merely runs ahead of the user's decision,
when in fact the index holds a different, wrong material. That is the gap between a disclosed
condition and a disclosed hazard, and item 12 is binary.

**Owner of the fix:** the `coordinator` (it holds this tree and wrote both record files). The
correction is two parts and neither is mine to make: (a) `git restore --staged
Content/StratArt/Materials/M_OverlayPulse.uasset` so nothing is staged, leaving the working-tree
bytes untouched and the commit decision entirely the user's, per `CLAUDE.md` non-negotiable 6;
and (b) amend the `content.md` staging bullet to record that the auto-added blob was the
**pre-rewire** duplicate, since that is the part with teeth and it is the part a future reader on
this route needs. Both are content-lane / record writes, not the reviewer's.

## Observations

*Non-gating.*

**A — the route is outside the clause that is cited for it, the entry says so, and the flagging is
honest and sufficient.** The pass was driven by a headless
`UnrealEditor-Cmd.exe -run=pythonscript` commandlet. `CLAUDE.md`'s editor-driver clause is written
about *driving the LIVE editor*. `content.md`'s new entry does not claim coverage — it carries a
dedicated bullet, **"SO THE ROUTE WAS NEITHER OF THE TWO THIS FILE HAS RECORDED, AND WHETHER IT
SATISFIES THE CLAUSE IS THE USER'S TO RULE"**, and states the distinction in terms: *"the route is
not the one the clause names, and that is flagged here rather than assumed covered."* I judge that
sufficient: it is prominent, inline, and names the open question rather than burying it. Two
smaller notes. (i) There is a real tension in citing the clause as the authority for the acting
(*"ACTING: the coordinator, under CLAUDE.md's EDITOR-DRIVER CLAUSE"*) in the same entry that says
the clause may not reach this route — but both halves are stated, in the same entry, so no reader
is misled, and the alternative (acting under no named authority at all) would be worse. (ii)
`global.md`'s stamp cites the clause for the acting **without** the caveat, but it does point at
the `content.md` entry as *"the authority on ... the route question it leaves for the user to
rule"*, so the pointer is intact. Not a finding. Worth recording that this is another pass where a
clause condition was met in substance by a route the clause does not name — `CLAUDE.md` itself
treats a repeated exception as a condition rather than an accident, and this route has now been
taken once.

**B — attribution is correctly split, and both fallback obligations are discharged in the entry.**
`content.md`'s header requires the editor-driver clause for the acting and this file's own
fallback for the write, and calls one authority for both a finding. The new entry reads
*"ACTING: the coordinator, under CLAUDE.md's EDITOR-DRIVER CLAUSE. WRITING: the coordinator, under
THIS FILE'S FALLBACK CONDITION."* — two authorities, two halves, inline. Obligation (1), measure
the absence with a control, is discharged with a genuine control in a single lookup:
`mcp__NeoStack_Connect__execute_script` and `mcp__unreal-editor-direct__execute_script` absent,
while `mcp__NeoStack_Connect__unreal_status` and `mcp__NeoStack_Connect__list_unreal_projects` —
same server, same lookup — answered. Obligation (2), name the coordinator as writer, is discharged
in the same sentence. The base commit `4e4e2d1` and the "no worktree and no merge" condition are
both named. Pass.

**C — record ownership holds, and the sweep was re-run rather than taken.**
`python Tools/architect/strat_banner_sweep.py`, run in `E:/MultiAgent/Stratocracy` **without a
pipe**, printed:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 389 entries, all Success (2026.09.04-16.30.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
  report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
  suite claims found: 62 (1 live, 61 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

exit **0**. `--explain` lists the single LIVE claim at **`global.md:32`, `389/389`**; every one of
the other 61 is called stamped. The new `content.md` entry states no `N/N` and no verdict — it says
so explicitly, *"No suite count and no verdict is stated here; both are `global.md`'s alone"* — and
neither stamp adds one. The `global.md` stamp says *"With this, W8 is closed"*, which is a
milestone-status claim in the one file entitled to make it. Correct.

**D — one sentence the stamps retire is still readable as an assertion elsewhere, in a block whose
own stamps are about something else.** The sweep does not collect claims of this shape, so this was
grepped by hand. `grep -rn -i "does not pulse|static tint"` (alternation) over
`Tools/architect/state/` returns five hits. Four are accounted for: `content.md:362` and `:367` are
the bullet stamped by this pass at `content.md:369`; `global.md:4315` is the bullet stamped at
`:4318`; `engine.md:2074` is about a C++ property being unset at the time it was written and is
scoped to that pass, not a claim about today's tree. The fifth is
**`Tools/architect/state/global.md:660`**:

> AND IT STILL DOES NOT PULSE. A static tint ships where Sec 2.11.5 asks for a pulse, because
> the master material family has no time input and this route cannot author a material graph.

That is **both** of the claims this pass retires, in one sentence — the pulse claim and the
narrowed "this route cannot author a material graph" claim — and neither new stamp reaches it. It
sits inside a `_Last run 2026-09-02` banner block, and that block *is* closed by a stamp
immediately below it whose text is *"it is history now, and never evidence. Kept rather than
deleted"* — so the block is formally retired, and this is **not** being called a finding. But the
retiring stamp's stated subject is the **suite figure and the overwritten report**, not the pulse,
and a reader arriving at `:660` by a grep for "does not pulse" — exactly how this reviewer arrived
— meets a flat present-tense assertion about the tree that is now false, with the nearest stamp
visibly about something else. Worth a one-line stamp on the next `global.md` write; the reasoning
the `content.md` stamp gives for stamping in place — *"so a reader arriving by a citation lands on
the correction"* — applies to this line too.

**E — the entry claims exactly as much as it measured about the compile, and no more.** This was
probed specifically. The entry states, in capitals, *"THEREFORE NOTHING IN THIS TREE ESTABLISHED
THAT THE MATERIAL COMPILES"*, and records all three silent instruments as **silent rather than
clean**: `get_statistics` all-zero on the known-good `M_Translucent` as well as on the new master
(which is how the instrument was known dead rather than the material bad), zero `LogMaterial` lines
with a **deliberately broken control that also logged nothing** — so the instrument is recorded as
unable to discriminate, which is the honest form — and the later editor log naming the material
zero times. It then says the human sighting is what establishes the compile, and gives the
inference rather than asserting equivalence: *"a material with translation errors renders as the
default checker and does not animate."* That is a stated inference from a stated observation, not a
measurement being laundered. It also records the disambiguating question it asked the user (preview
sphere vs. in-match) together with its answer, which is the part that makes the sighting
load-bearing at all. **The entry does not let the human sighting stand in for a measurement it did
not make.** One thing worth naming: the sighting lives in the session transcript and in no
checkout, so it is unfalsifiable from the tree — the entry does not say that of itself, though
`global.md`'s 2026-09-02 block already established the convention of saying it. Minor.

**F — the graph and the controls were re-derived independently from the saved bytes, and they
hold.** The entry's walker result was not taken. Scanning the printable strings of
`E:/MultiAgent/Stratocracy/Content/StratArt/Materials/M_OverlayPulse.uasset` (11384 bytes) yields
`MaterialExpressionTime`, `MaterialExpressionSine`, `MaterialExpressionMultiply`,
`MaterialExpressionAdd`, `MaterialExpressionLinearInterpolate`,
`MaterialExpressionScalarParameter`, `MaterialExpressionVectorParameter`, the three parameter
names `PulseSpeed` / `PulseMin` / `PulseMax`, and `BLEND_Translucent`. This confirms the node
*set* and the parameter *names*; it does **not** confirm the wiring, which no instrument available
to a read-only gate can reach, and it is not reported as if it did. The controls hold
independently: `MI_Overlay_BuildPulse.uasset` now names
`/Script/Engine.Material'/Game/StratArt/Materials/M_OverlayPulse.M_OverlayPulse'` as its parent,
while `MI_Overlay_Objective`, `MI_Overlay_Reach`, `MI_Overlay_Target`, `MI_Marker_Flag`,
`MI_Marker_Guided` and `MI_Marker_Pip` all still name
`/Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent` — exactly one instance moved, and
`M_Translucent` itself does not appear in the diff at all. `_TmpBrokenProbe` is gone from both disk
and index (a `git ls-files` filter and a `find` over the tree both return nothing), so the
hand-deletion the entry records is confirmed complete.

**G — one claim in the entry could not be verified from this checkout, and is recorded as not run
rather than as clean.** The entry states `PythonScriptPlugin` is enabled *transitively* by
`PCGPythonInterop`. `Stratocracy.uproject` does list `PCGPythonInterop` as enabled (confirmed) and
lists no `PythonScriptPlugin` of its own (confirmed) — but `PCGPythonInterop.uplugin` is not in
this repository (`find . -name "PCGPythonInterop.uplugin"` returns nothing; it lives in the engine
or marketplace install), so **the transitive dependency itself was not verified here**. The
commandlet demonstrably ran, which is strong circumstantial support, and the load-bearing half of
the claim — that no `Config/` or `.uproject` change was needed or made — is confirmed by the diff.
Not a finding; noted so it is not read as checked.

**H — no existing gate report was modified.** The twenty-nine pre-existing files in
`E:/MultiAgent/Stratocracy/Tools/architect/gate_reports/` were listed and hashed before writing and
none was opened for write. This report is the only new file.

VERDICT: BLOCK
