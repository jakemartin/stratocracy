# The Stratocracy Code Architect

A goal-oriented coding agent for **Stratocracy** (UE 5.8, C++17, turn-based hex
strategy). It reads the game's GDD, scans this repository, works out what is missing,
scores the candidates against each other, and writes the code for the one that wins.

Assignment #5, MAIGD Class 7. **Raw orchestration — no CrewAI, no agent framework.**
One direct Anthropic Messages API call per generation step, a prompt this repo
assembles by hand, and a parser it owns.

---

## Run it

```bash
cd Tools/architect
python architect.py
```

That is a **propose-only** run: it scans, scores, generates, and writes everything
into `runs/<timestamp>/` without touching the tree. Read the files, then:

```bash
python architect.py --apply
```

No API key? Every live call is recorded, and the recordings are committed:

```bash
python architect.py --offline
```

`--offline` needs no key and no packages at all — the whole reasoning layer is standard
library, and the only dependency (`anthropic`) is imported lazily on the live path.

| Flag | Effect |
|---|---|
| `--apply` | write generated files into the tree, then re-scan and iterate |
| `--offline` | replay recorded calls from `replay/` instead of calling the API |
| `--max-iterations N` | cap the loop (default 3) |
| `--gdd PATH` | point at the GDD (default: `../../../stratocracy-content/Stratocracy_Prototype_GDD.md`) |
| `--diff-report RUN` | diff what the agent wrote against what is in the tree now |

The API key is read from `ANTHROPIC_API_KEY`, else from `../../../stratocracy-crew/.env`.
Model defaults to `claude-opus-5`; override with `--model` or `STRATOCRACY_ARCHITECT_MODEL`.

---

## How it works

```
gdd.py    →  read the GDD          §4.11 build-order table + §4.7 stub invariants
scan.py   →  read the codebase     modules, deps, asserted IDs, capability probes
candidates.py → the gap set        open acceptance IDs → what would have to exist
score.py  →  priority_score()      dependency order · blockers · GDD priority · state
prompts.py→  assemble context      GDD's own words + the real contract + a pattern file
write.py  →  generate              one raw API call; record/replay
architect.py → the loop            scan → diff → score → write → re-scan
blackboard.py → make it visible    every decision, before it takes effect
```

### The GDD is parsed, not summarised

The GDD is ~440,000 characters on very long lines. Nothing reads it whole. Two
structures are parsed out of it:

- **§4.11's build-order table** — a dependency graph that is already written down. Each
  row states what it depends on and which acceptance IDs close it. The scorer's
  dependency edges come from there, so the agent does not invent an ordering the
  document already states.
- **§4.7's stub invariants** — 71 `T-XXX-NN` definitions. These are quoted *verbatim*
  into generation prompts, so the model builds against the GDD's words rather than my
  paraphrase.

It also reads §4.11's `†` marks, which flag the acceptance IDs whose gate needs the
**editor pass** rather than a headless run — precisely the IDs waiting on a real
engine-side artefact rather than on a design decision.

### The codebase scan asks about call paths, not file listings

An acceptance ID does not close because a file exists; it closes because something can
**call** something. So `scan.py` runs *capability probes* — named questions with the
regex that answers them, printed to the blackboard question-and-verdict so a reader can
check whether the agent asked the right thing:

```
[ NO] gamestate_to_uiworld   Can an engine-side caller obtain a Ui.h UiWorld from the
                             bridge — i.e. from the strat::GameState FStratBridge owns?
```

Probes run against a **comment-stripped** copy of each file. This project's headers
discuss their own absent features at length; `StratBridge.h` named `strat::UiWorld`
four times while declaring nothing, so a probe matching raw text would have answered
YES about prose.

### The scorer

`priority_score()` implements the four axes Class 7 names and nothing else — dependency
order, blockers, project priority, current state. Every weight is a module constant in
`score.py` so the ranking can be audited instead of believed.

Two deliberate choices:

- **Readiness is not folded into the score.** A blocked candidate keeps its full value
  and is marked NOT ACTIONABLE. That way the blackboard shows a high-value item being
  passed over *because of its blocker*, rather than sinking down the table for reasons
  a reader has to reconstruct.
- **Candidates the GDD explicitly leaves unruled are not scored at all.** §4.7 Stub 8
  says in as many words that T-UI-04's buildlist "has no stated shape — field or query —
  and inventing one here would pre-empt a Director ruling." The agent lists it, scores
  it `excluded`, and says why. A number there would be a vote in a decision the document
  reserves.

---

## Did the agent do the five things — and how

**Read the GDD.** Yes. `gdd.py` parses a 441,539-character document into two
structures: §4.11's build-order table (10 ledger rows, each with its dependency cell,
headless flag and acceptance IDs, plus the `†` marks for IDs needing the editor pass)
and §4.7's stub invariant blocks (71 `T-XXX-NN` definitions). It never reads the
document whole — the lines run to thousands of characters — and the invariant text is
quoted *verbatim* into generation prompts rather than summarised, so the model builds
against the GDD's words and not a paraphrase.

**Scan the codebase.** Yes. `scan.py` walks `Source/`, parses every `*.Build.cs` into a
module dependency graph, and records which acceptance IDs a test **in this tree**
actually asserts — the GDD says which IDs exist, only the tree says which are gated
here. It then runs *capability probes*: named questions with the regex that answers
them, evaluated against a comment-stripped copy of each file, because this project's
headers discuss their own absent features at length.

**Detect gaps.** Yes, and at two levels. An acceptance ID is OPEN when the GDD declares
it and no test asserts it. But an ID does not close because a file exists — it closes
because something can **call** something, so the probes ask about call paths. That is
what found the real gap: every file needed for the UI layer existed, compiled and was
vendored, and no engine-side caller could reach the view model at all.

**Prioritize.** Yes. `priority_score()` implements the four axes Class 7 names —
dependency order, blockers, project priority, current state — with every weight a module
constant so the ranking can be audited instead of believed. Readiness is deliberately
*not* folded into the score: a blocked candidate keeps its full value and is marked NOT
ACTIONABLE, so the blackboard shows a high-value item passed over *because of its
blocker*. Candidates the GDD explicitly leaves unruled are not scored at all.

**Generate code.** Yes. `write.py` makes one direct Anthropic Messages API call per
step — raw orchestration, no framework — with the prompt assembled by hand in
`prompts.py` and parsed by a delimiter this repo owns. Generated files land in the run
directory first; `--apply` is what writes them into the tree.

**How.** The loop in `architect.py`: scan → diff against the GDD → score → write →
re-scan, and stop when nothing actionable remains. Iteration 2's scores depend on what
iteration 1 changed, which is how the scoreboard became buildable only after the
projection it binds to existed. Everything is logged to the blackboard *before* it takes
effect: what was scored and why, the exact prompt issued, and the generated bytes.

**Could it run in the game?** Yes — compiled in UE 5.8, `Stratocracy.StratUI.T-UI-03.*`
passes 10/10 and the full suite is 18/18. See the verification table below.

---

## What the agent decided

Full record: `evidence/`. The first scored ranking, from a pristine tree:

| # | score | status | candidate | closes |
|---|---|---|---|---|
| 1 | **95** | **ACTIONABLE** | `bridge_view_model` | — |
| 2 | 65 | blocked | `scoreboard_widget` | T-UI-03 |
| 3 | 35 | blocked | `production_widget` | T-UI-04 |
| 4 | 35 | blocked | `presentation_statelessness` | T-INT-05 |
| 5 | 5 | ACTIONABLE | `bridge_event_list` | — |
| 6 | — | excluded | `buildlist_query` | — |

**The winner closes no acceptance ID at all**, and that is the interesting part. Its
entire score is what it unblocks:

