# Gate: the twelfth stale-claim site (`StratViewModel.cpp` presentation block) — strat-integration-reviewer

## Scope

The **uncommitted working tree** at `E:\MultiAgent\Stratocracy`, branch `master`, base
commit `1bd28c3`. Nothing staged (`git diff --cached --stat` printed nothing).
**Four modified files**, re-derived rather than accepted:

    M Source/StratPlay/StratUnitActor.h    | 12 ++++-
    M Source/StratUI/StratViewModel.cpp    | 39 +++++++++++++++-
    M Source/StratUI/StratViewModel.h      | 11 +++++
    M Tools/architect/state/engine.md      | 95 +++++++++++++++++++++++++++++++++++++++

`.agents/ue-project-context.md` was read first and is **not drifted** on any row this pass
touches: its `StratBridge` row already carries the conditional monolithic link edge, its
read-only table already defers to the two manifests rather than quoting a commit, and its
`StratUI` / `StratPlay` rows match the tree as derived below.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | **pass** — `git diff --stat -- Source/StratRules Data` printed nothing. **Control:** the same command shape over `Source/StratUI Data` printed two rows, so the instrument was shown able to speak before its silence was read. Manifests are the authority and were read: `rulesCommit 96d93ea…`, `dataCommit c2f5860…` |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | **pass** — the lowercase-initial call pattern across `Source/`, minus the two permitted directories and minus comment lines, returned nothing. **Control:** the same pattern inside `StratBridge.cpp` returned `strat::parseScenario(`, `strat::validateScenario(`, `strat::seedFromScenario(`. No `strat::` token appears anywhere in the diff |
| 3 | No vendored header in front of UHT | **pass** — every `.h` declaring `UCLASS`/`USTRUCT`/`UENUM` was tested for a `StratBridge.h` / `Ui.h` / `Data.h` / `Save.h` / `Scenario.h` / `Replay.h` include. The one hit is `StratBridge.h` itself, and it declares **no** reflected type: the anchored macro pattern returns nothing there (its six matches are prose). **Control:** the same anchored pattern over `StratViewModel.h` returns three `UENUM(BlueprintType)` |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | **pass** — `TUniquePtr<FStratBridge>` appears once, inside a comment at `StratViewModel.h:47` recording the `C4150` measurement. **Control:** `TPimplPtr<FStratBridge>` returns real declarations, incl. `StratMatchSubsystem.h:2614` |
| 5 | Module arrows unchanged and correct | **pass** — derived, not trusted. The extraction printed **16 rows across all five modules**, and the field census printed **10 names** (incl. the `BRIDGE=` string-literal false positive, not chased and not filtered). The 16th row, `StratBridge PrivateDependencyModuleNames StratRules`, was read at the source and **is** guarded: `StratBridge.Build.cs:135` is `if (!bCompileVendoredRulesHere)`, from `Target.LinkType != TargetLinkType.Monolithic` at `:133`. No `.Build.cs` and no `.uproject` is in the diff |
| 6 | New modules registered | **pass** — no new `Source/<Module>/` directory. `Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly absent from the `Modules` array; `Source/Stratocracy/Stratocracy.cpp:6` carries `IMPLEMENT_PRIMARY_GAME_MODULE` |
| 7 | `Source/Stratocracy/` untouched | **pass** — `git diff --stat -- Source/Stratocracy` printed nothing. **Control:** adding `Source/StratPlay` to the same invocation printed a row |
| 8 | No `/Game/` path literal in C++ | **pass** — no added line in the diff contains `/Game/`. The one new asset path is `Content/StratArt/Meshes/SM_GuidedMarker.uasset`, a comment and not a `/Game/` literal |
| 9 | No widget-side arithmetic | **pass** — every added line under `Source/StratUI` is a comment line; the filter that strips comment-prefixed added lines returned an empty set |
| 10 | Lane discipline | **pass** — `strat-gameplay-engineer` owns C++ outside `Tests/`; all three `Source/` files qualify, and `engine.md` is that agent's own record file. The entry attributes ACTING and WRITING, IN LANE, names base commit `1bd28c3`, and cites no exception clause — correctly, since none applies |
| 11 | Prose blocks present and house-style | **BLOCK** — the new prose is in house style and cites measurements as measurements, but **one of those measurements is false and one is an inference the record already refutes**. See F1–F4 |
| 12 | Nothing staged or committed | **pass** — `git status --porcelain` shows four modified entries and no staged content; HEAD is still `1bd28c3` |

## Independent re-derivation of the dispatch's own measurements

**Comment-only on all three `Source/` files — confirmed with my own instrument, not the agent's.**
A literal-aware stripper (line comments, block comments, string and char literals, escape
handling) that **refuses to emit zero lines** was run over the `HEAD` blob and the worktree copy
of each file and compared by sha256 of the stripped text:

    Source/StratPlay/StratUnitActor.h: HEAD 95 lines 3bea3e735b19 | WORK 95 lines 3bea3e735b19 | IDENTICAL
    Source/StratUI/StratViewModel.cpp: HEAD 380 lines ca46f3fb6c44 | WORK 380 lines ca46f3fb6c44 | IDENTICAL
    Source/StratUI/StratViewModel.h:   HEAD 399 lines 709971f4c8cf | WORK 399 lines 709971f4c8cf | IDENTICAL

95 / 380 / 399, matching the dispatch. **Three mutation controls on a disposable copy of
`StratViewModel.cpp`**, because a stripper that returns IDENTICAL is exactly what a broken one
returns too:

- **added statement** (a new `int` declaration after `Built.Units.Add(UnitView);`) — **DETECTED**
- **a `//` inside a string literal**, followed by real code on the same line — **DETECTED**, and
  the stripped output retains the whole line, so the literal did not swallow the code behind it
