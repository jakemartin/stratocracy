# Gate: W8 build-pulse material — RE-GATE after BLOCK — strat-integration-reviewer

## Scope

Re-gated the **uncommitted working tree** of `E:/MultiAgent/Stratocracy`, branch `master`
(`git rev-parse --abbrev-ref HEAD` -> `master`), base
`4e4e2d1ca043ae5113dfcbf9789192206e589b17` (`git rev-parse HEAD`). Every path below is in that
tree; nothing was read from a worktree. `git worktree list` shows eight entries, none of them
the subject of this gate.

**This is a full re-read of the pass, not a re-check of the prior finding.** All twelve items
were run against the tree as it stands. The prior report,
`Tools/architect/gate_reports/2026-09-04-build-pulse-material.md` (`VERDICT: BLOCK`), was read
and is untouched — the thirty files in `Tools/architect/gate_reports/` were hashed before this
one was written and none was opened for write.

**THE TREE MOVED UNDER THIS GATE WHILE IT WAS RUNNING, AND THAT IS ITSELF REPORTED BELOW AS
FINDING 1.** Two measurements bracket it, both quoted verbatim from this session.

At the **start** of this gate, `git status --porcelain`:

```
 M Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
?? Content/StratArt/Materials/M_OverlayPulse.uasset
?? Tools/architect/gate_reports/2026-09-04-build-pulse-material.md
```

and `git diff --cached --stat` printed **nothing** — the index was byte-identical to `HEAD`.
That confirmed the coordinator's claim (1): the `git restore --staged` had been run, the index
held nothing for `M_OverlayPulse.uasset`, and no other path in the pass carried a stale staged
blob.

At **15:49** the same two commands printed:

```
M  Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
A  Content/StratArt/Materials/M_OverlayPulse.uasset
A  Tools/architect/gate_reports/2026-09-04-build-pulse-material.md
M  Tools/architect/state/content.md
M  Tools/architect/state/global.md
```

```
 .../Materials/MI_Overlay_BuildPulse.uasset         |   4 +-
 Content/StratArt/Materials/M_OverlayPulse.uasset   |   3 +
 .../2026-09-04-build-pulse-material.md             | 362 +++++++++++++++++++++
 Tools/architect/state/content.md                   | 120 +++++++
 Tools/architect/state/global.md                    |  27 ++
 5 files changed, 514 insertions(+), 2 deletions(-)
```

`ls -l --time-style=full-iso .git/index` -> `2026-09-04 15:49:13.687716600 -0400`, against
`date` -> `Fri Sep  4 15:49:40 EDT 2026`. The index was rewritten some 27 seconds before that
reading. **This reviewer holds no write tool and ran no `git add`**; every command it issued was
`git status` / `git diff` / `git ls-files` / `git cat-file` / `git lfs status` / `grep` / `sed`
/ `find` / `python`, none of which writes the index.

Files read in full or in part, all under `E:/MultiAgent/Stratocracy/`:
`Tools/architect/gate_reports/2026-09-04-build-pulse-material.md`,
`Tools/architect/state/content.md` (header 1-44, entry 44-152, bullet 378-404),
`Tools/architect/state/global.md` (28-36, 605-616, 655-676, 866-945, 4325-4350, 4998-5006),
`.agents/ue-project-context.md`, `Source/StratBridge/StratBridge.Build.cs`,
`Stratocracy.uproject`, `Source/StratRules/StratRules.manifest.json`,
`Data/StratData.manifest.json`, `Saved/Config/WindowsEditor/SourceControlSettings.ini`, and the
raw bytes of `M_OverlayPulse.uasset`, seven `MI_*.uasset` overlay and marker instances, and the
LFS object `64ec50f...`.

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
| 11 | Prose blocks present | **BLOCK** — Finding 2 |
| 12 | Nothing staged or committed | **BLOCK** — Finding 1 |

### How each was run

**1.** `git diff --stat -- Source/StratRules Data` printed **nothing**; so did
`git diff --stat 4e4e2d1 -- Source/StratRules Data`, which also covers the index. The manifests
were read for the authority rather than quoted from the checklist:
`Source/StratRules/StratRules.manifest.json` -> `"rulesCommit": "96d93ea9e490c8cc33261f5d9671389a9c76ecfd"`,
`Data/StratData.manifest.json` -> `"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"`.
Unmoved.

