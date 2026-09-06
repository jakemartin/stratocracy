# Gate: the in-match input-mode discharge (RE-GATE 2, after the test-author's repair) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`c5d2c65`. This is the THIRD gate on this tree. Both prior reports were read in full first:
`Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md` (BLOCK) and
`...-regate.md` (BLOCK). `.agents/ue-project-context.md` was read and checked against the tree;
it needed no correction this pass.

`git status --porcelain` was run rather than taken from the brief, and prints **six** modified
files, nothing staged, plus the two prior gate reports as untracked:

```
 M Source/StratPlay/StratOptionsPresenter.h
 M Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp
 M Tools/architect/state/content.md
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md
```

`git diff --stat`: 212 insertions, 7 deletions across those six. `HEAD` is still `c5d2c65`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing, and the same command over `Source/StratPlay` printed two files, so the instrument could speak. Manifests derived, not quoted: `rulesCommit 96d93ea9...`, `dataCommit c2f58608...` |
| 2 | No `strat::` call outside `StratBridge/`/`StratRules/` | pass — the lowercase-initial call grep across `Source/`, minus those two directories, returned zero. Control: the same pattern inside `Source/StratBridge/` returns 36 hits in `StratBridge.cpp` alone. The broader `strat::` token grep outside those dirs returns only comments; no type-name hit needed judging as a call |
| 3 | No vendored header in front of UHT | pass — eight `.h` files include a vendored header; each was counted for line-start `UCLASS`/`USTRUCT`/`UENUM` and all eight returned 0. Control: the same count on `StratOptionsPresenter.h` returns 1 |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only `TUniquePtr<FStratBridge>` in `Source/` is prose at `StratUI/StratViewModel.h:47` recording the `C4150`. Control: `TPimplPtr<FStratBridge>` returns the two real members plus their prose |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted; 16 rows, 10 field names, guard read. See below |
| 6 | New modules registered | pass — no new `Source/<Module>/`; 4 `IMPLEMENT_MODULE` sites, 4 names in the `Modules` array, `StratRules` correctly absent from both; no `.Build.cs` in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — one non-`Tests/` hit, `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing commandlet exception. Instrument proven live: 248 hits across 77 files in total. Run with `MSYS_NO_PATHCONV=1`, per the blindness recorded in the prior report |
| 9 | No widget-side arithmetic | pass — `git diff --stat -- Source/StratUI` is empty; no `StratUI` file is in the diff |
| 10 | Lane discipline | pass — engineer in `Source/` plus `engine.md`; test-author in `Source/StratPlay/Tests/` plus `tests.md`; coordinator in `content.md` (fallback, paid for at the last gate) plus `global.md`. No test code outside `Tests/`, no production code inside it, no asset touched |
| 11 | Prose blocks present | **BLOCK** — not on the diff's own new prose, which is sound and was verified on the merits. On two present-tense claims the current tree carries that are false against it. See Findings 1 and 2 |
| 12 | Nothing staged or committed | pass — six ` M` entries, no index column set, `HEAD` still `c5d2c65` |

### Check 5, derived rather than restated

The extraction over `Source/*/*.Build.cs` printed **16 rows**, all five modules present, matching
the carried block row for row, including the sixteenth,
`StratBridge  PrivateDependencyModuleNames  StratRules`. The field census printed **10 names** —
the documented set including the `BRIDGE=` string-literal false positive, which was not chased and
not sed-ed away. There is no eleventh.

The sixteenth row was read for its guard rather than treated as an unconditional arrow.
`Source/StratBridge/StratBridge.Build.cs:133-137`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
	PrivateDependencyModuleNames.Add("StratRules");
}
```

Both halves come from the one expression; the link edge is monolithic-only and correct;
`Source/StratBridge/Vendored/` still holds 12 `*.strat.cpp`. Nothing in check 5 moved this pass.

### Comment-only across BOTH `Source/` files — re-derived, and by a stronger instrument than the diff filter

The brief asked for this to be re-derived across both files rather than accepted. Two independent
derivations agree:

1. **Diff-side.** Every added line and every removed line in `git diff -- Source/`, with `//`
   lines filtered out, is **empty on both sides**. `--numstat` is `18 0` for the header and
   `8 2` for the clause file.
2. **Whole-file, which the diff filter cannot fake.** Comparing HEAD's copy of each file to the
   worktree copy with all `//` lines stripped from both sides returns **no difference** for either
   file. That is a stronger claim than "the added lines were comments": it also rules out a line of
   code having been commented out, or a comment having been uncommented.

`IMPLEMENT_SIMPLE_AUTOMATION_TEST` count in the clause file is **3 at HEAD and 3 in the
worktree**. No assertion, no clause name, no fixture, no `#include` moved. The repair did not
overreach.

### The repair itself, judged on the merits

`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp:50-58` now keeps the main clause intact —
a human at the keyboard is the only instrument for legibility and operability — and replaces the
one stale word with the discharge's actual status, records it as **a report and not a measurement**,
states it is reversible, and refuses in terms to let it settle the mouse-axis rejection. That is
exactly the shape the last finding asked for, and it goes one better than a stamp by restating the
CONDITION ("named a human report as its own discharge condition") rather than the status, which is
the lesson the lane's own record draws from it.

### The `Tick` detail the brief flagged — checked, and the test-author is right

Measured in `Source/StratPlay/StratPlayerController.cpp`: the comment block runs `586`-`644`,
the mouse-axis rejection paragraph opens at `:616` (`WHY POLLING AND NOT AN INPUT MODE`), and
`void AStratPlayerController::Tick(float DeltaSeconds)` opens at `:646`. The block therefore
PRECEDES `Tick` and is `Tick`'s own documentation block. A backward scan for an enclosing symbol
from inside it lands on `void AStratPlayerController::OnToggleProductionMenu()` at `:574` — the
wrong function, exactly as reported. The test-author resolved it by scanning FORWARD, which is the
correct instrument here and is recorded as such in that lane's entry.

### The near-identical sentence in `tests.md` — confirmed fine

`tests.md:153-154` reads *"Whether the screen is legible or operable remains a human at the
keyboard, as `StratOptionsPresenter.h` already records for the in-match input mode."* It says the
header RECORDS the point and does not say the point is OWED. A header that records a discharge
still records the point, so the sentence survives the event that falsified the `.cpp`. Leaving it
alone was correct, and the reason given for leaving it alone is the correct reason.

### The two questions kept apart — re-verified

`StratOptionsPresenter.h:88-105` states the discharge rests on ONE unmeasured human report, that
no clause in this tree can observe an input mode, that it is reversible, and that it settles
nothing about the mouse-axis rejection. `global.md:243-255`, `engine.md:38-42` and the new
`tests.md` entry all keep the same separation. No file folds the two together.
`AStratPlayerController` still calls `SetInputMode` nowhere; the only call in `Source/` is
`StratShellHUD.cpp:243`.

### Only `global.md` carries a live figure

`strat_banner_sweep.py --explain` finds **one** LIVE suite claim in the whole record, at
`global.md:21` (`435/435`). Every other three-digit claim, including `data.md`'s `108/108` pair, is
classified stamped. `global.md:33` now reads
"THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-02.19.18`." and matches the exported
report.

### Attribution and base commit, per lane

- `engine.md:18-19` — `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, ... base commit
  `c5d2c65`, UNCOMMITTED), and it says in terms that no exception clause applies and none is cited.
- `tests.md:17-19` — `strat-test-author` (ACTING and WRITING; IN LANE ... base commit `c5d2c65`
  ... UNCOMMITTED).
- `content.md:174-179` — the `coordinator` named inline, under this file's fallback condition,
  with its control; discharged at the last gate and unchanged.
- `global.md` — the coordinator's own file; no clause needed.

All four attribute inline, and all three lane entries name `c5d2c65`.

## Instruments

**The exported report**, `Saved/AutomationReport/index.json` read with `utf-8-sig`:
`reportCreatedOn 2026.09.06-02.19.18`, `succeeded 435`, `failed 0`, `notRun 0`,
`succeededWithWarnings 0`, `totalDuration 5.039`. **435 entries, counted, every one in state
`Success`** — counted from the entry list, not taken from the summary fields.

**The banner sweep, with its exit code measured here rather than taken from any brief.** Run with
a redirect so that the status read is the script's own and not a pipeline's:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 435 entries, all Success (2026.09.06-02.19.18)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0
  report identity: mtime=1788661159, reportCreatedOn='2026.09.06-02.19.18'
  suite claims found: 63 (1 live, 62 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

Exit code on that run: **0**. An empty finding list is worthless without a control that the script
can fail, so one was run: a **disposable copy** of `global.md` with its single live `435/435`
changed to `434/434`, swept by passing that copy as the script's own `state_path` argument. It
printed two `SUITE COUNT AGREEMENT` findings and `SWEEP FAILED -- the record contradicts itself or
the tree`, and exited **1**. The real file swept through the identical invocation shape printed
`SWEEP CLEAN` and exited **0**. No file in the repository was modified to obtain this.

**So the standing claim that the sweep exits 0 on `SWEEP FAILED` is false, measured here
independently of the brief and of the test-author's own measurement:** `SWEEP FAILED` gives 1,
`SWEEP CLEAN` gives 0. The lane's record of the same result is accurate.

No build and no suite were run by this gate. `Result: Succeeded` in 12.07 s is
`strat-test-author`'s report and is recorded as theirs.

## Findings

**1. `Source/` carries an unqualified present-tense claim that the options WBP does not exist, and
it has been false since the commit that wrote it. Owner: `strat-gameplay-engineer`.**

`Source/StratUI/StratOptionsWidget.h:56-57`, closing the `THE OWNER LANDED 2026-09-05` stamp:

> `UStratOptionsPresenter`, a `UWorldSubsystem` in `StratPlay`, is the owner
> instead, and it exists in both worlds; its file header carries the full derivation. **What is
> still owed is the WBP.**

Every element of that debt is discharged in the tree this gate would be passing. The debt is stated
eleven lines above, at `:46-48`: *"DISCHARGED BY the WBP and an owner ... that creates this widget
on `IsOptionsPanelOpen` and binds the delegate."* Measured:

- `Content/UI/WBP_Options.uasset` is present and tracked. `git log --diff-filter=A` names
  **`d80b28a`** as the commit that added it.
- It derives from `UStratOptionsWidget`, and that is not taken on trust — it is pinned by a green
  clause: `Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp:234-236` asserts
  *"the agreed class derives from UStratOptionsWidget"* via
  `Shell.Panel->IsChildOf(UStratOptionsWidget::StaticClass())`, inside the 435/435 run above.
- `OptionsWidgetClass` is set on all three shipped GameModes and `UStratOptionsPresenter` is the
  owner that creates it — the record's own account at `global.md:179-185`.

**The sentence was false when it was written.** `git log --reverse -S "still owed is the WBP"` over
that file returns **`374398a`** as its origin — one commit AFTER `d80b28a` added the asset.
`--reverse` is used deliberately: the plain form answers most-recent, not origin. Nothing later in
the file corrects it; the only subsequent stamp is `AMENDED 2026-09-05` at `:77`, which discusses
how the asset must be authored and never says it landed. A reader arriving at `:57` by citation is
told the volume screen has no asset, three lines after being told — correctly, and in the past
tense — that *"as of this file's first commit ... a player who reaches `EStratShellRoute::Options`
today sees nothing."* The neighbouring sentence at `:43-45` is time-qualified and survives; this
one is not and does not.

**I am gating on it although this diff did not create it, and the reason is stated so it can be
argued with.** A PASS from this seat describes a tree, not a phase, and this tree contains a false
unqualified claim in `Source/` about the exact surface this phase is closing. The brief for this
pass commissioned a whole-tree sweep for surviving false sentences about the options screen and
said to treat a third as likely; this is the third, in a third file, found by the sweep it asked
for. What can fairly be weighed the other way, and is recorded rather than hidden: the sentence is
not in the diff, it predates `c5d2c65`, and the two prior gates passed over it because they swept
`outstanding` and `unseen` rather than `still owed`. The fix is one stamp in the house style — the
asset landed, name the commit, and the paragraph reads as history instead of as a debt.

**2. The new `tests.md` entry makes three present-tense claims about `global.md` and about the
sweep that are false against the tree it sits in. Owner: `strat-test-author`.**

`Tools/architect/state/tests.md:72-77`:

> **WHAT THIS PASS DID NOT TOUCH.** `Source/StratPlay/StratOptionsPresenter.h`, `content.md`,
> `engine.md` and `global.md` are other lanes' and were already gated; **they are byte-identical.**
> **`global.md:33`'s citation of `reportCreatedOn 2026.09.06-01.54.08` is staled BY this pass's
> re-run** and is not this lane's to fix; **the sweep's single remaining `REPORT PROVENANCE`
> finding names it and nothing else.**

All three emphasised clauses are false right now, measured above:

- `global.md` is **not** byte-identical to what the prior gate read. Its line 33 was changed after
  this entry was written; `git diff` against HEAD shows `-...00.22.38` / `+...02.19.18`, and the
  prior gate report records the intermediate `01.54.08` that this entry quotes.
- `global.md:33` does **not** cite `2026.09.06-01.54.08`. Line 33 reads
  "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-02.19.18`."
- The sweep has **no** remaining `REPORT PROVENANCE` finding, or any finding at all. It prints
  `SWEEP CLEAN` and exits 0, with the control above proving it can do otherwise.

This is the same defect as the previous gate's finding and as the one this pass repaired, one file
over and one lane over: a record entry describing another lane's file in the present tense,
falsified by that lane before the tree was gated. It is a lane's own record file, so it is
`strat-test-author`'s to fix and not the coordinator's to patch in place — and the fix is to
describe the CONDITION rather than the status, which is the lesson this very entry draws a few
bullets earlier. Time-qualifying it — "as of this pass's re-run, before the coordinator's citation
update" — is enough; nothing else in the entry is affected, and the rest of it is good work.

## Observations

Non-gating.

- The `.cpp` comment says the mouse-axis rejection is *"argued on `AStratPlayerController::Tick`"*;
  the `tests.md` entry describes the same text as *"argued above"* it, and `global.md:249` as
  *"recorded on"* it. *Above* is the literally accurate one — the block ends at `:644` and `Tick`
  opens at `:646`. All three resolve to the right block and none is wrong enough to gate; recorded
  only so the next reader chasing the citation is not surprised to land two lines short of a
  function body.
- `content.md:172` — *"The unseen half above is unchanged and still owed"* — remains a
  present-tense sentence that is now false, corrected eight lines below by the `:180` stamp which
  names the same phrase. This is the file's stamp-in-place convention working as designed and I did
  not gate on it at the previous gate either; the earlier stamp still does not point forward.
- Every other surviving `unseen` / `not been seen` / `nobody has seen` hit across `Source/` and
  `Tools/architect/state/` was read this pass and none is about the options screen or the in-match
  panel: `engine.md:4613` (camera path), `global.md:3961` (the strip, already corrected),
  `:4116` (the directive-strip z-order layout), `:5544`, `:7045`, `:7089`, `:7098`, `:7207`
  (rings, markers, window-end tag), `content.md:2044`-`2534` and `decisions.md:643`/`:663`
  (rows, rings, markers), `StratMatchSubsystem.h:2183`, `StratUnitActor.h:396` and
  `StratInfoPanelSourceParity.cpp:266` (different subjects entirely). Every `outstanding` hit in
  `Source/` other than the repaired one is about guided-opening beats.
- The `tests.md` entry quotes the withdrawn *"outstanding"* wording inside an italic quotation and
  corrects it in the following sentence. That is the right way to retire a sentence and is not a
  surviving claim; it was checked rather than assumed.
- `engine.md:66-68` states zero failed, zero notRun, zero succeededWithWarnings and defers the
  figure to `global.md`. Within precedent, and the sweep agrees.
- All six files in the diff still emit `warning: ... LF will be replaced by CRLF ...` on every
  `git diff`. Known behaviour of this tree under `core.autocrlf=true`; no bytes at risk.
- `.agents/ue-project-context.md` was checked against the tree this pass and needed no correction.
  Its `StratBridge` row reasons both link-type branches correctly.

VERDICT: BLOCK