> BLOCKS 3 downstream candidates: scoreboard_widget, production_widget,
> presentation_statelessness. — closes no acceptance ID directly; its value is what it
> unblocks. — READY: every prerequisite is present in the tree.

This is the gap: `strat::buildUiSnapshot` takes a `strat::UiWorld`, and the only
function in the tree that builds one is the headless driver's `uiWorldOf(const
Session&)`. `FStratBridge` owns a `strat::GameState`, not a `Session`. Every file
involved existed, compiled and was vendored — and **no engine-side caller could reach
the view model at all.** Three acceptance IDs were waiting behind a function that did
not exist.

Iteration 2 re-scanned, the probes flipped, and `scoreboard_widget` became actionable
and was selected — the live standings scoreboard of GDD §2.11.4, which is T-UI-03's
subject.

---

## What the agent built

Two artefacts, over two iterations of the loop.

**Iteration 1 — the bridge view-model surface** (`Source/StratBridge/`). Adds
`FStratBridge::MakeUiWorld()` and `MakeUiSnapshot()`, and retains the parsed
`strat::Scenario` that `LoadScenarioFromFile` had been discarding. That last part
mattered more than it looks: `UiWorld::guided` has no other source, and with it null
`isGuidedMarked` is silently false for every unit — the projection would not have
failed, it would have quietly unmarked the one unit §2.11.6's beat 1a is about.

**Iteration 2 — the live standings scoreboard** (`Source/StratUI/`), T-UI-03's subject:
a `USTRUCT` row model, a `UCLASS(Abstract, Blueprintable)` widget, and a 10-clause
Automation test under `Stratocracy.StratUI.T-UI-03.*`.

**Iteration 3 — the runtime host** (`AStratScoreboardHUD`). Once the widget existed, a
gap the scoreboard had been hiding became the top actionable candidate: *nothing outside
the Automation tests ever constructed an `FStratBridge`*. `StratBridge.Build.cs` says so
itself — "Nothing in the game module calls the bridge yet." The scoreboard was proven by
a test and could not appear in a running game. The host owns a bridge, loads the two
§4.8 tables, seeds the shipped scenario, creates the widget and refreshes it.

Its reasoning is worth reading: it argued itself out of a `UGameInstanceSubsystem` and
into an `AHUD` on three grounds — a GameMode points at a HUD as *asset data*, so the
game module never names `StratUI` in C++ and the un-reversible dependency arrow stays
intact; seeded state is per-map, and a GameInstance subsystem would carry one scenario's
`GameState` into the next level; and a scoreboard is client-local. It then recorded its
own architectural debt unprompted — this HUD owns the authoritative `GameState` only
because nothing else yet does, which contradicts §4.1's "never own rules" — and wrote
`RefreshScoreboard` to read through `GetBridge()` so ownership can move later without
touching the refresh path.

Every displayed value equals exactly one snapshot field, and the three §2.11.4 rules
that are easy to get wrong all landed right: `fameCombat` and not `fameTotal` (passive
income excluded, as the tiebreak excludes it), `objectiveTotal` for the N of "X of N"
rather than the literal 8, and `Snapshot.match.turnCap` rather than a hardcoded 20.

---

## What I changed before accepting it

### One correction to generated code

The Automation test called `strat::uiCheckSnapshotFidelity` and iterated the result as
a list of `{id, reason}` records. It returns a `UiFidelityResult` **struct** — `ok`,
three counters, and a `failures` vector of `{clause, field, detail}`. Not iterable.
It would not have compiled.

**The cause was my prompt, not the model.** I told the step to call that function while
my `Ui.h` excerpt stopped at `// Read-only projection` — before the T-UI-05 machinery
begins. With no declaration to read, the model guessed, and said so in its own comment.
It invented no other API, because every other API it was told to call was in the prompt.

