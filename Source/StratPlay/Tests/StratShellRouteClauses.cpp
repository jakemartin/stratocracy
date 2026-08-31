// The gate on §2.11.5's shell -- `UStratShellSubsystem`'s route decisions -- under the local
// gate name `GATE-TITLEMENU`, plus one clause under `T-TURN-09`.
//
// WHY THE GATE NAME AND NOT AN ACCEPTANCE ID. There is no acceptance ID for this screen. The
// subsystem's own header records the measurement: §2.11.5's last paragraph is the only place
// the title screen appears in the GDD, it names no route, and §2.11.8's must-have ranking does
// not list the screen at all. Inventing an ID here would put a string in a test name that greps
// exactly like a defined one, which this project has already been bitten by. `GATE-TITLEMENU`
// is a local gate name, ruled by the user on 2026-08-29, and these are its first clauses.
//
// WHERE THE EXPECTATIONS COME FROM, WHICH IS THE ONLY INTERESTING QUESTION ABOUT THIS FILE.
// There is no rules-module oracle for a menu: `strat::` knows nothing about screens, and the
// GDD states no route set. So the ordinary parity shape -- ask the module for the number, then
// compare -- has no module to ask. The answer this file takes is to pin RELATIONS BETWEEN TWO
// INDEPENDENTLY-WRITTEN FUNCTIONS rather than to retype the switch statements as expectations:
//
//   - The menu model's order and length are compared against `StaticEnum<EStratShellRoute>()`,
//     i.e. against UHT's own record of the declaration order. Not a literal list.
//   - Every row's `bEnabled` and `DisabledReason` are compared against what `IsRoutePermitted`
//     returns for that row's route under the same facts. Module-side value, read not computed.
//   - Both refusal-precedence clauses derive the sentence they expect FROM THE FUNCTION UNDER
//     TEST, taken under a fact set in which only one refusal is possible, and then assert it
//     appears under the fact set in which both are. No refusal string is typed in this file.
//   - `ResolveDestination` is compared against `IsRoutePermitted` rather than against a list of
//     which route opens which level.
//
// The one place a literal is unavoidable is the shape of the model itself (four rows, one per
// enumerator) and even that is read from reflection. Nothing in this file retypes a label, a
// refusal or a precondition.
//
// THE `FString` HAZARD AND HOW EACH ASSERTION DEFENDS AGAINST IT. `==`, `Contains` and
// `TestEqual` are all CASE-INSENSITIVE in this engine -- measured in this project, where it
// once produced a clause that could not fail. This file uses the two comparisons in opposite
// directions on purpose:
//   - Where the claim is SAME, the comparison is `ESearchCase::CaseSensitive`, which is the
//     STRICTER reading of sameness.
//   - Where the claim is DIFFERENT, the comparison is the default case-insensitive one, which
//     is the STRICTER reading of difference: two strings that differ only in case are reported
//     as equal and the clause goes red.
// A clause that asserted "different" case-sensitively would pass on a mutation that only
// changed capitalisation, and one that asserted "same" case-insensitively would pass on the
// same mutation. Neither shape appears below.
//
// WHAT NO CLAUSE IN THIS FILE REACHES, SAID PLAINLY RATHER THAN LEFT TO BE INFERRED.
//
// THREE OF THESE ITEMS WERE WRITTEN IN THE FIRST PASS, WERE MADE FALSE BY THE FOLLOW-UP PASS,
// AND ARE CORRECTED IN PLACE RATHER THAN DELETED. `strat-integration-reviewer` returned BLOCK
// on them and was right to: the follow-up section further down stamped ITSELF as history and
// left this list standing, so the file asserted a gap and its closure at the same time and a
// reader landing here was sent to redo pinning that already exists. That is this project's
// recorded "a correction that only adds leaves both claims live" failure, and the fix for it is
// never a deletion -- a reader arriving at a retracted sentence by a citation has to learn that
// it was wrong and what replaced it, which a deleted sentence cannot tell them.
//
//   - `ExecuteRoute`.
//     RETRACTED>  "It calls `OpenLevelBySoftObjectPtr` and `QuitGame`; neither survives a
//     RETRACTED>   headless run. The subsystem's header says so and it is correct that it
//     RETRACTED>   says so."
//     **[STAMPED 2026-08-30]** TRUE OF THE TRAVELLING PATH ONLY, AND THE FILE ALWAYS KNEW IT:
//     `FStratShellRefusedRouteArmsNothingTest` below calls `ExecuteRoute` for real. When
//     `IsRoutePermitted` refuses, the function returns before `RouteTravels`, before
//     `ResolveDestination`, before the arming and before either engine call, so nothing on
//     that path needs a viewport, a world or a level. What remains unreachable is the
//     TRAVELLING path: the single engine call, and the order of everything after the
//     permission check.
//
//   - WHICH SLOT `ExecuteRoute` ARMS. **STILL NOT REACHED. See the list below, where this item
//     now lives.** It is kept here as a heading because two stampings have pointed at it.
//     RETRACTED>  "It calls `ArmPendingLoadSlot(SaveSlotName)`, and that pairing exists in
//     RETRACTED>   exactly one place, inside the untestable function. Every clause below stays
//     RETRACTED>   green if it armed the wrong string. See this file's report; it is a
//     RETRACTED>   finding, not a coverage claim."
//     **[STAMPED 2026-08-30, FIRST CORRECTION]** The quoted line no longer exists; it reads
//     `ArmPendingLoadSlot(PendingSlotForRoute(Route, SaveSlotName))`, and the DECISION was
//     extracted into the world-free static `PendingSlotForRoute` precisely because this finding
//     measured that its absence was unfalsifiable.
//     RETRACTED>  "CLOSED ... Five clauses under `THE SLOT PAIRING` below pin it ... An
//     RETRACTED>   `ExecuteRoute` that armed the wrong string is now red, which is the exact
//     RETRACTED>   opposite of what the retracted sentence says."
//     **[STAMPED 2026-08-30, SECOND CORRECTION] THAT WAS FALSE, AND IT IS THE SUBJECT-VS-SCOPE
//     SLIP THE FIRST CORRECTION WAS ITSELF ABOUT.** The five clauses pin
//     `UStratShellSubsystem::PendingSlotForRoute`, THE STATIC. This item's own heading names
//     WHICH SLOT `ExecuteRoute` ARMS -- a different subject, and no clause reaches it.
//
//       THE CALL-SITE CENSUS, STATED ONCE HERE AND POINTED AT FROM EVERYWHERE ELSE IN THIS
//     FILE THAT NEEDS IT -- "in this file" being the honest scope: `UStratShellSubsystem`'s own
//     `ExecuteRoute` doc reaches the same result independently and in its own words. That is a
//     second statement rather than a pointer, and it is tolerable only because BOTH NAME THE
//     SAME TRIGGER -- a second `ExecuteRoute` caller appearing -- so they rot together and each
//     announces its own invalidation. Do not add a third. Run this from a checkout:
//         grep -rn "ExecuteRoute(" Source/ | grep -v "://" | grep -- "->ExecuteRoute("
//     It returns ONE line: the call in `FStratShellRefusedRouteArmsNothingTest`, whose own
//     fixture asserts the route is REFUSED -- so that call returns before `ArmPendingLoadSlot`
//     is ever reached. Nothing reaches that line, so editing it cannot redden anything.
//
//       THE MIDDLE FILTER IS NOT DECORATION AND THE CENSUS IS WRONG WITHOUT IT. This paragraph
//     CONTAINS the pattern it tells you to search for, so the unfiltered grep matches this
//     comment too -- measured on 2026-08-30 at FOUR hits, of which THREE were this file's own
//     prose and one was the call. That is this project's recorded "a census that quotes its own
//     search token" failure, arriving in the very sentence that exists to be re-executable.
//     `grep -v "://"` drops every `file:line://...` comment hit, so the count stays right no
//     matter how many times this file mentions the token again -- which it will.
//       AND IT IS CITED BY SYMBOL, NEVER BY LINE. That call site moved from line 1421 to 1489
//     during the very editing pass that wrote this paragraph. A `file:NNN` citation would
//     already be stale; the enclosing test's name is not.
//
//       WHY THIS DERIVATION IS WORTH MORE THAN THE MUTANT IT REPLACED, and this is the part to
//     keep: it says WHY nothing catches a change to that line -- there is no caller that
//     reaches it -- where a mutant could only ever report THAT nothing did. It also stands on
//     its own feet: it needs no mutant to have been run, so it does not inherit the
//     red-implies-relink reasoning below or any of its caveats.
//       IT HAS A TRIGGER, AND THE TRIGGER IS THE HONEST PART: this paragraph stops being true
//     the moment a SECOND `ExecuteRoute` caller appears. Whoever adds one owns rewriting it.
//     Re-run the census above before trusting a word of it.
//       WHAT THE EXTRACTION DID BUY, so the record does not now swing to the opposite
//     over-claim: the DECISION is pinned five ways where it was pinned none, which is strictly
//     more than before and is worth what it cost. What it did not buy is `ExecuteRoute` PASSING
//     that decision to `ArmPendingLoadSlot`. One line carries two properties -- WHICH VALUE it
//     passes and WHETHER it passes unconditionally -- and both are unpinned, for the same
//     reason: reaching that line means travelling.
//
//   - `GatherFacts`. STILL TRUE, AND UNCHANGED BY EITHER PASS. It needs a `UGameInstance` with
//     a world and a save file. The clauses below take the facts as data, which is the whole
//     reason the deciders are static. (`FStratShellRefusedRouteArmsNothingTest` causes
//     `GatherFacts` to RUN, on an instance with no world, but asserts nothing about what it
//     returns -- executing a function is not pinning it.)
//
//   - T-TURN-09's OWN SENTENCE. STILL TRUE. See the note above that clause.
//
// WHAT IS STILL NOT REACHED AFTER THE FOLLOW-UP PASS, stated so the corrections above are not
// read as claiming the whole function is covered. NOTE THE SHAPE OF THIS LIST: it says which
// PROPERTIES no clause here pins, and it does not enumerate what `ExecuteRoute` contains --
// `StratShellSubsystem.h` is the authority for that and a copy of it here would go stale the
// next time a line moves, which is a drift that has already happened once in that header.
//   - The permitted arm of `ExecuteRoute` in its entirety: reaching it means travelling. The
//     two items below are both consequences of this one and are named separately because each
//     has been mistaken for covered.
//   - WHICH SLOT `ExecuteRoute` ARMS -- i.e. that it passes `PendingSlotForRoute`'s answer, and
//     not some other string, to `ArmPendingLoadSlot`. The five slot clauses pin what
//     `PendingSlotForRoute` RETURNS; nothing pins what `ExecuteRoute` DOES WITH IT. Moved down
//     here from the list above on 2026-08-30, where it had been wrongly marked closed.
//   - THAT `ExecuteRoute`'S ARMING CALL IS UNCONDITIONAL. Putting the `if` back around it
//     reddens NOTHING. The clearing clause below composes the same two public entry points but
//     cannot see whether `ExecuteRoute` still composes them that way.
//       RETRACTED>  "measured with a built mutant over the whole suite, not reasoned."
//     **[STAMPED 2026-08-30]** THE CONCLUSION IS UNCHANGED AND THE CITATION WAS THE WEAKEST
//     AVAILABLE. It cited a GREEN mutant run -- and a green mutant cannot tell an unreachable
//     line from a build that never happened, which this branch then measured the hard way (see
//     `WHY A GREEN MUTANT PROVES NOTHING` below). THE SAME CALL-SITE CENSUS THAT CARRIES THE
//     SIBLING ITEM ABOVE CARRIES THIS ONE -- read it there rather than here, because two copies
//     of one derivation is how a correction goes stale on one of its faces. It settles both
//     properties at once for the same reason: they are the same unreachable line.
//     THESE TWO ITEMS ARE THE SAME SINGLE LINE OF CODE, and a reader who closes one has not
//     closed the other: a clause that reached that line would likely pin both at once, and
//     nothing short of reaching it pins either.
//   - Any ordering beyond "the permission check precedes the arming", which is the one
//     ordering fact the refusal path makes observable.
//
// =======================================================================================
// WHY A GREEN MUTANT PROVES NOTHING, AND HOW THIS BRANCH FOUND THAT OUT THE HARD WAY.
//
// READ THIS BEFORE YOU TRUST A MUTANT RUN. The practice these clauses were written under is
// that each is justified by a built mutant -- break the thing the clause pins, rebuild, re-run,
// watch it go red.
//   RETRACTED>  "Every clause in this file is justified by a built mutant."
// **[STAMPED 2026-08-30]** NARROWED, because as a universal over 23 clauses that sentence was
// an agent's self-report about runs a reader cannot see, and this file's own rule three
// paragraphs down forbids exactly that kind of citation. Worse, the sentence was written in the
// same wave in which TWO of those mutant runs turned out not to have happened at all.
//   WHAT A READER CAN ACTUALLY CHECK, STATED WITHOUT FLATTERING IT. Not the mutants: they are
// not in this tree in any form, and `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md`
// says so in terms -- no gate round built the project or ran the suite, and the whole mutant set
// "remain agent self-reports". Citing that file for a per-clause mutant table would be the same
// defect again, one directory over. What IS checkable is: the census command above, run from a
// checkout; the clause bodies themselves; and, for the two clauses that once leaned on a green
// mutant, the derivations that replaced it. Where a clause rests on something other than a
// mutant, that is said AT the clause with the command to re-run.
//
// THE FAILURE THAT MAKES THIS SECTION WORTH ITS LENGTH. On 2026-08-30 two clauses in this
// branch came back GREEN under mutants written specifically to redden them, and the obvious
// reading -- "the clauses are inert" -- was FALSE. The mutations were never in the binary. The
// builds were no-ops that reported success.
//
// THE PROOF SHAPE, because the conclusion is worth less than the method:
//   1. An unconditional `AddError` was inserted into the clause under test. The suite still
//      reported every test passing. A clause that runs CANNOT pass with an unconditional error
//      in it, so the clause was not running the code on disk.
//   2. `grep` on the built `UnrealEditor-StratPlay.dll`: the probe string ABSENT, the clause
//      NAME PRESENT. The binary was built from an older revision of the same file -- which is
//      exactly the state in which every name looks right and every behaviour is stale.
//   3. The DLL was deleted and the build re-run. It reported `Result: Succeeded` AND DID NOT
//      PRODUCE A DLL AT ALL. That is the whole finding in one line.
//
// SO: `Result: Succeeded` IS NOT EVIDENCE THAT A BUILD HAPPENED. THE ARTIFACT IS. Check that
// the binary's timestamp actually advanced before believing any mutant result. This is the same
// species as this project's recorded "exit code is not a verdict", arriving through a different
// door: the build tool's success string describes its own decision, not its output.
//
// AND WHEN YOU GREP THE BINARY, GREP FOR SOMETHING THAT MOVED. A timestamp says a link
// happened; it does not say WHICH SOURCE went into it. Searching the DLL for an identifier that
// has been in the tree for several rounds proves only "some build since that round" -- it
// cannot distinguish this round's binary from the last one, which is the very question a stale
// build makes urgent. The discriminating probe is a string that CHANGED IN THIS ROUND: the
// superseded wording should occur zero times in both source and binary, and the current wording
// should occur in both. That is a one-line check and it is strictly stronger than the
// unchanged-token version of itself, which reads as reassuring and settles nothing.
//   AND ONLY STRINGS THAT COMPILE CAN BE PROBED, which rules out most of what you may want to
// use. A comment in a `.cpp` reaches no binary at all -- so a comment-only round in THIS file
// leaves nothing to grep for, and its freshness rests on timestamps alone. What does survive is
// a `TEXT()` literal, a clause name, a symbol, and -- the non-obvious one -- a doc comment above
// a `UPROPERTY`/`UFUNCTION` in a HEADER, which UHT compiles into `Comment`/`ToolTip` metadata.
// That last is why an identifier this branch DELETED still greps in the DLL: it survives as
// quoted history inside a property's tooltip. Measured on this branch: the header's current
// rename note reads 1 in the header and 2 in the DLL. Check the sense of a hit before reporting
// it -- a removed identifier found in a binary is alarming, and here it was evidence of
// freshness rather than staleness.
//
// THE CAUSE, so it can be avoided rather than merely detected: these mutants are built in a
// disposable COPY of the tree, because a test author may not edit the code under test even
// temporarily. A copied tree carries a copied `Intermediate/Build`, whose cached build graph
// still resolves paths in the tree it was generated in -- so the build system concludes there
// is nothing to do, and says so successfully. `rm -rf Intermediate/Build` in the copy fixes it,
// at the cost of a full module rebuild. Do that FIRST, once, rather than trusting the first
// green.
//
// THE ASYMMETRY THAT SURVIVES ALL OF THIS, AND IT IS WHY THE RED RESULTS STAND:
//   - A RED mutant implies the mutation reached the binary -- a no-op build leaves the baseline
//     binary in place, and the baseline is green. So red ==> a relink incorporating the change,
//     ABSENT A FLAKE. The qualifier is not decoration: this project has recorded a ~1-in-4 red
//     flake, so the inference is "red ==> relinked, absent a flake" and never the absolute
//     form. Across a mutant set whose every red is semantically matched to its own subject --
//     the clause that went red is the clause that names the broken property -- a flake is not
//     a credible explanation, but it is the reader's judgement to make, not this comment's to
//     foreclose.
//   - A GREEN mutant implies NOTHING WHATEVER until the artifact is checked. It is equally
//     explained by an inert clause and by a build that never ran. Any claim resting on a green
//     mutant is therefore weaker than it looks, and this file no longer makes one: the two
//     places that did have been retracted in place above and now rest on the call-site census
//     instead. Put the qualifier where it belongs and the enumeration survives: a red still
//     proves a relink happened, absent a flake -- but nothing rescues a green.
//
// THE RULE THIS IS ONE HALF OF, AND IT IS NOT A TEST-LANE RULE. The engine lane arrived at the
// same place from four findings of its own in the same wave -- a miscounted "three lines", an
// unmeasured "only thing", a doc quoted from memory, and this discarded mutant -- and every one
// was the same defect wearing a different coat: A SENTENCE WHOSE CONCLUSION WAS RIGHT AND WHOSE
// WARRANT WAS NEVER MEASURED. The conclusions mostly survived; the citations mostly did not.
//
//   AN EVIDENCE CITATION SHOULD NAME SOMETHING A READER CAN RE-EXECUTE FROM A CHECKOUT.
//
// A grep qualifies -- provided it is not the self-quoting kind, which is why the census above
// carries its filter and its trap. A build somebody ran does not. A mutant somebody ran and
// discarded does not. A document as remembered does not. A persisted gate report DOES.
//   RETRACTED>  "A persisted gate report would, and this tree holds none, which is why
//   RETRACTED>   "See the report" was struck from this file earlier in the same wave."
// **[STAMPED 2026-08-30] FALSE, AND IT INVERTED ITS OWN REMEDY.** `Tools/architect/gate_reports/`
// exists, is tracked, is not ignored, and held FIVE reports at `347c722` -- one of them
// `2026-08-30-ci-build-and-suite.md`, dated the day this branch was written. Check it with
//     git ls-tree -r --name-only 347c722 -- Tools/architect/gate_reports
// The two `See the report` citations were struck on the belief that persisting a gate report is
// not a thing this tree does. IT IS EXACTLY WHAT THIS TREE DOES, in a tracked directory built
// for it, and the correct fix was to WRITE the report and KEEP the citations. So a paragraph
// three lines from the rule it lands on deleted re-executable evidence for want of a mechanism
// that was already there.
//   AND IT IS THIS BRANCH'S OWN DEFECT, COMMITTED IN THE PARAGRAPH DEFINING IT: a conclusion
// that was defensible and a warrant that was never measured. The warrant here was a claim about
// files in a directory, asserted without ever listing the directory -- one `git ls-tree` away,
// never run. Both citations now point at `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md`,
// which carries this wave's gate rounds including the two that blocked on green-mutant citations
// and the one that blocked on this sentence.
//   HOW IT GOT IN, in one line, with the detail left where it belongs: the claim was INHERITED
// from a gate that had measured the absence with two instruments which could not have found the
// files -- the wrong directory, and a `-newer` filter that excluded them by timestamp. Both are
// set out in `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md`; both were re-measured
// here and confirmed, and neither is restated here, because a second copy of someone else's
// measurement is how the next correction goes stale on one of its faces. THE RULE BELOW APPLIES
// TO YOUR OWN INPUTS TOO: a measurement handed to you is not a measurement you made.
//
// The dead-instrument finding above is the sharpest case rather than a separate lesson: the
// citation was a mutant run, the mutant run never happened, AND NOTHING IN THE TREE COULD HAVE
// TOLD A READER THAT. Prefer the evidence a stranger can reproduce, even when the evidence you
// have is real -- because a reader cannot tell your real one from your mistaken one, and
// neither, it turns out, could you.
// =======================================================================================

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "StratMatchSubsystem.h"
#include "StratSaveGame.h"
#include "StratShellGameMode.h"
#include "StratShellSubsystem.h"

