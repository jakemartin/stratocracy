# Gate: audio milestone phases C, C-prime, D -- RE-GATE -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`, `HEAD = 089c79c`
(`git rev-parse HEAD` -> `089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`). Nothing committed, nothing
staged. 17 tracked files modified, 28 untracked paths (the 27 of the prior gate plus that gate's
own report). The FULL standing twelve-item checklist was re-run over the whole tree, not only the
delta, because a verdict describes a tree and this tree has moved.

**The delta since the prior gate was measured, not assumed.** Instrument, run from the repo root:

```
find Source Content Config Tools/editor Stratocracy.uproject -newer \
  Tools/architect/gate_reports/2026-09-05-audio-milestone-phases-c-c-prime-d.md -type f
```

printed NOTHING, and the same predicate over `Tools/architect/state` printed exactly two paths:
`Tools/architect/state/data.md` and `Tools/architect/state/global.md`. So no `Source/`, `Content/`,
`Config/`, `Tools/editor/` or `.uproject` byte has changed since the prior gate; the entire delta is
the two record repairs. The eleven code/asset rows below were nonetheless re-derived by running
their own commands, not carried forward from the prior verdict.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing; neither path appears in `git status --porcelain`. Both manifests unmodified, so the `rulesCommit`/`dataCommit` gate is untouched. |
| 2 | No `strat::` call outside `Source/StratBridge/`, `Source/StratRules/` | pass -- the checklist grep, filtered of those two directories, returned ZERO lines. No hit to judge this pass. |
| 3 | No vendored header in front of UHT | pass -- every `.h` under `Source/` containing `UCLASS`/`USTRUCT`/`UENUM` was tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h`, `Replay.h`. One file matched, `Source/StratBridge/StratBridge.h:53-58`, and it is NOT a violation: grepping that file for the macro-invocation form (line-anchored, open-paren) returns nothing -- the macro names occur in its prose only. The three new headers include no vendored header (`StratSoundBank.h` -> `CoreMinimal.h`, `Engine/DataAsset.h`, `StratSoundCues.h`, its own `.generated.h`; `StratSoundDirector.h` -> `CoreMinimal.h`, `Subsystems/WorldSubsystem.h`, `StratSoundCues.h`, its own; `StratSoundCues.h` -> `CoreMinimal.h` and its own). |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two members exist, `Source/StratPlay/StratMatchSubsystem.h:2538` and `Source/StratUI/StratScoreboardHUD.h:1235`, both `TPimplPtr<FStratBridge>`. The single `TUniquePtr<FStratBridge>` occurrence in the tree is `Source/StratUI/StratViewModel.h:47`, inside a comment recording the measured `C4150` -- prose, not a declaration. |
| 5 | Module arrows unchanged and correct | pass -- derived, not trusted; both commands were extracted from the checklist and executed. The extraction printed 16 rows, including `StratBridge PrivateDependencyModuleNames StratRules`, and the guard is intact at `Source/StratBridge/StratBridge.Build.cs:133-137` (`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` then `if (!bCompileVendoredRulesHere) { PrivateDependencyModuleNames.Add("StratRules"); }`), so that row is monolithic-only and is NOT drift. `StratRules` appears in no `PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. The field census printed TEN names -- `BRIDGE=`, `PCHUsage =`, `PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =` -- matching the recorded set exactly, `BRIDGE=` being the known string-literal false positive. No eleventh name. All five modules appear; none is missing from the output. `StratUI` has gained no `Stratocracy` dependency; no cycle. |
| 6 | New modules registered | pass -- no new `Source/<Module>/` directory. `IMPLEMENT_MODULE` occurs in `StratBridge.cpp:23`, `StratPlay.cpp:14` and `StratUI.cpp:10`, plus `IMPLEMENT_PRIMARY_GAME_MODULE` in `Stratocracy.cpp:6`; all four names are in the `Modules` array of `Stratocracy.uproject`. `Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly ABSENT from that array. |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing and no untracked path is under it. |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- the only executable non-`Tests/` hit is `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing import-commandlet exception. Three further hits are at `Source/Stratocracy/StratData/Tests/StratDataParity.cpp:376,472,555`, the standing automation-fixture exception. The audio territory is reached from `Source/` only in `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp`, unchanged since the prior gate. |
| 9 | No widget-side arithmetic | pass -- no `StratUI` widget file changed at all this round (measured, see Scope). Re-scanned the module's `.cpp` files for an operator inside `SetText(`/`Format(`/`AsNumber(`: four hits, `StratScoreboardWidget.cpp:55,92,93,195`, each passing a view-model field straight through a formatter with no operator applied to a displayed value. |
| 10 | Lane discipline | pass -- the delta is exactly two files, each written by the agent that owns it: `global.md` by the `coordinator` (its own file, in lane) and `data.md` by `strat-data-steward` (its own file, in lane). No code, no test, no asset and no `Config/` byte moved. No exception clause invoked and none needed. |
| 11 | Prose blocks present, measurements recorded as measurements | **BLOCK** -- all three prior findings' subjects were re-derived and all three are CLOSED. But the repairs introduced three new checkable-and-wrong claims: a count claim in the live banner that its own paragraph three lines earlier falsifies, an engine-source citation to a file that does not exist, and a stamp whose date its own contents falsify. Findings 1-3. |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` shows a blank first column on every line (` M`, `??`); `git diff --cached --stat` printed NOTHING. `HEAD` is still `089c79c`; nothing was committed. This report itself adds one further untracked path, alongside the three gate reports already untracked. |

