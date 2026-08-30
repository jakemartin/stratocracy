# Gate: W6 title/menu screen — strat-integration-reviewer

**FIVE ROUNDS: BLOCK, BLOCK, BLOCK, BLOCK, BLOCK.** Findings 2, 2, 4, 2, 1. Each round gates the
remediation of the one before it, so this reads as a sequence and not as five opinions of one
tree. Dispatched by the `coordinator` on 2026-08-30 against the uncommitted working tree at
`E:\MultiAgent\Strat-wt\slot-1`, branch `feat/title-menu`, HEAD `347c722` — a parallel worktree,
not the integration tree, with `git merge-base HEAD master` equal to `master` throughout, so no
rebase was ever needed and none was performed.

**THIS FILE EXISTS BECAUSE ITS ABSENCE WAS THE FIFTH ROUND'S FINDING, AND THAT IS THE SHORTEST
STATEMENT OF WHAT THE WHOLE WAVE WAS ABOUT.** `Source/StratPlay/Tests/StratShellRouteClauses.cpp`
had struck two `See the report` citations on the ground that this tree persists no gate reports.
`git ls-tree -r --name-only 347c722 -- Tools/architect/gate_reports` returns **five tracked
files**, one of them dated the same day. The evidence was deleted because the mechanism was
believed absent, and the belief was never measured. Writing this file is the remedy the branch
should have reached for; striking the citations was the remedy it reached for instead.

**WRITTEN OUT OF LANE, DECLARED HERE RATHER THAN ONLY IN `global.md`.** `Tools/architect/` is
`strat-data-steward`'s. This file is the `coordinator`'s, on the precedent of
`2026-08-30-ci-build-and-suite.md`, which was written the same way for the same reason. There was
no lane draft and no merge, so this is **not** the transcription clause.

**GREP THE QUOTATION, NOT THE LINE NUMBER.** Every remediation below rewrote the prose its finding
cites, and this branch measured the cost directly: the one `ExecuteRoute` call site moved
**1421 → 1489 → 1521** across three readings in a single session, twice while an agent was editing.
Every citation in this file is by symbol for that reason.

---

## What the wave built

`UStratShellSubsystem` (a `UGameInstanceSubsystem`) and `AStratShellGameMode` in `Source/StratPlay/`,
plus edits to `AStratGameMode` and `UStratMatchSubsystem`. **No assets and no `Config/` change** —
the title level, the menu WBP and the `GameDefaultMap` move are a later editor batch, per the
parallel-lane rule that a feature branch needing an asset lands its C++ and the asset is authored
on the integration tree.

**USER RULING, 2026-08-30: W6 BUILDS NO DIFFICULTY-TIER CONTROL.** The conditional `T-FAME-02`
clause the 2026-08-29 acceptance-ID ruling would have owed is unowed and inert. No new clause
names `T-FAME-02`; the pre-existing
`Stratocracy.StratBridge.T-FAME-02.HandicapMovesTheScenariosConfiguredFameByTheDelta` is untouched
and is a different thing.

**THE GDD SPECIFIES NOTHING ABOUT THIS SCREEN BEYOND ITS NAME.** §2.11.5 fixes the screen list
(`That is the complete screen list for the prototype: title/menu, briefing, match, result`) and
stops. Case-insensitive counts over the whole document: `title/menu` **1**, and `title screen`,
`main menu`, `Quit`, `Rematch`, `Restart`, `Play Again`, `Return to` all **0**. §2.11.8's must-have
ranking does not list the screen at all. **So the four routes are ours**, derived from capabilities
already in the tree, and both new headers say so rather than claiming a design sentence that does
not exist. Verified independently by the reviewer in round 3.

> **AND THE `coordinator`'S FIRST RE-VERIFICATION OF THOSE COUNTS WAS RUN ON A BROKEN INSTRUMENT,
> WHICH IS RECORDED HERE BECAUSE IT IS THIS DOCUMENT'S OWN LESSON HAPPENING TO ITS AUTHOR.**
> `grep -o -i -F` over this file returned **0 for every term including the control**, `factories`,
> which the document certainly contains — the GDD has ~100 KB lines and that invocation reports
> nothing rather than failing. Re-run in Python with three controls returning 58, 158 and 161, the
> counts above hold exactly. **An absence proves nothing until the instrument is shown able to
> speak**, and a zero from a broken instrument is indistinguishable from a zero that means
> something. The first run would have "confirmed" the claim while measuring nothing at all.