**2.** `grep -rnE "strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(" Source/ --include=*.cpp --include=*.h
--include=*.hpp`, filtered to exclude `Source/StratBridge/` and `Source/StratRules/`, returned
**zero lines**. The instrument was controlled: the same grep **unfiltered** returns **143**
hits, so it can speak. No type-versus-call judgement was needed because the filtered set is
empty.

**3.** Every `Source/**/*.h` naming `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h`
or `Replay.h` was tested for a `UCLASS`/`USTRUCT`/`UENUM` declaration; **zero violations**.

**4.** `grep -rn "TUniquePtr<[[:space:]]*FStratBridge" Source/` returns exactly one line, and it
is a comment recording the defect. `Source/StratUI/StratViewModel.h:47` reads, in full:
`// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in`.
The real members are `TPimplPtr`:
`Source/StratPlay/StratMatchSubsystem.h:2496` -> `TPimplPtr<FStratBridge> Bridge;` and
`Source/StratUI/StratScoreboardHUD.h:1235` -> `TPimplPtr<FStratBridge> OwnedBridge;`.

**5.** Derived, not trusted. The extraction printed **16 rows**, all five modules present:

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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,AIModule,NavigationSystem,StateTreeModule,GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

Row 3 is a TEXTUAL hit and the output cannot show that it is guarded, so the branch was read at
source — `Source/StratBridge/StratBridge.Build.cs`, lines 133-137:

```
133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
135:  if (!bCompileVendoredRulesHere)
137:      PrivateDependencyModuleNames.Add("StratRules");
```

One expression driving both halves, monolithic-only. Correct, deliberate, **not** a finding.
`StratUI` has no `Stratocracy` dependency; no cycle. The field census printed **ten** names and
no eleventh:

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
```

`BRIDGE=` is the known false positive — the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` — and was neither chased nor filtered away.
`.agents/ue-project-context.md` is unmodified (`git status --porcelain .agents/` prints
nothing) and its `StratBridge` row still states the include edge in every target and the
monolithic-only link edge, naming `Target.LinkType != TargetLinkType.Monolithic` as their common
source. No drift.

**6.** `Stratocracy.uproject` `Modules` = `['Stratocracy', 'StratBridge', 'StratUI',
'StratPlay']`. `grep -rl "IMPLEMENT_MODULE\|IMPLEMENT_PRIMARY_GAME_MODULE" Source/ --include=*.cpp`
returns exactly `Source/StratBridge/StratBridge.cpp`, `Source/Stratocracy/Stratocracy.cpp`,
`Source/StratPlay/StratPlay.cpp` and `Source/StratUI/StratUI.cpp`. `Source/StratRules/` carries
none and is correctly absent from the array. No new `Source/<Module>/` in this pass.

**7.** `git diff --stat 4e4e2d1 -- Source/Stratocracy` printed **nothing**.

