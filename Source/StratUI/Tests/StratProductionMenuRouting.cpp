// The editor pass for §2.11.5's production-menu ROUTING at the view-model layer --
// GATE-BUILDMENU. GDD §2.11.5, §2.4, §2.7, §4.7 Stub 8.
//
// WHAT THIS FILE'S SUBJECT IS. `StratBuildProductionMenu` is the last leg of the route
// that starts at `strat::uiBuildOptions`: the bridge carries the module's answer to the
// module boundary, and this function carries it the rest of the way into engine types a
// widget can bind. The subject here is that leg -- that every field arrives copied and
// not recomputed, that the rows arrive in the module's order and none is dropped, and
// that a refusal leaves the caller exactly as it found it.
//
// WHAT IS GATED ELSEWHERE, and is deliberately not restated here:
//  - The MODULE's buildlist behaviour -- all four rows, the affordability split, the
//    T-TURN-10 and `buildWaiting` gates, Q31's boxed-in factory, availability never
//    varying by row -- is the crew repo's GATE-BUILDLIST, 14 clauses in `test_ui.cpp`.
//  - The BRIDGE leg -- parity with `strat::uiBuildOptions`, the two channels, the
//    one-row-per-§2.4-row guarantee, the side refusal -- is
//    Source/StratBridge/Tests/StratBuildOptionRouting.cpp. It has to live there:
//    calling `strat::uiBuildOptions` from THIS module is the LNK2019 that
//    StratViewModel.cpp's own comment records, so nothing in this file names that
//    function.
//  - T-UI-04, which is the production menu BINDING -- a widget that does not exist yet.
//    These clauses touch no Slate, construct no widget, and must not be read as moving
//    that row's ledger.
//
// WHERE EVERY EXPECTATION COMES FROM. The right-hand side of every comparison is
// `FStratBridge::BuildOptions` over the SAME bridge, in the same frame, with no command
// submitted between the two calls -- so the authoritative `GameState` is identical
// across both and any difference is this function's doing. That is a module-side value
// read, not a value this file computed: no price, no unit id, no purse comparison and
// no refusal sentence is written down anywhere below.
//
// THE CASE-INSENSITIVITY TRAP, AND WHAT WAS DONE ABOUT IT. UE's `FString` `==`,
// `Contains` and `FAutomationTestBase::TestEqual` are ALL case-insensitive, and `FName`
// comparison is too. A reason-string or unit-id clause written the obvious way
// therefore cannot fail on a casing defect -- it looks like a gate and is not one. So
// both string clauses below compare through `FString::Equals(..., ESearchCase::CaseSensitive)`
// against the module's own `std::string`, and each says so at the call site. `INFANTRY`
// arriving where the table says `Infantry` fails these clauses.
//
// NOT ASSERTED, because the assertion could not fail: that `isFlag` never appears in
// this query's output. `isFlag` is a Scenario.h PLACEMENT field, `FStratBuildOptionView`
// declares no such member, and `data/units.csv` carries no flag row -- so the check
// passes on an empty implementation and a wrong one alike.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint StratUI.Build.cs and StratViewModel.h both record.
#include "StratBridge.h"

// The vendored view-model header, by short name via StratRules' PublicIncludePaths.
// NAMING `strat::UiBuildOption` here is legal and CALLING `strat::uiBuildOptions` from
// this module is not; every rules answer below arrives through an `FStratBridge` method.
#include "Economy.h"
#include "Hex.h"
#include "Ui.h"

namespace StratProductionMenuRouting
{
	// The harness pin the rest of the engine-side suite carries. StratScoreboardParity,
	// StratBridgeParity and StratBridgeQueryParity all hold the same value for the same
	// reason: Replay.h states that no rule decides which side moves first.
	static const int32 kFirstSide = 0;

	// An ARGUMENT, not an expectation -- the malformed-question value the bridge refuses.
	static const int32 kNoSuchSide = strat::SIDE_COUNT;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString Describe(const FIntPoint& P)
	{
		return FString::Printf(TEXT("hex(q=%d,r=%d)"), P.X, P.Y);
	}

	/** The seeded opening: shipped tables mapped in, shipped scenario seeded. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** One question the menu can be asked, in the engine types the façade takes. */
	struct FCase
	{
		FIntPoint Hex   = FIntPoint::ZeroValue;
		int32     Side  = 0;
		FString   What;
	};

