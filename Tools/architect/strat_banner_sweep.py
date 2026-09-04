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

  5. REPORT PROVENANCE. Check 4 pins WHICH RUN the trusted report is; it says nothing about
     an entry's own PROSE CLAIM about which run or which tree produced a figure. 78ea508 fixed
     a `global.md` entry that cited `reportCreatedOn 2026.08.26-00.28.42` as certifying the
     live figure while, unstamped, it also said the figure was produced in
     `E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen` and that the suite
     "must be RE-RUN in the integration tree" -- a suite that had already run. So, WITHIN
     `global.md` only -- the one file condition 3 of the split already makes the owner of every
     live fact about the current run -- (a) every unstamped, unquoted `reportCreatedOn <stamp>`
     cited there must equal `result.report_created_on_raw`, the stamp of the report this sweep
     actually opened; and (b) an unstamped, unquoted sentence saying a figure or report was
     PRODUCED in a named filesystem path on a named branch must name THIS tree -- `_REPO` and
     `git rev-parse --abbrev-ref HEAD`, both derived from the running process, never typed.

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

CHECK 5's OWN BLIND SPOT IS THE SAME SHAPE, MEASURED RATHER THAN SUPPOSED. Part (b) only fires
on the COMBINED phrase "produced ... `<path>` on branch `<branch>`" -- both halves, in one
regex, together. Measured 2026-08-26 with three fixtures sharing one "produced" qualifier: one
naming both a wrong path AND a wrong branch (control), one naming ONLY the wrong path with no
"on branch" clause at all, one naming ONLY a wrong branch with no path. The control FAILED with
one REPORT PROVENANCE finding, exactly as designed; BOTH single-sided fixtures returned
`SWEEP CLEAN`, exit 0, zero REPORT PROVENANCE findings -- a path-only or branch-only provenance
claim is invisible to this check by construction, the same way check 1 cannot see a figure that
never says what it is counting. It is also blind to a tree/branch pair that IS this one but for
the wrong reason (a coincidental rename), and to any provenance claim not phrased with the word
"produced".

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
import subprocess
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

