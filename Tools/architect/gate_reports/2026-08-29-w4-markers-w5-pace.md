# Gate: W4 marker slots + W5 playback pace (asset batch) — strat-integration-reviewer

Tree: `E:/MultiAgent/Stratocracy`, branch `master`, HEAD `ec54dc9be2e3a3a04a1a25fc9098e2119a51f05c`.
Gated 2026-08-29 against the UNCOMMITTED working tree.

## Scope

Working-tree diff against HEAD, 7 paths. `git status --short` matched the dispatched
expectation byte for byte:

```
AM Content/StratArt/Materials/MI_Marker_Flag.uasset
AM Content/StratArt/Materials/MI_Marker_Pip.uasset
 M Content/StratPlay/BP_StratGameMode.uasset
 M Content/StratPlay/BP_StratGameMode_AiVsAi.uasset
 M Content/StratPlay/BP_StratUnit.uasset
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
```

5 assets, 2 record files, 0 source files. Audited in the user's standing priority order:
code, assets and module graph first; record prose only where load-bearing.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing. Manifests read `rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`, `dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`, derived from the manifests rather than quoted from the checklist. |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — call-shaped grep over `Source/`, minus the two legal directories, returned zero. No C++ changed, so this is a tree-state pass, not a diff pass. |
| 3 | No vendored header in front of UHT | pass — the only header including `Data.h`/`Replay.h`/`Save.h`/`Scenario.h`/`Ui.h` is `Source/StratBridge/StratBridge.h:53-57`. That file matched the `UCLASS/USTRUCT/UENUM` scan only in PROSE (lines 30, 118, 261, 263 are comments arguing why it declares none). Judged: not a violation. |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the sole `TUniquePtr<FStratBridge>` hit is `Source/StratUI/StratViewModel.h:47`, inside a comment recording the `C4150` measurement. Live members are `TPimplPtr`: `StratMatchSubsystem.h:1973`, `StratScoreboardHUD.h:1093`. |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. See "Module graph" below. 15 rows, 5 modules; field census returned exactly 6 names. |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory. `Stratocracy.uproject` Modules = `Stratocracy, StratBridge, StratUI, StratPlay`; `IMPLEMENT_MODULE` appears in exactly those three non-primary modules and NOT in `Source/StratRules/`, which is correctly absent from the array. |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing. |
| 8 | No `/Game/` literal in gameplay C++ | pass — all non-comment hits are under `Source/*/Tests/` (standing exception) plus `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54` (the other standing exception). The 24 hits outside `Tests/` in `StratPlay`/`StratUI` are prose in comment blocks stating the rule. No C++ changed. |
| 9 | No widget-side arithmetic | pass by absence of subject — nothing under `Source/StratUI/` changed. Not re-audited across the whole module. |
| 10 | Lane discipline | pass — see "Lane" below. |
| 11 | Prose blocks present | not applicable — no new source file and no new `.Build.cs` entry. Not a pass, not a miss; there was no subject. |
| 12 | Nothing staged or committed | pass with an Observation — nothing committed (HEAD is still `ec54dc9`). Two paths ARE staged (`A` in column 1): the two new `.uasset` files. Recorded below rather than gated; see Observation 1. |

## Module graph — derived this pass

Extracted from the checklist's own command and executed against `Source/*/*.Build.cs` with
comments stripped. 15 rows, three per module, five modules — no module missing from output.

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,EnhancedInput
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

Row by row against the checklist block: all five rows match. `StratBridge` carries
`StratRules` in `PublicIncludePaths` only, NOT in either dependency array — the include edge,
not a module arrow, exactly as the block predicts. `Source/StratBridge/Vendored/*.strat.cpp`
is still 12 files, so the vendored sources are still compiled INTO that module.
`StratUI` has no `Stratocracy` dependency. No cycle.

