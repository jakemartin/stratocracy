#!/usr/bin/env python3
"""Banner sweep for `Tools/architect/state.md` -- makes a self-contradicting record FAIL LOUDLY.

WHY THIS EXISTS, AND WHAT IT IS REPLACING. `11fd9ae` ("The banner check becomes the steward's
job instead of the coordinator's memory") made the banner sweep a RULE an agent has to remember
to run. On 2026-08-19 the rule was not remembered: `185e88f` closed the content-independence
item in a NEXT bullet and left the top-of-file banner saying that item was still open and the
suite was still 107/107, 425 lines apart in one file, both reading as authoritative. The
reviewer's gate caught it -- `VERDICT: BLOCK` -- which is the fourth instance of this exact
shape the record names, one of which had already cost a BLOCK in the combat-outcome
milestone's phase 3.

A rule an agent must remember is a rule that fails silently. This is the same rule as a
COMMAND, in the shape `strat_fixture_verdict_binding.py` already uses for fixtures: a
declaration the maintainer edits deliberately, checked mechanically, exiting non-zero when the
document disagrees with itself or with the tree.

WHAT IT CHECKS, and each is falsifiable by `--self-test`:

  1. SUITE COUNT AGREEMENT. Every LIVE `N/N` suite claim in the document must agree with every
     other live one, and -- when the artifacts are present -- with `Saved/AutomationReport/
     index.json` and with an independent `IMPLEMENT_SIMPLE_AUTOMATION_TEST` macro census.
     This is the check that would have caught 2026-08-19's banner directly.

  2. ITEM STATE AGREEMENT. For each DECLARED tracked item, an unstamped sentence calling it
     open and an unstamped sentence calling it closed cannot both stand. This is the
     content-independence half of the same failure.

  3. BANNER DATE FRESHNESS. The banner's own `_Last run YYYY-MM-DD` must not be older than the
     newest date the document's entries carry. The re-gate flagged this as live drift:
     the banner said 2026-08-14 while entries were dated 2026-08-19.

THE LIVE-VERSUS-STAMPED DISTINCTION IS THIS SCRIPT'S ONE PIECE OF JUDGEMENT, AND IT IS A
HEURISTIC -- SAID PLAINLY BECAUSE A READER WHO THINKS IT IS EXACT WILL TRUST A PASS TOO FAR.
`state.md`'s convention is that an older claim STAYS, stamped with its own item, and only the
newest statement carries the current number. Four rules separate them, each one added because
the version before it got a real case wrong, and each carrying its own `--self-test` fixture:

  * SECTION. Only the banner, `## NEXT`, `## BUILT` and `## DECISIONS` hold live claims;
    milestone sections are the record of a closed phase and their figures are correct FOR
    that phase (`_LIVE_SECTIONS`).
  * EXPLICIT STAMP. A `[STAMPED ...]`, a `CORRECTION,`, or the entry's own `reportCreatedOn`
    outranks everything -- present tense inside a stamped entry is still history.
  * TENSE. Failing that, the words immediately before a figure decide: "is now" is live,
    "was ... at that pass" is not. A window cannot do this, because a corrected banner puts
    both verbs in one sentence.
  * BANNER SUPERSESSION. The banner is a chronological narrative that accretes segments, so
    only its LAST figure is the current claim.

WHAT THIS CANNOT CATCH, MEASURED RATHER THAN SUPPOSED. Under banner supersession, a stale
figure sitting MID-banner passes: verified by editing the real `state.md` to claim
"Suite is now 107/107" in an early banner segment, running the sweep, and getting
`SWEEP CLEAN`. The same edit applied to the banner's LAST figure -- the exact shape of the
2026-08-19 defect -- fails with `EXIT=1`. So this sweep polices the CURRENT claim, not the
truth of every sentence in the narrative behind it. It also cannot read prose: it matches
shapes near a declared item's name, and `--explain` prints its call on every claim precisely
so a human can overrule it.

USAGE
    python Tools/architect/strat_banner_sweep.py                  # sweep, exit 0/1
    python Tools/architect/strat_banner_sweep.py --explain        # + per-claim classification
    python Tools/architect/strat_banner_sweep.py --self-test      # prove the sweep can FAIL
"""

