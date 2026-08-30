# Gate: `LayerFor` late-mesh fix + `GATE-BOARDCHURN` clause 8 — strat-integration-reviewer

**FIVE ROUNDS: BLOCK, BLOCK, PASS, BLOCK, BLOCK.** Each round gates the remediation of the one
before it, so the file reads as a sequence and not as five opinions of one tree. Round 1: one
finding. Round 2: two, one of them the SAME MECHANISM a second time. Round 3: PASS, zero findings,
plus two observations. Round 4: one, the same claim narrowed at one site and left standing at its
sibling. Round 5: one, two more copies of that claim surviving in different words. **ALL FIVE ARE
TRANSCRIBED HERE** -- rounds 3-5 were added after round 5 observed that the preamble cited rounds
it did not contain, which is in miniature the problem this file exists to solve. Each round begins
at a horizontal rule. Dispatched by the `coordinator` on 2026-08-29 against the UNCOMMITTED working tree at
`E:\MultiAgent\Stratocracy`, branch `master`, HEAD `d3efe78`.

**Both reports are the reviewer's as returned, transcribed by the `coordinator` and not edited.**
The only alteration is that a few `&gt;` sequences in round 2 are restored to `>`, an artifact of
how the report reached this session. They are persisted because a quoted gate that cannot be
produced from a checkout is unfalsifiable, and because each remediation rewrites the very prose
its finding cites -- without this file the findings would name text that no longer exists anywhere.

**THE LINE NUMBERS IN BOTH REPORTS ARE STALE AND THE QUOTED TEXT IS NOT.** Each remediation moved
the lines the previous round cited, so a `file:NNN` in round 1 or round 2 now resolves to
DIFFERENT content, and in one case to UNRELATED content. Checked in round 3 and corrected here:
`global.md:139-141` is the genuinely unrelated one -- it now lands in the flicker pass's
commit-message paragraph. `StratBoardActor.cpp:287-289`, `StratBoardActor.h:197-199` and
`tests.md:212-214` all still resolve to the SAME paragraph, corrected, which is the friendliest
outcome a stale citation has. An earlier draft of this note called all of them unrelated. Every finding also QUOTES the text it is about, which is what makes it
recoverable: grep the quotation, not the line number. Raised by the reviewer in round 3.

**What happened between and after the rounds is NOT part of either report and is stated here by
the `coordinator`.**

*After round 1:* the finding was verified independently before being accepted -- the call ordering
by reading `ApplyHexes`, and the HISM premise by scanning UE 5.8's ISM/HISM sources for a
mesh-change override. Both halves held. It was remediated by REMOVING the narrowing guard rather
than rewording its justification, which also closed half of round 1's Observation 1; the widening
is pinned by a second half added to clause 8 and proved red against a mutant that restores the
narrowed guard.

*After round 2:* both findings verified and accepted. Finding 1 -- four live sites still described
the removed guard -- is real, and the remedy was not proofreading: every site now states the
OUTCOME and points at `LayerFor` for the condition. **AN EARLIER DRAFT OF THIS SENTENCE
CONTINUED "so the shape is described in exactly one place", WHICH IS RETRACTED**: the property
holds inside the class -- `LayerFor`'s declaration and definition, one function -- and NOT
tree-wide, because `Tools/architect/state/global.md`'s banner states the condition too, correctly
and in present tense. `StratBoardActor.cpp` carries the same narrowing and the same count. Raised
by the reviewer in round 3 and again in round 4, where the unnarrowed sibling in the header was a
BLOCK on its own. Finding 2 is worse than the reviewer could see: the "30 other overrides" control figure was
a `head -30` on the coordinator's own pipeline, quoted as though it were a count. The uncapped
scan returns 75. `engine.md` now states the commands and both figures, and retracts the fabricated
one in place. The account of both rounds is `Tools/architect/state/engine.md`'s topmost entry, and
the two-round lesson is in `global.md`'s banner.

