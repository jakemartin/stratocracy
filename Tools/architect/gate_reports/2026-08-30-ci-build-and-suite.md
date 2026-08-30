# Gate: CI build-and-suite workflow + suite report gate — strat-integration-reviewer

**THREE ROUNDS: BLOCK, BLOCK, BLOCK -- three findings, three, then two.** Each round gates the
remediation of the one before it, so this reads as a sequence and not as three opinions of one
tree. Each round begins at a horizontal rule. Dispatched by the `coordinator` on 2026-08-30 against
the uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD `6b8c8e3`.

**ROUND 1'S FINDINGS WERE ABOUT THE ARTEFACTS; ROUND 2'S ARE ALL PROSE.** Round 2 attacked the
identity check that round 1 forced and could not break it -- an independently-written extractor
agreed with the capturing regex on all 287 names, and six adversarial probes plus a healthy
control all behaved. What it found instead was that three places still DESCRIBED the count
invariant the remediation had replaced, and that a fixture count did not reproduce.

**The report below the rule is the reviewer's as returned, transcribed by the `coordinator` and
not edited**, except that a few `&gt;` sequences are restored to `>` — an artifact of how it
reached this session. It is persisted for the same reason as
`2026-08-29-layerfor-late-mesh.md`: each remediation rewrites the prose its finding cites, so
without this file the findings would name text that no longer exists. **Grep the quotation, not
the line number** — the line numbers in it are already stale, by the remediation it caused.

**What happened after the report is NOT part of it and is stated here by the `coordinator`.**
All three findings were verified independently and accepted.

*Finding 1* — `Content/**` absent from the path filter. Re-derived: three tracked
`Content/StratData/DT_*.uasset` tables, loaded by 54 test source files. `Content/**` added
broad rather than `Content/StratData/**`, because a Blueprint default is an input to behaviour
too and being wrong in that direction costs a build rather than a regression.

*Finding 2* — the census compared cardinality where identity was free. **The reviewer's probe
was reproduced before being accepted**: a tree declaring `New0..2` against a report listing
`Old0..2` returned `SUITE REPORT GATE CLEAN`, exit 0. The macro regex now captures the clause
NAME, `census()` returns the declared names, and the check is a set difference in both
directions, naming MISSING and EXTRA separately because they mean different things. The same
probe now exits 1 and is kept as a fixture. FIVE fixtures added -- the stale-binary probe, a
duplicate in the report, a duplicate in the tree, and the two checks the reviewer found
unfixtured -- for 22 in total. **An earlier version of this paragraph said six**, and round 2
below caught it; the enumeration was always five and only the numeral was wrong.

*Finding 3* — the banner claimed not to restate the mechanism and then restated it. Cut from
the banner and from the workflow's step comments, leaving two homes: the gate script, which
reasons them, and `global.md`'s `## NEXT`, which records the pass.

*The non-gating `actions/checkout` observation* was also acted on: the `Saved/` persistence
premise is now marked unverified where it is reasoned, rather than asserted four times.

*After round 2:* all three findings verified and accepted. The `## NEXT` enumeration that
described the gate was the PRE-REMEDIATION description and had not been revisited -- fixing that
one bullet closed findings 1 and 2 at their common cause, and the workflow's gate-step comment
and the gate script's COMPLEX message were corrected to match. Finding 3 reproduced against the
tool: `--self-test` prints **22** `[OK]` lines, so five fixtures were added and not six; the six
came from `grep -c 'case('`, which counts the `def case(...)` helper, while the anchored
`grep -c '^    case('` agrees with the tool. Two collectors differing by one filter -- recorded
because it is the third figure this session manufactured that way.

*After round 3:* both findings verified and accepted. The first is the pass's sharpest lesson:
the retired count invariant survived in THIS GATE SCRIPT'S OWN DOCSTRING, which
`build-and-suite.yml` and `global.md`'s banner both designate as the authority they point at
instead of repeating -- and the coordinator's survivor sweep could not have found it, because
it searched for the three phrasings already known while that sentence shares vocabulary with
none of them. A shape-based scan found it at once. **A single authority that rots takes every
pointer with it, silently.** Retracted in place, for that reason. The second was a third copy
of the fixture numeral, twenty-five lines above the corrected one, in the bullet the previous
rewrite did not revisit; the numeral is now removed rather than corrected, since the tool
prints its own.