## The three prior findings, re-derived

Each was re-derived against the files, not accepted from the dispatch.

**Prior Finding 1 -- CLOSED.** The stale provenance sentence is gone, and the deletion orphaned
nothing. A sweep of the whole of `global.md` for the deleted claim's phrasing
(`phase-B run`, `landed on local`, `UTC`) returns no hit anywhere in the live banner. The
paragraph now reads, `global.md:29-34`, as one continuous chain from `THE LIVE FIGURE'S REPORT IS`
through the supersession list to the four-states clause -- no dangling pronoun, no half-sentence,
no reference to a removed antecedent. Deletion rather than a re-word is defensible on the reason
given: with the banner dated 2026-09-05 and the live report `2026.09.05-13.12.09`, there is no
UTC/local split left for that paragraph to explain.

**Prior Finding 2 -- CLOSED.** `global.md:43-46` now carries
`[STAMPED 2026-09-05: AT THIS PHASE no sound asset existed and none was set ... THE ASSET PASS
BELOW CHANGED BOTH HALVES OF THAT, and this sentence is kept only to say what the C++ phase was
measured against.]` -- stamped, not deleted, in the banner's own convention, and it does not
contradict the asset paragraph at `:74-77`. The count correction at `:39-42` is right and right
for the right reason: TWO C++ properties (`FStratMatchConfig::SoundBank`,
`AStratShellGameMode::SoundBank`) on THREE Blueprint defaults, with the `BP_StratGameMode_AiVsAi`
reason stated inline and the two short C++ headers routed to the engineer's lane. This matches
`tests.md`'s independent account ("two C++ classes carry a `SoundBank`, and THREE shipped
Blueprints set one, because `BP_StratGameMode_AiVsAi` is a second asset of the first class").

**Prior Finding 3 -- CLOSED, and the steward's conclusion re-derived independently here.** The
engine facts in `data.md:50-87` were re-measured against
`C:/Program Files/Epic Games/UE_5.8/Engine/Source/Runtime/Engine/Private/AudioDevice.cpp` on this
box rather than read off the steward's account:

- `grep -n "SoundMixClassEffectOverrides"` returns SEVEN lines -- `:943`, `:2942`, `:3031`,
  `:3111`, `:3860`, `:3913`, `:3960` -- exactly as the correction states.
