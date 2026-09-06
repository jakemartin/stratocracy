# Gate: the twelfth stale-claim site — RE-GATE after the four-finding round — strat-integration-reviewer

## Scope

The **uncommitted working tree** at `E:/MultiAgent/Stratocracy`, branch `master`, base commit
`1bd28c3`. Nothing staged. `git status --porcelain` was re-derived rather than accepted and it
agrees with the dispatch — **four modified files**, plus one untracked file the dispatch did not
list (the previous round's own gate report,
`Tools/architect/gate_reports/2026-09-06-twelfth-stale-claim-viewmodel-presentation.md`, which is
the reviewer's artifact and not lane content):

    M  Source/StratPlay/StratUnitActor.h        52 +  /  5 -
    M  Source/StratUI/StratViewModel.cpp        37 +  /  2 -
    M  Source/StratUI/StratViewModel.h          11 +  /  0 -
    M  Tools/architect/state/engine.md         147 +  /  0 -
    ?? Tools/architect/gate_reports/2026-09-06-twelfth-stale-claim-viewmodel-presentation.md

`.agents/ue-project-context.md` was read first. It is **not drifted** on any row this pass
touches: its `StratBridge` row already carries the conditional monolithic link edge AS a
conditional, its read-only table already defers to the two manifests rather than quoting a commit,
and its `StratUI` / `StratPlay` rows match the arrows derived below.

**This is a PASS-DESCRIBES-A-TREE gate, not a phase gate.** The four prior findings are judged
answered; a fifth site, in the same file and falsified by the same evidence this pass added, is
not.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | **pass** — `git diff --stat -- Source/StratRules Data` printed nothing. **Control:** the same command shape over `Source/StratUI Data` printed rows, and `git ls-files Source/StratRules Data` returns **32** tracked paths, so the silence is over a populated corpus and not an empty one. The manifests are the authority and neither was modified |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | **pass** — the lowercase-initial call pattern over `Source/`, minus the two permitted directories, returned **0 raw hits**, so no judgement of legal-versus-illegal was even required this round. **Control:** the same pattern inside `Source/StratBridge/StratBridge.cpp` returned `strat::parseScenario(`, `strat::validateScenario(`, `strat::seedFromScenario(`. No `strat::` token appears anywhere in the diff |
| 3 | No vendored header in front of UHT | **pass** — eight headers include one of `StratBridge.h` / `Ui.h` / `Data.h` / `Save.h` / `Scenario.h` / `Replay.h`; every one of them (`StratBridge.h` plus seven vendored `Source/StratRules/*.h`) declares **zero** anchored `UCLASS`/`USTRUCT`/`UENUM` lines. **Control:** the same anchored pattern over `Source/StratUI/StratViewModel.h` returns **18**, so the counter was shown able to speak before its zeros were read |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | **pass** — the `TUniquePtr` form returns exactly one line, `Source/StratUI/StratViewModel.h:47`, and it is prose recording the `C4150` measurement. **Control:** the `TPimplPtr` form returns real declarations, `StratMatchSubsystem.h:2614` and `StratScoreboardHUD.h:1253` |
| 5 | Module arrows unchanged and correct | **pass** — derived, not trusted. The extraction printed **16 rows, all five modules present**; the field census printed **10 names**, including the `BRIDGE=` string-literal false positive, which was not chased and not filtered away. The 16th row was read at source and **is** guarded: `StratBridge.Build.cs:135` is the negated `bCompileVendoredRulesHere` test, from `:133` `Target.LinkType != TargetLinkType.Monolithic`. No `.Build.cs` and no `.uproject` is in the diff |
| 6 | New modules registered | **pass** — no new module directory in the diff. `Source/StratRules/` remains correctly absent from the `Modules` array |
| 7 | `Source/Stratocracy/` untouched | **pass** — `git diff --stat -- Source/Stratocracy` printed nothing, over **53** tracked paths. **Control:** the same invocation with `Source/StratPlay` added printed a row |
| 8 | No `/Game/` path literal in C++ | **pass** — no added line under `Source/` contains `/Game/`. **Control:** the identical filter for `/Engine/` over the same added-line set returns **5**, so the filter reaches the added prose it is scanning |
| 9 | No widget-side arithmetic | **pass** — all **48** added lines under `Source/StratUI` are comment lines; the non-comment filter returns the empty set. Independently, the code-line census below shows both `StratUI` files code-equal to `HEAD` after comment stripping |
| 10 | Lane discipline | **pass** — three `Source/` files outside `Tests/`, which is `strat-gameplay-engineer`'s lane, plus that agent's own record file. The entry reads ACTING and WRITING, IN LANE, on `master`, base commit `1bd28c3`, UNCOMMITTED; it cites no exception clause and correctly cites none, since no lane topology stands and in-lane work needs no clause. No test code outside `Tests/`, no production code inside it, no asset changed |
| 11 | Prose blocks present and house-style | **BLOCK** — the new prose is house-style, cites measurements as measurements, and names its instruments inline. But the pass repaired three of the four stale marker sites in `StratUnitActor.h` and left the fourth — the one its own record enumerated as open — standing, and now directly contradicted by prose it added to the same file. See F1 |
| 12 | Nothing staged or committed | **pass** — `git status --porcelain` shows four modified entries, one untracked, and **no staged content**; `git diff --cached --stat` printed nothing and `HEAD` is still `1bd28c3` |

## The four prior findings — each re-measured, all four answered

**F1 (the planted "measured ... the unset ones" claim) — ANSWERED, and the replacement is
independently correct.** The withdrawn sentence is quoted in place inside a RETRACTED 2026-09-06
block, and the replacement names the instrument inline instead of summarising it as "measured". I
re-ran the instrument the block cites, on the asset the block cites:

    grep -aoF over Content/StratPlay/BP_StratUnit.uasset
      FlagMarkerMesh         1
      UnactedPipMesh         1
      FlagMarkerMaterial     1
      UnactedPipMaterial     1
      GuidedMarkerMesh       1     [known-SET positive control]
      FlagMarkerOffset       0     [known-UNSET control]
      ZzFabricatedName       0     [fabricated negative control]

The asset is a real package and not an LFS pointer — its first bytes are `c1 83 2a 9e ff ff ff ff`,
the Unreal package magic, not a git-lfs version line. **The instrument discriminates in both
directions on this one file**, which is what makes the 1s evidence rather than noise. The by-value
half is confirmed at `Tools/architect/state/content.md:571-572` and `:1185-1187`, which give
`FlagMarkerMesh = /Engine/BasicShapes/Cone` and `UnactedPipMesh = /Engine/BasicShapes/Sphere`.

**F2 ("No such mesh exists yet") — ANSWERED.** Retracted in the same block, with the withdrawn
parenthesis quoted, and the mesh-reuse speculation withdrawn with it.

**F3 (the "ONE FURTHER TRUE FINDING" heading) — ANSWERED.** `engine.md` now carries a bracketed
retraction of the heading AND the half-bullet, quoting both, with the surviving referent call
restated rather than merely asserted. A heading that certifies its own truth is gone, and nothing
replaced it with a differently-worded version of the same shape.

**F4 (the content-lane handoff) — ANSWERED, and withdrawn rather than narrowed.** The record now
reads "WITHDRAWN IN FULL, AND NOTHING REPLACES IT", and gives the standing instruction that caused
the miss. The 100 uu arithmetic is closed against `content.md:1191-1192` — `SM_GuidedMarker`'s
`ExtendedBounds` at `BoxExtent = (50, 50, 50)`, both engine primitives the same 100 uu, "The
assumption holds." Verified at that line.

## Independent re-derivation of the dispatch's own measurements

**Comment-only on all three `Source/` files, with my own instrument.** A literal-aware stripper
(line comments, block comments, string and char literals with escape handling) that **refuses to
emit zero lines** was run over the `HEAD` blob and the worktree copy of each file:

    Source/StratPlay/StratUnitActor.h: HEAD 95 lines 69d81f4368bd | WORK 95 lines 69d81f4368bd | IDENTICAL
    Source/StratUI/StratViewModel.cpp: HEAD 380 lines ce3a7de62593 | WORK 380 lines ce3a7de62593 | IDENTICAL
    Source/StratUI/StratViewModel.h:   HEAD 399 lines ece4a7d84b91 | WORK 399 lines ece4a7d84b91 | IDENTICAL

**95 / 380 / 399, matching the dispatch exactly.** The hash prefixes differ from the previous
round's report because this is a fresh stripper with its own normalisation — the LINE COUNTS are
the comparable figure, and they agree. **Three mutation controls on a disposable copy of
`StratViewModel.cpp`**, because IDENTICAL is also what a broken stripper returns:

- **added statement** (a new int declaration after `Built.Units.Add(UnitView);`) — **DETECTED**
- **a line-comment token inside a string literal**, followed by real code on the same line —
  **DETECTED**, and the code behind the literal survived into the stripped output rather than
  being swallowed by it
- **comment-only edit** (a trailing comment appended) — **correctly invisible**

This is the stronger form of the "no executable byte moved" claim the record makes: it does not
merely check that added and removed diff lines look like comments, it shows the stripped code is
bit-equal on both sides.

**Banner sweep.** `python Tools/architect/strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0. It
is its own control: it emitted a substantive census — 435 automation entries all Success at
`2026.09.06-04.29.34`, 435 `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, 63 suite claims, 2 tracked items —
rather than a bare clean line, so it read the files it swept.

## Judgement 1 — the extension past the dispatch. JUSTIFIED THIS TIME, and the difference is not luck

The dispatch asked this squarely and it deserves a square answer rather than a rule of thumb.
**"Stay in the dispatch" is the wrong lesson from the last round, and the right one is narrower.**

Last round's extension and this one are different acts:

- **Last round** the pass reached a site the dispatch did not name and wrote a NEW CLAIM about the
  world there — "FlagMarkerMesh and UnactedPipMesh are the unset ones" — from a measurement that
  covered a DIFFERENT SUBJECT: a directory listing under `Content/StratArt/Meshes/`, which licenses
  nothing whatever about a Blueprint default. The extension manufactured its own evidence gap. All
  four findings followed from it.
- **This round** the pass reached two sites and **applied evidence that already existed and that it
  had just used elsewhere in the same file** — `content.md`'s by-value editor read, plus the byte
  instrument with both of its controls. It introduced no new subject and asserted nothing the
  retraction block three property-blocks below does not assert with the same citations.

And the two sites are not arbitrary. **This lane's own record named them as open, in these exact
words.** `Tools/architect/state/engine.md:1753-1756` — the 2026-09-03 refusal — lists the open
sites as "StratUnitActor.h's FlagMarkerMesh block, its UnactedPipMesh block and the Sec 2.11.2
markers'-art bullet in its own header block", and `:1770-1771` concludes "the handoff is unchanged
at six sites and every one of them still needs an editor." **That editor reading arrived** — it is
`content.md`'s. Discharging a site your own record filed as blocked, in the moment the block lifts,
is the opposite of scope creep; it is the record working.

**So the correct rule is not "stay in the dispatch". It is: an extension may APPLY evidence and may
not MANUFACTURE it.** Last round's failed that test at its first clause. This round's passes it.

**And the pass flagged the extension in both artifacts rather than smuggling it**, which is the
other half of what makes it reviewable at all: `engine.md` carries "THE THREE SITES ARE REPAIRED IN
PLACE AND THE TWO BEYOND THE DISPATCH ARE NAMED AS SUCH." A reader finds the extension without
diffing for it.

**Where it stopped short is a finding, and it is F1 below** — the same record sentence that licensed
the extension names a THIRD site in the same file, and that one was left standing.

## Judgement 2 — the line-ending trap. Confirmed, and its recording obligation is already discharged

**Confirmed, by a byte census rather than by grep:**

    Source/StratPlay/StratUnitActor.h : total LF 1303  CRLF 1303  bare LF 0  lone CR 0
    Source/StratUI/StratViewModel.cpp : total LF  984  CRLF  984  bare LF 0  lone CR 0
    Source/StratUI/StratViewModel.h   : total LF 2343  CRLF 2343  bare LF 0  lone CR 0
    Tools/architect/state/engine.md   : total LF 8295  CRLF 8295  bare LF 0  lone CR 0

**Control:** the same counter over a synthetic three-line buffer built with two CRLF endings and one
bare LF reports **bare LF 1**, so it can see the defect it reports absent. **The four figures match
the dispatch's re-measurement exactly** (1303 / 8295 / 984 / 2343), uniform CRLF, zero bare LF, zero
lone CR.

**The trap itself reproduces on this box**, which is worth stating because it is why the disclosure
matters: a `grep -c` for a dollar-quoted carriage return over `Source/StratPlay/StratUnitActor.h`
returns **0**, on a file that `file(1)` describes as "UTF-8 text, with CRLF line terminators" and
that carries **1303** of them. An agent trusting that zero would have shipped mixed endings
believing it had checked.

**Does a corruption introduced and repaired inside one pass need recording beyond its report?**
**No — and it is already recorded in the right place.** The test is whether the artifact can mislead
a later reader. It cannot: the bytes are uniform, no phantom CR drift exists to diagnose, and
nothing downstream reads a line-ending property of these files. What DOES have future value is the
instrument lesson — that a grep for a carriage return and a piped `cat -A` both lie here while a
Python endswith census does not — and that is a TOOLING fact, not a TREE fact. It belongs exactly
where it is: in the pass report, and in the standing note this box already carries about grep on
this machine. Putting it in `engine.md` would be that record narrating its own drafting, which this
project has blocked on seven times. **Not a finding, and no further recording is owed.**

## Judgement 3 — the two `StratViewModel` files. Unchanged, with one honest limit

**They are unchanged, and I can prove three things short of byte-identity, and not byte-identity
itself. Stating which is which is the point.**

What is measured:

1. **Code-line identity to `HEAD`** — 380 and 399 lines, stripped hashes equal on both sides, with
   the three mutation controls above. Nothing executable moved.
2. **The diff histogram reproduces the cleared report's own numbers.** `git diff --numstat` gives
   `37 / 2` and `11 / 0`; the prior report's scope block recorded a 39-wide and an 11-wide
   histogram, and 37 + 2 = 39, 11 + 0 = 11. The added-line counts are unchanged from the tree that
   was cleared.
3. **No write occurred after the clearing gate.** `StratViewModel.h` mtime 08:35:31,
   `StratViewModel.cpp` mtime 08:40:25; the report that cleared them has mtime 08:54:30; the two
   files repaired this round have mtimes 08:59:03 and 09:00:05. The two viewmodel files sit strictly
   before the gate, the two repaired files strictly after it, and nothing sits between.

**What is NOT measured, said plainly: strict byte-identity is not falsifiable from this checkout.**
The previous report recorded stripped-CODE hashes only. Since every change in both files is a
COMMENT, a code-identity hash is precisely the instrument that cannot see a comment edit, and an
mtime is a timestamp rather than a content hash. So the correct verdict is **"unchanged on every
instrument available, and byte-identity is inferred rather than measured."** Under this project's
own standard — an unfalsifiable claim gets labelled, not asserted — I will not write
"byte-identical" as a measurement. The fix is cheap and is in Observations.

## Judgement 4 — the suite branch. Verified, with one correction to the dispatch's framing

**census() verified at source.** `Tools/architect/strat_suite_report_gate.py:110` iterates
`sorted(source_root.rglob("*.cpp"))` — `.cpp` only, `.h` invisible to it. Confirmed by reading the
function, not inferred from the rule.

**The gate's own regexes, extracted verbatim from `:85-90` and run on both sides:**

    Source/StratUI/StratViewModel.cpp   HEAD []  0  | WORK []  0  | IDENTICAL
    Source/StratPlay/StratUnitActor.h   HEAD []  0  | WORK []  0  | IDENTICAL

**Positive control** over `Source/StratUI/Tests/StratViewModelParity.cpp` returned **7** clause
names, the first two `Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField` and
`Stratocracy.StratUI.T-INT-05.OrdersArePreserved`. The regexes were shown able to find macros before
their silence was believed. **The no-re-run branch applies, and the existing report at
`reportCreatedOn 2026.09.06-04.29.34` stays valid for this tree.**

**The correction, and it does not change the verdict.** The dispatch relays the claim as "this pass
modified **no `.cpp` at all**". That is false of the working tree: `Source/StratUI/StratViewModel.cpp`
IS a modified `.cpp`, and filtering `git status --porcelain` for that extension returns it. **The
record itself is right and the dispatch's paraphrase is not** — `engine.md` says "Of the three files
touched, exactly one is a `.cpp` and it is not a test file", which is exactly correct. The branch
holds anyway, because the identical-on-both-sides measurement above is over that very file. Recorded
because a re-gate that accepted "no `.cpp` at all" would have skipped the one measurement that
actually carries the branch.

## The thirteenth-instance sweep: a provenance label with no instrument named inline

**The form, as the dispatch specifies it:** a claim carrying `measured` / `verified` / `confirmed` /
`checked` — a statement about the AUTHOR'S PROCESS, which a reader cannot check — where no
instrument is named inline. This is the shape F1 arrived in last round.

**Method.** A label-word scan, then a deliberately LOOSE mechanical filter and a STRICT manual
judgement, in that order. For each hit, a window of the line plus and minus two lines was tested for
any instrument token: a backtick, a `.cpp` / `.h` / `.md` / `.py` / `.ini` / `.uasset` / `.json`
extension, `grep`, `git`, `python`, `Build.bat`, or any digit. A window with none of those is a
candidate, because a claim with nothing checkable within five lines of it is the defect's signature.
The filter is dumb on purpose; narrowing it to shrink the pile is how it would stop seeing the next
one.

**Plant control on every corpus.** A file containing the bare comment line "This was measured and it
holds." — the form with nothing checkable near it — was appended to each of the three file lists and
was **returned every time**, so each figure below is a measurement and not a dead instrument.

| Corpus | Denominator | Raw label hits | No instrument in window | Judged instances |
|---|---|---|---|---|
| `Source` `.cpp`/`.h` outside `Tests/` | **161** | 335 | 24 | **0** |
| `Source` `.cpp`/`.h` inside `Tests/` | **106** | 482 | 65 | **0** |
| `Tools/architect/state/*.md`, `.agents/*.md`, `CLAUDE.md` | **8** | 1179 | 39 | **0** |
| the diff's own added lines | 4 files | 16 | — | **0** |

**275 files, 2012 raw label hits, 128 candidates.** Every candidate whose text asserts a fact about
the tree or the world was opened and read; the remainder are the label word appearing as a C++
identifier (a local named `Measured` at `StratMatchSubsystem.cpp:1067` and again at
`StratAiTurnRunnerClauses.cpp:678`), as a description of what CODE does ("the destination is checked
before the slot"), or inside a test's `AddInfo` / `TestTrue` string — none of which is a provenance
claim. **That subset judgement is the limit of this sweep and is stated rather than hidden: 128
candidates were surfaced mechanically, and the world-fact subset among them was read individually.**

The four strongest candidates, opened in full, and why none is an instance:

- **`Source/StratUI/StratViewModel.cpp:36`** — "Measured on this file, not predicted:" is followed
  immediately by the instrument AND both results: an unanchored search returns EIGHT, the anchored
  form returns THREE. Instrument inline. **Not an instance** — it is the correct shape.
- **`Source/StratPlay/StratOptionsPresenter.h:100`** — reads "it was **not** measured, and no clause
  in this tree can observe an input mode", and says in terms that the discharge "rests on ONE human
  report and nothing else". **An anti-instance:** it declares the weakness of its own evidence
  rather than dressing it in a label.
- **`Tools/architect/state/decisions.md:684`** — "Every claim under each question was independently
  re-verified against the tree before being written down" is a bare process claim, but each of the
  three questions beneath it carries its own citation, the first being a direct GDD line range. The
  instrument is distributed into the items rather than absent. **Not an instance**, though it is the
  closest thing in the record corpus.
- **`Source/StratPlay/StratMatchSubsystem.cpp:1884`** — "Measured: three of the seven deaths in a
  real hand-over were units built during that same hand-over." It names WHERE but not WITH WHAT, and
  a reader cannot re-run a past play session. It is corroborated with the same figures at
  `engine.md:1910`. **The only marginal one of the four**; it is an empirical frequency from a
  session, which is inherently a session observation, and it is not a claim about a file or a
  default. Recorded under Observations rather than gated.

**So the form does not generalise into a thirteenth instance on this ground.** The null is measured,
on a stated denominator, with a plant control on each corpus.

**The thirteenth instance is a different form, and it is F1 below** — not a label without an
instrument, but a stale existence claim in an edited file that the SAME PASS'S OWN new evidence
falsifies three property-blocks further down.

## Findings

### F1 — BLOCK. The pass repaired three of the four stale marker sites in `StratUnitActor.h` and left the fourth, which its own record had enumerated, now contradicted by prose it added to the same file.

`Source/StratPlay/StratUnitActor.h:151-157`, in that file's own header block, **unchanged by this
diff and standing**:

    // - The Sec 2.11.2 MARKERS' ART. `FlagMarker` and `UnactedPip` ship with no mesh and no
    //   material, exactly as `GuidedMarkerMesh` shipped before 2026-08-24: `EditDefaultsOnly`,
    //   no initializer, and no `/Game/` path in this file. Until the content lane assigns them
    //   the components exist, are positioned, and are toggled correctly on every refresh, and
    //   they draw nothing. That is a content gap and not a match failure, and `BeginPlay` says
    //   so once per actor for each

**Every clause of that is now false except the EditDefaultsOnly / no-initializer half**, and it is
false by the evidence this very pass added to this very file:

- "**ship with no mesh and no material**" — all four properties are set. `content.md:571-572` by
  value; the byte instrument returns 1 / 1 / 1 / 1 on `BP_StratUnit.uasset` against 0 for the
  known-unset control and 0 for a fabricated name, reproduced above.
- "**Until the content lane assigns them ... they draw nothing**" — the content lane assigned them.
- "**That is a content gap**" — there is no gap.
- "**exactly as `GuidedMarkerMesh` shipped before 2026-08-24**" — the analogy is now inverted, and
  **this same file says so at `:656`**: "The marker's mesh. ASSIGNED AS OF 2026-08-24 AND THAT IS
  THE STATE THIS SHIPS IN".

**This is not a pre-existing defect the pass merely failed to notice. It is the site its own licence
names.** `Tools/architect/state/engine.md:1753-1756` lists the open sites as "StratUnitActor.h's
FlagMarkerMesh block, its UnactedPipMesh block and the Sec 2.11.2 markers'-art bullet in its own
header block". The pass repaired the first two, **cited that exact refusal as its warrant for doing
so**, and did not touch the third. Grepping the new entry (`engine.md` lines 18-165) for `markers`,
`header block`, `three sites` or `beyond the dispatch` returns the "THE THREE SITES ARE REPAIRED"
line and nothing about the markers'-art bullet; **control**, the same grep over the 2026-09-03
region returns the enumeration three times. The site is neither repaired nor named as deliberately
left.

**The cost is worse than an ordinary stale sentence, because the file is now self-contradictory and
the false half comes first.** A reader opening `StratUnitActor.h` meets the header block at line 151
and is told the markers draw nothing; the correction lives at line 752, six hundred lines below,
behind a heading. The previous round's F2 was ruled a BLOCK for a strictly weaker version of this —
"under-fixed the site it correctly identified" — and this is the same class with the site pre-named
in the lane's own record.

**Owner: `strat-gameplay-engineer`.** The repair is the shape already used twice, at `:752` and
`:822`: retract the false clauses quoting them, keep the half that is about this file, and cite the
same by-value reading and the same two controls. The `engine.md` entry's "THE THREE SITES ARE
REPAIRED IN PLACE" line then needs the fourth added, or an explicit statement of why it was left.

## Observations

Non-gating.

- **The dispatch's "no `.cpp` at all" is a paraphrase error, not a record error.** Detailed under
  Judgement 4. The record is right; the relay dropped the qualifier "and it is not a test file".
  Worth noting only because the suite branch turns on that file.
- **The positive-control figure differs three ways across three artifacts and none is wrong.** The
  dispatch says 6 clause names, `engine.md` says "two clause names", and the previous gate report
  and this one both measure 7 on `StratViewModelParity.cpp`. The record almost certainly reported
  the first two it printed. Not a finding, but a count restated loosely is how a count goes stale.
- **Record a per-file sha256 in every gate report, so the next re-gate can answer "unchanged" by
  measurement.** Judgement 3 could only be answered by inference because the cleared tree was never
  hashed. Three lines of output would have made it a measurement. For this tree:
  `Source/StratUI/StratViewModel.cpp` is
  `9833f6c2c01b215e724d43ce60df1d2e2969cf5caabc1582530ab477880463c2`, and
  `Source/StratUI/StratViewModel.h` is
  `be8923fe2d8b7dc05f5a46ea8d919a9292ba6facfcce25c58c1601264ef81b3c`.
- **A second site of F1's exact class is standing in an UNTOUCHED file, and it is the largest item
  the next dispatch could take.** `Source/StratPlay/StratBoardActor.h:559-562` reads "**UNSET IS THE
  STATE THIS SHIPS IN AS OF 2026-09-01** ... No material instance for the pulse exists in `Content/`
  yet; `BP_StratBoard` carries no default here". `content.md:739-745` records
  `/Game/StratArt/Materials/MI_Overlay_BuildPulse` authored and "Assigned to `BuildPulseMaterial` on
  `BP_StratBoard`'s class default, **which was `None` before**", verified there by read-back through
  a different route than the write, with a control. **Both halves of that block are now false.** It
  is the third of the six sites the 2026-09-03 refusal enumerated, and the same editor reading that
  unblocked the two repaired this round unblocks it. **Not gated here**, since the file is outside
  this diff and outside the dispatch — reported so it is not rediscovered from zero a fourth time.
- **`engine.md`'s "no executable byte moved" is true and was verified more strongly than it claims.**
  It argues from added and removed diff lines being comment lines; the stripper above proves the
  stripped code is bit-equal on both sides, which is the claim that actually matters and which does
  not depend on a line-classification heuristic.
- **The build entry's "The editor was confirmed closed first" is a provenance label with no
  instrument named inline** — the marginal member of the thirteenth-form sweep inside the diff. It
  is rescued in practice by the two sentences above it, which record the link of
  `UnrealEditor-StratPlay.dll` succeeding: on this tree that link CANNOT succeed with the editor
  open, which is the instrument. Worth naming inline next time; not a finding.
- **The build and the suite were not run by me and are not vouched for here.** `engine.md` reports
  96 actions, `Result: Succeeded`, and a byte-identical re-run printing `Target is up to date` and
  `run 0 action(s)` — a good control shape, and `strat-test-author`'s to confirm, not mine.
- **`Tools/architect/gate_reports/` is untracked and its reports are LF-only** while the source tree
  is uniformly CRLF. Deliberate or not, it means a gate report is invisible to the tracked view of
  `git status --porcelain` and to any check reading committed history — the standing note that a
  gate you cannot produce is unfalsifiable applies to the directory itself.

VERDICT: BLOCK