---

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD `6b8c8e3`. Four paths: two modified (`.github/workflows/banner-sweep.yml`, `Tools/architect/state/global.md`), two untracked (`.github/workflows/build-and-suite.yml`, `Tools/architect/strat_suite_report_gate.py`). Nothing staged. `git diff HEAD -- Source/` is empty, so the tree under `Source/` is byte-identical to the committed, six-times-gated state. I ran the gate's self-test, ran the gate against the real report both clean and pinned-past, wrote two adversarial probes of my own against its central invariant, parsed both YAMLs, and queried the runner API. Did **not** build, run the suite, or execute the workflow's PowerShell.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff HEAD --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits; neither new file contains C++ |
| 3 | No vendored header in front of UHT | pass — no header changed |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — no C++ changed |
| 5 | Module arrows unchanged and correct | pass — re-derived: 15 rows, 6 field names, no `.Build.cs` in the diff |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass — no C++ changed; neither new file contains one |
| 9 | No widget-side arithmetic | pass (vacuous) |
| 10 | Lane discipline | pass — `strat_suite_report_gate.py` is an out-of-lane write into the steward's `Tools/architect/`, declared in the receiving artifact's own header (lines 32-38) and not only in `global.md`, correctly disclaimed as not the transcription clause |
| 11 | Prose blocks present | **BLOCK** — see Findings 1 and 3 |
| 12 | Nothing staged or committed | pass — `git diff --cached` empty, HEAD `6b8c8e3` |

## Findings

**1. BLOCK — `Content/**` is absent from the path filter, and the filter's own comment claims completeness it does not have.**

`E:\MultiAgent\Stratocracy\.github\workflows\build-and-suite.yml:28-36`:

```yaml
    paths:
      # Only what can change the answer. A record-only or documentation commit does not need a
      # ten-minute engine build, and `banner-sweep.yml` already covers those on every push.
      - 'Source/**'
      - 'Data/**'
      - 'Config/**'
      - 'Stratocracy.uproject'
      - '.github/workflows/build-and-suite.yml'
      - 'Tools/architect/strat_suite_report_gate.py'
```

`Content/StratData/DT_Units.uasset`, `DT_Terrain.uasset` and `DT_Effectiveness.uasset` are **tracked in git** (1907 files under `Content/` are), and **54 test source files** load them at runtime through `/Game/StratData/DT_Units.DT_Units` and friends — every parity fixture in `Source/StratBridge/Tests/`, most of `Source/StratPlay/Tests/`, and `Source/Stratocracy/StratData/Tests/StratDataParity.cpp`, whose entire subject is that the DataTable agrees with the vendored bytes. A commit that changes one of those `.uasset` files changes the suite's answer for a large fraction of the 287 clauses and **will not trigger this workflow**.

This is not hypothetical for this project: `Content/` is the editor-builder's lane, asset-only commits are a normal phase here, and the repo carries a whole procedure for committing one. The comment *"Only what can change the answer"* is an affirmative claim about the filter that the tree falsifies, and the gap it hides is the single asset directory the suite reads. Adding `Content/StratData/**` — or `Content/**`, at the cost of some unnecessary builds — closes it. **Owner: the `coordinator`** (workflow author).

**2. BLOCK — the census invariant compares CARDINALITY, not IDENTITY, and the check's own error text claims it catches a stale binary, which it cannot.**

`Tools/architect/strat_suite_report_gate.py:189-196` raises on `len(tests) != simple` with:

> *"A clause that fails to REGISTER does not go red -- it is absent, and absence is invisible to every results check above this one. Suspect a stale binary (a clause NAME is compiled), a filtered run, or a duplicate test name."*

Two of those three it does catch — a filtered run and a duplicate name both reduce the entry count. **A stale binary does not**, because the way it presents in this project is a clause *rename*, and a rename preserves the count exactly. I demonstrated it rather than arguing it. Probe A: a fixture tree declaring three clauses named `…New0/1/2` against a report listing three Success entries named `…Old0/1/2` — the exact "stale binary reports the OLD names green" scenario this repository already has on record:

```
  census:  IMPLEMENT_SIMPLE_AUTOMATION_TEST=3, COMPLEX/CUSTOM=0
  3/3 clauses Success, and the tree declares 3.
SUITE REPORT GATE CLEAN
EXIT= 0
```

