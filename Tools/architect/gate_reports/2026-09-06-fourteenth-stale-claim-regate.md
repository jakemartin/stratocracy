# Gate: fourteenth stale-claim pass, RE-GATE - strat-integration-reviewer

## Scope

Uncommitted working tree at `E:/MultiAgent/Stratocracy`, branch `master`, base commit
`92d78c1`. `git status --porcelain` is the authority and returned exactly four modified
files plus this report's predecessor as untracked:

```
 M Source/StratPlay/StratCameraPawn.h
 M Source/StratPlay/StratGuidedOpening.h
 M Source/StratUI/StratGuidanceWidget.h
 M Tools/architect/state/engine.md
?? Tools/architect/gate_reports/2026-09-06-fourteenth-stale-claim-gate.md
```

Diff is 331 insertions / 2 deletions across the four. This is a re-gate of the tree, not of
the phase: the prior finding is judged answered below, and the tree is then judged whole.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass - `git diff --stat -- Source/StratRules Data` empty; control below |
| 2 | No `strat::` call outside StratBridge/StratRules | pass - 137 call-shaped hits, all in `Source/StratBridge/`, 0 elsewhere; filter control below |
| 3 | No vendored header in front of UHT | pass - 59 UHT-declaring headers, 0 violations; detector control fired |
| 4 | `TPimplPtr` not `TUniquePtr` | pass - both real members `TPimplPtr`; 14 `TUniquePtr` mentions are all comments |
| 5 | Module arrows unchanged and correct | pass - 16 rows / 5 modules / 10 field names; 0 `.Build.cs` in the diff |
| 6 | New modules registered | pass - no new modules; `StratRules` correctly 0 `IMPLEMENT_MODULE` and correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass - `git diff --stat` empty; control below |
| 8 | No `/Game/` literal in gameplay C++ | pass - 248 hits, 247 in `Tests/` fixtures and 1 in the import commandlet; both standing exceptions |
| 9 | No widget-side arithmetic | pass - the `Source/StratUI/` change is comment-only, proved with a speaking comparator |
| 10 | Lane discipline | pass - C++ outside `Tests/` plus the engineer's own `engine.md` |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** - see finding 1 |
| 12 | Nothing staged or committed | pass - `git diff --cached --stat` empty; control below |

### Controls for the checks that pass by returning nothing

**Checks 1, 7, 12.** The empty results are not an empty instrument. The same
`git diff --stat -- <path>` that printed nothing for `Source/StratRules Data` and for
`Source/Stratocracy` printed two files for `Source/StratPlay` and one for `Source/StratUI`;
`git diff --cached --stat` printed nothing while `git diff --name-only` counted 4.

**Check 2.** The exclusion returned nothing, so the regex itself was controlled on synthetic
input. The lowercase-initial call pattern matched `strat::doThing(x);` and did NOT match
`strat::UiSnapshot s;` or `strat::GameState g;` - it separates calls from type names, which is
the distinction the checklist requires. Comments were stripped first, so the 137 hits are code.
By module: `StratBridge` 137, everything else 0.

**Check 3.** 0 violations, and the detector is alive: prepending `#include "StratBridge.h"` to
an in-memory copy of a real UHT-declaring header (`Source/Stratocracy/StratocracyCharacter.h`)
made the detector fire.

**Check 4.** The only two `FStratBridge` members are `StratMatchSubsystem.h:2906` and
`StratScoreboardHUD.h:1626`, both `TPimplPtr`. Control: `TUniquePtr` appears 14 times in
`Source/` with comments included and 0 times on a `FStratBridge` member line.

