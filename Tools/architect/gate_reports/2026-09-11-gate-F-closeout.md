# Gate: 2026-09-11 closeout (FIFTH PASS, base `a94c2c4`) — strat-integration-reviewer

> Persisted verbatim by the `coordinator` from the `strat-integration-reviewer` dispatch's final
> report, 2026-09-11. Reviewer agent id `a9698d38628d6ab4f`. The reviewer is read-only and writes
> no file.

## Scope
I audited the whole uncommitted working tree at `E:\MultiAgent\Stratocracy` on `master`, with HEAD `a94c2c4` equal to `origin/master`. Nothing is staged. That is 19 modified tracked files and 13 untracked files: three new `SKILL.md` files and ten gate reports. I left out `HOW_TO_PLAY.md`, as instructed. Only this tree appears in `git worktree list`.

## Checklist
| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass. `git diff --stat -- Source/StratRules Data` printed nothing, and `git status --porcelain` on those paths also printed nothing. The manifests read `rulesCommit 96d93ea…` and `dataCommit c2f5860…`. |
| 2 | No `strat::` call outside StratBridge/StratRules | pass. The only hits outside those two modules are string literals inside test assertion messages, for example `"asks strat::uiForecast per enemy…"`. None is a call. |
| 3 | No vendored header in front of UHT | pass. No header declaring `UCLASS`/`USTRUCT`/`UENUM` includes `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`. |
| 4 | `TPimplPtr` for `FStratBridge` members | pass. There are two members, `StratMatchSubsystem.h` `Bridge` and `StratScoreboardHUD.h` `OwnedBridge`, and both are `TPimplPtr`. No `TUniquePtr<FStratBridge>` exists outside a comment. |
| 5 | Module arrows | pass. The derivation printed **16 rows** and every row matches the block. The census printed **10 names**, the known ten, including the `BRIDGE=` false positive. `StratRules` appears in `PrivateDependencyModuleNames.Add` only inside `if (!bCompileVendoredRulesHere)` (`StratBridge.Build.cs`, the `Target.LinkType != TargetLinkType.Monolithic` expression). All 12 `Vendored/*.strat.cpp` files still carry `STRAT_VENDORED_RULES_IN_BRIDGE`. No `.Build.cs` file is in the diff. |
| 6 | New modules registered | pass. No module was added and `Stratocracy.uproject` is unchanged. The three new `.claude/` directories are documentation and contain no `IMPLEMENT_MODULE`. |
| 7 | `Source/Stratocracy/` untouched | pass. `git diff --stat -- Source/Stratocracy` printed nothing. The one addition is the untracked `Source/Stratocracy/.claude/skills/stratocracy/SKILL.md`, a markdown file that UBT never reads, so it cannot create a cycle. See Observations. |
| 8 | No `/Game/` literal in C++ | pass. The only hit outside `Tests/` is `ImportStratDataCommandlet.cpp`, which is the standing exception. No changed line contains `/Game/`. |
| 9 | No widget-side arithmetic | pass. The only `StratUI` change is a comment in `StratSoundCues.h`. |
| 10 | Lane discipline | pass. The engineer touched production `Source/` and `engine.md`. The test author touched `Tests/StratBoardPicking.cpp` and `tests.md`. The steward touched `Tools/architect/` and its own record files. The coordinator touched `.claude/skills`, `.agents/`, `CLAUDE.md` and the `Source/*/.claude/skills` markdown, which no lane owns. No assets or config changed. |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass. `git diff --cached` is empty and HEAD is `a94c2c4`. |

### Checks I ran beyond the list

