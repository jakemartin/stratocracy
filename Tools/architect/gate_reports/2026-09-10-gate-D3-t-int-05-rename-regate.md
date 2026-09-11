# Gate D3 -- second re-gate of the T-SAVE-04 -> T-INT-05 rename, over `7f0e05b` -- strat-integration-reviewer

Persisted in substance by the coordinator, 2026-09-10. Priors: gate-D (BLOCK, 3), gate-D2 (BLOCK, 1).

Since D2 only `Tools/architect/state/decisions.md` moved (17:56:14 -> 18:14:53 EDT); tests.md
17:59:04, engine.md 18:01:57, global.md 17:46:07 unchanged to the second; newest Source file
17:35:26.91 EDT; report `reportCreatedOn 2026.09.10-21.37.28` UTC = 17:37:28 EDT; StratPlay DLL
17:36:12 EDT. decisions.md hunk is a pure 93-line append. Checklist 1-12 pass; `--pin-to-tree`
CLEAN (479 = census 479); `--not-before 2026.09.10-21.36.16` CLEAN; banner sweep CLEAN (73, 1
live); citation gate CLEAN (56, 0 live). D2's finding RESOLVED; engine-half re-point lands; all
tests.md quotes land; no self-narration remains.

## Findings (both decisions.md, owner strat-data-steward; neither in the D2->D3 edits)
1. WHY point 2: *"verified by grep at the same snapshot (the block above `struct UiPresentationUnit`
   and its per-unit `lockedThisTurn` comment)"* -- the GDD snapshot contains 0 occurrences of
   `UiPresentationUnit`; the landmarks describe `Source/StratRules/Ui.h` (block comment above
   `struct UiPresentationUnit {`). In the snapshot the quoted words sit under `per-unit {done}`.
2. WHY point 4: *"the 2026-08-29 entry above quotes "`T-UI-05` STOPS AT THE SNAPSHOT: ..." and,
   further down the same passage, "those four ride `T-INT-05`, not `T-UI-05` -- the ID name
   moves and NOTHING ELSE DOES.""* -- decisions.md has no entry dated 2026-08-29. The first quote
   is an `[AMENDED 2026-08-29 BY USER RULING ...]` bracket inside the entry headed "FILED UPSTREAM
   2026-08-27 ... W3's End Turn surface and idle-unit count"; the second is inside a DIFFERENT
   entry, "FILED UPSTREAM 2026-08-27 ... W2's hex info panel and unit stat line". Quoted words
   accurate; attribution false.

## Observations
- "tests.md's 2026-09-10 entry" is ambiguous (two such entries); bullet heading disambiguates.
- W6 heading quote and passage quote run together with no connective.
- "each in a separate dispatch after this entry" not checkable from mtimes.
- §4.7 quote wraps across three snapshot lines; normalisation disclosure covers only the two ID
  sentences.
- D2 observations on tests.md (per-line 70 vs 71 occurrences) and engine.md (truncated W6 quote)
  unchanged.

VERDICT: BLOCK
