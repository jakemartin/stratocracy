# Gate: the in-match input-mode discharge (RE-GATE after two repairs) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`c5d2c65`. This is a RE-GATE of the tree that received `VERDICT: BLOCK` in
`Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md`, which was read first.
The repairs were verified by re-running the instruments rather than accepted as done.

`git status --porcelain` was run, not taken from the brief, and prints four modified files, nothing
staged, plus the prior gate report as untracked:

```
 M Source/StratPlay/StratOptionsPresenter.h
 M Tools/architect/state/content.md
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md
```

`git diff --stat`: 143 insertions, 5 deletions. At the prior gate it was 102 and 5. The 41-line
delta is `engine.md` 66 -> 99 and `content.md` 5 -> 13, and nothing else; `HEAD` is still `c5d2c65`.
`.agents/ue-project-context.md` was checked against the tree and needed no correction this pass.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge/`/`StratRules/` | pass — the lowercase-initial call grep across `Source/`, minus those two directories, returned zero lines; no type-name hits needed judging |
| 3 | No vendored header in front of UHT | pass — the eight `.h` files including a vendored header were each counted for `UCLASS`/`USTRUCT`/`UENUM` at line start; all eight returned 0 |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only `TUniquePtr<FStratBridge>` in the tree is prose at `StratUI/StratViewModel.h:47` recording the `C4150` measurement |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted; 16 rows and 10 field names, see below |
| 6 | New modules registered | pass — 4 `IMPLEMENT_MODULE` sites, 4 names in the `Modules` array, `StratRules` correctly absent from both; no `.Build.cs` in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — **but the first run of this check was BLIND and is written up in Observations.** Re-run correctly, every hit outside `Tests/` is a comment arguing the rule except `ImportStratDataCommandlet.cpp:54`, the standing import-commandlet exception |
| 9 | No widget-side arithmetic | pass — `git diff --stat -- Source/StratUI` is empty; no `StratUI` file is in the diff |
| 10 | Lane discipline | pass — the engineer's half in lane; the coordinator's `content.md` write is now paid for, see Finding 2 discharged below |
| 11 | Prose blocks present | **BLOCK** — not on the diff's own prose, which is sound, but on a claim in `Source/` the diff falsifies and leaves standing. See the Finding |
| 12 | Nothing staged or committed | pass — four ` M` entries, no index column set, `HEAD` still `c5d2c65` |

### Check 5, derived rather than restated

The extraction printed **16 rows**, all five modules present, matching the carried block row for
row, including the sixteenth (`StratBridge  PrivateDependencyModuleNames  StratRules`). The field
census printed **10 names** — exactly the documented set including the `BRIDGE=` string-literal
false positive, which was not chased and not sed-ed away. There is no eleventh.

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

Both halves come from the one expression, the link edge is monolithic-only and correct, and
`Source/StratBridge/Vendored/` still holds 12 `*.strat.cpp`. Nothing in check 5 moved this pass.

### The two files that had to be byte-identical — verified, and by what instrument

`Source/StratPlay/StratOptionsPresenter.h` and `Tools/architect/state/global.md` were asserted
unchanged since the prior gate. Two independent lines of evidence agree:

1. **Arithmetic.** Their `--stat` figures are unmoved (18 insertions; 13 insertions and 5
   deletions), and the whole 41-line growth is accounted for by `engine.md` (+33) and `content.md`
   (+8). 33 + 8 = 41 exactly, leaving nothing unaccounted.
2. **Mtimes, against the prior gate report as the reference clock.** The prior report was persisted
   at `1788660139` (2026-09-05 22:02:19 local). `StratOptionsPresenter.h` is `1788659516`
   (21:51:56) and `global.md` is `1788659767` (21:56:07) — both BEFORE it. `content.md`
   (`1788660178`, 22:02:58) and `engine.md` (`1788660322`, 22:05:22) are after it, which is where
   the repairs are. Neither of the two files has been written at all since the prior gate read
   them. (Limit of the instrument, stated: a writer that preserves mtime would defeat it. The
   arithmetic above is the independent check.)

Their diff hunks were also re-read in full and re-verified on the merits, below.

### The comment-only claim, re-derived

`git diff -- Source/` filtered to added lines that are NOT `//` comments returns **nothing**. Every
one of the header's 18 insertions is a comment; no declaration, member or `#include` moved. The
header's "NO EXECUTABLE BYTE MOVED FOR THIS DISCHARGE" is true as a source-text claim.

