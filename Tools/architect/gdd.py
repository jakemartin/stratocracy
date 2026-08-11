"""GDD perception -- step 01 of the Class 7 loop, "read your GDD".

The Stratocracy GDD is ~446KB of markdown on very long lines, so nothing here reads
it whole into a prompt. It is parsed for the two structures that actually carry the
build state, and only the slices a step needs are quoted onward:

  1. §4.11's BUILD ORDER TABLE -- a real dependency graph, already written down. Each
     row states what it depends on, whether it is headless, and which acceptance IDs
     close it. This is where the scorer's dependency edges come from; the agent does
     not invent an ordering the document already states.

  2. §4.7's STUB INVARIANT BLOCKS -- the `T-XXX-NN  <text>` entries that define what
     each acceptance ID actually asserts. This is what gets quoted into a generation
     prompt, so the model builds against the GDD's own words rather than a summary.

A note on the `†` mark, because the scorer leans on it: §4.11 daggers the acceptance
IDs whose gate needs the EDITOR PASS rather than a headless run. Those are precisely
the IDs that cannot close without real engine-side artefacts, which is why a dagger is
treated as evidence that an ID is waiting on something buildable rather than on a
ruling.
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class LedgerRow:
    """One row of §4.11's build-order table."""
    number: int
    system: str
    depends_on: list[int]
    headless: str
    acceptance_ids: list[str]
    editor_pass_ids: list[str]      # the `†`-marked subset
    raw_depends: str


@dataclass
class Invariant:
    """One `T-XXX-NN` entry from a §4.7 stub's `Invariants:` block."""
    id: str
    text: str


@dataclass
class GddFacts:
    path: Path
    size: int
    rows: list[LedgerRow] = field(default_factory=list)
    invariants: dict[str, Invariant] = field(default_factory=dict)

    def row_for_id(self, acceptance_id: str) -> LedgerRow | None:
        for row in self.rows:
            if acceptance_id in row.acceptance_ids:
                return row
        return None

    def all_ids(self) -> list[str]:
        out: list[str] = []
        for row in self.rows:
            out.extend(row.acceptance_ids)
        return out


# ---------------------------------------------------------------------------
# ID lists
# ---------------------------------------------------------------------------

_RANGE = re.compile(r"\b(T-[A-Z]+)-(\d+)\.\.(\d+)\b")
_SINGLE = re.compile(r"\b(T-[A-Z]+)-(\d+)\b")


def expand_ids(cell: str) -> list[str]:
    """Expands `T-UI-01..05` and picks up bare `T-HEX-07` singles.

    Ranges are expanded first and their spans blanked out, so the range's own
    endpoints are not then re-read as singles and duplicated.
    """
    found: list[str] = []
    remainder = cell
    for m in _RANGE.finditer(cell):
        prefix, lo, hi = m.group(1), int(m.group(2)), int(m.group(3))
        width = len(m.group(2))
        found.extend(f"{prefix}-{n:0{width}d}" for n in range(lo, hi + 1))
        remainder = remainder.replace(m.group(0), " " * len(m.group(0)))
    for m in _SINGLE.finditer(remainder):
        found.append(f"{m.group(1)}-{m.group(2)}")

    seen: set[str] = set()
    ordered: list[str] = []
    for i in found:
        if i not in seen:
            seen.add(i)
            ordered.append(i)
    return ordered


def _dagger_ids(cell: str) -> list[str]:
    """The `†`-marked IDs in an acceptance cell.

    The table writes these in a shorthand that drops the repeated prefix --
    `(**T-UI-03, 04 †**)` means T-UI-03 AND T-UI-04, and `(**T-SCN-08, 09, 11 †**)`
    means three. A parser that only matched full IDs would silently find one of each
    group and under-report exactly the IDs that most need reporting.
    """
    out: list[str] = []
    for group in re.findall(r"\*\*([^*]*†[^*]*)\*\*", cell):
        prefix: str | None = None
        for token in re.split(r"[,\s]+", group.replace("†", " ")):
            token = token.strip()
            if not token:
                continue
            m = re.fullmatch(r"(T-[A-Z]+)-(\d+)", token)
            if m:
                prefix = m.group(1)
                out.append(f"{m.group(1)}-{m.group(2)}")
            elif re.fullmatch(r"\d+", token) and prefix:
                out.append(f"{prefix}-{int(token):02d}")
    return out