**Check 5, derived and not trusted.** 16 rows, all five modules present so the extraction did
not silently fail:

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules      <- the conditional 16th row
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
StratRules   PublicDependencyModuleNames    Core
StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
Stratocracy  PublicDependencyModuleNames    ...,StratRules
```

The sixteenth row is confirmed guarded by reading the file rather than the extraction:
`StratBridge.Build.cs:133` sets `bCompileVendoredRulesHere` from
`Target.LinkType != TargetLinkType.Monolithic`, `:135` is `if (!bCompileVendoredRulesHere)`,
and `:137` is `PrivateDependencyModuleNames.Add("StratRules");`. Monolithic-only, deliberate,
not a finding. `StratRules` does NOT appear in `PublicDependencyModuleNames` and does not
appear unconditionally in the private array. The field census returned exactly ten names,
including the documented `BRIDGE=` false positive (the tail of the
`"STRAT_VENDORED_RULES_IN_BRIDGE="` string literal); no eleventh name. `StratUI` has not
gained a `Stratocracy` dependency; no cycle.

**Check 6.** `IMPLEMENT_MODULE` census: `StratBridge` 2, `StratPlay` 2, `StratUI` 2,
`StratRules` **0**, `Stratocracy` 0 (it uses `IMPLEMENT_PRIMARY_GAME_MODULE`). The `.uproject`
`Modules` array names `Stratocracy, StratBridge, StratUI, StratPlay` and plugin modules.
`StratRules` is absent and has no module object, which is the required state, not a finding.

**Check 8.** 248 stripped-code hits, 247 of them under `Tests/` and exactly one outside:
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, a
`FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName)` - the import commandlet, the
second standing exception, in a module the diff does not touch. Control: the same scan with
comments included returns 328, so the stripper is removing real text rather than everything.

**Check 9, comment-only proved rather than asserted.** A literal-aware stripper that refuses to
emit zero lines returned code-line sets identical to `HEAD` on all three headers - 37, 50 and
16 lines. Two mutation controls on in-memory copies: changing `-60.0f` to `-61.0f` WAS detected;
changing a comment word was correctly ignored. So the comparator answers neither "identical"
nor "different" to everything.

## Findings

### 1. BLOCK - the retracted control survives verbatim, asserted as a measurement, in `engine.md`

**Constraint:** checklist 11 - a prose block records measurements as measurements. This is the
same constraint and the same axis as the finding this pass was repairing.

The two headers were repaired correctly. `Tools/architect/state/engine.md` was not. Its
2026-09-06 shape-6 entry - which is **in this same uncommitted diff**, not committed history -
still states the miscounted control as its own present-tense measurement, unmarked and
uncorrected. Quoting the file, in the bullet beginning "THE CORRECTION IS DELIBERATELY NARROWER
THAN":

> (`/Script/StratUI.StratGuidanceWidget` returns 0 on it against **4** on each of
> `WBP_DirectiveStrip`, `WBP_Scoreboard`, `WBP_Options` and `WBP_CommandBar` as positive
> controls)

and, in the same entry, the sibling sentence the pass has just agreed is defective:

> `WBP_DirectiveStrip.uasset` carries `/Script/StratUI.StratGuidanceWidget` **4** times, the
> same figure as the three independently-known C++-parented controls above

Measured again this round on the package bytes, all five real content and none an LFS pointer:

```
token                                    DirectiveStrip Scoreboard Options CommandBar PreMatchBriefing
/Script/StratUI.StratGuidanceWidget            4            0         0         0            0
/Script/StratUI.StratScoreboardWidget          0            4         0         0            0
/Script/StratUI.StratOptionsWidget             0            0         4         0            0
/Script/StratUI.StratCommandBarWidget          0            0         0         4            0
/Script/UMG.UserWidget                         0            0         0         0            4
/Script/StratUI.StratFabricatedNopeWidget      0            0         0         0            0
```

So "4 on each of" is false by 3 of its 4 subjects, exactly as before.

**Why this gates rather than sitting under Observations.** Three reasons, and the third is the
decisive one.

1. It is not history. Both `engine.md` entries are uncommitted in this diff; the file's
   append-only stamp-not-strike convention protects committed entries, and neither of these is
   one. It is repairable in this pass at zero cost.
2. The file now contradicts itself inside one uncommitted diff. Its new top entry says, in
   bold, "THE RULE THIS COST ... A CONTROL'S FIGURES ARE MEASURED PER SUBJECT, NEVER
   GENERALISED ACROSS A SET THAT MERELY SHARES A PROPERTY" - and roughly 130 lines below, the
   entry that rule is about still generalises. Measured: the string `PER SUBJECT` appears once
   in each of the two repaired headers and **zero times** anywhere in `engine.md`.
3. A reader arriving by citation lands on the entry, not on the entry above it. That is the
   project's own stated reason for requiring inline attribution rather than a header, and it
   applies unchanged to an inline measurement. The two occurrences at the top of the file are
   fine - they are explicitly quoted AS the retracted wording. These two are not quoted, not
   stamped, and carry no pointer to the retraction.

**My own prior gate missed this.** The fourteenth report checked `engine.md`'s attribution line
and did not check its figures, and recorded that as a clean row. It was not measured, so it
should not have read as measured. Recorded here rather than left for someone else to find.

**Owner:** `strat-gameplay-engineer` - `engine.md` is its own record file and this is in lane.
The fix is a stamp at the two sites in the shape-6 entry, matching the treatment the two
headers already received.

## Adjudications requested

### The `StratGuidanceWidget.h:32-35` disagreement - you are right and I was wrong

You asked to have this adjudicated rather than quietly overridden. **Overruling myself: the
reword was warranted, and it is not a finding against you.**

The disputed sentence was:

> `WBP_DirectiveStrip.uasset` carries `/Script/StratUI.StratGuidanceWidget` 4 times, the SAME
> figure as `WBP_Scoreboard`, `WBP_Options` and `WBP_CommandBar` ... against 0 on
> `WBP_PreMatchBriefing`.

My prior gate called it correct. That judgement was reached by reading the elided subject as
"each asset's own parent token" in the positive-control clause and as "the named token" in the
negative-control clause - **I switched it mid-sentence without noticing, which is precisely the
defect the engineer diagnosed.** The engineer's argument is stronger than "ambiguous". Hold the
elided subject constant, which is the only reading a sentence naming one token supports, and
the positive-control clause is **false**: that token returns 0 on all three, not 4. Hold it
per-asset instead and the very next clause, "against 0 on `WBP_PreMatchBriefing`", becomes a
claim about `StratBriefingWidget`, a token that returns 0 on all five and controls nothing.
**There is no single reading on which the whole sentence is true.** That is worse than ambiguity
and squarely warrants the restatement.

Two of you agreeing is not what settles it; the sentence not having a true reading is. The
restated wording measures each asset against its own token and says so, and every figure in it
reproduces.

### Acting on a non-gating observation mid-repair - correct, and it did not widen the repair

Endorsed. The test is whether the action stayed inside the block already open, and it did,
measured rather than assumed. `PreMatchBriefing` now appears **4** times in all of `Source/`,
and all four are inside the two correction blocks this pass was already rewriting
(`StratGuidedOpening.h` twice, `StratGuidanceWidget.h` twice). No new file, no new section, no
line outside the open block. Control: `DirectiveStrip` returns 22 and a fabricated
`ZzNoSuchWidget` returns 0, so the counter is selective.

The alternative was worse in a specific way rather than merely slower: the observation was that
"nothing in `Source/` names it" had become self-falsifying, and committing a sentence already
known false in order to keep a diff minimal would have created the fifteenth stale claim inside
the pass whose subject is stale claims. The new wording - "EXCEPT this correction block and its
sibling" - is true at 4 hits and stays true if the block is quoted again.

The general rule I would apply: acting on a non-gating observation mid-repair is right when the
observation names a specific sentence inside the block already being rewritten, and wrong when
it opens a file or a block the repair had no reason to touch. This was the first.

## The fifteenth-instance sweep - EXTENDED, with the new denominator, and still a SCOPED null

The prior report's null covered 11 claims of 354. This round the instrument was widened and the
denominator restated. **It remains a scoped null and is reported as one.**

**Method.** Comment windows (maximal runs of comment lines) over all `.h` and `.cpp` under
`Source/`. A candidate carries a reason marker AND a flat assertion; windows also carrying a
temporal or existential hedge are separated out as already visible to prior sweeps. The flat
predicate was widened beyond the prior round's exclusivity terms to include "does not exist",
"zero hits", "0 hits", "exactly one", "sole", "no other", "nothing else".

**New denominator, measured this round:**

```
files scanned                                          267
comment windows                                       7559
candidates (reason marker + flat assertion)            711
  also hedged - visible to prior sweeps                277
  FLAT-ONLY - the uncovered ground                     434
    mechanically checkable existential/exclusivity     213
    normative never/always - a rule, cannot expire     221