namespace StratShellClauseFixture
{
	/** The four booleans of `FStratShellFacts` as one integer, so a clause can sweep all 16. */
	constexpr int32 FactCombinationCount = 16;

	FStratShellFacts FactsFromBits(const int32 Bits)
	{
		FStratShellFacts Facts;
		Facts.bMatchLevelConfigured = (Bits & 1) != 0;
		Facts.bTitleLevelConfigured = (Bits & 2) != 0;
		Facts.bSaveSlotIsRestorable = (Bits & 4) != 0;
		Facts.bMatchIsLive          = (Bits & 8) != 0;
		return Facts;
	}

	FString DescribeFacts(const FStratShellFacts& Facts)
	{
		return FString::Printf(
			TEXT("[match level %s, title level %s, save %s, live %s]"),
			Facts.bMatchLevelConfigured ? TEXT("set") : TEXT("unset"),
			Facts.bTitleLevelConfigured ? TEXT("set") : TEXT("unset"),
			Facts.bSaveSlotIsRestorable ? TEXT("restorable") : TEXT("not restorable"),
			Facts.bMatchIsLive          ? TEXT("yes") : TEXT("no"));
	}

	/**
	 * The declared routes, read from reflection rather than typed here.
	 *
	 * UHT appends a hidden `_MAX` enumerator to every `UENUM`, which is not a route; it is
	 * dropped by name, CASE-SENSITIVELY, so that a future enumerator legitimately called `Max`
	 * would still be counted as the route it is.
	 */
	bool DeclaredRoutes(TArray<EStratShellRoute>& OutRoutes)
	{
		OutRoutes.Reset();

		const UEnum* const RouteEnum = StaticEnum<EStratShellRoute>();
		if (RouteEnum == nullptr)
		{
			return false;
		}

		for (int32 Index = 0; Index < RouteEnum->NumEnums(); ++Index)
		{
			if (RouteEnum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"),
					ESearchCase::CaseSensitive))
			{
				continue;
			}
			OutRoutes.Add(static_cast<EStratShellRoute>(RouteEnum->GetValueByIndex(Index)));
		}

		return OutRoutes.Num() > 0;
	}

	/**
	 * Two soft pointers to levels that do not exist, and the non-existence is the point.
	 *
	 * `ResolveDestination` claims to decide on `IsNull()` and never to load. If that claim were
	 * false these paths would fail to resolve and the clauses would say so. They also differ
	 * from each other in more than capitalisation, so a case-insensitive comparison cannot
	 * report them as the same level.
	 *
	 * A `/Game/` LITERAL IN AN AUTOMATION FIXTURE IS THE PROJECT RULE'S OWN STANDING EXCEPTION.
	 */
	TSoftObjectPtr<UWorld> FixtureMatchLevel()
	{
		return TSoftObjectPtr<UWorld>(
			FSoftObjectPath(TEXT("/Game/StratShellFixture/NoSuchMatchMap.NoSuchMatchMap")));
	}

	TSoftObjectPtr<UWorld> FixtureTitleLevel()
	{
		return TSoftObjectPtr<UWorld>(
			FSoftObjectPath(TEXT("/Game/StratShellFixture/NoSuchTitleMap.NoSuchTitleMap")));
	}
} // namespace StratShellClauseFixture