**8.** `grep -rn "/Game/" Source/ --include=*.cpp --include=*.h` returns 44 lines. Judged, not
forwarded: 41 are prose inside comment blocks stating the rule. The live string literals are
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54` —
`const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);` —
the standing import-commandlet exception, and three `LoadTable(TEXT("/Game/StratData/..."))`
calls at `Source/Stratocracy/StratData/Tests/StratDataParity.cpp:376,472,555`, the standing
automation-fixture exception. **No gameplay-C++ literal.** The new
`/Game/StratArt/Materials/M_OverlayPulse` path exists only inside `.uasset` bytes and record
prose, never in C++.

**9.** No `.cpp`, no `.h` and no widget changed:
`git diff --stat 4e4e2d1 -- Source/ Config/ Stratocracy.uproject` printed **nothing**. This is a
pass on an **unchanged subject**, recorded as such rather than as a fresh re-derivation of every
`StratUI` binding.

**10.** Content-lane asset work performed by the `coordinator`; `Tests/` untouched; no
production code in `Tests/`; no test code outside it. The route question is Observation G.

**11. BLOCK — see Finding 2.** Both record files carry why-first prose in house style and
`.agents/ue-project-context.md` is correct as written. What fails is not the presence of a prose
block but the truth of a measurement stated inside one.

**12. BLOCK — see Finding 1.**

## Findings

### Finding 1 — item 12: the whole pass is staged, and it was staged *during* this gate

**Constraint violated:** *Nothing staged or committed. Agents do not commit. Unexpected staged
content is a finding.* (`CLAUDE.md` non-negotiable 6: *Staging and commits are the user's call.*)

Five paths now sit in the index. The two bracketing `git status --porcelain` readings and the
`.git/index` mtime are quoted in **Scope** above and are the evidence; they are not repeated
here.

**THE PRIOR GATE'S HAZARD IS NOT PRESENT, AND THAT DISTINCTION IS THE POINT OF THIS FINDING.**
What is staged is the delivered artifact in every case, verified blob by blob rather than
assumed:

```
$ git diff --stat            # index versus working tree
[prints nothing]
```

```
$ git cat-file -p 0dc3a87ecda23bcd2f173a52af39892f001becf9      # the staged M_OverlayPulse
version https://git-lfs.github.com/spec/v1
oid sha256:3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24
size 11384
```

`3a8aa7c... / 11384` is the **rewired, pulsing** master — the same sha256 this reviewer computed
directly off the working-tree file. The staged `MI_Overlay_BuildPulse` pointer is
`d6a6e2aa3ff693d106625abc6b7bc04ea9a341c8ecb387b50742b8330855c833`, size 7893, identical to that
file's own sha256. `git lfs status` reports an **empty** *"Objects not staged for commit"* list.
**A commit against this index would ship exactly the bytes this pass verified.** That is the
opposite of the prior finding, and this BLOCK must not be read as a repeat of it.

**What it is instead.** Item 12 is binary and the index is not clean. Two readings are available
and **neither can be distinguished by any instrument in this tree**: (a) the user staged the pass
at the keyboard in preparation to commit, which is entirely their call and would make this no
finding at all; or (b) an agent or a tool staged it, which is what non-negotiable 6 forbids. The
reviewer can state only what it measured — the index was clean at the start of this gate, is
full at 15:49:13, and this seat wrote nothing. **A gate cannot pass an index whose contents it
cannot account for**, and the auto-add mechanism the entry itself documents
(`Saved/Config/WindowsEditor/SourceControlSettings.ini` -> `Provider=Git`; verified, that is the
file's entire body under `[SourceControl.SourceControlSettings]`) is a live candidate cause that
would place it in reading (b).

**One path in the staged set is new and deserves naming on its own.**
`Tools/architect/gate_reports/2026-09-04-build-pulse-material.md` is staged as blob
`f2167b4666ec0fc09d0a3cb1b1ffae183fa7f606`, whose content hashes to `93aafe7c8fd4` —
byte-identical to the file on disk. The prior gate report is therefore intact and on the commit
path, which is what this project's own record asks for, since a citation to an untracked report
resolves to zero files in any checkout. That half is desirable. It is reported here only because
it arrived by the same unexplained write.

**Owner of the fix:** the `coordinator`, and the resolution is a question to the user before any
git operation — *did you stage this?* If yes, the index is the user's and item 12 should be
re-run once they say so. If no, `git restore --staged` on all five paths and the writer found.
**Do not simply re-run `git restore --staged`**: the prior gate prescribed exactly that, it was
done, and the state returned within the hour. Repeating it without identifying the writer treats
a recurring condition as an accident, which is the reasoning `CLAUDE.md` itself refuses.

### Finding 2 — item 11: a live present-tense sentence in `content.md` is false of this tree

**Constraint violated:** the record records measurements as measurements, and
`Tools/architect/state/content.md`'s header puts everything under `## NEXT` in the live-swept
set.

`Tools/architect/state/content.md:144-145`, inside the `[CORRECTED 2026-09-04 ...]` block
appended to the staging bullet of the topmost `## NEXT` entry:

> The path was unstaged with `git restore --staged` and now shows `??`; the index
> holds nothing for it.

**The index holds `100644 0dc3a87ecda23bcd2f173a52af39892f001becf9 0` for that path**
(`git ls-files -s Content/StratArt/Materials/M_OverlayPulse.uasset`), and `git status
--porcelain` reports `A `, not `??`. The sentence was true when written and is false now.

