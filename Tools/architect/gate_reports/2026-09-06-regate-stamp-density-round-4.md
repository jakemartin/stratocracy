# Gate: content.md stamp density, RE-GATE round 4 (post-`c69e519`) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, head `c69e519`.
Measured, not taken from the brief:

    $ git status --porcelain
     M Source/StratPlay/StratMatchSubsystem.h
     M Source/StratPlay/StratShellGameMode.h
     M Source/StratPlay/StratShellMenuWidget.h
     M Source/StratPlay/StratShellSubsystem.h
     M Source/StratPlay/Tests/StratMatchCompletionRecording.cpp
     M Source/StratPlay/Tests/StratShellRouteClauses.cpp
     M Source/StratUI/StratCommandBarWidget.h
     M Source/StratUI/StratOptionsWidget.cpp
     M Source/StratUI/StratScoreboardHUD.h
     M Source/StratUI/Tests/StratOptionsModelClauses.cpp
     M Tools/architect/state/content.md
     M Tools/architect/state/engine.md
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md
    ?? (three prior gate reports, this reviewer's own)
    $ git diff --stat | tail -1
     14 files changed, 778 insertions(+), 31 deletions(-)

FOURTEEN modified, three untracked, nothing staged. This report supersedes none of rounds 1-3;
all four stand.

**THE BRIEF SAYS "COMMENT-ONLY ACROSS ALL SIX `Source/` FILES" AND THERE ARE TEN.** The brief's
own eight-figure list -- `221/36/23/123/27/163/124/114` -- is the eight files round 3 audited, and
omits the two files it elsewhere names as NEW this round. Both the noun and the list undercount.
Corrected and re-derived below over all ten; the conclusion is unchanged.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — with a control |
| 2 | No `strat::` call outside `StratBridge/` or `StratRules/` | pass — with a control |
| 3 | No vendored header in front of UHT | pass — with a control |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass — with a control |
| 5 | Module arrows unchanged and correct | pass — DERIVED, 16 rows / 10 fields, guard read at source |
| 6 | New modules registered — unless they have no module object | pass — no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass — with a control |
| 8 | No `/Game/` path literal in C++ | pass — with a control; both added hits are comment lines |
| 9 | No widget-side arithmetic | pass — the `StratUI` diff has ZERO non-comment added lines |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present / record accurate | **BLOCK** — Findings 1 and 2 |
| 12 | Nothing staged or committed | pass — with a control |

### 1. Vendored bytes untouched — pass, with a control

    $ git diff --stat -- Source/StratRules Data
    (nothing)
    CONTROL, same instrument on a path that DID move:
    $ git diff --stat -- Source/StratPlay
     6 files changed, 234 insertions(+), 18 deletions(-)

Manifests are the authority and neither moved:
`rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`,
`dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`.

### 2. `strat::` calls — pass, with a control

    $ grep -rn -E "strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(" Source/ --include=*.cpp --include=*.h \
        | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
    (nothing)
    CONTROL, same pattern inside the permitted directory:
    Source/StratBridge/StratBridge.cpp:168: strat::parseScenario(ToStd(Text), ...)
    Source/StratBridge/StratBridge.cpp:175: strat::validateScenario(Parsed, Units, Terrain);
    Source/StratBridge/StratBridge.cpp:182: if (!strat::seedFromScenario(GameState, Parsed, ...))

Zero hits outside the two directories; the pattern is shown able to match. No type name reported
as a violation.

### 3. Vendored header in front of UHT — pass, with a control

Seventeen include hits across 8 headers: `Source/StratBridge/StratBridge.h` and
`Source/StratRules/{Ai,Driver,Economy,Move,Replay,Scenario,Ui}.h`. Each was tested for reflected
types and each returns **0**:

    $ grep -cE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)' <each of the 8>   -> 0 x8
    CONTROL: the same instrument on Source/StratPlay/StratShellGameMode.h -> 1

Run the other way as well -- every one of the 59 headers under `Source/` that DOES declare a
reflected type, tested for a vendored include -- returns nothing.

### 4. `TPimplPtr` — pass, with a control

    $ grep -rn "TUniquePtr<FStratBridge>" Source/
    Source/StratUI/StratViewModel.h:47: // ... the `C4150` measured on `TUniquePtr<FStratBridge>` ...

One hit, a prose citation of the measurement, not a declaration. CONTROL: 15 `TPimplPtr` sites
across `Source/**/*.h` -- unchanged from round 3, so the count that moved 14->15 last round has
settled and needs no further explanation.

### 5. Module arrows — pass, DERIVED, 16 rows and 10 fields