**The final figures**, each re-derived by the reviewer and by the `coordinator` separately from the
lane's own report: build `Result: Succeeded`, `REAL_EXIT=0`; exported report
`reportCreatedOn 2026.08.30-22.45.48`, **310 succeeded / 0 failed / 0 notRun / 0
succeededWithWarnings**, 310 entries; clause census from an asserted absolute root, base `347c722`
= 287 → tree 310, **NEW 23, REMOVED 0**; clause-name **set equality against the report 0/0 in both
directions**.

> **THAT STAMP NAMED `22.30.16` UNTIL THE SIXTH GATE CAUGHT IT, AND THE CORRECTION IS ITS OWN SMALL
> LESSON.** Round 5's remediation caused another build and another suite run, so the report on disk
> became `22.45.48` while this paragraph — headed **final** — still named the run before it. Every
> figure is identical and the two runs are behaviourally identical, because only a `.cpp`'s comments
> and this file changed between them. **Nothing was wrong except which run the numbers were attested
> to**, and this project has already recorded that as a real defect class: a citation naming a report
> that is no longer the one on disk is a stale claim about WHICH RUN backs the figure. A document
> that records a wave cannot stop moving before the wave does.

---

## Round 1 — BLOCK, 2 findings, both prose-borne coverage claims

The code, the module graph and all eight structural checks were clean from the first read.

**(1) The test file's header asserted non-coverage the same file refuted 1,200 lines later**, and
quoted a line of code that no longer existed. Its `WHAT NO CLAUSE IN THIS FILE REACHES` block listed
`ExecuteRoute` as unreachable, while `FStratShellRefusedRouteArmsNothingTest`'s own comment opened
*"THIS CLAUSE REACHES INTO `ExecuteRoute`, AND THE HEADER SAYS NO CLAUSE CAN."* It also quoted
`ArmPendingLoadSlot(SaveSlotName)` in the present tense after the engineer had replaced that line
with `ArmPendingLoadSlot(PendingSlotForRoute(Route, SaveSlotName))`.

**(2) `UStratShellSubsystem`'s class block claimed `ExecuteRoute` was wholly unreachable**, and its
`ExecuteRoute` doc closed *"the order is now the only thing about this function that could be wrong
without a clause noticing"* — wrong in both directions. Half the order is pinned; and the
conditionality of the arming call can be wrong with nothing noticing.

**Remediation.** Both stamped in place with `RETRACTED>` quotations, nothing deleted. The engineer
additionally found that the same paragraph had enumerated five statics and gone stale when
`PendingSlotForRoute` made six, and replaced the copy with a pointer to the declarations.

---

## Round 2 — BLOCK, 2 findings

**(1) The corrected header closed an item that was still open.** The `WHICH SLOT ExecuteRoute ARMS`
item was marked `CLOSED` with *"An `ExecuteRoute` that armed the wrong string is now red"*. The five
clauses it cited pin `UStratShellSubsystem::PendingSlotForRoute` — **the static**. The item's own
heading names *which slot `ExecuteRoute` arms* — a different subject, which no clause reaches. The
reviewer measured rather than reasoned: exactly one `ExecuteRoute` call site in the whole corpus, on
the refused arm, returning before the arming.

**(2) The `ExecuteRoute` enumeration omitted `RouteTravels`**, the call that chooses between the two
engine calls — in the one block the test file had just designated the sole authority on that
function's contents. **A pointer discipline is only as good as the authority it points at**, and the
authority was incomplete on the same day it acquired a dependent.

**Remediation.** The item was stamped twice — the first correction's factual half survives, its
status half retracted — and moved into `WHAT IS STILL NOT REACHED` beside its sibling, closing
*"THESE TWO ITEMS ARE THE SAME SINGLE LINE OF CODE, and a reader who closes one has not closed the
other."* What the extraction bought is stated plainly so the record does not swing to the opposite
over-claim.

---

## Round 3 — BLOCK, 4 findings, and the round that found the defect

Between rounds 2 and 3 the reviewer's standing suggestion was taken: `AStratShellGameMode::SaveSlotName`
now derives its default via `SaveSlotName = FStratMatchConfig().SaveSlotName;` rather than defaulting
empty, because `UStratMatchSubsystem::ResolveSaveSlotName` already refuses a second literal author of
that string in its own words. **That change surfaced a player-visible defect that would have shipped.**