**The Source edits are comment-only. Confirmed, with controls.** I stripped comments and kept literals, then compared the token streams of HEAD and the working tree for all eight files: 7 in production and `Tests/StratBoardPicking.cpp`. All eight are **IDENTICAL**, with equal token counts and equal literal counts. To check the comparison can fail, I made three mutants: one token changed (`const`→`constexpr`), a space added inside a `TEXT("…")` literal, and a broken `/*` opener. All three reported **DIFFER**. The unmutated file again reported IDENTICAL. No added line ends in `\`, so no line splice is hiding code inside a comment. There is no behavioural change.

**The claims in the edited comments are true.**
- `BP_StratGameMode.uasset` and `BP_StratGameMode_AiVsAi.uasset` both contain the name `AiPlaybackStepSeconds`.
- `BP_StratUnit.uasset` contains `MoveTweenSeconds` and `DamageFlashSeconds`, and does not contain `AiPlaybackStepSeconds`, which serves as the negative control.
- `FStratGuidedOpening::Observe` reaches `SetLockedThisTurn` through `PublishLocks`.
- `StratBoardActor.h` declares four overlays.
- `StartMatchInternal` contains exactly one `TearDownPresentation();`.
- `d59bf9b^2` is `f53ca26`, and `283d711` is its ancestor.
- The line numbers taken out of the `GetMatchEpoch` header are all still in `engine.md`, in the *"THE FINDING, RE-MEASURED RATHER THAN TAKEN"* bullet.

**The doc split lost no load-bearing constraint.**
- Every block removed from `ue-project-context.md` is either carried whole in a module skill or kept as a one-line summary with a pointer:
  - StratPlay dependency strength, the 4 × `LNK2019` and `InputCore` → the `stratplay` skill.
  - DT_Units/DT_Terrain and the `stateHash` trap → the `stratocracy` skill.
  - Widget arithmetic → the `stratui` skill.
- All four pointer paths in the module→skill table exist.
- I spot-checked the new skills against the tree, and every claim below holds:
  - StratPlay's public and private arrows, and the absence of `InputCore`.
  - Enhanced Input headers are included only from `StratPlayerController.cpp`, with 6 forward-declared `TObjectPtr`s.
  - No StratPlay code names a `strat::` type.
  - `StratUI.Build.cs` has `NoPCHs` and `bUseUnity = false`, plus its "NOTE ON WHAT IS NOT HERE".
  - Both scoreboard headers forward-declare `FStratBridge` and include it only from the `.cpp`.
  - `StratBridge.h` contains "a later, separate file".
  - `FUnitRow` and `FTerrainRow` are in `StratDataRows.h`.
  - `Stratocracy.StratData.T-DATA-05.TerrainTableMatchesCsv` exists.
  - The vendored claims hold: the `SaveCommand` fields, `"THE CANONICAL STATE HASH IS NOT DEFINED HERE"`, and `sc.terrainId[i]`.
  - The `GATE-BRIDGE-DEFS` rationale is already corrected in `StratBridgeParity.cpp`.
- The corrected capture sentence in `ue-project-context.md` is true: there are 10 `FOutputDevice` captures under `Source/*/Tests/`, and all 10 files override `CanBeUsedOnMultipleThreads`.

**My own definition against the edited context file.** The module table still matches the 16-row derivation. The *"AND THAT COMPILE-IN IS CONDITIONAL"* subsection is still present. The new symbol anchor ("the comment above `StratBridge.cpp`'s `IMPLEMENT_MODULE` line") is correct.

**Gate reports.** There are 10 files in `E:\MultiAgent\briefs\2026-09-10-gate-*.md` and 10 in `Tools/architect/gate_reports/`. All 10 pairs are `cmp` IDENTICAL and their sha256 values match. None is missing. As a control, a copy with one byte appended reported DIFFER. None of the files is gitignored.

**New self-test fixture can fail.** I reverted the `LIVE COUNT MISSING` message to its old text in a scratch copy of the script. The self-test then marked the new "names BOTH causes" line `**WRONG**`, and the unmutated copy in the same location marked it `[OK]`. Three tree/branch fixtures also went `WRONG` in that scratch location, but they did so identically with and without the mutation, so the cause is the location, not the mutant.

**Tool runs** (from the repo root):
```
python Tools/architect/strat_banner_sweep.py
  automation report: 479 entries, all Success (2026.09.11-20.34.29)
  suite claims found: 75 (1 live, 74 stamped)
SWEEP CLEAN -- no self-contradiction found                         EXIT=0
python Tools/architect/strat_banner_sweep.py --self-test
  70 [OK] lines, 0 WRONG
SELF-TEST: ALL FIXTURES CORRECT                                    EXIT=0
python Tools/architect/strat_doc_citation_gate.py
  files scanned: 43 (25 documents, 18 tooling scripts)
  citations found: 56 (56 read as historical, 0 as live)
DOC CITATION GATE CLEAN                                            EXIT=0
python Tools/architect/strat_suite_report_gate.py --pin-to-tree
  created: 2026.09.11-20.34.29 (UTC)
  fresh:   newest file Source\StratPlay\Tests\StratBoardPicking.cpp at 2026.09.11-20.31.39 (UTC) predates the report
  479/479 clauses Success, and every one is a name the tree declares.
SUITE REPORT GATE CLEAN                                            EXIT=0
```

**Other record-prose rules hold.** No live N/N appears outside `global.md`. No added line cites a line number in a record file. The stamps sit at the sentences they correct. Nothing narrates this pass's own gating, commit or push.

## Findings

**1. `global.md` names two different writers for its own banner, and one of them is wrong.** This breaks the rule that acting and writing are attributed inline, which `CLAUDE.md` calls load-bearing: an entry that names the wrong writer is "an affirmative false claim about authorship and is a finding".
- File: `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md`, in the FIFTH PASS `_Last run 2026-09-11` banner at the top of the file.
- Offending text:
  > `Written by the `coordinator`, who ran this pass; each lane wrote its own record entry, dispatched one at a time.`
  > `` `strat-data-steward` writes this banner.``
- This pass's sequence gives the banner to `strat-data-steward` (step 5), and the banner agrees with itself further down: *"this steward's own eyes on the exported file"*. So "Written by the `coordinator`" says the wrong agent wrote it. The phrasing was carried over from the FOURTH PASS banner's template ("Written by the `coordinator`; each lane wrote…"). The result is that a reader who arrives at this entry by citation cannot tell who wrote it.
- The fix is to attribute each actor once: the `coordinator` as the one who acted and ran the pass, and `strat-data-steward` as the writer of the banner.
- Owner: **strat-data-steward**.

## Observations
1. **One false claim that the shipped game runs with an inert `AiPlaybackStepSeconds` survives the sweep.** `Source/StratPlay/StratMatchSubsystem.cpp`, in `RecenterCameraOnViewingSide`'s flag-check comment, says: *"why the inertness `AiPlaybackStepSeconds` ships with does not transfer to a synchronous verb."* The matching sentence in the header was fixed in this pass ("argues at length that its C++ default is inert"), but this one was not. It is not in the debt's enumerated sites and does not use the "C++ field default" phrasing the `.cpp` stamp covers, so no record claim is falsified by it. It is the same claim, though. `StratPlayerController.cpp`'s 2026-08-29 `CORRECTED` bracket still says *"at the shipped default this block was invisible"*. Owner: strat-gameplay-engineer, in a later pass.
2. **My own definition still cites `StratBridge.cpp:10-14`** (checklist item 6). Those lines are now include comments, and the startup-abort note has moved to the comment above `IMPLEMENT_MODULE`. The context file now anchors to the symbol; my definition does not. `strat_doc_citation_gate.py` cannot catch this, because `StratBridge.cpp` is neither a vendored unit nor a record subject. The definition has no lane owner, so this goes to the coordinator or the user.
3. **The doc-citation gate does not scan the four `Source/*/.claude/skills/*/SKILL.md` files.** `DOC_ROOTS` covers only the top-level `.claude`, and the hook's check on `Source/` only catches vendored line citations. I grepped all four files by hand and found no line citations. Checklist item 7's command also cannot see untracked files, which is how the new `Source/Stratocracy/.claude/` file passed it silently.
4. **The `stratplay` skill states a live debt in the present tense.** It says *"Three sound-bank sentences … were left then as an engine-lane debt."* That sentence will go stale in a skill document the moment the debt is discharged. The same skill also says *"across this module's headers"*, but the `.cpp` files and `StratUI/StratSoundCues.h` were edited too.
5. **The `stratocracy` skill overstates a distance.** It says *"hundreds of commands later"*, but the parity fixture is a 169-command log. It inherited "three hundred" from the old context text and the `StratBridgeParity.cpp` comment.
6. **Two small wording problems in `strat_banner_sweep.py` and `decisions.md`.**
   - The comment on the new `_LIVE_COUNT_MISSING_BY_STAMP` fixture says *"`_STAMP_WINDOW` characters separate the citation from the figure"*. In the fixture they sit about 30 characters apart, inside the window.
   - The new `[OK]` line says the number is "read from the constant itself, not a literal". The assertion cannot tell a hardcoded `220` apart from the derived value; it only fails once the constant changes.
   - The doc-migration entry in `decisions.md` says *"done by the `coordinator`"* next to *"makes no claim about whether it has happened"*.
7. **The "sha256-identical" claims in `tests.md` and `global.md` hold for the working copies only.** The ten reports are LF-only and `core.autocrlf=true`, so the committed blobs will match the originals, but a fresh checkout will write CRLF.
8. **The fourth-pass stamps in `global.md` do not name a writer.** They say only `[STAMPED BY THE FIFTH PASS, 2026-09-11]`.

VERDICT: BLOCK
