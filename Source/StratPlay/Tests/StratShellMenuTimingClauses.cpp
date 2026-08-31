// The gate on §2.11.5's shell HUD -- `AStratShellHUD::DecideMenuTiming` and the subsystem flag
// it reads -- under the local gate name `GATE-TITLEMENU`.
//
// WHY THE GATE NAME AND NOT AN ACCEPTANCE ID. Unchanged from `StratShellRouteClauses.cpp`,
// which measured it: §2.11.5's last paragraph is the only place the title screen appears in the
// GDD, it names no route and no HUD, and §2.11.8's must-have ranking does not list the screen.
// Inventing an ID would put a string in a test name that greps exactly like a defined one.
// `GATE-TITLEMENU` is a local gate name ruled by the user on 2026-08-29.
//
// WHAT THIS FILE IS FOR, AND IT IS NOT "THE HUD DRAWS A MENU". It pins ONE property:
//
//     THE SHELL IS CONFIGURED BEFORE THE MENU IS ASKED FOR ITS MODEL.
//
// The distinction that makes this worth writing is that the OBVIOUS clause here was already
// offered to this lane and REFUSED, and the refusal is recorded in `global.md`.
// `AStratShellGameMode::BeginPlay` calls `Super::BeginPlay()` -- which fires a Blueprint's
// `Event BeginPlay` -- before `ConfigureMatchDestination`, so `BP_StratShellGameMode`'s graph
// hops one tick before creating the menu. A clause asserting THAT ORDERING would pin a hazard
// as a requirement and go red the day somebody repaired it. This project's own note says it in
// four words: pin the requirement, not the hazard.
//
// So nothing below asserts when `ConfigureMatchDestination` runs. The clauses assert that a
// configured shell is drawn from immediately and an unconfigured one is waited for -- which
// stays green under the repair, since moving the configuring call ahead of `Super::BeginPlay()`
// merely makes the wait zero ticks.
//
// WHERE THE EXPECTATIONS COME FROM, which is the only interesting question about any file here.
//   - The wait budget is READ FROM `AStratShellHUD`'s CLASS DEFAULT OBJECT, never typed. The
//     shipped default is a number this file must not become a second author of; a designer who
//     changes it on the C++ default moves these clauses with it, and one who changes it on a
//     Blueprint moves neither, which is correct -- the C++ default is what this file is about.
//   - The three arms are compared as ENUM VALUES, not as strings. `FString` comparison in this
//     engine is case-insensitive for `==`, `Contains` and `TestEqual` alike -- measured in this
//     project, where it once shipped a clause that could not fail -- so no arm name is ever
//     compared as text. Names are reflected out of `StaticEnum` for the FAILURE MESSAGE only,
//     where being wrong costs readability and not a verdict.
//   - The subsystem clause reads the flag and the configured value TOGETHER, so that "the call
//     happened" and "the call carried something" are asserted to be different facts rather
//     than assumed to be.
//
// THE VACUITY GUARD MATTERS MORE HERE THAN USUAL AND IS WRITTEN OUT. The boundary clause loops
// over `[0, Budget)`. If the shipped budget were ever zero that loop body would never run and
// the clause would pass having observed NOTHING -- this project's recorded "a clause can pass
// while measuring the defect" shape. The clause therefore asserts the budget is positive BEFORE
// the loop, so a budget of zero reddens it rather than silencing it.
//
// WHAT NO CLAUSE IN THIS FILE REACHES, said plainly rather than left to be inferred. THE
// BIGGEST GAP IS NAMED FIRST BECAUSE IT IS THE ONE A READER WOULD OTHERWISE ASSUME CLOSED:
//
//   - **THAT `AStratShellHUD::ResolveMenuTiming` ACTUALLY CONSULTS `DecideMenuTiming`.** These
//     clauses pin the decider. They do not pin the CALL SITE. A `ResolveMenuTiming` rewritten
//     to create the menu unconditionally would leave every clause below GREEN. That much is
//     still true and was re-measured: mutant D in the run described below did exactly that
//     rewrite, and all four clauses in THIS file stayed green under it.
//     **[STAMPED 2026-08-31 -- THE SENTENCE THAT FOLLOWED WAS FALSE AND IS QUOTED RATHER THAN
//     DELETED, because a reader who arrives here by a citation must learn what it claimed.]**
//     RETRACTED>  "Reaching it needs a spawned HUD with an owning player controller and a live
//     RETRACTED>   game instance carrying the subsystem, which is a PIE-shaped fixture and not
//     RETRACTED>   a headless one. ... this is the half that remains unpinned."
//     IT WAS NEVER MEASURED -- it was inferred from the arm ending in `CreateWidget` -- and it
//     was HALF WRONG. The decision is reachable headlessly and only the DRAWING is not: a
//     transient world, a `UGameInstance::InitializeStandalone` whose subsystem collection is
//     live, a spawned controller and `DispatchBeginPlay` reach `ResolveMenuTiming` for real,
//     with no PIE anywhere. `StratShellHudCallSiteClauses.cpp` is that fixture and it closes
//     this gap; its header records the three routes to a genuinely successful create that were
//     measured and rejected. What remains unreachable is the menu ever APPEARING, which needs a
//     viewport. That narrower statement is the same shape `UStratShellSubsystem` records for
//     `ExecuteRoute`'s travelling arm.
//   - The created widget, its Z-order, and `bTakeUiOnlyInputMode`'s effect on the player
//     controller. All three need a viewport.
//   - The re-arming next-tick timer. A headless clause has no tick to re-arm on.
//   - `bMenuDrewOnAConfiguredShell` and `ConfigurationTicksWaited` as WRITTEN VALUES. They are
//     written on the path above, which is the path no clause here reaches.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "Engine/GameInstance.h"

