#!/usr/bin/env python3
"""Vendored line-number citations in DOCUMENTATION -- the half no guard was watching.

WHY THIS EXISTS. `.github/workflows/banner-sweep.yml` and `Tools/architect/hooks/pre-commit`
both refuse a citation like `Save.h:64` -- a line number into `Source/StratRules/`, which is
vendored certified bytes. A re-vendor moves every one of those lines at once, silently, and
nothing in this repository reads a vendored line number to notice. 58 had accumulated by
2026-08-19 and were replaced with symbol references.

Both of those guards scan `Source/` and nothing else. On 2026-08-20 a reviewer gate found the
shape had come back in the place neither looks: a record entry citing `Ui.h:326`. Sweeping with
the guards' own pattern found six MORE live citations in documentation -- five in
`.agents/ue-project-context.md`, one in `Tools/architect/state/tests.md` -- every one of them
load-bearing in a current argument. `Source/` was clean. The guard worked exactly where it
looked, and the citations simply moved to where it did not.

WHY A DOC CANNOT SIMPLY BE BANNED THE WAY `Source/` IS. This record's convention is that an old
claim STAYS, stamped, rather than being deleted -- so its honest accounts of the 58 citations it
removed necessarily WRITE THOSE CITATIONS DOWN. Twenty-one such quotations exist today and every
one is correct as history. (The count is orientation, not a gate: the tool counts what it finds
and this line is prose. It read "nineteen" until the same pass's own entries pushed it to 21 --
a figure in a comment goes stale exactly the way a figure in the record does.) A flat ban would refuse the record for telling the truth about a
defect, which is how a maintainer learns to stop running a guard. So a citation is allowed when
the text around it is TALKING ABOUT a citation, and refused when it is MAKING one.

THE WINDOW IS 300, OF WHICH 258 IS MEASURED AND 42 IS DELIBERATE MARGIN -- said that way because
an earlier version of this paragraph called the whole number "measured, not chosen" and gave a
figure that did not match the tree. `strat_banner_sweep.py` uses 220 for its own stamp window,
and 220 was tried first: it false-positives on the `Economy.good.cpp:60` entry in `state/data.md`,
whose nearest exempting word (`cited`, in "reading the code at the cited line") begins 253
characters past the END of the citation. The operative constant is larger than 253, because
`re.search` needs the WHOLE term inside the window: binary search puts the tightest value that
admits every one of the 21 historical accounts at exactly **258**. 300 is 258 plus margin, and
is still far tighter than the enclosing bullet, which in this document routinely runs past 2,000
characters.

WHAT IT CANNOT CATCH, SAID PLAINLY. A live citation planted within 300 characters of citation
vocabulary is allowed. That is the cost of the exemption and it is a real hole, not a theoretical
one -- the same trade `strat_banner_sweep.py` makes for its stamp markers. What closes it is a
reviewer reading the diff, which is what caught the original.

    python Tools/architect/strat_doc_citation_gate.py              # gate, exit 0/1
    python Tools/architect/strat_doc_citation_gate.py --explain    # + every hit and its call
    python Tools/architect/strat_doc_citation_gate.py --self-test  # prove it can FAIL
"""

from __future__ import annotations

import argparse
import io
import os
import re
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
_REPO = os.path.abspath(os.path.join(_HERE, os.pardir, os.pardir))

VENDORED_DIR = os.path.join(_REPO, "Source", "StratRules")

# THE ROOTS ARE DECLARED; THE FILES UNDER THEM ARE DERIVED. Which directories hold prose an
# agent reads as current is a judgement, so it is written down deliberately -- the same shape
# as `strat_banner_sweep.py`'s TRACKED_ITEMS. What is NOT typed is the file list inside them,
# nor the vendored unit names, because those are the two things that go stale on their own.
DOC_ROOTS = (
    os.path.join("Tools", "architect", "state"),
    ".agents",
    ".claude",
    "CLAUDE.md",
)

