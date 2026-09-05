# Gate: audio milestone phases C, C-prime, D -- RE-GATE 2 -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`,
`HEAD = 089c79c1dea49ce45eb6cf061645ff37ba9a4b1d` (`git rev-parse HEAD`). Nothing committed,
nothing staged. 17 tracked files modified, 29 untracked paths (the 28 of the prior gate plus that
gate's own report). The FULL standing twelve-item checklist was re-run over the whole tree, not
only the delta, because a verdict describes a tree.

**The delta since the second gate was measured, not assumed.** Instrument, run from the repo root:

```
find Source Content Config Tools/editor Stratocracy.uproject -newer \
  Tools/architect/gate_reports/2026-09-05-audio-milestone-phases-c-c-prime-d-regate.md -type f
```

printed NOTHING; the same predicate over `Tools/architect/state` printed exactly two paths,
`Tools/architect/state/data.md` and `Tools/architect/state/global.md`. So no `Source/`,
`Content/`, `Config/`, `Tools/editor/` or `.uproject` byte has moved since the second gate, and
the whole delta is the two record repairs. The eleven code/asset rows below were nonetheless
re-derived by running their own commands rather than carried forward.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing; neither path appears in `git status --porcelain`. Both manifests unmodified, so the `rulesCommit`/`dataCommit` gate is untouched. |
| 2 | No `strat::` call outside `Source/StratBridge/`, `Source/StratRules/` | pass -- `grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*(" Source/` filtered of those two directories returned ZERO lines. No hit to judge this pass. |
| 3 | No vendored header in front of UHT | pass -- every `.h` under `Source/` carrying a line-anchored `UCLASS(`/`USTRUCT(`/`UENUM(` was tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h`, `Replay.h`. ZERO files matched. `StratBridge.h`, which does include the vendored headers, names those macros in prose only and declares no reflected type. |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two members exist, `Source/StratPlay/StratMatchSubsystem.h:2538` and `Source/StratUI/StratScoreboardHUD.h:1235`, both `TPimplPtr<FStratBridge>`. The single `TUniquePtr<FStratBridge>` occurrence is `Source/StratUI/StratViewModel.h:47`, inside a comment recording the measured `C4150` -- prose, not a declaration. |
| 5 | Module arrows unchanged and correct | pass -- derived, not trusted; both checklist commands were extracted and executed. The extraction printed 16 rows including `StratBridge PrivateDependencyModuleNames StratRules`, and the guard is intact at `Source/StratBridge/StratBridge.Build.cs:133-137`, so that row is monolithic-only and is NOT drift. `StratRules` is in no `PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. All five modules appear. The field census printed TEN names -- `BRIDGE=`, `PCHUsage =`, `PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =` -- matching the recorded set exactly, `BRIDGE=` being the known string-literal false positive. No eleventh name. `StratUI` has gained no `Stratocracy` dependency; no cycle. |
| 6 | New modules registered | pass -- no new `Source/<Module>/` directory. `IMPLEMENT_MODULE` occurs in `StratBridge.cpp:23`, `StratPlay.cpp:14`, `StratUI.cpp:10`, plus `IMPLEMENT_PRIMARY_GAME_MODULE` in `Stratocracy.cpp:6`; the `Modules` array of `Stratocracy.uproject` reads exactly Stratocracy, StratBridge, StratUI, StratPlay. `Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly ABSENT. |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing and no untracked path is under it. |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- the only executable non-`Tests/` hit is `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing import-commandlet exception. The audio territory is reached from `Source/` only inside `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp`, unchanged since the first gate. |
| 9 | No widget-side arithmetic | pass -- no `StratUI` file changed at all this round (measured, see Scope). A grep for an arithmetic operator inside `SetText(`, `Format(` or `AsNumber(` over `Source/StratUI/` outside `Tests/` returned ZERO lines. |
| 10 | Lane discipline | pass -- the delta is exactly two files, each written by an owner of it: `global.md` by the `coordinator` (a declared sole writer of that file) and `data.md` by `strat-data-steward` (its own file). No code, test, asset or `Config/` byte moved. No exception clause invoked and none needed. |
| 11 | Prose blocks present, measurements recorded as measurements | **BLOCK** -- prior Findings 1 and 2 are CLOSED and prior Finding 3(b) is CLOSED. Prior Finding 3(a), the stamp's date, is NOT closed, and the repair added a new sentence that states the false pairing explicitly and checkably. Finding 1. |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` shows a blank first column on every line (the only two-character prefixes across the whole output are a space-M and a double question mark); `git diff --cached --stat` printed NOTHING. `HEAD` is still `089c79c`; nothing was committed. This report adds one further untracked path. |

## The three prior findings, re-derived

**Prior Finding 1 -- CLOSED, and the replacement figures are correct against three independent
sources.** `global.md:32-35` now reads:

> ACROSS THOSE FOUR RUNS THE COUNT MOVED ONCE: `04.55.48` and `12.28.26` both
> read 408, `13.03.00` and `13.12.09` both read 413, so the +5 of the shipped-asset phase landed
> between `12.28.26` and `13.03.00` -- named rather than given as a position, because the list in
> the sentence above runs newest-first and "the second" would resolve to the wrong report.

Checked three ways rather than accepted. (a) The macro census, run per file:
`StratSoundCueClauses.cpp` = 10, `StratSoundDirectorCallSite.cpp` = 9,
`StratShippedSoundBankParity.cpp` = 5 `IMPLEMENT_SIMPLE_AUTOMATION_TEST`. 389 + 19 = 408 and
408 + 5 = 413, so both figures follow from the banner's own "+24 in two steps". (b) `tests.md`
pins the same mapping from the other side, and carries no number of its own: its phase-B entry
cites `reportCreatedOn 2026.09.05-04.55.48` for the two clause files and its phase-D entry cites
`reportCreatedOn 2026.09.05-13.03.00` for the five-clause file. (c) The live report was read
directly with `utf-8-sig`: 413 entries, `reportCreatedOn 2026.09.05-13.12.09`, 24 entries whose
JSON contains `GATE-AUDIO`, and a state census of Success=413 -- zero non-Success of any kind.
The figure 408 appears in exactly one place in the whole record (`global.md:33`), so the
one-file-owns-each-fact rule is not strained by it.

The positional-reference repair is also correct: the list at `:30-31` does run newest-first, so
"the second" would have resolved to `2026.09.05-12.28.26` read forwards and to
`2026.09.05-04.55.48` read backwards, and naming the two reports removes the ambiguity rather
than restating it.

**Prior Finding 2 -- CLOSED, and the surviving `ApplySoundMix.cpp` string is correct because it
is inside a negation.** `data.md:69-72` now reads `AudioDevice.cpp:2695-2717` and adds
*"no `ApplySoundMix.cpp` exists anywhere in the engine tree; the citation named the function, not
the file"*. Both halves measured. Instrument, with the control the standing rule requires so that
an empty result is not mistaken for a broken command:

```
find "/c/Program Files/Epic Games/UE_5.8/Engine" -name "ApplySoundMix.cpp"      -> nothing
find "/c/Program Files/Epic Games/UE_5.8/Engine/Source" -name "AudioDevice.cpp" -> one path
```

The search was widened from the prior gate's `Engine/Source` to the whole `Engine` tree, because
the record's claim is "anywhere in the engine tree" and a narrower instrument would not have
covered the claim as written. A grep for the string `ApplySoundMix.cpp` across
`Tools/architect/state/` returns exactly one line, `data.md:70`, inside that negation -- the
string does not survive anywhere as a live citation.

**Prior Finding 3 -- (b) CLOSED, (a) NOT CLOSED. See Finding 1 below.** The supersession count is
now right: `global.md:145` reads *"FOUR supersessions of `03.53.31`, not one"* and enumerates
`04.55.48`, `12.28.26`, `13.03.00`, `13.12.09` -- four members, four claimed, and the fourth is
the live report the head banner cites at `:29`.

**The whole `data.md` engine trace was re-derived line by line against
`C:/Program Files/Epic Games/UE_5.8/Engine/Source/Runtime/Engine/Private/AudioDevice.cpp` on this
box, not read off the steward's account.** Every citation in the corrected block is true:

- A grep for `SoundMixClassEffectOverrides` returns exactly the seven claimed lines: `:943`
  (Empty), `:2942` (Find), `:3031` (Find), `:3111` (Remove), `:3860` (FindOrAdd), `:3913` (Find),
  `:3960` (Remove).
- A grep for `ApplyClassAdjusters` returns exactly three: `:2713`, the definition at `:3021`, and
  `:3236` -- two callers, as claimed.
- `FAudioDevice::ClearSoundMix` opens at `:2927` and `:2942` is inside it, fifteen lines in; the
  body reads `FSoundMixClassOverrideMap* SoundMixOverrideMap = SoundMixClassEffectOverrides.Find(SoundMix);`
  followed by a loop setting `Entry.Value.bOverrideApplied = false;` -- exactly the "reset
  `bOverrideApplied` on teardown" the record describes.
- `FAudioDevice::RemoveSoundMix` is a real and DIFFERENT function, opening at `:2477`. It does not
  enclose `:2942`.
- `:2713` sits inside `FAudioDevice::ApplySoundMix` (opens `:2692`), five lines after
  `SoundMixState->InterpValue = 0.0f;` with only a comment and the `InitDeltaTime` initialiser
  between -- the record's "immediately after ... reset to `0.0f` on activation" is exact.
- `:3236` sits inside the loop over `SoundMixModifiers` opening at `:3190`, as the `:3190/:3236`
  citation states.
- `GameplayStatics.cpp:2052-2069` is `UGameplayStatics::SetSoundMixClassOverride` in full, ending
  in the `AudioDevice->SetSoundMixClassOverride(...)` forward. `AudioDevice.cpp:3839` opens
  `FAudioDevice::SetSoundMixClassOverride`; `:3735` opens `SetBaseSoundMix`; `:515-519` loads
  `GetDefault<UAudioSettings>()->DefaultBaseSoundMix` and `LoadObject`s it, inside the cited
  `:513-520`; `:2448` opens `SetDefaultBaseSoundMix`, which re-reads the same config key at
  `:2452`. `AudioSettings.h:138-140` is the `DefaultSoundClassName` comment, `UPROPERTY` and
  declaration.
- A grep of `Source/` for `PushSoundMixModifier`, `SetBaseSoundMix` and `SetSoundMixClassOverride`
  returns ZERO lines, re-run here rather than accepted, and
  `Source/StratPlay/StratSoundDirector.cpp:106` is `UGameplayStatics::PlaySound2D(World, Sound, ...)`
  -- the only `UGameplayStatics::` call in that file, matching "calls only `PlaySound2D`".
- The `Config/` claims hold: the raw bytes of `Config/DefaultEngine.ini` count 138 CRLF and 0
  LF-only newlines, and `git diff` shows the two-line `[/Script/Engine.AudioSettings]` section
  inserted after the unchanged `[/Script/EngineSettings.GameMapsSettings]` block, exactly as the
  entry describes.

**The two claims the dispatch asked to be verified rather than accepted are both TRUE.**
`FAudioDevice::ClearSoundMix` opens at `:2927`, so the record's existing label on `:2942` is
correct, and `RemoveSoundMix` at `:2477` is a different function -- confirmed by reading both
openers out of the file. And the steward's uncertainty never reached a record file: a grep of
`Tools/architect/state/` for `RemoveSoundMix`, "could not confirm", "uncertain", "unsure" and
"wondered" returns two lines, `data.md:456` (a packaging note about `RunUAT`) and
`global.md:4595` (a gate's self-flagged uncertainty from a different milestone), and no
occurrence of `RemoveSoundMix` anywhere in the record at all.

## Findings

**1. `Tools/architect/state/global.md:136-146` -- the re-anchored stamp is still dated 2026-09-04
while its contents require 2026-09-05, and the nested correction added this round states that
impossible pairing explicitly, in the file's own words. Owner: the `coordinator`.**

The bracket, at `:136` and `:140-143`:

> THE LIVE FIGURE'S REPORT WAS `reportCreatedOn 2026.09.04-16.30.20`. [STAMPED 2026-09-04:

> `2026.09.05-03.53.31`, itself since superseded by the phase-B run named in the banner above
> -- [STAMPED 2026-09-05, NESTED RATHER THAN EDITED INTO THE LINE ABOVE, ... **When this was
> written on 2026-09-04 the banner named `2026.09.05-04.55.48`.**

**`2026.09.05-04.55.48` is local 2026-09-05 00:55:48. It did not exist on 2026-09-04 at all**, so
the sentence's two halves cannot both hold, and the outer bracket's `[STAMPED 2026-09-04]` date
cannot hold either while the bracket names the phase-B run.

The offset was measured on this box rather than assumed, because the whole question turns on it.
`Saved/AutomationReport/index.json` carries `reportCreatedOn 2026.09.05-13.12.09` and a
filesystem mtime of `2026-09-05 09:12:09.156 -0400` -- the same minute and the same second, four
hours apart, so `reportCreatedOn` is UTC and this box is UTC-4. Two further sources agree:
`tests.md:361` glosses `reportCreatedOn 2026.09.04-04.47.20` as *"(UTC in the report; local date
2026-09-04)"*, which only works at a four-hour offset, and the standing note that a
`reportCreatedOn` runs a day ahead after 20:00 local is exactly UTC-4. Applying it:

| report | UTC | local |
|---|---|---|
| `2026.09.05-03.53.31` (phase A) | 09-05 03:53 | **2026-09-04** 23:53 |
| `2026.09.05-04.55.48` (phase B) | 09-05 04:55 | **2026-09-05** 00:55 |
| `2026.09.05-12.28.26` | 09-05 12:28 | 2026-09-05 08:28 |
| `2026.09.05-13.03.00` (phase D) | 09-05 13:03 | 2026-09-05 09:03 |
| `2026.09.05-13.12.09` (live) | 09-05 13:12 | 2026-09-05 09:12 |

The window in which "the banner named `2026.09.05-04.55.48`" was true runs from local 2026-09-05
00:55 to 08:28 -- entirely on 2026-09-05. There is no instant on 2026-09-04 at which the sentence
is true.

**Both halves of this were reachable before either bracket was written**, which is what makes it
a finding rather than an accident of timing: phase A's own run, `2026.09.05-03.53.31`, is the last
report of local 2026-09-04, and it is named three words earlier in the same sentence.

**Why the repair did not close prior Finding 3(a).** That finding was that a 2026-09-04 stamp
cannot contain reports that did not yet exist. The repair removed `12.28.26` and `13.03.00` from
the outer bracket and replaced them with "the phase-B run" -- but the phase-B run is `04.55.48`,
which is also on 2026-09-05, so the defect was reduced by three-quarters and not removed. The
nested stamp then converted what had been an implicit date error into an explicit, quotable
sentence asserting it.

**And the false claim it now asserts is the one round one deleted.** The first gate's Finding 1
struck a sentence from the head banner reading *"run landed on local 2026-09-04 ... It is the
phase-B run"*. That is the same pairing -- phase B, local 2026-09-04 -- re-entering the file
eleven lines further down, inside the bracket written to repair the second gate's Finding 3. This
is the third consecutive round in which a repair is the next finding, and the first in which the
repair reinstates a claim an earlier round had already removed.

**What is NOT wrong here, so the fix is not over-applied.** The nesting practice is right and is
the one `data.md:239-249` models. The supersession enumeration is right. "the banner above" is
correctly identified as a pointer that no longer resolves, and the instruction *"Take the live
report from the banner's own citation, never from this sentence"* is exactly the right remedy.
The defect is confined to two date assertions: the outer `[STAMPED 2026-09-04]` label on a
bracket that names the phase-B run, and the nested sentence *"When this was written on
2026-09-04"*. Both are in the `coordinator`'s own file and its own lane.

## Observations

Non-gating.

- **The sibling stamp at `global.md:127-129` carries the same 2026-09-04 date and a weaker version
  of the same problem, and I could not date its authorship.** It reads `[STAMPED 2026-09-04] The
  suite WAS **389/389** ... The LIVE figure is the audio pass's, in the banner above this one.`
  For that last clause to be true when written, the audio banner -- headed `_Last run 2026-09-05`
  and describing phases through D -- had to exist. No instrument in this tree dates an uncommitted
  edit, so this is recorded as a candidate for the same pass rather than as a finding. Both
  brackets are absent from `HEAD`: `git diff -U0` shows the hunk at `+127` and the hunk at `+136`
  as pure additions over `089c79c`, whose committed line is the bare
  "THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.04-16.30.20`." with no bracket at all.
  **A related consequence worth stating plainly: because the whole bracket is uncommitted, the
  description of it as "restored to its original wording" is unfalsifiable from a checkout** --
  git holds no earlier version of it to compare against. That is not a criticism of the claim,
  which may well be exact; it is the same declared-unfalsifiable shape `CLAUDE.md` names for
  obligation (1) of the in-session clause, and a reader is entitled to know which claims rest on
  an instrument and which on the author's honesty. The date defect in Finding 1 rests on an
  instrument.
- **`data.md:62-63` says "read at seven sites" where `:54` correctly says "returns SEVEN lines".**
  Of the seven, three are `Find` reads, one is `FindOrAdd`, one is `Empty()` and two are
  `Remove()`. The bracket disambiguates itself in the same breath -- "one of which ... is a
  genuine lookup, not merely a write site" concedes the rest are not lookups -- and the precise
  form sits nine lines above it, which is why this is an observation and not a finding. It is also
  **not new this round**: the identical string was present at the second gate, which quoted it at
  `:62` and passed over it. Recorded so the next reader does not have to re-derive it.
- **The banner sweep is green and was re-run rather than accepted.**
  `python Tools/architect/strat_banner_sweep.py`, unpiped, exit code read on the next line:
  `automation report: 413 entries, all Success (2026.09.05-13.12.09)`;
  `macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=413, COMPLEX/_CLASS=0`;
  `report identity: mtime=1788613929, reportCreatedOn='2026.09.05-13.12.09'`;
  `suite claims found: 63 (1 live, 62 stamped)`; `tracked items declared: 2`;
  `SWEEP CLEAN -- no self-contradiction found`; `EXIT=0`. **The sweep is green on Finding 1**, as
  it was on the second gate's Finding 1: it compares a live N/N against the report and the macro
  census, and has no check that a stamp's date is consistent with the report timestamps inside it.
  Given that three of the last six findings on these phases have been date or provenance claims
  inside stamps, a sweep part that parses a `reportCreatedOn` out of a `[STAMPED YYYY-MM-DD ...]`
  bracket and compares the two at a measured UTC offset is now a well-evidenced candidate for the
  steward's lane.
- **The positional-reference sweep found no other broken pointer in the milestone's record.** A
  case-insensitive grep for "banner above", "the second", "the third", "above this", "below this",
  "the line above", "sentence above", "see above" and "the pass below" over the first 200 lines of
  all six state files returns, in the live region: `global.md:35` (correct, and the reason is
  stated inline), `:129` (resolves to the audio banner), `:140-142` (disclosed by the nested stamp
  itself), `data.md:62,65,67` (all resolve to the correction block ten lines above), and entries
  in `content.md`, `decisions.md` and `tests.md` that predate this milestone and resolve
  correctly. No unresolved "the banner above", "the second" or "below" was found outside the one
  Finding 1 already covers.
- **The prior gates' open observations are unchanged and still open**, nothing outside the record
  having moved: `REPORT_PATH` at `Tools/editor/make_strat_sound_bank.py:35` is a hardcoded
  absolute path into the MAIN tree, so a run from any of the seven worktrees misattributes its
  report; `tests.md`'s phase-B sentence "No `UStratSoundBank` asset exists in this project" is
  still an unstamped present-tense claim that phase C falsified; and the four earlier gate reports
  plus this one are untracked while `data.md:51` cites the first of them by path, so that citation
  is dead in any checkout that takes the record without the reports. Committing them together
  resolves the last of these.
- **Nothing in the audibility discipline regressed.** `global.md:101-107` still reads "STILL NOT
  AUDIBLE, AND NOTHING HERE CLAIMS OTHERWISE ... A human at the keyboard is the only instrument,
  and the listening pass is owed."

VERDICT: BLOCK
