# Gate: unit damage alert, FOURTH PASS — strat-integration-reviewer

Base commit `7e83295`, branch `master`, everything unstaged. Gated 2026-09-06, after the third
gate (`Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate-2.md`, `VERDICT: BLOCK`,
one finding against `Tools/architect/state/global.md`) was worked. Its three predecessors stand
byte-identical; this is a new file, as the standing rule requires. A verdict describes a tree,
not a phase.

## Scope

Working-tree diff against `7e83295`. **17 paths**: 12 modified, 5 untracked (the fifth untracked
path is the third gate's own report, which did not exist when it wrote its scope).

    Source/StratPlay/StratMatchSubsystem.cpp             M
    Source/StratPlay/StratSoundDirector.cpp              M
    Source/StratPlay/StratUnitActor.cpp                  M
    Source/StratPlay/StratUnitActor.h                    M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.h   M
    Source/StratUI/StratSoundCues.cpp                    M
    Source/StratUI/StratSoundCues.h                      M
    Source/StratUI/Tests/StratSoundCueClauses.cpp        M
    Tools/architect/state/engine.md                      M
    Tools/architect/state/global.md                      M   <- the only file that moved this round
    Tools/architect/state/tests.md                       M
    Source/StratPlay/Tests/StratDamageFlashClauses.cpp   ??
    Source/StratUI/Tests/StratDamagedUnitsClauses.cpp    ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-gate.md      ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate.md    ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate-2.md  ??

**"Only `global.md` moved" was MEASURED, not accepted from the brief.** `stat -c '%y %n'` over
every path `git status --porcelain` names:

    2026-09-06 13:24:01  Source/StratPlay/StratUnitActor.cpp
    2026-09-06 13:24:01  Source/StratPlay/StratMatchSubsystem.cpp
    2026-09-06 13:33:26  Source/StratPlay/Tests/StratRouteTweenUnitDouble.h
    2026-09-06 13:41:08  Source/StratUI/Tests/StratDamagedUnitsClauses.cpp
    2026-09-06 13:45:35  Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp
    2026-09-06 13:57:53  Tools/architect/gate_reports/...-gate.md
    2026-09-06 14:02:15  Source/StratUI/StratSoundCues.cpp
    2026-09-06 14:02:25  Source/StratUI/StratSoundCues.h
    2026-09-06 14:02:38  Source/StratPlay/StratUnitActor.h
    2026-09-06 14:10:25  Source/StratPlay/Tests/StratDamageFlashClauses.cpp
    2026-09-06 14:16:21  Source/StratUI/Tests/StratSoundCueClauses.cpp
    2026-09-06 14:19:14  Tools/architect/state/tests.md
    2026-09-06 14:32:04  Tools/architect/gate_reports/...-regate.md
    2026-09-06 14:35:01  Source/StratPlay/StratSoundDirector.cpp
    2026-09-06 14:42:26  Tools/architect/state/engine.md
    2026-09-06 14:54:09  Tools/architect/gate_reports/...-regate-2.md
    2026-09-06 14:54:57  Tools/architect/state/global.md

Exactly one path is newer than the third gate's report (14:54:09): `global.md` at 14:54:57.
`StratSoundDirector.cpp` 14:35:01 and `engine.md` 14:42:26 are the figures the third gate itself
recorded, unchanged. **No source file moved this round**, and `git diff --numstat` over the record
directory confirms the record side too: `engine.md 276/2`, `tests.md 202/0`, `global.md 140/3`,
where only the last differs from what the third gate would have measured.

`.agents/ue-project-context.md` was read in full. Its `StratBridge` row already states the
monolithic-only link edge and matches the derivation below; no correction needed.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — zero hits; no candidate to judge |
| 3 | No vendored header in front of UHT | pass — zero reflected headers include any vendored header |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — the only `TUniquePtr<FStratBridge>` in the tree is inside a comment |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 field names, derived not quoted |
| 6 | New modules registered (unless no module object) | pass — no new module directory; `Source/` still holds five |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in gameplay C++ | pass — every hit outside `Tests/` is comment prose |
| 9 | No widget-side arithmetic | pass — no widget file moved this round or last |
| 10 | Lane discipline | pass — the one file that moved is the `coordinator`'s own |
| 11 | Prose blocks present | pass — no new file this round |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` printed nothing |
| A | Task 1 — the blocking finding is closed | **partly** — the stale verdict and the stale site count are gone; a NEW rotting claim arrived in the same paragraph. See finding 1 |
| B | Task 2 — did the deletion go too far / not far enough | pass — stating no verdict is within this file's own ruled precedent; the POINTER is the defect, folded into finding 1 |
| C | Task 3 — the entry's remaining claims true of the tree | pass — every one checked against source, `engine.md` and the exported report |
| D | Task 4 — only `global.md` moved, measured not accepted | pass — mtime table above |
| E | Task 5 — the not-verified account accurate and unsoftened | pass — five claims, five instruments, below |
| F | Standing pass-specific re-checks re-run against the current tree | pass |

### Instruments, inlined

**Checklist 1, 7, 12.** `git diff --stat -- Source/StratRules Data`, `git diff --stat --
Source/Stratocracy` and `git diff --cached --stat` each printed nothing.

**Checklist 2.**

    grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ |
      grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'

Zero hits. Not one legal candidate to judge either, which is worth stating so a reader can tell
this from an unrun check.

**Checklist 3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS`/`USTRUCT`/`UENUM`
was tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or
`Replay.h`. **Zero hits.** (The third gate's one hit was `StratBridge.h` itself, matched on its
prose; the line-anchored form used here excludes it at the source, and that file still declares no
reflected type.)

**Checklist 4.** `grep -rn 'TUniquePtr<[[:space:]]*FStratBridge' Source/` returns one line,
`Source/StratUI/StratViewModel.h:47`, which reads `// holds a bridge, so the C4150 measured on
TUniquePtr<FStratBridge> -- recorded in`. Prose about the measurement, not a member. The real
members are `StratMatchSubsystem.h:2614` `TPimplPtr<FStratBridge> Bridge;` and
`StratScoreboardHUD.h:1253` `TPimplPtr<FStratBridge> OwnedBridge;`. Judged, not forwarded.

**Checklist 5, module arrows, derived from the five `.Build.cs` files and not quoted.** The
standing derivation was extracted from the reviewer brief and executed, comments stripped, both
call forms matched. **16 rows, all five modules present:**

    StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
    StratBridge  PrivateDependencyModuleNames   (none)
    StratBridge  PrivateDependencyModuleNames   StratRules
    StratBridge  PublicIncludePaths             StratBridge,StratRules
    StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
    StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
    StratPlay    PublicIncludePaths             StratPlay
    StratRules   PublicDependencyModuleNames    Core
    StratRules   PrivateDependencyModuleNames   (none)
    StratRules   PublicIncludePaths             StratRules
    StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
    StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
    StratUI      PublicIncludePaths             StratUI
    Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                                AIModule,NavigationSystem,StateTreeModule,
                                                GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
    Stratocracy  PrivateDependencyModuleNames   (none)
    Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...

`StratUI` takes no `StratPlay` edge and no `Stratocracy` edge; **no cycle.** The sixteenth row was
read in source rather than treated as unconditional — `StratBridge.Build.cs:134-138`:

    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

Guarded, monolithic-only, from the same one expression that drives the define. The unconditional
`PrivateDependencyModuleNames.AddRange(new string[] { })` at `:75` is the empty row above it.
NOT drift.

**Field census, ten names and no eleventh:** `BRIDGE=` (the known false positive — the tail of the
string literal inside `PrivateDefinitions.Add`, left unfiltered deliberately), `PCHUsage =`,
`PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`,
`PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`,
`bUseUnity =`.

**Checklist 8.** `grep -rn '/Game/' Source/ --include=*.cpp --include=*.h`, minus `Tests/` and
minus the import commandlet, returns 47 lines and **every one is inside a comment** — most of them
headers stating in terms that they name no such path, e.g. `StratUnitActor.h:231`, *"- Any
`/Game/` path. Every mesh and material is an EditDefaultsOnly property."* The `Tests/` hits are the
standing fixture exception, `LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"))`. Judged, not
forwarded.

## Task 1 — is the blocking finding closed?

**Half of it, cleanly; the other half was replaced by a new instance of the same defect class.**

**Closed: the stale verdict.** `grep -n 'VERDICT' Tools/architect/state/global.md` returns three
hits inside the damage-alert banner (lines 76, 79, 84) and none of them is a live claim:

    76: THIS ENTRY STATES NO GATE VERDICT, AND THE ABSENCE IS DELIBERATE.
    79: An earlier form of this paragraph opened `THE GATE RETURNED VERDICT: PASS WITH ZERO
    80: FINDINGS`, and a later gate then returned `BLOCK` with a finding AGAINST THIS VERY SENTENCE

**The surviving `VERDICT: PASS` quotation is correctly marked as history and is NOT a finding.**
It is (a) preceded three lines earlier by the paragraph's own opening sentence denying any verdict,
(b) introduced by *"An earlier form of this paragraph opened"*, (c) fenced in backticks, and
(d) followed inside the same sentence by the later `BLOCK` that superseded it. That is the shape a
correction is supposed to take — it announces itself AT the false sentence rather than below it. A
reader arriving cold at line 79 cannot read it as live.

**Closed: the stale site count.** `grep -ni 'six sites' Tools/architect/state/global.md` returns
nothing inside the damage-alert entry; its one hit, line 856, belongs to an unrelated 2026-09-03
banner about stale debt prose. In its place, line 94: *"NO SITE COUNT IS STATED HERE"*, with the
running count deferred to `engine.md`, which carries it at seven — `engine.md:100-101`, *"SEVEN
sites, not six, and the method named here is why the seventh was missed"*. The two records no
longer disagree, because only one of them states a number. `grep -c '444'` on `global.md` returns
0; the second gate's finding stays closed.

**NOT closed — a new rotting claim in the very paragraph written to remove one.** See finding 1.

## Task 2 — did the deletion go too far, or not far enough?

**Stating no verdict is not an under-claim, and it is not passing a buck in principle.** This file
has already ruled the shape, in its own words, and a reader of `global.md` alone still gets a
usable answer: the pass was gated, the pass is uncommitted, and the verdicts are persisted
artifacts elsewhere. The precedent, `global.md:529-534` (the 2026-09-04 difficulty-ownership-pin
banner):

    THIS PASS WAS GATED, AND THE GATE REPORTS ARE THE FILES MATCHING
    `Tools/architect/gate_reports/2026-09-04-difficulty-ownership-pin*.md`. ... No number of
    them is written here and none is enumerated -- that set was still growing while this was
    written, and a closed statement about a moving subject is false on landing

**So the deletion did not go too far. It went not quite far enough in one place: the pointer, and
the count that came with it.** The ruled shape names a FILENAME PATTERN; this entry names the bare
directory. `ls Tools/architect/gate_reports/ | wc -l` returns **72**. A reader of `global.md` alone
is handed 72 files and no way to tell which of them are this pass's — and that is the difference
between a pointer and a buck. `Tools/architect/gate_reports/2026-09-06-unit-damage-alert*.md` costs
the same line and resolves to exactly the right set however many rounds it grows to.

**On whether it should carry a verdict once the tree stops moving: yes, and this file already
models when.** Its closed entries do state one — e.g. line 6710, *"THE GATE RETURNED `VERDICT:
PASS` WITH ZERO FINDINGS ON ITS THIRD ROUND, and that is the verdict for this pass"*. The condition
that licenses that sentence is that the set has stopped growing; in practice, the commit. Today it
is still growing, and this report is the artifact proving it — **finding 1 exists precisely because
the entry made a closed statement about that set.** Recorded as an obligation, not blocked on.

## Task 3 — the entry's remaining claims, against the tree

Every substantive claim in the damage-alert entry was checked. All hold.

- **`THE SUITE IS 445/445 ... reportCreatedOn 2026.09.06-18.17.58`.** `Saved/AutomationReport/index.json`
  read with `utf-8-sig`: `reportCreatedOn 2026.09.06-18.17.58`, `succeeded 445`, `failed 0`,
  `notRun 0`, `succeededWithWarnings 0`, entry array **445** long. Independent macro census,
  `grep -rho 'IMPLEMENT_[A-Z_]*AUTOMATION_TEST' Source/ | sort | uniq -c` gives `445
  IMPLEMENT_SIMPLE_AUTOMATION_TEST` across 92 files and zero of any other macro form.
  `435 + 10 = 445`. Two instruments, one figure.
- **`strat_banner_sweep.py` SWEEP CLEAN.** Run by this reviewer rather than accepted:
  `SWEEP CLEAN -- no self-contradiction found`, `EXIT=0`, `automation report: 445 entries, all
  Success (2026.09.06-18.17.58)`, `macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=445`,
  `suite claims found: 64 (1 live, 63 stamped)`. The script was checked for write handles first and
  has none. **Note what it does NOT vouch for:** it collects `N/N`-shaped claims, and finding 1's
  subject is a bare quantifier — the sweep is blind to it by construction, exactly as this file's
  own carried debt at line 535 states.
- **`It is now the sole caller of SetActorTickEnabled(false) in the class.`** TRUE.
  `grep -rn 'SetActorTickEnabled' Source/` outside `Tests/`: three `true` call sites
  (`StratUnitActor.cpp:509`, `:785`, `:1044`) and exactly one non-`true` argument,
  `StratUnitActor.cpp:1167`, `SetActorTickEnabled(TweenOffsets.Num() > 0 || DamageFlashElapsed >
  0.0f);` inside `UpdateTickEnabled`. Every remaining hit is comment prose.
- **The two flash triggers sit OUTSIDE `FindSoundDirector()`'s `if`.** TRUE at both sites, read in
  source. `StratMatchSubsystem.cpp:1142-1175`: the loop `for (const int32 DamagedId : Damaged) {
  ... Actor->PlayDamageFlash(); }` sits inside `if (!bTourExistenceHeld)` and AFTER the director
  block closes, with the reason written above it — *"OUTSIDE, BECAUSE A PROJECT WITH NO SOUND BANK
  MUST STILL FLASH"*. `:2381-2387`: `if (Current.Kind == EStratAiCommandKind::Attack &&
  Current.TargetId >= 0) { if (AStratUnitActor* const Defender = FindUnitActor(Current.TargetId))
  { Defender->PlayDamageFlash(); } }`, with the nearest `FindSoundDirector()` `if` at `:2327`
  closed before it. Not nested.
- **`DamageFlashSeconds ships 0.0f`.** `StratUnitActor.h:1240`, `float DamageFlashSeconds = 0.0f;`.
- **`DamageFlashMaterial ships UNSET`.** `StratUnitActor.h:1313`,
  `TObjectPtr<UMaterialInterface> DamageFlashMaterial;` — declared with no initializer.
- **The build-debt account, and who ran what.** `engine.md:189-212` matches the banner in every
  particular. The debt is STAMPED IN PLACE at the false words — **[DISCHARGED 2026-09-06 AT THE
  WORDS "NOT build-verified" ABOVE ...]** — and the bullet below it opens *"THE HALF THAT MUST NOT
  BE BLURRED IS THAT THIS LANE DID NOT RUN THE BUILD"*, then attributes inline: *"The `coordinator`
  (ACTING) confirmed the editor gone -- `Get-Process UnrealEditor` returning nothing -- and ran the
  build; `strat-gameplay-engineer` (WRITING) compiled nothing, launched nothing, and is recording a
  result it was handed rather than one it took"*. The banner's *"`Result: Succeeded`, one
  translation unit, zero warnings"* is exactly what that bullet's verbatim tail carries:
  `[1/4] Compile [x64] StratSoundDirector.cpp` through `Result: Succeeded`. Acting and writing are
  separated INLINE, in the entry, where a reader arriving by a citation lands. Neither half
  overclaims, and `engine.md:213-222` separately argues why a one-TU build covers this round rather
  than assuming it does.
- **`This seat wrote no code and no clause, and this entry is not a transcription of anything.`**
  Consistent with the mtime table: the source files and `tests.md` predate the coordinator's window,
  `engine.md` is the engineer's at 14:42:26, and the only file at 14:54:57 is `global.md`, whose
  declared writer is the `coordinator`. No exception clause is invoked and none is needed.
- **`StratBridge.cpp:575` for counter-damage to the attacker.** Accurate: that line reads *"a death
  can be the ATTACKER (the counter arm erases it when `atkHp - counter <= 0`)"*, which is the fact
  it is cited for.

## Task 5 — what is NOT verified, item by item

Each of the five is accurate and none is softened.

1. **Nothing visual; no human has seen it.** `DamageFlashMaterial` ships unset (above), `Content/`
   is absent from `git status` entirely, and the banner says it flat: *"THE FEATURE IS INVISIBLE IN
   PIE AND NO HUMAN HAS SEEN IT"*, with a named discharge condition — *"a human at the keyboard
   reports seeing a damaged unit flash and shake"* — and the overlay-compile caveat attached.
2. **No clause pins amplitude, frequency, X/Y ratio, decay shape, smoothness or redness.** TRUE by
   construction: `TickDamageFlash` is the only non-zero writer of `Shake` and no test advances it.
   `grep -rn 'TickDamageFlash\|->Tick(\|TickActor' Source/StratPlay/Tests/ Source/StratUI/Tests/`
   returns `TickDamageFlash` only in comment prose (`StratDamageFlashClauses.cpp:6`, `:26`, `:545`,
   `:574`; `StratRouteTweenUnitDouble.cpp:31`, `.h:149`), and the only live `->Tick(` calls in the
   whole test corpus are `StratHoverInputClauses.cpp:1066` and `:1094` on the player controller —
   a different actor and a different feature. **No test-only advance seam exists.**
3. **The marker re-parent is UNMEASURED against `BP_StratUnit`.** The banner disclaims the green
   build as evidence in terms — *"that is NOT evidence about reinstancing -- the editor was never
   opened in this pass"* — and no `Content/` path appears in `git status`. Accurate.
4. **The tour path is DEFENDER-ONLY.** Confirmed in code and, more sharply, in the code's own
   comment: `StratMatchSubsystem.cpp:2371-2380` states *"NO FIELD OF `FStratAiPlaybackStep` REPORTS
   THAT ... SO THE FLASH IS COMPLETE ON THE NO-TOUR PATH AND DEFENDER-ONLY ON THE TOUR PATH"*, and
   the code reads `Current.TargetId`, not `Current.UnitId`.
5. **The `static_assert` against a `Count` sentinel is NOT WRITTEN.**
   `grep -rn 'static_assert' Source/StratUI/ Source/StratPlay/` returns hits in exactly two files:
   the three real ones in `StratViewModel.cpp` (`:207`, `:214`, `:223` — pre-existing, about
   `strat::OWNER_NEUTRAL`, `SIDE_COUNT` and `SIDE_NONE`), and one prose line at
   `Source/StratUI/Tests/StratSoundCueClauses.cpp:1115` recording that the stronger form was not
   written. **Zero `static_assert` in `StratSoundCues.h`.** The banner states it as open and routes
   it correctly.

## Task 4 / F — pass-specific re-checks, re-run against the current tree

- `ApplyUnitView` is UNEDITED: it appears in the diff only inside comment text.
- The `ApplyView` flash loop sits inside `if (!bTourExistenceHeld)` and outside the director's
  `if` (quoted above); the tour flash is outside both and keys on `Current.TargetId`, guarded `>= 0`.
- `FinishDamageFlash` retires through `SetRelativeLocation_Direct(FVector::ZeroVector)` and
  `UpdateComponentToWorld()`, ending in `UpdateTickEnabled()` rather than a direct switch-off — the
  shape `StratUnitActor.cpp:995-1000` and `.h:1488-1502` both argue for.
- `StratUI.Build.cs` names no `StratPlay` and no `Stratocracy` dependency (rows 11 and 12 above).
- No source byte moved this round, so the third gate's comment-stripped-diff measurements
  (`StratSoundDirector.cpp` = 0 changed executable lines, with a `27c27` mutant control proving the
  stripper able to speak) stand unchallenged and were not re-derived.

## Findings

**1. BLOCK — the paragraph written to delete a rotting gate claim states a COUNT of the gate
rounds, which is a rotting claim of the same class, and it is false as of this report.**

`Tools/architect/state/global.md:76-79`, in the live damage-alert banner:

    THIS ENTRY STATES NO GATE VERDICT, AND THE ABSENCE IS DELIBERATE. A verdict describes A TREE AND
    NOT A PHASE, and this pass was gated THREE TIMES over a tree that kept moving -- each round's
    findings were worked, which moved the tree, which invalidated the verdict that had just been
    written here.

This pass has now been gated **four** times. `Tools/architect/gate_reports/2026-09-06-unit-damage-alert*.md`
resolved to three files when that sentence was written and resolves to four the moment this report
lands — so the falsifying artifact is IN THE TREE, produced by the very mechanism the sentence
describes, and a reader can re-run the glob and see it.

**This is not a novel judgement. It is this file's own ruling, stated twice, about this exact
shape.** `global.md:532-534`:

    No number of them is written here and none is enumerated -- that set was
    still growing while this was written, and a closed statement about a moving subject is false on
    landing

and `global.md:658-660`:

    **It NEITHER
    ENUMERATES NOR COUNTS the re-gates** -- no number of them, no list of them, no closed
    statement of their verdicts -- because that set was still growing while this was written.

The third gate prescribed the same remedy in the same words — *"point at the report FILENAME
PATTERN ... and neither count nor enumerate the re-gates"*. The verdict half of that remedy was
taken. The count half was not, and the sentence reintroducing it is the sentence explaining why
counts of this kind rot.

**A second, smaller half of the same finding, one line down.** `global.md:84-85`:

    THE VERDICTS LIVE WHERE THEY CANNOT ROT: `Tools/architect/gate_reports/`, one persisted file per
    round, none overwriting another, each carrying its own tree's evidence inline.

That names a directory holding **72** files. Both ruled precedents in this same file name a
filename PATTERN instead, which resolves to the pass's own set and keeps resolving as it grows.
Without it, "the verdicts live over there" is not reachable from `global.md` alone — which is the
Task-2 question answered in the affirmative for this one clause and no other.

**Why this blocks rather than sitting in Observations.** It is a false sentence in the live record,
in the only file `CLAUDE.md` permits to carry phase status, on the same axis and in the same
paragraph as the finding it was written to close — and no instrument in this tree can see it: the
sweep collects `N/N` shapes, and `THREE TIMES` is a bare quantifier, which is the blind spot this
file already carries as open debt at line 535.

**The fix is one clause and needs no re-run of anything:** drop the cardinality (*"gated repeatedly
over a tree that kept moving"*, or name no number at all) and change the pointer to
`Tools/architect/gate_reports/2026-09-06-unit-damage-alert*.md`.
**Owner: the `coordinator`**, sole writer of `global.md` jointly with `strat-data-steward`.

## Observations

Non-gating.

- **The entry states no phase verdict, which is correct TODAY and becomes an under-claim at
  commit.** This file's closed entries do state one; the condition that licenses it is that the
  gate set has stopped growing. Whoever commits this pass should carry the final round's verdict
  into the entry at that moment — not before. Recorded so the absence is not later read as a
  settled convention. Owner if taken: the `coordinator`.
- **The minus-census wording note from the third gate is unchanged and still worth eight words.**
  `StratSoundDirector.cpp` and `engine.md` both say *"over the comment-stripped file, `-` appears on
  exactly one line"*; the naive instrument returns 19, of which 18 are `->`. True about the
  OPERATOR, false about the CHARACTER, in the sentence offering itself as proof of the claim above
  it. Still not a finding, for the third gate's reasons, and still cheap to disambiguate in
  whatever pass touches finding 1. Owner if taken: `strat-gameplay-engineer`.
- **All three predecessor gate reports stand byte-identical and this one joins them.** Each records
  what a gate read in a tree that existed at the time. The third report quotes the now-deleted
  `THE GATE RETURNED VERDICT: PASS` sentence verbatim; that quotation is correct AS HISTORY and must
  not be updated to match the deletion.
- **`CancelDamageFlash` remains public with no production caller** — carried unchanged from all
  three prior gates, correctly recorded as (c) in the banner, changed by nobody.
- **The `static_assert` against a `Count` sentinel is still not written and still open**, routed to
  `strat-gameplay-engineer`. Unchanged this round.
- **`StratOptionsWidget.cpp` / `.h` still carry file- and class-scoped arithmetic claims outside
  this diff.** Measured TRUE by the third gate; not re-derived here because nothing touched them.
- **This reviewer ran no build and no suite.** The `Result: Succeeded` line is read from the output
  quoted in `engine.md`; the 445/445 figure I read myself out of the exported report and confirmed
  against an independent macro census and against `strat_banner_sweep.py`.

VERDICT: BLOCK
