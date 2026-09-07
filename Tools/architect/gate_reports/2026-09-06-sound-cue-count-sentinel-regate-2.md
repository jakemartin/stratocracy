# Gate: `Count` sentinel + sweep fixes -- THIRD GATE (re-gate 2) -- strat-integration-reviewer

## Scope

Uncommitted working tree over base commit `f7da9ca`, branch `master`, nothing staged. Eleven
modified files, no additions or deletions in the tracked set; the two prior reports for this pass
are the only untracked paths.

    Source/StratPlay/StratSoundBank.cpp
    Source/StratPlay/StratSoundDirector.cpp
    Source/StratPlay/Tests/StratShippedSoundBankParity.cpp
    Source/StratUI/StratSoundCues.cpp
    Source/StratUI/StratSoundCues.h
    Source/StratUI/Tests/StratSoundCueClauses.cpp
    Tools/architect/state/data.md
    Tools/architect/state/engine.md
    Tools/architect/state/global.md
    Tools/architect/state/tests.md
    Tools/architect/strat_banner_sweep.py

`Source/StratPlay/StratSoundDirector.cpp` is in the diff and was NOT in either prior report's
file list -- it entered with this correction round. No `Content/` change, no asset change, no new
module, no `.Build.cs` change. I did not build and did not run the suite; `strat-test-author`
reports those. Every premise below was re-measured against the tree or the engine rather than
carried from the prior two reports, and every instrument I cite is inlined here rather than
referenced by a scratch path.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass -- a grep for `strat::` followed by a lowercase-initial identifier and an open paren, across `Source/` minus the two permitted directories, returned zero hits. No type-name hit needed judging this pass |
| 3 | No vendored header in front of UHT | pass -- every `.h` outside `StratRules` carrying `UCLASS(`/`USTRUCT(`/`UENUM(` was scanned for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`; zero hits. `StratSoundCues.h`, the one UHT header this pass edits, includes only `CoreMinimal.h` and its own `.generated.h` |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass -- the only `TUniquePtr<FStratBridge>` in the tree is the historical note at `Source/StratUI/StratViewModel.h:47` |
| 5 | Module arrows unchanged and correct | pass -- DERIVED, not trusted. 16 rows across five modules, matching the recorded block exactly. The 16th (`StratBridge PrivateDependencyModuleNames StratRules`) confirmed guarded: `bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` at `StratBridge.Build.cs:133`, `if (!bCompileVendoredRulesHere)` at 135, the `.Add("StratRules")` at 137. Field census printed exactly TEN names, the expected set, including the `BRIDGE=` string-literal false positive from `PrivateDefinitions.Add` at line 134. No `.Build.cs` is in the diff |
| 6 | New modules registered | pass, vacuously -- no new module, no new `Source/` directory. `IMPLEMENT_MODULE` exists in exactly three files (`StratBridge.cpp`, `StratPlay.cpp`, `StratUI.cpp`), all registered; `StratRules` carries none and is correctly absent from the `.uproject` |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass -- every hit in a changed file is under a `Tests/` directory (`StratShippedSoundBankParity.cpp:129-152`, `StratSoundCueClauses.cpp:117-121`), the standing fixture exception, each carrying its own justifying block at `:63` and `:113` respectively |
| 9 | No widget-side arithmetic | pass -- no `StratUI` widget file in the diff |
| 10 | Lane discipline | pass -- engineer in `Source/` outside `Tests/` plus `engine.md`; test author in both `Tests/` files plus `tests.md`; steward in `strat_banner_sweep.py` plus `data.md`; coordinator in `global.md`. The one cross-lane item -- `tests.md`'s stale sizing bullet -- was HANDED OFF and written by the owning lane, which `tests.md` states inline at the correction itself. No file written by a lane that does not own it |
| 11 | Prose blocks present and honest | pass -- both re-gate findings closed at the false words; see below. Three residual items are Observations and I would not hold a commit for any of them |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` reports eleven unstaged-modified entries and two untracked (the two prior gate reports), zero staged; `git diff --cached --stat` is empty |

## Findings

None.

## Item 1 -- both re-gate findings, read adversarially

### Re-gate Finding 1 (two contradictory `[STAMPED` censuses inside the instrument) -- CLOSED.