// ---------------------------------------------------------------------------------------
// The model's shape.
//
// WHAT IT PINS. `BuildMenuModel` returns one row per declared route, in the enum's own
// declaration order, in every one of the sixteen fact combinations -- so every other clause in
// this file is entitled to address a row BY INDEX rather than by matching its label text, which
// on a case-insensitive engine would be the weaker instrument.
//
// WHAT WOULD HAVE TO BE TRUE FOR IT TO PASS OVER BROKEN CODE. `BuildMenuModel` would have to
// emit the right routes in the right order and be wrong about something else entirely -- which
// is the case, and is why the other clauses exist. It cannot pass over a model that hides a row
// when the row is unavailable, which is §2.11.5's own rule and the failure this clause is
// pointed at.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellMenuShapeTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.MenuCarriesEveryDeclaredRouteInReflectedOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellMenuShapeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	TArray<EStratShellRoute> Declared;
	if (!DeclaredRoutes(Declared))
	{
		AddError(TEXT("EStratShellRoute did not resolve through reflection; this clause has "
			"observed nothing and will not report that as agreement."));
		return false;
	}

	for (int32 Bits = 0; Bits < FactCombinationCount; ++Bits)
	{
		const FStratShellFacts Facts = FactsFromBits(Bits);
		const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);

		TestEqual(FString::Printf(TEXT("row count %s"), *DescribeFacts(Facts)),
			Model.Options.Num(), Declared.Num());

		if (Model.Options.Num() != Declared.Num())
		{
			continue;
		}

		for (int32 Index = 0; Index < Declared.Num(); ++Index)
		{
			TestEqual(
				FString::Printf(TEXT("row %d route %s"), Index, *DescribeFacts(Facts)),
				static_cast<int32>(Model.Options[Index].Route),
				static_cast<int32>(Declared[Index]));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------------------
// One authority for a precondition.
//
// WHAT IT PINS. Every row's `bEnabled` and every row's `DisabledReason` are what
// `IsRoutePermitted` said for that route under those facts -- not a second copy of the same
// conditions written inside `BuildMenuModel`. THE EXPECTATION IS READ FROM THE MODULE, which is
// the only clause shape this project accepts when a module-side value exists.
//
// SAID PLAINLY: WHILE `BuildMenuModel` DELEGATES, THIS CLAUSE CANNOT FAIL. That is not a defect
// in it, it is its subject. It goes red the moment somebody inlines or restates a condition in
// the model builder, which is exactly the drift that would let a greyed row and a refused
// `ExecuteRoute` start saying different sentences.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellRowsFollowPermissionTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.EveryRowsEnablementAndReasonComeFromIsRoutePermitted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellRowsFollowPermissionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	for (int32 Bits = 0; Bits < FactCombinationCount; ++Bits)
	{
		const FStratShellFacts Facts = FactsFromBits(Bits);
		const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);

		for (const FStratShellOption& Option : Model.Options)
		{
			FText Refusal;
			const bool bPermitted =
				UStratShellSubsystem::IsRoutePermitted(Option.Route, Facts, Refusal);

			TestTrue(
				FString::Printf(TEXT("route %d enablement %s"),
					static_cast<int32>(Option.Route), *DescribeFacts(Facts)),
				Option.bEnabled == bPermitted);

			// SAME, so the comparison is case-sensitive: the stricter reading.
			const FString Expected = bPermitted ? FString() : Refusal.ToString();
			TestTrue(
				FString::Printf(TEXT("route %d reason is the permission's own words %s"),
					static_cast<int32>(Option.Route), *DescribeFacts(Facts)),
				Option.DisabledReason.ToString().Equals(Expected, ESearchCase::CaseSensitive));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------------------
// Empty exactly when enabled -- BOTH directions, over a sweep proved to produce both.
//
// WHY THE COUNTS ARE ASSERTED. A guard goes inert on its own subject: if every row in the sweep
// happened to be enabled, the disabled direction would be unexercised and the clause would be
// green while pinning half of what it names. So the sweep counts what it saw and fails if it
// saw only one kind of row.
//
// WHAT WOULD HAVE TO BE TRUE FOR IT TO PASS OVER BROKEN CODE. Only that `bEnabled` and
// `DisabledReason` stay consistent with each other. It says nothing about whether either is
// RIGHT -- that is the previous clause's and the precedence clauses' subject. Named here so the
// pair is not mistaken for full coverage of the model.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellReasonEmptyIffEnabledTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ADisabledReasonIsEmptyExactlyWhenTheRowIsEnabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellReasonEmptyIffEnabledTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	int32 EnabledRowsSeen = 0;
	int32 DisabledRowsSeen = 0;

	for (int32 Bits = 0; Bits < FactCombinationCount; ++Bits)
	{
		const FStratShellFacts Facts = FactsFromBits(Bits);
		const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);

		for (const FStratShellOption& Option : Model.Options)
		{
			const bool bReasonEmpty = Option.DisabledReason.IsEmpty();

			if (Option.bEnabled)
			{
				++EnabledRowsSeen;
				TestTrue(
					FString::Printf(TEXT("enabled route %d carries no reason %s"),
						static_cast<int32>(Option.Route), *DescribeFacts(Facts)),
					bReasonEmpty);
			}
			else
			{
				++DisabledRowsSeen;
				TestFalse(
					FString::Printf(TEXT("greyed route %d names its shortfall %s"),
						static_cast<int32>(Option.Route), *DescribeFacts(Facts)),
					bReasonEmpty);
			}
		}
	}

	TestTrue(TEXT("the sweep reached at least one enabled row"), EnabledRowsSeen > 0);
	TestTrue(TEXT("the sweep reached at least one greyed row"), DisabledRowsSeen > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// Continue names the missing level ahead of the missing save.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NOT A TYPED STRING. Under facts where the match
// level is unset, `NewMatch` has exactly one possible refusal -- its only precondition is the
// destination. That refusal, taken from the function under test, IS the expectation. The clause
// then asserts that `ContinueMatch` gives the same sentence when BOTH of its preconditions
// fail, and a different one when only the save is missing.
//
// THE VACUITY GUARD. If both of `ContinueMatch`'s refusals were the same string, the equality
// would be satisfiable by an implementation with no precedence at all. So the clause first
// asserts the two sentences DIFFER -- case-insensitively, the stricter reading of difference,
// so a mutation that changed only capitalisation could not slip through as "different".
//
// WHAT WOULD HAVE TO BE TRUE FOR IT TO PASS OVER BROKEN CODE. The two checks would have to be
// in the stated order and something else be wrong. It cannot pass with them swapped: that is
// the mutant it was built against.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellContinuePrecedenceTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ContinueNamesTheMissingLevelAheadOfTheMissingSave",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellContinuePrecedenceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	// The oracle: the only sentence `NewMatch` can refuse with is the missing destination.
	FStratShellFacts NoLevel;
	NoLevel.bMatchLevelConfigured = false;

	FText MissingLevelReason;
	const bool bNewMatchRefused =
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::NewMatch, NoLevel,
			MissingLevelReason);

	TestFalse(TEXT("premise: New Match is refused with no match level"), bNewMatchRefused);
	TestFalse(TEXT("premise: that refusal is not silent"), MissingLevelReason.IsEmpty());

	// The other sentence: the save is missing but the destination is fine.
	FStratShellFacts LevelButNoSave;
	LevelButNoSave.bMatchLevelConfigured = true;
	LevelButNoSave.bSaveSlotIsRestorable = false;

	FText MissingSaveReason;
	TestFalse(TEXT("premise: Continue is refused with no save"),
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ContinueMatch, LevelButNoSave,
			MissingSaveReason));
	TestFalse(TEXT("premise: that refusal is not silent"), MissingSaveReason.IsEmpty());

	// DIFFERENT, so the comparison is the default case-insensitive one: the stricter reading.
	TestTrue(TEXT("premise: the two refusals are different sentences"),
		MissingSaveReason.ToString() != MissingLevelReason.ToString());

	// The subject: both preconditions fail at once.
	FStratShellFacts NeitherLevelNorSave;
	NeitherLevelNorSave.bMatchLevelConfigured = false;
	NeitherLevelNorSave.bSaveSlotIsRestorable = false;

	FText Reason;
	TestFalse(TEXT("Continue is refused when neither holds"),
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ContinueMatch,
			NeitherLevelNorSave, Reason));

	// SAME, so the comparison is case-sensitive: the stricter reading.
	TestTrue(
		TEXT("Continue names the missing match level, not the missing save, when both are missing"),
		Reason.ToString().Equals(MissingLevelReason.ToString(), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// Return to Title names the absent match ahead of the absent level.
//
// THE OPPOSITE ORDER TO THE CLAUSE ABOVE, AND THAT IS THE SUBJECT. On the title screen there is
// no match and no title level, and the fact that describes that player is "there is no match to
// leave" -- not a setup fault about the map they are already standing on. Same oracle shape as
// the Continue clause: the expectation is the refusal the function itself gives under facts
// where only one refusal is possible.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellReturnToTitlePrecedenceTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ReturnToTitleNamesTheAbsentMatchAheadOfTheAbsentLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellReturnToTitlePrecedenceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	// Oracle A: no match running, but the destination is configured -- only one refusal is
	// possible and it is the liveness one.
	FStratShellFacts NotLiveButConfigured;
	NotLiveButConfigured.bMatchIsLive          = false;
	NotLiveButConfigured.bTitleLevelConfigured = true;

	FText NoMatchReason;
	TestFalse(TEXT("premise: Return to Title is refused with no match running"),
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ReturnToTitle,
			NotLiveButConfigured, NoMatchReason));
	TestFalse(TEXT("premise: that refusal is not silent"), NoMatchReason.IsEmpty());

	// Oracle B: a match is running but the destination is unset -- only the setup refusal is
	// possible.
	FStratShellFacts LiveButUnconfigured;
	LiveButUnconfigured.bMatchIsLive          = true;
	LiveButUnconfigured.bTitleLevelConfigured = false;

	FText NoTitleLevelReason;
	TestFalse(TEXT("premise: Return to Title is refused with no title level"),
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ReturnToTitle,
			LiveButUnconfigured, NoTitleLevelReason));
	TestFalse(TEXT("premise: that refusal is not silent"), NoTitleLevelReason.IsEmpty());

	// DIFFERENT: default, case-insensitive, the stricter reading.
	TestTrue(TEXT("premise: the two refusals are different sentences"),
		NoMatchReason.ToString() != NoTitleLevelReason.ToString());

	// The subject: the title screen's own state, where both fail.
	FStratShellFacts OnTheTitleScreen;
	OnTheTitleScreen.bMatchIsLive          = false;
	OnTheTitleScreen.bTitleLevelConfigured = false;

	FText Reason;
	TestFalse(TEXT("Return to Title is refused when neither holds"),
		UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ReturnToTitle,
			OnTheTitleScreen, Reason));

	// SAME: case-sensitive, the stricter reading.
	TestTrue(
		TEXT("Return to Title names the absent match, not the absent level, when both are absent"),
		Reason.ToString().Equals(NoMatchReason.ToString(), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// The exit is never greyed.
//
// WHAT IT PINS. `QuitGame` is permitted, and carries no shortfall sentence, in all sixteen fact
// combinations and on both surfaces -- the direct permission call and the row the menu draws.
// A menu whose exit can be disabled is the defect the shell exists to fix, and it is the one
// route with no precondition to get wrong.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellQuitAlwaysPermittedTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.QuitIsPermittedAndUnexplainedInEveryFactCombination",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellQuitAlwaysPermittedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	int32 QuitRowsSeen = 0;

	for (int32 Bits = 0; Bits < FactCombinationCount; ++Bits)
	{
		const FStratShellFacts Facts = FactsFromBits(Bits);

		FText Reason;
		TestTrue(FString::Printf(TEXT("Quit is permitted %s"), *DescribeFacts(Facts)),
			UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::QuitGame, Facts, Reason));
		TestTrue(FString::Printf(TEXT("Quit is refused for no stated reason %s"),
				*DescribeFacts(Facts)),
			Reason.IsEmpty());

		const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);
		for (const FStratShellOption& Option : Model.Options)
		{
			if (Option.Route != EStratShellRoute::QuitGame)
			{
				continue;
			}
			++QuitRowsSeen;
			TestTrue(FString::Printf(TEXT("the Quit row is enabled %s"), *DescribeFacts(Facts)),
				Option.bEnabled);
		}
	}

	// Without this the loop above would be green on a model that had stopped emitting a Quit
	// row at all -- an absence that reads exactly like agreement.
	TestEqual(TEXT("every fact combination drew a Quit row"),
		QuitRowsSeen, FactCombinationCount);

	return true;
}

