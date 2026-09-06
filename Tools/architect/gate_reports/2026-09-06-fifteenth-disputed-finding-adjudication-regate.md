# Gate: fifteenth pass, DISPUTED-FINDING ADJUDICATION + full RE-GATE - strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base commit `92d78c1`.
`git status --porcelain` is the authority and returned exactly four modified files plus two
untracked gate reports:

```
 M Source/StratPlay/StratCameraPawn.h
 M Source/StratPlay/StratGuidedOpening.h
 M Source/StratUI/StratGuidanceWidget.h
 M Tools/architect/state/engine.md
?? Tools/architect/gate_reports/2026-09-06-fourteenth-stale-claim-gate.md
?? Tools/architect/gate_reports/2026-09-06-fourteenth-stale-claim-regate.md
```

331 insertions / 2 deletions. No file has moved since the fourteenth re-gate, as the brief
states and as the diffstat confirms. All twelve checks were re-run against this tree rather than
inherited from the prior round.

## Adjudication of the disputed finding - THE FINDING STANDS, on bytes the dispute did not read

The dispute has two halves. **Half (b) is correct and I am correcting the prior report for it.
Half (a) is false, and it is false because it quotes a different passage from the one the finding
is about.** The finding survives on half (a).

### (a) The finding is not about `engine.md:31-35`. It is about `engine.md:159-161` and `:167-169`.

The dispute quotes the retract-and-quote bullet at lines 31-35 and argues, correctly, that it is
attributed, quoted and corrected. **It is.** That bullet is clean and I do not fault it:

> - **THE BLOCKED CLAIM, AND IT REPRODUCES.** `StratGuidedOpening.h` **said** ... "returns 0 on
>   it, against 4 on each of ..." **Re-measured on the package bytes, that token returns 4 on
>   `WBP_DirectiveStrip` and 0 on the other three.**

But the prior report's finding names its site explicitly - "in the bullet beginning THE
CORRECTION IS DELIBERATELY NARROWER THAN" - and that string does not occur anywhere near
line 31. Measured, a grep for "on each of" over `Tools/architect/state/engine.md` returns
**three** lines: **28**, **32** and **160**. Lines 28 and 32 are the rule bullet and the
retraction. Line 160 is a different entry, in the "The shape-6 sweep" section, and reads:

> `Content/UI/WBP_PreMatchBriefing.uasset` is tracked, is real bytes ... **The correction is
> deliberately NARROWER than "the premise is false", because the narrower claim is what carries
> the bullet:** the asset derives from plain `UserWidget`
> (`/Script/StratUI.StratGuidanceWidget` returns 0 on it against **4** on each of
> `WBP_DirectiveStrip`, `WBP_Scoreboard`, `WBP_Options` and `WBP_CommandBar` as positive
> controls), nothing in `Source/` names it ...

There is no reporting verb, no quotation mark, no attribution and no correction. It is a
parenthetical stated in the present tense as this record's own measurement. Its sibling at
lines 167-169 inherits the same falsehood by back-reference:

> `WBP_DirectiveStrip.uasset` carries `/Script/StratUI.StratGuidanceWidget` **4** times, the same
> figure as **the three independently-known C++-parented controls above**, against **0** on
> `WBP_PreMatchBriefing`

"the three ... controls above" resolves to the three assets named at line 161, whose figure for
that token is **0**, not 4. The token table re-measured last round on the package bytes:

```
token                                    DirectiveStrip Scoreboard Options CommandBar PreMatchBriefing
/Script/StratUI.StratGuidanceWidget            4            0         0         0            0
/Script/StratUI.StratScoreboardWidget          0            4         0         0            0
/Script/StratUI.StratOptionsWidget             0            0         4         0            0
/Script/StratUI.StratCommandBarWidget          0            0         0         4            0
/Script/UMG.UserWidget                         0            0         0         0            4
/Script/StratUI.StratFabricatedNopeWidget      0            0         0         0            0
```