- **comment-only edit** — **correctly invisible**

**Banner sweep.** `python Tools/architect/strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0. It
is its own control: it emitted a substantive census — 435 automation entries all Success, 435
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, 63 suite claims, 2 tracked items — rather than a bare clean
line, so it read the files it swept.

**The no-re-run branch.** `strat_suite_report_gate.py`'s `SIMPLE_MACRO` and `COMPLEX_MACRO` were
extracted verbatim from the script and executed over `StratViewModel.cpp` at `HEAD` and in the
worktree: empty list and zero on **both sides, identical**. **Positive control** over
`Source/StratUI/Tests/StratViewModelParity.cpp` returned **7 clause names**, the first two being
`Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField` and
`Stratocracy.StratUI.T-INT-05.OrdersArePreserved`. The regexes were shown able to find macros
before their silence was believed. **The no-re-run branch applies.**

**Provenance arithmetic — re-derived, including the counts the dispatch did not check.**

| commit | `rev-list --count` | committer date | subject (truncated) |
|---|---|---|---|
| `f918e30` | **29** | 2026-08-12T14:23:07-04:00 | Phase 2: the view model knows no `strat::` type… |
| `ed27d5a` | **31** | 2026-08-12T18:00:28-04:00 | Phase 4: a clause cannot be its own witness… |
| `1d6f758` | **77** | 2026-08-21T09:35:39-04:00 | A save is a replay, and the guided opening… |
| `fe462e9` | 116 | 2026-08-24T12:26:20-04:00 | The turn-1a marker could never go out… |
| `38961f1` | 166 | 2026-08-29T14:24:58-04:00 | The flag and the unacted pip had been published… |

**29 / 31 / 77 confirmed exactly.** `git merge-base --is-ancestor` is strict in one direction on
all four pairs tested — `f918e30`→`ed27d5a`, `f918e30`→`1d6f758`, `ed27d5a`→`1d6f758`,
`fe462e9`→`38961f1` — and the reverse is false in every case, so the ordering is a real ancestry
and not a merge artefact. The same-day gap is 14:23:07 → 18:00:28 = **3 h 37 m**, so "about three
and a half hours later" is accurate, and a `--date=short` ordering genuinely could not have
settled that pair. `fe462e9` → `38961f1` is 2026-08-24 → 2026-08-29 = **five days**, as claimed.

Origins were re-derived with `git log --oneline --reverse -S` — `--reverse`, so the answer is the
ORIGIN and not the most recent touch — and by `--diff-filter=A`. The sentence
"neither exists in this engine yet" first appears at `f918e30`;
`Source/StratPlay/StratSelectionMachine.h` and the `U.bDone = DoneUnits.Contains` line both first
appear at `ed27d5a`; `Source/StratPlay/StratGuidedOpening.h` at `1d6f758`;
`Content/StratArt/Meshes/SM_GuidedMarker.uasset` at `fe462e9`; and "No such mesh exists yet" at
`38961f1`. **Control:** a `-S` search on a fabricated symbol returned blank.
**The verdict "true when written, aged into falsity" is confirmed.**

**The live route is a route.** `FStratSelectionMachine::DecorateViewModel` assigns both fields
unconditionally for every unit at `StratSelectionMachine.cpp:440-441`, and
`AStratPlayerController::DecorateForPresentation` calls it at `StratPlayerController.cpp:1457`,
on the seam every refresh runs through — `RefreshFromMachine` calls that function at `:1373` and
the subsystem binds it at `:162`.

## Judgement 1 — the builder was correctly left alone. NOT an under-fix.

Two independent reasons, both derived here rather than taken on report:

1. **The overlay is an unconditional overwrite, not a merge.** `StratSelectionMachine.cpp:440-441`
   reads `U.bDone = DoneUnits.Contains(U.UnitId);` and
   `U.bLockedThisTurn = LockedUnits.Contains(U.UnitId);`. Anything `StratBuildViewModel` wrote
   would be discarded on the same frame, so populating the builder would produce a **dead write**
   at best.
2. **The builder physically cannot populate them without inverting a module arrow.**
   `StratBuildViewModel` lives in `StratUI`; `DoneUnits` and `LockedUnits` live in
   `FStratSelectionMachine` in `StratPlay`; and the arrow derived under check 5 is
   `StratPlay → StratUI`, not the reverse. Making the builder authoritative would require
   `StratUI → StratPlay` and a cycle.

The design reading is **confirmed**: the claim about the world was wrong, the code is right, and
this pass did not under-fix on that axis.

## Judgement 2 — retract-and-quote is right here, not noise accumulating in a hot file.

The false clause is fifteen commits and twenty-five days old and sits in the file that
`StratViewModel.h` and the `engine.md` record both point at. It travelled: `engine.md`'s own
2026-09-05 sweep entry quotes it. A reader arriving holding "neither exists in this engine yet"
needs to see the words struck, not silently absent — the same warrant `StratShellSubsystem.h`
used when volume acquired a surface, and that file is in the same lane and is the declared
precedent. **Two `RETRACTED>` blocks inside a 39-line comment growth is proportionate**, and the
retracted words are short quotations rather than reproduced paragraphs. Not a finding.

## Judgement 3 — the header repair went the right direction.

`StratViewModel.cpp` pointed at "the debt and its discharge condition are in the header". The
header block states an OWNER per field — "OWNER: the selection machine", "OWNER: the guidance
layer" — and a reason the fields live on the model rather than in an actor, and states **no
discharge condition at all**. Since the built-false / overlaid-later arrangement is permanent
(Judgement 1), inventing a debt to make the pointer true would have made the tree agree with a
false sentence. Retracting the pointer and stamping the header as a **design statement** is the
correct direction, and the stamp is careful to say NOTHING ABOVE CHANGES AND NOTHING IS
DISCHARGED. Not a finding.

## Judgement 5 — count arithmetic confirmed.

See the provenance table above. 29 / 31 / 77 exact, ancestry strict in one direction.

## Judgement 6 — the sweep denominator and the four clearances.

**Denominator re-derived and exact.** Every `.cpp` and `.h` under `Source/StratPlay` and
`Source/StratUI`, excluding `/Tests/`: StratPlay **47**, StratUI **24**, **total 71**. Matches.

My own alternation is narrower than the agent's and returned **40** hits rather than 78; the
difference is the regex, not the corpus, and my set is a strict subset in the existence-asserting
direction. **Positive control:** the same regex over the `HEAD` blob of `StratViewModel.cpp`
returned line 414, the known instance.

**All four named clearances were opened and each is correctly cleared:**

- **`StratGuidedOpening.h:476` — correctly cleared.** "Neither exists today" refers to two
  **hypothetical changes** — a rules change letting a side hold an objective it did not take, and
  a Sec 2.11.6 rewrite making beat 2 about the arrival — not to engine objects. Its stated premise
  was re-verified in the vendored source: `Economy.good.cpp:151` reads
  `if (o.owner == side) { clearProgress(s, o.hex); continue; }   // already ours`. The
  short-circuit is real.