Field census returned exactly six names and no seventh:
`PublicDependencyModuleNames.AddRange`, `PrivateDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `PCHUsage =`, `bUseUnity =`, `ShadowVariableWarningLevel =`.
`PCHUsage = NoPCHs` and `bUseUnity = false` are set on StratBridge, StratPlay, StratRules
and StratUI.

## Asset audit — the four questions asked

### Q1. Do the two new `.uasset` files sit in the right lane, and do they touch the vendored or `/Game/`-literal rules?

Yes to the lane; no to both rules.

`Content/StratArt/Materials/MI_Marker_Flag.uasset` (5660 B) and `MI_Marker_Pip.uasset`
(5654 B) are real UE packages, not LFS pointers — both open with the package magic
`c1 83 2a 9e`. `git check-attr` confirms both are under the `lfs` filter. Worktree blob
hashes: `c201350676cbeab565a4e468740dcfbf1e09513e` and
`4017555c8b4e31516b01a91b0f8ad41e5f3a9e2d`.

They sit beside `MI_Marker_Guided.uasset` in the same directory — the content lane's own.
Neither is in `Source/` or `Data/`, so the vendored rule is untouched (check 1 printed
nothing). The `/Game/` rule binds C++ only; a `/Game/` path INSIDE a `.uasset` is an asset
reference and is exactly what the rule exists to force.

Independently verified against the record's claims, and both hold:

- Parent material on both new MIs is
  `/Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent` — the same parent
  `MI_Marker_Guided` carries. The "duplicated from Guided" claim is confirmed by the bytes.
- The `Color` vector parameter, decoded as four little-endian floats from the packages:
  - `MI_Marker_Flag` = `(1.0, 0.72, 0.05, 0.95)` — gold
  - `MI_Marker_Pip` = `(0.15, 0.95, 0.30, 0.95)` — green
  - `MI_Marker_Guided` = `(0.8, 0.1, 0.95, 0.95)` — magenta
  Exactly the values `content.md` records, to the digit.
- The record's "guided marker re-read after both writes and unchanged" control is corroborated
  by a stronger instrument than a readback: `MI_Marker_Guided.uasset` does not appear in
  `git status` at all. The source of the duplication was not edited.

### Q2. Any `/Game/` literal newly introduced into gameplay C++?

None. Zero bytes of `Source/` changed — `git diff --stat -- Source` printed nothing, and
`git diff --numstat` lists no path under `Source/`. The claim "NO C++ CHANGED" is confirmed
rather than assumed.

### Q3. `/Engine/BasicShapes/*` as a shipping mesh default — sound, or a cook hazard?

Sound, and this batch introduced no new engine dependency whatsoever. That is a stronger
answer than the record gives itself.

I smudged `HEAD:Content/StratPlay/BP_StratUnit.uasset` through `git lfs smudge` and compared
the engine-reference sets:

```
HEAD      : /Engine/BasicShapes/Cone, Cube, Cylinder, Sphere
worktree  : /Engine/BasicShapes/Cone, Cube, Cylinder, Sphere
```

Identical. `Cone` and `Sphere` were ALREADY hard-referenced by this Blueprint before the
batch. The batch reused engine content the package already pulled in; it did not widen the
cook set by one asset.

On the general question: `/Engine/BasicShapes/` lives under `Engine/Content/`, which is
runtime content, not `Engine/Content/Editor*`. A hard reference from a cooked `/Game/`
package pulls it into the cook through the reference graph automatically — no
`DirectoriesToAlwaysCook` entry is needed. The genuine hazard shape is a reference into
`/Engine/EditorMeshes/` or `/Engine/EditorResources/`, which are stripped from a cooked
build; `BasicShapes` is not one of those. I checked `Config/` for any cook directive that
could countermand this and found none — no `NeverCook`, `AlwaysCook` or `bCook*` key
anywhere in `Config/`.

Not a finding, and not an Observation either. The concern is answered.

### Q4. `AiPlaybackStepSeconds` set on BOTH GameMode Blueprints where the record said the singular.

Confirmed present on both, and confirmed to be a genuine change:

| package | at HEAD | in worktree |
|---|---|---|
| `BP_StratGameMode.uasset` | absent (0 hits) | present (1 hit) |
| `BP_StratGameMode_AiVsAi.uasset` | absent (0 hits) | present (1 hit) |

Absence from the `.uasset` at HEAD is evidence the property equalled the C++ default —
`Source/StratPlay/StratMatchSubsystem.h:479` declares `float AiPlaybackStepSeconds = 0.0f;`.
So both Blueprints moved off the compiled default in this batch. `AiTurnDelaySeconds` is
present in the same scan as a positive control that a field of this kind is visible to the
instrument, and `ZZZNOPE` returned 0 as the negative control.

`content.md` hands the singular-vs-plural question forward explicitly as a lane call that is
not the content lane's to settle, and names `BP_StratGameMode_AiVsAi` as the one to return to
`0.0` if the singular was deliberate. That is the correct disposition and I am not overriding
it. It is a design question, not a constraint violation, so it does not gate. My own read,
offered and not binding: setting both is defensible — `_AiVsAi` is the mode where the
playback tour is the entire product, so a `0.0` there would make W5 unobservable in the one
mode built to observe it.

## Lane

`Content/` was changed by the `coordinator` rather than `strat-editor-builder`. Both record
files changed are ones the `coordinator` may write: `content.md` under its header's fallback
condition, `global.md` under its own already-`coordinator`-and-steward ownership. Nothing was
written into `engine.md`, `tests.md`, `data.md` or `decisions.md`. No test code moved, no
production code moved, no file outside the content lane and the coordinator's own record
files was touched.

### The two claims tested hardest

**Claim A — "the fallback condition was met literally but not in its rationale."**

Honest, complete, and if anything harsher on itself than either authority requires. It does
not understate a lane violation; I find no lane violation to understate.

Both authorities are built the same way — a bare condition, then a rationale that is about
`strat-editor-builder`'s incapacity, not about the editor's reachability in general:

- `content.md` header: *"The `coordinator` may write it only when `execute_script` is absent
  from the session's tool surface — the agent holds the NeoStack tools ... and no Bash, so
  without that one tool it has no route to the editor..."*
- `CLAUDE.md` editor-driver clause: *"When `execute_script` is absent from the session's tool
  surface, the `coordinator` may drive the live editor ... because `strat-editor-builder`
  holds the NeoStack tools and no Bash and has no route to the editor at all."*

The stated condition is client-surface absence, and the entry measured exactly that with the
control both clauses demand — one `select:` lookup returning `unreal_status` while returning
neither `execute_script`. The instrument is shown able to speak before its silence is read.

The rationale is a claim about the LANE AGENT, and that claim held: `strat-editor-builder`
holds no Bash, so the `curl` route to `127.0.0.1:9315/mcp` was not available to it either. It
genuinely could not have done this work. So the premise the permission rests on was true and
was measured. The entry describes its own rationale as unmet; on a strict reading of what the
rationale actually asserts, it was met.

What is genuinely uncontemplated is narrower, and the entry would be more useful stating it
in these terms: neither clause anticipated that the tool could be absent from the CLIENT
surface while the SERVER still served it, so that the `coordinator` held a route the clause
implicitly assumed nobody had. That is a gap in the clauses' model of the world, not a breach
of them.

The entry does not hide the awkward half. It states outright that the endpoint's `tools/list`
carries `execute_script` — the single fact most damaging to its own permission — and routes
the question to the user rather than resolving it in its own favour. It also correctly rules
out the transcription clause by naming the absence of its trigger ("no worktree, no branch
and no merge"), foreclosing the wrong authority before anyone can reach for it.

Recorded as Observation 2: the wording, not the act, is what wants the user's pen.

**Claim B — separate inline attribution of acting and writing.**

Passes against both authorities. The entry's opening carries, inline in the entry body and
not in any header above it:

> **ACTING: the `coordinator`, under `CLAUDE.md`'s editor-driver clause. WRITING: the
> `coordinator`, under this file's own fallback condition.**

- `content.md` header requires: *"Cite THAT for the acting and THIS for the file write; an
  entry that cites one authority for both halves is a finding."* — Two authorities, one per
  half. Satisfied.
- `content.md` fallback obligation (2): *"say so in the entry, naming the coordinator as its
  writer."* — Named. Satisfied.
- `CLAUDE.md` editor-driver obligation (2): *"Attribute the acting and the writing separately
  in the record entry."* — Satisfied.
- `CLAUDE.md` editor-driver obligation (1) and `content.md` fallback obligation (1), the
  control on the absence: satisfied, as above.

A reader arriving by citation lands on the entry and sees both actors and both authorities.
That is the legibility the ruling asks for.

## Findings

None.

## Observations

Non-gating.

1. **Two paths are staged.** `git status` column 1 reads `A` for
   `Content/StratArt/Materials/MI_Marker_Flag.uasset` and `MI_Marker_Pip.uasset`. The standing
   rule is that staging is the user's call. This is the documented `AM` shape an LFS asset
   phase produces — a new `.uasset` has to be added before it can be hashed, diffed or smudged
   at all, and the editor leaves the index stale behind it. Nothing is committed; HEAD is
   still `ec54dc9`. Recorded so the user can unstage deliberately rather than discover it at
   commit time.

2. **`content.md` overstates its own defect, and a ruling is pending on the wording.** The
   entry says the fallback's "rationale was not met". As argued above, the rationale as
   written is a claim about `strat-editor-builder`, and that claim was measured and held. The
   thing neither clause models is that `execute_script` can be absent from the client surface
   while present on the server. If the user writes a clause, that distinction —
   *client-surface absence, not environment absence* — is the sentence to write, and it is
   narrower and more useful than "the coordinator has Bash".

3. **`.agents/ue-project-context.md` shows no module-graph drift on this pass.** Its
   `StratBridge → Stratocracy` paragraph (line 68) matches the derived graph and matches this
   reviewer's own checklist. No correction owed.

4. **The record's `SM_GuidedMarker` `BoxExtent = (50,50,50)` measurement is unverified by
   me.** It is an editor-side readback of a mesh's bounds and I have no route to reproduce it
   from a checkout. It is load-bearing for the "100 uu" assumption at
   `Source/StratPlay/StratUnitActor.h:349`, so it is worth an independent read by someone with
   the editor open — but nothing in this diff turns on it.

5. **The `0.5` float value itself is not decoded.** I confirmed the property NAME moved from
   absent to present in both GameMode packages, which proves a non-default was written. I did
   NOT decode the serialized float to `0.5` — `0.5` appears three times as a raw float in each
   package and I have no offset that localises it to this property. The value rests on the
   editor readback `content.md` records, not on my measurement.

## What I did NOT measure

Stated plainly, because an unrun check must never read as a clean one.

- **I did not build.** This box is MSVC-only and I did not enter `vcvars`. No compile of any
  kind was attempted.
- **I did not run the test suite.** No suite count appears in this report; `global.md` is the
  only file that may carry one and I am not restating it.
- **I did not open the editor or run PIE.** The record's PIE evidence — the `HighResShot`
  capture showing a gold cone, green pips and a magenta sphere — is not independently verified
  here. I confirmed only that no gap-shaped log line appears in
  `Saved/Logs/Stratocracy.log` and that the file carries 65 `LogStratPlay` lines as a control
  that logging of that category is live. That is weaker than the record's own claim and does
  not replace it.
- **I did not re-audit widget-side arithmetic across `StratUI`.** Check 9 passes by absence of
  subject only — no `StratUI` file changed.
- **`strat_banner_sweep.py` was re-run independently**, exit code captured directly from the
  command and not from `$?` after a pipe: `EXIT=0`, `SWEEP CLEAN -- no self-contradiction
  found`, `suite claims found: 41 (1 live, 40 stamped)`, automation report `279 entries, all
  Success (2026.08.29-18.31.03)`. This matches the dispatched figures. It is a sweep result,
  not a suite run — I did not execute the suite that produced those entries.

VERDICT: PASS
