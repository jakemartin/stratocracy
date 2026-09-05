// The fifth `EStratShellRoute` arm -- `Options` -- as a pair of clauses. The local ID
// `GATE-TITLEMENU` is authorized for this subject by the 2026-09-05 ruling in
// `Tools/architect/state/global.md`, which mints no acceptance ID and claims none: *"A new
// `Options` arm that does not travel is a route on the same enum that ID already covers, full
// stop"*. `Source/StratPlay/Tests/StratShellRouteClauses.cpp` is where the other twenty-odd
// `GATE-TITLEMENU` clauses live; this file is separate only because its two clauses share a
// subject that file's header does not describe, and putting them there would have made that
// header's careful account of what is and is not reachable wrong by addition.
//
// ---------------------------------------------------------------------------------------
// THE DEFECT THESE CLAUSES EXIST FOR, IN THE PLAYER'S TERMS: CLICKING "OPTIONS" QUIT THE GAME.
//
// `ExecuteRoute` reached its quit arm by `!RouteTravels(Route)`. That test named the quit route
// and only the quit route for exactly as long as `QuitGame` was the only route that did not
// travel. `Options` is the second, and on the shape the code had, a player asking for the volume
// screen would have been dropped to the desktop.
//
// NOTHING IN THE SUITE COULD SEE IT, AND THAT IS THE REASON THE FIRST CLAUSE IS SHAPED THE WAY
// IT IS. `UKismetSystemLibrary::QuitGame` does nothing under `-nullrhi` in an automation run --
// no window closes, no process exits, no line is logged -- so a clause that called
// `ExecuteRoute(Options)` on the broken code would have observed a `true` return and a silence
// indistinguishable from success. The wrong behaviour is unobservable AT THE CALL. It is fully
// observable one level down, at the two `static` predicates the engineer split the fact into,
// which is where the first clause reads it.
//
// SO THE TWO CLAUSES DIVIDE AS: WHAT THE ROUTE **IS** (predicates, world-free, where the bug
// lives) and WHAT `ExecuteRoute` **DOES** WITH IT (the arm, executed for real). Neither is
// sufficient alone. The first cannot see whether `ExecuteRoute` still consults the predicates;
// the second cannot see the difference between the shipped arm and the broken one.
//
// WHERE THE EXPECTATIONS COME FROM. Clause one enumerates `EStratShellRoute` off its own
// reflected `UENUM` and writes no route list of its own -- a hand-written list of five would be
// this file deciding what the route set is and would stop covering a sixth silently, which is
// the exact species of gap the whole file is about. It asserts a PARTITION rather than five
// individual answers, for the reason given at the clause. Clause two reads
// `GetOptionsPanelRequestCount()` -- the subsystem's own published counter -- before and after,
// and asserts nothing about an absolute value it did not first observe on the same object.
//
// WHAT NEITHER CLAUSE PINS, STATED RATHER THAN LEFT TO BE INFERRED.
//   - THAT THE PROCESS DOES NOT ACTUALLY EXIT. Nothing here calls `QuitGame` and nothing could
//     observe it if it did; see above. What is pinned is that `Options` is not classified as the
//     exiting route and that `ExecuteRoute`'s `Options` arm reaches `RequestOptionsPanel`. A
//     hypothetical `ExecuteRoute` that called `QuitGame` AND then requested the panel would pass
//     both clauses. That is not a shape any edit to the current body produces -- the two arms are
//     mutually exclusive `return`s -- and it is named because the clause names cannot say it.
//   - THAT ANY WIDGET IS SHOWN. `bOptionsPanelOpen` is a flag an owner binds to; no options
//     surface is wired to it in this tree, which `StratShellSubsystem.h`'s own header records as
//     an open debt. These clauses pin the request, not a pixel.
//   - THAT THE TITLE MENU CARRIES AN `Options` ROW. `BuildMenuModel`'s by-index parity against
//     the enum's declaration order is `StratShellRouteClauses.cpp`'s subject and is unchanged.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/GameInstance.h"
#include "Templates/UniquePtr.h"
#include "UObject/Class.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratShellSubsystem.h"

