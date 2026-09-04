# Gate report -- AiSides prose repair and the shipped-GameMode asset pin

**Subject:** the UNCOMMITTED working tree over base commit `0b17685` on `master`, main tree
`E:/MultiAgent/Stratocracy`. Nine paths: eight tracked-modified, one untracked-new, none staged.

**Gate run:** 2026-09-03 (local). **Reviewer:** `strat-integration-reviewer`.
**Persisted by:** the `coordinator`, VERBATIM below the horizontal rule -- this seat wrote no
part of the reviewer's report and did not edit its wording. Persisted because a gate whose text
cannot be produced later is unfalsifiable, which this record already carries as an open gap.

**Verdict:** `VERDICT: PASS`, zero findings -- none in code, assets or the module graph
(audited FIRST per the standing priority ruling, and reported as its own section), none in the
record. Five coordinator-made claims were re-measured by the reviewer and all five confirmed.

**One correction the gate made to the brief it was given:** the brief said EIGHT files; the tree
has NINE paths. The `coordinator` had edited `global.md` itself after fixing that count and did
not re-derive it. The reviewer's figure is the correct one.

**A NOTE ON THE INSTRUMENT, because the report cites a path a checkout does not contain.**
The reviewer verified the comment-only claim with a string-aware C++ comment stripper rather
than a leader-keyed grep, which can be fooled by a leader inside a block-comment body or inside
a `TEXT("...")` literal. It wrote that script to a SESSION SCRATCHPAD, which is deleted with the
session, so the report's citation of it would rot immediately. **The script is reproduced in
full at the foot of this file** so the check stays re-executable from a checkout. It is the
reviewer's, unmodified.

**TRANSPORT NOTE, so "verbatim" is an honest word.** The reviewer's report reached this seat
through a channel that HTML-escapes angle brackets, so `&gt;` and `&lt;` arrived in place of
`>` and `<` -- which would have corrupted every `RETRACTED>` and every `TArray<int32>` in it.
Those entities were decoded back to the characters the reviewer wrote. **That is the ONLY
change made to its text**; no wording, figure, ordering or verdict was touched.

---

# Gate: uncommitted pass over `0b17685` (AiSides ruling / T-FAME-02 asset pin) — strat-integration-reviewer

## Scope

Working-tree diff against `HEAD = 0b17685` on branch `master`, tree `E:/MultiAgent/Stratocracy`. **Nine paths**: seven tracked-modified, one untracked-new, zero staged. The claimed eight-file inventory was one short — `git status --porcelain` returns nine paths (the untracked `Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` was described in the brief but not counted in the "eight files" total). No `Content/`, `Config/`, `Data/` or `Source/StratRules/` path appears.

**Code, assets and the module graph were audited first and in full, per the standing priority ruling. That pass is Findings section A below, and it is empty.**

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` and `git status --porcelain` both empty on those paths. Manifests derived, not quoted: `rulesCommit 96d93ea…`, `dataCommit c2f5860…` |
| 2 | No `strat::` call outside `StratBridge/`, `StratRules/` | pass — subject returns **0**; control (same pattern inside `StratBridge/`) returns **143**, so the instrument speaks. 48 files carry `strat::`+uppercase *type* names outside those dirs; all legal, none reported |
| 3 | No vendored header in front of UHT | pass — scanned every `.h` declaring `UCLASS`/`USTRUCT`/`UENUM` for an include of `StratBridge.h`/`Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h`; zero violations |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — zero `TUniquePtr<FStratBridge>` declarations; the only textual hit is prose in `StratViewModel.h` recording the `C4150` measurement. Live members at `StratMatchSubsystem.h` `Bridge` and `StratScoreboardHUD.h` `OwnedBridge` are both `TPimplPtr` |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. First command printed **16 rows** including the conditional `StratBridge PrivateDependencyModuleNames StratRules`; field census printed **10 names** (incl. the known `BRIDGE=` false positive, not chased). No `.Build.cs` is in the diff. Guard branch read directly and confirmed conditional |
| 6 | New modules registered — unless no module object | pass (vacuous, and stated as such) — no new `Source/<Module>/` directory; no `IMPLEMENT_MODULE` added or removed. `StratRules` correctly still absent from the `Modules` array |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` and `git status --porcelain` both empty on that path |
| 8 | No `/Game/` literal in gameplay C++ | pass — 292 hits tree-wide; all non-test, non-commandlet hits are prose inside `//` or `*` comments stating the rule. The three real literals are all in the new automation fixture (lawful standing exception) |
| 9 | No widget-side arithmetic | pass (by no change) — zero `StratUI` files in the diff; nothing in this pass can introduce it |
| 10 | Lane discipline | pass — production edits confined to `Source/StratPlay/*.cpp/.h`, test edits to `Source/StratPlay/Tests/`, record edits each to its own lane's file. No test code outside `Tests/`, no production code inside it, no asset touched |
| 11 | Prose blocks present | pass — the new file opens with an 80-line why-first block citing GDD §2.7/§2.9/§2.11.6 and `T-FAME-02`, recording measurements as measurements and naming four things it deliberately does not pin. No new `.Build.cs` entry to check |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` empty; `git status --porcelain` shows unstaged-modified and untracked only; `HEAD` is still `0b17685` |

## Findings

### A. Code, assets and module graph

**None.** Zero findings in `Source/`, zero in `Content/`, zero in the module graph.

### B. Record and prose

**None.**

## Verification of your five claims — I re-measured each; I agree with all five

**Claim 1 — comment-only in the three production sources. CONFIRMED, and by a stronger instrument than a leader grep.** I did not filter on `//`/`*` leaders, because as you note that is foolable. I ran a character-level C++ comment stripper (a state machine that tracks string and char literals, so a leader inside a block-comment body or inside a `TEXT("…")` cannot fool it) over the `HEAD` blob and the working copy of each file and compared the surviving code lines:

- `StratMatchSubsystem.h` — 209 / 209, **identical**
- `StratMatchSubsystem.cpp` — 1403 / 1403, **identical**
- `StratGameMode.cpp` — 81 / 81, **identical**
- `StratAiMatchClauses.cpp` — 835 / 835, **identical**
- `StratDifficultyHandicapMatchClauses.cpp` — 468 / 469, **one difference**, and it is exactly the one `TestEqual` message string you declared. The compared operands (`StratHandicappedSide(HotSeat)`, `INDEX_NONE`) are byte-identical; only the message moved, and it grew from one string literal to a two-part concatenation, which is why the line count moved by one.

The instrument is not inert: it found that one real difference. Script at `C:\Users\me\AppData\Local\Temp\claude\E--MultiAgent-Stratocracy\db5a8bc8-22d2-47c0-9e7f-19de1053c837\scratchpad\strip.py`, re-executable against any two revisions.

**Claim 2 — the `RETRACTED>` convention. CONFIRMED, with the control, and the tree-wide question answered.** Running the declared anchored filter `grep -vE '^[[:space:]]*(//|\*)[[:space:]]*RETRACTED> '` over every tracked source file in this repo (worktrees under `.claude/worktrees/` excluded — they are separate repository boundaries), **every unanchored `RETRACTED>` hit in `Source/` is a line that merely MENTIONS the token**: the convention declaration itself in `StratSelectionMachineParity.cpp`, its restatement in `StratGuidanceInputGates.cpp`, and narrative mentions in `StratUnitActor.h`, `StratGuidedOpeningVisuals.cpp` and `StratProductionMenuSeam.cpp`. **None of the four edited source files appears in that list** — all 47 markers in `StratMatchSubsystem.h`, all 10 in `.cpp`, all 5 in `StratAiMatchClauses.cpp` and all 3 in `StratDifficultyHandicapMatchClauses.cpp` are anchored. The engineer's own before/after figures (`.h` 45/43/2 → 47/47/0; `.cpp` 10/9/1 → 10/10/0) reproduce.

**The control passes and it matters.** The same anchored filter over the same subject-bearing phrases still returns live claims — `StratMatchSubsystem.h:422`, `:695`, `StratMatchSubsystem.cpp:2751`, `:2757`, `:2775`, `StratGameMode.cpp:168`, `:171`, and four in the two test files. So the zero-survivor result is evidence, not a broken grep. Every surviving live line asserts the *corrected* direction; **no live line anywhere in `Source/` still asserts the old conflation.**

**Tree-wide consistency: it is consistent in `Source/`, and it is NOT tree-wide.** `Tools/architect/state/*.md` uses a bare, indented `RETRACTED>` with no comment leader, so the declared C++ anchor `(//|\*)` does not match there. That is not new drift and it is not this pass's doing — `tests.md` already records it in terms (*"TWO `RETRACTED>` CONVENTIONS NOW COEXIST"*). It is non-gating, but see Observations: it means a reader who applies the declared filter to a `.md` record file gets an unsound answer.

**Claim 3 — the new clause. CONFIRMED on all four sub-questions, and I judge it a well-built gate.**

