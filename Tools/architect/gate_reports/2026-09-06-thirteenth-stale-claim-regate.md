# Gate: the thirteenth stale-claim site — RE-GATE after the one-finding round — strat-integration-reviewer

## Scope

The **uncommitted working tree** at `E:/MultiAgent/Stratocracy`, branch `master`, base commit
`1bd28c3`. Nothing staged. `git status --porcelain` was re-derived rather than accepted and it
agrees with the dispatch on all six modified paths, plus two untracked files the dispatch did not
list — the previous two rounds' own gate reports, which are the reviewer's artifacts and not lane
content:

    M  Source/StratPlay/StratBoardActor.cpp     22 changed
    M  Source/StratPlay/StratBoardActor.h       63 changed
    M  Source/StratPlay/StratUnitActor.h        89 changed
    M  Source/StratUI/StratViewModel.cpp        39 changed
    M  Source/StratUI/StratViewModel.h          11 changed
    M  Tools/architect/state/engine.md         306 +  /  1 -
    ?? Tools/architect/gate_reports/2026-09-06-twelfth-stale-claim-viewmodel-presentation.md
    ?? Tools/architect/gate_reports/2026-09-06-twelfth-stale-claim-regate.md

`.agents/ue-project-context.md` was read first and is **not drifted** on any row this pass touches.
Its `StratBridge` row already carries the monolithic link edge AS a conditional and names the
expression it comes from; its `StratPlay` row already carries the public/private split; its
`StratBridge -> Stratocracy` note is present and correct. The two manifests derive to
`rulesCommit 96d93ea...` and `dataCommit c2f5860...`, and neither manifest is in the diff.

**This is a PASS-DESCRIBES-A-TREE gate.** The single prior finding is judged answered, and the four
extension calls the pass made on its own are each judged below against last round's stated test --
an extension may APPLY evidence and may not MANUFACTURE it.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | **pass** — `git diff --stat -- Source/StratRules Data` printed nothing. **Control:** `git diff --stat -- Source/StratUI Data` printed two rows, and `git ls-files Source/StratRules Data` returns **32** tracked paths, so the silence is over a populated corpus. The manifests are the authority and neither was modified |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | **pass** — the lowercase-initial call pattern over `Source/`, minus the two permitted directories, returned **0** hits. **Control:** the same pattern inside `Source/StratBridge/StratBridge.cpp` returns `strat::applyCommand(`, `strat::boardOf(`, `strat::buildUiSnapshot(`, `strat::canonicalStateHash(`, `strat::findGameUnit(`. Three added lines DO contain the token — `StratViewModel.cpp` prose naming `strat::buildUiSnapshot`, and two `engine.md` lines — and all three are comment or prose, judged and dismissed rather than forwarded |
| 3 | No vendored header in front of UHT | **pass** — eight headers include one of `StratBridge.h`/`Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h`; every one (`StratBridge.h` plus seven vendored `Source/StratRules/*.h`) declares **zero** anchored `UCLASS`/`USTRUCT`/`UENUM` lines. **Control:** the same anchored counter over `Source/StratUI/StratViewModel.h` returns **18** |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | **pass** — every `TUniquePtr` hit in `Source/` is prose recording the `C4150` measurement, 12 lines across 4 files; **zero declarations**. **Control:** `TPimplPtr` returns real declarations, `StratMatchSubsystem.h:2614` |
| 5 | Module arrows unchanged and correct | **pass** — derived, not trusted. The extraction printed **16 rows, all five modules present**; the field census printed **10 names**, including the `BRIDGE=` string-literal false positive, which was neither chased nor filtered away. The 16th row was read at source and **is** guarded: `StratBridge.Build.cs:135` wraps `:137` in a negated `bCompileVendoredRulesHere`, defined at `:133` from `Target.LinkType != TargetLinkType.Monolithic`. No `.Build.cs` and no `.uproject` is in the diff |
| 6 | New modules registered | **pass** — no new module directory in the diff; the changed top-level dirs are `Source/StratPlay`, `Source/StratUI`, `Tools/architect`. Four files carry a module macro, and the `.uproject` Modules array is exactly Stratocracy, StratBridge, StratUI, StratPlay. `Source/StratRules/` carries no module object and remains correctly absent |
| 7 | `Source/Stratocracy/` untouched | **pass** — `git diff --stat -- Source/Stratocracy` printed nothing, over **53** tracked paths. **Control:** the same invocation with `Source/StratPlay` appended printed three rows |
| 8 | No `/Game/` path literal in C++ | **pass** — the two added lines under `Source/` containing the token are both prose stating the prohibition itself, not literals. **Control:** the identical filter for `/Engine/` over the same added-line set returns **7**. Net per-file counts HEAD to WORK: `StratBoardActor.h` 3 to 3, `StratUnitActor.h` 6 to 6, the other three 0 to 0 — the dispatch's claim 4 reproduces exactly |
| 9 | No widget-side arithmetic | **pass** — the non-comment filter over added lines returns the **empty set** for `Source/StratUI` AND for `Source/StratPlay`. Independently, the code-line census below shows all five files code-equal to HEAD after comment stripping, so no arithmetic could have been added |
| 10 | Lane discipline | **pass** — five `Source/` files outside `Tests/`, which is `strat-gameplay-engineer`'s lane, plus that agent's own record file. The entry names ACTING and WRITING, IN LANE, on `master`, base commit `1bd28c3`, UNCOMMITTED, and states that no exception clause applies and none is cited — correct, since no lane topology stands. No test code outside `Tests/`, no production code inside it, no asset changed |
| 11 | Prose blocks present | **pass** — every new block is why-first, retracts by quoting the withdrawn text in place rather than deleting it, names its instrument inline, and states its own limits. One directional pointer inside a new annotation is wrong; it misleads about layout only, not about any fact, and is under Observations |
| 12 | Nothing staged or committed | **pass** — `git status --porcelain` shows six modified and two untracked, **no staged content**; `git diff --cached --stat` printed nothing and HEAD is still `1bd28c3fcae0f29909270313583f12df4febaa16` |