namespace StratShellOptionsRoute
{
	/** A route's reflected name, for failure messages. Asked of the `UENUM` rather than
	 *  switched over here, on `StratShippedSoundBankParity.cpp`'s `CueWord` precedent: a
	 *  hand-written switch is a second author of the route names and prints a stale one the day
	 *  a route is renamed. Display only; nothing is asserted about the string. */
	FString RouteWord(const EStratShellRoute Route)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratShellRoute>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Route));
		}
		return FString::Printf(TEXT("<unreflected route %d>"), static_cast<int32>(Route));
	}

	/**
	 * Every route the shell has, READ OFF THE REFLECTED ENUM AND NOT WRITTEN HERE.
	 *
	 * `UHT` appends a hidden `_MAX` sentinel to every `UENUM`; it is skipped by name. Returns
	 * empty when the enum is unreflected, and the caller treats empty as red rather than as
	 * "no routes to check" -- an empty loop passes every assertion inside it.
	 */
	TArray<EStratShellRoute> AllRoutes()
	{
		TArray<EStratShellRoute> Routes;

		const UEnum* const Reflected = StaticEnum<EStratShellRoute>();
		if (Reflected == nullptr)
		{
			return Routes;
		}

		for (int32 Index = 0; Index < Reflected->NumEnums(); ++Index)
		{
			const FString Name = Reflected->GetNameStringByIndex(Index);
			if (Name == TEXT("EStratShellRoute_MAX"))
			{
				continue;
			}
			Routes.Add(static_cast<EStratShellRoute>(Reflected->GetValueByIndex(Index)));
		}

		return Routes;
	}

	/** A fixture slot name used nowhere else, so its survival across a call is legible. */
	const TCHAR* const kStaleArm = TEXT("StratOptions_StaleArm_MiXeDcAsE");
}

