# Gate: `EStratSoundCue::Count` sentinel + banner-sweep instrument fixes — strat-integration-reviewer

## Scope

Uncommitted working tree over base commit `f7da9ca`, branch `master`, nothing staged.
Ten modified files, no additions, no deletions:

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

No `Content/` change, no asset change, no new file, no new module.
I did not build and did not run the suite; `strat-test-author` reports those.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — a grep for `strat::` followed by a lowercase-initial identifier and an open paren, across `Source/` minus the two permitted directories, returned zero hits |
| 3 | No vendored header in front of UHT | pass — one candidate, `Source/StratBridge/StratBridge.h`, judged and dismissed: the `UCLASS`/`USTRUCT` tokens there are inside comments at lines 119, 262 and 323 that state the header declares none. No `GENERATED_BODY` in the file |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only `TUniquePtr<FStratBridge>` in the tree is the historical note at `Source/StratUI/StratViewModel.h:47` |
| 5 | Module arrows unchanged and correct | pass — derived; 16 rows across five modules, matching the recorded block. The 16th row (`StratBridge PrivateDependencyModuleNames StratRules`) is confirmed guarded by `if (!bCompileVendoredRulesHere)` at `StratBridge.Build.cs:135-137`. The field census printed 10 names, the expected set, including the `BRIDGE=` string-literal false positive. No `.Build.cs` is in the diff |
| 6 | New modules registered | pass, vacuously — no new module. `IMPLEMENT_MODULE` exists in exactly four modules and all four are in the `.uproject`; `StratRules` correctly carries none and is correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in C++ | pass — every hit is under a `Tests/` directory (the standing fixture exception), and none is in a changed file |
| 9 | No widget-side arithmetic | pass — no `StratUI` widget file in the diff |
| 10 | Lane discipline | pass — engineer in `Source/` outside `Tests/` plus `engine.md`; test author in both `Tests/` files plus `tests.md`; steward in `Tools/architect/` plus `data.md`; coordinator in `global.md`. No file written by a lane that does not own it |
| 11 | Prose blocks present and honest | **BLOCK** — Findings 1, 2, 3, 4 |
| 12 | Nothing staged or committed | pass — `git status --porcelain` reports ten unstaged-modified entries and zero staged; `git diff --cached --stat` is empty |

## Findings

### Finding 1 — BLOCK. The new exemption block names an arithmetic operator that is not in the file.

`Source/StratUI/StratSoundCues.cpp:12-16`:

> THE ONE ARITHMETIC OPERATOR IN THIS FILE IS NAMED HERE RATHER THAN LEFT FOR A READER COUNTING
> OPERATORS TO FIND AND READ AS A BROKEN CLAIM. It is the **DIVISION** inside `UE_ARRAY_COUNT`, in
> `StratDecideSoundCues`' `Emit` -- **the macro expands to `sizeof(x) / sizeof(x[0])`** -- and it
> bounds-checks the one-per-kind gate's index. **Both operands are `sizeof`s** the compiler folds,
> neither is a quantity about the board, and **the quotient** reaches no caller and no screen

Measured against the engine this project builds with, UE 5.8, in
`Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h` lines 209-222:

    #ifdef __clang__
        template < typename T UE_REQUIRES(__is_array(T)) >
        auto UEArrayCountHelper(T& t) -> char(&)[sizeof(t) / sizeof(t[0]) + 1];
    #else
        template <typename T, uint32 N>
        char (&UEArrayCountHelper(const T (&)[N]))[N + 1];
    #endif

    // Number of elements in an array.
    #define UE_ARRAY_COUNT( array ) (sizeof(UEArrayCountHelper(array)) - 1)

The macro expands to **one `sizeof` and a SUBTRACTION**, on both compilers. The division exists
only inside the `__clang__` helper's *return type*, which is not the branch this MSVC-only box
takes and is not what the macro expands to in either case. The block is therefore wrong four
ways in five lines: it is not a division, the expansion quoted is not the expansion, there is
one `sizeof` operand and not two, and there is no quotient.