- **`StratHoverState.h:50` — correctly cleared.** "neither is here" is scoped to **fields on this
  struct**, and `FStratHoverView` still carries exactly two — `bHasHoveredHex` and `HoveredHex` —
  neither a forecast nor an info-panel field. Correct subject discipline: the forecast and the
  info panel have since shipped as their own waves, and the sentence stays true because its
  subject is the struct and not the engine.
- **`StratPlayerController.cpp:252` — correctly cleared.** "There is no such world in this
  milestone" is a debt naming its own discharge condition ("the day a second controller can
  exist"), and it is still true: Sec 2.11's hot seat is one controller holding two seats.
- **`StratShellSubsystem.h:106` — correctly cleared.** Already retracted and stamped when volume
  acquired a surface, and it is indeed the model shape this pass followed.

**A null that names its cleared candidates is checkable, and it checks out.**

## Handed-off scope: the negative-subject form on ground no sweep has covered

Method: a case-insensitive alternation over the negative-subject form —
"neither exists/exist/does/do/has/have/is/are", "none of them/these/those/which",
"nor does/is/are/has/have/do", "no such X exists", "neither X nor Y exists".
**Control on every corpus:** a plant file carrying the exact known sentence was appended to each
file list and was returned every time, so each null below is a measurement and not a dead
instrument.

