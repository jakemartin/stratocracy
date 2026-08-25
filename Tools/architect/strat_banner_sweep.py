#!/usr/bin/env python3
"""Banner sweep for `Tools/architect/state/` -- makes a self-contradicting record FAIL LOUDLY.

WHAT IT SWEEPS, AND WHY THAT IS DERIVED RATHER THAN TYPED. Until 2026-08-20 the record was one
3,524-line `state.md` and this script named it in a constant. It is now `Tools/architect/state/`,
one file per owning agent, and the file list is READ FROM THE DIRECTORY. Had the constant simply
been repointed, adding `state/ux.md` would have swept nothing and printed clean over it -- the
"a checker that types its own subject list stops covering it silently" shape this project has
already paid for twice in the pre-commit hook. A missing or empty directory is a hard finding
(`NOTHING SWEPT`), never a quiet exit 0. `state.md` itself is now FROZEN history and is not
swept; its own header says so.

THE COMPARISON IS ACROSS THE SET, NOT FILE BY FILE. Splitting the record moved contradictions
between files; it did not remove them. Six per-file runs would each print clean on a set whose
`global.md` and `tests.md` disagree. So claims are collected per file and compared globally,
and `check_record_ownership` enforces the split's third condition: exactly one file --
`global.md` -- may carry a LIVE suite count. Quoting a stamped older figure stays legal
everywhere, because this record's convention is that an old claim is stamped, not deleted.

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

  3. BANNER DATE FRESHNESS. The banner in `global.md` -- the file that OWNS it; a `_Last run`
     quoted as evidence elsewhere is prose -- must not be older than the newest date any file
     in the set carries. The re-gate flagged this as live drift:
     the banner said 2026-08-14 while entries were dated 2026-08-19.

  4. REPORT IDENTITY. Before `Saved/AutomationReport/index.json`'s count is trusted as ground
     truth for check 1, the sweep must know WHICH RUN produced it -- not merely that the count
     happens to agree. On 2026-08-21 the banner said 140/140, the report said 140/140, and the
     sweep printed SWEEP CLEAN, but the report was written from a PRE-MERGE tree; it was right
     only because the merge did not happen to move the count. A count comparison alone cannot
     tell a report that still describes the tree from one that no longer does -- only a
     timestamp can. So the report must carry a readable `reportCreatedOn` AND must not predate
     any test-defining `.cpp` file on disk; either failure is a hard FAIL, never a warning,
     because a sweep that cannot identify its evidence has not verified anything.

THE LIVE-VERSUS-STAMPED DISTINCTION IS THIS SCRIPT'S ONE PIECE OF JUDGEMENT, AND IT IS A
HEURISTIC -- SAID PLAINLY BECAUSE A READER WHO THINKS IT IS EXACT WILL TRUST A PASS TOO FAR.
`state.md`'s convention is that an older claim STAYS, stamped with its own item, and only the
newest statement carries the current number. Four rules separate them, each one added because
the version before it got a real case wrong, and each carrying its own `--self-test` fixture:

  * SECTION. Only the banner, `## NEXT`, `## BUILT` and `## DECISIONS` hold live claims;
    milestone sections are the record of a closed phase and their figures are correct FOR
    that phase (`_LIVE_SECTIONS`). After the split every `state/` file is banner-or-`## NEXT`,
    so in practice everything unstamped is live -- the closed-phase narrative that this rule
    was written for now lives in the frozen `state.md`.
  * OWNERSHIP. A live suite count outside `global.md` is a finding whatever its tense.
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
    python Tools/architect/strat_banner_sweep.py                  # sweep state/, exit 0/1
    python Tools/architect/strat_banner_sweep.py state/tests.md   # one file (labelled global.md)
    python Tools/architect/strat_banner_sweep.py --explain        # + per-claim classification
    python Tools/architect/strat_banner_sweep.py --self-test      # prove the sweep can FAIL
"""

from __future__ import annotations

import argparse
import datetime
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

# THE RECORD IS A DIRECTORY, AND THE FILE LIST IS DERIVED FROM IT RATHER THAN TYPED.
# `state.md` was split into `state/` by owning agent on 2026-08-20, and the version of this
# script that preceded the split hardcoded `STATE_MD = .../state.md`. Left that way, creating
# `state/ux.md` would have swept nothing, warned about nothing, and printed clean over it --
# the recorded "a checker that types its own subject list stops covering it silently" shape,
# which this project has already paid for twice in the pre-commit hook's citation loop. So the
# set is whatever `state/` contains, and an EMPTY or MISSING directory is a hard refusal rather
# than a quiet pass: a sweep with nothing to sweep is the inert-guard failure, not a clean run.
STATE_DIR = os.path.join(_HERE, "state")

# The one file allowed to carry a live suite count or phase verdict. Condition 3 of the split:
# if two files may both say "the suite is 108/108" they will drift, so exactly one owns it and
# the others link. Enforced by `check_record_ownership`, not by convention.
GLOBAL_DOC = "global.md"

STATE_MD = os.path.join(_HERE, "state.md")   # frozen history; NOT swept. See its own header.
REPORT_JSON = os.path.join(_REPO, "Saved", "AutomationReport", "index.json")
SOURCE_DIR = os.path.join(_REPO, "Source")