Probe B: a report listing the *same* clause three times against a tree declaring three distinct ones — also `SUITE REPORT GATE CLEAN`, exit 0. The gate never compares a single `fullTestPath` against a single clause name.

The docstring's headline claim at `:25-28` is true as written (an absent clause reduces the count), so the invariant is sound for the subject it names. What is false is the hint sentence, and it is the sentence a person reads at 2am when the check fires. The fix is cheap and the data is already in hand: `census()` opens every `.cpp` and the macro's second argument is the clause name, so a set-difference against `{t["fullTestPath"] for t in tests}` would turn cardinality into identity and subsume the count check entirely. **Owner: the `coordinator`.**

**3. BLOCK — the new banner says it does not restate the mechanism, in the sentence immediately before restating it, and the four mechanisms now exist in four copies each.**

`Tools/architect/state/global.md:26-28`:
> *"The `## NEXT` entry holds the detail and this banner does not restate it -- a pointer, because a restated mechanism is the defect the pass below spent four gate rounds on."*

Thirteen lines later, `global.md:41-49` restates it — the log undercount with the 286-of-287 anecdote, the unread exit code, the `Saved/` persistence premise, and the census check described as *"the only check in the set that can see a clause that never REGISTERED."* All four then appear again at `global.md:2854-2863` in the `## NEXT` bullet the banner points at. Counting the whole pass:

| mechanism | copies |
|---|---|
| log undercounts by exactly one | `global.md:42`, `global.md:2855`, `build-and-suite.yml:166`, gate script docstring `:8-16` |
| `Saved/` gitignored and persists between jobs | `global.md:46`, `global.md:2859`, gate script `:20`, `build-and-suite.yml:108` |
| entries-vs-census is the only check that sees a clause that never registered | `global.md:48`, `global.md:2862`, gate script `:26`, gate script `:193` |

You asked me to assume there was a fifth restatement in this pass. There is, it is self-referential, and it sits in the same file as the four-round lesson about exactly this. The `banner-sweep.yml` edit at `:20-22` shows the discipline applied correctly — a pointer, with the reason stated — which makes the banner's lapse a slip rather than a misunderstanding. The sentence at `:26-28` should either go, or say what is true: the mechanism is stated here and in `## NEXT`, and the workflow and the gate script are the authorities. **Owner: the `coordinator`.**

## Observations

Non-gating.

- **The `Saved/` persistence premise may not survive `actions/checkout@v5`, and I could not measure it.** `actions/checkout` defaults to `clean: true`, which runs `git clean -ffdx` — and `-x` removes ignored files, which `Saved/` is (`.gitignore:68`, `Saved/*`, zero tracked files under it). If that holds, no stale report can be present when the stamp step runs, and the premise stated four times above is wrong in its details. **This changes nothing about the design:** the `--not-before` pin and the explicit delete are correct, cheap, and remain the right defence for a `clean: false` runner or a report written outside the workspace. I am flagging it as an unverified premise, not a defect, because asserting third-party action behaviour I cannot execute here is exactly the kind of unmeasured claim this pass keeps blocking on.
- **Two checks are unfixtured**, out of thirteen: a malformed `--not-before` (`:118-121`) and a nonexistent `--source-root` (`:163-164`). Neither the script's self-test docstring nor the workflow comment claims total coverage — they claim each fixture proves one check can fail, which is true — so this is not a false claim, just a gap. Both are cheap to add.
- **The census is textual and fail-closed in the right direction.** `^IMPLEMENT_SIMPLE_AUTOMATION_TEST\b` multiline over `Source/**/*.cpp` would also count a macro inside `#if 0`, inside a block comment starting at column 0, or in a `.cpp` under `Source/` that no module compiles. Each would inflate the census and turn the gate permanently red — a false positive, never a false pass. Worth knowing before someone comments a clause out.
- **The invariant is sound for today's tree, and I checked the two things that could break it.** All 287 clause names extracted from the macros begin with `Stratocracy.`, so the `RunTests Stratocracy` filter should reach every one; and there are zero duplicate clause names. Both were the coordinator's stated concerns and both hold.
- **Everything the dispatch asked me to re-derive reproduces.** Self-test: 17 fixtures, 17 `[OK]`, `SELF-TEST: ALL FIXTURES CORRECT`, exit 0 — expected-PASS and expected-FAIL both present (3 pass-fixtures, 14 fail-fixtures). Real report: exit 0 clean, `287/287 clauses Success, and the tree declares 287`. Pinned to `2026.08.30-05.00.00`: exit 1, `STALE REPORT`. `gh api repos/jakemartin/stratocracy/actions/runners` → `{"total_count":0,"runners":[]}` (the remote is `jakemartin/stratocracy`, so the record names the right repo). Sweep clean, `44 claims (1 live, 43 stamped)`, report identity `2026.08.30-04.21.41`. Both workflow YAMLs parse. `git diff HEAD -- Source/` empty, so the deliberate compile break is fully restored.
- **The no-`if:`-guard decision is right and the reasoning is sound.** A queued job is visibly not-yet-run; a skipped job reports success to branch protection. Refusing to soften that is the correct call, and the "locate the engine and refuse to continue" step fails rather than skips for the same reason.
- **A green run cannot occur with the suite not having run**, as far as I can trace it: no report → gate fails; empty `tests` → fails; wrong count → fails; any non-`Success` state → fails; a missing `state` key reads as `None` and fails; an absent `succeeded` counter defaults to 0 and trips the counter check. An unset `SUITE_NOT_BEFORE` leaves `--not-before` with no value and argparse exits 2, which the step converts to a failure. Every path I could find is fail-closed.
- **The real report's shape matches what the gate assumes** — `tests` is a flat list of 287 dicts carrying `fullTestPath` and `state`, top-level counters present, `reportCreatedOn` in `YYYY.MM.DD-HH.MM.SS`, and the file carries a BOM that the gate's `utf-8-sig` read handles.