## The prior finding — re-measured, answered

**F1, the Sec 2.11.2 markers'-art bullet left standing — ANSWERED, and answered in the shape the
file already uses.** `Source/StratPlay/StratUnitActor.h:151` now opens the bullet with a bracketed
2026-09-06 correction that quotes the withdrawn text, states all four properties are assigned, calls
the `GuidedMarkerMesh` analogy "inverted rather than apt", and then keeps the half that is about
this file — `EditDefaultsOnly`, no initializer, no path literal — with the `BeginPlay` log restated
as a guard for ANOTHER Blueprint rather than a description of the shipped one. That last move is the
part worth naming: it is the difference between deleting a rule and re-siting it.

**Re-derived from the bytes, not read off the diff.** A census over
`Content/StratPlay/BP_StratUnit.uasset`, magic `c1832a9e`, 26925 bytes, a real package and not an
LFS pointer:

    FlagMarkerMesh        1     UnactedPipMesh        1
    FlagMarkerMaterial    1     UnactedPipMaterial    1
    MI_Marker_Flag        2     MI_Marker_Pip         2
    GuidedMarkerMesh      1     [positive control, assigned since 2026-08-24]
    FlagMarkerOffset      0     [known-unset control]
    ZzFabricatedNameXyz   0     [fabricated negative control]

The 1s and 2s are the same fingerprint the known-assigned sibling prints, and the instrument returns
0 in both directions on the same file. Every figure the header block asserts reproduces.

## The four extension calls, each judged

### 1. Closing the SECOND copy of the checklist at `engine.md:1920` — JUSTIFIED

The bullet restated the same list as "unchanged at six sites and every one of them still needs an
editor". The pass stamped it STALE in place, kept the sentence, and settled five of six. **It
applies exactly the evidence already used at the repair sites and introduces no new subject**, which
is the test. Leaving one of two copies of a closed list reading open is the rotted-amendment shape
this record has already paid for, and the bracket says so in terms.