# REPORT PROVENANCE PART (b)'S OWN MARKER SET -- GENUINE SUPERSESSION ONLY, NOT A NEARBY
# CITATION. `_PARAGRAPH_STAMP_MARKERS` above includes a bare `reportCreatedOn <date>` because
# for a SUITE FIGURE (part (a)'s subject, and every other check's), a report citation sitting
# near the figure is exactly what marks it historical -- "suite was 107/107 (`reportCreatedOn
# ...`)" is the record's own idiom for quoting an old pass. A tree/branch PROVENANCE CLAIM is a
# different subject: the live banner's own `reportCreatedOn` citation for TODAY'S report sits a
# short, unremarkable distance from its own "run in `<repo>` on branch `<branch>`" sentence --
# that is simply how the real banner is written, not a supersession -- so reusing the same
# marker made `is_stamped` exempt a bad tree/branch claim merely for standing near ANY
# `reportCreatedOn`, current or not. Measured 2026-08-26: replacing the real banner's own
# `run in \`E:/MultiAgent/Stratocracy\` on branch \`master\`` with a foreign lane path and the
# word "produced" -- otherwise touching nothing -- still printed `SWEEP CLEAN`, exit 0, because
# that edit landed 91 characters from the banner's own `reportCreatedOn` citation
# (match.start()-to-match.start(), `_CITED_REPORT_STAMP_RE` to `_PROVENANCE_PATH_BRANCH_RE`) --
# FAR inside `_STAMP_WINDOW` (220), not a near miss, so the bare-citation marker reached it with
# more than a hundred characters to spare. This set drops that marker: only an explicit
# supersession annotation stamps a tree/branch claim, never a bare date citation sitting nearby.
_PROVENANCE_TREE_STAMP_MARKERS: tuple[str, ...] = (
    r"\[STAMPED",
    r"\[BANNER CORRECTED",
    r"\*\*CORRECTED",
    r"\*\*CORRECTION",
    r"CORRECTED,",
    r"CORRECTION,",
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

# A LITERAL SPACE IN A MULTI-WORD PHRASE IS ONLY SAFE WHEN A MISS FALLS THROUGH TO SOMETHING
# STRICT. `stood at` and `had been` above, and `is now` in `_PRECEDING_LIVE_RE`, each carry a
# literal space that a line wrap defeats exactly like `on branch` once did (measured
# 2026-08-26: `is now` wrapped as `is\nnow` still fails to match, and so do `stood\nat` and
# `had\nbeen`) -- but a miss here just falls through to the OTHER regex or to the tense
# default, and that default is LIVE, the strict reading. So a wrapped `was **107/107**`
# degrades to being read as a live claim, never the reverse: over-strict, never a missed
# finding. `_BANNER_DATE_RE`'s `_Last run ` is the same shape again, and a miss there is
# refused outright (`run_sweep` raises rather than assumes a document with no discoverable
# banner date is clean). `_PROVENANCE_PATH_BRANCH_RE`'s `on branch` was the ONE regex in this
# file where a miss meant SKIP THIS CLAIM ENTIRELY -- part (b) never collected the sentence,
# so a real live provenance claim wrapped there was invisible rather than merely misread. That
# is what made it a functional defect and not a conservative edge. Before adding a literal
# space to a new regex here, name which of the two failure modes a miss produces.

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

# CHECK 5 PART (a)'S OWN WINDOW, ADDED 2026-08-26 to replace a whole-paragraph scope that went
# inert on the live banner the same day it grew a fourth `reportCreatedOn` citation. See
# `check_report_provenance`'s docstring and part (a)'s own call site for the measurement:
# `78ea508`'s two citations sit 234 characters apart start-to-start (needs >=234, missed by
# `_STAMP_WINDOW`'s 220 by 14) and the live 2026-08-26 banner's `[STAMPED ...]`-bracketed
# re-quote of an old stamp sits 371 characters from that bracket, start-to-start (needs >=371).
# 400 covers both with margin. NOT EVERY LEGITIMATE CITATION IN THAT SAME LIVE BANNER FITS THIS
# WINDOW: one `reportCreatedOn 2026.08.26-03.40.42` re-quote there sits ~3,738 characters from
# the nearest `[STAMPED ...]`/`CORRECTION,` marker, exempted only by prose ("that citation is
# now history by position, superseded by this segment") with no bracket nearby -- a window wide
# enough to reach it would be wide enough to reopen the whole-paragraph blind spot this fix
# exists to close. That citation is a known, reported gap under this window, not a silently
# accepted one -- see the steward's report dated 2026-08-26 for the measurement and the
# recommendation that it is a scope call for the record's own maintainer, not this script.
_PROVENANCE_CITATION_WINDOW = 400

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
# `cite`/`cited`/`cites`/`citing`/`citation`/`citations` ADDED 2026-08-26, when part (a) of
# REPORT PROVENANCE started using this same list via `quoting_window` and immediately found the
# gap: a `reportCreatedOn` citation is part (a)'s own subject, so "cited" is the single most
# likely reporting verb to appear next to one -- more likely than any word already in this list,
# which was built for suite figures -- and its absence was invisible until something finally
# searched for it. Measured on the real live `global.md`: "the banner segment below cited
# `reportCreatedOn 2026.08.26-03.40.42` while the report on disk had moved to `-15.21.53`" is an
# honest account of a past drift, structurally identical to `_QUOTED_ACCOUNT` below, and without
# this addition it was flagged as a live provenance defect.
#
# PROVED LOAD-BEARING BY BOTH SIDES OF A DIFFERENTIAL ON THE REAL SENTENCE, not merely added and
# assumed -- and the differential landed the OPPOSITE way from a first, cruder measurement, which
# is the part worth keeping. A raw 220-character look-back finds an unrelated "reading" (from
# "every wrong conclusion drawn today came from reading the verdict") 187 characters upstream of
# the citation, in a SEPARATE sentence -- and reading `quoting_window`'s OWN scope, not that raw
# distance, is what this check actually uses: it stops at the nearest sentence boundary, and that
# boundary sits between "reading"'s sentence and "cited"'s. Two scratch mutations of the real
# banner prove which one is load-bearing: with "reading" replaced by a neutral word and "cited"
# left untouched, the citation is STILL exempted (`SWEEP CLEAN`) -- isolating "cited" as
# sufficient on its own. With "cited" replaced by a neutral word and "reading" left untouched,
# the SAME citation is NO LONGER exempted (`SWEEP FAILED`, `REPORT PROVENANCE`) -- "reading"
# never reached it, because `quoting_window`'s sentence cut had already excluded it before this
# addition existed. So there is no hidden looseness here to flag: the sentence-scoped window is
# stricter than a raw character count would suggest, and "cited" is the whole reason this
# citation is exempted, not a coincidence of "reading" sitting nearby. `check_self_test` pins the
# same real sentence failing with the four new words removed, so the addition itself is provably
# load-bearing independent of this differential.
#
# THE FIRST SHIPPED PATTERN NAMED FIVE WORD-FORMS ITS OWN REGEX COULD NOT ALL REACH, found in
# review the same day. `\bcite\w*` requires an `e` immediately after `cit` before the `\w*`
# takes over, so it matches `cite`, `cited` and `cites` but NOT `citing` or `citation(s)` -- there
# is no `e` in either. Measured live at the time, against the THEN-shipped `\bcite\w*`
# -- long since replaced, twice; see the two paragraphs below, and do not call any literal
# spelling "the shipped pattern" again. The sentence "The suite count
# in that citation gives 999/999 on this tree." is COLLECTED as a live claim and FAILS the sweep
# (`SUITE COUNT AGREEMENT`, 225 vs 999); the same sentence with "cited" in place of "citation" is
# exempted, `SWEEP CLEAN`. Widened here to reach the whole family the comment always promised,
# rather than narrowing the comment to the forms the old pattern actually reached -- `citation`
# is at least as natural a way to report a stale figure as `cited` is ("that citation is now
# history" is this record's own idiom; `global.md` had already written it), so under-exempting
# it is a live false-positive risk, not a safe default.
#
# THE FIRST WIDENING SHIPPED AS `\bcit\w*`, AN OPEN WORD CLASS, AND THAT WAS ITSELF A DEFECT:
# it also matches any OTHER word starting `cit-` (`cities`, `citizen`, `citadel`, `citizenry`,
# ...), so it went on to falsify its own justifying comment the same day it shipped -- the
# comment claimed the record's vocabulary contained no such word, and a later paragraph
# documenting exactly this risk then wrote `cities` and `citizen` into `global.md`, which a
# claim about a growing document cannot survive. Tightened here to the closed alternation
# `\bcit(?:e|es|ed|ing|ation|ations)\b`: this is a fact about the PATTERN, not about the
# record's current text, so it cannot be falsified by anyone editing `global.md` -- the six
# forms are the whole of the `cite`/`citation` family this exemption is for, and the pattern
# has no `\w*` tail left to reach past a word boundary into `cities`, `citizen`, `citadel` or
# `citizenry`. Measured, not assumed: `\bcite\w*` vs `\bcit(?:e|es|ed|ing|ation|ations)\b` give
# byte-identical output on the full sweep (`--explain --no-tree`) run against the real
# `Tools/architect/state/*.md` files -- because every actual `cit`-initial word in that record is
# one of the six forms below the alternation reaches, so tightening away from the open class
# costs nothing on this tree today. If that ever changes, the differential above is the one to
# re-run -- but the comment no longer depends on it staying true.
_QUOTED_FIGURE_RE = re.compile(
    r"said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|"
    r"wrongly|stale|defect|would\s+have|reported|\bcit(?:e|es|ed|ing|ation|ations)\b", re.I)

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
# REPORT PROVENANCE (check 5). Two independent sub-checks, both born from the SAME commit:
# 78ea508 fixed a `global.md` entry that named THIS tree's report as certifying the live
# figure while, eight lines below, an unstamped paragraph asserted the opposite -- that the
# figure was the LANE tree's, produced in `E:/MultiAgent/Strat-wt/slot-1` on branch
# `feat/match-result-screen`, and that the suite "must be RE-RUN in the integration tree after
# the merge." That suite had already run. Check 4 (REPORT IDENTITY) grades the `N/N` figure and
# the artifact's own timestamp; it has nothing that reads an entry's PROSE claim about which
# run or which tree produced it. A stale provenance sentence was invisible to this sweep by
# construction until now.
#
# THE TOKEN IS OPTIONAL, ADDED 2026-09-04 -- A BARE STAMP WAS INVISIBLE TO PART (a) BY
# CONSTRUCTION. The pre-fix pattern required the literal `reportCreatedOn` token immediately
# before the digits, so a stamp written on its own -- `2026.09.04-04.09.26`, no token, exactly
# the shape a maintainer writes when the sentence around it already says "the report" or "this
# pass" -- was never collected as a citation at all, and REPORT PROVENANCE part (a) had no
# opinion on it whatever the report on disk said. That is how a stale bare citation survived a
# `SWEEP CLEAN` on 2026-09-04 and needed a human-dispatched `strat-integration-reviewer` gate
# instead. `(?:reportCreatedOn\s+)?` makes the token optional rather than adding a second,
# separate bare-stamp pattern: a token-prefixed occurrence is consumed as ONE match (the
# optional group is greedy and the token, when present, is always immediately followed by the
# digits in this record's own idiom), so `re.finditer` never also reports the same digits a
# second time as a bare match starting mid-way through an already-consumed span -- confirmed
# directly (not assumed) in `check_self_test`'s regex-differential block below, which asserts
# `_CITED_REPORT_STAMP_RE.findall(...)` returns exactly one hit on a token-prefixed stamp, not
# two. `\s+` already tolerated a line wrap between the token and the stamp; that is unchanged,
# and this record wraps at ~95 columns, so a wrapped citation like this still matches.
#
# `_PARAGRAPH_STAMP_MARKERS` (the SUITE-FIGURE stamp set) IS DELIBERATELY LEFT UNCHANGED, NOT
# WIDENED TO MATCH A BARE STAMP TOO. That marker set exists so a suite figure sitting near a
# report citation reads as historical, and its scope is "does ANY recognisable stamp-shaped
# thing sit near this claim" -- widening it to a bare `\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2}`
# would make it match any timestamp-shaped string in this record's prose, including ones that
# are not report citations at all (a filename, a quoted log line, a scratch-file stamp used in a
# fixture description). That is a real risk specifically for check 1 (SUITE COUNT AGREEMENT),
# which is not the check this defect was reported against: widening the marker there would trade
# a narrow, reported false-negative for a broader, unreported false-negative risk on a different
# check. `_CITED_REPORT_STAMP_RE`, by contrast, is used only by REPORT PROVENANCE part (a),
# whose entire job is comparing report-identity stamps against each other -- there a bare stamp
# genuinely is the same claim as a token-prefixed one, so widening THAT pattern costs nothing.
# So: this regex changes; `_PARAGRAPH_STAMP_MARKERS` does not.
_CITED_REPORT_STAMP_RE = re.compile(
    r"(?:reportCreatedOn\s+)?(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2})")

# `[A-Za-z]:[\\/]` is this project's own path shape (`E:/...`), matched rather than typed --
# the DRIVE LETTER is never fixed to `E`, so the check does not silently assume its own box.
#
# THE LITERAL SPACE IN `on branch` WAS THE HOLE. This record's own prose wraps at ~95
# columns, and `_GOOD_PROVENANCE` (below) wraps exactly between those two words -- "on\nbranch"
# -- so the literal single space never matched and this regex found ZERO matches in that
# fixture. Part (b) never saw the sentence at all: not exempted, not flagged, never reached.
# `_GOOD_PROVENANCE` was therefore a VACUOUS pass for part (b), proved 2026-08-26 by an
# over-fire build (`markers=()`, honouring no stamp): the shipped fixture still PASSED under
# over-fire, where the same text with the wrap removed correctly FAILED. `\s*` already
# tolerates any run of whitespace around the backticks; the fix makes the space BETWEEN
# "on" and "branch" the same kind of tolerant gap, `\s+`, so a wrap between those two words no
# longer hides a live claim from this check.
_PROVENANCE_PATH_BRANCH_RE = re.compile(
    r"`([A-Za-z]:[\\/][^`]+)`\s*on\s+branch\s*`([^`]+)`", re.I)

# THE SHAPE MUST SAY SOMETHING WAS *PRODUCED* THERE, NOT MERELY MENTION A WORKTREE. Without
# this qualifier the check flagged `engine.md`'s own honest build note -- "Built in worktree
# `E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen`, off `194de95`" -- which
# is a true, permanent statement of WHERE THE C++ WAS WRITTEN and is not a claim about which
# tree backs a live figure or report at all. Measured 2026-08-26 against the real
# `Tools/architect/state/engine.md`: without the qualifier this fired there; with it, it does
# not, because "Built in worktree" never says anything was PRODUCED.
_PROVENANCE_PRODUCED_RE = re.compile(r"\bproduced\b", re.I)
_PROVENANCE_QUALIFIER_WINDOW = 150


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
    # THE REPORT'S OWN SUCCEEDED COUNT, READ EVEN WHEN THE REPORT IS RED. Added 2026-09-01
    # alongside the `_collect_suite_claims` fix that lets an honest non-green figure (`346/347`)
    # become a live claim at all -- see `read_report_count` and `check_suite_counts` for why a
    # numerator now has its own ground truth, separate from `report_count` (the total).
    report_succeeded: int | None = None
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


def is_stamped(paragraph: str, markers: tuple[str, ...] = _PARAGRAPH_STAMP_MARKERS) -> bool:
    return any(re.search(m, paragraph) for m in markers)


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
    block is a `section_of` artefact rather than a reading of the document. `global.md` carries
    no heading at all until `## BUILT`, so `section_of` calls every line above that heading
    "BANNER" -- but that span is a run of successive `_Last run` blocks, each of which was the
    banner on its day and each of which was true then. `check_banner_date` already reads only
    the FIRST `_Last run` as the current one; this is the same rule applied to the figures
    underneath it.

    ANCHORED BY CONTENT, NOT BY LINE NUMBER, AND THAT IS A REPAIR RATHER THAN A STYLE CHOICE.
    This paragraph shipped in 2026-08-22 wording that put `## BUILT` "on line 625", called the
    span above it "the first 624 lines", and counted "eleven" successive `_Last run` blocks in
    it. All three describe a document that only ever grows ABOVE them, so all three were stale
    within days and none of them could be read as a pointer any more: re-measured 2026-08-26,
    `## BUILT` sat on line 1977 and the stack was 22 blocks deep. Those two figures are stamped
    with the day they were taken precisely because they will drift the same way -- what this
    docstring points WITH from here on is quoted text, which survives the growth. The dated
    measurement below keeps its own figures unchanged for the same reason: it is history.

    Measured 2026-08-22: `suite is now **104/104**`, written 2026-08-14 inside the then
    fourth-oldest banner, was collected as the file's live claim and superseded the real one in
    the then-current banner -- because the existing narrative rule keeps the LAST banner
    figure, which is right within one `_Last run` sentence (it accretes "ALSO ..." segments
    in order) and exactly backwards across a stack that grows upward. Both rules now apply,
    each to its own scope.
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
def read_report_count(path: str = REPORT_JSON) -> tuple[int | None, int | None, str]:
    """The report's TOTAL entry count and its SUCCEEDED count, read even when the report is RED.

    **2026-09-01 DEFECT AND FIX, MEASURED AGAINST THE REAL TREE, NOT SUPPOSED.** Until this
    fix, this function refused to return ANYTHING once any entry was non-Success -- the
    reasoning (recorded in `global.md`'s 2026-09-01 entry) was that a red report's count must
    never be mistaken for a trustworthy ALL-GREEN figure, which is still correct. But the old
    code threw away the TOTAL and the SUCCEEDED count along with that refusal, and those are
    separate facts from "the suite is clean": a report saying "347 entries, 346 Success, 1
    FAILED" is not lying about having 347 entries or about 346 of them succeeding merely
    because it is also red. Declining to hand back those two numbers meant `check_suite_counts`
    had NOTHING to compare a live non-green claim against, so it printed `SWEEP CLEAN` while a
    false `347/347` stood in the banner and the tree's own (red) report said 346 of 347 --
    verified by a direct call to the pre-fix function against that exact report, returning
    `(None, "... non-Success entries -- suite count not read from a red report")`.

    So: this now ALWAYS returns the total and the succeeded count when the report is loadable,
    whatever its own verdict, and the note says explicitly whether the report was red. It is
    still never used to assert the suite IS clean -- that judgement stays with the report's own
    `state` fields and with the LIVE CLAIM'S OWN wording (`346/347` reads as non-green on its
    face; nothing here relabels it). `check_suite_counts` uses the total to verify a claim's
    DENOMINATOR (works for green and non-green alike, since a green claim's numerator equals
    its denominator) and the succeeded count to verify a NON-GREEN claim's numerator
    specifically, which a macro census could never do -- it counts macros, not outcomes.
    """
    if not os.path.exists(path):
        return None, None, f"no automation report at {path} -- suite figures checked against each other only"
    try:
        # utf-8-sig: the report is UTF-8 WITH BOM. Two separate passes have
        # reported UTF-16 for this file and both were wrong.
        with io.open(path, encoding="utf-8-sig") as fh:
            data = json.load(fh)
    except Exception as exc:                                  # pragma: no cover
        return None, None, f"automation report unreadable ({exc}) -- treated as absent, not as agreement"
    tests = data.get("tests", [])
    total = len(tests)
    non_success = [t for t in tests if t.get("state") != "Success"]
    succeeded = total - len(non_success)
    if non_success:
        return (total, succeeded,
                f"automation report: {total} entries, {succeeded} Success, "
                f"{len(non_success)} non-Success -- a RED report, read for its own total and "
                f"succeeded count, never as a claim that the suite is clean "
                f"({data.get('reportCreatedOn')})")
    return total, succeeded, f"automation report: {total} entries, all Success ({data.get('reportCreatedOn')})"


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
            # AN UNEQUAL PAIR IS AN HONEST NON-GREEN SUITE FIGURE, NOT A "103 -> 104" STYLE
            # PROGRESSION -- AND THIS RECORD'S OWN PROGRESSION NOTATION CAN NEVER REACH THIS
            # CODE PATH IN THE FIRST PLACE. `_SUITE_CLAIM_RE` requires a literal "/" between
            # the two numbers; every progression this record has ever written uses an ARROW
            # ("103 -> 108", `decisions.md`, `global.md` throughout) which contains no "/" at
            # all. So the `if n != d: continue` this replaced was never once protecting against
            # the shape its own comment named -- grepped across the whole live record
            # 2026-09-01, zero `\d+/\d+` progressions exist, only arrows -- and its actual,
            # measured effect was to drop every HONEST partial figure unconditionally. Verified
            # directly against the real live `346/347` in `global.md`'s current banner
            # (2026-09-01): absent from this function's output under the old filter, present
            # under this one. See `global.md`'s own entry for the account of the false
            # `347/347` this silence let stand. `check_suite_counts` now verifies BOTH halves
            # of an unequal claim separately -- the denominator against the tree's total, the
            # numerator against the report's own succeeded count when the report is readable --
            # so removing this filter does not weaken verification; it is what MAKES
            # verification of a non-green claim possible at all.
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
    # COMPARED AS THE FULL PAIR, NOT JUST THE NUMERATOR. Before 2026-09-01 a non-green claim
    # was never collected at all, so two live claims agreeing on the numerator while disagreeing
    # on the denominator (`346/347` vs `346/348`) could not arise. Now that it can, the
    # numerator alone is not enough to catch it.
    distinct = sorted({(c.numerator, c.denominator) for c in live})
    if len(distinct) > 1:
        where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in live)
        result.findings.append(Finding(
            "SUITE COUNT AGREEMENT",
            f"live suite claims disagree with each other: {distinct} -- {where}",
        ))

    # DENOMINATOR TRUTH: how many tests exist, whether the report is green or red (see
    # `read_report_count`'s 2026-09-01 fix) or, failing that, the macro census. This is the
    # SAME comparison as before 2026-09-01 for a green claim -- n == d == truth, so checking
    # the denominator is identical to checking the numerator was -- and it is now also the
    # right comparison for a non-green claim, where the numerator is EXPECTED to differ from
    # the total.
    truth = result.report_count if result.report_count is not None else result.macro_count
    if truth is not None and live:
        wrong = [c for c in live if c.denominator != truth]
        if wrong:
            where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in wrong)
            result.findings.append(Finding(
                "SUITE COUNT AGREEMENT",
                f"live suite claim(s) disagree with the tree's total ({truth}): {where}",
            ))

    # NUMERATOR TRUTH, ONLY WHEN THE REPORT ITSELF IS READABLE. A macro census counts macros,
    # never outcomes, so it has no opinion on how many tests SUCCEEDED -- only the report does,
    # and now it is read for that even when it is red. This is the check that verifies an
    # honestly-stated `346/347` actually matches what the report says passed, rather than
    # merely matching the total.
    if result.report_succeeded is not None and live:
        wrong = [c for c in live if c.numerator != result.report_succeeded]
        if wrong:
            where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in wrong)
            result.findings.append(Finding(
                "SUITE COUNT AGREEMENT",
                f"live suite claim(s) disagree with the report's own succeeded count "
                f"({result.report_succeeded}): {where}",
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


def check_truth_available(result: SweepResult) -> None:
    """A live suite claim compared against NOTHING is not a verified claim -- it is silence.

    **2026-09-01 DEFECT AND FIX.** `check_suite_counts` compares every live claim against
    `report_count` (falling back to `macro_count`); when BOTH are `None` it has nothing to
    compare against and every comparison in that function passes VACUOUSLY -- the same
    "checked nothing, printed clean" shape `check_live_count_present` already exists to catch
    at the other end of this pipe, for the case where there is no live claim at all. This is
    the missing half: a live claim that DOES exist but that this run could not verify against
    anything is a finding, never a silent pass, however defensible the reason ground truth was
    unavailable this run (a missing report, an unreadable one, or a `Source/` tree with no
    test-defining macros to census). The read-the-report-anyway fix in `read_report_count`
    closes the most common way this used to happen -- a red report no longer refuses its own
    total and succeeded count -- but does not make ground truth unconditionally available: the
    report can still be genuinely absent or unreadable, and this is the check that refuses to
    let that combine with silence.

    Only checked when `check_tree` is True: `--no-tree` deliberately never establishes ground
    truth at all, by request, and is not this defect.
    """
    live = [c for c in result.suite_claims if c.live]
    if not live:
        return   # check_live_count_present already covers "no live claim at all"
    if result.report_count is None and result.macro_count is None:
        where = "; ".join(f"{c.doc}:{c.line_no}: {c.numerator}/{c.denominator}" for c in live)
        result.findings.append(Finding(
            "SUITE COUNT UNVERIFIABLE",
            f"live suite claim(s) exist ({where}) but neither the automation report nor the "
            f"macro census could establish a ground truth this run, so nothing above compared "
            f"anything. An unable-to-verify live claim is a finding with a non-zero exit, "
            f"never silence.",
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


def _mask_span(text: str, start: int, end: int) -> str:
    """`text` with `[start:end)` blanked to spaces, SAME LENGTH, so a stampedness test run
    against the result cannot self-match the very citation being judged.

    Every `reportCreatedOn <stamp>` occurrence IS one of `_PARAGRAPH_STAMP_MARKERS` -- that
    marker exists so a suite figure sitting NEAR a report identity reads as historical. Reused
    unmasked against a reportCreatedOn citation itself, `is_stamped` would call EVERY citation
    stamped, right or wrong, because the citation always matches its own marker. Masking only
    the matched span keeps every OTHER stamp and every other marker in the paragraph visible.
    """
    return text[:start] + (" " * (end - start)) + text[end:]


def _current_branch(repo: str = _REPO) -> "str | None":
    """`git rev-parse --abbrev-ref HEAD`, run in the tree this script is IN, never typed.

    A guard that types the branch or path it is comparing against goes inert on that exact
    subject the day the branch is renamed or the repo is cloned somewhere else -- the shape
    this project has already paid for twice. `_REPO` is likewise derived from `__file__`, not
    written here as a literal.
    """
    try:
        proc = subprocess.run(
            ["git", "-C", repo, "rev-parse", "--abbrev-ref", "HEAD"],
            capture_output=True, text=True, timeout=10,
        )
    except Exception:                                          # pragma: no cover
        return None
    if proc.returncode != 0:
        return None
    branch = proc.stdout.strip()
    return branch or None


def _normalize_fs_path(p: str) -> str:
    """Case-insensitive, slash-insensitive path comparison, for a record that writes the same
    tree as both `E:/MultiAgent/Stratocracy` and (occasionally) backslashed."""
    return os.path.normcase(os.path.normpath(p.replace("\\", "/")))


def check_report_provenance(docs: list[tuple[str, str]], result: SweepResult) -> None:
    """Check 5. Two independent parts, per the docstring's REPORT PROVENANCE section.

    (a) A CITED REPORT STAMP MUST BE THE REPORT THE SWEEP READ. Every unstamped, unquoted
    `reportCreatedOn <stamp>` in the swept set must equal `result.report_created_on_raw`.

    **[STAMPED 2026-08-26 -- THE PARAGRAPH-WIDE SCOPE BELOW WAS ITSELF A DEFECT, FOUND THE SAME
    DAY IT SHIPPED, AND THE SENTENCE THAT FOLLOWS THIS ONE IS THE FALSE PREMISE THAT CAUSED IT.
    Kept rather than deleted, because the record's own convention is that an old claim stays,
    stamped, and because the failure mode is the lesson: "the defective entry carries exactly
    ONE `reportCreatedOn` in that whole block" was TRUE of `78ea508`'s entry on the day it was
    written and became FALSE as the SAME banner paragraph grew across a single day's corrections.
    Measured against the live `global.md` banner segment on 2026-08-26: a single long paragraph,
    with no blank line inside it, carrying several `reportCreatedOn` citations and several
    `[STAMPED`/`CORRECTION,` markers -- the exact counts are not stated here because this is a
    paragraph the record keeps growing by its own stamp-in-place convention, so any count typed
    into this comment goes stale the next time someone corrects a claim in it; re-measure live
    rather than trust a figure quoted here. Falsifying the paragraph's OWN certifying citation --
    the live banner's `reportCreatedOn 2026.08.26-15.21.53` -- to any other value and sweeping
    the mutated copy printed `SWEEP CLEAN`, exit 0: `is_stamped` on the whole paragraph (even
    with the mutated span itself masked) still matched one of the other three citations, or the
    `[STAMPED`/`CORRECTION,` markers, and called the mutation stamped. The control that proves
    the check was not simply broken -- the same mutation applied to `git show
    HEAD:Tools/architect/state/global.md`, `78ea508`'s own shorter, single-citation entry, run
    through the SAME shipped script -- exited 1. So the paragraph-wide fallback did not merely
    have a narrow miss; it goes inert BY THE ACT OF THIS RECORD DOING WHAT IT ALWAYS DOES --
    stamping a correction in place rather than deleting the sentence it replaces. Every correction
    written into a growing banner paragraph ADDS a stamp marker to it, and each one widens the
    exemption `is_stamped` grants to every OTHER citation in that same paragraph, including a
    live one that has nothing to do with any of them. A guard whose scope is "the whole
    paragraph" cannot survive a record whose convention is "stamp, never delete", because the
    record is designed to grow exactly the way that defeats it. THE FIX gives part (a) a window,
    the way part (b) already has one -- see below.]**

    (b) A NAMED TREE MUST BE THIS TREE. A live sentence asserting a figure or report was
    PRODUCED in a named filesystem path on a named branch must agree with the tree this sweep
    is actually running in, derived from `_REPO` and `_current_branch()` -- never typed. Scope
    here IS the `_STAMP_WINDOW` window, not the whole paragraph, and deliberately so: unlike
    part (a), the claim being judged is never itself a `reportCreatedOn` citation, so the
    self-match problem does not apply, and the SAME giant paragraph that makes whole-paragraph
    scope safe for (a) makes it UNSAFE for (b) -- the defective entry's own wrong
    `reportCreatedOn 2026.08.26-00.28.42` sits in the identical unbroken block as its
    `produced in ... slot-1` sentence, so a whole-paragraph read would have let that ONE wrong
    stamp silently exempt the tree claim too.

    PART (b) NO LONGER READS THE SAME MARKER SET AS EVERY OTHER CHECK, AND THIS WAS ALSO
    MEASURED RATHER THAN ASSUMED. The window originally used `is_stamped` unmodified, whose
    `_PARAGRAPH_STAMP_MARKERS` includes a bare `reportCreatedOn <date>` -- correct for a SUITE
    FIGURE, where a nearby report citation is this record's own idiom for quoting an old pass,
    but wrong for a tree/branch claim: the LIVE banner's own citation of TODAY's report sits an
    unremarkable, un-superseded distance from its own "run in `<repo>` on branch `<branch>`"
    sentence, because that is simply how the real banner narrates its own current pass. Measured
    2026-08-26 by replacing the real banner's "run in `E:/MultiAgent/Stratocracy` on branch
    `master`" with a foreign lane path and the word "produced," touching nothing else: the edit
    landed 91 characters from the banner's own `reportCreatedOn` (match.start()-to-match.start(),
    `_CITED_REPORT_STAMP_RE` to `_PROVENANCE_PATH_BRANCH_RE`) -- FAR inside `_STAMP_WINDOW`
    (220), not a near miss -- and the sweep printed `SWEEP CLEAN` at exit 0 -- blind on exactly
    the ground its own subject is most likely to stand. `78ea508`'s own real defect sat OUTSIDE
    that window, at 249 characters by the same measure, so the one live defect this check was
    built to catch was flagged by MARGIN rather than by the window's design: a claim landing a
    little closer to 220 in either direction would have let the window's width decide the
    outcome, not any correct distinction between an own-current citation and a stale one. Part
    (b) now checks the window against
    `_PROVENANCE_TREE_STAMP_MARKERS`, which drops the bare `reportCreatedOn` marker and keeps
    only an explicit supersession annotation (`[STAMPED ...]`, `CORRECTION,`, and the like). The
    fixed record's own tree sentence is still exempted, because it sits beside an actual
    `[STAMPED ...]` block, not merely beside a date; the defective record's tree sentence, which
    has no such annotation, is now flagged where it previously was not. Part (a) is unchanged --
    it still reads a cited `reportCreatedOn` against the one the sweep opened, and still uses the
    full marker set, because for THAT subject a bare citation legitimately is the stamp.

    NEITHER PART USES `in_code_span`, AND THIS WAS MEASURED RATHER THAN ASSUMED. This record
    writes EVERY `reportCreatedOn` citation and every path in backticks as ordinary markdown
    code formatting -- including 78ea508's own defect, which reads `` `reportCreatedOn
    2026.08.26-00.28.42` ``. Wiring `in_code_span` into part (a), as the brief that named this
    check first specified, was tried and run against the real `d6492c9` bytes with the live
    report on disk as ground truth: EVERY citation in the document, right and wrong alike, sits
    inside backticks, so `in_code_span` treated the defect itself as a verbatim quote and
    exempted it -- the check never reached the comparison at all. Dropped from both parts, for
    the same reason: a backtick here is formatting, not a claim that the text is quoted
    history, and treating it as one would exempt nearly everything.

    BOTH PARTS ARE SCOPED TO `GLOBAL_DOC`, AND THIS TOO WAS MEASURED RATHER THAN ASSUMED FIRST.
    Run unscoped against the real `Tools/architect/state/` as it stands, this fires repeatedly --
    on ordinary dated bullets in `engine.md`, `data.md`, `tests.md` and `decisions.md` quoting
    the report THEY checked at THEIR OWN pass, which is not a claim about the tree's current
    live figure at all. (A fire count is not quoted here: it is a count of the live record, and
    the record grows, so a number typed into this comment is stale the next time an entry is
    added -- re-run the unscoped variant against the live tree if the figure itself matters.)
    `check_record_ownership` already draws this exact line for suite
    counts: only `global.md` may state a LIVE fact about the current run; every other file's
    entries are about the work that file's own agent did, not about which report backs the
    figure a reader should trust today. A `reportCreatedOn` or a build-location sentence
    outside `global.md` is evidence for a local claim, not a provenance assertion this check has
    any business grading -- so it is not graded.

    WITHIN `global.md`, BOTH PARTS ARE ALSO SCOPED TO THE CURRENT BANNER SEGMENT ONLY --
    `section_of(...) == "BANNER"` and above `current_banner_end`, NOT the blanket
    `in_live_section` every other check in this file uses. That distinction was forced by a
    real measurement, not chosen up front: `in_live_section` treats the WHOLE of `## NEXT` as
    live, on the documented ground that after the split there is nowhere else for a live claim
    to hide. That is correct for a suite figure, which this file states directly about the
    CURRENT tree in exactly one place. It is wrong for a `reportCreatedOn` citation, because
    `## NEXT` in this file is a long run of individually DATED, per-pass narrative bullets, and
    every one of them legitimately cites the report that was current WHEN IT WAS WRITTEN --
    that is simply how this record narrates history, and it is not a claim about today's
    report. Run with `in_live_section` in place of the banner-segment scope, part (a) fires on
    exactly that shape, against real dated entries weeks old -- again a count of the live
    record, not stated here for the same reason as above: it moves every time an old dated
    entry is added, so re-measure rather than trust a number left in this comment. Scoped to the
    current banner segment instead, it stops firing on those and still finds the one that matters: the CURRENT
    banner's own `reportCreatedOn` citation falling behind the report actually on disk, which
    is a live drift this record can genuinely fall into between two passes of concurrent
    editing -- see the report for the measurement that told the two apart.
    """
    # (a)
    if not result.report_created_on_raw:
        # UNCONDITIONAL NOTE, MIRRORING PART (b)'S SHAPE BELOW. `result.report_created_on_raw`
        # is None whenever no report was found, the report was unreadable, or its own
        # `reportCreatedOn` field was absent -- `read_report_timestamps`'s note already says
        # which. Without a note NAMED to this check, that silence read as "checked and clean"
        # rather than "could not check": a scratch run pointed at a nonexistent report printed
        # `SWEEP CLEAN` at exit 0 having compared no cited stamp against anything, and the
        # coordinator briefly reported that as a real result while proving this very check's
        # falsifiability. This does not change the exit code: `check_report_identity` (check 4)
        # returns early whenever `result.report_count is None`, so a MISSING report is fatal
        # NOWHERE, not merely "governed" elsewhere -- check 4 has already decided NOT fatal, and
        # this check does not get a second opinion on that.
        result.notes.append(
            "REPORT PROVENANCE part (a): no reportCreatedOn to compare against -- cited-stamp "
            "citations not checked this run")
    else:
        for label, text in docs:
            if label != GLOBAL_DOC:
                continue
            for start, para in paragraphs_with_lines(text):
                for m in _CITED_REPORT_STAMP_RE.finditer(para):
                    stamp = m.group(1)
                    if stamp == result.report_created_on_raw:
                        continue
                    line_no = start + para[:m.start()].count("\n")
                    if not (section_of(text, line_no) == "BANNER"
                            and line_no < current_banner_end(text)):
                        continue          # a dated pass's own report, not the live declaration
                    if _QUOTED_FIGURE_RE.search(quoting_window(para, m.start())):
                        continue
                    # PART (a)'S OWN WINDOW, NOT THE WHOLE PARAGRAPH -- see this function's
                    # docstring for the 2026-08-26 inertness this replaces. `_PROVENANCE_CITATION_
                    # WINDOW` (400) is sized from two measurements, not guessed: `78ea508`'s own
                    # fix needs >=234 (its two citations sit 234 characters apart start-to-start,
                    # and a 220-character reach missed the marker's own leading text by 14), and
                    # the live 2026-08-26 banner's own `[STAMPED ...]`-bracketed re-quote of
                    # `reportCreatedOn 2026.08.26-03.40.42` sits 371 characters from that bracket,
                    # start-to-start -- so 220 (this file's general `_STAMP_WINDOW`) covers
                    # neither and 400 covers both with margin. The self-match problem `_mask_span`
                    # exists for is unchanged by windowing: the slice is taken first and the
                    # citation's own span is masked WITHIN it, so a lone citation can never
                    # exempt itself by matching its own `reportCreatedOn` text.
                    ws = max(0, m.start() - _PROVENANCE_CITATION_WINDOW)
                    we = min(len(para), m.end() + _PROVENANCE_CITATION_WINDOW)
                    window = _mask_span(para[ws:we], m.start() - ws, m.end() - ws)
                    if is_stamped(window):
                        continue
                    result.findings.append(Finding(
                        "REPORT PROVENANCE",
                        f"{label}:{line_no}: cites `reportCreatedOn {stamp}`, but the report "
                        f"this sweep actually opened is `reportCreatedOn "
                        f"{result.report_created_on_raw}` -- an unstamped, unquoted citation of "
                        f"a DIFFERENT run is a stale claim about which report backs a live "
                        f"figure. Stamp it (`[STAMPED ...]` or similar), quote it as history, "
                        f"or correct it if it is meant to be current.",
                    ))

    # (b)
    branch = _current_branch()
    if branch is None:
        result.notes.append(
            f"REPORT PROVENANCE: `git rev-parse --abbrev-ref HEAD` failed at {_REPO} -- "
            f"named-tree citations not checked this run")
    else:
        for label, text in docs:
            if label != GLOBAL_DOC:
                continue
            for start, para in paragraphs_with_lines(text):
                for m in _PROVENANCE_PATH_BRANCH_RE.finditer(para):
                    qualifier = para[max(0, m.start() - _PROVENANCE_QUALIFIER_WINDOW):m.start()]
                    if not _PROVENANCE_PRODUCED_RE.search(qualifier):
                        continue  # not a claim about where a figure/report was PRODUCED
                    line_no = start + para[:m.start()].count("\n")
                    if not (section_of(text, line_no) == "BANNER"
                            and line_no < current_banner_end(text)):
                        continue          # a dated pass's own note, not the live declaration
                    if _QUOTED_FIGURE_RE.search(quoting_window(para, m.start())):
                        continue
                    window = para[max(0, m.start() - _STAMP_WINDOW):m.end() + _STAMP_WINDOW]
                    # PART (b)'S OWN MARKER SET, NOT THE GENERAL ONE -- see
                    # `_PROVENANCE_TREE_STAMP_MARKERS`'s own comment for the 2026-08-26 hole
                    # this closes: a nearby `reportCreatedOn` citation is not a supersession of
                    # a tree/branch claim, only an explicit stamp is.
                    if is_stamped(window, markers=_PROVENANCE_TREE_STAMP_MARKERS):
                        continue
                    cited_path, cited_branch = m.group(1), m.group(2).strip()
                    if (_normalize_fs_path(cited_path) == _normalize_fs_path(_REPO)
                            and cited_branch == branch):
                        continue
                    result.findings.append(Finding(
                        "REPORT PROVENANCE",
                        f"{label}:{line_no}: an unstamped, unquoted sentence says a figure or "
                        f"report was produced in `{cited_path}` on branch `{cited_branch}`, but "
                        f"this sweep is running in `{_REPO}` on branch `{branch}` -- exactly "
                        f"78ea508's defect. Stamp it or quote it if it describes a past pass, "
                        f"or correct it if it is meant to be a live claim about this tree.",
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
        result.report_count, result.report_succeeded, note = read_report_count(report_path)
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
    if check_tree:
        check_truth_available(result)
    check_record_ownership(result)
    check_item_states(docs, result)
    check_banner_date(docs, result)
    if check_tree:
        check_report_identity(result)
    # UNCONDITIONAL: part (a) self-gates on `result.report_created_on_raw` (None when
    # `check_tree` is False, so it silently has nothing to compare), and part (b) needs no
    # tree artifact at all -- only the record text and this process's own git branch.
    check_report_provenance(docs, result)
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

# REPORT PROVENANCE, part (a). BOTH PARTS ARE SCOPED TO THE CURRENT BANNER SEGMENT, NOT TO
# `## NEXT` -- a real `## NEXT` bullet is a dated per-pass narrative entry allowed to cite ITS
# OWN contemporary report, and only the banner asserts what backs today's figure -- so the
# citation under test sits INSIDE the `_Last run (...)` parenthetical, exactly where 78ea508's
# defect lived. Sentence-final comma on purpose, varying punctuation from the fixtures below.
# Exercised against a SCRATCH report by `check_provenance_self_test`, not by the plain
# `check_self_test` list, because part (a) has nothing to compare against without one.
# A BLANK LINE SPLITS THE BANNER INTO TWO PARAGRAPHS ON PURPOSE, BOTH STILL INSIDE THE BANNER
# REGION (section_of only starts a new section at "## "). The suite figure sits ALONE in its
# own paragraph so this record's OWN suite-count stamping rule -- a `reportCreatedOn` sitting
# near a figure reads that figure as historical -- cannot reach across and swallow it; the
# provenance citation under test lives in the second paragraph, on its own.
_BAD_PROVENANCE_STAMP = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

THIS PASS'S OWN CERTIFYING REPORT IS NAMED HERE, UNSTAMPED: `reportCreatedOn
2026.08.26-00.28.42`, and the figure above rests on it.

## NEXT

- **Nothing else here.**
"""

# The identical stale stamp, this time inside a `[STAMPED ...]` supersession block alongside
# the correct one -- the record's own convention that an old claim is kept, not deleted. Also
# carries the tree/branch half of part (b) in the same paragraph, and ends on a bare full stop
# right after the word "history", the sentence-final shape this sweep has been blind to
# before. Must NOT be flagged on either count.
_GOOD_PROVENANCE = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

THE REPORT THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn
2026.08.26-01.30.10`. **[STAMPED 2026-08-26 -- the lane tree's own figure, superseded by the
merge.]** It read: the certifying figure was produced in `E:/MultiAgent/Strat-wt/slot-1` on
branch `feat/match-result-screen`, and the earlier pass's own report was `reportCreatedOn
2026.08.26-00.28.42`, which is now history.

## NEXT

- **Nothing else here.**
"""

# THE 2026-09-04 BARE-STAMP DEFECT ITSELF, PINNED AS A FIXTURE. Identical shape to
# `_BAD_PROVENANCE_STAMP` above, EXCEPT the citation carries no `reportCreatedOn` token at all --
# just the bare digits, unstamped, unquoted, differing from the scratch report's own
# `2026.08.26-01.30.10`. Before the 2026-09-04 fix this was invisible to part (a) by
# construction (`_CITED_REPORT_STAMP_RE` required the literal token) and the sweep printed
# `SWEEP CLEAN` on it; the fix makes the token optional, so this must now FAIL exactly as the
# token-prefixed `_BAD_PROVENANCE_STAMP` does.
_BAD_PROVENANCE_BARE_STAMP = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

THIS PASS'S OWN CERTIFYING REPORT IS NAMED HERE, UNSTAMPED AND WITH NO TOKEN AT ALL:
2026.08.26-00.28.42, and the figure above rests on it.

## NEXT

- **Nothing else here.**
"""

# THE NEGATIVE CONTROL FOR THE SAME SHAPE: a BARE stamp that AGREES with the report this sweep
# actually opened (`2026.08.26-01.30.10`, the same scratch report `_BAD_PROVENANCE_BARE_STAMP`
# is checked against). No token, no stamp marker, no reporting verb -- it PASSES purely because
# the digits match, proving the broadened regex does not turn every bare timestamp into a
# finding, only a bare timestamp that actually disagrees with the report on disk.
_GOOD_PROVENANCE_BARE_STAMP = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

THIS PASS'S OWN CERTIFYING REPORT IS NAMED HERE, UNSTAMPED AND WITH NO TOKEN AT ALL:
2026.08.26-01.30.10, and the figure above rests on it.

## NEXT

- **Nothing else here.**
"""

# THE 2026-08-26 PARAGRAPH-WIDE-INERTNESS DEFECT ITSELF, PINNED AS A FIXTURE. A single unbroken
# paragraph -- no blank line -- carrying an unstamped, WRONG `reportCreatedOn` citation far from
# every stamp marker in it (over 400 characters of plain filler prose on both sides, carrying no
# reporting verb and no bracket), plus TWO genuine `[STAMPED ...]`/`CORRECTION,` markers
# elsewhere in that SAME paragraph, about something else entirely. Under the pre-fix, whole-
# paragraph scope (`is_stamped` over the entire paragraph with only the citation's own span
# masked) the two unrelated markers exempt the bad citation anyway, because the scope was never
# "near this claim", it was "anywhere in this block" -- exactly the shape that went inert on the
# live 2026-08-26 banner as it accreted stamps across a single day. Under the shipped windowed
# scope (`_PROVENANCE_CITATION_WINDOW`, 400) neither marker is reachable from the citation, so
# this correctly FAILS. `check_self_test` below pins BOTH directions: the shipped build fails on
# this text, and the pre-fix `is_stamped`-over-the-whole-paragraph call, run directly against the
# same paragraph without reconstructing a whole second build, would have passed it.
_BAD_PROVENANCE_LONG_PARAGRAPH = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

**[STAMPED 2026-08-26 -- an entirely unrelated earlier correction about the pairing corpus,
kept here for history and touching nothing about which report backs today's figure.]** This
entry narrates an ordinary pass of work on the record, listing which files changed and why, in
plain prose that names no bracketed marker of its own, simply carrying the account forward one
sentence after another so that a later reader can follow the day without consulting anything
else. This entry narrates an ordinary pass of work on the record, listing which files changed
and why, in plain prose that names no bracketed marker of its own, simply carrying the account
forward one sentence after another so that a later reader can follow the day without consulting
anything else. THE CERTIFYING REPORT FOR TODAY'S FIGURE CARRIES `reportCreatedOn
2026.08.26-00.28.42`, standing here with no stamp of its own and nothing marking it superseded.
This entry narrates an ordinary pass of work on the record, listing which files changed and why,
in plain prose that names no bracketed marker of its own, simply carrying the account forward
one sentence after another so that a later reader can follow the day without consulting
anything else. This entry narrates an ordinary pass of work on the record, listing which files
changed and why, in plain prose that names no bracketed marker of its own, simply carrying the
account forward one sentence after another so that a later reader can follow the day without
consulting anything else. **CORRECTION, 2026-08-26: a second, equally unrelated correction
about the scoreboard widget, kept here for history and touching nothing about which report
backs today's figure.**

## NEXT

- **Nothing else here.**
"""

# THE EXEMPTION ITSELF, PINNED AS A FIXTURE. An honest account of a past drift -- unstamped,
# unbracketed -- reporting that an EARLIER segment cited a since-superseded stamp, using "cited"
# and nothing else to mark it as an account rather than an assertion. Structurally the real
# `global.md` sentence opening `THE CHECK CAUGHT REAL DRIFT ON ITS FIRST DAY`, condensed --
# quoted rather than cited by line, because that file grows above the sentence and the line
# number this comment used to give (`global.md:76`) had already moved by one when it was read
# back. Without the `cite`-family addition to `_QUOTED_FIGURE_RE`
# this was flagged as a live provenance defect; with it, `check_self_test` below confirms it
# PASSES, and a direct regex differential -- the shipped pattern against a reconstruction of the
# one before this addition, on the identical `quoting_window` text -- proves the addition is
# what makes the difference, not a coincidence of some other word in this fixture.
_CITED_ACCOUNT_PROVENANCE = """# global

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

An earlier segment of this banner cited `reportCreatedOn 2026.08.26-00.28.42` before the suite
was re-run mid-pass; that citation is now history and no live claim rests on it.

## NEXT

- **Nothing else here.**
"""

# REPORT PROVENANCE, part (b). Unstamped, unquoted, and ends the sentence with a bare full
# stop directly against the closing backtick -- no space, no comma -- the exact spot a trailing
# lookahead has missed a figure before. `_REPO`/the branch are NEVER typed here as the
# "expected" value; the check derives them itself, so this fixture only has to be WRONG, which
# `E:/MultiAgent/Strat-wt/slot-1` on `feat/match-result-screen` always is on this box.
_BAD_PROVENANCE_TREE = """# Architect state

_Last run 2026-08-26 (suite is now **224/224**, every entry Success, zero failed and zero
notRun.)_

THE CERTIFYING FIGURE WAS PRODUCED IN `E:/MultiAgent/Strat-wt/slot-1` ON BRANCH
`feat/match-result-screen`, unstamped.

## NEXT

- **Nothing else here.**
"""

# THE HOLE ITSELF, MEASURED 2026-08-26 ON A SCRATCH COPY OF THE REAL BANNER: a bad tree/branch
# claim standing near a LEGITIMATE, current `reportCreatedOn` citation for the pass it is
# genuinely part of -- no `[STAMPED ...]`, no `CORRECTION,`, nothing marking either sentence as
# superseded. Before the marker split, `_PARAGRAPH_STAMP_MARKERS`'s bare `reportCreatedOn
# \d{4}\.\d{2}\.\d{2}` matched the citation and `is_stamped(window)` called the WHOLE window
# stamped, laundering the bad tree/branch claim sitting inside it; `SWEEP CLEAN`, exit 0. Written
# to read like real banner prose rather than test prose -- a suite figure, a macro-census
# cross-check, an ordinary comma-joined sentence -- and ending the tree/branch clause on a bare
# sentence-final full stop, the shape this sweep has been blind to before on a different check.
_BAD_PROVENANCE_NEAR_CITATION = """# global

_Last run 2026-08-26 (THE SUITE HOLDS AT **225/225**, every entry Success, zero failed and zero
notRun, unmoved since the prior pass.)_

THE REPORT BEHIND THIS FIGURE CARRIES `reportCreatedOn 2026.08.26-15.21.53`, matching the macro
census on this box exactly, and it was produced in `E:/MultiAgent/Strat-wt/slot-1` on branch
`feat/match-result-screen`.

## NEXT

- **Nothing else here.**
"""

# THE CLEAN COUNTERPART, PROVING THE REPAIR DOES NOT SIMPLY INVERT THE BUG: the SAME shape --
# a genuine `reportCreatedOn` citation standing an unremarkable distance from a tree/branch
# sentence -- but the tree/branch sentence names THIS tree and branch, derived by the check
# itself rather than typed here, so it is correct on whichever box runs the self-test. A
# provenance sentence beside a live citation must still PASS when it is simply true.
_GOOD_PROVENANCE_NEAR_CITATION = """# global

_Last run 2026-08-26 (THE SUITE HOLDS AT **225/225**, every entry Success, zero failed and zero
notRun, unmoved since the prior pass.)_

THE REPORT BEHIND THIS FIGURE CARRIES `reportCreatedOn 2026.08.26-15.21.53`, matching the macro
census on this box exactly, and it was produced in `{repo}` on branch `{branch}`.

## NEXT

- **Nothing else here.**
""".format(repo=_REPO.replace("\\", "/"), branch=_current_branch() or "master")

# A SECOND [STAMPED]-ONLY FIXTURE, ADDED 2026-08-26 AFTER THE REVIEWER'S FUNCTIONAL-DEFECT
# FINDING: nothing in this suite pinned that `_PROVENANCE_TREE_STAMP_MARKERS` still honours a
# genuine `[STAMPED ...]` annotation, because the two existing PASS fixtures both pass for a
# DIFFERENT reason -- `_GOOD_PROVENANCE_NEAR_CITATION` is true on the tree/branch facts
# themselves, and the pre-existing `_GOOD_PROVENANCE` passed VACUOUSLY: its own "on\nbranch"
# wrap put the tree/branch sentence entirely outside `_PROVENANCE_PATH_BRANCH_RE`'s reach (fixed
# above with `\s+`), so part (b) never even collected that sentence, let alone reached the
# stamp check on it. (An earlier version of this comment credited the pass to the
# quoted-figure exclusion tripping on `_GOOD_PROVENANCE`'s own `"It read:"` -- verified false:
# `_QUOTED_FIGURE_RE` alternates on `read ` with a trailing space and cannot match `read:`.)
# Proved with an over-fire build passing `markers=()` into part (b) -- honouring
# NO stamp at all -- under which `_GOOD_PROVENANCE` still passed while this fixture correctly
# flipped to FAIL. **[STAMPED 2026-08-26: THAT DIFFERENTIAL DESCRIBES THE TREE BEFORE THE
# `on\s+branch` WRAP FIX AND IS NO LONGER THE SHIPPED BUILD'S BEHAVIOUR. Re-run against the
# wrap-tolerant regex: under the SAME over-fire build, `_GOOD_PROVENANCE` and this fixture BOTH
# FAIL. `_GOOD_PROVENANCE` staying green was the symptom of the vacuity the wrap fix closed --
# closing it necessarily changed that reading, so `_GOOD_PROVENANCE` now genuinely pins the
# stamp itself and is no longer distinguished from this fixture by an over-fire probe. This
# fixture is kept anyway: it is still the only PASS fixture stamped with no reporting verb and
# no "It read:" anywhere near it, which the differential above never established.]**
# This one carries a WRONG tree/branch claim, a real `**[STAMPED ...]**` block,
# and deliberately no reporting verb and no `"It read:"` anywhere near it, so the stamp is the
# ONLY thing standing between it and a finding. Written to read like ordinary banner prose --
# a suite figure, a macro-census cross-check, an explanatory stamp -- ending on a bare
# sentence-final full stop after "that merge", the punctuation this sweep has been blind to
# before on a different check.
_GOOD_PROVENANCE_TREE_STAMP_ONLY = """# global

_Last run 2026-08-26 (THE SUITE HOLDS AT **225/225**, every entry Success, zero failed and zero
notRun, unmoved since the prior pass.)_

THE REPORT BEHIND THIS FIGURE CARRIES `reportCreatedOn 2026.08.26-15.21.53`, matching the macro
census on this box exactly. **[STAMPED 2026-08-26 -- the certifying number below belonged to the
lane tree before the merge carried it into this one.]** The figure was produced in
`E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen`, ahead of that merge.

## NEXT

- **Nothing else here.**
"""


# THE WRAP-VACUITY HOLE ITSELF, AS A FIXTURE: a WRONG tree/branch claim, unstamped, with no
# reporting verb and no marker anywhere near it -- exactly `_BAD_PROVENANCE_TREE`'s shape --
# except this one wraps its "on branch" straight down the middle, "on\nbranch", the way this
# record's own ~95-column prose wraps `_GOOD_PROVENANCE` above. Before the `\s+` fix this
# defeated `_PROVENANCE_PATH_BRANCH_RE` outright: zero matches, so part (b) never saw the
# sentence and the sweep PASSED it -- not because a stamp spared it, because the check never
# ran on it at all. After the fix the regex reaches across the line break and this correctly
# FAILS on the shipped build -- that direction is asserted mechanically, below, by the case
# `"an unstamped WRONG tree/branch claim wrapped ... FAILS"`. The OTHER direction -- that this
# same text PASSED on the pre-fix build, because the check never even reached it -- is not a
# regression this file can run (nothing here reconstructs a second build), so it is a recorded
# measurement instead: see `global.md`'s 2026-08-26 entry ("0 matches before, 1 after"). What
# `check_self_test` DOES assert mechanically, immediately after the case list below, is the
# narrower and more durable claim: the shipped `\s+`-tolerant regex matches this wrapped
# sentence and the literal `on branch` pattern this project shipped before the fix does not,
# on the identical text -- a real regression against anyone who reintroduces the literal,
# without needing to stand up a whole pre-fix build to prove it.
_BAD_PROVENANCE_TREE_WRAPPED = """# global

_Last run 2026-08-26 (suite is now **225/225**, every entry Success, zero failed and zero
notRun.)_

THE CERTIFYING FIGURE WAS PRODUCED IN `E:/MultiAgent/Strat-wt/slot-1` ON
BRANCH `feat/match-result-screen`, unstamped.

## NEXT

- **Nothing else here.**
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
# BOTH FIGURES MUST BE EXEMPTED BY THE QUALIFIER GAP ALONE, NOT BY A REPORTING VERB SITTING
# NEARBY BY COINCIDENCE. The first-shipped filler read "the control corpus reads 68/68 the same
# way" -- "reads" is one of `_QUOTED_FIGURE_RE`'s own reporting verbs, so `68/68` was exempted
# for TWO independent reasons at once and the fixture never isolated which one was doing the
# work. Measured 2026-08-26: neither `42/42` nor `68/68` ever reaches the reporting-verb check at
# all -- both fail `_SUITE_QUALIFIER_RE` first (`suite|succeeded|tests` is not within 90
# characters of either figure), so this coincidence was inert, not a live defect, in this
# fixture's own shape. Reworded so the filler carries no reporting verb: this pins the qualifier
# exclusion in isolation, and it stays green under both `\bcite\w*` and `\bcit\w*`.
_PAIRING_FIGURES = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **The corpus graded clean.** 42 applied attacks against 42 resolved, 42/42 with zero
  mismatches; the control corpus stands at 68/68 the same way.
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

# THE GATING GAP: `citing` AND `citation` HAVE NO `e` AFTER `cit`, SO `\bcite\w*` NEVER REACHED
# THEM. Deliberately avoids every OTHER word already in `_QUOTED_FIGURE_RE`
# ("said"/"says"/"saying"/"read "/"reads "/"reading"/"claim"/"shape"/"editing"/"planting"/
# "planted"/"wrongly"/"stale"/"defect"/"would have"/"reported") so this fixture cannot pass for
# any reason but the widened `cit` family, and carries no "is now"/"was" tense marker and no
# explicit stamp, so nothing else exempts the wrong 107/107. Under the pre-widen `\bcite\w*` this
# is a genuine gap: "citing" never matches, the 107/107 is collected as a LIVE claim, and the
# sweep FAILS against the real banner's 108/108. Under the SHIPPED `_QUOTED_FIGURE_RE`
# -- named by its object rather than by a re-typed literal, because this comment has already
# outlived two spellings of that pattern -- the same sentence is recognised as an honest
# account and PASSES.
_CITING_ACCOUNT = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A note.** A citing of the earlier suite figure recorded it at 107/107, a count already
  superseded by the total above.
"""

# THE SAME GAP, THE OTHER MISSING FORM. "citation" has no `e` after `cit` either, and this
# fixture is the noun form where `_CITING_ACCOUNT` above is the gerund -- both word-shapes the
# comment already promised and the pre-widen pattern could not reach. Same exclusion discipline:
# no other reporting word from the list, no tense marker, no explicit stamp.
_CITATION_ACCOUNT = """# Architect state

_Last run 2026-08-19 (suite is now **108/108**.)_

## NEXT

- **A note.** That citation of the earlier suite figure noted it at 107/107, a count already
  superseded by the total above.
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


def _write_fixture_report_partial(path: str, total: int, succeeded: int, created_on: str,
                                   mtime: float) -> None:
    """A RED report -- `succeeded` of `total` Success, the rest Fail. For the 2026-09-01
    non-green-figure fixtures, which need a report that genuinely disagrees with itself the
    way the real 2026-09-01 report does, not merely a green one truncated."""
    tests = [{"testDisplayName": f"T{i}", "state": "Success" if i < succeeded else "Fail"}
              for i in range(total)]
    data = {"reportCreatedOn": created_on, "succeeded": succeeded, "succeededWithWarnings": 0,
            "failed": total - succeeded, "notRun": 0, "tests": tests}
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


def check_provenance_self_test() -> tuple[bool, list[str]]:
    """REPORT PROVENANCE part (a) fixtures, which need a real `report_created_on_raw` to
    compare a cited stamp against -- built on a scratch report/source pair exactly as
    `check_identity_self_test` is, never against the real `Saved/` or `Source/`.

    Both fixtures cite `reportCreatedOn 2026.08.26-00.28.42`; the scratch report here is
    stamped `2026.08.26-01.30.10`, matching the real 78ea508 fix, so the two are directly the
    BAD and GOOD shapes of the same defect.
    """
    import tempfile
    import time

    lines: list[str] = []
    ok = True

    def run_case(name: str, body: str, want_pass: bool) -> None:
        nonlocal ok
        with tempfile.TemporaryDirectory() as d:
            report_path = os.path.join(d, "index.json")
            source_dir = os.path.join(d, "Source")
            os.makedirs(source_dir)
            state_dir = os.path.join(d, "state")
            os.makedirs(state_dir)
            base = time.time() - 100000
            _write_fixture_report(report_path, 224, created_on="2026.08.26-01.30.10",
                                   mtime=base + 200)
            _write_fixture_source(os.path.join(source_dir, "T.cpp"), 224, mtime=base + 100)
            gpath = os.path.join(state_dir, "global.md")
            with io.open(gpath, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(body)
            res = run_sweep(gpath, check_tree=True, report_path=report_path, source_dir=source_dir)
        got_pass = res.passed
        good = got_pass == want_pass
        ok = ok and good
        detail = "" if res.passed else " -- " + "; ".join(f.check for f in res.findings)
        lines.append(f"    [{'OK' if good else '**WRONG**'}] {name}: "
                     f"expected {'PASS' if want_pass else 'FAIL'}, "
                     f"got {'PASS' if got_pass else 'FAIL'}{detail}")

    run_case("an unstamped citation of a DIFFERENT reportCreatedOn than the one the sweep "
             "opened FAILS", _BAD_PROVENANCE_STAMP, False)
    run_case("the same stale stamp inside a [STAMPED] supersession block PASSES",
              _GOOD_PROVENANCE, True)
    # 2026-08-26, the paragraph-wide-inertness fix: a wrong citation far from every marker in
    # its own giant paragraph must still FAIL under the windowed scope, even though two
    # unrelated stamp markers sit elsewhere in that same paragraph.
    run_case("a wrong citation buried in a long paragraph, far from two UNRELATED stamp "
             "markers elsewhere in it, still FAILS", _BAD_PROVENANCE_LONG_PARAGRAPH, False)
    # 2026-08-26, the quoted-figure exclusion gap: an honest account naming an EARLIER citation
    # as superseded, using "cited" and no bracket, must PASS -- this is the real
    # `global.md` shape, condensed -- the sentence opening `THE CHECK CAUGHT REAL DRIFT ON ITS
    # FIRST DAY`, quoted rather than cited by a line number that moves whenever that file grows.
    run_case("an honest account of a superseded citation, using the reporting verb 'cited', "
             "PASSES", _CITED_ACCOUNT_PROVENANCE, True)
    # 2026-09-04, the bare-stamp fix: a citation with no `reportCreatedOn` token at all, just
    # the digits, must now be reachable by part (a) just as a token-prefixed one is.
    run_case("a BARE stamp (no token) disagreeing with the report this sweep opened FAILS",
              _BAD_PROVENANCE_BARE_STAMP, False)
    run_case("the same BARE stamp shape, agreeing with the report this sweep opened, PASSES",
              _GOOD_PROVENANCE_BARE_STAMP, True)
    return ok, lines


def check_truth_self_test() -> tuple[bool, list[str]]:
    """Fixtures for the two 2026-09-01 defects, built on scratch report/source pairs exactly as
    `check_identity_self_test` and `check_provenance_self_test` are -- never against the real
    `Saved/` or `Source/`, which are outside this steward's lane.

    (defect 2) An honestly-stated NON-GREEN suite figure must be collected as a live claim and
    verified -- both halves: the denominator against the tree's total, the numerator against
    the report's own succeeded count.

    (defect 1) A live claim this run genuinely CANNOT verify -- no report, no macro census --
    must be a hard finding, never a silent pass.
    """
    import tempfile
    import time

    lines: list[str] = []
    ok = True

    def run_case(name: str, banner_figure: str, report_kwargs: "dict | None",
                 source_kwargs: "dict | None", want_pass: bool,
                 want_check: "str | None" = None) -> None:
        nonlocal ok
        with tempfile.TemporaryDirectory() as d:
            report_path = os.path.join(d, "index.json")
            # ABSENT ENTIRELY, NOT MERELY EMPTY, WHEN `source_kwargs` IS NONE. An empty-but-
            # existing `Source/` directory is not the same fact as no `Source/` at all --
            # `read_macro_census` returns a genuine `0`, not `None`, for an existing empty
            # directory, and `0` IS a ground truth (a real ground truth of zero macros), not an
            # absence of one. The UNVERIFIABLE case needs `read_macro_census` to return `None`,
            # which only happens when `os.path.isdir(source_dir)` is False -- so the directory
            # itself must not exist for that case, caught once by this exact confusion when the
            # first draft of this fixture created it unconditionally and got `SUITE COUNT
            # AGREEMENT` (disagreeing with a ground truth of 0) instead of `SUITE COUNT
            # UNVERIFIABLE` (no ground truth at all).
            source_dir = os.path.join(d, "Source")
            state_dir = os.path.join(d, "state")
            os.makedirs(state_dir)
            if report_kwargs is not None:
                _write_fixture_report_partial(report_path, **report_kwargs)
            if source_kwargs is not None:
                os.makedirs(source_dir)
                _write_fixture_source(os.path.join(source_dir, "T.cpp"), **source_kwargs)
            gpath = os.path.join(state_dir, "global.md")
            with io.open(gpath, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(f"# global\n\n_Last run 2026-09-01 "
                         f"(the suite is **{banner_figure}**, honestly non-green.)_\n\n"
                         f"## NEXT\n\n- **Nothing else.**\n")
            res = run_sweep(gpath, check_tree=True, report_path=report_path, source_dir=source_dir)
        got_pass = res.passed
        good = got_pass == want_pass
        if good and want_check is not None:
            good = any(f.check == want_check for f in res.findings)
        ok = ok and good
        detail = "" if res.passed else " -- " + "; ".join(f.check for f in res.findings)
        lines.append(f"    [{'OK' if good else '**WRONG**'}] {name}: "
                     f"expected {'PASS' if want_pass else 'FAIL'}, "
                     f"got {'PASS' if got_pass else 'FAIL'}{detail}")

    base = time.time() - 100000

    # DEFECT 2, HEALTHY PATH: an honest 346/347, a red report that genuinely says 346 of 347,
    # and a macro census agreeing on the total -- the real 2026-09-01 shape. Must PASS: this is
    # the record's own live claim, truthfully stated and truthfully verified.
    run_case("an honest 346/347 verified against a matching red report PASSES",
              "346/347",
              dict(total=347, succeeded=346, created_on="2026.09.01-19.41.17", mtime=base + 200),
              dict(count=347, mtime=base + 100), True)

    # DEFECT 2, THE DEFECT ITSELF WOULD HAVE READ AS CLEAN: a claimed 346/347 where the report
    # actually says 345 succeeded -- a real disagreement that the pre-fix filter could never
    # even have SEEN, because it dropped 346/347 before comparing anything. Must FAIL.
    run_case("a claimed 346/347 that disagrees with the report's own succeeded count (345) FAILS",
              "346/347",
              dict(total=347, succeeded=345, created_on="2026.09.01-19.41.17", mtime=base + 200),
              dict(count=347, mtime=base + 100), False, "SUITE COUNT AGREEMENT")

    # DEFECT 2, DENOMINATOR HALF: a claimed 346/347 where the report's total is actually 348 --
    # the SAME failure the green-suite check has always caught, now also reachable through a
    # non-green claim.
    run_case("a claimed 346/347 that disagrees with the tree's total (348) FAILS",
              "346/347",
              dict(total=348, succeeded=346, created_on="2026.09.01-19.41.17", mtime=base + 200),
              dict(count=348, mtime=base + 100), False, "SUITE COUNT AGREEMENT")

    # DEFECT 1: no report at all, no macro-census source at all, and a live claim standing
    # unverified. Before the fix this printed SWEEP CLEAN, exit 0 -- `truth` was `None` and
    # every comparison in `check_suite_counts` passed vacuously. Must FAIL now, loudly, with
    # the new check named so a reader knows WHY: not a contradiction, an inability to check.
    run_case("a live claim with NO report and NO macro-census source is UNVERIFIABLE and FAILS",
              "346/347", None, None, False, "SUITE COUNT UNVERIFIABLE")

    # THE HEALTHY GREEN PATH MUST STILL PASS, UNCHANGED, THROUGH THE SAME NEW CODE PATHS. Not a
    # regression fixture for either defect on its own -- both `report_count` and
    # `report_succeeded` now flow through `read_report_count` unconditionally, so an ordinary
    # all-green claim has to be proven to still verify correctly, not merely assumed to.
    run_case("an ordinary all-green 347/347 still verifies and PASSES",
              "347/347",
              dict(total=347, succeeded=347, created_on="2026.09.01-19.41.17", mtime=base + 200),
              dict(count=347, mtime=base + 100), True)

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
        # 2026-08-26, the gating repair: `citing` and `citation` have no `e` after `cit`, so
        # `\bcite\w*` never reached them though the comment above `_QUOTED_FIGURE_RE` named
        # both. These PASS under the SHIPPED `_QUOTED_FIGURE_RE` (named by object: the
        # literal spelling has changed twice) and would FAIL under the pre-widen
        # `\bcite\w*` -- see the direct regex differential pin further down for the proof that
        # isolates the pattern change itself, independent of these fixtures' verdicts.
        ("an honest account using 'citing' (no `e` after `cit`) is not a live claim",
         _CITING_ACCOUNT, True),
        ("an honest account using 'citation' (no `e` after `cit`) is not a live claim",
         _CITATION_ACCOUNT, True),
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
        # 2026-08-26, check 5: REPORT PROVENANCE. Part (b) needs no scratch report -- only this
        # process's own git branch -- so it is exercised here; part (a) needs a real
        # `reportCreatedOn` to compare against and is proved by `check_provenance_self_test`.
        ("an unstamped sentence naming ANOTHER tree/branch as where a figure was produced "
         "FAILS", _BAD_PROVENANCE_TREE, False),
        ("the same tree/branch sentence, stamped, alongside a stamped stale reportCreatedOn, "
         "PASSES", _GOOD_PROVENANCE, True),
        # 2026-08-26, the marker-split repair: a bad tree/branch claim standing near a
        # LEGITIMATE, unstamped reportCreatedOn citation for the pass it is genuinely part of.
        # Before the split this passed (the bare citation stamped the whole window); it must
        # FAIL now.
        ("a bad tree/branch claim beside a genuine, unstamped reportCreatedOn citation FAILS",
         _BAD_PROVENANCE_NEAR_CITATION, False),
        ("the SAME shape with a TRUE tree/branch claim beside the citation still PASSES",
         _GOOD_PROVENANCE_NEAR_CITATION, True),
        # 2026-08-26, the missing fixture the reviewer filed: a WRONG tree/branch claim beside
        # a genuine `**[STAMPED ...]**` annotation, with no reporting verb and no "It read:" to
        # launder it any other way -- see `_GOOD_PROVENANCE_TREE_STAMP_ONLY`'s own comment for
        # why neither existing PASS fixture pinned this.
        ("a WRONG tree/branch claim beside a genuine [STAMPED ...] annotation PASSES on the "
         "stamp alone", _GOOD_PROVENANCE_TREE_STAMP_ONLY, True),
        # 2026-08-26, the reviewer's functional-defect finding: `_PROVENANCE_PATH_BRANCH_RE`'s
        # literal "on branch" was not wrap-aware, so a wrong tree/branch claim wrapped exactly
        # there was invisible to part (b) -- fixed to `on\s+branch` above.
        ("an unstamped WRONG tree/branch claim wrapped between \"on\" and \"branch\" FAILS "
         "now that the regex is wrap-tolerant", _BAD_PROVENANCE_TREE_WRAPPED, False),
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

    # THE WRAP FIX ITSELF, PINNED AGAINST THE REGEX DIRECTLY -- not only against the sweep's
    # verdict on one fixture. The case above proves the SHIPPED `_PROVENANCE_PATH_BRANCH_RE`
    # fails `_BAD_PROVENANCE_TREE_WRAPPED`; it says nothing about whether reintroducing the
    # literal `on branch` this project shipped before 2026-08-26 would silently undo that,
    # because nothing above ever runs the OLD pattern. This does, without reconstructing a
    # whole second build: the shipped `\s+`-tolerant pattern must MATCH the wrapped sentence
    # and the pre-fix literal pattern must NOT, on the identical text. A real regression
    # against anyone who reintroduces the literal; the companion fact that the OLD build
    # passed this text where it should have failed is a measurement recorded in `global.md`,
    # not something reconstructable here.
    _pre_fix_path_branch_re = re.compile(
        r"`([A-Za-z]:[\\/][^`]+)`\s*on branch\s*`([^`]+)`", re.I)
    _wrapped_sentence = "`E:/MultiAgent/Strat-wt/slot-1` ON\nBRANCH `feat/match-result-screen`"
    now_matches = _PROVENANCE_PATH_BRANCH_RE.search(_wrapped_sentence) is not None
    pre_fix_matches = _pre_fix_path_branch_re.search(_wrapped_sentence) is not None
    good = now_matches and not pre_fix_matches
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] `_PROVENANCE_PATH_BRANCH_RE` matches "
                 f"a wrapped \"on\\nbranch\" sentence while the literal pre-fix pattern does "
                 f"not, on the identical text -- the regression that pins the wrap fix itself")

    # THE PARAGRAPH-WIDE-INERTNESS FIX ITSELF, PINNED DIRECTLY AGAINST THE OLD AND NEW SCOPE --
    # not only against the sweep's verdict on `_BAD_PROVENANCE_LONG_PARAGRAPH` above, the same
    # way the wrap fix is pinned directly against its regex just above this block. The pre-fix
    # scope was `is_stamped(_mask_span(para, m.start(), m.end()))` over the WHOLE paragraph; the
    # shipped scope masks the same span but restricts `is_stamped` to
    # `_PROVENANCE_CITATION_WINDOW` characters either side. Run both directly against the same
    # fixture paragraph and the same citation match, without reconstructing a second build: the
    # pre-fix call must return True (laundered by the two unrelated markers merely being
    # SOMEWHERE in the paragraph) and the shipped call must return False (neither marker is
    # within reach of the citation). This is the direct proof that closing measurement (1) did
    # not merely add a coincidental finding elsewhere -- it is the specific mechanism, isolated.
    _long_para = next(p for _s, p in paragraphs_with_lines(_BAD_PROVENANCE_LONG_PARAGRAPH)
                      if "reportCreatedOn" in p)
    _long_m = next(_CITED_REPORT_STAMP_RE.finditer(_long_para))
    _pre_fix_stamped = is_stamped(_mask_span(_long_para, _long_m.start(), _long_m.end()))
    _ws = max(0, _long_m.start() - _PROVENANCE_CITATION_WINDOW)
    _we = min(len(_long_para), _long_m.end() + _PROVENANCE_CITATION_WINDOW)
    _post_fix_stamped = is_stamped(
        _mask_span(_long_para[_ws:_we], _long_m.start() - _ws, _long_m.end() - _ws))
    good = _pre_fix_stamped and not _post_fix_stamped
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] the pre-fix whole-paragraph "
                 f"`is_stamped` call launders the buried citation (True) while the shipped "
                 f"windowed call does not (False), on the identical paragraph and match -- the "
                 f"regression that pins the 2026-08-26 inertness fix itself")

    # THE BARE-STAMP WIDENING OF `_CITED_REPORT_STAMP_RE` ITSELF, PINNED DIRECTLY -- not only
    # against `_BAD_PROVENANCE_BARE_STAMP`'s verdict in `check_provenance_self_test`. Two
    # properties, asserted against the live module-level object:
    # (1) a BARE stamp with no token is now found at all -- the pre-fix pattern, reconstructed
    #     here as the literal it shipped as before this fix, finds nothing on bare digits.
    # (2) a TOKEN-PREFIXED stamp is still found exactly ONCE, not twice -- the optional group
    #     consumes the token when present, so `re.finditer` never re-reports the same digits a
    #     second time starting mid-match. This is the property the file's own comment above the
    #     regex claims; asserted here rather than left as an unverified claim in a comment.
    _pre_fix_cited_stamp_re = re.compile(
        r"reportCreatedOn\s+(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2})")
    _bare_text = "the certifying report is 2026.08.26-00.28.42, unstamped."
    _prefixed_text = "the certifying report is reportCreatedOn 2026.08.26-00.28.42, unstamped."
    _bare_found_now = _CITED_REPORT_STAMP_RE.findall(_bare_text)
    _bare_found_pre_fix = _pre_fix_cited_stamp_re.findall(_bare_text)
    _prefixed_found_now = _CITED_REPORT_STAMP_RE.findall(_prefixed_text)
    good = (_bare_found_now == ["2026.08.26-00.28.42"] and _bare_found_pre_fix == []
            and _prefixed_found_now == ["2026.08.26-00.28.42"])
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] `_CITED_REPORT_STAMP_RE` now finds a "
                 f"BARE stamp (the pre-fix literal pattern finds none, on the identical text) "
                 f"and still finds a TOKEN-PREFIXED stamp exactly ONCE, not twice -- the "
                 f"regression that pins the 2026-09-04 bare-stamp fix and its non-double-count "
                 f"property together")

    # THE `cite`-FAMILY ADDITION TO `_QUOTED_FIGURE_RE`, PINNED DIRECTLY -- not only against
    # `_CITED_ACCOUNT_PROVENANCE`'s verdict above. Extracted straight from the real
    # `global.md` sentence ("the banner segment below cited `reportCreatedOn ...`") -- the quote
    # is the anchor, replacing a `global.md:76` line citation that growth had already moved off
    # the sentence it named. This
    # asserts the shipped pattern (with `cite`-family words) matches the `quoting_window` text
    # and a reconstruction of the pattern WITHOUT them does not, on the identical text -- a real
    # regression against anyone who removes the addition, the same shape as the wrap-fix and
    # paragraph-wide-inertness pins just above.
    _pre_addition_quoted_figure_re = re.compile(
        r"said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|"
        r"wrongly|stale|defect|would\s+have|reported", re.I)
    _cited_sentence = ("THE CHECK CAUGHT REAL DRIFT ON ITS FIRST DAY, and it was ours: the "
                       "banner segment below cited\n`reportCreatedOn 2026.08.26-03.40.42`")
    now_matches = _QUOTED_FIGURE_RE.search(_cited_sentence) is not None
    pre_addition_matches = _pre_addition_quoted_figure_re.search(_cited_sentence) is not None
    good = now_matches and not pre_addition_matches
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] `_QUOTED_FIGURE_RE` matches the real "
                 f"'cited `reportCreatedOn ...`' sentence while the pre-addition pattern does "
                 f"not, on the identical text -- the regression that pins the cite-family "
                 f"addition itself")

    # THE GATING WIDENING (`\bcite\w*` -> `\bcit(?:e|es|ed|ing|ation|ations)\b`) ITSELF, PINNED
    # DIRECTLY ON BOTH FAILURE MODES IT HAS NOW HAD -- not only against `_CITING_ACCOUNT`'s and
    # `_CITATION_ACCOUNT`'s verdicts above. `cite`/`cited`/`cites` all put an `e` immediately
    # after `cit`, which is why the first-shipped pattern reached them; `citing` and
    # `citation(s)` do not, and no amount of `\w*` recovers a character the pattern never
    # anchored on -- that is the GAP the first widening closed. But the first widening shipped
    # as the open class `\bcit\w*`, which OVER-reaches into `cities`/`citizen`/`citadel`/
    # `citizenry` -- a second, opposite defect, found and closed 2026-08-26 by tightening to
    # the closed six-form alternation. This asserts DIRECTLY AGAINST THE LIVE MODULE-LEVEL
    # `_QUOTED_FIGURE_RE` OBJECT, not a re-typed literal -- a hand-typed copy of "the shipped
    # pattern" is exactly how the previous version of this pin went on pinning `\bcit\w*` after
    # the code had already moved past it, undetected until the next reviewer read the two side
    # by side. Asserting against `_QUOTED_FIGURE_RE` itself means reverting the module-level
    # pattern in EITHER direction, back to `\bcite\w*` or forward to the open `\bcit\w*`, is
    # caught by this same fixture with no separate literal to keep in sync.
    _pre_widen_cite_re = re.compile(r"\bcite\w*", re.I)
    _gap_words = ("citing", "citation", "citations")
    _over_reach_words = ("cities", "citizen", "citadel", "citizenry")
    _pre_widen_hits = [w for w in _gap_words if _pre_widen_cite_re.search(w)]
    _shipped_gap_hits = [w for w in _gap_words if _QUOTED_FIGURE_RE.search(w)]
    _shipped_over_reach_hits = [w for w in _over_reach_words if _QUOTED_FIGURE_RE.search(w)]
    good = (_shipped_gap_hits == list(_gap_words) and _pre_widen_hits == []
            and _shipped_over_reach_hits == [])
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] the live `_QUOTED_FIGURE_RE` matches "
                 f"all of {_gap_words} (closing the pre-widen `\\bcite\\w*` gap) and none of "
                 f"{_over_reach_words} (closing the open-class `\\bcit\\w*` over-reach) -- "
                 f"asserted against the module-level object itself, so the pin cannot go on "
                 f"naming a pattern no longer shipped")

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

    provenance_ok, provenance_lines = check_provenance_self_test()
    ok = ok and provenance_ok
    lines.append("    -- REPORT PROVENANCE part (a) (cited stamp vs the report actually read) --")
    lines.extend(provenance_lines)

    # THE COLLECTION ITSELF, PINNED DIRECTLY -- not only through a full sweep's verdict via
    # `check_truth_self_test` below. This is the exact regression `global.md`'s own 2026-09-01
    # entry measured: a live `346/347` was ABSENT from `_collect_suite_claims`'s output because
    # of the unconditional `if n != d: continue` this fix removed. Asserted against the live
    # function itself, so reintroducing that continue anywhere in this function is caught here
    # even if every case in `check_truth_self_test` happened to still pass for some other reason.
    _probe_result = SweepResult()
    _collect_suite_claims("global.md", "_Last run 2026-09-01 (the suite is **346/347**.)_\n",
                          _probe_result)
    _collected = [(c.numerator, c.denominator, c.live) for c in _probe_result.suite_claims]
    good = (346, 347, True) in _collected
    ok = ok and good
    lines.append(f"    [{'OK' if good else '**WRONG**'}] a non-green 346/347 figure IS "
                 f"collected as a LIVE claim by `_collect_suite_claims` directly: {_collected}")

    truth_ok, truth_lines = check_truth_self_test()
    ok = ok and truth_ok
    lines.append("    -- 2026-09-01: a non-green figure is a claim, and unverifiable is a finding --")
    lines.extend(truth_lines)

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