- **(a) No path skips or passes vacuously.** `ResolveCdoOrFail` has four early exits — package load, native-class resolution, `IsChildOf(AStratGameMode)`, null CDO — and **each records a failure through `TestNotNull`/`TestTrue` before returning `nullptr`**, with `RunTest` then returning `false`. There is no `AddWarning`-and-continue and no `AddInfo`-and-return-true anywhere in the file. The premise check on the C++ default is first and fatal.
- **(b) The `BP_StratGameMode_AiVsAi` discrimination is a real assertion, not a remark** — two of them: `TestEqual(ControlAiSides.Num(), 2)` and `TestNotEqual(Describe(Shipped), Describe(Control))`. Both fail the clause red. I confirmed **both assets exist**: `Content/StratPlay/BP_StratGameMode.uasset` and `BP_StratGameMode_AiVsAi.uasset`.
- **(c) A reader silently returning the C++ default CANNOT pass.** I verified the premise against the actual declaration: `TArray<int32> AiSides;` in `StratMatchSubsystem.h` has **no initialiser and therefore initialises empty**, while the expectation is `Num() == 1`. The clause asserts that premise explicitly and fatally, so the day someone gives `AiSides` a non-empty C++ default the clause goes red rather than quietly ceasing to mean anything. `ViewingSide = 0` genuinely cannot discriminate — and the clause says so, in its own failure message, rather than in a comment a reader of the output would never see. That is the right call.
- **(d) The `/Game/` literals do not reach a non-test path.** The only three in the file are the two class paths and one in the comment; the tree-wide scan (check 8) finds no `/Game/` literal in any non-test, non-commandlet C++.

