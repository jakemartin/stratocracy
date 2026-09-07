# Gate: player hand-back cue + camera recentre — strat-integration-reviewer

Run 2026-09-07 (local) against the UNCOMMITTED working tree over base `6d882a3` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

## Scope

`git status --porcelain` — 16 modified, 4 untracked, index empty. Twenty files:

- Code: `Source/StratPlay/StratMatchSubsystem.{h,cpp}`, `Source/StratPlay/StratSoundBank.{h,cpp}`,
  `Source/StratPlay/StratSoundDirector.h`, `Source/StratUI/StratSoundCues.{h,cpp}`.
- Tests: `Source/StratPlay/Tests/StratPlayerHandbackClauses.cpp` (new, untracked),
  `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp`,
  `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp`,
  `Source/StratUI/Tests/StratSoundCueClauses.cpp`.
- Assets: `Content/StratAudio/DA_StratSoundBank.uasset` (modified),
  `Content/StratAudio/MS_Strat_RoundChange.uasset` (untracked).
- Tooling: `Tools/editor/make_round_change_cue.py`, `Tools/editor/verify_round_change_cue.py`
  (both untracked, new).
- Record: `Tools/architect/state/{global,decisions,engine,tests,content}.md`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing. Authority is the manifests (`Source/StratRules/StratRules.manifest.json`, `Data/StratData.manifest.json`); neither is in the diff. |
| 2 | No `strat::` call outside `StratBridge/` / `StratRules/` | pass — the call-shaped grep (lowercase-initial identifier followed by an open paren, over `*.cpp` and `*.h` under `Source/`) filtered to exclude those two directories returned ZERO. CONTROL: the same pattern unfiltered returns hits in 20+ files (`StratBridge.cpp` 36, `StratBridgeQueryParity.cpp` 12, `StratCombatOutcomeParity.cpp` 14), so the instrument speaks. Loose `strat::` hits in the changed files are comment prose and type names only (`strat::GameState`, `strat::UiSnapshot`, `strat::SaveCommand`) — judged, not forwarded. |
| 3 | No vendored header in front of UHT | pass — an include-line grep for `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` and `Replay.h` across every `*.h` under `Source/` matches only inside `Source/StratBridge/StratBridge.h` and `Source/StratRules/*.h`. `StratMatchSubsystem.h` and `StratSoundCues.h` both declare reflected types and neither includes any of them. |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only members are `StratMatchSubsystem.h:2869 TPimplPtr<FStratBridge> Bridge;` and `StratScoreboardHUD.h:1253 TPimplPtr<FStratBridge> OwnedBridge;`. Zero `TUniquePtr<FStratBridge>` declarations anywhere in the tree. |
| 5 | Module arrows unchanged and correct | pass — the derivation over `Source/*/*.Build.cs` with comments stripped printed exactly SIXTEEN rows, including the conditional `StratBridge  PrivateDependencyModuleNames   StratRules`, which `StratBridge.Build.cs` guards under `if (!bCompileVendoredRulesHere)` — monolithic only, deliberate, not a finding. `StratUI  PublicDependencyModuleNames  Core,CoreUObject,Engine,StratBridge` — no `Stratocracy`, no `StratPlay`, no cycle. No `.Build.cs` is in the diff. The field census printed TEN names, matching the recorded set including the `BRIDGE=` false positive that is the tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="`. No eleventh name. |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory. `Stratocracy.uproject` unmodified; `StratRules` correctly still absent from `Modules`. |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing. |
| 8 | No `/Game/` literal in gameplay C++ | pass — every `/Game/` hit under `Source/` is inside a `Tests/` directory, inside `ImportStratDataCommandlet.cpp` (the authoring commandlet), or inside a comment forbidding the literal. Zero in the diff's production C++. The two new `Tools/editor/*.py` carry `/Game/StratAudio` legitimately — outside the rule. |
| 9 | No widget-side arithmetic | pass — the only `StratUI` production changes are `StratSoundCues.h` (one enumerator plus doc prose) and `StratSoundCues.cpp` (comment only). No widget file is in the diff. |
| 10 | Lane discipline | pass — all four test edits are under a `Tests/` directory; no production code in `Tests/`; no test code outside it. Assets confined to `Content/StratAudio/`. Record entries attributed per lane, see claim 8 below. |
| 11 | Prose blocks present | pass — the new `StratPlayerHandbackClauses.cpp` and both new `Tools/editor/*.py` open with why-first blocks citing `GATE-HANDBACK`, the minting ruling, and the route. Every new member in `StratMatchSubsystem.h` carries a declaration block. |
| 12 | Nothing staged or committed | pass — `git diff --cached --numstat` returned zero lines and `git ls-files --stage Content/StratAudio/MS_Strat_RoundChange.uasset` printed nothing, so the force-unstage held and the asset did not come back. |