from __future__ import annotations

import argparse
import io
import json
import os
import re
import sys
from dataclasses import dataclass, field

# ---------------------------------------------------------------------------
# Paths, resolved from this file rather than from the caller's cwd, so the
# sweep behaves the same run from the repo root or from Tools/architect/.
# ---------------------------------------------------------------------------
_HERE = os.path.dirname(os.path.abspath(__file__))
_REPO = os.path.abspath(os.path.join(_HERE, os.pardir, os.pardir))

STATE_MD = os.path.join(_HERE, "state.md")
REPORT_JSON = os.path.join(_REPO, "Saved", "AutomationReport", "index.json")
SOURCE_DIR = os.path.join(_REPO, "Source")


# ---------------------------------------------------------------------------
# THE DECLARATION. Adding an item here is a deliberate act, exactly as adding a
# FixtureSpec to strat_fixture_verdict_binding.py is -- the script never guesses
# what the project is tracking.
#
# `open_shapes` and `closed_shapes` are matched case-insensitively against a
# sentence that also mentions `slug_re`. An item whose two shape sets both hit
# LIVE sentences is a contradiction.
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class TrackedItem:
    name: str
    slug_re: str
    open_shapes: tuple[str, ...] = (
        r"remains? genuinely open",
        r"remains? open",
        r"still open",
        r"is NOT and is carried",
        r"\bnot discharged\b",
    )
    closed_shapes: tuple[str, ...] = (
        r"\bDISCHARGED\b",
        r"\bis now closed\b",
        r"\bnow closed\b",
    )


TRACKED_ITEMS: tuple[TrackedItem, ...] = (
    TrackedItem(
        name="content-independence corpus",
        slug_re=r"content[- ]independen\w*",
    ),
    TrackedItem(
        name="chooseBuild buildlist question",
        slug_re=r"chooseBuild",
        # `chooseBuild` is RULED and FILED but NOT implemented, so "not discharged"
        # is a legitimate live claim about the mechanism. Only the specific
        # "awaiting a Director ruling" shape is now false -- that ruling exists.
        open_shapes=(r"awaiting a Director ruling",),
        closed_shapes=(r"\bRULED\b", r"\bFILED\b"),
    ),
)


# A paragraph carrying any of these is treated as STAMPED -- an older claim that
# the document has already marked as historical. See the docstring's warning.
_PARAGRAPH_STAMP_MARKERS: tuple[str, ...] = (
    r"\[STAMPED",
    r"\[BANNER CORRECTED",
    r"\*\*CORRECTED",
    r"\*\*CORRECTION",
    r"CORRECTED,",
    r"CORRECTION,",
    r"reportCreatedOn\s+\d{4}\.\d{2}\.\d{2}",
)

# TENSE BINDS TO WHAT IT PRECEDES, and this replaced a generic "was **N/N**" marker that
# lived in the list above. A corrected banner says "suite **was** 107/107 at that pass and
# **is now** 108/108" inside ONE sentence, and no window around a figure can tell which of
# the two verbs owns it -- the marker version stamped BOTH and would have passed a banner
# whose live number was false. The self-test fixture `_BANNER_MIXED` is that exact shape and
# caught it. Read the words immediately before the figure instead.
_PRECEDING_STAMPED_RE = re.compile(r"(?:\bwas|\bwere|\bstood at|\bhad been)\s*$", re.I)
_PRECEDING_LIVE_RE = re.compile(r"(?:\bis now|\bnow|\bcurrently)\s*$", re.I)

# WHERE A CLAIM LIVES DECIDES WHETHER IT IS LIVE, and this is the document's own
# convention rather than a convenience. `state.md` is a banner plus `## NEXT`
# (current state) followed by milestone sections (the record of a closed phase).
# A `**87/87**` inside "## AI-opponent milestone" is the correct figure FOR THAT
# PHASE and must never be flagged; the same string in the banner is a defect.
# Discovered by running this sweep against the real file, which flagged exactly
# that 87/87 -- the first finding was the script's own, not the document's.
_LIVE_SECTIONS: tuple[str, ...] = ("## NEXT", "## BUILT", "## DECISIONS")

