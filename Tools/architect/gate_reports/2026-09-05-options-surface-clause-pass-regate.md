# Gate: options-surface clause pass, re-gate — strat-integration-reviewer

## Scope

Working-tree diff of `E:\MultiAgent\Stratocracy` on `master` against base `374398a`, which is
`HEAD`. Nothing committed on top of it, nothing staged. **16 files.** 7 modified
(`Source/StratUI/StratOptionsWidget.cpp`, `Source/StratUI/StratOptionsWidget.h`,
`Source/StratUI/Tests/StratOptionsWidgetDouble.h`, `Tools/architect/state/content.md`,
`Tools/architect/state/engine.md`, `Tools/architect/state/global.md`,
`Tools/architect/state/tests.md`; 584 insertions, 29 deletions) and 9 untracked source files
(7 clause `.cpp`, 2 test-double headers).

The dispatch's claim that no source, test or asset byte moved since the previous gate is
consistent with what is here: the only `Source/` deltas against `HEAD` are the same prose-only
`.h`/`.cpp` pair and the same 9 untracked files the previous gate cleared, re-derived below.
`Content/` is not in this diff at all.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — the lowercase-initial call pattern across `Source/`, with the two permitted directories excluded, returned **zero** hits. No candidate to judge |
| 3 | No vendored header in front of UHT | pass — the only headers including `Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h` are `Source/StratBridge/StratBridge.h:53-58` and the vendored `Source/StratRules/*.h`. StratBridge.h's six `UCLASS`/`USTRUCT`/`UENUM` hits (`:30, :119, :262, :264, :323, :486`) are every one of them **inside a comment arguing why the header declares none** — judged, not counted. The 9 new/changed headers include none of the six |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the single `TUniquePtr<FStratBridge>` hit in the tree is prose at `Source/StratUI/StratViewModel.h:47` recording the `C4150` measurement |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. The extraction printed **16 rows**; the sixteenth is `StratBridge PrivateDependencyModuleNames StratRules`, and `StratBridge.Build.cs:133-137` confirms it sits inside `if (!bCompileVendoredRulesHere)` off `Target.LinkType != TargetLinkType.Monolithic`, i.e. the monolithic-only link edge, which is correct and not a finding. Field census printed **10** names, exactly the recorded set, with `BRIDGE=` present as the known string-literal false positive. No `.Build.cs` is in this diff |
| 6 | New modules registered | pass, vacuously — `IMPLEMENT_MODULE` appears in exactly four `.cpp` files (`Stratocracy`, `StratBridge`, `StratUI`, `StratPlay`) and `Stratocracy.uproject` lists exactly those four. `StratRules` carries none and is correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — with `Tests/` excluded, the only `/Game/` string hit in `Source/` is `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing commandlet exception |
| 9 | No widget-side arithmetic | pass — `SyncBoundWidgetsToModel` (`StratOptionsWidget.cpp:222-253`) assigns each of the three sliders one `Model` float and each of the three texts one `Model` `FText`; no `AsNumber`, no format string, no conditional. The one percent expression stays in the free model builder |
| 10 | Lane discipline | pass — all 9 new files are under `Source/<Module>/Tests/`. Every non-comment added line in the three modified `Source/` files lands in `Source/StratUI/Tests/StratOptionsWidgetDouble.h` (a test double); `StratOptionsWidget.h` and `.cpp` are **prose-only, re-derived**: `git diff -U0` on each, with comment and blank lines stripped from both sides, returned zero lines |
| 11 | Prose blocks present | pass — every one of the 9 new files opens with a why-first block (24-30 comment lines in the first 30) naming its subject and acceptance ID. No new `.Build.cs` entries |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows no staged column, `git diff --cached --stat` is empty, `HEAD` is `374398a` |
| — | **Record accuracy (`global.md`)** | **BLOCK** — two findings below |

### Measurements re-derived rather than accepted

- `Saved/AutomationReport/index.json`, read with `utf-8-sig`: `succeeded 435`, `failed 0`,
  `notRun 0`, `succeededWithWarnings 0`, `inProcess 0`; **435** entries, every state `Success`;
  `reportCreatedOn 2026.09.05-23.18.02`. Group counts: `GATE-TITLEMENU` **42**,
  `GATE-AUDIO` **24**, `GATE-AUDIO-SETTINGS` **10**.
- Macro census over `Source/`: `IMPLEMENT_*_AUTOMATION_TEST` **435** across 90 files — equal to
  the report, in both directions.
- **The whole +46 arithmetic at `global.md:23-29` was re-derived file by file and every term
  holds.** The 7 new clause files carry 2+1+3+1+1+2+3 = **13** macros; `StratSoundCueClauses.cpp`
  **10** and `StratSoundDirectorCallSite.cpp` **9** = 19; `StratShippedSoundBankParity.cpp` **5**;
  `StratShellOptionsRouteClauses.cpp` 2 + `StratTitleMusicLifecycle.cpp` 1 +
  `StratAudioVolumeClauses.cpp` 4 + `StratOptionsModelClauses.cpp` 2 = **9**. 13+19+5+9 = 46;
  389+46 = 435. Every addition is accounted for by NEW files, which is the direct evidence for the
  "additions only, no removals" claim at `:58`.
- The 13 new clause names extracted from the macros: **10 under `GATE-TITLEMENU`, 3 under
  `GATE-AUDIO-SETTINGS`**, the split running through `StratOptionsPresenterClauses.cpp` exactly as
  the steward's third ruling directs. The parenthetical at `global.md:23-25` is correct.
- `strat_banner_sweep.py`: `SWEEP CLEAN`, `EXIT=0`, macro census 435, report identity matched,
  `suite claims found: 63 (1 live, 62 stamped)`.

### The three prior findings, checked at their subjects

- **BLOCK 1 — CLOSED.** `global.md:30-35` now states what was read and how and restates no figure:
  a digit census of those six lines returns the single character `8`, from `utf-8-sig`. Its own
  account of the defect is true — the superseded form carried 422, 6 and 389 + 19 + 5 + 9 = 422,
  and the sweep collects `N/N` shapes only.
- **BLOCK 2 — CLOSED at its own lines, with a consequence carried into finding 2 below.**
  `global.md:55-62` contains **no digits at all**. Its substantive claim was tested rather than
  read: every one of the four movements is accounted for by macros in newly added files, so
  "an ADDITION OF CLAUSES, never a removal" is true of this milestone's four steps.
- **BLOCK 3 — CLOSED as to the sentence named.** `NO GATE HAS RUN ON THE OPTIONS SURFACE` is gone
  from `global.md`; the only surviving `NO GATE HAS RUN` hits in that file are the two `STRUCK:`
  quotations at `:2136` and `:2196` and one quotation at `:4840`. In `content.md`, the
  `NO GATE HAS RUN ON THIS PASS.` sentence is likewise deleted, and `NO CLAUSE COVERS ANY OF IT`
  is stamped in place with the unseen half kept unchanged. **But the paragraph BLOCK 3 named was
  edited at its head and not at its tail, and the tail is finding 1.**

## Findings

### BLOCK 1 — the tail of the `WHAT IS STILL OWED` paragraph is the un-updated remainder of the same edit, and it states four things the tree falsifies

`Tools/architect/state/global.md:234-239`, live prose inside the `_Last run 2026-09-05` banner
(the parenthesis opened at `:14` closes at `:239`), carrying no `[STAMPED ...]` and no past-tense
marker:

> No clause
> covers the content half; the steward has ruled the two names
> they will take -- `GATE-TITLEMENU` for the `Options` route and title music, and the newly minted
> `GATE-AUDIO-SETTINGS` for the settings slot, the mix application and the options widget. No
> title-music asset exists, no `WBP_Options` exists, and `GATE-AUDIO` still covers the seven-cue
> clauses ONLY.

Measured against the tree:

- **"no `WBP_Options` exists"** — `Content/UI/WBP_Options.uasset` exists and is **tracked**
  (`git ls-files` returns it), committed in `374398a`. The same banner says so **fifty-four lines
  above**, at `:180-181`: *"`WBP_Options` carries three labelled sliders and a Back button."*
- **"No title-music asset exists"** — `Content/StratAudio/MS_Strat_TitleTheme.uasset` exists and
  is tracked, one of sixteen tracked assets under `Content/StratAudio/`.
- **"`GATE-AUDIO` still covers the seven-cue clauses ONLY"** — `GATE-AUDIO` carries **24** clauses,
  five of them shipped-asset clauses, by name: `EveryCueInTheShippedBankHasASound`,
  `EveryCueInTheShippedBankReachesTheEngine`, `EveryShippedCueSoundCarriesTheSfxClass`,
  `EveryShippedGameModeNamesTheSameSoundBank`, `ShippedGameModesAuthorTheirSoundBank`. The banner
  narrates that widening itself at `:108-112`.
- **"No clause covers the content half"** — the five above read the shipped bank and its sounds,
  and `Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass`
  is in the report and reads the three shipped GameMode Blueprints.
- **"the two names they will take"** — future tense; 13 clauses now stand under both.

This is BLOCK 1's defect exactly, one paragraph further down. The edit updated the head of a
run-on sentence — `WHAT IS STILL OWED. NO CLAUSE COVERS ANY OF THE OPTIONS SURFACE and NO GATE HAS
RUN ON IT; both lanes named the clauses they want and they are not written. Nor has a human seen
the screen: a` became `WHAT IS STILL OWED. No human has seen the screen: a` — and left the rest
standing. The previous gate endorsed four items in this block as durable: the unseen screen, the
pixel-less commandlet, the input mode, the `ApplyOptionsRow` seam. Those four are still true and
should stay. **That gate did not read to the end of the paragraph, and the sentences after those
four are this finding.** Every false clause here is a bare noun phrase, so the sweep is blind to
it for the same reason it was blind to BLOCK 1.

**Owner: the `coordinator`**, sole writer of `global.md` with the steward.

### BLOCK 2 — the sentence written to close BLOCK 2 asserts a property of the file that the file does not have

`Tools/architect/state/global.md:62`:

> The count itself is stated ONCE, above, and nowhere else.

A count of the string `435` in `Tools/architect/state/global.md` returns **2**:

- `:21` — `The suite is **435/435**, every entry Success ...`
- `:23` — `THE COUNT MOVED 389 -> 435: **+46 added, 0 removed, 0 state changes**, in four steps ...`

Both are live, both are currently true, and **both must be edited by hand at the next suite
movement** — which is the entire property the sentence claims. This is not a quibble about
wording: the question it answers for a reader arriving cold is where the one authoritative figure
lives, and the answer the file gives about itself is wrong by one.

It matters more than an off-by-one would, because of where the figures went. BLOCK 2's finding was
that the durability paragraph *restated the running total and the size of each addition*. The
rewrite did not remove that restatement; it **relocated** it nine lines up, into `:23-29`, which
now carries +46, four steps, +13, +19 (split 10/9), +5 and +9 — six figures and a per-file
breakdown that grows by one term per phase, the shape `:52-55` diagnoses two sentences earlier as
*"a claim that acquires a new way to be wrong every time a suite runs."* Every one of those terms
is true today; I re-derived all six. The defect is that `:57-62` reads as though the arithmetic
had been dropped — *"What is durable is the SHAPE and not the arithmetic ... The count itself is
stated ONCE"* — when it was moved, so the next reader is told there is one figure to maintain and
will find seven.

**Owner: the `coordinator`.**

## Observations

Non-gating.

- **Answering the dispatch's question about the two rewrites directly: neither has become vague or
  unfalsifiable, and that is measured rather than asserted.** `:30-35` describes an act rather than
  a figure, which is the shape that usually goes unfalsifiable — but every fact it claims to have
  checked is stated elsewhere in the same file and is independently re-derivable, and I re-derived
  all of them: the entry count (435), the per-ID name counts (42 / 24 / 10), the state of every
  entry (435 `Success`), and that the additions sum to the total (13+19+5+9 = 46, 389+46 = 435).
  It is weaker than a figure and it is still checkable. `:55-62` is likewise falsifiable — one
  removed or renamed clause, or one non-`Success` entry in a published figure, would break it —
  and it survives that test on this tree. The fresh false claim is at `:62` and is finding 2; the
  rest of both rewrites holds.
- **One clause at `:58` is true only of the PUBLISHED figures, and is worth a reader knowing.**
  *"never a state change"* is correct for every figure this banner has published — but the base
  commit `374398a` records a suite that went red mid-pass on
  `GATE-AUDIO.EveryCueInTheShippedBankHasASound`, and `content.md` records the same event in its
  own words. The sentence is defensible on the reading that "this milestone's figure" means the
  published figure, and no wording change is required; this is recorded so a later reader who
  finds the red run does not read `:58` as denying it.
- **The measured-against list at `:59-62` is itself a growing chronology.** *"no audio assets,
  then assets with no clause reading them, then clauses over the shipped assets, then a base sound
  mix in `Config/`, then the title-music and volume-screen C++, then the options surface"* — six
  items where the count moved four times, gaining one item per phase. It carries no arithmetic, so
  it cannot go stale the way its predecessor did, but it is the same shape one axis over. Not a
  finding today.
- **The dispatch's sweep claim is over-broad, and one live instance survives outside a `STRUCK:`
  quotation.** `Tools/architect/state/decisions.md:104` reads
  `- **No reviewer verdict.** This is post-milestone work and has not been gated.` It is
  unstamped, present-tense, and about the 2026-08-19 content-independence phase. It is **not**
  falsified by this gate — that phase is not what is being gated — so it is not a finding here.
  But "a grep for live gate-narration across `Tools/architect/state/*.md` now returns nothing" is
  not true of the tree, and the class the sweep was hunting still has an older member in it.
  (`engine.md:723` and `tests.md:1974, :4647, :5717, :6205` use "ungated" as a design term about
  code paths, not about review; those are false positives.)
- **The new `content.md` stamp states a figure in the same sentence that defers figures to
  `global.md`.** It reads *"thirteen clauses were written the same day under `GATE-TITLEMENU` and
  `GATE-AUDIO-SETTINGS`, and the figures are `global.md`'s."* The count is correct — 13, verified
  by macro extraction over the seven new clause files — and it is a clause count rather than a
  suite count, so it is not a RECORD OWNERSHIP violation and the sweep is right to pass it. But
  the same file writes *"(the figures are `global.md`'s and are not restated here)"* eighteen
  lines above, which is its own convention, and this stamp does not follow it.
- **The live-count question, stated plainly because the dispatch asks for it.** Across the six
  files of `Tools/architect/state/`, no file other than `global.md` carries a live suite count;
  the sweep agrees (`1 live, 62 stamped`), and a `435` search over all six returns only
  `global.md:21` and `global.md:23` plus three unrelated substring hits inside byte sizes and a
  path in `engine.md`. **So the second half of the claim holds and the first half does not: the
  live count is in exactly one FILE, and in two places within it.**
- **The clause prose continues to declare its own limits well.** `StratOptionsPresenterClauses.cpp`
  opens by naming the two-ID facet split rather than the class, and
  `StratShippedOptionsWidgetClassParity.cpp` names `GATE-TITLEMENU` in its first line while
  carrying one `GATE-AUDIO-SETTINGS` clause — the file is not the subject, which is the steward's
  own stated test. Nothing here needs changing.
- **Line endings.** `git diff` warns `LF will be replaced by CRLF` on six files. Expected under
  `core.autocrlf=true`. Not a finding.

VERDICT: BLOCK