**And the one it leaves open is honestly open. Confirmed independently.** A census over
`Content/StratPlay/BP_StratGameMode.uasset` returns `AiSides` **1** against `ZzFabricatedNameXyz`
**0**. That is a name-table PRESENCE — it proves the property serialises and says nothing whatever
about its value, on this bullet's own unchanged reasoning. The discriminator that settled the other
five does not exist here: `BuildPulseMaterial` was settled by the ASSET NAME appearing alongside it,
`MI_Overlay_BuildPulse` twice, and an integer or array default names no asset, so there is nothing
for the byte instrument to count. **`AiSides` on `BP_StratGameMode` is discharged only by a by-value
editor read, and the record says so.** Correctly filed as the one remaining item.

### 2. Replacing the ARGUMENT under NO ANIMATION IS DRIVEN FROM C++ — JUSTIFIED, and it is the best call of the pass

The rule's 2026-09-01 premise was an ABSENCE: `MI_Overlay_Objective` was "the only member of this
material family today", with zero scalar parameters, "so there is no parameter named anywhere in the
tree for a pulse to drive." The family grew. The pass did not leave a true conclusion standing on a
dead reason, and did not quietly delete the reason either — it retracted the premise by quoting it
and put a PRESENCE in its place: the pulse already pulses, in the material, off `Time`, so C++ would
be a second animator, and retuning would need a rebuild instead of a material edit.

**This is the right call and the reason it gives is the right reason.** A prohibition resting on a
premise a reader can falsify is a prohibition that will be broken by the first reader who checks —
and checking is the behaviour this project asks for. A rule satisfied by construction survives that
reader; a rule satisfied by absence does not.

**Every empirical claim in the replacement reproduces here, with the control discriminating:**

    M_OverlayPulse.uasset    PulseSpeed 1  PulseMin 1  PulseMax 1
                             MaterialExpressionTime 1  MaterialExpressionSine 1   Color 7
    M_Translucent.uasset     PulseSpeed 0  PulseMin 0  PulseMax 0
    [CONTROL]                MaterialExpressionTime 0  MaterialExpressionSine 0   Color 7

Both packages carry the Unreal magic `c1832a9e`. `Color` returning 7 on BOTH is what makes the five
zeros a measurement rather than a dead read.

**And the parent claim — which the block asserts and the dispatch did not ask about — holds with a
mirror-image control I ran myself:**

    MI_Overlay_BuildPulse.uasset   M_OverlayPulse 4   M_Translucent 0
    MI_Overlay_Objective.uasset    M_OverlayPulse 0   M_Translucent 4     [sibling control]

The reparent is real and the untouched sibling still points at the old master. That is as clean a
discriminator as this instrument produces.

**The block also states what it CANNOT settle** — whether the graph compiles and what it looks like
on screen — and cites `content.md`'s measurement of `get_statistics` returning zeros for a
known-good control. Naming the limit inline is the difference between this and a claim that would
have needed gating.

### 3. A stronger positive control than the dispatch suggested — JUSTIFIED

Rather than "a nonzero hit", each subject is paired with a property **already recorded as assigned
in its own block**, so the subject prints the same fingerprint as a known-assigned sibling in the
same file. Verified: `ObjectiveMaterial` 1 and `MI_Overlay_Objective` 2 against `BuildPulseMaterial`
1 and `MI_Overlay_BuildPulse` 2 on `BP_StratBoard.uasset`; `GuidedMarkerMesh` 1 on
`BP_StratUnit.uasset`. Known-unset `FlagMarkerOffset` 0, fabricated name 0. **This is the correct
upgrade**: a nonzero hit only shows the file is non-empty, while a matched sibling shows the
instrument produces THIS shape for a property that is known to be set.

### 4. Stripping two path prefixes from its own prose — JUSTIFIED and verified