The same block's parent bullet is falsified on its own tell.
`Tools/architect/state/content.md:130-131`:

> the MODIFIED `MI_Overlay_BuildPulse` is unstaged, which is the tell.

`MI_Overlay_BuildPulse.uasset` is now staged (`M ` in `git status --porcelain`), so the
diagnostic that bullet hands a future reader for telling an editor auto-add apart from a
deliberate `git add` **no longer discriminates in this tree**. That is worse than a stale
number: it is a stale *instrument*, offered to the next reader as reliable.

This is the failure mode this project already carries in its own record — true when written,
false by the commit; measurements rot too — and it gates because the sentence is an affirmative
claim about the state of the index, in the one entry a reader consults about the state of the
index.

**Owner of the fix:** the `coordinator` (`content.md` under this file's fallback condition,
which the entry already invokes with a control). The correction must be stamped in place, per
this file's own convention, and **it must not be written until Finding 1 is resolved** —
otherwise it records a third index state for the next gate to falsify again. Note the ordering
hazard explicitly: any sentence in that block describing the index is a claim whose subject has
moved twice in one day, so the durable form is the mechanism — *what source control auto-adds is
the asset as it was at creation, not as delivered* — which the block already states correctly
and which neither finding touches.

## Observations

*Non-gating.*

**A — the correction block's substantive measurements are all true of this tree, and were
re-derived rather than taken.** Only the two index sentences in Finding 2 are false. Everything
else checks out.

- Working-tree `M_OverlayPulse.uasset` is 11384 bytes and its sha256 is
  `3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24`, matching the block's
  `oid 3a8aa7c... , size 11384`.
- `git cat-file -p c1c9d5454366139e06de3bbeff27d040a5e6786a` — the blob the prior index held —
  returns `oid sha256:64ec50f34132607680e503454fc0108b8038c6b0e2baf922c4378c66d3ad18bf`,
  `size 6477`, matching `Index blob c1c9d54, LFS oid 64ec50f..., size 6477`.
- The pre-rewire diagnosis was **independently reproduced from the LFS object itself**, not taken
  from the entry or from the prior report. Scanning printable strings of
  `.git/lfs/objects/64/ec/64ec50f...` (6477 bytes) for `MaterialExpression*` and `Pulse*` tokens
  returns, in full: `BLEND_Translucent`, `M_OverlayPulse`, `MaterialExpressionCollection`,
  `MaterialExpressionEditorX`, `MaterialExpressionEditorY`, `MaterialExpressionGuid`,
  `MaterialExpressionVectorParameter`. **`MaterialExpressionTime` ABSENT,
  `MaterialExpressionSine` ABSENT, `PulseSpeed` ABSENT, `MaterialExpressionVectorParameter`
  PRESENT as the control** — exactly as the block states.
- The same scan over the working-tree file returns `MaterialExpressionTime`,
  `MaterialExpressionSine`, `MaterialExpressionMultiply`, `MaterialExpressionAdd`,
  `MaterialExpressionLinearInterpolate`, `MaterialExpressionScalarParameter`,
  `MaterialExpressionVectorParameter`, `PulseMin`, `PulseMax`, `PulseSpeed` and
  `BLEND_Translucent`. This confirms the node **set** and the parameter **names**; it does
  **not** confirm the wiring, which no instrument available to a read-only gate can reach, and it
  is not reported as if it did.
- The hazard sentence is accurate and is the part with teeth: *"what is staged is a DIFFERENT
  ARTIFACT from what was verified"* names the mechanism rather than only the condition, which is
  precisely what the prior gate asked for.

**B — the controls hold, and exactly one instance moved.** `MI_Overlay_BuildPulse.uasset` names
`M_OverlayPulse` (4 occurrences) and no longer names `M_Translucent` or `GridUI`. The six
siblings — `MI_Overlay_Objective`, `MI_Overlay_Reach`, `MI_Overlay_Target`, `MI_Marker_Flag`,
`MI_Marker_Guided`, `MI_Marker_Pip` — each still name `M_Translucent` (4) and `GridUI` (2) and
never `M_OverlayPulse`. `_TmpBrokenProbe` is gone from disk (`find . -iname "*TmpBrokenProbe*"`
returns nothing) and from the index (`git ls-files | grep -i tmpbroken` returns nothing), so the
hand-deletion the entry records is confirmed complete. **No other package from the commandlet
run is present anywhere in the tree**, staged or untracked:
`git status --porcelain --untracked-files=all` lists five paths and no others.