```

The prior report's figures were 647 / 293 / 354 on a narrower flat predicate; the uncovered set
is now **434**, and still larger than the covered one.

**Plant.** A synthetic instance - "The strip is rebuilt on every reconcile because
StratScoreboardHUD is the only caller of this builder, so a cached copy could never go stale" -
was injected **into the in-memory file map only**, never written to the tree. It returned in
the checkable set, and counts moved 435 to 434 flat-only and 214 to 213 checkable on its
removal, so the instrument is alive and the delta is exactly one. `git status --porcelain`
showed the same four modified files throughout.

**Machine triage, 11 to 39.** Of the 213 checkable windows, those naming a C++ identifier
adjacent to an exclusivity noun were extracted and each identifier counted across non-`Tests/`
sources: **39 claims**, up from 11 read last round. Instrument control: a fabricated
`ZzNoSuchIdentifier` returned 0 and `ApplyView` returned 188, so the counter speaks. The eight
highest-value "is the only writer" claims were then checked by **assignment site** in stripped
code rather than by raw name count, with a fabricated member returning 0 sites as the control.

**Result: no fifteenth found, and the null is scoped in three named ways.**

- The two claims that looked like counterexamples last round survive re-checking. `ApplyView`
  is the only writer of `AppliedModel` (3 assignment sites: `StratMatchSubsystem.cpp:203`,
  `:1738`, `:5029`) and of `SoundMark` (`:220`, `:1669`, `:5050`) - in both cases the two extra
  sites are the resets the same block names in its own next paragraph. Internally coherent,
  not expired.
- **The 174 checkable windows that name no identifier were not individually read**, and neither
  were the 221 normative windows.
- **The identifier counter is loose and I am not convicting on it.** A raw name census cannot
  distinguish a member from an identically-named field on another type: "SetViewingSide is the
  only writer" produced 18 assignment sites, nearly all of them `int32 ViewingSide = 0;`
  declarations on six unrelated structs. That is the name-collision inflation hazard, and it
  means this instrument can raise false positives but cannot be trusted to have cleared the
  windows it passed.

Recorded as a partial sweep. An unrun check must never appear as a clean one.

## Observations

Non-gating.

- **Every figure in the brief reproduces exactly.** Comment-only 37 / 50 / 16 code lines
  identical to `HEAD`. `StratCameraPawn.h` sha256
  `003b74eed95d6e0fd4dbe4320a26b6db2633be0fd524dc11b267a9663766bf26`. Line endings by Python
  `endswith(chr(13))`: `StratCameraPawn.h` 0 CRLF / 193 bare LF, `StratGuidedOpening.h` 579 / 0,
  `StratGuidanceWidget.h` 191 / 0 - per-file variance preserved, and the census was controlled
  both ways on fabricated pure-LF (0) and pure-CRLF (2) two-line strings. Not measured with
  `grep`, for the reason my prior report recorded against itself.
- **`StratCameraPawn.h` is unchanged since the version I passed**, on every instrument I have:
  identical sha256 to the one stated in the brief, 37 code lines identical to `HEAD` as before,
  the same 0/193 line-ending profile, and the same 50-line diffstat. I did not record a sha256
  in the prior report, so this is a four-way agreement rather than a byte-proof against my own
  earlier copy - said plainly rather than overclaimed.
- **The briefing-asset control reproduces per subject.** `WBP_PreMatchBriefing.uasset` is 50,480
  bytes of real content, contains the substring `/Script/Strat` **zero** times, carries
  `/Script/UMG.UserWidget` 4 times and `Callout` 18 times, and is referenced by **0** of 1940
  packages under `Content/` against **3** for `WBP_Options` (`BP_StratGameMode`,
  `BP_StratGameMode_AiVsAi`, `BP_StratShellGameMode`). A cross-control worth noting: the four
  `StratUI`-parented assets carry `/Script/UMG.UserWidget` **0** times, so the 4 on the briefing
  is a discriminating figure and not a constant.
- **Suite branch confirmed, no re-run owed.** 0 `.cpp` in the diff.
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` returns 0 on all three touched headers against a control of
  7 on `Source/StratUI/Tests/StratScoreboardHUDSeam.cpp`.
- **`strat_banner_sweep.py`** printed `SWEEP CLEAN`, exit 0 - 435 entries all Success, macro
  census 435, 63 suite claims (1 live, 62 stamped).
- **`engine.md` attribution is correct** on the new entry: 2026-09-06,
  `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, base commit `92d78c1`, UNCOMMITTED),
  with the live suite figure and the phase verdict explicitly deferred to `global.md`. The
  finding above is about the entry's figures, not its attribution.
- **My own instrument nearly reported garbage this round, and did.** The first identifier
  extractor used the wrong capture group and printed the exclusivity noun where the identifier
  should have been - "caller", "writer", "consumer" as symbol names, with a 457-occurrence count
  for the word "caller". That is the failure mode worth recording: it was loud rather than
  silent, and a null from the same bug would have been indistinguishable from a clean sweep. It
  was rewritten and re-run before any conclusion was drawn from it.

VERDICT: BLOCK
