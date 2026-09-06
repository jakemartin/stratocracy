# Gate: unit damage alert — ASSET PASS — strat-integration-reviewer

2026-09-06 (local). Working tree at `E:/MultiAgent/Stratocracy` on branch `master`, base
commit `4a01418`, uncommitted. This is the CONTENT half; the C++ half was gated to PASS and
committed at `4a01418` (five reports, `2026-09-06-unit-damage-alert*.md`). Gate priority is
CODE AND ASSETS FIRST, RECORD SECOND, per the standing user ruling — and the asset half is
clean. The BLOCK below is in the record, and specifically in a claim about a GUARD.

## Scope

Five paths, from `git status --porcelain`:

```
A  Content/StratArt/Materials/M_UnitDamageFlash.uasset
 M Content/StratPlay/BP_StratUnit.uasset
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
?? Tools/architect/evidence/11-damage-flash-assets/
```

Nothing under `Source/`, `Data/`, `Config/` or `Stratocracy.uproject` moved — measured, not
assumed (`git diff --stat` and `git status --porcelain` scoped to each printed nothing).

Two headless read-only `UnrealEditor-Cmd.exe -run=pythonscript` probes were run BY THIS GATE
against the working tree, so the asset claims below rest on this seat's own instrument rather
than on `content.md` or on the committed JSON. Both probe scripts are inlined below.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` and `git status --porcelain` scoped to both printed NOTHING |
| 2 | No `strat::` call outside `StratBridge/`, `StratRules/` | pass — grep for a lowercase-initial `strat::` call outside those two dirs returned zero; no C++ moved this pass in any case |
| 3 | No vendored header in front of UHT | pass — every real include of `StratBridge.h`/`Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h` is inside `Source/StratBridge/StratBridge.h` (declares no reflected type) or `Source/StratRules/`. A loose grep for the STRING `StratBridge.h` returns 17 headers; all 17 are PROSE mentions in comment blocks, not includes — judged, not forwarded |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the single `TUniquePtr<FStratBridge>` hit is `StratViewModel.h:47`, inside a comment recording the `C4150` measurement. Not a declaration |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. 16 rows, 10 field names, both matching the checklist figures; the 16th row is confirmed GUARDED at `StratBridge.Build.cs:133-138`. Full output inlined below |
| 6 | New modules registered (unless no `IMPLEMENT_MODULE`) | pass — no new `Source/<Module>/` directory this pass |
| 7 | `Source/Stratocracy/` untouched | pass — scoped `git diff --stat` printed nothing |
| 8 | No `/Game/` path literal in gameplay C++ | pass — 46 hits in `Source/`, EVERY one on a comment line; excluding comment-leading lines leaves zero. No C++ moved this pass |
| 9 | No widget-side arithmetic | pass — nothing under `Source/StratUI/` changed |
| 10 | Lane discipline | pass — `Content/` and `content.md` written by the `coordinator` under two separately-cited authorities; checked rather than accepted, below |
| 11 | Prose blocks present | pass — the four evidence scripts each carry why-first comments recording measurements as measurements; the `content.md` entry is in house style |
| 12 | Nothing staged or committed | pass with a note — `M_UnitDamageFlash.uasset` IS staged (`A`), by the editor's source-control auto-add, a recorded standing behaviour of this project, explained and MEASURED in the entry. Nothing is committed; `HEAD` is still `4a01418`. See Observation 1 |

### Specifically-requested checks

| Ask | Result |
|---|---|
| 1. The material is what the entry says | pass — verified independently by this gate's own commandlet probe, with controls |
| 2. Blueprint defaults persisted; nothing else clobbered | pass on the seven named properties; the "nothing else" half is NOT RUN — see Finding 2 |
| 3. LFS staging claim | pass — staged LFS oid and size equal the working file's `sha256sum` and byte size exactly; `BP_StratUnit.uasset` correctly NOT auto-staged; both assets LFS-tracked |
| 4. The recorded sweep gap | account accurate; FIX INADEQUATE — BLOCK. See Finding 1 |
| 5. `content.md`'s honesty about what is unverified | pass on the visual claim (stated flatly, not softened); one scoped over-claim elsewhere — Finding 2 |
| 6. Standing checks and evidence-dir hygiene | pass — no secrets, no user-profile paths, no machine-specific junk beyond the expected absolute `E:\MultiAgent\Stratocracy\...` |

## Findings

### FINDING 1 — BLOCK. `global.md` records a sweep-gap fix that does not work, and the paragraph documenting the gap is what re-creates it.

**Constraint violated:** a live record must not assert as fixed a guard that is measurably still
inert, and a measurement must be recorded as a measurement.

**File:line:** `Tools/architect/state/global.md:31` (the citation) and `:32-41` (the paragraph
that exempts it).

**The offending text**, at `global.md:31-33`:

```
THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-20.22.36`.
AND ITS POSITION IS LOAD-BEARING, WHICH WAS LEARNED BY THIS CITATION GOING STALE UNSEEN. It sat
directly above a sentence carrying a `[STAMPED ...]` marker, and `strat_banner_sweep.py` read the
```