Fixed in both places: the test now reads `Result.failures` and checks `Result.ok`
explicitly rather than inferring success from an empty list; and `prompts.py` now ships
the fidelity declarations to that step, so the agent cannot repeat it. The exact diff is
`evidence/02-scoreboard-widget/acceptance_diff.md`, generated by `--diff-report`.

### One correction that only a build could find

The test called `strat::uiCheckSnapshotFidelity` directly from `StratUI`. It compiled
and then failed to **link**:

```
LNK2019: unresolved external symbol strat::uiCheckSnapshotFidelity
```

This is the exact failure `StratBridge.Build.cs` documents: the vendored `strat`
sources carry no `_API` macro, so `UnrealEditor-StratBridge.dll` exports `FStratBridge`
and nothing beside it, and a cross-module call into `strat::` cannot resolve. That file
records the first bridge hitting it as "8 × LNK2019"; this was the second time anything
tried.

Fixed the way the module's own design says to: `FStratBridge::CheckSnapshotFidelity()`
now runs the check inside the module that compiles the rules sources and hands the
failures back as `FString`s, and the test calls that. It forwards the module's `ok`
rather than re-deriving it from an empty failure list.

Worth being precise about what this says: **source-level review did not catch it, and
could not have.** Every symbol existed, every type matched, every call was spelled
correctly. Only the linker knew. That is the argument for the Working Feature line
being a build and a test run rather than a reading.

### A second one only a build could find

The host held the bridge as `TUniquePtr<FStratBridge>` behind a forward declaration and
declared both its constructor and destructor out-of-line — textbook pimpl, and exactly
what the prompt asked for. It still failed:

```
C4150: deletion of pointer to incomplete type 'FStratBridge'
```

The deleter was not instantiated in any hand-written file. UHT emits a vtable-helper
constructor into `Module.StratUI.gen.cpp`, a translation unit that includes the header
and never includes `StratBridge.h` — an out-of-line destructor cannot reach generated
code.

Switched to `TPimplPtr`, which is UE's own answer: its header states the deleter is
"determined at binding time and type-erased, allowing the object to be deleted without
access to the definition of the type". Erasure happens at the `MakePimpl` call site in
the `.cpp`. Every call site — `Get`, `Reset`, `IsValid`, `operator->` — was unchanged.

Two corrections, both invisible to review and both found by compiling. That is the whole
case for the Working Feature line being a build.

Everything else was accepted unchanged.

### Four defects in the agent itself

Found by running it, and all four are recorded in the code beside the fix:

1. **Probes tested a shape, not a capability.** They looked for a free function
   returning `UiWorld`; the model wrote the project's out-parameter convention,
   `FStratResult MakeUiSnapshot(strat::UiSnapshot&) const`. The agent could not see its
   own success and regenerated the same file on all three iterations. Probes now ask
   whether an *operation* is declared.
2. **Probes matched comments.** `StratBridge.h` names `strat::UiWorld` four times in
   prose while declaring nothing. Probes now run against a comment-stripped copy.
3. **A silent `max_tokens` failure.** A step returned `output_tokens = 16000` and *zero*
   text blocks — the whole budget went to reasoning and the answer never began. The
   recording captured neither `stop_reason` nor block types, so it surfaced as "no FILE
   block", which described the symptom and not the cause. Both are now recorded and
   warned about.
4. **One call could not finish three files.** Header, implementation and test together
   hit the cap with two never started. The scoreboard is now generated in three chained
   steps, each handed the previous step's real bytes rather than a description.

### Decisions I made, not the agent

- **`StratUI` is a new module.** The widget could not live in `Stratocracy` (the
  dependency runs `StratBridge → Stratocracy`, so the game module cannot include the
  bridge) and should not live in `StratBridge` (a `UCLASS` there would put UHT in front
  of the vendored `strat` headers, which `StratBridge.h` was written to prevent). The
  deciding factor was that it is **irreversible**: a Blueprint deriving from the widget
  bakes `/Script/StratUI.StratScoreboardWidget` into the asset, exactly as
  `/Script/Stratocracy.UnitRow` is baked into DT_Units. Cheap now, asset surgery later.