The checklist's extraction was extracted and RUN. **16 rows**, all five modules present, so the
extraction did not fail silently. Identical to round 3's derivation, including
`StratBridge PrivateDependencyModuleNames StratRules` as row 3. That row was read at its source
rather than treated as unconditional -- `Source/StratBridge/StratBridge.Build.cs`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)

Monolithic-only link edge, deliberate, not a finding. The field census printed **10**, including
the known `BRIDGE=` false positive that is the tail of a string literal. No eleventh name.
No `.Build.cs` is in this diff; the derivation is a drift check, not a change review.

### 6. New modules — pass

Exactly five module directories, none new. `Stratocracy.uproject`'s Modules array names
`Stratocracy`, `StratBridge`, `StratUI`, `StratPlay` (plus plugin modules). **`StratRules` is
absent and must stay absent** -- a grep for `IMPLEMENT_MODULE` across `Source/**/*.cpp` returns
declarations in `StratBridge.cpp`, `Stratocracy.cpp`, `StratPlay.cpp` and `StratUI.cpp` and
**none in `Source/StratRules/`**. Correct as-is.

### 7. `Source/Stratocracy/` untouched — pass, with a control

    $ git diff --stat -- Source/Stratocracy
    (nothing)
    CONTROL: $ git diff --stat -- Source/StratPlay   -> 6 files changed

### 8. `/Game/` literals — pass, with a control

    $ git diff -U0 -- Source/ | grep '^+' | grep '/Game/'
    +//   `coordinator` building `/Game/UI/WBP_Options` under CLAUDE.md's EDITOR-DRIVER CLAUSE,
    +//   `GlobalDefaultGameMode=/Game/StratPlay/BP_StratGameMode`. Both resolve to a GameMode that

Both added hits are comment lines. CONTROL: the same diff carries **340** added lines under
`Source/`, so the filter had material to work on.

### 9. Widget-side arithmetic — pass, categorically

    $ git diff -U0 -- Source/StratUI | grep '^+' | grep -vE '^\+\s*(//|\*|/\*)'
    (only the four +++ file headers)
    CONTROL: the same diff carries 100 added lines under Source/StratUI.

Zero non-comment added lines in `StratUI`. There is no arithmetic to judge. A categorical pass,
not a search that happened to find nothing.

### 10. Lane discipline — pass

Six non-`Tests/` `Source/` files and `engine.md` -> `strat-gameplay-engineer`. Three `Tests/`
files and `tests.md` -> `strat-test-author`. `global.md` -> the `coordinator` and the steward.
`content.md` -> the `coordinator` under the standing fallback clause, carried from round 3
unchanged. No test code outside `Tests/`, no production code inside `Tests/`, no asset changed.
`git worktree list` shows slot-1 and slot-2 standing on feature branches, but no code was written
in this pass under the in-session clause and none is claimed, so that clause has no subject here.

### 12. Nothing staged — pass, with a control

    $ git diff --cached --stat
    (empty)
    CONTROL: $ git diff --stat | tail -1  -> 14 files changed, 778 insertions(+), 31 deletions(-)

The index is empty against a working tree that demonstrably is not.

---

## A CORRECTION TO MY OWN ROUND-3 REPORT — I CORRECT IT, I DO NOT DEFEND IT

Round 3 wrote that the stamps at `:1448` and `:1455` were *"six and four lines above the offending
sentence."* **That is false, and the report's own quoted numbers contradict it.** Re-measured
against the tree round 3 audited:

    $ git show HEAD:Source/StratPlay/Tests/StratShellRouteClauses.cpp | grep -n "STAMPED"
      59, 73, 80, 146, 166, 255, 1448, 1455, 1624        (nine, as reported)
    $ git show HEAD:... | grep -n "those assets do not exist yet"
      1854

`:1448` is **406** lines above and `:1455` is **399** lines above. The nearest stamp above the
sentence is `:1624`, **230** lines up; the nearest stamp of any kind is `:1856`, two lines BELOW,
and that one is the remedy this round is judging, not a neighbour that preceded it.

**What survives and what I withdraw.** The finding's substance is untouched: the file carries nine
corrections in the house idiom, so the remedy was the file's own established form and not a
convention it lacked, and the sentence went uncorrected through all nine. **I withdraw the
characterisation** -- *"A reader landing on line 1854 is inside a region whose neighbours were
corrected and this one was not"* -- which was a claim about proximity, was doing rhetorical work in
the finding, and is not supported by the numbers the same paragraph prints. Two figures were
subtracted from the wrong operand; no instrument in that report would have caught it, and the
reader did. Recorded flat here rather than patched into the round-3 file, which stands as the
record of its round.

---

## The judgement the brief asked for first: is `PARTLY` right, or a dodge?

**`PARTLY` IS RIGHT. Stamping `DISCHARGED` would have swapped a stale sentence for a false one.**
Verified at the discharging clause's own text, not at the stamp's summary of it.