---

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD `d3efe78`. Six modified files, zero staged, zero untracked — matching the dispatch list. Audited the working-tree diff (`417 +/50 -`), the derived module graph, the full `GATE-BOARDCHURN` roster, and the four record edits. I did **not** build, did **not** run the suite, and did **not** reproduce the mutant (it is not reproducible from this checkout by the coordinator's own statement).

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — `strat::[a-z]\w*\s*\(` over `Source/` returned zero hits outside the two permitted directories; no legal-hit judgement was even needed this pass |
| 3 | No vendored header in front of UHT | pass — `StratBoardActor.h` includes only `CoreMinimal.h`, `GameFramework/Actor.h`, `StratViewModel.h`, `StratBoardActor.generated.h`; `UHierarchicalInstancedStaticMeshComponent` is forward-declared at `StratBoardActor.h:95` and the new `AssignTerrainMesh` signature at `:552` uses only that forward declaration. The tree-wide scan's only hit is `Source/StratBridge/StratBridge.h` itself, unchanged by this diff |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — no `FStratBridge` member added or changed; the test's `FStratBridge Bridge;` is a stack local in a `.cpp` |
| 5 | Module arrows unchanged and correct | pass — derived, 15 rows, all five modules present; matches the carried block exactly (`StratRules→Core`; `StratBridge→Core,CoreUObject,Engine,Stratocracy` + `PublicIncludePaths StratBridge,StratRules`; `StratUI→…,StratBridge` + private UMG/Slate/SlateCore; `StratPlay→…,StratUI` + private StratBridge/EnhancedInput). Field census printed exactly the six known names — no seventh. No `.Build.cs` in the diff. `.agents/ue-project-context.md` rows agree |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory |
| 7 | `Source/Stratocracy/` untouched | pass — printed nothing |
| 8 | No `/Game/` literal in C++ | pass — the three literals in `StratBoardChurnClauses.cpp:136-140` are automation-test fixture table loads, a standing exception; every hit in `StratBoardActor.h` is prose forbidding them |
| 9 | No widget-side arithmetic | pass (vacuous) — no `Source/StratUI/` file in the diff |
| 10 | Lane discipline | pass — code in `Source/StratPlay/` outside `Tests/`, clause in `Tests/`, records in `Tools/architect/state/`. The out-of-lane write is declared in both receiving files, not only in `global.md` |
| 11 | Prose blocks present | **BLOCK** — present at every new site, but the narrowing rationale asserts something this tree falsifies. See Finding 1 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries (unstaged), no `A`/`M ` column-1 marks, HEAD still `d3efe78` |

## Findings

**1. BLOCK — the stated justification for narrowing the find-path re-read to the null case is false of this tree, at four sites.**

`E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.cpp:164-168`:

```
// NARROWED TO THE NULL CASE DELIBERATELY. An unconditional re-read would call
// `SetStaticMesh` on a component that is already drawing instances on every hex of
// every refresh, and a HISM drops its instances when its mesh is set -- turning a
// no-op refresh into a full repopulate, which is the churn the early-out above
// exists to remove.
```

`LayerFor` has exactly one caller in the tree: `StratBoardActor.cpp:334`, inside `ApplyHexes`. That call is reachable only after two things have already happened:

- `StratBoardActor.cpp:307` — `if (DrawsExactlyTheseHexes(Hexes)) { return true; }`. **A no-op refresh returns here and never reaches `LayerFor` at all.** So an unconditional re-read cannot "turn a no-op refresh into a full repopulate"; the no-op refresh does not execute this code.
- `StratBoardActor.cpp:322-330` — every layer's `Layer.Tiles->ClearInstances()` and `Layer.InstanceHexes.Reset()`, run *before* the loop at 332 that calls `LayerFor`. **So the component is never "already drawing instances" when the find path runs — its instance count is zero by construction.**

Both halves of the causal claim are contradicted by the same file, ten and twenty lines below. The claim is also load-bearing in the sense the user's gate-priority ruling reserves for record prose: it is the sole recorded reason a live guard condition (`Existing.Tiles->GetStaticMesh() == nullptr`, `:169`) exists, and an engineer who later widens or removes that guard will be told by this comment that it costs a whole-board repopulate per refresh. It does not. Separately, the premise *"a HISM drops its instances when its mesh is set"* is stated flatly as fact with no measurement beside it, in a file whose house style records measurements as measurements — and nothing in this pass measured it.

Same claim, three further sites, each needing the same correction:
- `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.h:557-560` — *"an unconditional re-read would repopulate every layer on every refresh and undo the churn `ApplyHexes`'s early-out exists to remove."*
- `E:\MultiAgent\Stratocracy\Tools\architect\state\engine.md`, topmost entry, third bullet — *"call `SetStaticMesh` on every already-drawing layer on every `ApplyHexes`, turning the no-op refresh the pass below built into a full repopulate."*
- `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md`, topmost banner — *"an unconditional re-read would have reintroduced the very whole-board churn the pass below removed."*

This is the same defect class the pass itself was built to prevent — a comment made false by the code beside it, with no diff on the false line. Here the comment is *new*, and was false the moment it was written.

**Note what this finding does NOT say:** the narrowing is not itself wrong, and the code is not broken. `GetStaticMesh() == nullptr` is a correct and harmless guard; the behaviour under review is sound. What must be fixed is the four prose sites, which currently justify a correct guard with an incorrect measurement. **Owner: `strat-gameplay-engineer` for the two C++ sites; the `coordinator` for the two record sites it authored.**

## Observations

Non-gating.

- **The residual half of the same defect family is unnamed anywhere.** With the guard narrowed to null, a `TerrainMeshes` entry configured *after* a layer already wears `FallbackTerrainMesh` still never reaches that component — the exact "a late mesh does not reach an existing component" shape the fix closes, one case narrower. `StratBoardActor.cpp:168` says only *"A component that has a mesh is left untouched"*, which is accurate but does not name the consequence, and no clause pins it. Worth a line in `engine.md` so it is not read as closed by association.
- **`AnUnmeshedBoardIsNeverRememberedAsDrawn` really does hold on both sides.** I checked the claim structurally rather than by running it: clause 4's `if/else` asserts on both branches, so the fix cannot move it. The record's account of why clause 8 was needed is sound.
- **Clause 8's precondition genuinely establishes what it claims.** `bFirstOk == false`, non-empty reason, and `GetDrawnHexCount() == 0` on a freshly spawned board can only arise via the `GetStaticMesh() == nullptr` arm at `StratBoardActor.cpp:341` (the `Layer.Tiles == nullptr` arm at `:336` would also give 0, but that state makes clause 8 fail rather than falsely pass, since `:169`'s `Existing.Tiles != nullptr` skips assignment). The layers therefore exist and are null-meshed. Sound.
- **`GatherTileLayers` identifies the right components.** `HexAtInstance` (`StratBoardActor.cpp:389-415`) returns true only for a component present in `TerrainLayers` *and* holding a valid `InstanceHexes[0]`, so overlays are excluded by the class's own answer, not by elimination. It also means only layers with ≥1 instance are gathered — which is correct here, since the gather runs after the successful second apply, and a layer that drew nothing would be excluded and drop `InstanceTotal` below `Model.Hexes.Num()` rather than hide.
- **Independently re-derived figures, all matching the brief:** macro census `IMPLEMENT_SIMPLE_AUTOMATION_TEST = 287`, COMPLEX `= 0`; `StratBoardChurnClauses.cpp` moved 7 → 8 (`git show HEAD:` vs working tree), so the tree-wide delta is +1 from 286; eight `GATE-BOARDCHURN` clause names, all distinct; the new name appears exactly once as a registration.
- **The banner sweep is a weak signal and I am saying so.** `python Tools/architect/strat_banner_sweep.py` printed `SWEEP CLEAN` and exited 0 — but per this project's own record it has never exited non-zero, so the exit code carries almost nothing. What *is* worth something is the sweep's own independent parse of `Saved/AutomationReport/index.json`: `287 entries, all Success (2026.08.30-03.06.26)`, `macro census 287`, `suite claims: 43 (1 live, 42 stamped)`. That corroborates the report identity and the single live claim in `global.md` without my running the suite.
- **No file other than `global.md` carries a live suite count.** `287/287` appears once (`global.md:20`); `286/286` at `global.md:65` sits in the stamped prior banner. `engine.md` and `tests.md` both explicitly defer the figure to `global.md`.
- **Out-of-lane declarations are readable in the receiving files.** Both `engine.md` and `tests.md` carry their own block at the top disclaiming the transcription clause (no merge, no lane, no draft), and both topmost entries attribute acting and writing inline — `coordinator (ACTING AND WRITING; see the block above)` and `Acted and written by the coordinator (see the block above; there was no lane)` — so a reader arriving by citation lands on the attribution. This is the shape the standing rule asks for.
- **`RETRACTED>` markers survive as retractions.** The two lines at `StratBoardActor.cpp:265-266` are byte-identical to HEAD's `:233-234` and remain quoted-and-marked; the surrounding prose moved to past tense around them rather than absorbing them. The ten other `RETRACTED>` lines in the header are untouched.
- **Cosmetic:** `tests.md`'s new entry butts directly against the previous `- **2026-08-29 (local…` bullet with no blank line separating them.

