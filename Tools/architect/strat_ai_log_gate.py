"""STRAT-AI log field parser and machine-repeatable AI-vs-AI gate -- AI-opponent phase D, piece (a).

Parses the three `STRAT-AI` format strings that `Source/StratPlay/StratAiTurnRunner.cpp` is the
SOLE holder of (re-read directly from that file, not transcribed from `state.md`, for this
module):

    STRAT-AI applied kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d hash=%s
    STRAT-AI refused phase=%s kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d reason=%s
    STRAT-AI turn-ended side=%d turn=%d commands=%d hash=%s

`phase` (refused lines only) is one of `decide` | `apply` | `bound` | `handover`.

This module returns STRUCTURED FIELDS, not a boolean match -- the three dataclasses below
(`StratAiApplied`, `StratAiRefused`, `StratAiTurnEnded`) are the parser's contract, and
`Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields` (written by `strat-test-author`
in phase D, at `StratAiMatchClauses.cpp:922`, green in the 87/87 suite) asserts against this exact
field shape. Field names and types were
chosen to match the format string's own field names verbatim, so a reader can go from a log line
to a dataclass attribute with no translation table.

A line beginning `STRAT-AI ` that does not match one of the three known shapes is a
`StratAiParseFailure`, not a skip. A format drift that makes every `STRAT-AI` line unparseable
must show up as N parse failures, never as "zero faults found" -- see the module docstring on
`run_gate` for how this is kept true even when literally nothing else in the log fails.

Nothing in this module writes to `Data/` or `Source/`. It only reads a log path given on argv.
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Iterator, Union

# ---------------------------------------------------------------------------
# The field contract. Three shapes, one failure type.
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class StratAiApplied:
    """`STRAT-AI applied ...` -- one AI command that reached `strat::GameState`."""

    kind: str        # "Build" | "Move" | "Attack" | "EndTurn" (never "Unknown"/"None" -- see notes)
    unit: int         # UnitId, or -1 (INDEX_NONE) where the kind carries none
    hex_x: int        # Hex.X, or -1
    hex_y: int        # Hex.Y, or -1
    def_index: int    # DefIndex (Build only), or -1 -- the §4.10 load-bearing raw index
    target: int       # TargetId (Attack only), or -1
    turn: int         # read before submission
    side: int         # read before submission
    hash: str         # StateHash() read AFTER submission


@dataclass(frozen=True)
class StratAiRefused:
    """`STRAT-AI refused ...` -- one of the AI turn's four fault arms."""

    phase: str        # "decide" | "apply" | "bound" | "handover"
    kind: str          # "Build" | "Move" | "Attack" | "EndTurn" | "None" (None on decide/bound/handover)
    unit: int
    hex_x: int
    hex_y: int
    def_index: int
    target: int
    turn: int
    side: int
    reason: str        # free text; may itself start with a bracketed acceptance ID, e.g. "[T-SAVE-05] ..."


@dataclass(frozen=True)
class StratAiTurnEnded:
    """`STRAT-AI turn-ended ...` -- the one successful exit from `FStratAiTurnRunner::RunTurn`."""

    side: int          # the side that just played, captured at entry
    turn: int          # the turn the AI PLAYED (not the turn an applied EndTurn advanced into)
    commands: int       # CommandsApplied for this turn
    hash: str           # StateHash() read AFTER the closing EndTurn


@dataclass(frozen=True)
class StratAiParseFailure:
    """A line that begins `STRAT-AI ` but matches none of the three known shapes.

    This is reported, never silently dropped -- a format drift must surface as N of these, not
    as an empty result set that reads like a clean run.
    """

    line_no: int
    raw: str
    reason: str


StratAiEvent = Union[StratAiApplied, StratAiRefused, StratAiTurnEnded]
StratAiParsed = Union[StratAiEvent, StratAiParseFailure]

# ---------------------------------------------------------------------------
# The three patterns, anchored full-line, mirroring the C++ format strings field for field.
# ---------------------------------------------------------------------------

_INT = r"-?\d+"
_TOKEN = r"\S+"  # kind / phase / hash never contain whitespace in the C++ formatter

_APPLIED_RE = re.compile(
    r"^STRAT-AI applied kind=(?P<kind>" + _TOKEN + r") unit=(?P<unit>" + _INT + r") "
    r"hex=(?P<hex_x>" + _INT + r"),(?P<hex_y>" + _INT + r") def=(?P<def_index>" + _INT + r") "
    r"target=(?P<target>" + _INT + r") turn=(?P<turn>" + _INT + r") side=(?P<side>" + _INT + r") "
    r"hash=(?P<hash>" + _TOKEN + r")$"
)