Both comments now carry NO cardinality. `strat_banner_sweep.py:227-238` reads *"MEASURED SAFE,
NOT ASSUMED, BUT REPRODUCE RATHER THAN TRUST A CARDINALITY HERE"*, states the mechanism (the set
grows with every edit to the files being counted, including by the entry doing the counting),
attributes the prior contradictory figures to the re-gate and to `data.md` as history, and names
a command. `strat_banner_sweep.py:276-282` reads *"DO NOT STATE A CARDINALITY HERE"* and does the
same. Neither prints an integer about the record.

**The durable claim that replaced the numbers is TRUE, and I measured it rather than accepting
the lane's word.** My instrument, inlined: a Python pass over every file matching
`Tools/architect/state/*.md`, counting occurrences of the marker opener (an open bracket then the
word STAMPED) and separately counting how many of those match the metasyntactic ellipsis form --
the marker word, optional whitespace, three dots, optional whitespace, a closing bracket -- tested
against the 30 characters starting at each hit, tallied per file. Against the tree being gated:

    content.md     total   20   ellipsis  0
    data.md        total   24   ellipsis 10
    decisions.md   total    2   ellipsis  0
    engine.md      total   11   ellipsis  0
    global.md      total  108   ellipsis  0
    tests.md       total   32   ellipsis  0
    TOTAL 197   ELLIPSIS 10   NON-ELLIPSIS 187

Exactly one file of the six carries the ellipsis form and it is `data.md`; the other five carry
zero. I then read all ten by eye -- `data.md` lines 26, 29, 36, 58, 72, 154, 388, 400, 695 and
699 -- and every one is prose quoting the marker's SHAPE, in sentences such as *"standing near a
GENUINE ... marker that governed a DIFFERENT, unrelated"* and *"no ... marker sits within"*.
**Not one is a real stamp anyone relied on.** That is the property the tightening's safety rests
on and it holds.

The figures moved again since the re-gate (9 to 10, 190 to 197) purely because the correction
round edited the record -- which is the mechanism both comments now name, and is precisely why
neither should print a number. The fix is of the right shape, not merely of the right value.

**The lane's null on other stale cardinalities in the script: independently confirmed.** I ran a
different filter from the lane's -- every comment line in `strat_banner_sweep.py` containing a
1-to-4-digit integer, then narrowed to those also naming "occurr", "census", "marker", "record",
"state/*", "of N total" or "exactly N". Twenty hits, every one either an algorithm constant
(`_STAMP_WINDOW` 220, `_PROVENANCE_CITATION_WINDOW` 400, the ~95-column wrap width), a date, or a
reference to a named gate report. **No surviving cardinality of a record-derived growing set.**

### Re-gate Finding 2 (a third site of the `+ 1` description, in `StratSoundCues.h`) -- CLOSED.

`Source/StratUI/StratSoundCues.h:454-473` now reads *"That wording was false OF THE FILE AS IT
STOOD THAT MORNING -- the `.cpp` SIZED its one-per-kind `bool[]` with a `+ 1` over `MatchEnded`,
which was the enum's last member at the time."* -- past tense, at the words -- followed by
*"CORRECTED 2026-09-06 AT THE WORDS BEFORE THIS STAMP, over base commit `f7da9ca`"*. The
misdirecting pointer the re-gate named is repaired in the same breath: *"the ONE exemption that
survives it -- which is now the `- 1` inside `UE_ARRAY_COUNT` and is NOT the `+ 1` this sentence
names"*. The correction announces itself from the false sentence, names its base commit, and keeps
the historical work the sentence was doing.

**And the transferable fact it records is the right one.** *"The sweep that found and fixed the
other two grepped the `.cpp`; it was bounded by the FILE it searched, not by the claim it
searched for."* That is the generalisable lesson, and the round that followed acted on it: the
`StratSoundDirector.cpp` cross-file citation was **widened to the exemption's CATEGORY** (*"a
compile-time array bound over constants"*) rather than re-pointed at the `- 1`, on the stated
ground that naming one operator is what made that line stale twice in one day. I agree with that
call. `StratTransientReceipts.cpp:5` already states its own exemption as a category -- *"a copy,
a comparison, or an index bound"* -- so the widened form matches the precedent it borrows from
and cannot go stale when the operator next moves.

### No correction in this round introduced a new claim of the same class.

I checked the newly ADDED lines of `data.md` for fresh live cardinalities: every integer in the
new entry is either quoted as a superseded figure with its source named (the gate's 174, the
re-gate's 186/8, the script's old 4 and 169-of-174) or struck through. None is asserted live.

## Item 2 -- the three premises, re-measured here

