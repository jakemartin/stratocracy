Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `f92ce01` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: second mutant campaign — record correction — strat-integration-reviewer

## Scope
Uncommitted working tree at `E:\MultiAgent\Stratocracy`, base `f92ce01` on `master`, nothing staged (`git status --porcelain` shows six ` M` entries and no index entries). Six files, +674/−30, matching the brief exactly. `.agents/ue-project-context.md` read for drift; both `.Build.cs` derivations re-run against the tree; the shipped asset, the loaded module DLLs, the exported report and the `Source/` clause sites measured directly.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`git diff --stat -- Source/StratRules Data`) | pass — empty output. Manifests still name `96d93ea` / `c2f5860`; nothing in the changeset reaches them |
| 2 | No `strat::` call outside `StratBridge/`, `StratRules/` | pass — repo-wide grep for `strat::[a-z]\w*\s*\(` outside those two dirs returns nothing; the diff adds no `strat::` token at all |
| 3 | No vendored header in front of UHT | pass — no header changed; the two `.cpp` edits are comment-only |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — no member declaration touched |
| 5 | Module arrows unchanged and correct | pass — derivation printed **16 rows**, field census printed **10 names** (incl. the `BRIDGE=` string-literal false positive). `StratBridge PrivateDependencyModuleNames StratRules` verified guarded: `StratBridge.Build.cs:133-137`, `bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` … `if (!bCompileVendoredRulesHere) { PrivateDependencyModuleNames.Add("StratRules"); }`. `StratUI` has no `Stratocracy` edge. No `.Build.cs` in the diff |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — empty diff |
| 8 | No `/Game/` literal in C++ | pass — the added `/Game/UI/WBP_Options` and `/Game/UI/WBP_NoSuchAsset` occurrences are all on `//` comment lines; the live `kOptionsClassPath` literals are unchanged automation fixtures (standing exception) |
| 9 | No widget-side arithmetic | pass — no `StratUI` production code changed |
| 10 | Lane discipline | pass — `Tests/*.cpp` + `tests.md` = test author; `engine.md` = engineer; `content.md` = editor-builder; `global.md` = coordinator. In-session clause: both receiving files carry inline `ACTING`/`WRITING` attribution and name base `f92ce01`. Obligation (1) (both routes offered) is unfalsifiable from a checkout |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** — findings 1 and 2 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` clean of staged content; `HEAD` still `f92ce01` |

Additional instruments run: `python Tools/architect/strat_banner_sweep.py` → `SWEEP CLEAN`, exit 0, *"suite claims found: 70 (1 live, 69 stamped)"*, report identity `reportCreatedOn 2026.09.08-18.37.07`, macro census 477.

## Findings

**1. BLOCK — `Tools/architect/state/engine.md`: the campaign section's headline count contradicts its own enumeration, and contradicts `tests.md` and `global.md`.**

`engine.md:10063` — *"## 2026-09-08 -- the SECOND engine-lane mutant campaign, over base `f92ce01`: **eleven mutants run, ten killed**, one surviving exactly as predicted, and one that cannot be compiled at all"* — repeated at `engine.md:10192` — *"**Ten of the eleven runnable mutants were killed**, seven of them by exactly one clause each"* — and `engine.md:10195` — *"**The eleventh survived** as predicted"*.

The same section enumerates **twelve** mutants: R3b, R1b, R1c, R2b, R2c, R3c, R4b, R4c, R6b, R7a, R7c, R9c. Its own arithmetic gives eleven kills, not ten: seven reddened one clause each (R3b, R3c, R4b, R6b, R7a, R7c, R9c) and, per its own line, *"R1b, R1c and R2b each hit N1 and N2; R4c hit N4 and N6"* — four more. 7 + 4 = 11 killed, + R2c surviving = 12 run. It is inconsistent under the other scoping too: if R9c is excluded (as `tests.md:779`'s block legitimately does, scoping *"eleven further source mutants **against these seven clauses**"*), then only **six** mutants reddened exactly one clause, not seven, and R9c is still enumerated as run in this section. `tests.md`'s 18-mutant total (12 production source + 2 test-file + 4 asset) and `global.md`'s 27-run/26-killed arithmetic both require **twelve**.

Discharged by either: restating the heading and the two summary bullets as *twelve mutants run, eleven killed, the twelfth survived*; or explicitly scoping them to the seven options-exit clauses, correcting *"seven of them by exactly one clause each"* to six and naming R9c and its N9 kill outside that count. Owner: **`strat-gameplay-engineer`** (`engine.md` is its file).

**2. BLOCK — `Tools/architect/state/tests.md`: the stale-binary block misdescribes the instrument that produced one of its own numbers.**

In the block headed *"MEASURED, NOT ASSUMED, AND QUOTED HERE RATHER THAN CITED TO A PATH"*: *"`grep -a -c` for `ReturnToTitleButtonRenamed` returned **11** in the DLL against **0** in `Source/` … after the rebuild **the same two greps** returned **0** and **12**."*

The two greps as defined are (DLL, `Source/`) for the token `ReturnToTitleButtonRenamed`. After the rebuild that pair is **0 and 0**, not 0 and 12 — measured now: `grep -rc ReturnToTitleButtonRenamed Source/` → no hits; `grep -a -c ReturnToTitleButtonRenamed Binaries/Win64/UnrealEditor-StratUI.dll` → `0`. The `12` is reproducible only from a **third** grep, of the DLL for the **original** name: `grep -a -c ReturnToTitleButton Binaries/Win64/UnrealEditor-StratUI.dll` → `12` (and `0` in the other four Strat module DLLs). The numbers are right; the sentence naming which instrument produced them is not — the "verified numbers, false cause" shape.

Discharged by restating the post-rebuild pair as what it was: the DLL grepped for the renamed token returned 0 **and the DLL grepped for the original name returned 12**. Owner: **`strat-test-author`** (`tests.md` is its file).

## Observations (non-gating)

**a. The two recorded errors, checked as far as a checkout allows.**
- *(ii) The stale binary* — **corroborated, not merely asserted.** The mechanism is reproducible in the tree today: `Source/` carries zero `ReturnToTitleButtonRenamed`, `UnrealEditor-StratUI.dll` carries zero of it and twelve of the original name, and the mtime chain fits the account exactly — test headers 14:33:45/14:34:27, DLL 14:35, `Saved/AutomationReport/index.json` 14:37:07 local (= `reportCreatedOn 2026.09.08-18.37.07` UTC). So the final rebuild did follow the last revert and the live report is a run over the restored tree. What is **unfalsifiable from a checkout**: the `11` in the *contaminated* DLL, the *pre-revert* mtime delta ("about two minutes"), the `N9 RED` in the superseded `A1` run and its `N8 RED ALONE, N9 GREEN` re-run — all of those lived in a binary that has since been overwritten and in reports that `.gitignore:68` (`Saved/*`) keeps untracked. The record correctly states them inline rather than citing a path; a reader must take the run outcomes on the author's word. Finding 2 above is the one part of that block a checkout *can* falsify, which is why it matters.
- *(i) The five "not runnable as written" mutants* — **unfalsifiable in detail from a checkout, and not named anywhere.** `global.md` records the environment cause (an editor reopened mid-run holding the module DLLs) but names none of the five, so nothing in the tree can be matched against them. The only corroboration available is indirect: twelve production-source mutants did run afterwards, which is consistent with the mutants having been runnable all along. Recorded here so it is not read as verified.

**b. R2c's survival — verified, and the record is right.** `StratShellSubsystem.cpp:100` is `OutRefusalReason = FText::GetEmpty();`, the first statement of `IsRoutePermitted`, and every arm that returns `true` (`ReturnToTitle` at :153, `QuitGame` :160, `Options` :169, `NewMatch` :112, `ContinueMatch` :134) returns without writing it. `Refusal` at `StratOptionsPresenter.cpp:309` is a fresh default-empty local. So `ExitModel.ReturnToTitleReason = Refusal;` and the ternary at :316-317 write the same value on both arms — there is nothing observable under the substitution. **A confirmed prediction, not a hole; no clause should have caught it, and the record is correct at every site.**

**c. R3b's split — real, not excessive caution; `engine.md` is not under-claiming.** R3b moves `CloseOptionsPanel()` ahead of `ExecuteRoute` (`StratOptionsPresenter.cpp:434-453`). On the refusal arm the early return at :452 leaves the panel already down — observable, and one clause says so. On the **permitted** arm the close still happens either way, so the ordering swap produces no difference there; and the deferred-travel argument at :464-469 is about the close call *existing* alongside `Deinitialize`, which R3b does not delete. R3b genuinely cannot reach that half. The under-claim is correct as written.

**d. The unrunnable mutant — count and claim both verified.** `->ExitModel` is read at **16** sites in the clause files (12 × `Widget->ExitModel` in `StratOptionsExitClauses.cpp`, 4 × `Panel->ExitModel` in `StratOptionsExitRouteClauses.cpp`); `ExitModel` is declared `FStratOptionsExitModel` at `StratOptionsWidget.h:568`. Moving `bReturnToTitleEnabled`/`ReturnToTitleReason` onto `FStratAudioOptionsModel` (`:325`) breaks compilation at all sixteen, i.e. the mutant must edit its own oracle. **"Unrunnable" is the right word.** "Roughly fifteen" is an understatement of 16 by one — not worth a finding given the hedge, but the exact number is available and would be stronger. Separately, R9c's *"16 sites"* is exact: 6 + 6 + 4 occurrences across the header, the widget `.cpp` and `StratOptionsWidgetDouble.h`.

**e. `content.md` — re-verified independently, and the "additions only" claim holds byte-exactly.** `git diff --numstat` → `113 0`; a `difflib` opcode comparison of `git show HEAD:…content.md` against the worktree file yields **one opcode, `insert` at line 51, 113 lines** and nothing else — every pre-existing byte survives in order. No BOM (first bytes `# Strato`), decodes as UTF-8, zero CRLF in both HEAD and worktree. `Content/UI/WBP_Options.uasset` hashes to `2beba…4667` and `git status -- Content/` is empty. Note for the record's own reasoning: `+N/−0` against HEAD proves nothing was removed *relative to `f92ce01`*, which is exactly the claim being made here, but it could not have seen a rewrite between two uncommitted passes — the `difflib` check above is what actually closes that.

**f. Date discipline — clean.** Every date the diff adds is `2026-09-08` (29 occurrences); the ten `2026-09-07` and one `2026-08-22` are references to prior passes. Local clock and the UTC report stamp agree today (14:37 local = 18:37 UTC). The sweep's banner-freshness check (3) and report-identity check (4) both pass, so no entry is dated ahead of the banner and the report does not predate any test-defining `.cpp`.

**g. Exactly one live figure — confirmed, and the older banner's edit was forced by the tool, not chosen.** The sweep classifies 1 live / 69 stamped and the live claim sits in the newest banner (`THE SUITE IS **477/477** … reportCreatedOn 2026.09.08-18.37.07`). The older banner's `reportCreatedOn` had to be advanced to the same stamp because sweep check 5(a) fails any *unstamped* `reportCreatedOn` in `global.md` that is not the opened report's — worth knowing, since it means that in-place edit of a historical banner is mechanically required rather than a convention breach; the superseded `2026.09.08-03.43.53` is quoted immediately below under `[STAMPED]`. **One untidiness there:** the stamp block's last line runs on into the older banner's own live prose — `…a fact about the shape and not about either author. NOTHING IN THIS PASS MOVED THE COUNT: no clause was added, removed or` — so the `[STAMPED]` prefix visually annexes a sentence that is not part of the stamp. Cosmetic, non-gating, and cheap to fix with a line break while finding 1 is being addressed.

**h. `PREDICTED, UNRUN` census matches the record's own warning.** The token greps 7× across the two shipped-asset test files — 4 in `StratShippedOptionsCaptionPresence.cpp` (`:170,180,188,193`) and 3 in `StratShippedOptionsExitControlParity.cpp` (`:183,193,200`) — every one inside a `was *"PREDICTED, UNRUN"* until 2026-09-08` quotation. `tests.md`'s "seven times, quoted as history" is exact.

**i. `.agents/ue-project-context.md` shows no drift** against the 16-row derivation, including the conditional monolithic edge (its lines 40, 47-49, 69, 74, 114).

VERDICT: BLOCK