The stale sentence is `Source/StratPlay/Tests/StratShellRouteClauses.cpp:1852-1854`:

    // WHAT IT DOES **NOT** PIN, and the debt is real: a Blueprint that OVERRIDES either property
    // re-opens exactly the drift this pins in C++. That discharges on a clause reading both
    // Blueprint CDOs, and those assets do not exist yet.

The drift *"this pins in C++"* is named in the same block twenty lines up: *"`AStratShellGameMode`'s
`SaveSlotName` class default equals `FStratMatchConfig`'s, because the constructor derives it
rather than restating the string."* So the question is whether the new parity clause protects that
DERIVATION from Blueprint overrides. It does not, and **it says so itself.**
`Source/StratPlay/Tests/StratShellBlueprintSlotParity.cpp:72-76`, in its own
`WHAT THIS CLAUSE DOES **NOT** PIN` list:

    //   - It does not pin either value against `FStratMatchConfig`'s C++ default. That is
    //     Stratocracy.StratPlay.GATE-TITLEMENU.TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn,
    //     which pins the NATIVE defaults and is a different subject. Asserting it here as well would
    //     redden on a designer who renamed the slot correctly on both assets, which is a change this
    //     clause must permit.

And at `:57-62`, from the other direction: *"The clause asserts a RELATION between two independent
reads and never states what the slot ought to be called, so it stays green over a deliberate
rename that is applied to both."*

**So the two readings of `either` are genuinely different and the lane's split is exact.** ONE
asset overridden: the CDOs disagree, the clause reddens -- covered. BOTH overridden to the same new
string: the CDOs agree, the clause stays green, and the C++ derivation has stopped deciding what
ships -- not covered, and deliberately so, because covering it would forbid a legitimate rename.
A `DISCHARGED` stamp would have asserted the debt block closed when half of it is open **by the
discharging clause's own design**, which is a stronger claim than the stale sentence made.

**The third-asset point is also true and also verified.**

    $ git ls-files Content/StratPlay/
      ... BP_StratGameMode.uasset, BP_StratGameMode_AiVsAi.uasset, BP_StratShellGameMode.uasset ...
    $ grep -rn "AiVsAi" <both clause files>
      StratShellBlueprintSlotParity.cpp:77: "It does not pin `BP_StratGameMode_AiVsAi` or any other subclass."
      StratShellRouteClauses.cpp:1887:      the stamp's own statement of the same fact

`BP_StratGameMode_AiVsAi` is tracked, is a third asset of the first class, and is read by neither
clause. CONTROL that the instrument can find the name: ten files under `Source/` mention it,
including `StratShippedGameModeOptIn.cpp` and `StratShippedSoundBankParity.cpp`, so its absence
from these two is a measured absence and not a mute grep.

**This is the correct call and it is made against the lane's own interest** -- the same shape as
the engineer's three-of-five accounting last round. A full discharge was the flattering answer and
the lane declined it.

## The dispatched finding's own evidence, re-derived

    $ git ls-files Content/StratPlay/BP_StratGameMode.uasset Content/StratPlay/BP_StratShellGameMode.uasset
      both returned
    $ git log --reverse --diff-filter=A -- <each>   -> d310aa1, e4a21b0
    CONTROL, an asset that genuinely does not exist:
    $ git ls-files Content/StratPlay/BP_ThisDoesNotExist.uasset   -> (nothing)

The stamp's insistence on spelling out `Content/StratPlay/` rather than `Content/Blueprints/`, and
on running the negative control before reading an ABSENT as an absence, is correct practice and is
the specific error the brief records itself making earlier in this pass. Both halves confirmed.

## The two undispatched true positives — both hold, both verified at their instruments

`Source/StratPlay/Tests/StratMatchCompletionRecording.cpp:997` stamps three sentences false. All
three are false:

- *"the packaged game's guided opening re-arms every match"* --
  `Source/StratPlay/Tests/StratShippedGameModeOptIn.cpp:216` asserts
  `Cdo->MatchConfig.bRecordCompletionOnMatchEnd` off `BP_StratGameMode_C` (path literal at `:105`),
  and `:206` states the §2.11.6 expectation in terms. The shipped asset DOES opt in.
- *"That clause is owed"* -- it is written. `:115` names
  `"Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn"`.

        $ git log --reverse --format='%h %ad' --date=short -- .../StratShippedGameModeOptIn.cpp
          7d8185f 2026-08-21
        $ git log --reverse -S "has no such checkbox yet" -- .../StratMatchCompletionRecording.cpp
          f7b934a 2026-08-21

  The debt note and its discharge landed **the same day**, in one directory, and the note stood for
  a fortnight.