_REFUSED_RE = re.compile(
    r"^STRAT-AI refused phase=(?P<phase>" + _TOKEN + r") kind=(?P<kind>" + _TOKEN + r") "
    r"unit=(?P<unit>" + _INT + r") hex=(?P<hex_x>" + _INT + r"),(?P<hex_y>" + _INT + r") "
    r"def=(?P<def_index>" + _INT + r") target=(?P<target>" + _INT + r") turn=(?P<turn>" + _INT + r") "
    r"side=(?P<side>" + _INT + r") reason=(?P<reason>.+)$"
)

_TURN_ENDED_RE = re.compile(
    r"^STRAT-AI turn-ended side=(?P<side>" + _INT + r") turn=(?P<turn>" + _INT + r") "
    r"commands=(?P<commands>" + _INT + r") hash=(?P<hash>" + _TOKEN + r")$"
)


_PRODUCTION_CATEGORY = "LogStratPlay:"


def find_strat_ai_payload(line: str) -> str | None:
    """Slices a raw log line (with any `[timestamp][frame]LogStratPlay: Warning:` prefix UE
    prepends) down to the `STRAT-AI ...` payload. Returns `None` if the line carries no such
    payload at all -- that is a skip, not a failure, because most log lines are not `STRAT-AI`
    lines and were never meant to be.

    Anchored on the `LogStratPlay:` category -- the ONLY category `StratAiTurnRunner.cpp` logs
    through -- not on the bare substring `STRAT-AI`. The automation harness itself echoes lines
    like:

        LogAutomationController: Suppressed expected ('Warning') level log message or higher
        matching 'STRAT-AI refused' 1 times.

    which contains the substring `STRAT-AI` but is commentary ABOUT a suppressed expected-error
    match, not a production log line -- there is no `LogStratPlay:` category on it, and its
    payload (`STRAT-AI refused' 1 times.`, note the truncation and stray quote) is not one of the
    three known shapes. A bare-substring search parses the harness's commentary as if it were the
    runner's own output. Anchoring on the category, not on excluding `LogAutomationController`
    specifically, means this stays correct against any OTHER category that happens to mention
    `STRAT-AI` in prose -- the allowlist is one line, not an ever-growing blocklist.

    This does not weaken the parse-failure guarantee: a genuine `LogStratPlay:` line that is
    itself malformed still reaches `parse_strat_ai_payload` and is still reported as a
    `StratAiParseFailure`, because this function only chooses WHERE to start reading the payload
    from, never whether the result is well-formed.
    """
    cat_idx = line.find(_PRODUCTION_CATEGORY)
    if cat_idx == -1:
        return None
    idx = line.find("STRAT-AI ", cat_idx)
    if idx == -1:
        return None
    return line[idx:].rstrip("\r\n")


def parse_strat_ai_payload(payload: str, line_no: int = -1) -> StratAiParsed:
    """Parses one `STRAT-AI ...` payload (as returned by `find_strat_ai_payload`) into one of
    the three structured events, or a `StratAiParseFailure` if it matches none of them.
    """
    m = _APPLIED_RE.match(payload)
    if m:
        g = m.groupdict()
        return StratAiApplied(
            kind=g["kind"],
            unit=int(g["unit"]),
            hex_x=int(g["hex_x"]),
            hex_y=int(g["hex_y"]),
            def_index=int(g["def_index"]),
            target=int(g["target"]),
            turn=int(g["turn"]),
            side=int(g["side"]),
            hash=g["hash"],
        )

    m = _REFUSED_RE.match(payload)
    if m:
        g = m.groupdict()
        return StratAiRefused(
            phase=g["phase"],
            kind=g["kind"],
            unit=int(g["unit"]),
            hex_x=int(g["hex_x"]),
            hex_y=int(g["hex_y"]),
            def_index=int(g["def_index"]),
            target=int(g["target"]),
            turn=int(g["turn"]),
            side=int(g["side"]),
            reason=g["reason"],
        )

    m = _TURN_ENDED_RE.match(payload)
    if m:
        g = m.groupdict()
        return StratAiTurnEnded(
            side=int(g["side"]),
            turn=int(g["turn"]),
            commands=int(g["commands"]),
            hash=g["hash"],
        )

    return StratAiParseFailure(
        line_no=line_no,
        raw=payload,
        reason="matches no known STRAT-AI shape (applied / refused / turn-ended)",
    )


def iter_strat_ai_events(lines: Iterable[str]) -> Iterator[tuple[int, StratAiParsed]]:
    """Yields `(line_no, parsed)` for every line carrying a `STRAT-AI ` payload. Non-`STRAT-AI`
    lines are not yielded at all -- they are neither events nor failures.
    """
    for line_no, line in enumerate(lines, start=1):
        payload = find_strat_ai_payload(line)
        if payload is None:
            continue
        yield line_no, parse_strat_ai_payload(payload, line_no=line_no)