	/**
	 * Every question this board can put to the menu, enumerated from the view model
	 * rather than picked -- the same construction the bridge-side file uses, and for the
	 * same reason: a routing failure that bites one case must not be able to hide behind
	 * a case this file happened not to choose. The factory hexes come from
	 * `FStratViewModel::Factories`, which is exactly the value a real caller would pass
	 * back in, so the encoding (X = q, Y = r) is exercised rather than restated.
	 */
	static bool BuildCases(const FStratViewModel& Model, TArray<FCase>& OutCases, FString& OutError)
	{
		OutCases.Reset();

		if (Model.Factories.Num() == 0)
		{
			OutError = TEXT("the seeded scenario carries no factories, so §2.11.5 has nothing to route");
			return false;
		}

		for (const FStratFactoryView& Factory : Model.Factories)
		{
			for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
			{
				FCase Entry;
				Entry.Hex  = Factory.Hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(
					TEXT("side %d at the factory %s (owner %d, builtThisTurn %d, buildWaiting %d)"),
					Side, *Describe(Factory.Hex), Factory.Owner,
					Factory.bHasBuiltThisTurn ? 1 : 0, Factory.bBuildWaiting ? 1 : 0);
				OutCases.Add(Entry);
			}
		}

		// One hex that is no factory at all -- found, never written down.
		for (const FStratHexView& HexView : Model.Hexes)
		{
			bool bIsFactory = false;
			for (const FStratFactoryView& Factory : Model.Factories)
			{
				if (Factory.Hex == HexView.Hex)
				{
					bIsFactory = true;
					break;
				}
			}
			if (bIsFactory)
			{
				continue;
			}

			for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
			{
				FCase Entry;
				Entry.Hex  = HexView.Hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(TEXT("side %d at %s, which is not a factory"),
					Side, *Describe(HexView.Hex));
				OutCases.Add(Entry);
			}
			break;
		}

		return true;
	}

	/** Seeds, builds the view model, and enumerates the case list. */
	static bool Prepare(FStratBridge& Bridge, TArray<FCase>& OutCases, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		FStratViewModel Model;
		if (!StratBuildViewModel(Bridge, kFirstSide, Model, OutError))
		{
			return false;
		}

		return BuildCases(Model, OutCases, OutError);
	}

	/** THE EXPECTATION SIDE: the bridge's own answer to the same question. */
	static bool AskTheBridge(const FStratBridge& Bridge, const FCase& Ask,
	                         std::vector<strat::UiBuildOption>& Out, FString& OutError)
	{
		strat::Hex Factory;
		Factory.q = static_cast<int>(Ask.Hex.X);
		Factory.r = static_cast<int>(Ask.Hex.Y);

		const FStratResult Queried = Bridge.BuildOptions(Ask.Side, Factory, Out);
		if (!Queried.bOk)
		{
			OutError = Queried.Reason;
			return false;
		}
		return true;
	}
}