**What I did not measure:** the build; the claim that `Build.bat` exits 6 on a compile error and 0 on success; the PowerShell native-argument form around `-ExecCmds=Automation RunTests Stratocracy;Quit`; the suite run itself; and any behaviour of `actions/checkout@v5` or the GitHub Actions runner, none of which exists on this box to execute.

VERDICT: BLOCK

---

# Gate: CI build-and-suite workflow + suite report gate (ROUND 2) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD `6b8c8e3`. Five paths: two modified (`banner-sweep.yml`, `global.md`), three untracked (`build-and-suite.yml`, `strat_suite_report_gate.py`, `gate_reports/2026-08-30-ci-build-and-suite.md`). Nothing staged; `git diff HEAD -- Source/ Data/` is empty. I re-derived the name-capturing regex against an independently-written extractor, ran seven adversarial probes of my own against the new identity check, ran the self-test and the real gate, censused the four mechanisms, and counted the fixtures three ways.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — printed nothing |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — no header changed |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — no C++ changed |
| 5 | Module arrows unchanged and correct | pass — 15 rows, 6 field names |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass |
| 9 | No widget-side arithmetic | pass (vacuous) |
| 10 | Lane discipline | pass — the out-of-lane write into `Tools/architect/` is declared in the receiving file's own header (`:32-38`) |
| 11 | Prose blocks present | **BLOCK** — Findings 1-3 |
| 12 | Nothing staged or committed | pass — `git diff --cached` empty, HEAD `6b8c8e3` |

## The new identity check — attacked, and it holds

I could not break it. Reporting this first because it is the substance of the round.

**The regex captures every real declaration and nothing else.** I wrote an independent extractor — scan for the bare macro token anywhere, then walk the arguments by hand, supporting concatenated string literals — and compared. 287 token occurrences in `Source/**/*.cpp`, **all 287 at column 0**, so the `^` anchor currently discards nothing and there is no occurrence of the token that is not a declaration. Their regex extracts 287; mine extracts 287; the two sets are **equal**, with zero names built from more than one literal, so the single-literal `([^"]+)` capture misses nothing today. The `[^,]+` first-argument match cannot cross a comma, so it cannot run past its own declaration.

**Seven probes, six adversarial and one healthy control:**

| probe | result |
|---|---|
| Probe A — 3 OLD names reported vs 3 NEW declared (stale binary) | **exit 1**, `CLAUSE SET DISAGREEMENT` — the round-1 hole is closed |
| report lists one clause twice, count still matches | exit 1, `the REPORT lists the same clause more than once` |
| tree declares one name twice | exit 1, `the TREE declares the same clause name more than once` |
| one MISSING and one EXTRA at once, count matches | exit 1, `CLAUSE SET DISAGREEMENT` — answers "more than one way at once" |
| an entry with no `fullTestPath` key at all | exit 1 — fails closed rather than treating it as unnamed |
| a macro inside a block comment at column 0 | exit 1 — false positive, fail-closed, and no such occurrence exists today |
| healthy 3/3 control | **exit 0** — the guard stays quiet on the healthy path |

