# Gate: fourteenth stale-claim pass (shape 6) - strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base commit
`92d78c1`. Four modified files, nothing staged, nothing untracked. `git status --porcelain`
is the authority and returned exactly:

```
 M Source/StratPlay/StratCameraPawn.h
 M Source/StratPlay/StratGuidedOpening.h
 M Source/StratUI/StratGuidanceWidget.h
 M Tools/architect/state/engine.md
```

Diff is 219 insertions / 2 deletions. All three header changes are comment-only, verified
below with a speaking mutation control.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass - `git diff --stat -- Source/StratRules Data` empty; control: same command on `Source/StratPlay` printed 2 files |
| 2 | No `strat::` call outside StratBridge/StratRules | pass - every lowercase-initial call is under `Source/StratBridge/`; filter control below |
| 3 | No vendored header in front of UHT | pass - 59 real UHT-declaring headers, none includes a vendored header; control fired on `StratScoreboardHUD.h:367` |
| 4 | `TPimplPtr` not `TUniquePtr` | pass - both real members are `TPimplPtr`; the single `TUniquePtr` hit is a comment |
| 5 | Module arrows unchanged and correct | pass - 16 rows, 5 modules, 10 field names; no `.Build.cs` in the diff |
| 6 | New modules registered | pass - no new modules; `.uproject` untouched; `StratRules` correctly has 0 `IMPLEMENT_MODULE` |
| 7 | `Source/Stratocracy/` untouched | pass - `git diff --stat` empty, with the control above |
| 8 | No `/Game/` literal in C++ | pass - single hit is the import commandlet, a standing exception, in an untouched module |
| 9 | No widget-side arithmetic | pass - `Source/StratUI/` change is comment-only; no added arithmetic |
| 10 | Lane discipline | pass - engineer's lane: C++ outside `Tests/`, plus its own `engine.md` |
| 11 | Prose blocks present | **BLOCK** - see finding 1 |
| 12 | Nothing staged or committed | pass - `git diff --cached --stat` empty; the command sees 4 unstaged files |

### Check 2, filter control

The exclusion returned nothing, so the filter itself was controlled on synthetic input:

```
Source/StratUI/Fake.cpp:1: strat::doThing(x);     <- passed through
Source/StratPlay/Fake.cpp:2: strat::other(y);     <- passed through
Source/StratBridge/Real.cpp:3: strat::legal(z);   <- correctly suppressed
```

The filter is not eating everything; the null is real.

