Run 2026-09-08 (local) against the UNCOMMITTED working tree over base `525ad5c` on `master`,
in `E:\MultiAgent\Stratocracy`. Read-only: no build was run, no suite was run, nothing was
edited, nothing was staged.

# Gate: mutant-campaign record, third pass — strat-integration-reviewer

## Scope
Working-tree diff against base `525ad5c`, nothing staged, nothing committed. Six modified files, **+606 / −49** — matching the stated scope: `Source/StratUI/Tests/StratShippedOptionsCaptionPresence.cpp`, `Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`, and `Tools/architect/state/{content,engine,global,tests}.md`. All twelve checks re-derived from the tree, not carried forward from my prior two reports.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` prints nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — the lowercase-initial call pattern over `Source/` returns zero hits outside `StratBridge/` and `StratRules/` |
| 3 | No vendored header in front of UHT | pass — the only file including `Data.h`/`Replay.h`/`Save.h`/`Scenario.h`/`Ui.h` is `Source/StratBridge/StratBridge.h`, and every `UCLASS`/`USTRUCT`/`UENUM` token in it is prose in a `//` comment explaining why it declares none. Judged, not counted. |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — `StratMatchSubsystem.h:2869` and `StratScoreboardHUD.h:1253` are both `TPimplPtr<FStratBridge>`; the only `TUniquePtr` occurrence is `StratViewModel.h:47` describing the measured `C4150` |
| 5 | Module arrows unchanged and correct | pass — derivation printed **16 rows** and the field census printed **10 names**, both matching. Row 3 (`StratBridge PrivateDependencyModuleNames StratRules`) verified as the guarded one: `StratBridge.Build.cs:133-137`, `bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` … `if (!bCompileVendoredRulesHere) { PrivateDependencyModuleNames.Add("StratRules"); }`. Conditional, monolithic-only, not a finding. `BRIDGE=` present as the known literal-tail false positive. No cycle; `StratUI` has no `Stratocracy` edge. |
| 6 | New modules registered | pass — no new module directory; `IMPLEMENT_MODULE` in exactly three files, all three registered; `StratRules` correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — diffstat empty |
| 8 | No `/Game/` literal in C++ | pass — every hit outside comments is in a `Tests/` fixture or `ImportStratDataCommandlet.cpp`, the two standing exceptions |
| 9 | No widget-side arithmetic | pass — no `StratUI` production code changed; both `StratUI` edits are comment-only |
| 10 | Lane discipline | pass — test sources under `Source/StratUI/Tests/`, each record file written by its own owner, acting/writing attributed inline with the base commit named |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows six ` M` entries, `git diff --cached --stat` empty |

**Structural re-derivation of the two things edited around four times:**

- **`Content/` is byte-identical to `525ad5c`, verified in both directions and by blob rather than worktree.** The committed LFS pointer at `525ad5c:Content/UI/WBP_Options.uasset` reads `oid sha256:2bebaccbbd291a0351069a51354983903150631b113c923f4697a28293de4667`, `size 43152`; `sha256sum` of the worktree file returns the identical digest; `git hash-object` of the working file resolves to the same blob `db49656e…` as `git rev-parse 525ad5c:…`. `git status --porcelain --untracked-files=all -- Content` and `git diff --numstat HEAD -- Content` are both **empty**, so nothing was added, deleted or modified — a total alone would have netted an add against a delete, which is why both directions were run.
- **Both `.cpp` files are comment-only.** I read the full unified diff. Every added and every removed line begins with `//`; no `IMPLEMENT_SIMPLE_AUTOMATION_TEST` line, no clause name, no assertion, no `namespace` body line moved. The macro census independently agrees: `IMPLEMENT_SIMPLE_AUTOMATION_TEST=477`, matching the report's 477 entries.
- **The banner's own numbers hold.** `Saved/AutomationReport/index.json` read with `utf-8-sig`: `succeeded 476`, `succeededWithWarnings 1`, `failed 0`, `notRun 0`, 477 entries, all `Success`, `reportCreatedOn 2026.09.08-03.43.53` — matching `global.md:39-47` exactly, including the summary-field note. `strat_banner_sweep.py` reports **SWEEP CLEAN — 69 claims, 1 live, 68 stamped**.

## Findings

**1. BLOCK — the 2026-09-08 banner narrates the process that is still gating it, and the narration is now false. `Tools/architect/state/global.md:14-29`.**

Offending text, `:14-17`:

> `_Last run 2026-09-08 (THE MUTANT CAMPAIGN'S RECORD WAS GATED AND THE GATE BLOCKED ON TWO PROSE`
> `FINDINGS, NEITHER IN CODE OR ASSET -- the fourth consecutive pass whose only findings are prose.`
> … `FINDING ONE IS A CLAIM THAT ROTTED UNDER ITS AUTHOR` … `FINDING TWO IS AN ACCOUNT MISSING FROM THE LANE THAT ACTED.`