- *"`Tools/architect/state/tests.md` [records it as owed]"* -- `tests.md:6508` opens
  **"THE OWED CLAUSE IS WRITTEN."** The cited authority was corrected; the citing site was not.

The cross-citation claim holds too: `StratShippedGameModeOptIn.cpp:4` names
`StratMatchCompletionRecording.cpp` and `:22`, `:49` and `:134` cross-cite
`T-SAVE-06.TheOptInDefaultsOffInCpp`, which lives there. The citation ran one way, exactly as the
entry says. **This is the more serious of the two** -- a stale debt note is a navigation cost; a
false present-tense claim about what the packaged game does sends a reader hunting a §2.11.6 bug
that was fixed on 2026-08-21.

## The `Tests/` sweep denominator — re-derived, and it re-derives EXACTLY

    $ git ls-files | grep '/Tests/' | wc -l                    -> 107
        Source/StratBridge/Tests 18, Source/StratPlay/Tests 54, Source/StratUI/Tests 33,
        Source/Stratocracy/StratData/Tests 1,
        Tools/architect/evidence/02-scoreboard-widget/generated/Source/StratUI/Tests 1
    $ (same list) | xargs wc -l | tail -1                       -> 83357

**107 exact.** The 107th is the generated copy under `Tools/architect/evidence/`, which a
`Source/*/Tests/*` glob misses -- that glob returns 106 -- so the figure is not only right, it is
right for a reason a looser derivation gets wrong. The line figure re-derives to the line:
**83357 today = 83277 + the 80 lines this pass's own two stamps added** (39 + 41, from
`git diff --stat`). The sweep measured the tree it swept and did not count its own output, which is
the a-record-that-measures-itself rule observed correctly and without being asked.