# How much text around a claim counts as "its own sentence" for the stamp test.
# Wide enough to catch a stamp attached to the claim, narrow enough that a stamp
# on a NEIGHBOURING claim in the same paragraph does not launder this one.
_STAMP_WINDOW = 220

# BOLD IS NOT PART OF THE CLAIM, and assuming it was made this check blind to the very
# figure it exists to police: the corrected banner reads "**is now 108/108**", with the
# asterisks wrapping the PHRASE rather than the number, so a `\*\*N/N\*\*` pattern did
# not see the current figure at all and the sweep reported the banner's SECOND-newest
# number as its live one. Match the figure; let the markup fall where it likes.
_SUITE_CLAIM_RE = re.compile(r"(?<![\d/.-])(\d{1,4})/(\d{1,4})(?![\d/.-])")
# What makes an `N/N` a SUITE claim rather than any other pair of equal numbers.
# How close a verdict word must sit to an item's own name to count as a claim ABOUT it.
# Reporting verbs that mark a nearby figure as QUOTED evidence rather than a live claim.
_QUOTED_FIGURE_RE = re.compile(
    "said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|"
    "wrongly|stale|defect|would have|reported", re.I)

_SUBJECT_WINDOW = 110

_SUITE_QUALIFIER_RE = re.compile("suite|succeeded|tests", re.I)

_BANNER_DATE_RE = re.compile(r"_Last run (\d{4})-(\d{2})-(\d{2})")
_ANY_DATE_RE = re.compile(r"\b(\d{4})-(\d{2})-(\d{2})\b")


# ---------------------------------------------------------------------------
# Findings
# ---------------------------------------------------------------------------
@dataclass
class Finding:
    check: str
    detail: str


@dataclass
class SuiteClaim:
    line_no: int
    numerator: int
    denominator: int
    live: bool
    text: str


@dataclass
class SweepResult:
    findings: list[Finding] = field(default_factory=list)
    suite_claims: list[SuiteClaim] = field(default_factory=list)
    report_count: int | None = None
    macro_count: int | None = None
    notes: list[str] = field(default_factory=list)

    @property
    def passed(self) -> bool:
        return not self.findings


# ---------------------------------------------------------------------------
# Paragraph splitting. `state.md` is a bullet document, so a "paragraph" here is
# a top-level bullet or a blank-line-separated block -- whichever is smaller --
# because a stamp on one bullet must not launder the bullet below it.
# ---------------------------------------------------------------------------
def paragraphs_with_lines(text: str) -> list[tuple[int, str]]:
    lines = text.splitlines()
    blocks: list[tuple[int, list[str]]] = []
    current: list[str] = []
    start = 1
    for idx, line in enumerate(lines, start=1):
        starts_bullet = re.match(r"^\s*-\s+\*\*", line) is not None
        if not line.strip() or starts_bullet:
            if current:
                blocks.append((start, current))
            current = [line]
            start = idx
        else:
            if not current:
                start = idx
            current.append(line)
    if current:
        blocks.append((start, current))
    return [(s, "\n".join(b)) for s, b in blocks if "\n".join(b).strip()]


def is_stamped(paragraph: str) -> bool:
    return any(re.search(m, paragraph) for m in _PARAGRAPH_STAMP_MARKERS)


def section_of(text: str, line_no: int) -> str:
    """The `## ` heading a line sits under, or "BANNER" above the first one."""
    heading = "BANNER"
    for idx, line in enumerate(text.splitlines(), start=1):
        if idx > line_no:
            break
        if line.startswith("## "):
            heading = line.strip()
    return heading


def in_live_section(text: str, line_no: int) -> bool:
    sec = section_of(text, line_no)
    return sec == "BANNER" or sec in _LIVE_SECTIONS


