Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `f92ce01` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: second mutant campaign, record corrections (RE-GATE) — strat-integration-reviewer

## Scope

Working-tree diff against base `f92ce01`, nothing staged, nothing committed. Six modified files, 725 insertions / 30 deletions: `Source/StratUI/Tests/StratShippedOptionsCaptionPresence.cpp`, `Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`, and `Tools/architect/state/{content,engine,global,tests}.md`. Every structural check below was re-derived against the current tree, not carried over from the previous pass.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` prints nothing |
| 2 | No `strat::` call outside the two permitted dirs | pass — grep for `strat::[a-z]\w*\s*\(` outside `StratBridge/`+`StratRules/` returns zero hits |
| 3 | No vendored header in front of UHT | pass — `StratBridge.h` includes `Data.h`/`Replay.h`/`Save.h`/`Scenario.h`/`Ui.h` but declares **no** `UCLASS`/`USTRUCT`/`UENUM` and has no `generated.h`; all six macro-name hits in that file are inside comments. Judged, not forwarded |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — only live holders are `StratMatchSubsystem.h:2869` and `StratScoreboardHUD.h:1253`, both `TPimplPtr<FStratBridge>`; the sole `TUniquePtr<FStratBridge>` string is prose at `StratViewModel.h:47` |
| 5 | Module arrows unchanged and correct | pass — derivation printed **16 rows**, all five modules present, matching the carried block including `StratBridge PrivateDependencyModuleNames StratRules`. Read the guard: `StratBridge.Build.cs:133-138` — `bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic`, the `.Add` sits inside `if (!bCompileVendoredRulesHere)`, so it is the conditional monolithic-only edge and not drift. Field census printed **10** names, matching (incl. the `BRIDGE=` false positive from the `"STRAT_VENDORED_RULES_IN_BRIDGE="` literal). No eleventh name. No `.Build.cs` in the diff |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — prints nothing |
| 8 | No `/Game/` literal in C++ | pass — all hits are `LoadTable(TEXT("/Game/StratData/..."))` inside `Source/*/Tests/`, the standing automation-fixture exception; none in the diff |
| 9 | No widget-side arithmetic | pass — no `StratUI` production code changed; the two `StratUI` files touched are `Tests/` and comment-only |
| 10 | Lane discipline | pass — `Tests/` .cpp by `strat-test-author`; `engine.md` by `strat-gameplay-engineer`; `tests.md` by `strat-test-author`; `content.md` by `strat-editor-builder` (ACTING and WRITING, under no clause — declares `execute_script` reached directly); `global.md` by the coordinator. Every entry names base `f92ce01` and attributes ACTING/WRITING separately inline |
| 11 | Prose blocks present | **BLOCK** — see Finding 1 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries, no index entries |

**Ask (a) — structural checks re-derived, not trusted:**
- `git diff --stat -- Content` prints nothing. `sha256sum Content/UI/WBP_Options.uasset` = `2bebaccbbd291a0351069a51354983903150631b113c923f4697a28293de4667`, matching the value all four record files quote. (`git check-attr` confirms the file is LFS, so the hash was taken from the working-tree bytes, not `git show`.)
- Comment-only proven mechanically, not by eye: stripping `+`/`-`, leading whitespace, blank lines and `//` lines from `git diff -U0` over both `.cpp` files leaves **0 lines**.

**Ask (c) — every count re-derived.** All of these close:
- engine.md's twelve: the enumerated ids **R3b, R1b, R1c, R2b, R2c, R3c, R4b, R4c, R6b, R7a, R7c, R9c** = 12, and the three sub-sections list 1 + 4 + 7 = 12. Seven single-clause kills (R3b, R3c, R4b, R6b, R7a, R7c, R9c) + four two-clause kills (R1b, R1c, R2b, R4c) = **11 killed**, + R2c surviving = **12 run**. Six of the seven single-clause kills are options-exit clauses, R9c is the seventh and is scoped out explicitly. Closes.
- tests.md's 18/17/1: 13 source-and-test-file mutants + 1 survivor + 4 asset mutants = 18; 21 suite runs = 18 + 2 controls + 1 superseded contaminated run. Closes.
- Cross-file: engine.md's 12 (engine-side) + 2 test-file `kOptionsClassPath` mutants + 4 asset mutants = tests.md's 18. tests.md's "ELEVEN FURTHER SOURCE MUTANTS AGAINST THESE SEVEN CLAUSES, ten reddened" = engine.md's 12 minus R9c (aimed at N9, not one of the seven). global.md's 28/27/26 = second campaign 18/17 + first campaign 9/9 + 1 unrunnable. All four files agree.
- `16 sites` for the member rename: re-derived at exactly **16** (`StratOptionsWidget.h` 6, `StratOptionsWidget.cpp` 6, `StratOptionsWidgetDouble.h` 4).
- `PREDICTED, UNRUN` residue: **4** in `StratShippedOptionsCaptionPresence.cpp`, **3** in `StratShippedOptionsExitControlParity.cpp`, 7 total, exactly as claimed, and every one is inside `was *"PREDICTED, UNRUN"* until 2026-09-08`.
- `477`: `strat_banner_sweep.py` reports SWEEP CLEAN, 477 report entries all Success, macro census `IMPLEMENT_SIMPLE_AUTOMATION_TEST=477`, one live suite claim / 69 stamped. Independently re-derived 477.
- Old wording survives only inside quotation: `eleven mutants run, ten killed` occurs at `engine.md:10063` and `:10077`, both inside `*"..."*`; `Ten of the eleven runnable` at `:10214`, inside `*"..."*`. Nothing live.

## Findings

**1. `tests.md` — the bullet headed "WHICH HALF A CHECKOUT CAN RE-RUN" is false for two of the three numbers it answers with, and it applies a standard the same entry applies correctly two bullets earlier.**

`Tools/architect/state/tests.md`, in the 2026-09-08 campaign entry, the stale-binary bullet:

> **WHICH HALF A CHECKOUT CAN RE-RUN, because it is worth more than the numbers alone:** the POST-rebuild triple -- **0**, **0**, **12** -- is LIVE in the tree today and the gate reproduced all three of them. The PRE-rebuild **11** and the mtime delta are HISTORICAL AND NOT REPRODUCIBLE ... a reader is entitled to read them as such rather than as something a checkout can check.

I re-ran all three greps and every number is right:

- (1) `grep -a -c ReturnToTitleButtonRenamed Binaries/Win64/UnrealEditor-StratUI.dll` → **0**
- (2) `grep -rc ReturnToTitleButtonRenamed Source/` → **0** in every file
- (3) `grep -a -c ReturnToTitleButton Binaries/Win64/UnrealEditor-StratUI.dll` → **12**, against **0** in each of `UnrealEditor-StratBridge.dll`, `-StratPlay.dll`, `-StratRules.dll`, `-Stratocracy.dll`

So the correction the engineer and test-author made to my Finding 2 is **sound** — three greps, correctly labelled, each number attributed to the instrument that produced it. That half is discharged.

What is wrong is the new sentence added on top of it. `Binaries/` is gitignored — `git check-ignore -v` returns `.gitignore:50:Binaries/*`, and `git ls-files Binaries/` is empty. Greps (1) and (3) read an **untracked build artifact**. A reader on a fresh checkout who does exactly what the heading invites gets `No such file or directory` for (1) and (3), a bare `0` for (2), and concludes the **12** was wrong — which is the identical failure mode I blocked on, one level up: the numbers stay right and the sentence about how to reproduce them is what misleads. Only grep (2) is re-runnable from a checkout; (1) and (3) need a built tree.

The entry already knows this standard and states it correctly about a different artifact, four bullets above:

> **THE MEASUREMENTS ARE STATED INLINE AND NOTHING IS CITED TO A REPORT PATH.** The campaign's exported reports are UNTRACKED; no checkout has them, so a citation to one is unfalsifiable by the reader it is written for.

`Saved/AutomationReport/index.json` is ignored by `.gitignore:68:Saved/*`; `Binaries/Win64/UnrealEditor-StratUI.dll` is ignored by `.gitignore:50:Binaries/*`. Same condition, opposite treatment, inside one entry.

The split needs three states, not two: **checkout-re-runnable** — grep (2) over `Source/`; **built-tree-re-runnable, artifact untracked** — greps (1)-post and (3); **historical, binary since overwritten** — grep (1)-pre at 11, and the mtime delta. Note that this repair makes the entry's own argument *stronger*, not weaker: the reason (2) is the load-bearing grep is precisely that it is the one a bare checkout can check, and it is the one that proves the source was clean while the binary was not.

**Owner: `strat-test-author`** — `tests.md` is its file and the sentence is inside its entry.

## Observations

Non-gating.

**(a) The engineer's generalisation — I do not recommend cutting it.** `engine.md`: *"Any section that promotes one item out of a list and then totals the list has this hole."* I checked the narration question directly: grepping the `engine.md` added lines for `reviewer|integration-review|VERDICT|BLOCKED|gate` returns exactly one hit, and it is `"No suite figure and no phase verdict appear below; both are global.md's"` — a scope declaration, not narration. The note names no reviewer, no verdict, and no pass. It does not commit the defect I found once. On breadth: the universal is one notch stronger than the mechanism supports — the hole opens when the total is computed *by counting the lists*, not merely by promoting an item, so `has this hole` would be more exactly `invites this error`. That is a wording nit, not a false claim, and it costs nothing to leave. Against cutting it: it is the only sentence in the section that says what produced the wrong number, and this record's own convention is that a corrected figure must say so. Keep it; soften `has` to `invites` if the lane touches the block for any other reason.

**(b) "roughly fifteen" — the exact number is not owed, but the subject is.** The hedge is not false: `->ExitModel` occurs **16** times across the clause files (`StratOptionsExitClauses.cpp` 12, `StratOptionsExitRouteClauses.cpp` 4), and 16 is roughly fifteen. The argument it carries — a mutant relocating the fields would have to edit its own oracle — holds at one site, so the number is decoration and understating it weakens nothing. What is genuinely imprecise is the *subject*: all three files attach `roughly fifteen` to a conjunction of two different tokens, and the two do not have the same count — `->ExitModel` is 16 in the clause files while `FStratOptionsExitModel` is 11 there (20 across all of `Source/`). A reader cannot tell which is being hedged. Not a finding, because no re-run of it produces a contradiction; worth one word if the block is ever touched.

**(c) Observation (g) from the previous pass is discharged, verified at the bytes.** In `global.md`, `NOTHING IN THIS PASS MOVED THE COUNT: no clause was added, removed or` now begins at column 0 on its own line with no `[STAMPED]` prefix, and the seven `[STAMPED]` lines above it enclose only the superseded `reportCreatedOn 2026.09.08-03.43.53` citation. The live sentence is no longer annexed.

**(d) The three corrections in `engine.md` sit at their sites, as asked.** The heading at `:10063` carries its `[COUNT CORRECTED ...]` inline; the two closing bullets carry theirs inline at `:10214` and at the "TWELFTH survived" bullet, each quoting what it replaced. The scoping note added at the seven-mutant bullet and the `## NEXT` bullet both correct *at* the now-false words rather than beneath them. The restatement remedy the engineer chose does close on its own arithmetic — I re-derived 7 + 4 = 11 from the enumerated ids without using either list — and `seven of them by exactly one clause each` survives true, as its stated reason predicted.

**(e) `content.md`'s entry correctly declares itself under no clause.** `strat-editor-builder` (ACTING and WRITING), reaching the editor itself through `execute_script`, so neither the editor-driver clause nor `content.md`'s fallback-writer condition is in play — and it says so rather than leaving it inferred. It restates no suite figure. This is the shape the CLAUDE.md route ruling asks for.

**(f) One cross-file number collision that is correctly scoped but reads sharply.** `tests.md` says *"ELEVEN FURTHER SOURCE MUTANTS AGAINST THESE SEVEN CLAUSES ... Ten of the eleven reddened"* in the same changeset where `engine.md` corrects an **eleven** to a **twelve**. They are different elevens and `tests.md` scopes its one explicitly in the same sentence, so it is not a contradiction — but it is the one place a reader skimming both files could reconstruct the error that was just corrected. No action; noted because the sweep cannot see this shape.

**(g) The un-derivable counts I could not check, and I am naming them rather than passing them.** The mutant reds themselves (`RED N8 alone`, `19 -> 19`, `MASTER` at 0 while `LabelMaster` at 1, the Blueprint compile failure under A3), the `11` pre-rebuild DLL count, and the mtime delta are all testimony about trees that no longer exist. Each is stated inline rather than cited to a path, which is the right treatment, and the closing control plus the verified sha256 is the strongest thing available. I did not re-run the suite and did not attempt to reproduce any mutant — that is `strat-test-author`'s report, not mine.

VERDICT: BLOCK