// ---------------------------------------------------------------------------------------
// The two deciders never disagree.
//
// WHAT IT PINS, AND IT IS THE HEAVIEST CLAUSE IN THIS FILE. `ExecuteRoute` asks
// `IsRoutePermitted` (which reads FACTS) and then `ResolveDestination` (which reads POINTERS),
// and carries a branch for the case where they disagree. This sweeps every travelling route
// against every configuration of the two pointers, deriving the facts from those pointers by
// `GatherFacts`'s own rule (`!IsNull()`), and asserts the implication that makes that branch
// dead: PERMITTED AND TRAVELLING IMPLIES A RESOLVED DESTINATION.
//
// THE EXPECTATION IS `IsRoutePermitted`'S ANSWER, read per case, never a list of which route
// opens which level.
//
// THE INERTNESS GUARD. The implication is vacuously true wherever the route is refused, so a
// sweep that never reached a permitted travelling route would be green having tested nothing.
// The count is asserted.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellPermittedTravelResolvesTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.PermittedTravelAlwaysResolvesADestination",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellPermittedTravelResolvesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	TArray<EStratShellRoute> Declared;
	if (!DeclaredRoutes(Declared))
	{
		AddError(TEXT("EStratShellRoute did not resolve through reflection."));
		return false;
	}

	int32 PermittedTravelCasesSeen = 0;
	int32 NonTravelCasesSeen = 0;

	for (int32 PointerBits = 0; PointerBits < 4; ++PointerBits)
	{
		const TSoftObjectPtr<UWorld> MatchLevel =
			(PointerBits & 1) ? FixtureMatchLevel() : TSoftObjectPtr<UWorld>();
		const TSoftObjectPtr<UWorld> TitleLevel =
			(PointerBits & 2) ? FixtureTitleLevel() : TSoftObjectPtr<UWorld>();

		for (int32 RestBits = 0; RestBits < 4; ++RestBits)
		{
			FStratShellFacts Facts;
			// DERIVED FROM THE POINTERS BY `GatherFacts`'S OWN RULE, so the two deciders are
			// being asked about the same world rather than about two unrelated fixtures.
			Facts.bMatchLevelConfigured = !MatchLevel.IsNull();
			Facts.bTitleLevelConfigured = !TitleLevel.IsNull();
			Facts.bSaveSlotIsRestorable = (RestBits & 1) != 0;
			Facts.bMatchIsLive          = (RestBits & 2) != 0;

			for (const EStratShellRoute Route : Declared)
			{
				FText PermissionReason;
				const bool bPermitted =
					UStratShellSubsystem::IsRoutePermitted(Route, Facts, PermissionReason);

				TSoftObjectPtr<UWorld> Destination;
				FText ResolveReason;
				const bool bResolved = UStratShellSubsystem::ResolveDestination(
					Route, MatchLevel, TitleLevel, Destination, ResolveReason);

				if (!UStratShellSubsystem::RouteTravels(Route))
				{
					++NonTravelCasesSeen;
					TestFalse(
						FString::Printf(TEXT("route %d does not travel yet resolved a level %s"),
							static_cast<int32>(Route), *DescribeFacts(Facts)),
						bResolved);
					TestTrue(
						FString::Printf(TEXT("route %d refused travel silently %s"),
							static_cast<int32>(Route), *DescribeFacts(Facts)),
						!ResolveReason.IsEmpty());
					continue;
				}

				if (!bPermitted)
				{
					continue;
				}

				++PermittedTravelCasesSeen;
				TestTrue(
					FString::Printf(
						TEXT("route %d was permitted but had nowhere to go %s"),
						static_cast<int32>(Route), *DescribeFacts(Facts)),
					bResolved);
				TestFalse(
					FString::Printf(TEXT("route %d resolved to a null level %s"),
						static_cast<int32>(Route), *DescribeFacts(Facts)),
					Destination.IsNull());
			}
		}
	}

	TestTrue(TEXT("the sweep reached at least one permitted travelling route"),
		PermittedTravelCasesSeen > 0);
	TestTrue(TEXT("the sweep reached the non-travelling route"), NonTravelCasesSeen > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// One match destination, two routes to it.
//
// WHAT IT PINS. `NewMatch` and `ContinueMatch` resolve to the SAME level, and `ReturnToTitle`
// to a different one. The sameness is the load-bearing half: two destinations for "start" and
// "restore" would be two answers to the question `T-TURN-09` exists to keep single.
//
// The comparison of the two match destinations is CASE-SENSITIVE (a sameness claim); the
// comparison against the title destination is case-INSENSITIVE (a difference claim), so two
// paths differing only in capitalisation would be reported as the same level and this clause
// would go red rather than green.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellSharedMatchDestinationTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.NewMatchAndContinueResolveToTheOneMatchDestination",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellSharedMatchDestinationTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	const TSoftObjectPtr<UWorld> MatchLevel = FixtureMatchLevel();
	const TSoftObjectPtr<UWorld> TitleLevel = FixtureTitleLevel();

	TSoftObjectPtr<UWorld> FromNewMatch;
	TSoftObjectPtr<UWorld> FromContinue;
	TSoftObjectPtr<UWorld> FromReturn;
	FText Unused;

	TestTrue(TEXT("New Match resolves"), UStratShellSubsystem::ResolveDestination(
		EStratShellRoute::NewMatch, MatchLevel, TitleLevel, FromNewMatch, Unused));
	TestTrue(TEXT("Continue resolves"), UStratShellSubsystem::ResolveDestination(
		EStratShellRoute::ContinueMatch, MatchLevel, TitleLevel, FromContinue, Unused));
	TestTrue(TEXT("Return to Title resolves"), UStratShellSubsystem::ResolveDestination(
		EStratShellRoute::ReturnToTitle, MatchLevel, TitleLevel, FromReturn, Unused));

	const FString NewMatchPath = FromNewMatch.ToSoftObjectPath().ToString();
	const FString ContinuePath = FromContinue.ToSoftObjectPath().ToString();
	const FString ReturnPath   = FromReturn.ToSoftObjectPath().ToString();

	TestTrue(TEXT("New Match and Continue open one and the same level"),
		NewMatchPath.Equals(ContinuePath, ESearchCase::CaseSensitive));

	TestTrue(TEXT("Return to Title opens a different level"),
		ReturnPath != NewMatchPath);

	// The match routes got the MATCH pointer and not merely each other's answer.
	TestTrue(TEXT("the shared destination is the configured match level"),
		NewMatchPath.Equals(MatchLevel.ToSoftObjectPath().ToString(),
			ESearchCase::CaseSensitive));
	TestTrue(TEXT("the title route's destination is the configured title level"),
		ReturnPath.Equals(TitleLevel.ToSoftObjectPath().ToString(),
			ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// The decision is taken without loading anything.
//
// WHAT IT PINS. The header's `IsNull()`-and-not-`IsValid()` claim. `IsValid()` on a
// `TSoftObjectPtr` asks whether the object is LOADED, which is false for every unloaded level
// in a shipped project -- an implementation that asked it would refuse every correctly
// configured destination in the tree while every fact-only clause stayed green.
//
// HOW IT DISTINGUISHES THE TWO. The fixture pointers name levels that do not exist. They are
// non-null and can never be valid. A resolver written on `IsNull()` accepts them; one written
// on `IsValid()` refuses them. The clause also asserts the pointer is still unloaded after the
// call, so a resolver that quietly loaded to decide would be caught rather than merely passing.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellResolveWithoutLoadingTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ResolveDestinationDecidesWithoutLoadingTheLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellResolveWithoutLoadingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	const TSoftObjectPtr<UWorld> MatchLevel = FixtureMatchLevel();
	const TSoftObjectPtr<UWorld> TitleLevel = FixtureTitleLevel();

	TestFalse(TEXT("premise: the fixture match level is not null"), MatchLevel.IsNull());
	TestFalse(TEXT("premise: the fixture match level is not loaded"), MatchLevel.IsValid());

	TSoftObjectPtr<UWorld> Destination;
	FText Reason;
	TestTrue(TEXT("an unloaded but configured level is a destination"),
		UStratShellSubsystem::ResolveDestination(EStratShellRoute::NewMatch, MatchLevel,
			TitleLevel, Destination, Reason));

	TestFalse(TEXT("resolving did not load the level"), Destination.IsValid());
	TestFalse(TEXT("resolving did not load the caller's pointer either"), MatchLevel.IsValid());
	TestTrue(TEXT("the path came back untouched"),
		Destination.ToSoftObjectPath().ToString().Equals(
			MatchLevel.ToSoftObjectPath().ToString(), ESearchCase::CaseSensitive));

	// The other direction: an unset destination is refused, and says so.
	TSoftObjectPtr<UWorld> Nothing;
	TSoftObjectPtr<UWorld> Refused;
	FText RefusalReason;
	TestFalse(TEXT("an unset match level is not a destination"),
		UStratShellSubsystem::ResolveDestination(EStratShellRoute::NewMatch, Nothing,
			TitleLevel, Refused, RefusalReason));
	TestTrue(TEXT("the refused destination came back null"), Refused.IsNull());
	TestFalse(TEXT("the refusal is not silent"), RefusalReason.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------------------
// Only the restart label moves.
//
// WHAT IT PINS. `bMatchIsLive` changes the `NewMatch` row's LABEL and changes nothing else
// about it -- not its route, not its enablement -- and it changes no other row's label. That is
// §2.11.5's "one route, two words for it" stated as a property rather than as a copy of the two
// strings. NO LABEL TEXT IS TYPED IN THIS FILE: a clause that retyped "Restart Match" would be
// asserting a copy of its own subject and would go green on a mutation applied to both places.
//
// THE COMPARISONS RUN IN OPPOSITE DIRECTIONS, DELIBERATELY: the labels that must DIFFER are
// compared case-insensitively (so differing only in case reads as the same and fails), and the
// labels that must NOT differ are compared case-sensitively (so any change at all fails).
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellRestartLabelTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.OnlyTheLiveMatchLabelMovesWithTheFacts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellRestartLabelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	// Identical in every respect except liveness, so liveness is the only possible cause of a
	// difference below.
	FStratShellFacts Idle;
	Idle.bMatchLevelConfigured = true;
	Idle.bTitleLevelConfigured = true;
	Idle.bSaveSlotIsRestorable = true;
	Idle.bMatchIsLive          = false;

	FStratShellFacts Live = Idle;
	Live.bMatchIsLive = true;

	const FStratShellMenuModel IdleModel = UStratShellSubsystem::BuildMenuModel(Idle);
	const FStratShellMenuModel LiveModel = UStratShellSubsystem::BuildMenuModel(Live);

	if (IdleModel.Options.Num() != LiveModel.Options.Num() || IdleModel.Options.Num() == 0)
	{
		AddError(TEXT("the two models are not the same shape; this clause cannot compare them "
			"row by row and will not report that as agreement."));
		return false;
	}

	int32 MovedLabels = 0;

	for (int32 Index = 0; Index < IdleModel.Options.Num(); ++Index)
	{
		const FStratShellOption& IdleRow = IdleModel.Options[Index];
		const FStratShellOption& LiveRow = LiveModel.Options[Index];

		TestEqual(FString::Printf(TEXT("row %d keeps its route across liveness"), Index),
			static_cast<int32>(IdleRow.Route), static_cast<int32>(LiveRow.Route));

		const FString IdleLabel = IdleRow.Label.ToString();
		const FString LiveLabel = LiveRow.Label.ToString();

		if (IdleRow.Route == EStratShellRoute::NewMatch)
		{
			++MovedLabels;
			// DIFFERENT: default, case-insensitive -- the stricter reading of difference.
			TestTrue(TEXT("the New Match row is worded differently once a match is live"),
				IdleLabel != LiveLabel);
			TestEqual(TEXT("and its route is unchanged by that rewording"),
				static_cast<int32>(LiveRow.Route),
				static_cast<int32>(EStratShellRoute::NewMatch));
			TestTrue(TEXT("and its enablement is unchanged by that rewording"),
				IdleRow.bEnabled == LiveRow.bEnabled);
			TestTrue(TEXT("and it is offered in both states"), LiveRow.bEnabled);
		}
		else
		{
			// SAME: case-sensitive -- the stricter reading of sameness.
			TestTrue(
				FString::Printf(TEXT("row %d is worded the same in both states"), Index),
				IdleLabel.Equals(LiveLabel, ESearchCase::CaseSensitive));
		}
	}

	TestEqual(TEXT("exactly one row was expected to move its label"), MovedLabels, 1);

	return true;
}

// ---------------------------------------------------------------------------------------
// The cross-level handoff is consumed once.
//
// WHAT IT PINS. `ArmPendingLoadSlot` writes, `PeekPendingLoadSlot` reads without clearing, and
// `ConsumePendingLoadSlot` answers once and clears. A player who continues and then restarts
// must not be handed the old save on the second travel.
//
// HOW IT TELLS "CONSUMED" FROM "NEVER ARMED", WHICH IS THE TRAP THIS CLAUSE IS SHAPED AROUND.
// An empty read is the DEFAULT, so an empty read alone is evidence of nothing -- a writer that
// was never called looks identical. The clause therefore runs an ordered sequence on ONE
// instance and asserts a non-empty state between the two empty ones: fresh (empty) -> arm
// (non-empty, and equal to what was armed) -> peek again (still non-empty, so peeking is not
// consuming) -> consume (returns it) -> peek (empty) -> consume (empty) -> arm a DIFFERENT
// value (non-empty again). That last re-arm is the positive control: it proves the object can
// still hold a value, so the empty in the middle is a clearing rather than a broken writer.
//
// THE SLOT NAMES ARE MIXED-CASE AND THE EQUALITIES ARE CASE-SENSITIVE, because `FString`
// comparison here is not, and a store that lower-cased what it was given would otherwise be
// indistinguishable from one that stored it.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellPendingSlotConsumedOnceTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ThePendingLoadSlotSurvivesOnePeekAndOneConsumeOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellPendingSlotConsumedOnceTest::RunTest(const FString& /*Parameters*/)
{
	// THE OUTER IS A `UGameInstance` AND NOT THE TRANSIENT PACKAGE, WHICH IS WHERE THIS CLAUSE
	// FIRST WENT RED AND IS WORTH THE LINE. `StratAiPlaybackClauses.cpp` constructs a worldless
	// `UStratMatchSubsystem` with a bare `NewObject<T>()`, and copying that shape here fails:
	// `UGameInstanceSubsystem` declares `ClassWithin = UGameInstance`, so the transient package
	// is an illegal outer and `StaticAllocateObject` fires a handled ensure --
	// "Object None of class /Script/StratPlay.StratShellSubsystem with ClassWithin of
	// /Script/Engine.GameInstance was created in invalid Outer /Script/CoreUObject.Package!"
	// The game instance below takes the transient package itself, which IS legal for it --
	// `UGameInstance` declares no `ClassWithin` -- and it is never initialised and never gets a
	// world; it is an outer and nothing else. Every member exercised in this clause is a plain `FString` field, which is
	// why this still needs no map and survives `-nullrhi`.
	const TStrongObjectPtr<UGameInstance> OwningInstance(
		NewObject<UGameInstance>());
	if (!OwningInstance.IsValid())
	{
		AddError(TEXT("could not construct a UGameInstance to own the shell; nothing was "
			"observed."));
		return false;
	}

	const TStrongObjectPtr<UStratShellSubsystem> Shell(
		NewObject<UStratShellSubsystem>(OwningInstance.Get()));
	if (!Shell.IsValid())
	{
		AddError(TEXT("could not construct a UStratShellSubsystem; nothing was observed."));
		return false;
	}

	const FString FirstSlot  = TEXT("StratShell_FirstSlot_MiXeDcAsE");
	const FString SecondSlot = TEXT("StratShell_SecondSlot_MiXeDcAsE");

	TestTrue(TEXT("a fresh shell has no pending slot"),
		Shell->PeekPendingLoadSlot().IsEmpty());

	Shell->ArmPendingLoadSlot(FirstSlot);

	TestFalse(TEXT("arming leaves something to be found"),
		Shell->PeekPendingLoadSlot().IsEmpty());
	TestTrue(TEXT("and what is found is exactly what was armed"),
		Shell->PeekPendingLoadSlot().Equals(FirstSlot, ESearchCase::CaseSensitive));
	TestTrue(TEXT("peeking twice does not consume"),
		Shell->PeekPendingLoadSlot().Equals(FirstSlot, ESearchCase::CaseSensitive));

	const FString Taken = Shell->ConsumePendingLoadSlot();
	TestTrue(TEXT("consuming returns the armed slot"),
		Taken.Equals(FirstSlot, ESearchCase::CaseSensitive));

	TestTrue(TEXT("and clears it, so the next world is handed nothing"),
		Shell->PeekPendingLoadSlot().IsEmpty());
	TestTrue(TEXT("a second consume returns nothing"),
		Shell->ConsumePendingLoadSlot().IsEmpty());

	// THE POSITIVE CONTROL. Without this, every empty above is equally explained by a writer
	// that never worked at all.
	Shell->ArmPendingLoadSlot(SecondSlot);
	TestTrue(TEXT("the shell can still hold a slot after a consume"),
		Shell->PeekPendingLoadSlot().Equals(SecondSlot, ESearchCase::CaseSensitive));
	TestTrue(TEXT("and it holds the new one, not the consumed one"),
		Shell->ConsumePendingLoadSlot().Equals(SecondSlot, ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// A restart asks for no saved state to seed from.
//
// THE STRETCH IS STATED IN THE CLAUSE'S OWN WORDS, AS THE 2026-08-29 RULING REQUIRES.
// `T-TURN-09` reads "same scenario -> identical state, with an empty command prefix". THIS
// CLAUSE REACHES ONLY THE SEEDING HALF OF THAT SENTENCE AND MUST NOT BE READ AS DISCHARGING IT.
// It does not run two matches, it does not compare two state hashes, and it does not travel. It
// pins the one decision the shell contributes to the question: a restart is the SAME route as a
// fresh start, to the SAME destination, and it requests NO save restore -- so the destination
// world seeds from `FStratMatchConfig` exactly as a fresh boot does, and the existing
// `T-TURN-09` determinism clauses apply to it unchanged. If a restart requested a restore, that
// premise would be false and those clauses would be pinning a match this route never produces.
//
// BOTH DIRECTIONS ARE ASSERTED so the clause cannot go inert: `ContinueMatch` must load a slot
// while `NewMatch` must not. A `RouteLoadsSaveSlot` that returned false for everything would
// satisfy the half that names the restart and is caught by the half that names the restore.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellRestartSeedsFreshTest,
	"Stratocracy.StratPlay.T-TURN-09.ARestartRouteRequestsNoSavedStateToSeedFrom",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellRestartSeedsFreshTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;

	TestFalse(TEXT("a restart requests no saved state"),
		UStratShellSubsystem::RouteLoadsSaveSlot(EStratShellRoute::NewMatch));
	TestTrue(TEXT("a continue does request saved state"),
		UStratShellSubsystem::RouteLoadsSaveSlot(EStratShellRoute::ContinueMatch));

	// The two routes differ in THAT and not in where they go: a second destination for
	// "restart" would be a second answer to what a fresh match seeds from.
	const TSoftObjectPtr<UWorld> MatchLevel = FixtureMatchLevel();
	const TSoftObjectPtr<UWorld> TitleLevel = FixtureTitleLevel();

	TSoftObjectPtr<UWorld> RestartTo;
	TSoftObjectPtr<UWorld> ContinueTo;
	FText Unused;

	TestTrue(TEXT("the restart route has a destination"),
		UStratShellSubsystem::ResolveDestination(EStratShellRoute::NewMatch, MatchLevel,
			TitleLevel, RestartTo, Unused));
	TestTrue(TEXT("the continue route has a destination"),
		UStratShellSubsystem::ResolveDestination(EStratShellRoute::ContinueMatch, MatchLevel,
			TitleLevel, ContinueTo, Unused));
	TestTrue(TEXT("a restart re-opens the same map a continue would"),
		RestartTo.ToSoftObjectPath().ToString().Equals(
			ContinueTo.ToSoftObjectPath().ToString(), ESearchCase::CaseSensitive));

	// And the restart is actually offered from inside a live match -- otherwise the clause
	// would be describing a route the player cannot take, and the whole question is moot.
	int32 LiveRestartRowsSeen = 0;

	for (int32 Bits = 0; Bits < FactCombinationCount; ++Bits)
	{
		const FStratShellFacts Facts = FactsFromBits(Bits);
		if (!Facts.bMatchIsLive || !Facts.bMatchLevelConfigured)
		{
			continue;
		}

		const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);
		for (const FStratShellOption& Option : Model.Options)
		{
			if (Option.Route != EStratShellRoute::NewMatch)
			{
				continue;
			}
			++LiveRestartRowsSeen;
			TestTrue(FString::Printf(TEXT("the restart row is offered %s"),
					*DescribeFacts(Facts)),
				Option.bEnabled);
			TestFalse(FString::Printf(TEXT("and still requests no saved state %s"),
					*DescribeFacts(Facts)),
				UStratShellSubsystem::RouteLoadsSaveSlot(Option.Route));
		}
	}

	TestTrue(TEXT("the sweep reached a live match with a configured destination"),
		LiveRestartRowsSeen > 0);

	return true;
}

// =======================================================================================
// THE SLOT PAIRING. Added in the follow-up pass, over `PendingSlotForRoute`.
//
// WHAT CHANGED AND WHY THESE CLAUSES EXIST. The first pass reported that the pairing of a
// route to the slot it arms lived only as `ArmPendingLoadSlot(SaveSlotName)` inside
// `ExecuteRoute` -- the one member no clause THEN REACHED, which is a weaker and truer
// statement than the "could reach" this sentence carried until 2026-08-30: the refusal path was
// reachable all along and simply had no clause on it, as `ARefusedRouteArmsNothing` below now
// shows -- so an `ExecuteRoute` that armed a literal, an empty string or the wrong member left
// all twelve clauses green. The engineer extracted `PendingSlotForRoute(Route, ConfiguredSlot)`
// and made the call unconditional, and the five clauses below pin THAT FUNCTION.
//
// WHAT THAT DOES AND DOES NOT SETTLE, STATED HERE BECAUSE THIS BANNER SAID IT TOO LOOSELY
// UNTIL 2026-08-30.
//   RETRACTED>  "The gap is only actually closed if the extracted function is pinned, so it is
//   RETRACTED>   pinned here."
// It reads as though pinning the static closes the finding, and it does not. The finding named
// WHICH SLOT `ExecuteRoute` ARMS; these clauses pin WHAT `PendingSlotForRoute` RETURNS. The
// decision is now pinned five ways where it was pinned none -- strictly more than before, and
// the reason the extraction was worth making -- while `ExecuteRoute` passing that decision to
// `ArmPendingLoadSlot` remains unreachable, on the same line whose conditionality is also
// unreachable. Both are named in this file's header list of what is not reached.
//
// THE FIVE CLAUSES CUT THE PROPERTY INTO PARTS THAT FAIL FOR DIFFERENT REASONS, which is the
// point of cutting it at all:
//   - WHICH routes carry a slot. Asserted with no string comparison whatsoever -- emptiness is
//     compared against `RouteLoadsSaveSlot`, a module-side value.
//   - WHETHER the string is the one passed in. Two different fixture strings, each asserted to
//     come back exactly, so a baked-in literal cannot satisfy both.
//   - WHETHER an unconfigured slot stays unconfigured, i.e. no default is substituted for it.
//   - WHETHER arming the empty result of a non-restoring route CLEARS a stale request. This is
//     what makes the now-unconditional call in `ExecuteRoute` correct, and nothing pinned it.
//   - WHETHER the permission check precedes the arming. This one reaches INTO `ExecuteRoute`;
//     see its own comment for exactly how much of that function it touches and how much it
//     does not.
//
// THE TWO NAMED HAZARDS, AND WHERE EACH IS DEFENDED. `FString` comparison here is
// case-insensitive, so every "the string came back" assertion below uses
// `ESearchCase::CaseSensitive` and every fixture string carries mixed case; a literal differing
// from a fixture only in capitalisation is therefore caught. And an empty read is the default,
// so no clause below concludes anything from an empty alone: each one either sees a non-empty
// value from the same call under a different argument, or arms a stale value first and asserts
// what happened to THAT.
// =======================================================================================

namespace StratShellSlotFixture
{
	// UNIQUE TO THESE CLAUSES AND TO NOTHING ELSE IN THE TREE. If `PendingSlotForRoute`
	// returned a baked-in name instead of its argument, no literal a reasonable implementation
	// could contain would match either of these -- and it would have to match BOTH to satisfy
	// the identity clause, which is why there are two.
	const TCHAR* const ConfiguredSlot = TEXT("StratShellFixture_PendingSlot_MiXeD_9f31");
	const TCHAR* const OtherSlot      = TEXT("StratShellFixture_OtherSlot_MiXeD_4c08");
	const TCHAR* const StaleArm       = TEXT("StratShellFixture_StaleArm_MiXeD_7b52");
} // namespace StratShellSlotFixture

// ---------------------------------------------------------------------------------------
// Which routes carry a slot at all.
//
// WHAT IT PINS. For every declared route, `PendingSlotForRoute` returns something exactly when
// `RouteLoadsSaveSlot` says that route restores. NO STRING IS COMPARED IN THIS CLAUSE -- only
// emptiness, against a module-side answer. That is deliberate: this clause is about ROUTING and
// the next one is about the VALUE, and a clause that mixed them would go red for two unrelated
// reasons and tell a reader neither.
//
// THE POSITIVE CONTROL. An implementation returning empty for everything would satisfy every
// "is empty" assertion below, so the clause counts the non-empty results and fails if it saw
// none. An empty return proves nothing on its own; an empty return from the same call that
// returned a value for a different route does.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellOnlyRestoringRouteCarriesASlotTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.OnlyTheRestoringRouteCarriesASlotToArm",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellOnlyRestoringRouteCarriesASlotTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;
	using namespace StratShellSlotFixture;

	TArray<EStratShellRoute> Declared;
	if (!DeclaredRoutes(Declared))
	{
		AddError(TEXT("EStratShellRoute did not resolve through reflection; this clause has "
			"observed nothing and will not report that as agreement."));
		return false;
	}

	int32 RoutesCarryingASlot = 0;
	int32 RoutesCarryingNothing = 0;

	for (const EStratShellRoute Route : Declared)
	{
		const FString Armed =
			UStratShellSubsystem::PendingSlotForRoute(Route, FString(ConfiguredSlot));

		// THE EXPECTATION IS READ FROM THE MODULE, not from a list of which route restores.
		const bool bRestores = UStratShellSubsystem::RouteLoadsSaveSlot(Route);

		TestTrue(
			FString::Printf(
				TEXT("route %d carries a slot exactly when it restores (restores=%s, armed=%s)"),
				static_cast<int32>(Route),
				bRestores ? TEXT("yes") : TEXT("no"),
				Armed.IsEmpty() ? TEXT("nothing") : TEXT("something")),
			bRestores != Armed.IsEmpty());

		if (Armed.IsEmpty()) { ++RoutesCarryingNothing; } else { ++RoutesCarryingASlot; }
	}

	TestTrue(TEXT("some route carried a slot -- without this the clause is satisfied by a "
		"function that returns empty for everything"), RoutesCarryingASlot > 0);
	TestTrue(TEXT("some route carried nothing -- without this it is satisfied by a function "
		"that returns the configured slot for everything"), RoutesCarryingNothing > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// The slot armed is the one configured.
//
// WHAT IT PINS. The restoring route's result is THE ARGUMENT, byte for byte, and not a name
// baked into the body. This is the exact defect the extraction was made to expose: an
// `ExecuteRoute` that armed a literal was previously indistinguishable from one that armed the
// configured slot.
//
// WHY TWO FIXTURE STRINGS AND NOT ONE. One string can be matched by a literal -- unlikely, but
// unfalsifiable from the clause's side. Two different arguments producing two different
// results, each equal to its own input, cannot be satisfied by any constant at all.
//
// CASE-SENSITIVE COMPARISON THROUGHOUT, because `==`, `Contains` and `TestEqual` are not, and a
// body that returned a lower-cased or differently-capitalised copy of its argument would
// otherwise read as a faithful passthrough.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellArmedSlotIsTheConfiguredOneTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheArmedSlotIsTheConfiguredOneAndNotABakedInName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellArmedSlotIsTheConfiguredOneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellSlotFixture;

	const FString First = UStratShellSubsystem::PendingSlotForRoute(
		EStratShellRoute::ContinueMatch, FString(ConfiguredSlot));
	const FString Second = UStratShellSubsystem::PendingSlotForRoute(
		EStratShellRoute::ContinueMatch, FString(OtherSlot));

	TestTrue(TEXT("the restoring route arms the slot it was given"),
		First.Equals(FString(ConfiguredSlot), ESearchCase::CaseSensitive));
	TestTrue(TEXT("and arms a different slot when configured with a different one"),
		Second.Equals(FString(OtherSlot), ESearchCase::CaseSensitive));

	// A constant cannot satisfy both of the above; this states that directly so the clause's
	// message says WHY when it fails.
	TestTrue(TEXT("the two results differ, so the answer is a function of the argument and not "
		"a name baked into the body"), First != Second);

	// The premise the two assertions above rest on: the fixture strings are themselves
	// distinguishable by the comparison this engine actually performs.
	TestTrue(TEXT("premise: the two fixture slot names differ by more than capitalisation"),
		FString(ConfiguredSlot) != FString(OtherSlot));

	return true;
}

// ---------------------------------------------------------------------------------------
// An unconfigured slot stays unconfigured.
//
// WHAT IT PINS. With no slot name configured, EVERY route arms nothing -- including the
// restoring one. No default, no fallback, no stand-in name is substituted.
//
// WHY THIS IS A SEPARATE CLAUSE AND NOT A LINE IN THE ONE ABOVE. This project has already been
// bitten by a resolver whose real default could not signal "unset", which wrote the player's
// save every run. A body reading `ConfiguredSlot.IsEmpty() ? TEXT("SaveSlot") : ConfiguredSlot`
// passes both clauses above -- they always pass a configured name -- and would silently restore
// somebody else's slot on a GameMode whose `SaveSlotName` default was never filled in.
// `AStratShellGameMode`'s own header says an empty slot must degrade to the ordinary
// first-time-player state; this is that sentence made assertable.
//
// THE ANTI-VACUITY GUARD. Every assertion here is "is empty", which is the default and proves
// nothing alone, so the clause first shows the same call returning a NON-empty value for the
// same route under a configured name. The empties that follow are then attributable.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellUnconfiguredSlotArmsNothingTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AnUnconfiguredSlotArmsNothingOnEveryRoute",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellUnconfiguredSlotArmsNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellClauseFixture;
	using namespace StratShellSlotFixture;

	TArray<EStratShellRoute> Declared;
	if (!DeclaredRoutes(Declared))
	{
		AddError(TEXT("EStratShellRoute did not resolve through reflection."));
		return false;
	}

	// THE CONTROL, FIRST. Without it every assertion below is equally explained by a function
	// that never returns anything.
	TestFalse(TEXT("control: the restoring route does arm something when a slot is configured"),
		UStratShellSubsystem::PendingSlotForRoute(
			EStratShellRoute::ContinueMatch, FString(ConfiguredSlot)).IsEmpty());

	for (const EStratShellRoute Route : Declared)
	{
		TestTrue(
			FString::Printf(TEXT("route %d arms nothing when no slot is configured"),
				static_cast<int32>(Route)),
			UStratShellSubsystem::PendingSlotForRoute(Route, FString()).IsEmpty());
	}

	return true;
}

// ---------------------------------------------------------------------------------------
// A non-restoring route clears a stale request.
//
// WHAT IT PINS, AND IT IS A PROPERTY THE FOLLOW-UP PASS CREATED. `ExecuteRoute` now arms
// UNCONDITIONALLY -- the `if (RouteLoadsSaveSlot(Route))` is gone rather than moved -- so a
// `NewMatch` route arms the EMPTY result of `PendingSlotForRoute` over whatever was there. That
// is a behaviour change: under the previous shape a restart left an earlier `Continue`'s
// request standing. It is the better behaviour, it matches what `ConsumePendingLoadSlot`'s own
// comment says the class is for, and nothing pinned it.
//
// WHAT MAKES IT CORRECT IS THAT ARMING AN EMPTY VALUE CLEARS, and that is what this clause
// asserts: `ArmPendingLoadSlot` is a store and not an "update if non-empty". A store that
// ignored empty input would leave the stale request in place and hand a restarting player their
// old save -- exactly the failure `ConsumePendingLoadSlot` exists to prevent, arriving by a
// different door. The consumed-once clause above cannot see it: it only ever arms non-empty
// values.
//
// WHAT IT DOES **NOT** PIN, said plainly. It composes `ArmPendingLoadSlot(PendingSlotForRoute(
// Route, Slot))` -- the same two public entry points `ExecuteRoute` composes, in the same
// order -- but it is this clause performing that composition, not `ExecuteRoute`. IT CANNOT
// SEE WHETHER `ExecuteRoute`'S OWN CALL IS STILL UNCONDITIONAL. Restoring the `if` there would
// leave this clause green. Stated as unreachable rather than implied as covered.
//   RETRACTED>  "MEASURED rather than reasoned: a built mutant that restores that `if` leaves
//   RETRACTED>   the entire suite green."
// **[STAMPED 2026-08-30]** Same correction as the header's conditionality item, for the same
// reason: that cited a GREEN mutant, and this branch measured that a green mutant run can mean
// the build never happened. The claim now rests on THE CALL-SITE CENSUS IN THIS FILE'S HEADER
// -- stated once there, with the command to re-run and the trap that makes the obvious version
// of it miscount. Nothing reaches the arming line, so no edit to it can redden anything.
// (An earlier version of this citation read "See the report".
//   RETRACTED>  "pointing at a gate report no checkout holds."
// **[STAMPED 2026-08-30]** That was false -- `Tools/architect/gate_reports/` is tracked and held
// five reports at `347c722` -- and striking the citation on that ground was the wrong remedy;
// see the header's `AN EVIDENCE CITATION` rule, where the error and its provenance are recorded
// once. This wave's report is `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md`, and it
// is a second thing a reader can check besides the census.)
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellNonRestoringRouteClearsStaleSlotTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ARouteThatDoesNotRestoreClearsAStaleArmedSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellNonRestoringRouteClearsStaleSlotTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellSlotFixture;

	// The outer must be a `UGameInstance`: `UGameInstanceSubsystem` declares
	// `ClassWithin = UGameInstance` and the transient package fires a handled ensure. Measured
	// in the first pass of this file.
	const TStrongObjectPtr<UGameInstance> OwningInstance(NewObject<UGameInstance>());
	if (!OwningInstance.IsValid())
	{
		AddError(TEXT("could not construct a UGameInstance to own the shell."));
		return false;
	}

	const TStrongObjectPtr<UStratShellSubsystem> Shell(
		NewObject<UStratShellSubsystem>(OwningInstance.Get()));
	if (!Shell.IsValid())
	{
		AddError(TEXT("could not construct a UStratShellSubsystem; nothing was observed."));
		return false;
	}

	// The composition `ExecuteRoute` performs, for the route that restores.
	Shell->ArmPendingLoadSlot(UStratShellSubsystem::PendingSlotForRoute(
		EStratShellRoute::ContinueMatch, FString(ConfiguredSlot)));

	TestTrue(TEXT("a continue route arms the configured slot"),
		Shell->PeekPendingLoadSlot().Equals(FString(ConfiguredSlot),
			ESearchCase::CaseSensitive));

	// The same composition, for a route that does not restore.
	Shell->ArmPendingLoadSlot(UStratShellSubsystem::PendingSlotForRoute(
		EStratShellRoute::NewMatch, FString(ConfiguredSlot)));

	TestTrue(TEXT("a restart afterwards leaves nothing pending, so the restarting player is "
		"not handed the earlier save"), Shell->PeekPendingLoadSlot().IsEmpty());

	// And the shell is not merely broken: it can still hold a request after that clearing.
	Shell->ArmPendingLoadSlot(UStratShellSubsystem::PendingSlotForRoute(
		EStratShellRoute::ContinueMatch, FString(OtherSlot)));
	TestTrue(TEXT("control: the shell still stores a request after the clearing"),
		Shell->PeekPendingLoadSlot().Equals(FString(OtherSlot), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// A refused route arms nothing.
//
// THIS CLAUSE REACHES INTO `ExecuteRoute`, WHICH THIS FILE'S OWN HEADER ONCE SAID NO CLAUSE
// COULD -- see the stamped retraction at the top, which this clause is the reason for. (The
// sentence here read "AND THE HEADER SAYS NO CLAUSE CAN" until 2026-08-30, and correcting the
// header without correcting this line would have left the file citing a claim it no longer
// makes -- the same defect in the opposite direction.) The retracted claim is
// true of the TRAVELLING path and not of the REFUSING one: when `IsRoutePermitted` says no,
// `ExecuteRoute` returns before `RouteTravels`, before `ResolveDestination`, before the arming
// and before the single engine call. Nothing on that path needs a viewport, a world or a level.
// `GatherFacts` is safe on a bare instance -- `GetGameInstance()` is the outer, its `GetWorld()`
// is null and is guarded, and the save-file question is short-circuited by the empty-name
// check -- which is what makes the path reachable at all.
//
// WHAT IT PINS, AND IT IS AN ORDERING FACT: THE PERMISSION CHECK PRECEDES THE ARMING. That is
// one of the two things the engineer's header lists as left unreachable, and one half of it
// turns out not to be. An `ExecuteRoute` that armed first and asked afterwards would leave a
// refused `Continue` request armed, and the next world's `AStratGameMode::BeginPlay` would
// consume and restore a slot the player was just told they could not have.
//
// HOW IT TELLS "UNTOUCHED" FROM "EMPTY". A stale value is armed FIRST, with a fixture string
// used nowhere else, and the assertion after the refused call is that THAT value is still
// there -- case-sensitively. An empty read would fail. So the clause distinguishes all three
// states: untouched (green), cleared (red), overwritten with the configured slot (red).
//
// WHAT IT DOES **NOT** PIN. The rest of the order -- destination before arming, arming before
// the engine call -- is not reachable, because reaching it means travelling. Nor does it pin
// that the arming call is unconditional. Both are reported as unreachable rather than implied.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellRefusedRouteArmsNothingTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ARefusedRouteArmsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellRefusedRouteArmsNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellSlotFixture;

	// DECLARED, NOT SUPPRESSED, AND `Occurrences = 1` IS ITSELF AN ASSERTION. The refusal this
	// clause provokes logs a warning from `ExecuteRoute`; leaving it undeclared reports the
	// clause as SucceededWithWarnings, which is a state a reader has to go and interpret.
	// Declaring exactly one occurrence pins that the route was refused ONCE -- a second
	// refusal, or none at all, fails the clause. (`Occurrences = 0` would mean "one or more",
	// which is measured in `StratAiMatchClauses.cpp` and is the weaker statement.)
	AddExpectedMessagePlain(TEXT("Shell route refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 1);

	const TStrongObjectPtr<UGameInstance> OwningInstance(NewObject<UGameInstance>());
	if (!OwningInstance.IsValid())
	{
		AddError(TEXT("could not construct a UGameInstance to own the shell."));
		return false;
	}

	const TStrongObjectPtr<UStratShellSubsystem> Shell(
		NewObject<UStratShellSubsystem>(OwningInstance.Get()));
	if (!Shell.IsValid())
	{
		AddError(TEXT("could not construct a UStratShellSubsystem; nothing was observed."));
		return false;
	}

	// A CONFIGURED SLOT AND A NULL DESTINATION, WHICH IS THE FACT SET THIS CLAUSE NEEDS. The
	// slot must be configured, or a premature arming would write an empty string and be
	// invisible; the destination must be absent, or the route would be permitted and the call
	// would travel.
	Shell->ConfigureMatchDestination(TSoftObjectPtr<UWorld>(), FString(ConfiguredSlot));

	TestTrue(TEXT("premise: the shell is configured with the fixture slot"),
		Shell->GetSaveSlotName().Equals(FString(ConfiguredSlot), ESearchCase::CaseSensitive));

	// The stale request whose survival is the observation.
	Shell->ArmPendingLoadSlot(FString(StaleArm));

	FString FailureReason;
	const bool bTook = Shell->ExecuteRoute(EStratShellRoute::ContinueMatch, FailureReason);

	TestFalse(TEXT("the route is refused, with no destination configured"), bTook);
	TestFalse(TEXT("and the refusal says why"), FailureReason.IsEmpty());

	TestTrue(TEXT("a refused route armed nothing: the earlier request is untouched, neither "
		"cleared nor overwritten with the configured slot"),
		Shell->PeekPendingLoadSlot().Equals(FString(StaleArm), ESearchCase::CaseSensitive));

	// Named so the failure message distinguishes the two ways this can go wrong.
	TestFalse(TEXT("in particular the refused route did not arm the configured slot"),
		Shell->PeekPendingLoadSlot().Equals(FString(ConfiguredSlot),
			ESearchCase::CaseSensitive));

	return true;
}

// =======================================================================================
// RESTORABILITY. Added after a defect this file's own clauses could not see.
//
// THE DEFECT, IN THE PLAYER'S TERMS. Finish a match without ever saving. The shipped
// `BP_StratGameMode` opts into completion recording, so `RecordMatchCompletionOnSave` writes a
// payload carrying one bool and NO match text -- creating the slot FILE. The title screen asked
// `DoesSaveGameExist`, got true, and drew `Continue` ENABLED. Taking it refused inside
// `LoadMatchFromSlot` and the player silently got turn 1 instead of their match.
//
// WHY NOTHING CAUGHT IT. THE CAUSE HAS TWO HALVES; THIS PARAGRAPH ONCE NAMED ONLY ONE, WHICH
// UNDERSTATED IT IN THE VERY PLACE IT CLAIMS TO TEACH THE LESSON. Half one is a test-lane fact
// and is stated here because it is this lane's own. Half two is not a test-lane fact and is
// therefore POINTED AT RATHER THAN RETOLD -- `StratSaveGame.h` and `StratMatchSubsystem.h`
// carry it, and a second copy here would be a competing account that drifts.
//
// HALF ONE -- AN UNPINNED ARM, AND NOT ONLY THIS ONE. The refusal existed and was correct:
// `LoadMatchFromSlot` has always refused an empty `SaveText`. What did not exist was a clause.
//   RETRACTED>  "...while the sibling arms are pinned by `T-SAVE-04` clauses through the
//   RETRACTED>   loader."
// **[STAMPED 2026-08-30]** That reads as one unlucky arm. TWO OF THE THREE WERE UNPINNED, which
// is systematic and sends a reader somewhere different. Measured at `347c722`, scoped to
// `Source/*/Tests/*.cpp`:
//   - THE VERSION ARM was pinned, and was the ONLY one --
//     `Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAWrongSavedDataVersion`, through the loader.
//   - THE WRONG-CLASS ARM (`does not hold a UStratSaveGame`): zero occurrences corpus-wide.
//   - THE EMPTY-TEXT ARM, this clause's subject: zero.
// THE ABSENCE WAS RUN WITH A CONTROL, an absence proving nothing until the instrument is shown
// able to speak: the same `git grep -F` finds the wrong-class phrase once in
// `StratMatchSubsystem.cpp` at `347c722` and zero times under `Tests/`.
//
// HALF TWO -- THE TREE ALREADY WARNED US IN CURRENT PROSE AND NOBODY READ IT, and this half
// matters more than the clauses do. `StratSaveGame.h`, unmodified in this branch, carries it
// under the heading `ONE CONSEQUENCE, WRITTEN DOWN RATHER THAN DISCOVERED`: written before W6
// existed, it names the "Continue" affordance, names the function that must NOT be gated on,
// and states the rule -- an offer of Continue "must gate on a LOADABLE slot". READ IT THERE.
// It was written down AND discovered anyway, which is the whole finding, and the engine-side
// account of it is that file's and `StratMatchSubsystem.h`'s.
//   ONE THING THIS LANE CHECKED AND FOUND FALSE, recorded so nobody re-derives it: the
// hypothesis that the warning went unread because it named a wrong symbol. It names
// `DoesSaveSlotExist`, which IS a real `UStratMatchSubsystem` method at `347c722`, so the
// sentence was accurate and greppable in its own terms. Nothing about the warning explains
// why it went unread except that nobody read it.
//
// SO THE LESSON IS NOT ONLY "PIN EVERY ARM". It is that prose is not a gate: a hazard already
// written down in a header stays true, stays unenforced, and ships. These clauses are that
// warning made mechanical, which is the only form of it that can fail a build.
//   AND IT WAS UNREACHABLE, NOT MERELY UNWRITTEN. The condition lived behind a `UWorld`, a
// configured subsystem and a file on disk. `UStratMatchSubsystem::IsPayloadRestorable` is the
// seam that changed that: static, world-free, subsystem-free, disk-free, so the exact byte
// pattern `RecordMatchCompletionOnSave` writes can be constructed in memory and asked about.
//
// THE ACCEPTANCE ID, RULED AGAINST THE ID'S OWN WORDS RATHER THAN BY FAMILY RESEMBLANCE.
// `T-SAVE-04` was proposed and IS REFUSED HERE for these clauses. The GDD defines it as
// "refusal: any header mismatch (version/rules/data/scenario hash)" and says it "refuses on the
// header alone and never applies a command". A payload whose header is VALID AND CURRENT and
// whose BODY is empty is not a header mismatch by any reading of that sentence -- indeed the
// whole reason the defect shipped is that every header field was in order. A clause riding an
// ID whose sentence excludes its subject is the unfalsifiable shape this project has already
// been caught by, so these ride `GATE-TITLEMENU`: the fact under test is the one the title
// menu reads, and the user's 2026-08-29 ruling puts the screen and its routes on that local
// gate name. The version arm's RULE remains squarely `T-SAVE-04`'s and already has its clause
// (`Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAWrongSavedDataVersion`), and no clause here
// restates that rule. Said precisely, because "nothing here touches it" stood here until
// 2026-08-30 and was loose: `TheRestorabilityRefusalIsEmptyExactlyWhenRestorable` DOES build a
// wrong-version payload. It uses it as one of four states to exercise the OUT-PARAMETER
// CONTRACT and asserts nothing about the version rule -- a different subject reached through
// the same input.
// =======================================================================================

namespace StratRestorableFixture
{
	/** Unique to these clauses. Written to and deleted by the end-to-end clause below. */
	const TCHAR* const SlotName = TEXT("StratShellFixture_RestorableGate_MiXeD_5a17");
	constexpr int32 UserIndex = 0;

	/**
	 * A payload byte-identical in the fields that matter to what `RecordMatchCompletionOnSave`
	 * writes when no slot existed: the completion bit set, the match text empty, and the
	 * version left at whatever this build initialises it to.
	 *
	 * NOTHING HERE IS SET TO MAKE THE CLAUSE PASS. `SaveText` is not cleared -- it is empty
	 * because `UStratSaveGame` initialises it empty, which is precisely the writer's own
	 * situation.
	 */
	UStratSaveGame* MakeCompletionOnlyPayload()
	{
		UStratSaveGame* const Payload = NewObject<UStratSaveGame>();
		if (Payload != nullptr)
		{
			Payload->bHasCompletedAMatch = true;
		}
		return Payload;
	}
} // namespace StratRestorableFixture

// ---------------------------------------------------------------------------------------
// A finished match is not a saved match.
//
// WHAT IT PINS, AND IT IS THE CLAUSE THAT WOULD HAVE CAUGHT THE SHIPPED DEFECT. A payload with
// a current header and no match text is NOT restorable.
//
// THE PREMISE THAT CARRIES THE WHOLE POINT, ASSERTED RATHER THAN ASSUMED: the version gate
// CANNOT catch this payload. `UStratSaveGame::SavedDataVersion` is initialised TO
// `kCurrentSavedDataVersion`, so anything this build creates is current by construction, and a
// reader who assumed the version check was the guard would be wrong. The clause asserts that
// equality first, so if a later change made fresh payloads non-current this clause would report
// that its subject had moved instead of passing for a new reason.
//
// WHAT WOULD HAVE TO BE TRUE FOR IT TO PASS OVER BROKEN CODE. The predicate would have to
// refuse this payload for the RIGHT reason and be wrong elsewhere -- which is why the positive
// control below flips only `SaveText` and requires the answer to flip with it. A predicate that
// refused everything passes the first half and fails the control.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCompletionOnlySlotNotRestorableTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ACompletionOnlyPayloadIsNotARestorableMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCompletionOnlySlotNotRestorableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRestorableFixture;

	const TStrongObjectPtr<UStratSaveGame> Payload(MakeCompletionOnlyPayload());
	if (!Payload.IsValid())
	{
		AddError(TEXT("could not construct a UStratSaveGame; nothing was observed."));
		return false;
	}

	// THE PREMISE. The header is in order, which is why no header-mismatch clause covers this.
	TestEqual(TEXT("premise: a freshly created payload's version is already current, so the "
		"version gate cannot be what refuses it"),
		Payload->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion);
	TestTrue(TEXT("premise: it carries no match text, exactly as the completion writer leaves it"),
		Payload->SaveText.IsEmpty());
	TestTrue(TEXT("premise: it does carry the completion bit, which is why the file exists"),
		Payload->bHasCompletedAMatch);

	FText Refusal;
	TestFalse(TEXT("a completion-only payload is not a restorable match"),
		UStratMatchSubsystem::IsPayloadRestorable(Payload.Get(), Refusal));
	TestFalse(TEXT("and the refusal says why"), Refusal.IsEmpty());

	// THE POSITIVE CONTROL. One field moves and the answer must move with it; without this the
	// clause is satisfied by a predicate that refuses everything.
	Payload->SaveText = TEXT("2026-08-30 fixture text; contents are irrelevant, presence is not");

	FText NoRefusal;
	TestTrue(TEXT("control: the same payload with match text IS restorable"),
		UStratMatchSubsystem::IsPayloadRestorable(Payload.Get(), NoRefusal));
	TestTrue(TEXT("control: and carries no refusal"), NoRefusal.IsEmpty());

	// The null case, which is the cast failure a foreign slot lands on.
	FText NullRefusal;
	TestFalse(TEXT("a payload that is not a UStratSaveGame at all is not restorable"),
		UStratMatchSubsystem::IsPayloadRestorable(nullptr, NullRefusal));
	TestFalse(TEXT("and that refusal says why too"), NullRefusal.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------------------
// The refusal is empty exactly when the payload is restorable.
//
// WHAT IT PINS. `IsPayloadRestorable`'s out-parameter contract, in BOTH directions, over a set
// of payload states that actually produces both. Same shape as this file's menu-row clause and
// for the same reason: a caller that composes the refusal into a sentence -- which
// `LoadMatchFromSlot` now does -- would otherwise be able to compose an empty one and say
// "slot 'X' " with nothing after it.
//
// THE INERTNESS GUARD. The counts are asserted. A predicate that returned false for everything,
// or true for everything, fails on the count rather than passing on a one-sided sweep.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRestorableRefusalEmptyIffRestorableTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheRestorabilityRefusalIsEmptyExactlyWhenRestorable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRestorableRefusalEmptyIffRestorableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRestorableFixture;

	int32 RestorableSeen = 0;
	int32 RefusedSeen = 0;

	// Four states: absent, wrong header, valid header with no body, and whole.
	const TStrongObjectPtr<UStratSaveGame> WrongVersion(MakeCompletionOnlyPayload());
	const TStrongObjectPtr<UStratSaveGame> NoText(MakeCompletionOnlyPayload());
	const TStrongObjectPtr<UStratSaveGame> Whole(MakeCompletionOnlyPayload());
	if (!WrongVersion.IsValid() || !NoText.IsValid() || !Whole.IsValid())
	{
		AddError(TEXT("could not construct the payload fixtures; nothing was observed."));
		return false;
	}

	WrongVersion->SavedDataVersion = UStratSaveGame::kCurrentSavedDataVersion + 1;
	WrongVersion->SaveText = TEXT("fixture text");
	Whole->SaveText = TEXT("fixture text");

	const UStratSaveGame* const Cases[] = {
		nullptr, WrongVersion.Get(), NoText.Get(), Whole.Get()
	};

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Cases); ++Index)
	{
		FText Refusal;
		const bool bRestorable = UStratMatchSubsystem::IsPayloadRestorable(Cases[Index], Refusal);

		if (bRestorable)
		{
			++RestorableSeen;
			TestTrue(FString::Printf(
					TEXT("case %d is restorable and therefore carries no refusal"), Index),
				Refusal.IsEmpty());
		}
		else
		{
			++RefusedSeen;
			TestFalse(FString::Printf(
					TEXT("case %d is refused and therefore names a reason"), Index),
				Refusal.IsEmpty());
		}
	}

	TestTrue(TEXT("the sweep reached at least one restorable payload"), RestorableSeen > 0);
	TestTrue(TEXT("the sweep reached at least one refused payload"), RefusedSeen > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// The title map's slot name is derived from the match's, not copied.
//
// WHAT IT PINS. `AStratShellGameMode`'s `SaveSlotName` class default equals
// `FStratMatchConfig`'s, because the constructor derives it rather than restating the string.
// It reddens the moment somebody reintroduces a literal, which is the shape that drifts: two
// spellings of one slot name agree until one of them is edited, and the symptom -- a title
// screen reading a slot the match never writes -- lands on "No saved match.", indistinguishable
// from a genuine first run.
//
// IT IS READ FROM THE CDO, following `StratShippedGameModeOptIn.cpp`'s precedent, and it needs
// no world and no asset: this is a C++ class default, not a Blueprint override.
//
// THE VACUITY GUARD, WHICH THIS CLAUSE WOULD BE WORTHLESS WITHOUT. Two empty strings are equal.
// If both defaults were ever empty the equality would hold while the property was unset, so the
// clause asserts the derived value is NON-EMPTY first and only then that the two agree.
//
// WHAT IT DOES **NOT** PIN, and the debt is real: a Blueprint that OVERRIDES either property
// re-opens exactly the drift this pins in C++. That discharges on a clause reading both
// Blueprint CDOs, and those assets do not exist yet.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellDefaultSlotDerivedTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellDefaultSlotDerivedTest::RunTest(const FString& /*Parameters*/)
{
	const AStratShellGameMode* const ShellDefaults = GetDefault<AStratShellGameMode>();
	if (ShellDefaults == nullptr)
	{
		AddError(TEXT("could not resolve AStratShellGameMode's class default object; this "
			"clause has observed nothing and will not report that as agreement."));
		return false;
	}

	const FString MatchConfigSlot = FStratMatchConfig().SaveSlotName;

	// THE VACUITY GUARD, FIRST.
	TestFalse(TEXT("premise: the match config's default slot name is not empty, so the "
		"comparison below cannot be satisfied by two empty strings"),
		MatchConfigSlot.IsEmpty());
	TestFalse(TEXT("premise: the shell's default slot name is not empty either"),
		ShellDefaults->SaveSlotName.IsEmpty());

	// SAME, so the comparison is case-sensitive: the stricter reading, and the necessary one
	// on an engine whose `FString` comparison is not.
	TestTrue(TEXT("the title map's default slot is the match config's own, character for "
		"character -- a literal here would be a second author of one string"),
		ShellDefaults->SaveSlotName.Equals(MatchConfigSlot, ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// Continue stays greyed after a finished but unsaved match.
//
// THE DEFECT, END TO END, IN THE PLAYER'S TERMS, THROUGH THE REAL DISK PATH. This clause writes
// the exact payload the completion recorder writes to a real slot, then asks the two questions
// in order: does a file exist, and does that slot hold a restorable match. THE FIRST IS TRUE
// AND THE SECOND IS FALSE, and that gap IS the defect -- the old fact asked only the first.
// The answer is then fed into `BuildMenuModel` as the fact it is, and `Continue` must be greyed.
//
// THE EXPECTATION IS READ FROM THE MODULE AT EVERY STEP. `bSaveSlotIsRestorable` is not set by
// hand here: it is whatever `DoesSlotHoldARestorableMatch` returns. That is the difference
// between this clause and every other menu clause in this file, which construct facts by hand
// -- and it is why this one, alone, would have gone red on the shipped tree.
//
// IT WRITES TO DISK AND CLEANS UP, on the precedent of `StratMatchCompletionRecording.cpp` and
// `StratDifficultyHandicapMatchClauses.cpp`. The slot name is unique to this clause, it is
// deleted before the first write as well as after the last, and the delete-first matters: a
// slot left behind by an earlier aborted run would otherwise decide this clause's answer.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratFinishedUnsavedMatchLeavesContinueGreyedTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AFinishedButUnsavedMatchLeavesContinueGreyed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratFinishedUnsavedMatchLeavesContinueGreyedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRestorableFixture;

	// DELETED FIRST. A leftover slot from an aborted run would decide the answer below.
	UGameplayStatics::DeleteGameInSlot(FString(SlotName), UserIndex);

	FText Reason;
	TestFalse(TEXT("premise: with no file at all, the slot holds no restorable match"),
		UStratMatchSubsystem::DoesSlotHoldARestorableMatch(FString(SlotName), Reason));

	const TStrongObjectPtr<UStratSaveGame> Payload(MakeCompletionOnlyPayload());
	if (!Payload.IsValid())
	{
		AddError(TEXT("could not construct a UStratSaveGame; nothing was observed."));
		return false;
	}

	if (!UGameplayStatics::SaveGameToSlot(Payload.Get(), FString(SlotName), UserIndex))
	{
		AddError(TEXT("could not write the fixture slot; this clause has observed nothing "
			"and will not report that as agreement."));
		return false;
	}

	// THE TWO QUESTIONS, AND THE GAP BETWEEN THEM IS THE WHOLE CLAUSE.
	TestTrue(TEXT("a file now exists in the slot -- which is all the old fact asked, and is "
		"why Continue used to light up"),
		UGameplayStatics::DoesSaveGameExist(FString(SlotName), UserIndex));

	FText Unrestorable;
	const bool bRestorable =
		UStratMatchSubsystem::DoesSlotHoldARestorableMatch(FString(SlotName), Unrestorable);

	TestFalse(TEXT("but the slot does not hold a restorable match"), bRestorable);
	TestFalse(TEXT("and the refusal says why"), Unrestorable.IsEmpty());

	// THE FACT IS TAKEN FROM THE MODULE, NOT ASSERTED BY HAND. This is the join the shipped
	// tree got wrong.
	FStratShellFacts Facts;
	Facts.bMatchLevelConfigured = true;
	Facts.bSaveSlotIsRestorable = bRestorable;

	const FStratShellMenuModel Model = UStratShellSubsystem::BuildMenuModel(Facts);

	int32 ContinueRowsSeen = 0;
	for (const FStratShellOption& Option : Model.Options)
	{
		if (Option.Route != EStratShellRoute::ContinueMatch)
		{
			continue;
		}
		++ContinueRowsSeen;
		TestFalse(TEXT("Continue is greyed after a finished but unsaved match"), Option.bEnabled);
		TestFalse(TEXT("and the greyed row names its shortfall"),
			Option.DisabledReason.IsEmpty());
	}
	TestEqual(TEXT("the menu drew exactly one Continue row to judge"), ContinueRowsSeen, 1);

	// THE POSITIVE CONTROL. The same slot, with match text, must light Continue up -- otherwise
	// this clause is satisfied by a menu that greys Continue unconditionally, which would be a
	// different defect wearing this clause's green.
	Payload->SaveText = TEXT("2026-08-30 fixture text; presence is what is being measured");
	if (UGameplayStatics::SaveGameToSlot(Payload.Get(), FString(SlotName), UserIndex))
	{
		FText ControlReason;
		const bool bControlRestorable =
			UStratMatchSubsystem::DoesSlotHoldARestorableMatch(FString(SlotName), ControlReason);

		TestTrue(TEXT("control: a slot carrying match text IS restorable"), bControlRestorable);

		FStratShellFacts ControlFacts;
		ControlFacts.bMatchLevelConfigured = true;
		ControlFacts.bSaveSlotIsRestorable = bControlRestorable;

		const FStratShellMenuModel ControlModel =
			UStratShellSubsystem::BuildMenuModel(ControlFacts);
		for (const FStratShellOption& Option : ControlModel.Options)
		{
			if (Option.Route == EStratShellRoute::ContinueMatch)
			{
				TestTrue(TEXT("control: and Continue is offered for it"), Option.bEnabled);
			}
		}
	}
	else
	{
		AddError(TEXT("could not rewrite the fixture slot for the positive control."));
	}

	// CLEANED UP. The suite must not leave a slot behind for the next run to read.
	UGameplayStatics::DeleteGameInSlot(FString(SlotName), UserIndex);
	TestFalse(TEXT("the fixture slot is gone after the clause"),
		UGameplayStatics::DoesSaveGameExist(FString(SlotName), UserIndex));

	return true;
}

// ---------------------------------------------------------------------------------------
// The slot-level question delegates rather than restates.
//
// WHAT IT PINS. `DoesSlotHoldARestorableMatch` hands back `IsPayloadRestorable`'s refusal
// UNCHANGED -- not a wrapper, not a paraphrase, not a second statement of the same conditions.
// There are now three askers of "is this restorable": the title menu's fact, the loader, and
// this slot-level convenience. One authority, three askers, and each join is pinned by a
// different clause so a break says which join broke.
//
// THE EXPECTATION IS THE OTHER FUNCTION'S OUT-PARAMETER, read for the same payload. No refusal
// text appears in this file.
//
// EQUALITY AND NOT `Contains`, deliberately: this function adds no wrapper of its own, so the
// strings must match exactly. The loader's clause -- which DOES add a wrapper -- uses
// `Contains` for that reason. The two assertions differ because the two compositions differ.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSlotQuestionDelegatesTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheSlotQuestionReturnsThePredicatesRefusalUnchanged",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSlotQuestionDelegatesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRestorableFixture;

	UGameplayStatics::DeleteGameInSlot(FString(SlotName), UserIndex);

	const TStrongObjectPtr<UStratSaveGame> Payload(MakeCompletionOnlyPayload());
	if (!Payload.IsValid())
	{
		AddError(TEXT("could not construct a UStratSaveGame; nothing was observed."));
		return false;
	}

	// THE ORACLE, taken from the payload-level function for this exact payload.
	FText PredicateRefusal;
	TestFalse(TEXT("premise: the predicate refuses this payload"),
		UStratMatchSubsystem::IsPayloadRestorable(Payload.Get(), PredicateRefusal));
	TestFalse(TEXT("premise: and names a reason"), PredicateRefusal.IsEmpty());

	if (!UGameplayStatics::SaveGameToSlot(Payload.Get(), FString(SlotName), UserIndex))
	{
		AddError(TEXT("could not write the fixture slot; nothing was observed."));
		return false;
	}

	FText SlotRefusal;
	TestFalse(TEXT("the slot-level question refuses it too"),
		UStratMatchSubsystem::DoesSlotHoldARestorableMatch(FString(SlotName), SlotRefusal));

	TestTrue(
		*FString::Printf(
			TEXT("and returns the predicate's refusal unchanged (predicate: '%s', slot: '%s')"),
			*PredicateRefusal.ToString(), *SlotRefusal.ToString()),
		SlotRefusal.ToString().Equals(PredicateRefusal.ToString(), ESearchCase::CaseSensitive));

	UGameplayStatics::DeleteGameInSlot(FString(SlotName), UserIndex);
	TestFalse(TEXT("the fixture slot is gone after the clause"),
		UGameplayStatics::DoesSaveGameExist(FString(SlotName), UserIndex));

	return true;
}