**The control it cites re-derives too:** files under any `Tests/` directory carrying
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` -> **91**, exactly as claimed.

## Shape 3, mechanical citation checking — is the declared limit fatal to the null?

**NOT FATAL, AND ACCEPTABLE, BECAUSE IT IS DECLARED AND BECAUSE THE NULL IS SCOPED TO WHAT WAS
ACTUALLY MEASURED.** The claim is *"Zero stale verbatim citations"* -- a claim about whether quoted
strings still EXIST -- and not *"zero misleading citations"*. A declared limit turns a null from a
claim about truth into a claim about a named property, which is the only kind of null this project
accepts.

**But the blind spot is precisely the shape this same lane stamped the day before, and that is
worth stating plainly.** If an authority appends a correcting sentence AFTER a quoted span, the
span still matches and the check passes while the citation misleads -- which is exactly
`StratOptionsModelClauses.cpp`'s 2026-09-05 finding, recorded in the entry directly below as
*"the correction landed at the cited file and did not propagate to the site citing it."* So the
instrument cannot see the defect its own lane named as this pass's new lesson.

**It did not bite here**, because this pass's four stale citations had text inserted INSIDE their
spans, which breaks the span -- and the entry reports them as detected and triaged, not as missed.
Recorded as an Observation, not a finding: the null is honest, the limit is stated, and the gap is
in the instrument rather than in the claim.

## Shape 3, the declined stamps — the restraint was RIGHT, not a missed instance

Four sites carry the engineer's insertions inside sentences this lane quotes verbatim:

    StratShellGameMode.h:165   ...once the editor lane has created them [CONDITION MET; SEE THE STAMP BELOW].
    StratShellGameMode.h:199   DISCHARGED BY a clause reading both [READ: THREE] Blueprint CDOs...
    StratShellGameMode.h:271   DISCHARGED BY a clause reading both [READ: THREE] Blueprint CDOs...
    StratMatchSubsystem.h:368  DISCHARGED BY a clause comparing the two [READ: three] CDOs...

Each quoting site reconciles **in its own next paragraph**, verified at the text and not taken from
the entry's summary:

- `StratShippedOptionsWidgetClassParity.cpp:18-19` quotes the pre-insertion sentence; `:24`
  immediately opens **"THREE ASSETS AND NOT TWO, WHICH IS THE ONE PLACE THIS DEPARTS FROM ITS
  PRECEDENT"** and explains that `BP_StratGameMode_AiVsAi` is read here.
- `StratShippedSoundBankParity.cpp:20-27` quotes both headers; `:29` opens **"Both headers say
  TWO, and they are right about CLASSES and understate the ASSETS"**, distinguishes two classes
  from three assets, and explains why the clause is NAMED for three -- *"A clause named `Both...`
  that asserted over three would be an overclaiming name of exactly the kind this project already
  renamed a clause to fix."*
- `StratShellBlueprintSlotParity.cpp:6-13` quotes the `SaveSlotName` block, whose insertion is
  `[CONDITION MET]` and not a count correction -- and that clause really does read TWO, so there
  was nothing to reconcile. `:13` supplies the condition anyway: *"W6's asset tail created
  `BP_StratShellGameMode` and the debt became payable. This file pays it."*

**Stamping any of these would have been over-stamping**, which round 3 identified as a finding in
its own right. A citation whose next sentence already states the corrected fact is not a stale
citation; it is a quote followed by its own commentary, which is what a citation is for. The
restraint is correct, and the reason the entry gives for it -- that a dated *"at the time of
writing"* stamp is anchored and therefore cannot itself rot -- is the right general rule and is
the thing that separates these four from a real instance.

## The banner date / headline split — LEGITIMATE, and the check's own text says so

This is the question the brief most wanted answered, so it is answered at the instrument rather
than from the shape of the edit.

**The check does not model a headline at all.** `Tools/architect/strat_banner_sweep.py:45-48`
states its own subject:

    3. BANNER DATE FRESHNESS. The banner in `global.md` ... must not be older than the newest date
       any file in the set carries. The re-gate flagged this as live drift:
       the banner said 2026-08-14 while entries were dated 2026-08-19.

and the implementation at `:1190-1202` compares date tuples only, taking the newest date found in
any of the six record files. **Its own worked example is a record-write drift, not a behavioural
one.** So "the date tracks the record's last write" is not a redefinition invented to silence the
check -- it is the check's definition, in the check's own prose, since it was written.

**The three available moves, and only one leaves no false sentence standing:**

| Move | Result |
|---|---|
| Leave the date at 2026-09-05 | The sweep FAILS. The record is stale by its own rule. |
| Move the date AND rewrite the headline to describe this pass | The banner would assert a behavioural change that did not occur -- no executable byte moved, certified over all ten `Source/` files below. |
| Move the date, keep the headline, declare the split | No false sentence anywhere. |

The third was taken. **That is the opposite of silencing a check.** Silencing is bumping a number
and saying nothing; this bumped a number and wrote seven lines naming the check, quoting what it
compares, and stating which half of the banner answers which question. Measured live:

    $ python Tools/architect/strat_banner_sweep.py ; echo EXIT=$?
      ... report identity: reportCreatedOn='2026.09.06-04.29.34'
      ... suite claims found: 63 (1 live, 62 stamped) ; tracked items declared: 2
      SWEEP CLEAN -- no self-contradiction found
      EXIT=0
    CONTROL -- the sweep is able to say FAIL, and on this exact check family:
    $ python Tools/architect/strat_banner_sweep.py --self-test ; echo EXIT=$?
      [OK] an unstamped citation of a DIFFERENT reportCreatedOn than the one the sweep opened
           FAILS: expected FAIL, got FAIL -- REPORT PROVENANCE
      ... SELF-TEST: ALL FIXTURES CORRECT ; EXIT=0

`SWEEP CLEAN` is a parsed result over 63 collected claims from an instrument whose own fixtures
prove it can fail. Exit code captured by redirect, per this project's rule that an exit code is not
a verdict until measured on a failing run first -- and the self-test is that failing run.

**Edit (a) is also correct.** `global.md:33` now reads
*"THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-04.29.34`"*, which is the stamp of the
report the sweep actually opened. That is REPORT PROVENANCE part (a) satisfied by measurement.

**The one real cost, and it is an Observation rather than a finding.** The declaring paragraph sits
at `global.md:198-204`, roughly 190 lines below the banner it disclaims. A reader who reads only
`_Last run 2026-09-06 (THE GAME HAS A VOICE, ...)` -- the single most-read line in this record --
takes the implicature that the 2026-09-06 pass was the audio pass. This project's own
inline-attribution rule exists for exactly that reader: *a reader arriving by a citation lands on
the entry, not on any header above it.* The same reasoning applies to a disclaimer placed far
below its subject. **The split is right; its declaration is 190 lines from where it is needed.**
Six words inside the banner parenthesis would close it.

## The unsettled property — HONESTLY OPEN, not a dodge

`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:132-137` carries two debts conditioned on the
asset pass, and the entry claims **neither way**. That is the honest posture and I confirm it:

- The only instrument available in a comment-only pass is `grep -a` on a `.uasset`, and it returns
  `TitleMusic` present. **Presence in a package proves nothing about an override**, which is the
  rule `StratShellBlueprintSlotParity.cpp:63-70` states in terms -- *"A property ABSENT from a
  package proves it equals the C++ default ... an asset with no override hands back the C++
  default, an asset with one hands back the override"* -- so a name-table hit cannot distinguish a
  SET property from a merely declared one.
- Settling it needs a CDO read, which is a new automation clause, which is a compiled change, which
  this pass's comment-only scope forbids.