This matters more than a typo would, for two reasons that are the block's own reasons for
existing. First, the operator that is actually there is a **subtraction** — the single operator
category this file's lineage treats as most dangerous (`StratTransientReceipts.cpp`'s "second
subtraction", and the retracted sentence quoted three lines below it, which enumerated "No
subtraction" first). A reader who trusts this block will look for a division, find none, and
read past a `- 1`. Second, this pass exists precisely because the previous exemption named an
operator that had stopped being there; the replacement was false on the day it was written.

**The standing claim itself survives and is still true of the file.** A `sizeof` minus one is a
compile-time constant over types, reaches no caller and no screen, and cannot be wrong about
game state — so the exemption's *reasoning* transfers to the subtraction unchanged, and "THIS
FILE COMPUTES NO MAGNITUDE ABOUT GAME STATE" holds. Only the description is false. The `+ 1` is
genuinely gone: the gate is now
`bool bEmitted[static_cast<int32>(EStratSoundCue::Count)]`, a cast and no operator, as claimed.

**Owner: `strat-gameplay-engineer`.** It is a prose fix in one paragraph.

### Finding 2 — BLOCK. Three stated `[STAMPED` censuses, all false against the tree they name, and two of them contradict each other.

`Tools/architect/strat_banner_sweep.py`, in `_PARAGRAPH_STAMP_MARKERS`' comment:

> every `[STAMPED` occurrence across the live `Tools/architect/state/*.md` that matches the
> ellipsis form (**4**, all in `data.md`, ...)

`Tools/architect/strat_banner_sweep.py`, in `_PROVENANCE_TREE_STAMP_MARKERS`' comment:

> every one of this record's **169** non-ellipsis `[STAMPED` occurrences (of **174** total,
> measured against the live `Tools/architect/state/*.md` 2026-09-06)

`Tools/architect/state/data.md`:

> Of **178** `[STAMPED` occurrences across `Tools/architect/state/*.md` (up from the gate's 174,
> the record having grown since), exactly **4** match the ellipsis form

The instrument I used, inlined so a later reader can re-run it rather than take my word: a
Python pass over every file matching `Tools/architect/state/*.md`, counting occurrences of the
regex `\[STAMPED`, and separately counting how many of those are the metasyntactic ellipsis
form, matched with `\[STAMPED\s*\.\.\.\s*\]` against the 30 characters starting at each hit,
tallied by file. Its output against the tree being gated:

    total: 186   ellipsis: 8   non-ellipsis: 178   {'Tools/architect/state/data.md': 8}

So: 186 total, neither 174 nor 178. Eight ellipsis-form, not four. And 174 minus 169 is 5, which
is neither of the two 4s — the two comments in one file disagree with each other as well as with
the tree.

This is the "a record that measures itself moves its own subject" shape, committed knowingly:
`data.md`'s entry *names* the mechanism ("up from the gate's 174, the record having grown
since") and then repeats it, because writing that entry added four more ellipsis-form
quotations to the very file being counted.

**The qualitative conclusion is unaffected, and I verified it independently.** All eight
ellipsis-form occurrences are in `data.md`; all eight are the steward's own prose quoting the
marker's shape while describing this fix; zero are in `global.md`, `content.md`, `engine.md`,
`tests.md` or `decisions.md`. So "this costs no real marker" is true, and Arm E below shows the
tightening does not touch a genuine stamp. What is false is every stated count.

**Owner: `strat-data-steward`.** The durable fix is not a re-count — the same drift recurs the
next time the entry is edited — but to state the shape and the file rather than the integer, or
to scope the count to a named commit.

### Finding 3 — BLOCK. `global.md`'s new entry calls two defects open that the same tree closes, and its LANES line omits a lane that wrote two of the ten changed files.

`Tools/architect/state/global.md:63-65`:

> This is the SECOND adjacency-or-scope defect found in this instrument in one day; both are
> `strat-data-steward`'s and **both are open**.