**`UE_ARRAY_COUNT`.** In this box's engine, `Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h`,
the macro at line 222 expands to `(sizeof(UEArrayCountHelper(array)) - 1)` -- **one `sizeof` and a
subtraction**, on both branches. The `/` and `+ 1` at line 215 sit inside a return TYPE on the
`__clang__` branch this MSVC-only box does not take. The shipped exemption block names the
operator correctly.

**The `[STAMPED` census.** Measured above: 197 total, 10 ellipsis-form, all ten in `data.md`, all
ten metasyntactic.

**Is `bool[7]` still true? YES, and I checked it the way that could have falsified it.**
`EStratSoundCue` declares seven cues then `Count UMETA(Hidden)`. I scanned the enum body for any
`=` outside `UMETA(DisplayName = ...)`: **zero explicit enumerator values.** So `ButtonClick == 0`
through `MatchEnded == 6`, `Count == 7`, and `bool bEmitted[static_cast<int32>(EStratSoundCue::Count)]`
is a `bool[7]`. `StratSoundCues.cpp:112`'s *"THE ONE-PER-KIND COLLAPSE IS A `bool[7]`"* is true of
the tree, and so are `StratDamagedUnitsClauses.cpp:34` and `:172`, `engine.md:75` and
`global.md:137`, which say the same thing. **The engineer's decision to leave all of them alone
and record a debt instead is correct**, and it is the decision that stops this loop: a correction
pass that rewrites true prose manufactures the next round's diff.

## Item 3 -- MY OWN sweep for further sites, by a method the three lanes did not use

The three lanes swept by WORDING, by SUBJECT within a named 18-file set, and by CLAIM-CATEGORY.
**My method differed in its FRAME: subject-enumeration over the ENTIRE TRACKED TREE**, not over a
file set anyone had chosen. I iterated `git ls-files` -- every tracked text file in the
repository, including `CLAUDE.md`, `.agents/`, `Config/`, all of `Tools/architect/` including the
GDD snapshot, and all of `Source/` -- printing every line matching `bEmitted`, `one-per-kind` or
`MatchEnded`, then read all ~110 hits site by site. The lanes' sweeps were bounded by a file list
derived from the claim; mine was bounded only by the repository, which is the one boundary the
header's own retraction identifies as the failure mode (*"bounded by the FILE it searched, not by
the claim it searched for"*). Gate reports were excluded as immutable history.

**Result: no further stale site.** Every hit outside the corrected set is one of: current code; an
explicitly quoted `RETRACTED>` block; a stamped historical annotation naming its own base commit;
a `bool[7]` statement that is true (see item 2); or an unrelated `MatchEnded` cue reference. **One
site is worth naming and it is not a finding** -- `global.md:230`, discussed under Observations.

Five independent sweeps have now run over this claim under four different framings, and the two
most recent -- the engineer's cross-directory pass and mine over the whole tree -- returned one
annotated site between them. **I read that as meaningful evidence the set is closed**, and I say
so explicitly because a null from an unstated method would be worth nothing.

## Item 4 -- prose-only, verified independently, with the limit stated first

**THE LIMIT.** The tree as it stood between the code pass and each correction round exists in no
commit and no artifact in this checkout. *"The correction round moved no executable byte"* is
therefore **not falsifiable from a checkout by anyone**, including me, and I did not verify it.
`data.md`'s *"VERIFIED COMMENT-ONLY: both edits changed only lines beginning with the comment
character"* is a claim about an intermediate state and rests on the steward's honesty; a reader is
entitled to know which claims in this pass rest on that and which are measured. What I verified is
the strictly weaker and still decisive claim: **the executable content of the tree NOW is exactly
the pass the first gate certified, and nothing executable has moved across either correction
round.**

**My instrument, inlined.** A character-scanner over C++ source that skips line comments
(honouring line-continuations) and block comments, preserves string and character literals so a
comment opener inside one survives, normalises CRLF, drops blank lines, and refuses on an empty
result rather than reporting a null as a clean compare. **Five control arms, because two
instruments in this pass have already printed a blind green:**

| Arm | What it does | Result |
|---|---|---|
| 1 | a comments-only input | empty -> refuse; it will not report a null as identical |
| 2 | the real `StratSoundCues.cpp` | 123 code lines -- the instrument speaks |
| 3 | a comment mutated to carry code-shaped tokens (`bool bEmitted[9];` inside a line comment) | invisible, correct |
| 4 | `bEmitted`'s bound mutated to `Count) + 9` | seen -- a code change is visible |
| 5 | a string literal containing a double slash | survives intact |

