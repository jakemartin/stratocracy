#!/usr/bin/env python3
"""Gate the UE automation suite's EXPORTED REPORT, not the run's exit code or its log.

WHY THIS EXISTS AS A SCRIPT RATHER THAN THREE LINES OF YAML. Three separate instruments in
this project have already been measured lying about a suite run, and each one is a check
below:

  1. THE LOG UNDERCOUNTS BY ONE, RELIABLY. `UnrealEditor-Cmd ... -ExecCmds="Automation
     RunTests X;Quit"` writes `Saved/Logs/Stratocracy.log`, and the LAST test's
     `Test Completed` line is never flushed -- the process exits first. On 2026-08-29 that
     read as 286 of 287 clauses with zero failures, and the missing one
     (`Stratocracy.StratUI.T-UI-04.TheFiveArgumentFormIsTheSixArgumentFormMinusThePurse`)
     looked exactly like a clause that had silently stopped registering. It had not. Only
     `-ReportExportPath`'s `index.json` carries the full set.

  2. THE EXIT CODE IS NOT A VERDICT. This repository already records a gate that SKIPPED and
     exited 2, and another whose `$?` read a pipe's last stage rather than the tool's. The
     editor's own exit code is not a trustworthy statement about clause results.

  3. A REPORT ON DISK MAY BE FROM A PREVIOUS RUN. `Saved/` is gitignored, and a self-hosted
     runner's workspace is reused between jobs, so an `index.json` from an earlier run can be
     sitting there before this one starts. A gate that reads whatever is lying there will
     happily certify yesterday's green over today's red. `--not-before` pins the report to the
     run being gated.

     UNVERIFIED, AND SAID SO RATHER THAN ASSERTED: `actions/checkout` defaults to `clean:
     true`, which runs `git clean -ffdx`, and `-x` removes ignored files -- so it may well
     delete `Saved/` before any of this matters. Nobody here has executed a GitHub Actions
     runner to find out. The pin is kept regardless, because it costs nothing and is the
     correct defence for a `clean: false` runner, a report written outside the workspace, or a
     future change to that default. A guard justified by a premise nobody measured is still a
     guard; a premise stated as fact would be the defect.

AND ONE INVARIANT THAT CATCHES WHAT ALL THREE MISS: every clause is identified BY NAME. The set
of names the tree declares must equal the set the report lists, with MISSING and EXTRA reported
separately because they mean different things -- a clause that did not run, versus a clause the
binary knows and the tree does not. A clause that fails to REGISTER does not fail; it is absent,
and an absent clause is invisible to every check that only reads results.

    THIS PARAGRAPH USED TO SAY "the number of entries in the report must equal the number of
    clause macros in the tree", AND THAT IS WHAT THE GATE ACTUALLY DID until 2026-08-30.
    `strat-integration-reviewer` broke it with a fixture: a tree declaring three clauses against
    a report listing three OTHERS returned `SUITE REPORT GATE CLEAN`, exit 0 -- the stale-binary
    case, where a clause NAME is compiled so a rename leaves the count untouched. The sentence
    then survived the remediation, because the sweep for stale descriptions searched for the
    PHRASINGS already known rather than for the SHAPE of the claim, and this wording shares no
    vocabulary with them. It is retracted here rather than deleted because THIS DOCSTRING IS THE
    AUTHORITY two other files point at instead of repeating -- `build-and-suite.yml` and
    `global.md` both say so in terms -- and a single authority that rots takes every pointer
    with it, silently, with nothing in the tree able to notice.

Refuses to pass vacuously. A missing report, an unparseable report, or a source tree with no
clauses in it is a FAILURE, never a skip.
"""

# LANE NOTE, 2026-08-30. `Tools/architect/` is `strat-data-steward`'s. This file was written by
# the `coordinator` on the user's direct instruction to add a CI workflow that builds and runs
# the suite, with no lane agent, no worktree and no merge -- so it is NOT the transcription
# clause, which covers neither authoring nor a non-merge context. It is declared here, in the
# artifact that received the write, rather than only in `global.md`: a gate has already found
# that recording an out-of-lane write solely in the coordinator's own file is itself a finding.

from __future__ import annotations

import argparse
import json
import os
import re
import sys
import tempfile
from pathlib import Path

