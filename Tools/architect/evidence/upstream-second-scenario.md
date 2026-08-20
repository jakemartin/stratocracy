# Upstream request — a second scenario file, to move the SCENARIO axis

**Filed against** `E:\MultiAgent\stratocracy-crew`, HEAD
`85995b8dcc3304b360fd261425fa86b991845152`
**Subject** `data/ferrum_crossing.json` (the only scenario that exists), `spec/scenario_spec.md`'s
Director scope ruling, and `sync_stratdata.py`'s `SCENARIOS` list.
**Status** Investigation plus one spec edit upstream. No source file was touched in either repo,
no scenario was authored, nothing was staged and nothing was committed.
**What this does NOT do.** It does not close the scenario axis. Filing a request is not a second
map; the axis stays open in `state.md` until a scenario file exists and a corpus is graded on it.

---

## 1. What is open, and why this repo cannot close it

The phase-D "avoid one-corpus proof" precedent named three axes of content independence —
scenario, buildlist, first side. `185e88f` discharged two of them: first side and buildlist both
moved, derived rather than typed, and the second corpus graded 42/42 with zero pairing mismatches.
The third did not move, and `09-content-independence/blackboard.md` says so under "What this does
NOT prove".

The reason is structural, not an omission. `Data/` is vendored certified bytes and this repo may
not edit it (`CLAUDE.md` non-negotiable 1), and it holds exactly one scenario. Measured, by git
blob id rather than by a worktree diff — the crew checkout is CRLF and the vendored copy LF, so a
plain `diff` reports a difference on identical bytes:

```
ferrum_crossing.json  ue=64e3e3a8c31fde2cb825adce34cfc0b986b75edf  crew=64e3e3a8c31fde2cb825adce34cfc0b986b75edf  SAME
units.csv             ue=f3f49d84afc08cc9cc21cbddbcbd14737ec520e1  crew=f3f49d84afc08cc9cc21cbddbcbd14737ec520e1  SAME
terrain.csv           ue=d1c80b3fd25fb3c0ed02908589a712e2f6228d86  crew=d1c80b3fd25fb3c0ed02908589a712e2f6228d86  SAME
```

`Data/StratData.manifest.json` records `dataCommit` `c2f5860`, and `git diff --stat c2f5860 HEAD --
data/` in the crew repo is empty: the vendored copy is not merely equal to some past crew state,
it is equal to the CURRENT one. The scenario axis is blocked on content that does not exist
anywhere, not on a stale vendor.

## 2. The consumer side is already parameterised — the missing thing is the datum, not the code

`UStratMatchSubsystem::ScenarioFile` is an `EditAnywhere`/`BlueprintReadWrite` `FString` holding a
path relative to the project directory, resolved against `FPaths::ProjectDir()` in the `.cpp`, and
`FirstSide` sits beside it in the same idiom. Every automation clause that opens a scenario sets
that string (`Data/ferrum_crossing.json`) rather than compiling one in. So a second file dropped
into `Data/` is consumable with **no C++ change, no new bridge method, and no Blueprint work** —
one clause configuring a different path, in the shape `T-INT-05` already uses.

**One thing that looks like a second scenario and is not.** `StratSelectionFacadeParity.cpp` arm 6
writes `ferrum_crossing_variant.json` under `Saved/StratTests/`. It is the shipped file with the
`scenarioHash` line dropped and `scenarioId` relabelled — the same board, hex for hex. It exists to
prove a seeded-from-a-different-scenario refusal, and it moves the content axis by nothing. Named
here so nobody later reads it as the axis already having moved.

## 3. Why this is a Director call and not a task

`spec/scenario_spec.md` opens with a scope ruling that forbids exactly the work that would close
the axis:

> **This build validates the shipped map only.** *Longwater March* (§2.13.5) and *The Causeway*
> (§2.13.6) are **not** authored as scenario files, not even as test fixtures.

That ruling was made for row 7's own reasons — it costs T-SCN-08 two fixtures, T-SCN-09 its whole
asserting branch, T-SCN-11 one fixture, and it holds row 7's ledger row at `*pending*`. Nothing in
it anticipated a downstream consumer needing a second board for a reason unrelated to validating
row 7. The request is therefore not "please do the stretch maps"; it is: **does the
content-independence need license authoring one of them now, ahead of §2.13.7's week-4 condition,
and if so which.**

## 4. What is transcribable today, measured rather than assumed

`scenario_spec.md` says **"Author nothing. If a value you need is not in the document, stop and
file a change request rather than choosing one."** That test is nearly passed for *Longwater
March*, and the residue is one field.

Checked against §2.13.5's own ASCII grid, its key-coordinate list and its terrain census, by
script rather than by eye:

- The grid is **8 rows × 13 columns = 104 hexes**, matching the stated dimensions.
- Its glyph census is `Plains 86 · Woods 4 · Mountains 4 · Towns 4 · Factories 6`, **identical** to
  the distribution the section states in prose, with Water 0 and Bridge 0.
- Every named coordinate lands on the glyph it is named as: home factories (1,3)/(11,4), neutral
  factories (4,1)(8,1)(4,6)(8,6), towns (6,0)(3,3)(9,4)(6,7), the Woods knot
  (5,3)(6,3)(6,4)(7,4), corner Mountains (0,0)(12,0)(0,7)(12,7).
- The grid is **ρ-symmetric hex for hex** under ρ(c,r) = (12−c, 7−r) — not just at the nine pairs
  the section lists, but at all 104 hexes.
- All ten starting positions are on Plains, all distinct, both home factory hexes left free, and
  each East hex is the exact ρ-image of its West counterpart.
- Both `guidedOpening.infantry` hexes are Plains and both `objective` hexes are Factories, and the
  pair is a ρ-image of the other seat's.