I did not re-derive that table this round and say so plainly; it is carried from the fourteenth
re-gate. It does not need re-deriving to settle this finding, because the pass's own top entry
states the same figures independently at `engine.md:34-37` - "that token returns **4 on
`WBP_DirectiveStrip` and 0 on the other three**" - and that sentence and line 161 cannot both be
true.

**And it is not history.** `git diff -U0` on `Tools/architect/state/engine.md` returns exactly one
hunk, `@@ -17,0 +18,201 @@` - a pure insertion. Lines 160 and 168 are inside it. Both the rule
and its violation are uncommitted bytes in this same diff. The append-only stamp-not-strike
convention protects committed entries; neither of these is one.

**The dispute's own diagnosis is right about the shape and lands on the other side.** A grep for
retracted wording does find the retraction, and reading the site rather than the hit count is
what separates them. That is exactly what distinguishes line 32 from line 160: I read both sites,
and they are not the same site.

### (b) `PER SUBJECT` appears once in `engine.md`, at line 26. The prior report's parenthetical was FALSE.

Measured this round: count of "PER SUBJECT" -> **1**; located at line **26**; fabricated control
"PER ZZOBJECT" -> **0**, so the counter is selective. The prior report wrote that the string
"appears once in each of the two repaired headers and **zero times** anywhere in `engine.md`" -
and contradicted itself two lines earlier by quoting that very phrase out of `engine.md`'s own
top entry. **The dispute is right, the prior report is wrong, and this is the correction.**

It does not rescue the finding, and it slightly strengthens it. The rule sits at line 26; its
unmarked violation sits at line 160. That is **134 lines**, which is what "roughly 130 lines
below" was reaching for and got right by the wrong route.

### What I am NOT ruling

I am not ruling that a quoted retraction needs its own inline stamp. The dispute offers that as
the reading it would accept a BLOCK on; I decline it. Lines 31-35 are correct as written and no
repair is owed there. **The repair owed is at two sites only: the parenthetical at
`engine.md:160-161` and the "same figure as the three ... controls above" clause at `:167-169`.**

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass - `git diff --stat -- Source/StratRules Data` empty; control below |
| 2 | No `strat::` call outside StratBridge/StratRules | pass - 137 call-shaped hits, all `StratBridge`, 0 elsewhere; regex control below |
| 3 | No vendored header in front of UHT | pass - 59 UHT-declaring headers, 0 violations; detector control fired |
| 4 | `TPimplPtr` not `TUniquePtr` | pass - both real members `TPimplPtr`; control below |
| 5 | Module arrows unchanged and correct | pass - 16 rows / 5 modules / 10 field names; 0 `.Build.cs` in the diff |
| 6 | New modules registered | pass - no new modules; `StratRules` 0 `IMPLEMENT_MODULE` and correctly absent from the `.uproject` |
| 7 | `Source/Stratocracy/` untouched | pass - empty; control below |
| 8 | No `/Game/` literal in gameplay C++ | pass - 248 stripped-code hits, 247 in `Tests/`, 1 in the import commandlet |
| 9 | No widget-side arithmetic | pass - the `Source/StratUI/` change is comment-only, proved with a three-way control |
| 10 | Lane discipline | pass - three `.h` outside `Tests/` plus the engineer's own `engine.md`; no `Tests/`, `Content/` or `Config/` file touched |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** - finding 1 |
| 12 | Nothing staged or committed | pass - `git diff --cached --stat` empty; control below |

### Controls for every check that passes by returning nothing

**Checks 1, 7, 12.** The same `git diff --stat -- <path>` that printed nothing for
`Source/StratRules Data` and for `Source/Stratocracy` printed **two** files for
`Source/StratPlay` and **one** for `Source/StratUI`. `git diff --cached --stat` printed nothing
while a name-only count of the unstaged diff printed **4**. The instruments are not mute.

