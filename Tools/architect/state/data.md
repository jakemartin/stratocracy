# Stratocracy state — data

> **Sole writer: `strat-data-steward`, except that the `coordinator` transcribes
> lane-authored entries after a merge, attributing acting and writing separately (user ruling,
> 2026-08-29; the clause and its limits are stated in `CLAUDE.md`, which is the authority).** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** `Config/`, DataTables, the gates and hooks under `Tools/architect/`, evidence slicing practice, and checked-in fixtures.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

- **DONE, 2026-08-31 -- the project's `ProjectName` no longer reads the template's name, and a
  dead gate-report citation is corrected in place.** Working tree: `E:/MultiAgent/Stratocracy`,
  branch `master`, base HEAD `2592276`.
  **THE EDIT, `Config/DefaultGame.ini`, line 3.** Was `ProjectName=Top Down Game Template`, now
  `ProjectName=Stratocracy`. CRLF preserved (15 `\r\n` before and after, verified by byte count).
  **KEY CHOSEN, AGAINST THE ALTERNATIVES, READ FROM THE ENGINE SOURCE, NOT GUESSED.**
  `UGeneralProjectSettings` (`C:/Program Files/Epic Games/UE_5.8/Engine/Source/Runtime/
  EngineSettings/Classes/GeneralProjectSettings.h`) declares `ProjectName` as "The project's
  non-localized name" — the one field this task's own evidence (two archived Win64 packages
  carrying the template's name) points at. It also declares a SEPARATE
  `ProjectDisplayedTitle` ("The project's title as displayed on the window title bar") under
  `Category=Displayed`, plus `CompanyName`, `ProjectVersion`, `Description`, `Homepage`,
  `CopyrightNotice`, `LicensingTerms`, `PrivacyPolicy`, `SupportContact`. None of these appear in
  `Config/DefaultGame.ini` today (absent = engine default, empty string), and none is touched by
  this edit. `ProjectDisplayedTitle` in particular falls back to `ProjectName` when unset, per the
  engine's own convention for that field, so this one edit already reaches the window-title case
  without inventing a value for a field the user did not name. `CompanyName`, a version string, a
  description and the rest are all editorial/business decisions this agent has no brief for and
  is not this agent's call — left alone, stated here rather than silently defaulted.
  **NOT VERIFIED FROM A CHECKOUT:** whether the already-archived Development/Shipping packages
  need to be re-packaged to pick this up — packaging was not re-run by this agent, per its own
  lane; that is the user's or the editor-builder's next step if a corrected archive is wanted.
  **GATE REPORT CITATION, `Tools/architect/gate_reports/2026-08-31-w8-lanes.md`.** The
  `2026.09.01-00.02.33` stamp this file's own idiom had already superseded once was itself dead —
  `strat-gameplay-engineer`'s post-link-fix suite re-run overwrote `Saved/AutomationReport/` a
  second time. Verified myself, not taken on the dispatch prompt's figure: read
  `Saved/AutomationReport/index.json` directly with `utf-8-sig` —
  `reportCreatedOn='2026.09.01-03.15.35'`, `succeeded=347`, `succeededWithWarnings=0`, `failed=0`,
  `notRun=0`, matching the brief exactly. Added a second `[SUPERSEDED 2026-08-31 ...]` block
  after the first one, in the same idiom (stamped in place, old text kept, new stamp naming the
  live report and who read it), rather than editing the dead stamp in place. LF line endings
  preserved (205 `\n`, 0 `\r\n`, before and after). Full detail, the exact before/after text and
  the banner-sweep sweep this pass ran, is in this agent's own dispatch report; not restated here
  beyond what a future reader of this record needs to find the edit.
  **BANNER SWEEP.** `python Tools/architect/strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0,
  both before and after both edits above — the automation-report line in its own printed summary
  moved from nothing-yet-observed to `347 entries, all Success (2026.09.01-03.15.35)`, matching
  the report read directly.
  **`global.md` NEEDS AN EDIT THIS AGENT DOES NOT MAKE**, per its own lane boundary: the sentence
  citing the dead `2026.09.01-00.02.33` gate-report debt should be stamped superseded and point at
  this entry and at the gate report's own second `[SUPERSEDED ...]` block, now correcting to
  `2026.09.01-03.15.35`. Left to the `coordinator`, stated here rather than silently assumed done.

- **DONE, 2026-08-31 -- packaged Win64 builds can now read `Data/ferrum_crossing.json` at
  runtime, via a new `[/Script/UnrealEd.ProjectPackagingSettings]` stanza in
  `Config/DefaultGame.ini`.** Working tree: `E:/MultiAgent/Stratocracy`, branch `master`, HEAD
  `2592276`. The read site (measured, not assumed by this entry -- named in the dispatch and
  independently re-read here): `FStratBridge::LoadScenarioFromFile`
  (`Source/StratBridge/StratBridge.cpp`) calls `FFileHelper::LoadFileToString` against a full
  path built by two callers named `ResolveScenarioPath`
  (`Source/StratPlay/StratMatchSubsystem.cpp`, `Source/StratUI/StratScoreboardHUD.cpp`) as
  `FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), Configured))`, and the
  configured string `Data/ferrum_crossing.json` lives as a Blueprint default on
  `BP_StratGameMode` and `BP_StratGameMode_AiVsAi` (confirmed by grepping printable ASCII runs
  out of the raw `.uasset` bytes -- a `.uasset` cannot be read any other way from a checkout, and
  this entry does not claim to have opened it in the editor). `Config/` before this edit had NO
  `ProjectPackagingSettings` section anywhere (`grep -rn "ProjectPackagingSettings" Config/`
  returned nothing, confirmed live with a printed sentinel rather than trusting silent grep).
  **THE EDIT, `Config/DefaultGame.ini`.** Appended a new section after the existing
  `[ConsoleVariables]` block (was absent; now present):
  ```
  [/Script/UnrealEd.ProjectPackagingSettings]
  +DirectoriesToAlwaysStageAsNonUFS=(Path="../Data")
  ```
  **MECHANISM CHOSEN AND WHY, AGAINST THE ALTERNATIVES.** `UProjectPackagingSettings` (engine
  header `Developer/DeveloperToolSettings/Classes/Settings/ProjectPackagingSettings.h`, read at
  `C:/Program Files/Epic Games/UE_5.8/...`, this project's registered engine per the `.uproject`'s
  `EngineAssociation: "5.8"`) is `UCLASS(config=Game, defaultconfig)`, so it reads from
  `DefaultGame.ini`, not `DefaultEngine.ini` -- confirmed by reading the `UCLASS` line, not
  assumed from the property's editor category. Two array properties exist for staging files
  outside `Content/`: `DirectoriesToAlwaysStageAsUFS` (packed into the `.pak`) and
  `DirectoriesToAlwaysStageAsNonUFS` (copied as real loose files, never pak'd -- the engine's own
  comment in `CopyBuildToStagingDirectory.Automation.cs` says exactly that: "NonUFS files are
  never in pak files and should always be remapped"). Chose NonUFS because
  `FFileHelper::LoadFileToString` is a raw platform-file read against an absolute path this
  project computes itself; NonUFS guarantees the byte in question exists as an ordinary file on
  disk at the resolved path with no dependency on how the pak virtual filesystem chooses to remap
  a directory that was never under `Content/`, which is the more conservative choice for a read
  path that already fails loudly (`scenario file unreadable at %s`) rather than silently. Both
  properties are documented `RelativeToGameContentDir`; read the automation-tool source rather
  than assume that meant "must live under Content" --
  `StageAdditionalDirectoriesFromConfig` (`CopyBuildToStagingDirectory.Automation.cs:817-856`)
  resolves the configured path with `DirectoryReference.Combine(ProjectContentRoot, RelativePath)`
  and mirrors it into the stage tree with `StagedDirectoryReference.Combine(StageContentRoot,
  RelativePath)`, so `"../Data"` resolves to the project root's `Data/` folder (a sibling of
  `Content/`, confirmed by `ls`) and stages to the SAME relative position under the staged
  project root -- exactly where `FPaths::ProjectDir()` resolves at runtime in a packaged,
  non-editor build.
  **WHAT WAS STAGED: THE WHOLE `Data/` DIRECTORY, NOT A SUBSET, AND WHY.** The measurement handed
  to this agent showed only `ferrum_crossing.json` is proven needed by a runtime call site; the
  three CSVs are read only by an editor commandlet (`ImportStratDataCommandlet.cpp`, never in a
  packaged build) and `parity_fixture.save` only from `Tests/`. A directory-level NonUFS entry
  has no per-file filter, so staging only the JSON would mean naming individual files, which
  breaks silently the next time someone adds a new runtime-read data file to `Data/` without
  updating this ini -- the same "guard that types its own subject list stops covering the next
  one added" lesson this record already carries (2026-08-19, the vendored-citation guard). The
  CSVs and the fixture cost nothing at runtime (they are never opened by shipped code) and
  staging the directory is the lower-maintenance, equally-correct choice; said here rather than
  silently defaulted to.
  **VERIFIED FROM A CHECKOUT, EACH CLAIM AGAINST ITS OWN SOURCE, NOT NARRATED.** (1) `UCLASS`
  line read directly: `config=Game, defaultconfig` (line 178 of the header above). (2)
  `FDirectoryPath`'s single field is named `Path`
  (`Runtime/CoreUObject/Public/UObject/SoftObjectPath.h:757-768`) -- confirms the ini array
  syntax `(Path="...")`. (3) `ConfigHierarchyType.Game` (used to build `PlatformGameConfig` in
  the automation script, five call sites grepped) includes `DefaultGame.ini` per the engine's own
  config hierarchy -- this is the standard UE config-layer fact, not independently re-derived
  from the .cs file, and is flagged as such. (4) `Data/` sits directly beside `Content/` at
  project root (`ls E:/MultiAgent/Stratocracy/` lists both), so `"../Data"` from
  `ProjectContentRoot` lands exactly on it. (5) CRLF preserved: `Config/DefaultGame.ini` was
  CRLF before the edit (hex-dumped, `0d0a` throughout) and the new lines were written with the
  same terminator (hex-dumped after the edit, `0d0a` at every line end including the two new
  ones).
  **NOT VERIFIED FROM A CHECKOUT, STATED RATHER THAN ASSERTED.** This agent did not run
  `RunUAT BuildCookRun` and did not confirm the staged output tree on disk -- the user has a
  Win64 archive running against this same tree right now (started before this edit) and this
  entry does not treat its output as evidence either way, per the dispatch's own instruction. So:
  the config syntax and the resolution logic are read off the engine source and are correct as
  read, but **no packaged build produced under this exact ini has actually been inspected for the
  presence of `Data/ferrum_crossing.json` at the resolved path** -- that is the one thing this
  entry cannot close from a checkout, and the user's own next run (started after this write) is
  the way to close it.
  **NOT RE-RUN THIS PASS, DELIBERATELY: the headless automation suite.** A `Config/`-only change
  with no gameplay-code or asset dependency does not, on its own, change suite behaviour, and
  starting a competing headless UE process on this box while the user's own `RunUAT` archive is
  in flight risks resource contention on a run this agent was explicitly told not to interfere
  with. Current suite status and its full provenance are `global.md`'s, not restated or
  re-claimed here.

- **DONE, 2026-08-30 -- `GameDefaultMap` and `EditorStartupMap` moved from the match map to the
  new title level, W6's last asset-tail item, ordinary in-lane `Config/` write, no exception
  needed.** Working tree: `E:/MultiAgent/Stratocracy`, branch `master`, HEAD `40609e7` (the W6
  merge commit). No rebuild: `git diff --stat a6ebbe2d..HEAD -- Source/` returned empty, so the
  binaries the W6 merge itself built and verified (`UnrealEditor-StratPlay.dll` etc., dated
  2026-08-30 19:59-20:00 local) are current for a `Config/`-only change. Editor confirmed CLOSED
  first: no `UnrealEditor*` process, port 9315 not listening.
  **THE EDIT, `Config/DefaultEngine.ini`.** Line 2, `GameDefaultMap`: was
  `/Game/StratMaps/Lvl_FerrumCrossing.Lvl_FerrumCrossing`, now
  `/Game/StratMaps/Lvl_Title.Lvl_Title`. Line 3, `EditorStartupMap`: same before value, same new
  value, moved with it -- **BUT BY A SEPARATE DECISION, NOT BY IMPLICATION FROM LINE 2.**
  Reasoning: the editor opening onto the title screen keeps dev/QA iteration on the same boot
  path the shipping build now takes, and this milestone's own headline (the title/menu shell)
  makes that path the one worth exercising by default; a developer who wants the match map
  directly can still open `Lvl_FerrumCrossing` from the Content Browser. Line 4,
  `GlobalDefaultGameMode`, **DID NOT MOVE** and stays
  `/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C` -- `Lvl_Title` carries its own World
  Settings GameMode override to `BP_StratShellGameMode_C`, which is what makes the title map
  boot the shell while every other map keeps the project default. Read, not assumed: the exact
  line-3 convention (`Package.Package_C` vs `Package.Package`) was read off the existing lines
  before editing, not guessed.
  **THE UNCOMMITTED-ASSET CONSTRAINT, STATED FOR THE USER WHO DOES THE COMMITTING.** The four
  assets this edit depends on are uncommitted right now:
  `Content/StratMaps/Lvl_Title.umap` (staged, further modified unstaged),
  `Content/StratPlay/BP_StratGameMode.uasset` (modified unstaged), and
  `Content/StratPlay/BP_StratShellGameMode.uasset` / `Content/UI/WBP_TitleMenu.uasset`
  (untracked) -- all authored by the `coordinator` under `CLAUDE.md`'s editor-driver clause, none
  of them touched by this agent. **This `Config/DefaultEngine.ini` edit must not be committed
  without them landing in the same commit or before it**: a fresh checkout, or CI's self-hosted
  runner, would get a `GameDefaultMap` pointing at a package that does not exist on disk. This
  agent does not commit; that sequencing is the user's call.
  **SUITE, RE-RUN AFTER THE EDIT, NOT ASSERTED.** Headless run, unpiped throughout,
  `REAL_EXIT=0` read on the line after the redirect; then `Tools/architect/strat_suite_report_gate.py`
  printed `310/310 clauses Success, and every one is a name the tree declares.` /
  `SUITE REPORT GATE CLEAN` at exit 0, off `Saved/AutomationReport/index.json` at
  `reportCreatedOn 2026.08.31-03.52.25` (UTC; 2026-08-30 23:52 local). **The count did not
  move** -- unchanged from the figure `global.md` already carried for this same HEAD before this
  edit; this change adds no clause, and the live figure and its full provenance live in
  `global.md`'s banner, not restated here. This agent's own re-run overwrote the report file
  `global.md`'s existing W6-merge citation names, so that citation was stamped in place there
  (not rewritten) rather than left to silently disagree with the sweep -- see `global.md`'s
  banner, both the inline `[STAMPED 2026-08-30 ...]` beside the old citation and the new dated
  entry immediately below it.
  `strat_banner_sweep.py --explain` ran clean (`SWEEP CLEAN`, exit 0) after both the config edit
  and the `global.md` stamps.
  **NOT MEASURED THIS PASS:** PIE behaviour on `Lvl_Title`, the menu flow, or any content-lane
  fact -- the editor was never opened. That is `content.md`'s and `tests.md`'s territory, not
  restated or guessed at here.
- **FIXED, 2026-08-26 -- two wrong measurements and one missing fixture in
  `strat_banner_sweep.py`'s REPORT PROVENANCE docstring/fixtures, all found by
  `strat-integration-reviewer` on the marker-split repair's own record.** (1) The docstring's
  "~178 characters" figure for how close the mutated live-banner test landed to `_STAMP_WINDOW`
  (220) matched no measured convention. Re-derived with ONE stated convention --
  match.start()-to-match.start() using the check's own two regexes,
  `_CITED_REPORT_STAMP_RE` to `_PROVENANCE_PATH_BRANCH_RE` -- and it is **91** for the mutated
  live banner and **249** for `78ea508`'s real defect, at both sites the figure appeared (the
  `_PROVENANCE_TREE_STAMP_MARKERS` comment and part (b)'s docstring). The old conclusion ("roughly
  thirty characters of prose either way decided it") was also wrong and is corrected: 91 sits FAR
  inside the 220 window (129 to spare) and 249 sits outside it (by 29) -- not a near miss in
  either direction, so the one real defect this check exists to catch was flagged by MARGIN, not
  by the window's design. (2) No fixture pinned that `_PROVENANCE_TREE_STAMP_MARKERS` still
  honours an explicit `[STAMPED ...]` -- the two existing PASS fixtures both pass for a different
  reason (`_GOOD_PROVENANCE_NEAR_CITATION` is true on the tree/branch facts; `_GOOD_PROVENANCE`
  trips the quoted-figure exclusion on its own `"It read:"` first). Added
  `_GOOD_PROVENANCE_TREE_STAMP_ONLY`: a WRONG tree/branch claim beside a genuine
  `**[STAMPED ...]**`, no reporting verb, no `"It read:"` -- the stamp is the only thing that can
  spare it. Proved directional by monkeypatching `_PROVENANCE_TREE_STAMP_MARKERS = ()` (honouring
  no stamp at all) and re-running both fixtures outside `--self-test`: the new one FLIPS to FAIL
  (`REPORT PROVENANCE`), `_GOOD_PROVENANCE` stays green either way, confirming the old fixture
  never tested the stamp. `--self-test` -> `SELF-TEST: ALL FIXTURES CORRECT`, every pre-existing
  fixture unchanged in verdict; full sweep over `Tools/architect/state/` -> `SWEEP CLEAN`, `echo
  $?` on its own line -> `EXIT=0`. File is CRLF; CR == LF (2087 == 2087) after edit, and `git diff
  --check` reports no line-ending errors. This entry does not change what the shipped check
  DOES -- only the comments describing it and the fixture coverage proving it.
- **VENDORED 2026-08-26 -- `Tools/architect/kb_snapshot/setting.md`, a new sibling of
  `gdd_snapshot/`, so `StratMatchResultModelClauses.cpp` can READ the nine banned-register
  words instead of typing them as a stated literal.** Source is
  `E:\MultiAgent\stratocracy-content\kb\setting.md`; the vendored copy is byte-identical:
  2,111 bytes, sha256 `8b1a6c85ff1152ef5345f20fb36d2cf7f3829266649132ca5c3ee08eb6a2ad71`, LF
  line endings (0 CR over 40 lines, confirmed against a `printf 'a\r\nb\r\n'` control that
  reads 2), `git hash-object` `d8d2d0590f30f5d9153c8a12f77ed4763cae6c03`. Full measurement and
  the CRLF-trap discipline are in `Tools/architect/kb_snapshot/MANIFEST.md`, written in
  `gdd_snapshot/MANIFEST.md`'s own shape: **this is a snapshot, not a sync** -- nothing
  hash-gates it against the source, no `sync_*.py` runs behind it, no acceptance ID asserts
  over it, and it is not part of the vendoring discipline `Source/StratRules/` and `Data/`
  follow. Where the two disagree, the document in `stratocracy-content` is the real one.
  **[STAMPED 2026-08-26, SAME PASS -- FALSE NOW. The rewrite this bullet called `strat-test-author`'s
  and "not done yet" landed in this same uncommitted tree while this entry was being written. Kept
  rather than deleted, on this record's standing practice. It read:]**
  *"The clause rewrite that reads this file at run time is `strat-test-author`'s and is not
  done yet -- this entry only vendors the reference data."*
  **WHAT ACTUALLY HAPPENED.** `Stratocracy.StratUI.T-UI-03.ResultLinesAreTheGddSamplesVerbatimAndInsideTheVoiceBudget`
  in `Source/StratUI/Tests/StratMatchResultModelClauses.cpp` now PARSES the nine banned-register
  words out of this snapshot's own `- **Banned register:**` bullet at run time, wrap-aware (the
  bullet's list wraps across two lines and the read joins the continuation before cutting the
  italic span out), and refuses on a missing file, a missing bullet, an unclosed italic span, a
  malformed token, or a parsed count other than nine. Nothing about the register is typed in the
  suite any more. This entry still only vendors the reference data; the clause's own evidence
  (falsifiability, the wrap trap, the refusal-path measurements) is `strat-test-author`'s and is
  recorded in `Tools/architect/state/tests.md`, not restated here. Current suite status is in
  `Tools/architect/state/global.md`, not restated here.

- **Slice a growing log by content markers, never by EOF.** "To EOF" of a file a running editor is
  still appending to means "to whenever the cut happened to run," not a fixed boundary, and a
  line-number range into a rotating log is a fragile name even when stable at cut time (a
  neighbouring session's content can ride along under a boundary chosen for a different reason).
  Bound evidence slices by a session's own open/shutdown line, a turn boundary, or a result line;
  name the file for what it *is*, not for where it sat in the log when cut. Applied to both PIE
  session slices in `evidence/07-ai-opponent/` (`pie-session/session1-completed-game.log`,
  `pie-screenshots/session2-screenshot-run.log`); see `evidence/07-ai-opponent/blackboard.md:267-275`
  for the original derivation.
- **`Saved/AutomationReport/index.json` is UTF-8-with-BOM, not UTF-16.** First bytes are
  `EF BB BF`; all three UTF-16 codecs fail to parse it and `utf-8-sig` succeeds. Two separate
  gate passes reported UTF-16 for this file and were both wrong. **A phase-6 gate that
  hardcodes UTF-16 to parse the report will read zero tests and may not say so** — the same
  failure shape as the bare-`Stratocracy.uproject` test command that exits in ~1s having run
  nothing. Recorded here so a phase-6 reader hits it before writing that parser.
- **BUILT 2026-08-19 -- `Tools/architect/strat_banner_sweep.py`, because a rule an agent must
  REMEMBER is a rule that fails silently.** `11fd9ae` made the banner sweep a memory job; five
  commits later the memory failed and `185e88f` shipped a banner saying the content half was open
  and the suite 107/107 while this same file said discharged and 108. The reviewer's gate caught
  it (`VERDICT: BLOCK`) -- the fourth instance of that shape the record names. This is the same
  rule as a COMMAND, in `strat_fixture_verdict_binding.py`'s idiom: a declaration the maintainer
  edits deliberately, checked mechanically, exit 1 when the document disagrees with itself or the
  tree.

  ```
  python Tools/architect/strat_banner_sweep.py              # sweep, exit 0/1
  python Tools/architect/strat_banner_sweep.py --explain    # + its LIVE/STAMPED call on every claim
  python Tools/architect/strat_banner_sweep.py --self-test  # fixtures proving it can FAIL
  ```

  **[CORRECTED 2026-08-22 -- the crib above said "14 fixtures" from this entry's own day and was
  never updated when the count moved; the 2026-08-20 and 2026-08-21 entries below each recorded a
  new count without the correction reaching this line, which is where a reader actually looks.
  The number is now derived rather than restated: `--self-test` prints one `[OK]` per fixture, so
  `python Tools/architect/strat_banner_sweep.py --self-test | grep -c "\[OK\]"` answers it and
  cannot go stale. As of 2026-08-22 that is 33 -- 29 inline plus 4 for REPORT IDENTITY. The
  dated accounts below say 15, 23 and 27 and are correct FOR THEIR OWN PASSES; they are history
  and are left alone.]**

  - **Three checks:** SUITE COUNT AGREEMENT (live claims must agree with each other AND with
    `Saved/AutomationReport/index.json` and an independent macro census), ITEM STATE AGREEMENT
    (a declared item cannot be called open in one live place and closed in another), BANNER DATE
    FRESHNESS (`_Last run` not older than the document's own newest entry).
  - **It found a real defect on its first real run and it was not the one it was written for:**
    the banner still read `_Last run 2026-08-14` after two days of 2026-08-19 entries — the same
    drift the re-gate had flagged as an observation. Fixed; the banner now leads with this
    session's work and hands off to the unchanged milestone narrative.
  - **FOUR OF ITS OWN RULES EXIST BECAUSE THE VERSION BEFORE THEM GOT A REAL CASE WRONG, and
    every one has a fixture.** (1) A `**87/87**` inside a closed milestone section is correct for
    that phase — section awareness. (2) One bullet saying "discharged on two axes, the third
    still open" is coherent, not contradictory — a contradiction is disagreement BETWEEN sites.
    (3) A corrected banner says "was **107/107** ... is now **108/108**" in ONE sentence, and no
    window can tell which verb owns which figure — tense binds to what it precedes, and an
    explicit stamp outranks tense. (4) Matching only bolded `N/N` made the check blind to the
    banner's live figure (the bold wraps the phrase); widening it then swallowed the pairing
    gate's `42/42` and `68/68`, so a claim must now say it is counting a suite. Rule 4's first
    half is the sharpest: **the check that exists to police the banner was, for one revision,
    silently inert on the banner** — it reported "17 claims, 0 live" against a file whose banner
    carried a live figure.
  - **Falsifiability measured against the REAL file, not only fixtures.** Editing the banner's
    LAST suite figure to `107/107` — the exact 2026-08-19 shape — fails with
    `live suite claim(s) disagree with the tree (108): line 75: 107/107`, `EXIT=1`; reverted,
    `SWEEP CLEAN`, `EXIT=0`.
  - **Its stated limit, measured rather than supposed:** under banner supersession a stale figure
    sitting MID-banner passes — verified by planting one and getting `SWEEP CLEAN`. It polices the
    CURRENT claim, not every sentence of the narrative behind it, and the docstring says so.
  - **A FIFTH rule, found by this very entry.** Writing an honest account of the 2026-08-19
    defect means writing the wrong number down, and the first version of the sweep failed on
    its own record of itself — three hits inside the bullet you are reading. A sweep that
    punishes an honest account of a past miscount is a sweep a maintainer stops running, so a
    figure preceded by a reporting verb (`said`, `reading`, `editing`, `defect`, `stale`) or
    sitting in a code span is read as QUOTED evidence, not a live claim. Fixture:
    `_QUOTED_ACCOUNT`. Falsifiability re-measured AFTER that rule went in, because a
    suppression rule is exactly the kind that could blunt the check it protects: breaking the
    banner's live figure still fails, `EXIT=1`.
  - **WIRED, 2026-08-19, same day** — `Tools/architect/hooks/pre-commit`, installed by
    `sh Tools/architect/hooks/install.sh`. It runs the sweep only when `state.md` is staged, so
    every other commit is untouched. The bullet this replaces said wiring was "deliberately not
    taken here"; it was taken immediately afterwards, on request.
    - **It checks the STAGED blob (`git show :<path>`), not the working tree, and that is the
      whole reason it is a hook rather than a wrapper script.** `git commit` records the INDEX,
      so a working-tree sweep would wave through a commit whose staged `state.md` is a different,
      contradictory document — what a partial `git add -p` produces. **Measured, not argued:**
      with a contradiction staged and the working tree repaired,
      `python Tools/architect/strat_banner_sweep.py` prints `SWEEP CLEAN` while the hook prints
      `COMMIT REFUSED` and `HEAD` does not move.
    - **The tree-side figures are still read from the working tree, deliberately.** The report and
      the macro census are the TRUTH the document is checked against and are in no commit.
    - **`core.hooksPath` was rejected on measurement, not taste.** This repo already carries four
      Git LFS hooks (`post-checkout`, `post-commit`, `post-merge`, `pre-push`); redirecting
      `core.hooksPath` would silently disable every one of them. The installer copies into the
      free `pre-commit` slot instead, refuses to clobber a different existing hook, refuses to run
      at all if `core.hooksPath` is set, and is idempotent.
    - **Five behaviours measured end to end, HEAD checked after each:** a commit not touching
      `state.md` → hook silent, commit allowed; `state.md` staged and clean → `banner sweep: clean`,
      allowed; a contradiction staged → `COMMIT REFUSED`, `HEAD` unmoved; staged-bad/worktree-good
      → still refused (above); `git commit --no-verify` → allowed. All probe commits were reset and
      the tree returned to `b502686`.
    - **The bypass is documented in the failure message on purpose.** A WIP commit on a branch is
      not a lie, and a gate nobody can get past is a gate people delete.
    - **Hooks are not version-controlled**, so `install.sh` must be run once per clone.
      **CLOSED the same day:** `/strat-hotseat` now runs it as a startup step, before the
      editor-mode invariant and before any dispatch, so every crew session installs or confirms
      the hook. The skill also tells the coordinator not to `--no-verify` past a refusal on
      `strat-data-steward`'s behalf, and not to edit `state.md` to satisfy it — both are the
      coordinator taking a lane that is not theirs; re-dispatch the steward with the sweep output
      the way a reviewer `BLOCK` is re-dispatched.
    - **The skill's command is cwd-independent, and the relative form was a measured trap.**
      `sh Tools/architect/hooks/install.sh` works only from the repo root, and a session's cwd is
      often not the repo root — the coordinator's own has been `Tools/context` for whole sessions.
      From there the relative form gives `No such file or directory`, and piped anywhere its real
      exit code is masked by the pipe, so it fails while appearing to succeed — the same shape as
      the bare `Build.bat` form. Both skill commands now use
      `"$(git rev-parse --show-toplevel)/..."` and both were run verbatim from a subdirectory
      before being written down.
    - **CI BACKSTOP, `.github/workflows/banner-sweep.yml`, added 2026-08-19** — the repository's
      first workflow. The hook is per-clone and the skill's startup step only helps a crew
      session; a commit from a machine that never ran `install.sh`, from a fresh worktree, or
      from the GitHub web editor was still unchecked. This runs on every push and pull request.
      - **It runs `--self-test` BEFORE the sweep**, because a sweep whose own fixtures have
        rotted can report `SWEEP CLEAN` for the wrong reason — and one revision of this very
        tool was silently inert on the banner it polices. On failure it additionally runs
        `--explain`, so a red run shows which figure was judged current.
      - **What a green tick means, and what it does not.** `Saved/` is gitignored, so the
        automation report does NOT exist on a CI checkout; the sweep says so and falls back to
        the `IMPLEMENT_SIMPLE_AUTOMATION_TEST` macro census over the tracked `Source/`. **CI
        therefore checks the record against the tests that EXIST in the tree, not against a run
        of them.** It does not build and does not run the UE suite — neither is possible on a
        hosted runner without an engine install.
      - **Verified by simulating CI rather than by assuming it:** cloned the repo to a scratch
        directory (no `Saved/`), ran both workflow steps verbatim — `no automation report ...
        suite figures checked against each other only`, census 108, `SWEEP CLEAN`, exit 0 — then
        broke that clone's banner to `107/107` and got `live suite claim(s) disagree with the
        tree (108)`, `SWEEP FAILED`, exit 1. The fallback path detects the defect, which was the
        open question worth answering before writing the workflow.
- **DISCHARGED 2026-08-19 — the vendored line-citation sweep, and a CI guard so it cannot come
  back.** 46 citations into vendored headers across 16 files (`Save.h:64`, `Ui.h:346`,
  `Ai.h:49-53`, …) are now symbol references. These were the worst-aged citations in the tree:
  correct the day each was written, and **wrong all at once, silently, the day
  `Source/StratRules/` is re-vendored**, with nothing in this repository reading a vendored line
  number to notice. Doing it before the capped `chooseBuild` lands was the point.
  - **Every citation was resolved against the vendored source before being rewritten**, not
    guessed from its surrounding sentence: 32 distinct citations, 50 uses, each read at its cited
    line to learn its subject. None had rotted yet — vendored bytes have not moved since the
    citations were written, which is exactly why they all read as fine while being one re-vendor
    from worthless.
  - **The blanket substitution broke sentences, and reading the diff caught it.** Sixteen sites
    came out wrong: duplicates the map created (`` `strat::killAward` (`strat::killAward`) ``),
    lowercase sentence starts where a citation had opened a sentence, and one WRONG SUBJECT —
    `Turn.h:97-100` cited *where `TurnState` lives*, and the map turned it into
    `TurnState::builtThisTurn`, naming the member instead of the struct. All sixteen repaired by
    hand. **A mechanical rewrite of prose needs its diff read line by line; the count of
    replacements is not the measure of success.**
  - **`Economy.good.cpp:60` nearly went in as `strat::initSide` and is `strat::accrueIncome`.**
    The enclosing-function scan matched the last single-line signature above it and missed the
    real one, whose signature wraps across two lines. Caught by reading the code at the cited
    line rather than trusting the scan — the third instance today of naming a neighbour instead
    of the subject.
  - **Verified:** `Result: Succeeded`; suite **108/108**, zero non-Success,
    `reportCreatedOn 2026.08.19-20.59.53`; and the diff proven comment-only rather than asserted
    — non-comment changed lines **0** across all 16 files.
  - **CI GUARD, added to `.github/workflows/banner-sweep.yml`:** a step that fails the build on
    any `X.h:NN` citation into a vendored header outside `Source/StratRules/` (the vendored files'
    own citations are their business). **Run in both directions before being committed**, using
    the step's own body extracted from the YAML rather than a retyped approximation: clean tree →
    `clean: no vendored line-number citations outside Source/StratRules/`, exit 0; one citation
    planted back into `StratViewModel.h` → the offending line printed with `Cite the SYMBOL, not
    the line`, exit 1; reverted → exit 0.
  - **Not covered:** `.agents/` and `Tools/` prose are outside the guard's `Source/` scope, and
    citations INTO this repo's own moving files (e.g. `StratBridge.cpp:448`) are a different
    debt the guard deliberately does not police — a line number into a file this repo controls
    rots on any edit, not only on a re-vendor, and wants its own decision.
  - **CORRECTED 2026-08-19, same day, on a `VERDICT: BLOCK` — the guard was BLIND to two
    vendored units and this entry repeated the overclaim.** The alternation was TYPED by hand
    (`Save|Ai|Combat|Economy|Move|Turn|Ui|Data|Hex|Driver|Rules`) and omitted **`Replay` and
    `Scenario`**, which are real vendored translation units. Consequence, verified by re-running
    the extended pattern: **12 citations of exactly the forbidden class were live in `Source/`
    while the step printed `clean`** — a FALSE GREEN, the worst failure a guard has — and five of
    them sat in files the sweep itself had just edited. Among them
    `Replay.good.cpp:486-487`, the citation the `DT_Units` row-order argument rests on. **The
    headline figure was therefore wrong too: 46 was the count the broken pattern could see; the
    true population was 58.** Both halves fixed: the 12 are rewritten by symbol
    (`strat::applyCommand`'s `Attack`/`Build`/turn-tag arms, `strat::openTurn`,
    `strat::scenarioHash`), and **the guard now DERIVES its unit list from
    `ls Source/StratRules/*.h` rather than naming its own subjects** — a guard that types its
    subjects stops covering the next one added. Re-measured after the fix: derived list reads
    `Ai|Combat|Data|Driver|Economy|Hex|Move|Replay|Save|Scenario|Turn|Ui`, tree clean, and a
    planted `Replay.good.cpp:299-308` is caught with `EXIT=1`.
  - **Second finding from the same gate, also correct: one replacement named a NEIGHBOUR.**
    `Ui.h:346` was rendered as ```UiForecast`'s `defenderCanCounter` note`` at two sites, but that
    note is the doc comment for the FUNCTION `strat::uiForecast` (declared just below it); the
    STRUCT `UiForecast` carries no comment at all. A reader grepping for `UiForecast` would land
    on a struct that does not say what the sentence claims. Fixed to "the `defenderCanCounter`
    note above `strat::uiForecast`". **This is the fourth instance in one day of a scan naming a
    neighbour instead of a subject** — after `initSide`-for-`accrueIncome`, the `TurnState`
    member-for-struct near-miss, and the `Economy.good.cpp:60` enclosing-function scan. The
    reviewer resolved 31 distinct citations covering all 50 uses and found the other 30 correct.
  - **Three non-gating observations from the same gate, all acted on:** the dropped
    definition-site half of `Ui.good.cpp:290` is restored as "declared in `Ui.h`, defined in
    `Ui.good.cpp`"; the circular `unitId` sentence in `StratViewModel.h` is unwound; and the
    citation check now ALSO runs in the pre-commit hook, ahead of the `state.md` gate so it fires
    on a source-only commit — verified by planting a citation in `StratBoardActor.h`, staging
    only that file, and getting `COMMIT REFUSED` with `HEAD` unmoved.
  - **`install.sh` gained an update path, which the same pass proved it needed.** Refreshing the
    hook hit `REFUSING ... differs`, because the first version could only install, never update.
    It now overwrites a file carrying this project's own signature line and still refuses a hook
    it did not write — both branches tested.
  - **Re-verified after all of the above:** `Result: Succeeded`, suite **108/108**, zero
    non-Success, `reportCreatedOn 2026.08.19-21.41.27`, and non-comment changed lines **0** — the timestamp re-derived after the LAST edit in this pass rather than the first, on the reviewer's finding that it named a report predating the tree it claimed to describe.
  - **BLOCKED A SECOND TIME, 2026-08-19, and the finding was the SAME SCAN BUG this entry
    already documents — recorded because repeating a trap you have just written down is the
    finding, not the citation.** Two of the twelve rewrites named `strat::openTurn` for the
    terrain-id resolution loop at `Replay.good.cpp:299-308`. That loop is inside
    **`strat::seedFromScenario`**: `openTurn` spans 240-277 and contains no terrain-id loop at
    all. The enclosing-function scan reported `openTurn` because `seedFromScenario`'s signature
    **wraps two lines and so does not end in `{`** — byte for byte the failure that produced the
    `initSide`-for-`accrueIncome` near-miss earlier in the same pass, which this entry names two
    bullets above. It was documented and then repeated within one batch. One of the two sites had
    gone self-contradicting on its own line: "what SEEDING itself resolves by
    (`strat::openTurn`'s …)". Both now read `strat::seedFromScenario`. The reviewer resolved the
    other ten against the vendored source and confirmed them. **A scan that finds enclosing
    functions by `^\w.*{$` is wrong for this codebase and should not be used again; read the
    file around the line instead.**
  - **The hook's citation check now reads the STAGED blobs, not the working tree.** The reviewer
    found the two halves of the same hook disagreeing: the sweep did `git show ":<path>"` while
    the citation grep scanned the checkout, so a partially-staged commit could be refused for a
    citation it was not committing, or pass while the staged bytes carried one. Given this same
    pass lost a fix to worktree-versus-HEAD confusion, that was not hypothetical. Fixed and
    measured both ways: a citation present ONLY in the worktree no longer blocks (commit
    created, then reset), and a STAGED citation is refused with `HEAD` unmoved.
  - **Wrap-width debt, with an owner and a trigger rather than a shrug.** The sweep's standing
    cost is **+10** over-100-column comment lines (10 before, 20 now — the reviewer's own
    re-measurement. **The earlier "11 sites" was NOT wrong and this entry over-corrected it** —
    it counted added long lines in one diff, a different quantity from standing long lines across
    the files. Corrected on the reviewer's own objection to a correction of mine.) Deliberately not re-wrapped here:
    a cosmetic pass inside a correction commit buries the evidentiary diff. **Trigger: the next
    commit that touches those blocks for a substantive reason re-wraps them.** Owner:
    `strat-gameplay-engineer` for `Source/`, since that is its lane.
  - **BLOCKED A THIRD TIME, 2026-08-19 — the staged-blob hook silently skipped two classes of
    file, both DEMONSTRATED by the reviewer in a throwaway repo rather than argued.** (1) A file
    staged as a RENAME was never scanned: `--diff-filter=ACM` drops `R`, and a rename plus a
    SMALL edit — exactly the shape a citation edit has — landed a citation in `HEAD` with the
    hook printing nothing. (A rename with a large edit degrades to `D`+`A` and was already
    caught, which is why the hole opened only for the small case.) (2) A path containing a space
    was never scanned: the unquoted `for f in $(...)` split it, `git show` failed on the
    fragment, and `2>/dev/null` turned the failure into silence — **a failure to READ a blob was
    indistinguishable from a CLEAN blob**, which is precisely the vacuous pass the CI step
    refuses at the list level and this loop was not refusing at the file level.
    Fixed with `--diff-filter=ACMR`, NUL-delimited `-z` / `read -r -d ''` iteration, and an
    explicit refusal when a staged blob cannot be read. Re-measured in a sandbox, four ways:
    a true `R093` rename carrying a citation → REFUSED; `B space.h` carrying one → REFUSED;
    a clean staged file → allowed; an UNSTAGED citation → allowed WITH a warning.
  - **A defect in the hardening itself, found by running it rather than reading it.** The new
    block sets a `trap` naming `$STAGED` before the sweep creates that variable, and `set -u`
    turned it into `STAGED: unbound variable` on **every commit**. Caught on the first sandbox
    run; `STAGED=""` is now declared up front. Worth recording because the same commit that
    hardens a guard can break it, and only executing it shows that.
  - **The tree-wide net is back, as a WARNING.** Moving to staged blobs was right for "what am I
    committing" but gave up something real: the old worktree scan is what rescued `84eed05`,
    where a fix had been reverted to `HEAD` by a stray `git checkout --` and so was not staged at
    all. A staged-only check cannot see that. The hook now also scans the tree and prints a
    non-blocking warning naming each stray, because an unstaged citation elsewhere is not this
    commit's fault — and CI still holds the blocking tree-wide line on push. Raised by the
    reviewer as a loss neither the hook's prose nor this record had disclosed.
  - **BLOCKED A FOURTH TIME, 2026-08-19 — and the first finding was THIS HOOK'S OWN TITLE
    DEFECT, one level up.** `HITS=$(mktemp) || exit 0` meant a scratch-file failure skipped the
    ENTIRE citation check and created the commit, with no output but `mktemp`'s own stderr —
    which names neither this hook nor the fact that a check was skipped. Demonstrated, not
    argued: with `TMPDIR` pointed at a non-existent directory and `Save.h:64` staged, the
    reviewer landed that citation in `HEAD`. **A failure to SET UP the check is not a check that
    passed** — the same substitution the previous commit condemned at the file level, reproduced
    at the setup level by the very commit that condemned it. Both `mktemp` calls now refuse with
    a message naming the hook. Re-measured: broken `TMPDIR` + staged citation → `COMMIT REFUSED:
    mktemp failed, so the citation check did not run.`, `HEAD` unmoved.
  - **Second finding: two temp files leaked per record commit.** A second `trap ... EXIT`
    REPLACES the handler rather than adding to it, so the sweep's `trap 'rm -f "$STAGED"'`
    silently dropped `$HITS` and `$FAILED`. Measured by the reviewer (8 → 10 `/tmp/tmp.*` across
    one commit) and re-measured here after the fix: **leaked 0**. The later trap now names all
    three.
  - **`#!/bin/sh` overstated and is now `#!/bin/bash`.** `read -r -d ''` is a bashism; under
    `dash` it fails with `read: Illegal option -d`, the staged check degrades to nothing and the
    commit proceeds. It happened to work because `/bin/sh` on this machine IS bash 5.2 (Git for
    Windows). A shebang is where a reader checks portability, and that one promised POSIX it did
    not meet. **The restored tree-wide warning is what kept that failure soft** — under `dash`
    the guard became advisory rather than blind, which is an argument for the warning that was
    not anticipated when it was added.
  - **Four attack surfaces came back CLEAN, recorded because a negative result is a result.**
    The absolute pathspec emits repo-relative paths (so the `Source/StratRules/*` filter still
    matches, verified by staging a citation INTO a vendored file — correctly allowed); committing
    from a subdirectory still refuses (git chdirs to the top level before running hooks); a clean
    100%-similarity rename does NOT produce a false "could not read" refusal, because
    `--name-only -z` emits only the destination for an `R` entry; and the tree-wide warning grep
    costs `real 0m0.052s`. Worktrees remain untested, and the "unread blob" refusal has **no live
    trigger** — the reviewer could not reach it and said so rather than letting it read as
    verified.
  - **Regression-tested after the fixes, all six behaviours:** space-path citation REFUSED, true
    rename citation REFUSED, clean staged file ALLOWED, unstaged stray WARNED and allowed,
    broken `TMPDIR` REFUSED, temp files leaked 0.
  - **The hook then caught the coordinator, on this very commit, and it was a real defect.**
    `git commit` was REFUSED: `StratBoardActor.h:23` still carried `Replay.good.cpp:299-308`.
    Cause — the hook test itself. Planting a citation, then restoring with
    `git checkout -- <file>`, reverts the file to **HEAD**, which silently discarded that file's
    symbol fix along with the planted line. A `cp` from a backup would have been safe; the
    `checkout` was not. **Testing a guard by damaging the tree can undo the very work being
    guarded, and only the guard noticed.** Re-applied, re-verified: zero citations tree-wide,
    build `Result: Succeeded` again.

- **BUILT 2026-08-20 -- the record is now a DIRECTORY, and the sweep that polices it derives its
  own subject list.** `Tools/architect/state.md` was 3,524 lines with one banner, one `## NEXT`,
  and fourteen closed-phase sections, and it was the single highest-conflict file in the repo:
  every lane wanted to write it, and the recurring banner-vs-body contradictions are the symptom
  of a document nobody reads end to end any more. It is split into `Tools/architect/state/`, one
  file per **owning agent** rather than per topic, because topic boundaries do not map onto who is
  allowed to write and lane boundaries already do: `global.md` (coordinator + steward),
  `decisions.md` (steward, append-only), `engine.md`, `tests.md`, `content.md`, `data.md`.
  `state.md` keeps the closed-phase narrative and is FROZEN -- its own header says so, and says
  that nothing sweeps it.
  - **Three things break silently when a record is split naively, and two of them are failure
    modes this project has already paid for. All three are closed here.**
  - **1. The guard goes inert the moment you split.** `strat_banner_sweep.py` carried
    `STATE_MD = os.path.join(_HERE, "state.md")` and the hook carried
    `STATE_REL="Tools/architect/state.md"` -- both TYPED paths. Repointed rather than derived,
    creating `state/ux.md` would have swept nothing and printed clean over it: the recorded
    "a checker that types its own subject list stops covering it silently" shape, which the
    pre-commit hook's own citation loop had already been fixed for twice. The sweep now reads
    `state/` and the hook reads its file list from the **index** (`git ls-files --cached`), which
    is what the commit will actually record. A missing or empty record directory is a hard
    `NOTHING SWEPT` finding, never a quiet exit 0.
  - **2. Contradictions changed address; they did not disappear.** The sweep's whole job was
    catching a banner contradicting a claim 425 lines below it *in the same file*. Split six ways,
    that same contradiction sits *between* files, where six per-file runs each print clean. Claims
    are now collected per file and compared **globally**, and the hook extracts every file in the
    set -- not only the staged ones -- because a `global.md` swept without the `tests.md` that
    contradicts it is a clean run that means nothing.
  - **3. Exactly one file owns each fact class**, and that is mechanical rather than a convention:
    a new `RECORD OWNERSHIP` check fails a LIVE suite count anywhere but `global.md`. Quoting a
    STAMPED older figure stays legal everywhere, because this record's convention is that an old
    claim is stamped, not deleted.
  - **DEFECT FIXED IN THE SWEEP: a suite claim that ends a sentence was invisible to it.**
    `_SUITE_CLAIM_RE`'s trailing lookahead was `(?![\d/.-])`, which excluded `.` outright, so
    a claim of the shape `999/999.` went unmatched while `999/999 green` matched. All 20
    real claims in the old `state.md` happened to be followed by `*`, a space, or a comma, so the
    blind spot never once fired -- the guard had never been tested against the most natural way to
    end the sentence. The lookahead now rejects only what would make the figure a different number
    (another digit, a second slash, a hyphen, or a decimal point with a digit behind it), so
    `1.2/3.4` is still not a suite claim.
  - **BOTH FIXES SHOWN FALSIFIABLE, EACH AGAINST A CONTROL, AND THE FIRST ATTEMPT AT THE CONTROL
    WAS INVALID.** A guard that refuses proves nothing until the thing it replaced is shown NOT to
    refuse on the same bytes. The first run paired HEAD's *hook* with the *new* sweep -- a chimera
    that measured neither -- and was redone with HEAD's own code on both sides:
    - Sentence-final regex: a `999/999.` appended to `state/global.md`. **OLD sweep (HEAD bytes)
      on those exact bytes: `SWEEP CLEAN`, exit 0.** NEW sweep: exit 1,
      `live suite claims disagree with each other: [108, 999]`.
    - Derived file list: a `999/999 green` appended to `state/tests.md`, staged alone. **OLD hook
      (HEAD bytes) on that index: 0 bytes of output, exit 0** -- its trigger never fired, because
      nothing named `state.md` was staged. NEW hook: exit 1 with `SUITE COUNT AGREEMENT` and
      `RECORD OWNERSHIP`.
    - **The healthy path was measured too, in both directions**, because five ways to fail and
      zero ways to stay quiet is how a guard goes red in CI: before the breaks and again after
      reverting them, the hook printed `banner sweep: clean (6 staged file(s) under
      Tools/architect/state)` and exited 0.
    - **The proof never touched the user's index.** It ran against a scratch `GIT_INDEX_FILE`, and
      invoked the hook directly rather than through `git commit`, so no commit was created,
      refused or otherwise. One trap found doing it: leaving `GIT_INDEX_FILE` exported after
      deleting the scratch index makes a following `git status` report the entire tree as staged
      deletions. That is a display artifact of the harness, not a change to the index -- checked
      again in a shell with the variable unset.
  - **Self-test grew from 15 fixtures to 23**, six of them new for the split: the sentence-final
    shape, two-files-disagreeing, an owner file restating a figure `global.md` owns, an owner file
    legitimately quoting a stamped one, an item called open in one file and closed in another, a
    banner made stale by an entry in another file -- plus a fixture proving a NEW `.md` dropped
    into the directory is swept **without editing the script**, and one proving a missing or empty
    directory is reported rather than swept as clean.
  - **A fixture can be inert for the same reason a guard can.** The cross-file item-clash fixture
    passed twice while proving nothing: it was worded "not discharged", which contains the word
    the item's CLOSED shape matches, so the paragraph hit both shapes and was exempted as
    in-bullet nuance. That exemption is correct behaviour. The fixture was reworded to
    "still open", and the explanation moved OUT of the fixture string into a comment -- because an
    explanation mentioning the closed word puts it straight back into the paragraph, which is how
    the second failure happened.
  - **`check_banner_date` had to learn ownership too, and a false finding taught it.** Written to
    take the FIRST `_Last run` in the set, it found the one this very entry QUOTES as evidence in
    `data.md`, compared it against `content.md`, and reported the record stale against a string
    that was never a banner. The banner is now the one in `global.md` -- the file that owns it --
    and a `_Last run` anywhere else is prose. A `global.md` with no banner at all is a finding
    rather than a skipped check, since a skipped check reads as a passed one.
- **FIXED 2026-08-20 -- `hooks/install.sh` died in a linked worktree, and the skill's claim about
  it was backwards.** The script computed `HOOK_DIR="$REPO_ROOT/.git/hooks"`, but in a worktree
  created by `git worktree add`, `.git` is a 55-byte FILE: `mkdir -p` failed with
  `mkdir: cannot create directory 'E:/MultiAgent/Strat-wt/slot-1/.git': Not a directory` and
  `set -eu` exited 1. Reproduced against HEAD's copy in slot-1 this pass. It now resolves the
  hooks directory with `git rev-parse --path-format=absolute --git-common-dir`, and the fixed
  script run from slot-1 reported
  `updated an older copy of this project's hook: E:/MultiAgent/Stratocracy/.git/hooks/pre-commit`,
  exit 0.
  - **`--path-format=absolute` is load-bearing and was measured, not assumed.** Bare
    `--git-common-dir` answers `.git` -- RELATIVE -- from the main tree, and an absolute path from
    a worktree (git 2.53.0). A relative answer resolves against the caller's cwd, which is not
    necessarily the repo root, so the bare form would have swapped one wrong path for another.
    The older-git fallback beneath it is **unverified**: no git older than 2.31 was available to
    run it against, and it is marked as such in the script rather than presented as tested.
  - **The skill's claim that a new worktree has no hook protection is FALSE and is corrected.**
    Hook lookup in a linked worktree resolves through the common directory, so the copy installed
    once in the main clone runs in every worktree of it -- `git rev-parse --git-path hooks` in a
    probe returned `E:/MultiAgent/Stratocracy/.git/hooks` and the hook fired, refusing the same
    staged bytes the integration tree refused. It is once per **clone**, not once per worktree.
  - **Running `install.sh` from a worktree installs THAT tree's copy of the hook**, because `SRC`
    follows the caller's `--show-toplevel` while `HOOK_DIR` is now correctly shared. Doing exactly
    that during this pass silently replaced the new hook with slot-1's older one (slot-1 is
    detached at `3314461`). Reinstalled from the integration tree and confirmed byte-identical
    with `cmp`. The shared hooks directory means the LAST tree to run the installer wins.
- **FIXED 2026-08-20 -- `architect.py` would have destroyed the frozen history on its next run.**
  `write_state()` does a whole-file `path.write_text()` of a generated BUILT/DECISIONS/NEXT stub,
  and it was still aimed at `Tools/architect/state.md` -- a file that had long since stopped being
  a generated stub and, as of this pass, is the frozen record of fourteen closed phases. One run
  would have replaced all of it with `- nothing outstanding`. This is a latent defect that
  PREDATES the split; the split only changed what it would have destroyed. The caller now writes
  the stub into the run's own directory, and `write_state` refuses outright to overwrite a file
  that does not carry its own generated-by signature. **Not measured:** the refusal path was not
  exercised by running `architect.py`, which would have written a run directory; it is a read of
  the code and a guard added, not a demonstration.
- **The CI workflow needed no change, and that is the point.** `.github/workflows/banner-sweep.yml`
  invokes the sweep with no path argument, so it picked up `state/` automatically. Its comment
  claiming `--self-test` runs 14 fixtures was stale and is corrected to 23, with a note that the
  number there is orientation only -- the step runs whatever the script declares and fails on any
  wrong fixture, so a stale count there cannot weaken the gate.

- **GATED 2026-08-20, `VERDICT: BLOCK`, five findings, all five fixed in place -- and every one of
  them was in a guard or a brief, not in game code.** That is the fifth consecutive gate to land
  that way, and the reviewer said so. The pass itself (record split, sweep, hook, `install.sh`)
  survived the audit: the reviewer independently proved the sweep falsifiable on REAL bytes rather
  than fixtures, proved a brand-new `state/ux.md` is swept with no script edit, proved a missing
  and an empty record directory both refuse, and proved the vendored-citation check above the
  replaced block still fires. What it found was around the edges.
  - **F5, the only code finding, and it was real. `pre-commit`'s EXIT trap leaked its temp
    directory on a tree with no `Source/StratRules/*.h`.** `HITS` and `FAILED` are assigned only
    inside `if [ -n "$UNITS" ]`, while `STAGED` and `STAGEDIR` are pre-initialised; the trap names
    all four, so under `set -u` it aborted MID-LIST on `$HITS` and never reached
    `rm -rf "$STAGEDIR"`. The hook still exited 0 and still printed `banner sweep: clean`, so
    nothing revealed it. **Reproduced with a control before fixing and re-measured after**, in a
    throwaway repo:

    | condition | before | after |
    |---|---|---|
    | `UNITS` empty | `line 1: HITS: unbound variable`, temp dir left behind | silent, nothing left |
    | `UNITS` non-empty (control) | silent, nothing left | silent, nothing left |

    Fixed by initialising `HITS=""` and `FAILED=""` beside `STAGED`. **This is the FOURTH time in
    this one file that a fix landed at one site and missed its sibling** -- the `ACMR` filter, the
    two `mktemp` guards, and now the trap. The comment above the trap asserted "they are all in
    scope by here", which was false, and a prose block claiming a safety property that does not
    hold is worse than no comment: it is what stopped this being noticed while the file was being
    edited. Corrected to state the precondition instead.
  - **F1 -- the fresh-session resume path pointed at the file this pass froze, and told the
    steward to edit it.** `strat-hotseat`'s "resuming in a fresh session" section still read the
    current phase from `Tools/architect/state.md` and dispatched `strat-data-steward` to update
    it, which `.claude/agents/strat-data-steward.md` now forbids in as many words. Adding the
    parenthetical `(frozen history)` in this same pass made it *look* reviewed without making it
    correct. Now points at `state/global.md`, with the correction recorded in place.
  - **F2/F3 -- `CLAUDE.md` still described the pre-split world**, and it is loaded by every agent
    on every dispatch, so it was the most-read stale pointer in the tree. It named `state.md` as
    the live record, and its lane table said the steward owns all of `Tools/architect/` -- which
    now contradicts `state/engine.md`, `state/tests.md` and `state/content.md`, whose own headers
    name three *other* agents as sole writer. Both corrected; the lane table now states the
    exception explicitly.
  - **F4 -- two of the six record files named a sole writer that had never been told.**
    `grep -c 'Tools/architect' .claude/agents/strat-test-author.md .claude/agents/strat-editor-builder.md`
    returned `0` and `0`. Declaring an owner in a file the owner has never read is a lane that
    exists only in the coordinator's head. Both briefs now carry their own record section, and --
    because the reviewer measured that a *correct*, present-tense `108/108` in `tests.md` is
    refused by `RECORD OWNERSHIP` while the same figure stamped passes -- both are told the two
    legal ways to say a true thing: link to `global.md`, or stamp it.
  - **Not a finding, fixed anyway: a latent basename collision.** The hook extracts staged record
    files as `$STAGEDIR/$(basename "$f")`. `state/` is flat today, so nothing collides; a future
    `state/ui/global.md` would silently overwrite `state/global.md` in the temp dir and sweep a
    set missing a file while reporting the full count. The hook now refuses a basename collision
    rather than leaving it to be discovered.
    - **It was UNVERIFIED when written, and is verified now** -- said explicitly because a guard
      whose record does not say whether anyone watched it fire is a guard nobody has tested. The
      re-gate demonstrated it without touching the working tree, by staging a nested path into a
      scratch index alone (`git update-index --add --cacheinfo` for
      `Tools/architect/state/ui/global.md`): `COMMIT REFUSED: two record files share the basename
      global.md`, exit 1; and on the real flat six-file set,
      `banner sweep: clean (6 staged file(s))`, exit 0.
    - **It cannot fire on a legitimate flat set, and the reason is incidental rather than
      designed.** During an unmerged merge `git ls-files --cached` returns one entry per stage, so
      a conflicted record merge yields three identical `state/global.md` entries -- but the
      collision loop is never reached, because the `ACMR` trigger returns empty while the index is
      unmerged and the hook exits 0 first (and `git commit` refuses earlier still). Measured on
      the re-gate. The protection rests on that trigger short-circuit, NOT on the collision guard
      understanding merge stages, so changing the trigger would surface a message that
      misdescribes the situation. Noted in the hook itself.
  - **DISCLOSED BY THE REVIEWER, THEN CORRECTED BY IT ON RE-GATE: its own testing wrote at least
    NINE unreferenced loose objects**, not the three first reported -- three via
    `git hash-object -w`, plus a blob for each of the six then-untracked `state/` files staged
    into a scratch index, which had no object in the store yet. Measured:
    `git hash-object Tools/architect/state/global.md` gives `5fe11bb2f780...`, and that object
    appears in `git fsck --unreachable --no-reflogs`. All are unreachable and GC-collectable, and
    no ref, index or tracked file points at any of them. For scale, the repository holds 111
    unreachable objects in total, most predating this session and not attributable to the gate.
    **The understatement was the reviewer's and this record copied it faithfully, which is the
    record working correctly** -- a figure passed in from a report is only as good as the report,
    and the fix is that the re-gate re-derived it rather than re-reading it.
  - **CARRIED, NOT YET DONE -- the tooling and the record must land in the SAME commit.** With
    `state/` untracked, the hook correctly prints its "already inert" warning and exits 0, while
    CI runs the sweep with no path argument and the sweep hard-refuses a missing record directory.
    So a commit that lands `strat_banner_sweep.py` without `state/` turns CI red. Staging is the
    user's call; this is the constraint on it.

- **BUILT 2026-08-20 -- `Tools/architect/strat_doc_citation_gate.py`, because the banned citation
  shape came back in the one place no guard was looking.** A reviewer gate found `Ui.h:326` in a
  record entry. Sweeping the docs with the guards' OWN pattern found **six more live citations**,
  every one load-bearing in a current argument: five in `.agents/ue-project-context.md` (the
  units/terrain/state-hash reasoning) and one in `state/tests.md`. `Source/` was clean throughout
  -- `grep` with the CI step's exact command returned zero. **The guard worked exactly where it
  looked, and the shape simply moved to where it did not.** That is the same lesson as the
  `state.md` split in this file, arriving from the other direction.
  - **All six are now symbol references.** Each was derived by locating the ENCLOSING definition,
    not by a backward regex scan: `strat::applyCommand`'s `SaveCommandKind::Build` arm, the
    `SaveCommand` struct, `strat::seedFromScenario`'s terrain-id resolution loop,
    `strat::stateHash`'s accumulator, `Save.h`'s file-header note, and `strat::chooseBuild`'s
    affordable-set-then-`buildPriorityLess` reduction. **`seedFromScenario`'s signature wraps
    across two lines**, so the recorded `^\w.*{$` shape would have named `openTurn`, the function
    before it -- the trap this project logged four times in one day, avoided here by walking to
    the definition whose body contains the target.
  - **ONE OF THE SIX IS IN ANOTHER LANE'S FILE, AND THAT IS DISCLOSED RATHER THAN QUIET.**
    `state/tests.md` names `strat-test-author` as sole writer under the model published earlier in
    this same pass. No crew agent was dispatched for this, so the coordinator made the edit. It is
    a one-clause citation fix that changes no claim; its owner should know it happened.
  - **WHY THE DOC RULE CANNOT BE THE `Source/` RULE.** `Source/` bans the shape outright. A
    document cannot: this record's convention is that an old claim STAYS, so its honest accounts
    of the 58 citations it removed necessarily write those citations down. **Twenty-one** such
    quotations exist today and every one is correct as history. **[CORRECTED on the re-gate: this
    said "Nineteen", counted before this pass's own two entries were written. A figure in a
    record entry goes stale exactly the way a banner does, and this one went stale inside the
    same pass that wrote it.]** A flat ban would refuse the record
    for telling the truth about a defect, which is how a maintainer learns to stop running a
    guard. So a citation is allowed when the surrounding text is TALKING ABOUT a citation and
    refused when it is MAKING one.
  - **THE WINDOW IS 258 MEASURED PLUS 42 OF DELIBERATE MARGIN, AND CALLING THE WHOLE 300
    "measured" WAS WRONG.** `strat_banner_sweep.py` uses 220 for its stamp window and 220 was
    tried first: it false-positives on the `Economy.good.cpp:60` entry in this file. **[CORRECTED
    on the re-gate, twice over: this said the exempting phrase sits "about 286 characters past
    the citation", which matched nothing anyone could re-derive. The nearest exempting word
    (`cited`) begins 253 characters past the END of the citation -- and the operative constant is
    larger still, because `re.search` needs the WHOLE term inside the window. Binary search puts
    the tightest value admitting all 21 accounts at exactly 258. So 300 is 258 plus margin, not a
    measurement.]** Still far tighter than the enclosing bullet, which here runs past 2,000
    characters.
  - **THE HOLE IS STATED AND CARRIES ITS OWN FIXTURE, because a limitation nobody wrote down is
    one somebody re-derives at the worst moment.** A live citation within 300 characters of
    citation vocabulary is allowed. `--self-test` asserts that hole explicitly, so anyone who
    closes it has to decide to.
  - **THE HOLE WAS THREE TIMES WIDER THAN IT NEEDED TO BE, and the re-gate measured the
    difference at zero cost.** The first vocabulary carried this record's general editorial
    markers -- `REFUSED`, `CORRECTED`, `STAMPED`, plus `rendered as`, `restored as`,
    `still carried`, `rotted` -- words that appear in entries about anything at all. The reviewer
    smuggled live citations past every one of them in prose this record plausibly contains --
    a `CORRECTED`-stamped bullet carrying a live `Replay.good.cpp` line, a sentence mentioning a
    `REFUSED` commit, a `STAMPED` phase-closure note. (The example originally quoted a dated
    `[CORRECTED ...]` marker, and the banner sweep refused it: the date was a future one, and a
    freshness check cannot tell a quoted example from a real entry. The prose gives way -- same
    call as the `82 of 82 actions` rewording earlier in this pass.) Dropping the
    seven costs **nothing**: all 21 accounts stay exempt on six subject-naming terms
    (`citation|cited|cites|symbol reference|line number|planted`), and none becomes a finding.
    Re-measured independently before accepting it. `planted` is the one term that does not name
    the subject; it is kept because two real accounts depend on it alone, and it is flagged in
    the script as the weakest entry.
  - **THE HOLE WAS THEN HIT BY ACCIDENT, ON THE FIRST TRY, WHICH IS THE EVIDENCE IT IS REAL.** The
    first planted break read *"A planted live citation for the proof"* -- and the gate ALLOWED it,
    because the word "citation" in the probe's own wording tripped the exemption. **The same
    shape as the fixture that had to be reworded twice earlier in this pass:** a test whose prose
    describes what it is testing can launder the very thing it plants. The break was rewritten to
    read like a real argument, and only then did it refuse.
  - **FALSIFIABLE END TO END, THROUGH THE REAL HOOK, WITH CONTROLS.** Scratch `GIT_INDEX_FILE`;
    no commit created; `.agents/ue-project-context.md` confirmed byte-identical afterwards.

    | case | exit | findings |
    |---|---|---|
    | healthy path, nothing planted | 0 | 0 |
    | live citation planted and STAGED | 1 | 1 |
    | break on disk, FIXED version staged | 0 | 0 |
    | reverted | 0 | 0 |

    **Controls:** the old `Source/`-only command returned **0** hits on the same broken tree, and
    `strat_doc_citation_gate` appears **0** times in HEAD's hook and **0** times in HEAD's CI --
    so the coverage is new, not a rename of something that already existed.
  - **IT READS THE STAGED BLOBS**, like the rest of the hook: the whole document set is
    materialised from the index into a temp tree, not just the changed files, so the gate sees the
    documents as the commit will leave them. Row 3 of the table above is that property measured --
    a break sitting on disk unstaged does NOT refuse.
  - **A THIRD DEFECT, FOUND ONLY BY THE RE-GATE, AND IT IS THE SAME ONE AS (1) AND (2) AT THEIR
    SIBLING SITE.** The gate's own closing summary printed `DOC CITATION GATE FAILED -- a live
    vendored line citation is in the prose` on **every** non-zero exit, including both
    `NOTHING SCANNED` paths and `UNREADABLE`, none of which involve a citation. The hook's
    message had just been fixed for exactly this; the gate's was left asserting it one level
    down -- and because the hook echoes the gate's output ABOVE its own hedge, the false sentence
    is the one a reader sees first. **The two self-test fixtures that exercise those paths
    asserted only the exit code, so the guard's own tests passed while it said a false thing.**
    Both fixtures now also assert that no citation is claimed. That is the FIFTH sibling-site
    repeat in this tooling.
  - **TWO DEFECTS FOUND IN THE WIRING BY RUNNING IT, both fixed.** (1) `os.path.relpath` RAISES
    across Windows drive letters, and the hook runs the gate exactly that way -- temp tree on
    `C:`, `--vendored-dir` on `E:`. The traceback aborted the gate, the hook read the non-zero
    exit as a finding, and it **refused a clean commit while printing a message about a citation
    that did not exist**. Failing closed was the right direction; saying the wrong thing about why
    was not. (2) That message asserted a citation unconditionally; it now says the gate refused
    and defers to the gate's own output, because a check that did not run is not a check that
    passed.
  - **The interpreter search moved UP in the hook** so the doc gate and the banner sweep share one
    copy of it. A second copy is precisely how the `ACMR` filter and the two `mktemp` guards each
    ended up fixed at one site and broken at their sibling, three times in that file. `DOCDIR`
    joins the initialisers and BOTH traps for the same reason the `HITS`/`FAILED` fix was needed.
  - **CI runs it before the `Source/` step**, with its own `--self-test` step ahead of that, on
    the reasoning that the docs are where the shape came back and so are the failure a red run
    should name first.

- **DRAFTED 2026-08-21 — a scenario-adjacency upstream request, for §2.11.6's attack-arm
  reachability and overlay-zero attribution.** `upstream-second-scenario.md` was checked and does
  NOT cover this: its subject is the content-independence SCENARIO axis and its only
  `guidedOpening` claim is that both infantry/objective hexes are Plains/Factory, never adjacency
  to an enemy. Verified against GDD §2.13.5 directly that *Longwater March* as specified would
  not have satisfied it either (guided infantry 10 columns from the nearest enemy unit). Full
  request, the git-blob-hash pin check, and the explicit `FILED IS NOT LANDED` status (drafted
  only, not filed) are at `Tools/architect/evidence/upstream-guided-opening-adjacency.md`.

- **OPEN DEBT, filed 2026-08-20, both raised by the reviewer as non-gating on the pass that
  returned `VERDICT: PASS`.** Neither was fixed. They are written down here so the next pass
  inherits them instead of rediscovering them, which is the whole reason this file exists.
  - **1. `strat_doc_citation_gate.py` does not scan itself, and it is currently the largest
    holder of the thing it was built to catch.** Measured: 8 vendored line citations in that
    file. `DOC_ROOTS` covers `.md` under `state/`, `.agents/`, `.claude/` and `CLAUDE.md`; the
    `Source/`-side guards in the hook and in CI do not reach `Tools/` at all. So the gate's own
    directory is exactly the shape it exists to close, one level over -- *the guard works where
    it looks, and the shape sits where it does not*, which is the same sentence this file already
    carries twice about other guards.
    - **Why it is debt and not a defect today.** All 8 are illustrative probe text inside
      comments that repeatedly use the word the exemption matches, so every one would be exempt
      even if the file were scanned, and none makes a live claim about current code. Nothing
      rots when the next re-vendor lands, because nothing there is load-bearing.
    - **Why it should still be closed.** "Benign today" is a property of today's contents, not of
      the coverage boundary. The next person to add a real citation to a `Tools/architect/*.py`
      docstring gets no warning from anything. Closing it means extending `DOC_ROOTS` to `.py`
      under `Tools/architect/` -- and then re-checking that the exemption still admits the probe
      text, which is the part that needs measuring rather than assuming.
  - **2. A STANDING CONSTRAINT ON HOW THIS RECORD MAY BE WRITTEN, hit twice in one pass, which is
    where a workaround stops being an incident.** The guards read quoted text as a claim, because
    neither has an exemption for "this is an example":
    - a build's action count written as a ratio was read as a suite claim, and the prose was
      reworded to spell it out in words;
    - a quoted editorial marker carrying a future date was read as a real entry date by the
      banner sweep's freshness check, and the example was reworded to carry no date at all.
    - **The rule, stated once so the next person does not derive it twice: an example quoted in
      this record must carry no date literal and no bare slash-separated pair of equal numbers.**
      Spell the figure out in words, or drop it from the example. The alternative -- teaching the
      freshness check to recognise quoted examples -- would open in a one-comparison check the
      exact exemption hole this pass spent three gate rounds narrowing in the doc gate. The prose
      gives way; the check stays strict.
  - **A third item, smaller, and it is a measurement discipline rather than a code change.** The
    coordinator's dispatch to the reviewer stated "16 changed paths"; the tree carries 21 --
    13 tracked modified plus 8 untracked. Both numbers are real: `git status --porcelain`
    collapses an untracked DIRECTORY to one entry, while `-uall` expands it. Neither is wrong;
    quoting one without saying which is. For "what will this commit contain", `-uall` is the
    question being asked. The reviewer re-derived it rather than inheriting it, which is the
    behaviour the brief asks for and the reason the error cost nothing.
- **FIXED, 2026-08-21 -- `strat_banner_sweep.py` compared the suite COUNT against
  `Saved/AutomationReport/index.json` but never checked WHICH RUN produced that artifact.** On
  2026-08-21 the banner said 140/140, the report said 140/140, and the sweep printed SWEEP
  CLEAN, but the report was written from a pre-merge tree -- it was right only because the merge
  did not happen to move the count. A count match cannot tell a report that still describes the
  tree from one that no longer does; only a timestamp can. Added a fourth check, REPORT IDENTITY:
  before the report is trusted as ground truth it must (a) carry a readable `reportCreatedOn` and
  (b) not predate any test-defining `.cpp` file on disk (measured by this script's own
  `os.path.getmtime`, not by the report's claims about itself). Either gap is a hard FAIL, never a
  warning -- an unidentifiable run has not been verified. `read_macro_census` now also returns the
  newest mtime among files it found a test macro in, and `run_sweep` / the CLI gained
  `--report-json` / `--source-dir` overrides so the check is provable against a scratch copy
  without touching `Saved/` or `Source/`, neither of which is this steward's lane.
  - **Falsifiable against the REAL artifact, not only a fixture.** Run live, unpiped, against
    this tree: `Tools/architect/strat_banner_sweep.py --explain` printed `[**REPORT IDENTITY**]
    …/Saved/AutomationReport/index.json (reportCreatedOn 2026.08.21-15.17.35, written
    2026-08-21 11:17:35) predates a test-defining source file modified 2026-08-21 13:43:07 --
    this report is evidence about a PAST tree, not the current one, whatever its count happens to
    say.` and `SWEEP FAILED`, `echo EXIT=$?` on the immediately following line printed `EXIT=1`.
    This is genuine, unplanted drift -- `strat-test-author` is concurrently editing
    `Source/StratPlay/Tests/` in this same tree this session, and the checked-in report predates
    that edit. (The same run also found the report's `140` disagreeing with a `141` macro
    census, an independent, pre-existing drift this steward does not own and did not fix here.)
  - **Healthy path stays quiet, proven on a controlled pair, not asserted.** Four new
    `--self-test` fixtures (in `check_identity_self_test`, called from `check_self_test`): a
    report written AFTER its source PASSES; a report written BEFORE a later source edit --the
    real shape above-- FAILS; a report missing `reportCreatedOn` FAILS; a report with no
    test-defining source found to compare against FAILS. `python
    Tools/architect/strat_banner_sweep.py --self-test` -> `SELF-TEST: ALL FIXTURES CORRECT`,
    `echo EXIT=$?` on the next line -> `EXIT=0`, all 27 fixtures (23 prior + 4 new) reported
    `[OK]`, none silently skipped.
  - **Exit code measured on the failing run, not inferred.** Both the live-tree FAIL above and a
    piped-nothing check were done with the exit code captured on the line immediately after the
    command, never through a pipeline that would launder it -- the crew-gate false-0 this record
    already names elsewhere is exactly the failure mode a piped exit code would have hidden here.
  - **Nothing outside this steward's lane was touched to prove any of this.** No file under
    `Saved/` or `Source/` was edited, doctored, or reverted; the real-artifact proof reads the
    tree as it already stood, and the self-test proof runs entirely inside `tempfile` scratch
    directories via the new `--report-json` / `--source-dir` overrides.
