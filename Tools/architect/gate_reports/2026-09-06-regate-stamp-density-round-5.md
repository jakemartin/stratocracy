# Gate: content.md stamp density, RE-GATE round 5 (post-`c69e519`) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, head `c69e519`.
Measured, not taken from the brief:

    $ git status --porcelain | wc -l      -> 18   (14 modified, 4 untracked)
    $ git diff --stat | tail -1           -> 14 files changed, 795 insertions(+), 31 deletions(-)
    $ git diff --cached --stat            -> (empty)

Fourteen modified, four untracked (this reviewer's rounds 1-4), nothing staged — exactly as the
brief states. Round 4 measured 778 insertions; the **+17 are entirely in the record files**, and
the `Source/` diff is byte-for-byte what round 4 audited (340 added lines under `Source/`, 234 in
`StratPlay`, 100 in `StratUI` — all three re-derived here, all three unchanged).

Rounds 1-4 stand. This report supersedes none of them and corrects two of round 4's minor figures.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — with a control |
| 2 | No `strat::` call outside `StratBridge/` or `StratRules/` | pass — with a control |
| 3 | No vendored header in front of UHT | pass — with a control |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass — with a control |
| 5 | Module arrows unchanged and correct | pass — DERIVED, 16 rows / 10 fields, guard read at source |
| 6 | New modules registered — unless they have no module object | pass — with a control |
| 7 | `Source/Stratocracy/` untouched | pass — with a control |
| 8 | No `/Game/` path literal in C++ | pass — with a control; both added hits are comment lines |
| 9 | No widget-side arithmetic | pass — categorically; zero non-comment added lines in `StratUI` |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present / record accurate | pass — both round-4 findings verified repaired |
| 12 | Nothing staged or committed | pass — with a control |

### 1. Vendored bytes untouched — pass, with a control

    $ git diff --stat -- Source/StratRules Data
    (nothing)
    CONTROL, same instrument on a path that DID move:
    $ git diff --stat -- Source/StratPlay   -> 6 files changed, 234 insertions(+), 18 deletions(-)

Manifests are the authority and neither moved:
`rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`,
`dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`.

### 2. `strat::` calls — pass, with a control

    $ grep -rn -E "strat::[a-z][A-Za-z0-9_]*[[:space:]]*[(]" Source/ --include=*.cpp --include=*.h
        | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
    (nothing)
    CONTROL, same pattern inside the permitted directory:
    Source/StratBridge/StratBridge.cpp:168: strat::parseScenario(...)
    Source/StratBridge/StratBridge.cpp:175: strat::validateScenario(...)
    Source/StratBridge/StratBridge.cpp:182: strat::seedFromScenario(...)

Zero hits outside the two directories; the pattern is shown able to match. No type name reported
as a violation.

### 3. Vendored header in front of UHT — pass, with a control

Eight headers include a vendored header: `Source/StratBridge/StratBridge.h` and
`Source/StratRules/{Ai,Driver,Economy,Move,Replay,Scenario,Ui}.h`. Each tested for reflected types:

    $ grep -cE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)' <each of the 8>   -> 0 x8
    CONTROL: the same instrument on Source/StratPlay/StratShellGameMode.h -> 1

### 4. `TPimplPtr` — pass, with a control

    $ grep -rn "TUniquePtr<FStratBridge>" Source/
    Source/StratUI/StratViewModel.h:47: // ... the `C4150` measured on `TUniquePtr<FStratBridge>` ...

One hit, a prose citation of the measurement, not a declaration. CONTROL: 15 `TPimplPtr` sites
across `Source/**/*.h`, unchanged from rounds 3 and 4.

### 5. Module arrows — pass, DERIVED, 16 rows and 10 fields

The checklist's extraction was extracted and RUN. **16 rows, all five modules present**, so the
extraction did not fail silently. Identical to rounds 3 and 4. The sixteenth row —
`StratBridge PrivateDependencyModuleNames StratRules` — was read at its source rather than treated
as unconditional, `Source/StratBridge/StratBridge.Build.cs`:

    :133  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    :134  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (...));
    :135  if (!bCompileVendoredRulesHere)
    :137      PrivateDependencyModuleNames.Add("StratRules");