**Check 2.** The exclusion returned nothing outside `StratBridge`, so the regex was controlled on
synthetic input. The lowercase-initial call pattern matched `strat::doThing(x);` -> **True**, and
did NOT match `strat::UiSnapshot s;` -> **False** or `strat::GameState g;` -> **False**. It
separates calls from type names, which is the distinction the checklist requires. Comments
stripped first. By module: `StratBridge` **137**, every other module **0**.

**Check 3.** 0 violations, detector alive: prepending a `StratBridge.h` include to an in-memory
copy of `Source/Stratocracy/StratocracyCharacter.h` made the detector fire -> **True**.

**Check 4.** Two real `FStratBridge` members, both `TPimplPtr`:
`Source/StratPlay/StratMatchSubsystem.h:2614` - `TPimplPtr<FStratBridge> Bridge;` - and
`Source/StratUI/StratScoreboardHUD.h:1253` - `TPimplPtr<FStratBridge> OwnedBridge;`. Control:
`TUniquePtr` occurs **12** times in `Source/` and **0** of them on an `FStratBridge` member line;
the co-occurring lines are all comments, including `Source/StratUI/StratViewModel.h:47`, "the
`C4150` measured on `TUniquePtr<FStratBridge>`".

**Check 5, derived and not trusted.** **16 rows**, all five modules present, so the extraction did
not silently fail:

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules        <- the conditional 16th row
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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,AIModule,NavigationSystem,StateTreeModule,GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

The sixteenth row is confirmed guarded **by reading the file, not the extraction**.
`StratBridge.Build.cs:133` sets `bCompileVendoredRulesHere` from
`Target.LinkType != TargetLinkType.Monolithic`, `:135` is `if (!bCompileVendoredRulesHere)`, and
`:137` is `PrivateDependencyModuleNames.Add("StratRules");`. Monolithic-only, deliberate, **not a
finding**. `StratRules` does **not** appear in `StratBridge`'s `PublicDependencyModuleNames` and
does not appear unconditionally in the private array. The modular branch still compiles the
vendored sources in: `Source/StratBridge/Vendored/` holds the ten `*.strat.cpp` shims,
`git diff --stat` on that directory is empty, and `Ai.strat.cpp`'s own header block states the
monolithic exclusion in terms. Field census returned exactly **ten** names - the three graph
fields plus `PCHUsage =`, `bUseUnity =`, `ShadowVariableWarningLevel =`,
`PrivateDependencyModuleNames.Add`, `PrivateDefinitions.Add`, `bCompileVendoredRulesHere =`, and
the documented `BRIDGE=` false positive, which is the tail of the
`"STRAT_VENDORED_RULES_IN_BRIDGE="` string literal at `:134`. **No eleventh name.** `StratUI` has
not gained a `Stratocracy` dependency; no cycle.

**Check 6.** `IMPLEMENT_MODULE` census: `StratBridge` **2**, `StratPlay` **2**, `StratUI` **2**,
`StratRules` **0**, `Stratocracy` **0** (it uses `IMPLEMENT_PRIMARY_GAME_MODULE`). The string
`StratRules` does not occur in `Stratocracy.uproject` at all. No module object, correctly
unregistered - the documented false positive, not raised.

**Check 8.** 248 stripped-code hits, 247 under `Tests/`, exactly one outside:
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp` - the import commandlet, the second
standing exception, in a module the diff does not touch. Control: the same scan with comments
included returns **328**, so the stripper removes real text rather than everything.

**Check 9, proved with a three-way control.** A literal-aware stripper that refuses to emit zero
lines returned code-line sets **identical to `HEAD`** on all three headers - **37 / 50 / 16**
lines. Three mutation controls on in-memory copies of `StratCameraPawn.h`:

- **A**, a code value `-60.0f` changed to `-61.0f`: detected as different -> **True**
- **B**, a word inside a `//` comment changed: mutation applied **True**, correctly ignored **True**
- **C**, an added `int ZZProbe = 1;` line: detected as different -> **True**

Control B was run twice: the first attempt substituted a word that was not present, so the
mutation did not apply and "ignored" would have been vacuous. It was re-run against a word the
file actually contains before being reported. The comparator answers neither "identical" nor
"different" to everything. No arithmetic was added to `StratUI` because no code line was added
to it.