The brief's claim that the repairs since the automation run were Markdown only is also verified,
and by the same clock: the report is `reportCreatedOn 2026.09.06-01.54.08` with mtime `1788659649`
(21:54:09), the only `Source/` file in the diff was last written at 21:51:56 — BEFORE it — and both
files written after it are `.md`. So the exported report covers the header as it now stands, and
`engine.md`'s claim that "`Source/` is byte-identical to what the build below covers" holds.

### The two questions kept apart — re-verified

`StratOptionsPresenter.h:98-105` refuses in terms to settle the mouse-axis question and names the
separate rejection without a line number. That rejection was read at
`Source/StratPlay/StratPlayerController.cpp:616-626` and the characterisation is accurate: it
rejects `SetInputMode` with capture because `bShowMouseCursor = true` is load-bearing and "every
input mode that reliably feeds mouse axis does so by capturing, which changes how the cursor
behaves at the viewport edge and how a click lands." `global.md:246-249` and `engine.md:38-42`
keep the same separation. No file folds the two together.

Note in passing that those comments now sit at `:611` and `:616`, having been at `:443`/`:448` at
`c8c81cb` and `:468`/`:473` at `ee4acf5`. The header cites the block by name and not by line, which
is why it survived three moves.

`AStratPlayerController` calls `SetInputMode` nowhere: the only call in the whole of `Source/` is
`Source/StratPlay/StratShellHUD.cpp:243`. Header line 86 still true.

## Finding 1 (prior) — RESOLVED, and the override was CORRECT

The `-S` justification was re-derived from scratch rather than checked against the entry's own
account. **Every command the new text cites returns what the text says it returns.** Run from this
checkout:

```
$ git grep -n "SetInputMode" c8c81cb -- Source/StratPlay/
c8c81cb:Source/StratPlay/StratPlayerController.cpp:443:// `UPlayerInput` -- this class sets `bShowMouseCursor = true` and calls `SetInputMode`
c8c81cb:Source/StratPlay/StratPlayerController.cpp:448:// WHY POLLING AND NOT AN INPUT MODE. The obvious repair is `SetInputMode` with capture, and

$ git grep -n "SetInputMode" ee4acf5 -- Source/StratPlay/
ee4acf5:Source/StratPlay/StratPlayerController.cpp:468:// ... calls `SetInputMode`
ee4acf5:Source/StratPlay/StratPlayerController.cpp:473:// WHY POLLING AND NOT AN INPUT MODE. ...
ee4acf5:Source/StratPlay/StratShellHUD.cpp:15:// it is one `CreateWidget`, one `AddToViewport` and one `SetInputMode`.
ee4acf5:Source/StratPlay/StratShellHUD.cpp:243:	OwningPlayer->SetInputMode(InputMode);
```

Exactly two lines and no call at `c8c81cb`; four lines at `ee4acf5`, the two comments drifted to
`:468`/`:473`, a third comment, and one call — precisely as written.
`git grep -n "ApplyMenuInputMode" ee4acf5 -- Source/StratPlay/StratShellHUD.cpp` returns
`:225:void AStratShellHUD::ApplyMenuInputMode()`, so the named enclosing definition is right.
`c8c81cb` is dated 2026-08-27 and `ee4acf5` 2026-08-31, both as stated.

**The override of the proposed widening was correct, and I verified the reason rather than the
conclusion.** All four `-S` forms were run:

```
$ git log --oneline -S "OwningPlayer->SetInputMode" -- Source/StratPlay/            -> ee4acf5 (only)
$ git log --oneline --reverse -S "OwningPlayer->SetInputMode" -- Source/StratPlay/  -> ee4acf5 (only)
$ git log --oneline -S "SetInputMode" -- Source/StratPlay/            -> 374398a, ee4acf5, c8c81cb
$ git log --oneline --reverse -S "SetInputMode" -- Source/StratPlay/  -> c8c81cb, ee4acf5, 374398a
```

So: on the narrow token `--reverse` changes nothing and cannot have been load-bearing, exactly as
the entry now says. On the broad token the plain form does put `374398a` first — the widening the
coordinator proposed would have produced a true sentence — but `--reverse` on that same token
returns **`c8c81cb` first**, and `c8c81cb` is the commit where the grep above shows comments and no
call. The engineer's stated reason is therefore correct on the measurement: `-S` counts occurrences
of a string, a comment reflow reads as an origin, and on this subject the widened command would
have named the wrong commit as the arrival of the CALL. Replacing the instrument rather than
widening it was the right call.