# The same macro the banner sweep censuses, but this one CAPTURES THE CLAUSE NAME -- the macro's
# second argument -- because a COUNT is not what this gate needs and the name was free.
#
# THE COUNT-ONLY VERSION OF THIS GATE PASSED A REPORT OF ENTIRELY DIFFERENT CLAUSE NAMES, and
# said `SUITE REPORT GATE CLEAN` while doing it. `strat-integration-reviewer` demonstrated it
# with a fixture: a tree declaring three clauses against a report listing three others, exit 0.
# That is the stale-binary case -- a clause NAME is compiled, so a rename leaves the count
# untouched -- which this check's own error text used to claim it caught.
#
# `^` ANCHORED AND MULTILINE. The arguments sit on the lines AFTER the macro throughout this
# project, so a line-oriented scan finds nothing; and an unanchored scan would match the macro
# quoted inside a comment, of which this repository has many.
SIMPLE_MACRO = re.compile(
    r'^IMPLEMENT_SIMPLE_AUTOMATION_TEST\s*\(\s*[^,]+,\s*"([^"]+)"', re.M
)
COMPLEX_MACRO = re.compile(
    r"^IMPLEMENT_COMPLEX_AUTOMATION_TEST\b|^IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST\b", re.M
)

# `reportCreatedOn` is UTC and formatted `YYYY.MM.DD-HH.MM.SS`. It sorts lexically, which is
# the only property this gate needs from it -- no timezone arithmetic, and therefore no chance
# of the local/UTC confusion that has already put a wrong date in this project's record.
STAMP_RE = re.compile(r"^\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2}$")


class GateFailure(Exception):
    """One reason the suite report may not be trusted."""


def census(source_root: Path) -> tuple[list[str], int]:
    """(clause names declared, complex-macro count) under `source_root`.

    A LIST AND NOT A SET, so the caller can see a name declared twice. Collapsing duplicates
    here would hide one of the things the caller exists to catch.
    """
    names: list[str] = []
    complex_ = 0
    for path in sorted(source_root.rglob("*.cpp")):
        text = path.read_text(encoding="utf-8", errors="replace")
        names.extend(SIMPLE_MACRO.findall(text))
        complex_ += len(COMPLEX_MACRO.findall(text))
    return names, complex_