# ---------------------------------------------------------------------------
# Independent readings of the tree, so the document is checked against the
# ARTIFACTS and not only against itself.
# ---------------------------------------------------------------------------
def read_report_count(path: str = REPORT_JSON) -> tuple[int | None, str]:
    if not os.path.exists(path):
        return None, f"no automation report at {path} -- suite figures checked against each other only"
    try:
        # utf-8-sig: the report is UTF-8 WITH BOM. Two separate passes have
        # reported UTF-16 for this file and both were wrong.
        with io.open(path, encoding="utf-8-sig") as fh:
            data = json.load(fh)
    except Exception as exc:                                  # pragma: no cover
        return None, f"automation report unreadable ({exc}) -- treated as absent, not as agreement"
    tests = data.get("tests", [])
    non_success = [t for t in tests if t.get("state") != "Success"]
    if non_success:
        return None, f"automation report has {len(non_success)} non-Success entries -- suite count not read from a red report"
    return len(tests), f"automation report: {len(tests)} entries, all Success ({data.get('reportCreatedOn')})"


def read_macro_census(source_dir: str = SOURCE_DIR) -> tuple[int | None, str]:
    """Count clauses BY MACRO, never by acceptance-ID name -- an ID grep once undercounted 8 as 5."""
    if not os.path.isdir(source_dir):
        return None, f"no Source/ at {source_dir} -- macro census skipped"
    simple = 0
    complex_ = 0
    for root, _dirs, files in os.walk(source_dir):
        for name in files:
            if not name.endswith(".cpp"):
                continue
            try:
                with io.open(os.path.join(root, name), encoding="utf-8", errors="replace") as fh:
                    body = fh.read()
            except OSError:                                   # pragma: no cover
                continue
            simple += len(re.findall(r"IMPLEMENT_SIMPLE_AUTOMATION_TEST", body))
            complex_ += len(re.findall(r"IMPLEMENT_COMPLEX_AUTOMATION_TEST(?:_CLASS)?", body))
    note = f"macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST={simple}, COMPLEX/_CLASS={complex_}"
    if complex_:
        note += " -- COMPLEX macros exist, so the SIMPLE sum alone is not the suite size"
        return None, note
    return simple, note