**THE DEFECT.** `UStratMatchSubsystem::ApplyView` calls `RecordMatchCompletionOnSave(FString(), Reason)`;
when no slot exists it calls `CreateSaveGameObject` and writes, and the writer's own comment says the
payload *"goes to disk with an EMPTY `SaveText`"*. So after a completed-but-unsaved match the slot file
exists, `DoesSaveGameExist` answers true, **Continue draws enabled and silently gives turn 1 instead of
a restore**. The version gate cannot catch it by construction: `StratSaveGame.h` initialises
`SavedDataVersion = kCurrentSavedDataVersion`.

**NEITHER HALF IS A DEFECT ALONE.** The completion writer is correct; the derived default is correct.
The empty default had been hiding the combination rather than fixing it. **The suite could not see it
because the error was in the FACT, not the decision** — `bSaveSlotExists` said *a file is there* while
every consumer read *a match can be restored* — and that fact is produced in the one function needing
a world.

**THE FIX.** Three payload refusals extracted out of `LoadMatchFromSlot` into two world-free statics,
`UStratMatchSubsystem::IsPayloadRestorable` and `UStratMatchSubsystem::DoesSlotHoldARestorableMatch`,
with the loader now calling the first — one statement of the conditions, two askers.
`FStratShellFacts::bSaveSlotExists` renamed `bSaveSlotIsRestorable`. **The reviewer confirmed the
extraction behaviour-preserving mechanically**, extracting every literal from `LoadMatchFromSlot` at
`347c722` and from `IsPayloadRestorable` now, composing through the new `"slot '%s' %s"` template, and
comparing: order-equal on all three, `Load refused: ` intact.

**THE FOUR FINDINGS.**

**(1) `DoesSaveSlotExist`'s new doc quoted a sentence that was never on it and blamed a call that never
happened.** It read *"the doc that stood in its place invited exactly that — it read 'Exposed so a menu
can label the button'"*. That sentence is `ResolveSaveSlotName`'s, one declaration below;
`DoesSaveSlotExist`'s doc at `347c722` was neutral. And `DoesSaveSlotExist` has exactly one
non-declaring call site in the tree — a clause. The shell reached `UGameplayStatics::DoesSaveGameExist`
directly. The function was accused of saying something it did not say, in the doc that had just become
the tree's authority on why the defect shipped.

**(2) "The sibling arms are pinned" was false for one of the two.** Measured at `347c722`, scoped to
`Source/*/Tests/*.cpp`: the version arm pinned, the wrong-class arm **0 occurrences corpus-wide**, the
empty-text arm 0. **Two of three payload arms had no clause, not one** — systematic, not bad luck on a
single arm. The reviewer ran the absence with a control showing the instrument could speak.

**(3) THE TREE ALREADY CARRIED A CORRECT ADVANCE WARNING, AND NOBODY READ IT.** `StratSaveGame.h`,
unmodified by this branch, carries under the heading **`ONE CONSEQUENCE, WRITTEN DOWN RATHER THAN
DISCOVERED`**: *"the writer CREATES a slot that does not exist, so a player who finishes a match
without ever saving now has a slot carrying an empty `SaveText`. `LoadMatchFromSlot` refuses that by
name; anything that offers a "Continue" affordance must gate on a LOADABLE slot and not on
`DoesSaveSlotExist`, which now answers true for a slot with no match in it."* **It names the
affordance, the wrong function and the right rule, before W6 was written, under a heading claiming it
would not have to be discovered.** It was discovered. So the cause has two halves — an unpinned arm,
**and** a live in-tree warning nobody read.

**The lesson the remediation reached, which is not "read more carefully":** a warning placed in the
header of the type it concerns is invisible to someone working from the consuming end. Nothing about
writing a menu sends you to a save-game payload's file comment, and grep finds it only if you already
suspect what it warns about. **What would actually have caught it is the clause the arm never had — a
prose warning cannot fail a build and an executable one can.** The test-author additionally formed and
**killed** a hypothesis that the warning went unread because it named a wrong symbol: `DoesSaveSlotExist`
is a real method at `347c722` and the warning was accurate and greppable. Recorded as refuted so nobody
re-derives it.