**This is the correct shape for an unsettleable property and it is the one this project has got
wrong most often.** The failure mode is claiming a null from a mute instrument; the entry instead
names the instrument, names why it cannot speak to this subject, and closes with *"The conditions
may already have fired; nothing here claims they have not."* A dodge would have reported it as
swept-and-clear, or omitted it and let the sweep's coverage figure absorb it. It is listed under
its own heading -- **"ONE THING THIS SWEEP COULD NOT SETTLE, AND IT IS NOT A NULL."** Honestly
open, and the debt it leaves is smaller than the debt a false null would have left.

## Comment-only across all TEN `Source/` files — CONFIRMED, my own instrument, controls speaking

The brief says six and lists eight; there are ten. Derived over all ten with a stripper that drops
whole-line `//`, block-comment interiors and blank lines, CR-normalised, `git show HEAD:<file>`
against the worktree copy, and **which REFUSES to certify on a zero-line projection** -- the exact
failure the brief records hitting today:

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

The eight figures the brief lists match mine on the eight files it covers. **The two additional
files -- 957 and 694 -- are the two the brief's list omits, and they are the two it separately
identifies as new this round.**

**Four controls, so the instrument is shown sensitive to exactly what the claim is about and blind
to exactly what it must ignore.** Run against in-memory copies of `StratShellRouteClauses.cpp`;
nothing was written to the tree:

    MUTANT A  one CODE line inserted             -> DIFFERS, 957 -> 958      [SPEAKS]
    MUTANT B  one COMMENT line inserted          -> identical, 957           [correctly blind]
    MUTANT C  empty input                        -> projection len 0         [guard REFUSES]
    MUTANT D  BindWidget -> BindWidgetOptional   -> identical; the token is absent from this file,
              so the probe is reported INCONCLUSIVE and is not counted as a pass

On that instrument, **comment-only is confirmed for all ten files.** No assertion, clause name,
include, macro or `UPROPERTY` specifier moved.

## The eleventh-instance sweep — MY SCOPE, MY DENOMINATOR, AND IT IS A NULL

**Scope, chosen to be exactly what the brief lists as uncovered by anyone:** `Tools/editor/`,
`Config/`, `.agents/`, `CLAUDE.md`, `.claude/agents/`, `.claude/skills/`, and
`Source/StratBridge/` outside `Tests/`.

    FILES         : 45   (Tools/editor 5, Config 5, .agents 6, .claude/agents 5,
                          .claude/skills 7, CLAUDE.md 1, Source/StratBridge non-Tests 16)
    LINES SCANNED : 10285
    CANDIDATES    : 175
    CONTROL (an impossible token through the same collector): 0

Collector, inlined so a later reader can re-run it rather than take it on trust:

    ABS = (does not exist|do not exist|no such|not yet|yet\b|until the|once the|when the|
           unwritten|nothing asserts|no clause|DISCHARGE|will be|owed|absent|has no|have no|
           is not present)      [case-insensitive, per line]

**175 triaged, narrowed to 20 by the strong present-tense-absence shape, and every one resolves.**
The four worth naming, each with a control:

- **`Source/StratBridge/StratCombatLog.h:82`** -- *"DELIBERATELY NOT FIXED WITH AN EXPORT MACRO. No
  such call site exists today"*, about a cross-module `UE_LOG(LogStratBridge, ...)`. **Still true:**

        $ grep -rn "LogStratBridge" Source/ | grep -v '^Source/StratBridge/'   -> (nothing)
        CONTROL: the same token inside Source/StratBridge/                     -> 7 hits

  A real open condition, correctly stated, anchored by *"today"* and naming its own successor
  (`STRATBRIDGE_API` on that line, *"made then, with the caller in hand"*).
- **`.agents/ue-project-context.md:245-246`** -- the two vendored-commit rows. `96d93ea` and
  `c2f5860` both match the manifests read under check 1; both are anchored *"as of 2026-08-25"*;
  and both rows say in their own text that a quoted commit is the same defect not yet triggered and
  that the manifest is the authority. Correct and self-aware.
- **`Tools/editor/make_options_wbp.py:20`** -- *"nothing here can connect a slider's
  OnValueChanged to anything."* Scoped to `here` (this script), and the next sentence names what
  does it instead: `UStratOptionsWidget`'s `BindWidget` members and `NativeConstruct`. Not a claim
  about the tree.
- **`Source/StratBridge/StratBridge.h:1490` and `:1504`** -- *"WHY NO CLAUSE CAN EXIST FOR..."*
  Impossibility claims with their mechanism stated, not absence claims about an artifact.