Both are closed in this tree. `Tools/architect/state/data.md`'s new entry opens **"DONE,
2026-09-06 -- Two measured defects fixed in `Tools/architect/strat_banner_sweep.py`"**, the fixes
are in `strat_banner_sweep.py` in this diff, and I re-derived both below.

And, in the same entry:

> LANES: `strat-gameplay-engineer` wrote `Source/` outside `Tests/` and `engine.md`;
> `strat-test-author` wrote both `Tests/` files and `tests.md`; the `coordinator` wrote
> `global.md` only. No exception clause was invoked and none was needed.

`strat-data-steward` wrote `Tools/architect/strat_banner_sweep.py` and
`Tools/architect/state/data.md` — two of the ten changed files — and is not in the list. The
lane discipline itself is correct; the *statement* of it is not, and an enumeration of writers
reads as an affirmative claim that there were no others.

Both sentences read as an entry written at the midpoint of the pass and not revisited after the
steward's half landed. **Owner: the `coordinator`.**

### Finding 4 — The header recommends the discriminator its only two callers refused, and names one of the two instruments that cover the residual hole.

`Source/StratUI/StratSoundCues.h`, in `Count`'s own doc comment:

> a reflection walk can ask `UEnum::HasMetaData(TEXT("Hidden"), Index)` and tell a sentinel
> from a cue without a hard-coded name list. ... this one can be skipped by PROPERTY, and
> **callers that want "every real cue" should prefer that.**

There are exactly two such callers in the tree — `StratShippedSoundBankParity.cpp`'s `AllCues()`
and `StratSoundCueClauses.cpp`'s walk — and **both deliberately do not do this**, on three
grounds recorded in the same pass. I verified the third directly: `UEnum::HasMetaData` is
declared at `Engine/Source/Runtime/CoreUObject/Public/UObject/Class.h:3224`, inside the
`#if WITH_METADATA` block that opens at line 3215 and closes at line 3255. The Game-target
`C2039` the test lane cites is a real property of this engine version, not a guess. A header
that tells the next author to prefer a mechanism that does not compile in a target CI builds,
without citing the counter-argument sitting in both of its callers, will cost that measurement
a second time.

Second, smaller half, in the same header's hole-(3) paragraph:

> IT IS CLOSED AT RUNTIME INSTEAD, by a `check` on the index in `StratDecideSoundCues`' `Emit`.
> ... No automation clause can exercise it without editing this enum

`StratSoundCueClauses.cpp`'s banner states the opposite inventory, correctly: *"Two instruments
see that case: a `check` in `Emit` ... and this clause, which sees it on any suite run whether
the cue ever fires or not."* The clause detects an after-`Count` enumerator on the real enum
with no edit at all, and its CONTROL 4 exercises the decision logic over a synthetic mutant on
every run. The header understates its own coverage by half and points the reader at the weaker
of the two nets.

**Owner: `strat-gameplay-engineer`.**

## What I verified that is correct, because most of this pass is

**The C++ is sound.** `bEmitted` is sized from `Count`; the `+ 1` is gone;
`check(Slot >= 0 && Slot < UE_ARRAY_COUNT(bEmitted))` sits one instruction before
`bEmitted[Slot] = true`, which is where it has to be. The first `static_assert` pins
`ButtonClick == 0` and the second pins `MatchEnded + 1 == Count`; between them they catch an
explicit value anywhere in the list and an insertion between the last cue and the sentinel, and
they cannot catch a declaration *after* `Count`, which is exactly what the header says. The
residual hole is real, correctly reasoned — no C++17 construct can name a symbol it does not
know — and neither overstated nor, except as noted in Finding 4, understated.

**The engineer's refusal was right and this seat's instruction was not achievable.** "Appending
after `MatchEnded` becomes a compile error" needs a hard-coded magnitude; that fires on the safe
append and is silent on the dangerous one. What shipped pins the two properties `Count`'s
correctness actually rests on.