# `Source/` is NOT here: the pre-commit hook and the CI step already ban citations there
# outright, with no exemption, and duplicating that with a weaker rule would be a downgrade.
# `Tools/architect/state.md` is NOT here either -- it is frozen history, is not edited, and
# its citations are the record of the defect this guard exists for.

_WINDOW = 300

# ONLY WORDS THAT NAME THE SUBJECT. The first version of this list also carried `REFUSED`,
# `CORRECTED`, `STAMPED`, `rendered as`, `restored as`, `still carried` and `rotted` -- this
# record's general editorial markers, which appear in entries about anything at all. A reviewer
# gate smuggled live citations past every one of them in prose this record plausibly contains:
#
#     **[CORRECTED 2026-08-21]** The terrain-seeding loop at `Replay.good.cpp:299-308` ...
#     The commit was REFUSED by the hook. The bridge reads the unit id from `Save.h:64` ...
#     **[STAMPED 2026-08-20]** Phase 4 closed. The forecast path still delegates through ...
#
# All three were EXEMPT. Dropping the seven costs nothing, measured rather than assumed: all 21
# historical accounts in the record stay exempt on the six terms below, and none becomes a
# finding. A term that never exempts anything only widens the hole.
#
# `planted` is the one term here that does not name a citation. It is kept because two real
# accounts depend on it alone, and it is the weakest entry in this list -- if it ever exempts
# something it should not, drop it and reword those two.
_EXEMPT_RE = re.compile(
    r"citation|cited|cites|symbol reference|line number|planted",
    re.I)


def _rel(path: str, start: str) -> str:
    """`os.path.relpath` RAISES ACROSS WINDOWS DRIVE LETTERS, and this gate is run that way.

    The pre-commit hook materialises the staged documents into a temp tree -- which is on C:
    under this project's scratch convention -- while `--vendored-dir` still points at the real
    checkout on E:. `relpath` then dies with "path is on mount 'E:', start on mount 'C:'".
    Measured 2026-08-20 the first time the hook ran it: the traceback aborted the gate, the
    hook read a non-zero exit as a finding, and it REFUSED a clean commit while printing a
    message about a citation that did not exist. Failing closed is the right direction; saying
    the wrong thing about why is not. This is display only -- nothing branches on it.
    """
    try:
        return os.path.relpath(path, start)
    except ValueError:
        return path


def vendored_units(vendored_dir: str = VENDORED_DIR) -> tuple[list[str], str]:
    """Derived from the vendored directory, never typed.

    The CI step's FIRST version hardcoded an alternation, omitted `Replay` and `Scenario`, and
    printed `clean` over twelve live citations until a reviewer gate caught it. That is the
    single most repeated defect in this project's guards, so the list is read off disk here and
    an empty answer is a hard refusal rather than a vacuous pass.
    """
    if not os.path.isdir(vendored_dir):
        return [], f"NO VENDORED DIRECTORY at {vendored_dir}"
    units = sorted(f[:-2] for f in os.listdir(vendored_dir) if f.endswith(".h"))
    if not units:
        return [], f"NO .h FILES in {vendored_dir}"
    return units, ""


def citation_re(units: list[str]) -> re.Pattern:
    # `.good.cpp` and `.buggy.cpp` are vendored too and rot identically. The CI step already
    # covers all three; a doc rule that covered only `.h` would be the weaker sibling, which is
    # this project's other recurring guard defect.
    return re.compile(r"(?:%s)\.(?:h|good\.cpp|buggy\.cpp):[0-9]+(?:-[0-9]+)?"
                      % "|".join(re.escape(u) for u in units))


def doc_files(repo: str = _REPO, roots: tuple[str, ...] = DOC_ROOTS) -> list[str]:
    out: list[str] = []
    for root in roots:
        full = os.path.join(repo, root)
        if os.path.isfile(full):
            out.append(full)
            continue
        for dirpath, _dirs, files in os.walk(full):
            for f in files:
                if f.endswith(".md"):
                    out.append(os.path.join(dirpath, f))
    return sorted(out)