def discover_state_files(state_dir: str = STATE_DIR) -> tuple[list[str], str]:
    """Every `*.md` in the record directory, sorted. Derived, never enumerated by hand."""
    if not os.path.isdir(state_dir):
        return [], f"NO RECORD DIRECTORY at {state_dir}"
    names = sorted(n for n in os.listdir(state_dir)
                   if n.endswith(".md") and os.path.isfile(os.path.join(state_dir, n)))
    if not names:
        return [], f"RECORD DIRECTORY {state_dir} CONTAINS NO .md FILES"
    return [os.path.join(state_dir, n) for n in names], ""


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
# AND THE MARKUP SITS BETWEEN THE VERB AND THE FIGURE, so the anchor has to reach across it.
# `suite was **107/107**` puts `**` between `was` and the number, and a `\s*$` anchor does not
# span it: the verb was never seen, the claim fell through to the default, and the default is
# LIVE. Every `was **N/N**` in the real record happens to carry an explicit stamp -- a
# `[STAMPED ...]` or a `reportCreatedOn` -- which outranks the verb anyway, so the whole record
# masked this and the `_CLEAN` and `_BANNER_MIXED` fixtures masked it too. Found 2026-08-22 by
# writing a fixture whose banner is stamped by ITS VERB ALONE, which nothing had ever done.
_TENSE_MARKUP = r"[\s*_]*$"
_PRECEDING_STAMPED_RE = re.compile(r"(?:\bwas|\bwere|\bstood at|\bhad been)" + _TENSE_MARKUP, re.I)
_PRECEDING_LIVE_RE = re.compile(r"(?:\bis now|\bnow|\bcurrently)" + _TENSE_MARKUP, re.I)

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
# A FULL STOP IS PUNCTUATION, NOT PART OF THE FIGURE, and the trailing lookahead used to
# treat it as part of the figure: `(?![\d/.-])` excluded `.` outright, so
# `The suite is now 999/999.` did not match while `The suite is now 999/999 green` did.
# Measured both ways 2026-08-20 against the live file and against a staged commit in two
# separate trees: with the full stop the hook printed `banner sweep: clean` and exited 0;
# without it, `SWEEP FAILED` and `COMMIT REFUSED`. All 20 real claims in `state.md` happened
# to be followed by `*`, a space or a comma, so the blind spot never fired -- the guard had
# never once been tested against the most natural way to end the sentence.
# The lookahead now rejects only what would make this a DIFFERENT number: another digit, a
# second slash, a hyphen, or a decimal point WITH A DIGIT BEHIND IT (`1.2/3.4`). A `.` that
# ends a sentence no longer hides the claim.
_SUITE_CLAIM_RE = re.compile(r"(?<![\d/.-])(\d{1,4})/(\d{1,4})(?![\d/-])(?!\.\d)")
# What makes an `N/N` a SUITE claim rather than any other pair of equal numbers.
# How close a verdict word must sit to an item's own name to count as a claim ABOUT it.
# Reporting verbs that mark a nearby figure as QUOTED evidence rather than a live claim.
_QUOTED_FIGURE_RE = re.compile(
    "said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|"
    "wrongly|stale|defect|would have|reported", re.I)

# HOW FAR UPSTREAM A REPORTING VERB REACHES, and this exclusion is the widest hole this
# script has ever had. Measured 2026-08-22 by planting `The suite is now 161/161 on this
# tree.` at each of the 766 non-blank lines of `global.md`'s `## NEXT` in turn: at 132 of
# them the claim was not merely called stamped, it was NEVER COLLECTED -- `continue` drops
# it before it reaches the live/stamped call -- and the sweep printed `SWEEP CLEAN`, exit 0.
# Every one of the 132 was this exclusion firing, on `read `, `claim`, `reads `, `shape`,
# `reading`, `defect`, `stale`, `says`, `reported`, `would have` or `said`. Those are words
# this record uses constantly, so the guard's coverage of its own subject was decided by the
# vocabulary of whatever sentence happened to sit upstream.
# It also explains a divergence that looks like a markup rule and is not: at two of those
# sites `THE SUITE **IS NOW 161/161**` was seen while `The suite is now 161/161` was missed.
# The bold form is two characters longer before the figure, which pushed the reporting verb
# past the cut. Nothing about bold is load-bearing -- `_SUITE_CLAIM_RE` has matched a bare
# `N/N` since 2026-08-20 and `_UNBOLDED` fixes that. It was this window's edge, twice.
_QUOTE_WINDOW = 120

# A REPORTING VERB QUOTES THE FIGURE IN ITS OWN SENTENCE, NOT THE ONE AFTER IT. "The proxy's
# Reason string describes a read it never performed. The suite is now 161/161." is two
# sentences: the first is an honest account, the second is a live assertion, and only a raw
# character count confuses them. So the quoting window stops at the last sentence boundary
# inside it. The boundary has to be recognised in THIS document's prose, where a full stop is
# far more often part of `2.11.6-B`, `global.md` or `2026.08.14-21.47.35` than the end of a
# sentence -- so a break is a `.`, `!` or `?`, optionally closing a bold run, followed by
# whitespace and something that starts a sentence. A version-number dot is followed by a
# digit and a filename dot by a lowercase letter, so neither is read as a break.
_SENTENCE_BREAK_RE = re.compile(
    r"""[.!?][*"'\)\]]{0,3}\s+(?=(?:\*\*|["'`(\[])?[A-Z])""")

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
    doc: str
    line_no: int
    numerator: int
    denominator: int
    live: bool
    text: str


@dataclass
class SweepResult:
    docs: list[str] = field(default_factory=list)
    findings: list[Finding] = field(default_factory=list)
    suite_claims: list[SuiteClaim] = field(default_factory=list)
    report_count: int | None = None
    macro_count: int | None = None
    # REPORT IDENTITY, added for the "which run" fix: the report artifact's own write time,
    # its self-declared `reportCreatedOn` (parsed; None if missing/unparseable), and the
    # newest mtime among test-defining source files -- the instrument that catches a report
    # that describes a tree the source has since moved past.
    report_mtime: float | None = None
    report_created_on: "datetime.datetime | None" = None
    report_created_on_raw: str | None = None
    newest_test_mtime: float | None = None
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


def in_code_span(para: str, at: int) -> str | None:
    """The backtick span containing the figure at `at`, or None if it sits in prose.

    A code span is a VERBATIM QUOTE and outranks everything, including an explicit live
    tense: this record's own entries quote the banner's exact text, backticks and asterisks
    and all, when describing what the banner said. The predecessor of this function tested
    only the single character before the figure, which caught `` `107/107` `` and missed
    `` `THE SUITE **IS NOW 160/160**` `` -- the character before that figure is an asterisk.
    Measured 2026-08-22: with the live tense given precedence over the reporting-verb
    exclusion, that quoted line in `global.md` was collected as a SECOND live claim. It
    agreed with the banner by luck, so the sweep stayed green while carrying a claim it had
    misread. Backticks are counted within the figure's own LINE, so an unclosed span cannot
    swallow the rest of the paragraph.
    """
    line_start = para.rfind("\n", 0, at) + 1
    line_end = para.find("\n", at)
    line_end = len(para) if line_end == -1 else line_end
    if para.count("`", line_start, at) % 2 == 0:
        return None
    open_at = para.rfind("`", line_start, at)
    close_at = para.find("`", at, line_end)
    return para[open_at:(close_at + 1) if close_at != -1 else line_end]