**The value bound is genuinely better than the metadata discriminator, and the test lane's
refusal was not over-cautious.** Ground (1) is decisive on its own: `Hidden` means "hide in
Blueprint", so a real cue hidden cosmetically would vanish from both walks silently — the
precise under-coverage both files exist against, and unlike a value bound that failure is
invisible in a green run. Ground (3) is measured, and I confirmed the `#if WITH_METADATA` guard
above. Ground (2) is correct as an argument that the metadata route buys nothing structurally,
since the `_MAX` name rule survives it either way. The shipped filter, keeping values in
`[0, Count)`, drops both sentinels with one rule, needs no `#if WITH_EDITOR` guard, and needs no
edit when a second sentinel or a rename arrives. That is a better design than the one refused,
not a cheaper one.

**The four repaired clauses still kill their mutants, and the renamed one pins what the asserts
cannot.** `NoSoundCueIsDeclaredAfterTheCountSentinel` asserts every enumerator's value lies in
`[0, Count)` over the reflected `UEnum` — the after-`Count` case, which is exactly the residue.
Its CONTROL 4 is the part worth naming: the decision was extracted as a free function over
name/value pairs, and the clause runs it over a synthetic healthy enum (expect no offenders) and
a synthetic mutant with one enumerator after the sentinel (expect `DeclaredAfterCount=4`, named)
on every suite run, permanently, without production code being edited. CONTROL 2d is a second,
independent detector of the same mutant via the checked count. The three parity clauses now
route through `CueSetControlOrFail`, which upgrades a "more than one" vacuity guard into an
equality against `EStratSoundCue::Count` — the only one of the three that can see the walk
quietly *losing* a cue, which is the failure the refused metadata route would have risked.

**The `StratSoundBank.cpp` retraction is honest and its measurement is right.** `C4062`
("enumerator in switch of enum is not handled") and `C4061` are both MSVC level-4 and off by
default, and nothing in any `.Build.cs` in this tree promotes them — the field census run for
check 5 shows `ShadowVariableWarningLevel` is the only warning field any of them touches. So the
old sentence asserted a mechanism that never existed on this toolchain, and the entry says so in
those terms rather than as a stale comment. Declining `/we4062` in this pass is correct: it is a
module-wide build-configuration change with its own blast radius.

## Item 3 — whether the sentinel changes any shipped asset: MEASURED, not reasoned. It does not.

The reasoning relayed to this gate was "seven named `TObjectPtr` slots ... plus name-based enum
serialization", with the grep "inconclusive because `.uasset` are LFS pointers". Two corrections,
one of which matters.

**The asset is not an LFS pointer in the working tree, and the measurement was available.** LFS
smudges on checkout; only the *blob* is a pointer, which is why `git show` returns one.
`Content/StratAudio/DA_StratSoundBank.uasset` is 3656 bytes of real package data whose first
four bytes are the UE package magic `c1832a9e`. Extracting every printable ASCII and UTF-16LE
run of length four or more from it yields a name table containing exactly these property names:

    ButtonClick  TurnEnded  UnitMoved  UnitAttacked  UnitDestroyed  FactoryBuiltUnit  MatchEnded
    Concurrency  BaseMix  MasterSoundClass  SfxSoundClass  MusicSoundClass
    ObjectProperty  SoundClass  SoundConcurrency  SoundMix  MetaSoundSource
    /Script/StratPlay.StratSoundBank  and the thirteen /Game/StratAudio/... object paths

Control that the instrument speaks: it returned sixty-odd names including every one known to be
present — the class path, the seven `MS_Strat_*` MetaSound references, the three `SCL_Strat_*`
sound classes, `SMX_Strat_Base` and `SC_StratCues`. Against that control the absences are
evidence: **no `MinSecondsBetween`, no `EStratSoundCue`, no `ByteProperty`, no `EnumProperty`,
no `Count`.** The only serialized property tag in the file is `ObjectProperty`. So no enum value
of any kind is serialized in this asset, and the sentinel cannot reach it.

