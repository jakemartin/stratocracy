# Gate: the in-match input-mode discharge (comment-and-record pass) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`c5d2c65`. `git status --porcelain` was run rather than taken from the brief and prints exactly
four modified files and nothing staged:

```
 M Source/StratPlay/StratOptionsPresenter.h
 M Tools/architect/state/content.md
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
```

`git diff --stat`: 102 insertions, 5 deletions across those four. `.agents/ue-project-context.md`
was read first and checked against the tree; it needed no correction this pass.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing. The manifests remain the authority; nothing else was needed, since the empty diff is the whole check |
| 2 | No `strat::` call outside `StratBridge/` or `StratRules/` | pass — the lowercase-initial call grep across `Source/`, minus those two directories, returned zero lines. No type-name hits needed judging this pass |
| 3 | No vendored header in front of UHT | pass — every `.h` including `StratBridge.h`/`Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h` was tested for `UCLASS`/`USTRUCT`/`UENUM`; zero violations |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only `TUniquePtr<FStratBridge>` in the tree is inside a prose block at `StratUI/StratViewModel.h:47` recording the `C4150` measurement |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted; see the derivation below |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory. `IMPLEMENT_MODULE` census unchanged; `StratRules` correctly still absent from the `Modules` array |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — every hit outside `Tests/` is either a comment arguing the rule or `ImportStratDataCommandlet.cpp:54`, the standing producer exception |
| 9 | No widget-side arithmetic | pass — no `StratUI` file is in the diff |
| 10 | Lane discipline | pass on the engineer's half, **BLOCK on the coordinator's** — see Finding 2 |
| 11 | Prose blocks present | pass on shape, **BLOCK on content** — see Finding 1 |
| 12 | Nothing staged or committed | pass — four ` M` entries, no index column set, and `HEAD` is still `c5d2c65` |

### Check 5, derived rather than restated

The extraction over `Source/*/*.Build.cs` printed **16 rows**, all five modules present, matching
the carried block row for row — including the sixteenth, `StratBridge PrivateDependencyModuleNames
StratRules`. The field census printed **10 names**, exactly the documented set, including the
`BRIDGE=` string-literal false positive; there is no eleventh.