def quoting_window(para: str, at: int) -> str:
    """The text a reporting verb would have to sit in to make the figure at `at` a quote.

    `_QUOTE_WINDOW` characters back, then truncated at the last sentence boundary in that
    span, for the reason `_SENTENCE_BREAK_RE` records.
    """
    window = para[max(0, at - _QUOTE_WINDOW):at]
    cut = 0
    for mm in _SENTENCE_BREAK_RE.finditer(window):
        cut = mm.end()
    return window[cut:]


def section_of(text: str, line_no: int) -> str:
    """The `## ` heading a line sits under, or "BANNER" above the first one."""
    heading = "BANNER"
    for idx, line in enumerate(text.splitlines(), start=1):
        if idx > line_no:
            break
        if line.startswith("## "):
            heading = line.strip()
    return heading


def current_banner_end(text: str) -> int:
    """The line at which the CURRENT banner stops, i.e. where the superseded ones begin.

    THE BANNER REGION IS A STACK OF BANNERS, NEWEST FIRST, and treating all of it as one live
    block is a `section_of` artefact rather than a reading of the document. `global.md` has no
    heading until `## BUILT` on line 625, so `section_of` calls its first 624 lines "BANNER" --
    but those lines are eleven successive `_Last run` blocks, each of which was the banner on
    its day and each of which was true then. `check_banner_date` already reads only the FIRST
    `_Last run` as the current one; this is the same rule applied to the figures underneath it.
    Measured 2026-08-22: `suite is now **104/104**` on line 550, written 2026-08-14 inside the
    fourth-oldest banner, was collected as the file's live claim and superseded the real one on
    line 41 -- because the existing narrative rule keeps the LAST banner figure, which is right
    within one `_Last run` sentence (it accretes "ALSO ..." segments in order) and exactly
    backwards across a stack that grows upward. Both rules now apply, each to its own scope.
    """
    hits = [i for i, line in enumerate(text.splitlines(), start=1) if _BANNER_DATE_RE.search(line)]
    return hits[1] if len(hits) > 1 else sys.maxsize


def in_live_section(text: str, line_no: int) -> bool:
    sec = section_of(text, line_no)
    if sec == "BANNER":
        return line_no < current_banner_end(text)
    return sec in _LIVE_SECTIONS


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


def read_macro_census(source_dir: str = SOURCE_DIR) -> tuple[int | None, str, float | None]:
    """Count clauses BY MACRO, never by acceptance-ID name -- an ID grep once undercounted 8 as 5.

    Also returns the NEWEST mtime among files that actually define a test macro -- this is the
    "which run" instrument: a report older than this timestamp describes a tree the test source
    has since moved past, independent of whether its count happens to still match.
    """
    if not os.path.isdir(source_dir):
        return None, f"no Source/ at {source_dir} -- macro census skipped", None
    simple = 0
    complex_ = 0
    newest_mtime: float | None = None
    for root, _dirs, files in os.walk(source_dir):
        for name in files:
            if not name.endswith(".cpp"):
                continue
            path = os.path.join(root, name)
            try:
                with io.open(path, encoding="utf-8", errors="replace") as fh:
                    body = fh.read()
            except OSError:                                   # pragma: no cover
                continue
            s = len(re.findall(r"IMPLEMENT_SIMPLE_AUTOMATION_TEST", body))
            c = len(re.findall(r"IMPLEMENT_COMPLEX_AUTOMATION_TEST(?:_CLASS)?", body))
            if s or c:
                try:
                    mtime = os.path.getmtime(path)
                    if newest_mtime is None or mtime > newest_mtime:
                        newest_mtime = mtime
                except OSError:                               # pragma: no cover
                    pass
            simple += s
            complex_ += c
    note = f"macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST={simple}, COMPLEX/_CLASS={complex_}"
    if complex_:
        note += " -- COMPLEX macros exist, so the SIMPLE sum alone is not the suite size"
        return None, note, newest_mtime
    return simple, note, newest_mtime


_REPORT_CREATED_ON_RE = re.compile(r"^(\d{4})\.(\d{2})\.(\d{2})-(\d{2})\.(\d{2})\.(\d{2})$")


def parse_report_created_on(raw: "str | None") -> "datetime.datetime | None":
    """UE's `reportCreatedOn` shape: `2026.08.21-15.17.35`. None if missing or unparseable."""
    if not raw:
        return None
    m = _REPORT_CREATED_ON_RE.match(raw.strip())
    if not m:
        return None
    y, mo, d, h, mi, s = (int(g) for g in m.groups())
    try:
        return datetime.datetime(y, mo, d, h, mi, s)
    except ValueError:
        return None


def read_report_timestamps(report_path: str = REPORT_JSON) -> tuple[float | None, "datetime.datetime | None", str | None, str]:
    """The report's OWN write time (filesystem mtime) and its self-declared `reportCreatedOn`.

    Deliberately two separate reads: the mtime is measured off the artifact by this script and
    cannot be faked by the report's own content; `reportCreatedOn` is what the report claims
    about itself. Comparing the mtime against the newest test-source mtime is what proves
    staleness; `reportCreatedOn` is the human-readable identity quoted in the finding.
    Returns (mtime, parsed reportCreatedOn, raw reportCreatedOn string, note).
    """
    if not os.path.exists(report_path):
        return None, None, None, f"no automation report at {report_path} -- identity not established"
    try:
        mtime = os.path.getmtime(report_path)
    except OSError as exc:                                   # pragma: no cover
        return None, None, None, f"cannot stat {report_path} ({exc}) -- identity not established"
    try:
        with io.open(report_path, encoding="utf-8-sig") as fh:
            data = json.load(fh)
    except Exception as exc:                                  # pragma: no cover
        return mtime, None, None, f"{report_path} unreadable ({exc}) -- reportCreatedOn not read"
    raw = data.get("reportCreatedOn")
    created = parse_report_created_on(raw)
    note = f"report identity: mtime={mtime:.0f}, reportCreatedOn={raw!r}"
    if created is None:
        note += " -- UNPARSEABLE, cannot confirm identity from the field"
    return mtime, created, raw, note


