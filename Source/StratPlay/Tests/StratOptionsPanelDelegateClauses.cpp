// `UStratShellSubsystem::OnOptionsPanelStateChanged` -- the shell's route-state signal, under
// `GATE-TITLEMENU`.
//
// THE ID IS RULED AND NOT CHOSEN HERE. `Tools/architect/state/global.md`'s third acceptance-ID
// ruling of 2026-09-05: *"This is the shell's route-state signal for the SAME `Options` arm
// `GATE-TITLEMENU`'s authorizing text already claims ... It is the identical class and the
// identical two verbs the existing `GATE-TITLEMENU.ExecuteRouteOptionsRequestsThePanelOncePerCall`
// clause already exercises; a new clause on the delegate's own unfiltered-broadcast property is
// the same subject, not a new one."* No name is minted.
//
// ---------------------------------------------------------------------------------------
// WHAT THIS PINS THAT `StratShellOptionsRouteClauses.cpp` DOES NOT.
//
// That file's second clause drives `ExecuteRoute(Options)` and reads the COUNTER afterwards. It
// says nothing about the delegate, because no observer was bound. THE DELEGATE IS A DIFFERENT
// FACT WITH A DIFFERENT FAILURE MODE, and it has exactly one consumer --
// `UStratOptionsPresenter` -- so a broadcast that is filtered, or that fires before the members
// it announces are written, is a defect whose only symptom is a screen that does not appear.
//
// THE TWO PROPERTIES, AND WHY EACH IS NOT OBVIOUS.
//
//   1. IT IS UNFILTERED, ON BOTH WRITERS. The natural way to write `CloseOptionsPanel` is
//      `if (bOptionsPanelOpen) { ... Broadcast(); }`, and the natural way to write
//      `RequestOptionsPanel` is the mirror of it. Either would be a CHANGE-filtered delegate,
//      and the declaration of `FStratOptionsPanelStateChanged` refuses that in terms:
//      *"`RequestOptionsPanel` on an already-open panel still increments the count, and the
//      count is this route's whole observability."* A filtered broadcast is invisible in every
//      ordinary session -- the panel still opens the first time -- and shows up only as a
//      reconcile that never runs after a state the observer missed. So the clause drives BOTH
//      writers TWICE, and the second call of each pair is the whole point.
//
//   2. IT CARRIES THE POST-WRITE VALUES, NOT THE PRE-WRITE ONES. `RequestOptionsPanel`'s body
//      says why: *"An observer that reads `GetOptionsPanelRequestCount` from inside the handler
//      -- which is the natural thing for a clause to do -- must not see a count that lags the
//      flag it was just handed."* That ordering is unobservable from outside the broadcast, so
//      `UStratOptionsPanelStateObserver` reads both members from INSIDE its handler and the
//      clause compares what it saw there against what the subsystem reports afterwards.
//
// WHERE THE EXPECTATIONS COME FROM. Every value compared is the subsystem's own published
// reading -- `IsOptionsPanelOpen()` and `GetOptionsPanelRequestCount()` -- taken either after
// the call or, for the ordering half, during it. The only literals are broadcast COUNTS, and
// those are the property itself ("one call, one broadcast") rather than a derived expectation;
// each is asserted as a delta on an observer whose zero was checked first.
//
// NO WORLD, NO LEVEL AND NO SAVE SLOT. `UStratShellSubsystem` is a `UGameInstanceSubsystem` and
// the two verbs under test write two members and broadcast. The outer must be a `UGameInstance`
// -- `ClassWithin` -- and the transient package fires a handled ensure, measured and recorded in
// `StratShellRouteClauses.cpp`.
//
// WHAT THIS DOES **NOT** PIN.
//   - THAT ANY OBSERVER IS EVER BOUND IN A SHIPPED WORLD. `UStratOptionsPresenter` binds this
//     in `OnWorldBeginPlay`; that is `StratOptionsPresenterClauses.cpp`'s subject.
//   - THAT `ExecuteRoute` REACHES `RequestOptionsPanel`. That is
//     `GATE-TITLEMENU.ExecuteRouteOptionsRequestsThePanelOncePerCall`, unchanged.
//   - ANY ORDER BETWEEN MULTIPLE OBSERVERS. One observer is bound; a multicast's delivery order
//     is the engine's and no consumer here depends on it.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/GameInstance.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratOptionsPanelDoubles.h"
#include "StratShellSubsystem.h"