| Corpus | Denominator | Raw hits | Existence-asserting | Verdict |
|---|---|---|---|---|
| `Source/StratBridge/` non-`Tests/` `.cpp` and `.h` | **15** | 5 | 1 | clean |
| `Source/Stratocracy/` non-`Tests/` `.cpp` and `.h` | **51** | **0** | 0 | clean, control spoke |
| `Source/**/Tests/` `.cpp` and `.h` | **106** | 68 | 7 | clean |
| record files: `Tools/architect/state/*.md`, `.agents/ue-project-context.md`, `CLAUDE.md` | **8** | 95 | 17 | one item — F3 below |

Every existence-asserting hit on the new ground was opened. The ones that could plausibly have
been stale are not:

- **`StratBridge.cpp:2156`** — "no such gate exists for `AiCommandKind`". **True, and measured
  against a control:** no `static_assert` and no parity test pins `AiCommandKind`, while the gate
  it contrasts itself with — the `EUnitType` one — is real and is named two lines above.
  `StratAiBridgeParity.cpp` tests the four-way **dispatch**, not the enumerator mapping, which is
  precisely the gap the comment claims.
- **`StratBridge/Tests/StratPathPreviewParity.cpp:418`** — "no such board exists". A mathematical
  claim (every prefix of a cheapest path is itself a cheapest path), permanently true, and the
  file records the surviving mutant that measured it.
- **`StratPlay/Tests/StratAiPlaybackClauses.cpp:2690`** — "and neither exists", referring to a
  fixture scenario authored for the case and a seam letting a clause place a step on the reel.
  Neither is in the tree.
- **`StratPlay/Tests/StratMatchReconcile.cpp:1589`** — already a `RETRACTED>` block.
- **`StratPlay/Tests/StratSelectionWaitClauses.cpp:311`** — self-correcting in place: "written as
  though none of them existed. They do."

**So the form does generalise past the lane, and the new ground is clean — with one exception,
and it is in the diff itself.**

## Findings

### F1 — BLOCK. A false claim about the world, planted by the pass that was removing false claims about the world.

