Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `525ad5c` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: options-exit mutant campaign, record pass (RE-GATE) — strat-integration-reviewer

## Scope
Working-tree diff against base `525ad5c`, `master`, in `E:\MultiAgent\Stratocracy`. Six modified files, nothing staged, nothing committed: `Source/StratUI/Tests/StratShippedOptionsCaptionPresence.cpp`, `Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`, and `Tools/architect/state/{content,engine,global,tests}.md` — 588 insertions, 48 deletions. Every check below was re-run against the current tree; nothing is carried forward from the earlier BLOCK. I also re-verified the two `.cpp` diffs rather than accepting the standing result, and corroborated the campaign's per-clause claims against the twelve exported reports in `Saved/MutantReports/`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — the lowercase-initial call pattern across `Source/` returns zero hits outside `StratBridge/` and `StratRules/` |
| 3 | No vendored header in front of UHT | pass — the only `#include "Data.h" / "Replay.h" / "Save.h" / "Scenario.h" / "Ui.h"` hits are in `Source/StratBridge/StratBridge.h:53-58`, which declares no `UCLASS`/`USTRUCT`/`UENUM`; the anchored pattern returns 0 there and 1 on `StratScoreboardHUD.h` as a control. The grep matched only comment prose naming the macros |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — no `TUniquePtr<FStratBridge>` in code; the two live members are `StratMatchSubsystem.h:2869` and `StratScoreboardHUD.h:1253`, both `TPimplPtr` |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. **16 rows**, matching the block row for row. The 16th (`StratBridge PrivateDependencyModuleNames StratRules`) is inside `if (!bCompileVendoredRulesHere)` at `StratBridge.Build.cs:135-138`, guarded by the single expression at line 133 — permitted, not a finding. Field census prints **10** names; `BRIDGE=` is the known false positive, the tail of the literal `"STRAT_VENDORED_RULES_IN_BRIDGE="` at line 134. No eleventh name. No cycle; `StratUI` has gained no `Stratocracy` dependency |
| 6 | New modules registered | pass — no new `Source/<Module>/` in the diff. `StratRules` still carries no `IMPLEMENT_MODULE` and is still correctly absent from `Stratocracy.uproject` |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass — every hit is under a `Tests/` directory (the standing fixture exception); none in the diff |
| 9 | No widget-side arithmetic | pass — no `StratUI` production file is touched; the only `StratUI` changes are under `Tests/` and are comment-only |
| 10 | Lane discipline | pass — engineer→`engine.md` only, editor-builder→`content.md` only, coordinator→`global.md` only, test-author→`tests.md` + two files under `Source/StratUI/Tests/`. No production code in `Tests/`, no test code outside it, `Content/` byte-identical |
| 11 | Prose blocks, measurements recorded as measurements | **BLOCK** — see Finding 1 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries and nothing in the index |

## Findings

**Finding 1 — a measurement in `global.md` that the artifacts it describes falsify.** Constraint: check 11, measurements recorded as measurements; and the house rule that when a note disagrees with the tree, the tree wins.

`Tools/architect/state/global.md:71`:

> `Build.bat` and a full suite run, then reverted; **the campaign ran the suite eleven times.** Both

It ran **twelve** times. `Saved/MutantReports/` holds twelve report directories, each with a distinct `reportCreatedOn` and a distinct result set:

| run | stamp (UTC) | red |
|---|---|---|
| baseline | 02.29.26 | none |
| M1 | 02.33.34 | two clauses |
| M2 | 02.39.32 | tracks-clause alone |
| M3 | 02.44.20 | refusal clause alone |
| M4 | 02.46.12 | two clauses |
| M5 | 02.48.04 | volume-drag alone |
| M6 | 02.49.59 | unpushed alone |
| M7 | 02.58.45 | broadcast alone |
| **restored** | **03.01.58** | **none — the run "eleven" omits** |
| M8_rename | 03.15.05 | `ShippedOptionsWidgetCarriesReturnToTitleButton` alone |
| M9_backlabel | 03.19.12 | `ShippedOptionsWidgetCarriesItsUnboundCaptions` alone |
| final | 03.26.17 | none |