def scan_text(text: str, cite: re.Pattern) -> list[tuple[int, str, bool, str]]:
    """(line_no, citation, exempt, snippet) for every vendored citation in one document."""
    hits = []
    for m in cite.finditer(text):
        window = text[max(0, m.start() - _WINDOW):m.end() + _WINDOW]
        exempt = _EXEMPT_RE.search(window) is not None
        line_no = text[:m.start()].count("\n") + 1
        snippet = " ".join(text[max(0, m.start() - 60):m.end() + 60].split())
        hits.append((line_no, m.group(0), exempt, snippet))
    return hits


def run(repo: str = _REPO, roots: tuple[str, ...] = DOC_ROOTS,
        vendored_dir: str = VENDORED_DIR) -> tuple[int, list[str], list[str]]:
    """Returns (exit_code, failure_lines, note_lines)."""
    notes: list[str] = []
    fails: list[str] = []

    units, why = vendored_units(vendored_dir)
    if not units:
        return 1, [f"[**NOTHING SCANNED**] {why} -- the vendored unit list could not be derived, "
                   f"so this gate checked nothing at all. That is a broken guard, not a clean "
                   f"tree."], notes
    notes.append(f"vendored units derived from {_rel(vendored_dir, repo)}: "
                 f"{'|'.join(units)}")

    cite = citation_re(units)
    files = doc_files(repo, roots)
    if not files:
        return 1, [f"[**NOTHING SCANNED**] no .md files found under {list(roots)} -- this gate "
                   f"checked nothing at all."], notes
    notes.append(f"documents scanned: {len(files)}")

    total = exempt_n = 0
    for path in files:
        try:
            text = io.open(path, encoding="utf-8", errors="replace").read()
        except OSError as exc:                                    # pragma: no cover
            fails.append(f"[**UNREADABLE**] {path}: {exc} -- an unread file is not a clean file")
            continue
        rel = _rel(path, repo).replace("\\", "/")
        for line_no, text_hit, exempt, snippet in scan_text(text, cite):
            total += 1
            if exempt:
                exempt_n += 1
                continue
            fails.append(
                f"[**VENDORED LINE CITATION**] {rel}:{line_no}  `{text_hit}`\n"
                f"    ...{snippet}...\n"
                f"    A re-vendor moves that line and nothing reads a vendored line number to "
                f"notice. Cite the SYMBOL instead -- the enclosing function, struct or comment "
                f"block. If this is an account of a past citation rather than a live one, say so "
                f"in words near it (\"cited\", \"citation\", \"CORRECTED\", ...) and it is allowed."
            )
    notes.append(f"citations found: {total} ({exempt_n} read as historical, "
                 f"{total - exempt_n} as live)")
    return (1 if fails else 0), fails, notes


# ---------------------------------------------------------------------------
_LIVE = """# A note

**Units.** `applyCommand` uses it as a raw index into the definitions vector
(`Replay.good.cpp:486-487`). So a different order resolves to a different type.
"""

_HISTORICAL = """# A record entry

- **DISCHARGED -- the vendored line-citation sweep.** 46 citations into vendored
  headers across 16 files (`Save.h:64`, `Ui.h:346`, `Ai.h:49-53`) are now symbol
  references.
"""

# The hole, stated as a fixture rather than left for someone to find: a LIVE citation sitting
# close to citation vocabulary is allowed. This fixture exists to make that visible and to fail
# loudly if anyone ever "fixes" it by accident without deciding to.
#
# THE HOLE IS NOW MUCH NARROWER THAN IT WAS, and the fixture is kept rather than deleted so that
# stays a decision. It used to be "any entry carrying CORRECTED, STAMPED or REFUSED", which is
# most of this record; it is now "text genuinely discussing citations". A reviewer gate measured
# the difference and found the wide version bought nothing.
_LIVE_NEAR_VOCAB = """# A note

- **The vendored line-citation sweep is done.** Separately, and this IS a live
  citation the guard lets through: seeding resolves terrain by name
  (`Replay.good.cpp:299-308`).
"""