### Check 5, derived not trusted

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules        <- the conditional 16th row
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
StratRules   PublicDependencyModuleNames    Core
StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
Stratocracy  PublicDependencyModuleNames    ...,StratRules
```

All five modules present, so the extraction did not silently fail. The sixteenth row is the
guarded `.Add(`, confirmed conditional by reading `StratBridge.Build.cs:133-137`:
`bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic`, then
`if (!bCompileVendoredRulesHere)` adds `StratRules`. Monolithic-only, deliberate, not a
finding. The field census returned the expected ten names including the documented `BRIDGE=`
false positive; no eleventh name.

## Findings

### 1. BLOCK - a positive control stated with figures that do not reproduce

**Constraint:** checklist 11, prose blocks must record measurements as measurements. This is
a correction block whose entire warrant is that it measures rather than asserts.

`Source/StratPlay/StratGuidedOpening.h:165-168`:

> `/Script/StratUI.StratGuidanceWidget` returns 0 on it, against 4 on each of
> `WBP_DirectiveStrip`, `WBP_Scoreboard`, `WBP_Options` and `WBP_CommandBar`
> as positive controls

Measured on the worktree package bytes:

```
WBP_DirectiveStrip    /Script/StratUI.StratGuidanceWidget   = 4
WBP_Scoreboard        /Script/StratUI.StratGuidanceWidget   = 0
WBP_Options           /Script/StratUI.StratGuidanceWidget   = 0
WBP_CommandBar        /Script/StratUI.StratGuidanceWidget   = 0
```

The named token returns 4 on ONE of the four assets, not on each. What is actually true is
that each asset carries **its own** parent token exactly four times:

```
WBP_DirectiveStrip -> 4 /Script/StratUI.StratGuidanceWidget
WBP_Scoreboard     -> 4 /Script/StratUI.StratScoreboardWidget
WBP_Options        -> 4 /Script/StratUI.StratOptionsWidget
WBP_CommandBar     -> 4 /Script/StratUI.StratCommandBarWidget
```

The conclusion the control supports is unaffected and independently confirmed:
`WBP_PreMatchBriefing.uasset` carries **0** `/Script/Strat*` tokens of any kind and 4
`/Script/UMG.UserWidget`, so it does derive from plain `UserWidget`. Only the control's
stated figures are wrong.

This gates because the identical measurement is stated **correctly** in the sibling file
changed in the same pass. `StratGuidanceWidget.h:30-33` says "4 times, the SAME figure as",
which is true of each asset's own token. One pass, one author, two files, the same axis, one
right and one wrong: the intra-commit drift the transcription clause was written about, and
the same "dead instrument reporting green" failure this pass otherwise caught well.

**Owner:** `strat-gameplay-engineer`. The fix is to restate the control in
`StratGuidedOpening.h` as the per-asset own-parent-token figure, matching the sibling wording.

## Observations

Non-gating.

- **Judgement 1, the instrument death, verified.** Comment-only confirmed on all three
  headers with a literal-aware stripper refusing to emit zero lines: 37/37, 50/50, 16/16 code
  lines identical to HEAD. Mutation control: changing `-60.0f` to `-61.0f` was detected, so
  the comparator speaks. `float ArmPitch = -60.0f;` unmoved at `:192`. The pass's plant
  discipline and byte-identical restoration are sound and its final denominator is defensible.
- **Judgement 2, the restraint, endorsed.** "Satisfied, not replaced" is the right call, and
  the absence-decides reasoning is applied correctly: `ArmPitch` = 0 in
  `BP_StratCamera.uasset` is the decisive absence, while the three arm lengths at 1 each and a
  fabricated name at 0 are used only as instrument controls and not as override claims.
  Provenance re-derived independently: `bc4ff79` (2026-08-12 16:31) is an ancestor of
  `d310aa1` (23:00), the reverse fails, `rev-list` is 2 against 0, and `--reverse -S` returns
  `bc4ff79` alone. Mesh census reproduced exactly - `SM_HexTile` 16 (eight names x 2),
  `SM_HexOverlay` 2, `SM_Hex` 18, `SM_GuidedMarker` 0, `HexMesh` 0, fabricated 0. The meshes
  are real bytes, not LFS pointers.
- **Judgement 3, the over-stamping guard, confirmed working.** Narrowing to "there is no C++
  surface" rather than "the premise is dead" is correct, and the narrower claim is itself
  true: 0 `/Script/Strat*` tokens on the briefing asset, and 0 referrers among 1940 `Content/`
  packages against 3 for `WBP_Options` as a speaking control. The two `Source/` hits for
  `PreMatchBriefing` are both this pass's own new comment lines, so "nothing in `Source/`
  names it" was true when written and is now self-falsifying by a hair - worth a word if that
  block is ever revised, not a finding.
- **Judgement 4, the residual limit, real and correctly scoped.** Confirmed by construction:
  a sweep keyed on temporal or existential hedges cannot see a flat present-tense premise.
  Measured on this tree, of 647 windows carrying both a reason marker and a flat assertion,
  293 also carry a hedge and **354 do not** - the uncovered set is larger than the covered one.
- **Handoff confirmed honest.** `Content/UI/WBP_PreMatchBriefing.uasset` is tracked, 50,480
  bytes, real content rather than an LFS pointer, carries `Callout` 18 times, and is
  referenced by 0 of 1940 packages. An orphan in the content lane's ownership, not a defect
  in this pass.
- **Line endings: per-file preservation was the right call.** Re-derived byte-exactly rather
  than by `grep`. After state: `StratCameraPawn.h` 0 CRLF / 193 bare LF; `StratGuidedOpening.h`
  555 / 0; `StratGuidanceWidget.h` 169 / 0 - each file's own convention intact, matching the
  pass's claim. Normalising would have rewritten all 193 lines of `StratCameraPawn.h` and
  buried a comment-only change inside a whole-file diff. The `git show` caution is confirmed
  and now derived rather than asserted: the blob reads CRLF = 0 for **all three** files, so
  the blob column cannot establish a before-state at all.
- **A caution about `grep` on this box, met head-on.** My first line-ending census used a
  `grep -c` carriage-return pattern and reported `StratCameraPawn.h` as uniform CRLF,
  contradicting both the pass and git's own warning. An instrument control settled it: that
  grep returns **2 on a known pure-LF two-line file**. The grep was lying; the byte-exact
  count is the one reported above. My first summary line on this point was a pre-written
  literal that did not match its own output, and was discarded rather than reported.
- **Suite branch verified.** Zero `.cpp` in the diff. The gate's own
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` pattern returns 0 on both HEAD and worktree of all three
  headers, with a control of 7 on `Source/StratUI/Tests/StratScoreboardHUDSeam.cpp` proving
  the counter speaks. No suite re-run is owed.
- **`strat_banner_sweep.py`** printed `SWEEP CLEAN`, exit 0.
- **`engine.md` attribution is correct**: `2026-09-06, strat-gameplay-engineer (ACTING and
  WRITING; IN LANE, ... base commit 92d78c1, UNCOMMITTED)`, and it explicitly defers the live
  suite figure and the phase verdict to `global.md` rather than restating either.

## The fifteenth sweep - method, denominator, plant, result

**Form swept:** shape 6 whose expired premise is a FLAT PRESENT-TENSE ASSERTION rather than a
temporal or existential hedge - the residual the pass named in judgement 4, and uncovered by
every sweep run so far, including my own prior ones.

**Method.** Comment windows (maximal runs of `//`, `*`, `/*` lines) across all `.h` and `.cpp`
under `Source/`. A window is a candidate when it carries a reason marker
(`because|since|so that|for that reason|which is why|that is why`) AND a flat assertion
(`is/are the only|sole|single`, `the only caller|consumer|user|reader|writer|site|place`,
`no other`, `nothing else`, `never`, `always`, `has no caller`, `is unused`). Windows also
carrying a hedge were separated out as already visible to prior sweeps.

**Denominator.** 267 files scanned, 647 candidate windows, 293 also hedged, **354 flat-only** -
the new ground.

**Plant.** A synthetic instance - *"recomputed on every rebuild because StratScoreboardHUD is
the only caller of this builder, so a cached copy could never go stale"* - was appended to a
**disposable copy** of `Source/` in the scratchpad, never to the tree. It returned at
`StratViewModel.h:2344`, by its own text. Removing it dropped the flat-only count 355 -> 354
and the plant's own string to 0 matches. The copy was then byte-identical to the real file by
sha256 (`be8923fe2d8b7dc0`), and `git status --porcelain` still showed exactly the four
modified files. The instrument is alive and the tree was never written to.

*The instrument nearly died silently twice.* Two earlier drafts of the sweep script were
mangled by shell heredoc escaping and raised `SyntaxError`; both runs reported **plant
returned 0**, which is indistinguishable from a clean sweep. Only running the plant before
reading candidates caught it - the same failure the checklist records about its own census
draft.

**Triage.** Of the 354, the mechanically checkable exclusivity claims - those asserting a
unique caller, consumer, reader, writer, site or place - number **11**, and all 11 were read
in full context. Two looked like counterexamples and both survive:

- `StratScoreboardHUDSeam.cpp:153`, "`GetBridge()` is the ONLY reader of either ownership
  member". `StratScoreboardHUD.cpp:530` reads `OwnedBridge.Get()` directly, but that is the
  allocation site reading back what it created on the line above, and the claim's own next
  sentence scopes it to the consumer paths the assertion covers - "refresh, liveness,
  teardown". Loose, not expired.
- `StratMatchSubsystem.h:2701`, "`ApplyView` is the only writer" of `SoundMark`. Three writers
  exist, but two are the resets the very same block names in its next paragraph, "RESET
  WHEREVER `ReceiptMark` IS RESET". Internally coherent.

**Result: no fifteenth found - and this is a SCOPED null, not a clean one.** The 11 checkable
claims were read. The remaining 343 flat-only windows, mostly normative `never` / `always`
statements rather than factual premises, were **not** individually read. Recorded as a partial
sweep, because an unrun check must never appear as a clean one.

VERDICT: BLOCK