# ---------------------------------------------------------------------------
# The three checks
# ---------------------------------------------------------------------------
def _collect_suite_claims(label: str, text: str, result: SweepResult) -> None:
    """Every suite claim in ONE file, with this script's live/stamped call on each."""
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
            if in_code_span(para, m.start()) is not None:
                continue
            # AN EXPLICIT LIVE TENSE OUTRANKS THE EXCLUSION, and the ordering was the other
            # way round until 2026-08-22. "is now" pressed against the figure is a deliberate
            # assertion by the maintainer about the tree as it stands; a `reads` or a `claim`
            # a hundred characters upstream is a sentence about something else. Letting the
            # stray word win made the exclusion silently outrank the claim it was meant to
            # spare. The exclusion still applies to every figure that does NOT assert itself,
            # and a code span above outranks both -- a verbatim quote is nobody's assertion.
            before = para[max(0, m.start() - 40):m.start()]
            asserts_live = _PRECEDING_LIVE_RE.search(before) is not None
            if not asserts_live and _QUOTED_FIGURE_RE.search(quoting_window(para, m.start())):
                continue
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
            result.suite_claims.append(SuiteClaim(label, line_no, n, d, live, snippet))

    # THE BANNER IS A CHRONOLOGICAL NARRATIVE, NOT ONE SENTENCE. It accretes
    # "ALSO POST-MILESTONE ..." segments in order, each of which said "suite is now N/N"
    # truthfully on its day, so it legitimately reads 104 -> 106 -> 107 -> 108 top to
    # bottom. Only its LAST figure is the current claim; the earlier ones are superseded
    # by position, the same way the document supersedes prose by appending. Anything
    # outside the banner keeps every live claim, so a stale NEXT bullet is still caught --
    # and a banner whose LAST figure disagrees with NEXT or with the tree, which is
    # exactly 2026-08-19's defect, still fails.
    # SUPERSESSION IS SCOPED TO THE FILE THAT OWNS THE BANNER. Before the split there was one
    # document and "the banner" was unambiguous; now `global.md` carries it and the other five
    # files have no banner at all. Comparing against `result.suite_claims` globally here would
    # let a claim in `tests.md` be superseded by `global.md`'s banner -- silently laundering the
    # exact cross-file drift the ownership check below exists to catch.
    banner_live = [c for c in result.suite_claims
                   if c.doc == label and c.live and section_of(text, c.line_no) == "BANNER"]
    superseded = {id(c) for c in banner_live[:-1]}
    for c in result.suite_claims:
        if id(c) in superseded:
            c.live = False
    if superseded:
        result.notes.append(
            f"{label}: banner narrative: {len(superseded)} earlier 'is now' figure(s) superseded "
            f"by the banner's own last one -- only the newest banner claim is treated as current")


def check_suite_counts(docs: list[tuple[str, str]], result: SweepResult) -> None:
    """Collect per file, then compare LIVE claims ACROSS the set.

    CONTRADICTIONS CHANGED ADDRESS WHEN THE RECORD WAS SPLIT; THEY DID NOT DISAPPEAR. The whole
    job of this check before 2026-08-20 was catching a banner that contradicted a claim 425 lines
    below it IN THE SAME FILE. Split into six files and that same contradiction sits BETWEEN two
    files, where a per-file sweep run six times cannot see it -- each file is internally coherent
    and every run prints clean. So the collection is per file and the comparison is global.
    """
    for label, text in docs:
        _collect_suite_claims(label, text, result)

    live = [c for c in result.suite_claims if c.live]
    distinct = sorted({c.numerator for c in live})
    if len(distinct) > 1:
        where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in live)
        result.findings.append(Finding(
            "SUITE COUNT AGREEMENT",
            f"live suite claims disagree with each other: {distinct} -- {where}",
        ))

    truth = result.report_count if result.report_count is not None else result.macro_count
    if truth is not None and live:
        wrong = [c for c in live if c.numerator != truth]
        if wrong:
            where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in wrong)
            result.findings.append(Finding(
                "SUITE COUNT AGREEMENT",
                f"live suite claim(s) disagree with the tree ({truth}): {where}",
            ))


def check_record_ownership(result: SweepResult) -> None:
    """Condition 3 of the split: exactly ONE file may carry a live suite count.

    If `global.md` and `tests.md` may both say "the suite is 108/108" then one day they will
    say different numbers, and the sweep's own agreement check would report a contradiction
    without saying which file is allowed to be right. Making ownership mechanical rather than
    a convention is the difference between a rule and a habit. STAMPED figures are untouched:
    every file is free to quote history, and this record's whole convention is that an older
    claim stays, stamped, rather than being deleted.
    """
    trespass = [c for c in result.suite_claims if c.live and c.doc != GLOBAL_DOC]
    if trespass:
        where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in trespass)
        result.findings.append(Finding(
            "RECORD OWNERSHIP",
            f"only {GLOBAL_DOC} may carry a LIVE suite count, but one appears in: {where} -- "
            f"link to the figure in {GLOBAL_DOC} instead of restating it, or stamp it as history",
        ))


def check_live_count_present(result: SweepResult) -> None:
    """A sweep of the owner file that finds NO live suite count has checked nothing.

    This is the same shape as the empty-record refusal in `run_sweep`, at the other end of the
    pipe. `check_suite_counts` compares live claims; with zero of them there is nothing to
    compare, and every comparison below passes vacuously. Yet `global.md`'s banner always
    carries the current figure -- that is what makes it the owner file -- so zero live claims
    means either the banner lost its count or something upstream of the comparison swallowed
    it. Measured 2026-08-22: the quoted-figure exclusion swallowed the real banner's own
    `160/160` at 132 of 766 plant sites in `## NEXT`, and each time the script printed
    `SWEEP CLEAN`, exit 0, having compared nothing. A guard that cannot find its subject has
    to say so; silence is the one answer it must never give.
    """
    if GLOBAL_DOC not in result.docs:
        return                                   # not sweeping the owner file; nothing to say
    if any(c.live and c.doc == GLOBAL_DOC for c in result.suite_claims):
        return
    quoted = [c for c in result.suite_claims if c.doc == GLOBAL_DOC]
    result.findings.append(Finding(
        "LIVE COUNT MISSING",
        f"{GLOBAL_DOC} carries no LIVE suite count ({len(quoted)} claim(s) there, all read as "
        f"stamped or quoted), so every suite comparison in this run passed with nothing to "
        f"compare. Either the banner has lost its figure, or the wording around it -- a "
        f"reporting verb such as 'reads', 'claim' or 'defect' in the figure's own sentence -- "
        f"is hiding it from this sweep. Run with --explain and reword, do not ignore this",
    ))