_CLEAN = """# A note

**Units.** `strat::applyCommand`'s `SaveCommandKind::Build` arm bounds-checks and
indexes with it. No line numbers here.
"""


def check_self_test() -> tuple[bool, str]:
    import tempfile
    cases = [
        ("a LIVE citation in a doc FAILS", _LIVE, False),
        ("an honest account of REMOVED citations PASSES", _HISTORICAL, True),
        ("a doc with no citation at all PASSES", _CLEAN, True),
        ("KNOWN HOLE: a live citation near citation vocabulary is ALLOWED",
         _LIVE_NEAR_VOCAB, True),
    ]
    lines: list[str] = []
    ok = True
    for name, body, want_pass in cases:
        with tempfile.TemporaryDirectory() as d:
            os.makedirs(os.path.join(d, "Source", "StratRules"))
            for u in ("Replay", "Save", "Ui", "Ai"):
                io.open(os.path.join(d, "Source", "StratRules", u + ".h"), "w").write("//\n")
            os.makedirs(os.path.join(d, "docs"))
            with io.open(os.path.join(d, "docs", "n.md"), "w",
                         encoding="utf-8", newline="\n") as fh:
                fh.write(body)
            code, fails, _notes = run(d, ("docs",),
                                      os.path.join(d, "Source", "StratRules"))
        got_pass = (code == 0)
        good = (got_pass == want_pass)
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] {name}: "
                     f"expected {'PASS' if want_pass else 'FAIL'}, "
                     f"got {'PASS' if got_pass else 'FAIL'}")

    # The two inert-guard shapes, both hard refusals rather than quiet passes.
    with tempfile.TemporaryDirectory() as d:
        os.makedirs(os.path.join(d, "docs"))
        io.open(os.path.join(d, "docs", "n.md"), "w", encoding="utf-8").write("# x\n")
        code, fails, _ = run(d, ("docs",), os.path.join(d, "Source", "StratRules"))
        good = (code == 1 and any("NOTHING SCANNED" in f for f in fails))
        # AND IT MUST NOT CLAIM A CITATION WHILE DOING SO. The two fixtures below used to assert
        # the exit code alone, which is how the false summary survived them.
        good = good and not any("VENDORED LINE CITATION" in f for f in fails)
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] a MISSING vendored directory "
                     f"refuses, and does NOT claim a citation while doing it")

    with tempfile.TemporaryDirectory() as d:
        os.makedirs(os.path.join(d, "Source", "StratRules"))
        io.open(os.path.join(d, "Source", "StratRules", "Ui.h"), "w").write("//\n")
        os.makedirs(os.path.join(d, "docs"))
        code, fails, _ = run(d, ("docs",), os.path.join(d, "Source", "StratRules"))
        good = (code == 1 and any("NOTHING SCANNED" in f for f in fails)
                and not any("VENDORED LINE CITATION" in f for f in fails))
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] a doc root with NO .md files "
                     f"refuses, and does NOT claim a citation while doing it")

    # And that the unit list is genuinely derived: a NEW vendored header is covered without
    # editing this script.
    with tempfile.TemporaryDirectory() as d:
        os.makedirs(os.path.join(d, "Source", "StratRules"))
        io.open(os.path.join(d, "Source", "StratRules", "Brandnew.h"), "w").write("//\n")
        os.makedirs(os.path.join(d, "docs"))
        io.open(os.path.join(d, "docs", "n.md"), "w", encoding="utf-8").write(
            "# x\n\nSee `Brandnew.h:42` for the rule.\n")
        code, fails, _ = run(d, ("docs",), os.path.join(d, "Source", "StratRules"))
        good = (code == 1 and any("Brandnew.h:42" in f for f in fails))
        ok = ok and good
        lines.append(f"    [{'OK' if good else '**WRONG**'}] a NEW vendored header is covered "
                     f"without editing this script")

    return ok, "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(
        description="Refuse vendored line-number citations in documentation, where the "
                    "Source/-only guards do not look.")
    # --repo AND --vendored-dir EXIST SO THE PRE-COMMIT HOOK CAN OBEY ITS OWN RULE. That hook's
    # whole principle is that it checks the STAGED bytes and not the working tree, because
    # `git commit` records the index -- a partially-staged doc is a different document from the
    # one on disk. So the hook extracts the staged docs into a temp tree and points --repo at it,
    # while --vendored-dir keeps pointing at the real Source/StratRules: the unit list is derived
    # from vendored files that are not part of the commit being checked.
    ap.add_argument("--repo", default=_REPO,
                    help="Tree to scan (default: this checkout). The hook passes a temp tree "
                         "of STAGED document blobs.")
    ap.add_argument("--vendored-dir", default=VENDORED_DIR,
                    help=f"Where the vendored headers live (default: {VENDORED_DIR}). The unit "
                         f"list is derived from it and never typed.")
    ap.add_argument("--explain", action="store_true",
                    help="Print every citation found and this gate's historical/live call.")
    ap.add_argument("--self-test", action="store_true",
                    help="Run the inline fixtures proving this gate can FAIL, and exit.")
    args = ap.parse_args(argv)

    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")   # type: ignore[union-attr]
    except Exception:                                                # pragma: no cover
        pass

    if args.self_test:
        ok, report = check_self_test()
        print(report)
        print("SELF-TEST: ALL FIXTURES CORRECT" if ok else "SELF-TEST: AT LEAST ONE FIXTURE WRONG")
        return 0 if ok else 1

    code, fails, notes = run(args.repo, DOC_ROOTS, args.vendored_dir)
    print("Doc citation gate")
    for n in notes:
        print(f"  {n}")
    if args.explain:
        units, _ = vendored_units(args.vendored_dir)
        if units:
            cite = citation_re(units)
            for path in doc_files(args.repo):
                text = io.open(path, encoding="utf-8", errors="replace").read()
                rel = _rel(path, args.repo).replace("\\", "/")
                for line_no, hit, exempt, snippet in scan_text(text, cite):
                    print(f"    {rel}:{line_no}  "
                          f"{'historical' if exempt else 'LIVE      '}  {hit}")
    if fails:
        print("")
        for f in fails:
            print(f"  {f}")
    print("")
    # THE SUMMARY MUST NOT ASSERT A CITATION WHEN THERE ISN'T ONE, AND IT DID. This line read
    # "FAILED -- a live vendored line citation is in the prose" on EVERY non-zero exit, including
    # both `NOTHING SCANNED` paths and `UNREADABLE`, none of which involve a citation. Measured
    # in a probe repo with no `Source/StratRules/`: the gate printed `NOTHING SCANNED` and then
    # claimed a citation that did not exist.
    #
    # THE EXACT DEFECT HAD JUST BEEN FIXED ONE LEVEL UP, in the pre-commit hook's own message,
    # and was left standing here -- the fifth time in this tooling that a fix landed at one site
    # and missed its sibling. It is worse here than there, because the hook echoes this output
    # ABOVE its own careful hedge, so the false sentence is the one a reader sees first. Caught
    # by a reviewer gate, not by the two self-test fixtures that exercise these very paths and
    # assert only the exit code.
    if code == 0:
        print("DOC CITATION GATE CLEAN")
    elif any("VENDORED LINE CITATION" in f for f in fails):
        print("DOC CITATION GATE FAILED -- a live vendored line citation is in the prose")
    else:
        print("DOC CITATION GATE FAILED -- it could not check what it was asked to check; "
              "see above. A check that did not run is not a check that passed.")
    return code


if __name__ == "__main__":
    sys.exit(main())
