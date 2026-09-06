# Gate: sixteenth pass, REWRITE-NOT-STAMP RE-GATE - strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base commit `92d78c1`.
`git status --porcelain` is the authority and returned exactly four modified files:

```
 M Source/StratPlay/StratCameraPawn.h
 M Source/StratPlay/StratGuidedOpening.h
 M Source/StratUI/StratGuidanceWidget.h
 M Tools/architect/state/engine.md
```

plus untracked gate reports under `Tools/architect/gate_reports/`, of which this file is one.
`git diff --stat` reports 349 insertions / 2 deletions. `Tools/architect/state/engine.md` is the
only file that moved since the fifteenth re-gate: its hunk went from `@@ -17,0 +18,201 @@` to
`@@ -17,0 +18,219 @@`, `--numstat` **219 / 0**, still a single pure insertion. The three `.h`
files are byte-identical to the versions the fifteenth gate passed - sha256 prefixes
`003b74eed95d6e0f`, `e6c0c96eb8e1f7eb`, `cc39059bc23422f8`, all three reproducing the brief.

All twelve checks were re-run against this tree. Nothing is inherited from the prior round; the
token table that settled the last finding was **re-derived from the package bytes this round**
rather than carried, which the prior report said plainly it had not done.

## The first judgement: rewrite rather than stamp - ACCEPTED, and it is the better repair

`strat-gameplay-engineer` was offered a stamp and did something else. Judged on the constraint
rather than on the remedy I happened to name:

**The premise is true and I verified it independently.** `git diff --numstat` on the file is
`219 0` and `git diff -U0` is the single hunk `@@ -17,0 +18,219 @@`. Both faulted sites are
inside it. Neither has ever been committed. `Tools/architect/state/engine.md`'s own header says
"stamp an entry that has become history rather than deleting it" - the convention is about
entries that HAVE BECOME HISTORY, and these bytes have not been anywhere.

**The constraint was checklist 11 - a measurement recorded as a measurement - and it is
discharged.** The finding was that a retracted control survived as this record's own
present-tense assertion. It no longer does. Re-derived on the package bytes this round, every
clause of both rewritten sites holds per subject:

```
token                                        DirectiveStrip Scoreboard Options CommandBar PreMatch
/Script/StratUI.StratGuidanceWidget                 4            0        0         0         0
/Script/StratUI.StratScoreboardWidget               0            4        0         0         0
/Script/StratUI.StratOptionsWidget                  0            0        4         0         0
/Script/StratUI.StratCommandBarWidget               0            0        0         4         0
/Script/UMG.UserWidget                              0            0        0         0         4
/Script/StratUI.StratFabricatedNopeWidget           0            0        0         0         0
```

All five assets are real package bytes, not LFS pointers (39-181 KB, none opening
`version https`), and the fabricated token returns 0 across the row, so the counter is not
answering 4 to everything. `engine.md:164-169` now reads "returns **0** on it; that same token
returns **4** on `WBP_DirectiveStrip` and **0** on `WBP_Scoreboard`, `WBP_Options` and
`WBP_CommandBar`, each of which carries **4** of ITS OWN parent token instead" - which is the
table, per subject. `engine.md:180-184` likewise. The surrounding figures in the same
parenthetical also reproduce: **1940** packages under `Content/`, **0** referencing
`WBP_PreMatchBriefing`, **3** referencing `WBP_Options` (`BP_StratGameMode`,
`BP_StratGameMode_AiVsAi`, `BP_StratShellGameMode`), **0** for a fabricated name.

**The legibility a stamp would have bought is bought another way, and I checked that rather than
assuming it.** Both sites carry the retraction in prose - `:172-177` "That parenthetical read
'0 on it against 4 on each of' all four assets in this entry's first draft ... rewritten per
subject rather than stamped, because these bytes are an uncommitted insertion in this same diff
and have never shipped", and `:186-189` for the sibling. The old wording is QUOTED, the reason it
was wrong is stated, and the reason for rewriting rather than stamping is stated at the site
rather than in a header. A reader arriving by citation lands on all three facts.

**Why this is better than the stamp I named.** A stamp keeps the wrong figure as the entry's
assertion and appends a correction after it; the reader must carry the false number past the
point of reading it. Rewriting demotes the false figure to a quotation - which is what it is -
and promotes the measured one to the assertion. The distinction the last two rounds turned on is
exactly the one between a claim and a quoted claim, and the rewrite puts each on the correct side
of it. **A reviewer that requires its own suggested remedy rather than the constraint being met
is legislating a form.** I named a sufficient repair; the agent found a sufficient repair that is
also a better one. Accepted.