**What I did not measure:** the build, the suite execution, the mutant/falsifiability run, and the claim that a HISM drops instances on `SetStaticMesh`. The first three are `strat-test-author`'s to report and are not reproducible from this checkout; the fourth is an engine-behaviour claim I have no instrument for here, and it is part of Finding 1 precisely because nobody in this pass had one either.

VERDICT: BLOCK

---

# Gate: `LayerFor` widening + clause 8 second half (RE-GATE) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD still `d3efe78`. Six modified files (now `583 +/50 -`, up from `417 +/50 -`) plus one untracked file, `Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md`. Nothing staged. Re-derived the module graph, the field census, the macro census, the clause roster and the two engine claims from the UE 5.8 install. Did **not** build, run the suite, or reproduce either mutant.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — `StratBoardActor.h` includes unchanged; `AssignTerrainMesh` at `:552` still needs only the forward declaration at `:95` |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — none added |
| 5 | Module arrows unchanged and correct | pass — 15 rows, all five modules, identical to the carried block; field census printed exactly the six known names, no seventh; no `.Build.cs` in the diff |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass — `SecondPlaceholderMesh` uses `/Engine/BasicShapes/Sphere.Sphere`, not `/Game/`; the only `/Game/` literals remain the fixture table loads |
| 9 | No widget-side arithmetic | pass (vacuous) — no `Source/StratUI/` file in the diff |
| 10 | Lane discipline | pass — out-of-lane write declared in both receiving files |
| 11 | Prose blocks present | **BLOCK** — four live sites describe a guard this tree no longer contains. See Finding 1 |
| 12 | Nothing staged or committed | pass — six ` M`, one `??`; HEAD unmoved |

## Findings

**1. BLOCK — the removed guard is still described as present, at four live sites, three of which were written or rewritten in this same pass.**

The guard is gone. `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.cpp:185` now reads `if (Existing.Tiles != nullptr)` — no mesh test. But four places in the tree still say the re-read happens only when the component has no mesh:

- `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.cpp:287-289` — *"THE BOARD NOW DOES TAKE A LATE MESH -- `LayerFor`'s find path re-reads the mesh configuration **when the component has none**"*. Rewritten in this diff, two hundred lines below the unconditional call it describes.
- `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.h:197-199` — *"its find path now re-reads the configuration **when the component has none**."* Rewritten in this diff. Its own sibling paragraph at `:552-560` correctly says *"the find path unconditionally"* — the two declaration blocks in one header now contradict each other.
- `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md:139-141` — the **new** stamp on the previous banner: *"A late-assigned terrain mesh DOES draw -- `LayerFor`'s find path re-reads the mesh configuration **when the component has none**"*. Written in this diff.
- `E:\MultiAgent\Stratocracy\Tools\architect\state\tests.md:212-214` — the **new** stamp on the superseded paragraph: *"`LayerFor`'s find path now re-reads `TerrainMeshes`/`FallbackTerrainMesh` **when the component has none**"*. Written in this diff.

I enumerated these by scanning the whole tree for the narrowing's vocabulary and reading each hit. Two further matches are correct as history and are **not** findings: `engine.md:189` sits inside the flicker-pass bullet that describes the deferred plan and is now covered by the discharge stamp above it, and `tests.md:76` and `:234` describe what the fix *was first written* as, in the past tense.