Run base-to-worktree over all six C++ files:

    Source/StratPlay/StratSoundDirector.cpp                  0 executable line-diffs
    Source/StratPlay/StratSoundBank.cpp                      0 executable line-diffs
    Source/StratUI/StratSoundCues.cpp                        3  (the bound, and the check)
    Source/StratUI/StratSoundCues.h                         11  (Count UMETA(Hidden), the two
                                                                static_asserts, and the comma
                                                                added to MatchEnded)
    Source/StratPlay/Tests/StratShippedSoundBankParity.cpp  53  (the value bound, CueSetControlOrFail)
    Source/StratUI/Tests/StratSoundCueClauses.cpp          185  (rename, CollectCueBoundOffenders,
                                                                the controls)

**`StratSoundDirector.cpp` comes back with ZERO executable delta against `f7da9ca`** -- it entered
the diff purely as a prose correction, and that is the one result here needing no reference to any
prior report. `StratSoundBank.cpp` is likewise zero, confirming its retraction is entirely prose.
The `.cpp` and `.h` deltas are exactly the five constructs the first gate quoted and nothing else.

**`strat_banner_sweep.py`, comment-only: what I could and could not verify.** I built a Python
token-stripper (dropping comments and statement-position docstrings) with four arms of its own --
refuse on empty; instrument speaks (9864 base tokens, 10845 current); a comment mutation
invisible; a mutation of the shipped marker regex visible. Against base `f7da9ca` the script shows
52 executable token hunks, which is the STEWARD'S WHOLE PASS (the new marker tuple,
`sentence_scope`, the all-`Source` mtime walk) and was certified by the two prior gates. It cannot
isolate the correction round. **What can be checked and is:** all three marker tuples still carry
the identical shipped lookahead the re-gate quoted, and the script's BEHAVIOUR is verified by
execution in item 5 -- which is stronger evidence than a byte comparison anyway.

## Item 5 -- the pass-specific checks, re-run

**The sentinel and its asserts.** `StratSoundCues.cpp:199` is
`bool bEmitted[static_cast<int32>(EStratSoundCue::Count)] = {};`; the `+ 1` is gone from the file.
`check(Slot >= 0 && Slot < UE_ARRAY_COUNT(bEmitted));` sits at line 213, one statement before
`if (bEmitted[Slot])` at 215 and the write at 219 -- which is where it has to be. `static_assert`
(1) at `StratSoundCues.h:185` pins `ButtonClick == 0`; (2) at 198 pins `MatchEnded + 1 == Count`.
Neither can see a declaration AFTER `Count`, which is exactly what the header says and no more.

**The clauses.** `NoSoundCueIsDeclaredAfterTheCountSentinel` takes its bound from `Count` with a
strict comparison and walks the reflected `UEnum`; `CollectCueBoundOffenders` is extracted and run
over a synthetic healthy enum and a synthetic mutant on every suite run, with no production edit.
The three parity clauses route through `CueSetControlOrFail`, which asserts the walk yielded
exactly `Count` cues -- the only form that can see the walk quietly LOSING one.

**The sweep, live and unpiped:** `SWEEP CLEAN -- no self-contradiction found`, exit 0, reading
`automation report: 445 entries, all Success (2026.09.06-23.39.35)`, macro census agreeing at 445,
`suite claims found: 65 (1 live, 64 stamped)`. `--self-test`: `SELF-TEST: ALL FIXTURES CORRECT`,
exit 0. The cited `reportCreatedOn` in `global.md:26` matches the report the sweep opened.

**Both fixes re-derived on a disposable copy** (`cp -rp` throughout; a `cp -r` without `-p`
produces a REPORT IDENTITY finding on its own):

| Arm | Mutation | Result |
|---|---|---|
| 0 | faithful copy, no mutation | `SWEEP CLEAN`, exit 0 |
| A | touch `Source/StratUI/StratSoundCues.cpp` -- production, no test macro | `REPORT IDENTITY`, "predates **a production source file** modified ...", `SWEEP FAILED` |
| B | touch `Source/StratUI/Tests/StratSoundCueClauses.cpp` -- test-defining | `REPORT IDENTITY`, "predates **a test-defining source file** ...", `SWEEP FAILED` |
| A/B restored from the live tree | -- | `SWEEP CLEAN` |
| C | `global.md:26`'s citation replaced with `reportCreatedOn 2026.09.05-11.11.11` | `REPORT PROVENANCE` at `global.md:26`, naming both the cited run and the one actually opened; `SWEEP FAILED` |
| D | the same wrong citation, plus the metasyntactic ellipsis form written as prose in the next sentence | **still FAILS** -- the literal quotation no longer exempts |
| E | the same wrong citation, plus a genuine dated stamp with a real date in the next sentence | `SWEEP CLEAN`, exit 0 -- **a real stamp one sentence away still exempts** |

