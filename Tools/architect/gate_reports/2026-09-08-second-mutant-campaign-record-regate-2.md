Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `f92ce01` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: second mutant campaign, record repair, third pass — strat-integration-reviewer

## Scope
Working-tree diff against base `f92ce01`, nothing staged, nothing committed. Six modified files, 749 insertions / 30 deletions: `Source/StratUI/Tests/StratShippedOptionsCaptionPresence.cpp`, `Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`, and `Tools/architect/state/{content,engine,global,tests}.md`. Structural checks re-derived from the current tree, not carried in. The three greps under dispute were re-run here.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` prints nothing |
| 2 | No `strat::` call outside the two permitted dirs | pass — `strat::[a-z]\w*\s*\(` over `Source/` minus `StratBridge/`/`StratRules/` returns zero hits |
| 3 | No vendored header in front of UHT | pass — of the 8 headers including a vendored header, none declares `UCLASS`/`USTRUCT`/`UENUM` |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — sole `TUniquePtr<FStratBridge>` hit is prose at `Source/StratUI/StratViewModel.h:47` |
| 5 | Module arrows unchanged and correct | pass — derivation printed **16 rows** and the census **10 field names**, matching the carried block including the `BRIDGE=` false positive; the 16th row is the guarded `PrivateDependencyModuleNames.Add("StratRules")` at `Source/StratBridge/StratBridge.Build.cs:137`, inside `if (!bCompileVendoredRulesHere)` at line 135, from the single expression at line 133. Not a finding |
| 6 | New modules registered | pass — no new module directories; `StratRules` correctly absent from `Stratocracy.uproject` and carries no `IMPLEMENT_MODULE` |
| 7 | `Source/Stratocracy/` untouched | pass — diff stat prints nothing |
| 8 | No `/Game/` literal in C++ | pass — every hit is a comment or an automation fixture under `Tests/`; the two modified `.cpp` add no literal |
| 9 | No widget-side arithmetic | pass — no widget code changed; both modified `.cpp` are comment-block edits inside `Tests/` |
| 10 | Lane discipline | pass — `Tests/` edits and `tests.md` by `strat-test-author`; `engine.md`, `content.md` each by their own lane with acting/writing separated inline; the out-of-lane in-session acting is declared with base `f92ce01` named. `strat_banner_sweep.py`: `SWEEP CLEAN` |
| 11 | Prose blocks present and true | **BLOCK** — two findings below |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries, index empty, HEAD `f92ce01` |

## Findings

**1. BLOCK — the new bullet's punchline inverts which grep carries the diagnosis, and contradicts the same entry forty lines above.**

`Tools/architect/state/tests.md:89-93`:

> "**AND THIS SPLIT MAKES THE ARGUMENT STRONGER, NOT WEAKER: grep (2) IS THE LOAD-BEARING ONE PRECISELY BECAUSE IT IS THE ONE A BARE CHECKOUT CAN CHECK.** It is the grep that proves the SOURCE was clean while the BINARY was not, which is the whole stale-binary diagnosis; the two a reader cannot run corroborate it, and the one they can run carries it."

Grep (2) is `grep -rc ReturnToTitleButtonRenamed Source/`, and the entry itself says at line 60 that it returned **0 "throughout, before AND after"**. A number that is 0 in the mutated tree, 0 in the reverted tree, 0 today and 0 on any checkout where the mutant was never planted is **non-discriminating**: re-running it establishes that the source is clean *now*, never that it was clean *at the moment the binary was stale*. The grep that actually discriminates is grep (1)-PRE at **11** — the renamed token alive in the DLL while absent from source — and that is exactly the one the bullet correctly files under state (iii), unrepeatable. So the two clauses are backwards: the unrunnable number carries the diagnosis, the runnable one corroborates.

The entry states it correctly earlier and incorrectly here, in one file: line 60-62 says grep (2) is 0 "because the source had already been reverted — **which is what makes the loaded binary the only place the token could still live**", i.e. a conjunction with grep (1)-PRE. Line 91-93 promotes one half of that conjunction to the whole. This is the third instance of the very pattern the bullet names two sentences later — a true number under a false description of what obtaining it buys you — and it is the sentence a reader is most likely to act on.

Owner: `strat-test-author`.

**2. BLOCK — an off-by-one cross-reference a reader falsifies by counting.**

`Tools/architect/state/tests.md:99-101`:

> "The standard was already stated correctly in **the bullet two above this one**, about the UNTRACKED exported automation reports"

The entry's bullets at that indent are, in order: `**THE METHOD, which is what makes "RED, alone" mean anything.**` (line 31), `**THE MEASUREMENTS ARE STATED INLINE AND NOTHING IS CITED TO A REPORT PATH.**` (line 40 — the untracked-reports bullet), and `**THE UNEXPLAINED RE-RUN IS EXPLAINED…**` (line 43, which contains the sentence). The untracked-reports bullet is **one** above, not two; a reader counting two lands on the METHOD bullet, which says nothing about untracked artifacts. Introduced by this repair.

Owner: `strat-test-author`.

## Observations

Non-gating.

- **The three-state split itself is correct, and I verified it against this built tree.** grep (1)-post returns **0**, grep (2) returns **0**, grep (3) returns **12** against `Binaries/Win64/UnrealEditor-StratUI.dll`. `.gitignore:50` is literally `Binaries/*` and `git ls-files Binaries/` is empty, so the content anchor holds and states (i)/(ii) are labelled correctly. Only the *weight* assigned to state (i) is wrong, which is finding 1.
- **`tests.md:79-81`** — "Every figure here was and remains right, and `strat-integration-reviewer` re-ran all three greps on 2026-09-08 over base `f92ce01` and confirmed each one" sits eight lines above the statement that grep (1)-PRE at 11 is testimony about a tree that no longer exists and that "Nothing can re-run those." Read literally the two disagree about the 11. No checkout can settle it either way; tightening to "re-ran the three greps as they stand today and confirmed 0, 0 and 12" would remove the tension.
- **`tests.md:100-101`** — "the same ignore condition" glosses two different rules: the DLL is ignored by `Binaries/*` (`.gitignore:50`), the exported reports by `Saved/*` (`.gitignore:68`). Same *condition class* (untracked), not the same rule.
- **Ask (d): no underivable count was introduced.** "three states" and "two of the three" match the enumeration; "0, 0, 12, 11" re-verified above; "SECOND time in this one entry" is derivable from the two corrections at lines 66 and 74. The surrounding counts also reconcile: 13 source/test-file mutants (line 153 carries two) + 4 asset + 1 survivor = **18**, of which **17** killed; 18 runs + 2 controls + 1 superseded contaminated run = **21**; the uncompilable mutant (line 209) is correctly outside the 18.
- The two `.cpp` comment corrections are well-formed on the axis this gate has previously blocked: both quote the false sentence at its own site rather than stamping beneath it, both name base `f92ce01`, and both restate the changed claim flat. `StratShippedOptionsCaptionPresence.cpp` also preserves the one half that stayed a prediction ("a double deletion reads as two messages") instead of sweeping it into the correction — that is the right treatment.

VERDICT: BLOCK
