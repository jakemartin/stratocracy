# Gate: options-surface clause pass, second re-gate — strat-integration-reviewer

## Scope

Working-tree diff of `E:\MultiAgent\Stratocracy` on `master` against base `374398a`, which is
`HEAD`. Nothing committed on top of it, nothing staged. **16 files**: 7 modified
(`Source/StratUI/StratOptionsWidget.cpp`, `Source/StratUI/StratOptionsWidget.h`,
`Source/StratUI/Tests/StratOptionsWidgetDouble.h`, `Tools/architect/state/content.md`,
`Tools/architect/state/engine.md`, `Tools/architect/state/global.md`,
`Tools/architect/state/tests.md`; 600 insertions, 39 deletions) and 9 untracked source files
(7 clause `.cpp`, 2 test-double headers). Two untracked gate reports from this pass are also
present and are this seat's own output, not the lane's.

**The dispatch's claim that nothing outside `global.md` moved is MEASURED, not accepted.** The
previous gate report was written at `2026-09-05 19:40`. Every file in the diff has an mtime at or
before `19:29` — `content.md` 19:29, `tests.md` 19:16, `StratTitleMenuOptionsRowClauses.cpp` 19:14,
`engine.md` 19:08, `StratOptionsWidget.cpp`/`.h` 19:01, `StratOptionsWidgetDouble.h` 17:58 —
**except `global.md`, at 19:41**, which is the only file newer than the report. `Content/` is not
in this diff at all.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing. Manifests re-read as the authority: `rulesCommit 96d93ea9...`, `dataCommit c2f58608...` |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — the lowercase-initial call pattern across `Source/`, with the two permitted directories excluded, returned **zero** hits. No candidate to judge |
| 3 | No vendored header in front of UHT | pass — the only non-vendored header including `StratBridge.h`/`Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h` is `Source/StratBridge/StratBridge.h` itself. Its six `UCLASS`/`USTRUCT`/`UENUM` hits (`:30, :119, :262, :264, :323, :486`) are every one of them inside a comment arguing why the header declares none — judged, not counted. None of the 9 new files is such a header |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the single `TUniquePtr<FStratBridge>` hit in the tree is prose at `Source/StratUI/StratViewModel.h:47` recording the `C4150` measurement |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. The extraction printed **16 rows**, the sixteenth being `StratBridge PrivateDependencyModuleNames StratRules`; `StratBridge.Build.cs:133-137` confirms it sits inside `if (!bCompileVendoredRulesHere)` off `Target.LinkType != TargetLinkType.Monolithic`, i.e. the monolithic-only link edge, which is correct and not a finding. `PublicIncludePaths` still carries `StratBridge,StratRules` — an include edge, unchanged. Field census printed **10** names, exactly the recorded set, with `BRIDGE=` present as the known string-literal false positive. No `.Build.cs` is in this diff |
| 6 | New modules registered | pass, vacuously — `IMPLEMENT_MODULE` appears in exactly the game-module `.cpp` files and `Stratocracy.uproject` lists exactly those four game modules. `StratRules` carries none and is correctly absent. No new module directory |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — with `Tests/` excluded, the only `/Game/` string hit in `Source/` is `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing commandlet exception |
| 9 | No widget-side arithmetic | pass, re-judged rather than carried forward. `StratOptionsWidget.cpp:46` carries `FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Volume * 100.0f))` — it is inside the **free function** `StratBuildAudioOptionsModel`, the model builder, not inside a widget method. `SyncBoundWidgetsToModel` assigns each text block one `FText` the model already holds and each slider one model float; no `AsNumber`, no format string, no conditional |
| 10 | Lane discipline | pass — all 9 new files are under `Source/<Module>/Tests/`. `StratOptionsWidget.h`/`.cpp` are prose-only, re-derived: `git diff -U0` on each, comment and blank lines stripped from both sides, returned zero lines. Every non-comment added line in the `Source/` diff lands in `Source/StratUI/Tests/StratOptionsWidgetDouble.h`, a test double |
| 11 | Prose blocks present | pass — 8 of the 9 new files open with 30 comment lines in their first 30; `StratCommandBarOptionsDouble.h` opens with 24. No new `.Build.cs` entries |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows no staged column, `git diff --cached --stat` is empty, `HEAD` is `374398a` |
| — | **Record accuracy (`global.md`)** | **BLOCK** — one finding below |

### Measurements re-derived rather than accepted

- `Saved/AutomationReport/index.json`, read with `utf-8-sig`: `succeeded 435`, `failed 0`,
  `notRun 0`, `succeededWithWarnings 0`, `inProcess 0`; **435** entries, every state `Success`;
  `reportCreatedOn 2026.09.05-23.18.02`. Group counts include `GATE-TITLEMENU` **42**,
  `GATE-AUDIO` **24**, `GATE-AUDIO-SETTINGS` **10**.
- Macro census over `Source/`: `IMPLEMENT_*_AUTOMATION_TEST` **435**, equal to the report.
- `strat_banner_sweep.py`: `SWEEP CLEAN -- no self-contradiction found`, `EXIT=0`, macro census
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0`, report identity matched,
  `suite claims found: 63 (1 live, 62 stamped)`.