**The premise as relayed was incomplete, but `engine.md` is not.** `UStratSoundBank` is not
purely seven named slots: `Source/StratPlay/StratSoundBank.h:151` carries
`UPROPERTY(EditDefaultsOnly) TMap<EStratSoundCue, float> MinSecondsBetween;`, a genuinely
enum-keyed serialized property. `engine.md`'s entry names it, and names the second one
(`UStratSoundDirector::LastPlayedAtSeconds`), and gives the right reason — an absent key is a
defined state, and `Count` is appended so no existing enumerator's value shifts. The measurement
above closes it independently: the map is empty on disk, exactly as that header's "AN UNSET MAP
IS THE SHIPPED DEFAULT" says. **The `UMETA(Hidden)` is load-bearing on that map specifically** —
without it, a designer would see `Count` in that map's key dropdown in the details panel.

## Item 4 — both instrument fixes, re-derived rather than trusted.

Run against a faithful copy of the tree, using `cp -rp` throughout so mtimes are preserved. (A
`cp -r` without `-p` resets them and produces a REPORT IDENTITY finding on its own — which is
itself the fresh-checkout result noted under item 5, measured here by accident.)

| Arm | Mutation | Result |
|---|---|---|
| 0 (control) | none — faithful copy | `SWEEP CLEAN` |
| A | touch `Source/StratUI/StratSoundCues.cpp`, production, carries no test macro | `REPORT IDENTITY`, "predates **a production source file** modified ...", `SWEEP FAILED` |
| B | touch `Source/StratUI/Tests/StratSoundCueClauses.cpp`, test-defining | `REPORT IDENTITY`, "predates **a test-defining source file** ...", `SWEEP FAILED` |
| A/B restored | mtimes restored from the live tree | `SWEEP CLEAN` |
| C | `global.md:26`'s citation replaced with `reportCreatedOn 2026.09.05-11.11.11` | `REPORT PROVENANCE` at `global.md:26`, naming both the cited run and the one actually opened |
| D | the same wrong citation, plus the metasyntactic ellipsis form of the marker written as prose in the next sentence | **still FAILS** — the literal quotation no longer exempts |
| E | the same wrong citation, plus a genuine dated `[STAMPED 2026-09-06: superseded by the run below.]` in the next sentence | `SWEEP CLEAN` — a real stamp one sentence away still exempts |

Arms A and B confirm 2(b) in both directions, and the arm-naming in the finding text works.
**Arm E is the one the brief asked for and it is the important one: no real stamp is newly
treated as live.** That is confirmed a second, independent way by the census in Finding 2 —
every ellipsis-form occurrence in the record is metasyntactic prose in `data.md`, so the
tightened regex refuses nothing anyone relied on.

Live instrument, unmutated: `python Tools/architect/strat_banner_sweep.py` returns
`SWEEP CLEAN`, exit 0, reading `445 entries, all Success (2026.09.06-22.46.08)` with the macro
census agreeing at 445; `--self-test` returns `SELF-TEST: ALL FIXTURES CORRECT`, including the
four new cases and the direct `read_macro_census` regression pin.

## Item 5 — the judgment call. The shipped behaviour is RIGHT. Do not refine it now.

**Plainly: right, not too strict, and comment-stripping is the wrong refinement.** Four reasons,
in order of weight.

1. **The proposed refinement fails in the silent direction.** A freshness check that
   over-reports staleness costs a suite run. One that under-reports it returns CLEAN over red
   clauses — which is the defect this fix closes, and which was avoided last time only because a
   lane chose to run the suite by hand. A comment stripper is a C++ lexer: a `//` inside a string
   literal, raw string literals, a line-continuation inside a `//` comment, and — measured in this
   tree already — per-file CRLF/LF variance. Every one of those, gotten wrong, decides a real
   code change was "comment-only" and hands back a green. Trading a loud false positive for a
   quiet false negative is the wrong direction for a guard whose whole job is refusing stale
   evidence.
