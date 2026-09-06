# Gate: the in-match input-mode discharge (RE-GATE 4) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`c5d2c65`. This is the FIFTH gate on this tree. `...-regate-3.md` was read in full first, as the
brief directed, and the two findings it raised were re-derived here rather than accepted as
answered. `.agents/ue-project-context.md` was checked against the tree and needed no correction.

`git status --porcelain` was run rather than taken from the brief. **Seven** modified files,
nothing staged, plus the FOUR prior gate reports untracked:

```
 M Source/StratPlay/StratOptionsPresenter.h
 M Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp
 M Source/StratUI/StratOptionsWidget.h
 M Tools/architect/state/content.md
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate-2.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate-3.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md
```

`git diff --stat`: **402 insertions, 11 deletions** across those seven. `HEAD` is still `c5d2c65`.
No build and no suite were run by this gate.

**One instrument failed inside this gate and is recorded rather than quietly re-run.** The first
attempt at the comment-only derivation wrote its stripped copies to a scratch path built from a
relative parent of the home directory, which `mkdir` refused. Every `git show` redirect and every
`diff` then failed on a missing file -- and the wrapper printed **DIFFERS for all three source
files**, a verdict produced by an instrument that had read nothing. Re-run against the correct
absolute scratch path it prints IDENTICAL for all three, on inputs of 164, 412 and 318 non-comment
lines each, so the files were non-empty and the comparison was real. A blind instrument here
happened to fail toward a finding rather than toward a pass, which is luck and not design.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing. Control: the same command over `Source/StratUI` printed one file and one changed line, so the instrument can speak. Manifests derived, not quoted: `rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`, `dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5` |
| 2 | No `strat::` call outside `StratBridge/`/`StratRules/` | pass -- the lowercase-initial call grep over `Source/` minus those two directories returned **zero**. Control: the identical pattern inside `Source/StratBridge/` returns **143**, so the pattern matches real calls and the zero is a measurement. No type name was reported as a call |
| 3 | No vendored header in front of UHT | pass -- eight `.h` files include a vendored header (`StratBridge.h` plus seven under `Source/StratRules/`); each counted for line-start `UCLASS`/`USTRUCT`/`UENUM` and all eight returned **0**. Control: the same count on `Source/StratUI/StratOptionsWidget.h` returns **2** |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass -- the only `TUniquePtr<FStratBridge>` in `Source/` is prose at `StratUI/StratViewModel.h:47` recording the `C4150`. Control: `TPimplPtr<FStratBridge>` returns the two real members, `StratPlay/StratMatchSubsystem.h:2601` and `StratUI/StratScoreboardHUD.h:1235`, plus their prose |
| 5 | Module arrows unchanged and correct | pass -- derived, not trusted: **16 rows**, all five modules present, **10 field names**, guard read in place. Detail below |
| 6 | New modules registered | pass -- five `Source/*/` directories; four carry a module object (`Stratocracy` via `IMPLEMENT_PRIMARY_GAME_MODULE`, the other three via `IMPLEMENT_MODULE`) and all four are named in `Stratocracy.uproject`'s `Modules`. `StratRules` carries neither and is correctly absent. No `.Build.cs` in the diff |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing; control as in check 1 |
| 8 | No `/Game/` path literal in C++ | pass -- run with `MSYS_NO_PATHCONV=1`. One non-`Tests/` executable hit, `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing commandlet exception. Instrument proven live: **325** `/Game/` hits across `Source/` in total |
| 9 | No widget-side arithmetic | pass -- `Source/StratUI/StratOptionsWidget.h` is the only `StratUI` file in the diff and its non-comment bytes are **identical** to HEAD's, with a control. No arithmetic could have been added |
| 10 | Lane discipline | pass -- engineer in `Source/StratUI/` + `Source/StratPlay/` (non-`Tests/`) + `engine.md`; test-author in `Source/StratPlay/Tests/` + `tests.md`; coordinator in `content.md` (fallback) + `global.md`. No test code outside `Tests/`, no production code inside it, no asset touched |
| 11 | Prose blocks present | pass -- house-style throughout, and the two claims the previous gate blocked on are gone. Every checkable command claim in the new prose was re-run here; all match. Detail below |
| 12 | Nothing staged or committed | pass -- the seven ` M` entries read through `cat -A`, no index column set on any; `HEAD` still `c5d2c65` |

### Check 5, derived rather than restated

The extraction over `Source/*/*.Build.cs` printed **16 rows**, all five modules present, matching
the carried block row for row -- including the sixteenth,
`StratBridge  PrivateDependencyModuleNames  StratRules`. The field census printed **10 names**:
the three graph fields in both `.Add` and `.AddRange` forms, `PCHUsage`, `bUseUnity`,
`ShadowVariableWarningLevel`, `PrivateDefinitions.Add`, `bCompileVendoredRulesHere =`, and the
documented `BRIDGE=` string-literal false positive, which was neither chased nor `sed`-ed away.
There is no eleventh.

The sixteenth row was read for its guard rather than treated as an unconditional arrow.
`Source/StratBridge/StratBridge.Build.cs:133-138`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
	PrivateDependencyModuleNames.Add("StratRules");
}
```

Both halves come from the one expression; the link edge is monolithic-only and correct.
`Source/StratBridge/Vendored/` still holds **12** `*.strat.cpp`, so the modular branch still
compiles them in. `StratUI` has gained no `Stratocracy` dependency and no cycle exists. Nothing in
check 5 moved this pass.

### Comment-only across all THREE `Source/` files -- re-derived, with a control

For each of `Source/StratPlay/StratOptionsPresenter.h`,
`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` and
`Source/StratUI/StratOptionsWidget.h`: HEAD's copy and the worktree copy, both CR-normalised and
with every comment-only line stripped, compare **IDENTICAL** -- at 164, 412 and 318 non-comment
lines respectively, printed so that a silent empty read cannot pass as agreement. That is stronger
than "the added lines were comments": it also rules out a line of code having been commented out,
or a comment uncommented.

**Control:** renaming `UStratOptionsWidget` in a **disposable copy** of the stripped
`StratOptionsWidget.h` makes the same `diff` report a difference. No file in the repository was
modified to obtain it; `git status --porcelain` was re-read afterwards and is unchanged.

### The exported report, read here

`Saved/AutomationReport/index.json`, `utf-8-sig`: `reportCreatedOn 2026.09.06-02.19.18`,
`succeeded 435`, `failed 0`, `notRun 0`, `succeededWithWarnings 0`,
`totalDuration 5.038963794708252`. **435 entries counted from the entry list, every one in state
`Success`.**

### The "no suite re-run was owed" claim -- verified, not accepted

The newest `.cpp` under `Source/` carrying an `IMPLEMENT_*_AUTOMATION_TEST` macro is
`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` at **22:17:19**. The report's own mtime
is **22:19:18** -- newer, which is exactly what the sweep's REPORT IDENTITY check compares. The
files touched after that run are `global.md` (22:22:11), `tests.md` (22:39:07), `content.md`
(22:50:42), `StratOptionsWidget.h` (22:52:31) and `engine.md` (22:52:55) -- one `.h` and four
`.md`, none of which can enter `newest_test_mtime`, because the extension test drops a `.h` before
the macro test ever runs. `StratOptionsPresenter.h` is older still (21:51:56). The claim holds as
measured.

### The banner sweep, with its exit code measured and a control

Run with a redirect so the status read is the script's own and not a pipeline's:

```
  automation report: 435 entries, all Success (2026.09.06-02.19.18)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0
  report identity: mtime=1788661159, reportCreatedOn='2026.09.06-02.19.18'
  suite claims found: 63 (1 live, 62 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

Exit code **0**. **Control:** the six record files were copied to a scratch directory, the single
live `435/435` in the COPY of `global.md` changed to `434/434`, and the script run against those
copies by passing them as its `state_path` arguments. It printed two `SUITE COUNT AGREEMENT`
findings and `SWEEP FAILED -- the record contradicts itself or the tree`, and exited **1**.
`git status --porcelain -- Tools/architect/state/` was re-read afterwards and still shows the same
four modified entries; no repository file was modified to obtain the control.

`--explain` finds **one** LIVE suite claim in the whole record, `global.md:21` (`435/435`); the
other three-digit claims it names (`global.md:1169`, `:1492`, `:6516`) are classified stamped.
Check on the "only `global.md` may carry a live suite count or a phase verdict" rule: **pass**.

### Attribution and base commit, per lane

- `engine.md`'s two new entries -- both open
  `strat-gameplay-engineer` (ACTING and WRITING; IN LANE ..., base commit `c5d2c65`, UNCOMMITTED),
  inline, each saying in terms that no exception clause applies and citing none.
- `tests.md`'s new entry -- `strat-test-author` (ACTING and WRITING; IN LANE ..., base commit
  `c5d2c65`, ... UNCOMMITTED), inline.
- `content.md`'s two new stamps -- the `coordinator` named inline, under this file's fallback
  condition, with the control re-measured at the time of writing in both.
- `global.md` -- the coordinator's own file; no clause needed, and the stamp added there names none.

All lane entries attribute inline and name `c5d2c65`. Neither `content.md` stamp names a base
commit; the fallback clause does not require one, and this is carried forward as an Observation
rather than a finding, as at the previous gate.

## The two regate-3 findings, judged on their answers

### Finding 1 -- the `content.md` `## NEXT` entry. ANSWERED, and the remedy chosen is the right one.

**The brief asked me to judge whether stamping is the right remedy at all, or whether the entry
should have been moved out of `## NEXT`. Stamping is right, and moving would have been wrong, for
a reason that is measurable rather than stylistic: `content.md` has exactly ONE `##` heading in
the whole file -- `## NEXT`, at line 51.** There is nowhere to move an entry TO. Moving it out
would mean either deleting it or inventing a section the file does not have, and that file's own
header forecloses the first in terms:

> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

So stamping is not merely permitted here, it is the header's prescribed remedy, and the header
names its own precedent. The choice is also consistent with `StratOptionsWidget.h`, which was
stamped in this same pass for the same class of defect -- and consistency across the two matters,
because a reader who learns the convention in one file should not have to relearn it in the other.
There is a second reason the "move it" reading does not survive contact: the file is chronological,
so every entry below the topmost one is history. If "under `## NEXT`" meant "must be true of the
tree", the entire record would be a standing finding. The header's stamping rule is precisely what
makes a live-swept section able to carry history at all.

**Three sites stamped, all three verified against the tree, and none of them over-stamped.** The
brief specifically asked whether a true sentence was stamped as false. It was not, and the
distinction the brief says it preserved is genuinely preserved:

- **The headline stamp** says *"THIS ENTIRE ENTRY IS HISTORY AND ITS HEADLINE IS FALSE OF THE
  TREE."* Note the wording: false **of the tree**, not false when written. That is correct and is
  the weaker of the two available claims -- the entry is based on `089c79c`, and the headline was
  true then. It does not repeat `StratOptionsWidget.h`'s "false when committed" finding where that
  finding does not apply.
- Its evidence re-derived here, with the control run rather than quoted: `grep -a -c` on
  `Content/UI/WBP_Options.uasset` returns **1** each for `MasterSlider`, `MusicSlider`,
  `SfxSlider` and `BackButton`, against **0** for all four on the negative control
  `Content/UI/WBP_TitleMenu.uasset`. The instrument discriminates rather than matching everything.
  `Source/StratUI/StratOptionsWidget.h` declares those four `UPROPERTY(meta = (BindWidget))`,
  which is a Blueprint compile error if the named child is missing, and all three shipped GameModes
  load green in the 435/435 run.
- **The stamp on "THE OPTIONS SCREEN HAS NO SLIDERS AND CANNOT GET THEM ON THIS ROUTE".** This is
  the site the brief flagged, and it is stamped exactly as narrowly as it should be: *"the sliders
  were added later the same day and are in the shipped asset ... The ROUTE limit stated below was
  real and is why this pass could not add them; it is not a claim about what the asset holds now."*
  The compound sentence has a false half and a true half, and only the false half is stamped.
  **Not over-stamped.**
- **The stamp on "which no surface reaches".** Stamped `FALSE SINCE 2026-09-05`, a staleness claim
  rather than a false-when-written claim -- and that is the correct characterisation, which I
  checked rather than assumed. That sentence sits inside the listening-pass stamp, and this file
  records the sliders as built over base `c13eb81`, which is the commit the listening pass
  discharged. The sliders therefore landed AFTER it was written, so it was true when written.
  **Not over-stamped.**

The `CORRECTED BY THE STAMP DIRECTLY BELOW` repair on the earlier bullet also holds: the sentence
*"the in-match panel takes no input mode, which no clause can observe either"* is now followed
immediately by a stamp saying the panel still takes no mode and that this is a recorded decision
rather than an open question, which is true of the tree -- `AStratPlayerController` calls
`SetInputMode` nowhere, verified below.

### Finding 2 -- `engine.md`'s "the rule genuinely bites". ANSWERED, and the correction is measured rather than asserted.

Both sites were re-run here, not read:

```
$ git log --oneline -S "still owed is the WBP" -- Source/StratUI/StratOptionsWidget.h
374398a ...
$ git log --oneline --reverse -S "still owed is the WBP" -- Source/StratUI/StratOptionsWidget.h
374398a ...
```

Identical single line both ways. The false claim is gone from both sites and the replacement says
so in terms: `engine.md` -- *"`--reverse` is inert on this query and is not load-bearing for this
claim"* -- and `StratOptionsWidget.h` -- *"`--reverse` IS KEPT BECAUSE IT IS THE RIGHT HABIT FOR AN
ORIGIN QUESTION, NOT BECAUSE IT BIT HERE."* The weaker implicature the previous gate named at the
second site is gone with it.

**The control was re-measured here, and it does discriminate**, which is what licenses keeping the
rule rather than deleting it. On the broad token scoped to `Source/StratPlay/`, over three commits:

```
$ git log --oneline -S "SetInputMode" -- Source/StratPlay/           -> 374398a, ee4acf5, c8c81cb
$ git log --oneline --reverse -S "SetInputMode" -- Source/StratPlay/ -> c8c81cb, ee4acf5, 374398a
```

Genuinely opposite first lines. The adjacent stamp resting on that is correct and was left
untouched, correctly.

**The `-S`-versus-`git grep` distinction, re-derived rather than taken.** The engineer's ground is
that `-S` is right for a COMMENT SENTENCE and wrong for "when did the CALL arrive", and that this
is the same property in both directions. That is sound, and the mechanism is worth stating because
it is easy to get backwards. `git log -S` is textual: it reports commits where the **number of
occurrences of a literal string** changed, and it is indifferent to whether an occurrence is code
or a comment. When the claim's subject IS the text -- "when did this sentence arrive" -- the
instrument's subject and the claim's subject are the same object, and `-S` answers exactly the
question asked. When the claim's subject is a semantic construct -- a CALL -- the string is only a
proxy, and it matches the comments that discuss the call as readily as the call itself; a commit
that merely moves comment text reads as an origin. That is precisely what happened at `c8c81cb`,
where `git grep -n "SetInputMode" c8c81cb -- Source/StratPlay/` returns two lines, both comments
and no call. The paired grep is the right instrument there because it reads FILE STATE at a commit
and lets each hit be classified. So the two conclusions come from one property, correctly applied
in opposite directions.

**One refinement, offered as an Observation and not a finding:** `-S` also cannot distinguish an
addition from a removal -- it fires on any count change in either direction -- so it establishes an
origin only when the string is present in `HEAD` and the result is a single commit. That is exactly
this case, and it is what the new text already rests on (*"returns `374398a` AND NOTHING ELSE ... a
one-line result has no first-versus-last left to get wrong"*), so the record's justification does
not lean on `--reverse` at all. It leans on the singleton, which is the durable ground.

**Is the lesson carried usefully or merely at length?** Usefully. The bullet does three separable
things a later reader can act on, rather than narrating: it names the transferable failure as
REACHING FOR A TRUE RULE THAT DOES NOT APPLY and identifies the repeat, not either instance, as the
evidence that the first fix did not reach the cause; it states the operational rule -- *"a sentence
naming a general rule as the reason is a claim that the rule DISCRIMINATES THERE ... settled only by
running both and diffing them"*; and it explicitly refuses the over-correction of deleting the rule,
backing the refusal with the re-measured three-commit case where the rule does bite. Length here is
carrying distinct load per paragraph. It is long prose, not padded prose.

## The untracked gate reports -- both halves of the coordinator's refutation, checked

**Confirmed, both halves, and the defect is genuinely not present in this tree.**

- **Every other gate report is tracked.** `Tools/architect/gate_reports/` holds **56** `.md` files
  on disk; `git ls-files` on that directory returns **52**. The difference is exactly the four
  `in-match-input-mode-discharge` reports, and `git ls-files --others --exclude-standard` on that
  directory names those four and nothing else. So their untracked status is a property of nothing
  having been committed yet, not of a convention being broken.
- **No added line in this pass cites any of them.** `git diff -U0 | grep '^+'` filtered for
  `in-match-input-mode-discharge` returns **0**, and filtered for any `gate_reports/` path at all
  returns **0**. Control, because a zero from a grep is worthless without one: the same added-line
  set contains **55** lines carrying a `.md`/`.cpp`/`.h` filename token, so the instrument can see
  a filename where one exists.

**One qualification, and it is why this sits in the report rather than being waved through.** What
is measured is the tree; *"they will be staged with the commit"* is a promise about a future commit
and no instrument here can settle it. The standing rule -- commit the evidence before the record
that cites it -- is not triggered, because nothing cites them. If a later pass adds a record line
citing one of these four, that line and the four files must enter the same commit. **This report is
a fifth untracked file in that directory and cites the other four in its Scope; it belongs in the
same commit as they do.**

## Findings

None.

## Observations

Non-gating.

- **Two sentences in the stamped `content.md` entry carry no bracket of their own.** *"THE THREE
  SLIDERS AND THE BACK CONTROL ARE OWED TO A HUMAN IN THE EDITOR"* and *"Until then the `Options`
  route opens a blank panel"* are covered by the entry-level headline stamp and sit six and eight
  lines BELOW the inline stamp that says the sliders are in the shipped asset, in the same
  contiguous paragraph. The header's requirement -- stamp an entry that has become history -- is
  discharged, three times over, so this is stamp density and not an unstamped entry. Recorded
  because the correction is above rather than below these two sentences, and a reader arriving by
  citation reads downward.
- **Two W6-era lines say a menu WBP is owed and are stale**, but their subject is `WBP_TitleMenu`,
  the title level and `BP_StratShellGameMode` -- not the options screen, not `WBP_Options`, not the
  in-match input mode. They read *"Owed: the title level; the menu WBP; `BP_StratShellGameMode` ..."*
  in `engine.md` and *"STILL OWED FOR W6: the title level, the menu WBP ..."* in `global.md`. Named
  rather than gated on, for the same scope reason the previous gate gave: this is not a licence to
  leave every superseded entry in this record unstamped, and it is not this phase's subject either.
- The `content.md` stamps added this pass discharge the fallback condition's two obligations -- the
  control is re-measured inline and the writer is named inline -- but neither names a base commit,
  while the three lane entries each name theirs. Nothing in that clause requires it; carried
  forward unchanged from the previous gate.
- `engine.md` names `strat-integration-reviewer` three times as the provenance of a corrected
  sentence. That is provenance for a specific claim, not a record narrating its own gate rounds,
  and it carries no verdict and no count; it is the shape this record already uses elsewhere.
  Not a finding, recorded so a later reader does not have to re-decide it.
- All seven files in the diff emit `warning: ... LF will be replaced by CRLF ...` on every `git`
  invocation. Known behaviour of this tree under `core.autocrlf=true`; no bytes at risk.
- `.agents/ue-project-context.md` was checked against the tree this pass and needed no correction.
  Its `StratBridge` row already states the monolithic-only link edge and reasons both branches from
  the one `Target.LinkType != TargetLinkType.Monolithic` expression, and its `Source/StratRules/**`
  row correctly points at the manifest as the authority rather than quoting a commit.

## The sweep the brief commissioned -- what was searched, and how

Stated as a measurement so the absence is not a silence. Four passes have each found one surviving
false sentence created by a different pass, so a fifth was assumed until looked for.

Three greps over `Source/`, `Tools/architect/state/`, `.agents/` and `CLAUDE.md`
(`--include=*.h --include=*.cpp --include=*.md`), deliberately loose so the filter would not kill
the defect first, with every hit read in place rather than counted:

1. `(no controls on it|has no sliders|no surface reaches|blank panel|nobody has seen|no human has
   seen|has not been seen|unseen|not been seen)` -- **32 hits.** Every hit about the options
   surface is one of the three sites now stamped, or the "blank panel" sentence covered by them.
   All the rest are about a different subject entirely -- the hover marker, the production rows, a
   ring on a unit, the camera path, a layout strip -- and each was read to confirm it.
2. `owed`, intersected with `wbp|option|slider|input mode|volume` -- **21 hits**, of which several
   are the substring inside *followed*, which is the instrument being loose on purpose. The
   substantive ones are the `StratOptionsWidget.h` sentence (stamped immediately below it), the
   `content.md` sentence in the Observation above, the `engine.md` bullets that quote the old
   sentence as history, and the two W6 lines noted above.
3. `input.mode`, intersected with `owed|outstanding|open |still|no input mode|not taken|missing` --
   **4 hits**: three in the `content.md` stamped block, correct as written, and one unrelated line
   in `StratInputClaimClauses.cpp` describing the title-screen mode that followed the player into
   the match, which is that clause file's own subject and is not a debt.

**Controls, because three greps returning "nothing relevant" is worthless without them:** the bare
token `owed` returns **436** hits across the same tree and `input.mode` returns **43**, so both
instruments were speaking and the narrowing is what removed the rest.

`Content/` was not re-swept. It is byte-identical to `c5d2c65` -- no `Content/` path appears in
`git status --porcelain` -- and the previous gate swept it with `grep -a` and a working control.

**No fifth surviving sentence was found.** The specific claims searched for -- that the options
screen is unseen, that it has no controls, that the in-match input mode is open, that the WBP is
owed -- each survive in the tree only inside a stamp that corrects them, or as an entry that has
been stamped as history.

## The claims in the new prose that a checkout can re-run, re-run here

Every one matched:

- `git merge-base --is-ancestor d80b28a 374398a` exits **0**; `git log --oneline d80b28a~1..374398a`
  prints `374398a`, `c13eb81`, `d80b28a` newest first.
- `git log --oneline --diff-filter=A -- Content/UI/WBP_Options.uasset` names **`d80b28a`**.
- `grep -a -c` on the three shipped GameMode assets returns **2** for `WBP_Options` and **1** for
  `OptionsWidgetClass` on each of `BP_StratShellGameMode`, `BP_StratGameMode` and
  `BP_StratGameMode_AiVsAi`; the negative control
  `Content/TopDown/Blueprints/BP_TopDownGameMode.uasset` returns **0** and **0**.
- `global.md`'s *"`AStratPlayerController` still calls `SetInputMode` nowhere"* -- confirmed. The
  two `SetInputMode` occurrences in `StratPlayerController.cpp` are both comment lines; the only
  call in `Source/StratPlay/` is `StratShellHUD.cpp:243`,
  `OwningPlayer->SetInputMode(InputMode);`.
- `tests.md`'s measured correction that the sweep exits **1** on `SWEEP FAILED` -- independently
  confirmed by this gate's own control run.

VERDICT: PASS