## The eight inherited claims, re-derived against the tree

1. **Suite 468/468.** Not inherited: read from `Saved/AutomationReport/index.json` with the
   `utf-8-sig` codec. `reportCreatedOn 2026.09.07-15.59.11`, `succeeded 468`,
   `succeededWithWarnings 0`, `failed 0`, `notRun 0`, and the `tests` array is 468 long. The run
   covers THIS tree: all 37 clause-name string literals declared across the four touched test
   files are present in the report's set of `fullTestPath` values, with zero missing. The
   arithmetic 456 + 12 = 468 is consistent with a macro census of 9
   `IMPLEMENT_SIMPLE_AUTOMATION_TEST` in the new file plus 3 added by diff across the other three.
   I did not run the build or the suite; this is the exported artifact, read.
2. **Latch resets ABOVE each `EndAiPlaybackTour()`.** Verified in `StratMatchSubsystem.cpp`: in
   `Deinitialize`, `bPlayerHandbackPending = false;` immediately precedes `EndAiPlaybackTour();`;
   in `TearDownPresentation`, likewise. The reasoning was re-derived rather than accepted:
   `EndAiPlaybackTour` ends with a call to `NotePlayerTurnBeganIfDue()`, whose guards are
   `bPlayerHandbackPending`, `IsAiPlaybackRunning()`, `IsMatchLive()` and
   `StratHandsBackToPlayer(AppliedModel, ActiveConfig.AiSides)` — and at both sites `Bridge` and
   `AppliedModel` are still live (both cleared later in the same function), so every guard would
   pass and the cue plus the camera would fire for a match being destroyed. Below the call this
   would be a live defect. It is above.
3. **Camera above and OUTSIDE `if (FindSoundDirector())`.** Verified. In
   `NotePlayerTurnBeganIfDue`, `RecenterCameraOnViewingSide();` is a bare statement at function
   scope; the `if (UStratSoundDirector* const Director = FindSoundDirector())` block follows it
   and contains only the `Director->EmitCue(...)` call. This is the inverse of the `4a01418`
   defect, as claimed.
4. **Fires once per hand-back, never on match end, no double-raise, no early consume.** Re-derived.
   Raised at exactly one site — `RunAiTurnsNow`, on the line above `BeginAiPlayback()`.
   `RunAiTurnsNow` refuses re-entry on `bAiTurnRunning` before taking
   `TGuardValue<bool> ReentrancyGuard(bAiTurnRunning, true)`, so no two loops interleave.
   Consumed unconditionally on every non-deferring path in `NotePlayerTurnBeganIfDue`, including
   the `!IsMatchLive()` and `!StratHandsBackToPlayer` returns, so nothing is left standing to be
   spent later. `StratHandsBackToPlayer` is
   `!Model.Match.bHasResult && !AiSides.Contains(Model.Match.SideToMove)`, so a match-ending
   transition consumes without emitting.
   **The one sub-claim I refused to take on trust is `EndAiPlaybackTour`'s own comment that the
   pre-refill caller in `RunAiTurnsNow` "arrives with the flag down".** For it to arrive raised, a
   player command would have to reach `RunAiTurnsIfDue` while a tour is still running. It cannot:
   `AStratPlayerController::HandleSelectionEvent` calls `Match->SkipAiPlayback()` and returns
   `true` AHEAD OF EVERY OTHER GATE, and `RequestEndTurn` routes through `HandleSelectionEvent`
   rather than around it — so the first input during a tour stops the tour (spending the flag
   through `EndAiPlaybackTour`) and is consumed. Had it not, that path would have sounded a cue
   and moved the camera at the start of a NEW AI run. The comment is correct, and it is correct
   because of a property in a different file; recorded here so the next gate need not re-derive it.
5. **Both `static_assert`s byte-identical.** Verified structurally: the diff on
   `Source/StratUI/StratSoundCues.h` touches lines ~1, ~35-98 and ~353-394 only, and the asserts
   sit at lines 226 and 239 — outside every hunk, therefore untouched bytes. They remain
   `ButtonClick == 0` and `MatchEnded + 1 == Count`, and both still HOLD under the new placement,
   because `PlayerTurnBegan` was inserted between `TurnEnded` and `UnitMoved` and not after
   `MatchEnded`.