`Source/StratPlay/StratUnitActor.h:862`, added this pass:

    * `FlagMarkerMesh` and `UnactedPipMesh` are the unset ones; measured 2026-09-06, the

**Both are set.** `Tools/architect/state/content.md` records them read out of a live editor **by
value**, twice. First, in the pass that set them:

    `FlagMarkerMesh = /Engine/BasicShapes/Cone`, `FlagMarkerMaterial = MI_Marker_Flag`,
    `FlagMarkerOffset = (0, -40, 300)`; `UnactedPipMesh = /Engine/BasicShapes/Sphere`,
    `UnactedPipMaterial = MI_Marker_Pip`, `UnactedPipOffset = (0, 40, 300)`.

and again later:

    - `FlagMarkerMesh` is `/Engine/BasicShapes/Cone` and `UnactedPipMesh` is
      `/Engine/BasicShapes/Sphere`. **Engine primitives are not a new precedent in this class:**

plus "`BP_StratUnit.uasset` carries all four of `FlagMarkerMesh`, `FlagMarkerMaterial`,
`UnactedPipMesh` and `UnactedPipMaterial`".

**Re-measured here from the raw bytes, with both controls this lane's own record specifies.**
`grep -aoF` over `Content/StratPlay/BP_StratUnit.uasset`:

    FlagMarkerMesh         1
    UnactedPipMesh         1
    FlagMarkerMaterial     1
    UnactedPipMaterial     1
    GuidedMarkerMesh       1     [known-SET positive control]
    FlagMarkerOffset       0     [known-UNSET control: the C++ default ships]
    ZzFabricatedName       0     [fabricated negative control]

The instrument discriminates in **both** directions on this very asset, and both properties read
PRESENT alongside a property this record already proves is set.

**The aggravating circumstance.** This lane's own record, `Tools/architect/state/engine.md`,
already ruled on this exact axis and **refused** it — naming "`FlagMarkerMesh`, which the file
says is unset" as a claim the byte instrument cannot settle, ruling the five "unset is the state
this ships in" sites "NEITHER false NOR correct", and naming `StratUnitActor.h`'s
`FlagMarkerMesh` block and its `UnactedPipMesh` block as open handoff sites. This pass converted
an explicitly-refused claim into a **flat assertion at a third site**, inside a clause that opens
"measured 2026-09-06". The head commit's own subject warns that "three were created by the
passes that were fixing the others"; this is the next one, created by the pass fixing the
twelfth.

**Owner: `strat-gameplay-engineer`.**

### F2 — BLOCK. Right referent, wrong verdict: the disambiguated sentence is itself stale.

