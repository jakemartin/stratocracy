# Vendored GDD snapshot

| | |
|---|---|
| File | `Stratocracy_Prototype_GDD.md` |
| Source | `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md` |
| Bytes | 446,133 |
| sha256 | `bfa4f713ab1415cff4e0e0ef09d94f751ca727f7893bc0d44900d81248ed954c` |
| Taken | 2026-08-11 |

## Why a copy exists here

The GDD lives in a **different repository** (`stratocracy-content`) from the game. The
agent reads it on every run — the scorer's dependency edges come from §4.11's build-
order table and generation prompts quote §4.7's invariants verbatim — so a grader who
clones only the game repo could not run the agent at all without it.

`architect.py` prefers the live document at its real path and falls back to this copy
only when that path is absent. So a working checkout always reads the current GDD and
never this snapshot, and the snapshot exists purely so the deliverable is runnable
standalone.

## This is a snapshot, not a sync

Unlike `Source/StratRules/` and `Data/`, nothing hash-checks this file against its
source and no acceptance ID asserts over it. It is **not** part of the vendoring
discipline those directories follow — there is no `sync_*.py` behind it and no gate
that fails when it goes stale.

If the GDD moves on, this copy silently does not. Re-take it with a plain copy and
update the hash above. Where the two disagree, **the document in `stratocracy-content`
is the real one.**