This is the same defect class as my original Finding 1, in the same four-site pattern, propagated the same way — and `global.md`'s own new banner names that mechanism as the lesson of the last round: *"it was propagated by being restated rather than linked."* The remediation restated it again. A reader landing on `StratBoardActor.cpp:287` is told the board does not follow a mesh **change**, which is exactly the behaviour clause 8's second half now asserts it does. **Owner: `strat-gameplay-engineer` for the two C++ sites; the `coordinator` for the two record stamps it authored.**

**2. BLOCK (record prose, and the conclusion it supports is independently sound) — the control figure quoted for the HISM claim does not reproduce on this box.**

`E:\MultiAgent\Stratocracy\Tools\architect\state\engine.md`, topmost entry: *"measured with a control: the same scan lists **30 other** `UInstancedStaticMeshComponent::` overrides in that file, so it was able to speak."*

On `C:\Program Files\Epic Games\UE_5.8\Engine\Source\Runtime\Engine\Private\InstancedStaticMesh.cpp` I get 171 occurrences of the qualified name and 146 distinct method names — not 30. The nearest figures I could find for any plausible alternate scan are 111 `virtual` and 75 `override` in `InstancedStaticMeshComponent.h`; none is 30. The entry does not state the command, so the figure cannot be reconciled from the record. In a project whose standing rule is that a stated measurement must be falsifiable from a checkout, a control figure nobody can reproduce is a control in name only — and this one is doing real work, since it is the entire reason the "no override exists" claim counts as measured rather than asserted.

**The underlying conclusion is correct and I verified it with a stronger control**, so this is about the record, not the code: `grep -rn "SetStaticMesh"` over `InstancedStaticMesh.cpp`, `HierarchicalInstancedStaticMesh.cpp`, `InstancedStaticMeshComponent.h` and `HierarchicalInstancedStaticMeshComponent.h` returns **zero hits**, against controls of 171 and 69 qualified definitions in the two `.cpp` files — the scan is amply able to speak. Neither class overrides `SetStaticMesh` in UE 5.8. **Owner: the `coordinator`** — state the command, or replace the figure with one that reproduces.

## Observations

Non-gating.

- **The widening itself is safe at this call site, and I verified the quoted engine early-out rather than taking it.** `C:\Program Files\Epic Games\UE_5.8\Engine\Source\Runtime\Engine\Private\Components\StaticMeshComponent.cpp:2366-2372`:
  ```
  bool UStaticMeshComponent::SetStaticMesh(UStaticMesh* NewMesh)
  {
      // Do nothing if we are already using the supplied static mesh
      if(NewMesh == GetStaticMesh())
      {
          return false;
      }
  ```
  The quotation in `StratBoardActor.cpp` is accurate. Two consequences worth recording, neither of them in the tree: the unchanged case returns **before** the mobility check at `:2374-2384`, so the once-per-hex call cannot spam the `SetMeshOnStatic` PIE warning; and it would not matter anyway, because `StratBoardActor.cpp:206` sets `EComponentMobility::Movable` on every tile component at creation. There is no case the widening now reaches that it should not.
