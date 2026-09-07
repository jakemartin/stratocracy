# Gate: `Count` sentinel + sweep fixes -- RE-GATE of the four-finding correction round -- strat-integration-reviewer

## Scope

Uncommitted working tree over base commit `f7da9ca`, branch `master`, nothing staged. The same
ten modified files as the prior gate
(`Tools/architect/gate_reports/2026-09-06-sound-cue-count-sentinel-gate.md`), plus that report
itself as the only untracked path:

    Source/StratPlay/StratSoundBank.cpp
    Source/StratPlay/Tests/StratShippedSoundBankParity.cpp
    Source/StratUI/StratSoundCues.cpp
    Source/StratUI/StratSoundCues.h
    Source/StratUI/Tests/StratSoundCueClauses.cpp
    Tools/architect/state/data.md
    Tools/architect/state/engine.md
    Tools/architect/state/global.md
    Tools/architect/state/tests.md
    Tools/architect/strat_banner_sweep.py

No `Content/` change, no asset change, no new module, no `.Build.cs` change. I did not build and
did not run the suite; `strat-test-author` reports those. I re-measured both of the prior gate's
premises against the engine headers rather than accepting them, and re-ran every pass-specific
arm on disposable copies.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass -- `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass -- a grep for `strat::` followed by a lowercase-initial identifier and an open paren, across `Source/` minus the two permitted directories, returned zero hits |
| 3 | No vendored header in front of UHT | pass -- every `.h` outside `StratRules` carrying `UCLASS(`/`USTRUCT(`/`UENUM(` was scanned for an `#include` of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`; zero hits. `StratSoundCues.h`, the one UHT header this pass edits, includes only `CoreMinimal.h` and its own `.generated.h` |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass -- the only `TUniquePtr<FStratBridge>` in the tree is the historical note at `Source/StratUI/StratViewModel.h:47` |
| 5 | Module arrows unchanged and correct | pass -- derived, not trusted: 16 rows across five modules, matching the recorded block; the 16th (`StratBridge PrivateDependencyModuleNames StratRules`) confirmed guarded by `if (!bCompileVendoredRulesHere)` at `StratBridge.Build.cs:135-137`, with `bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic` at line 133. The field census printed 10 names, the expected set, including the `BRIDGE=` string-literal false positive from `PrivateDefinitions.Add` at line 134. No `.Build.cs` is in the diff |
| 6 | New modules registered | pass, vacuously -- no new module, no new `Source/` directory |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass -- every hit in a changed file is under a `Tests/` directory (`StratShippedSoundBankParity.cpp:129-152`, `StratSoundCueClauses.cpp:117-121`), the standing fixture exception, and each carries its own justifying block |
| 9 | No widget-side arithmetic | pass -- no `StratUI` widget file in the diff |
| 10 | Lane discipline | pass -- engineer in `Source/` outside `Tests/` plus `engine.md`; test author in both `Tests/` files plus `tests.md`; steward in `strat_banner_sweep.py` plus `data.md`; coordinator in `global.md`. No file written by a lane that does not own it |
| 11 | Prose blocks present and honest | **BLOCK** -- Findings 1 and 2 |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` reports ten unstaged-modified entries, one untracked (the prior gate report), zero staged; `git diff --cached --stat` is empty |

## Findings

### Finding 1 -- BLOCK. The prior gate's Finding 2 is one-third closed. Two of the three censuses it quoted are untouched and still false, and they still contradict each other.

The prior gate's Finding 2 is headed *"Three stated `\[STAMPED` censuses"* and quotes three sites: two
comments in `Tools/architect/strat_banner_sweep.py` and one entry in
`Tools/architect/state/data.md`. **Only `data.md` was corrected.** The two in the instrument are
byte-identical to the ones that gate quoted.

`Tools/architect/strat_banner_sweep.py:227-229`:

> MEASURED SAFE, NOT ASSUMED: every `\[STAMPED` occurrence across the live
> `Tools/architect/state/*.md` that matches the ellipsis form (**4**, all in `data.md`, all this
> steward's own prose ...) is confirmed metasyntactic by direct inspection

`Tools/architect/strat_banner_sweep.py:267-269`:

> every one of this record's **169** non-ellipsis `\[STAMPED` occurrences (of **174** total,
> measured against the live `Tools/architect/state/*.md` 2026-09-06) is followed by an actual date
> or word

The instrument, inlined so a later reader can re-run it rather than take my word: a Python pass
over every file matching `Tools/architect/state/*.md`, counting occurrences of the regex
`\[STAMPED`, and separately counting how many of those match `\[STAMPED\s*\.\.\.\s*\]` against the 30
characters starting at each hit, tallied by file. Against the tree being gated, 2026-09-06:

    content.md    total  20   ellipsis 0
    data.md       total  20   ellipsis 9
    decisions.md  total   2   ellipsis 0
    engine.md     total  10   ellipsis 0
    global.md     total 108   ellipsis 0
    tests.md      total  30   ellipsis 0
    TOTAL 190   ELLIPSIS 9   NON-ELLIPSIS 181

So the shipped comments state 4 where the tree says 9, and 169-of-174 where the tree says
181-of-190. The internal contradiction the prior gate named survives untouched as well: 174 minus
169 is 5, which is neither comment's 4.

`data.md`'s own correction is exemplary and I am not asking for it to be redone -- it struck both
cardinalities, attributed the prior gate's 186/8 to that report as a historical measurement, and
replaced the number with the durable classification plus a reproducible `grep`. **I re-measured
that durable claim and it holds exactly**: exactly one file of the six carries the ellipsis form,
and it is `data.md`; the other five carry zero. That is the fact the tightening's safety rests on,
and it is true.

What is not closed is the finding. The instrument still asserts two live measurements of the live
record, in the comment that exists to justify a regex change as *"MEASURED SAFE, NOT ASSUMED"*, and
both are false against the record that sentence names. This is the same shape `data.md` itself
diagnoses two paragraphs later -- a cardinality of a growing set, stated where the set will move --
committed in a second file the correction did not reach. It is also a recurrence: this record
already carries a 2026-09-04 entry about *"stale claims in `strat_banner_sweep.py`'s REPORT
PROVENANCE docstring/fixtures, all found by `strat-integration-reviewer`"*.

**I would hold a commit for this.** Not because 9 differs from 4, but because a comment whose whole
rhetorical work is *"I measured this"* is the one place a wrong number is load-bearing: the next
maintainer deciding whether to widen or narrow that lookahead will read it as evidence, and it is
not. The fix is the one `data.md` already worked out for itself -- state the shape and the file, or
scope the count to a named commit -- applied to the two comments the correction round missed.
Owner: **`strat-data-steward`**.

### Finding 2 -- BLOCK. A third site of the `+ 1` description survives, in the header this pass edits, and this pass is what made it false.

`Source/StratUI/StratSoundCues.h:454-458`:

> (This sentence quoted that claim in its earlier wording, "THERE IS NOT ONE ARITHMETIC OPERATION
> IN IT", until 2026-09-06. That wording was false -- **the `.cpp` sizes a `bool[]` with a `+ 1`
> over an enum's last member** -- and the retraction, together with the one exemption that survives
> it, is stated in that file's own opening block.)

Measured: the `.cpp` no longer does. `Source/StratUI/StratSoundCues.cpp:191` now reads
`bool bEmitted[static_cast<int32>(EStratSoundCue::Count)] = {};`, and a scan of that file with
comments and string literals stripped finds no `+ 1` anywhere. The claim was TRUE at base `f7da9ca`
and was falsified by the code change in the first half of this same uncommitted pass.

**Why this is not a nitpick, and why I would hold a commit for it.** It is the third site of one
false description, and the engineer's own retraction block in the `.cpp` states the standard:

> It is named here rather than fixed silently because it was the SECOND site of one false
> description, and a reader who found only the first would have no reason to think there had been
> another.

The sweep for other sites stopped at the `.cpp` and did not cross into the `.h` -- the same file the
pass rewrote 120 lines of. And the sentence does not merely go stale in place: it hands the reader a
pointer, *"the one exemption that survives it, is stated in that file's own opening block"*, and a
reader who follows it now finds an exemption written about a **subtraction** in `UE_ARRAY_COUNT`,
with no `+ 1` anywhere -- which is precisely the confusion the prior gate's Finding 1 was raised to
prevent.

**The counter-reading, stated so the owner can weigh it.** The clause is preceded by "until
2026-09-06", so a charitable reader may scope the present tense to that date. I do not think that
saves it: the tense is present, the subject is the current `.cpp`, and the `.cpp`'s own retraction
block explicitly forecloses exactly this reading for its own copy of the sentence -- *"A reader
grepping this file for `+ 1` on the strength of that sentence now finds nothing and would reasonably
conclude the block had gone stale unnoticed."*

It is one sentence. Owner: **`strat-gameplay-engineer`**.

## The prior gate's Findings 1, 3 and 4: CLOSED, and the corrections read adversarially

### Prior Finding 1 (`UE_ARRAY_COUNT` misnamed) -- CLOSED, and the premise re-measured here.

Measured against this box's engine, in
`C:/Program Files/Epic Games/UE_5.8/Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h`:

    215:  auto UEArrayCountHelper(T& t) -> char(&)[sizeof(t) / sizeof(t[0]) + 1];   // __clang__ branch
    218:  char (&UEArrayCountHelper(const T (&)[N]))[N + 1];                        // else branch
    222:  #define UE_ARRAY_COUNT( array ) (sizeof(UEArrayCountHelper(array)) - 1)

One `sizeof` and a subtraction, on both branches. The corrected block at
`StratSoundCues.cpp:12-23` quotes line 222 verbatim and names the operator correctly, and every
subsidiary claim in it survives inspection: the helper is declared and never defined, so it is only
ever an unevaluated operand; the left operand is a `sizeof` and the right is the literal `1`; and
the `/` and the `+ 1` a reader will find at line 215 are inside a return TYPE on a branch this
MSVC-only box does not take, and they do pair with the macro's `- 1` as the block says.

**The added forbidden-vs-exempt paragraph, read adversarially, is correct.** It claims
`StratTransientReceipts.cpp` forbids a second subtraction and that the thing forbidden is an HP
delta. Verified at `Source/StratUI/StratTransientReceipts.cpp:4-7` -- *"THERE IS ONE SUBTRACTION IN
THIS FILE and it is the `KillFame` amount ... the header's argument for why the one subtraction is
allowed does not extend to a second one"* -- and at lines 12-13, *"an HP delta IS a second
subtraction"*. The distinguishing property the new paragraph offers, runtime operands naming a board
quantity versus none, is the property those blocks actually turn on and not a newly invented one.

**And the "one arithmetic operator" claim is true of the file as it now stands.** I stripped
comments and string literals and scanned the remaining lines of `StratSoundCues.cpp` for the five
arithmetic operators: eight hits, every one a pointer declaration or dereference
(`TMap<int32, const FStratUnitSoundMark*>`, `**Found`) at lines 243, 257-258, 269, 343, 356-357 and
366. No arithmetic operator remains outside the macro.

**The claim that the exemption needed no rewording is right, and I checked it rather than took it.**
`StratTransientReceipts.cpp:5` states the exemption as *"a copy, a comparison, or an index bound"* --
a category, not an operator -- so it admits a `sizeof` minus one exactly as it admitted a quotient.

### Prior Finding 3 (`global.md`) -- CLOSED, both halves, at the false words.

The "both are open" sentence now carries an inline `[CORRECTED 2026-09-06 AT THE WORDS ABOVE ...]`
naming what it said, why it was true when written, and that both defects are now closed in this
tree; the paragraph following describes what the steward actually built. The LANES line names all
four actors, including `strat-data-steward` for `strat_banner_sweep.py` and `data.md`, and records
parenthetically that the omission was gate-reported rather than silently inserted.

### Prior Finding 4 (`UEnum::HasMetaData`) -- CLOSED, and ground (3) re-measured here.

`StratSoundCues.h:137-166` retracts the recommendation in full, quotes the retracted text, and
records all three refusal grounds. I verified ground (3) directly against the engine rather than via
the report: in
`C:/Program Files/Epic Games/UE_5.8/Engine/Source/Runtime/CoreUObject/Public/UObject/Class.h`,
`COREUOBJECT_API bool HasMetaData( const TCHAR* Key, int32 NameIndex=INDEX_NONE ) const;` sits at
line 3224, inside the `#if WITH_METADATA` block that opens at 3215 and closes at 3255
(`#endif // WITH_METADATA`). The `C2039` the test lane cites is a real property of this engine
version.

`UMETA(Hidden)` is retained and its purpose corrected to the `MinSecondsBetween` key dropdown, with
an explicit "THE MARKER ITSELF IS NOT RETRACTED AND MUST NOT BE REMOVED". The hole-(3) paragraph now
reads "AND BY TWO INSTRUMENTS RATHER THAN ONE", credits
`NoSoundCueIsDeclaredAfterTheCountSentinel` as the stronger of the two, and narrows the surviving
"no automation clause can exercise it" sentence to the `check` line it is true of, with a dated
parenthetical saying so. Nothing here overclaims.

## Item 3 -- the prose-only claim, verified independently, with its limit stated

**The limit first, because it decides what the check is worth.** The intermediate state -- the tree
as it stood between the code pass and the correction round -- exists in no commit and in no artifact
in this checkout. "Not one executable byte moved in the correction round" is therefore **not
falsifiable from a checkout by anyone**, including me. I did not verify that sentence. What I
verified is the weaker and still useful claim: the executable content of the tree now is exactly
what the prior gate described, item by item, so nothing executable moved between that gate and this
one that that report would have caught.

**My instrument, and its four control arms, because the engineer's first one printed a blind green.**
I hit the identical failure: my first stripper was written to disk through a heredoc that ate its
backslashes, the script died on `SyntaxError`, and both sides of every comparison came back empty.
**CONTROL 4 caught it** -- it printed "IDENTICAL (WRONG)" on a file I had deliberately mutated -- and
CONTROL 1 caught it a second way. That is a second, independent measurement of this trap on this box
within one day: the naive route to putting a stripper on disk here loses backslashes, and only a
mutation arm notices.

The shipped instrument: a character-scanner over the file that skips line comments (honouring
line-continuations) and block comments, preserves string and character literals so a comment opener
inside one survives, normalises CRLF, drops blank lines, and **exits 2 rather than printing nothing**
if the result is empty. Its arms, run against `Source/StratUI/StratSoundCues.cpp`:

| Arm | What it does | Result |
|---|---|---|
| 1 | a file of comments only | `REFUSE: empty result`, exit 2 -- it will not report a null as a clean compare |
| 2 | the real file | 123 code lines -- the instrument speaks |
| 3 | one comment line mutated | identical -- a comment change is invisible, as required |
| 4 | `bEmitted`'s bound mutated to `Count)+9` | that one line reported -- a code change is visible |

Run base-to-worktree over the five C++ files, the executable diff is exactly the pass the prior gate
verified and nothing else:

- `StratSoundBank.cpp` -- **zero** executable lines changed across the whole pass, confirming that
  file's change is entirely the retraction it claims to be.
- `StratSoundCues.cpp` -- two lines: the bound moved from `MatchEnded) + 1` to `Count)`, and
  `check(Slot >= 0 && Slot < UE_ARRAY_COUNT(bEmitted));` was added.
- `StratSoundCues.h` -- the `Count UMETA(Hidden)` enumerator and the two `static_assert`s.
- `StratShippedSoundBankParity.cpp` -- `AllCues()` switched from the `_MAX` name rule to the value
  bound, and `CueSetControlOrFail` extracted and called at all three sites.
- `StratSoundCueClauses.cpp` -- the clause rename, the extracted `CollectCueBoundOffenders`, the
  bound changed to `Count` with a strict comparison replacing the inclusive one, CONTROL 2a's
  1-to-2, CONTROL 2d, and the synthetic healthy/mutant CONTROL 4 block.

Every one of those is an item the prior gate names and verified. No executable change is present
that the prior gate did not already see.

## Item 4 -- the pass-specific checks, re-run because the tree moved

**The sentinel and its asserts.** `bEmitted` is sized from `Count`; the `+ 1` is gone;
`check(Slot >= 0 && Slot < UE_ARRAY_COUNT(bEmitted))` sits one statement before `if (bEmitted[Slot])`
and the write below it, which is where it has to be. `static_assert` (1) pins `ButtonClick == 0`;
(2) pins `MatchEnded + 1 == Count`. Between them an explicit value anywhere in the list and an
insertion between the last cue and the sentinel both redden at compile time, and neither can see a
declaration AFTER `Count` -- which is exactly what the header now says, and no more.

**The clauses.** `NoSoundCueIsDeclaredAfterTheCountSentinel` walks the reflected `UEnum` and asserts
every enumerator's value lies below `Count` and at or above zero, which is the residue the asserts
cannot reach. Its CONTROL 4 runs the extracted decision over a synthetic healthy enum (expects no
offenders) and a synthetic mutant carrying `DeclaredAfterCount` (expects exactly
`DeclaredAfterCount=4`), on every suite run, with no production edit. CONTROL 2d is a second,
independent detector of the same mutant through the checked count. The three parity clauses route
through `CueSetControlOrFail`, which upgrades a "more than one" vacuity guard into an equality
against `EStratSoundCue::Count` -- the only form of the three that can see the walk quietly losing a
cue.

**The sweep, live and unpiped**: `SWEEP CLEAN -- no self-contradiction found`, exit 0, reading
`automation report: 445 entries, all Success (2026.09.06-23.07.53)` with the macro census agreeing at
445 and `suite claims found: 65 (1 live, 64 stamped)`. `--self-test` returns
`SELF-TEST: ALL FIXTURES CORRECT`, exit 0, including the four new cases and the regression pins that
assert the pre-fix pattern and the shipped one differ on identical text.

**Both sweep fixes, re-derived on disposable copies** (`cp -rp` throughout so mtimes survive; a
`cp -r` without `-p` produces a REPORT IDENTITY finding on its own):

| Arm | Mutation | Result |
|---|---|---|
| 0 | faithful copy, no mutation | `SWEEP CLEAN`, exit 0 |
| A | touch `Source/StratUI/StratSoundCues.cpp` -- production, carries no test macro | `REPORT IDENTITY`, "predates **a production source file** modified ...", `SWEEP FAILED` |
| B | touch `Source/StratUI/Tests/StratSoundCueClauses.cpp` -- test-defining | `REPORT IDENTITY`, "predates **a test-defining source file** ...", `SWEEP FAILED` |
| C | `global.md:26`'s citation replaced with `reportCreatedOn 2026.09.05-11.11.11` | `REPORT PROVENANCE` at `global.md:26`, naming both the cited run and the one actually opened; `SWEEP FAILED` |
| D | the same wrong citation, plus the metasyntactic ellipsis form of the marker written as prose in the next sentence | **still FAILS** -- the literal quotation no longer exempts |
| E | the same wrong citation, plus a genuine dated `[STAMPED 2026-09-06: superseded by the run below.]` in the next sentence | `SWEEP CLEAN`, exit 0 -- **a real stamp one sentence away still exempts** |

Arm E is the one that matters and it is confirmed twice: once here, and once by the census in
Finding 1, which shows every ellipsis-form occurrence in the record is metasyntactic prose in
`data.md`, so the tightening refuses nothing anyone relied on. Arms A and B confirm 2(b) in both
directions, and the arm-naming in the finding text distinguishes them correctly.

## Item 5 -- record honesty and ownership

Nobody wrote outside their own file. Only `global.md` states a suite count -- a grep for a
four-four-x pair or an "N entries" phrase across `engine.md`, `tests.md`, `data.md`, `content.md`
and `decisions.md` returns nothing, while `global.md` carries `445/445` three times. The sweep's own
RECORD OWNERSHIP check agrees, and it is the mechanical authority for that rule.

`global.md`'s live citation `reportCreatedOn 2026.09.06-23.07.53` matches the report the sweep
opened. Its account of the pass matches the diff on everything I could check: one clause renamed and
three repaired with none added; the four-clause blast radius; the three refusal grounds; and the
crashed mutant recorded as a mutant that killed the process rather than one the clause killed.

## Item 6 -- ready to commit? No. And I agree with the prior gate's 2(b) ruling.

**Not ready.** Two findings above, both of which I would genuinely hold a commit for, both one
sentence to fix, neither in the design of the work. Everything else in this pass -- the sentinel, the
asserts, the runtime `check`, the value bound, the four clauses, both sweep fixes, and the three
closed findings -- is sound, and I re-derived it rather than trusting it.

**On the 2(b) freshness ruling: I agree, and I would not refine it.** The prior gate's four reasons
hold, and the load-bearing one is the direction of failure: a check that over-reports staleness costs
a suite run, while one that under-reports it returns CLEAN over red clauses -- which is the exact
defect this fix closes, and which was avoided last time only because a lane chose to run the suite by
hand. I add one measurement of my own in support: my comment-stripping instrument printed a **blind
green on its first run**, and would have certified a mutated file as unchanged had CONTROL 4 not
existed. That is the second independent demonstration on this box, within one day, that a
comment-stripping instrument fails silently -- a direct argument against promoting one to a standing
input to a verdict. **If it is ever refined, the shape is a content hash and not a comment strip**,
for the reason the prior gate gives: a hash makes no lexical judgement about what a byte means, so it
cannot fail in the quiet direction, and it also removes the real spurious-finding source, mtime churn
from a fresh checkout, a `git restore`, or `autocrlf` rewriting on checkout. A fresh checkout of this
repository will trip REPORT IDENTITY until the suite is re-run; that is correct under the fail-safe
reading and is worth knowing before it surprises someone.

## What remains open once the two findings are fixed

- Nothing in the C++ design. The sentinel work is complete and correctly bounded by its own header's
  statement of what it cannot see.
- The `we4062` promotion is still not enabled module-wide, so `SoundFor`'s switch completeness rests
  on a clause rather than on the compiler. Correctly declined in this pass and correctly recorded as
  needing a ruling nobody has made.
- The decisive production mutant -- a cue declared after `Count` -- remains unrun by the test lane,
  by lane rule, and is recorded as such. The synthetic CONTROL 4 is the standing substitute.

## Observations

Non-gating.

- **`Tools/architect/state/tests.md:216-218`** states in the present tense that
  `StratDecideSoundCues` *"sizes its one-per-kind gate as
  `bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1]`"*, which is no longer true of
  the tree. Not raised as a finding because a `[STAMPED 2026-09-06, later the same day, over base
  commit f7da9ca]` block sits three lines below inside the same bullet and names the change
  explicitly, including the phrase "the `MatchEnded + 1` array". A reader arriving by citation lands
  close enough to it. Worth a sentence next time that bullet is touched.
- **`Tools/architect/state/engine.md:1725-1727`** carries the identical wording but inside a
  `[STAMPED 2026-09-06: ...]` annotation whose own clause scopes it -- "was FALSE when this entry was
  written". That one reads as history and is fine.
- **`Source/StratUI/StratSoundCues.cpp:16`** cites the engine header with an unclosed backtick and a
  stray trailing colon before the quoted macro. Cosmetic; the citation itself is correct and I
  re-measured it.
- **`Emit`'s `check` compares an `int32` against the unsigned result of `UE_ARRAY_COUNT`.** The
  zero-or-greater conjunct makes it correct. Carried forward from the prior gate; if the `4018`
  warning is ever promoted, this is the line where it lands.
- **`Tools/architect/gate_reports/` now holds ten reports dated 2026-09-06**, this one included. A
  future search for "the 2026-09-06 gate" will not resolve to one document.

VERDICT: BLOCK