# ---------------------------------------------------------------------------
# §4.11 build-order table
# ---------------------------------------------------------------------------

def parse_build_order(text: str) -> list[LedgerRow]:
    start = text.find("### 4.11 Build order")
    if start < 0:
        return []
    window = text[start:start + 60000]

    rows: list[LedgerRow] = []
    for line in window.splitlines():
        if not line.startswith("|"):
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) < 5 or not cells[0].isdigit():
            continue

        number = int(cells[0])
        depends_cell = cells[2]
        # Row 9 and 10's dependency cells are paragraphs, not lists -- they narrate
        # "run vs close" across several sentences. Taking every bare integer out of
        # such a cell would pull in commit-adjacent numbers and week references, so
        # only the leading list form (`5, 7` / `1, 2`) is read as edges, and prose
        # cells contribute the row numbers named in an explicit "rows N" phrase.
        head = re.match(r"^[\s—–-]*((?:\d+\s*(?:,|and|–|-|\s)\s*)*\d+)\b", depends_cell)
        deps: list[int] = []
        if head:
            deps = [int(n) for n in re.findall(r"\d+", head.group(1))]
        else:
            for phrase in re.findall(r"rows?\s+([\d,\s–\-and]+)", depends_cell):
                # `rows 1–3` is a RANGE and means three rows. Reading it as the two
                # integers it literally contains would drop row 2 from row 9's
                # dependency set, which is a wrong graph rather than a coarse one.
                for rng in re.finditer(r"(\d+)\s*[–-]\s*(\d+)", phrase):
                    deps.extend(range(int(rng.group(1)), int(rng.group(2)) + 1))
                deps.extend(int(n) for n in re.findall(r"\d+", re.sub(
                    r"\d+\s*[–-]\s*\d+", " ", phrase)))
        deps = sorted({d for d in deps if 1 <= d <= 10})

        rows.append(LedgerRow(
            number=number,
            system=cells[1],
            depends_on=deps,
            headless=cells[3],
            acceptance_ids=expand_ids(cells[4]),
            editor_pass_ids=sorted(set(_dagger_ids(cells[4]))),
            raw_depends=depends_cell,
        ))
    return rows


# ---------------------------------------------------------------------------
# §4.7 stub invariants
# ---------------------------------------------------------------------------

_INV_START = re.compile(r"^ {2}(T-[A-Z]+-\d+)\s{2,}(\S.*)$")


def parse_invariants(text: str) -> dict[str, Invariant]:
    """Reads the `Invariants:` blocks of the §4.7 stubs.

    They are laid out as a two-space-indented ID, its text starting on the same line,
    and continuation lines indented further. Continuations are joined so a quoted
    invariant reaches a prompt as one sentence rather than as ragged column text.
    """
    out: dict[str, Invariant] = {}
    current: str | None = None
    buf: list[str] = []

    def close() -> None:
        nonlocal current, buf
        if current:
            joined = " ".join(part.strip() for part in buf if part.strip())
            out[current] = Invariant(id=current, text=re.sub(r"\s+", " ", joined).strip())
        current, buf = None, []

    for line in text.splitlines():
        m = _INV_START.match(line)
        if m:
            close()
            current, buf = m.group(1), [m.group(2)]
            continue
        if current is not None:
            if line.startswith(" " * 8) and line.strip():
                buf.append(line)
            elif not line.strip():
                continue
            else:
                close()
    close()
    return out


# ---------------------------------------------------------------------------
# Slicing, for prompt context
# ---------------------------------------------------------------------------

def find_section(text: str, needle: str, span: int = 6000) -> str:
    """A slice of the document around `needle`.

    Exists because the GDD's lines run to thousands of characters -- reading "the
    section" by line is not available here, so a step that wants §2.11.4's scoreboard
    rules takes a character window and says how wide it was.
    """
    i = text.find(needle)
    if i < 0:
        return ""
    return text[i:i + span]


def load(path: Path) -> GddFacts:
    text = path.read_text(encoding="utf-8")
    return GddFacts(
        path=path,
        size=len(text),
        rows=parse_build_order(text),
        invariants=parse_invariants(text),
    )