- `:2942` is `FSoundMixClassOverrideMap* SoundMixOverrideMap = SoundMixClassEffectOverrides.Find(SoundMix);`
  and its enclosing function is `void FAudioDevice::ClearSoundMix(USoundMix* SoundMix)`, which
  opens at `:2927` -- a genuine lookup, as claimed.
- `grep -n "ApplyClassAdjusters"` returns THREE lines: `:2713`, the definition at `:3021`, and
  `:3236` -- two callers, as claimed.
- The `:2713` reasoning is exact. Read in context, `SoundMixState->InterpValue = 0.0f;` sits four
  lines above `ApplyClassAdjusters(NewMix, SoundMixState->InterpValue, InitDeltaTime);` with only
  a comment and the `InitDeltaTime` initialiser between them, inside the success arm of
  `FAudioDevice::ApplySoundMix`. That caller fires only as a mix transitions to active.
- `:3236` sits inside `void FAudioDevice::UpdateSoundClassProperties(float DeltaTime)` (opens
  `:3181`), within `for (TPair<TObjectPtr<USoundMix>, FSoundMixState>& SoundMix : SoundMixModifiers)`
  at `:3190` -- the per-tick loop over the active-mix map, as claimed.
- `grep -rn "PushSoundMixModifier\|SetBaseSoundMix\|SetSoundMixClassOverride" Source/` returns
  ZERO lines, re-run by this gate rather than accepted.

**The conclusion survives, and this gate reaches it independently:** both callers require the mix
to be, or to be entering, a member of `SoundMixModifiers`; nothing in `Source/` populates that
map; so `DefaultBaseSoundMix` is the only route today by which `SMX_Strat_Base` becomes active.

**Does the strikethrough read as a correction to someone arriving cold at `data.md:62`? Yes.**
The judgement is about what a reader who lands mid-file by citation actually sees -- the same
reasoning `CLAUDE.md` gives for requiring inline attribution. At `:62` the struck text and its
verdict are on the SAME LINE: `~~That map is read in exactly one place~~ **[FALSE, see above --
read at seven sites ...]**`, and the second at `:65-66` is likewise `~~... called from exactly one
place~~ **[FALSE, see above -- TWO callers]**`. No scrolling is needed to learn the claim is
retracted, in rendered markdown or in raw bytes; the `[CORRECTED 2026-09-05 ...]` block at
`:50-61` supplies the provenance and the `[RE-CONFIRMED ...]` block at `:81-87` the surviving
reasoning. The original phrasing does NOT read as a live claim. One cosmetic cost, not a finding:
with the struck spans excised the sentence at `:62-67` is a fragment, which is the price of this
record's kept-not-rewritten practice and is paid openly.

**The steward's decision not to touch `decisions.md` is correct.** Checked rather than accepted:
the new `decisions.md` entry contains no "exactly one place" claim and no enumeration of engine
call sites at all. It says *"Full reasoning and the engine-source trace are in `data.md`'s entry
for the same date"* -- it LINKS rather than restates, which is this record's own
one-file-owns-each-fact rule working as designed. The defect was a measurement stated too
narrowly inside a decision whose scope call (decline `DefaultSoundClassName` and
`DefaultSoundConcurrencyName` on an unbounded blast radius) did not rest on that narrowness.
Nothing in `decisions.md` became false, so nothing in it needed a stamp.

## Findings

All three are the pattern the dispatch asked to be hunted: the repair of a previous round's
finding is itself the next finding.

**1. `Tools/architect/state/global.md:32` -- the live banner states that the suite count did not
move across four runs, and the same banner states three lines earlier that it moved by five
across two of them. Owner: the `coordinator`.**

Lines 32-34:

> The count did not move across any of those four runs; what moved was WHAT THE
> FIGURE WAS MEASURED AGAINST -- no audio assets, then assets with no clauses reading them, then
> the shipped-asset clauses, then the base sound mix declared in `Config/`.