# ---------------------------------------------------------------------------
# The three checks
# ---------------------------------------------------------------------------
def check_suite_counts(text: str, result: SweepResult) -> None:
    for start, para in paragraphs_with_lines(text):
        for m in _SUITE_CLAIM_RE.finditer(para):
            # STAMPEDNESS IS JUDGED ON THE CLAIM'S OWN SENTENCE, NOT ITS WHOLE PARAGRAPH.
            # The banner is one enormous paragraph carrying many claims, and after it was
            # corrected it contains BOTH "suite was **107/107** at that pass" (stamped) and
            # "is now 108/108" (live). Judging the paragraph as a unit marked the whole
            # banner stamped and made this check -- the one that exists for the banner --
            # silently inert on it. Measured: "17 claims (0 live)" on a file whose banner
            # carries a live figure. A window, not the paragraph.
            # A BARE `N/N` IS NOT NECESSARILY A SUITE COUNT. Dropping the bold requirement
            # made this check swallow the pairing gate's own figures -- `42/42` and `68/68`
            # are attacks paired with resolutions, and flagging them as stale suite counts
            # is noise that would train a reader to ignore the sweep. The claim has to say
            # what it is counting.
            qualifier = para[max(0, m.start() - 90):m.start()]
            if not _SUITE_QUALIFIER_RE.search(qualifier):
                continue
            # A FIGURE CAN BE QUOTED AS EVIDENCE RATHER THAN ASSERTED AS CURRENT, and this
            # document is now full of that: the entry describing the 2026-08-19 defect
            # necessarily writes the wrong number down ("the banner said the suite 107/107")
            # and the falsifiability record quotes the failing output verbatim. Flagging
            # those would make the sweep fail on any honest account of a past miscount --
            # which would train a maintainer to stop running it. A reporting verb, or a
            # code span, marks the figure as quoted.
            if _QUOTED_FIGURE_RE.search(para[max(0, m.start() - 120):m.start()]):
                continue
            if para[max(0, m.start() - 1):m.start()] == "`":
                continue
            before = para[max(0, m.start() - 40):m.start()]
            window = para[max(0, m.start() - _STAMP_WINDOW):m.end() + _STAMP_WINDOW]
            # PRECEDENCE, and both orderings were wrong before this one. An EXPLICIT stamp
            # (a `[STAMPED ...]`, a `CORRECTION,`, or the entry's own `reportCreatedOn`)
            # is a deliberate act by the maintainer and outranks everything: a bullet
            # reading "Suite is now **107/107** (`reportCreatedOn 2026.08.14-21.47.35`)"
            # is history in present tense, and the timestamp is the tell. Only when no
            # explicit stamp is near does the verb decide.
            if is_stamped(window):
                stamped = True
            elif _PRECEDING_LIVE_RE.search(before):
                stamped = False              # "is now **N/N**" with nothing stamping it
            elif _PRECEDING_STAMPED_RE.search(before):
                stamped = True               # "was **N/N** at that pass"
            else:
                stamped = False
            n, d = int(m.group(1)), int(m.group(2))
            if n != d:
                continue                      # "103 -> 104" style progressions are not claims
            line_no = start + para[:m.start()].count("\n")
            snippet = " ".join(para[max(0, m.start() - 70):m.end() + 40].split())
            live = (not stamped) and in_live_section(text, line_no)
            result.suite_claims.append(SuiteClaim(line_no, n, d, live, snippet))

    # THE BANNER IS A CHRONOLOGICAL NARRATIVE, NOT ONE SENTENCE. It accretes
    # "ALSO POST-MILESTONE ..." segments in order, each of which said "suite is now N/N"
    # truthfully on its day, so it legitimately reads 104 -> 106 -> 107 -> 108 top to
    # bottom. Only its LAST figure is the current claim; the earlier ones are superseded
    # by position, the same way the document supersedes prose by appending. Anything
    # outside the banner keeps every live claim, so a stale NEXT bullet is still caught --
    # and a banner whose LAST figure disagrees with NEXT or with the tree, which is
    # exactly 2026-08-19's defect, still fails.
    banner_live = [c for c in result.suite_claims if c.live and section_of(text, c.line_no) == "BANNER"]
    superseded = {id(c) for c in banner_live[:-1]}
    for c in result.suite_claims:
        if id(c) in superseded:
            c.live = False
    if superseded:
        result.notes.append(
            f"banner narrative: {len(superseded)} earlier 'is now' figure(s) superseded by the "
            f"banner's own last one -- only the newest banner claim is treated as current")

    live = [c for c in result.suite_claims if c.live]
    distinct = sorted({c.numerator for c in live})
    if len(distinct) > 1:
        where = "; ".join(f"line {c.line_no}: {c.numerator}/{c.denominator}" for c in live)
        result.findings.append(Finding(
            "SUITE COUNT AGREEMENT",
            f"live suite claims disagree with each other: {distinct} -- {where}",
        ))

    truth = result.report_count if result.report_count is not None else result.macro_count
    if truth is not None and live:
        wrong = [c for c in live if c.numerator != truth]
        if wrong:
            where = "; ".join(f"line {c.line_no}: {c.numerator}/{c.denominator}" for c in wrong)
            result.findings.append(Finding(
                "SUITE COUNT AGREEMENT",
                f"live suite claim(s) disagree with the tree ({truth}): {where}",
            ))


def check_item_states(text: str, result: SweepResult) -> None:
    for item in TRACKED_ITEMS:
        open_hits: list[tuple[int, str]] = []
        closed_hits: list[tuple[int, str]] = []
        for start, para in paragraphs_with_lines(text):
            if is_stamped(para) or not re.search(item.slug_re, para, re.I):
                continue
            # THE SHAPE MUST BE ABOUT THIS ITEM, NOT MERELY NEAR IT. Paragraph proximity is
            # far too coarse and produced two false positives on the real file, both of the
            # "subject, not scope" class this project has paid for repeatedly:
            # "host-independence DISCHARGED; content-independence carried to NEXT" read as
            # the content item being called closed, and a bullet about the FIXTURES binding
            # being "still open" read as this item being called open. A verdict counts only
            # when it sits within `_SUBJECT_WINDOW` characters of the item's own name.
            slug_spans = [mm.span() for mm in re.finditer(item.slug_re, para, re.I)]
            for shapes, bucket in ((item.open_shapes, open_hits), (item.closed_shapes, closed_hits)):
                hit = None
                for shape in shapes:
                    for m in re.finditer(shape, para, re.I):
                        if any(abs(m.start() - e) <= _SUBJECT_WINDOW
                               or abs(b - m.end()) <= _SUBJECT_WINDOW
                               for b, e in slug_spans):
                            hit = m
                            break
                    if hit:
                        break
                if hit:
                    line_no = start + para[:hit.start()].count(chr(10))
                    snippet = " ".join(para[max(0, hit.start() - 60):hit.end() + 60].split())
                    bucket.append((line_no, snippet, start))
        # A CONTRADICTION IS DISAGREEMENT BETWEEN SITES, NOT NUANCE WITHIN ONE.
        # A single bullet saying "discharged on two of three axes, the third still
        # open" is COHERENT and must not be flagged; two separate unstamped bullets,
        # one calling the item open and the other closed, is the actual defect this
        # sweep exists for. Learned by running the first version against the real
        # file, where it flagged the content-independence bullet for being precise.
        open_paras = {h[2] for h in open_hits}
        closed_paras = {h[2] for h in closed_hits}
        if open_paras - closed_paras and closed_paras - open_paras:
            o = sorted(h[0] for h in open_hits if h[2] not in closed_paras)
            c = sorted(h[0] for h in closed_hits if h[2] not in open_paras)
            result.findings.append(Finding(
                "ITEM STATE AGREEMENT",
                f"'{item.name}' is called open at line(s) {o} and closed at line(s) {c} "
                f"by unstamped text in DIFFERENT places -- one of them is wrong",
            ))


