# Gate: unit damage alert — ASSET PASS — REGATE — strat-integration-reviewer

2026-09-06 (local). Working tree at `E:/MultiAgent/Stratocracy` on branch `master`, base commit
`4a01418`, uncommitted. This is the RE-GATE of the CONTENT half, following
`2026-09-06-unit-damage-alert-assets-gate.md`, which returned `BLOCK` on one finding: a
sweep-gap fix that did not work.

Gate priority is CODE AND ASSETS FIRST, RECORD SECOND, per the standing user ruling.

## Scope

Six paths, from `git status --porcelain`, unchanged in shape from the predecessor gate except
that its own report is now present as an untracked file:

```
A  Content/StratArt/Materials/M_UnitDamageFlash.uasset
 M Content/StratPlay/BP_StratUnit.uasset
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
?? Tools/architect/evidence/11-damage-flash-assets/
?? Tools/architect/gate_reports/2026-09-06-unit-damage-alert-assets-gate.md
```

`git status --porcelain -- Source Config Stratocracy.uproject Data` printed NOTHING. No C++,
no config, no vendored bytes moved this pass. Measured, not assumed.

Only ONE file changed since the predecessor gate: `Tools/architect/state/global.md`. The two
assets are bit-identical to the bytes that gate probed — proven by hash below — so the asset
half is carried forward deliberately and the mutant work is redone from scratch here.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat -- Source/StratRules Data` printed NOTHING; `git status --porcelain` scoped to both printed NOTHING |
| 2 | No `strat::` call outside `StratBridge/`, `StratRules/` | pass — a lowercase-initial `strat::` call grep over `Source/` minus those two directories returned ZERO lines. No C++ moved this pass |
| 3 | No vendored header in front of UHT | pass — every include of a vendored header from a `.h` is inside `Source/StratBridge/StratBridge.h` (declares no reflected type) or `Source/StratRules/`. Full list inlined below |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the single hit is `Source/StratUI/StratViewModel.h:47`, a comment recording the `C4150` measurement, not a declaration. Judged, not forwarded |
| 5 | Module arrows unchanged and correct | pass — DERIVED, not trusted. 16 rows, 10 field names, both matching the checklist figures; the 16th row confirmed GUARDED at `StratBridge.Build.cs:133-138`. Output inlined below |
| 6 | New modules registered (unless no `IMPLEMENT_MODULE`) | pass — no new module directory; no `Source/` file moved at all |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed NOTHING |
| 8 | No `/Game/` path literal in gameplay C++ | pass — non-comment hits are all under `Source/*/Tests/` (automation fixtures) plus ONE, `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the import commandlet. Both are the standing exceptions CLAUDE.md names. Judged, not forwarded |
| 9 | No widget-side arithmetic | pass — nothing under `Source/StratUI/` changed |
| 10 | Lane discipline | pass — `Content/` and `content.md` written by the coordinator under two separately-cited authorities; `global.md`'s writer is already the coordinator. `strat_banner_sweep.py` was NOT touched — it is `strat-data-steward`'s lane, and `git status` scoped to it printed nothing |
| 11 | Prose blocks present | pass — the four evidence scripts carry why-first comments recording measurements as measurements; both record entries are in house style |
| 12 | Nothing staged or committed | pass with a note — `M_UnitDamageFlash.uasset` IS staged (`A`) by the editor source-control auto-add, a recorded standing behaviour, re-measured here. `HEAD` is still `4a01418`; nothing is committed. See Observation 1 |

### Specifically-requested checks

| Ask | Result |
|---|---|
| 1. Reproduce both mutant directions; verify the restore left no residue | pass — reproduced independently, both directions, plus a byte-identity check on the restore |
| 2. No reintroduction of the exemption by another token; no other live banner claim left adjacent to a marker | pass — the live banner carries exactly ONE marker occurrence, at line 39, and it stamps only what it is meant to |
| 3. The recorded account is accurate and does not overclaim the fix | pass — it says in terms that the script defect is NOT fixed, and names the owning lane |
| 4. Asset half re-verified / carried forward, distinguishably | pass — re-measured: both assets' bytes, LFS staging, `check-attr`. Carried forward: the two commandlet probes. Section below states which is which |
| 5. content.md honesty about what no commandlet can establish | pass — all five named unknowns present, flat and unsoftened |
| 6. Evidence directory hygiene | pass — no secrets, no user-profile paths; not gitignored, so committable |
| 7. Ready to commit? | yes, with what remains open stated below |

## Findings

None.

The predecessor's blocking FINDING 1 is DISCHARGED. Verified below by this seat's own mutant,
not by accepting the transcript.

## The blocking finding, re-measured from scratch

### The window around the live citation is no longer stamped

Instrumented against the live `Tools/architect/state/global.md`, using the script's own
functions and its own constants rather than a re-implementation:

```python
import io, sys, re
sys.path.insert(0, r"E:\MultiAgent\Stratocracy\Tools\architect")
import strat_banner_sweep as S
text = io.open(r"E:\MultiAgent\Stratocracy\Tools\architect\state\global.md",
               encoding="utf-8").read()
for start, para in S.paragraphs_with_lines(text):
    for m in S._CITED_REPORT_STAMP_RE.finditer(para):
        line_no = start + para[:m.start()].count("\n")
        if line_no != 31:
            continue
        ws = max(0, m.start() - 400); we = min(len(para), m.end() + 400)
        w = S._mask_span(para[ws:we], m.start() - ws, m.end() - ws)
        print("is_stamped:", S.is_stamped(w))
        for mk in S._PARAGRAPH_STAMP_MARKERS:
            for mm in re.finditer(mk, w):
                print("MARKER HIT:", mk, "@", mm.start())
```

Output: `is_stamped: False`, and **not one marker hit of any kind** — not the bracketed stamp
token, not a doubled-asterisk CORRECTION form, not a comma-suffixed CORRECTED form, not the
bare `reportCreatedOn` marker (the citation's own span is masked by `_mask_span`, so it cannot
exempt itself). The predecessor gate's identical instrument printed `is_stamped: True` with the
stamp token at offset 572. The window itself, printed in full, ends mid-word at
`THE LOAD-BEARING MEASUREMENT WAS TAKEN BEF` — the genuine stamp block at line 39 is now
outside the 400-character reach.

### The mutant, both directions, run by this seat

| Run | Bytes | Verdict | Exit |
|---|---|---|---|
| Control — live tree as it stands | unmodified | `SWEEP CLEAN` | 0 |
| Mutant A — the same wrong value planted on line 31 | live plus one value | **`SWEEP FAILED`** | **1** |
| Restore | backup copied back | `SWEEP CLEAN` | 0 |

The plant was made programmatically on line 31 only, with newline handling pinned so no line
endings moved:

```python
lines[30] = lines[30].replace("2026.09.06-20.22.36", "2026.09.06-20.00.49")
```

Planted line: THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-20.00.49`.

Mutant A's finding, quoted from the run:

```
  [**REPORT PROVENANCE**] global.md:31: cites `reportCreatedOn 2026.09.06-20.00.49`, but the
  report this sweep actually opened is `reportCreatedOn 2026.09.06-20.22.36` -- an unstamped,
  unquoted citation of a DIFFERENT run is a stale claim about which report backs a live figure.
SWEEP FAILED -- the record contradicts itself or the tree
```

**The guard on `global.md:31` is armed.** The control matters as much as the mutant: the
unmutated tree exits 0, so the exit-1 is the plant being caught and not the sweep being broken.

### The restore left no residue — measured, not assumed

`sha256sum Tools/architect/state/global.md` before the plant and after the restore:

```
e68f3e549e87e5ad4c0b97daef888834d07b2c107e3b1e3dcee77c791a68df83   (before)
e68f3e549e87e5ad4c0b97daef888834d07b2c107e3b1e3dcee77c791a68df83   (after)
```

Byte-identical. `git status --porcelain` after all mutation work prints the same six lines it
printed at session start — no extra path, and no `dev/null/` directory (checked: `ls dev`
fails). The sweep exits 0 on the restored file. Three further mutants were run in this gate
(below); every one was restored from the same backup, and the hash above is after ALL of them.

## Check 2 — is the exemption reintroduced anywhere else in the live banner?

The defect is general, so this was audited across the whole live banner rather than only at the
citation. `current_banner_end(text)` returns **180**, so the live region is lines 1-180.

**Marker census over the live banner** — every `_PARAGRAPH_STAMP_MARKERS` pattern except the
bare `reportCreatedOn` form (enumerated separately as citations), matched line by line. The
entire output:

```
  L39   \[STAMPED   | IN IT, and the figure it returned [STAMPED 2026-09-06: a MID-PASS reading, superseded within the
```

**Exactly one marker occurrence in the entire live banner.** No BANNER CORRECTED form, no
doubled-asterisk CORRECTED or CORRECTION form, no comma-suffixed CORRECTED or CORRECTION form.
The rewritten paragraph introduces no token of any kind — checked against the full marker tuple,
not against the bracketed stamp token alone, which is the narrower check that would have missed
a CORRECTION-form reintroduction.

**Bare-citation census:** the only `_CITED_REPORT_STAMP_RE` hit inside the live banner is line
31 itself. The next is line 212, far outside. So no second citation can exempt the first.

**What that one marker at L39 governs, mutated to prove it:**

| Run | Mutation | Verdict | Exit | Correct? |
|---|---|---|---|---|
| Mutant B | live suite figure, L24 445/445 changed to 999/999 | `SWEEP FAILED` | 1 | YES — the live figure is guarded |
| Mutant C | stamped figure, L44 "stood at 435/435" changed to 999/999 | `SWEEP CLEAN` | 0 | YES — that figure is DELIBERATELY stamped by L39 |

Mutant B's finding, quoted:

```
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the tree's total (445): global.md:24: 999/999
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the report's own succeeded count (445): global.md:24: 999/999
```

Mutant C is the one that could be mistaken for the defect and is not: the L39 stamp exists
precisely to mark the 435/435 mid-pass reading as history, and its own text says so. An
exemption that lands on the sentence its stamp was written for is the guard working, not the
adjacency hole.

**Part (b) — tree/branch provenance claims.** Enumerated with `_PROVENANCE_PATH_BRANCH_RE` over
the live banner: **0 candidates**. Nothing there to be silently disarmed.

So: one marker, it reaches exactly one claim, and that claim is the one it was written for. The
two live claims that matter — the 445/445 figure and the `reportCreatedOn` citation — are both
caught when falsified.

## Check 3 — does the recorded account overclaim the fix?

It does not. The new `global.md` paragraph, in the WHAT THE GATES CHANGED region, quoted:

```
WHAT IS FIXED HERE AND WHAT IS NOT. Fixed: this file no longer names the token near the citation,
and the mutant now fails as it should. NOT FIXED: `strat_banner_sweep.py` still exempts by
ADJACENCY rather than by whether the marker actually governs the citing sentence, so the next
citation written next to a genuine stamp is silently unguarded. The script is
`strat-data-steward`'s, not this seat's.
```

Three things this gate checked rather than accepted:

1. **It does not read as though the instrument was repaired.** "Fixed" is scoped to *this file*
   in the same sentence that names what is not. The remedy is described as the citation being
   moved, never as the script being changed.
2. **The script really is untouched.** `git status --porcelain` scoped to
   `Tools/architect/strat_banner_sweep.py` prints nothing. The lane boundary held.
3. **The defect is recorded as open and routed.** It names `strat-data-steward` as the owner
   and cites the predecessor's analysis by filename rather than restating its numbers.

The paragraph at lines 32-35, sitting beside the citation, states the hazard **without naming
the token** — "AND THE STAMP TOKEN IS DELIBERATELY NOT WRITTEN ANYWHERE NEAR THIS LINE" — and
forwards the reader to the full account further down. That is the shape that was measured to
work, and the marker census above confirms it introduced nothing.

One note, not a finding: the citation of
`Tools/architect/gate_reports/2026-09-06-unit-damage-alert-assets-gate.md` is to an UNTRACKED
file. See Observation 3.

## Check 4 — the asset half: what was RE-MEASURED here, and what is CARRIED FORWARD

**RE-MEASURED BY THIS GATE.** The two assets are byte-identical to the bytes the predecessor
probed, which is what makes the carry-forward legitimate rather than an assumption:

```
$ sha256sum Content/StratArt/Materials/M_UnitDamageFlash.uasset Content/StratPlay/BP_StratUnit.uasset
eef5c98d5cc241d52dca817ebd3eed2079d813542ffe18b6eb7e7ffbd64acf9d  M_UnitDamageFlash.uasset
84aa4351da874bd3cbfff3e9bc50a644c7e012184cc84eb76e682fe57a65d228  BP_StratUnit.uasset
$ stat -c %s ...
7207    M_UnitDamageFlash.uasset
27573   BP_StratUnit.uasset
```

Both match the predecessor report's figures exactly (eef5c98d.../7207 and 84aa4351.../27573).

LFS staging, re-measured in full:

```
$ git cat-file -p :Content/StratArt/Materials/M_UnitDamageFlash.uasset
oid sha256:eef5c98d5cc241d52dca817ebd3eed2079d813542ffe18b6eb7e7ffbd64acf9d
size 7207
$ git cat-file -p HEAD:Content/StratPlay/BP_StratUnit.uasset
oid sha256:b153ee667496ffe0c3f2c86331d9778abcd88141542cdfaa55930ab58364339c
size 26925
$ git cat-file -p :Content/StratPlay/BP_StratUnit.uasset
oid sha256:b153ee667496ffe0c3f2c86331d9778abcd88141542cdfaa55930ab58364339c
size 26925
```

The staged pointer for the new material equals the working file's own hash and byte size — the
auto-add staged the SAVED bytes, not the created ones, which is the standing hazard and it did
not fire. The `BP_StratUnit.uasset` index blob is identical to HEAD's while the working file
hashes differently, so its modification is correctly unstaged. `git check-attr -a` returns
`filter: lfs`, `diff: lfs`, `merge: lfs`, `text: unset` on both assets and on
`M_OverlayPulse.uasset` as the control.

**CARRIED FORWARD FROM `2026-09-06-unit-damage-alert-assets-gate.md`, NOT RE-RUN HERE.** No
commandlet was launched by this gate. The following rest on the predecessor's cold
`-run=pythonscript` probes and are reported as inherited, not as this seat's measurement:

- the material's BLEND_TRANSLUCENT, MSM_UNLIT, MD_SURFACE, two_sided False, 4 expressions, and
  the three parameter values (FlashIntensity 3.0, FlashOpacity 0.85, FlashColor
  1.0/0.045/0.045/1.0);
- the four `BP_StratUnit` CDO defaults and the controls (BodyZOffset 50.0, MoveTweenSeconds 0.2,
  cdos_are_distinct, bogus_prop_raised);
- the four probe controls that make those figures measurements rather than echoes.

The warrant for carrying them is the hash identity above and nothing else. If either asset had
moved by one byte, none of it would transfer.

**And the gap in that half is inherited too, and is still open:** the `content.md` sentence
"nothing was clobbered alongside the four writes" is wider than the two controls behind it, and
the predecessor's own attempt at a full-CDO census did not run (its positive control returned
`sees_known_diff_body_z_offset: false`). The sentence is unchanged in the working tree. Recorded
as Observation 2, at the severity the predecessor set, not re-graded.

## Check 5 — content.md honesty

Accurate and unsoftened. Quoted in full from the new entry:

```
**WHAT NO INSTRUMENT HERE CAN SAY, AND IT IS THE WHOLE VISUAL CLAIM.** A commandlet cannot
prove a material COMPILES and cannot prove it has PIXELS; `-nullrhi` compiles no shaders, and
this project has already measured `get_statistics` returning zeros for a known-good asset and
translation errors logging nothing. **NOBODY HAS SEEN THIS FLASH.** Whether the overlay pass
renders at all, whether unlit translucent reads as a flash rather than as a flat red decal,
whether `0.2 s` and `8` uu are right, and whether the three markers still sit at their offsets
after the `Body` -> `Shake` re-parent are ALL open and ALL need a human at the keyboard.
```

All five items the task names are present: compilation, pixels, whether unlit translucent reads
as a flash, whether 0.2 s and 8 uu are right, and whether the markers survived the Body-to-Shake
re-parent. None is hedged into a half-claim; "NOBODY HAS SEEN THIS FLASH" is flat. The route is
named (headless `UnrealEditor-Cmd.exe -run=pythonscript`, no editor running), and the clause
condition is measured at the LANE AGENT'S TOOL SURFACE with its control — the same lookup served
`list_unreal_projects` and `unreal_status` while returning no `execute_script`, which is what the
2026-09-04 route ruling requires. Acting and writing are attributed separately to two named
authorities. Nothing here is a finding.

## Check 6 — evidence directory hygiene

`Tools/architect/evidence/11-damage-flash-assets/` holds eight files: four Python scripts and
four JSON outputs, 480-4013 bytes each. Checked and clean:

- **No secrets.** A case-insensitive grep for password / secret / token / api-key / user-profile
  path shapes over the directory returns ZERO lines.
- **No user-profile or machine-specific paths.** The only absolute paths are four OUT targets
  under `E:\MultiAgent\Stratocracy\Tools\architect\scratch\`, the repo root this project writes
  throughout.
- **Not gitignored.** `git check-ignore -v` exits non-zero on a path inside it, so the whole
  directory is committable and the record's citations will resolve once committed.
- **Nothing that should not be committed.** No binaries, no logs, no Saved/ spill.

Two inherited untidiness items about this directory are carried as Observations 4 and 5.

## Module arrows, derived

Derived by extracting and executing the checklist's own command, not retyped:

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

All FIVE modules present, so the extraction did not silently fail — an absent module would
itself have been a finding. The field census returns **10** names, no eleventh: the known
false-positive BRIDGE tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="` (not a
field, and deliberately not stripped away), `PCHUsage =`, `PrivateDefinitions.Add`,
`PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`,
`PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =`.

The 16th row is confirmed CONDITIONAL by reading the branch — the extraction is textual and
cannot show this:

```csharp
// Source/StratBridge/StratBridge.Build.cs:133-138
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
```

Monolithic targets only, from the one expression that also drives the compile-in. Correct, not
a finding. `StratRules` appears in no `PublicDependencyModuleNames` and in no unconditional
`PrivateDependencyModuleNames`. No cycle; `StratUI` has gained no `Stratocracy` dependency.

## The vendored-header check, in full

Every include of a vendored header from a `.h` file in the tree:

```
Source/StratBridge/StratBridge.h:53,55,56,57,58   Data.h, Replay.h, Save.h, Scenario.h, Ui.h
Source/StratRules/Ai.h:20        Source/StratRules/Driver.h:12,16,18
Source/StratRules/Economy.h:14   Source/StratRules/Move.h:8
Source/StratRules/Replay.h:46,50,51                Source/StratRules/Scenario.h:64
Source/StratRules/Ui.h:22,26
```

All inside `Source/StratBridge/StratBridge.h` (which declares no UCLASS, USTRUCT or UENUM) or
`Source/StratRules/`. Every other reference to `StratBridge.h` in the tree is from a `.cpp`.
Judged individually; none forwarded as a violation.

## Observations

Non-gating.

1. **The staged material.** Checklist item 12 says unexpected staged content is a finding; this
   staged content is EXPECTED, explained in the record and re-measured here, so it is recorded
   rather than reported. It means the user's commit will pick up `M_UnitDamageFlash.uasset`
   whether or not it is re-added.

2. **The content.md no-clobber sentence is still wider than its evidence.** Unchanged from the
   predecessor's FINDING 2: "the two controls above were re-read unchanged in that same cold
   run, so nothing was clobbered alongside the four writes." Two controls support "these two
   were not clobbered", not "nothing was". No full-CDO census has run — the predecessor's
   attempt returned `sees_known_diff_body_z_offset: false` on its own positive control, so its
   empty diff proves nothing. Suggested narrowing, unchanged: "the two controls were re-read
   unchanged, so neither was clobbered; no full-CDO census was run." Owner: the coordinator, as
   the content.md fallback writer. Reported twice now and still open.

3. **Two live citations point at untracked files.** The new `global.md` paragraph cites
   `Tools/architect/gate_reports/2026-09-06-unit-damage-alert-assets-gate.md`, and `content.md`
   says the evidence scripts "are committed at
   `Tools/architect/evidence/11-damage-flash-assets/`" — a present-tense claim that is false
   right now, both paths being untracked in `git status`. The entry does open by declaring
   itself "uncommitted at time of writing", so this is internal tension rather than a bare
   falsehood. Both resolve if the user commits all paths together; a commit that drops the
   untracked ones would leave two citations resolving to nothing on a fresh checkout.

4. **The evidence scripts write to `Tools/architect/scratch/`, not to their own directory.**
   All four carry an OUT path under `Tools/architect/scratch/` while the committed JSON sits
   beside the scripts. That directory is not gitignored (`git check-ignore` exits non-zero), so
   a checkout that re-runs them — as the entry invites — gets an untracked directory in
   `git status`. Cheap fix: point OUT at the script's own directory.

5. **`probe_materials.py` does not probe the new material.** It reads `M_OverlayPulse`,
   `MI_Side_0` and `MI_Overlay_Target` — it predates the asset it is offered beside. The
   readback that DOES cover `M_UnitDamageFlash` is the readback block inside
   `make_flash_material.py`, plus the predecessor gate's own probe. Harmless; a reader sent
   there will not find what they expect.

6. **No blackboard.md in the evidence directory.** Seven of the nine numbered directories under
   `Tools/architect/evidence/` carry one. A convention, not a rule, enforced by nothing.

7. **The general adjacency defect in `strat_banner_sweep.py` remains open**, correctly, and is
   recorded as open. It is routed to `strat-data-steward`. The fix in this pass is a LOCAL
   workaround in one file: any future citation written next to a genuine stamp is silently
   unguarded again, and nothing in the tree will say so.

8. **`.agents/ue-project-context.md` was checked for drift against the tree** on the axes this
   pass touches and needed none — the module table's `StratBridge` row correctly describes the
   conditional link edge, and the read-only-territory rows defer to the manifests rather than
   quoting a commit.

## Ready to commit?

**Yes.** All the pass's paths belong together in one commit, and the untracked ones —
`Tools/architect/evidence/11-damage-flash-assets/` and the predecessor's gate report — must go
in with it, or Observation 3's two citations resolve to nothing. This report belongs in the
same commit.

What remains open after the commit, none of it gating this pass:

- **Nobody has seen the flash.** Compilation, pixels, whether unlit translucent reads as a
  flash, whether 0.2 s and 8 uu are right, and whether the three markers survived the
  Body-to-Shake re-parent all need a human at the keyboard. The record says so plainly.
- **The `strat_banner_sweep.py` adjacency exemption** — `strat-data-steward`'s lane, routed.
- **The content.md no-clobber sentence** — Observation 2, twice-reported, unrepaired.
- **This report describes THIS tree.** Any edit made after it — including narrowing that
  sentence — invalidates it and needs a re-gate.

VERDICT: PASS