The one-sentence append to the rule bullet (`:30-34`, "AND STATING THE RULE IN THIS DOCUMENT DID
NOT PROPAGATE IT THROUGH THIS DOCUMENT") is present and its arithmetic is right as a historical
statement: rule at line 26, the generalisation the fifteenth gate found at line 160, 134 lines.

## The second judgement: my report's grep count - CORRECTED, it was FALSE when written

The prior report states that a grep for `on each of` over `Tools/architect/state/engine.md`
returns "**three** lines: **28**, **32** and **160**". Re-run on this tree it returns **four**:

```
28:    N on a known-positive case*. Measuring ONE subject and then writing "N on each of A, B, C
36:    /Script/StratUI.StratGuidanceWidget "returns 0 on it, against 4 on each of
173:   parenthetical read "0 on it against 4 on each of" all four assets in this entry's first
677:   the assets: grep -a -c "WBP_Options" returns **2** on each of
```

Control on the absent phrase `on each zzof` returns **0**; the same file returns **94** for
`measured`, so the instrument is neither mute nor matching everything.

**I correct it, and I do not defend it.** The count was not merely stale. `git show HEAD:` on the
same file returns one hit for the same pattern, at line **458**; the previous working tree was
HEAD plus a 201-line insertion at line 18, so 458 + 201 = **659**, and the fourth site was
present and matchable at the moment the prior report was written. The prior round's own published
pattern returns four on the prior round's own tree. Whatever invocation produced "three", it is
not the one the report printed, and the report did not say so. The finding and the adjudication
are undisturbed - line 659 was committed history outside the insertion hunk and was never in
scope - but **the sentence that gates on re-runnable counts carried a count that does not
re-run**, in a report that had already corrected itself once that round for a false count. That
is twice in one report. Recorded here under my own banner rather than left to be found.

All four sites are dispositioned, and the disposition is checkable:

| line | site | disposition |
|---|---|---|
| 28 | the rule's own illustration, generic `A, B, C and D` | not a measurement - it is the rule text |
| 36 | quoted past-tense retraction, attributed to `StratGuidedOpening.h`, corrected in the next sentence | clean, and I decline again to require a stamp on it |
| 173 | quoted past-tense retraction of this entry's own first draft | clean, same shape as 36 |
| 677 | committed history, outside the hunk | see below - **re-derived and TRUE** |

## The third judgement: `engine.md:677` - the restraint was right, and the site needs nothing

The agent filed this rather than reaching for it, did not re-derive the figure, and made no claim
either way. **That restraint was correct**: the site is committed history, outside the insertion
hunk, and re-opening a committed entry unasked is not its lane's business on a repair pass.
Filing it as a handoff is what the situation called for.

**I am not restrained by the same thing - a gate reads the tree - so I settled it.** Re-derived
per subject, on the worktree bytes:

```
Content/StratPlay/BP_StratGameMode.uasset         23625 B  WBP_Options=2  WBP_ZZFabricated=0
Content/StratPlay/BP_StratGameMode_AiVsAi.uasset  22442 B  WBP_Options=2  WBP_ZZFabricated=0
Content/StratPlay/BP_StratShellGameMode.uasset    23191 B  WBP_Options=2  WBP_ZZFabricated=0
```

None is an LFS pointer; the fabricated token returns 0 on all three, so the counter is selective.
**"returns 2 on each of" those three files is TRUE, measured on each of the three, and it is not
a generalisation.** The wording is the shape the rule at line 26 polices, but the shape is not
the offence - generalising from one subject is - and here the figure holds on every subject
independently. The surrounding text is also unusually careful for this axis: it states that `-c`
counts LINES and not occurrences, it names `strings` as unavailable so a zero would have been
unreadable, and it gives per-file negative controls with per-file speaking figures.

**No stamp is owed, now or in a later pass.** A stamp records a retraction; stamping a claim that
re-derives true would put a false correction into the record. The agent's handoff is answered:
the site is fine as-is, and the answer is a measurement rather than an opinion.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass - `git diff --stat -- Source/StratRules Data` empty; control below; manifests still `96d93ea` / `c2f5860` |
| 2 | No `strat::` call outside StratBridge/StratRules | pass - 137 call-shaped hits, all `StratBridge`, 0 elsewhere; regex control below |
| 3 | No vendored header in front of UHT | pass - 59 UHT-declaring headers, 0 violations; planted control fired |
| 4 | `TPimplPtr` not `TUniquePtr` | pass - both real members `TPimplPtr`; `TUniquePtr` is 0 in stripped code tree-wide |
| 5 | Module arrows unchanged and correct | pass - 16 rows / 5 modules / 10 field names, no eleventh; 0 `.Build.cs` in the diff; 16th row read as guarded |
| 6 | New modules registered | pass - no new module directories; `StratRules` has 0 `IMPLEMENT_*` and is correctly absent from the `.uproject` |
| 7 | `Source/Stratocracy/` untouched | pass - empty; control below |
| 8 | No `/Game/` literal in gameplay C++ | pass - 248 stripped-code hits, 247 under `Tests/`, 1 in the import commandlet |
| 9 | No widget-side arithmetic | pass - the `Source/StratUI` change is comment-only, proved with three mutation controls; 0 non-comment added lines |
| 10 | Lane discipline | pass - three `.h` outside `Tests/` plus the engineer's own `engine.md`; no `Tests/`, `Content/` or `Config/` file touched |
| 11 | Prose blocks present, measurements as measurements | pass - both faulted sites repaired and re-derived; two non-gating wrinkles under Observations |
| 12 | Nothing staged or committed | pass - `git diff --cached --stat` empty; control below |

### Controls for every check that returned nothing

**Checks 1, 7, 12.** The same `git diff --stat -- <path>` that printed nothing for
`Source/StratRules Data` and for `Source/Stratocracy` printed **two** files for
`Source/StratPlay` and **one** for `Source/StratUI` in the same breath. `git diff --cached --stat`
printed nothing while `git diff --name-only | wc -l` printed **4**. `git diff --stat` on
`Source/StratBridge/Vendored` printed nothing while the directory holds **12** entries.

**Check 2.** Comments stripped with a literal-aware stripper first. The lowercase-initial call
pattern matched `strat::doThing(x);` -> **True** and did NOT match `strat::UiSnapshot s;` ->
**False** or `strat::GameState g;` -> **False**, so it separates calls from type names, which is
the distinction the checklist requires. By module: `StratBridge` **137**, every other module
**0**. 267 `.h`/`.cpp` files scanned.

**Check 3.** 0 violations with the detector shown alive: prepending an include of `StratBridge.h`
to an in-memory copy of `Source/Stratocracy/StratocracyCharacter.h` made it fire -> **True**; the
unplanted copy -> **False**.

**Check 4.** Two real `FStratBridge` members, both `TPimplPtr` -
`Source/StratPlay/StratMatchSubsystem.h:2614` `TPimplPtr<FStratBridge> Bridge;` and
`Source/StratUI/StratScoreboardHUD.h:1253` `TPimplPtr<FStratBridge> OwnedBridge;`. The same regex
alternation would have matched `TUniquePtr<FStratBridge>` and did not, and the count of
`TUniquePtr` anywhere in stripped code across `Source/` is **0** - every occurrence of that
identifier in the tree is inside a comment, including the one recording the measured `C4150`.

**Check 5, derived and not trusted.** The prescribed extraction was run verbatim and printed
**16 rows**, all five modules present, so the extraction did not silently fail:

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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                            AIModule,NavigationSystem,StateTreeModule,
                                            GameplayStateTreeModule,Niagara,UMG,Slate,Json,
                                            StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

The sixteenth row is confirmed guarded **by reading the file, not the extraction**:
`Source/StratBridge/StratBridge.Build.cs:133` sets `bCompileVendoredRulesHere` from
`Target.LinkType != TargetLinkType.Monolithic`, `:135` is `if (!bCompileVendoredRulesHere)`,
`:137` is the `PrivateDependencyModuleNames.Add` of `StratRules`, and `:134` emits the
`STRAT_VENDORED_RULES_IN_BRIDGE=` definition from that same one expression, so the dependency
half and the compile half cannot drift apart. Monolithic-only, deliberate, **not a finding**.
`StratRules` appears in `StratBridge`'s `PublicDependencyModuleNames` **not at all**, and in the
private array only under that guard. The modular branch still compiles the vendored sources in -
`Source/StratBridge/Vendored/` holds 12 entries and its diff is empty. `StratUI` has not gained a
`Stratocracy` dependency; no cycle. Field census printed exactly **ten** names - the three graph
fields, `PCHUsage =`, `bUseUnity =`, `ShadowVariableWarningLevel =`,
`PrivateDependencyModuleNames.Add`, `PrivateDefinitions.Add`, `bCompileVendoredRulesHere =`, and
the documented `BRIDGE=` false positive, which is the tail of the vendored-rules string literal
at `:134` and was not sedded away. **No eleventh name.** 0 `.Build.cs` files appear in the diff,
so nothing here moved; it is derived anyway, because that is the point of the row.

**Check 6.** Five module directories under `Source/`. Files carrying `IMPLEMENT_MODULE` or
`IMPLEMENT_PRIMARY_GAME_MODULE`: `StratBridge` **1**, `StratPlay` **1**, `StratUI` **1**,
`Stratocracy` **1**, `StratRules` **0**. `Stratocracy.uproject`'s `Modules` array parses to
exactly `Stratocracy`, `StratBridge`, `StratUI`, `StratPlay`, and the literal `StratRules` does
not occur anywhere in that file. No module object, correctly unregistered - the documented false
positive, checked for `IMPLEMENT_MODULE` before being not-raised.

**Check 8.** 248 stripped-code hits, 247 under `Tests/`, exactly one outside -
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp`, the import commandlet, the second
standing exception, in a module the diff does not touch. Control: the same scan without stripping
returns **328**, so the stripper removes real text rather than everything.

**Check 9, proved with three mutation controls.** A literal-aware stripper that refuses to emit
zero lines returned code-line sets **identical to HEAD** on all three headers - 37 / 50 / 16
lines. On in-memory copies of `StratCameraPawn.h`: **A**, a code value changed from -60.0f to
-61.0f, applied True, detected different **True**; **B**, a word the file actually contains
changed inside a line comment, applied True, correctly ignored **True**; **C**, an added
`int ZZProbe = 1;`, applied True, detected different **True**. The comparator answers neither
"identical" nor "different" to everything. Independently, `git diff -U0 -- Source/StratUI` yields
**39** added lines of which **0** survive comment-stripping, so no code line - and therefore no
arithmetic - entered `StratUI`.

**Check 11 supporting measurements.** `engine.md` is uniform CRLF: **8672** CR-terminated of 8673
LF-split segments, **0** bare LF. Counted in Python with a byte census, controlled both ways on
fabricated pure-CRLF (**2**) and pure-LF (**0**) two-line strings, and deliberately not with
`grep`, for the reason this record already carries against itself. The brief's figure reproduces
exactly.

**`strat_banner_sweep.py` printed `SWEEP CLEAN`, exit 0** - 435 automation entries all Success,
macro census 435, 63 suite claims (1 live, 62 stamped), 2 tracked items declared. **Its census
half is shown speaking by those non-zero figures; its contradiction detector is NOT proved able
to fire this round**, because proving that would require mutating a tracked file, and I will not
write to the tree to test my own instrument. Said plainly so `SWEEP CLEAN` is not read as more
than it is.

**Not run, and named so it does not read as a check that passed.** I did not run the build and
did not run the suite; `strat-test-author` owns those, and the diff contains **0** `.cpp` files,
which I verified rather than took from the brief. `.agents/ue-project-context.md` was not
re-derived, because no `.Build.cs` and no module boundary moved. **And one claim in the brief is
not verifiable from a checkout at all:** that the clean retract-and-quote is byte-identical
before and after this round. Both states are uncommitted, so git holds neither, and the md5 the
brief cites cannot be re-derived by anyone reading this tree. It does not change the verdict -
that passage is correct **as written now**, which is what a gate on a tree can say - but an
unverifiable claim is reported as unverifiable rather than accepted.

## Findings

None.

## Observations

Non-gating.

- **A flat existential in the record is measurably false, and the same document already knows
  it.** `engine.md:169` states, in a list of the bullet's current grounds, "nothing in `Source/`
  names it". Measured, `WBP_PreMatchBriefing` is named on **four** lines in `Source/` -
  `StratGuidedOpening.h:160` and `:170`, `StratGuidanceWidget.h:38` and `:49` - all of them
  comments written by this same pass. The production header is precise where the record is not:
  `StratGuidedOpening.h:177-178` reads "Nothing in `Source/` names the asset EXCEPT this
  correction block and its sibling in `StratGuidanceWidget.h` -- 2 hits before this rewrite",
  and `engine.md:63-66` records that the unqualified wording "was self-falsifying by a hair" and
  that the header got the word. **The word did not reach the record's own restatement 100 lines
  below**, which is the identical non-propagation the appended sentence at `:30-34` was added to
  record about the previous defect.
- **Why that is an Observation and not the sixteenth BLOCK, stated so the next reviewer can
  disagree with a reason rather than by fresh reading.** The last finding was a number false on
  every reading, with no corrective anywhere in the file, and acting on it would have carried a
  wrong fact about three assets into the next pass. This one has its corrective in the same file,
  in the entry above it, dated the same day, naming this exact wording; the artifact a code
  reader hits is correct; and the operative conclusion the bullet rests on - there is no declared
  C++ type for three strings to bind to - is true and untouched by whether four comment lines
  mention the asset. The cost is a reader's wasted grep, not a wrong action. Worth the word on
  the next revision of that entry; not worth a round.
- **`engine.md:64` drops a time qualifier its own header carries.** It reads "appears in
  `Source/` **twice**, and both hits are this pass's own comment lines". `StratGuidedOpening.h`
  says "2 hits **before this rewrite**". Measured now: 4 lines, in 2 files, in 2 correction
  blocks - so the figure reproduces only if the unit is blocks or files, and the unit is not
  stated. Same axis as the item above and the same disposition.
- **Two sentences apart, "stamped" means two different things.** `engine.md:188` says the clause
  is "rewritten per subject rather than stamped"; `:189-191` says "The paragraph is stamped and
  kept whole". The first is about this record's own bytes, the second about
  `StratGuidanceWidget.h`'s paragraph. Both are true; a reader arriving by citation has to work
  out which subject each has. Untidiness, not a defect.
- **My own prior report's grep count is corrected in the body above, and it was false when
  written rather than stale.** That is the second false count under my banner in that one report.
  Recorded here rather than left for the next round to find.
- **`StratCameraPawn.h` is LF in the worktree and git warns on every diff** that LF will become
  CRLF the next time git touches it. Pre-existing `autocrlf` condition, not introduced by this
  pass, and the file is byte-identical to the version the fourteenth gate passed.
- **This report is itself an untracked file, and the Scope block does not count the gate
  reports.** Scope names the four MODIFIED files, which is what checklist row 12 turns on;
  nothing is staged. Said rather than silently corrected, because a record that counts a set it
  belongs to moves its own subject.

## The fifteenth-instance sweep - the standing position, restated so it travels with the verdict

**I did not re-run it this round and I am not reopening it. My judgement from the fifteenth pass
stands, unchanged, and it is restated in full here rather than cited:**

**Another pass is not worth it.** Three reasons. (1) Two independent implementations of "parse
the comment windows" disagreed on the denominator by roughly 40% - 7559 windows against 4774 -
because that is a method description and not a specification, so a third pass would most likely
produce a third denominator rather than a fifteenth instance. (2) The normative windows - 79 by
my run, 221 by the prior round's - are `never`/`always` rules, which state a constraint rather
than a time-varying fact and therefore **cannot expire**; they sit in the denominator only
because the flat predicate cannot tell a rule from an assertion. (3) The identifier counter is
known to inflate and known not to clear what it passes, so widening it raises the false-positive
load without moving the null.

**What that leaves unchecked, named so it stays checkable:** every flat-only comment window that
names no backticked identifier - 28 by my run's figures, 174 by the prior round's - and every
normative window, across all 267 `.h`/`.cpp` files under `Source/`. **The better next instrument
is a narrower one, not a wider one:** resolve exclusivity claims by ASSIGNMENT SITE in stripped
code, scoped to the declaring type, rather than by raw name census. That converts a loose counter
into one whose null means something. Until someone builds it, **this ground is uncovered and
should be described as uncovered rather than as swept.**

The instrument lesson from that pass is kept too, because it generalises past this tree: a plant
prepended to a file that already opens with a comment is absorbed into the existing window and
returns a **zero delta**, which is indistinguishable from a detector that cannot see it. Plant
into a window that does not already exist, and measure the window count as well as the candidate
count.

VERDICT: PASS