def check_item_states(docs: list[tuple[str, str]], result: SweepResult) -> None:
    """Item verdicts, compared across the SET for the reason `check_suite_counts` explains."""
    for item in TRACKED_ITEMS:
        open_hits: list[tuple[str, str, tuple[str, int]]] = []
        closed_hits: list[tuple[str, str, tuple[str, int]]] = []
        paras = [(label, start, para)
                 for label, text in docs for start, para in paragraphs_with_lines(text)]
        for label, start, para in paras:
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
                    bucket.append((f"{label}:{line_no}", snippet, (label, start)))
        # A CONTRADICTION IS DISAGREEMENT BETWEEN SITES, NOT NUANCE WITHIN ONE.
        # A single bullet saying "discharged on two of three axes, the third still
        # open" is COHERENT and must not be flagged; two separate unstamped bullets,
        # one calling the item open and the other closed, is the actual defect this
        # sweep exists for. Learned by running the first version against the real
        # file, where it flagged the content-independence bullet for being precise.
        # A "place" is now (file, paragraph), so the nuance exemption still protects a single
        # bullet that is precise about two axes, while a disagreement BETWEEN two files -- the
        # shape the split newly makes possible -- is a contradiction like any other.
        open_paras = {h[2] for h in open_hits}
        closed_paras = {h[2] for h in closed_hits}
        if open_paras - closed_paras and closed_paras - open_paras:
            o = sorted(h[0] for h in open_hits if h[2] not in closed_paras)
            c = sorted(h[0] for h in closed_hits if h[2] not in open_paras)
            result.findings.append(Finding(
                "ITEM STATE AGREEMENT",
                f"'{item.name}' is called open at {o} and closed at {c} "
                f"by unstamped text in DIFFERENT places -- one of them is wrong",
            ))


def check_report_identity(result: SweepResult) -> None:
    """Pin `Saved/AutomationReport/index.json` to a point in time before trusting its count.

    2026-08-21: the banner said 140/140, the report said 140/140, and the sweep printed SWEEP
    CLEAN -- but the report was written from a PRE-MERGE tree, and the count agreed only because
    the merge did not happen to move it. Comparing counts alone cannot tell a report that still
    describes the tree from one that no longer does; only a timestamp can. So before the report
    is trusted as ground truth (`result.report_count is not None`), this requires:

      (a) a readable `reportCreatedOn` -- proof the sweep can even NAME which run it read, and
      (b) the report's own mtime not predating any test-defining `.cpp` file on disk.

    Either gap is a hard FAIL, never a warning: an unidentifiable run is not verified evidence,
    whatever the numbers say. Both are measured off the filesystem by this script, not read from
    the report's own claims about itself -- a report cannot vouch for its own freshness.
    """
    if result.report_count is None:
        return  # the report was never trusted as ground truth; nothing to pin
    if result.report_created_on is None:
        result.findings.append(Finding(
            "REPORT IDENTITY",
            f"{REPORT_JSON} has no readable `reportCreatedOn` "
            f"({'raw value ' + repr(result.report_created_on_raw) if result.report_created_on_raw else 'field missing'}) "
            f"-- the sweep was about to trust its {result.report_count}/{result.report_count} as "
            f"ground truth without being able to name which run produced it, so it refuses "
            f"instead of trusting an unidentified report",
        ))
        return
    if result.report_mtime is None:
        result.findings.append(Finding(
            "REPORT IDENTITY",
            f"{REPORT_JSON} could not be stat'd for its own write time -- staleness cannot be "
            f"measured, so the report is refused rather than assumed current",
        ))
        return
    if result.newest_test_mtime is None:
        result.findings.append(Finding(
            "REPORT IDENTITY",
            f"no test-defining .cpp file was found under {SOURCE_DIR} to compare the report's "
            f"write time against -- staleness cannot be measured, so the report is refused "
            f"rather than assumed current",
        ))
        return
    if result.newest_test_mtime > result.report_mtime:
        report_str = datetime.datetime.fromtimestamp(result.report_mtime).strftime("%Y-%m-%d %H:%M:%S")
        source_str = datetime.datetime.fromtimestamp(result.newest_test_mtime).strftime("%Y-%m-%d %H:%M:%S")
        result.findings.append(Finding(
            "REPORT IDENTITY",
            f"{REPORT_JSON} (reportCreatedOn {result.report_created_on_raw}, written {report_str}) "
            f"predates a test-defining source file modified {source_str} -- this report is "
            f"evidence about a PAST tree, not the current one, whatever its count happens to "
            f"say. Re-run the suite before trusting it as ground truth for a live claim.",
        ))


def check_banner_date(docs: list[tuple[str, str]], result: SweepResult) -> None:
    """The banner's `_Last run` must not be older than the newest date ANYWHERE in the set.

    THE FAILURE THIS NOW CATCHES IS THE ONE THE SPLIT CREATED. One banner, in `global.md`,
    dates the whole record; the five owner files carry entries but no banner. A lane that
    writes a 2026-08-25 entry into `engine.md` while `global.md` still says `_Last run
    2026-08-20` has made the banner stale from another file, which is invisible to any
    per-file check. So the banner is found wherever it lives and compared against every
    date in every file.
    """
    # THE BANNER IS THE ONE IN `global.md`, NOT THE FIRST `_Last run` IN THE SET, and taking
    # the first cost a false finding the moment the split landed: `data.md`'s entry about
    # building this very sweep QUOTES a `_Last run 2026-08-14` line as evidence, `content.md`
    # sorts after it, and the check reported the record stale against a string that was never
    # a banner. Ownership decides -- the same rule `check_record_ownership` enforces for
    # suite counts -- so a quoted banner anywhere else is just prose.
    banner = None
    banner_doc = ""
    by_label = dict(docs)
    if GLOBAL_DOC in by_label:
        m = _BANNER_DATE_RE.search(by_label[GLOBAL_DOC])
        if m:
            banner = tuple(int(g) for g in m.groups())
            banner_doc = GLOBAL_DOC
    if banner is None:
        # NOT A QUIET SKIP. Before the split this could only mean a hand-written fixture; now
        # it can also mean the banner was moved out of `global.md`, which would leave the whole
        # set undated and this check permanently inert -- so it is a note the render prints.
        result.findings.append(Finding(
            "BANNER DATE FRESHNESS",
            f"no `_Last run YYYY-MM-DD` banner in {GLOBAL_DOC} -- nothing dates this record, "
            f"so its freshness cannot be checked at all. A skipped check reads as a passed "
            f"one, which is how a guard goes quiet; this refuses instead.",
        ))
        return
    newest = banner
    newest_doc = banner_doc
    for label, text in docs:
        for d in _ANY_DATE_RE.finditer(text):
            cand = tuple(int(g) for g in d.groups())
            if cand > newest:
                newest, newest_doc = cand, label
    if newest > banner:
        result.findings.append(Finding(
            "BANNER DATE FRESHNESS",
            "%s says _Last run %04d-%02d-%02d but %s carries entries dated %04d-%02d-%02d"
            % ((banner_doc,) + banner + (newest_doc,) + newest),
        ))