"Those four runs" are the three named at `:30-31` plus the live one: `2026.09.05-04.55.48`,
`2026.09.05-12.28.26`, `2026.09.05-13.03.00`, `2026.09.05-13.12.09`. Lines 23-25 of the same
banner:

> THE COUNT MOVED 389 -> 413: **+24 added, 0 removed, 0 state changes**, in two steps -- +19 for
> the clause phase (10 in `StratSoundCueClauses.cpp`, 9 in `StratSoundDirectorCallSite.cpp`) and
> +5 for the shipped-asset phase (`StratShippedSoundBankParity.cpp`)

The banner's own four-state list maps the runs one-to-one: `04.55.48` is "no audio assets",
`12.28.26` is "assets with no clauses reading them", `13.03.00` is "the shipped-asset clauses".
`tests.md` pins the same mapping from the other side -- its phase-B entry cites
`reportCreatedOn 2026.09.05-04.55.48` for the two clause files ("Counted BY MACRO, not by name:
10 + 9 `IMPLEMENT_SIMPLE_AUTOMATION_TEST`") and its phase-D entry cites
`reportCreatedOn 2026.09.05-13.03.00` for `StratShippedSoundBankParity.cpp` ("five clauses,
counted BY MACRO"). So `04.55.48` and `12.28.26` each read 389 + 19 = 408 entries and `13.03.00`
read 413. The count moved, by exactly the +5 the banner itself states, between the second and
third of the four runs it says it did not move across.

This is the same defect class as prior Finding 1 -- a provenance sentence that is false of the
reports it names -- in the sentence immediately adjacent to the one that repair deleted, and in
the only file permitted to carry a live suite figure. `data.md`'s own phase-C-prime entry is
careful about exactly this, scoping its no-movement claim to ONE prior run (**"Unmoved** from the
`2026.09.05-13.03.00` figure"); the banner widened the same claim to four.

**2. `Tools/architect/state/data.md:69` -- the correction block cites a source file that does not
exist. Owner: `strat-data-steward`.**

> read in context
> (`ApplySoundMix.cpp:2695-2717` on this box), `ApplyClassAdjusters(NewMix, ...)` at `:2713` fires
> at the moment a mix TRANSITIONS TO active

There is no `ApplySoundMix.cpp` in UE 5.8. Instrument:

```
find "/c/Program Files/Epic Games/UE_5.8/Engine/Source" -name "ApplySoundMix.cpp"
```

printed nothing. `ApplySoundMix` is a member function of `FAudioDevice`, and its body is at
`AudioDevice.cpp:2695-2717` -- which this gate read, and where the cited content is exactly as
described. Every other citation in the same entry is correctly written `AudioDevice.cpp:NNNN`.
The finding is not that the measurement is wrong -- it is right, and re-derived above -- but that
the block written to repair two false citations introduced a third, and a reader who runs `find`
on the named file gets zero results. That is precisely the cite-what-a-checkout-can-re-run
standard this record holds itself to. A one-token fix, in the steward's own lane.

**3. `Tools/architect/state/global.md:133-144` -- the re-anchored stamp is dated 2026-09-04 while
its own contents are from 2026-09-05, and it undercounts the supersessions it enumerates.
Owner: the `coordinator`.**

> THE LIVE FIGURE'S REPORT WAS `reportCreatedOn 2026.09.04-16.30.20`. [STAMPED 2026-09-04:
> THAT REPORT NO LONGER EXISTS AND A READER CANNOT OPEN IT ... itself since superseded three
> times over -- by `2026.09.05-04.55.48`, `2026.09.05-12.28.26` and `2026.09.05-13.03.00` in turn

Two wrong claims in one bracket, both created by re-anchoring it in place:

(a) **The date.** A stamp bracket in this record asserts when the stamp was written -- that is the
whole function of the idiom, and `data.md` observes it exactly (`[CORRECTED 2026-09-05, SAME
PASS ...]`, `[STAMPED 2026-08-26, SAME PASS ...]`). This bracket names `2026.09.05-13.03.00`, a
report that did not exist until 13:03 on 2026-09-05, so its own contents falsify its date. The
record's established practice for amending a stamp is to NEST a second dated stamp inside it --
`data.md:239-249` is the worked example, a `[STAMPED 2026-09-04, SAME TREE ...]` block added
after an earlier one rather than edited into it. Editing the 2026-09-04 stamp in place broke that,
and it is the one change of the three that a later reader has no way to date correctly.

(b) **The count.** `2026.09.05-03.53.31` has been superseded FOUR times, not three: by
`04.55.48`, `12.28.26`, `13.03.00` and `13.12.09`, the last being the live report this same file
cites at `:29`. The trailing clause ("the live report is now whichever one the banner at the head
of this file cites") points at the fourth without counting it. This is the identical shape as the
"two Blueprints / three assets" defect this same pass corrected fourteen lines earlier: an
enumeration right about its members and short by one.

## Observations

Non-gating.

- **`global.md` still carries the only live suite count, and no verdict.** Re-run rather than
  accepted. `python Tools/architect/strat_banner_sweep.py`, unpiped, exit code read on the next
  line, printed:
  `automation report: 413 entries, all Success (2026.09.05-13.12.09)`;
  `macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=413, COMPLEX/_CLASS=0`;
  `report identity: mtime=1788613929, reportCreatedOn='2026.09.05-13.12.09'`;
  `suite claims found: 63 (1 live, 62 stamped)`; `tracked items declared: 2`;
  `SWEEP CLEAN -- no self-contradiction found`; `EXIT=0`. One live claim across five files is the
  mechanical instrument for the ownership rule, and it holds. **Note that the sweep is green on
  Finding 1**: it compares a live `N/N` against the report and against the macro census, and has
  no check for a prose claim about how a count BEHAVED across superseded reports that no longer
  exist on disk. That is a genuine gap in the instrument rather than a fault in it, and it is
  worth recording as a candidate for the steward's lane.
- **The stamps do not narrate this gate.** A sweep of the new banner (`global.md:14-104`) for
  `gate`, `verdict`, `BLOCK`, `PASS` and `integration-reviewer` returns only `GATE-AUDIO` (the
  acceptance name), one "slide gate" (a gameplay mechanism) and incidental words. No phase verdict
  is claimed and no gate is narrated.
- **`data.md:51` cites a gate report that is currently untracked.** The correction block names
  `Tools/architect/gate_reports/2026-09-05-audio-milestone-phases-c-c-prime-d.md`, which
  `git status --porcelain` reports as `??`. Both files are uncommitted in the same tree, so
  committing them together resolves it -- but if the record lands and the reports do not, the
  citation is dead in every checkout. This report is likewise untracked and inherits the same
  obligation.
- **The head banner's supersession list omits `2026.09.05-03.53.31`.** `global.md:30-31` names
  three superseded reports; the full chain from the phase-A run is five, and the fifth appears
  only in the older banner's stamp at `:137`. Not false -- the head banner never claims its list
  is exhaustive -- but a reader reconstructing the chain has to visit two banners.
- **The prior gate's open observations are unchanged and still open**, since nothing outside the
  record moved: `REPORT_PATH` at `Tools/editor/make_strat_sound_bank.py:35` is a hardcoded
  absolute path into the MAIN tree, so a run from any worktree misattributes its report; and
  `tests.md`'s phase-B present-tense sentence "No `UStratSoundBank` asset exists in this project"
  is still unstamped, the same class as the prior Finding 2 that this round closed in `global.md`.
  Neither gates.
- **Nothing in the audibility discipline regressed.** `global.md:98-104` still reads "STILL NOT
  AUDIBLE, AND NOTHING HERE CLAIMS OTHERWISE ... A human at the keyboard is the only instrument,
  and the listening pass is owed."

VERDICT: BLOCK