The sixteenth row was then read for its guard rather than treated as an unconditional arrow.
`StratBridge.Build.cs:133-138`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
```

Both halves come from the one expression, the link edge is monolithic-only and correct, and
`Source/StratBridge/Vendored/*.strat.cpp` is intact. No `.Build.cs` is in this diff at all.

### Comment-only assertion, re-derived rather than assumed

Every added line in `git diff -- Source/StratPlay/StratOptionsPresenter.h` (18 insertions, 0
deletions) begins with `//`. No declaration, no member and no `#include` moved. The header's claim
**"NO EXECUTABLE BYTE MOVED FOR THIS DISCHARGE"** is true of the diff as a source-text claim.

### The two questions kept apart — verified

`StratOptionsPresenter.h:98-105` states that the discharge rests on ONE unmeasured human report,
that no clause in this tree can observe an input mode, and that it is reversible, and it refuses
in terms to settle the mouse-axis question. That separate rejection was read at
`StratPlayerController.cpp:616-626` and the characterisation is accurate: it rejects `SetInputMode`
with capture because `bShowMouseCursor = true` is load-bearing and every mode that reliably feeds
a mouse axis captures, which changes how a click lands. `global.md:249-251` and `engine.md:38-42`
keep the same separation. No file folds the two together.

The header's surviving line 86 — `AStratPlayerController` calls `SetInputMode` nowhere — was
re-verified against `HEAD`: the only `SetInputMode` CALL anywhere in `Source/StratPlay/` is
`StratShellHUD.cpp:243`, in `AStratShellHUD`. Still true.

### Instruments

`Saved/AutomationReport/index.json` read with `utf-8-sig`: `reportCreatedOn 2026.09.06-01.54.08`,
`succeeded 435`, `failed 0`, `notRun 0`, `succeededWithWarnings 0`, 435 entries, every one in
state `Success`. `global.md:33`'s citation matches the exported report.

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

The printed line is the verdict; the exit code was 0 and was not read as one. Only `global.md`
carries a live suite figure — one live claim, and it is there.

The build was NOT re-run. This gate does not run builds or suites; `Result: Succeeded` in 13.29 s
is the engineer's report and is recorded here as theirs, not as this gate's measurement.

## Findings

**1. A recorded measurement in `engine.md` does not reproduce, and it is the one sentence that
justifies the instrument choice. Owner: `strat-gameplay-engineer`.**

`Tools/architect/state/engine.md:51-52`:

> **The `--reverse` is load-bearing** -- the plain `-S` form answers MOST RECENT and returns
> `374398a` first, which would have written a false provenance sentence.

That sentence is attached to a command named two lines above it, at `engine.md:49-50`:
`git log --reverse --oneline -S "OwningPlayer->SetInputMode" -- Source/StratPlay/`. Re-run from
this checkout, the plain form of THAT command returns exactly one commit, and it is not `374398a`:

```
$ git log --oneline -S "OwningPlayer->SetInputMode" -- Source/StratPlay/
ee4acf5 The title menu's one-tick hop is a correct fix nothing in the tree can assert, ...
```

`374398a` appears only under the BROADER token, which the entry does not name:

```
$ git log --oneline -S "SetInputMode" -- Source/StratPlay/
374398a The volume screen exists and is reachable from the title menu and from inside a match, ...
ee4acf5 The title menu's one-tick hop is a correct fix nothing in the tree can assert, ...
c8c81cb The hover was listening on a channel the engine never speaks on, ...
```

The same claim is repeated inside the stamped sentence itself, at `engine.md:5484`:

> answers ORIGIN; the plain `-S` form answers most-recent and would have named `374398a` here.

**The conclusion is correct and is not in dispute.** Everything else in the provenance was
re-derived independently and holds: `git grep -n "SetInputMode" c8c81cb -- Source/StratPlay/`
returns exactly two lines, both comments in `StratPlayerController.cpp` (`:443`, `:448`), and no
call; `ee4acf5` is dated 2026-08-31 and added `Source/StratPlay/StratShellHUD.cpp` in one commit
of 245 insertions, carrying `ApplyMenuInputMode` with `const FInputModeUIOnly InputMode;` and
`OwningPlayer->SetInputMode(InputMode);`. The general principle — plain `-S` answers most recent,
`--reverse` answers origin — is true and worth keeping.

What is wrong is the WITNESS. An entry that names a command and then reports what it returns is
making a measurement claim, and this one is false against the command it names. That is exactly
the defect this record blocks on elsewhere: a re-runnable citation that does not re-run, sitting
in the sentence whose whole job is to tell a later reader why the narrower instrument was needed.
The fix is one sentence — name the broader token that actually produces `374398a`, or state the
`--reverse` principle without a counterfactual output that command did not produce.

**2. The new `content.md` stamp discharges neither of the two obligations its own file header
attaches to the fallback it relies on. Owner: the `coordinator`.**

`Tools/architect/state/content.md:173-177`, the whole of the added text:

> [STAMPED 2026-09-05, LATER THE SAME DAY: THE UNSEEN HALF IS NO LONGER OWED. A human has seen
> and operated the screen and signed off on the layout pass that followed, and the in-match
> input mode named in the sentence above is DISCHARGED -- not by taking a mode, but by the
> human report that `StratOptionsPresenter.h` had named as its discharge condition. The panel
> still takes no input mode; that is now a recorded decision rather than an open question.]

`content.md`'s own header, at `content.md:20-23`, states the fallback's terms:

> **Two obligations ride with the fallback:** (1) measure the absence with a control before
> invoking it — an absent tool name proves nothing alone, so show the same lookup returning the
> tools that ARE served; and (2) say so in the entry, naming the coordinator as its writer.

The stamp names no writer, cites no clause, and carries no control measurement. Obligation (2) is
violated on its face: the requirement is greppable and there is nothing there. Obligation (1) is
violated as far as this tree can tell — no control appears anywhere in the diff. A reader arriving
at line 173 by a citation lands on an unattributed edit to a file whose sole writer is
`strat-editor-builder`, which is precisely the failure the inline-attribution rule exists to
prevent, stated in `CLAUDE.md` in those words.

**On the substantive question the brief asked — whether the fallback is being STRETCHED — my
answer is no, and the finding is not that.** The header's licence is temporal, not
content-scoped: *"The `coordinator` may write it only when `execute_script` is absent from the
session's tool surface"*, and it licenses the coordinator *"to AUTHOR this file's content"* with
no qualification as to which content. A stamp on a prior entry is this file's content. The
rationale sentence beside the condition explains WHY the line was drawn there, and a rationale
narrows nothing by itself. So the citation reaches this edit in kind. It simply was not paid for.

Note also that the immediately preceding stamp, `content.md:170-172`, is likewise unattributed and
is already committed at `c5d2c65`. This is a repeat rather than a novelty, which makes it worth
fixing in the record's own terms and not only in this diff.

## Observations

Non-gating.

- `engine.md:67-68` states `zero failed, zero notRun, zero succeededWithWarnings` and then says
  the figure is `global.md`'s to state. That is within precedent — `engine.md:5302` does the same
  — and the sweep reports one live suite claim, in `global.md`. No count moved on this work.
- `global.md:244`'s *"see the paragraph above on the options screen"* resolves: `global.md:186-196`
  records the human report and the later sign-off on the layout pass, to the same depth and no
  greater. The citation is good.
- The coordinator's judgment to leave `tests.md:88-93` alone is CORRECT. That passage says *"NO
  CLAUSE PROVES A PIXEL"* and that legibility and operability *"remains a human at the keyboard,
  as `StratOptionsPresenter.h` already records for the in-match input mode"* — a claim about what
  clauses prove and about what the header records, both still true after the discharge. Rewriting
  it would have made it worse. The other surviving `unseen` / `not been seen` hits across the
  record (`engine.md:4586` on the camera path, `global.md:3961`, `:7045`, `:7207`) have different
  subjects; each was read, and none is about the options screen or the in-match panel. Nothing was
  missed.
- Nothing in the record misstates the routing question. The engineer's entry opens
  `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, ... base commit `c5d2c65`, UNCOMMITTED)
  and says in terms that no exception clause applies and none is cited, which is the right shape
  for a dispatched in-lane pass. No file claims an in-session write, and `CLAUDE.md`'s in-session
  clause is correctly not invoked anywhere. Whether both routes were genuinely offered to the user
  is not observable from this checkout, as `CLAUDE.md` already declares about that obligation.
- Both `Source/StratPlay/StratOptionsPresenter.h` and `Tools/architect/state/content.md` emit
  `warning: in the working copy of ..., LF will be replaced by CRLF the next time Git touches it`
  on every `git diff`. Known behaviour of this tree under `core.autocrlf=true`; no bytes are at
  risk. Recorded only so a later reader does not mistake it for drift.
- `.agents/ue-project-context.md` was checked against the tree this pass and needed no correction.
  Its `StratBridge` row already reasons both link-type branches correctly.

VERDICT: BLOCK