## Findings

### 1. BLOCK - a retracted control survives unmarked, as its own present-tense measurement, at `engine.md:160-161` and `:167-169`

**Constraint:** checklist 11 - a prose block records measurements as measurements. Same
constraint and same axis as the defect this pass exists to repair.

**Offending text, quoted:**

> (`/Script/StratUI.StratGuidanceWidget` returns 0 on it against **4** on each of
> `WBP_DirectiveStrip`, `WBP_Scoreboard`, `WBP_Options` and `WBP_CommandBar` as positive
> controls)

and

> `WBP_DirectiveStrip.uasset` carries `/Script/StratUI.StratGuidanceWidget` **4** times, the same
> figure as the three independently-known C++-parented controls above

**Why it gates.** Both sites are uncommitted insertions in this diff, inside the single hunk
`@@ -17,0 +18,201 @@`, so the stamp-not-strike protection for committed history does not reach
them and the repair is free. The same file states the governing rule in bold at line 26 - "A
CONTROL'S FIGURES ARE MEASURED PER SUBJECT, NEVER GENERALISED ACROSS A SET THAT MERELY SHARES A
PROPERTY" - and violates it 134 lines below, in an entry describing the very sweep that produced
the rule. A reader arriving by citation lands on line 160, not on line 26; that is this project's
own stated reason for requiring inline attribution rather than a header, and it applies unchanged
to an inline measurement.

**The repair.** A stamp at the two sites, matching the treatment the two headers already
received. Nothing else in `engine.md` is faulted; lines 31-35 in particular are correct and must
not be "fixed".

**Owner:** `strat-gameplay-engineer` - `engine.md` is its own record file and this is in lane.

## The fifteenth-instance sweep - re-run independently, and the null is SCOPED

**The prior round's scoping is restated here so it travels with the verdict:** denominator 434
flat-only of 711 candidates over 7559 windows; machine-triaged claims 11 -> 39; **174 checkable
windows naming no identifier were not individually read**, and neither were **221 normative
windows**; and the identifier counter is loose enough to raise false positives but not to clear
what it passes - name-collision inflation, e.g. a "SetViewingSide is the only writer" claim
producing 18 assignment sites that are mostly `int32 ViewingSide = 0;` declarations on six
unrelated structs.

**My re-run does not reproduce those denominators, and I am reporting that rather than papering
over it.** I rebuilt the instrument from the method description, because the prior report states
its predicate list only in part. Mine returned, over the same 267 files:

```
comment windows                                       4774
candidates (reason marker + flat assertion)            498
  also hedged - visible to prior sweeps                271
  FLAT-ONLY - the uncovered ground                     227
    mechanically checkable existential/exclusivity     148
    normative never/always - a rule, cannot expire      79
```

The two instruments are **not the same instrument** and the figures are not comparable. That is
itself the honest result: "parse the comment windows" is a method description, not a
specification, and two reviewers implementing it faithfully produced 7559 windows and 4774. **No
conclusion about the tree should be drawn from the difference between those numbers.**

**Plant, and the plant nearly reported a dead instrument as a clean one.** The synthetic instance
- "The strip is rebuilt on every reconcile because StratScoreboardHUD is the only caller of this
builder, so a cached copy could never go stale" - was injected **into the in-memory file map
only**, never written to the tree; `git status --porcelain` showed the same four modified files
throughout. Prepended to `Source/StratUI/StratViewModel.h`, it returned **delta 0 on every
counter**, which reads exactly like a dead detector. Diagnosed rather than assumed: that file's
first line is already a `//` comment, so the plant **fused into the file's existing leading
comment window**, which was already counted as a candidate. Re-planted as a standalone synthetic
file, the delta was **exactly 1** on files, windows, candidates, flat-only and checkable. The
instrument is alive; the first result was a plant-placement artifact.