# ---------------------------------------------------------------------------
# The terminal-refusal caveat.
#
# An AI-vs-AI game that reaches a §2.8 result MID-TURN ends with the rules module correctly
# refusing the winning side's closing EndTurn:
#
#   STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=6 side=1 \
#       reason=[T-SAVE-05] no match is running
#
# `RunAiTurnsNow` returns false for this -- a game that finished CORRECTLY -- so the gate cannot
# use that return value (state.md, AI-opponent phase B, "The deferred finding"), and it cannot
# treat "any refused line" as a blanket failure either. This function is the ONLY place that
# carve-out is expressed, and it is narrow on purpose: phase, kind, and the acceptance ID inside
# `reason` must all match, not just the phrase "no match is running" alone, so an unrelated
# refusal that happens to end in similar words is never waved through.
#
# `Source/StratPlay/Tests/StratAiMatchClauses.cpp:610` pins the SAME fact for the synchronous
# test suite -- `AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ..., Occurrences 0)`, inside
# `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` (:581) -- as a tripwire
# against this ever being silently fixed upstream. `Occurrences = 0` in UE 5.8's
# `AddExpectedMessagePlain` does NOT mean "expect zero" -- it means "the message must be seen ONE
# OR MORE times, with no upper limit, or the test fails" (`AutomationTest.h:1817-1819`; the clause's
# own comment says the same thing at `:607-609`). So this tripwire is not a fixture that happens to
# see no refusals: it is a clause that DRIVES the real production subsystem
# (`UStratMatchSubsystem::RunAiTurnsNow`). What `:610`'s "Occurrences 0" actually REQUIRES, precisely,
# is one or more `Warning`-or-higher messages CONTAINING THE PLAIN SUBSTRING "STRAT-AI refused" --
# not specifically the terminal `[T-SAVE-05]` refusal. The clause's sibling at `:934-938`
# (`HandoverRefusalCarriesTheFixedFields`) is explicit about this: it uses the same broad phrase,
# on purpose, "so that a second, unintended refusal is absorbed here rather than failing as an
# unexpected warning," and asserts the EXACT count of the narrower thing (there, `phase=handover`
# lines) separately. In THIS clause's run, the substring is satisfied by exactly one line -- the
# real terminal `[T-SAVE-05]` refusal, confirmed by `real-game/isolated-run.log` having exactly one
# production `STRAT-AI refused` line and by the harness's own echo at line 3290 reporting the
# expected-message match count as `1 times` -- so the operative claim holds FOR THIS RUN. It is not
# a structural guarantee: if `T-SAVE-05` were fixed *and* some unrelated refusal appeared in the
# same run, `:610` would stay green (its "one or more" is still met) while this gate's `terminal
# refusals (ok)` count dropped to 0 -- the two would NOT go red together in that one case, unlike
# the strict claim above. `Tools/architect/evidence/07-ai-opponent/
# real-game/isolated-run.log` is a captured isolated run of that very clause, and this gate's own
# output against it (`real-game/gate_output.txt`) reads `terminal refusals (ok): 1` -- the tripwire
# and this gate are proven consistent against the same real corpus, not merely argued to be. If the
# upstream `T-SAVE-05` terminal-refusal defect is ever fixed so `RunAiTurnsNow` stops emitting this
# refusal AND no other `STRAT-AI refused` line appears in the same run, `StratAiMatchClauses.cpp:610`
# FAILS (its "one or more" requirement goes unmet) and this gate's `terminal refusals (ok)` count
# drops to 0 on the next real log -- both are designed to go red together, deliberately, so the fix
# gets revisited rather than silently accepted. The one case that guarantee does not cover is stated
# above: an unrelated refusal coinciding with the T-SAVE-05 fix in the same run.
# ---------------------------------------------------------------------------

TERMINAL_REFUSAL_ACCEPTANCE_ID = "T-SAVE-05"
TERMINAL_REFUSAL_PHASE = "apply"
TERMINAL_REFUSAL_KIND = "EndTurn"
TERMINAL_REFUSAL_REASON_SUBSTRING = "no match is running"


def is_terminal_handover_refusal(refused: StratAiRefused) -> bool:
    """True only for the one refusal shape that means "the match already finished correctly",
    never for any other refusal -- see the block comment above.
    """
    return (
        refused.phase == TERMINAL_REFUSAL_PHASE
        and refused.kind == TERMINAL_REFUSAL_KIND
        and refused.reason.startswith(f"[{TERMINAL_REFUSAL_ACCEPTANCE_ID}]")
        and TERMINAL_REFUSAL_REASON_SUBSTRING in refused.reason
    )


# ---------------------------------------------------------------------------
# The gate.
# ---------------------------------------------------------------------------


