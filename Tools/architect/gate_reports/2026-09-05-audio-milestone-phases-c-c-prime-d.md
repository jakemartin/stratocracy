# Gate: audio milestone phases C, C-prime, D -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`, `HEAD = 089c79c`
(`git rev-parse HEAD`). Nothing committed, nothing staged. 17 tracked files modified, 27
untracked paths present (15 `.uasset` under `Content/StratAudio/`, 9 `Source/` files, 2 prior
gate reports, 1 `Tools/editor/` script). The FULL standing twelve-item checklist was re-run over
the whole tree, not only the delta since the phases A-prime/A/B gate, because a verdict describes
a tree and this tree has moved.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing, and neither path appears in `git status --porcelain`. The manifests at `Source/StratRules/StratRules.manifest.json` and `Data/StratData.manifest.json` are unmodified, so the `rulesCommit`/`dataCommit` gate is untouched by this pass. |
| 2 | No `strat::` call outside `Source/StratBridge/`, `Source/StratRules/` | pass -- `grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*(" Source/` filtered of those two directories returned ZERO lines. No hit to judge this pass; the new audio C++ routes nothing through the vendored surface. |
| 3 | No vendored header in front of UHT | pass -- every `.h` under `Source/` containing `UCLASS`/`USTRUCT`/`UENUM` was tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`. One file matched, `Source/StratBridge/StratBridge.h:53-58`, and it is NOT a violation: grepping that file for the macro-invocation forms returns nothing -- the macro names occur in its prose only. The three new headers (`StratSoundBank.h`, `StratSoundDirector.h`, `StratSoundCues.h`) include no vendored header. |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two members exist, `Source/StratPlay/StratMatchSubsystem.h:2538` and `Source/StratUI/StratScoreboardHUD.h:1235`, both `TPimplPtr<FStratBridge>`. Zero occurrences of `TUniquePtr<FStratBridge>` anywhere in `Source/`. |
| 5 | Module arrows unchanged and correct | pass -- derived, not trusted. The extraction printed 16 rows, including the conditional `StratBridge PrivateDependencyModuleNames StratRules`; the guard is intact at `Source/StratBridge/StratBridge.Build.cs:133-137` (`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` then `if (!bCompileVendoredRulesHere) { PrivateDependencyModuleNames.Add("StratRules"); }`), so the row is monolithic-only and is NOT drift. `StratRules` appears in no `PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. The field census printed TEN names, matching the recorded set exactly (`BRIDGE=` is the known string-literal false positive from `PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" ...)`); no eleventh name. `Source/StratPlay/StratPlay.Build.cs` is modified but the diff is +18 lines of comment only -- a new NOTE ON WHAT IS NOT HERE bullet arguing the absence of `MetasoundEngine` -- and no dependency array changed. `StratUI` has gained no `Stratocracy` dependency; no cycle. |
| 6 | New modules registered | pass -- no new `Source/<Module>/` directory. `IMPLEMENT_MODULE` occurs in `StratBridge.cpp`, `StratPlay.cpp` and `StratUI.cpp` only, and all three are in the `Modules` array of `Stratocracy.uproject` alongside `Stratocracy`. `Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly ABSENT from that array. |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing and no untracked path is under it. |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- see Finding-free note below the table. |
| 9 | No widget-side arithmetic | pass -- no `StratUI` widget file was touched by phases C/C-prime/D; the module's only new file is a world-free decider (`StratSoundCues.*`), not a widget. Re-scanned the module's `.cpp` files for arithmetic inside `SetText(`/`Format(`/`AsNumber(`: the four hits are `StratScoreboardWidget.cpp:55,92,93,195`, each passing a view-model field straight through a formatter with no operator applied. |
| 10 | Lane discipline | pass -- phase D touched exactly `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp` plus `tests.md`; phase C-prime touched exactly `Config/DefaultEngine.ini` plus `data.md`/`decisions.md`; phase C touched `Content/StratAudio/`, three `Content/StratPlay/*.uasset`, `Tools/editor/make_strat_sound_bank.py` and `content.md`. No test code outside `Tests/`, no production code inside `Tests/`, no asset touched by a source lane. `Tools/editor/` is judged in Observations. |
| 11 | Prose blocks present, measurements recorded as measurements | **BLOCK** -- the new source file carries a full why-first block citing its debts verbatim, and the `StratPlay.Build.cs` addition is exemplary (it labels itself AN ARGUED ABSENCE AND NOT A MEASURED ONE and contrasts itself with the `InputCore` note's two quoted `LNK2019`s). The failure is in the record: the live banner in `global.md` states two measurements its own later paragraphs falsify, and the engine-source trace in `data.md` states two enumerations the engine source falsifies. Findings 1-3. |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` shows a blank first column on every line (` M`, `??`); `git diff --cached --stat` printed NOTHING. The auto-staging provider that captured assets three times in this milestone has not re-captured anything since both prior lanes force-unstaged. `HEAD` is still `089c79c`; nothing was committed. |

**Item 8, in full, because the brief asked for it specifically.** `grep -rn "/Game/" Source/`
filtered of `Tests/` returns 41 lines; 40 are inside comment prose arguing the rule, and the
single executable one is `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`
(`FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName)`), the standing import-commandlet
exception. The new `Content/StratAudio/` territory is reached from `Source/` in exactly three
places, all inside the automation-fixture exception and legitimately so:
`Source/StratPlay/Tests/StratShippedSoundBankParity.cpp:147` and `:152`
(`/Game/StratAudio/SCL_Strat_SFX...` and `.../SCL_Strat_Music...`) and a diagnostic string at
`:860`. That file is a fixture, the class asset IS the expectation (nothing module-side names a
sound class, by design), and it is loaded as an object and compared by identity rather than by
string. No gameplay `.cpp` or `.h` names the bank, a cue, a class, a submix or the mix -- the
bank reaches the engine only through the three Blueprint defaults.

## Findings

**1. `Tools/architect/state/global.md:36-37` -- the live banner misidentifies the report backing
the live suite figure, contradicting itself seven lines earlier. Owner: the `coordinator`.**

Line 29, of the stamp `2026.09.05-13.12.09`:

> THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.05-13.12.09`, made after the `Config/`
> change below.

Lines 36-37, of the same stamp:

> run landed on local 2026-09-04, as `engine.md`, `tests.md` and the steward's ruling below all
> independently date it. It is the phase-B run and it overwrote two earlier reports in place

Both cannot hold. The phase-C-prime entry in `data.md` records that run as the one made AFTER the
`Config/DefaultEngine.ini` edit on local 2026-09-05, and `tests.md` pins phase B to
`2026.09.05-04.55.48` and phase D to `2026.09.05-13.03.00`. The UTC-versus-local paragraph and
the phase-B identification are true of a SUPERSEDED stamp and have been carried onto the current
one. This is a false provenance sentence attached to the only live suite figure in the project,
in the only file permitted to carry one.

**2. `Tools/architect/state/global.md:44` -- the live banner asserts, in the present tense and
unstamped, that the assets the same banner announces do not exist. Owner: the `coordinator`.**

> a `UStratSoundBank` `UDataAsset` carried on two Blueprint defaults. NO SOUND ASSET EXISTS YET
> and none is set

Line 76 of the same banner:

> `DA_StratSoundBank` now exist under `/Game/StratAudio`, and all three shipped GameMode
> Blueprints name the bank

Fifteen assets exist in the tree and three Blueprints name the bank, so both halves of line 44
are false of the tree at the moment the line was written. The "two Blueprint defaults" count is
also the two-versus-three trap that `StratShippedSoundBankParity.cpp`'s own header block warns
about in terms -- two C++ CLASSES, three shipped ASSETS, because `BP_StratGameMode_AiVsAi` is a
second asset of the first class. The banner's own convention for a claim that has become history
is a bracketed STAMPED note, applied correctly to the 389/389 figure a few lines below in this
same diff and not applied here. A reader arriving at line 44 by citation lands on a false
present-tense claim with no marker.

**3. `Tools/architect/state/data.md:49` and `:51` -- the engine-source trace states two "exactly
one place" enumerations that the engine source falsifies. The CONCLUSION survives; the stated
measurement does not. Owner: `strat-data-steward`.**

> **That map is read in exactly one place**, `FAudioDevice::ApplyClassAdjusters`

> and `ApplyClassAdjusters` is itself called from exactly one place, the per-tick loop over
> `SoundMixModifiers`

Measured against `UE_5.8/Engine/Source/Runtime/Engine/Private/AudioDevice.cpp` on this box:
`SoundMixClassEffectOverrides` is touched at `:943`, `:2942`, `:3031`, `:3111`, `:3860`, `:3913`
and `:3960`, and `:2942` is a genuine READ inside `FAudioDevice::ClearSoundMix` -- it looks the
map up to reset `bOverrideApplied` on teardown -- so "read in exactly one place" is false as
written. `ApplyClassAdjusters` has TWO callers, `:3236` (the `SoundMixModifiers` tick loop, as
stated) and `:2713` inside `FAudioDevice::ApplySoundMix`, which runs when a mix is activated.

The rest of the trace verifies exactly as written: `UGameplayStatics::SetSoundMixClassOverride`
is at `GameplayStatics.cpp:2052` and forwards to `FAudioDevice::SetSoundMixClassOverride` at
`AudioDevice.cpp:3839`, which does nothing but `SoundMixClassEffectOverrides.FindOrAdd(InSoundMix)`
at `:3860`; `SetBaseSoundMix` is at `:3735`; `UAudioSettings::DefaultBaseSoundMix` is loaded once
in `Init` at `:516-519` and reloaded by `SetDefaultBaseSoundMix` at `:2452-2474`. The second
`ApplyClassAdjusters` caller does not disturb the conclusion, since `ApplySoundMix` runs only as
a mix BECOMES active, so a mix absent from `SoundMixModifiers` still never has its overrides
applied. I independently confirmed the premise the conclusion rests on: grepping `Source/` for
`PushSoundMixModifier`, `SetBaseSoundMix` and `SetSoundMixClassOverride` returns ZERO lines, so
nothing in this tree pushes a mix and `DefaultBaseSoundMix` is indeed the only route today.

The key itself is correctly scoped and this gate does not block on the `Config/` edit. One key,
one new section, appended between two existing blocks; `DefaultSoundClassName` and
`DefaultSoundConcurrencyName` were declined on a real and unbounded blast radius (they would
capture every `AdvancedTurnBasedTileToolkit` sound this project did not author), the refusal is
recorded in `decisions.md` as a scope call, and the applied key is inert today by the steward's
own reasoning. The finding is that a measurement stated in the strongest available form --
"exactly one place", twice -- is checkable and wrong, in the entry a later phase is told to lean on.

## Observations

Non-gating.

- **Phase C's authority is cited correctly, and this was checked rather than accepted.** The new
  entry in `content.md` names the two authorities separately and inline -- "ACTING: the
  `coordinator`, under `CLAUDE.md`'s EDITOR-DRIVER CLAUSE. WRITING: the `coordinator`, under THIS
  FILE'S FALLBACK CONDITION" -- and does not lean one on the other. The absence is measured with
  a control in the required shape: one `ToolSearch` lookup in which two `execute_script` names
  came back ABSENT while `unreal_status` and `list_unreal_projects`, from the SAME server in the
  SAME lookup, came back SERVED. The entry also records the editor's own state (no editor at all;
  `runtimes.json` fossils carrying `mcpRunning:true` on dead PIDs) while saying explicitly that
  the editor's state is NOT the condition -- which is the distinction the 2026-09-04 route ruling
  turns on. The route taken (headless `-run=pythonscript` commandlet) is named, as that ruling
  still requires.
- **Record ownership across five files holds.** `strat_banner_sweep.py` exits 0 with `SWEEP
  CLEAN`, `automation report: 413 entries, all Success (2026.09.05-13.12.09)`, `macro census:
  IMPLEMENT_SIMPLE_AUTOMATION_TEST=413, COMPLEX/_CLASS=0`, and `suite claims found: 63 (1 live,
  62 stamped)`. This gate re-ran it rather than accepting the reported exit code. `content.md`
  states no figure and says so in terms; `tests.md` cites its pass by `reportCreatedOn` and says
  the figure lives in `global.md` and nowhere else; the `413` figures in `data.md` are
  report-attributed rather than live, which the sweep's 62-stamped count is the mechanical
  instrument for. The banner claims no verdict and narrates no gating -- checked; the only
  occurrences of "gate" in the new banner are `GATE-AUDIO` and a slide gate.
- **The audibility discipline is honest everywhere this gate looked.** `content.md`: "NOT
  AUDIBLE, AND NOTHING HERE CLAIMS OTHERWISE ... A commandlet has no audio device". `tests.md`:
  "Not that anything is AUDIBLE: `Played` means `PlaySound2D` was called and returned, and under
  `-nullrhi` there is no device." `global.md`: "STILL NOT AUDIBLE, AND NOTHING HERE CLAIMS
  OTHERWISE." The script's own header: "WHAT THIS SCRIPT CANNOT PROVE: THAT ANY OF IT IS
  AUDIBLE." No sentence was found in the record that could be read as claiming a sound has been
  heard.
- **`Tools/editor/make_strat_sound_bank.py` belongs where it is, and one line in it will rot.**
  The directory already holds `make_scoreboard_wbp.py` by the same actor for the same reason, so
  the placement is precedent rather than a new claim on unowned ground. Its two caveats check out
  against its own behaviour: the `unreal.log` claim is labelled MEASURED with a control (a probe
  reporting only through it printed "Python script executed successfully" in 0.14 s and emitted
  none of its own lines), and the file-reporting discipline it forced is what caught the real
  defect -- `create_float_meta_sound_literal` returning a 2-tuple, defended at the `setf` helper
  with the nativizer's own error quoted. The `build_to_asset` caveat is stated in the script as a
  bare assertion; `content.md` supplies the control the script's header omits, an empty source
  built by the same call path whose `build_to_asset` returned `SUCCEEDED` identically at 28,886
  bytes against 59,143-78,189 for the seven real ones. THE ROT: `REPORT_PATH` at line 35 is a
  hardcoded absolute `E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/report.txt`, so a run from
  any of this project's seven worktrees writes its report into the MAIN tree and a reader would
  attribute it to the wrong checkout. `unreal.Paths.project_saved_dir()` is the fix. The `UE_5.8`
  path in the ROUTE comment is documentation and does not execute.
- **The phase-D citation `reportCreatedOn 2026.09.05-13.03.00` in `tests.md` can no longer be
  opened** -- phase C-prime overwrote the single export path nine minutes later. `global.md`
  discloses this ("It supersedes ... all of which are gone"), so the record as a whole is honest;
  the citation is simply no longer resolvable, which is a standing property of a single-file
  export and not this pass's defect.
- **The phase-B entry in `tests.md` carries a present-tense claim that phase C falsified the next
  day:** "No `UStratSoundBank` asset exists in this project". It was true when written and sits
  under `## NEXT`, where that file's own header says an entry that has become history is stamped
  rather than deleted. Not gating and not that lane's fault -- phase C is what falsified it --
  but it is the same class as Finding 2 and could be stamped in the same pass.

VERDICT: BLOCK
