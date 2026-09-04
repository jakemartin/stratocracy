# Gate: shipped-`Difficulty` pin, RE-GATE 6 (record-deletion pass) — strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`, over base commit
`9d705ca`. Not a worktree; the absolute-path verification is inlined.

```bash
$ git -C E:/MultiAgent/Stratocracy rev-parse HEAD
9d705ca6d7b639312ce761379786237e2d381cd3
$ git -C E:/MultiAgent/Stratocracy rev-parse --abbrev-ref HEAD
master
$ git -C E:/MultiAgent/Stratocracy status --porcelain
 M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-5.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
?? Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
$ git -C E:/MultiAgent/Stratocracy diff --stat
 .../Tests/StratShippedGameModeMatchConfig.cpp      | 148 ++++++++++++++++-
 Tools/architect/state/global.md                    | 159 +++++++++++++++++-
 Tools/architect/state/tests.md                     | 185 ++++++++++++++++++++-
 3 files changed, 482 insertions(+), 10 deletions(-)
```

THREE modified files, SIX untracked gate reports, NOTHING STAGED (`git diff --cached --stat` is
empty). The brief says no code, no test, no rebuild and no suite run this round; the `.cpp` is
carried unchanged from the previous rounds and `reportCreatedOn` is still `2026.09.04-04.47.20`,
both measured below rather than accepted.