@dataclass
class GateResult:
    log_path: str
    applied: list[tuple[int, StratAiApplied]]
    turn_ended: list[tuple[int, StratAiTurnEnded]]
    terminal_refusals: list[tuple[int, StratAiRefused]]
    blocking_refusals: list[tuple[int, StratAiRefused]]
    parse_failures: list[tuple[int, StratAiParseFailure]]
    saw_any_strat_ai_line: bool
    empty_log_blocks: bool  # True iff zero STRAT-AI lines were seen AND --allow-empty was not given

    @property
    def passed(self) -> bool:
        return not self.parse_failures and not self.blocking_refusals and not self.empty_log_blocks

    def render(self) -> str:
        lines = [
            f"STRAT-AI gate: {self.log_path}",
            f"  applied lines:            {len(self.applied)}",
            f"  turn-ended lines:         {len(self.turn_ended)}",
            f"  terminal refusals (ok):   {len(self.terminal_refusals)}",
            f"  blocking refusals:        {len(self.blocking_refusals)}",
            f"  parse failures:           {len(self.parse_failures)}",
        ]
        for line_no, failure in self.parse_failures:
            lines.append(f"    FAIL line {line_no}: unparseable STRAT-AI line: {failure.raw!r}")
        for line_no, refused in self.blocking_refusals:
            lines.append(
                f"    FAIL line {line_no}: non-terminal STRAT-AI refused "
                f"phase={refused.phase} kind={refused.kind} turn={refused.turn} "
                f"side={refused.side} reason={refused.reason!r}"
            )
        if not self.saw_any_strat_ai_line:
            prefix = "FAIL" if self.empty_log_blocks else "OK (allowed)"
            lines.append(
                f"    {prefix}: zero STRAT-AI lines found in this log at all -- either no AI "
                "side played, or this is the wrong log file. Pass --allow-empty if an AI-less "
                "run is genuinely expected."
            )
        lines.append("PASS" if self.passed else "FAIL")
        return "\n".join(lines)


def run_gate(log_path: str | Path, allow_empty: bool = False) -> GateResult:
    path = Path(log_path)
    text = path.read_text(encoding="utf-8", errors="replace")

    applied: list[tuple[int, StratAiApplied]] = []
    turn_ended: list[tuple[int, StratAiTurnEnded]] = []
    terminal_refusals: list[tuple[int, StratAiRefused]] = []
    blocking_refusals: list[tuple[int, StratAiRefused]] = []
    parse_failures: list[tuple[int, StratAiParseFailure]] = []
    saw_any = False

    for line_no, parsed in iter_strat_ai_events(text.splitlines()):
        saw_any = True
        if isinstance(parsed, StratAiApplied):
            applied.append((line_no, parsed))
        elif isinstance(parsed, StratAiTurnEnded):
            turn_ended.append((line_no, parsed))
        elif isinstance(parsed, StratAiRefused):
            if is_terminal_handover_refusal(parsed):
                terminal_refusals.append((line_no, parsed))
            else:
                blocking_refusals.append((line_no, parsed))
        elif isinstance(parsed, StratAiParseFailure):
            parse_failures.append((line_no, parsed))
        else:  # pragma: no cover -- exhaustiveness guard
            raise AssertionError(f"unhandled parsed type: {type(parsed)!r}")

    return GateResult(
        log_path=str(path),
        applied=applied,
        turn_ended=turn_ended,
        terminal_refusals=terminal_refusals,
        blocking_refusals=blocking_refusals,
        parse_failures=parse_failures,
        saw_any_strat_ai_line=saw_any,
        empty_log_blocks=(not saw_any and not allow_empty),
    )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Parse and gate a Stratocracy log's STRAT-AI lines (AI-opponent phase D). "
            "Point this at an ISOLATED, real AI-vs-AI match log -- e.g. Saved/Logs/Stratocracy.log "
            "captured while it held exactly one run of "
            "Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound, or a PIE "
            "session log covering one match. Do NOT point this at a synchronous automation-suite "
            "log (e.g. a full `RunTests Stratocracy` run): the suite deliberately manufactures "
            "refused-command fixtures as test coverage, and this gate will correctly report those "
            "as parse failures and blocking refusals -- that is expected, not a regression, and it "
            "is not a summary of suite health. Known limit: this gate has only ever been proven "
            "against a headless automation-test log; a PIE session log is a different corpus shape "
            "and the `LogStratPlay:` anchor this gate keys on has not yet been demonstrated "
            "against PIE output."
        )
    )
    parser.add_argument("log_path", help="Path to a Stratocracy .log file to gate.")
    parser.add_argument(
        "--allow-empty",
        action="store_true",
        help="Do not fail if the log contains zero STRAT-AI lines (e.g. an AI-less run).",
    )
    args = parser.parse_args(argv)

    result = run_gate(args.log_path, allow_empty=args.allow_empty)
    print(result.render())
    return 0 if result.passed else 1


if __name__ == "__main__":
    sys.exit(main())