Nine mutants plus **three** controls, not two. The undercount is exactly `restored` — the engine half's trailing control, which `engine.md:9904` counts correctly ("NINE times — a baseline control, seven engine-lane mutants, and a final control" = baseline + M1–M7 + restored), and which the campaign-wide total then drops. The adjacent sentence "**Both** controls reported zero non-success" carries the same arithmetic.

Everything else in that block is corroborated: all nine per-clause results match the reports exactly, including which two mutants reddened two clauses; the three controls are genuinely zero-failure; and `Content/UI/WBP_Options.uasset` is `sha256 2beba…4667`, identical to the LFS oid at `525ad5c`.

Discharges it: correct `eleven` to `twelve` and `Both controls` to three, or scope the sentence to the two campaign-wide controls explicitly. Owner: the `coordinator` + steward (`global.md` is theirs).

## Observations

Non-gating.

- **Your Finding 1 is discharged at the false sentence.** `engine.md` no longer contains "the standing report's"; the citation now reads "**the report that was standing when this dispute arose, SINCE SUPERSEDED**", keeps the 23.39.55 UTC / 19:39:55 local arithmetic that was the point of the citation, and refers the live identity to `global.md` without restating it. No equivalent claim survives elsewhere in that file — the other 20 hits on "standing" are unrelated senses. The file states no suite figure and no phase verdict.
- **Item (a), checked rather than accepted.** "Neither writer was wrong at the moment of writing" is *corroborated but not provable from a checkout*. The corroboration: the campaign's twelve runs are stamped 02.29–03.26 UTC and live in `Saved/MutantReports/`, while every `AutomationReport` identity the records cite forms a separate ascending chain — 21.01.50, 21.25.09, 23.24.24, 23.39.55, then 03.43.53 — with **no campaign stamp in it**. That is consistent only with the mutant runs having been exported elsewhere, leaving `Saved/AutomationReport/index.json` at 23.39.55 throughout the campaign and moved only by the final re-run. So the claim survives the strongest test this tree affords. The last step — which agent wrote which sentence when — is a session fact no gate reads, and the block does not pretend otherwise.
- **Item (b) verified against the report.** `Saved/AutomationReport/index.json`, `utf-8-sig`, `reportCreatedOn 2026.09.08-03.43.53`: `succeeded=476`, `succeededWithWarnings=1`, `failed=0`, `notRun=0`, `len(tests)=477`, and every one of the 477 entries has `state == "Success"`. 476 + 1 = 477. The "off by one" was a misread and the record now says so correctly, in both the new banner and the 2026-09-07 one. The false claim was made in session and never persisted, so there is no false sentence in the tree for a reader to land on; stating the correction in the live banner is the right place available.
- **Item (3) verified independently.** `strat_banner_sweep.py` reports `suite claims found: 69 (1 live, 68 stamped)` and `SWEEP CLEAN`. Exactly one live count claim.
- **But the live REPORT IDENTITY is now asserted twice, unstamped, in two banners.** `global.md:39` (new banner) and `global.md:109-110` (the 2026-09-07 banner, edited in place) both read "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.08-03.43.53`". The sweep does not object — it takes the live figure from the newest banner — and the `[STAMPED BY THE MUTANT CAMPAIGN]` block at line 112 does announce the supersession from the sentence it corrects. The debt is that the next re-run must now move the identity in two places, and a historical banner now names a report that postdates it. Worth collapsing line 109-110 to the "NAMED IN THE BANNER ABOVE THIS ONE" form this file already uses at line 595.
- **A surgical-edit seam in `global.md:116`.** The stamp block's last line runs live prose onto a stamped line: `[STAMPED] tree it describes. THE COUNT DID NOT MOVE; only the report backing it did. The one warning is a headless `no tile mesh for terrain``. The `The one warning is…` sentence is live text about the current run and now sits behind a `[STAMPED]` marker on an over-long line. No claim is falsified and the sweep is unaffected; it should start a fresh line.
- **`content.md` read for damage — none found.** No truncation, no duplicated adjacent lines, no orphan bullet marker, no sentence stopping mid-clause, backticks balanced throughout the new entry. The three over-long lines are 56 (101 chars), 109 (103) and 125 (131) — matching the author's own count, and cosmetic. Two claims inside it are independently true: the file is UTF-8 with `# Stratocracy st` in the clear at byte 0 (no BOM, no UTF-16 widening), and `IyBTdHJhdG9jcmFj` decodes to exactly `# Stratocrac`. The restore claim holds against the LFS blob, not the worktree: `git show HEAD:Content/UI/WBP_Options.uasset` gives `oid sha256:2bebaccbbd291a0351069a51354983903150631b113c923f4697a28293de4667`, `git diff -- Content/` is empty, and the worktree file hashes to the same value.
- **One scope over-statement in `content.md:71`.** It says of Mutant A that "no `duplicate_widget` and no `remove_widget` were used, deliberately — that pair is what destroyed the original `BackLabel`… and the standing instruction there forbids it." The standing instruction at `content.md:154` reads **"do not run `duplicate_widget` on a subtree with children"** — it names one verb, not the pair. Mutant B nine lines later deliberately uses `remove_widget("BackLabel")`, which that instruction does not forbid. The entry is recoverable on a careful read ("that pair" meaning the two used together), but a reader landing on line 71 would think the instruction was broken by the next paragraph. This is the "a ruling forbids less than it is cited for" shape; naming the mechanism rather than the pair would fix it.
- **The two `.cpp` diffs re-verified fresh, with a control.** 43+/20− and 33+/14−; after stripping added and removed lines whose first non-space characters are `//`, `*` or `/*`, **zero** lines remain, while the unfiltered form prints them — so the filter is not silently empty. Comment-only, confirmed.
- **Date discipline is clean.** `2026-09-08` appears exactly once in the whole of `Tools/architect/state/`, in the `global.md` banner at line 14. `content.md`, `engine.md`, `tests.md`, `data.md` and `decisions.md` carry zero instances — no lane file is dated ahead of the banner, and `strat_banner_sweep.py` raises no BANNER DATE FRESHNESS. The UTC/local offset is consistent with the artifacts: M8 at `03.15.05` UTC and M9 at `03.19.12` UTC sit just after the "roughly 23:11 and 23:16 local" planting times `content.md` records, and the live report's `03.43.53` is the 23:43 local run `global.md` names.
- **The new banner narrates the gating of the changeset it is part of** — "THE GATE BLOCKED ON TWO PROSE FINDINGS… the fourth consecutive pass whose only findings are prose". That is a claim about a gate's outcome, written into the tree the gate reads, and this re-gate had to be run to know whether it was still true. It survives: my one finding is also prose. It is worth knowing that the sentence is the kind that can be falsified by the next gate rather than by any edit, and this file already carries the standing lesson that a record should not narrate its own gating.
- **Lane topology, since the in-session clause turns on it.** `git worktree list` shows `Strat-wt/slot-1` and `slot-2`, but both sit on `feat/w8-*` branches last committed 2026-08-31 and **both are already merged into `master`** — fossils, not a standing wave. The clause's condition holds, and the entries in `engine.md` and `tests.md` discharge its obligations (2), (3) and (4): the receiving file gets the entry, the lane agent writes it, acting and writing are attributed inline, and base `525ad5c` is named. Obligation (1) — that both routes were offered — remains unfalsifiable from a checkout, exactly as `CLAUDE.md` declares.

VERDICT: BLOCK