// ---------------------------------------------------------------------------------------
// The panel-state delegate fires on every write, unfiltered, after both members are written.
//
// MUTANTS:
//   - `CloseOptionsPanel` wrapped in `if (bOptionsPanelOpen)` -- red on the fourth broadcast.
//   - `RequestOptionsPanel` wrapped in `if (!bOptionsPanelOpen)` -- red on the second.
//   - `RequestOptionsPanel` broadcasting BEFORE `++OptionsPanelRequestCount` -- red on the
//     inside-the-handler count, with a message printing the lagged value.
//   - either `Broadcast` deleted -- red on the first delta.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsPanelDelegateIsUnfilteredTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheOptionsPanelDelegateFiresUnfilteredAfterBothMembersAreWritten",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsPanelDelegateIsUnfilteredTest::RunTest(const FString& /*Parameters*/)
{
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

	const TStrongObjectPtr<UStratOptionsPanelStateObserver> Observer(
		NewObject<UStratOptionsPanelStateObserver>());
	if (!Observer.IsValid())
	{
		AddError(TEXT("could not construct an observer; a dynamic multicast cannot be watched "
			"without one and nothing would have been measured."));
		return false;
	}

	Observer->Watched = Shell.Get();
	Shell->OnOptionsPanelStateChanged.AddDynamic(
		Observer.Get(), &UStratOptionsPanelStateObserver::HandleStateChanged);

	// ---- PREMISES --------------------------------------------------------------------------
	TestEqual(TEXT("premise: a fresh shell has broadcast nothing"), Observer->BroadcastCount, 0);
	TestFalse(TEXT("premise: and the panel is closed"), Shell->IsOptionsPanelOpen());
	TestEqual(TEXT("premise: and the request count is zero"),
		Shell->GetOptionsPanelRequestCount(), 0);

	// ---- 1. THE FIRST REQUEST --------------------------------------------------------------
	Shell->RequestOptionsPanel();

	TestEqual(TEXT("RequestOptionsPanel broadcasts exactly once"), Observer->BroadcastCount, 1);
	TestTrue(TEXT("and the broadcast carries true, which is the post-write flag"),
		Observer->bLastFlag);
	TestEqual(TEXT("and the argument agrees with what the subsystem reports afterwards"),
		Observer->bLastFlag, Shell->IsOptionsPanelOpen());

	// THE ORDERING HALF. `RequestOptionsPanel` writes the flag, then the counter, THEN
	// broadcasts. An observer that saw 0 here would be reading a count that lags the flag it was
	// handed -- the exact defect that body's own comment names.
	TestEqual(*FString::Printf(
		TEXT("the observer saw the INCREMENTED request count from inside the handler (it saw "
		     "%d; the subsystem reports %d afterwards). A lower number here means the broadcast "
		     "happens before the members it announces are written"),
		Observer->CountSeenInsideHandler, Shell->GetOptionsPanelRequestCount()),
		Observer->CountSeenInsideHandler, Shell->GetOptionsPanelRequestCount());
	TestTrue(TEXT("and it saw the flag already true from inside the handler"),
		Observer->bFlagSeenInsideHandler);

	// ---- 2. THE SECOND REQUEST, AGAINST AN ALREADY-OPEN PANEL ------------------------------
	// THIS IS THE UNFILTERED HALF ON THE OPENING SIDE. The flag does not change here, so a
	// change-filtered delegate is silent and this delta is 0.
	const int32 BroadcastsBefore = Observer->BroadcastCount;
	const int32 CountBefore      = Shell->GetOptionsPanelRequestCount();

	Shell->RequestOptionsPanel();

	TestEqual(TEXT("a SECOND request against an already-open panel still broadcasts. This is the "
		"unfiltered property: the flag did not change, and the delegate must fire anyway"),
		Observer->BroadcastCount - BroadcastsBefore, 1);
	TestTrue(TEXT("and it still carries true"), Observer->bLastFlag);
	TestEqual(TEXT("and the count it saw moved with it"),
		Observer->CountSeenInsideHandler - CountBefore, 1);

	// ---- 3. THE CLOSE ----------------------------------------------------------------------
	const int32 BeforeClose = Observer->BroadcastCount;
	Shell->CloseOptionsPanel();

	TestEqual(TEXT("CloseOptionsPanel broadcasts exactly once"),
		Observer->BroadcastCount - BeforeClose, 1);
	TestFalse(TEXT("and the broadcast carries false"), Observer->bLastFlag);
	TestEqual(TEXT("which agrees with what the subsystem reports afterwards"),
		Observer->bLastFlag, Shell->IsOptionsPanelOpen());
	TestFalse(TEXT("and the observer saw the flag already false from inside the handler"),
		Observer->bFlagSeenInsideHandler);

	// ---- 4. THE SECOND CLOSE, AGAINST AN ALREADY-CLOSED PANEL ------------------------------
	// THE UNFILTERED HALF ON THE CLOSING SIDE, AND THE ONE A DEFENSIVE `if` WOULD KILL FIRST.
	// `UStratOptionsPresenter::Deinitialize` closes a flag it may never have opened, and it
	// depends on nothing about whether the flag was set.
	const int32 BeforeSecondClose = Observer->BroadcastCount;
	Shell->CloseOptionsPanel();

	TestEqual(TEXT("a SECOND close against an already-closed panel still broadcasts"),
		Observer->BroadcastCount - BeforeSecondClose, 1);
	TestFalse(TEXT("and still carries false"), Observer->bLastFlag);

	// ---- 5. THE CLOSE DID NOT DECREMENT THE HISTORY ----------------------------------------
	TestEqual(TEXT("neither close moved the request count -- it is a history, not a depth"),
		Shell->GetOptionsPanelRequestCount(), CountBefore + 1);

	// ---- 6. AND THE OBSERVER'S TOTAL IS FOUR, NOT TWO --------------------------------------
	// The whole-clause restatement: four writes, four broadcasts. A change-filtered delegate
	// gives two and every delta above is where it went wrong.
	TestEqual(TEXT("four writes produced four broadcasts"), Observer->BroadcastCount, 4);

	return true;
}
