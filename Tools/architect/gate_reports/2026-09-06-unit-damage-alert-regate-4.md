# Gate: unit damage alert, FIFTH PASS — strat-integration-reviewer

Base commit `7e83295`, branch `master`, everything unstaged. Gated 2026-09-06, after the fourth
gate (`Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate-3.md`, `VERDICT: BLOCK`,
one finding against `Tools/architect/state/global.md`) was worked. Its four predecessors stand
byte-identical; this is a new file, as the standing rule requires. A verdict describes a tree,
not a phase.

**GATE PRIORITY: CODE AND ASSETS FIRST, RECORD SECOND** — standing user ruling, followed here.

## Scope

Working-tree diff against `7e83295`. **18 paths**: 12 modified, 6 untracked (the sixth untracked
path is the fourth gate's own report, which did not exist when it wrote its scope).

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
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate-3.md  ??

`git diff --stat` totals `12 files changed, 1960 insertions(+), 48 deletions(-)` over the tracked
side; the two new clause files are untracked and outside that total.

### "Only `global.md` moved" — MEASURED on mtimes, not accepted from the brief

`stat -c '%y  %n'` over every path `git status --porcelain` names, sorted:

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
    2026-09-06 15:03:56  Tools/architect/gate_reports/...-regate-3.md
    2026-09-06 15:04:21  Tools/architect/state/global.md

The fourth gate's report landed at **15:03:56**. Exactly one path in the tree is newer:
`global.md` at **15:04:21**, twenty-five seconds later. **No source file, no test file and no
other record file moved this round.** `tests.md` 14:19:14 and `engine.md` 14:42:26 are the
figures the fourth gate itself recorded, unchanged; `StratSoundDirector.cpp` 14:35:01 likewise.
The brief's summary is confirmed against the filesystem rather than accepted.

Note also what the mtime table does NOT establish: mtimes can be forged and a same-content
rewrite still moves one. As a cross-check, `git diff --numstat` over the record directory returns
`engine.md 276/2`, `tests.md 202/0` — identical to the fourth gate's figures — and `global.md`
`146/3`, moved from that gate's `140/3`. Two instruments, same conclusion.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — RE-MEASURED; `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — RE-MEASURED; zero hits, no candidate to judge |
| 3 | No vendored header in front of UHT | pass — RE-MEASURED; zero reflected headers include any vendored header |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — RE-MEASURED; the sole `TUniquePtr<FStratBridge>` in the tree is inside a comment |
| 5 | Module arrows unchanged and correct | pass — RE-DERIVED; 16 rows, 10 field names, no eleventh, no cycle |
| 6 | New modules registered (unless no module object) | pass — RE-MEASURED; five module dirs, four `IMPLEMENT_MODULE`, `StratRules` correctly absent from the `.uproject` |
| 7 | `Source/Stratocracy/` untouched | pass — RE-MEASURED; `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in gameplay C++ | pass — RE-MEASURED; zero non-comment hits outside `Tests/` |
| 9 | No widget-side arithmetic | pass — no widget file is in the diff at all; CARRIED from the third gate for the untouched files |
| 10 | Lane discipline | pass — the one file that moved is the coordinator's own declared file |
| 11 | Prose blocks present | pass — no new file this round; CARRIED for the two clause files gated clean twice |
| 12 | Nothing staged or committed | pass — RE-MEASURED; `git diff --cached --stat` printed nothing |
| A | Task 1 — the fourth gate's finding is closed, without a new claim of the same class | **pass** — both halves closed; nothing in the replacement counts, measures or dates a growing set from inside itself; one prose imprecision is an Observation, not a finding |
| B | Task 2 — `global.md` genuinely the only file that moved, on mtimes | pass — table above |
| C | Task 3 — standing and pass-specific checks, re-measured vs carried, labelled | pass — see the column above and the carry-forward list |
| D | Task 4 — the not-verified account still accurate and unsoftened | pass — five claims, five instruments, all re-run |
| E | Task 5 — ready to commit? | pass — see the closing section |

### Instruments, inlined

**Checklist 1, 7, 12.** `git diff --stat -- Source/StratRules Data`, `git diff --stat --
Source/Stratocracy` and `git diff --cached --stat` each printed nothing. The manifests are the
authority for what "untouched" means and were read rather than quoted from the brief:
`Source/StratRules/StratRules.manifest.json` carries
`"rulesCommit": "96d93ea9e490c8cc33261f5d9671389a9c76ecfd"` and `Data/StratData.manifest.json`
carries `"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"` — the `96d93ea` / `c2f5860`
the standing checklist derives, unmoved.

**Checklist 2.**

    grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ |
      grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'

Zero hits, and zero legal candidates to judge either — stated so a reader can tell this from an
unrun check.

**Checklist 3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS`/`USTRUCT`/`UENUM`
was tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or
`Replay.h`. **Zero hits.**

**Checklist 4.** `grep -rn 'TUniquePtr<[[:space:]]*FStratBridge' Source/` returns exactly one
line, `Source/StratUI/StratViewModel.h:47`, which is prose about the C4150 measurement inside a
comment, not a member declaration. Judged, not forwarded.

**Checklist 5 — derived from the five Build.cs files, both call forms, comments stripped.**
**16 rows, all five modules present:**

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

`StratUI` takes no `StratPlay` and no `Stratocracy` edge; **no cycle.** The sixteenth row was read
in source rather than treated as unconditional — `Source/StratBridge/StratBridge.Build.cs`:

    133: bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    134: PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    135: if (!bCompileVendoredRulesHere)
    137:     PrivateDependencyModuleNames.Add("StratRules");

Guarded, monolithic-only, from the same one expression that drives the define. The unconditional
empty-array AddRange at line 75 is the empty row above it. **NOT drift.**

**Field census — ten names, no eleventh:** the known false positive from the string literal inside
PrivateDefinitions.Add (left unfiltered deliberately), PCHUsage, PrivateDefinitions.Add,
PrivateDependencyModuleNames.Add, PrivateDependencyModuleNames.AddRange,
PublicDependencyModuleNames.AddRange, PublicIncludePaths.AddRange, ShadowVariableWarningLevel,
bCompileVendoredRulesHere, bUseUnity.

**Checklist 6.** `Source/` holds five module directories — StratBridge, StratPlay, StratRules,
StratUI, Stratocracy — plus the two Target.cs files. IMPLEMENT_MODULE and
IMPLEMENT_PRIMARY_GAME_MODULE occur in exactly four files: StratBridge.cpp, StratPlay.cpp,
StratUI.cpp, Stratocracy.cpp. Those four are the four game modules named in Stratocracy.uproject.
**`StratRules` carries no module object and is correctly absent** — the standing false positive
that costs a startup abort to discover. No new module directory this pass.

**Checklist 8.** A recursive grep for the /Game/ literal across .cpp and .h under `Source/`, minus
`Tests/`, minus the import commandlet, and minus lines whose first non-space characters open a
comment, returns **zero lines**. Every occurrence outside the two standing exceptions is comment
prose. Judged, not forwarded.

**Suite figure, read by this reviewer rather than accepted.** `Saved/AutomationReport/index.json`
opened with utf-8-sig: reportCreatedOn 2026.09.06-18.17.58, succeeded 445, failed 0, notRun 0,
succeededWithWarnings 0, entry array **445** long. An independent macro census over `Source/`
returns 445 IMPLEMENT_SIMPLE_AUTOMATION_TEST and zero of any other macro form. Two instruments,
one figure, matching the live 445/445 in `global.md`.

**strat_banner_sweep.py, run rather than accepted:**

    Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
      automation report: 445 entries, all Success (2026.09.06-18.17.58)
      macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=445, COMPLEX/_CLASS=0
      report identity: mtime=1788718678, reportCreatedOn=2026.09.06-18.17.58
      suite claims found: 64 (1 live, 63 stamped)
      tracked items declared: 2

    SWEEP CLEAN -- no self-contradiction found

EXIT=0, measured without a pipe. **What it does NOT vouch for, restated because it matters to the
judgement below:** it collects claims BY SHAPE — an N/N, a reportCreatedOn, a tracked-item line —
so bare quantifiers and bare cardinals are invisible to it by construction. That is this file's
own carried debt and is why the fourth gate's finding had to be found by reading.

## Task 1 — is the fourth gate's finding closed, and closed cleanly?

**Yes, both halves, and the replacement introduces no claim of the same class.**

**Half one, the rotting count — CLOSED.** The blocked sentence read *"this pass was gated THREE
TIMES over a tree that kept moving"*. It now reads, in the live damage-alert banner:

    THIS ENTRY STATES NO GATE VERDICT, AND THE ABSENCE IS DELIBERATE. A verdict describes A TREE AND
    NOT A PHASE, and this pass was gated REPEATEDLY over a tree that kept moving -- each round's
    findings were worked, which moved the tree, which invalidated the verdict that had just been
    written here. NO ROUND COUNT IS STATED, AND THAT IS THE SAME RULE ONE LEVEL DOWN: a count of the
    gates on this pass is a count of a STILL-GROWING SET taken from inside the thing being counted,
    false at the next gate, which is the shape this file already rules out twice in bold. The rounds
    are enumerable by anyone who wants them, at the pattern named below.

The cardinality is gone and the reason is stated. **A targeted sweep for a replacement cardinality
found none.** Over the live entry, a grep for the pattern "once|twice|three|four|five|N times",
for "three/four/five gates|rounds|re-gates", for 72, for 73, and for VERDICT, returns four lines
and **not one of them counts the gate set**: the no-verdict declaration itself; the historical
VERDICT: PASS quotation; the pointer; and the owes-a-verdict-at-commit obligation. No cardinal of
the gate set survives.

**Half two, the pointer — CLOSED, and it resolves.** The entry now reads:

    THE VERDICTS LIVE WHERE THEY CANNOT ROT: `Tools/architect/gate_reports/2026-09-06-unit-damage-alert*.md`
    -- the PATTERN and not the bare directory, which holds this project's every gate -- one persisted
    file per round, none overwriting another, each carrying its own tree's evidence inline.

Counting that glob returns **4**, and the four are exactly this pass's reports: -gate.md,
-regate.md, -regate-2.md, -regate-3.md. Counting the whole directory returns **73** — and note
that the entry states no number for either set, which is what keeps the sentence true as this
report makes it 5 and 74. The bare-directory form no longer appears anywhere in the live entry.

**The fourth gate's Task-2 observation is now recorded in the entry itself:** *"AND THIS ENTRY
OWES A VERDICT AT COMMIT: stating none is honest only while the gate set is still growing; once it
stops, an entry with no verdict UNDER-claims, and the last round's is the one to state."* That is
the observation carried into the record rather than lost with the report.

### Read adversarially for a NEW claim of the same class

I looked specifically for anything in the replacement that counts, measures or dates a growing set
from inside itself, and for any pointer that does not resolve. Four candidates, each judged rather
than forwarded:

1. **"the shape this file already rules out twice in bold"** — the one imprecision in the
   paragraph, and it is NOT a finding. The substantive half is TRUE and re-executable: exactly two
   prior rulings exist, both locatable by grepping this file for the phrase "still growing while
   this was written" — one in the 2026-09-04 difficulty-ownership-pin banner, one in the
   shipped-difficulty-pin banner. The **typographic** half is true of one and not the other:
   counting asterisk pairs across the first ruling's six lines returns **0**, so that one is plain
   prose, while the second reads "**It NEITHER ENUMERATES NOR COUNTS the re-gates**" and is bold.
   So "twice" is right and "in bold" is one-for-two. It misdirects no reader — both cited passages
   exist and say what they are cited for — and it is an over-specified stylistic descriptor, not a
   false claim about the tree, a verdict, a count of a growing set, or a measurement.
   **Observation, not a finding.**
2. **Is "twice" itself a rotting count?** No, and the word doing the work is "already", which
   scopes the count to rulings PRIOR to this sentence and so excludes the sentence itself — which
   is, on its face, a third ruling-out of the same shape. Without "already" this would be the very
   defect it describes. With it, the counted set is closed historical banners rather than a set the
   sentence belongs to. Correct as written.
3. **"one persisted file per round, none overwriting another"** — a ratio and an invariant, not a
   cardinality. It stays true as the set grows, and this report keeps it true: fifth round, fifth
   file, four predecessors byte-identical.
4. **"The three hazards ... were measured clear on every round"** — a universal quantifier over a
   growing set, which is a weaker cousin of the blocked shape. It does not rot mechanically the way
   a cardinal does: it is falsified only by a round that finds one of the three, and such a round
   would be reporting that finding anyway. I re-measured all three clear this round (below), so it
   is true as of this report. Observation at most.

## Task 4 — the not-verified account, item by item, re-run

Each is accurate and **none is softened**. Each was re-measured against the tree this round.

1. **Nothing visual; no human has seen it.** The entry says it flat: *"`DamageFlashMaterial` ships
   UNSET and `BP_StratUnit` carries no tuned values, so THE FEATURE IS INVISIBLE IN PIE AND NO
   HUMAN HAS SEEN IT."* Instruments: `Source/StratPlay/StratUnitActor.h:1313` declares
   DamageFlashMaterial with no initializer, and counting Content/ paths in `git status --porcelain`
   returns **0**. The named discharge condition survives — *"DISCHARGED WHEN a human at the
   keyboard reports seeing a damaged unit flash and shake"* — with the overlay-compile caveat still
   attached. Unsoftened.
2. **DamageFlashSeconds ships 0.0f, so nothing arms at the shipped default.**
   `Source/StratPlay/StratUnitActor.h:1240` reads `float DamageFlashSeconds = 0.0f;`. The banner's
   load-bearing 435/435-at-default argument rests on this and it holds.
3. **The marker re-parent is UNMEASURED against BP_StratUnit.** The entry disclaims the green build
   as evidence in terms, and no Content/ path appears in `git status` (0, above). The editor was
   never opened in this pass. Accurate.
4. **The tour path is DEFENDER-ONLY.** Confirmed in code AND in the code's own comment at
   `Source/StratPlay/StratMatchSubsystem.cpp:2371-2380`, which states that no field of
   FStratAiPlaybackStep reports counter-damage and that the flash is therefore COMPLETE on the
   no-tour path and DEFENDER-ONLY on the tour path; the call at :2381-2387 reads Current.TargetId,
   not Current.UnitId, guarded on >= 0.
5. **The static_assert against a Count sentinel is NOT WRITTEN.** The entry says *"is NOT WRITTEN.
   It is production code, the test lane correctly refused to write it, and it is open."* Counting
   static_assert in `Source/StratUI/StratSoundCues.h` returns **0**. Open, and correctly routed to
   `strat-gameplay-engineer`.

## Task 3 — what was RE-MEASURED this round and what is CARRIED FORWARD

Stated explicitly so a reader can tell a re-measurement from an inheritance.

**RE-MEASURED against the current tree**, every one run in this session with output inlined above:
checklist 1 through 8 and 12; the module-arrow derivation and the field census in full; the guard
at StratBridge.Build.cs:133-137; the exported automation report; the macro census;
strat_banner_sweep.py; the mtime table; the two manifest commits; and the three pass-specific
hazards —

- **UpdateTickEnabled is the sole caller of SetActorTickEnabled(false).** SetActorTickEnabled
  outside `Tests/`, filtered to non-true arguments and non-comment lines, returns exactly one line:
  `Source/StratPlay/StratUnitActor.cpp:1167`,
  `SetActorTickEnabled(TweenOffsets.Num() > 0 || DamageFlashElapsed > 0.0f);`. Sole arbiter, as
  claimed.
- **ApplyUnitView is UNEDITED.** A -U0 diff over StratUnitActor.cpp and .h grepped for
  ApplyUnitView returns 11 lines and **every one is comment text** — ten added comment lines and
  one removed one. No executable line of that function is in the diff.
- **The tour flash sits outside FindSoundDirector()'s if.** Read in source at :2381-2387; the
  Attack-arm block is not nested inside any director guard.

**CARRIED FORWARD, not re-derived, because no source byte moved this round and the mtime table is
the warrant:** the comment-stripped-diff measurement on StratSoundDirector.cpp (0 changed
executable lines, with the 27c27 mutant control proving the stripper able to speak, from the third
gate); the prose-block audit of the two new clause files (gated clean at the first and second
rounds); the widget-arithmetic audit of StratOptionsWidget.cpp and .h (third gate); and the
ApplyView and FinishDamageFlash shape readings (fourth gate). Each of those files' mtimes is
unchanged from the gate that measured it.

**NOT RUN, and named so it is not read as clean:** this reviewer ran **no build and no suite**.
The Result: Succeeded line in the banner is read from the output quoted verbatim in `engine.md`,
not taken by me. The 445/445 figure I did read myself, from the exported report and from an
independent macro census.

## Findings

**None.**

The fourth gate's finding is closed on both halves. Nothing in the replacement paragraph counts,
measures or dates a growing set from inside itself; the one pointer resolves, to exactly four
files, all of this pass.

**On the loop risk, stated plainly because the seat asked for it.** I looked hard for a
fifth-round finding in that paragraph and I am not going to manufacture one. The single
imperfection I found — "twice in bold", where one of the two cited rulings is plain prose — is
**improvable, not defective**. It is a typographic descriptor attached to a substantive claim that
is TRUE and re-executable by anyone with a grep; no reader is misled about what the file says,
where it says it, or what follows from it. Holding a commit for it would trade a real, verified,
working feature against a two-word adjective, in a paragraph that has now been rewritten twice,
each rewrite carrying its own fresh risk of introducing something worse. The defect class the last
two gates blocked on — a closed statement about a still-growing set, in the only file CLAUDE.md
permits to carry phase status — is genuinely absent. **The prose is good enough to commit.** If
someone touches that paragraph for another reason, deleting two words is free; nobody should touch
it for this.

## Observations

Non-gating. None of these blocks a commit.

- **"twice in bold" is one-for-two on the adjective.** Both rulings exist; only the second is bold.
  The word "twice" is correct. Owner if ever taken: the coordinator. **Not worth a pass of its
  own.**
- **"were measured clear on every round"** is a universal quantifier over a set that is still
  growing, a weaker relative of the blocked shape. It is TRUE as of this report — I re-measured all
  three hazards clear — and is falsifiable only by a gate that would be reporting that finding
  anyway. Recorded so a future reader knows it was considered and passed, not missed.
- **The entry still states no phase verdict, which remains correct TODAY and becomes an under-claim
  at commit** — and the entry now says so itself, which is the fourth gate's observation properly
  carried into the record. Whoever commits should carry the final round's verdict into the entry at
  that moment, and not before.
- **The minus-census wording note is unchanged and still worth eight words.**
  StratSoundDirector.cpp and `engine.md` both say that over the comment-stripped file the minus
  character appears on exactly one line; the naive instrument returns 19, of which 18 are arrow
  operators. True about the OPERATOR, false about the CHARACTER, in the sentence offering itself as
  proof. Carried unchanged from the third and fourth gates; still not a finding. Owner if taken:
  `strat-gameplay-engineer`.
- **CancelDamageFlash remains public with no production caller** — carried unchanged from all four
  prior gates, correctly recorded as (c) in the banner, changed by nobody.
- **The static_assert against a Count sentinel is still not written and still open**, routed to
  `strat-gameplay-engineer`. Unchanged this round.
- **All four predecessor gate reports stand byte-identical and this one joins them.** The third and
  fourth reports quote sentences that have since been rewritten in `global.md`; those quotations
  are correct AS HISTORY and must not be updated to match. Each report records what a gate read in
  a tree that existed at the time.
- **`.agents/ue-project-context.md`** was checked against the derivation above; its StratBridge row
  already states the monolithic-only link edge and matches. No drift, no correction needed.

## Task 5 — is this pass ready for the user to commit?

**Yes.**

**What the user is committing.** Twelve modified files and two new clause files, plus five gate
reports and three record files. In substance: a Shake USceneComponent under Body on AStratUnitActor
with the three markers re-parented onto it, four EditDefaultsOnly properties and four public verbs;
the red implemented on Body's OVERLAY channel rather than slot 0, which is why ApplyUnitView needed
no edit; UpdateTickEnabled as the single arbiter of the tick flag and now the sole caller of
SetActorTickEnabled(false) in the class; StratDecideDamagedUnits in StratUI as a second reader of
the existing FStratSoundMark rather than a second mark; two flash trigger sites, both outside
FindSoundDirector()'s if; and ten new clauses taking the suite from 435 to 445, including
MatchEndedIsTheLastSoundCue, which pins a real latent out-of-bounds hazard in Emit's bEmitted bound
that nothing previously pinned. **No vendored byte, no Content/ asset, no Source/Stratocracy/ byte,
no .uproject change, no module-graph change.**

**What remains open, and none of it blocks a commit.** The feature is invisible in PIE:
DamageFlashMaterial ships unset and DamageFlashSeconds ships 0.0f, so at the shipped default
nothing arms — which is precisely why committing it is safe, and the 435/435-at-default
measurement is the evidence for that. No human has seen it. The marker re-parent is unmeasured
against BP_StratUnit. The tour path flashes the defender only. The static_assert form is not
written. Every one of those is stated in the record, unsoftened, with an owner, and the largest
carries a named discharge condition.

**Two things to do at the moment of commit, neither of them a gate finding.** (1) Carry this
round's verdict into the entry, which the entry itself now says it owes. (2) This report is
untracked until it is committed, so a record claim citing it resolves to zero in a fresh checkout
until the commit lands — commit the evidence and the record together.

VERDICT: PASS
