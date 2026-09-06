# Gate: unit damage alert (uncommitted working tree) -- strat-integration-reviewer

## Scope

Working tree of `E:\MultiAgent\Stratocracy`, branch `master`, base commit `7e83295`.
Everything audited is UNSTAGED / UNTRACKED; nothing is staged and nothing is committed.

`git status --porcelain` (verbatim):

```
 M Source/StratPlay/StratMatchSubsystem.cpp
 M Source/StratPlay/StratUnitActor.cpp
 M Source/StratPlay/StratUnitActor.h
 M Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp
 M Source/StratPlay/Tests/StratRouteTweenUnitDouble.h
 M Source/StratUI/StratSoundCues.cpp
 M Source/StratUI/StratSoundCues.h
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Source/StratPlay/Tests/StratDamageFlashClauses.cpp
?? Source/StratUI/Tests/StratDamagedUnitsClauses.cpp
```

12 files: 7 C++ (4 production, 3 test-lane), 3 record files, 2 of the 12 new.
`git diff --stat` totals 1354 insertions / 30 deletions across the 10 modified files.

I did not build and did not run the suite; `strat-test-author` owns that. I did run the
record instrument `strat_banner_sweep.py`, which is read-only.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`git diff --stat -- Source/StratRules Data`) | pass -- printed nothing |
| 2 | No `strat::` call outside `Source/StratBridge/` and `Source/StratRules/` | pass -- zero hits |
| 3 | No vendored header in front of UHT | pass -- one candidate, judged a false positive (below) |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass -- 2 members, both `TPimplPtr`; no `TUniquePtr<FStratBridge>` anywhere |
| 5 | Module arrows unchanged and correct | pass -- 16 rows, 10 field names, both derived this gate (inlined below) |
| 6 | New modules registered / `StratRules` still absent | pass -- no new module directory; `.uproject` `Modules` unchanged |
| 7 | `Source/Stratocracy/` untouched | pass -- `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in C++ | pass -- every hit is under a `Tests/` directory (`Source/StratBridge/Tests/*`), the standing exception; zero hits in any file this pass touched |
| 9 | No widget-side arithmetic | pass -- no `StratUI` widget was touched; the only `StratUI` production change is `StratSoundCues` |
| 10 | Lane discipline | pass -- production C++ all outside `Tests/`, all test code inside `Tests/`, no assets |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass -- `git status --porcelain` first column is a space on every modified path |
| A | `UpdateTickEnabled()` the ONLY caller of `SetActorTickEnabled(false)` | pass |
| B | `ApplyUnitView` unedited | pass |
| C | Nothing arms a flash from inside `ApplyUnitView` | pass |
| D | Tour-step flash OUTSIDE `FindSoundDirector()`'s `if` | pass |
| E | `Shake`'s retirement uses `SetRelativeLocation_Direct` + `UpdateComponentToWorld` | pass |
| F | `StratDecideDamagedUnits` contains no arithmetic | pass for the FUNCTION; the FILE-WIDE claim it re-asserts is inaccurate -- Observation 1 |
| G | No test-only advance seam; no production surface widened purely for a clause | pass on the seam; one production verb has no production caller -- Observation 2 |
| H | `StratUI` gained no dependency on `StratPlay` | pass |
| I | Acceptance IDs: no ID minted, `T-INT-05` spans both modules, names do not overclaim | pass, with Observation 3 |
| J | Record honesty: ownership, one suite count, unverified claims stated | pass, with Observations 4 and 5 |

### Check 5 -- the derivation, inlined

The instrument (extracted from the reviewer checklist and executed against this tree, not
retyped from memory):

```
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

Its output, this gate:

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

16 rows, all five modules present, matching the checklist block row for row. The sixteenth --
`StratBridge PrivateDependencyModuleNames StratRules` -- is the conditional one; I read
`StratBridge.Build.cs` and it is still inside the `if (!bCompileVendoredRulesHere)` guard, i.e.
monolithic targets only, which is sanctioned and not a finding. No `.Build.cs` was edited by
this pass.

Check H is answered by row 11: `StratUI` depends on `Core, CoreUObject, Engine, StratBridge`
publicly and `UMG, Slate, SlateCore` privately. `StratPlay` appears nowhere in it. No cycle.

The field census printed TEN names, matching the recorded figure:

```
BRIDGE=                               PCHUsage =
PrivateDefinitions.Add                PrivateDependencyModuleNames.Add
PrivateDependencyModuleNames.AddRange PublicDependencyModuleNames.AddRange
PublicIncludePaths.AddRange           ShadowVariableWarningLevel =
bCompileVendoredRulesHere =           bUseUnity =
```

`BRIDGE=` is the known false positive: the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` inside `PrivateDefinitions.Add`. No eleventh name.

### Check 3 -- the one candidate, judged

`Source/StratBridge/StratBridge.h` is the only header matching both `UCLASS|USTRUCT|UENUM` and
an include of a vendored header. All six of its matches are PROSE arguing the rule -- line 262
reads `// this header must declare no UCLASS/USTRUCT/UENUM, because`. It declares no reflected
type. False positive, and unchanged by this pass. A type name is not a declaration, exactly as
a type name is not a call.

### Checks A-E -- the evidence

**A.** `grep -n "SetActorTickEnabled" Source/StratPlay/StratUnitActor.cpp` returns five code
lines: three `SetActorTickEnabled(true)` at 509, 785 and 1044 (`ApplyUnitView`'s arming branch,
`PlayRouteSlide`, `PlayDamageFlash` -- the ON direction, which cannot strand anything), and one
`false`-capable call, line 1167, inside `UpdateTickEnabled`:

```cpp
SetActorTickEnabled(TweenOffsets.Num() > 0 || DamageFlashElapsed > 0.0f);
```

`FinishTween` no longer writes the flag; its trailing statement is now `UpdateTickEnabled();`
at `StratUnitActor.cpp:1001`, and `FinishDamageFlash` ends the same way. There is no direct
`false` in either `Finish*`. The invariant holds.

**B.** `AStratUnitActor::ApplyUnitView` begins at `StratUnitActor.cpp:274`. The hunk headers
from `git diff -U0` are at old lines 60, 66, 68, 76, 90, 103, 112, 189, 800 and 943 -- none
falls inside the function. It is unedited, so the overlay-channel argument that justifies the
whole design still holds rather than having quietly stopped.

**C.** The only production callers of `PlayDamageFlash` are `StratMatchSubsystem.cpp:1173` and
`:2385`. Neither is in `ApplyUnitView` -- `ApplyView` on the subsystem is a different function
from `ApplyUnitView` on the actor, and nothing arms from the idempotent total refresh that a
hover crossing a hex boundary reaches.

**D.** Read at `StratMatchSubsystem.cpp:2379-2386`. The tour-step flash is a SIBLING of the
director block, not a child of it -- the `if (UStratSoundDirector* const Director = ...)` with
its `switch` closes above it:

```cpp
	if (Current.Kind == EStratAiCommandKind::Attack && Current.TargetId >= 0)
	{
		if (AStratUnitActor* const Defender = FindUnitActor(Current.TargetId))
		{
			Defender->PlayDamageFlash();
		}
	}
```

The plan's `case Attack:` placement was corrected as the engineer reported. It flashes
`Current.TargetId`, the defender, not `Current.UnitId`. The `ApplyView` site at `:1168-1175` is
likewise inside `if (!bTourExistenceHeld)` and BELOW the director `if`'s closing brace, and
`StratDecideDamagedUnits(SoundMark, Model, Damaged)` is called at `:1111`, ABOVE the
`SoundMark = StratSoundMarkFromView(Model);` re-mark at `:1113` -- the ordering the whole
feature depends on. Both placements are correct.

**E.** `FinishDamageFlash`, `StratUnitActor.cpp:1153-1154`:

```cpp
		Shake->SetRelativeLocation_Direct(FVector::ZeroVector);
		Shake->UpdateComponentToWorld();
```

`TickDamageFlash` uses the ordinary `SetRelativeLocation` for its in-flight displacement, which
is correct and not a finding: the ~1e-14 round-trip residue only matters for the terminal exact
zero, which is what `IsZero()` is asserted against.

## Findings

None.

## Observations

Non-gating.

**1. `StratSoundCues.cpp`'s FILE-WIDE "not one arithmetic operation" claim is inaccurate, and
this pass re-underwrites it.** The new function is clean -- `StratDecideDamagedUnits` compares
`Unit.Hp < Before.Hp` and appends an id, with no operator anywhere, so check F passes for the
function and the design intent (no HP delta, no amount reaching a drawing layer) is honoured.
But the file's opening claim is stated of the WHOLE FILE -- "THERE IS NOT ONE ARITHMETIC
OPERATION IN IT. No subtraction, no sum, no ratio, no distance" -- and
`Source/StratUI/StratSoundCues.cpp:86` is a sum:

```cpp
	bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1] = {};
```

That line is PRE-EXISTING -- unchanged at `7e83295` and outside every hunk in this diff -- and
it is a compile-time array bound over an enum constant rather than a decision or an operation on
data, which is why it does not touch the substantive claim the block defends. It is still a `+`,
and the same block's own next paragraph calls that array out by name ("THE ONE-PER-KIND COLLAPSE
IS A `bool[7]`"), so the two sentences are already in tension without this pass. What makes it
worth recording NOW is that this pass added a paragraph re-affirming the claim's survival --
"AND IT SURVIVED `StratDecideDamagedUnits` JOINING IT ON 2026-09-06" -- which is the moment a
reader is invited to trust the wording again. The honest repair is one clause naming the array
bound as the single exception; the claim then becomes true instead of nearly true. Owner:
`strat-gameplay-engineer`.

**2. `AStratUnitActor::CancelDamageFlash` has no production caller.** Check G passes on what it
was written for: `Tick` is still `protected`, `TickMoveTween` and `TickDamageFlash` are
`private`, no advance entry point exists, and the 2026-09-02 refusal recorded in
`Source/StratPlay/Tests/StratMatchReconcile.cpp` -- "`Tick` is protected and stays so; no
test-only advance entry point was asked for or added, and a future clause that wants the curve
needs a PIE fixture rather than a seam" -- is honoured by both lanes. But `CancelDamageFlash` is
a public verb whose only callers are in `Tests/`, and its declaration says so itself: "IT IS
`FinishDamageFlash` MADE PUBLIC, AND THAT IS DELIBERATE ... so a clause has no way to let a flash
expire." Its cited precedent is not exact -- `CancelRouteSlide` has a production caller at
`Source/StratPlay/StratMatchSubsystem.cpp:2598` (`Entry.Value->CancelRouteSlide();`) and this
one has none. I judge it NOT a BLOCK: it exposes the ONE existing retirement path rather than
adding a second, it changes no behaviour on any shipped path, and the widening is argued in the
header rather than smuggled. The asymmetry with the precedent is real and is recorded here so it
is not inherited as settled. Owner: `strat-gameplay-engineer`, if anyone acts on it.

**3. One clause name is looser than what it pins.**
`Stratocracy.StratPlay.T-INT-05.DamageFlashArmsAShakeAndAnOverlay` asserts that the clock is
strictly positive, the actor tick is on, the overlay channel holds the pointer the fixture
handed over -- and that `Shake` HAS NOT MOVED (`ShakeRelativeLocation().IsZero()`). No shake
displacement is observed or observable, because `TickDamageFlash` never runs headless. The
clause's own banner is explicit about this and every assertion message is honest, so a reader who
opens the file is not misled; a reader who meets only the name in a suite listing is. Non-gating,
and the file is otherwise unusually careful about exactly this hazard. Owner: `strat-test-author`.

Acceptance IDs otherwise check out. No new ID was minted. `T-INT-05` genuinely spans both
modules already -- it appears in `Source/StratUI/Tests/StratViewModelParity.cpp` (the cited
precedent), in `Source/StratUI/Tests/StratGuidanceStripClauses.cpp`,
`StratMatchResultModelClauses.cpp`, `StratMatchResultRouting.cpp` and
`StratViewingSideSelectorClauses.cpp`, in `Source/StratBridge/Tests/StratMatchResultClauses.cpp`
and `StratRosterDeltaClauses.cpp`, and across `Source/StratPlay/`. The nine new clauses are six
`Stratocracy.StratPlay.T-INT-05.*` and three `Stratocracy.StratUI.T-INT-05.*`, counted by
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` macro (6 and 3) rather than by name.

**4. `Tools/architect/state/global.md`'s new banner entry does not close its italic marker.**
Every prior entry ends `...)_`; the new one ends `...needs an editor session.)` with no trailing
underscore, so the `_` opened at `_Last run 2026-09-06 (A DAMAGED UNIT NOW FLASHES RED AND
SHAKES...` runs on into the entry below it as far as a Markdown renderer is concerned. Cosmetic,
and `strat_banner_sweep.py` does not see it. Owner: `coordinator` / `strat-data-steward`.

**5. Record honesty -- checked, clean, and recorded BECAUSE it was checked.** `engine.md`'s new
entry opens "2026-09-06, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in
the main..." and `tests.md`'s opens "2026-09-06 (local), `strat-test-author` (ACTING and WRITING;
IN LANE -- two new files,...". Each lane wrote its own file, attribution is INLINE in the entry
rather than only in a header, and neither the transcription clause nor the in-session clause was
invoked or needed -- `global.md` says so in terms ("this entry is not a transcription of
anything"). Neither lane file states a suite count: grepping their added lines for an `N/N` shape
and for any bare `4xx` integer returns nothing. The live figure is stated once, in `global.md`,
and the superseded `435/435` sentences carry their stamps ABOVE the false claims rather than
below them.

`strat_banner_sweep.py`, executed at this gate:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 444 entries, all Success (2026.09.06-17.47.11)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=444, COMPLEX/_CLASS=0
  report identity: mtime=1788716832, reportCreatedOn='2026.09.06-17.47.11'
  suite claims found: 64 (1 live, 63 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

The banner's account of what is NOT verified is accurate and is not softened, which was the
specific thing this gate was asked to check. It states that `DamageFlashMaterial` ships unset,
that the feature is invisible in PIE and NO HUMAN HAS SEEN IT, and that not one clause pins
amplitude, frequency, the X/Y ratio, decay shape, smoothness or redness. It states that the
marker re-parent is UNMEASURED against `BP_StratUnit` and that a warning-free build is not
evidence about reinstancing because the editor was never opened in this pass. And it states that
the tour path flashes the defender only, because a counterattack takes HP off the attacker and no
`FStratAiPlaybackStep` field reports it. All three match the tree as I read it -- the last one is
exactly what `StratMatchSubsystem.cpp:2385`'s guard on `Current.TargetId` produces, and the
source comment there records the same limit in the same direction rather than a weaker one. The
banner additionally declares its own obligation-(1) unfalsifiability about the route offer, which
is the correct posture rather than a hedge.

**6. Two dependent claims I could not check, stated so they are not read as passed.** I did not
build and did not run the suite, so `444/444` and the `reportCreatedOn 2026.09.06-17.47.11`
identity are the test lane's and the coordinator's measurement rather than mine; the sweep output
above is consistent with both, but it reads the same exported report and is therefore not an
independent second instrument on the report's contents (its macro census IS independent of the
report, and it agrees). And no instrument in this tree -- this gate included -- can say that
anything red ever reached a screen. That remains a human at the keyboard, exactly as the banner
says.

VERDICT: PASS
