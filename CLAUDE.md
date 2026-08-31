# Stratocracy

Turn-based hex strategy in Unreal Engine 5.8, C++17. The rules are deterministic and headless;
the engine asks them for answers and never decides an outcome itself.

## Non-negotiables

1. **Never edit `Source/StratRules/` or `Data/`.** Vendored certified bytes, hash-gated against
   `E:\MultiAgent\stratocracy-crew`. Fix upstream and re-vendor; file the request instead.
2. **Never call a `strat::` function outside `Source/StratBridge/` or `Source/StratRules/`.**
   The vendored sources carry no `_API` macro — a cross-module call is `LNK2019`, measured 8×.
   Route it through an `FStratBridge` method.
3. **Never `#include "StratBridge.h"` from a header that declares a `UCLASS`/`USTRUCT`/`UENUM`.**
   It would put the vendored headers in front of UHT. Forward declare, include from the `.cpp`,
   and hold the bridge as `TPimplPtr` — never `TUniquePtr` (measured: `C4150`).
4. **Never write a `/Game/` path literal into gameplay C++.** Asset references are
   `EditDefaultsOnly` properties set on a Blueprint default. Automation-test fixtures and the
   import commandlet are the two standing exceptions.
5. **Never `cat` the GDD.** It is at `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`,
   446 KB with ~100 KB lines. Grep it or slice it.
6. **Agents do not commit.** Staging and commits are the user's call.

## Where the detail lives

- **Architecture, module arrows, and the reason for each** → `.agents/ue-project-context.md`
- **What is built and what is next** → `Tools/architect/state/` — the live record, one file per
  owning agent. `global.md` carries the banner and milestone status and is **the only file that
  may state a suite count or a phase verdict**; the others link to it rather than restating it,
  and `strat_banner_sweep.py` enforces that mechanically.
- **The reasoning behind a closed phase** → `Tools/architect/state.md`, FROZEN 2026-08-20. Read
  it; never edit it. Nothing sweeps it.

When a note disagrees with the working tree, **the tree wins** — then fix the note.

## The crew

Five subagents in `.claude/agents/` build this project. `/strat-hotseat` sequences them in a
single tree; `/strat-parallel` runs them across worktrees, which is the mode for the remaining
milestones. Lanes are strict: `strat-gameplay-engineer` owns C++ outside `Tests/`,
`strat-test-author` owns `Tests/` only, `strat-editor-builder` owns `Content/` through the live
editor, `strat-data-steward` owns `Config/` and `Tools/architect/`, and
`strat-integration-reviewer` is a read-only gate.

**The one place a lane reaches outside its own directory is its own record file.** Since the
2026-08-20 split, `Tools/architect/state/` holds one file per owning agent, and each agent writes
**only its own**: `engine.md` is the engineer's, `tests.md` the test-author's, `content.md` the
editor-builder's, `data.md` and `decisions.md` the steward's, `global.md` the coordinator's and
the steward's. So "the steward owns `Tools/architect/`" holds for everything in that directory
**except** those three files. Each file names its writer in its own header.

**The transcription clause. USER RULING, 2026-08-29.** **Five of the six sole-writer lines**
in `Tools/architect/state/` -- `engine.md`, `tests.md`, `data.md`, `decisions.md` and
`content.md`, and NOT `global.md`, per limit (4) below -- now read *except that the `coordinator` transcribes lane-authored
entries after a merge, attributing acting and writing separately.* It is a documented rule
rather than a per-write exception, and it exists because the per-write alternative was measured
and found fragile: on 2026-08-29 the hand-written exception blocks **drifted between `engine.md`
and `tests.md` inside one commit, by one author, on the same day and the same axis a gate had
blocked for that morning** -- `tests.md` credited the writing to the agent that had not written
those bytes. **The cost the user named in ruling it: it removes that drift risk, and costs a
little of the strictness that makes ownership legible.**