Net counts of the `/Game/` token are unchanged HEAD to WORK on every touched file — 3 to 3, 6 to 6,
and 0 to 0 on the other three — and the two added lines carrying it both state the prohibition
rather than a path. The pass removed prefixes it had itself introduced, matching the sibling block's
convention. Self-correcting inside the same pass, at no cost to any claim.

### The DECLINED site, `StratBoardActor.h:446` — the restraint is correct

The line reads: *"A reader who meets a dark board first will reach for `BuildPulseMaterial` being
unset, and that is the wrong place to look on every other turn."* Read in full at source. It is a
DIAGNOSTIC POINTER — it tells a reader where NOT to look — and it makes no claim about whether the
property is set. **Now that the material is assigned, the sentence is strictly MORE true**, because
the wrong place to look has become wrong on every turn rather than merely on most. Editing it would
have been a correction with nothing to correct, and this pass has already been blocked once for
reaching further than its evidence. **Not a finding; the restraint is the right call and is worth
recording as such.**

## Independent re-derivation of the dispatch's own measurements

**Comment-only on all five `Source/` files, with my own instrument.** A literal-aware stripper —
line comments, block comments, string and char literals with escape handling — that **refuses to
emit zero lines**, run over the HEAD blob and the worktree copy of each file:

    Source/StratPlay/StratBoardActor.cpp   HEAD 311 0fa467088f1c | WORK 311 0fa467088f1c | IDENTICAL
    Source/StratPlay/StratBoardActor.h     HEAD 102 6e530f95a2a3 | WORK 102 6e530f95a2a3 | IDENTICAL
    Source/StratPlay/StratUnitActor.h      HEAD  95 3bea3e735b19 | WORK  95 3bea3e735b19 | IDENTICAL
    Source/StratUI/StratViewModel.cpp      HEAD 380 ca46f3fb6c44 | WORK 380 ca46f3fb6c44 | IDENTICAL
    Source/StratUI/StratViewModel.h        HEAD 399 709971f4c8cf | WORK 399 709971f4c8cf | IDENTICAL

**311 / 102 / 95 / 380 / 399 — matching the dispatch on every file.** Three mutation controls on a
disposable copy of `StratBoardActor.cpp`, because IDENTICAL is also what a broken stripper returns:

- **added statement**, a new int declaration appended — 311 becomes **312 and the hash changes.
  DETECTED**
- **a line-comment token inside a string literal** followed by real code on the same line — 311
  becomes **312, DETECTED**, and the stripped output was inspected to confirm the whole line
  survived rather than being swallowed by the literal
- **comment-only edit**, a trailing comment appended — 311, hash unchanged: **correctly invisible**

**Line-ending census, by byte count rather than by grep** — a grep for a carriage return returns
zero on these files on this box, which is why this is counted and not grepped:

    StratBoardActor.cpp   LF  594  CRLF  594  bare LF 0  lone CR 0
    StratBoardActor.h     LF  754  CRLF  754  bare LF 0  lone CR 0
    StratUnitActor.h      LF 1321  CRLF 1321  bare LF 0  lone CR 0
    StratViewModel.cpp    LF  984  CRLF  984  bare LF 0  lone CR 0
    StratViewModel.h      LF 2343  CRLF 2343  bare LF 0  lone CR 0
    engine.md             LF 8453  CRLF 8453  bare LF 0  lone CR 0

**Control:** the same counter over a synthetic buffer built with two CRLF endings and one bare LF
reports total 3, CRLF 2, bare LF 1 — so it can see the defect it reports absent. The five source
figures match the dispatch exactly.

**BYTE-IDENTITY OF THE TWO `StratViewModel` FILES IS NOW MEASURED, AND IT WAS THE ONE THING THE LAST
REPORT COULD NOT ASSERT.** Full-file sha256 this round:

    Source/StratUI/StratViewModel.cpp  9833f6c2c01b215e724d43ce60df1d2e2969cf5caabc1582530ab477880463c2
    Source/StratUI/StratViewModel.h    be8923fe2d8b7dc05f5a46ea8d919a9292ba6facfcce25c58c1601264ef81b3c

