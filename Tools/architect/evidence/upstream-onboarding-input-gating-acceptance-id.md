# Upstream request — an acceptance ID for §2.11.6-B's input-gating constraint (Q27)

**Filed against** `E:\MultiAgent\stratocracy-crew`, HEAD `4d36a160ff18f203a19918e5c30274c4a160412d`.
**Subject** `spec/ui_spec.md`'s `T-UI-*` acceptance-ID block (GDD §4.11) and the prose at GDD
§2.11.6 beat 1a (Q27).
**Status** Investigation only. No source file was touched in either repo, no acceptance ID was
minted here, nothing was staged and nothing was committed. **FILED, not LANDED** — the distinction
this evidence tree already keeps: a filing is a request sitting in a spec document; it lands when
a commit exists carrying it (`upstream-second-scenario.md` §8 landed at crew `3b5ef9d`; the
`uiBuildOptions` ruling landed at crew `4d36a16` per `state/decisions.md`'s 2026-08-20 entries).
Nothing below is claimed to have landed.

---

## 1. What is currently asserted, and where

The three player-input gates §2.11.6-B beat 1a specifies —

- **End Turn** is inert until the turn's marked Infantry (`guidedOpening.infantry`) has moved,
  hover text `Move the marked Infantry first.`
- **Wait** (`Space`) is inert on the same footing, for the same reason: neither is a route from
  `SELECTED` to `DONE` that does not pass through `MOVED` for that one unit.
- **Attack** is closed because that unit's attack targets are not lit, so the `SELECTED → attack`
  transition (§2.11.1) never opens.

— are today asserted in this repository under **`T-UI-02`**, on the precedent this tree set at
`870c611` (`T-UI-02.ControllerTicksSoInputDispatches`, `T-UI-02.BoardHexRoundTrip`; see
`Tools/architect/state/decisions.md`'s 2026-08-21 entry for the full reassignment reasoning and
the file/line evidence for the companion `T-SCN-02` → `T-SCN-07` move).

`T-UI-02`'s written invariant, verified verbatim against `Stratocracy_Prototype_GDD.md:2505`
(GDD §4.11, the Stub-8 UI Queries/Invariants block):

> `T-UI-02` the reachable-hex highlight displays exactly the T-MOVE-01 set — the UI queries the
> module and never recomputes movement (§2.5)

That sentence is about the reachable-hex highlight. It says nothing about End Turn, Wait, or
Attack being inert, and nothing about beat 1a at all.

## 2. Why `T-UI-02` is a stretch, stated plainly

1. **Different subject.** `T-UI-02` pins a *query result* (the highlighted hex set equals
   `T-MOVE-01`'s reachable set) against a module call that already exists
   (`uiReachable` → `Move.h::reachable`). Q27's gates pin the *availability of three player
   actions* — a state-machine property of the guided-opening layer, not a highlight comparison.
2. **No shared mechanism.** `T-UI-02` is satisfied by comparing two sets. The Q27 gates are
   satisfied by the selection machine's own transition graph having no SELECTED→DONE edge for the
   marked unit that does not pass through MOVED (see GDD §2.11.6, the beat-1a row, quoted in
   `state/decisions.md`). Asserting Q27 "under" `T-UI-02` therefore asserts two independent
   properties under one ID; a future edit that breaks the highlight-set invariant and one that
   breaks End Turn's gating would both, wrongly, look like the same finding by ID alone.
3. **No ID exists that fits better.** GDD §4.11's `T-UI-*` block runs `T-UI-01` (forecast =
   resolution), `T-UI-02` (reachable-hex highlight, above), `T-UI-03` (scoreboard binding),
   `T-UI-04` (buildlist / flag exclusion), `T-UI-05` (snapshot fidelity). None names beat-gating,
   input inertness, or the guided-opening state machine.

## 3. The family was enumerated, not recalled, and there is no onboarding family

Checked by script rather than by memory, over the whole GDD:

```
grep -oE "T-(AI|CAP|COMBAT|DATA|FAME|HEX|INT|MOVE|REPAIR|SAVE|SCN|TURN|UI)-01" \
  Stratocracy_Prototype_GDD.md
```

returns exactly the thirteen families `AI, CAP, COMBAT, DATA, FAME, HEX, INT, MOVE, REPAIR, SAVE,
SCN, TURN, UI` — no `T-ONBOARD-*`, `T-GUIDE-*`, or similar family exists anywhere in the document.
§2.11.6 itself (the whole onboarding section, all four beats) never mints an ID of its own; it
cites into other families' IDs instead — `T-MOVE-01`'s accounting for the guided lane's movement
cost, `T-SCN-01`'s non-producible-flag clause reused at the UI layer, and now `T-SCN-07`'s
opening-capture naming for the guided-opening hex pair itself. Q27 is the one exception: GDD §4.7
names it in prose at the beat-1a directive row —

> adopted under **Q27** (§4.7), ruled — it was registered rather than assumed because it gates an
> input

— and stops there. No `T-` identifier accompanies that sentence anywhere in the document. This is
a genuine gap, not a citation this repository failed to find: the constraint is ruled (Q27 exists,
is registered, and is not in dispute) but never given an acceptance ID to be asserted under.

## 4. Why this is upstream's call and not authored here

`spec/ui_spec.md` is where the `T-UI-*` block lives and where `85995b8` (chooseBuild) and
`3b5ef9d` (the scenario axis) both put their own "Change requests for the Director" section
before Acceptance. A test in this repository may not mint an acceptance ID — that is the Director's
naming authority, exercised in the spec documents upstream, exactly as `T-UI-04`'s "no third
query without a ruling" refusal (quoted in `state/decisions.md`'s 2026-08-20 entry) already
establishes for a different gap in the same file. This request follows that shape: state what is
missing and why the nearest existing ID is a stretch, and let the ruling assign (or decline to
assign) the ID.

**The change request, as it would be filed** (drafted here; not yet written into
`spec/ui_spec.md`, matching the "Investigation ... No source file was touched" status above):

> **An acceptance ID for §2.11.6-B's input-gating constraint (Q27).** Beat 1a of the guided
> opening closes three player-input routes — End Turn, Wait, Attack — for the one marked Infantry
> until it has moved, "registered rather than assumed because it gates an input" (§4.7, Q27). No
> `T-UI-*` or other acceptance ID names this constraint; the nearest existing ID, `T-UI-02`, is
> written for the reachable-hex highlight and is a stretch when asked to also cover input
> inertness — two properties under one ID. The `T-*` family list has no onboarding family
> (`AI, CAP, COMBAT, DATA, FAME, HEX, INT, MOVE, REPAIR, SAVE, SCN, TURN, UI`, enumerated by
> `grep -oE` over the shipped GDD, not recalled). Requesting either a new ID (e.g. `T-UI-06`) or an
> explicit ruling that Q27 stays unassigned and downstream consumers cite it by its own name (`Q27`)
> rather than by acceptance ID.

## 5. What this does NOT do

- It does not mint `T-UI-06` or any other ID. That is the Director's call, exercised upstream.
- It does not change how this repository's tests cite the constraint today — they stay filed
  under `T-UI-02` as a recorded partial fit until an ID exists to move to.
- It is not the `uiBuildOptions` request. That item (GDD §2.11.5, Lane A, the production menu) is
  a header change to `cpp_reference/Ui.h` that has already landed as a spec entry at crew
  `4d36a16` and is still blocked on implementation + re-vendor. This filing is spec-only and
  narrower: an ID assignment, not a new function.

## 6. What I did not verify

- No claim about whether the Director would grant a new ID versus rule Q27 stays unassigned by
  design — both are legitimate outcomes and this filing states the question rather than presuming
  the answer.
- The 15-commit gap between this repo's vendored `rulesCommit cb8e12b` and crew HEAD `4d36a16`
  (`git log --oneline cb8e12b..4d36a16 | wc -l` — measured at 15, not the 14 assumed going in) was
  not walked commit by commit for unrelated drift; only `spec/ui_spec.md`'s `T-UI-*` block and
  GDD §2.11.6/§4.7's own text were read for this filing.
