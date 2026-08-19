"""Fixture-verdict binding for `strat_combat_pairing_gate.py` -- closes the debt recorded in
`Tools/architect/state.md`: the checked-in `.log` fixtures under
`evidence/08-combat-pairing-gate/fixtures/` had NO re-runnable assertion binding each file to
its expected verdict. `gate_self_test_output.txt` was a hand-recorded table -- a future parser
edit could leave it silently stale and nothing would notice. It already had: `git ls-files
.../fixtures/` returns 11 tracked fixtures, `grep -c "^== " gate_self_test_output.txt` returns
10 -- the table was stale (`fail_pre_sliced_zero_events.log` missing) BEFORE this module existed.

WHAT THIS MODULE PROVES, PRECISELY. For every fixture file actually present on disk in
`fixtures/`, this module declares (a) the exact CLI invocation it must be run with and (b) the
exact verdict (PASS/FAIL) that invocation must produce, then re-runs the real gate against the
real file and checks both. Three ways this can go red, all deliberate:
  1. A fixture's expected verdict is wrong (e.g. someone edits the parser and a `pass_*`
     fixture starts failing, or vice versa) -> per-fixture mismatch, reported by name.
  2. A fixture exists on disk with no declaration -> `set(disk) - set(declared)` is non-empty,
     reported, not silently skipped.
  3. A declaration exists for a fixture no longer on disk -> `set(declared) - set(disk)` is
     non-empty, reported.
This is symmetric-difference against `Path("fixtures").glob("*.log")`, not a fixed count, so a
twelfth fixture dropped in later with no declaration is caught the same way a missing one is.

THE INVOCATION IS PART OF WHAT IS BOUND, NOT JUST THE VERDICT. Ten of the eleven fixtures are
run with `--test-path Stratocracy.Fixture.FIX-01.FixtureTest` (they carry their own
`Test Started.`/`Test Completed.` markers under that path). The eleventh,
`fail_pre_sliced_zero_events.log`, is run with `--pre-sliced` and NO `--test-path` --
`--pre-sliced` ignores `--test-path` entirely (see `run_gate`'s own docstring in
`strat_combat_pairing_gate.py`), and the file carries no automation markers to slice on in the
first place (it is empty). Declaring a single uniform invocation across all eleven would be
false for this one, so each spec below carries its own `pre_sliced` flag and the harness never
assumes uniformity.

WHAT THIS DOES NOT PROVE. This binds the CHECKED-IN `.log` fixture corpus (11 files) to their
verdicts. It says nothing about the in-script `--self-test` case count (`check_self_test()`'s
`record()` calls) -- state.md is explicit that the two counts are different numbers that have
drifted apart before, and this module derives and reports the fixture-file count only, never
the self-test case count. Run `strat_combat_pairing_gate.py --self-test` separately for that.

Exit 0 iff every declared fixture is present, every present fixture is declared, and every
fixture's actual verdict matches its declaration. Exit 1 otherwise.
"""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import strat_combat_pairing_gate as gate  # the module under test -- run_gate, not a reimplementation.

FIXTURES_DIR = (
    Path(__file__).resolve().parent
    / "evidence" / "08-combat-pairing-gate" / "fixtures"
)

FIX01_TEST_PATH = "Stratocracy.Fixture.FIX-01.FixtureTest"


@dataclass(frozen=True)
class FixtureSpec:
    filename: str
    expect_pass: bool  # True -> PASS/EXIT=0 required; False -> FAIL/EXIT=1 required.
    pre_sliced: bool = False
    test_path: str | None = FIX01_TEST_PATH  # ignored by run_gate when pre_sliced=True.
    expect_min_pairs: int | None = None