**Both are character-for-character the hashes the previous report recorded**, which that report
recorded precisely so this question could be answered by measurement rather than by inference. It
worked. The two files that passed twice are byte-identical to the tree that passed them, and
"unchanged" is no longer a label resting on a chain of weaker instruments. That practice should stay.

**Banner sweep.** `python Tools/architect/strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0. It
is its own control: it emitted a substantive census — 435 automation entries all Success at
`2026.09.06-04.29.34`, 435 simple-automation macros, 63 suite claims of which 1 is live and 62
stamped, 2 tracked items — rather than a bare clean line, so it read the files it swept.

## The suite branch — VERIFIED, and this round the easy argument really is unavailable

`Source/StratPlay/StratBoardActor.cpp` IS a modified `.cpp`, so the branch turns on content and not
on file extension. The gate's `census()` was read at source: `strat_suite_report_gate.py:110`
iterates the sorted `rglob` of `*.cpp`, so headers are invisible to it and only the one `.cpp`
matters.

**The gate's own two regexes were EXTRACTED FROM THE GATE FILE BY PATTERN MATCH AND COMPILED, not
retyped**, then run over the HEAD blob and worktree copy of all five files:

    StratBoardActor.cpp   HEAD simple=0 complex=0 | WORK simple=0 complex=0 | IDENTICAL
    StratBoardActor.h     HEAD simple=0 complex=0 | WORK simple=0 complex=0 | IDENTICAL
    StratUnitActor.h      HEAD simple=0 complex=0 | WORK simple=0 complex=0 | IDENTICAL
    StratViewModel.cpp    HEAD simple=0 complex=0 | WORK simple=0 complex=0 | IDENTICAL
    StratViewModel.h      HEAD simple=0 complex=0 | WORK simple=0 complex=0 | IDENTICAL

**Positive control** over `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` returned **10** clause
names, the first two `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState` and
`Stratocracy.StratPlay.GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder` — matching the dispatch's
figure exactly. The regexes were shown able to find macros before their silence was believed. **The
no-re-run branch applies and the existing report at `reportCreatedOn 2026.09.06-04.29.34` stays
valid for this tree.** The build and the suite were not run by me and are not vouched for here.

## The content-lane handoff filed as a NON-DEFECT — I agree, and here is the discriminator

The question: `content.md:740` says `MI_Overlay_BuildPulse` is *"a `MaterialInstanceConstant`
parented to `M_Translucent`"*, while the bytes say `M_OverlayPulse`.

**Correctly filed as a non-defect, and the reason is a distinction worth stating.** `content.md` is
newest-first — the reparent entry sits at `:419-425` and the authoring entry at `:739-744`, so the
newer reading is above the older one. **A record entry is a dated statement about an ACT; a code
comment is a standing statement about the CURRENT file.** "`MI_Overlay_BuildPulse` was parented to
`M_Translucent`" was true on the day it was written and remains a true account of what was done.
That is why the twelve stale claims this milestone has chased were all in comments and in
prose-as-present-tense, and why this one is not.

**And the record does not rely on chronology alone.** The bullet at `:773` that carried the live
consequence — *"AND IT DOES NOT PULSE, WHICH IS THE HALF THAT IS STILL OWED"* — is stamped IN PLACE
at `:783`: *"THIS BULLET IS DISCHARGED AND IS STAMPED IN PLACE RATHER THAN DELETED, so a reader
arriving by a citation lands on the correction"*, naming the reparent as the route taken. The
sentence a reader could act on wrongly is stamped; the sentence that is merely historical is not.
**Nothing is owed.**

## The fourteenth-instance sweep: a true conclusion resting on a stale premise

**The form:** a rule or prohibition that is still CORRECT, whose stated reason has expired. Invisible
to every sweep run in this milestone, because the sentence a reader checks — the rule — is true.
This is the form call 2 above repaired.

**Method.** Three regexes over a five-to-nine-line window, ANDed. A NORMATIVE marker on the line
itself — must not, must never, may not, none may, no <word> may, is forbidden, shall not, never be
added, case-insensitive. A REASON connective anywhere in the window — because, since, the reason, on
the ground, which is why, that is why, so, thus, hence. And, the part that makes the form tractable,
an EXPIRABLE PREMISE token in the window: an existence, absence, uniqueness or count claim about the
tree — no such, does not exist, there is no, the only member/one/mesh/material/asset/file/caller/
place, exists yet, zero, has no, none exist, not yet, unset is the state, ships with no. **A rule
justified by a DESIGN reason cannot expire; a rule justified by a COUNT can, and only the second
kind is collectable.** The filter is deliberately loose and was not narrowed to shrink the pile.

**THE PLANT IS THE REAL PRE-REPAIR BLOCK, TAKEN FROM THE HEAD BLOB**, not a synthetic sentence:
`git show HEAD:Source/StratPlay/StratBoardActor.h` contains the NO ANIMATION IS DRIVEN FROM C++ AND
NONE MAY BE ADDED HERE ON AN ASSUMPTION block with its expired "the only member of this material
family today ... ZERO scalar parameters ... So there is no parameter named anywhere in the tree"
premise. That file was appended to each corpus.

**THE FIRST DRAFT OF THIS SWEEP RETURNED NOTHING FOR THE PLANT ON ALL THREE CORPORA AND ITS 41
CANDIDATES WERE DISCARDED UNREAD.** The normative regex was case-sensitive on some alternatives and
did not carry "none may be added", so it could not match the very block the sweep exists to find — a
dead instrument printing a plausible pile. It was fixed and re-run; the figures below are from the
run where the plant came back at line 575 of the plant file on every corpus. **Recorded because a
sweep whose plant fails silently is exactly the failure this project has measured before, and the
41-candidate first pile would have read as a real result.**

| Corpus | Denominator | Plant returned | Candidates | Judged instances |
|---|---|---|---|---|
| `Source` `.cpp`/`.h` outside `Tests/` | **161** | YES | 22 | **1**, in an untouched file outside the diff |
| `Source` `.cpp`/`.h` inside `Tests/` | **106** | YES | 17 | **0** |
| `Tools/architect/state/*.md`, `CLAUDE.md`, `.agents/*.md` | **8** | YES | 25 | **0** |

**275 files, 64 candidates, every one read.** The two repaired sites in this diff were themselves
returned by the sweep — `StratBoardActor.h:577` and `:589` — which is a second confirmation that the
instrument reaches the form. The rest fall into three groups, none of which is the shape: rules
whose reason is a DESIGN fact that cannot expire, such as `StratSaveGame.h:42`, "there is no
rules-module field for either, and there must not be: guidance is a teaching surface and the rules
are deterministic and headless"; rules whose premise is about the FILE ITSELF and still true, such
as `StratGuidanceWidget.cpp:10`, "There is no `if` on `Guidance` here and there must not be one",
where a branch count over that file returns **0** against **20** over `StratViewModel.cpp` as the
control; and rules already carrying their own correction, such as `StratUnitActor.h:677`, which
opens "ASSIGNED AS OF 2026-08-24".

**The one instance is real, and it is in a file no one has opened this milestone.**
`Source/StratPlay/StratCameraPawn.h:142-144`:

    /** Downward pitch, in degrees. Negative looks down; -60 is a readable three-quarter
     *  view of a hex board and -90 is straight down. A property rather than a constant
     *  because which one reads better depends on tile meshes that do not exist yet. */