**Does the new instrument support the provenance conclusion?** A pair of greps at two commits
strictly only BOUNDS the arrival to the interval, and the top-of-file bullet says exactly that
("the call appears between them"). The stamp at `engine.md:5507-5511` makes the stronger claim
that it arrived AT `ee4acf5`, and it carries its own support in the same sentence — that `ee4acf5`
"added `Source/StratPlay/StratShellHUD.cpp`". I verified that independently:
`git log --diff-filter=A -- Source/StratPlay/StratShellHUD.cpp` returns `ee4acf5` and nothing else.
I also scanned every one of the 39 commits in `c8c81cb..ee4acf5` for a call rather than a comment;
every one returns zero until `ee4acf5`, which returns one. The conclusion is pinned, not merely
bounded, and the entry states the fact that pins it.

`ee4acf5` is 2026-08-31 and the stale sentence's own entry describes `c8c81cb`, 2026-08-27, so
"false as of today, true when written" is right.

**Both sites are correct and consistent, and there is no third.** Every `374398a` mention in
`Tools/architect/state/` was read: `engine.md:60` and `:5513` quote the withdrawn text as a
quotation and correct it in the next sentence; `:63` and `:70` are the two surviving assertions
about that commit and both are true as measured above; `:99`, `global.md:271`, `:4598`,
`tests.md:20` and `:6753` have other subjects (a base commit, an asset-tracking fact, rulings).
Every `git log ... -S` mention in the record is one of the four sites already read. The stale
sentence itself survives, stamped in place and not rewritten, at `engine.md:5503` — the only place
in the tree carrying "anywhere in `Source/StratPlay/`".

## Finding 2 (prior) — RESOLVED

`content.md:173-179` now opens:

> [STAMPED 2026-09-05, LATER THE SAME DAY, BY THE `coordinator` UNDER THIS FILE'S FALLBACK
> CONDITION -- re-measured with its control at the time of writing: one `ToolSearch` against the
> NeoStack server returned `list_unreal_projects` and `unreal_status` SERVED and no
> `execute_script`, the same lookup speaking about the same server, and the separate
> `unreal-editor-direct` server was `ConnectionRefused` at session start. The fallback licenses
> the FILE WRITE only; no acting clause is cited because nothing was acted on -- this stamp
> corrects a prior entry and touches no asset.

Against the file's own header at `content.md:20-23`, which is the authority:

> **Two obligations ride with the fallback:** (1) measure the absence with a control before
> invoking it — an absent tool name proves nothing alone, so show the same lookup returning the
> tools that ARE served; and (2) say so in the entry, naming the coordinator as its writer.

Obligation (2) is discharged on its face — the writer is named, inline, in the first line a reader
arriving by citation lands on. Obligation (1) is discharged in the shape the header specifies and
not a looser one: the SAME lookup, against the SAME server, returning two tools SERVED alongside
the one absent. That is a control, not a bare absence. The `ConnectionRefused` on
`unreal-editor-direct` is additional colour and is correctly not offered as the control.

**The "no acting clause because nothing was acted on" reasoning is right, and it is not a dodge —
because it is checkable and I checked it.** `CLAUDE.md` keeps the file-write permission and the
editor-driver permission apart and warns that naming one actor while leaving the other unnamed is
itself a finding. That warning bites only when there IS acting to attribute. Here there is none:
`git status --porcelain` shows four modified files, none under `Content/`, and no untracked asset;
the stamp edits one Markdown file and corrects a prior entry. `content.md`'s header also says in
terms that the fallback "licenses writing this file only — it is not a general licence to act in
the content lane", which is the sentence the stamp is echoing. A stamp claiming an editor-driver
acting clause here would have been the overclaim, not the omission.

I record without gating on it that the control itself is a session-time measurement no checkout can
re-run — the same class of unfalsifiability `CLAUDE.md` already declares about the transcription
clause's limit (1). What a checkout can verify is the SHAPE of the control and the "touches no
asset" half, and both hold.

## Findings

**1. The discharge falsifies a present-tense claim in `Source/` and leaves it standing. Owner:
`strat-test-author`.**

`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp:50-52`:

> A human at the keyboard remains the only instrument for
> whether the screen is legible or operable, which `StratOptionsPresenter.h` already
> records as an outstanding discharge for the in-match input mode.

`StratOptionsPresenter.h` no longer records an OUTSTANDING discharge for the in-match input mode.
As of this diff it records `DISCHARGED 2026-09-05, IN THE DIRECTION THAT LEAVES THE CODE ALONE`
(`StratOptionsPresenter.h:91`). The debt was paid; the sentence says it is owed. The main clause —
that a human is the only instrument for legibility and operability — is untouched and still true,
and so is the near-identical sentence at `tests.md:92-93`, which says only that the header
"already records" the point and does not call it outstanding. It is the single word
**outstanding** that this pass falsified.

This is the same defect the pass itself is about, one file over. The entry at `engine.md:44-52`
makes a virtue of finding the module-wide sentence that "went stale without its subject moving"
and stamping it; this sentence went stale in exactly that way, in the test file for exactly this
presenter, and was not found. I swept for others and there is only this one: every `discharge`,
`operable` and `input mode` site in `Source/` was read, and the neighbouring
`StratOptionsWidgetBindingClauses.cpp:61-62` ("a human at the keyboard remains the only
instrument") carries no staleness.

The fix is one word or one stamp, and it is `Tests/` so it is not the engineer's to make. Route it
to `strat-test-author`.

## Observations

Non-gating.

- **My own check 8 ran blind on its first attempt this pass, and I am recording it because a
  reviewer's instrument failing silently is worse than the thing it was looking for.**
  `git grep -n "/Game/" -- Source/` returned **zero lines** — a clean-looking result — because Git
  Bash converted the leading-slash pattern into a Windows path before `git` ever saw it. It was
  caught only because my own prior gate report cited `ImportStratDataCommandlet.cpp:54` as a hit,
  which contradicted the empty output; `sed -n '54p'` on that file shows
  `const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);`, so the
  literal was there all along. Re-run as `MSYS_NO_PATHCONV=1 git grep -n "/Game/" -- Source/` and,
  independently, as `git grep -nE '"[^"]*/Game/'`, it returns the expected corpus and the check
  passes on the merits. An empty grep on this box is not evidence of absence unless the pattern
  cannot be path-mangled.
- **A correction to my own prior report.** It stated under check 8 that "every hit outside
  `Tests/` is either a comment arguing the rule or `ImportStratDataCommandlet.cpp:54`". That is
  the correct finding and I re-derived it today; but this report's re-run, with the mangling
  hazard named, is the one a later reader should cite.
- `content.md` now carries two stamps dated the same day, eight lines apart, saying opposite things
  about the same subject: `:172` "The unseen half above is unchanged and still owed" and `:180`
  "THE UNSEEN HALF IS NO LONGER OWED". The later one self-dates as LATER THE SAME DAY, which is
  what separates them, since the dates alone cannot. This is the file's stamp-in-place convention
  working as designed and I am not gating on it, but a reader landing on `:172` by citation reads a
  present-tense sentence that is now false, with only the adjacency of the next stamp to save them.
  The earlier stamp does not point forward.
- `engine.md:60-73` records the withdrawn justification, quotes it, names the gate that blocked it,
  and states the general lesson — "A justification sentence that names a command is a claim ABOUT
  THAT COMMAND, and the only way to write one is to run it and paste back what came out." Written
  flat, not nested, and it reaches the handoff sentence. This is the right shape for a correction.
- `engine.md:66-68` states zero failed, zero notRun, zero succeededWithWarnings and defers the
  figure to `global.md`. Within precedent, and the sweep agrees: 63 suite claims, 1 live, and the
  live one is in `global.md`. No other file carries a live suite count or a phase verdict.
- Surviving `unseen` / `not been seen` hits elsewhere were each read and none is about the options
  screen or the in-match panel: `engine.md:4613` (the camera path), `global.md:3961` (the strip,
  already corrected), `:4819`, `:7045`, `:7207` (window-end tag and gated end-turn hovers),
  `StratMatchSubsystem.h:2183` and `StratInfoPanelSourceParity.cpp:266` (different subjects).
- `Source/StratPlay/StratOptionsPresenter.h` and `Tools/architect/state/content.md` still emit
  `warning: ... LF will be replaced by CRLF ...` on every `git diff`. Known behaviour of this tree
  under `core.autocrlf=true`; no bytes at risk.

## Instruments

`Saved/AutomationReport/index.json` read with `utf-8-sig`: `reportCreatedOn 2026.09.06-01.54.08`,
succeeded 435, failed 0, notRun 0, succeededWithWarnings 0, 435 entries, every one in state
Success (counted, not taken from the summary fields).

`python Tools/architect/strat_banner_sweep.py` printed, in full:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 435 entries, all Success (2026.09.06-01.54.08)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0
  report identity: mtime=1788659649, reportCreatedOn='2026.09.06-01.54.08'
  suite claims found: 63 (1 live, 62 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

The printed line is the verdict; the exit code was 0 and was not read as one.

No build and no suite were run by this gate. Result: Succeeded in 13.29 s is
`strat-gameplay-engineer`'s report, recorded as theirs.

VERDICT: BLOCK