6. **`FindCameraPawn` extracted and `FocusPlaybackStep` rewritten onto it in the same pass.**
   Verified in one diff hunk: the world to controller to pawn to cast chain is REMOVED from
   `FocusPlaybackStep`, which now reads `AStratCameraPawn* const Camera = FindCameraPawn();`, and
   ADDED as the body of the new `FindCameraPawn`. No intermediate state with two copies exists in
   the diff. Neither new member is a `UFUNCTION`, and `class AStratCameraPawn;` is forward declared
   at `StratMatchSubsystem.h:139`, so the pointer return type puts nothing new in front of UHT.
7. **Mutant residue.** None found. A grep for `MUTANT`, `TODO`, `XXX` and `HACK` across the four
   changed production files returns nothing outside prose discussing mutants as instruments.
   Corroborating: the suite is 468/468 with zero failures, which a live mutant in any of the four
   would not survive. I cannot verify the claimed sha256 revert byte-for-byte from a checkout that
   holds no pre-mutation copy; what I can state is that the production files carry no mutation.
8. **Record ownership.** Holds. `global.md` is the only file whose added lines carry a suite figure
   (`THE SUITE IS **468/468**`) or a phase verdict; the same greps over `content.md`,
   `decisions.md`, `engine.md`, `tests.md` and `data.md` returned only the English word "pass"
   inside the phrase "ON THE SAME PASS". `engine.md`'s new entry opens
   `2026-09-07, strat-gameplay-engineer (ACTING and WRITING; IN LANE, on master in the main ...`;
   `tests.md`'s opens `2026-09-07 (local), strat-test-author (ACTING and WRITING; IN LANE -- ONE
   new Tests/ ...`. `content.md`'s entry names BOTH authorities, one per half, inline:
   *"ACTING: the `coordinator`, under `CLAUDE.md`'s EDITOR-DRIVER CLAUSE. WRITING: the
   `coordinator`, under THIS FILE'S FALLBACK CONDITION. Two authorities, one for each half, as
   this file's header requires; neither covers the other."* It measures the precondition at the
   LANE AGENT'S TOOL SURFACE with a control (`unreal_status` and `list_unreal_projects` returned
   with schemas; both `execute_script` names did not), names the route (a headless
   `UnrealEditor-Cmd.exe -run=pythonscript` commandlet with no editor running), and names the base
   commit. That is what both clauses require, and the two halves are not conflated.

## Findings

None.

## Observations — non-gating

- **`Tools/editor/` has no declared owning lane.** `CLAUDE.md` assigns `Config/` and
  `Tools/architect/` to the steward, `Content/` to the editor-builder, `Source/` outside `Tests/`
  to the engineer, and `Tests/` to the test-author. `Tools/editor/` is named in none of them, and
  this pass added two files there. They are plainly part of the editor-driver route and
  `content.md` records both by name, so nothing is hidden — but the directory's ownership is
  INFERRED rather than declared, which is the shape that has cost gates here before. Worth one
  line in `CLAUDE.md` the next time that file is opened. It is not a defect in this tree.
- **`Saved/AutomationReport/` is a single export path.** The 468/468 figure above was read from an
  artifact the next headless run overwrites in place, so it is a measurement of a moment and not a
  durable citation. `global.md` already stamps this; it is repeated here because this report cites
  that artifact and a reader is entitled to know the citation is perishable.
- **`EndAiPlaybackTour`'s "six call sites" figure is prose, not a derived count.** I did not
  enumerate the six; what I checked is the narrower claim that the pre-refill caller arrives with
  the flag down, which holds (claim 4 above). If a seventh site is added the paragraph's
  arithmetic goes stale silently — the CODE stays correct, because the beat rides the single call
  inside that function rather than being restated per site, which is that design's whole point.
- **The owed human observations are correctly declared and remain owed**: that the cue is audible,
  that the recentre reads well, and — carried from `6d882a3` — that the guided opening and the
  damage alert have never been seen by anyone. A commandlet has no audio device and no headless
  fixture composites a viewport. One PIE playtest closes all four.
- **`Side` vs `ViewingSide` is genuinely unpinnable, as declared.** `StratHandsBackToPlayer` gates
  on `Match.SideToMove` being a human seat, and on the shipped single-player configuration that
  side IS `ViewingSide`, so no fixture can construct a state where the camera's field and the cue's
  field disagree. Both sites document the divergence in prose at their own end, which is the only
  instrument available and is the right one.
- **The camera-above-the-director-lookup placement is likewise unpinnable, as declared.**
  `UStratSoundDirector::DoesSupportWorldType` gives every Game and PIE world a director, so no
  reachable world has a camera pawn and no director, and the `4a01418`-shaped mutant is invisible
  to any clause. `NotePlayerTurnBeganIfDue` says so in terms at the site, which converts the
  hazard into a MOVE in a diff rather than one level of indentation — that is the available
  protection and it was taken.

VERDICT: PASS