# ---------------------------------------------------------------------------
def run_sweep(paths: "str | list[str] | None" = None, *, check_tree: bool = True,
              report_path: str = REPORT_JSON, source_dir: str = SOURCE_DIR) -> SweepResult:
    """Sweep the record. `paths` defaults to whatever `state/` contains, DERIVED not typed.

    A single path is still accepted -- the `--self-test` fixtures and any ad-hoc check of one
    file rely on it -- and is labelled `global.md` so that a lone fixture is treated as the
    file that owns the banner and the suite count.

    `report_path` / `source_dir` default to the real tree's artifacts, and are overridable so
    the REPORT IDENTITY check can be proven against a doctored copy in a scratch directory
    without touching `Saved/` or `Source/` in this repo -- neither is this steward's lane.
    """
    result = SweepResult()

    if paths is None:
        paths, why = discover_state_files()
        if not paths:
            # AN EMPTY SET IS THE INERT-GUARD FAILURE, NOT A CLEAN RUN. The predecessor of this
            # function took one hardcoded path; if the record ever moved, it read a file that
            # was not there. Refusing loudly is the only answer that cannot be mistaken for a pass.
            result.findings.append(Finding(
                "NOTHING SWEPT",
                f"{why} -- this sweep checked nothing at all. That is a broken guard, not a "
                f"clean record. Point STATE_DIR at the record or restore the directory.",
            ))
            return result
    elif isinstance(paths, str):
        paths = [paths]

    docs: list[tuple[str, str]] = []
    for p in paths:
        label = os.path.basename(p)
        if len(paths) == 1:
            label = GLOBAL_DOC
        with io.open(p, encoding="utf-8") as fh:
            docs.append((label, fh.read()))
    result.docs = [d[0] for d in docs]

    if check_tree:
        result.report_count, note = read_report_count(report_path)
        result.notes.append(note)
        result.macro_count, note, result.newest_test_mtime = read_macro_census(source_dir)
        result.notes.append(note)
        result.report_mtime, result.report_created_on, result.report_created_on_raw, note = \
            read_report_timestamps(report_path)
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

    check_suite_counts(docs, result)
    check_live_count_present(result)
    check_record_ownership(result)
    check_item_states(docs, result)
    check_banner_date(docs, result)
    if check_tree:
        check_report_identity(result)
    return result


