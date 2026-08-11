"""The Stratocracy Code Architect -- a goal-oriented coding agent.

    scan -> diff against the GDD -> score -> write -> re-scan, and stop when done.

Raw orchestration: no agent framework, one direct Messages API call per generation
step, every decision printed to the blackboard before it takes effect.

    python architect.py                     # propose only -- nothing touches the tree
    python architect.py --apply             # write accepted files, then re-scan and loop
    python architect.py --offline           # replay recorded calls; no API key needed
    python architect.py --diff-report RUN   # what changed between generated and landed

THE HUMAN GATE IS THE DEFAULT. Without `--apply` the agent writes only into its own
run directory. That is deliberate: Class 7's own instruction is to read every generated
file before it goes into the game, and an agent whose default is to write into the tree
makes that step something you have to remember rather than something you have to skip.

WHY `--apply` IS ALSO WHAT ENABLES ITERATION. The loop re-scans after each write, and
the second iteration's scores depend on what the first one changed. With nothing
applied there is nothing to re-scan, so a propose-only run is one iteration by
construction rather than by a separate flag.
"""

from __future__ import annotations

import argparse
import difflib
import sys
from datetime import datetime, timezone
from pathlib import Path

import candidates as candidates_mod
import gdd as gdd_mod
import prompts as prompts_mod
import scan as scan_mod
import score as score_mod
from blackboard import Blackboard
from write import DEFAULT_MODEL, Generator, load_api_key

HERE = Path(__file__).resolve().parent
GAME_REPO = HERE.parent.parent                      # ...\Stratocracy
WORKSPACE = GAME_REPO.parent                        # ...\MultiAgent
# The GDD lives in a SIBLING REPOSITORY, so a grader who clones only the game repo does
# not have it -- and this agent cannot run a single step without one, since the scorer's
# dependency edges are §4.11's table and the prompts quote §4.7 verbatim. The live
# document is preferred and a vendored snapshot is the fallback, in that order, so a
# working checkout always reads the current GDD and never the copy. See
# gdd_snapshot/MANIFEST.md for what that copy is and is not.
LIVE_GDD = WORKSPACE / "stratocracy-content" / "Stratocracy_Prototype_GDD.md"
SNAPSHOT_GDD = HERE / "gdd_snapshot" / "Stratocracy_Prototype_GDD.md"
DEFAULT_GDD = LIVE_GDD if LIVE_GDD.is_file() else SNAPSHOT_GDD
ENV_FILES = [WORKSPACE / "stratocracy-crew" / ".env", HERE / ".env"]


def stamp() -> str:
    return datetime.now(timezone.utc).strftime("%Y%m%d-%H%M%S")


# ---------------------------------------------------------------------------
# Reporting
# ---------------------------------------------------------------------------

def report_perception(bb: Blackboard, gddf: gdd_mod.GddFacts, code: scan_mod.CodebaseFacts) -> None:
    bb.header("01 RECEIVE -- the GDD")
    bb.bullet(f"read `{gddf.path}` ({gddf.size:,} chars)"
              + ("" if gddf.path != SNAPSHOT_GDD
                 else " — the VENDORED SNAPSHOT; the live document was not found at "
                      f"`{LIVE_GDD}`"))
    bb.bullet(f"§4.11 build order: {len(gddf.rows)} ledger rows parsed")
    bb.bullet(f"§4.7 stub invariants: {len(gddf.invariants)} acceptance IDs defined")
    declared = gddf.all_ids()
    bb.bullet(f"{len(declared)} acceptance IDs declared across the build order")

    bb.header("02 SCAN -- the codebase")
    bb.bullet(f"root `{code.root}` -- {len(code.files)} source files under Source/")
    bb.table(
        ["module", "depends on"],
        [[m.name, ", ".join(m.deps) or "—"] for m in code.modules.values()],
    )
    bb.say("Acceptance IDs asserted by a test **in this tree**:")
    if code.asserted_ids:
        for i, where in sorted(code.asserted_ids.items()):
            bb.bullet(f"`{i}` — {', '.join(where)}")
    else:
        bb.bullet("none")

    bb.say("\nCapability probes — what the code can actually reach today:")
    bb.table(
        ["", "probe", "question", "evidence"],
        [
            ["YES" if p.present else "NO", f"`{p.name}`", p.question, p.evidence]
            for p in code.probes.values()
        ],
    )


def report_scores(bb: Blackboard, ranked: list[score_mod.Score]) -> None:
    bb.header("03 SCORE -- what to build, and why")
    bb.say(
        "Weights are module constants in `score.py`: "
        f"unblocks {score_mod.W_UNBLOCKS:g}/each, open acceptance ID "
        f"{score_mod.W_OPEN_ID:g}/each, †-marked ID {score_mod.W_DAGGER:g} extra, "
        f"§4.11 dependant row {score_mod.W_CRITICAL_PATH:g}/each, partially built "
        f"{score_mod.W_PARTIAL:g}, lands in this repo {score_mod.W_IN_REPO:g}."
    )
    bb.table(
        ["#", "score", "status", "candidate", "closes"],
        [
            [
                n,
                s.label,
                ("ACTIONABLE" if s.actionable else "blocked") if s.total is not None
                else "excluded",
                f"`{s.candidate.key}`",
                ", ".join(s.candidate.open_ids) or "—",
            ]
            for n, s in enumerate(ranked, 1)
        ],
    )
    for n, s in enumerate(ranked, 1):
        bb.section(f"{n}. {s.candidate.title} — score {s.label}")
        if s.components:
            bb.bullet("components: " + ", ".join(
                f"{k} {v:g}" for k, v in s.components.items() if v))
        for r in s.reasons:
            bb.bullet(r)