#include "StratShellHUD.h"
#include "StratShellSubsystem.h"

namespace
{
	/**
	 * An arm's declared name, for a failure message and for nothing else.
	 *
	 * REFLECTED RATHER THAN SWITCHED, so a renamed enumerator renames itself here too and a
	 * message can never name an arm that no longer exists. It is never compared against
	 * anything -- see the file header on why no arm is compared as text.
	 */
	FString TimingName(const EStratShellMenuTiming Timing)
	{
		if (const UEnum* const Declared = StaticEnum<EStratShellMenuTiming>())
		{
			return Declared->GetNameStringByValue(static_cast<int64>(Timing));
		}

		return TEXT("<EStratShellMenuTiming is not reflected>");
	}
}

// ---------------------------------------------------------------------------------------
// THE REQUIREMENT ITSELF, AND THE ONE CLAUSE IN THIS FILE THAT IS LOAD-BEARING.
//
// A configured shell is asked for its model immediately -- at every point in the wait budget
// INCLUDING PAST ITS END. The second half is what makes this more than a restatement of the
// first branch: a `DecideMenuTiming` whose budget check came FIRST would answer
// `CreateUnconfigured` for a shell that is configured, and the menu would draw its greyed
// fallback over a shell that had everything it needed. That reordering is the plausible
// refactor here, and it is the one this clause exists to redden.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellConfiguredShellNeverWaitsTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AConfiguredShellIsDrawnFromAtEveryPointInTheWaitBudget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellConfiguredShellNeverWaitsTest::RunTest(const FString& /*Parameters*/)
{
	// SWEPT RATHER THAN SAMPLED, and the range deliberately runs past the budget on both
	// arguments: the claim is that NO combination of waiting and budget can route a configured
	// shell anywhere but `CreateNow`, so a single well-chosen pair would be a weaker claim than
	// the one the header makes.
	int32 ObservedPairs = 0;

	for (int32 Budget = 0; Budget <= 5; ++Budget)
	{
		for (int32 Waited = 0; Waited <= 6; ++Waited)
		{
			const EStratShellMenuTiming Answer =
				AStratShellHUD::DecideMenuTiming(/*bShellIsConfigured=*/true, Waited, Budget);

			++ObservedPairs;

			TestTrue(FString::Printf(
				TEXT("a configured shell is drawn from at once -- waited %d of a %d-tick "
					"budget, expected %s, got %s"),
				Waited, Budget,
				*TimingName(EStratShellMenuTiming::CreateNow),
				*TimingName(Answer)),
				Answer == EStratShellMenuTiming::CreateNow);
		}
	}

	// THE VACUITY GUARD. A loop that ran zero times would have asserted nothing and reported
	// agreement, which is the failure this project has already shipped once.
	TestTrue(TEXT("premise: the sweep above actually ran"), ObservedPairs > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// THE BOUNDARY, TAKEN AT THE SHIPPED BUDGET RATHER THAN AT A NUMBER THIS FILE CHOSE.
//
// Below the budget the answer is `WaitForConfiguration`; at it and past it the answer is
// `CreateUnconfigured`. The budget is read from the class default object, so this clause moves
// when the shipped default moves and cannot become a second author of it.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellUnconfiguredWaitBoundaryTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AnUnconfiguredShellIsWaitedForUntilItsShippedBudgetIsSpent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellUnconfiguredWaitBoundaryTest::RunTest(const FString& /*Parameters*/)
{
	const AStratShellHUD* const HudDefaults = GetDefault<AStratShellHUD>();
	if (HudDefaults == nullptr)
	{
		AddError(TEXT("could not resolve AStratShellHUD's class default object; this clause "
			"has observed nothing and will not report that as agreement."));
		return false;
	}

	const int32 Budget = HudDefaults->MaxConfigurationWaitTicks;

	// THE VACUITY GUARD, FIRST, AND IT IS NOT DECORATION HERE. The loop below runs `Budget`
	// times. At zero it would run none, assert nothing, and pass -- so the premise is asserted
	// rather than assumed, and a shipped budget of zero reddens this clause instead of
	// silencing it. The third clause in this file is the one that covers a zero budget on
	// purpose; this one covers the shipped one.
	TestTrue(FString::Printf(
		TEXT("premise: the shipped wait budget is positive, so the wait arm below is actually "
			"exercised -- read %d from AStratShellHUD's class default object"), Budget),
		Budget > 0);

	for (int32 Waited = 0; Waited < Budget; ++Waited)
	{
		const EStratShellMenuTiming Answer =
			AStratShellHUD::DecideMenuTiming(/*bShellIsConfigured=*/false, Waited, Budget);

		TestTrue(FString::Printf(
			TEXT("an unconfigured shell is still being waited for after %d of %d ticks -- "
				"expected %s, got %s"),
			Waited, Budget,
			*TimingName(EStratShellMenuTiming::WaitForConfiguration),
			*TimingName(Answer)),
			Answer == EStratShellMenuTiming::WaitForConfiguration);
	}

	// AT THE BUDGET AND PAST IT. Both, because an off-by-one that waited one tick too long
	// would be invisible to a clause that only asked about `Budget + 1`.
	for (const int32 Waited : { Budget, Budget + 1 })
	{
		const EStratShellMenuTiming Answer =
			AStratShellHUD::DecideMenuTiming(/*bShellIsConfigured=*/false, Waited, Budget);

		TestTrue(FString::Printf(
			TEXT("a spent budget draws the menu anyway rather than waiting forever -- waited "
				"%d of %d, expected %s, got %s"),
			Waited, Budget,
			*TimingName(EStratShellMenuTiming::CreateUnconfigured),
			*TimingName(Answer)),
			Answer == EStratShellMenuTiming::CreateUnconfigured);
	}

	return true;
}

// ---------------------------------------------------------------------------------------
// A ZERO BUDGET DRAWS RATHER THAN WAITING, WHICH IS A PROMISE THE HEADER MAKES IN TERMS:
// "at zero the class still draws, on the `CreateUnconfigured` arm, which is why that arm
// exists."
//
// WHAT THIS CLAUSE IS WORTH, STATED NARROWLY BECAUSE THE WIDE VERSION IS FALSE. It is NOT the
// only clause that catches an off-by-one: a `DecideMenuTiming` using `<=` where it uses `<`
// reddens the boundary clause above as well, at `Waited == Budget`, for as long as the shipped
// budget is positive. Saying otherwise would have been an over-broad claim about a clause's
// subject, which is a shape this project has been bitten by and which the mutant run for this
// file measured directly.
//
// WHAT IT IS ACTUALLY FOR is the ONE configuration the clause above cannot observe: a budget of
// zero, where that clause's loop runs no iterations and its own vacuity guard reddens rather
// than reporting on the arm. A designer who sets `MaxConfigurationWaitTicks` to zero on a
// Blueprint is asking for "draw immediately", and this is the clause that says what that means.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellZeroBudgetDrawsAtOnceTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AZeroWaitBudgetDrawsTheMenuRatherThanWaitingForATick",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellZeroBudgetDrawsAtOnceTest::RunTest(const FString& /*Parameters*/)
{
	const EStratShellMenuTiming Answer =
		AStratShellHUD::DecideMenuTiming(/*bShellIsConfigured=*/false,
			/*TicksWaited=*/0, /*MaxTicksToWait=*/0);

	TestTrue(FString::Printf(
		TEXT("a zero budget draws immediately on the unconfigured arm -- expected %s, got %s"),
		*TimingName(EStratShellMenuTiming::CreateUnconfigured),
		*TimingName(Answer)),
		Answer == EStratShellMenuTiming::CreateUnconfigured);

	return true;
}

// ---------------------------------------------------------------------------------------
// THE FLAG THE REQUIREMENT RESTS ON, AND WHY A SUBSYSTEM CLAUSE LIVES IN A HUD FILE.
//
// `AStratShellHUD` asks `UStratShellSubsystem::HasMatchDestinationBeenConfigured` and nothing
// else. If that function ever came to mean "the destination is USABLE" rather than "the call
// HAPPENED", the HUD's whole wait would change meaning without a line of the HUD changing --
// a shell configured with a null level would be waited for and then drawn as a timeout, which
// is a Warning in the log for an ordinary unconfigured Blueprint.
//
// THE MUTATION THIS EXISTS TO CATCH IS A TIDYING ONE. `return !MatchLevel.IsNull();` looks like
// a simplification that removes a redundant bool. It is the recorded
// real-default-cannot-signal-unset defect, which in this project once wrote the player's save
// every run. The clause below configures the shell with NOTHING -- a null level and an empty
// slot -- and asserts the flag flips anyway.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellConfiguredFlagRecordsTheCallTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheShellsConfiguredFlagRecordsTheCallAndNotItsArguments",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellConfiguredFlagRecordsTheCallTest::RunTest(const FString& /*Parameters*/)
{
	// THE OUTER IS A `UGameInstance` AND NOT THE TRANSIENT PACKAGE. `UGameInstanceSubsystem`
	// declares `ClassWithin = UGameInstance`, so the transient package is an illegal outer and
	// `StaticAllocateObject` fires a handled ensure. `StratShellRouteClauses.cpp` records the
	// exact message this went red with; the shape is copied from there rather than rediscovered.
	// The instance is never initialised and never gets a world -- it is an outer and nothing
	// else -- and every member this clause touches is a plain field, which is why it needs no
	// map and survives `-nullrhi`.
	const TStrongObjectPtr<UGameInstance> OwningInstance(NewObject<UGameInstance>());
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

	// BOTH DIRECTIONS, AND THE FALSE ONE FIRST. A flag that was true from construction would
	// make the HUD's wait unreachable and every other assertion here vacuous.
	TestFalse(TEXT("a freshly constructed shell has not been configured"),
		Shell->HasMatchDestinationBeenConfigured());

	// CONFIGURED WITH NOTHING AT ALL: a null level and an empty slot. Every argument is the
	// value an unconfigured Blueprint would have carried, so nothing about the call except the
	// call itself can be what flips the flag.
	Shell->ConfigureMatchDestination(TSoftObjectPtr<UWorld>(), FString());

	TestTrue(TEXT("the call flipped the flag even though it carried a null level and an empty "
		"slot -- the flag records the call, not its arguments"),
		Shell->HasMatchDestinationBeenConfigured());

	// THE OTHER HALF OF THE SAME FACT, and it is what stops the assertion above from being
	// satisfiable by a flag that merely tracked `SaveSlotName`. The configured value really is
	// empty at the moment the flag reads true, so "configured" and "carries something" are
	// observably two different questions.
	TestTrue(TEXT("premise: the slot the call carried really was empty, so the flag above is "
		"not reporting on a value that happened to be set"),
		Shell->GetSaveSlotName().IsEmpty());

	// A SECOND CALL DOES NOT UNSET IT. The header says the flag never clears; a reader who
	// added an `Unconfigure` path would break the HUD's wait in a way nothing else would see.
	Shell->ConfigureMatchDestination(TSoftObjectPtr<UWorld>(), TEXT("StratShell_MiXeDcAsE"));

	TestTrue(TEXT("a second configure leaves the flag set"),
		Shell->HasMatchDestinationBeenConfigured());

	return true;
}