Self-test `ALL FIXTURES CORRECT`; the real report exits 0 with `287/287 clauses Success, and every one is a name the tree declares`. Duplicate handling runs before the set difference and raises on the first thing wrong, which is the documented contract. **I found no shape that passes while the suite was broken** — every gap I could construct (missing key, absent counter, unparseable report, uncompiled clause, clause in a `.h`) fails closed.

**`Content/**` closes the filter.** I re-derived the fourth-input question: the suite reads `Source/`, `Content/` (the three tracked `DT_*.uasset`), `Data/` (`ferrum_crossing.json`, `units.csv`, `terrain.csv`, `effectiveness.csv` and the scenario fixtures), `Config/`, and `Stratocracy.uproject` — all six are now listed, plus the workflow and the gate script. I could not name a seventh.

## Findings

**1. BLOCK — three live descriptions still describe the count invariant that Finding 2 replaced.**

The gate no longer has a count branch — the code comment at `strat_suite_report_gate.py:211-214` says the set difference "subsumes the count check", and the failure output is `CLAUSE SET DISAGREEMENT`. Three places still say otherwise:

- `E:\MultiAgent\Stratocracy\.github\workflows\build-and-suite.yml:196` — *"the entry count cross-checked against the tree's clause-macro census"*. This is the comment on the step that invokes the gate; someone debugging a red run is sent looking for a count discrepancy that the gate no longer computes.
- `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md:2867`, `## NEXT` item **(4)** — *"The entry count must equal the tree's clause-macro census -- the only check that can see a clause which never registered"*. Present tense, unqualified. A later bullet in the same entry (`:2897-2899`) gives the correct description — *"the check is a set difference in both directions, reporting MISSING and EXTRA separately"* — but does not stamp item (4), so the entry asserts both.
- `E:\MultiAgent\Stratocracy\Tools\architect\strat_suite_report_gate.py:206` — *"This gate's entries-equal-macros invariant assumes SIMPLE macros only"*. Mildest of the three: the *reasoning* still holds under identity (a complex macro emits one entry per row, which now surfaces as EXTRA), but the invariant it names no longer exists.

**Owner: the `coordinator`.**

**2. BLOCK — the `Saved/` persistence premise is asserted as fact in the record while the gate script marks it UNVERIFIED.**

The qualification you added to `strat_suite_report_gate.py:26-32` is exemplary — it names `clean: true`, `git clean -ffdx` and `-x`, says plainly that nobody here has executed a runner, and keeps the pin with its reasoning intact. That is exactly right.

But you asked me to check that no unqualified copy survives, and one does: `E:\MultiAgent\Stratocracy\Tools\architect\state\global.md:2865`, `## NEXT` item **(3)** — *"`Saved/` is gitignored and **PERSISTS** between jobs on a self-hosted runner"*. Flat present-tense assertion. The same entry marks it unverified thirty lines later at `:2905-2910`, again without stamping item (3).

Items (3) and (4) share one cause: **the enumerated five-item bullet at `global.md:2860-2870` is the pre-remediation description of the gate and was not revisited when the later findings bullet was added.** Fixing that bullet closes both findings. **Owner: the `coordinator`.**

**3. BLOCK — "Six fixtures added in all" does not reproduce; five were added, and the total is 22.**

`E:\MultiAgent\Stratocracy\Tools\architect\state\global.md:2900`: *"**Six fixtures added in all**, including the two checks the reviewer found unfixtured."*

The tool's own output disagrees. `--self-test` prints **22** `[OK]` lines; I measured **17** in round 1. Five added, 22 total. Your own enumeration in the dispatch lists exactly five — the stale-binary probe, a report listing one clause twice, a tree declaring one name twice, and the two unfixtured checks — so the enumeration is right and only the numeral is wrong.

Where the 23 comes from: `grep -c 'case('` returns 23, but one of those is the helper definition `def case(label, should_pass, fn):` at `:313`. The anchored `grep -c '^    case('` returns 22. **Two counts gathered by different filters**, subtracted against each other — the same collector mismatch this project records, and the same shape as the `head -30` figure from the previous pass. **Owner: the `coordinator`.**