def check(report_path: Path, source_root: Path, not_before: str | None) -> list[str]:
    """Returns the lines to print. Raises GateFailure on the first thing that is wrong."""
    out: list[str] = []

    if not report_path.is_file():
        raise GateFailure(
            f"no automation report at {report_path} -- refusing to pass vacuously.\n"
            "The suite did not run, or it ran without -ReportExportPath. Either way this gate\n"
            "has nothing to gate, and a silent pass here would be worse than a red run."
        )

    try:
        # utf-8-sig: the editor writes a BOM, and a plain utf-8 read fails on the first byte.
        report = json.loads(report_path.read_text(encoding="utf-8-sig"))
    except (json.JSONDecodeError, UnicodeDecodeError) as exc:
        raise GateFailure(f"{report_path} is not readable JSON: {exc}") from exc

    tests = report.get("tests")
    if not isinstance(tests, list) or not tests:
        raise GateFailure(
            f"{report_path} carries no `tests` array -- refusing to pass on an empty report."
        )

    created = report.get("reportCreatedOn", "")
    out.append(f"  report:  {report_path}")
    out.append(f"  created: {created} (UTC)")

    # ---- FRESHNESS, FIRST, because every figure below is meaningless if this is yesterday's.
    if not_before is not None:
        if not STAMP_RE.match(str(created)):
            raise GateFailure(
                f"reportCreatedOn is {created!r}, which is not a `YYYY.MM.DD-HH.MM.SS` stamp.\n"
                "Freshness cannot be established, so the report is refused rather than trusted."
            )
        if not STAMP_RE.match(not_before):
            raise GateFailure(
                f"--not-before {not_before!r} is not a `YYYY.MM.DD-HH.MM.SS` UTC stamp."
            )
        if created < not_before:
            raise GateFailure(
                f"STALE REPORT. reportCreatedOn {created} predates this run's start "
                f"{not_before}.\n"
                "`Saved/` is gitignored and survives between jobs on a self-hosted runner, so\n"
                "this is a report from an EARLIER run being offered as evidence for this one."
            )
        out.append(f"  fresh:   >= {not_before}")

    # ---- THE COUNTERS AND THE ENTRIES ARE CHECKED SEPARATELY, ON PURPOSE.
    # A summary counter is a claim the report makes about itself. The entries are the evidence.
    # Checking only the counters would trust the claim; checking only the entries would miss a
    # report that counts tests it did not list.
    failed = int(report.get("failed", 0) or 0)
    not_run = int(report.get("notRun", 0) or 0)
    warned = int(report.get("succeededWithWarnings", 0) or 0)
    succeeded = int(report.get("succeeded", 0) or 0)

    out.append(
        f"  counters: succeeded={succeeded} failed={failed} "
        f"notRun={not_run} succeededWithWarnings={warned}"
    )

    bad = [t for t in tests if t.get("state") != "Success"]
    if bad:
        lines = "\n".join(
            f"    {t.get('state', '?'):<12} {t.get('fullTestPath', '?')}" for t in bad[:20]
        )
        more = "" if len(bad) <= 20 else f"\n    ... and {len(bad) - 20} more"
        raise GateFailure(f"{len(bad)} clause(s) did not succeed:\n{lines}{more}")

    if failed or not_run or warned:
        raise GateFailure(
            f"every listed clause is Success, but the report's own counters disagree: "
            f"failed={failed}, notRun={not_run}, succeededWithWarnings={warned}.\n"
            "The report contradicts itself; it is refused rather than reconciled here."
        )

    if succeeded != len(tests):
        raise GateFailure(
            f"the report claims {succeeded} succeeded but lists {len(tests)} entries."
        )

    # ---- THE CENSUS CROSS-CHECK. This is the only check that can see a clause that never ran.
    if not source_root.is_dir():
        raise GateFailure(f"source root {source_root} does not exist -- census impossible.")

    declared, complex_ = census(source_root)
    simple = len(declared)
    out.append(f"  census:  IMPLEMENT_SIMPLE_AUTOMATION_TEST={simple}, COMPLEX/CUSTOM={complex_}")

    if simple == 0:
        raise GateFailure(
            f"no clause macros found under {source_root} -- refusing to pass vacuously.\n"
            "Either the tree is wrong or the macro this gate censuses has been renamed."
        )

    if complex_:
        # LOUD RATHER THAN SILENTLY WEAKENED. A complex test contributes MANY report entries
        # for ONE macro, so entries == macros stops holding the day the first one lands. This
        # gate refuses instead of quietly dropping its strongest check, because a guard that
        # silently stops covering its subject is the failure mode this repository has the most
        # recorded history with.
        raise GateFailure(
            f"{complex_} COMPLEX/CUSTOM automation macro(s) found under {source_root}.\n"
            "This gate identifies clauses BY NAME, and a complex macro emits one entry PER\n"
            "ROW -- so every row but the first would surface as an EXTRA clause the tree does\n"
            "not declare, which is a misdiagnosis rather than a finding. Teach this gate to\n"
            "expand them before the first one merges; it refuses rather than reporting that."
        )

    # ---- IDENTITY, NOT CARDINALITY. Comparing counts let a report of entirely DIFFERENT
    # clause names pass, which is exactly the stale-binary case: a clause NAME is compiled, so
    # a rename leaves the count identical. Set difference in both directions costs nothing --
    # the census already had the names in hand -- and it subsumes the count check.
    dup_declared = sorted({n for n in declared if declared.count(n) > 1})
    if dup_declared:
        raise GateFailure(
            "the TREE declares the same clause name more than once: "
            + ", ".join(dup_declared)
            + "\nTwo macros with one name cannot both be identified in a report, so this gate\n"
            "refuses rather than guessing which one ran."
        )

    reported = [str(t.get("fullTestPath", "")) for t in tests]
    dup_reported = sorted({n for n in reported if reported.count(n) > 1})
    if dup_reported:
        raise GateFailure(
            "the REPORT lists the same clause more than once: " + ", ".join(dup_reported)
        )

    declared_set, reported_set = set(declared), set(reported)
    missing = sorted(declared_set - reported_set)   # in the tree, absent from the run
    extra = sorted(reported_set - declared_set)     # in the run, absent from the tree

    if missing or extra:
        lines = []
        if missing:
            lines.append(
                f"  {len(missing)} clause(s) the tree DECLARES but the report does not list:"
            )
            lines += [f"    {n}" for n in missing[:20]]
            if len(missing) > 20:
                lines.append(f"    ... and {len(missing) - 20} more")
        if extra:
            lines.append(
                f"  {len(extra)} clause(s) the report LISTS but the tree does not declare:"
            )
            lines += [f"    {n}" for n in extra[:20]]
            if len(extra) > 20:
                lines.append(f"    ... and {len(extra) - 20} more")
        raise GateFailure(
            "CLAUSE SET DISAGREEMENT between the tree and the run.\n"
            + "\n".join(lines)
            + "\n\nA clause that fails to REGISTER does not go red -- it is absent, and absence\n"
            "is invisible to every results check above this one. Both halves are named because\n"
            "they mean different things: MISSING is a clause that did not run (a filtered run,\n"
            "or one that never registered); EXTRA is a clause the binary knows and the tree\n"
            "does not, which is what a STALE BINARY looks like -- a clause NAME is compiled, so\n"
            "a rename shows up here and nowhere else."
        )

    out.append(
        f"  {len(tests)}/{len(tests)} clauses Success, and every one is a name the tree declares."
    )
    return out