def check_banner_date(text: str, result: SweepResult) -> None:
    m = _BANNER_DATE_RE.search(text)
    if not m:
        result.notes.append("no `_Last run YYYY-MM-DD` banner date found -- freshness check skipped")
        return
    banner = tuple(int(g) for g in m.groups())
    newest = banner
    for d in _ANY_DATE_RE.finditer(text):
        cand = tuple(int(g) for g in d.groups())
        if cand > newest:
            newest = cand
    if newest > banner:
        result.findings.append(Finding(
            "BANNER DATE FRESHNESS",
            "banner says _Last run %04d-%02d-%02d but the document carries entries dated "
            "%04d-%02d-%02d" % (banner + newest),
        ))


# ---------------------------------------------------------------------------
def run_sweep(state_path: str = STATE_MD, *, check_tree: bool = True) -> SweepResult:
    result = SweepResult()
    with io.open(state_path, encoding="utf-8") as fh:
        text = fh.read()

    if check_tree:
        result.report_count, note = read_report_count()
        result.notes.append(note)
        result.macro_count, note = read_macro_census()
        result.notes.append(note)
        if (result.report_count is not None and result.macro_count is not None
                and result.report_count != result.macro_count):
            result.findings.append(Finding(
                "TREE DISAGREES WITH ITSELF",
                f"automation report says {result.report_count}, macro census says "
                f"{result.macro_count} -- fix the tree before trusting any document about it",
            ))
    else:
        result.notes.append("--no-tree: document checked against itself only")

    check_suite_counts(text, result)
    check_item_states(text, result)
    check_banner_date(text, result)
    return result


def render(result: SweepResult, explain: bool = False) -> str:
    out: list[str] = ["Banner sweep: state.md"]
    for n in result.notes:
        out.append(f"  {n}")
    live = [c for c in result.suite_claims if c.live]
    out.append(f"  suite claims found: {len(result.suite_claims)} ({len(live)} live, "
               f"{len(result.suite_claims) - len(live)} stamped)")
    out.append(f"  tracked items declared: {len(TRACKED_ITEMS)}")
    if explain:
        out.append("")
        out.append("  --explain: every suite claim and this script's LIVE/STAMPED call on it.")
        out.append("  The call is a shape heuristic; check it rather than trusting it.")
        for c in sorted(result.suite_claims, key=lambda c: c.line_no):
            out.append(f"    line {c.line_no:>5}  {'LIVE  ' if c.live else 'stamped'}  "
                       f"{c.numerator}/{c.denominator}  ...{c.text}...")
    if result.findings:
        out.append("")
        for f in result.findings:
            out.append(f"  [**{f.check}**] {f.detail}")
    out.append("")
    out.append("SWEEP CLEAN -- no self-contradiction found" if result.passed
               else "SWEEP FAILED -- the record contradicts itself or the tree")
    return "\n".join(out)