**Verdict up front: BLOCK, on TWO findings, both in `global.md`, both the same class this gate
has now blocked on four rounds running — a live universal negative about the record's own
contents, falsified by its own paragraph. The deletions the brief describes ARE real deletions
and `tests.md` is CLEAN; the replacement block in `global.md` does not obey the rule it states.**

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass — `git diff --stat` on both prints nothing |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass — empty, with a positive control showing the grep can speak |
| 3 | No vendored header in front of UHT | pass — only `StratBridge.h` matched, and it declares no reflection macro (measured) |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — two members, both `TPimplPtr`; the only `TUniquePtr<FStratBridge>` string in the tree is inside a comment explaining the `C4150` |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 census names, derived and inlined; no `.Build.cs` in the diff |
| 6 | New modules registered (unless no module object) | pass — no new module directory; four `IMPLEMENT_MODULE` files, four `.uproject` entries; `StratRules` correctly absent from both |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat` prints nothing |
| 8 | No `/Game/` literal in C++ outside the two exceptions | pass — zero ADDED by this diff; the only non-`Tests/` literal is the import commandlet |
| 9 | No widget-side arithmetic | pass — `StratUI` is not in the diff at all |
| 10 | Lane discipline | pass — `Source/*/Tests/*.cpp` + `tests.md` (test-author), `global.md` (coordinator, own file); no lane reaches another's |
| 11 | Prose blocks present | pass — the clause carries a why-first header, GDD Sec 2.11.6 quoted, acceptance ID `T-FAME-02`, measurements recorded as measurements |
| 12 | Nothing staged or committed | pass — `git diff --cached --stat` empty; HEAD is still `9d705ca` |
| — | **The record: deletions, cross-file sweep, closure** | **BLOCK — two findings, both `global.md`** |

## Findings

### FINDING 1 (BLOCK) — `global.md:101` denies restating what `global.md:59`, `:70-83` and `:96` restate

**Constraint violated:** a record file must not carry a live claim about itself that is false
against its own text. This is the fourth consecutive round on this axis, and the brief named it
as the trap to check.

`Tools/architect/state/global.md:98-101`, the replacement block:

```
therefore re-gated. **The gate reports are the files matching
`Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md`. Each is a new file leaving
its predecessors byte-identical, each carries its own `VERDICT:` line and its own findings, and
THIS BANNER RESTATES NONE OF IT -- not the verdicts, not the count, not what any of them found.**
```

The glob is defined ONE SENTENCE EARLIER and it includes the first gate report,
`2026-09-04-shipped-difficulty-pin.md`. The banner then restates that report's verdict, its
findings count, its observation count, and what it found — in three separate places, one of them
FIVE LINES ABOVE the denial.

`global.md:96` — the verdict, restated:

```
**THE FIRST GATE'S `VERDICT: PASS` DESCRIBES A TREE THIS ONE IS NO LONGER**, since a
```

`global.md:59-60` — the verdict AND the finding count AND the observation count, restated:

```
GENERAL LESSON THIS PASS PRODUCED. `strat-integration-reviewer` gated the pass **PASS with zero
findings and FOUR observations**, all four in that report under `## Observations`.
```

`global.md:70-83` — what that report found, restated in substance, all four numbered items:

```
THE OTHER THREE OBSERVATIONS ARE RECORDED HERE BECAUSE THE SENTENCE ABOVE ONCE SAID "ONE" AND
THEREFORE DROPPED THEM -- ... (2) The `autocrlf` warning on the `.cpp` is REAL and PRE-EXISTING
... (3) The liveness control compares the RENDERED STRINGS `(1)` and `(0,1)` ... (4) Obligation
(1) of the in-session clause is unfalsifiable from a checkout ...
```

Every one of those restatements is itself TRUE — I checked each against the report and they are
accurate (see the cross-file sweep below). **The false sentence is the one that denies them.**
Line `:101` tells a reader that the banner restates no verdict, no count and nothing any report
found; a reader who trusts it will not go looking for the four-observation summary sitting thirty
lines up. That is an affirmative false claim about the document the reader is holding — the same
defect in kind as the "one observation" miscount this pass was opened to repair.

It is NOT caused by this report landing and is not fixed by it: `:101` is false against the tree
as it stands right now, before this file existed.

The charitable reading — that "IT" means only the RE-gates — is not recoverable from the text.
"IT" refers to the immediately preceding subject, "The gate reports ... matching
`...-shipped-difficulty-pin*.md`", a set whose first member's verdict is quoted five lines above.
`tests.md` gets exactly this right on the identical subject and is the model:

```
Tools/architect/state/tests.md:130-133
    - **Later re-gates of this same tree are on disk and are NOT enumerated here.** Read them
      by the glob `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate*.md`
      and count them yourself.
```

That glob EXCLUDES the first report, so `tests.md` may — and does — restate the first gate's
verdict and observations without contradicting itself, and it makes no claim about the re-gates
beyond "on disk" and "not enumerated here". Both sentences survive this report's arrival.

**Owner: the `coordinator`**, in lane on its own file. The cheapest true repair is to scope the
denial to the set it was meant for and drop the universal: restate what the banner DOES restate
(the first gate's four observations, since the rename came out of one of them) and claim silence
only about the re-gates, matching `tests.md`.

### FINDING 2 (BLOCK) — `global.md:81-82` calls one distribution claim "the only" one, two sentences after making two others

**Constraint violated:** same class — a live universal negative falsified inside its own
paragraph. This is the FINDING A axis from round five, repaired correctly in `tests.md` and
re-introduced in `global.md` in a different shape.

`Tools/architect/state/global.md:77-82`, one paragraph, quoted whole so the contradiction is
visible without opening the file:

```
THE RENAME POST-DATES THE FIRST GATE, AND NO GATE REPORT WAS EDITED.
Gate reports name the OLD identifier, and that is correct and must stay: each is the record of a
gate on a tree that no longer exists, and editing one would falsify what was actually gated. The
old name also survives in record-file stamps, so a reader arriving from a report's citations can
follow it forward to the live name. **It does not occur under `Source/` at all** -- that is the
only claim about its distribution this file makes, and it is the one a checkout can re-execute.
```

Sentence 2 asserts the old name occurs in gate reports. Sentence 3 asserts — with the word
**"also"**, which makes the additive intent explicit — that it occurs in record-file stamps.
Both are claims about where the identifier occurs, i.e. about its distribution, and both are TRUE
(measured below: eight files, six gate reports plus `global.md` and `tests.md`). Sentence 4 then
says the `Source/`-absence is "the only claim about its distribution this file makes", which is
false by the two sentences immediately preceding it — and false again at `global.md:75-76`, inside
the stamp annotation:

```
STAMPED>  -- the clause's name before the rename. Retired from `Source/`, where it does not
STAMPED>  occur at all; it survives deliberately in record-file stamps and in gate reports.
```

What the sentence appears to MEAN — that the `Source/`-absence is the only CLOSED or
RE-EXECUTABLE property it claims — would be true, and would be a good sentence. What it SAYS is
false. I am reporting the second half of its own sentence, not the first: "and it is the one a
checkout can re-execute" is correct, and I verified it with a positive control.

**Owner: the `coordinator`**, in lane on its own file. Deleting the clause "that is the only claim
about its distribution this file makes" while keeping "and it is the one a checkout can
re-execute" repairs it without losing anything.

## The brief's four questions, answered against the tree

### 1. Are the deletions real deletions, and does the replacement block obey its own rule?

**The self-referential narration IS gone, and it did NOT migrate.** The prior rounds' blocked
constructions — a count of re-gates, an enumeration with the count implied, a universal about
every defect being invisible, a distribution claim about the retired name, and a false explanation
on a stamp — are absent from both files. I searched for each SHAPE rather than for the old
wording:

```bash
$ grep -n "the one surviving stamp\|four times\|survives in one place" \
    Tools/architect/state/global.md Tools/architect/state/tests.md
   (nothing)
$ grep -nE 're-gate[sd]?|regate' Tools/architect/state/global.md
   global.md:98 -- "It was therefore re-gated." and nothing else in the new banner
```

`tests.md` did not absorb it either: its only statement about the re-gates is the "NOT enumerated
here ... count them yourself" bullet quoted in Finding 1, which carries no number, no "both", and
no terminated list.

**But the replacement block does not obey its own rule.** Checked sentence by sentence for
closure, which is what Finding 1 records:

| Sentence in the block (`global.md:96-105`) | Closure? | True? |
|---|---|---|
| "THE FIRST GATE'S `VERDICT: PASS` DESCRIBES A TREE THIS ONE IS NO LONGER" | no — singular, fixed subject | **true** — measured: that file's last line is `VERDICT: PASS` |
| "It was therefore re-gated." | no | true |
| "The gate reports are the files matching `...-shipped-difficulty-pin*.md`" | no — a pattern, not a list | **true** — resolves to exactly the six gate reports of this tree and nothing else |
| "Each is a new file leaving its predecessors byte-identical" | **"each"** | **true, and stays true** — mtimes strictly monotonic, no report modified after creation; this report is a new file and touches none |
| "each carries its own `VERDICT:` line and its own findings" | **"each"** | **true, and stays true** — measured: one `^VERDICT:` line and one `## Findings` section in every one; this report has both |
| "THIS BANNER RESTATES NONE OF IT -- not the verdicts, not the count, not what any of them found" | **universal negative** | **FALSE — FINDING 1** |
| "A RECORD MUST NOT NARRATE THE PROCESS THAT IS STILL GATING IT." | none — a rule, no subject count | true, and correctly stated |
| "any closed statement about it ... is false on landing" | none | true as a rule; the block then makes three closed statements, two of which survive and one of which does not |

The two "each" clauses are the interesting case, and I want to be explicit that I am NOT reporting
them. The block's own rule says a closed statement about a growing set "is false on landing";
these two are not. They are universals over a set that grows only by files this gate creates, and
every such file satisfies both predicates, including this one. A universal that holds is not a
false claim, and I will not manufacture a finding out of a rule stated slightly more broadly than
the tree requires. The lesson sentence itself carries no closure and is fine.

### 2. Does the glob resolve, and does it survive THIS report landing?

Both measured. The glob resolves to six files today and to seven the moment this report lands;
nothing in either record file is falsified by that arrival.

```bash
$ for f in Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md; do
    printf '%-70s VERDICT_LINES=%s OLDNAME=%s NEWNAME=%s\n' "$f" \
      "$(grep -c '^VERDICT:' "$f")" \
      "$(grep -c 'ShippedGameModeAuthorsEasyDifficulty' "$f")" \
      "$(grep -c 'ShippedGameModeRunsAtEasy' "$f")"; done
...-shipped-difficulty-pin-regate-2.md VERDICT_LINES=1 OLDNAME=1 NEWNAME=6
...-shipped-difficulty-pin-regate-3.md VERDICT_LINES=1 OLDNAME=2 NEWNAME=6
...-shipped-difficulty-pin-regate-4.md VERDICT_LINES=1 OLDNAME=1 NEWNAME=1
...-shipped-difficulty-pin-regate-5.md VERDICT_LINES=1 OLDNAME=8 NEWNAME=6
...-shipped-difficulty-pin-regate.md   VERDICT_LINES=1 OLDNAME=1 NEWNAME=6
...-shipped-difficulty-pin.md          VERDICT_LINES=1 OLDNAME=4 NEWNAME=1

$ for f in Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md; do
    printf '%-70s LAST=[%s]\n' "$f" "$(tail -n 1 "$f")"; done
...-shipped-difficulty-pin-regate-2.md LAST=[VERDICT: BLOCK]
...-shipped-difficulty-pin-regate-3.md LAST=[VERDICT: BLOCK]
...-shipped-difficulty-pin-regate-4.md LAST=[VERDICT: BLOCK]
...-shipped-difficulty-pin-regate-5.md LAST=[VERDICT: BLOCK]
...-shipped-difficulty-pin-regate.md   LAST=[VERDICT: BLOCK]
...-shipped-difficulty-pin.md          LAST=[VERDICT: PASS]

$ for f in <the same glob>; do
    printf '%-70s Findings=%s Observations=%s\n' "$f" \
      "$(grep -c '^## Findings' "$f")" "$(grep -c '^## Observations' "$f")"; done
   every file: Findings=1 Observations=1

$ stat -c '%y  %n' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md | sort
2026-09-04 00:29:09.656065700 -0400  ...-shipped-difficulty-pin.md
2026-09-04 01:14:22.593592600 -0400  ...-shipped-difficulty-pin-regate.md
2026-09-04 01:31:02.942922500 -0400  ...-shipped-difficulty-pin-regate-2.md
2026-09-04 01:50:28.567706900 -0400  ...-shipped-difficulty-pin-regate-3.md
2026-09-04 02:09:16.777310200 -0400  ...-shipped-difficulty-pin-regate-4.md
2026-09-04 02:27:37.121383100 -0400  ...-shipped-difficulty-pin-regate-5.md
```

Strictly monotonic mtimes matching creation order, so "leaving its predecessors byte-identical"
holds and no earlier report was edited. Each names the OLD identifier at least once, so
`global.md:78` ("Gate reports name the OLD identifier") holds — and this report names it too, so
it survives. This report is a NEW file, matches the glob, carries one `VERDICT:` line and one
`## Findings` section, and modifies nothing. **No sentence in either record file is falsified by
this report's arrival.** The two findings above were false before it.

### 3. `tests.md` — FINDING A deleted, FINDING B's stamp verified TRUE

**FINDING A (round five: the "survives in one place" claim) is DELETED, not reworded.** What
stands in its slot claims only the `Source/`-absence, and declines to say where else it survives:

```
Tools/architect/state/tests.md:88-96
    **[STAMPED 2026-09-04: that name is retired. It is written out here in full so that a
    reader who arrives holding it can follow the trail forward to the live name. WHERE ELSE IT
    SURVIVES IS DELIBERATELY NOT STATED HERE, in a number or a list or a "the one": the set of
    files carrying it is still growing, so any such construction in this file is false by the
    next write, and three attempts to state it were falsified exactly that way. The one
    property about its distribution that a checkout can re-execute, and that stays true as that
    set grows, is that IT DOES NOT OCCUR UNDER `Source/` AT ALL -- `grep -rl` over `Source/`
    returns nothing for it while returning the live name, which is the control showing the
    search can see that tree.]**
```

That is correct, and it is the construction `global.md:81-82` should have copied. Re-executed here
with the control it names:

```bash
$ grep -rl "ShippedGameModeAuthorsEasyDifficulty" Source/
   (no output; exit 1)
$ grep -rn "ShippedGameModeRunsAtEasy" Source/          # THE CONTROL
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:77://     this file, `T-FAME-02.ShippedGameModeRunsAtEasy`, and that clause is a
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:382:  FStratShippedGameModeRunsAtEasyTest,
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:383:  "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:386:bool FStratShippedGameModeRunsAtEasyTest::RunTest(const FString& /*Parameters*/)
   (exit 0)
