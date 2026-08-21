# Upstream request — an acceptance ID for §2.11.6-B's input-gating constraint (Q27)

**Filed against** `E:\MultiAgent\stratocracy-crew`, HEAD `4d36a160ff18f203a19918e5c30274c4a160412d`.
**Subject** `spec/ui_spec.md`'s `T-UI-*` acceptance-ID block (GDD §4.11) and the prose at GDD
§2.11.6 beat 1a (Q27).
**Status** **SUPERSEDED 2026-08-21 — this request WAS FILED, at crew `3510bc2`.** See §7 and §8
below. The paragraph immediately below described this document's state before filing and is kept,
quoted rather than deleted, because a stale "not filed" run in reverse is its own hazard — a
reader would re-file a request already in flight against a repo where it now sits committed.

> Investigation only. No source file was touched in either repo, no acceptance ID was minted
> here, nothing was staged and nothing was committed. **FILED, not LANDED** — the distinction this
> evidence tree already keeps: a filing is a request sitting in a spec document; it lands when a
> commit exists carrying it (`upstream-second-scenario.md` §8 landed at crew `3b5ef9d`; the
> `uiBuildOptions` ruling landed at crew `4d36a16`; the adjacency request landed at crew `a8c8cdb`
> — all per `state/decisions.md`'s corresponding entries). Nothing below was, at the time this was
> written, claimed to have landed.

**What "filed" does NOT mean, stated so a reader does not read it as "granted".** No Director has
ruled on the naming question in §1. No acceptance ID has been minted — `T-UI-06` does not exist
anywhere in `cpp_reference/` or `spec/ui_spec.md`. The downstream clauses named in §1 stay filed
under `T-UI-02` as a recorded partial fit, exactly as before filing. Filing supplied a place for a
ruling to land, not the ruling itself.

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

**SUPERSEDED 2026-08-21 — the draft below was written before filing and differs from the wording
that actually landed.** Kept, quoted rather than deleted, per this file's own convention. §7 below
quotes the request as filed, taken from the landed diff rather than retyped from this draft.

> **The change request, as it would be filed** (drafted here; not yet written into
> `spec/ui_spec.md`, matching the "Investigation ... No source file was touched" status above):
>
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

## 7. The change request, as filed

**FILED UPSTREAM 2026-08-21**, on the user's explicit instruction, at crew **`3510bc2`**
(`"A ruled constraint with no name gets asserted under an ID written for something else"`),
as item 2 of the existing "Change requests for the Director" section in `spec/ui_spec.md`,
immediately before `## Acceptance` — the same placement item 1 (the `uiBuildOptions` ruling,
landed `4d36a16`) already uses. **Distinct commits, kept distinct:** `4d36a16` is the crew HEAD
this request was *drafted* against (§ above, "Filed against ... HEAD `4d36a16`" in the header
still names the drafting point); `3510bc2` is the commit it was *filed at*, one commit ahead of
`a8c8cdb` (the adjacency filing) on crew `main`.

**Verified against the crew repo directly, not taken on a dispatch prompt's word:**
`git -C E:\MultiAgent\stratocracy-crew show --stat 3510bc2` shows exactly one file touched,
`spec/ui_spec.md`, **64 insertions, 4 deletions** — spec-only, `data/` and `cpp_reference/`
untouched, no manifest or hash moved, `sync_stratdata.py` unchanged, no acceptance ID minted.

**The 4 deletions are an intro rewrite, not a retraction.** The "Change requests for the Director"
section previously opened "There is one, and it is not a question" (true when item 1, the
`uiBuildOptions` ruling, was the only entry). Item 2 falsifies that count, so the intro is
rewritten in the same commit rather than left stale: it now reads "There are two, and they are
different in kind" and states which is a ruling awaiting implementation (item 1) and which is a
naming question awaiting a ruling (item 2). Recorded here because a stale count is exactly the
failure this record keeps paying for.

**One substantive correction the filing made to this document's own §1, and it must survive into
this record.** §1 above describes "the three player-input gates §2.11.6-B beat 1a specifies" in a
way that could read as though Q27 ruled all three. It does not. Verified directly against
`Stratocracy_Prototype_GDD.md` (grepped, not `cat`): Q27's own ruling text (the `| **Q27** |` row)
names **End Turn** only. The §2.11.6-B beat-1a row is what separately extends the constraint to
**Space** ("inert for it on the same footing as End Turn and for the same reason") and closes the
**Attack** transition (§2.11.1) to that unit. The filed request states this split explicitly,
rather than flattening it the way §1's own summary sentence does; a Director asked to name "the
Q27 constraint" needs to know which sentence ruled what.

**The request, as landed** (quoted from `git show 3510bc2 -- spec/ui_spec.md`, not the pre-filing
draft quoted above in §4):

> **An acceptance ID for §2.11.6-B's input-gating constraint (Q27). FILED 2026-08-21; not decided
> here.** This asks for a NAMING decision, not for work. Beat 1a of the guided opening closes
> three player-input routes for the one marked Infantry until it has moved, and **no acceptance ID
> names any of them.**
>
> **What is ruled, stated exactly, because the three gates do not all come from the same
> sentence.** Q27 (§4.7) is RULED and its ruling text names **End Turn**: inert during beat 1a
> until the marked Infantry has moved, hover `Move the marked Infantry first.`, scoped to the
> first match, dying with `Skip guidance`. The §2.11.6-B beat-1a row is what extends that to
> **Space**, "inert for it on the same footing as End Turn and for the same reason" — the two
> being the machine's only routes from `SELECTED` to `DONE` that do not pass through `MOVED` —
> and separately closes the **Attack** transition (§2.11.1) to that unit. The constraint is not in
> dispute; only its name is missing.
>
> **The nearest existing ID is a stretch, and saying why is the request.** Downstream currently
> asserts these gates under `T-UI-02`, whose written invariant is *"the reachable-hex highlight
> displays exactly the T-MOVE-01 set — the UI queries the module and never recomputes movement
> (§2.5)"*. That is a **query-result comparison**: two sets are equal. The Q27 gates are a
> **state-machine availability property** of the guided-opening layer. They share no mechanism,
> and filing them under one ID puts two independent properties behind one name — so a future edit
> breaking the highlight set and one breaking End Turn's gating would be indistinguishable by ID
> alone.
>
> **No better ID exists, enumerated rather than recalled.** `grep -oE` over the shipped GDD
> returns thirteen `T-*` families — AI, CAP, COMBAT, DATA, FAME, HEX, INT, MOVE, REPAIR, SAVE,
> SCN, TURN, UI — and no onboarding or guidance family at all. The `T-UI-*` block runs 01–05
> (forecast=resolution; reachable-hex highlight; scoreboard binding; buildlist/flag exclusion;
> snapshot fidelity) and none names beat-gating, input inertness, or the guided-opening state
> machine. §2.11.6 mints no ID of its own anywhere; it cites into other families instead. Q27 is
> the exception that gets prose and no identifier.
>
> **The ask, and both answers are legitimate:** either mint an ID (e.g. `T-UI-06`) for the beat-1a
> input-gating constraint, or rule explicitly that Q27 stays unassigned by design and downstream
> consumers cite it as `Q27` rather than by acceptance ID. This filing states the question and
> does not presume which. It follows the precedent this file already sets, where `T-UI-04` refused
> to invent a third query rather than pre-empt a ruling: **a downstream test may not mint an
> acceptance ID**, because naming is the Director's authority and is exercised here.
>
> **Spec-only, and narrower than it may look.** No ID is minted here, no source file is touched,
> `data/` and `cpp_reference/` are unchanged, and nothing in downstream's citation changes today —
> those clauses stay under `T-UI-02` as a recorded partial fit until an ID exists to move them to.
> This is **not** the `uiBuildOptions` item in request 1: that one is a header change awaiting
> implementation and re-vendor; this one is an ID assignment.
>
> Full investigation, including the family enumeration and the verbatim `T-UI-02` comparison:
> `Stratocracy/Tools/architect/evidence/upstream-onboarding-input-gating-acceptance-id.md`.

**What "filed" does not mean, restated at the point it matters most.** No Director has ruled
either way. `T-UI-06` is not minted — it appears only inside the quoted request text above as an
example, not as an assigned ID. Downstream's clauses stay exactly where §1 above says they are
filed today, under `T-UI-02`, as a recorded partial fit. Filing is not landing in the content
sense either: filing this request did not change what any test asserts.

## 8. What this does NOT do

- It does not mint `T-UI-06` or any other ID. That is the Director's call, exercised upstream —
  and, as of `3510bc2`, still unexercised. No Director has ruled either branch of §7's ask.
- It does not change how this repository's tests cite the constraint today — they stay filed
  under `T-UI-02` as a recorded partial fit until an ID exists to move to.
- It is not the `uiBuildOptions` request. That item (GDD §2.11.5, Lane A, the production menu) is
  a header change to `cpp_reference/Ui.h` that has already landed as a spec entry at crew
  `4d36a16` and is still blocked on implementation + re-vendor. This filing is spec-only and
  narrower: an ID assignment, not a new function.
- It is not the adjacency request either (crew `a8c8cdb`, item 1 of `spec/scenario_spec.md`'s own
  Change-requests section) — a different file, a different subject, a different question shape
  (board content vs. naming).

## 9. What I did not verify, and the commit gap re-derived fresh

- No claim about whether the Director would grant a new ID versus rule Q27 stays unassigned by
  design — both are legitimate outcomes and this filing states the question rather than presuming
  the answer.
- **Re-derived 2026-08-21, superseding the 15-commit figure below rather than editing it:**
  `git -C E:/MultiAgent/stratocracy-crew rev-list --count cb8e12b..HEAD` against crew HEAD
  `3510bc2` (`main`) returns **17**. The crew has moved twice since the figure below was taken —
  `a8c8cdb` (the adjacency filing) and `3510bc2` (this filing) — so 17 is a count of commits since
  the vendored `rulesCommit`, not a re-walk for unrelated drift; only `spec/ui_spec.md`'s
  `T-UI-*` block and GDD §2.11.6/§4.7's own text were read for either filing.
- Superseded rather than edited, per this file's convention:
  > The 15-commit gap between this repo's vendored `rulesCommit cb8e12b` and crew HEAD `4d36a16`
  > (`git log --oneline cb8e12b..4d36a16 | wc -l` — measured at 15, not the 14 assumed going in)
  > was not walked commit by commit for unrelated drift; only `spec/ui_spec.md`'s `T-UI-*` block
  > and GDD §2.11.6/§4.7's own text were read for this filing.