- Report file mtime `19:18` local against `reportCreatedOn 23:18` — the recorded UTC/local offset,
  not a discrepancy. Newest clause `.cpp` is `19:14`, before the report; the sweep's REPORT
  IDENTITY check is therefore correctly green rather than accidentally so.

## The two prior findings, checked at their subjects

- **BLOCK 1 (the `WHAT IS STILL OWED` tail) — the four false claims are GONE.** A grep of
  `global.md` for the removed noun phrases returns them only inside the `[STAMPED 2026-09-05]`
  quotation at `:240-245`, which quotes the removed sentence and says why it survived the previous
  edit. That is the correct shape. **But the replacement sentence carries a new false claim, and it
  is the finding below.**
- **BLOCK 2 (the live count stated twice) — CLOSED, and the two attacks the dispatch invited both
  fail. Reasoning under Observations, because neither is a finding.**

## Findings

### BLOCK — `global.md:235` asserts an exhaustive enumeration of two unpinned properties, and the same banner names a third twenty-eight lines above it

`Tools/architect/state/global.md:235-239`, live prose inside the `_Last run 2026-09-05` banner,
carrying no stamp and no past-tense marker:

> TWO PROPERTIES ARE PINNED BY NOTHING AND BOTH ARE NAMED RATHER THAN LEFT TO BE FOUND.
> Label-from-model on the title menu's options row is unreachable by any fixture ... And
> `SuppressedByCooldown` reads `UWorld::GetTimeSeconds` while a fixture world is never ticked.