Arms A and B name the file class correctly in both directions. **Arm E is the one that matters and
it holds:** the tightening refuses the metasyntactic quotation and nothing else, which the census
in item 1 confirms a second, independent way -- all ten ellipsis-form occurrences in the record are
metasyntactic prose and none is a marker anyone relied on. One limitation of my copy, stated rather
than left to be discovered: the disposable tree has no `.git`, so the sweep printed *"named-tree
citations not checked this run"* on every arm. The subject under test -- the report citation --
was checked on all five.

## Item 6 -- record honesty and ownership

Nobody wrote outside their own file. `engine.md` is the engineer's and carries only engine-lane
content; `tests.md` the test author's; `data.md` the steward's; `global.md` the coordinator's.
The one cross-lane item is handled correctly: the stale sizing bullet in `tests.md` was found by
`strat-gameplay-engineer` and **handed off rather than written**, and `tests.md` says so inline at
the correction -- *"Handed off by `strat-gameplay-engineer` during the `Count` sentinel re-gate;
this file was outside that agent's sweep"*. No transcription clause and no in-session clause was
invoked, and none was needed.

**Only `global.md` states a suite count.** It carries `445/445` three times; a search for a
four-four-x pair or an "N entries" phrase across `engine.md`, `tests.md`, `content.md` and
`decisions.md` returns nothing live -- the two numeric hits outside `global.md` (`data.md:542`,
`decisions.md:70`) are historical figures inside stamped entries, which the sweep's own RECORD
OWNERSHIP check accepts and which it is the mechanical authority for. It returns CLEAN.

**`tests.md`'s two corrections, read adversarially, are the strongest prose in this pass.** The
parent bullet's retraction is at the HEAD of the paragraph and says so in terms (*"READ THIS
BEFORE THE PARAGRAPH BELOW"*). The sub-bullet stamp below it was found to be *worse* than the
re-gate reported: it had asserted the parent was *"all still correct ... only the bound expression
and the clause's name moved"* when the array itself had moved -- an affirmation of truth sitting
below the one sentence that had stopped being true. It is struck through and corrected at its own
head, and the entry names the shape it committed: *"the exact shape this project's 'a correction
must announce itself from the false sentence' rule exists against, committed by a correction."*
The `OPEN, ROUTED TO` debt is discharged with its prediction marked **half wrong** rather than
vindicated: the sentinel landed, but a cue declared after `Count` is still not a compile error,
which is why the clause was renamed rather than deleted. I verified that residue is real -- the two
`static_assert`s cannot see it, and C++17 cannot enumerate an enum.

## Item 7 -- ready to commit? YES.

Nothing in this tree needs another round. The C++ design was sound at the first gate and has not
moved since; both re-gate findings are closed at the false words with their base commits named;
the instrument's two censuses are fixed in the shape that cannot recur rather than with a fourth
number; and my own whole-tree sweep, run under a framing none of the lanes used, found no further
stale site. The three Observations below are improvable sentences, not defective ones, and I would
not hold a commit for any of them.

**On the pattern the brief asked me to weigh explicitly.** The convergence is real and this round
is where it lands. Round 1 produced four findings, round 2 produced two, this round produces none.
The searches did not merely narrow -- they changed FRAME each time (file, then wording, then
subject, then claim-category, now whole-tree), and the last two frames returned one annotated site
between them. The remaining risk of a loop is now on the CORRECTION side, not the defect side: the
two things I could still write up are (a) one word in a comment about a confirmatory grep and (b) a
tense marker sitting three lines late inside a historical paragraph. Correcting either would
generate a new paragraph describing the correction, in files that have already absorbed four such
paragraphs today, and would move no reader closer to the truth. **The lane that declined to rewrite
`bool[7]` because it was true made exactly the right call, and the same judgment applies to what is
left.** Ship it.

## What remains open

- **The `bool[7]` debt**, recorded by the engineer rather than fixed: a hardcoded 7 survives in
  prose in five places, in the one pass whose subject was deleting a hardcoded bound. True today;
  it goes stale the day an eighth cue lands, and nothing pins it. Correctly recorded as a debt.
