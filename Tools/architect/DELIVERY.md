# Packaging note — Assignment #5

Written against the instructor's own deliverables list. This is a checklist for
submission day, not part of the agent.

## The two required items

**1. The goal-oriented agent — complete and runnable, with any config needed to run it.**

Everything is under `Tools/architect/`:

| | |
|---|---|
| `architect.py` | the loop, and the entry point |
| `gdd.py` `scan.py` `candidates.py` `score.py` `prompts.py` `write.py` `blackboard.py` | the reasoning layer |
| `requirements.txt` | one package, `anthropic>=0.40` |
| `replay/*.json` | recorded live calls, so `--offline` runs with **no API key** |
| `gdd_snapshot/` | the GDD, because it lives in a different repo and the agent cannot run without one |
| `evidence/` | blackboards, verbatim prompts, raw generated files, acceptance diffs |
| `state.md` | the BUILT / DECISIONS / NEXT markdown state file |

Config needed to run it: **none for `--offline`.** For live, `ANTHROPIC_API_KEY` in the
environment or in `stratocracy-crew/.env`. Say this explicitly in the submission text —
a grader who assumes a key is required may not try it.

**2. The README** — `Tools/architect/README.md`.

## Instructor's questions, and where each is answered

| Asked | Section |
|---|---|
| What features the agent built | *What the agent built* |
| Why the agent selected that feature | *What the agent decided* — the ranked table and the 95-vs-65 reasoning |
| Were you able to run it in your game | *Verification status* — 10/10 T-UI-03, 18/18 suite |
| Read your GDD | *Did the agent do the five things* |
| Scan the codebase | same |
| Detect gaps | same |
| Prioritize | same |
| Generate code | same |
| How did they do it | same, final paragraph |

That section was added specifically to answer the five bullets in the instructor's own
wording and order. Before it existed the answers were all present but spread across the
document, which asks the grader to assemble them.

## Before submitting

- [ ] `git status` clean, and **pushed** — the repo is the submission for the code half.
- [x] **DONE 2026-08-11.** Clean-clone check: `git clone` the repo, then
      `python architect.py --demo`. Reproduced all 3 iterations and all 7 generated
      files with no API key and no packages, and fell back to the vendored GDD snapshot
      on its own. Note `--demo`, not plain `--offline`: against the finished tree the
      agent correctly finds nothing left with curated context and stops, which is the
      right answer and a useless demonstration. Tell the grader to run `--demo` first.
- [x] **DONE 2026-08-11.** Tests re-run from a CLEAN CLONE, not the working tree:
      `git clone` → `Build.bat` → Result: Succeeded → 18/18, 0 failed. Do it this way
      and not in place. The working tree passed all evening while
      `BP_TopDownGameMode.uasset` — the GameMode that spawns the HUD — sat uncommitted,
      so a grader would have cloned a project where nothing put the scoreboard on
      screen. Every agent-generated file was committed; the hand-set property that
      starts them was not, and only a clone could show that.
- [ ] Confirm no `.env` and no key is tracked: `git ls-tree -r HEAD --name-only | grep -i env`
- [ ] If the WBP is laid out and on screen by then, update the verification table and say
      so. If it is not, **leave the table as it is** — a green test list honestly
      described beats a screenshot claim that is not reproducible.

## Known caveats to state rather than hide

- **T-UI-03 is not "closed."** The GDD applies Q29 per acceptance ID at one commit. This
  is the editor-pass artefact the ID was waiting on; flipping the §3 ledger row is the
  Director's call, not the agent's.
- **`gdd_snapshot/` is a snapshot, not a sync.** Nothing hash-gates it the way
  `Source/StratRules/` is gated. It can go stale silently.
- **`prompts.py` has curated context for three candidates.** Any other selection raises
  `NotImplementedError` and stops loudly rather than generating from a thin prompt. That
  is a deliberate limit, and it is better stated than discovered.

## What is NOT part of this submission

- `Tools/editor/make_scoreboard_wbp.py` — human-authored editor tooling, not agent
  output. Kept outside `Tools/architect/` for exactly that reason. Mention it if the
  Blueprint comes up; do not present it as something the agent produced.
- `Tools/context/` — exported project-memory notes, gitignored and untracked.