2. **The cost is not new, which is the argument that settles it.** As `read_macro_census`'s own
   docstring correctly notes, this check *already* gave a comment-only edit to a TEST file
   exactly this treatment, and has for as long as it has existed. Nobody routed around it. The
   change extends an accepted cost to a second file class; it does not introduce a new class of
   friction whose tolerance is unknown.
3. **The engineer's comment-stripper is a one-off proof instrument, not a gate input.** It was
   built to demonstrate a negative once, over a bounded diff, under a human's eye. Promoting it
   to a standing input to a verdict raises its reliability bar by a large step, and nothing has
   measured it at that bar.
4. **The remedy is cheap and self-clearing.** A comment-only pass that trips this re-runs the
   suite and loses minutes. There is no state to unpick and no judgement call to make.

**If it is ever refined, the right shape is a content hash, not a comment strip.** Record a hash
per `Source/**/*.{cpp,h}` at report time and compare hashes rather than mtimes. That removes the
largest real source of spurious findings — mtime churn with no byte change, from a fresh
checkout, a `git restore`, `autocrlf` rewriting on checkout, or an editor saving an unchanged
file, none of which moved a byte — and it makes no lexical judgement about what a byte *means*,
so it cannot fail silently. I measured that churn case accidentally and it is worth naming: a
`cp -r` of `Source/` without `-p` produced "predates a production source file" on an otherwise
byte-identical tree. **A fresh checkout of this repository will trip REPORT IDENTITY until the
suite is re-run.** That is correct behaviour under the fail-safe reading, and is worth someone
knowing before it surprises them.

**This is a FOLLOW-UP for `strat-data-steward`, not a finding.** Nothing shipped here is wrong.

## Item 6 — record honesty, beyond Findings 2 and 3.

Nobody wrote outside their own file. Only `global.md` states a suite count — the numeric hits in
`engine.md` and `tests.md` are line counts and a `startingFame` pair, and `data.md`'s are
historical and fixture figures inside stamped entries, which the sweep itself accepts (it returns
CLEAN, and it is the mechanical authority for that rule).

`global.md`'s new entry is accurate on everything I could check except Finding 3. The `445/445`
and `reportCreatedOn 2026.09.06-22.46.08` match the report and the census. "One clause RENAMED
and three REPAIRED, none added" matches the diff exactly — the rename in
`StratSoundCueClauses.cpp`, the three parity clauses routed through `CueSetControlOrFail`. The
four-clause blast radius, and the note that the fourth arrived through a shared `AllCues()`
helper neither prediction had traced, are both borne out by the diff. The account of what the
test lane refused and why matches both `Tests/` files, and survives the one ground I could verify
independently, the `#if WITH_METADATA` guard in Finding 4. The crashed mutant that wrote no
report is not checkable from a checkout and is stated as exactly that, which is the honest form.
The third retracted guard in `StratSoundBank.cpp` is real, correctly measured, and correctly
scoped.

## Observations

Non-gating.

- **`Emit`'s `check` compares an `int32` against the unsigned result of `UE_ARRAY_COUNT`.** The
  `Slot >= 0` conjunct makes it correct, and the pass reports the build succeeded, so this is
  noted rather than raised. If `/we4018` is ever turned on, this line is where it lands.
- **The rename's stated cost may be understated.** The clause banner says the old name
  `MatchEndedIsTheLastSoundCue` "appears in `global.md` and in two `Tools/architect/gate_reports/`
  files". There are eight `2026-09-06-*` reports in that directory; I did not count occurrences
  across all of them, and the figure is exposed to the same growth mechanism as Finding 2.
- **`Tools/architect/gate_reports/` now holds nine reports dated 2026-09-06**, eight of them
  written before this one. Nothing is wrong with that; it is worth noting that a future search
  for "the 2026-09-06 gate" will not resolve to one document.

VERDICT: BLOCK