- **`/we4062` is still not promoted module-wide**, so `SoundFor`'s switch completeness rests on a
  clause rather than the compiler. Correctly declined here -- a build-configuration change with its
  own blast radius -- and correctly recorded as needing a ruling nobody has made.
- **The decisive production mutant** -- a cue declared after `Count` -- remains unrun by the test
  lane, by lane rule. The synthetic CONTROL 4 is the standing substitute and is honest about being
  one.
- **A fresh checkout of this repository will trip REPORT IDENTITY until the suite is re-run.**
  Correct under the fail-safe reading; worth knowing before it surprises someone.

## Observations

Non-gating. I would not hold a commit for any of these.

- **`strat_banner_sweep.py:280-282` says a command "lists every non-ellipsis hit" and it lists 25
  of 187.** The named pipeline greps for the marker opener followed by a run of non-bracket
  characters and a closing bracket, then filters out the three-dot form. Because that pattern
  requires a closing bracket on the SAME line, it cannot see any marker whose bracketed text wraps
  -- and 98 of `global.md`'s 108 marker occurrences do exactly that. Measured: the pipeline returns
  25 rows where my census counts 187 non-ellipsis occurrences. **Why this is an Observation and not
  a finding, stated so the reader can disagree with me:** the classification's safety rests
  entirely on the ELLIPSIS arm, and that one is complete -- every ellipsis-form occurrence is
  single-line by construction, and the sibling command at line 237 listed all ten (plus, harmlessly,
  `data.md:675`'s genuine dated marker that also contains three dots, an over-list in the safe
  direction). The under-listing is of the set the change does not touch, so no reader running it can
  reach a wrong conclusion about whether the tightening costs a real marker. The word to fix is
  "every", next time that comment is touched; it is not worth a round of its own.
- **The same comment block's classification qualifier over-specifies.** `strat_banner_sweep.py:233-235`
  says every ellipsis-form occurrence is this steward's prose *"while describing this exact fix"*.
  Four of the ten (`data.md:388`, `400`, `695`, `699`) describe an adjacency window, a carried-debt
  paragraph and fixture design instead. The load-bearing half -- **never a real stamp anyone relied
  on** -- is true of all ten and I verified it by eye. The qualifier is narrower than the tree, in
  the harmless direction.
- **`Tools/architect/state/global.md:230-231` is the one site my whole-tree sweep surfaced.** It
  states in the present tense *"`bEmitted`'s bound is `MatchEnded + 1`, so it is correct ONLY while
  `MatchEnded` is the last enumerator"*. Not raised as a finding: the correction is inside the SAME
  sentence-complex, three lines on -- *"by the sentinel pass above, which also removed the `+ 1`
  this sentence describes ... this sentence is kept for the reasoning, not as a live citation"* --
  and it was added in this pass (`git diff` on `global.md` shows those two lines as additions). The
  paragraph's own opening frames it as history of a prior pass. It is distinguishable from the
  `StratSoundCues.h` site that WAS blocked: that one handed the reader a pointer to an exemption
  block that no longer matched, whereas this one tells the reader the operator is gone. Strictly,
  the marker should sit at the false words rather than after them; worth one sentence the next time
  that entry is touched.
- **The backslash-eating heredoc trap reproduced twice more during this gate**, on two different
  instruments -- a Python token-stripper and a C comment-stripper, both written to disk through a
  quoted heredoc, both arriving with their escape sequences collapsed. One died on `SyntaxError`;
  the other **printed a wrong control result** (`code mutation visible: False`) because its mutation
  needle had been mangled and matched nothing. **Both were caught only by a control arm that
  asserted its own setup.** That is now the fourth and fifth independent reproduction on this box
  within two days. The durable workaround, used for both final instruments and for this report:
  never write a backslash literal through a heredoc -- build it with `chr(92)`, or write a
  placeholder and substitute afterwards.
- **One artifact of my own instrument, named so nobody re-derives it as a finding.** My comment
  stripper reported a one-line delta in `StratSoundCueClauses.cpp` on a line containing a section
  sign. `git diff` shows that line unchanged and the file's bytes are correct UTF-8; the difference
  was my reader's decoding of `git show` output, not a tree change. No character in the tree is
  corrupted.
- **`Tools/architect/gate_reports/` now holds eleven reports dated 2026-09-06**, this one included;
  three of the eleven are this one pass. A future search for "the 2026-09-06 gate" will not resolve
  to one document.

VERDICT: PASS