# ---------------------------------------------------------------------------
# Self-test: inline fixtures proving each check CAN fail, and that a clean
# document passes. Without this the sweep is a green light nobody has tested.
# ---------------------------------------------------------------------------
_CLEAN = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**; the content-independence corpus is DISCHARGED.)_

## NEXT

- **An older entry, stamped with its own item.** Suite was **107/107** at that pass
  (`reportCreatedOn 2026.08.14-21.47.35`), and content-independence remains genuinely open.
  **[STAMPED 2026-08-19 -- DISCHARGED since; true when written.]**
"""

_BAD_SUITE = """# Architect state

_Last run 2026-08-19 (suite is now **107/107**.)_

## NEXT

- **A later bullet.** Suite **108/108** as of today.
"""

_BAD_ITEM = """# Architect state

_Last run 2026-08-19 (the content-independence half is NOT and is carried forward.)_

## NEXT

- **DISCHARGED 2026-08-19 -- the content-independence corpus.** Done.
"""

_BAD_DATE = """# Architect state

_Last run 2026-08-14 (suite is now **108/108**.)_

## NEXT

- **A bullet dated later than the banner.** Landed 2026-08-19.
"""

_BAD_CHOOSEBUILD = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **chooseBuild is RULED and FILED upstream.** Done today.
- **Another bullet.** The chooseBuild question is a request draft awaiting a Director ruling.
"""


_NUANCED = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **DISCHARGED 2026-08-19 -- the content-independence corpus.** Closed for two of the three
  axes the precedent named; the scenario axis is still open and is stated so rather than
  folded in.
"""

_HISTORY = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **Nothing outstanding.**

## AI-opponent milestone

- **Measured:** Suite **87/87**, 0 failed. Correct for that phase, and not a live claim.
"""


# The banner as it looks AFTER a correction: a stamped historical figure and a live
# one in the same paragraph. The live one here is wrong (the other bullet says 108),
# and the sweep must still catch it -- this is the regression fixture for the hole
# the paragraph-wide stamp test opened.
_BANNER_MIXED = """# Architect state

_Last run 2026-08-19 (suite was **107/107** at that pass and is now **106/106** as of today.)_

## NEXT

- **A later bullet.** Suite **108/108**.
"""


_STAMPED_PRESENT = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **An older correction, present tense, stamped by its own timestamp.** Suite is now
  **107/107** (`succeeded 107 / failed 0`, `reportCreatedOn 2026.08.14-21.47.35`).
"""

_BANNER_NARRATIVE = """# Architect state

_Last run 2026-08-19 (a pass: suite is now **104/104**. ALSO: suite is now **106/106**.
ALSO, later: suite is now **108/108**.)_

## NEXT

- **A bullet.** Suite **108/108**.
"""

_BANNER_NARRATIVE_STALE = """# Architect state

_Last run 2026-08-19 (a pass: suite is now **104/104**. ALSO, later: suite is now **107/107**.)_

## NEXT

- **DISCHARGED today.** Suite **108/108**.
"""


# The bold wraps the phrase, not the number -- the shape that made this check blind.
_UNBOLDED = """# Architect state

_Last run 2026-08-19 (suite **is now 107/107** as of today.)_

## NEXT

- **A later bullet.** Suite **108/108**.
"""


# The gate's own corpus figures live in this document too, and they are not suite counts.
_PAIRING_FIGURES = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **The corpus graded clean.** 42 applied attacks against 42 resolved, 42/42 with zero
  mismatches; the control corpus reads 68/68 the same way.
"""


# Two subjects, one sentence, one verdict -- and the verdict belongs to the other one.
_CROSS_TALK = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A phase entry.** host-independence discharged; content-independence carried to NEXT as its
  own future work, unchanged by this phase.
- **Still open, untouched by this pass:** the checked-in fixtures have no binding, which is a
  debt distinct from the corpus named in the bullet above.
"""


# Describing a past miscount requires writing the wrong number down. If that fails the
# sweep, nobody will write the account -- and the account is the point of the document.
_QUOTED_ACCOUNT = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A defect recorded honestly.** The banner said the suite 107/107 while this same file said
  108, which is the shape the sweep now catches; editing the banner to `107/107` fails with
  `live suite claim(s) disagree with the tree (108)`.
"""