Judgement was requested rather than a check, so: **cut it.** Not scope it, not repair it.

This file states the rule against itself at `:1097` — *"**A RECORD MUST NOT NARRATE THE PROCESS THAT IS STILL GATING IT.** The subject is still moving, so any closed statement about it — a number, 'both', 'each', 'every', a terminated list — is false on landing, and the artifact that falsifies it is usually the very report that reads it."* The block violates that on all three of the named shapes at once: a **number** ("TWO PROSE FINDINGS"), an **ordinal count** ("the fourth consecutive pass"), and a **terminated list** ("FINDING ONE … FINDING TWO"). It also asserts a singular, completed gating event — "THE GATE BLOCKED" — about a changeset that has now been gated three times.

And it is no longer merely at risk of being false; **it is false as of this report.** My second gate's blocking finding was the eleven/twelve miscount at `global.md:71` — a third finding, in a third gate, and it is not among the two the banner enumerates. The passage that predicted this failure mode at `:1097` was falsified by the very next artifact to read it, exactly as it said it would be.

Leaving the block in place and having it survive a second BLOCK proves the point against the block, not for it. The rule's prediction is that each repair produces a fresh live claim — and that is precisely what happened here, since the repair to `:71` **itself became a finding the block does not contain**.

**What to keep, and how.** The substance is worth keeping; it is the *framing as gate findings* that must go. Restate it as what the pass DID, with no count, no ordinal, no enumeration, and no reference to a gate's verdict:

- `engine.md`'s `reportCreatedOn` reference was scoped, and the live identity referred to this file rather than restated — with the reason (a re-run supersedes it).
- `content.md` gained the account of the two asset mutants planted in the shipped LFS asset, which had lived only in two other lanes' files.
- The date rolled past midnight mid-pass, which is why this banner exists at all — a fact about the pass, not about its gating, and the strongest line in the block. Keep it verbatim.

Delete `:14-17`'s gate framing, the `FINDING ONE` / `FINDING TWO` labels, and `:35-36` (*"AND THE GATE WAS RUN ON AN UNCOMMITTED TREE"*), which is the same shape one level down. Point at `Tools/architect/gate_reports/` by naming pattern if a reader needs the gates; let them open them. That is the remedy `:1100` already prescribes.

**Owner:** the `coordinator` + `strat-data-steward` (`global.md`'s sole writers). No other file needs touching for this — I swept `engine.md`, `tests.md`, `content.md` and `data.md`; none narrates this pass's gating.

## Observations

Non-gating.

- **`global.md:117-118`, "NAMED IN THE BANNER ABOVE THIS ONE", is a positional pointer with a future.** The fix is right and the reasoning for it is right. But the banner above is the 2026-09-08 one, whose own identity line at `:39-40` will carry a `[STAMPED]` on the next re-run — at which point this pointer resolves to a superseded identity sitting beside a live one. "The live identity is recorded in this file's topmost banner" would be immune to that; "above this one" is immune only to re-runs, not to stamping. Worth a word when finding 1 is addressed, since the same seat owns both.
- **`global.md:71-83`: the `[CORRECTED]` block splits a live sentence.** Line 74 ends *"and a final one after every revert"*; lines 75-80 are the correction; line 81 resumes *"with the shipped asset restored to sha256…"*. The convention here is that a reader skips stamped lines, so it parses — and the correction is correctly placed AT the false figure rather than below it, which is the property that matters. But this is the mirror image of the seam fixed at `:126`, and if `:14-29` is being edited anyway, closing the sentence before the block costs one line break.
- **`Saved/MutantReports/` is untracked**, so the twelve-directory instrument that falsified the eleven-count is not reproducible from a checkout. Both test-source headers already handle this correctly by stating the run inline rather than citing a path (*"WHAT WAS RUN, STATED INLINE RATHER THAN CITED. The run's exported reports are untracked"*). `global.md:79-80` cites *"the twelve run directories the campaign left behind"* without that caveat — true today, unfalsifiable from a fresh clone. A half-sentence would match the practice the two `.cpp` files already set.
- **The two test-source corrections are exemplary and I want that on the record**, since this report is otherwise a BLOCK. `StratShippedOptionsExitControlParity.cpp:140-146` and `StratShippedOptionsCaptionPresence.cpp:128-135` both quote the superseded sentence verbatim, name what falsified it, name the base commit, and — the part that is usually missed — **hold the line on what is still unrun**, marking the remaining mutants `**PREDICTED, UNRUN**` at each site and explaining at `:167-169` why the rename mutant does not cover the re-type one. That is a correction that under-claims rather than over-claims.

VERDICT: BLOCK