**That is the finding worth keeping from this sweep**, and it generalises past this tree: a plant
prepended to a file that opens with a comment is absorbed into an existing window and produces a
zero delta, which is indistinguishable from an instrument that cannot see it. Plant into a window
that does not already exist, and measure the window count as well as the candidate count.

**Result: no fifteenth instance found, and I did not look hard enough to claim there is none.** I
did not individually read the 148 checkable windows my own run produced. The null is scoped
strictly more heavily than the prior round's, not less.

**Is another pass worth it? My judgement: no - and here is precisely what that leaves
unchecked.** Three reasons. (1) The two independent implementations disagree on the denominator by
roughly 40%, so a third pass would most likely produce a third denominator rather than a
fifteenth instance. (2) The normative windows - 79 by my count, 221 by the prior round's - are
never/always rules, which state a constraint rather than a time-varying fact and therefore
**cannot expire**; they sit in the denominator only because the flat predicate cannot tell a rule
from an assertion. (3) The identifier counter is known to inflate and known not to clear what it
passes, so widening it raises the false-positive load without moving the null.

**What that leaves unchecked, named so it is checkable later:** every flat-only comment window
that names no backticked identifier - 28 by my run's figures, 174 by the prior round's - and
every normative window, across all 267 `.h`/`.cpp` files under `Source/`. **The better next
instrument is not a wider sweep but a narrower one**: resolve exclusivity claims by assignment
site in stripped code, scoped to the declaring type, rather than by raw name census. That
converts a loose counter into one whose null means something. Until someone builds it, this
ground is uncovered and should be described as uncovered rather than as swept.

## Observations

Non-gating.

- **The prior report's `PER SUBJECT` parenthetical is corrected above.** It is a defect in a gate
  report, not in the tree, so it does not gate - but a gate report that states a false measured
  count is exactly the failure this checklist exists to catch, and it went out under my own
  banner. Recorded here rather than left for someone else to find.
- **Every figure in the brief reproduces exactly.** Comment-only 37 / 50 / 16 code lines identical
  to `HEAD`. sha256 prefixes: `StratCameraPawn.h` `003b74eed95d6e0f`, `StratGuidedOpening.h`
  `e6c0c96eb8e1f7eb`, `StratGuidanceWidget.h` `cc39059bc23422f8`. Line endings by Python
  `endswith(chr(13))`: `StratCameraPawn.h` **0 CRLF / 193 bare LF**, `StratGuidedOpening.h`
  **579 / 0**, `StratGuidanceWidget.h` **191 / 0** - per-file variance preserved, and the census
  was controlled both ways on fabricated pure-LF (**0**) and pure-CRLF (**2**) two-line strings.
  Not measured with `grep`, for the reason the prior report recorded against itself.
- **`StratCameraPawn.h` is LF in the worktree and git warns on every diff** that LF will become
  CRLF the next time git touches it. Pre-existing `autocrlf` condition, not introduced by this
  pass, and the file is byte-identical to the version the fourteenth gate passed.
- **Suite branch confirmed, no re-run owed.** 0 `.cpp` files in the diff.
- **`strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0** - 435 entries all Success, macro
  census 435, 63 suite claims (1 live, 62 stamped), 2 tracked items declared.
- **`engine.md` attribution is correct** on the new entry: 2026-09-06,
  `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, base commit `92d78c1`, UNCOMMITTED),
  with the live suite figure and the phase verdict explicitly deferred to `global.md`. Finding 1
  is about the entry's figures, not its attribution - the same split the prior report drew, and
  it holds.
- **`.agents/ue-project-context.md` was not re-derived this round**, because no `.Build.cs` and no
  module boundary moved. Said plainly so it does not read as a check that passed.

- **This report is its own third untracked file, and the Scope block above does not count
  it.** Scope was measured before this file existed and said "two untracked gate reports";
  `git status --porcelain` now returns three, the third being this report. The four MODIFIED
  files are unchanged and checklist row 12 is unaffected - nothing is staged. Recorded rather
  than silently corrected upward, because a record that counts a set it is a member of moves
  its own subject.

VERDICT: BLOCK