- **The clause's second half genuinely pins the widening, checked at the fixture rather than through M2.** With the narrowed guard restored, the three layers-worth of components already wear `PlaceholderMesh` (Cube) after the first half, so no re-read fires and `Layer->GetStaticMesh() == SecondMesh` fails for every gathered layer — the assertion cannot pass under the blocked shape. `SecondPlaceholderMesh` is legitimate: `/Engine/BasicShapes/Sphere.Sphere` is an engine asset like the existing `Cube.Cube`, and the fixture asserts `SecondMesh != Mesh` before relying on the distinction rather than assuming two paths give two objects.
- **The changed-model choice is necessary and I confirmed the pop is actually detected.** `Shorter.Pop()` drops the last hex; `DrawsExactlyTheseHexes`'s end-of-layer loop at `StratBoardActor.cpp:300-308` (`Cursors[Index] != TerrainLayers[Index].InstanceHexes.Num()`) catches the orphaned instance and returns false, so the rebuild runs and `LayerFor` is reached. On an unchanged list it would not be — which is the residual, and the clause is shaped so as not to assert it away.
- **The residual debt is stated correctly and nothing reads as though it were closed.** `engine.md`'s *"WHAT IS STILL NOT CLOSED"* bullet carries a real discharge condition ("the board grows a way to be told its mesh configuration moved, with a clause over it"), `global.md`'s `## NEXT` restates it as new debt without folding it into the closed item, and the clause's own comment at `StratBoardChurnClauses.cpp` points at `engine.md` for it. The `## NEXT` entry's *"of the two things the flicker pass left open, the late-assigned terrain mesh is CLOSED"* is accurate, and it separately keeps the overlay-cache half open.
- **The retractions read as retractions.** `global.md:33-34` and `engine.md`'s topmost entry both quote the false claim behind `RETRACTED>` markers. `StratBoardActor.cpp:172-182` uses prose rather than a marker but is unambiguous — it quotes the old wording and then says *"The first is false of this call site by the ordering just described; the second is not supported by any override in `InstancedStaticMesh.cpp`."* A tree-wide scan for the false claim's phrasings found no unretracted live copy; the only other hits are in the persisted gate report, where they are the quoted subject.
- **My earlier cosmetic note is fixed** — `tests.md`'s new entry now has a blank line before the previous bullet.
- **The persisted gate report is untracked (`??`).** `Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md` is cited by name from `engine.md`, `tests.md` and `global.md`, and the whole reason to persist it is that a checkout can produce the quoted wording. Until it is staged with the rest, a fresh clone has three citations pointing at a file that does not exist. Agents do not commit, so this is the user's to stage — flagged so it is not lost.
- **Re-derived figures, all matching the brief:** macro census `SIMPLE = 287`, `COMPLEX = 0`; `StratBoardChurnClauses.cpp` 7 → 8 versus HEAD, so tree-wide delta is still +1 and the second half is correctly an extension rather than a new clause; eight `GATE-BOARDCHURN` names. `strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0 — and as before the exit code is near-worthless here, so what I actually take from it is its independent parse: `287 entries, all Success (2026.08.30-03.24.12)`, `macro census 287`, `suite claims: 43 (1 live, 42 stamped)`. The report identity matches the one the banner cites, and `287/287` remains the single live count, in `global.md` alone.
- **`global.md`'s new banner corrects the previous round's *"no gate ran over this change"*** to *"A gate DID run, blocked once on prose"*. That was a real error in the last draft and it is now right.

**What I did not measure:** the build, the suite run, and either mutant (M1 and M2 are not reproducible from this checkout, as the record itself states). The claim that M2 produced *seven* assertions specifically I could not check — it depends on how many tile layers retain instances after the popped hex, which is a runtime fact.

VERDICT: BLOCK

---

# Gate: `LayerFor` widening + clause 8 second half (ROUND 3) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD still `d3efe78`. Six modified files (`620 +/50 -`, up from `583 +/50 -`), one untracked (`Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md`), nothing staged. Re-derived the module graph, the field census, the macro census, the clause roster, the comment-stripped code delta, and both of `engine.md`'s newly stated engine commands. Did **not** build, run the suite, or reproduce either mutant.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — `StratBoardActor.h` includes unchanged; the only tree-wide hit is `Source/StratBridge/StratBridge.h`, not in the diff |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — none added |
| 5 | Module arrows unchanged and correct | pass — 15 rows, all five modules, identical to the carried block; field census printed exactly the six known names, no seventh |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass — only the three fixture table loads at `StratBoardChurnClauses.cpp:136-140` |
| 9 | No widget-side arithmetic | pass (vacuous) — no `Source/StratUI/` file in the diff |
| 10 | Lane discipline | pass — out-of-lane write declared in both receiving files |
| 11 | Prose blocks present | pass — see Findings |
| 12 | Nothing staged or committed | pass — six ` M`, one `??`, HEAD unmoved |

## Findings

None.

**Round-2 Finding 1 is closed.** The guard at `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.cpp:185` is still `if (Existing.Tiles != nullptr)`, and all four sites now state the outcome and defer the condition:
- `StratBoardActor.cpp:287-294` — *"the find path re-reads the mesh configuration"*, then *"THE CONDITION UNDER WHICH IT RE-READS IS NOT RESTATED HERE, DELIBERATELY … `LayerFor` is the one place that describes its own shape."*
- `StratBoardActor.h:198-200` — *"its find path now re-reads the configuration, and `LayerFor`'s own declaration is the only place that says under what condition — a shape restated here has already gone stale once."* This resolves the self-contradiction I found with `:559`; the two blocks now agree because only one of them makes the claim.
- `global.md:156-158` and `tests.md:212-213` — both reduced to *"re-reads the mesh configuration"* with the condition dropped.

`grep -rn "when the component has none"` over `Source/` and `Tools/architect/state/` returns three hits, all correctly historical and none live: `global.md:76` quotes the defect inside the lesson narrative, `tests.md:76` and `tests.md:233` are past-tense accounts of what the fix was *first* written as.

**Round-2 Finding 2 is closed, and both commands reproduce exactly.** Run against `C:\Program Files\Epic Games\UE_5.8`:
- SUBJECT — `grep -rn "SetStaticMesh"` over `Private/InstancedStaticMesh.cpp`, `Private/HierarchicalInstancedStaticMesh.cpp`, `Classes/Components/InstancedStaticMeshComponent.h`, `Classes/Components/HierarchicalInstancedStaticMeshComponent.h` → **0, 0, 0, 0**. Matches.
- CONTROL — `grep -c "UInstancedStaticMeshComponent::\|UHierarchicalInstancedStaticMeshComponent::"` on the two `.cpp` files → **171** and **74**. Matches both stated figures.
- The retracted figure's replacement — `grep -oE "^void UInstancedStaticMeshComponent::[A-Za-z]+" … | sort -u | wc -l` → **75**. Matches.

The retraction reads as a retraction: `RETRACTED>` markers on the quoted "30 other overrides", the cause named plainly (*"THE 30 WAS A `head -30` ON MY OWN PIPELINE, QUOTED AS THOUGH IT WERE A COUNT"*), and the commands stated inline so the control now reproduces from a checkout. That is the fix the finding asked for.

**The 74-versus-69 discrepancy is fully accounted for, and neither figure is wrong.** `grep -c` counts matching *lines*, not occurrences. In `HierarchicalInstancedStaticMesh.cpp`: 69 lines contain `UHierarchicalInstancedStaticMeshComponent::`, and a further 5 contain `UInstancedStaticMeshComponent::` — 69 + 5 = 74. My round-2 figure came from the HISM-only pattern; `engine.md`'s comes from the alternation it states. No action needed.

## Observations

Non-gating.

- **"The shape is described in exactly one place in the tree" is true of the code and loose about the record.** The condition is stated at `StratBoardActor.h:559` (*"the find path unconditionally"*) and `StratBoardActor.cpp:163` (*"UNCONDITIONAL, AND THE COST IS MEASURED"*) — declaration and definition of the same function, which is one place by any fair reading. But it is also stated at `Tools/architect/state/global.md:29`: *"The find path now re-reads `TerrainMeshes`/`FallbackTerrainMesh` on every layer that has a component."* That is present-tense, currently **true**, and swept as live — so it is not a finding — but it is a third copy that will go stale if `LayerFor` moves again, and it is the same category of prose I blocked twice. A record entry describing what a pass changed is the right home for that sentence, so I am not asking for its removal; I am recording that the "exactly one place" property does not hold tree-wide, so nobody later relies on a guarantee that was not actually established. `engine.md` no longer restates it and is clean on this axis.
- **I answered the tree-wide half of question 1 for the other mechanism this pass owns.** The `TerrainMeshes`-then-`FallbackTerrainMesh` decision order is stated in full at exactly one site, `StratBoardActor.h:545`; `StratBoardActor.h:208` states only the outcome (*"A HEX WHOSE `TerrainId` HAS NO MESH IS STILL DRAWN, using `FallbackTerrainMesh`"*). That is already the "outcome here, condition there" discipline, arrived at independently. No other code site restates a mechanism this pass could change.
- **I independently corroborated "the C++ change since the last suite run is comment-only", by a different route than the coordinator's.** I diffed all three source files against `HEAD` with comment lines stripped. The non-comment delta is exactly what I reviewed in round 2 and nothing more: `AssignTerrainMesh` extracted, the find path calling it under `if (Existing.Tiles != nullptr)`, the creation path delegating to it, the `SecondPlaceholderMesh` helper, and clause 8 with both halves. The code is byte-for-byte the code the round-2 mutants were run against in substance, so those mutant results still describe this binary. This is corroboration, not verification — I could not diff against the exact snapshot the suite ran on, because it exists in no commit.
- **Re-derived figures, all matching:** macro census `SIMPLE = 287`, `COMPLEX = 0`; `StratBoardChurnClauses.cpp` 7 → 8 versus HEAD (delta still +1); eight `GATE-BOARDCHURN` names. `strat_banner_sweep.py` → `SWEEP CLEAN`, exit 0 — again a weak signal on the exit code, but its independent parse reads `287 entries, all Success (2026.08.30-03.36.27)`, `macro census 287`, `suite claims: 43 (1 live, 42 stamped)`. The new report identity is restamped in both places that cite it (`global.md:103`, `tests.md:63`), and `287/287` at `global.md:20` remains the only live suite count in `Tools/architect/state/`.
- **The persisted gate report is faithful.** Both rounds appear verbatim with both `VERDICT: BLOCK` lines intact at `:103` and `:179`, my round-2 figures preserved including the superseded 69, and the preamble discloses the `&gt;` restoration. The single remaining `&gt;` in the file is at `:9`, inside the sentence describing that substitution — correct, not a leftover.
- **The persisted report's line citations have gone stale, which the preamble does not quite name.** It discloses that each remediation rewrites the prose a finding cites, so the *text* may be gone. The sharper hazard is that the *line numbers* now resolve to different, unrelated content — `StratBoardActor.cpp:287-289` and `global.md:139-141` in the round-2 text no longer point where they did. Because each finding also quotes the offending text, a reader can still grep for it, which is what saves this. Worth a one-line note in the preamble.
- **The report is still untracked (`??`)** and is cited by name from `engine.md`, `tests.md` and `global.md`. Noted as already surfaced for the user to stage; repeated here only so it is not lost between rounds.

**What I did not measure:** the build, the suite execution, and both mutants (M1 and M2 are not reproducible from this checkout, as the record states). The claim that M2 produced exactly seven assertions remains unchecked — it depends on how many tile layers retain instances after the popped hex, which is a runtime fact no static reading settles.

VERDICT: PASS

---

# Gate: `LayerFor` widening + clause 8 (ROUND 4) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD still `d3efe78`. Six modified files (`627 +/50 -`, up from `620 +/50 -` — the entire delta since round 3 is in `StratBoardActor.cpp`, 94 → 101 lines), one untracked gate report, nothing staged. Re-derived the module graph, the field census, the macro census, the clause roster, the comment-stripped code delta, the report identity at both citing sites, and what each round-2 citation now resolves to. Did **not** build, run the suite, or reproduce either mutant.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — only `Source/StratBridge/StratBridge.h`, not in the diff |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — none added |
| 5 | Module arrows unchanged and correct | pass — 15 rows, all five modules, matching the carried block; census printed exactly the six known names |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass — only the three fixture table loads |
| 9 | No widget-side arithmetic | pass (vacuous) |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | **BLOCK** — see Finding 1 |
| 12 | Nothing staged or committed | pass — six ` M`, one `??`, HEAD unmoved |

## Findings

**1. BLOCK — the quantifier was narrowed at one site and left standing at its sibling, and the two now contradict each other inside the same class.**

The narrowing you asked me to check is **correct**. `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.cpp:294-301` now reads:

> *"NO OTHER COMMENT IN THIS CLASS RESTATES THE CONDITION -- `LayerFor`'s declaration and definition are the only two, and they are the same function. THE QUANTIFIER IS "IN THIS CLASS" AND NOT "IN THE TREE" … `Tools/architect/state/global.md`'s banner states the condition too, correctly and in present tense … An earlier draft of this sentence claimed the tree-wide property and was over-broad by exactly one site."*

I scanned every live statement of the re-read condition across both class files, the clause file and all three record files. There are exactly three: `StratBoardActor.h:559` (*"the find path unconditionally"*), `StratBoardActor.cpp:163` (*"UNCONDITIONAL, AND THE COST IS MEASURED"*), and `Tools/architect/state/global.md:29` (*"on every layer that has a component"*). In-class: two, and they are the declaration and definition of one function. Tree-wide, counting decl+def as one place: over-broad by exactly one site. **Every clause of that narrowed sentence is true as written.**

But `E:\MultiAgent\Stratocracy\Source\StratPlay\StratBoardActor.h:198-200` was not narrowed with it:

```
 * only at creation; its find path now re-reads the configuration, and `LayerFor`'s own
 * declaration is the only place that says under what condition -- a shape restated here
 * has already gone stale once.