Monolithic-only link edge, guarded, deliberate, not a finding. Field census printed **10**,
including the known `BRIDGE=` false positive that is the tail of a string literal. No eleventh
name. No `.Build.cs` is in this diff, so the derivation is a drift check rather than a change
review.

### 6. New modules — pass, with a control

Exactly five module directories, none new. **CORRECTION TO MY OWN ROUND-4 REPORT:** round 4 listed
four `IMPLEMENT_MODULE` sites including `Stratocracy.cpp`. Measured here, there are **three** —
`StratBridge.cpp`, `StratPlay.cpp`, `StratUI.cpp`; `Source/Stratocracy/Stratocracy.cpp:6` uses
`IMPLEMENT_PRIMARY_GAME_MODULE`, a different macro. The substance is untouched: **`StratRules` has
neither macro and is correctly absent** from `Stratocracy.uproject`'s Modules array.

### 7. `Source/Stratocracy/` untouched — pass, with a control

    $ git diff --stat -- Source/Stratocracy   -> (nothing)
    CONTROL: $ git diff --stat -- Source/StratPlay   -> 6 files changed

### 8. `/Game/` literals — pass, with a control

Two added hits, both comment lines (a `coordinator` clause citation and a
`GlobalDefaultGameMode=` citation). CONTROL: the same diff carries **340** added lines under
`Source/`, so the filter had material to work on.

### 9. Widget-side arithmetic — pass, categorically

    $ git diff -U0 -- Source/StratUI | grep '^+' | grep -vE '^[+][[:space:]]*(//|[*]|/[*])'
    (only the four +++ file headers)
    CONTROL: the same diff carries 100 added lines under Source/StratUI.

Zero non-comment added lines in `StratUI`. There is no arithmetic to judge — a categorical pass,
not a search that happened to find nothing.

### 10. Lane discipline — pass

Six non-`Tests/` `Source/` files and `engine.md` -> `strat-gameplay-engineer`. Four `Tests/` files
and `tests.md` -> `strat-test-author`. `global.md` -> the `coordinator` and the steward.
`content.md` -> the `coordinator` under the standing fallback clause. No test code outside
`Tests/`, no production code inside `Tests/`, no asset changed.

### 12. Nothing staged — pass, with a control

    $ git diff --cached --stat   -> (empty)
    CONTROL: $ git diff --stat | tail -1  -> 14 files changed, 795 insertions(+), 31 deletions(-)

The index is empty against a working tree that demonstrably is not.

---

## Comment-only across all TEN `Source/` files — CONFIRMED, and the instrument had a real defect

All ten re-derived with my own stripper: whole-line and trailing `//`, block-comment interiors,
blank lines, CR-normalised, string- and char-literal aware, and **refusing to certify on a
zero-line projection**.

    SAME  head=221  work=221   Source/StratPlay/StratMatchSubsystem.h
    SAME  head=36   work=36    Source/StratPlay/StratShellGameMode.h
    SAME  head=23   work=23    Source/StratPlay/StratShellMenuWidget.h
    SAME  head=123  work=123   Source/StratPlay/StratShellSubsystem.h
    SAME  head=694  work=694   Source/StratPlay/Tests/StratMatchCompletionRecording.cpp
    SAME  head=957  work=957   Source/StratPlay/Tests/StratShellRouteClauses.cpp
    SAME  head=27   work=27    Source/StratUI/StratCommandBarWidget.h
    SAME  head=163  work=163   Source/StratUI/StratOptionsWidget.cpp
    SAME  head=124  work=124   Source/StratUI/StratScoreboardHUD.h
    SAME  head=114  work=114   Source/StratUI/Tests/StratOptionsModelClauses.cpp
    EXIT=0

All ten figures match the brief's ten, and 694/957 match the test-author's independent counts.