**Claim 4 — the banner. CONFIRMED, exported report and macro census both.** I read `Saved/AutomationReport/index.json` with `utf-8-sig` (it is BOM'd; the log undercounts and I did not use it): **succeeded 387, failed 0, notRun 0, `reportCreatedOn 2026.09.04-03.04.52`** — agreeing with `global.md`'s live figure and its citation exactly. Macro census over `Source/` including the untracked file: **387**. Report entries: **387**. **The two name sets are exactly equal in both directions** (`tree − report = ∅`, `report − tree = ∅`), and `ShippedGameModeAuthorsOneAiSide` is present in the report by name, so the new clause was built, ran, and passed. Independent set-difference against `HEAD`: **386 → 387, exactly one added, zero removed, zero renamed**, the added name being the one claimed.

**The 400-character window holds with wide margin.** Measured from the live citation's byte offset in `global.md`, the nearest `[STAMPED`/`STAMPED>`/`RETRACTED>` marker in either direction is **1359 characters away** — against `_PROVENANCE_CITATION_WINDOW = 400`. No adjacent bracket can excuse the banner's own certifying citation. `strat_banner_sweep.py` prints `SWEEP CLEAN` and — measured without a pipe, because `$?` after a pipe is a known phantom on this project — **exits 0**.

**Claim 5 — attribution. CONFIRMED; no lane wrote another lane's file, so far as a checkout can tell.** `engine.md`'s topmost entry is `strat-gameplay-engineer (ACTING and WRITING; IN LANE … base commit 0b17685, UNCOMMITTED)`; `tests.md`'s is `strat-test-author (ACTING and WRITING; IN LANE -- Source/*/Tests/ only, plus this file …)`; `global.md` names the `coordinator` writing its own file *"IN LANE AND UNDER NO EXCEPTION CLAUSE"* and disclaims writing code or clauses. Every entry names its base commit inline. **Neither the transcription clause nor the in-session clause is invoked, so neither clause's unfalsifiable limb is load-bearing in this pass** — which is the cleanest possible answer to the attribution question.

## Your three suspicions

**The `StratGameMode.cpp` rewrite carrying no `RETRACTED>` marker — I second-guessed it and I agree with your judgement, on the convention's own text.** The withdrawn words are *"which is the default and is every match before phase C"*, a subordinate clause mid-sentence. The convention's partial-line paragraph offers exactly two lawful handlings: *"Break the sentence so the quoted words start their own line, **or do not quote them**."* Rewriting in place is the second option, stated by the convention itself. `engine.md` records the site and what it read. **Not a finding.** It does carry a small real cost, which is in Observations.

**The provenance of the `AiSides=(1)` measurement — the record is honest about it, and this pass materially improved the situation.** Every place that carries the value names whose measurement it is and that it came off the live editor: `StratMatchSubsystem.h`'s AI-block preamble says *"THE MEASUREMENT BEHIND THAT RULING IS NOT THIS FILE'S AND IS NOT RESTATED AS ITS OWN"*; `engine.md` says *"I did not read the asset and this entry claims no such reading"*; `tests.md` attributes it to the `coordinator` with its three controls. I found **no site that reads as though a checkout could re-execute the live-editor reading.** And your premise has now partly expired in the right direction: as of this pass **one instrument in the tree does read the `.uasset`** — the new clause — so the *value* is re-executable from a checkout even though the original *reading* is not. That is precisely the gap the clause was built to close, and it closes it.

**The `AiSides` debt cannot be misread as discharged.** Its bullet carries, in sequence: `[ANSWERED … THIS QUESTION IS CLOSED. THE DEBT IS NOT, AND MUST NOT BE READ AS CLOSED]`, then the `DISCHARGED WHEN` conjunction naming three sites, then `[HALF-MET 2026-09-03, AND HALF-MET IS NOT MET. THIS DEBT REMAINS OPEN AND IS DELIBERATELY NOT STAMPED.]`, then the arithmetic — two of the three sites repaired, `IsAiPlaybackRunning`'s comment untouched because it is an `AiPlaybackStepSeconds` site the user's scope ruling excluded. **Confirmed open, and unusually hard to misread.**

## Observations — non-gating

- **The `RETRACTED>` convention is consistent within `Source/` and split across the tree.** `Tools/architect/state/*.md` uses a bare indented marker with no comment leader, so the declared anchor `^[[:space:]]*(//|\*)[[:space:]]*RETRACTED> ` matches nothing there and a sweep that applies it to a record file is silently unsound in the permissive direction. Already recorded at `tests.md` (*"TWO `RETRACTED>` CONVENTIONS NOW COEXIST"*), not caused by this pass, and not something this pass was scoped to fix. Worth a future pass declaring the markdown form explicitly, since the declaration currently enumerates only `//` and `*`.
- **The `StratGameMode.cpp` in-place rewrite is lawful but leaves those withdrawn words ungreppable at their own site.** The convention permits it; the cost is that a future claim-shape sweep looking for *"which is the default"* will find the corrected text and no record that the site ever said otherwise. `engine.md` carries it, so the fact is not lost — it is just not local. No action needed.
- **The falsifiability evidence for the new clause rests on the lane's self-report and is not re-executable from this checkout.** `tests.md` describes a mutant run whose failure message printed the read value `(1)` while asserting a mutated `0` — good evidence, and exactly the right shape. I did not run it: builds and suites are `strat-test-author`'s to report, not mine to re-execute. I state this so an unrun check does not read here as a clean one. What I *did* independently verify is that the clause is present in the exported report by name and passed, which establishes it is not inert-by-absence.
- **`Source/StratPlay/Tests/StratAiMatchClauses.cpp` triggers a git line-ending warning** (*"LF will be replaced by CRLF the next time Git touches it"*). Committed bytes will be LF either way under `.gitattributes`, so this is cosmetic — but it is the shape that produces phantom CRLF drift later; diagnose with `git diff --ignore-cr-at-eol --numstat` if it resurfaces.
- **The brief's file count was eight; the tree has nine paths.** The untracked new test file was described but not counted. Cosmetic, and named only because a count in a brief has been wrong on this project before and the habit of checking it is cheap.

VERDICT: PASS


---

## Appendix -- the comment stripper the report cites, reproduced verbatim

The reviewer's instrument, copied unmodified from the session scratchpad it was written to.
It tracks string and char literals, so a `//` or `*` inside a body or a literal cannot fool it.
Usage is as the reviewer ran it: strip the `HEAD` blob and the working copy of a file and
compare surviving code lines. **This is the instrument that found the ONE real non-comment
change in the pass -- the `TestEqual` message string -- so it is demonstrably not inert.**

```python
import sys,subprocess,re
BS=chr(92)
def strip(src):
    out=[];i=0;n=len(src);state='code'
    while i<n:
        c=src[i]
        if state=='code':
            if src.startswith('//',i): state='line';i+=2
            elif src.startswith('/*',i): state='block';i+=2
            elif c=='"': out.append(c);state='str';i+=1
            elif c=="'": out.append(c);state='chr';i+=1
            else: out.append(c);i+=1
        elif state=='line':
            if c=='\n': out.append(c);state='code'
            i+=1
        elif state=='block':
            if src.startswith('*/',i): state='code';i+=2
            else:
                if c=='\n': out.append(c)
                i+=1
        elif state=='str':
            out.append(c)
            if c==BS: out.append(src[i+1]);i+=2;continue
            if c=='"': state='code'
            i+=1
        elif state=='chr':
            out.append(c)
            if c==BS: out.append(src[i+1]);i+=2;continue
            if c=="'": state='code'
            i+=1
    return re.sub(r'[ \t]+',' ',''.join(out))
for p in sys.argv[1:]:
    head=subprocess.run(['git','show','HEAD:'+p],capture_output=True).stdout.decode('utf-8',errors='replace')
    work=open(p,encoding='utf-8',errors='replace').read()
    a=[l.rstrip() for l in strip(head).splitlines() if l.strip()]
    b=[l.rstrip() for l in strip(work).splitlines() if l.strip()]
    print(p+": HEAD code lines="+str(len(a))+" WORK="+str(len(b))+" IDENTICAL="+str(a==b))
    if a!=b:
        import difflib
        for d in difflib.unified_diff(a,b,lineterm='',n=1): print("   ",d)
```