- **The scorer was written before I knew what it would pick.** I expected the widget
  layer to win. It ranked the bridge projection first instead, on a gap I had not seen
  until I read the tree: `buildUiSnapshot` takes a `UiWorld` and nothing could build one
  from a `GameState`.
- **The buildlist candidate is excluded rather than scored**, because §4.7 Stub 8 says
  its shape is deliberately unruled.

---

## Honest limits

- **T-UI-03 is not "closed".** The GDD applies Q29 per acceptance ID at one commit; this
  is the editor-pass artefact the ID was waiting on, and the ledger row's flip is the
  Director's call, not this agent's.
- The **presentation block** (`UiPresentation`) is still unfilled — its owner is a
  selection machine that does not exist — so `T-INT-05` remains open. The scoreboard
  binds only to parts of the contract that are actually filled, which is part of why it
  was the right first widget.
- `prompts.py` has curated context for two candidates. Any other selection raises
  `NotImplementedError` and stops loudly rather than generating from a thin prompt.
- The GDD snapshot under `gdd_snapshot/` is **not** hash-gated the way
  `Source/StratRules/` is. It can go stale silently; the document in
  `stratocracy-content` is the real one.

---

## Rubric map

| Line | Where |
|---|---|
| Working Feature /4 | `Source/StratUI/` — the §2.11.4 scoreboard, from the GDD and not invented for the assignment. Compiles in UE 5.8; run `Stratocracy.StratUI.T-UI-03.*` from Session Frontend |
| Agent Code /3 | `Tools/architect/` — parses the GDD, probes the codebase for call paths, scores, generates. Raw orchestration, no framework |
| Judgment & Review /2 | "What I changed before accepting it" above, and `evidence/*/acceptance_diff.md` — machine-generated, not narrated |
| Code Quality /1 | matches the project's conventions; `turnCap` and objective `N` read from the scenario, never hardcoded |

### Verification status

| Claim | Status |
|---|---|
| The agent runs live end to end | verified — 4 recorded calls across 2 iterations |
| The agent runs keyless via `--offline` | verified — replayed from a pristine `HEAD` tree into all 6 files |
| Generated bridge + widget + host compile in UE 5.8 | verified — `StratocracyEditor Win64 Development`, Result: Succeeded |
| `Stratocracy.StratUI.T-UI-03.*` passes | verified — **10 succeeded, 0 failed, 0 not run** |
| No regressions elsewhere | verified — the whole `Stratocracy` suite is **18/18** after the host landed too, including `T-INT-02`, `T-INT-03` and `T-DATA-05`, which run over the bridge this change touched |

```
[Success] Stratocracy.StratUI.T-UI-03.ChevronFollowsCriteriaOrder
[Success] Stratocracy.StratUI.T-UI-03.ColumnsFollowViewingSide
[Success] Stratocracy.StratUI.T-UI-03.DestroyedBindsCombatFame
[Success] Stratocracy.StratUI.T-UI-03.NoEngagementsMirrorsZeroCombatFame
[Success] Stratocracy.StratUI.T-UI-03.ObjectivesBindHeldOfScenarioTotal
[Success] Stratocracy.StratUI.T-UI-03.RefusesRatherThanShowingZeroes
[Success] Stratocracy.StratUI.T-UI-03.RowsInTiebreakOrder
[Success] Stratocracy.StratUI.T-UI-03.SnapshotFidelity
[Success] Stratocracy.StratUI.T-UI-03.TurnAgainstCap
[Success] Stratocracy.StratUI.T-UI-03.UnitHpBindsSurvivingStrength
```

Reproduce it:

```bash
"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "Stratocracy.uproject" -ExecCmds="Automation RunTests Stratocracy.StratUI.T-UI-03;Quit" -unattended -nopause -nosplash -nullrhi -ReportExportPath="Saved/AutomationReport"
```