**THE FIRST RUN OF THIS INSTRUMENT REPORTED A FALSE `DIFFERS`, AND THE WAY IT LIED IS WORTH THE
SPACE.** `StratMatchCompletionRecording.cpp` came back `DIFFERS` at **head=694 work=694** — equal
counts, unequal content, which is precisely the case a count-based check cannot see and which
would have read as clean. The projection diff pointed at every line containing a section sign.
Settled at the byte level rather than by re-reading the code:

    HEAD  valid UTF-8   CRLF=0     bare-LF=1188   0xA7=30   C2A7=30
    WORK  valid UTF-8   CRLF=1227  bare-LF=0      0xA7=31   C2A7=31

**Both sides are valid UTF-8.** The defect was mine: the HEAD blob was decoded with the box's
locale codec while the worktree copy was opened as UTF-8, so the comparison was between two
decoders and not between two trees — the control-must-be-the-same-on-both-sides failure, reached
from an unexpected direction. Re-run with both sides decoded identically, all ten are `SAME`.
**Had the asymmetry fallen the other way it would have been a false CLEAN.** Recorded because the
instrument's own count check (694 == 694) was green while the instrument was wrong.

**Four controls, on in-memory copies; nothing written to the tree:**

    CONTROL BASE                         957 lines
    MUTANT A  one CODE line inserted     -> DIFFERS, 957 -> 958         [SPEAKS]
    MUTANT B  one COMMENT line inserted  -> identical, 957              [correctly blind]
    MUTANT C  empty input                -> projection len 0            [guard REFUSES]
    MUTANT D  TestTrue -> TestFalse      -> DIFFERS                     [SPEAKS on an assertion verb]

Mutant D is the one round 4 had to report INCONCLUSIVE; on this file the token is present, so it
is a real control here. No assertion, clause name, include, macro or `UPROPERTY` specifier moved.

---

## ROUND-4 FINDING 1 — VERIFIED REPAIRED, and the generalisation is right

The offending sentence is gone. The 2026-09-06 entry in `tests.md` now opens:

    When this lane re-ran `python Tools/architect/strat_banner_sweep.py` over base `c69e519`
    on 2026-09-06 it printed `SWEEP CLEAN -- no self-contradiction found`; what the sweep says
    on any later tree is what running it says, and this entry does not predict it.

That names an ACTOR, a COMMAND, a BASE COMMIT, a DATE and the OUTPUT, and then explicitly
disclaims the prediction. It is the anchored form, and the second clause is stronger than the
remedy round 4 proposed: it does not merely date the claim, it states the claim has no forward
reach at all. The only surviving occurrence of the old wording is at `tests.md:125`, where it
appears **inside quotation marks as the text being corrected**, which is the right place for it.

**THE GENERALISATION IS CORRECT AND I WOULD NOT NARROW IT.** The bullet reads that this lane's
prior discipline is phrased about FILES THIS LANE DOES NOT OWN, while this sentence described a
SCRIPT'S OUTPUT, which no lane owns — and corrects the anchor rule to cover both, because a claim
about a tool's current output is exactly as perishable as a claim about a file's current contents,
the tool being a reader of those files. That is sound: the sweep is a pure function of the tree,
so its output inherits the tree's perishability exactly. If anything the lane **under**-claims —
the real axis was never ownership but derivation, and any present-tense claim about a derived
property of the tree is perishable whoever owns the inputs. Under-claiming in a correction is the
safe direction, and the lane reached the operative sentence, which is what matters.

**DROPPING THE EXIT CODE — GOOD JUDGEMENT, NOT AN EVASION, THOUGH THE STATED REASON MISDIAGNOSES.**
Three points, and they do not all fall the same way.

1. **The stated reason is wrong.** "Citing an exit code is what produced the finding" is a
   misdiagnosis: what produced it was the unanchored present tense. "EXITCODE=0 when this lane ran
   it over `c69e519` on 2026-09-06" would have been perfectly durable. **But the entry itself does
   not make that mistake** — at `tests.md:128` it gives the correct diagnosis, *"The sentence named
   no moment"*, and cites the anchor rule "four bullets up". I checked that locator: the anchor-rule
   bullet is exactly four bullets above. The file's reasoning is sound whatever gloss the brief put
   on it.