def render(result: SweepResult, explain: bool = False) -> str:
    out: list[str] = ["Banner sweep: " + (", ".join(result.docs) if result.docs else "NOTHING")]
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
        for c in sorted(result.suite_claims, key=lambda c: (c.doc, c.line_no)):
            out.append(f"    {c.doc:>12}:{c.line_no:<5} {'LIVE  ' if c.live else 'stamped'}  "
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

_Last run 2026-08-19 (the content-independence half is NOT and is carried forward;
suite is now **108/108**.)_

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


# A SUITE CLAIM THAT ENDS A SENTENCE. The trailing lookahead used to exclude `.`, so BOTH
# figures below were invisible and this document passed while contradicting itself. The blind
# spot survived from the sweep's first commit to 2026-08-20 because all 20 real claims in
# `state.md` happened to be followed by `*`, a space, or a comma -- the guard had never been
# tested against the most natural way to end the sentence. Both figures are bare and
# sentence-final on purpose: with the old regex this case PASSED.
_SENTENCE_FINAL = """# Architect state

_Last run 2026-08-19 (the suite is now 108/108.)_

## NEXT

- **A later bullet.** The suite is now 107/107.
"""

# A REPORTING VERB IN THE SENTENCE BEFORE, WHICH USED TO SILENCE THE FIGURE COMPLETELY.
# The quoted-figure exclusion looked 120 raw characters upstream, so `reads` in one sentence
# swallowed the live claim in the next -- not called stamped, never collected at all, and the
# script printed SWEEP CLEAN. Measured 2026-08-22 at 132 of 766 plant sites in the real
# `global.md`. Both wordings below are ones this record actually uses, and the bullet is a
# whole sentence away from the claim it was silencing.
_VERB_PREVIOUS_SENTENCE = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A later bullet.** The proxy's own Reason string describes a read it never performed, so
  the restart it recommends would have fixed nothing. The suite is now 107/107 on this tree.
"""


# ... and the same shape with the verb pressed right against the figure, in its OWN sentence,
# which must still be exempt. This is the pair that proves the scoping rather than a blanket
# loosening: one wording fails, its near-twin passes.
_VERB_SAME_SENTENCE = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A defect recorded honestly.** The banner as it stood that morning reads 107/107, which is
  the figure this pass corrected.
"""


# A VERBATIM QUOTE OF THE BANNER, BACKTICKS AND ASTERISKS AND ALL. This record quotes its own
# banner text when describing what the banner said, and the figure inside that code span is
# not a claim about anything. The exclusion used to test only the single character before the
# figure, which is an asterisk here, not a backtick; giving the live tense precedence over the
# reporting-verb exclusion then collected this line as a SECOND live claim. The quoted number
# below deliberately DISAGREES with the banner, so reading it as live fails this fixture.
_VERBATIM_QUOTE = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A correction, with the wording that caused it.** The banner that morning ended
  `THE SUITE **IS NOW 107/107**` and the figure was never compared to anything.
"""


# ZERO LIVE CLAIMS IN THE OWNER FILE IS THE SILENT-PASS SHAPE ITSELF. Every suite comparison
# in this document passes vacuously -- there is nothing to compare -- and before 2026-08-22
# that printed SWEEP CLEAN, exit 0. The banner here has a figure, correctly stamped as the
# previous pass's, and simply never states the current one.
_NO_LIVE_COUNT = """# Architect state

_Last run 2026-08-19 (suite was **107/107** at that pass, and the corpus is DISCHARGED.)_

## NEXT

- **A later bullet.** Nothing outstanding.
"""


# THE BANNER REGION IS A STACK OF BANNERS, NEWEST FIRST -- the real `global.md` carries eleven
# `_Last run` blocks above its first heading, each true on its day. The pair below is the
# falsifiability control for that rule in both directions: the older banner's figure must not
# be read as current, and the CURRENT banner's figure must still be checked. If the rule ever
# widens to "any banner figure is history", the second fixture goes green and says so.
_BANNER_STACK = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**; the corpus is DISCHARGED.)_

_Last run 2026-08-14 (a post-milestone note, not a phase: suite is now **104/104** after the
grep-contract tightening landed.)_

## NEXT

- **A later bullet.** Suite **108/108**.
"""


_BANNER_STACK_STALE = """# Architect state

_Last run 2026-08-19 (suite is now **107/107**; the corpus is DISCHARGED.)_

_Last run 2026-08-14 (a post-milestone note, not a phase: suite is now **108/108** after the
grep-contract tightening landed.)_

## NEXT

- **A later bullet.** Suite **108/108**.
"""


# THE SPLIT'S OWN NEW FAILURE MODE: two files, each internally coherent, contradicting each
# other. A per-file sweep run six times prints clean six times.
_SPLIT_DISAGREE = {
    "global.md": """# global

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **Nothing else here.**
""",
    "tests.md": """# tests

## NEXT

- **A clause bullet.** The suite is now 107/107.
""",
}

# CONDITION 3. `tests.md` restating a figure `global.md` owns is a finding even when the two
# AGREE -- because agreeing today is exactly how two copies come to disagree tomorrow.
_SPLIT_TRESPASS = {
    "global.md": """# global

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **Nothing else here.**
""",
    "tests.md": """# tests

## NEXT

- **A clause bullet.** The suite is now 108/108.
""",
}

# ... while QUOTING history in an owner file stays legal, because that is this record's whole
# convention: an older claim stays, stamped, rather than being deleted.
_SPLIT_STAMPED_OK = {
    "global.md": """# global

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **Nothing else here.**
""",
    "tests.md": """# tests

## NEXT

- **A clause bullet.** Suite was **107/107** at that pass
  (`reportCreatedOn 2026.08.14-21.47.35`). **[STAMPED 2026-08-19 -- superseded.]**
""",
}

# An item called open in one file and closed in another. Same defect as before the split,
# at its new address.
#
# WORDED "still open" AND NOT "not discharged", AND THE DIFFERENCE MATTERS TO THIS FIXTURE.
# "not discharged" contains the word this item's CLOSED shape matches, so a paragraph saying
# it hits both shapes and is exempted as in-bullet nuance. That exemption is correct -- a
# single bullet being precise about two axes is not a contradiction -- but a fixture written
# that way proves nothing about the cross-file comparison it exists to test, and passed
# silently while doing so. Measured twice while writing this. The same trap applies to any
# prose added INSIDE these fixture strings: an explanation mentioning the closed word puts it
# back in the paragraph, so explanations live out here in the comment.
_SPLIT_ITEM_CLASH = {
    "global.md": """# global

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **DISCHARGED 2026-08-19 -- the content-independence corpus.** Done.
""",
    "data.md": """# data

## NEXT

- **The content-independence corpus is still open** and is carried forward.
""",
}

# A banner made stale from ANOTHER file -- invisible to any per-file check.
_SPLIT_STALE_BANNER = {
    "global.md": """# global

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **Nothing else here.**
""",
    "engine.md": """# engine

## NEXT

- **A bullet written on 2026-08-25.** Landed then.
""",
}


def _write_fixture_report(path: str, count: int, created_on: str, mtime: float) -> None:
    tests = [{"testDisplayName": f"T{i}", "state": "Success"} for i in range(count)]
    data = {"reportCreatedOn": created_on, "succeeded": count, "succeededWithWarnings": 0,
            "failed": 0, "notRun": 0, "tests": tests}
    with io.open(path, "w", encoding="utf-8") as fh:
        json.dump(data, fh)
    os.utime(path, (mtime, mtime))


def _write_fixture_source(path: str, count: int, mtime: float) -> None:
    body = "\n".join(f'IMPLEMENT_SIMPLE_AUTOMATION_TEST(T{i}, "x", 0)' for i in range(count))
    with io.open(path, "w", encoding="utf-8") as fh:
        fh.write(body)
    os.utime(path, (mtime, mtime))


def check_identity_self_test() -> tuple[bool, list[str]]:
    """REPORT IDENTITY fixtures. Built on a doctored SCRATCH report/source pair, never on the
    real `Saved/` or `Source/` -- those are outside this steward's lane -- so `--report-json` /
    `--source-dir` exist specifically to make this provable without touching them.
    """
    import tempfile
    import time

    lines: list[str] = []
    ok = True
    base = time.time() - 100000   # comfortably in the past, clear of "now" jitter

    def run_case(name: str, report_kwargs: "dict | None", source_kwargs: "dict | None",
                 claim_count: int, want_pass: bool) -> None:
        nonlocal ok
        with tempfile.TemporaryDirectory() as d:
            report_path = os.path.join(d, "index.json")
            source_dir = os.path.join(d, "Source")
            os.makedirs(source_dir)
            state_dir = os.path.join(d, "state")
            os.makedirs(state_dir)
            if report_kwargs is not None:
                _write_fixture_report(report_path, claim_count, **report_kwargs)
            if source_kwargs is not None:
                _write_fixture_source(os.path.join(source_dir, "T.cpp"), claim_count, **source_kwargs)
            gpath = os.path.join(state_dir, "global.md")
            with io.open(gpath, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(f"# global\n\n_Last run 2026-08-21 "
                         f"(suite is now **{claim_count}/{claim_count}**.)_\n\n"
                         f"## NEXT\n\n- **Nothing else.**\n")
            res = run_sweep(gpath, check_tree=True, report_path=report_path, source_dir=source_dir)
        got_pass = res.passed
        good = got_pass == want_pass
        ok = ok and good
        detail = "" if res.passed else " -- " + "; ".join(f.check for f in res.findings)
        lines.append(f"    [{'OK' if good else '**WRONG**'}] {name}: "
                     f"expected {'PASS' if want_pass else 'FAIL'}, "
                     f"got {'PASS' if got_pass else 'FAIL'}{detail}")

    run_case("report written AFTER the source PASSES -- identity confirmed, evidence current",
              dict(created_on="2026.08.21-15.00.00", mtime=base + 200),
              dict(mtime=base + 100), 5, True)
    run_case("report written BEFORE a later source edit FAILS -- the real 2026-08-21 shape "
              "(count matched by luck; the report was pre-merge)",
              dict(created_on="2026.08.21-11.00.00", mtime=base + 100),
              dict(mtime=base + 200), 5, False)
    run_case("report with no reportCreatedOn FAILS -- the sweep cannot NAME the run it read",
              dict(created_on="", mtime=base + 200),
              dict(mtime=base + 100), 5, False)
    run_case("no test-defining source found FAILS -- staleness cannot be measured at all",
              dict(created_on="2026.08.21-15.00.00", mtime=base + 200),
              None, 5, False)
    return ok, lines


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
        # 2026-08-22: the quoted-figure exclusion reached a whole sentence upstream and
        # dropped the claim before it was ever collected, the tense markers could not reach
        # across a bold opener, and the banner region was read as one live block rather than
        # as the stack of superseded banners it is. These six are those repairs.
        ("a reporting verb in the PREVIOUS sentence does not silence a wrong live figure",
         _VERB_PREVIOUS_SENTENCE, False),
        ("a reporting verb in the figure's OWN sentence still exempts it", _VERB_SAME_SENTENCE, True),
        ("a banner quoted VERBATIM inside a code span is not a live claim", _VERBATIM_QUOTE, True),
        ("an owner file with NO live suite count at all FAILS", _NO_LIVE_COUNT, False),
        ("a SUPERSEDED banner's own older figure PASSES", _BANNER_STACK, True),
        ("a stale figure in the CURRENT banner still FAILS with older banners below it",
         _BANNER_STACK_STALE, False),
        ("banner 107 vs bullet 108 FAILS", _BAD_SUITE, False),
        ("item called open and closed FAILS", _BAD_ITEM, False),
        ("banner older than its entries FAILS", _BAD_DATE, False),
        ("chooseBuild ruled and awaiting a ruling FAILS", _BAD_CHOOSEBUILD, False),
        # 2026-08-20, the split. The first proves the sentence-final regex fix; the rest prove
        # the sweep compares across the SET rather than file by file.
        ("a suite claim ending a SENTENCE is seen, and still FAILS when wrong",
         _SENTENCE_FINAL, False),
        ("two files disagreeing about the suite FAILS", _SPLIT_DISAGREE, False),
        ("an owner file restating a figure global.md owns FAILS", _SPLIT_TRESPASS, False),
        ("an owner file QUOTING a stamped older figure PASSES", _SPLIT_STAMPED_OK, True),
        ("an item called open in one file and closed in another FAILS", _SPLIT_ITEM_CLASH, False),
        ("a banner made stale by an entry in ANOTHER file FAILS", _SPLIT_STALE_BANNER, False),
    ]
    lines: list[str] = []
    ok = True
    for name, body, want_pass in cases:
        with tempfile.TemporaryDirectory() as d:
            # A fixture is either ONE document (labelled global.md, so a lone file is treated
            # as the one that owns the banner and the count) or a whole SET keyed by filename.
            if isinstance(body, dict):
                p = []
                for fname, text in sorted(body.items()):
                    fp = os.path.join(d, fname)
                    with io.open(fp, "w", encoding="utf-8", newline="\n") as fh:
                        fh.write(text)
                    p.append(fp)
            else:
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

    # THE FILE LIST IS DERIVED, AND THAT IS ITSELF WORTH A FIXTURE. `discover_state_files` is
    # the thing standing between this sweep and the "checker types its own subject list" shape,
    # so it is checked here rather than assumed: a NEW file dropped into the directory must be
    # picked up without anyone editing this script, and an EMPTY directory must refuse.
    with tempfile.TemporaryDirectory() as d:
        for fname in ("global.md", "tests.md", "ux.md"):
            with io.open(os.path.join(d, fname), "w", encoding="utf-8", newline="\n") as fh:
                fh.write("# x\n\n## NEXT\n\n- **Nothing.**\n")
        with io.open(os.path.join(d, "notes.txt"), "w", encoding="utf-8", newline="\n") as fh:
            fh.write("not markdown, not swept\n")
        found, why = discover_state_files(d)
        got = sorted(os.path.basename(f) for f in found)
        want = ["global.md", "tests.md", "ux.md"]
        good = (got == want)
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] a NEW .md in the directory is swept "
                     f"without editing this script: expected {want}, got {got}")

    with tempfile.TemporaryDirectory() as d:
        found, why = discover_state_files(os.path.join(d, "does-not-exist"))
        res = run_sweep([], check_tree=False) if found else None
        empty_found, _ = discover_state_files(d)
        good = (found == [] and empty_found == [] and why.startswith("NO RECORD DIRECTORY"))
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] a MISSING or EMPTY record directory "
                     f"is reported, not silently swept as clean")

    identity_ok, identity_lines = check_identity_self_test()
    ok = ok and identity_ok
    lines.append("    -- REPORT IDENTITY (pins the artifact to a point in time) --")
    lines.extend(identity_lines)

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
    parser.add_argument("state_path", nargs="*", default=None,
                        help=f"Record file(s) to sweep. Default: every *.md in {STATE_DIR}, "
                             f"DERIVED from the directory rather than named here.")
    parser.add_argument("--explain", action="store_true",
                        help="Print every suite claim with this script's LIVE/STAMPED call on it.")
    parser.add_argument("--no-tree", action="store_true",
                        help="Check the document against itself only; skip the report and macro census.")
    parser.add_argument("--report-json", default=REPORT_JSON,
                        help="Override the automation report path (for testing REPORT IDENTITY "
                             "against a scratch copy without touching Saved/).")
    parser.add_argument("--source-dir", default=SOURCE_DIR,
                        help="Override the Source/ tree used for the macro census and the "
                             "REPORT IDENTITY staleness check.")
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

    result = run_sweep(args.state_path or None, check_tree=not args.no_tree,
                       report_path=args.report_json, source_dir=args.source_dir)
    print(render(result, explain=args.explain))
    return 0 if result.passed else 1


if __name__ == "__main__":
    sys.exit(main())