# The binding itself. One entry per checked-in fixture, both directions of set-difference are
# checked against the real directory listing at run time -- see `check_bindings()`.
FIXTURE_SPECS: tuple[FixtureSpec, ...] = (
    FixtureSpec("pass_clean_pairing.log", expect_pass=True),
    FixtureSpec("pass_commentary_echo_ignored.log", expect_pass=True),
    FixtureSpec("pass_terminal_refusal_ok.log", expect_pass=True),
    FixtureSpec("fail_ordering_scramble.log", expect_pass=False),
    FixtureSpec("fail_silent_path_gap.log", expect_pass=False),
    FixtureSpec("fail_format_drift.log", expect_pass=False),
    FixtureSpec("fail_divergence_present.log", expect_pass=False),
    FixtureSpec("fail_blocking_ai_refusal.log", expect_pass=False),
    FixtureSpec("fail_missing_markers.log", expect_pass=False),
    FixtureSpec("fail_truncated_no_completed.log", expect_pass=False),
    # The one fixture NOT run with --test-path: --pre-sliced ignores it entirely, and this file
    # is empty on disk with no Test Started/Completed markers to slice on regardless.
    FixtureSpec(
        "fail_pre_sliced_zero_events.log", expect_pass=False,
        pre_sliced=True, test_path=None,
    ),
)


@dataclass
class BindingResult:
    filename: str
    declared_expect_pass: bool
    invocation: str
    actual_passed: bool | None  # None if the file could not even be read/run.
    actual_exit: int | None
    error: str | None

    @property
    def ok(self) -> bool:
        return self.error is None and self.actual_passed == self.declared_expect_pass


def _run_one(spec: FixtureSpec) -> BindingResult:
    path = FIXTURES_DIR / spec.filename
    if spec.pre_sliced:
        invocation = f"strat_combat_pairing_gate.py {spec.filename} --pre-sliced"
        if spec.expect_min_pairs is not None:
            invocation += f" --expect-min-pairs {spec.expect_min_pairs}"
    else:
        invocation = f"strat_combat_pairing_gate.py {spec.filename} --test-path {spec.test_path}"

    if not path.exists():
        return BindingResult(
            filename=spec.filename, declared_expect_pass=spec.expect_pass,
            invocation=invocation, actual_passed=None, actual_exit=None,
            error="declared fixture is not present on disk",
        )

    result = gate.run_gate(
        path,
        test_path=(spec.test_path or gate.DEFAULT_TEST_PATH),
        pre_sliced=spec.pre_sliced,
        expect_min_pairs=spec.expect_min_pairs,
    )
    return BindingResult(
        filename=spec.filename, declared_expect_pass=spec.expect_pass,
        invocation=invocation, actual_passed=result.passed,
        actual_exit=(0 if result.passed else 1), error=None,
    )


def check_bindings() -> tuple[bool, str]:
    """Returns (all_ok, report_text). `all_ok` is False if any declared fixture's actual verdict
    disagrees with its declaration, OR if the declared set and the on-disk set differ in either
    direction."""
    lines: list[str] = []
    all_ok = True

    disk_files = sorted(p.name for p in FIXTURES_DIR.glob("*.log"))
    declared_files = sorted(spec.filename for spec in FIXTURE_SPECS)

    lines.append(f"fixtures directory: {FIXTURES_DIR}")
    lines.append(f"on-disk .log fixture count:  {len(disk_files)}")
    lines.append(f"declared binding count:      {len(declared_files)}")

    undeclared = sorted(set(disk_files) - set(declared_files))
    missing_on_disk = sorted(set(declared_files) - set(disk_files))
    if undeclared:
        all_ok = False
        lines.append(f"**UNDECLARED FIXTURES ON DISK (no binding covers these): {undeclared}")
    if missing_on_disk:
        all_ok = False
        lines.append(f"**DECLARED FIXTURES MISSING FROM DISK: {missing_on_disk}")
    if not undeclared and not missing_on_disk:
        lines.append("set(disk) == set(declared) -- every fixture on disk is bound, and vice versa.")

    lines.append("")
    for spec in FIXTURE_SPECS:
        r = _run_one(spec)
        expected_word = "PASS" if spec.expect_pass else "FAIL"
        if r.error is not None:
            all_ok = False
            lines.append(f"[**MISSING**] {spec.filename} -- expected {expected_word} -- {r.error}")
            continue
        actual_word = "PASS" if r.actual_passed else "FAIL"
        status = "OK" if r.ok else "**MISMATCH**"
        if not r.ok:
            all_ok = False
        lines.append(
            f"[{status}] {spec.filename} -- invocation: `{r.invocation}` -- "
            f"expected {expected_word}, actual {actual_word}, EXIT={r.actual_exit}"
        )

    lines.append("")
    lines.append("ALL BINDINGS HOLD" if all_ok else "AT LEAST ONE BINDING FAILED")
    return all_ok, "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    ok, report = check_bindings()
    print(report)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