**(4) The per-frame disk-read debt was declared nowhere in the bytes.** `GatherFacts` now performs a
full `LoadGameFromSlot` deserialize where it read a directory entry, and `GetMenuModel` calls it while a
UMG property binding runs per frame. A shape sweep across all changed files returned zero hits. The
reviewer **agreed with the decision** not to cache — a cache with no invalidation is worse, and the
invalidation points are unknowable until the WBP exists — but an undeclared decision is
indistinguishable from an unnoticed one. Now on `GetMenuModel`, with both discharge routes named.

**THE ACCEPTANCE ID: `T-SAVE-04` WAS PROPOSED AND REFUSED, BY THE LANE THAT WAS OFFERED IT.** The GDD
defines it as `refusal: any header mismatch (version/rules/data/scenario hash)` and *"refuses on the
header alone and never applies a command"*. **A completion-only payload's header is valid and current
— that is the whole reason it shipped** — so an empty body is not a header mismatch under either
sentence. All four new clauses ride `GATE-TITLEMENU` instead. Both GDD quotes were verified
independently by the reviewer and by the `coordinator`. Reported but **not fixed**, correctly: the
pre-existing `Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAnUnconfiguredSubsystem` is likewise outside
that sentence — it refuses before `LoadGameFromSlot` is called, so there is no header to mismatch.

---

## Round 4 — BLOCK, 2 findings, and a dead measuring instrument

**BETWEEN ROUNDS, THE TEST-AUTHOR DISCOVERED ITS MUTANT INSTRUMENT HAD BEEN DEAD.** Its first mutant run
for two new clauses came back **green**, which reads as "these clauses are inert", and it was one step
from reporting that. In the `robocopy`'d mutant tree, **UBT's cached build graph still resolved the
SOURCE tree, so every "mutant build" was a no-op that printed `Result: Succeeded`.** Proved three ways
rather than inferred: an unconditional `AddError` planted in the clause never fired and the suite still
read all-passing; `grep` on the built DLL found the probe string **absent** and the clause name
**present**; and deleting the DLL and rebuilding produced `Result: Succeeded` **with no DLL emitted at
all**. Fixed by clearing `Intermediate/Build` in the copy, after which the mutations bit immediately.

**`Result: Succeeded` IS NOT EVIDENCE THAT A BUILD HAPPENED. THE ARTIFACT IS.** Same species as this
project's recorded *"exit code is not a verdict"*.

**(1) The scoping argument had a counterexample inside its own enumeration.** The reviewer endorsed the
core inference — if the baseline is green and the mutant build is a no-op, the binary is unmutated and
the suite comes back green, therefore **red ⟹ a relink incorporating the mutation happened, absent a
flake**. But the claim *"M1–M10, N1–N6 and R1–R4 all produced reds"* is false: **N6 was green**, and had
been reported as green. Three sites cited it, two saying *"measured … not reasoned"* — disclaiming the
derivation that would have rescued them.

**The remedy was already in the file three lines away**, and it is better evidence than a re-run: the
call-site derivation says **why** nothing catches a change to that line rather than reporting that
nothing did, and it needs no mutant to have been run at all. **A checkout can check a grep and cannot
check a mutant somebody ran and discarded.**

**(2) The dead-instrument discovery was nowhere in the tree.** Zero hits across all changed files for
`Intermediate/Build`, `robocopy`, `no-op build`, `cached build graph`, `UBT`, `relink`. The branch's
most transferable lesson would have died at the merge. It is now a named header section carrying the
three-step proof shape, the cause, the fix, and the red/green asymmetry with the flake judgement left
to the reader rather than foreclosed.

**AND THE CALL-SITE DERIVATION WAS ITSELF THE SELF-QUOTING TRAP.** `grep -rnE "(->|\.)ExecuteRoute\("
Source/` returned **4 hits, 3 of them the test file's own comments — two added by the previous
correction.** The sentence claiming *"returns EXACTLY ONE call site"* was in a paragraph that made the
count four. The recorded *"a census that quotes its own search token"* failure landed in the exact
sentence whose purpose was to be re-executable. The fix is a command immune to its own prose, stated
**once** canonically with the other sites pointing at it:

```
grep -rn "ExecuteRoute(" Source/ | grep -v "://" | grep -- "->ExecuteRoute("
```

Verified by the `coordinator`: returns **1**. The unfiltered form returns more, so the middle filter is
doing real work. The paragraph names the trigger that voids the derivation entirely — **a second
`ExecuteRoute` caller appearing.**