**The conclusion is true — `ArmPitch` should be an `EditDefaultsOnly` property — and the premise is
false.** Eight tile meshes exist under `Content/StratArt/Meshes/`: `SM_HexTile_Plains`, `_Woods`,
`_Mountains`, `_Water`, `_Town`, `_Factory`, `_Bridge`, `_Fallback`, plus `SM_HexOverlay`.
**Control:** a glob for `SM_ZzFabricated*` under `Content/` returns **0**, so the lister
discriminates. They are not merely on disk but ON THE SHIPPED BOARD: a byte census of
`Content/StratPlay/BP_StratBoard.uasset` returns `SM_Hex` **18** and `TerrainMesh` **2** against
`ZzFabricatedNameXyz` **0**, and `content.md:2984` records the design reason there are "seven
near-identical `SM_HexTile_*` assets instead of one mesh and seven materials". **The thing the pitch
was said to depend on arrived, and the sentence saying it had not was never revisited.**

**NOT GATED**, on the same ground the previous report used for its `StratBoardActor.h` observation:
the file is outside this diff and outside the dispatch, and a reviewer that widens a gate to a file
the pass never touched is doing the thing it blocked the pass for last round. It is recorded with
its measurement and its control so the next dispatch can take it without rediscovering it.
**Owner: `strat-gameplay-engineer`.**