# ---------------------------------------------------------------------------
# Applying
# ---------------------------------------------------------------------------

def apply_files(bb: Blackboard, code_root: Path, generation, dry_run: bool) -> list[Path]:
    landed: list[Path] = []
    bb.header("04 BUILD -- proposed files")
    if not generation.files:
        bb.bullet("nothing proposed.")
        return landed

    for f in generation.files:
        target = code_root / f.path
        existed = target.is_file()
        before = target.read_text(encoding="utf-8", errors="replace") if existed else ""
        verb = "REWRITES" if existed else "CREATES"
        bb.bullet(f"{verb} `{f.path}` ({len(f.body)} bytes)")

        if existed:
            diff = list(difflib.unified_diff(
                before.splitlines(True), f.body.splitlines(True),
                fromfile=f"a/{f.path}", tofile=f"b/{f.path}", n=2,
            ))
            bb.bullet(f"  {sum(1 for d in diff if d.startswith('+') and not d.startswith('+++'))} "
                      f"lines added, "
                      f"{sum(1 for d in diff if d.startswith('-') and not d.startswith('---'))} "
                      f"removed vs the tree")

        if dry_run:
            continue
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(f.body, encoding="utf-8", newline="\n")
        landed.append(target)

    if dry_run:
        bb.say(
            "\n**Nothing was written to the tree.** This is a propose-only run; the "
            "generated files are under this run's `generated/`. Re-run with `--apply` "
            "after reading them."
        )
    return landed


def write_state(path: Path, built: list[str], decisions: list[str], nxt: list[str]) -> None:
    """The markdown state file -- Class 7's BUILT / DECISIONS / NEXT."""
    lines = [
        "# Architect state",
        "",
        f"_Last run {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC')}._",
        "",
        "## BUILT",
        "",
        *([f"- {b}" for b in built] or ["- nothing yet"]),
        "",
        "## DECISIONS",
        "",
        *([f"- {d}" for d in decisions] or ["- none recorded"]),
        "",
        "## NEXT",
        "",
        *([f"- {n}" for n in nxt] or ["- nothing outstanding"]),
        "",
    ]
    path.write_text("\n".join(lines), encoding="utf-8")


# ---------------------------------------------------------------------------
# Diff report -- the acceptance evidence
# ---------------------------------------------------------------------------