Both named properties check out. `SuppressedByCooldown` is driven by no clause —
`StratShippedSoundBankParity.cpp:90` says so in terms (*"IT DOES NOT PIN `MinSecondsBetween` OR
THE `SuppressedByCooldown` ARM"*), and no test file mentions `MinSecondsBetween` outside that
block and `:1082`. Label-from-model is confirmed at `StratShellSubsystem.cpp:240` and by the
clause file's own annotation.

**The enumeration is not exhaustive, and its incompleteness is documented in three places in this
tree.**

1. `global.md:207-213`, the SAME banner: *"A THIRD INSTANCE OF THE SAME DEFECT WAS THEN FOUND BY
   CONSTRUCTION, IN A LINE THE BATTERY NEVER RAN. The same clause asserts the row's `bEnabled`
   copy, and `IsRoutePermitted`'s `Options` arm returns `true` with no branch on any fact -- so
   that assertion compares `true` against `true` in every state a fixture can build."*
2. `Source/StratPlay/Tests/StratTitleMenuOptionsRowClauses.cpp:61-67`, which says the line
   *"used to end 'What is pinned here is the COPY', and that overclaims ... the copy assertion
   compares `true` against `true` and a constant `true` passes it. The COPY is pinned only once
   that arm can answer `false`."*
3. The same file at `:190-197`, the `(SURVIVES BY CONSTRUCTION ...)` block, stating the same
   finding against the mutant battery.

Verified in production source rather than inferred from the prose:
`Source/StratPlay/StratShellSubsystem.cpp`'s `EStratShellRoute::Options` arm is
*"ALWAYS PERMITTED, FOR A DIFFERENT REASON FROM QUIT'S AND A WEAKER ONE"* followed by an
unconditional `return true;` — no branch on any member of `FStratShellFacts`, so
`Option.bEnabled = IsRoutePermitted(Route, Facts, Refusal);` at `:248` yields `true` in every
fixture-reachable state, and `StratTitleMenuOptionsRowClauses.cpp:312` compares
`Menu->Button()->GetIsEnabled()` against `OptionsRow.bEnabled` — `true` against `true`.

So the enabled-copy is a **third** property pinned by nothing, in exactly the sense the sentence
uses: a clause exists, is green, and cannot discriminate. It sits in the same `RefreshOptionsRow`
as label-from-model and is the same defect class the banner itself labels *"THE SAME DEFECT"*.

**Why this is a BLOCK and not a quibble.** `WHAT IS STILL OWED` is the section a reader consults
to learn what is not covered, and this sentence closes the question explicitly — *"BOTH ARE NAMED
RATHER THAN LEFT TO BE FOUND"* is a promise that the list is complete. A reader who takes it will
not go looking for the third, and the third is precisely the kind of thing — a green clause
measuring nothing — whose late discovery costs trust. It is also the same failure shape as the two
previous rounds, one axis over: the last round's false claims were **bare noun phrases** and the
round before's were **bare integers**, and this one is a **bare numeral quantifier**. The sweep
collects `N/N`-shaped claims and can see none of the three. This is the third consecutive round in
which the sentence written to close a finding introduced a sweep-invisible claim of its own, and
that pattern is itself worth the banner recording.

**Two fixes are available and either closes it.** Name the third alongside the other two, or
narrow the quantifier to what is actually being claimed — the two that need a SEAM or a TICKED
WORLD, as against the enabled-copy which needs only a refusal in a production arm and is already
narrated at `:207-213`. The second is probably the truer sentence; the count should not be restated
either way, for the reason `:29-32` already gives about bare integers.

**Owner: the `coordinator`**, sole writer of `global.md` with the steward.

## Observations

Non-gating.

- **ATTACK 1 — the `:59` distinction is SOUND, not a rationalisation, and I tried to break it.**
  A `435` count over `global.md` now returns **three** hits (`:21`, `:23`, `:59`), one more than
  last round — but the third is the sentence quoting the second, which is the correct way to
  disclose it. The test that matters is not the hit count; it is whether a reader is misled about
  which figure is authoritative and what must be maintained. They are not: `:59-62` states where
  the live figure is, quotes the other line verbatim, says what class of claim it is, and records
  that an earlier form conflated them and a gate measured it false. Nothing is concealed.
  **The one place it over-claims: "does not track a future run" is true of a run outside this
  milestone and not of one inside it.** The audio milestone is not declared closed anywhere in this
  banner — the steward's third acceptance-ID ruling for it is dated today — so a further
  in-milestone phase would move the endpoint and `389 -> 435` would need editing, which `:52-54`
  records happening to the paragraph this one replaced. That is an imprecise durability adjective
  on a disclosed figure, not a false claim about the file. **It is not a finding, and I say so
  having gone looking for one.** If it is tightened, the honest form is *"does not move until this
  milestone's own figure does"*.
- **ATTACK 2 — deleting the per-phase breakdown loses nothing a reader needs, and I checked the
  recovery route rather than accepting the assurance.** The movement is fully verifiable from the
  record and the tree without it: the start point `389` is stamped in this same file at `:265`
  (*"[STAMPED 2026-09-04] The suite WAS 389/389"*), the end point is the report and the macro
  census (both 435, re-derived above), and *"all of it added and none removed"* is checkable by the
  macro census over the newly added files. The per-file detail the breakdown carried is in the lane
  record where it belongs — `tests.md:439` (*"five clauses"*) and `:572` (*"nine clauses"*) carry
  it per file, and that file's entries are phase-labelled. **This is an improvement over the
  relocation BLOCK 2 objected to, not a different failure.**
  One small imprecision worth a later edit and not a finding: `:23-26` offers the recovery route as
  *"the clause files it named are in the tree"*, and the naming was deleted with the breakdown, so
  the pointer is dangling by one step. The files are findable by acceptance ID, and `tests.md` is
  the better pointer.
- **ATTACK 3 — the rest of the new tail holds; only the count is wrong.** Both named properties
  were checked against source, not against the dispatch's summary, and both stand (above). *"both
  lanes agree"* on label-from-model is corroborated: `global.md:214-219` states it and
  `StratTitleMenuOptionsRowClauses.cpp:52-60` states it independently, with the same
  `ApplyOptionsRow(const FStratShellOption&)` discharge and the same *"named here as the change
  that would be needed, not proposed as one that should be made"*. The `[STAMPED 2026-09-05]` block
  at `:240-245` quotes the removed sentence accurately against the previous gate's own text and its
  account of why it survived is true.
- **`decisions.md:104`, answering the dispatch's question directly: leave it, and file it as
  carried debt rather than charging it to anyone.** *"No reviewer verdict. This is post-milestone
  work and has not been gated."* is a true statement about the 2026-08-19 content-independence
  pass. It is not falsified by anything in this tree, so there is no defect to charge. What it is,
  is an unstamped present-tense member of the class the sweep was built to hunt, sitting in a file
  this pass did not touch — the right handling is a line in the carried-debt ledger naming it for
  `strat-data-steward` to stamp when that file is next opened, not an edit made under this gate and
  not a finding against the `coordinator`. **The dispatch was right to withdraw the "greps to
  nothing" claim rather than defend it, and right not to edit another lane's file to make its own
  sentence true.**
- **Gate-report citations resolve.** Every `gate_reports/` path cited from
  `Tools/architect/state/*.md` is a tracked file — checked including `tests.md:301`'s
  `2026-09-05-audio-milestone-final-gate-options-and-title-music-regate.md`, which `git ls-files`
  returns. Neither of this pass's two untracked reports is cited by the record, so the
  untracked-glob defect is not present here.
- **Line endings.** `git diff` warns `LF will be replaced by CRLF` on six files. Expected under
  `core.autocrlf=true`. Not a finding.
- **On whether this round is the loop feeding on itself — stated plainly, because the dispatch
  asked.** It is not, but it is close, and the difference is worth naming. Two of the three items
  the dispatch put up for attack survived attack: the `:59` distinction and the breakdown deletion
  are both genuine repairs and I could not break either. The finding above is not a refinement of a
  previous finding — it is a claim about the tree that three separate places in the tree contradict,
  in the one section a reader goes to for what is not covered. **The code, the tests and the assets
  have passed every check on this pass for three rounds running and not one byte of them has
  moved.** If the next round's only change is `global.md:235`, it should pass.

VERDICT: BLOCK
