Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `f92ce01` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: fifth pass, second mutant campaign record — strat-integration-reviewer

## Scope
Working tree over base `f92ce01`, nothing staged, nothing committed. Six modified files, 717 insertions / 30 deletions: `Source/StratUI/Tests/StratShippedOptionsCaptionPresence.cpp`, `Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`, and `Tools/architect/state/{content,engine,global,tests}.md`. Only `tests.md` moved since the fourth block.

## Checklist
| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — regex over `Source/` returned zero hits outside the two permitted dirs |
| 3 | No vendored header in front of UHT | pass — one candidate, `Source/StratBridge/StratBridge.h`; every `UCLASS`/`USTRUCT`/`UENUM` token in it is inside a comment explaining why it declares none. False positive, judged and dismissed |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` | pass — sole hit is prose in `StratViewModel.h` recording the `C4150` measurement |
| 5 | Module arrows unchanged and correct | pass — derived: 16 rows, all five modules present, matching the expected block including the guarded `StratBridge PrivateDependencyModuleNames StratRules`. Field census printed 10 names. Guard confirmed live at `bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` with the `.Add("StratRules")` inside `if (!bCompileVendoredRulesHere)` |
| 6 | New modules registered | pass — no new `Source/<Module>/` directory in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` printed nothing |
| 8 | No `/Game/` literal in C++ | pass — no `+` line in the diff adds one; the pre-existing `kOptionsClassPath` fixtures are the standing automation-test exception |
| 9 | No widget-side arithmetic | pass — no `StratUI` production code changed; both `.cpp` diffs are comment-only |
| 10 | Lane discipline | pass — `Tests/` edits are the test-author's lane, `tests.md` is its record file, and the entry head attributes acting/writing separately and names base `f92ce01` |
| 11 | Prose blocks present | **BLOCK** — see Findings 1 and 2 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries, no staged content, HEAD still `f92ce01` |

## Findings

**1. The two repaired cross-references do not resolve uniquely — they resolve FOUR ways, and the first match is the wrong entry.**

Both sites quote the same opening words:

> `N1`-`N7` denote the seven options-exit clauses of the entry opening *"2026-09-07 (local), `strat-test-author` (ACTING and WRITING; IN LANE"*

> The entry opening *"2026-09-07 (local), `strat-test-author` (ACTING and WRITING; IN LANE"* already says so in terms, in its own WHAT THESE CLAUSES DO NOT PIN list

That string is the opening of **four** distinct entries in `Tools/architect/state/tests.md` — at lines 284 (`-- ONE new file under`), 435 (`-- ONE new \`Tests/\` file,`), 615 (`-- two new \`Tests/\``) and 843 (`-- ONE new \`Tests/\``). The intended target is the one at 615: it is the only one carrying `SEVEN NEW CLAUSES, COUNTED BY MACRO` and the `WHAT THESE CLAUSES DO NOT PIN` list containing the quoted ternary prediction *"not reachable as an independent line"*. A reader searching the quoted words lands on 284 first and reads the wrong entry. The fourth pass flagged the previous repair as unique-by-one-word; this repair is not unique at all. The quote must extend past `IN LANE` to include `-- two new \`Tests/\``, which does disambiguate. Owner: `strat-test-author`.

**2. The cut left a correction fragment whose antecedent no longer exists anywhere.**

Surviving at the head of the grep passage:

> **MEASURED, NOT ASSUMED, AND QUOTED HERE RATHER THAN CITED TO A PATH. THREE GREPS ARE INVOLVED, NOT TWO, AND EACH NUMBER BELOW NAMES THE ONE THAT PRODUCED IT.**

`NOT TWO` is a correction of a "two greps" claim that the cut deleted and that `git log -S` shows was never published. Searching the file for `two greps` returns exactly one hit, at an unrelated 2026-08 entry about access specifiers. So the clause contradicts nothing any checkout contains — it is the residue of the correction narrative the cut was authorised to remove, re-importing the gate-narration shape in miniature. It should read `THREE GREPS ARE INVOLVED, AND EACH NUMBER BELOW NAMES THE ONE THAT PRODUCED IT.` Owner: `strat-test-author`.

## Observations

- **The replacement's substance verified true, and two of its three greps reproduced exactly.** Against the current built tree I re-ran grep (1)-POST — `grep -a -c ReturnToTitleButtonRenamed Binaries/Win64/UnrealEditor-StratUI.dll` → **0** — and grep (3) — `grep -a -c ReturnToTitleButton …` → **12**. Both match. `git check-ignore -v` confirms the DLL is ignored under `.gitignore:50:Binaries/*`, exactly the rule the record names. The `PREDICTED, UNRUN` accounting also verifies: 4 hits in the caption file, 3 in the parity file, 7 total as claimed, and every one is the marker quoted as history on its correction line. The 21-run arithmetic (18 + 2 controls + 1 superseded) and the 18-mutant enumeration both close, and `global.md`'s cumulative "28 predicted, 27 run" is consistent with 9 + 18 + 1 unrunnable.
- **Grep (2)'s stated return does not match its stated command.** `grep -rc ReturnToTitleButtonRenamed Source/` prints one `path:count` line per file — measured, **283 lines**, all `:0`. The record calls it CHECKOUT-RE-RUNNABLE and gives the return as **0**. The *number* is true (I confirmed zero non-`:0` lines), but a reader re-running the quoted command gets 283 lines, not `0`. Non-gating, and cheaply fixed by saying "0 in all 283 files searched".
- **The diagnostic sentence attributes a two-part fact to one grep.** *"Grep (1)-PRE at 11 is the grep that carries the diagnosis -- the renamed token alive in the loaded binary while absent from `Source/`."* The "absent from `Source/`" half is grep (2)'s, not grep (1)-PRE's, which sits awkwardly against the same block's "each number below names the one that produced it". It under-claims rather than over-claims about grep (1)-PRE itself, so it is not a finding — but the conjunction is two greps wide.
- **The cut is not over-aggressive and nothing a reader needs is missing.** Read fresh, the bullet still carries the mechanism, the three commands with returns, the mtime delta, the three honestly-separated re-runnability states, the `A1b` re-run result, and the 1-in-4-flake caveat. The diagnosis is fully re-derivable from what stands.
- **The executed cut does not match the authorised range as described.** The brief said the deletion ran from *"MEASURED, NOT ASSUMED…"*; that sentence survives at the head of the passage. Harmless apart from Finding 2, which lives inside it.
- **Comment-only, confirmed mechanically.** Filtering the `Tests/` diff to non-comment `+`/`-` lines returns nothing, and `IMPLEMENT_SIMPLE_AUTOMATION_TEST` is 1 in each file — so the entry's claim that no clause was added, removed or weakened holds.

VERDICT: BLOCK