`Source/StratPlay/StratUnitActor.h:856`:

    * No such mesh exists yet. ("SUCH MESH" IS THE FLAG MARKER'S AND THE PIP'S, NOT

The **disambiguation is correct** — see the Observations on Judgement 4 — and the pass was right
not to retract on the ground that the sentence denied `SM_GuidedMarker`. But having correctly
established that "such mesh" means the flag marker's and the pip's, the pass left the sentence
**standing**, and under exactly that reading **it is now false**: both slots are filled on
`BP_StratUnit` (F1), the markers draw, and `content.md` closes the very question the sentence
exists to hedge:

    `SM_GuidedMarker`'s `ExtendedBounds` measures `BoxExtent = (50, 50, 50)`, and both engine
    primitives are the same 100 uu. The assumption holds; it is now measured rather than hoped.

So the correct repair was neither a retraction on the `SM_GuidedMarker` reading nor a
disambiguation that leaves the verdict alone. It was to record that both slots ship 100 uu engine
primitives and that the arithmetic's assumption has been **measured to hold**. This pass
**under-fixed the site it correctly identified.**

Same site, `:865`:

    * reusing the one mesh under three materials -- which this file cannot see and does not

**The speculation is false.** The content lane is not reusing `SM_GuidedMarker` under three
materials; it used `/Engine/BasicShapes/Cone` and `/Engine/BasicShapes/Sphere`, and `content.md`
says so in terms — "Engine primitives are not a new precedent in this class".

**Owner: `strat-gameplay-engineer`.**

### F3 — BLOCK. The record entry carries F1 and F2 forward under a heading that calls them true.

`Tools/architect/state/engine.md`, added this pass, in the bullet headed
"ONE FURTHER TRUE FINDING, AND IT IS NOT THE STALE-CLAIM SHAPE":

    rather than rewritten. `MI_Marker_Flag` and `MI_Marker_Pip` do exist while no flag or pip
    **mesh** does, so the content lane may be reusing the one mesh under three materials -- which
    that file cannot see and now says it does not assume.

The narrow clause — that no flag or pip **mesh asset** exists under `Content/StratArt/Meshes/` —
is true as scoped. The inference drawn from it is the false one, and it sits under a heading that
labels the bullet a TRUE FINDING, so a reader arriving by citation lands on a false conclusion
presented as a confirmed one. This is the record restating the header's error rather than
catching it.

**Owner: `strat-gameplay-engineer`** — it is that lane's own record file.

### F4 — BLOCK. The content-lane handoff is an unmeasured excuse, not an honest limit.

The handoff — that whether `BP_StratUnit` reuses `SM_GuidedMarker` under those materials is
unreadable from this lane, with `FlagMarkerOffset`'s 100 uu arithmetic depending on the answer —
**is answered, in this tree, in a plain-text file the lane may read.**
`Tools/architect/state/content.md` gives the two mesh paths by value, gives
`FlagMarkerOffset = (0, -40, 300)` and `UnactedPipOffset = (0, 40, 300)` by value, and closes the
100 uu arithmetic against `SM_GuidedMarker`'s measured `BoxExtent = (50, 50, 50)`. No editor, no
`execute_script`, no `.uasset` parsing is required — only `Read`.

This lane's own standing instruction is to read the lane record before doing lane work, and the
`engine.md` bullet that raised these sites says in terms that they are named "so the content lane
settles them in the same reading that settles `AiSides`" — the record is where the answer was
expected to arrive, and it had already arrived. **Not out of reach.**

**Owner: `strat-gameplay-engineer`**, to withdraw the handoff. Nothing is owed to the content
lane on this item.

## Observations

Non-gating.

- **Judgement 4 answered in full: the disambiguation did not stamp a sentence that was never
  wrong, and it did not let a real one through by calling it ambiguous — but it stopped one step
  short.** The referent analysis is right and the ordering evidence is right: `fe462e9` is five
  days before `38961f1`, confirmed above, and `StratUnitActor.h:657` does say that
  `BP_StratUnit`'s default carries `SM_GuidedMarker` here, so the author demonstrably knew the
  asset existed. The reverse error was correctly avoided. What was missed is that the sentence is
  stale on its **own** reading — that is F2, and it gates rather than sitting here.
- **The `-S` control is weaker than the entry implies, and the pass got the right answer anyway.**
  A `-S` search returning blank is also what a mistyped search string returns: my own `-S` probe
  on the phrase `class FStratSelectionMachine` returned blank, because the declaration is
  `struct STRATPLAY_API FStratSelectionMachine`. A blank-returning fabricated-name control cannot
  distinguish those two cases. The origins were therefore re-derived here by `--diff-filter=A` on
  the files instead, and they agree — but the control shape is worth sharpening before the next
  provenance claim rests on it.
- **`StratHoverState.h`'s NOT IN THIS ROUND heading now sits above a correct sentence in a stale
  frame.** The line about no forecast and no info panel, ending "neither is here", is true of the
  struct, and both surfaces have since shipped as their own waves. The words are correct and the
  clearance is correct; the heading invites a cold reader to take them as an engine-wide absence.
  Not a finding, since the sentence's subject is explicit.
- **Coverage arithmetic for the sweeps in this pass.** 71 (prior ground, re-derived) + 15 + 51 +
  106 + 8 = **251 files** carrying this form's sweep. Three of the four new corpora returned no
  existence-asserting defect; the record corpus returned one, and it is in the diff rather than in
  the standing tree.
- **No suite was re-run and none was needed**, on the branch measured at the gate script rather
  than accepted from the rule. That measurement and its positive control are recorded above.

VERDICT: BLOCK