**C — record ownership holds, and the sweep was re-run rather than taken.**
`python Tools/architect/strat_banner_sweep.py`, run in `E:/MultiAgent/Stratocracy` **without a
pipe**:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 389 entries, all Success (2026.09.04-16.30.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
  report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
  suite claims found: 62 (1 live, 61 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

exit **0**. `--explain` puts the single LIVE figure at **`global.md:32`, `389/389`**; the other
61 are called stamped. The new `content.md` entry states no `N/N` and no verdict and says so in
terms — *"No suite count and no verdict is stated here; both are `global.md`'s alone"* — and
neither new stamp adds one. `global.md`'s *"With this, W8 is closed"* is a milestone-status claim
in the one file entitled to make it. Correct. **The sweep vouches for none of Observation D**: it
collects claims by shape and does not collect claims of the pulse shape at all.

**D — the retired-claim sweep was re-run independently, and the prior gate's Observation D is now
discharged at its own site.** Grepping `Tools/architect/state/` case-insensitively for
`DOES NOT PULSE`, `static tint`, `no time input`, `cannot author a material graph`,
`no node-graph authoring`, `has no time` and `still a static` returns hits at
`content.md:143`, `:383`, `:384`, `:388`, `:399` and `global.md:660`, `:661`, `:4327`, `:4328`,
`:4329`. Each was read at its site, not counted.

- `content.md:143` is inside the correction block, describing what a commit *would have* shipped
  — a quoted hypothetical, not an assertion about the tree.
- `content.md:383-388` is the 2026-09-02 bullet, stamped in place immediately below at
  `content.md:393-404`.
- `content.md:399` is quoted text *inside* that stamp.
- `global.md:660-661` — the paragraph the prior gate raised — is now stamped at
  `global.md:662-673`, at its own site, naming both sentences and correcting both.
- `global.md:4327-4329` is the `WHAT W8 STILL OWES` bullet, stamped at `global.md:4333-4348`.

**Every hit is either stamped at its own site or is quoted text inside a stamp.** The
coordinator's claim on this point is confirmed by an independent sweep.

**E — one further stale W8 sentence, in a superseded block, offered for the next `global.md`
write and not raised as a finding.** `Tools/architect/state/global.md:933`:

> WHAT W8 STILL OWES, AND NONE OF IT IS BUILT. Item (2) the capture-progress pip and item (4) the
> BUILD pulse are DRAWING-ONLY

Item (4) is now built and item (2) was cut by user ruling (`global.md:4577`). The sentence sits
in the `_Last run 2026-08-31` block, which is closed by a `[STAMPED 2026-09-01 -- SUPERSEDED BY
THE BANNER ABOVE ...]` at its head, and it carries its own in-place `[STAMPED 2026-09-01 ...]`
immediately below — but that stamp's subject is `bAffordable` being read by drawing code, not the
build status of the items. **The hazard is specific:** the live bullet at `global.md:4327` opens
with the same words, `WHAT W8 STILL OWES`, so a reader grepping that phrase lands on both and the
older one reads as an assertion. This is the identical shape the prior gate raised about `:660`
and the coordinator then stamped. One line on the next `global.md` write closes it.

**F — attribution is correctly split, in the entry and in the correction block.**
`content.md`'s header, lines 32-33, makes *"an entry that cites one authority for both halves"* a
finding. The entry's opening reads **"ACTING: the `coordinator`, under `CLAUDE.md`'s
EDITOR-DRIVER CLAUSE. WRITING: the `coordinator`, under THIS FILE'S FALLBACK CONDITION."** — two
authorities, two halves, inline. The newly appended correction block opens *"CORRECTED 2026-09-04
BY THE `coordinator`, SAME DAY, AFTER `strat-integration-reviewer` BLOCKED ON IT"*: it names its
writer and cites **no** authority for acting, which is right rather than an omission — the acting
it records is a `git restore --staged`, an index operation and not content-lane editor work, and
it sits inside an entry whose ACTING/WRITING split is stated at its head. **Not a finding.**
Obligation (1) of the fallback, measure the absence with a control, is discharged in one lookup
with a genuine control: `mcp__NeoStack_Connect__execute_script` and
`mcp__unreal-editor-direct__execute_script` absent, while `mcp__NeoStack_Connect__unreal_status`
and `mcp__NeoStack_Connect__list_unreal_projects` — same server, same lookup — answered. The base
commit `4e4e2d1` and the "no worktree and no merge" condition are both named.

**G — the route question is still unruled, is still flagged rather than claimed, and this
reviewer does not rule it.** The pass was driven by a headless
`UnrealEditor-Cmd.exe -run=pythonscript` commandlet. `CLAUDE.md`'s editor-driver clause is
written about *driving the LIVE editor*. The entry carries a dedicated bullet — **"SO THE ROUTE
WAS NEITHER OF THE TWO THIS FILE HAS RECORDED, AND WHETHER IT SATISFIES THE CLAUSE IS THE USER'S
TO RULE"** — and states the distinction in terms. That flagging is prominent, inline, and names
the open question rather than burying it: sufficient, and unchanged from the prior gate's
judgement. Worth recording that this route has now been taken **once**; `CLAUDE.md` treats a
repeated exception as a condition rather than an accident, so a second occurrence should produce
a ruling rather than a third flag.

**H — one claim in the entry remains unverifiable from this checkout and is recorded as not run
rather than as clean.** The entry states `PythonScriptPlugin` is enabled *transitively* by
`PCGPythonInterop`. `Stratocracy.uproject` lists `PCGPythonInterop` at line 67 and contains no
other `Python` string — `grep -n "Python" Stratocracy.uproject` returns that single line — but
`PCGPythonInterop.uplugin` is not in this repository, so **the transitive dependency itself was
not verified here**. The commandlet demonstrably ran, and the load-bearing half — that no
`Config/` or `.uproject` change was needed or made — is confirmed by
`git diff --stat 4e4e2d1 -- Source/ Config/ Stratocracy.uproject` printing nothing.

**I — a small internal contradiction in `global.md`'s new stamp, worth a word on the next write.**
The stamp at `global.md:4333-4348` says *"`content.md`'s topmost `## NEXT` entry is the authority
on the asset ... none of it is restated here"*, and then restates the user's sighting verbatim:
*"I saw it on the hex in a match, the cycle speed is fine."* The quote is arguably necessary — a
stamp discharging an owed item has to say what discharged it — but the sentence claiming nothing
is restated is falsified by the sentence beside it. Non-gating: the mechanically enforced
ownership rule covers suite counts and phase verdicts, and neither is restated.

**J — no existing gate report was modified.** The thirty files in
`E:/MultiAgent/Stratocracy/Tools/architect/gate_reports/` were listed and sha256-hashed before
this file was written and none was opened for write. This report is the only new file, and the
only file this reviewer wrote.

## Addendum — the pass was COMMITTED while this gate was open

**Everything above was written against an uncommitted tree, which is what this gate was
dispatched to audit. Before the report was finished the pass was committed.** The addendum is
appended rather than folded into the sections above, so that the sequence stays readable and no
sentence above is quietly rewritten to match a state it was not measured in.

Measured after the last observation was written:

```
$ git rev-parse HEAD
54709ac56060a087c8541323a8e019c986442cf8

$ git status --porcelain
?? Tools/architect/gate_reports/2026-09-04-build-pulse-material-regate.md

$ git rev-list --left-right --count origin/master...HEAD
0	1
```

`git reflog` records it as `HEAD@{0}: commit:` directly over `4e4e2d1` — a single new commit,
**not pushed** (`origin/master` is one behind), and therefore still amendable at the time of
writing. `git show --name-status 54709ac` lists exactly the five paths this gate audited:

```
M	Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
A	Content/StratArt/Materials/M_OverlayPulse.uasset
A	Tools/architect/gate_reports/2026-09-04-build-pulse-material.md
M	Tools/architect/state/content.md
M	Tools/architect/state/global.md
```

**THE BYTES COMMITTED ARE THE BYTES THIS GATE VERIFIED, and that is the first thing to say.**

```
$ git cat-file -p 54709ac:Content/StratArt/Materials/M_OverlayPulse.uasset
oid sha256:3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24
size 11384

$ git cat-file -p 54709ac:Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
oid sha256:d6a6e2aa3ff693d106625abc6b7bc04ea9a341c8ecb387b50742b8330855c833
size 7893
```

Both match the sha256 values computed off the working-tree files earlier in this report. **The
shipped defect the prior gate blocked to prevent did not occur:** the committed master is the
rewired, pulsing one, the reparent went with it, and the prior gate report is tracked in the same
commit rather than left untracked for a citation to miss. The working tree is now identical to
`HEAD` apart from this report.

### What the commit does to each finding

**Finding 1 changes form and does not go away.** Its staged half is moot — the index is clean and
`git status --porcelain` lists only this report. What replaces it is the plainer half of item 12,
*nothing committed*: **the pass was committed while its own gate was open and about to return
`BLOCK`.** This project's record already carries that shape as a lesson — a gate reads an
uncommitted tree, and a `PASS` describes a tree rather than a phase — and the cost here is
concrete rather than theoretical: Finding 2 is now in history instead of in a working file.
Whether the commit was the user's own call, which is their prerogative and forbids nothing, or an
agent's, which non-negotiable 6 forbids outright, **cannot be distinguished from this checkout**;
the reviewer states the measurement and not the cause, exactly as it did for the staging.

**Finding 2 is now committed, and its text should be read at `54709ac` rather than in the
working file.** `git show 54709ac:Tools/architect/state/content.md`, lines 144-145:

> The path was unstaged with `git restore --staged` and now shows `??`; the index
> holds nothing for it.

That sentence is false of the commit that contains it. `54709ac` records
`Content/StratArt/Materials/M_OverlayPulse.uasset` as an added path — the index held it at the
moment of the commit, which is what a commit is — and `git status` never showed `??` for it
again. The parent bullet's tell at lines 130-131, *"the MODIFIED `MI_Overlay_BuildPulse` is
unstaged, which is the tell"*, is falsified the same way by the same commit. **The fix is a
stamp in a follow-up commit**, in place, per this file's own convention; it is not a reason to
rewrite history, and the durable sentence the block already contains — *what source control
auto-adds is the asset as it was at creation, not as delivered* — needs no change.

**One thing in the commit message itself, reported because the window to fix it is open and
narrow.** The subject line ends:

> so this commit would have shipped the pre-rewire duplicate under the message of the pass that
> fixes it

Read literally, *"this commit ... shipped the pre-rewire duplicate"* is false of `54709ac`, which
ships `3a8aa7c... / 11384`, the material that pulses. The intended reading is plainly the
counterfactual — what a commit against the *pre-restore* index would have done — and *"would
have"* carries it for an attentive reader. But the subject is `this commit`, the reader most
likely to meet it is skimming `git log`, and this project's own record notes that **a pushed
commit message cannot be amended**. `origin/master` is one commit behind, so the message is
amendable right now and will not be after the next push. Naming the pre-restore index as the
subject — rather than `this commit` — would remove the ambiguity at no cost.

### What this gate can and cannot say about a committed pass

The subject moved twice while this gate ran: clean index at the start, five paths staged at
15:49:13, committed as `54709ac` before the report closed. **Every measurement in this report is
therefore reported with the moment it was taken, and none of them is offered as describing the
tree at the moment you read this.** The code and asset checks are unaffected — `Source/` is
byte-identical to `4e4e2d1` in both the working tree and `54709ac`, and the asset bytes were
verified by content hash in all three states, so items 1 through 10 hold across the move. Item 11
and item 12 are what the move touched, and both are reported at their measured moments above.

**Owner of the follow-up:** the `coordinator` for the `content.md` stamp and for putting the
commit-message question to the user before the next push; the user for the ruling on whether the
commit and the staging were theirs. **Re-gate after those land** — this report describes three
successive trees and none of them is the one the fix will produce.

VERDICT: BLOCK