**`.agents/ue-project-context.md` shows no drift** against this round's own derivation: the module
table at `:38-42` matches all 16 rows; `:40` states the `PublicIncludePaths`-in-every-target versus
monolithic-only-link-edge split and warns against deriving one link type and reporting the other as
a violation; `:82-87` carries the `IMPLEMENT_MODULE` exception for `Source/StratRules/` and says an
audit that flags it *"has this backwards"*; and `:105-114` reasons both branches from the one
expression.

**ZERO new instances in the previously-uncovered scope**, stated with its denominator so that it is
a measurement and not a shrug. **But the pass is not clean** -- the eleventh instance is in the
diff itself, and is Finding 1.

---

## Findings

**FINDING 1 — BLOCK. THE ELEVENTH INSTANCE, AND IT IS INSIDE THE ENTRY REPORTING THE SWEEP FOR
STALE CLAIMS. A flat, present-tense, unanchored claim about what an instrument in this repository
does, which is false of this tree.**

`Tools/architect/state/tests.md`, in the new 2026-09-06 entry, opening paragraph:

    `strat_banner_sweep.py` exits 1 on this tree with one finding, `REPORT PROVENANCE` on
    `global.md:33`, which is the expected and correct consequence of re-running the suite from a
    file this lane does not own.

**Measured, with a control:**

    $ python Tools/architect/strat_banner_sweep.py ; echo EXIT=$?
      SWEEP CLEAN -- no self-contradiction found
      EXIT=0
    $ sed -n '33p' Tools/architect/state/global.md
      THE LIVE FIGURE'S REPORT IS `reportCreatedOn 2026.09.06-04.29.34`.
    CONTROL, that the instrument can say FAIL and can say it on this exact check:
    $ python Tools/architect/strat_banner_sweep.py --self-test
      [OK] an unstamped citation of a DIFFERENT reportCreatedOn than the one the sweep opened
           FAILS: expected FAIL, got FAIL -- REPORT PROVENANCE

It exits **0**, not 1. There is no finding. `global.md:33` is the line the `coordinator` corrected,
and that correction is what falsified this sentence -- which was the sentence's own predicted and
requested outcome. **The mechanism is benign; the residue is not.** Committed as written, this
record permanently asserts that a script in this repository fails on this tree; a reader who runs
it gets `SWEEP CLEAN` and must then decide whether the record or the script is wrong.

**Why this is a BLOCK and not an Observation, in the entry's own words.** The same entry states the
rule that condemns it: *"a bare 'not ... yet' asserts the absence NOW and adds only an expectation
of change, with no anchor"*, and *"a qualifier saves a sentence only if it names a MOMENT."*
*"exits 1 on this tree"* names no moment. The entry's base-commit anchor (`c69e519`) does not reach
it, because the sentence is about the WORKING TREE and not about the commit -- and the working tree
was the thing that changed. This is the identical defect class as round 1's Findings 3 and 4,
round 2's Finding 1 and round 3's Finding 1, now in a sixth file. Not blocking would make four
prior BLOCKs arbitrary.

**The remedy is one clause**, in the anchored form the entry itself argues for: *"as this entry was
written, and before `global.md`'s citation was updated to match, `strat_banner_sweep.py` exited 1
with `REPORT PROVENANCE` on `global.md:33`; the `coordinator` has since updated it and the sweep is
clean."* That records the handoff, keeps the reason the sentence was worth writing, and stays true.

**Owner: `strat-test-author`.** `tests.md` is its file and its sole-writer line. The transcription
clause does not apply: no merge is involved.

---

**FINDING 2 — BLOCK. The banner-split paragraph was inserted INTO THE MIDDLE OF A SENTENCE in
`global.md`, severing it and orphaning its attribution.**

`Tools/architect/state/global.md:196-205`. At HEAD the sentence is intact:

    $ git show HEAD:Tools/architect/state/global.md | sed -n '190,192p'
    ... The layout pass that followed -- centring the panel over
    a dark scrim and mirroring the command bar's options button against End Turn -- is
    `content.md`'s. THAT PASS HAS SINCE BEEN SEEN TOO: ...

In the working tree the seven-line banner paragraph sits between `-- is` and `` `content.md`'s. ``:

    a dark scrim and mirroring the command bar's options button against End Turn -- is
    THE BANNER DATE MOVED TO 2026-09-06 WITHOUT ITS HEADLINE MOVING, AND THAT IS DELIBERATE.
    ... they are different questions and this banner answers both.
    `content.md`'s. THAT PASS HAS SINCE BEEN SEEN TOO: ...

**Two things break.** The sentence naming which lane owns the layout pass no longer parses, so
`content.md`'s ownership of that pass is no longer stated anywhere a reader can follow. And the
orphaned fragment `` `content.md`'s. `` now sits immediately below the banner-split paragraph,
where it reads as attributing that paragraph -- the `coordinator`'s own reasoning about its own
banner -- to a record file that did not write it. **In a record whose governing rule is that acting
and writing are attributed separately and INLINE, a severed sentence that relocates an attribution
is not cosmetic.**