**What this sweep did NOT cover, stated rather than left to be discovered.** It collects rules whose
premise is an existence or count claim. A rule resting on a stale BEHAVIOURAL premise — because `X`
returns false on this path — carries no token in the premise list and would not be returned. The
form is real and this instrument does not reach it.

## Findings

**None.** The prior finding is answered, all four extension calls pass the APPLY-not-MANUFACTURE
test, the declined site was correctly declined, and every empirical claim in the diff reproduced
independently with a discriminating control.

## Observations

Non-gating.

- **A directional pointer in a new annotation points the wrong way.**
  `Tools/architect/state/engine.md:1921` reads *"THIS BULLET RESTATES THE LIST THE BRACKET BELOW
  CLOSES"*. The bracket it means is at `:1898` — twenty-three lines ABOVE, not below; a grep for the
  closing text finds it at that line and nowhere else in the file. **This costs a reader nothing they
  need**, because the annotation at `:1920` states the whole disposition inline — "FIVE OF THE SIX
  ARE SETTLED ... `AiSides` ... IS THE ONE THAT REMAINS" — which is precisely the property that makes
  it not a finding: no fact is misstated, only a location. Worth fixing on the next touch of that
  file, by the lane that owns it.
- **The fourteenth-instance sweep first draft was a dead instrument and its 41 candidates were
  discarded unread.** Detailed above. The generalisable point is narrower than "use a plant": **use a
  REAL known positive as the plant.** A synthetic sentence written by the same author as the regex
  will tend to match the regex — and the synthetic plant here failed too, but for a DIFFERENT reason
  than the real one would have, and only the real one exercised the alternative wording the actual
  corpus uses.
- **Recording per-file sha256 in the gate report paid off on its first use.** The previous report
  Judgement 3 could only answer "unchanged" by inference and said so; it then recorded the two hashes
  as the remedy. This round both matched exactly, and the weakest claim in the last report became the
  strongest in this one. Keep doing it; the cost is three lines.
- **The `content.md:740` phrase "parented to `M_Translucent`" would read better with an in-place
  cross-reference**, in the shape `:783` already uses on its sibling bullet. Not owed — the
  chronological argument holds and the actionable bullet IS stamped — but the tightest form of that
  record would let a reader landing on `:740` by citation see the reparent without scrolling up 320
  lines. The content lane call, not a defect.
- **`Tools/architect/gate_reports/` is untracked and LF-only** while the source tree is uniformly
  CRLF. Carried forward unchanged from the previous report: a gate report invisible to the tracked
  view of `git status --porcelain` is a gate you cannot produce from a checkout.
- **The build and the suite were not run by me and are not vouched for here.** That is the
  `strat-test-author` report to make. What I verified is only that the suite-report gate own census
  is unmoved by this diff, which is the branch that licenses NOT re-running.

VERDICT: PASS