and, at `global.md:41`:

```
LIVE PROVENANCE CITATION OUT OF A STAMPED SENTENCE'S NEIGHBOURHOOD, or the guard goes quiet.
```

**The account of the DEFECT is accurate.** Verified: the pre-fix bytes — `git show
HEAD:Tools/architect/state/global.md`, which carry `reportCreatedOn 2026.09.06-20.00.49` while
the report on disk is `2026.09.06-20.22.36` — sweep CLEAN, exit 0. The adjacency exemption is
real and it did hide a stale live citation.

**The FIX does not work.** The identical wrong value placed in the CURRENT, post-fix position,
with nothing else touched, ALSO sweeps CLEAN, exit 0. The live provenance citation is still
exempt.

**The cause, isolated by a control.** The exempting marker is no longer the genuine
`[STAMPED 2026-09-06:` block further down — it is the literal token `[STAMPED ...]` inside the
NEW WARNING PARAGRAPH ITSELF, written to describe the hazard. `is_stamped` matches
`_PARAGRAPH_STAMP_MARKERS`'s bare `\[STAMPED` against it. Instrumented against the live file,
`check_report_provenance` part (a)'s own plus-or-minus-400-character window
(`_PROVENANCE_CITATION_WINDOW = 400`, with the citation's own span masked by `_mask_span`) for
the line-31 citation is:

```
THE LIVE FIGURE'S REPORT IS `                                   `.
AND ITS POSITION IS LOAD-BEARING, WHICH WAS LEARNED BY THIS CITATION GOING STALE UNSEEN. It sat
directly above a sentence carrying a `[STAMPED ...]` marker, and `strat_banner_sweep.py` read the
citation as stamped BECAUSE OF THE ADJACENCY -- ...
```

Offset of `[STAMPED` within that window: 572. Offset of the citation: 400. Distance 172
characters, far inside the 400 window. `is_stamped(window)` returns `True`; the marker hit is
`\[STAMPED`. The claim is therefore exempted and never compared.

**The control that proves this is the cause and not a broken check.** The same mutant — wrong
value in the current position — with ONLY the warning paragraph's own quoted token neutralised
(the phrase "a `[STAMPED ...]` marker" changed to "a supersession marker"), everything else
byte-identical:

```
  [**REPORT PROVENANCE**] global.md:31: cites `reportCreatedOn 2026.09.06-20.00.49`, but the
  report this sweep actually opened is `reportCreatedOn 2026.09.06-20.22.36` -- ...
SWEEP FAILED -- the record contradicts itself or the tree
```

**The four runs, with exit codes:**

| Run | Bytes | Verdict | Exit |
|---|---|---|---|
| Control — live tree, as it stands | unmodified | `SWEEP CLEAN` | 0 |
| Mutant A — wrong value, CURRENT (post-fix) position | live plus one value | `SWEEP CLEAN` | 0 |
| Mutant B — the actual PRE-fix bytes (`HEAD:global.md`) | unmodified `HEAD` | `SWEEP CLEAN` | 0 |
| Mutant C — Mutant A plus the quoted `[STAMPED ...]` token removed | live plus one value plus one phrase | `SWEEP FAILED` | 1 |

Mutant A is the one that matters: the guard is still quiet on the live citation today. The
record's own instruction — "KEEP A LIVE PROVENANCE CITATION OUT OF A STAMPED SENTENCE'S
NEIGHBOURHOOD" — has not been carried out, because the sentence that states it is itself a
stamped sentence by the script's reckoning.

This is the recorded "a census that quotes its own search token" failure, arriving in the banner:
the prose written to document a token-matching gap contains the token.

**Owner of the fix (record half):** the `coordinator`, as `global.md`'s writer, jointly with
`strat-data-steward`. The minimal correction is to move the live citation out of the
400-character window of EVERY `[STAMPED` or `CORRECTION,` token — including the ones the warning
paragraph quotes — and to re-measure with Mutant A rather than reason about it. Note that the
guard's own finding text also contains `[STAMPED ...]`, so quoting the finding near a citation
re-creates the gap a third time.

**Owner of the fix (script half): `strat-data-steward`.** `strat_banner_sweep.py` is that lane's
and is deliberately not touched here. Two measured inputs for that lane, neither of them a patch:

1. *A narrow tightening exists and is safe on the live record.* Replacing part (a)'s marker
   `\[STAMPED` with a form that refuses a literal ellipsis payload — so the METASYNTACTIC
   `[STAMPED ...]`, which is prose ABOUT a marker rather than a marker, stops counting — makes
   Mutant A FAIL (exit 1) while the live tree stays CLEAN (exit 0). The exact pattern measured
   was the original with a negative lookahead for optional whitespace, three dots and a closing
   bracket appended. Blast radius measured: the record carries 174 `[STAMPED` occurrences across
   `Tools/architect/state/*.md`, of which only 5 are the ellipsis form. A cruder narrowing that
   requires a date would silently disarm 28 genuine markers — including
   `[STAMPED, AND IT DESCRIBES THE P...` (12 times) and `[STAMPED AGAIN 2026-08-29` (2 times) —
   so do NOT take that route. `_PARAGRAPH_STAMP_MARKERS` is shared with check 1 (SUITE COUNT
   AGREEMENT), which is why this is offered as a measurement and not as an edit.
2. *That tightening does NOT close the originally reported defect.* Mutant B — the real pre-fix
   bytes, exempted by a GENUINE `[STAMPED 2026-09-06:` block — still sweeps CLEAN (exit 0) under
   the narrowed markers. The structural gap the entry actually describes is untouched by it and
   needs its own design: part (b) already solved the same problem with a narrower marker set
   (`_PROVENANCE_TREE_STAMP_MARKERS`), and part (a) has a live-declaration idiom ("THE LIVE
   FIGURE'S REPORT IS") that could be made unexemptable. That is a design call for the steward,
   not a finding against this pass.

### FINDING 2 — Observation-level, NOT a block. `content.md`'s no-clobber sentence is wider than the evidence behind it.

**File:line:** `Tools/architect/state/content.md`, in the new entry, the sentence:

```
THE WRITE WAS VERIFIED FROM A SEPARATE PROCESS, not from the in-process read and not from
`save_asset` returning true; the two controls above were re-read unchanged in that same cold
run, so nothing was clobbered alongside the four writes.
```

Two controls re-read unchanged support "these two were not clobbered". They do not support
"nothing was clobbered" — that is a claim about the whole CDO, and no instrument in this pass,
or in this gate, measured the whole CDO.

**And this gate could not close the gap either, which is stated rather than left as silence.** A
full BP-CDO-versus-native-CDO census was attempted twice from a headless commandlet and DID NOT
RUN as a census both times. Attempt 1 enumerated Python property descriptors on
`unreal.StratUnitActor` and found `n_props = 0` — a silent zero. Attempt 2 enumerated `dir()` on
the CDO: `n_tried = 223`, `n_readable = 30`, `n_diffs = 4` — and its own positive control
returned `sees_known_diff_body_z_offset: false`, i.e. the census could not see the known
50.0-versus-0.0 difference on `BodyZOffset`, so the four diffs it did report (`actor_guid`,
`actor_instance_guid`, `content_bundle_guid`, `root_component` — all structural) mean nothing
about this pass. An empty diff from a blind instrument is not a clean census, and is not reported
as one here. The seven named properties WERE verified from a cold process; the eighth claim was
not.

Suggested wording for the owner: "the two controls were re-read unchanged, so neither was
clobbered; no full-CDO census was run." Owner: the `coordinator`, as `content.md`'s fallback
writer.

## What this gate verified independently

### The material — `Content/StratArt/Materials/M_UnitDamageFlash.uasset`

Read back from a cold `-run=pythonscript` process started by this gate, with four controls:

```json
"mat": {
  "class": "Material",
  "blend_mode": "<BlendMode.BLEND_TRANSLUCENT: 2>",
  "shading_model": "<MaterialShadingModel.MSM_UNLIT: 0>",
  "material_domain": "<MaterialDomain.MD_SURFACE: 0>",
  "two_sided": "False",
  "num_expressions": 4,
  "scalar_params": ["FlashIntensity", "FlashOpacity"],
  "vector_params": ["FlashColor"],
  "scalar_FlashIntensity": 3.0,
  "scalar_FlashOpacity": 0.8500000238418579,
  "vector_FlashColor": "{r: 1.000000, g: 0.045000, b: 0.045000, a: 1.000000}"
},
"controls": {
  "absent_path": false,
  "present_path": true,
  "M_OverlayPulse_shading_model": "<MaterialShadingModel.MSM_DEFAULT_LIT: 1>",
  "M_OverlayPulse_two_sided": "False",
  "M_OverlayPulse_num_expressions": 11,
  "bogus_prop_raised": true
}
```

Every value the entry claims is confirmed, INCLUDING the two the task singled out —
`BLEND_TRANSLUCENT` and `MSM_UNLIT`. `two_sided: False` is confirmed here for the first time:
the pass's own `make_out.json` readback does not carry that field, so the entry's "not
two-sided" was, until this run, an unread setter value. It is now measured.

The controls make the four figures measurements rather than echoes: an absent path answers
`false` and a present one `true`, so the lookup can speak both ways; a bogus property name
RAISES, so a successful read is a read; and `M_OverlayPulse`, read by the same helper in the
same run, answers `MSM_DEFAULT_LIT` and 11 expressions against this asset's `MSM_UNLIT` and 4 —
the helper distinguishes assets.

### The Blueprint defaults — `Content/StratPlay/BP_StratUnit.uasset`

Same cold process:

```json
"bp": { "cdo_class": "StratUnitActor", "reads": {
  "BodyZOffset":           "50.0",
  "MoveTweenSeconds":      "0.20000000298023224",
  "MoveTweenEaseFraction": "0.25",
  "DamageFlashSeconds":    "0.20000000298023224",
  "DamageShakeAmplitude":  "8.0",
  "DamageShakeFrequency":  "20.0",
  "DamageFlashMaterial":   "<Object '/Game/StratArt/Materials/M_UnitDamageFlash.M_UnitDamageFlash' Class 'Material'>"
}}
```

All four writes persisted. Both stated controls confirmed: `BodyZOffset` reads `50.0` and
`MoveTweenSeconds` reads `0.2` where the C++ initialisers are `0.0` — which pins BOTH that a
protected `EditDefaultsOnly` UPROPERTY is being read successfully AND that this is the
BLUEPRINT's CDO rather than the native one. A `cdos_are_distinct: true` control from the second
probe confirms the two CDOs are different objects. The "nothing ELSE was clobbered" half is
Finding 2 and did not run.

### The LFS staging claim

```
$ git ls-files -s Content/StratArt/Materials/M_UnitDamageFlash.uasset
100644 c760ac4a0ec7c587169f37749a05c20fe98074c9 0	Content/StratArt/Materials/M_UnitDamageFlash.uasset

$ git cat-file -p :Content/StratArt/Materials/M_UnitDamageFlash.uasset
version https://git-lfs.github.com/spec/v1
oid sha256:eef5c98d5cc241d52dca817ebd3eed2079d813542ffe18b6eb7e7ffbd64acf9d
size 7207

$ sha256sum Content/StratArt/Materials/M_UnitDamageFlash.uasset
eef5c98d5cc241d52dca817ebd3eed2079d813542ffe18b6eb7e7ffbd64acf9d
$ stat -c %s Content/StratArt/Materials/M_UnitDamageFlash.uasset
7207
```

Staged oid and size equal the saved file's own hash and byte size exactly. The entry's claim
holds, and it is the interesting case: the standing hazard is that the auto-add stages an asset
AS CREATED rather than as saved, and this time it did not.

`BP_StratUnit.uasset` is correctly NOT auto-staged — its index blob is byte-identical to the one
at `HEAD` (`oid sha256:b153ee66...`, size 26925) while the working file hashes `84aa4351...` at
27573 bytes, so the modification is unstaged, as the entry says.

Both assets are LFS-tracked like their neighbours. `git check-attr -a` returns `filter: lfs`,
`diff: lfs`, `merge: lfs`, `text: unset` on both, and on `M_OverlayPulse.uasset` as the control.

### The clause situation

Both authorities are cited and neither is stretched. The entry reads "ACTING: the coordinator,
under CLAUDE.md's EDITOR-DRIVER CLAUSE. WRITING: the coordinator, under THIS FILE'S FALLBACK
CONDITION" — which is exactly the split `content.md`'s own header demands: "Cite THAT for the
acting and THIS for the file write; an entry that cites one authority for both halves is a
finding." The absence is measured at the LANE AGENT'S TOOL SURFACE, not at the editor, which is
what the 2026-09-04 route ruling requires, and it carries its control: the same lookup returned
`list_unreal_projects` and `unreal_status` from the same server, so the null is a measurement
and not a broken query. The route — a headless `UnrealEditor-Cmd.exe -run=pythonscript`
commandlet with no editor running — is named, and the ruling's "any route is covered" holds.
Nothing here is a finding.

The entry's honesty about the visual claim is flat and unhedged and needs nothing flagged:
"NOBODY HAS SEEN THIS FLASH", with compilation, pixels, whether unlit translucent reads as a
flash, whether 0.2 s and 8 uu are right, and whether the three markers survived the Body-to-Shake
re-parent all listed as open and all needing a human at the keyboard.

### Module arrows, derived

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
ROWS=16
```

Field census — 10, as the checklist states: `BRIDGE=` (the known false positive, the tail of the
string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="` and not a field), `PCHUsage =`,
`PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`,
`PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`,
`bUseUnity =`. No eleventh name.

All five modules are present, so the extraction did not silently fail. The 16th row is confirmed
CONDITIONAL by reading the branch, `Source/StratBridge/StratBridge.Build.cs:133-138`:

```csharp
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
```

Monolithic-only, from the one expression that also drives the compile-in. Correct, not a
finding. No cycle; `StratUI` has gained no `Stratocracy` dependency.

## The instruments, inlined

Neither of these is cited by path; they are reproduced here so a checkout can re-run them.

The module-arrow derivation and the field census are the two commands carried verbatim in the
reviewer checklist's item 5; they were extracted and executed, not retyped, and printed the 16
rows and 10 names above.

This gate's asset probe, run as `UnrealEditor-Cmd.exe "<project>" -run=pythonscript
-script=<file> -unattended -nopause -nosplash -nullrhi`, process exit 0:

```python
import unreal, json, traceback
res = {"mat": {}, "bp": {}, "controls": {}, "error": None}
try:
    eal = unreal.EditorAssetLibrary
    mel = unreal.MaterialEditingLibrary
    MP = "/Game/StratArt/Materials/M_UnitDamageFlash"
    # CONTROLS: the lookup must answer BOTH ways, or a False is a broken query.
    res["controls"]["absent_path"]  = bool(eal.does_asset_exist("/Game/StratArt/Materials/M_NoSuchThingGate"))
    res["controls"]["present_path"] = bool(eal.does_asset_exist("/Game/StratPlay/BP_StratUnit"))
    m = eal.load_asset(MP)
    d = res["mat"]; d["class"] = type(m).__name__
    for p in ["blend_mode", "shading_model", "material_domain", "two_sided"]:
        d[p] = str(m.get_editor_property(p))
    d["num_expressions"] = mel.get_num_material_expressions(m)
    d["scalar_params"]   = [str(n) for n in mel.get_scalar_parameter_names(m)]
    d["vector_params"]   = [str(n) for n in mel.get_vector_parameter_names(m)]
    for n in d["scalar_params"]:
        d["scalar_" + n] = mel.get_material_default_scalar_parameter_value(m, unreal.Name(n))
    for n in d["vector_params"]:
        d["vector_" + n] = str(mel.get_material_default_vector_parameter_value(m, unreal.Name(n)))
    # CONTROL: the same reads on the project's existing master, so a value above
    # is a measurement and not an echo of the write.
    c = eal.load_asset("/Game/StratArt/Materials/M_OverlayPulse")
    res["controls"]["M_OverlayPulse_shading_model"]   = str(c.get_editor_property("shading_model"))
    res["controls"]["M_OverlayPulse_two_sided"]       = str(c.get_editor_property("two_sided"))
    res["controls"]["M_OverlayPulse_num_expressions"] = mel.get_num_material_expressions(c)

    bp  = eal.load_asset("/Game/StratPlay/BP_StratUnit")
    cdo = unreal.get_default_object(bp.generated_class())
    res["bp"]["cdo_class"] = type(cdo).__name__
    reads = {}
    for prop in ["BodyZOffset", "MoveTweenSeconds", "MoveTweenEaseFraction",
                 "DamageFlashSeconds", "DamageShakeAmplitude",
                 "DamageShakeFrequency", "DamageFlashMaterial"]:
        try:
            reads[prop] = str(cdo.get_editor_property(prop))
        except Exception as e:
            reads[prop] = "<UNREACHABLE: %s>" % e
    res["bp"]["reads"] = reads
    # CONTROL: a bogus property name must RAISE, or a successful read proves nothing.
    try:
        cdo.get_editor_property("NoSuchPropertyGate")
        res["controls"]["bogus_prop_raised"] = False
    except Exception:
        res["controls"]["bogus_prop_raised"] = True
except Exception:
    res["error"] = traceback.format_exc()
# THE REPORT CHANNEL MUST BE A FILE THE SCRIPT WRITES ITSELF -- print() from a
# pythonscript commandlet never reaches the log, as this pass itself measured.
with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
```

The sweep-gap instrumentation behind Finding 1, run against the live
`Tools/architect/state/global.md`:

```python
import io, sys
sys.path.insert(0, r"E:\MultiAgent\Stratocracy\Tools\architect")
import strat_banner_sweep as S
text = io.open(r"E:\MultiAgent\Stratocracy\Tools\architect\state\global.md",
               encoding="utf-8").read()
for start, para in S.paragraphs_with_lines(text):
    for m in S._CITED_REPORT_STAMP_RE.finditer(para):
        line_no = start + para[:m.start()].count("\n")
        if line_no != 31:
            continue
        ws = max(0, m.start() - 400)
        we = min(len(para), m.end() + 400)
        w = S._mask_span(para[ws:we], m.start() - ws, m.end() - ws)
        print("is_stamped:", S.is_stamped(w))
        print("offset of [STAMPED in window:", w.find("[STAMPED"),
              "citation offset:", m.start() - ws)
```

Output: `is_stamped: True`, `offset of [STAMPED in window: 572  citation offset: 400`.

The narrowed-marker measurement offered to `strat-data-steward`. Note the rebinding of the
default: `is_stamped`'s `markers=` default is bound at `def` time, so reassigning the module
attribute alone does nothing and produces a FALSE "no change" reading. That trap was hit and
corrected during this gate, and it is exactly the "unread is not clean" shape.

```python
new = tuple((NARROWED if m == ORIGINAL else m) for m in S._PARAGRAPH_STAMP_MARKERS)
S._PARAGRAPH_STAMP_MARKERS = new
S.is_stamped.__defaults__ = (new,)      # the default was bound at def time
S.main(sorted(glob.glob(os.path.join(path, "*.md"))))
```

where ORIGINAL is the escaped literal `[STAMPED` and NARROWED is that same literal followed by a
negative lookahead for optional whitespace, three dots and a closing bracket. Live tree exits 0;
Mutant A exits 1; Mutant B exits 0.

## Observations

Non-gating.

1. **The staged material.** Checklist item 12 says unexpected staged content is a finding; this
   staged content is EXPECTED, explained and measured, so it is recorded here rather than as a
   finding. It does mean the user's commit of this pass will pick up
   `M_UnitDamageFlash.uasset` whether or not it is re-added.

2. **The evidence directory has no `blackboard.md`.** Seven of the nine numbered directories
   under `Tools/architect/evidence/` carry one; `11-damage-flash-assets/` does not. A convention,
   not a rule, and not enforced by anything.

3. **The committed scripts do not write where the entry implies.** All four carry an `OUT` path
   under `Tools/architect/scratch/`, while the committed JSON sits in the evidence directory
   beside them. A checkout that re-runs them, as the entry invites, gets its output in
   `Tools/architect/scratch/` — which is NOT gitignored (measured: `git check-ignore` exits
   non-zero on a path under it) — so a re-run leaves an untracked directory in `git status`.
   Cheap fix: point `OUT` at the script's own directory. Owner: whoever writes the evidence next.

4. **`probe_materials.py` does not probe the new material.** It reads `M_OverlayPulse`,
   `MI_Side_0` and `MI_Overlay_Target` — it predates the asset it is offered as the read pattern
   for. Harmless, but a reader sent there to "see the read pattern" will not find
   `M_UnitDamageFlash` in it. The pattern that DOES read it is `make_flash_material.py`'s
   readback block, and this gate's own probe above.

5. **The evidence is untracked at gate time.** The entry says so in terms ("uncommitted at time
   of writing"), which is the honest form, but the standing lesson applies: commit the evidence
   in the same commit as the record that cites it, or the citation resolves to nothing on a
   fresh checkout.

6. **`.agents/ue-project-context.md` was checked against the tree for drift and needed none** on
   the axes this pass touches: the module table's `StratBridge` row correctly describes the
   conditional link edge, and the two read-only-territory rows correctly defer to the manifests
   rather than to a quoted commit.

7. **The suite figure is not in dispute and was re-read here.** `Saved/AutomationReport/index.json`
   read with `utf-8-sig` gives `reportCreatedOn 2026.09.06-20.22.36`, `succeeded 445`, `failed 0`,
   `notRun 0`, and the sweep's own macro census agrees at 445. The BLOCK is about whether the
   GUARD over that citation works, not about the number.

VERDICT: BLOCK