**This is not a defect in the banner split itself**, which this report finds legitimate above. It is
a defect in where the paragraph was placed, and it was introduced by this pass -- the sentence is
whole at `c69e519`. The remedy is to move the paragraph to a paragraph boundary, the natural one
being immediately after *"...produced no pixels of its own."* -- or, better, per the Observation
above, to put six words of it inside the banner parenthesis where the reader who needs it will be.

**Owner: the `coordinator` / the steward**, who own `global.md`.

## Observations

Non-gating.

- **The brief's scope sentence undercounts its own subject twice.** It says *"all six `Source/`
  files"*, lists eight figures, and there are ten modified `Source/` files -- the two omitted being
  the two it separately names as new this round. The conclusion it asserts is correct and I
  re-derived it over all ten. Recorded because a comment-only certification that names the wrong
  denominator is the same shape as a control that cannot fire, which the brief also records hitting
  twice today.
- **Shape 3's mechanical check is blind to the exact defect its own lane named as this pass's new
  lesson** -- a correction appended AFTER a quoted span leaves the span matching. It did not bite
  here, the limit is declared, and the null is scoped to it, so it is acceptable. Worth carrying as
  a known gap rather than re-derived from scratch next time.
- **The one per-shape figure I could not reproduce is shape 4's `267 -> 61`.** My own collector over
  the same 107 files returns 1483 raw and 628 after excluding lines containing `clause`. Neither
  figure is wrong -- they are different collectors -- but the entry does not state its pattern,
  which makes this the only figure in an otherwise precisely-stated entry that a reader cannot
  re-run. One inlined regex would fix it, exactly as the entry does for its comment-only stripper.
- **The banner-split declaration is 190 lines below the banner it disclaims.** The split is
  legitimate; a reader who reads only the banner still takes the wrong implicature.
- **`content.md`'s brackets STILL carry no writer attribution -- raised now in rounds 1, 2, 3 and
  4.** `content.md:172` and `:177` say *"A gate caught it"* and *"A gate caught the bad reason"*
  with no actor, no clause name and no date-line; the surrounding brackets DO carry dates
  (*"FALSE SINCE 2026-09-05"*), so the omission is specifically of the WRITER. `content.md` is
  written by the `coordinator` under the fallback clause, whose own obligation is that acting and
  writing are attributed separately, and an in-place bracket inherits no attribution from a header
  a citing reader never reaches. **Four identical raisings across four rounds is a standing
  condition, not four incidents.** It should now be either RULED -- that in-place record brackets
  need no attribution -- or fixed. Continuing to raise it non-gatingly is the one outcome that
  resolves nothing, and this project has already recorded that a repeated exception is a condition.
- **The `PARTLY`/`FULLY` distinction is the most valuable thing produced in this four-round pass**
  and deserves to outlive it. The general form: a `DOES NOT PIN` block is a SET of residues, and a
  clause that closes one residue discharges the block only if the block named exactly one. Neither
  the instrument rule nor the anchor rule catches a partial discharge; only reading the discharging
  clause's own `does not pin` list does.
- **Both new comment blocks are in the house style and record measurements as measurements** --
  commit SHAs obtained with `--reverse` rather than plain `log -S`, a negative control on a
  nonexistent asset path, clause NAMES rather than line citations wherever a clause name exists,
  and an explicit statement of why the wrong-directory ABSENT would have looked identical to a real
  one.
- **Eleven instances across eleven passes.** The tenth was found by a reviewer in a scope the
  author's own sweep declared out of bounds; the eleventh was created by the pass sweeping for the
  tenth and found by the gate reviewing that sweep. The invariant is intact and is now sharper than
  round 3 stated it: **no pass has ever found the instance it created**, and the instance a pass
  creates tends to sit in the artifact where that pass explains its own method.

## Disclosure — this reviewer wrote nothing to the tree under review

`git status --porcelain` returns the same 14 modified and 3 untracked entries before and after this
audit -- 17 lines both times. Every mutation control was run against in-memory copies inside the
reviewing process; nothing was written to `Source/`, `Content/`, `Config/` or
`Tools/architect/state/`, and `strat_banner_sweep.py` was run twice against an unmodified tree with
the same result. The only file this reviewer created is this report, which becomes a fourth
untracked entry under `Tools/architect/gate_reports/`. Round 3 disclosed a guarded append that
succeeded against `global.md` and was reverted byte-exactly; no such edit occurred this round, and
the guard that failed there was not reused.

VERDICT: BLOCK
