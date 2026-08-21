# Upstream request — a scenario whose guided-opening Infantry deploys adjacent to an enemy

**Filed against** `E:\MultiAgent\stratocracy-crew`, crew `main` HEAD `4d36a16` — DRAFTED HERE, NOT
FILED. See §4, `FILED IS NOT LANDED`.

**Subject** `data/*.json` (there is exactly one scenario, `ferrum_crossing.json`), and by
extension any future authored scenario file's `guidedOpening.infantry` placement relative to the
opposing side's starting units.

**Status** Drafted in this repository only. No file in either repo was touched, nothing was
staged, nothing was committed, upstream or here.

---

## 0. First: is this already filed? Checked, not assumed — NO.

`Tools/architect/evidence/upstream-second-scenario.md`, **FILED UPSTREAM 2026-08-19** at crew
`3b5ef9d`, is the only existing filing that touches `guidedOpening`. Its subject is the
content-independence **SCENARIO axis** — it asks whether a second scenario file (*Longwater
March*, GDD §2.13.5) may be authored at all. The only thing it says about the guided opening is a
transcription check: "Both `guidedOpening.infantry` hexes are Plains and both `objective` hexes
are Factories" (§4, matching §2.13.5's own table). It never asks for **contact at deployment** —
that property is absent from every numbered section of that filing, including §8's verbatim
change-request text.

**Verified against §2.13.5 itself** (`Stratocracy_Prototype_GDD.md:1229-1284`, read directly, not
recalled) rather than taken on the earlier filing's word: *Longwater March* would **not** satisfy
the adjacency property even if authored as specified. Its starting positions:

| Unit | West | East |
|---|---|---|
| Flag Tank | (0,3) | (12,4) |
| Infantry ×2 | **(1,2)**, (1,4) | **(11,5)**, (11,3) |
| Artillery | (0,2) | (12,5) |
| Recon | (0,4) | (12,3) |

`guidedOpening.infantry` is (1,2) West / (11,5) East on a 13-column (0–12) board. The nearest West
unit to any East unit is column 1 vs column 11 — 10 columns apart on a board whose home factories
are themselves stated as "10 hexes apart" (§2.13.5, "unchanged from the 13×9 draft"). No hex pair
between the two sides' starting units is adjacent by any measure. **A second scenario and a
scenario-with-deployment-contact are different requests, and *Longwater March* as specified
satisfies the first and not the second.** This is therefore a new, separate request, not an
amendment recording that the existing one already covers it.

*The Causeway* (GDD §2.13.6, `Stratocracy_Prototype_GDD.md:1308-1368`) was also checked as a
candidate, since it is the map named next in the §2.13.7 stretch order. Its
`guidedOpening.infantry` is (1,2) West / (7,5) East on a 9-column board with homes "6 hexes
apart" — also not adjacent. Neither existing stretch-scenario draft has the property this request
needs; it cannot be satisfied by amending the existing filing to point at either one.

---

## 1. What is requested

A scenario file (new or a revision of a not-yet-authored one) whose authored
`guidedOpening.infantry` hex, for at least one side, is **adjacent to at least one starting unit
of the opposing side**, so that the marked Infantry has a legal attack target at its own
deployment hex, before any unit has moved. Stated for a Director to rule on directly:

> Does an authored scenario's `guidedOpening.infantry` hex ever start adjacent to an enemy unit?
> If not for the shipped map or either stretch draft, may one be drafted (or an existing stretch
> draft's starting layout be revised) specifically to carry that property, ahead of and separate
> from the §2.13.7 stretch-scenario schedule?

## 2. Why this repo cannot do it

`Data/` is vendored certified bytes, hash-gated, and `CLAUDE.md` non-negotiable 1 forbids editing
it — the same constraint the prior filing cites. Verified fresh rather than inherited:

- `Data/StratData.manifest.json`: `"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"`.
- `Source/StratRules/StratRules.manifest.json`: `"rulesCommit": "cb8e12b3a897c7329497ced4d1c6207630f37101"`.
- Compared by **git blob hash**, not a worktree diff (the crew checkout is CRLF, the vendored copy
  LF, so a plain diff would cry "moved ahead" on identical bytes):

  ```
  $ git -C E:/MultiAgent/Stratocracy hash-object Data/ferrum_crossing.json
  64e3e3a8c31fde2cb825adce34cfc0b986b75edf
  $ git -C E:/MultiAgent/stratocracy-crew rev-parse c2f5860:data/ferrum_crossing.json
  64e3e3a8c31fde2cb825adce34cfc0b986b75edf
  ```

  Same blob both sides. The vendored copy is exactly the shipped board, and the shipped board's
  own `guidedOpening.infantry` is not adjacent to an enemy unit either — Ferrum Crossing's opening
  Infantry hex sits well inside its own half of an 11×9 board, symmetric with the enemy's,
  exactly the geometry that produced the zero-attack-target result cited in §3 below. There is
  exactly one scenario file in existence and it does not have the property; a second scenario
  with the property does not exist anywhere, in either repo.

## 3. What it unblocks, and what it does not — stated exactly, not folded together

**It unblocks the attack arm's reachability.** With no starting hex adjacent to an enemy unit,
`FStratBridge::AttackTargetHexes` for the marked Infantry at its deployment hex has nothing to
return — there is no board configuration under the current scenario in which the arm could be
exercised at that beat. A scenario carrying the adjacency property is a *necessary* condition for
reaching that arm at all.

**For the overlay-zero attribution, it makes the experiment possible but does NOT by itself settle
it.** The open question is whether an empty target overlay at deployment is caused by the **input
gate** (beat-1a's Attack transition never lighting, per the Q27 ruling already filed — see
`upstream-onboarding-input-gating-acceptance-id.md`) or by the **board** (no legal target exists
regardless of gating). A scenario with a genuinely adjacent enemy unit only *removes the board as
an explanation* — the experiment still has to be run and both branches observed: with the gate as
currently implemented (expect: still zero, if the gate is the cause) and, separately, with the
gate bypassed at the rules-module level via `FStratBridge::AttackTargetHexes` called directly
against the new scenario's seeded state (expect: non-zero, proving a target exists). Filing this
request supplies the board; it does not supply the second half of that comparison, which is
implementation and test work outside this lane.

## 4. `FILED IS NOT LANDED.`

This document is **drafted in this repository only.** It has not been filed against
`stratocracy-crew`. No file in `E:\MultiAgent\stratocracy-crew` has been touched, no branch
created, nothing staged, nothing committed. The crew commit named at the top (`4d36a16`) is the
HEAD this draft is written against, not a commit it was filed at — there is no crew commit to
cite as "filed at" because filing has not happened. If and when this is filed, record the crew
commit it lands at here, in the shape `upstream-second-scenario.md` §8 and
`upstream-onboarding-input-gating-acceptance-id.md` use, and update `state/decisions.md`
accordingly. Until then this is a draft, not a request in flight.

## 5. What already exists and must not be mistaken for this

`Source/StratPlay/Tests/StratSelectionFacadeParity.cpp` arm 6 writes
`ferrum_crossing_variant.json` under `Saved/StratTests/` — the shipped board, hex for hex, with
`scenarioHash` dropped and `scenarioId` relabelled. It exists to prove a seeded-from-a-different-
scenario refusal and moves **no content axis at all**, adjacency or otherwise: it is the same
board, so its `guidedOpening.infantry` hex is exactly as non-adjacent as *Ferrum Crossing*'s.
Named here, as `upstream-second-scenario.md` §2 already named it for the SCENARIO axis, so nobody
reads it as this request being satisfied.

Also not this: the existing `T-UI-02.TheMarkedInfantryHasNoAttackTargetAtDeployment` clause in
`Source/StratPlay/Tests/StratGuidanceInputGates.cpp`. That test *proves the absence this request
exists to give a scenario to work around* — it asks `FStratBridge::AttackTargetHexes` for the
marked Infantry directly and gets `bOk` true (the rules module answered, not refused) with **zero**
hexes, and separately walks the whole 10-unit deployment finding **0 of 10** answered units with
a legal attack anywhere. Its own control, `FindABoardPositionWithALegalAttack`, plays a private
AI-vs-AI match until the same method returns non-empty for *some* unit — so the zero is proven to
be a fact about *this board*, not a limitation of the method or a bug in the test. That control is
exactly why this filing can state the board as the cause with confidence, rather than guessing.