```

*"`LayerFor`'s own declaration is the only place that says under what condition"* is false twice over, and the `.cpp` block is what proves it:

- **By the `.cpp`'s own accounting.** It says the declaration and the definition *"are the only two"*. Two is not one. `StratBoardActor.cpp:163` states the condition and carries the substantive justification — the `SetStaticMesh` early-out and the `ClearInstances` ordering — which is the material a reader most needs. Told the declaration is *the only place*, a reader goes to `:559` and stops, and never reaches the measurement.
- **By `global.md:29`**, which the `.cpp` explicitly names as a third copy while the `.h`, unqualified, denies it exists.

This is the same defect I raised in round 2 at this same line — the `.h`'s `ApplyHexes` paragraph contradicting the `.h`'s `LayerFor` declaration — recurring after a remediation that fixed only the `.cpp` half. It is also exactly the failure mode the dispatch asked me to look for: narrowing a claim at one site does not narrow its copies, and an unqualified *"the only place"* sitting ten lines of reading from a sentence that says *"THE QUANTIFIER IS 'IN THIS CLASS' AND NOT 'IN THE TREE'"* is the over-broad quantifier that pass exists to remove. **Owner: `strat-gameplay-engineer`.**

The narrow fix is to give `:199` the same treatment the `.cpp` got — state where the condition lives without claiming exclusivity, or carry the same scope qualifier and count.

## Observations

Non-gating.

- **The preamble's stale-citation paragraph is right in substance and over-broad in wording, and one of its two named examples is the mild case.** It says a `file:NNN` from round 1 or 2 *"now resolves to different, unrelated content"* and names `StratBoardActor.cpp:287-289` and `global.md:139-141` as sharpest. I checked all four round-2 citations. Only `global.md:139-141` resolves to genuinely unrelated content (it now lands in the flicker pass's commit-message paragraph) — that example is exact and is the strongest case. The other three (`StratBoardActor.cpp:287-289`, `StratBoardActor.h:197-199`, `tests.md:212-214`) all still resolve to the *same paragraph, corrected* — which is different content but pointedly related, and about the friendliest outcome a stale citation can have. The paragraph's actual instruction — *"grep the quotation, not the line number"* — is correct and safe either way, so nothing misleads a reader into a wrong action. Worth tightening to "different content, and in one case unrelated content".
- **The over-broad tree-wide claim also survives in the preamble's own narration.** `Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md`, *After round 2*: *"every site now states the OUTCOME and points at `LayerFor` for the condition, so the shape is described in exactly one place."* That is the property the `.cpp` has now retracted by name. It is coordinator narration in a persisted artifact rather than code or a record entry, and it does not gate — but it is a third live copy of the sentence this pass just narrowed, and it will read as fact to anyone who finds this file later.
- **The report identity moved together at both citing sites, and the sweep agrees.** `global.md:103` and `tests.md:63` both now read `2026.08.30-03.42.56`; no stale `03.36.27`, `03.24.12` or `03.06.26` survives anywhere in `Tools/architect/state/`. `strat_banner_sweep.py` independently parses `287 entries, all Success (2026.08.30-03.42.56)`, `macro census 287`, `suite claims: 43 (1 live, 42 stamped)` — exit 0, which as always is the weak part of that signal; the parse is the useful part. `287/287` at `global.md:20` remains the only live suite count.
- **Nothing my round-3 PASS covered has been disturbed.** Census `SIMPLE = 287` / `COMPLEX = 0`, eight `GATE-BOARDCHURN` names, 15 graph rows, 6 `.Build.cs` field names, zero `strat::` crossings, vendored and `Source/Stratocracy/` untouched. The comment-stripped delta against `HEAD` is byte-identical in size to round 3 (29 and 5 diff lines for the `.cpp` and `.h`), independently confirming the change since the mutant runs is comment-only — same conclusion as round 3, reached the same way, on the newer bytes.
- **Both carried items are unchanged and unaddressed, as stated:** the gate report is still untracked (`??`), and the M2 seven-assertion count remains a runtime fact no static reading settles.

**What I did not measure:** the build, the suite execution, and both mutants (neither is reproducible from this checkout).

VERDICT: BLOCK

---

# Gate: `LayerFor` widening + clause 8 (ROUND 5) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD still `d3efe78`. Six modified files (`631 +/50 -`; the entire delta since round 4 is in `StratBoardActor.h`, 49 → 53 lines), one untracked gate report, nothing staged. Re-derived the module graph, field census, macro census, clause roster, the comment-stripped deltas for both class files, the report identity at both citing sites, and ran the exclusivity scan tree-wide myself. Did **not** build, run the suite, or reproduce either mutant.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — only `Source/StratBridge/StratBridge.h`, not in the diff |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — none added |
| 5 | Module arrows unchanged and correct | pass — 15 rows, 6 field names, no seventh |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass — only the three fixture table loads |
| 9 | No widget-side arithmetic | pass (vacuous) |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | **BLOCK** — see Finding 1 |
| 12 | Nothing staged or committed | pass — six ` M`, one `??`, HEAD unmoved |

## Findings

**1. BLOCK — question 3 answers yes. The retracted tree-wide claim is still live in `global.md`, twice, and the two copies contradict each other and the same file's own account.**

Your two round-5 targets are both clean. `StratBoardActor.h:198-205` no longer claims exclusivity — it says the condition is not restated there, disclaims knowing where it is stated, points at *"`LayerFor`'s DEFINITION, which carries the measurement behind it"* (correct: `StratBoardActor.cpp:163-172` is where the `SetStaticMesh` early-out and the `ClearInstances` ordering live, so a reader does now reach the measurement), and retracts the old wording by name with both refutations stated. Both preamble corrections are accurate and both read as corrections rather than fresh claims. I confirmed the stale-citation paragraph against my own round-4 measurement: `global.md:139-141` does land in the flicker pass's commit-message paragraph, and the other three do resolve to the same paragraph corrected.

But the scan you ran was for the *phrasings you had used*. Two copies survive in different words, in `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md`:

- **`global.md:79`**, inside the topmost live banner (lines 14-107), in the lesson paragraph:
  > *"Every one of those sites now states the OUTCOME and points at `LayerFor` for the condition, and **`LayerFor` is the only place in the tree that describes its own shape**."*

  That is the tree-wide property, verbatim in substance, retracted at `StratBoardActor.cpp:294-301` and at the gate report's `:36`. It is false — and it is falsified by **line 29 of its own banner**, fifty lines above: *"The find path now re-reads `TerrainMeshes`/`FallbackTerrainMesh` on every layer that has a component."* The banner's account paragraph states the condition; the banner's lesson paragraph then says nothing outside `LayerFor` does.

- **`global.md:158`**, in the stamp written this pass on the previous banner:
  > *"the topmost banner in this file is the account, **which is also the only place the CONDITION is stated**."*

  Also false — `StratBoardActor.h:559` and `StratBoardActor.cpp:163` both state it. And it contradicts `:79` directly: one says `LayerFor` is the only place, the other says the banner is. They cannot both hold, and neither does.

The aggravating detail is where `:79` sits. Its own paragraph opens *"A GUARD'S SHAPE WAS RESTATED AT FOUR SITES INSTEAD OF LINKED TO … THEN THE REMEDIATION DID IT AGAIN, IN THE SAME PASS, WHILE THE BANNER ABOVE IT NAMED THAT EXACT MECHANISM AS THE LESSON."* The sentence stating the lesson is itself the un-narrowed copy the lesson is about — the fourth occurrence, in the paragraph written to prevent a fourth occurrence.

**Owner: the `coordinator`** (`global.md` is its file and the steward's). The fix is the one already applied twice: at `:79` scope the claim to the class and name `global.md:29` as the third copy; at `:158` drop or qualify *"the only place the CONDITION is stated"*.

## Observations

Non-gating.

- **`strat_banner_sweep.py` prints `SWEEP CLEAN — no self-contradiction found` over a file that contradicts itself twice.** That is not a sweep defect — its subject is suite claims and banner stamps, and it correctly reports `287 entries, all Success (2026.08.30-03.48.37)`, `macro census 287`, `43 claims (1 live, 42 stamped)`. But the phrase "no self-contradiction found" is broader than what it measures, and this pass is now the second time a reader could take that line as covering prose it never looks at. Worth knowing before anyone cites it as coverage.
- **The two carried items remain open, as stated:** the gate report is still untracked (`??`), and the M2 seven-assertion count is still a runtime fact no static reading settles.
- **Rounds 3 and 4 are cited by the persisted report but not contained in it.** The preamble now says *"Raised by the reviewer in round 3"* (`:16`, `:21`) and *"again in round 4, where the unnarrowed sibling in the header was a BLOCK on its own"* (`:40`), while the header still reads *"TWO ROUNDS, BOTH `VERDICT: BLOCK`"* and the file holds only rounds 1-2. That is accurate about the file's contents and honest about attribution, but it reintroduces in miniature the problem the file was created to solve: two of the four rounds it cites cannot be produced from a checkout. Adding them, or noting in the header that later rounds are cited but not transcribed, would close it.
- **Both engine-side claims and the comment-only claim re-verified.** The `.h`'s only non-comment delta versus `HEAD` is exactly the three-line `AssignTerrainMesh` declaration, as stated. The `.cpp`'s comment-stripped delta is 29 lines — byte-identical in size to rounds 3 and 4, so the code has not moved since the mutants ran. Report identity moved together at both citing sites (`global.md:103`, `tests.md:63`, both `2026.08.30-03.48.37`); no stale stamp survives. `287/287` at `global.md:20` remains the only live suite count.
- **Everything my round-3 PASS covered is undisturbed:** 15 graph rows, 6 `.Build.cs` field names, zero `strat::` crossings, census `SIMPLE = 287` / `COMPLEX = 0`, eight `GATE-BOARDCHURN` names, vendored and `Source/Stratocracy/` untouched.

**What I did not measure:** the build, the suite execution, and both mutants (neither is reproducible from this checkout).

VERDICT: BLOCK