Worth noting alongside it: removing the `17 inline fixtures` count from the workflow comment was the right call and it worked — `build-and-suite.yml:109` now carries no number and says why. The surviving figure is in the record, which the workflow fix did not cover.

## Observations

Non-gating.

- **The restatement census confirms: two homes each, and the banner and workflow now point rather than restate.** Log undercount → gate script `:10-11`, `global.md:2861-2863`. `Saved/` persistence → gate script `:20`/`:28`/`:145`, `global.md:2865`/`:2907`. Never-registered → gate script `:35`/`:254`/`:257`, `global.md:2868`. Exit code → gate script `:18`, `global.md:2864`. Multiple occurrences within a home, two homes each — which is what you claimed. The banner at `global.md:46-49` now states the outcome and names the gate script as the authority, and the workflow's suite-step comment at `:178-184` explicitly says *"reasoned once, in `strat_suite_report_gate.py`'s docstring. Pointed at rather than repeated."* `banner-sweep.yml:10` mentions `Saved/` for its own separate point (that the report does not exist on a hosted runner) and is not a third home.
- **The persisted gate report is faithful and its citation resolves.** My round-1 text verbatim, single `VERDICT: BLOCK` at `:131`, preamble disclosing the `&gt;` restoration and warning that its line numbers are already stale by the remediation it caused. Cited at `global.md:2884` as `Tools/architect/gate_reports/2026-08-30-ci-build-and-suite.md`; the file exists at that path.
- **`.gitattributes` is the only input I can name that is outside the filter and could conceivably change what the suite sees** — it drives LFS smudging for `Content/**/*.uasset`. Editing it is not a normal operation here and I am not asking for it; recorded only so the "fourth input" question has a complete answer.
- **`build-and-suite.yml:109`'s countless phrasing is the right pattern** and is worth copying the next time a figure would rot.

**What I did not measure:** the build; `Build.bat`'s exit 6 on a compile error; the PowerShell `-ExecCmds` argument form; the suite run; and any behaviour of `actions/checkout@v5` or a GitHub Actions runner — none of which exists on this box to execute.

VERDICT: BLOCK

---

# Gate: CI build-and-suite workflow + suite report gate (ROUND 3) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, HEAD `6b8c8e3`. Five paths, nothing staged, `git diff HEAD -- Source/ Data/` empty. I re-read the rewritten enumeration item by item against the code, ran my own shape-based survivor sweep with vocabulary deliberately unlike yours, re-derived the fixture figures three ways, and re-ran all four gates and the standard checklist.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside StratBridge/StratRules | pass — zero hits |
| 3 | No vendored header in front of UHT | pass — no header changed |
| 4 | `TPimplPtr` for `FStratBridge` members | pass — no C++ changed |
| 5 | Module arrows unchanged and correct | pass — 15 rows, 6 field names |
| 6 | New modules registered | pass — none |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in C++ | pass |
| 9 | No widget-side arithmetic | pass (vacuous) |
| 10 | Lane discipline | pass — out-of-lane write declared in the receiving file |
| 11 | Prose blocks present | **BLOCK** — Findings 1 and 2 |
| 12 | Nothing staged or committed | pass — `git diff --cached` empty, HEAD `6b8c8e3` |

## Findings

**1. BLOCK — the retired count invariant survives in the gate script's own docstring, which is the single place every other file points at.**

`E:\MultiAgent\Stratocracy\Tools\architect\strat_suite_report_gate.py:34-36`:

```
AND ONE INVARIANT THAT CATCHES WHAT ALL THREE MISS: the number of entries in the report must
equal the number of clause macros in the tree. A clause that fails to REGISTER does not fail
-- it is absent, and an absent clause is invisible to every check that only reads results.
```

Unqualified, present tense, no retraction marker — and it describes the check I proved in round 2 lets a report of three entirely different names through. The code four hundred lines below does a set difference on names; this paragraph says it compares two numbers.

**Your sweep could not have caught it, and that is the point you asked me to test.** Your patterns were `entry count (must|cross-checked)`, `entries-equal-macros`, and `count must equal the .*census`. This sentence says *"the number of entries in the report must equal the number of clause macros in the tree"* — no "entry count", no hyphenated form, no "census". Zero of three match. My sweep looked for the *shape* — a quantity word within ~70 characters of `census|macro|entries|entry`, minus retraction vocabulary — and it surfaced immediately. Phrase sweep versus claim-shape sweep, exactly the distinction that cost three rounds on the previous pass.

