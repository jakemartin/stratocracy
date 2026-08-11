"""Utility scoring -- step 03 of the Class 7 loop, "score and prioritise".

`priority_score()` implements the four axes the Class 7 deck names, and nothing else:

    dependency order   what must exist first
    blockers           what is preventing other work
    project priority   what the GDD marks as critical
    current state      what is partially built vs untouched

Every weight is a module constant so the ranking can be audited rather than believed,
and every candidate carries back the sentences that produced its number.

TWO THINGS THIS DELIBERATELY DOES NOT DO.

It does not fold readiness into the score as a multiplier. A blocked candidate keeps
its full value and is marked NOT ACTIONABLE instead, so the blackboard shows a
high-value item being passed over BECAUSE of its blocker rather than showing it sink
down the table for reasons a reader then has to reconstruct. That is the deck's own
dungeon-crawler case: the shop UI is not low-value, it is downstream of the inventory
system.

It does not score `unruled` candidates at all. Where the GDD states in as many words
that a shape is undecided, a number here would be a vote in a decision the document
reserves for the Director. They are listed, scored `None`, and excluded.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from candidates import Candidate
from gdd import GddFacts

# ---------------------------------------------------------------------------
# Weights. Ordered by the deck's own emphasis: dependencies matter most, because a
# thing built out of order may have to be rebuilt, and rebuilt work is the only cost
# here that is paid twice.
# ---------------------------------------------------------------------------

W_UNBLOCKS = 30.0        # per downstream candidate this one currently blocks
W_OPEN_ID = 20.0         # per open acceptance ID it would let close
W_DAGGER = 10.0          # extra, per open ID the GDD marks as needing the editor pass
W_CRITICAL_PATH = 8.0    # per §4.11 row that depends on this candidate's row
W_PARTIAL = 6.0          # already partly built -- cheaper to finish than to start
W_IN_REPO = 5.0          # lands in the repo the grader clones

PENALTY_BLOCKED = 0.0    # see the module docstring: value is kept, actionability is not


@dataclass
class Score:
    candidate: Candidate
    total: float | None
    actionable: bool
    reasons: list[str] = field(default_factory=list)
    components: dict[str, float] = field(default_factory=dict)

    @property
    def label(self) -> str:
        if self.total is None:
            return "excluded"
        return f"{self.total:.0f}"


def _critical_path_weight(candidate: Candidate, gdd_facts: GddFacts) -> tuple[float, str]:
    """How much of §4.11 stands behind this candidate's ledger row.

    Read off the build-order table's own `Depends on` cells: a row that many rows
    depend on is one the document already treats as critical path. Nothing is invented
    here -- if the candidate closes no ID, it belongs to no row and scores zero on
    this axis, which is correct rather than a gap.
    """
    rows = {
        gdd_facts.row_for_id(i).number
        for i in candidate.closes
        if gdd_facts.row_for_id(i) is not None
    }
    if not rows:
        return 0.0, "closes no acceptance ID, so it sits on no §4.11 row"

    dependents = sum(
        1 for row in gdd_facts.rows if set(row.depends_on) & rows
    )
    names = ", ".join(f"row {n}" for n in sorted(rows))
    return (
        dependents * W_CRITICAL_PATH,
        f"{names} in §4.11; {dependents} row(s) depend on it",
    )


def priority_score(
    candidate: Candidate,
    peers: list[Candidate],
    gdd_facts: GddFacts,
) -> Score:
    """The utility score for one candidate, with its reasoning attached."""

    if candidate.unruled:
        return Score(
            candidate=candidate,
            total=None,
            actionable=False,
            reasons=[
                f"EXCLUDED, not scored: {candidate.unruled_reason}.",
                "The GDD reserves this shape; a score here would be a vote in it.",
            ],
        )

    reasons: list[str] = []
    parts: dict[str, float] = {}

    # -- axis 1: blockers -- what this is preventing --------------------------
    blocking = [
        p.key for p in peers
        if p.key != candidate.key and candidate.probe in p.requires and not candidate.exists
    ]
    parts["unblocks"] = len(blocking) * W_UNBLOCKS
    if blocking:
        reasons.append(
            f"BLOCKS {len(blocking)} downstream candidate(s): {', '.join(blocking)}."
        )
    else:
        reasons.append("blocks nothing else currently in the set.")

    # -- axis 2: project priority -- what the GDD is waiting on ---------------
    parts["open_ids"] = len(candidate.open_ids) * W_OPEN_ID
    daggered = [
        i for i in candidate.open_ids
        if any(i in row.editor_pass_ids for row in gdd_facts.rows)
    ]
    parts["dagger"] = len(daggered) * W_DAGGER
    if candidate.open_ids:
        reasons.append(
            f"would let {len(candidate.open_ids)} open acceptance ID(s) close: "
            f"{', '.join(candidate.open_ids)}."
        )
        if daggered:
            plural = len(daggered) > 1
            reasons.append(
                f"{', '.join(daggered)} {'carry' if plural else 'carries'} §4.11's † -- "
                f"{'they need' if plural else 'it needs'} the editor pass, so "
                f"{'they are' if plural else 'it is'} waiting on a buildable artefact "
                "rather than on a ruling."
            )
    else:
        reasons.append(
            "closes no acceptance ID directly -- its value is what it unblocks."
        )

    cp, cp_reason = _critical_path_weight(candidate, gdd_facts)
    parts["critical_path"] = cp
    reasons.append(cp_reason + ".")

    # -- axis 3: current state ------------------------------------------------
    parts["partial"] = W_PARTIAL if candidate.exists else 0.0
    reasons.append(
        "partially built already -- finishing is cheaper than starting."
        if candidate.exists else "untouched in the tree."
    )

    in_repo = not candidate.artifact.startswith("stratocracy-crew")
    parts["in_repo"] = W_IN_REPO if in_repo else 0.0
    if not in_repo:
        reasons.append(
            "lands OUTSIDE the repo the grader clones, in vendored certified bytes."
        )

    # -- axis 4: dependency order --------------------------------------------
    actionable = not candidate.blocked_by
    if actionable:
        reasons.append("READY: every prerequisite is present in the tree.")
    else:
        reasons.append(
            f"NOT ACTIONABLE: waiting on {', '.join(candidate.blocked_by)}."
        )

    return Score(
        candidate=candidate,
        total=sum(parts.values()) + PENALTY_BLOCKED,
        actionable=actionable,
        reasons=reasons,
        components=parts,
    )


def rank(
    cands: list[Candidate],
    gdd_facts: GddFacts,
) -> list[Score]:
    """Every candidate scored, highest first. Unscored (`unruled`) entries sort last
    so they stay visible in the record without competing."""
    scores = [priority_score(c, cands, gdd_facts) for c in cands]
    return sorted(
        scores,
        key=lambda s: (s.total is not None, s.total or 0.0),
        reverse=True,
    )


def select(scores: list[Score]) -> Score | None:
    """The highest-scoring ACTIONABLE candidate.

    Dependency order is enforced here rather than in the arithmetic: a blocked
    candidate can outscore the thing blocking it and still not be chosen, which is
    exactly the case the deck describes and exactly what the blackboard should show.
    """
    for s in scores:
        if s.total is not None and s.actionable:
            return s
    return None