- **No new terrain kind.** Longwater names Plains, Woods, Mountains, Town and Factory; all five are
  rows in the vendored `terrain.csv` and all five already appear on *Ferrum Crossing*. It drops
  Water and Bridge. The terrain table does not move, so `T-DATA-05` and the DataTable import are
  untouched.
- `startingFame` is derivable: §2.7 sets 200 per side at Normal, and Stub 7 states 200/200 as the
  baseline.

**The one field §2.13.5 does not state is `turnCap`.** It gives "Estimated match length 16–20
turns, frequently reaching the cap" — an estimate of play, not a cap value, and §2.8 is explicit
that the cap is per-scenario data (20 is *Ferrum Crossing*'s, §2.13.2). Under this spec's own
"author nothing" rule that number is the Director's to state, and it is the single blocking value.

## 5. The costs, stated rather than folded in

A second scenario is not a data drop. Four consequences, in descending order of how much work they
are:

1. **T-SCN-09's asserting branch has never run.** The scope ruling removed it because the shipped
   map declares `none`. *Longwater March* declares `rot180` on an even row count, so authoring it
   turns that branch on for the first time — new implementation surface in row 7, not a fixture
   addition. (§4 above establishes the map would actually satisfy it, which is a reason to expect
   the branch to pass, not a reason to skip writing it.)
2. **Row 7's ledger posture becomes a live question again.** T-SCN-08 recovers fixture (b) and
   T-SCN-11's dropped fixture is *The Causeway*'s, not this map's; the spec's "the row records a
   partial pass and stays `*pending*`" reasoning was written against a scope that this would move.
   Whoever lands the map has to restate that posture rather than inherit it.
3. **Vendoring is a declared list, not a glob.** `sync_stratdata.py` holds
   `SCENARIOS = ["ferrum_crossing.json"]`, deliberately declared "so the manifest's scope is a
   decision". A second scenario needs that line edited upstream, a re-run producing a new
   `Data/StratData.manifest.json` entry, and `GATE-DATA-VENDOR` hash-checking it like the rest.
4. **`scenarioHash` is produced, never typed.** It is a canonical serialisation hash from
   `strat::scenarioHash`; the file must be emitted or validated upstream, where that function
   lives.

## 6. Ordering — why *Longwater March* and not *The Causeway*

§2.13.7 is the one place that states the stretch condition, and it is explicit: *The Causeway* is
attempted only after *Longwater March* lands. The request follows that order rather than picking
the map that happens to suit the downstream corpus. It also names the rest of the condition
honestly — neither map may pull work forward of week 4 or block core, and if week 4 goes to
balance the set stays on paper. **Whether that condition is met is not a fact this repo holds**;
the ask is put in front of the Director with the condition quoted, not argued around.

## 7. What I did NOT verify

- **No loader ran.** Nothing here was parsed by `strat::loadScenario` or `validateScenario`; §4 is
  a consistency check of GDD prose against its own grid, not a validation pass.
- **No path costs.** §2.13.5's "4 MP each, 2 MP of slack against the 6 MP ceiling" for the guided
  opening is taken as written. Pricing it needs a Stub-3 path, and T-SCN-06/08/11 are the tests
  that would do it properly.
- **Ownership was not transcribed.** *Ferrum Crossing* encodes owners as `0`/`1`/`-1` over eight
  hexes; §2.13.5 names home versus neutral factories and four towns, and reading that into an
  `ownership` list is a transcription judgement the authoring pass makes, not one made here.
- **No claim about the AI's behaviour on this map.** The buildlist finding (per-type population
  cap, filed at crew `85995b8`) is orthogonal and is not re-argued here.

## 8. The change request, as filed

**FILED UPSTREAM 2026-08-19** at crew `3b5ef9d`, in a new "Change requests for the Director"
section in `spec/scenario_spec.md` placed before Acceptance, matching where `ai_spec.md` keeps its
own. Spec-only: no scenario was authored, no source file was touched, `sync_stratdata.py` is
unchanged and no acceptance ID moved. Its substance:

> **A second authored scenario, for a reason outside row 7.** The scope ruling above forbids
> authoring *Longwater March* and *The Causeway*, decided on row 7's own costs. A downstream
> consumer now needs one for an unrelated reason: the UE project's content-independence precedent
> asks that an invariant not rest on a single corpus, and two of its three axes (first side,
> buildlist) are discharged while the SCENARIO axis cannot move — `data/ferrum_crossing.json` is
> the only scenario that exists. The consumer needs no code: `UStratMatchSubsystem::ScenarioFile`
> is already a configured path. **The ask is a ruling, not a task:** does this license authoring
> *Longwater March* (§2.13.5, first per §2.13.7's ordering) ahead of the week-4 condition. If it
> does, one value is missing from the document and is the Director's to state — §2.13.5 gives an
> estimated match length but no `turnCap`, and §2.8 makes the cap per-scenario data. Everything
> else transcribes: the section's grid is 13 × 8 = 104, its census matches its prose, every named
> coordinate lands correctly, the grid is ρ-symmetric at all 104 hexes under ρ(c,r) = (12−c, 7−r),
> and it names no terrain kind absent from `data/terrain.csv`. Three consequences go with a yes,
> and none is free: T-SCN-09's asserting branch, dropped by the scope ruling, would run for the
> first time; row 7's `*pending*` ledger posture would need restating rather than inheriting; and
> `sync_stratdata.py`'s `SCENARIOS` list is declared rather than globbed, so vendoring is an
> upstream edit plus a manifest entry. One drafting trap for whoever transcribes: the ASCII grid's
> `w` is **Woods**, not Water — this map has no Water at all, and *Ferrum Crossing*'s grid trains
> the opposite reading.