---

## Round 5 — BLOCK, 1 finding, and the reviewer seeded it

**`Tools/architect/gate_reports/` EXISTS AND IS TRACKED.** The finding, and the reason this file exists.
See the head of this document.

**THE GATE OWNED HAVING CAUSED IT.** In round 2 it reported "no persisted report artifact" from
`ls Tools/architect/runs/` — a directory that does not exist — plus
`find . -iname '*report*' -newer Source/StratPlay/StratGameMode.h`. The gate reports are stamped
`15:48:05` and that header is `15:59:50`, so **its own `-newer` filter excluded exactly the files it was
looking for**, and it repeated the claim in rounds 2 and 4. This project's recorded *"instruments loose,
claims strict"* hazard, committed by the instrument. The lane then inherited the measurement rather than
re-running it, which is the thing this whole wave keeps proving you must not do.

**ONE NON-GATING RESULT WORTH KEEPING, because it reads the wrong way round.** The removed token
`bSaveSlotExists` greps to 2 hits **in the built DLL**. That reads as a stale binary. It is not: both are
UHT-generated `Comment`/`ToolTip` metadata for `bSaveSlotIsRestorable`'s doc comment, which deliberately
records the rename; `bSaveSlotIsRestorable` appears once as the property name and **no reflected property
carries the old name**. So no removed identifier survives in the binary. **But the argument
under-discriminates**, as the reviewer noted: that token has been in the doc since the rename two rounds
earlier, so its presence proves *post-rename*, not *this round*. The discriminating evidence is that the
doc's **wording changed** this round — the round-3 phrase occurs 0 times in header and DLL, the current
phrase 1 in the header and 2 in the DLL. **A changed string matching only current source proves this
round's binary; an unchanged token does not.**

---

## What the gates did NOT measure, every round

**No round built the project or ran the suite.** Every check was compile-free. The mutant set
M1–M10, N1–N5, R1–R4 and the re-run C1/C2 remain **agent self-reports**, accepted on the
red ⟹ relink inference, absent a flake — and two of those mutants are now known to have needed
re-running. The reviewer verified the DLL *contains* this branch's strings but not that every clause
body in it matches source. No asset was measured, because none changed.

**One universal the gate could not falsify and flagged rather than accepted:** the test file's claim
that *"every clause in this file is justified by a built mutant"* is a universal quantifier over an
agent self-report.

**And one evidentiary limit the reviewer raised against itself:** its claim that the shell previously
called `UGameplayStatics::DoesSaveGameExist` directly rests on its own readings in rounds 1 and 2,
because `StratShellSubsystem.cpp` was untracked when it held that call. **That prior state is not
re-derivable from a checkout by anyone**, and the remediation says so where it matters rather than
asserting it as though the repository could show it.

---

## The one sentence worth carrying out of this wave

Every finding across five rounds was the same defect in a different coat: **a sentence whose
conclusion was right and whose warrant was never measured.** A restated count, a coverage claim, an
invented quotation, a discarded mutant, a self-quoting grep, and a directory listing nobody ran.
**At every gate, the C++ audited was sound: the reviewer blocked zero times on the code's own
behaviour, and every one of its eleven findings was in prose or in evidence.**

> **THAT SENTENCE READ "the code was sound each time" UNTIL THE SIXTH GATE, AND IT WAS THE
> SUBJECT-VS-SCOPE SHAPE THIS DOCUMENT IS ABOUT, COMMITTED IN THE DOCUMENT'S CLOSING PARAGRAPH.**
> True of the gates; false of the wave. **This wave had a player-visible defect** — Continue offered
> after a completed-but-unsaved match, silently yielding turn 1 — recorded at length three sections
> above, in this same file. It was found by the lane and fixed before any gate saw it, which is
> exactly why a sentence quantified over gates reads as a claim about the code and is not one. The
> quantifier is now stated rather than implied.

> **AN EVIDENCE CITATION SHOULD NAME SOMETHING A READER CAN RE-EXECUTE FROM A CHECKOUT.** A grep
> qualifies — provided it is not the self-quoting kind. A build somebody ran does not. A mutant
> somebody ran and discarded does not. A document as remembered does not.

**Prefer the evidence a stranger can reproduce, even when the evidence you have is real — because a
reader cannot tell your real one from your mistaken one, and neither, it turns out, could you.**