```

Absence measured with a positive control on the same tree: the search CAN see `Source/`, and the
old name is not there. Where it does survive — for the record, and not as a claim either file
makes in closed form — eight files, six gate reports plus the two record files:

```bash
$ grep -rln "ShippedGameModeAuthorsEasyDifficulty" Tools/ .agents/ Config/
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-2.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-3.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-4.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate-5.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin-regate.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
Tools/architect/state/global.md
Tools/architect/state/tests.md
```

**FINDING B's stamp is now TRUE, and I verified its two measurements myself rather than accepting
them.** The stamp at `tests.md:99-113` retires both the miscount and the false *"written before
that report existed"* explanation, keeping both visible:

```
Tools/architect/state/tests.md:99-106
      **[STAMPED 2026-09-04: this sentence originally read "gated the pass PASS with zero
      findings but raised **one observation**". THAT WAS FALSE BY THREE, AND IT WAS FALSE THE
      INSTANT IT WAS WRITTEN -- not merely falsified later. An earlier stamp in this slot
      explained it as *"written before that report existed to be checked against"*; THAT
      EXPLANATION IS ITSELF FALSE and is retired together with the claim it excused, because a
      stamp that retires a claim for a wrong reason teaches the wrong lesson to the next
      reader.
```

Measurement 1 — the first gate report's mtime:

```bash
$ stat -c '%n  mtime=%y' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md
Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md  mtime=2026-09-04 00:29:09.656065700 -0400
```

Measurement 2 — the committed file contains neither the miscount nor the bullet:

```bash
$ git show HEAD:Tools/architect/state/tests.md | grep -cE 'raised \*\*one observation\*\*'
0
$ git show HEAD:Tools/architect/state/tests.md | grep -c "ShippedGameModeAuthorsEasyDifficulty"
0
$ git show HEAD:Tools/architect/state/tests.md | grep -c "ShippedGameModeRunsAtEasy"
0
$ git show HEAD:Tools/architect/state/tests.md | grep -c "2026-09-04"
0
$ git show HEAD:Tools/architect/state/tests.md | wc -l
5796
```

**Two near-miss hits I chased rather than reporting, because a truncated scan is how a false
absence gets certified.** `grep -c "one observation"` on the HEAD blob returns 1, and
`grep -c "zero findings"` returns 1. Neither is the miscount:

```bash
$ git show HEAD:Tools/architect/state/tests.md | grep -n -C 1 "one observation"
2984:       neutral-capturable / non-capturable PAIR at the same owner, which is the one observation
2985-       the shortcut cannot produce.
$ git show HEAD:Tools/architect/state/tests.md | grep -n -C 1 "zero findings"
3555:    the gate returned `VERDICT: PASS` with zero findings and five observations, of which this was
3556-    the first, and it was right.
```

One is about hex ownership; the other is a 2026-08-25 entry about a different gate. **The stamp's
claim holds: the miscount and the bullet were both authored in this uncommitted pass, which began
after 00:29:09, when the report it miscounts was already on disk.** The retired explanation was
indeed false, and retiring it while keeping it visible is the right shape.

The count the stamp corrects TO is also right — the first report's `## Findings` reads `**None.**`
and its `## Observations` section carries four numbered items:

```bash
$ awk '/^## Observations/,0' Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md \
    | grep -cE '^[0-9]+\. '
4
$ awk '/^## Findings/,/^## Observations/' \
    Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin.md | head -4
## Findings

**None.** No constraint in the twelve is violated, and the adversarial premise the brief asked to
be tested holds against the tree in both halves.
```

### 4. Cross-file sweep — every live assertion in both files' newest entries

Adjudicated against each other and against the tree. Everything below is TRUE unless marked.

| Live assertion | Where | Adjudication |
|---|---|---|
| Suite is `388/388`, all Success, zero failed/notRun/warnings | `global.md:28-29` | TRUE — report read below; and it is the ONLY live count in the record set |
| `+1 added, 0 removed`, one collector over both sides | `global.md:30-32` | TRUE — reproduced independently below, +1/0 |
| Live report is `reportCreatedOn 2026.09.04-04.47.20` | `global.md:35`, `tests.md:20` | TRUE, and unchanged from the brief's claim |
| Old name ABSENT / new name PRESENT in the report by name | `global.md:32-34` | TRUE — both directions asserted below |
| Clause pins the VALUE, not the AUTHORSHIP | `global.md:109-115`, `tests.md:30-41` | TRUE — `Difficulty` defaults to `Easy` and `Easy` is enum value 0, both measured below; the assertion message says so in terms |
| No premise block, deliberately | `global.md:116-119`, `tests.md:42-47` | TRUE — the clause body carries none, and the sibling's premise would be FALSE on this field |
| Liveness control is on a DIFFERENT FIELD and is labelled so everywhere | `global.md:124-130`, `tests.md:48-58` | TRUE — the label appears in the header, in the control's own comment, and inside the assertion message |
| Renamed on a user ruling, out of observation (1) | `global.md:58-69`, `tests.md:114-129` | TRUE — the first report's observation 1 proposes `ShippedGameModeRunsAtEasy` "or similar", calls it a lane decision, names `strat-test-author` as owner |
| First gate: PASS, zero findings, FOUR observations | `global.md:59-60`, `tests.md:97-99` | TRUE — measured above |
| The other three observations, in substance | `global.md:70-83`, `tests.md:134-158` | TRUE — both summaries match the report's items 2, 3 and 4 |
| `autocrlf` warning real and pre-existing; LF in worktree AND in the HEAD blob | `global.md:72-74`, `tests.md:136-142`, `:184-188` | TRUE — measured: worktree 0 CRLF / 438 LF, HEAD blob 0 CRLF / 294 LF |
| Attribution, acting and writing, INLINE | `global.md:16-19`, `tests.md:17-19` | TRUE — `global.md` is the coordinator's own file under no exception clause; `tests.md:17` reads "`strat-test-author` (ACTING and WRITING; IN LANE ...)" and names base commit `9d705ca` |
| Route was a user choice; obligation (1) unverifiable from a checkout | `global.md:20-27`, `:76-80` | The entry SAYS it, in the required inline shape. I verify that it says it; I cannot verify that it happened, and I record which of the two I did |
| Debt: authorship pin needs a different C++ default or an override-aware reader; `strat-gameplay-engineer`'s; NOT written | `global.md:145-149`, `tests.md:55-58` | TRUE and consistent across both files; nothing in the diff implements it |
| Debt: the sweep cannot see a BARE report stamp; `strat-data-steward`'s; NOT written | `global.md:50-57` | TRUE — `_CITED_REPORT_STAMP_RE` still requires the `reportCreatedOn` token |
| Set-difference collector asymmetry (fossil `StratScoreboardParity.cpp` under `Tools/architect/evidence/`) | `tests.md:172-180` | TRUE — that path IS committed; my own difference below scopes BOTH sides to `Source/` and the file sets are now identical at 159 each |
| "the only claim about its distribution this file makes" | `global.md:81-82` | **FALSE — FINDING 2** |
| "THIS BANNER RESTATES NONE OF IT" | `global.md:101` | **FALSE — FINDING 1** |

**`global.md` is the only file with a live count or verdict.** The sweep's own tally agrees
(`suite claims found: 61 (1 live, 60 stamped)`), and no non-`global.md` record file carries a live
`N/N` suite figure. `tests.md`'s new entry carries no number at all and restates the rule
("never by a number -- the figure lives in `Tools/architect/state/global.md` and nowhere else").

## The `[STAMPED` distances — MEASURED, not reasoned

```python
import re, io
for p, LIVE in [
  (r"E:/MultiAgent/Stratocracy/Tools/architect/state/global.md", "reportCreatedOn 2026.09.04-04.47.20"),
  (r"E:/MultiAgent/Stratocracy/Tools/architect/state/tests.md",  "reportCreatedOn 2026.09.04-04.47.20"),
]:
    text = io.open(p, encoding="utf-8").read()
    print(p)
    print("occurrences of live citation:", text.count(LIVE))
    marks = [m.start() for m in re.finditer(re.escape("[STAMPED"), text)]
    print("total [STAMPED markers in file:", len(marks))
    for m in re.finditer(re.escape(LIVE), text):
        i = m.start()
        before = [x for x in marks if x < i]; after = [x for x in marks if x > i]
        print("  citation offset:", i,
              "| nearest [STAMPED BEFORE dist:", (i - before[-1]) if before else None,
              "| AFTER dist:", (after[0] - i) if after else None)
        off = 0
        for para in re.split(r"\n\s*\n", text):
            j = text.find(para, off)
            if j <= i < j + len(para):
                k = i - j
                pm = [x.start() for x in re.finditer(re.escape("[STAMPED"), para)]
                pb = [x for x in pm if x < k]; pa = [x for x in pm if x > k]
                print("    containing paragraph: offset", j, "len", len(para))
                print("    [STAMPED inside paragraph:", pm)
                print("    in-paragraph BEFORE dist:", (k - pb[-1]) if pb else None,
                      "| AFTER dist:", (pa[0] - k) if pa else None)
                break
            off = j + len(para)
```

```
E:/MultiAgent/Stratocracy/Tools/architect/state/global.md
occurrences of live citation: 1
total [STAMPED markers in file: 85
  citation offset: 2685 | nearest [STAMPED BEFORE dist: None | AFTER dist: 3985
    containing paragraph: offset 850 len 11289
    [STAMPED inside paragraph: [5820]
    in-paragraph BEFORE dist: None | AFTER dist: 3985

E:/MultiAgent/Stratocracy/Tools/architect/state/tests.md
occurrences of live citation: 1
total [STAMPED markers in file: 24
  citation offset: 1609 | nearest [STAMPED BEFORE dist: None | AFTER dist: 180
    containing paragraph: offset 1157 len 15459
    [STAMPED inside paragraph: [632, 6051, 7186]
    in-paragraph BEFORE dist: None | AFTER dist: 180
```

`global.md`: the nearest marker is **3985 characters after** the live citation and there is **none
before**, so at the sweep's 400-character window the live citation is unstamped and stays
unstamped — nearly ten times the window. `tests.md`: the nearest marker is **180 characters
after**, inside the window — but that marker is the stamp on the PRECEDING sentence's retired
`2026.09.04-04.09.26` citation, not on the live one, and no marker precedes the live citation at
all. The sweep passes both files and reports exactly one live claim, which is the correct
partition. **Neither live citation is laundered as already-retired.**

## Instruments — re-run here, INLINED so a checkout can re-execute them

A report that cites a scratchpad path is unfalsifiable, so every instrument is text.

### Instrument A — the standing architectural greps, each with a control

```bash
cd E:/MultiAgent/Stratocracy

# (1) no strat:: CALL outside the two permitted directories
grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ --include=*.cpp --include=*.h \
  | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'
#   -> empty, exit 1

# CONTROL: the same pattern inside StratBridge, proving the grep can speak
grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/StratBridge/ --include=*.cpp | head -4
#   Source/StratBridge/StratBridge.cpp:168:  strat::parseScenario(ToStd(Text), ...)
#   Source/StratBridge/StratBridge.cpp:175:  strat::validateScenario(Parsed, Units, Terrain);
#   Source/StratBridge/StratBridge.cpp:182:  if (!strat::seedFromScenario(GameState, ...))
#   Source/StratBridge/StratBridge.cpp:307:  return (Id == INDEX_NONE) ? nullptr : strat::findUiUnitView(S, Id);

# (2) no vendored header in a reflection header
for h in $(grep -rlE 'UCLASS|USTRUCT|UENUM' Source/ --include=*.h); do
  grep -qE '#include "(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' "$h" && echo "HIT: $h"
done
#   -> HIT: Source/StratBridge/StratBridge.h   ... JUDGED, not reported:
grep -nE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)' Source/StratBridge/StratBridge.h
#   -> empty, exit 1. Its UCLASS/USTRUCT/UENUM strings are all inside COMMENTS explaining why
#      the header declares none, e.g. StratBridge.h:262 "this header must declare no
#      `UCLASS`/`USTRUCT`/`UENUM`". Not a violation.

# (3) no NEW /Game/ literal
git diff -U0 -- Source/ | grep '^+.*/Game/'
#   -> empty, exit 1
grep -rn '"/Game/' Source/ --include=*.cpp --include=*.h | grep -v '/Tests/'
#   Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54 -- the import commandlet,
#   the second standing exception, untouched by this diff.

# (4) TPimplPtr, not TUniquePtr
grep -rn "TUniquePtr<[[:space:]]*FStratBridge" Source/
#   Source/StratUI/StratViewModel.h:47 -- inside a COMMENT recording the measured C4150.
grep -rn "TPimplPtr<[^>]*FStratBridge" Source/ --include=*.h
#   Source/StratPlay/StratMatchSubsystem.h:2496:  TPimplPtr<FStratBridge> Bridge;
#   Source/StratUI/StratScoreboardHUD.h:1235:   TPimplPtr<FStratBridge> OwnedBridge;

# (6) module objects vs .uproject
grep -rl "IMPLEMENT_MODULE\|IMPLEMENT_PRIMARY_GAME_MODULE" Source/ --include=*.cpp
#   Source/StratBridge/StratBridge.cpp, Source/Stratocracy/Stratocracy.cpp,
#   Source/StratPlay/StratPlay.cpp, Source/StratUI/StratUI.cpp
python -c "import json,io;d=json.load(io.open('Stratocracy.uproject',encoding='utf-8-sig'));print([m['Name'] for m in d['Modules']])"
#   ['Stratocracy', 'StratBridge', 'StratUI', 'StratPlay']
#   Source/StratRules/ has NO IMPLEMENT_MODULE and is correctly absent. No new module dir.
```

### Instrument B — the module-arrow derivation and the field census

Extracted from the reviewer checklist and EXECUTED against this tree, not read.

```bash
for f in Source/*/*.Build.cs; do
  m=$(basename "$f" .Build.cs)
  sed 's#//.*##' "$f" | tr '\n' ' ' |
  grep -oE '(Public|Private)(DependencyModuleNames|IncludePaths)\.(AddRange|Add)\([^)]*\)' |
  while read -r line; do
    vals=$(printf '%s' "$line" | grep -oE '"[A-Za-z0-9_/]+"' | tr -d '"' | paste -sd, -)
    printf '%-12s %-30s %s\n' "$m" "${line%%.Add*}" "${vals:-(none)}"
  done
done
```

**16 rows, all five modules present** (a module missing would itself be a finding):

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
StratPlay    PublicIncludePaths             StratPlay
StratRules   PublicDependencyModuleNames    Core
StratRules   PrivateDependencyModuleNames   (none)
StratRules   PublicIncludePaths             StratRules
StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
StratUI      PublicIncludePaths             StratUI
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                            AIModule,NavigationSystem,StateTreeModule,
                                            GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

Row 3 is the CONDITIONAL sixteenth row. The extraction is textual and cannot show that it is
conditional, so the file was READ for the branch:

```
Source/StratBridge/StratBridge.Build.cs:133  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
Source/StratBridge/StratBridge.Build.cs:134  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
Source/StratBridge/StratBridge.Build.cs:135  if (!bCompileVendoredRulesHere)
Source/StratBridge/StratBridge.Build.cs:137      PrivateDependencyModuleNames.Add("StratRules");
```

Guarded, monolithic-only, from the ONE expression — so the dependency half and the compile half
cannot drift apart. `StratRules` appears in `PublicDependencyModuleNames` NOWHERE and in
`PrivateDependencyModuleNames` ONLY inside the guard: the permitted shape, NOT a finding.
`StratUI` has not gained a `Stratocracy` dependency. No cycle. `StratBridge -> Stratocracy` is the
deliberate, correct edge. **No `.Build.cs` appears in this diff at all**
(`git diff --stat -- 'Source/*/*.Build.cs'` is empty), so nothing here could have moved.

The field census, because the three graph fields are themselves a typed subject list:

```bash
for f in Source/*/*.Build.cs; do sed 's#//.*##' "$f"; done |
  grep -oE '[A-Za-z]+[[:space:]]*(\.AddRange|\.Add|=)' | sed 's/[[:space:]]*$//' | sort -u
```

**TEN names, exactly the documented set, no eleventh:**

```
BRIDGE=                                <- the KNOWN FALSE POSITIVE: the tail of the string
PCHUsage =                                literal "STRAT_VENDORED_RULES_IN_BRIDGE=" inside
PrivateDefinitions.Add                    PrivateDefinitions.Add. Not a field. Not chased,
PrivateDependencyModuleNames.Add          and deliberately not sed-ed away either.
PrivateDependencyModuleNames.AddRange
PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange
ShadowVariableWarningLevel =
bCompileVendoredRulesHere =
bUseUnity =
```

`.agents/ue-project-context.md:40` and `:113` describe the conditional edge accurately, naming the
`Target.LinkType != TargetLinkType.Monolithic` expression. **No drift to report in the context
document this pass.**

### Instrument C — the banner sweep, REAL exit code (not through a pipe), plus a MUTANT

The exit code was captured into a variable BEFORE anything read the output, because `$?` after a
pipe reports the last stage and a false "exit 0" has been recorded in this project before.

```bash
cd E:/MultiAgent/Stratocracy
python Tools/architect/strat_banner_sweep.py > "$TEMP/sweep6.out" 2>&1
rc=$?                      # captured HERE, before any pipe or read
echo "REAL EXIT CODE: $rc"
cat "$TEMP/sweep6.out"
```

```
REAL EXIT CODE: 0
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 388 entries, all Success (2026.09.04-04.47.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=388, COMPLEX/_CLASS=0
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 61 (1 live, 60 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

**A CLEAN RESULT IS AN ABSENCE AND AN ABSENCE NEEDS A CONTROL.** Two were run.

(a) The shipped self-test, exit code again captured before any read:

```bash
python Tools/architect/strat_banner_sweep.py --self-test > "$TEMP/st6.out" 2>&1
rc=$?; echo "SELF-TEST REAL EXIT CODE: $rc"   # -> 0
grep -c "\[OK\]" "$TEMP/st6.out"              # -> 55
tail -1 "$TEMP/st6.out"                       # -> SELF-TEST: ALL FIXTURES CORRECT
```

59 lines, 55 `[OK]` fixtures, the four non-`[OK]` lines being section headers.

(b) **A REAL MUTANT against a disposable copy of the live banner, with a pristine control on the
same invocation shape** — because a self-test proves the fixtures, not that the binary would fire
on THIS file. `strat_banner_sweep.py` takes a positional `state_path`, so both sides were run
out-of-tree and nothing tracked was touched:

```bash
SP=<scratchpad>
cp Tools/architect/state/global.md "$SP/control_global.md"
cp Tools/architect/state/global.md "$SP/mutant/global.md"
python - <<'PY'
import io
p = "<scratchpad>/mutant/global.md"
t = io.open(p, encoding="utf-8").read()
old = "The suite is **388/388**, every entry Success"
assert t.count(old) == 1, ("anchor count", t.count(old))     # refuse on anything but 1
t = t.replace(old, "The suite is **389/389**, every entry Success")
io.open(p, "w", encoding="utf-8", newline="").write(t)
print("MUTANT WRITTEN: live count 388/388 -> 389/389 (anchor count 1)")
PY
python Tools/architect/strat_banner_sweep.py "$SP/control_global.md" > "$SP/ctl.out" 2>&1
echo "CONTROL EXIT: $?"
python Tools/architect/strat_banner_sweep.py "$SP/mutant/global.md"  > "$SP/mut.out" 2>&1
echo "MUTANT EXIT: $?"
```

```
MUTANT WRITTEN: live count 388/388 -> 389/389 (anchor count 1)
CONTROL EXIT: 0
  suite claims found: 58 (1 live, 57 stamped)
  SWEEP CLEAN -- no self-contradiction found
MUTANT EXIT: 1
  report identity: mtime=1788497240, reportCreatedOn='2026.09.04-04.47.20'
  suite claims found: 58 (1 live, 57 stamped)
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the tree's total (388): global.md:28: 389/389
  [**SUITE COUNT AGREEMENT**] live suite claim(s) disagree with the report's own succeeded count (388): global.md:28: 389/389
  SWEEP FAILED -- the record contradicts itself or the tree
```

**Exit 0 on the pristine copy, exit 1 on the one-token mutant, same invocation shape.** The CLEAN
above is a measurement and not a silence. The 58-vs-61 claim tally is the expected consequence of
sweeping one file rather than six; both sides of the mutant pair report 58, so the comparison is
like-for-like.

**The sweep's known blind spot is still open and still relevant.** `global.md:50-57` records that
`_CITED_REPORT_STAMP_RE` requires the literal `reportCreatedOn` token, so a bare
`YYYY.MM.DD-HH.MM.SS` is invisible to REPORT PROVENANCE. Still true: `global.md` carries 83
stamp-shaped tokens of which only 50 are preceded by that token. Every bare one in the NEW banner
is a retrospective mention of the retired `2026.09.04-04.09.26` — `global.md:45` ("It cited
`2026.09.04-04.09.26` -- retired by the rename re-run") and `:48` (the debt's own illustrative
example, which must be bare to BE the example) — not a live citation backing a figure. Not a
finding; recorded under Observations.

### Instrument D — the automation report, opened `utf-8-sig`

The log undercounts by exactly one every run and was NOT used.

```python
import json, io, collections
p = r"E:/MultiAgent/Stratocracy/Saved/AutomationReport/index.json"
d = json.load(io.open(p, encoding="utf-8-sig"))
for k in ("reportCreatedOn","succeeded","succeededWithWarnings","failed","notRun","totalDuration"):
    print(f"{k:24} {d.get(k)}")
tests = d.get("tests", [])
print("entry count (len(tests))", len(tests))
print("state histogram:", dict(collections.Counter(t.get("state") for t in tests)))
NEW = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy"
OLD = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty"
SIB = "Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide"
for label, n in (("NEW", NEW), ("OLD", OLD), ("SIBLING", SIB)):
    hits = [t for t in tests if t.get("fullTestPath") == n]
    print(f"{label} present BY NAME:", bool(hits), "|", n)
    for t in hits:
        print("    state:", t.get("state"), "errors:", t.get("errors"), "warnings:", t.get("warnings"))
```

```
reportCreatedOn          2026.09.04-04.47.20
succeeded                388
succeededWithWarnings    0
failed                   0
notRun                   0
totalDuration            4.527674674987793
entry count (len(tests)) 388
state histogram: {'Success': 388}
NEW present BY NAME: True | Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
    state: Success errors: 0 warnings: 0
OLD present BY NAME: False | Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsEasyDifficulty
SIBLING present BY NAME: True | Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide
    state: Success errors: 0 warnings: 0
```

`succeeded` 388 equals `len(tests)` 388 equals the histogram's only bucket. **`reportCreatedOn` is
still `2026.09.04-04.47.20`, as the brief states — measured, not accepted.** New name PRESENT and
`Success` by name; OLD name ABSENT by name.

The artifact is also pinned in time, independently of the sweep's own identity check:

```
report mtime : 1788497240.21  (2026-09-04 00:47:20 local)
newest source: 1788497056.05  (2026-09-04 00:44:16 local)
  path: Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
REPORT NEWER THAN SOURCE: True  delta(s): 184.2
```

The report POSTDATES the last source edit by 184 seconds, so it describes THIS tree. Nothing in
this round's diff touched `Source/`, so it is still the right run to cite.

### Instrument E — my own macro-based multiline set difference, ONE collector, SYMMETRIC file set

A single-line grep on the macro returns zero because the macro wraps across lines, so the pattern
is `re.S` and spans them. **The symmetric file-set rule is stated in the code and enforced on both
sides**: a path counts iff it starts with `Source/` and ends `.cpp`. That is precisely what closes
the collector asymmetry `tests.md:172-180` reports — the committed fossil at
`Tools/architect/evidence/02-scoreboard-widget/generated/Source/StratUI/Tests/StratScoreboardParity.cpp`
is reachable by a `git ls-tree` path filter but not by an `os.walk` of `Source/`, and the
`Source/` prefix test excludes it from BOTH sides.

```python
import re, subprocess, io, os
REPO = r"E:/MultiAgent/Stratocracy"
MACRO = re.compile(
    r"IMPLEMENT_(?:SIMPLE|COMPLEX)_AUTOMATION_TEST\s*\(\s*[A-Za-z_][A-Za-z0-9_]*\s*,\s*"
    r'(?:TEXT\s*\(\s*)?"([^"]+)"', re.S)

def eligible(p):                       # THE SYMMETRIC FILE-SET RULE, applied to BOTH sides
    return p.startswith("Source/") and p.endswith(".cpp")

def collect(get_text, paths):          # ONE collector, used on BOTH sides
    out = set()
    for p in paths:
        t = get_text(p)
        if t is None: continue
        out |= set(MACRO.findall(t))
    return out

def git(*a):
    return subprocess.run(["git","-C",REPO,*a], capture_output=True, text=True, check=True).stdout

head_paths = [p for p in git("ls-tree","-r","--name-only","HEAD").splitlines() if eligible(p)]
def head_text(p):
    r = subprocess.run(["git","-C",REPO,"show",f"HEAD:{p}"], capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None

wt_paths = []
for root, _, files in os.walk(os.path.join(REPO, "Source")):
    for f in files:
        rp = os.path.relpath(os.path.join(root, f), REPO).replace(os.sep, "/")
        if eligible(rp): wt_paths.append(rp)
def wt_text(p):
    try: return io.open(os.path.join(REPO, p), encoding="utf-8", errors="replace").read()
    except OSError: return None

print("HEAD side .cpp files under Source/    :", len(head_paths))
print("WORKTREE side .cpp files under Source/:", len(wt_paths))
print("file-set symmetric (same paths?)      :", set(head_paths) == set(wt_paths))
print("  only in HEAD:", sorted(set(head_paths) - set(wt_paths)))
print("  only in WT  :", sorted(set(wt_paths) - set(head_paths)))
H = collect(head_text, head_paths); W = collect(wt_text, wt_paths)
print("HEAD clause count     :", len(H))
print("WORKTREE clause count :", len(W))
print("ADDED   (+):", len(W - H));  [print("   +", n) for n in sorted(W - H)]
print("REMOVED (-):", len(H - W));  [print("   -", n) for n in sorted(H - W)]
print("collector non-vacuity (>0 both sides):", len(H) > 0 and len(W) > 0)
```

```
HEAD side .cpp files under Source/    : 159
WORKTREE side .cpp files under Source/: 159
file-set symmetric (same paths?)      : True
  only in HEAD: []
  only in WT  : []
HEAD clause count     : 387
WORKTREE clause count : 388
ADDED   (+): 1
   + Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy
REMOVED (-): 0
collector non-vacuity (>0 both sides): True
```

**+1 added, 0 removed, 387 -> 388, the added name is the NEW one, and the two sides enumerate the
IDENTICAL 159 files** — so the difference measures the trees and not two collectors. This agrees
with the report (`succeeded` 388), with the sweep's independent macro census
(`IMPLEMENT_SIMPLE_AUTOMATION_TEST=388`) and with `global.md:28`, by four routes that do not share
an implementation.

### Instrument F — the clause's load-bearing premise, re-verified rather than inherited

```bash
$ grep -n "EStratDifficulty Difficulty" Source/StratPlay/StratMatchSubsystem.h
436:  EStratDifficulty Difficulty = EStratDifficulty::Easy;
$ grep -n -A 8 "enum class EStratDifficulty" Source/StratPlay/StratMatchSubsystem.h
212:enum class EStratDifficulty : uint8
213-{
215-  Easy   UMETA(DisplayName = "Easy"),
218-  Normal UMETA(DisplayName = "Normal"),
220-  /** Sec 2.9: the player opens 100 below the scenario's configured value. */
```

No explicit initialisers, so `Easy` is 0 and IS the C++ default. **Both halves of the premise
hold**, so every "pins the VALUE, not the AUTHORSHIP" statement in both record files is sound, and
the deliberate ABSENCE of a premise block is correct rather than an omission. The clause body at
`Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp:381-437` matches its description
exactly: it resolves the shipped CDO, asserts `ShippedTier == EStratDifficulty::Easy`, carries the
sentence "Do not cite this clause as evidence that a designer set the tier." INSIDE the assertion
message, and runs the `AiSides` liveness control over the AI-vs-AI package with "LIVENESS CONTROL,
ON A DIFFERENT FIELD" in its own message. Every failure path goes through `ResolveCdoOrFail` and
returns false only after the test is already red; there is no skip and no warn-and-continue.

### Instrument G — line endings on the changed `.cpp`

`grep` cannot see CR on this box, so this is a byte count and not a grep.

```python
import io, subprocess
p = "Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp"
wt = io.open("E:/MultiAgent/Stratocracy/" + p, "rb").read()
hd = subprocess.run(["git", "-C", "E:/MultiAgent/Stratocracy", "show", "HEAD:" + p],
                    capture_output=True).stdout
for n, b in (("worktree", wt), ("HEAD blob", hd)):
    print(n, "CRLF=", b.count(b"\r\n"), " bare LF=", b.count(b"\n") - b.count(b"\r\n"))
```

```
worktree   CRLF= 0   bare LF= 438
HEAD blob  CRLF= 0   bare LF= 294
```

LF on both sides, zero CRLF: the `autocrlf` warning is real, pre-existing and repo-wide, and this
pass preserved the file's line endings. Both record files describe it correctly. Watch it at
staging time.

## Observations

Non-gating.

1. **`tests.md` is the model for the sentence `global.md` got wrong, and the asymmetry is worth
   naming.** Both files faced the identical problem — how to point at a growing set of gate
   reports without closing over it. `tests.md:130-133` solved it ("NOT enumerated here ... count
   them yourself", over a glob that EXCLUDES the first report, leaving it free to restate that
   report's contents elsewhere). `global.md:98-101` solved half of it — the glob is right and both
   "each" predicates hold — and then added a universal negative the same banner does not satisfy.
   The repair for Finding 1 is largely a copy of what already sits in the sibling file.

2. **Two bare report stamps sit in the new banner, both ungated by the sweep, and neither is a
   defect.** `global.md:45` and `:48` both write `2026.09.04-04.09.26` without the
   `reportCreatedOn` token. The first describes it as "retired by the rename re-run"; the second
   IS the debt's illustrative example and must be bare to be the example. Neither backs a live
   figure, so neither is the defect the banner's own rule ("every citation in this file must carry
   the `reportCreatedOn` token or it is ungated") exists to prevent. But a reader should know the
   rule is stated more absolutely than the banner follows it, and that the underlying instrument
   debt is still open in `strat-data-steward`'s lane.

3. **`global.md:115` names `BP_StratGameMode_AiVsAi` where `tests.md:53` names
   `BP_StratGameMode_AiVsAi_C`.** Package versus generated class; both are accurate descriptions
   of `kAiVsAiGameModeClassPath`'s subject. Not a discrepancy — recorded so a future reader does
   not chase it as one.

4. **The `strat::` grep produced four legal hits, all in `Source/StratBridge/StratBridge.cpp`.**
   Reported above as the CONTROL for the absence, not as candidates. This check yields legal hits
   every pass and forwarding them would train the reader to skim.

5. **The debts both files carry as open are still open, and nothing in this diff pretends
   otherwise:** the authorship pin (`strat-gameplay-engineer`, not written) and the sweep's
   bare-stamp blind spot (`strat-data-steward`, not written). Both are correctly attributed and
   neither is claimed closed.

6. **Nothing else in the twelve moved.** No `Source/` path appears in this round's change, so the
   `.cpp` is byte-identical to what rounds four and five gated; items 1-11 were re-verified rather
   than inherited, and all pass. **Both findings are entirely inside
   `Tools/architect/state/global.md`, and both are one-sentence edits by the `coordinator`.**

VERDICT: BLOCK