def diff_report(run_dir: Path, code_root: Path) -> int:
    """What the agent wrote vs what is in the tree now.

    This is the record of human review: everything here is a change a person made to
    the agent's output before accepting it, or after.
    """
    gen_root = run_dir / "generated"
    if not gen_root.is_dir():
        print(f"no generated/ directory under {run_dir}", file=sys.stderr)
        return 1

    out = [f"# Acceptance diff — {run_dir.name}", "",
           "Agent output on the left, the tree as it stands on the right.", ""]
    any_diff = False
    for gen in sorted(gen_root.rglob("*")):
        if not gen.is_file():
            continue
        rel = gen.relative_to(gen_root).as_posix()
        target = code_root / rel
        generated = gen.read_text(encoding="utf-8", errors="replace")
        landed = target.read_text(encoding="utf-8", errors="replace") if target.is_file() else ""
        if generated == landed:
            out += [f"## `{rel}`", "", "Accepted unchanged.", ""]
            continue
        any_diff = True
        diff = "".join(difflib.unified_diff(
            generated.splitlines(True), landed.splitlines(True),
            fromfile=f"agent/{rel}", tofile=f"tree/{rel}", n=3,
        ))
        out += [f"## `{rel}`", "", "```diff", diff.rstrip(), "```", ""]

    path = run_dir / "acceptance_diff.md"
    path.write_text("\n".join(out), encoding="utf-8")
    print(f"wrote {path}")
    print("differences found" if any_diff else "every generated file accepted unchanged")
    return 0


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Stratocracy goal-oriented coding agent")
    ap.add_argument("--gdd", type=Path, default=DEFAULT_GDD)
    ap.add_argument("--repo", type=Path, default=GAME_REPO)
    ap.add_argument("--apply", action="store_true",
                    help="write generated files into the tree and iterate")
    ap.add_argument("--offline", action="store_true",
                    help="replay recorded model calls; no API key required")
    ap.add_argument("--model", default=None)
    ap.add_argument("--max-iterations", type=int, default=3)
    ap.add_argument("--diff-report", metavar="RUN_DIR", type=Path, default=None)
    args = ap.parse_args(argv)

    if args.diff_report:
        return diff_report(args.diff_report.resolve(), args.repo.resolve())

    if not args.gdd.is_file():
        print(f"GDD not found at {args.gdd}\n"
              f"Pass --gdd with the path to Stratocracy_Prototype_GDD.md.", file=sys.stderr)
        return 2

    run_dir = HERE / "runs" / stamp()
    bb = Blackboard(run_dir=run_dir)
    bb.say(f"# Architect run {run_dir.name}")
    bb.bullet(f"mode: {'OFFLINE replay' if args.offline else 'LIVE'}"
              f"{' + --apply' if args.apply else ' (propose only)'}")

    api_key = None
    if not args.offline:
        api_key = load_api_key(ENV_FILES)
        if not api_key:
            bb.say("\n**No ANTHROPIC_API_KEY found.** Set it, or run with `--offline` "
                   "to replay the recorded calls.")
            return 2

    import os
    model = args.model or os.environ.get("STRATOCRACY_ARCHITECT_MODEL") or DEFAULT_MODEL
    gen = Generator(
        replay_dir=HERE / "replay",
        offline=args.offline,
        model=model,
        api_key=api_key,
    )
    bb.bullet(f"model: `{model}`" + (" (unused in offline replay)" if args.offline else ""))

    gdd_text = args.gdd.read_text(encoding="utf-8")
    gddf = gdd_mod.load(args.gdd)

    built: list[str] = []
    decisions: list[str] = []

    for iteration in range(1, args.max_iterations + 1):
        bb.header(f"ITERATION {iteration}")
        code = scan_mod.scan(args.repo)
        report_perception(bb, gddf, code)

        cands = candidates_mod.build(gddf, code)
        ranked = score_mod.rank(cands, gddf)
        report_scores(bb, ranked)
        bb.record_json(f"scores-{iteration}", [
            {
                "key": s.candidate.key,
                "title": s.candidate.title,
                "score": s.total,
                "actionable": s.actionable,
                "components": s.components,
                "open_ids": s.candidate.open_ids,
                "blocked_by": s.candidate.blocked_by,
                "reasons": s.reasons,
            }
            for s in ranked
        ])

        pick = score_mod.select(ranked)
        if pick is None:
            bb.say("\n**STOP** — no actionable candidate remains.")
            break

        bb.say(f"\n**SELECTED: `{pick.candidate.key}` (score {pick.label})** — "
               f"{pick.candidate.title}")
        decisions.append(
            f"iteration {iteration}: selected `{pick.candidate.key}` at score "
            f"{pick.label}; runner-up "
            f"{ranked[1].candidate.key if len(ranked) > 1 else '—'} was "
            f"{'blocked on ' + ', '.join(ranked[1].candidate.blocked_by) if len(ranked) > 1 and ranked[1].candidate.blocked_by else 'lower-valued'}"
        )

        # A candidate is generated in one or more steps, each its own API call and its
        # own replay slot. Splitting is not cosmetic: a single call asking for header,
        # implementation and test together hit the output cap with two files never
        # started. Each step is handed the previous step's real bytes.
        steps = prompts_mod.plan(pick.candidate)
        if len(steps) > 1:
            bb.bullet(f"generating in {len(steps)} steps: {', '.join(steps)}")

        produced: dict[str, str] = {}
        landed: list[Path] = []
        for step in steps:
            bb.section(f"step `{step}`")
            try:
                system, user = prompts_mod.build(
                    pick.candidate, gddf, code, gdd_text, step, produced)
            except NotImplementedError as exc:
                bb.say(f"\n**STOP** — {exc}")
                break

            generation = gen.generate(step, system, user, bb)
            landed += apply_files(bb, args.repo, generation, dry_run=not args.apply)
            built.extend(f"`{f.path}` — {'applied' if args.apply else 'proposed'} "
                         f"(iteration {iteration}, step `{step}`, {generation.source})"
                         for f in generation.files)
            for f in generation.files:
                produced[f.path] = f.body
            if not generation.files:
                bb.bullet(f"step `{step}` produced nothing; not continuing the chain.")
                break

        if not args.apply:
            bb.say("\nPropose-only run: stopping after one iteration by construction.")
            break
        if not landed:
            bb.say("\n**STOP** — nothing landed, so a re-scan would score identically.")
            break

    # NEXT: whatever is still open after the last scan.
    code = scan_mod.scan(args.repo)
    remaining = score_mod.rank(candidates_mod.build(gddf, code), gddf)
    nxt = [
        f"`{s.candidate.key}` — {s.candidate.title} "
        f"({'blocked on ' + ', '.join(s.candidate.blocked_by) if s.candidate.blocked_by else 'actionable'}"
        f"{', excluded: ' + s.candidate.unruled_reason if s.candidate.unruled else ''})"
        for s in remaining
    ]
    write_state(HERE / "state.md", built, decisions, nxt)
    bb.header("STATE")
    bb.bullet(f"wrote `{(HERE / 'state.md')}`")
    bb.bullet(f"blackboard: `{run_dir / 'blackboard.md'}`")
    if args.apply:
        bb.bullet(f"acceptance diff: `python architect.py --diff-report \"{run_dir}\"`")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