**Where it sits makes it worse than an ordinary stale sentence.** `build-and-suite.yml:178-184` says the reasoning is *"reasoned once, in `strat_suite_report_gate.py`'s docstring. Pointed at rather than repeated"*, and `global.md:46-49` names the same docstring as the authority. The pointer discipline you adopted is right — but a single authority that rots takes every pointer with it, silently, and nothing in the tree can notice. It also sits four lines below item 3's exemplary UNVERIFIED block, so it was on screen while that paragraph was being written.

**Owner: the `coordinator`.**

**2. BLOCK — `global.md` still carries "The gate's 17 fixtures", the third copy of the numeral and now the stale one.**

`E:\MultiAgent\Stratocracy\Tools\architect\state\global.md:2882`, in the "MEASURED BEFORE BEING WIRED IN, NOT ASSUMED" bullet:

> *"The gate's **17 fixtures** each prove ONE check can FAIL, the healthy path among them, and the gate was additionally run against the REAL report from this tree: exit 0 clean, exit 1 when pinned to a later stamp."*

Twenty-five lines below, `:2907` says *"**FIVE fixtures added, 22 in total** -- the tool's own `--self-test`…"*. The same `## NEXT` entry asserts 17 and 22. The 17 was correct when written and was not revisited — **the identical cause as findings 1 and 2 of round 2**: the correction was appended in a lower bullet while the earlier paragraph stating the same fact was left alone. You fixed items (3), (4) and (5) of the first bullet; this is the second bullet.

You asked whether a third copy exists. It does, and this is it. **Owner: the `coordinator`.**

## Observations

Non-gating.

- **The rewritten enumeration is correct, item by item, and each correction reads as a correction.** I checked all five against the code rather than against your description. (1) and (2) unchanged and still true. (3) now says a report *can* be sitting on a reused workspace, marks the `actions/checkout` question UNVERIFIED with `clean: true`/`git clean -ffdx`/`-x` named, and records that an earlier version asserted persistence as fact — honest, and it matches the gate script's own qualification. (4) matches the implementation exactly, including MISSING/EXTRA being reported separately, and names what the earlier version said and why it was wrong. (5) is subtler than it looks and is right: the COMPLEX refusal fires *before* the identity block, so a complex macro never actually reaches the EXTRA path — and the item's counterfactual phrasing (*"would surface as EXTRA … rather than letting that happen"*) says precisely that.
- **The other two round-2 symptoms are fixed at their own sites.** `build-and-suite.yml:195-198` now reads *"every clause identified BY NAME against the set the tree declares"*, and the gate script's COMPLEX message no longer names "entries-equal-macros".
- **The five/22 figures reproduce.** `--self-test` prints 22 `[OK]` lines; `grep -c '^    case('` gives 22; `grep -c 'case('` gives 23 because of `def case(...)` at `:313`. Seventeen previously, so five added. `global.md:2907` and the persisted report's preamble both now name the tool's output as the authority and show both greps — that is the right fix, and it is why `:2882` is the only survivor.
- **I agree with leaving `.gitattributes` alone.** It was a completeness note on the "fourth input" question, not a request. Adding it to the path filter would trigger engine builds on a file nobody edits, and the LFS config changing is not a failure mode this project has.
- **Nothing rounds 1 and 2 passed has been disturbed.** All four gates clean — `SELF-TEST: ALL FIXTURES CORRECT`, `SUITE REPORT GATE CLEAN`, `SWEEP CLEAN`. Checklist re-derived: 15 graph rows, 6 field names, zero `strat::` crossings, 287 SIMPLE macros, seven path-filter entries with `Content/**` still among them, nothing staged, HEAD unmoved.
- **The persisted gate report is faithful and its citation resolves.** Header `TWO ROUNDS, BOTH VERDICT: BLOCK, THREE FINDINGS EACH`; two `# Gate` headings at `:1` and `:152`; two `VERDICT: BLOCK` lines at `:148` and `:238`; both my reports verbatim, including my round-1 figure of 17 where it is the historical measurement. Cited at `global.md:2891` and the file exists at that path, 25528 bytes.

**What I did not measure:** the build; `Build.bat`'s exit codes; the PowerShell `-ExecCmds` argument form; the suite run; any behaviour of `actions/checkout@v5` or a GitHub Actions runner.

VERDICT: BLOCK