2. **The printed verdict is the better thing to cite**, and this project's own record says so.
   `tests.md:257` records the rule: *"The printed line is still the verdict to READ."* Citing
   `SWEEP CLEAN -- no self-contradiction found` is citing the instrument's verdict rather than a
   proxy for it.
3. **But the exit code IS meaningful here, and I measured it rather than assuming either way.**
   The standing rule is that an exit code is not a verdict until measured on a FAILING run — and
   round 4's control was `--self-test`, which exits 0 when all its fixtures behave, so it is not a
   failing run at all. I built one, against a **disposable copy** of the six record files in a
   scratch directory outside the repository, with the live suite figure mutated 435 to 434:

        CONTROL, unmutated copy : SWEEP CLEAN -- no self-contradiction found      EXITCODE=0
        MUTATED copy            : [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree
                                  with the report's own succeeded count (435): global.md:28: 434/435
                                  SWEEP FAILED -- the record contradicts itself or the tree
                                  EXITCODE=1
        Real tree after both runs: git status --porcelain -> 18 lines, unchanged.

   So `SWEEP CLEAN` at exit 0 is a real verdict from an instrument that demonstrably exits 1 and
   says `SWEEP FAILED` when the record contradicts the tree. **This also independently confirms the
   measured correction the same lane recorded at `tests.md:254-259`** — that the standing claim
   "the sweep exits 0 even when it prints SWEEP FAILED" is false.

**Net: the entry is true, anchored and re-runnable, and a reader who wants the exit code has the
command.** The small cost — the lane established the exit code's meaning one entry below and then
omitted the number — is an Observation, not a finding.

## The second passage — VERIFIED, and it did contradict its own file

Confirmed at `tests.md:269-279`, inside the **2026-09-05** entry, one entry below the offending
one. Properly anchored and past-tense throughout:

    AND THE FIRST DRAFT OF THE BULLET ABOVE COMMITTED, IN THIS FILE, THE EXACT DEFECT THE ENTRY
    ABOVE IT DIAGNOSES. ... All three were true when written and all three were false within
    the hour ... and the sweep, re-run here, printed `SWEEP CLEAN -- no self-contradiction
    found` with no findings at all.

**So the round-4 finding is confirmed sharper than round 4 stated it.** The offending sentence did
not merely lack an anchor — it asserted "exits 1 ... with one finding" while a paragraph **later in
its own file**, written the day before by the same lane, recorded that the sweep *"printed SWEEP
CLEAN ... with no findings at all."* The file contradicted itself across two entries and the older
entry was the correct one. That belongs on the record: the lane had already written this lesson
down and then re-committed the defect one entry up, which is exactly why restating the rule in the
new entry — rather than assuming it was carried — was the right repair.

## ROUND-4 FINDING 2 — VERIFIED REPAIRED, and the move severed nothing else

**The sentence is whole.** At `global.md:203-205` the worktree now reads, contiguously:

    The layout pass that followed -- centring the panel over
    a dark scrim and mirroring the command bar's options button against End Turn -- is
    `content.md`'s. THAT PASS HAS SINCE BEEN SEEN TOO: later the same day, after the assets were

byte-identical to the same sentence at `:190-192` of `git show HEAD:Tools/architect/state/global.md`.
The attribution of the layout pass to `content.md` is restored and no longer reads as attributing
the `coordinator`'s banner reasoning to a record file.

**The note reads correctly in its new position**, `global.md:21-27`, immediately under the banner
headline and above the suite figure — which is where the reader who takes the wrong implicature
actually is. That closes my round-4 Observation, which asked for exactly this and said six words in
the banner would do it; seven lines under the banner do it better, because they name the check and
quote what it compares.

**The move severed nothing else.** `git diff -U4` on `global.md` shows exactly three hunks — the
banner date plus the relocated note, the `reportCreatedOn` line, and the WHAT THE REPORT DOES NOT
COVER stamp at `:143` — and no residue at the old insertion point.

**One thing I checked and am NOT reporting**: the banner's opening `(` after `_Last run 2026-09-06`
never closes within the block. That is **pre-existing at HEAD** — measured on both sides, the paren
depth returns to zero at `L274` at HEAD and `L287` in the worktree, a shift of exactly the 13 added
lines. The insertion neither created it nor worsened it.

---

## THE TWELFTH INSTANCE — FOUND, IN SCOPE THAT WAS CLAIMED COVERED

`Source/StratUI/StratViewModel.cpp:412-418`, a comment block in the view-model builder:

    // `bDone` and `bLockedThisTurn` are DELIBERATELY LEFT AT FALSE. They are the
    // presentation block's (`strat::UiPresentationUnit`), their owners are §2.11.1's selection
    // machine and the guidance layer, and neither exists in this engine yet. ...

**"neither exists in this engine yet" is false, and both halves are false.**

    $ git ls-files Source/StratPlay/StratSelectionMachine.* Source/StratPlay/StratGuidedOpening.*
      StratGuidedOpening.cpp, StratGuidedOpening.h, StratSelectionMachine.cpp, StratSelectionMachine.h
    $ git log --reverse ... -- .../StratSelectionMachine.cpp -> ed27d5a 2026-08-12
    $ git log --reverse ... -- .../StratGuidedOpening.cpp     -> 1d6f758 2026-08-21
    CONTROL, a file that genuinely does not exist:
    $ git ls-files Source/StratPlay/StratDoesNotExist.cpp   -> 0

`--reverse` is used because the question is ORIGIN, per this project's recorded rule that the plain
form answers *most recent*. The selection machine predates this comment's subject by over three
weeks and the guidance layer by two.

**And it is not merely that the files exist — the named owner writes the exact two fields, on a
live route.**

    Source/StratPlay/StratSelectionMachine.cpp:440   U.bDone           = DoneUnits.Contains(U.UnitId);
    Source/StratPlay/StratSelectionMachine.cpp:441   U.bLockedThisTurn = LockedUnits.Contains(U.UnitId);
    Source/StratPlay/StratPlayerController.cpp:1457  SelectionMachine.DecorateViewModel(Model);

The ROUTE was checked rather than the mechanism, per this project's recorded rule that a reflected
verb with no caller reads as built. `DecorateViewModel` has a caller in the shipped controller.
`StratSelectionMachine.h:393` states the transition in its own past tense: *"`bLockedThisTurn` had
no producer at all before this file."*

**The file carries no stamp of any kind** — a grep for `STAMPED`, `FALSE SINCE`, `[READ:` and
`CONDITION MET` over `StratViewModel.cpp` returns nothing, while the same tokens return 2 in
`Source/StratUI/StratCommandBarWidget.h`, a file this pass did stamp.

**What survives in that comment, and it is most of it.** The first sentence — that the two bits are
DELIBERATELY LEFT AT FALSE by this builder — is still exactly true, and the reasoning that follows
(setting them from `hasMoved`/`hasActed` would invent the DONE bit, wrong in the Wait and
RMB-in-MOVED cases) is still correct and still valuable. **Only the clause "and neither exists in
this engine yet" is false.** The practical consequence is small — the selection machine overwrites
these fields downstream, so no behaviour is wrong — but the sentence sends a reader looking for two
classes that have been sitting in `Source/StratPlay/` for a month.

**TWO THINGS I NEARLY REPORTED AND WITHDREW, both because a control refused them.**

- **`StratViewModel.h:575`, "OWNER: the guidance layer" for `bLockedThisTurn`, is NOT a finding.**
  It looks contradicted by `StratGuidedOpening.h:271` (*"IT DOES NOT WRITE `bLockedThisTurn`. That
  bit is the selection machine's to publish"*) — but that same sentence continues *"...from
  `LockedUnits`, which this class filled in `Observe`."* Owner-who-decides and publisher-who-writes
  are distinguished deliberately and consistently across all three files. Reporting it would have
  been the rationale-block error of reading a slogan instead of the members.
- **I do NOT claim the engineer's sweep instrument was blind to this line.** I tested its ten
  published patterns against `:414` and none matched — but the control refuses the inference: the
  same ten patterns also fail to match `StratCommandBarWidget.h:256`, a line the engineer
  demonstrably **did** catch and stamp. So the entry's pattern list is illustrative rather than
  complete, and I cannot say from a checkout why this line was not caught. **The honest statement
  is that the file was inside a claimed denominator and the line was not stamped** — not that any
  named instrument failed.

**Owner: `strat-gameplay-engineer`.** `Source/StratUI/StratViewModel.cpp` is non-`Tests/` `Source/`.
The repair is one bracket in the house idiom, naming both classes and their landing commits.

## Why this does NOT gate, stated rather than assumed

**`Source/StratUI/StratViewModel.cpp` is not in this pass's diff.** `git status --porcelain` on that
path returns nothing; the defect predates `c69e519` and all five of my rounds. It violates none of
the twelve constraints — there is no checklist item reading "no stale comment anywhere under
`Source/`" — and rounds 1-4 blocked, correctly, on defects the pass under review had **introduced or
left in its own artifacts**. Both of round 4's are repaired and verified at their instruments.

Blocking round 5 for a month-old comment in an untouched file would make this gate a ratchet that
can never close: the same reasoning would retroactively have blocked rounds 1 through 4, and would
block round 6 on whatever the next sweep turns up in the other 2,206 files. **The finding is
dispatched, not suppressed** — it should be fixed in a follow-up pass, and a follow-up pass is not
this pass.

---

## The coverage arithmetic — asked for, computed, and it is NOT total

Nobody had made this claim, so here it is with its denominators. Sets taken from `git ls-files`:

| Set | Definition | Files |
|---|---|---|
| A | `Source/StratPlay` + `Source/StratUI`, non-`Tests/`, `.h`/`.cpp` — engineer | 71 |
| B | any `/Tests/` directory — test-author | 107 |
| C | `Tools/editor`, `Config`, `.agents`, `.claude`, `CLAUDE.md`, `StratBridge` non-`Tests/` — me, round 4 | 46 |
| D | `Tools/architect/state/` record files | 6 |
| | **UNION** (measured; the four sets are pairwise disjoint, overlap = 0) | **232** |
| | **TOTAL TRACKED** | **2438** |
| | **UNCOVERED** | **2206** |

232 + 2206 = 2438, checked. **Coverage is 9.5% of the tracked tree. It is not total and it is not
close.**

**Two of the figures re-derived — one confirmed, one was mine to correct.** A re-derives to **73**
if you take all tracked files, and to **71** on `.h`/`.cpp` only; the two extra are
`StratPlay.Build.cs` and `StratUI.Build.cs`, and the engineer's entry says `.h`/`.cpp` in terms, so
**71 is correct and my first derivation was the wrong one.** C is **46**, not the 45 my own round-4
report claimed: `.claude/` holds 13 tracked files and I counted 12 (5 agents + 7 skills), missing
`.claude/settings.json`. Correcting my own arithmetic against myself.

**The uncovered 2206, classified**, because "2206 uncovered" without a breakdown is a scare number:

    Content/            1931      Tools/architect/     173      Source/Stratocracy/   52
    Source/StratRules/    26      (root/other)          15      Data/                  6
    .github/               2      Tools/                 1

Of those, **1976 are binary or non-prose** (chiefly `.uasset`) and **230 carry a text extension**.
Of the 230, `Source/StratRules/` (26) and `Data/` (2) are **vendored** — a stale claim there is real
but unfixable locally and needs an upstream re-vendor — and `Source/Stratocracy/` (52) is the Epic
template module, untouchable for this milestone. **That leaves roughly 150 actionable uncovered text
files, almost all under `Tools/architect/`.**

## My round-5 sweep of new ground — scope, denominator, and result

Scope: the uncovered text-bearing set minus vendored, minus the Epic module, minus
`Tools/architect/gate_reports/` (57 dated historical snapshots, anchored by their own headers) and
minus `.vscode/` (generated). **90 files, 32,150 lines.**

    CANDIDATES                            : 585
    STRONG present-tense absence          : 138
    STRONG and unanchored on the line     : 125
    CONTROL (an impossible token through the SAME collector): 0

**Triaged, and the 125 resolve into four classes, none of them a live defect:**

- **Frozen phase evidence** (`evidence/*/generated/`, `evidence/*/prompts/`, the `blackboard.md`
  files) — the large majority. These are snapshots of what a past phase was given and produced;
  their claims are anchored by being evidence of a dated phase, and several already carry explicit
  `SUPERSEDED` and `RETRACTED>` markers in the house idiom.
- **`Tools/architect/gdd_snapshot/Stratocracy_Prototype_GDD.md`** — claims about the game DESIGN
  ("there is no undamaged-strike bonus"), not about the tree. Wrong shape entirely, and that
  snapshot's own `MANIFEST.md:27` already declares it is not hash-gated and can go stale silently.
- **The two CI workflows — both true, both verified with controls.** `build-and-suite.yml:25`
  *"There is no `pull_request` trigger"*: the file's own `on:` block at `:44-46` is
  `workflow_dispatch` and `push`, so the claim is self-verifying and correct — and it is
  load-bearing security prose about a self-hosted runner. `banner-sweep.yml:10` *"`Saved/` is
  gitignored, so `Saved/AutomationReport/index.json` does NOT exist on a CI checkout"*:
  `git check-ignore -v` returns `.gitignore:68 Saved/*`. True.
- **`Tools/architect/README.md:370`** — *"the presentation block (`UiPresentation`) is still
  unfilled ... so `T-INT-05` remains open."* **Checked and NOT reported.** `strat::UiPresentation`
  is declared at `Source/StratRules/Ui.h:222`, no vendored `.cpp` writes it, and
  `StratViewModel.h:556` confirms the engine built its own presentation block rather than the rules
  module's — so the first half is true. The `T-INT-05` half is about a ledger row, not an artifact,
  and is not mine to adjudicate.

**Zero new instances in the previously-uncovered scope — the second consecutive null on new ground.**
The twelfth instance came from somewhere else entirely: from following this README's citation back
into `Source/StratUI/`, which is set A.

## Is the tree clean of this shape? No — and the round-4 null was method reach

The brief asks me to answer this against my own report, so: **round 4's null was a true statement
about a 46-file scope and a false implicature about the tree.** I hedged it then; I can now say it
as a measurement, because the twelfth instance turned up in **set A, which was claimed covered**,
while my round-5 sweep of genuinely new ground again found nothing. Two nulls on new scope and one
hit in old scope is evidence that **the remaining instances are inside the covered scope, behind the
sweeps' phrasings — not out in the uncovered tail.**

The mechanism is now visible. Every sweep in this pass keyed on a phrase list, and every phrase list
was assembled from the instances already known. `:414` says *"neither exists in this engine yet"* —
an absence expressed through a **negative pronoun** (`neither`) with a **positive verb** (`exists`),
where the lists are built almost entirely from negated verbs (`does not exist`, `has no`, `there is
no`, `nothing asserts`). **A collector assembled from known instances cannot see a paraphrase of
one**, and the honest inference is that a thirteenth exists somewhere in the same 232 files, in a
phrasing nobody has hit yet.

**The convergence question, answered plainly.** Twelve instances across twelve passes, none found by
the pass that created it. But the more useful figure is a different one: **this instance was created
by no pass in this milestone at all.** It is a month-old comment that four gates and two lane sweeps
read past. That is a better sign than it sounds — the passes are no longer generating instances
faster than they are found, and what is left is a backlog in prose written before the shape had a
name. **I do not think this pass is failing to converge. I think the sweeps have converged on their
own phrase lists rather than on the shape**, and the next useful move is not a thirteenth sweep with
a longer list of negations but a pass over the 232 covered files for the *positive* verbs —
`exists`, `lands`, `arrives`, `will have`, `has somewhere to put` — which is the half no list has
carried.

---

## Findings

**None that gate.** All twelve constraints pass, each with a control. Both round-4 findings are
verified repaired at their instruments.

The twelfth instance — `Source/StratUI/StratViewModel.cpp:414`, *"and neither exists in this engine
yet"*, false of a tree that has held both named classes since 2026-08-12 and 2026-08-21 — is
reported in full above and dispatched to **`strat-gameplay-engineer`**. It is pre-existing, outside
this pass's diff, and violates none of the twelve; the reasoning for why it does not gate is stated
under its own heading rather than left to be inferred.

## Observations

Non-gating.

- **My own comment-only instrument reported a false `DIFFERS` at equal line counts (694 vs 694)
  before it reported the truth**, because the two sides were decoded by two different codecs. The
  failure was legible only at the byte level. Recorded because the near-miss runs the other way: the
  same asymmetry with the operands swapped is a false CLEAN, and nothing in the run would have said
  so.
- **My round-4 report said four `IMPLEMENT_MODULE` sites; there are three.** `Stratocracy.cpp` uses
  `IMPLEMENT_PRIMARY_GAME_MODULE`. Corrected here rather than patched into the round-4 file, which
  stands as the record of its round. The conclusion it supported is unaffected.
- **My round-4 report said 45 files in scope C; there are 46.** I counted `.claude/` as 12 (5 agents
  + 7 skills) and it holds 13 tracked files; the missed one is `.claude/settings.json`. The null it
  reported is unaffected — I re-swept the superset here.
- **The brief's "71" is right and my first derivation of it was wrong.** Recorded because I was
  about to raise it as drift, and the engineer's entry names its `.h`/`.cpp` restriction in terms.
- **`tests.md`'s new bullet says the corrected sentence is "THREE PARAGRAPHS UP"; it is the entry's
  opening paragraph, eleven bullets up.** Not raised as a finding: the same bullet quotes the
  offending text verbatim immediately after, and this project's own rule is that quoted words beat
  line numbers — the quote makes the locator redundant. Worth noting only because the same bullet's
  other locator, *"the anchor rule four bullets up"*, is exactly right, so the file is precise where
  precision carries weight.
- **The exit code the lane established one entry below is now absent from the entry above it.**
  `tests.md:254-259` measured and corrected a standing false claim about `strat_banner_sweep.py`'s
  exit code; the 2026-09-06 entry then cites only the printed verdict. Both are true and the printed
  line is the better citation, but a reader arriving at the newer entry does not learn that the exit
  code means anything. One parenthesis would carry it forward.
- **`content.md`'s brackets STILL carry no writer attribution — now raised in rounds 1, 2, 3, 4 and
  5.** `content.md:172` and `:177` say *"A gate caught it"* and *"A gate caught the bad reason"* with
  no actor and no date-line, while the surrounding brackets DO carry dates. **Five identical raisings
  across five rounds is a standing condition**, and this project has already recorded that a repeated
  exception is a condition rather than a series of incidents. It should be RULED — that in-place
  record brackets need no attribution — or fixed. Raising it non-gatingly a sixth time is the one
  outcome that resolves nothing, and I am declining to pretend otherwise.
- **The `PARTLY`/`FULLY` distinction from round 4 deserves to outlive this pass.** A `DOES NOT PIN`
  block is a SET of residues; a clause closing one residue discharges the block only if the block
  named exactly one. Neither the instrument rule nor the anchor rule catches a partial discharge —
  only reading the discharging clause's own `does not pin` list does.

## Disclosure — this reviewer wrote nothing to the tree under review

`git status --porcelain` returned **18 lines before this audit and 18 after it**, and
`git diff --stat` returned `14 files changed, 795 insertions(+), 31 deletions(-)` both times. Every
mutation control ran against in-memory copies inside the reviewing process. The failing-sweep control
ran against a **copy of the six record files in a scratch directory outside the repository**; the
originals were never opened for writing, and the sweep was re-run against the real tree afterwards
with the same `SWEEP CLEAN` / `EXITCODE=0` result. The only file this reviewer created is this
report, which becomes a fifth untracked entry under `Tools/architect/gate_reports/`.

VERDICT: PASS