# --------------------------------------------------------------------------------------------
# SELF-TEST. Every fixture proves ONE check can FAIL, because a gate whose fixtures only prove
# it can pass is indistinguishable from a gate that always passes. The healthy path is a
# fixture too -- this project has shipped a guard that was tested five ways it should fail and
# zero ways it should stay quiet.
# --------------------------------------------------------------------------------------------

def _fixture_tree(tmp: Path, clause_names: list[str]) -> Path:
    """A source tree declaring one SIMPLE macro per name, written the way the real ones are."""
    src = tmp / "Source" / "Mod" / "Tests"
    src.mkdir(parents=True, exist_ok=True)
    body = ""
    for i, name in enumerate(clause_names):
        body += (
            "IMPLEMENT_SIMPLE_AUTOMATION_TEST(\n"
            f"\tFFixture{i}Test,\n"
            f'\t"{name}",\n'
            "\tEAutomationTestFlags::EditorContext)\n\n"
        )
    (src / "Clauses.cpp").write_text(body, encoding="utf-8")
    return tmp / "Source"


def _fixture_report(tmp: Path, entries: list[tuple[str, str]], created: str, **over) -> Path:
    tests = [{"fullTestPath": p, "state": s} for p, s in entries]
    doc = {
        "reportCreatedOn": created,
        "succeeded": sum(1 for _, s in entries if s == "Success"),
        "failed": sum(1 for _, s in entries if s == "Fail"),
        "notRun": sum(1 for _, s in entries if s == "NotRun"),
        "succeededWithWarnings": 0,
        "tests": tests,
    }
    doc.update(over)
    path = tmp / "index.json"
    # The editor writes a BOM; the fixtures do too, so the real read path is what is tested.
    path.write_text(json.dumps(doc), encoding="utf-8-sig")
    return path