def check_self_test() -> tuple[bool, str]:
    import tempfile

    cases: list[tuple[str, str, bool]] = [
        ("clean document passes", _CLEAN, True),
        ("one bullet, discharged-except-one-axis, PASSES", _NUANCED, True),
        ("a milestone section's own older count PASSES", _HISTORY, True),
        ("a corrected banner carrying was-107 AND a WRONG live figure still FAILS",
         _BANNER_MIXED, False),
        ("present-tense figure stamped by its own reportCreatedOn PASSES", _STAMPED_PRESENT, True),
        ("banner narrative 104 -> 108 with NEXT at 108 PASSES", _BANNER_NARRATIVE, True),
        ("banner narrative whose LAST figure is stale still FAILS", _BANNER_NARRATIVE_STALE, False),
        ("an UNBOLDED live figure is still seen, and still FAILS when wrong", _UNBOLDED, False),
        ("a pairing figure like 42/42 is NOT a suite claim", _PAIRING_FIGURES, True),
        ("a verdict about a NEIGHBOURING subject is not this item's", _CROSS_TALK, True),
        ("an honest account of a PAST miscount is not a live claim", _QUOTED_ACCOUNT, True),
        ("banner 107 vs bullet 108 FAILS", _BAD_SUITE, False),
        ("item called open and closed FAILS", _BAD_ITEM, False),
        ("banner older than its entries FAILS", _BAD_DATE, False),
        ("chooseBuild ruled and awaiting a ruling FAILS", _BAD_CHOOSEBUILD, False),
    ]
    lines: list[str] = []
    ok = True
    for name, body, want_pass in cases:
        with tempfile.TemporaryDirectory() as d:
            p = os.path.join(d, "state.md")
            with io.open(p, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(body)
            # check_tree=False: these fixtures are about the DOCUMENT's internal
            # consistency, and pointing them at the real Source/ would make the
            # self-test's verdict depend on today's suite size.
            res = run_sweep(p, check_tree=False)
        got_pass = res.passed
        good = (got_pass == want_pass)
        ok = ok and good
        detail = "" if res.passed else " -- " + "; ".join(f.check for f in res.findings)
        lines.append(f"    [{'OK' if good else '**WRONG**'}] {name}: "
                     f"expected {'PASS' if want_pass else 'FAIL'}, "
                     f"got {'PASS' if got_pass else 'FAIL'}{detail}")
    return ok, "\n".join(lines)


# ---------------------------------------------------------------------------
def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Banner sweep for Tools/architect/state.md. Fails when the document contradicts "
            "itself on a suite count or a tracked item's open/closed state, or when its banner "
            "date is older than its own entries. Mechanises the rule 11fd9ae made a memory job."
        )
    )
    parser.add_argument("state_path", nargs="?", default=STATE_MD,
                        help=f"Path to state.md (default: {STATE_MD}).")
    parser.add_argument("--explain", action="store_true",
                        help="Print every suite claim with this script's LIVE/STAMPED call on it.")
    parser.add_argument("--no-tree", action="store_true",
                        help="Check the document against itself only; skip the report and macro census.")
    parser.add_argument("--self-test", action="store_true",
                        help="Run the inline fixtures proving this sweep can FAIL, and exit.")
    args = parser.parse_args(argv)

    # THE DOCUMENT CONTAINS CHARACTERS THIS CONSOLE CANNOT SPELL, and dying on one is
    # not an acceptable failure for a tool whose whole job is to be run casually.
    # Measured: `--explain` on the real state.md raised UnicodeEncodeError on U+2194
    # under Windows cp1252, from a snippet quoted out of the file itself.
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")   # type: ignore[union-attr]
    except Exception:                                                # pragma: no cover
        pass

    if args.self_test:
        ok, report = check_self_test()
        print(report)
        print("SELF-TEST: ALL FIXTURES CORRECT" if ok else "SELF-TEST: AT LEAST ONE FIXTURE WRONG")
        return 0 if ok else 1

    result = run_sweep(args.state_path, check_tree=not args.no_tree)
    print(render(result, explain=args.explain))
    return 0 if result.passed else 1


if __name__ == "__main__":
    sys.exit(main())