// ---------------------------------------------------------------------------
// Clause 1 -- the menu is the bridge's answer, row for row and field for field.
//
// ORDER IS PART OF THE CLAIM, for the reason GATE-BRIDGE-DEFS records: a §4.10 Build
// command carries `defIndex` as a RAW index into the definitions vector, so a menu
// whose four offers arrived in a different order would draw correctly and build the
// wrong unit off every button.
//
// `DefIndex` IS SHOWN TO BE A REAL ROW BEFORE IT IS COMPARED. `FStratBuildOptionView::DefIndex`
// defaults to `INDEX_NONE` and `UiBuildOption::defIndex` defaults to -1, so an equality
// alone could not tell "copied" from "both left at their default". Establishing that
// the module's value is a real §2.4 row index is what gives the equality teeth.
//
// `Id` AND `Reason` ARE COMPARED CASE-SENSITIVELY, through `ESearchCase::CaseSensitive`
// against the module's own bytes -- see this file's header block. The default UE
// comparison would pass on `INFANTRY`, which is not the id the table carries and is not
// the id `SubmitBuildAtHex` would be logged against.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuMirrorsBridgeTest,
	"Stratocracy.StratUI.GATE-BUILDMENU.MenuMirrorsTheBridgeRowForRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuMirrorsBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuRouting;

	FStratBridge Bridge;
	TArray<FCase> Cases;
	FString Error;
	// On its own line: the call WRITES Error, and a call that both fills the string and
	// formats it has unspecified argument evaluation order.
	const bool bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> FromBridge;
		FString BridgeError;
		if (!TestTrue(*FString::Printf(TEXT("%s: the bridge answers"), *Ask.What),
				AskTheBridge(Bridge, Ask, FromBridge, BridgeError)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *BridgeError));
			continue;
		}

		TArray<FStratBuildOptionView> Menu;
		FString Refusal;
		if (!TestTrue(*FString::Printf(TEXT("%s: the menu builds"), *Ask.What),
				StratBuildProductionMenu(Bridge, Ask.Side, Ask.Hex, Menu, Refusal)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *Refusal));
			continue;
		}

		TestTrue(*FString::Printf(TEXT("%s: a successful build clears the caller's failure string"), *Ask.What),
			Refusal.IsEmpty());

		if (!TestEqual(*FString::Printf(TEXT("%s: every row carried across, none added or dropped"), *Ask.What),
			Menu.Num(), static_cast<int32>(FromBridge.size())))
		{
			continue;
		}

		for (int32 i = 0; i < Menu.Num(); ++i)
		{
			const FStratBuildOptionView& View = Menu[i];
			const strat::UiBuildOption&  Source = FromBridge[static_cast<size_t>(i)];
			const FString Slot = FString::Printf(TEXT("%s: row %d"), *Ask.What, i);

			TestTrue(*FString::Printf(
					TEXT("%s: the module names a real §2.4 row, so a copied DefIndex is distinguishable from the INDEX_NONE default"),
					*Slot),
				Source.defIndex >= 0);
			TestEqual(*FString::Printf(TEXT("%s: DefIndex is the module's, and in the module's order"), *Slot),
				View.DefIndex, static_cast<int32>(Source.defIndex));

			// CASE-SENSITIVE. FName's own comparison is not, and neither is TestEqual on
			// the FStrings behind them.
			const FString ViewId   = View.Id.ToString();
			const FString SourceId = FromStd(Source.id);
			TestTrue(*FString::Printf(TEXT("%s: Id is the module's id byte for byte ('%s' vs '%s')"),
					*Slot, *ViewId, *SourceId),
				ViewId.Equals(SourceId, ESearchCase::CaseSensitive));

			TestEqual(*FString::Printf(TEXT("%s: CostFame is the §2.4 price, not re-priced here"), *Slot),
				View.CostFame, static_cast<int32>(Source.costFame));
			TestEqual(*FString::Printf(TEXT("%s: bAvailable is the module's verdict"), *Slot),
				View.bAvailable, Source.available);

			// Also CASE-SENSITIVE, and compared whole: nothing may append a price, a hint
			// or a unit name to the sentence the module wrote.
			const FString ViewReason   = View.Reason.ToString();
			const FString SourceReason = FromStd(Source.reason);
			TestTrue(*FString::Printf(TEXT("%s: Reason is the module's sentence, whole and byte for byte ('%s' vs '%s')"),
					*Slot, *ViewReason, *SourceReason),
				ViewReason.Equals(SourceReason, ESearchCase::CaseSensitive));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2 -- `bAffordable` survives the trip UNCOMPUTED.
//
// WHY THIS IS ITS OWN CLAUSE rather than one line of the one above. T-UI-03's defining
// rule is that no surface does arithmetic over rules values, and `costFame <= fameTotal`
// inside a binding is exactly that arithmetic. `UiBuildOption::affordable` exists so
// the menu need not do it; `FStratBuildOptionView::bAffordable` exists so the widget
// need not either. A second spelling of that comparison one module closer to the screen
// is the defect this clause is named after, and it is a defect even on the day it
// agrees.
//
// THE MUTANT IT IS BUILT TO CATCH is the fold the field's own comment warns about:
// `bAffordable = Source.affordable && Source.available`, which is invisible on a factory
// that can build and wrong on every factory that cannot. That is why the assertion runs
// across the WHOLE enumerated case list including the unavailable ones, and why the
// clause refuses to pass without having seen an unavailable-but-affordable row -- see
// the coverage assertion at the end.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuAffordabilityCopiedTest,
	"Stratocracy.StratUI.GATE-BUILDMENU.AffordabilityIsCopiedNotRecomputed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuAffordabilityCopiedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuRouting;

	FStratBridge Bridge;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 UnavailableButAffordableRows = 0;

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> FromBridge;
		FString BridgeError;
		if (!TestTrue(*FString::Printf(TEXT("%s: the bridge answers"), *Ask.What),
				AskTheBridge(Bridge, Ask, FromBridge, BridgeError)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *BridgeError));
			continue;
		}

		TArray<FStratBuildOptionView> Menu;
		FString Refusal;
		if (!TestTrue(*FString::Printf(TEXT("%s: the menu builds"), *Ask.What),
				StratBuildProductionMenu(Bridge, Ask.Side, Ask.Hex, Menu, Refusal)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *Refusal));
			continue;
		}
		if (!TestEqual(*FString::Printf(TEXT("%s: every row carried across"), *Ask.What),
			Menu.Num(), static_cast<int32>(FromBridge.size())))
		{
			continue;
		}

		for (int32 i = 0; i < Menu.Num(); ++i)
		{
			const FStratBuildOptionView& View = Menu[i];
			const strat::UiBuildOption&  Source = FromBridge[static_cast<size_t>(i)];

			TestEqual(*FString::Printf(
					TEXT("%s: row %d ('%s', %d Fame): bAffordable is the module's comparison, copied"),
					*Ask.What, i, *FromStd(Source.id), static_cast<int32>(Source.costFame)),
				View.bAffordable, Source.affordable);

			if (!Source.available && Source.affordable)
			{
				++UnavailableButAffordableRows;
			}
		}
	}

	// WITHOUT ONE OF THESE THE CLAUSE IS BLIND to the fold it exists to catch: on rows
	// where `available` is true, `affordable && available` equals `affordable` and the
	// mutant is invisible. So the clause states its own coverage requirement rather than
	// hoping the board supplies it, and fails loudly if a scenario edit ever stops
	// producing a factory that cannot build something this side could pay for.
	TestTrue(
		TEXT("this board offers at least one row that is unaffordable to build BECAUSE THE FACTORY WON'T, "
		     "yet affordable to the purse -- the only rows on which folding availability into "
		     "affordability is visible"),
		UnavailableButAffordableRows > 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 3 -- `bAvailable` and `bAffordable` are INDEPENDENT, in both directions.
//
// `FStratBuildOptionView::bAvailable`'s own comment: "A SEPARATE FIELD FROM
// `bAffordable`, AND NOT AN AND OF IT" -- "this factory has already built this turn" and
// "you are still saving for this" are different answers and §2.11.5 shows them
// differently. A menu handed one boolean could not tell the player which it was looking
// at, and a route that quietly correlated them would hand the widget exactly that.
//
// BOTH DIRECTIONS ARE REQUIRED TO EXIST ON THIS BOARD, and both are found rather than
// constructed:
//   - unavailable AND affordable -- an enemy-held or non-active-side factory, where the
//     side can still pay for the cheapest §2.4 row. This is "an unavailable factory
//     still reports what the side could pay for".
//   - available AND NOT affordable -- an own buildable factory carrying a §2.4 row
//     priced above this side's opening purse. This is the other half: a price the purse
//     cannot meet does not make the FACTORY unavailable.
// Neither the purse nor the price is written down here; both are read from the module's
// own answer, and the clause reports which rows supplied each direction so a future
// reader can see what the board happened to offer.
//
// IT IS NOT THE MODULE'S SPLIT UNDER TEST. GATE-BUILDLIST owns that upstream. What is
// under test is that the split SURVIVES the trip into engine types -- a route that
// collapsed the two would be green upstream and wrong on screen.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuFlagsIndependentTest,
	"Stratocracy.StratUI.GATE-BUILDMENU.AvailabilityAndAffordabilityAreIndependent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuFlagsIndependentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuRouting;

	FStratBridge Bridge;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	FString UnavailableYetAffordable;
	FString AvailableYetUnaffordable;

	for (const FCase& Ask : Cases)
	{
		TArray<FStratBuildOptionView> Menu;
		FString Refusal;
		if (!TestTrue(*FString::Printf(TEXT("%s: the menu builds"), *Ask.What),
				StratBuildProductionMenu(Bridge, Ask.Side, Ask.Hex, Menu, Refusal)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *Refusal));
			continue;
		}

		for (const FStratBuildOptionView& View : Menu)
		{
			if (!View.bAvailable && View.bAffordable && UnavailableYetAffordable.IsEmpty())
			{
				UnavailableYetAffordable = FString::Printf(TEXT("%s: '%s' at %d Fame"),
					*Ask.What, *View.Id.ToString(), View.CostFame);
			}
			if (View.bAvailable && !View.bAffordable && AvailableYetUnaffordable.IsEmpty())
			{
				AvailableYetUnaffordable = FString::Printf(TEXT("%s: '%s' at %d Fame"),
					*Ask.What, *View.Id.ToString(), View.CostFame);
			}
		}
	}

	TestFalse(
		TEXT("an unavailable factory still reports what the side could pay for "
		     "(bAvailable false with bAffordable true)"),
		UnavailableYetAffordable.IsEmpty());
	TestFalse(
		TEXT("a price the purse cannot meet does not make the factory unavailable "
		     "(bAvailable true with bAffordable false)"),
		AvailableYetUnaffordable.IsEmpty());

	// Logged rather than merely counted, so the record says which rows carried each
	// direction on the day this ran.
	if (!UnavailableYetAffordable.IsEmpty())
	{
		AddInfo(FString::Printf(TEXT("unavailable yet affordable: %s"), *UnavailableYetAffordable));
	}
	if (!AvailableYetUnaffordable.IsEmpty())
	{
		AddInfo(FString::Printf(TEXT("available yet unaffordable: %s"), *AvailableYetUnaffordable));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 4 -- an ANSWERED "no" is not a refused call.
//
// The view-model half of the two-channel rule. StratBridge.h names four states that are
// ANSWERS and not refusals -- a factory this side does not hold, one that has already
// taken its build, one already holding a waiting build, and a hex that is not a build
// point -- and `StratBuildProductionMenu`'s boolean return is the refusal channel one
// layer out. A function that returned `false` on any of them would blank a menu §2.11.5
// requires to be SHOWN, with the module's reason beside each row.
//
// So: every enumerated question returns `true` with a full menu, and at least one of
// them comes back true carrying unavailable rows -- an answered no. The coverage half is
// asserted rather than assumed, or the clause would be satisfied by a board on which
// every question happened to answer yes and would then be silent about the whole
// distinction.
//
// THE FULL MENU IS PART OF IT. The row count is compared against the bridge's own,
// which is the module's; a route that returned true but dropped the unavailable rows
// would satisfy a boolean-only check and leave the player unable to see a price he
// cannot act on THIS turn but can next.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuAnsweredNoIsNotARefusalTest,
	"Stratocracy.StratUI.GATE-BUILDMENU.AnAnsweredNoIsNotARefusal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuAnsweredNoIsNotARefusalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuRouting;

	FStratBridge Bridge;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 CasesAnsweringNo = 0;

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> FromBridge;
		FString BridgeError;
		if (!TestTrue(*FString::Printf(TEXT("%s: the bridge answers"), *Ask.What),
				AskTheBridge(Bridge, Ask, FromBridge, BridgeError)))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *BridgeError));
			continue;
		}

		TArray<FStratBuildOptionView> Menu;
		FString Refusal;
		const bool bBuilt = StratBuildProductionMenu(Bridge, Ask.Side, Ask.Hex, Menu, Refusal);

		if (!TestTrue(*FString::Printf(
				TEXT("%s: this is a question the rules answered, so the menu is BUILT and not refused"),
				*Ask.What), bBuilt))
		{
			AddError(FString::Printf(TEXT("%s: refused with '%s', which belongs on the row's own reason"),
				*Ask.What, *Refusal));
			continue;
		}

		TestEqual(*FString::Printf(TEXT("%s: the whole menu is shown, unavailable rows included"), *Ask.What),
			Menu.Num(), static_cast<int32>(FromBridge.size()));

		bool bAnyUnavailable = false;
		for (const FStratBuildOptionView& View : Menu)
		{
			if (!View.bAvailable)
			{
				bAnyUnavailable = true;
				TestFalse(*FString::Printf(TEXT("%s: an unavailable row carries the module's reason for a player to read"),
						*Ask.What),
					View.Reason.IsEmpty());
			}
			else
			{
				TestTrue(*FString::Printf(TEXT("%s: an available row carries no reason"), *Ask.What),
					View.Reason.IsEmpty());
			}
		}
		if (bAnyUnavailable)
		{
			++CasesAnsweringNo;
		}
	}

	TestTrue(
		TEXT("at least one question on this board is answered NO through a SUCCESSFUL build -- "
		     "without one, this clause could not tell a correct route from one that refuses every no"),
		CasesAnsweringNo > 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 5 -- ALL-OR-NOTHING. A refusal leaves the caller's array exactly as it found it.
//
// `StratBuildProductionMenu`'s own header states it: it fills a local and moves it
// across on the last line, so a transient failure cannot blank a menu that was correct a
// moment ago. On this surface that has teeth -- §2.11.5's menu is open while the player
// reads it, and a refresh that refused mid-read must not empty the panel under him.
//
// THE SENTINEL IS DISTINGUISHABLE FROM THE DEFAULT, deliberately. `FStratBuildOptionView::DefIndex`
// defaults to `INDEX_NONE`, so a test that planted a default-constructed row could not
// tell "left untouched" from "overwritten with fresh defaults". The planted row carries
// a `DefIndex` no successful build could produce and an `Id` no §2.4 row carries, and
// both are checked after the refusal.
//
// THE REFUSAL USED IS THE MALFORMED SIDE, which is the one refusal channel this pair
// has: the bridge refuses a side outside the match, and this function forwards that
// refusal in the bridge's own words. Not-seeded is exercised too, since that is the
// state a menu opened during startup would actually meet.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuRefusalLeavesArrayIntactTest,
	"Stratocracy.StratUI.GATE-BUILDMENU.ARefusalLeavesTheCallersMenuIntact",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuRefusalLeavesArrayIntactTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuRouting;

	// A row no successful build could produce: `DefIndex` is a real §2.4 row index on
	// every answer, and no §2.4 row is named `NotAUnit`.
	FStratBuildOptionView Sentinel;
	Sentinel.DefIndex    = 4242;
	Sentinel.Id          = FName(TEXT("NotAUnit"));
	Sentinel.CostFame    = 99999;
	Sentinel.bAffordable = true;
	Sentinel.bAvailable  = true;
	Sentinel.Reason      = FText::FromString(TEXT("the caller's previous menu"));

	const auto CheckIntact = [this, &Sentinel](const TArray<FStratBuildOptionView>& Menu, const TCHAR* Which)
	{
		if (!TestEqual(*FString::Printf(TEXT("%s: the caller's array still holds exactly what it held"), Which),
			Menu.Num(), 1))
		{
			return;
		}
		TestEqual(*FString::Printf(TEXT("%s: the planted DefIndex is untouched"), Which),
			Menu[0].DefIndex, Sentinel.DefIndex);
		TestTrue(*FString::Printf(TEXT("%s: the planted Id is untouched"), Which),
			Menu[0].Id == Sentinel.Id);
		TestEqual(*FString::Printf(TEXT("%s: the planted CostFame is untouched"), Which),
			Menu[0].CostFame, Sentinel.CostFame);
	};

	// (a) Never seeded -- the state a menu opened during startup meets.
	{
		FStratBridge Bare;
		TArray<FStratBuildOptionView> Menu;
		Menu.Add(Sentinel);

		FString Refusal;
		const bool bBuilt = StratBuildProductionMenu(Bare, kFirstSide, FIntPoint::ZeroValue, Menu, Refusal);
		TestFalse(TEXT("an unseeded bridge has no menu to offer"), bBuilt);
		TestFalse(TEXT("the refusal carries the bridge's own reason"), Refusal.IsEmpty());
		CheckIntact(Menu, TEXT("unseeded"));
	}

	// (b) Seeded, but the question is malformed -- the bridge's one refusal channel.
	{
		FStratBridge Bridge;
		TArray<FCase> Cases;
		FString Error;
		const bool bReady = Prepare(Bridge, Cases, Error);
		if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
		{
			AddError(Error);
			return false;
		}

		const FIntPoint Factory = Cases[0].Hex;

		TArray<FStratBuildOptionView> Menu;
		Menu.Add(Sentinel);

		FString Refusal;
		const bool bBuilt = StratBuildProductionMenu(Bridge, kNoSuchSide, Factory, Menu, Refusal);
		TestFalse(TEXT("a side outside the match is refused rather than answered"), bBuilt);
		TestFalse(TEXT("that refusal carries the bridge's own reason"), Refusal.IsEmpty());
		CheckIntact(Menu, TEXT("malformed side"));

		// And the same array is FULLY REPLACED on the success that follows -- otherwise
		// "leaves it intact" would be satisfied by a function that never writes at all.
		FString SecondRefusal;
		const bool bSecond = StratBuildProductionMenu(Bridge, kFirstSide, Factory, Menu, SecondRefusal);
		if (TestTrue(TEXT("the same array is written on a question the rules can answer"), bSecond))
		{
			TestTrue(TEXT("a successful build replaces the caller's previous menu rather than appending to it"),
				Menu.Num() > 0 && Menu[0].DefIndex != Sentinel.DefIndex);
			TestTrue(TEXT("the sentinel row is gone from the rebuilt menu"),
				!Menu.ContainsByPredicate([&Sentinel](const FStratBuildOptionView& V)
					{ return V.DefIndex == Sentinel.DefIndex; }));
		}
	}

	return true;
}
