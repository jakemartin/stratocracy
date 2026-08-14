"""STRAT-COMBAT / STRAT-AI pairing gate -- combat-outcome milestone, phase 3, piece (c).

WHAT THIS GATE PROVES, PRECISELY. `Source/StratBridge/StratBridge.cpp`'s `FStratBridge::Submit`
emits exactly one `STRAT-COMBAT resolved` line for every Attack command that reaches
`strat::applyCommand`, on the SAME call that (for an AI-submitted command) causes
`Source/StratPlay/StratAiTurnRunner.cpp` to log `STRAT-AI applied kind=Attack ...` immediately
after. This gate asserts that pairing holds, IN ORDER, over one isolated AI-vs-AI match slice --
not merely that the two counts agree.

THE BRIEF FOR THIS PHASE WAS WRONG, AND THE CORRECTION IS THE RESULT THIS GATE ENCODES. The
brief said the gate pairs `STRAT-CMD accepted kind=Attack` 1:1 with `STRAT-COMBAT resolved`.
`STRAT-CMD accepted` is `StratSelectionMachine.cpp`'s line for a HUMAN click applier; the AI path
never touches it (`StratAiTurnRunner.cpp:242-247` states this explicitly: AI commands produce no
`STRAT-CMD` line because routing them through `StratSubmitSelectionCommand` would mean inventing
a click). In an AI-vs-AI match, measured against
`Saved/Logs/Stratocracy.log` (2026-08-14 session), sliced to the one test
`Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` (lines 4179-4474 in
that run):

    STRAT-CMD accepted kind=Attack : 0    (grep -c "STRAT-CMD accepted kind=Attack" slice.log)
    STRAT-AI applied kind=Attack   : 68   (grep -c "STRAT-AI applied kind=Attack" slice.log)
    STRAT-COMBAT resolved          : 68   (grep -c "STRAT-COMBAT resolved" slice.log)

The real pairing is `STRAT-AI applied kind=Attack` <-> `STRAT-COMBAT resolved`, and this gate
asserts it by ORDERED IDENTITY on `(unit, hex)` vs `(attacker, hex)` -- position i's applied
event must name the same unit and target hex as position i's resolved event -- not merely that
the two totals match. A multiset-equal, order-scrambled corpus is a real fault this gate is built
to catch; a bare count comparison would not catch it. See `check_self_test()` below for the
fixture that proves this positively.

SLICING IS MANDATORY, AND BY CONTENT MARKERS, NEVER EOF. This log has no `Log file closed` line
-- it ends `**** TEST COMPLETE. EXIT CODE: 0 ****`, and it is a suite log carrying 493
`STRAT-COMBAT resolved` lines in total, 425 of which come from bridge/fixture tests that submit
directly and never emit `STRAT-AI applied` (a 1:1 count comparison over the WHOLE log fails on
both sides for that reason). `slice_by_test_name` bounds the corpus to one test's own
`Test Started.` / `Test Completed.` pair -- the only markers this run's corpus actually contains
for that purpose -- keyed on the test's own `Path={...}`, and refuses to run (a hard failure, not
a silent full-log scan) when either marker is missing.

THE COMMENTARY-ECHO TRAP, MEASURED IN THIS SAME LOG. `LogAutomationController` echoes captured
log lines verbatim between a test's `BeginEvents:` / `EndEvents:` markers -- e.g. line 3649 of
this run reads `LogAutomationController: STRAT-COMBAT resolved attacker=10 ...`, byte-identical
to the real `LogStratBridge:` emission three lines above it at line 3645. A bare substring search
for `STRAT-COMBAT` double-counts every one of these. Both this module's payload finder and
`strat_ai_log_gate.find_strat_ai_payload` anchor on the PRODUCING category
(`LogStratBridge:` / `LogStratPlay:`) and not on the payload substring, for exactly this reason.

THE `!bSeeded` SILENT PATH (phase 1's documented hole) IS HANDLED STRUCTURALLY, NOT BY A COUNT
FLOOR. `FStratBridge::Submit` returns before either `STRAT-COMBAT` emitter runs when the bridge
is unseeded, so a would-be attack refused there leaves NO `STRAT-COMBAT` line of any kind. Within
this corpus that command never even reaches `STRAT-AI applied` -- an unseeded `Submit` makes
`Port.Submit` return false, which `StratAiTurnRunner.cpp:303-314` reports as `STRAT-AI refused
phase=apply kind=Attack ...`, not `applied` -- so the pairing this gate checks (applied <->
resolved) cannot by construction include a command silenced by that guard. It is handled
structurally rather than assumed away: `run_gate` counts `applied_attack_count` and
`combat_resolved_count` independently and FAILS on any mismatch between them, so a future run in
which some other path produces an applied-but-unresolved attack reports a real length gap rather
than being waved through by a floor check. Measured zero hits in this slice
(`applied_attack_count == combat_resolved_count == 68`); that is a measurement of this run, not a
guarantee the gap can never fire.

`ReplayLog` DOES NOT ROUTE THROUGH `Submit` (also carried from phase 1): a combat log loaded from
disk in a fresh process emits no `STRAT-COMBAT` line at all. A reader pointing this gate at a
replayed-log session, expecting the pairing to hold, will see it fail for that reason and should
look here first, not at the parser.

Nothing in this module writes to `Data/` or `Source/`. It only reads a log path given on argv.
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, Iterator, Union

sys.path.insert(0, str(Path(__file__).resolve().parent))
import strat_ai_log_gate as ai_gate  # the STRAT-AI field parser -- one copy of that rule, reused.

# ---------------------------------------------------------------------------
# The STRAT-COMBAT field contract, mirroring `StratBridge.cpp`'s `DescribeCommonFields` /
# `EmitResolved` / `EmitRefused` field order verbatim (read directly from that file for this
# module, not transcribed from `state.md`).
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class StratCombatResolved:
    """`STRAT-COMBAT resolved ...` -- one attack that reached `strat::applyCommand`."""

    attacker: int
    defender: int
    hex_x: int
    hex_y: int
    turn: int
    side: int
    attacker_side: int
    fqueried: int
    flegal: int
    fdist: int
    fdmg: int
    fdies: int
    fcounter: int
    fcdmg: int
    ahp_before: int
    ahp_after: int
    ahp_max: int
    adied: int
    dhp_before: int
    dhp_after: int
    dhp_max: int
    ddied: int
    fame_before: int
    fame_after: int
    fame_combat_before: int
    fame_combat_after: int
    agree: int
    diverge: int


@dataclass(frozen=True)
class StratCombatRefused:
    """`STRAT-COMBAT refused ...` -- an attack that reached `Submit` (past the `!bSeeded`
    guard) and was refused by a later check inside it."""

    attacker: int
    defender: int
    hex_x: int
    hex_y: int
    turn: int
    side: int
    attacker_side: int
    fqueried: int
    flegal: int
    fdist: int
    fdmg: int
    fdies: int
    fcounter: int
    fcdmg: int
    reason: str


@dataclass(frozen=True)
class StratCombatDivergence:
    """`STRAT-COMBAT divergence ...` -- emitted BESIDE a `resolved` line, at `Error` level,
    when the forecast disagreed with what was measured. Never a substitute for `resolved`."""

    attacker: int
    defender: int
    hex_x: int
    hex_y: int
    turn: int
    side: int
    attacker_side: int
    fqueried: int
    flegal: int
    fdist: int
    fdmg: int
    fdies: int
    fcounter: int
    fcdmg: int
    diverge: int


@dataclass(frozen=True)
class StratCombatParseFailure:
    """A line that begins `STRAT-COMBAT ` (past the `LogStratBridge:` anchor) but matches none
    of the three known shapes. Reported, never silently dropped -- a format drift must surface
    as N of these, not as an empty result set that reads like a clean run."""

    line_no: int
    raw: str
    reason: str


StratCombatEvent = Union[StratCombatResolved, StratCombatRefused, StratCombatDivergence]
StratCombatParsed = Union[StratCombatEvent, StratCombatParseFailure]

_INT = r"-?\d+"
_COMMON = (
    r"attacker=(?P<attacker>" + _INT + r") defender=(?P<defender>" + _INT + r") "
    r"hex=(?P<hex_x>" + _INT + r"),(?P<hex_y>" + _INT + r") turn=(?P<turn>" + _INT + r") "
    r"side=(?P<side>" + _INT + r") attackerSide=(?P<attacker_side>" + _INT + r") "
    r"fqueried=(?P<fqueried>" + _INT + r") flegal=(?P<flegal>" + _INT + r") "
    r"fdist=(?P<fdist>" + _INT + r") fdmg=(?P<fdmg>" + _INT + r") fdies=(?P<fdies>" + _INT + r") "
    r"fcounter=(?P<fcounter>" + _INT + r") fcdmg=(?P<fcdmg>" + _INT + r")"
)

_RESOLVED_RE = re.compile(
    r"^STRAT-COMBAT resolved " + _COMMON + r" "
    r"ahpBefore=(?P<ahp_before>" + _INT + r") ahpAfter=(?P<ahp_after>" + _INT + r") "
    r"ahpMax=(?P<ahp_max>" + _INT + r") adied=(?P<adied>" + _INT + r") "
    r"dhpBefore=(?P<dhp_before>" + _INT + r") dhpAfter=(?P<dhp_after>" + _INT + r") "
    r"dhpMax=(?P<dhp_max>" + _INT + r") ddied=(?P<ddied>" + _INT + r") "
    r"fameBefore=(?P<fame_before>" + _INT + r") fameAfter=(?P<fame_after>" + _INT + r") "
    r"fameCombatBefore=(?P<fame_combat_before>" + _INT + r") "
    r"fameCombatAfter=(?P<fame_combat_after>" + _INT + r") "
    r"agree=(?P<agree>" + _INT + r") diverge=(?P<diverge>" + _INT + r")$"
)

_REFUSED_RE = re.compile(
    r"^STRAT-COMBAT refused " + _COMMON + r" reason=(?P<reason>.+)$"
)

_DIVERGENCE_RE = re.compile(
    r"^STRAT-COMBAT divergence " + _COMMON + r" diverge=(?P<diverge>" + _INT + r")$"
)

_COMBAT_CATEGORY = "LogStratBridge:"


def find_strat_combat_payload(line: str) -> str | None:
    """Slices a raw log line down to the `STRAT-COMBAT ...` payload, anchored on the
    `LogStratBridge:` category -- the sole emitter -- exactly as
    `strat_ai_log_gate.find_strat_ai_payload` anchors on `LogStratPlay:`.

    MEASURED NECESSARY, NOT DEFENSIVE POSTURE: `LogAutomationController` echoes a test's
    captured log lines verbatim between its own `BeginEvents:` / `EndEvents:` markers -- this
    run's own log carries `LogAutomationController: STRAT-COMBAT resolved attacker=10 ...` at
    line 3649, byte-identical in payload to the real `LogStratBridge:` emission three lines
    above it at line 3645. A bare substring search for `STRAT-COMBAT` double-counts every one
    of these echoes. Returns `None` for a line carrying no `LogStratBridge:` category at all --
    that is a skip, not a failure, because most lines are not this family's.
    """
    cat_idx = line.find(_COMBAT_CATEGORY)
    if cat_idx == -1:
        return None
    idx = line.find("STRAT-COMBAT ", cat_idx)
    if idx == -1:
        return None
    return line[idx:].rstrip("\r\n")


def parse_strat_combat_payload(payload: str, line_no: int = -1) -> StratCombatParsed:
    m = _RESOLVED_RE.match(payload)
    if m:
        g = {k: int(v) for k, v in m.groupdict().items()}
        return StratCombatResolved(**g)

    m = _REFUSED_RE.match(payload)
    if m:
        g = m.groupdict()
        ints = {k: int(v) for k, v in g.items() if k != "reason"}
        return StratCombatRefused(reason=g["reason"], **ints)

    m = _DIVERGENCE_RE.match(payload)
    if m:
        g = {k: int(v) for k, v in m.groupdict().items()}
        return StratCombatDivergence(**g)

    return StratCombatParseFailure(
        line_no=line_no,
        raw=payload,
        reason="matches no known STRAT-COMBAT shape (resolved / refused / divergence)",
    )


def iter_strat_combat_events(lines: Iterable[str]) -> Iterator[tuple[int, StratCombatParsed]]:
    for line_no, line in enumerate(lines, start=1):
        payload = find_strat_combat_payload(line)
        if payload is None:
            continue
        yield line_no, parse_strat_combat_payload(payload, line_no=line_no)


# ---------------------------------------------------------------------------
# Slicing by content markers, never by EOF.
# ---------------------------------------------------------------------------


@dataclass
class SliceFailure:
    reason: str


def slice_by_test_name(lines: list[str], test_path: str) -> tuple[int, int] | SliceFailure:
    """Returns the (start, end) 0-based half-open Python slice bounds of one automation test's
    own `Test Started.` / `Test Completed.` line pair, matched on its full `Path={test_path}`.

    A missing marker is a hard failure, not a silent fallback to the whole file -- an unsliced
    scan over this corpus finds 493 `STRAT-COMBAT resolved` lines against 68 `STRAT-AI applied
    kind=Attack` ones (425 of the 493 are from bridge/fixture tests that submit directly and
    never emit `STRAT-AI applied`), which fails both directions of the pairing for a reason that
    has nothing to do with the thing this gate exists to check.
    """
    started_re = re.compile(
        r"Test Started\. Name=\{[^}]*\} Path=\{" + re.escape(test_path) + r"\}"
    )
    completed_re = re.compile(
        r"Test Completed\. Result=\{[^}]*\} Name=\{[^}]*\} Path=\{" + re.escape(test_path) + r"\}"
    )

    start_line = None
    for i, line in enumerate(lines):
        if started_re.search(line):
            start_line = i
            break
    if start_line is None:
        return SliceFailure(
            f"no 'Test Started.' marker found for Path={{{test_path}}} -- this log does not "
            "contain a run of the expected test, or the test's full path has drifted."
        )

    end_line = None
    for i in range(start_line, len(lines)):
        if completed_re.search(lines[i]):
            end_line = i
            break
    if end_line is None:
        return SliceFailure(
            f"'Test Started.' found at line {start_line + 1} for Path={{{test_path}}}, but no "
            "matching 'Test Completed.' marker follows it -- the log is truncated mid-test, or "
            "the process did not exit cleanly. This log has no 'Log file closed' line to fall "
            "back on; it ends '**** TEST COMPLETE. EXIT CODE: 0 ****' when it exits cleanly, and "
            "that marker itself is not a valid substitute for THIS test's own boundary."
        )

    return (start_line, end_line + 1)  # half-open, inclusive of the Completed line


DEFAULT_TEST_PATH = "Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound"


# ---------------------------------------------------------------------------
# The pairing check.
# ---------------------------------------------------------------------------


@dataclass
class PairingMismatch:
    index: int
    applied_line_no: int | None
    resolved_line_no: int | None
    detail: str


@dataclass
class GateResult:
    log_path: str
    test_path: str
    slice_bounds: tuple[int, int] | None  # 1-based (first, last) line numbers actually scanned

    applied_attacks: list[tuple[int, "ai_gate.StratAiApplied"]]
    combat_resolved: list[tuple[int, StratCombatResolved]]
    combat_refused: list[tuple[int, StratCombatRefused]]
    combat_divergences: list[tuple[int, StratCombatDivergence]]
    combat_parse_failures: list[tuple[int, StratCombatParseFailure]]

    ai_parse_failures: list[tuple[int, "ai_gate.StratAiParseFailure"]]
    ai_blocking_refusals: list[tuple[int, "ai_gate.StratAiRefused"]]
    ai_terminal_refusals: list[tuple[int, "ai_gate.StratAiRefused"]]

    pairing_mismatches: list[PairingMismatch]
    slice_failure: SliceFailure | None

    @property
    def passed(self) -> bool:
        if self.slice_failure is not None:
            return False
        return not (
            self.combat_parse_failures
            or self.ai_parse_failures
            or self.ai_blocking_refusals
            or self.combat_divergences
            or self.pairing_mismatches
        )

    def render(self) -> str:
        if self.slice_failure is not None:
            return (
                f"STRAT-COMBAT pairing gate: {self.log_path}\n"
                f"  test:  {self.test_path}\n"
                f"  FAIL: slicing failed -- {self.slice_failure.reason}\n"
                "FAIL"
            )

        lines = [
            f"STRAT-COMBAT pairing gate: {self.log_path}",
            f"  test:                       {self.test_path}",
            f"  slice (1-based lines):      {self.slice_bounds[0]}..{self.slice_bounds[1]}",
            f"  STRAT-AI applied kind=Attack:  {len(self.applied_attacks)}",
            f"  STRAT-COMBAT resolved:         {len(self.combat_resolved)}",
            f"  STRAT-COMBAT refused:          {len(self.combat_refused)}",
            f"  STRAT-COMBAT divergence:       {len(self.combat_divergences)}",
            f"  STRAT-AI terminal refusals:    {len(self.ai_terminal_refusals)}",
            f"  STRAT-AI blocking refusals:    {len(self.ai_blocking_refusals)}",
            f"  STRAT-COMBAT parse failures:   {len(self.combat_parse_failures)}",
            f"  STRAT-AI parse failures:       {len(self.ai_parse_failures)}",
            f"  pairing mismatches:            {len(self.pairing_mismatches)}",
        ]
        for line_no, failure in self.combat_parse_failures:
            lines.append(f"    FAIL line {line_no}: unparseable STRAT-COMBAT line: {failure.raw!r}")
        for line_no, failure in self.ai_parse_failures:
            lines.append(f"    FAIL line {line_no}: unparseable STRAT-AI line: {failure.raw!r}")
        for line_no, refused in self.ai_blocking_refusals:
            lines.append(
                f"    FAIL line {line_no}: non-terminal STRAT-AI refused "
                f"phase={refused.phase} kind={refused.kind} turn={refused.turn} "
                f"side={refused.side} reason={refused.reason!r}"
            )
        for line_no, div in self.combat_divergences:
            lines.append(
                f"    FAIL line {line_no}: STRAT-COMBAT divergence attacker={div.attacker} "
                f"defender={div.defender} hex={div.hex_x},{div.hex_y} diverge={div.diverge}"
            )
        for mm in self.pairing_mismatches:
            lines.append(
                f"    FAIL pair index {mm.index}: applied line {mm.applied_line_no}, "
                f"resolved line {mm.resolved_line_no}: {mm.detail}"
            )
        lines.append("PASS" if self.passed else "FAIL")
        return "\n".join(lines)


def run_gate(log_path: str | Path, test_path: str = DEFAULT_TEST_PATH) -> GateResult:
    path = Path(log_path)
    text = path.read_text(encoding="utf-8", errors="replace")
    all_lines = text.splitlines()

    bounds = slice_by_test_name(all_lines, test_path)
    if isinstance(bounds, SliceFailure):
        return GateResult(
            log_path=str(path), test_path=test_path, slice_bounds=None,
            applied_attacks=[], combat_resolved=[], combat_refused=[], combat_divergences=[],
            combat_parse_failures=[], ai_parse_failures=[], ai_blocking_refusals=[],
            ai_terminal_refusals=[], pairing_mismatches=[], slice_failure=bounds,
        )

    start, end = bounds
    slice_lines = all_lines[start:end]
    # Report 1-based line numbers relative to the WHOLE file, so a reader can jump straight to
    # them; line_no from the iterators below is relative to slice_lines (1-based within slice).
    offset = start  # add to a slice-relative 1-based line_no to get whole-file 1-based.

    applied_attacks: list[tuple[int, ai_gate.StratAiApplied]] = []
    ai_parse_failures: list[tuple[int, ai_gate.StratAiParseFailure]] = []
    ai_blocking_refusals: list[tuple[int, ai_gate.StratAiRefused]] = []
    ai_terminal_refusals: list[tuple[int, ai_gate.StratAiRefused]] = []

    for rel_line_no, parsed in ai_gate.iter_strat_ai_events(slice_lines):
        line_no = rel_line_no + offset
        if isinstance(parsed, ai_gate.StratAiApplied) and parsed.kind == "Attack":
            applied_attacks.append((line_no, parsed))
        elif isinstance(parsed, ai_gate.StratAiRefused):
            if ai_gate.is_terminal_handover_refusal(parsed):
                ai_terminal_refusals.append((line_no, parsed))
            else:
                ai_blocking_refusals.append((line_no, parsed))
        elif isinstance(parsed, ai_gate.StratAiParseFailure):
            ai_parse_failures.append((line_no, parsed))
        # StratAiApplied of other kinds and StratAiTurnEnded are outside this gate's scope.

    combat_resolved: list[tuple[int, StratCombatResolved]] = []
    combat_refused: list[tuple[int, StratCombatRefused]] = []
    combat_divergences: list[tuple[int, StratCombatDivergence]] = []
    combat_parse_failures: list[tuple[int, StratCombatParseFailure]] = []

    for rel_line_no, parsed in iter_strat_combat_events(slice_lines):
        line_no = rel_line_no + offset
        if isinstance(parsed, StratCombatResolved):
            combat_resolved.append((line_no, parsed))
        elif isinstance(parsed, StratCombatRefused):
            combat_refused.append((line_no, parsed))
        elif isinstance(parsed, StratCombatDivergence):
            combat_divergences.append((line_no, parsed))
        elif isinstance(parsed, StratCombatParseFailure):
            combat_parse_failures.append((line_no, parsed))
        else:  # pragma: no cover -- exhaustiveness guard
            raise AssertionError(f"unhandled parsed type: {type(parsed)!r}")

    # ---- The pairing check: ORDERED IDENTITY on (unit, hex) vs (attacker, hex). ----
    # A length mismatch is itself a failure -- it is the structural net under the `!bSeeded`
    # silent path and the `ReplayLog` gap: either would make these two lists a different
    # length, and neither is waved through by only checking positions that exist on both sides.
    pairing_mismatches: list[PairingMismatch] = []
    n = max(len(applied_attacks), len(combat_resolved))
    for i in range(n):
        applied = applied_attacks[i] if i < len(applied_attacks) else None
        resolved = combat_resolved[i] if i < len(combat_resolved) else None
        if applied is None:
            resolved_line_no, r = resolved
            pairing_mismatches.append(PairingMismatch(
                index=i, applied_line_no=None, resolved_line_no=resolved_line_no,
                detail=(
                    f"STRAT-COMBAT resolved (attacker={r.attacker} hex={r.hex_x},{r.hex_y}) "
                    "has no corresponding STRAT-AI applied kind=Attack at this position -- more "
                    "resolutions than applied attacks."
                ),
            ))
            continue
        if resolved is None:
            applied_line_no, a = applied
            pairing_mismatches.append(PairingMismatch(
                index=i, applied_line_no=applied_line_no, resolved_line_no=None,
                detail=(
                    f"STRAT-AI applied kind=Attack (unit={a.unit} hex={a.hex_x},{a.hex_y}) has "
                    "no corresponding STRAT-COMBAT resolved at this position -- more applied "
                    "attacks than resolutions. Possible causes: the !bSeeded silent path "
                    "(phase 1), or a submission not routed through FStratBridge::Submit."
                ),
            ))
            continue

        applied_line_no, a = applied
        resolved_line_no, r = resolved
        if (a.unit, a.hex_x, a.hex_y) != (r.attacker, r.hex_x, r.hex_y):
            pairing_mismatches.append(PairingMismatch(
                index=i, applied_line_no=applied_line_no, resolved_line_no=resolved_line_no,
                detail=(
                    f"identity mismatch: applied (unit={a.unit}, hex={a.hex_x},{a.hex_y}) != "
                    f"resolved (attacker={r.attacker}, hex={r.hex_x},{r.hex_y})"
                ),
            ))

    return GateResult(
        log_path=str(path),
        test_path=test_path,
        slice_bounds=(start + 1, end),  # 1-based, inclusive
        applied_attacks=applied_attacks,
        combat_resolved=combat_resolved,
        combat_refused=combat_refused,
        combat_divergences=combat_divergences,
        combat_parse_failures=combat_parse_failures,
        ai_parse_failures=ai_parse_failures,
        ai_blocking_refusals=ai_blocking_refusals,
        ai_terminal_refusals=ai_terminal_refusals,
        pairing_mismatches=pairing_mismatches,
        slice_failure=None,
    )


# ---------------------------------------------------------------------------
# Self-test: fixtures proving this gate can FAIL. Run with `--self-test`.
#
# The phase-D precedent (`gate_self_test_output.txt`) records that the phase-D gate was itself
# blocked once for fixtures that could not fail it -- a gate returning exit 0 on a corpus it
# cannot actually fault is the failure mode this function exists to rule out.
# ---------------------------------------------------------------------------

_STARTED = (
    "[2026.08.14-14.23.35:247][726]LogAutomationController: Display: Test Started. "
    "Name={FixtureTest} Path={Stratocracy.Fixture.FIX-01.FixtureTest}"
)
_COMPLETED = (
    "[2026.08.14-14.23.35:273][728]LogAutomationController: Display: Test Completed. "
    "Result={Success} Name={FixtureTest} Path={Stratocracy.Fixture.FIX-01.FixtureTest}"
)
_FIXTURE_TEST_PATH = "Stratocracy.Fixture.FIX-01.FixtureTest"


def _resolved_line(attacker: int, hex_xy: tuple[int, int], turn: int = 1, side: int = 1) -> str:
    return (
        f"[t][1]LogStratBridge: STRAT-COMBAT resolved attacker={attacker} defender=5 "
        f"hex={hex_xy[0]},{hex_xy[1]} turn={turn} side={side} attackerSide={side} fqueried=1 "
        "flegal=1 fdist=1 fdmg=3 fdies=0 fcounter=1 fcdmg=1 ahpBefore=12 ahpAfter=11 ahpMax=12 "
        "adied=0 dhpBefore=12 dhpAfter=9 dhpMax=12 ddied=0 fameBefore=100 fameAfter=100 "
        "fameCombatBefore=0 fameCombatAfter=0 agree=1 diverge=0"
    )


def _applied_attack_line(unit: int, hex_xy: tuple[int, int], turn: int = 1, side: int = 1) -> str:
    return (
        f"[t][1]LogStratPlay: STRAT-AI applied kind=Attack unit={unit} "
        f"hex={hex_xy[0]},{hex_xy[1]} def=-1 target=5 turn={turn} side={side} hash=deadbeef"
    )


def _wrap(body_lines: list[str], test_path: str = _FIXTURE_TEST_PATH) -> str:
    started = _STARTED if test_path == _FIXTURE_TEST_PATH else _STARTED.replace(
        _FIXTURE_TEST_PATH, test_path)
    completed = _COMPLETED if test_path == _FIXTURE_TEST_PATH else _COMPLETED.replace(
        _FIXTURE_TEST_PATH, test_path)
    return "\n".join([started, *body_lines, completed])


def check_self_test() -> tuple[bool, str]:
    """Runs every fixture; returns (all_correct, report_text). Each fixture asserts the gate
    produces the SPECIFIC verdict it should -- FAIL for a faulty corpus, PASS for a clean one --
    so this cannot be satisfied by a gate that always returns one answer."""
    import tempfile

    report_lines: list[str] = []
    all_ok = True

    def record(name: str, ok: bool, detail: str = "") -> None:
        nonlocal all_ok
        status = "OK" if ok else "**FIXTURE FAILED**"
        report_lines.append(f"[{status}] {name}{(' -- ' + detail) if detail else ''}")
        if not ok:
            all_ok = False

    def gate_on(text: str, test_path: str = _FIXTURE_TEST_PATH) -> GateResult:
        with tempfile.NamedTemporaryFile("w", suffix=".log", delete=False, encoding="utf-8") as f:
            f.write(text)
            tmp_path = f.name
        try:
            return run_gate(tmp_path, test_path=test_path)
        finally:
            Path(tmp_path).unlink(missing_ok=True)

    # 1. Clean corpus: two attacks, applied/resolved pairs in order, identity holds -> PASS.
    clean = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
        _resolved_line(9, (5, 2), turn=2), _applied_attack_line(9, (5, 2), turn=2),
    ])
    r = gate_on(clean)
    record("clean corpus passes", r.passed,
           f"expected PASS, got {'PASS' if r.passed else 'FAIL'}")

    # 2. Ordering scramble: SAME multiset of (unit, hex) pairs, wrong order -> must FAIL.
    #    A naive count-only or set-only comparison would pass this; ordered identity must not.
    scrambled = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(9, (5, 2)),   # mismatched pair 0
        _resolved_line(9, (5, 2), turn=2), _applied_attack_line(10, (3, 4), turn=2),
    ])
    r = gate_on(scrambled)
    record("ordering scramble fails despite equal multiset", (not r.passed) and bool(r.pairing_mismatches),
           f"passed={r.passed}, mismatches={len(r.pairing_mismatches)}")

    # 3. The silent-path shape: an applied attack with no resolved line at all -> length
    #    mismatch, reported as a real gap, never silently absorbed.
    silent_gap = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
        _applied_attack_line(9, (5, 2), turn=2),  # no matching resolved line
    ])
    r = gate_on(silent_gap)
    record("unmatched applied attack (silent-path shape) fails", (not r.passed) and bool(r.pairing_mismatches),
           f"passed={r.passed}, mismatches={len(r.pairing_mismatches)}")

    # 4. Unmatched resolved line (more resolutions than applied attacks) -> also a real gap.
    extra_resolved = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
        _resolved_line(9, (5, 2), turn=2),  # no matching applied line
    ])
    r = gate_on(extra_resolved)
    record("unmatched resolved line fails", (not r.passed) and bool(r.pairing_mismatches),
           f"passed={r.passed}, mismatches={len(r.pairing_mismatches)}")

    # 5. Malformed STRAT-COMBAT line -> parse failure, not a skip.
    malformed = _wrap([
        "[t][1]LogStratBridge: STRAT-COMBAT resolved attacker=10 THIS IS NOT THE REST OF THE LINE",
        _applied_attack_line(10, (3, 4)),
    ])
    r = gate_on(malformed)
    record("malformed STRAT-COMBAT line is a parse failure, not silence",
           (not r.passed) and len(r.combat_parse_failures) == 1,
           f"passed={r.passed}, parse_failures={len(r.combat_parse_failures)}")

    # 6. A divergence line present -> blocking failure (Error-level fault, per project posture).
    diverging = _wrap([
        _resolved_line(10, (3, 4)),
        "[t][1]LogStratBridge: STRAT-COMBAT divergence attacker=10 defender=5 hex=3,4 turn=1 "
        "side=1 attackerSide=1 fqueried=1 flegal=1 fdist=1 fdmg=3 fdies=0 fcounter=1 fcdmg=1 "
        "diverge=1",
        _applied_attack_line(10, (3, 4)),
    ])
    r = gate_on(diverging)
    record("a divergence line fails the gate", (not r.passed) and len(r.combat_divergences) == 1,
           f"passed={r.passed}, divergences={len(r.combat_divergences)}")

    # 7. A non-terminal STRAT-AI refused line -> blocking failure.
    blocked_refusal = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
        "[t][1]LogStratPlay: STRAT-AI refused phase=apply kind=Attack unit=9 hex=5,2 def=-1 "
        "target=2 turn=2 side=1 reason=some other refusal reason entirely",
    ])
    r = gate_on(blocked_refusal)
    record("a non-terminal STRAT-AI refused line fails the gate",
           (not r.passed) and len(r.ai_blocking_refusals) == 1,
           f"passed={r.passed}, blocking_refusals={len(r.ai_blocking_refusals)}")

    # 8. The terminal handover refusal is NOT a failure on its own (positive control matching
    #    the phase-D precedent's carve-out) -- still requires the pairing itself to hold.
    terminal_ok = _wrap([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
        "[t][1]LogStratPlay: Verbose: STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 "
        "def=-1 target=-1 turn=6 side=1 reason=[T-SAVE-05] no match is running",
    ])
    r = gate_on(terminal_ok)
    record("the terminal T-SAVE-05 handover refusal alone does not fail the gate", r.passed,
           f"passed={r.passed}, terminal={len(r.ai_terminal_refusals)}, "
           f"blocking={len(r.ai_blocking_refusals)}")

    # 9. Missing markers entirely -> hard failure, never a silent full-file scan.
    no_markers = "\n".join([
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
    ])
    r = gate_on(no_markers)
    record("missing Test Started/Completed markers is a hard failure",
           (not r.passed) and r.slice_failure is not None,
           f"passed={r.passed}, slice_failure={r.slice_failure}")

    # 10. Test Started with no matching Test Completed -> hard failure (truncated log), not a
    #     silent scan to EOF.
    truncated = "\n".join([_STARTED, _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4))])
    r = gate_on(truncated)
    record("Test Started with no Test Completed is a hard failure",
           (not r.passed) and r.slice_failure is not None,
           f"passed={r.passed}, slice_failure={r.slice_failure}")

    # 11. The commentary-echo trap: an `STRAT-COMBAT` payload under `LogAutomationController:`
    #     (not `LogStratBridge:`) must be ignored entirely -- proving the category anchor, not
    #     merely asserting it.
    echoed = _wrap([
        "[t][1]LogAutomationController: STRAT-COMBAT resolved attacker=999 defender=5 hex=9,9 "
        "turn=1 side=1 attackerSide=1 fqueried=1 flegal=1 fdist=1 fdmg=3 fdies=0 fcounter=1 "
        "fcdmg=1 ahpBefore=12 ahpAfter=11 ahpMax=12 adied=0 dhpBefore=12 dhpAfter=9 dhpMax=12 "
        "ddied=0 fameBefore=100 fameAfter=100 fameCombatBefore=0 fameCombatAfter=0 agree=1 "
        "diverge=0",
        _resolved_line(10, (3, 4)), _applied_attack_line(10, (3, 4)),
    ])
    r = gate_on(echoed)
    record("an echoed STRAT-COMBAT line under LogAutomationController is ignored, and the real "
           "pairing still passes clean", r.passed and len(r.combat_resolved) == 1,
           f"passed={r.passed}, combat_resolved={len(r.combat_resolved)}")

    return all_ok, "\n".join(report_lines)


# ---------------------------------------------------------------------------
def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Pairing gate for combat-outcome milestone phase 3: asserts every "
            "'STRAT-AI applied kind=Attack' pairs 1:1, IN ORDER, by (unit,hex) identity, with a "
            "'STRAT-COMBAT resolved' line, over one isolated AI-vs-AI test's own log slice. "
            "Point this at a log carrying a run of "
            "Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound (or pass "
            "--test-path for a different test); it slices to that test's own Test "
            "Started/Completed markers and refuses to scan the whole file."
        )
    )
    parser.add_argument("log_path", nargs="?", help="Path to a Stratocracy .log file to gate.")
    parser.add_argument("--test-path", default=DEFAULT_TEST_PATH,
                         help=f"Full automation test path to slice on (default: {DEFAULT_TEST_PATH}).")
    parser.add_argument("--self-test", action="store_true",
                         help="Run the fixture self-test proving this gate can FAIL, and exit.")
    args = parser.parse_args(argv)

    if args.self_test:
        ok, report = check_self_test()
        print(report)
        print("SELF-TEST: ALL FIXTURES CORRECT" if ok else "SELF-TEST: AT LEAST ONE FIXTURE WRONG")
        return 0 if ok else 1

    if not args.log_path:
        parser.error("log_path is required unless --self-test is given")

    result = run_gate(args.log_path, test_path=args.test_path)
    print(result.render())
    return 0 if result.passed else 1


if __name__ == "__main__":
    sys.exit(main())