// ---------------------------------------------------------------------------------------
// `Options` does not exit the process.
//
// A PARTITION AND NOT FIVE SEPARATE ANSWERS, AND THE DIFFERENCE IS THE WHOLE CLAUSE. Asserting
// only `RouteExitsProcess(Options) == false` would go green against a `RouteExitsProcess` that
// returned false for everything -- including for `QuitGame`, which would leave the Quit button
// doing nothing at all. Asserting only "exactly one route exits" would go green if the exiting
// one were `ReturnToTitle`. So the clause states the full shape: over every declared route,
// EXACTLY ONE of `RouteTravels` / `RouteExitsProcess` / `RouteOpensOptions` holds, the exiting
// one is `QuitGame` and the options-opening one is `Options`.
//
// THE MUTANT THIS IS BUILT AGAINST IS THE ORIGINAL DEFECT, RESTATED AS A PREDICATE:
// `RouteExitsProcess(Route) { return !RouteTravels(Route); }`. Under it, `Options` satisfies TWO
// of the three predicates and the partition fails on the route the whole feature is about, with
// a message naming it.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellOptionsDoesNotExitTheProcessTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheOptionsRouteDoesNotExitTheProcess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellOptionsDoesNotExitTheProcessTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellOptionsRoute;

	const TArray<EStratShellRoute> Routes = AllRoutes();

	// AN EMPTY LIST IS RED AND NOT VACUOUSLY GREEN. Every assertion below lives inside the
	// loop, so an unreflected enum would report a clean pass over nothing.
	if (Routes.Num() == 0)
	{
		AddError(TEXT("EStratShellRoute is unreflected, so no route was classified at all; "
			"nothing was observed."));
		return false;
	}

	int32 Travelling = 0;
	int32 Exiting    = 0;
	int32 Opening    = 0;

	for (const EStratShellRoute Route : Routes)
	{
		const bool bTravels = UStratShellSubsystem::RouteTravels(Route);
		const bool bExits   = UStratShellSubsystem::RouteExitsProcess(Route);
		const bool bOpens   = UStratShellSubsystem::RouteOpensOptions(Route);

		Travelling += bTravels ? 1 : 0;
		Exiting    += bExits   ? 1 : 0;
		Opening    += bOpens   ? 1 : 0;

		const int32 Claims = (bTravels ? 1 : 0) + (bExits ? 1 : 0) + (bOpens ? 1 : 0);

		TestEqual(*FString::Printf(
			TEXT("route %s is exactly one of travelling, exiting or options-opening "
			     "(travels=%d exits=%d opens=%d)"),
			*RouteWord(Route), bTravels ? 1 : 0, bExits ? 1 : 0, bOpens ? 1 : 0),
			Claims, 1);
	}

	// THE THREE IDENTITIES, so the partition cannot be satisfied by the wrong routes wearing
	// each other's roles.
	TestFalse(TEXT("the Options route does not exit the process"),
		UStratShellSubsystem::RouteExitsProcess(EStratShellRoute::Options));
	TestTrue(TEXT("control: the Quit route does exit the process, so the predicate is not "
		"simply false for everything"),
		UStratShellSubsystem::RouteExitsProcess(EStratShellRoute::QuitGame));
	TestTrue(TEXT("the Options route is the one that opens the options panel"),
		UStratShellSubsystem::RouteOpensOptions(EStratShellRoute::Options));
	TestFalse(TEXT("and the Options route does not travel, which is what made the old "
		"!RouteTravels test ambiguous"),
		UStratShellSubsystem::RouteTravels(EStratShellRoute::Options));

	// EXACTLY ONE OF EACH KIND, over the whole enum. The counts are compared against 1 rather
	// than against the number of routes, because "how many routes travel" is a number this file
	// has no business fixing -- a sixth travelling route is not this clause's business, while a
	// second exiting route or a second options route is.
	TestEqual(TEXT("exactly one route exits the process"), Exiting, 1);
	TestEqual(TEXT("exactly one route opens the options panel"), Opening, 1);
	TestTrue(TEXT("control: at least one route still travels, so the partition is not being "
		"satisfied by a shell that goes nowhere"), Travelling > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// `ExecuteRoute(Options)` requests the panel, once per call.
//
// THIS IS THE FIRST GRANTED ARM OF `ExecuteRoute` ANY CLAUSE HAS EXECUTED END TO END, which
// `StratShellSubsystem.h` states in its own words at `GetOptionsPanelRequestCount`. The
// travelling arms open a level and the quit arm is unobservable; this one changes a member on
// the object in front of us and returns.
//
// WHY IT REACHES `ExecuteRoute` ON A BARE `UGameInstance` AT ALL. Everything the `Options` path
// touches is world-free: `GatherFacts` is safe on a bare instance (its `GetWorld()` is null and
// guarded, the save question short-circuits on the empty name), `IsRoutePermitted(Options)` is
// unconditional, `RouteTravels` is false, and the arm writes two members. The `ButtonClick` cue
// at the top of `ExecuteRoute` is skipped for the same reason -- no world, so no director -- so
// this clause emits no log line and declares no expected message.
//
// COUNTED, NOT FLAGGED, AND THE COUNT IS READ BEFORE AND AFTER. `IsOptionsPanelOpen()` alone
// cannot tell a call that ran from a flag some earlier call left true, and an absolute count
// cannot tell a first call from a repeat. Both are asserted as DELTAS on the same object.
//
// THE SECOND CALL IS THE POSITIVE CONTROL AND ALSO THE "EXACTLY ONCE" HALF. A body that
// requested the panel twice per call, or that requested it once and never again, is red here;
// a body that never requested it is red on the first delta.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellExecuteRouteOptionsRequestsPanelTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ExecuteRouteOptionsRequestsThePanelOncePerCall",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellExecuteRouteOptionsRequestsPanelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellOptionsRoute;

	// The outer must be a `UGameInstance`: `UGameInstanceSubsystem` declares
	// `ClassWithin = UGameInstance`, and the transient package fires a handled ensure. Measured
	// and recorded in `StratShellRouteClauses.cpp`.
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

	// NOTHING IS CONFIGURED, DELIBERATELY. No match level and no title level, which is the
	// state in which every travelling route is refused -- so a success here cannot be an
	// accident of a permissive fixture, and an `Options` arm that fell through to the
	// travelling path would be refused rather than quietly passing.
	TestEqual(TEXT("premise: a fresh shell has requested the panel zero times"),
		Shell->GetOptionsPanelRequestCount(), 0);
	TestFalse(TEXT("premise: and the panel is not open"), Shell->IsOptionsPanelOpen());

	// A stale arming whose survival separates "the options arm ran" from "some other arm ran".
	Shell->ArmPendingLoadSlot(FString(kStaleArm));

	const int32 Before = Shell->GetOptionsPanelRequestCount();

	FString FailureReason;
	const bool bTook = Shell->ExecuteRoute(EStratShellRoute::Options, FailureReason);

	TestTrue(TEXT("the Options route is taken"), bTook);
	TestTrue(*FString::Printf(TEXT("and it reports no failure (it said '%s')"), *FailureReason),
		FailureReason.IsEmpty());

	TestEqual(TEXT("one call requested the panel exactly once"),
		Shell->GetOptionsPanelRequestCount() - Before, 1);
	TestTrue(TEXT("and the panel is open"), Shell->IsOptionsPanelOpen());

	// THE OPTIONS ARM IS A LOCAL ACTION AND NOT A TRAVEL, observed through the one piece of
	// cross-level state this subsystem carries: a route that had fallen through to the
	// continue/travel path would have overwritten this.
	TestTrue(TEXT("the Options route armed no load slot: the earlier request is untouched"),
		Shell->PeekPendingLoadSlot().Equals(FString(kStaleArm), ESearchCase::CaseSensitive));

	// THE SECOND CALL. Both halves matter: the counter must move again (so the first delta was
	// not a one-shot latch) and it must move by exactly one (so the arm is not requesting twice).
	const int32 Between = Shell->GetOptionsPanelRequestCount();
	FString SecondFailureReason;
	const bool bTookAgain = Shell->ExecuteRoute(EStratShellRoute::Options, SecondFailureReason);

	TestTrue(TEXT("the Options route is taken a second time"), bTookAgain);
	TestEqual(TEXT("and the second call also requested the panel exactly once"),
		Shell->GetOptionsPanelRequestCount() - Between, 1);

	// CLOSING THE PANEL DOES NOT UNDO THE HISTORY, which is the property that makes the counter
	// usable as an instrument at all. Asserted here rather than in a clause of its own because
	// it is the premise every delta above rests on.
	Shell->CloseOptionsPanel();
	TestFalse(TEXT("closing the panel closes it"), Shell->IsOptionsPanelOpen());
	TestEqual(TEXT("but leaves the request history alone"),
		Shell->GetOptionsPanelRequestCount(), Between + 1);

	return true;
}