> **What it licenses, and the four limits that make the trade pay.** (1) **Transcription only.**
> It carries across what the lane agent already wrote and delivered as a draft entry; it does
> NOT license the `coordinator` to author a lane's record content. (2) **After a merge only** --
> the condition that creates the need, since two lanes editing one record file conflict on
> rebase, so lanes are told not to write these files in their trees. **The entry MUST NAME THE
> MERGE COMMIT it follows**, or the condition is satisfied forever after any past merge, which
> is no condition at all; naming it costs nothing, as both entries written under this clause
> already did. (3) **Acting and writing
> attributed separately, INLINE in the entry** -- `2026-08-29, X (ACTING; entry TRANSCRIBED by
> the coordinator)` -- and inline is load-bearing rather than stylistic: a reader arriving by a
> citation lands on the entry, not on any header above it, so an entry that names only one actor
> is an affirmative false claim about authorship and is a finding. **This is the legibility the
> ruling trades a little strictness for; it is not optional.** (4) It does not touch
> `global.md`, whose writer is already the `coordinator` and the steward.
>
> **AND LIMIT (1) IS UNFALSIFIABLE FROM A CHECKOUT, STATED HERE RATHER THAN LEFT TO BE
> DISCOVERED.** Nothing requires the lane's draft to exist as an artifact, so a `coordinator`
> who authored an entry wholesale and opened it `X (ACTING; entry TRANSCRIBED by the
> coordinator)` is **indistinguishable from a real transcription by every instrument in this
> tree**. No wording fixes that; only an artifact does -- persist the lane's draft under
> `Tools/architect/` and have the entry cite it, which would close the identical gate-report
> gap this record already carries as open. Until then limit (3) rests on a grep and limit (1)
> rests on the agent's honesty, and a reader is entitled to know which is which.

This is a different clause from `content.md`'s fallback writer below, and the two must not be
conflated: that one is triggered by an ABSENT TOOL and licenses AUTHORING; this one is triggered
by A MERGE and licenses only carrying across what a lane already wrote.

`content.md` has one standing exception, added 2026-08-23: the `coordinator` is its **fallback
writer** when `execute_script` is absent from the session's tool surface, because without that
tool `strat-editor-builder` has no route to the editor and cannot write anything. The condition
and the two obligations that ride with it are stated in that file's own header, which is the
authority. No other record file has a fallback writer.

**Writing `content.md` and driving the editor are two different permissions, and until
2026-08-26 only one of them was granted.** The fallback above licenses the FILE WRITE — its own
header says so in terms — and it kept being cited for the ASSET WORK as well, because the same
absent tool causes both. Three consecutive `strat-integration-reviewer` gates turned on that
wording while the asset it governed was clean from the first read. So the second permission is
now stated here, where lanes are defined, rather than inferred from a record file's header:

> **The editor-driver clause.** When `execute_script` is absent from the session's tool surface,
> the `coordinator` may drive the live editor to do content-lane work — author and edit assets,
> set Blueprint defaults, run PIE — because `strat-editor-builder` holds the NeoStack tools and
> no Bash and has no route to the editor at all. **Two obligations ride with it, and they mirror
> the file-write clause's own.** (1) **Measure the absence with a control before invoking it.**
> An absent tool name proves nothing alone; show the same lookup returning the tools that ARE
> served. (2) **Attribute the acting and the writing separately in the record entry.** This
> clause covers the driving; the `content.md` header covers the file write; neither covers the
> other half, and naming one actor while leaving the other unnamed is itself a finding.

Nothing here widens the lane rule for a session where the editor IS reachable — when
`strat-editor-builder` can work at all, `Content/` is its lane and only its lane.

**A defect reported in a live session has no lane to route it to, and for four passes that was
handled by writing a fresh exception block each time.** Lanes are assigned per dispatched phase,
and both defined modes are wave-shaped — `/strat-hotseat` runs the crew phase by phase in one
tree, `/strat-parallel` across worktrees. Nothing in `.claude/agents/`, `.claude/skills/` or this
file describes a route for work that arrives mid-session, so in that condition every code write is
out of lane BY DEFINITION. Four entries said so in nearly the same words — 2026-08-29
(`LayerFor`), and three on 2026-08-31 over bases `ee4acf5`, `da06812` and `edda819`, each
recording *"there was no lane, no worktree, no branch, no merge and no draft."* Four identical
exceptions are a condition, not four accidents:

> **The in-session clause. USER RULING, 2026-08-31.** When the user reports a defect or asks for a
> change **in a live session with no lane topology standing** — no worktree, no lane branch, no
> dispatched wave in flight — the `coordinator` may write code in any lane. **The condition is not
> "the user asked"; it is "the user asked AND there is no lane to ask."** Both halves are
> checkable: `git worktree list` and the current branch say whether a topology exists.
>
> **THIS IS NOT THE EDITOR-DRIVER CLAUSE'S REASONING AND MUST NOT BE READ AS IT.** That clause is
> triggered by AN ABSENT TOOL: `strat-editor-builder` holds no Bash and no `execute_script`, so it
> **cannot act at all**. `strat-gameplay-engineer` and `strat-test-author` hold `Read`, `Write`,
> `Edit`, `Bash` and `PowerShell` and are FULLY CAPABLE of this work. Nothing here rests on
> incapacity, and no measurement of an absent tool discharges anything below.
>
> **THE FOUR OBLIGATIONS.**
>
> 1. **OFFER BOTH ROUTES BEFORE ACTING, AND RECORD WHICH THE USER CHOSE.** This is the obligation
>    the other clauses do not have, and it is here because it is the step that actually produced
>    all four writes. In each case the `coordinator` offered ITSELF as the writer and the user's
>    "yes" was then recorded as *"the user's explicit instruction in session"* — a true sentence
>    about a menu with one item on it. The instruction is only meaningful if dispatching the lane
>    agent was the other item. **Name the cost of each when offering:** dispatch is slower and
>    needs a brief the agent cannot check against the conversation, and briefs from this seat have
>    been measured wrong (2026-08-31: a stale HEAD and a collapsed mutant sequence, both caught by
>    the agents against the tree); acting in session is faster and skips the brief, and puts the
>    write outside its lane.
> 2. **THE RECEIVING FILE GETS THE ENTRY, AND THE LANE AGENT WRITES IT.** The `coordinator` does
>    NOT author the lane's record entry. Dispatch `strat-gameplay-engineer` for `engine.md` and
>    `strat-test-author` for `tests.md` after the code lands; recording an out-of-lane write only
>    in `global.md` is a verdict block. **Dispatch them in SEPARATE messages, not concurrently**
>    — on 2026-08-31 two lanes run in one message each wrote a true status claim about the
>    other's file that was false before either finished, and no time qualifier fixes it, because
>    in a parallel session the qualifier's window can be zero.
> 3. **ATTRIBUTE ACTING AND WRITING SEPARATELY, INLINE IN THE ENTRY** — the shape is
>    *2026-08-31, the* `coordinator` *(ACTING; OUT OF LANE, in session) and* `<lane agent>`
>    *(WRITING)*. Inline is load-bearing for the reason the transcription clause already gives: a
>    reader arriving by a citation lands on the entry, not on any header above it.
> 4. **NAME THE BASE COMMIT THE WORK WAS DONE OVER.** Without it the condition reads as satisfied
>    forever, which is the defect the transcription clause's merge-naming requirement exists to
>    prevent. All four prior entries already named theirs, so this costs nothing.
>
> **WHAT IT DOES NOT LICENSE.** It does not apply when a lane topology IS standing — with a
> worktree or a wave in flight, `Source/` is the lane's and dispatch is the route. It does not
> reach `Content/`, which the editor-driver clause governs on its own separate condition. It does
> not license the `coordinator` to author a lane's RECORD entry, which is obligation (2) and is
> the line between this clause and the transcription clause. And it is not a licence to skip the
> reviewer gate.
>
> **OBLIGATION (1) IS UNFALSIFIABLE FROM A CHECKOUT, STATED HERE RATHER THAN LEFT TO BE
> DISCOVERED.** Whether both routes were genuinely offered lives in the session transcript, which
> no gate reads and no checkout contains. A `coordinator` that offered only itself and then wrote
> *"the user chose in-session over dispatch"* is **indistinguishable from one that offered both**
> by every instrument in this tree. No wording fixes that. Obligations (2), (3) and (4) are
> greppable in the record; (1) rests on the agent's honesty, and a reader is entitled to know
> which is which — the same split the transcription clause already declares about its own
> limit (1).