def self_test() -> int:
    names = ["Stratocracy.Mod.ID.One", "Stratocracy.Mod.ID.Two"]
    ok = [(n, "Success") for n in names]
    cases: list[tuple[str, bool, object]] = []

    def case(label, should_pass, fn):
        cases.append((label, should_pass, fn))

    case("a clean, fresh, count-matching report PASSES", True,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), "2026.08.30-03.00.00"))

    case("no --not-before pin still PASSES (freshness is optional)", True,
         lambda t: (_fixture_report(t, ok, "1999.01.01-00.00.00"),
                    _fixture_tree(t, names), None))

    case("a FAILED clause FAILS", False,
         lambda t: (_fixture_report(t, [(names[0], "Success"), (names[1], "Fail")],
                                    "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("a NotRun clause FAILS", False,
         lambda t: (_fixture_report(t, [(names[0], "Success"), (names[1], "NotRun")],
                                    "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("all entries Success but a nonzero `failed` counter FAILS", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00", failed=1),
                    _fixture_tree(t, names), None))

    case("all entries Success but succeededWithWarnings FAILS", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00", succeededWithWarnings=1),
                    _fixture_tree(t, names), None))

    case("`succeeded` disagreeing with the entry count FAILS", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00", succeeded=99),
                    _fixture_tree(t, names), None))

    case("a report with FEWER entries than the tree declares FAILS", False,
         lambda t: (_fixture_report(t, ok[:1], "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("a report with MORE entries than the tree declares FAILS", False,
         lambda t: (_fixture_report(t, ok + [("Stratocracy.Mod.ID.Three", "Success")],
                                    "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    # THE STALE-BINARY PROBE. Same COUNT, different NAMES -- the case the count-only version of
    # this gate passed with `SUITE REPORT GATE CLEAN`, exit 0, while its error text claimed to
    # catch it. `strat-integration-reviewer` built this fixture; it is kept verbatim in shape so
    # the regression it represents can never come back unnoticed.
    case("a report of the SAME COUNT but DIFFERENT clause names FAILS (stale binary)", False,
         lambda t: (_fixture_report(t, [("Stratocracy.Mod.ID.Old1", "Success"),
                                        ("Stratocracy.Mod.ID.Old2", "Success")],
                                    "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("a report listing ONE clause twice instead of two distinct ones FAILS", False,
         lambda t: (_fixture_report(t, [(names[0], "Success"), (names[0], "Success")],
                                    "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("a TREE declaring the same clause name twice FAILS", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    _fixture_tree(t, [names[0], names[0]]), None))

    # The two checks the re-gate found unfixtured. Neither was claimed as covered, but an
    # unfixtured check is a check nobody has shown can fire.
    case("a malformed --not-before FAILS rather than being ignored", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), "not-a-stamp"))

    case("a nonexistent --source-root FAILS rather than censusing nothing", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    t / "no-such-source", None))

    case("a report OLDER than --not-before FAILS", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-02.59.59"),
                    _fixture_tree(t, names), "2026.08.30-03.00.00"))

    case("a report exactly AT --not-before PASSES (the boundary is inclusive)", True,
         lambda t: (_fixture_report(t, ok, "2026.08.30-03.00.00"),
                    _fixture_tree(t, names), "2026.08.30-03.00.00"))

    case("a malformed reportCreatedOn FAILS when freshness is pinned", False,
         lambda t: (_fixture_report(t, ok, "yesterday"),
                    _fixture_tree(t, names), "2026.08.30-03.00.00"))

    case("a MISSING report FAILS rather than skipping", False,
         lambda t: (t / "nope.json", _fixture_tree(t, names), None))

    case("an unparseable report FAILS", False,
         lambda t: (_write(t / "index.json", "{not json"), _fixture_tree(t, names), None))

    case("an EMPTY tests array FAILS", False,
         lambda t: (_fixture_report(t, [], "2026.08.30-04.00.00"),
                    _fixture_tree(t, names), None))

    case("a source tree with NO clause macros FAILS rather than passing vacuously", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    _empty_tree(t), None))

    case("a COMPLEX macro FAILS loudly instead of misdiagnosing its rows as EXTRA", False,
         lambda t: (_fixture_report(t, ok, "2026.08.30-04.00.00"),
                    _complex_tree(t, names), None))

    bad = 0
    for label, should_pass, build in cases:
        with tempfile.TemporaryDirectory() as td:
            report, root, pin = build(Path(td))
            try:
                check(Path(report), Path(root), pin)
                got_pass, why = True, ""
            except GateFailure as exc:
                got_pass, why = False, str(exc).splitlines()[0]
            if got_pass == should_pass:
                verdict = "PASS" if got_pass else f"FAIL -- {why}"
                print(f"    [OK] {label}: expected "
                      f"{'PASS' if should_pass else 'FAIL'}, got {verdict}")
            else:
                bad += 1
                print(f"    [BAD] {label}: expected "
                      f"{'PASS' if should_pass else 'FAIL'}, got "
                      f"{'PASS' if got_pass else 'FAIL'}")

    if bad:
        print(f"SELF-TEST: {bad} FIXTURE(S) WRONG")
        return 1
    print("SELF-TEST: ALL FIXTURES CORRECT")
    return 0


def _write(path: Path, text: str) -> Path:
    path.write_text(text, encoding="utf-8")
    return path


def _empty_tree(tmp: Path) -> Path:
    d = tmp / "Source" / "Mod"
    d.mkdir(parents=True, exist_ok=True)
    (d / "NotATest.cpp").write_text("int main() { return 0; }\n", encoding="utf-8")
    return tmp / "Source"


def _complex_tree(tmp: Path, names: list[str]) -> Path:
    root = _fixture_tree(tmp, names)
    (root / "Mod" / "Tests" / "Complex.cpp").write_text(
        "IMPLEMENT_COMPLEX_AUTOMATION_TEST(\n\tFC,\n\t\"Stratocracy.Mod.ID.C\",\n\t0)\n",
        encoding="utf-8",
    )
    return root


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--report", default=os.path.join("Saved", "AutomationReport", "index.json"))
    ap.add_argument("--source-root", default="Source")
    ap.add_argument(
        "--not-before",
        default=None,
        help="UTC `YYYY.MM.DD-HH.MM.SS` taken BEFORE the suite launched. A report older than "
             "this is refused as belonging to an earlier run.",
    )
    ap.add_argument("--self-test", action="store_true")
    args = ap.parse_args()

    if args.self_test:
        return self_test()

    print("Suite report gate")
    try:
        for line in check(Path(args.report), Path(args.source_root), args.not_before):
            print(line)
    except GateFailure as exc:
        print("")
        print("SUITE REPORT GATE FAILED")
        print(str(exc))
        return 1

    print("")
    print("SUITE REPORT GATE CLEAN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
