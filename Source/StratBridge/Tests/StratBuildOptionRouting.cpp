// The editor pass for §2.11.5's production-menu ROUTING at the bridge layer --
// GATE-BUILDMENU. GDD §2.11.5, §2.4, §2.7, §4.7 Stub 8, §4.9 part 2.
//
// WHAT THIS FILE'S SUBJECT IS, AND WHAT IT IS NOT. The rules module's own buildlist
// behaviour is gated upstream, in the crew repo's `test_ui.cpp` under GATE-BUILDLIST:
// all four rows returned, affordability split from availability, the T-TURN-10 and
// `buildWaiting` gates, Q31's boxed-in factory staying available, and availability
// never varying by row. None of that is re-asserted here and none of it should be.
// The subject of THIS file is the TRANSPORT: that `FStratBridge::BuildOptions` carries
// `strat::uiBuildOptions`'s answer across the module boundary without losing, dropping,
// reshaping, reordering or recomputing any part of it -- and that it puts the two
// channels where the header says it puts them.
//
// WHY THIS FILE IS IN StratBridge AND COULD NOT BE ANYWHERE ELSE. Every parity clause
// below compares an `FStratBridge` answer against the `strat::` answer to the same
// question, so the file must be able to CALL both. The vendored rules sources carry no
// `_API` macro (§4.9 forbids them engine headers), so `UnrealEditor-StratBridge.dll`
// exports `FStratBridge` and nothing beside it -- the 8 x LNK2019 StratBridge.h opens
// with. This module compiles the rules sources into itself via `Vendored/*.strat.cpp`,
// so a test .cpp here links both sides. The same file written in StratUI is not a
// weaker test, it is a link error. The view-model half of the route, which does NOT
// need `strat::` to be callable, lives in Source/StratUI/Tests/StratProductionMenuRouting.cpp.
//
// WHERE EVERY EXPECTATION COMES FROM. The right-hand side of every comparison is
// `strat::uiBuildOptions` over the bridge's own `MakeUiWorld()`, and the case list is
// built from `MakeUiSnapshot`'s own factory enumeration. This file writes down no
// price, no unit id, no factory coordinate and no refusal sentence. The one literal it
// contains is `kNoSuchSide`, and that is an argument rather than an expectation.
//
// TWO THINGS THIS FILE DELIBERATELY DOES NOT ASSERT, each because the assertion could
// not fail:
//  - That `isFlag` never appears in this query's output. `isFlag` is a Scenario.h
//    PLACEMENT field, the buildlist has no such field, and `data/units.csv` carries no
//    flag row -- so the assertion passes on an empty implementation and on a wrong one
//    alike. It is evidence of nothing.
//  - Any equality against a refusal SENTENCE spelled out here. `FString`'s `==`,
//    `Contains` and `TestEqual` are all CASE-INSENSITIVE in UE, so a transcribed
//    sentence cannot fail on the casing defect it looks like it is guarding. Reason
//    strings are checked by CASE-SENSITIVE parity against the module's own
//    `std::string` in the view-model file; here they are checked only for the
//    structural property the module states (present when unavailable, empty when
//    available), which is a property no transcription is needed to express.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths. Named
// explicitly rather than left to the transitive include through StratBridge.h, because
// this file calls into them directly and a reader should not have to discover that.
#include "Economy.h"
#include "Hex.h"
#include "Ui.h"

namespace StratBuildOptionRouting
{
	// The same harness pin StratBridgeParity.cpp, StratBridgeQueryParity.cpp and
	// StratScoreboardParity.cpp all carry, and it must stay equal to theirs. Replay.h
	// states that no rule in this project decides which side moves first, so this is a
	// harness constant and not a rule.
	static const int32 kFirstSide = 0;

	// A side index no scenario or table edit can bring into existence, on
	// StratBridgeQueryParity's `kNoSuchUnitId` reasoning: a check whose subject must be
	// re-picked every time the board moves is a check that gets quietly relaxed. It is
	// an ARGUMENT, not an expectation -- nothing below compares a value against it.
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

	/** Both axial and odd-r offset, because the scenario file authors the second. */
	static FString Describe(const strat::Hex& H)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(H, Col, Row);
		return FString::Printf(TEXT("axial(%d,%d)=offset[%d,%d]"), H.q, H.r, Col, Row);
	}

	/** Definitions only -- the bridge state between the two refusal reasons. */
	static bool LoadDefsOnly(FStratBridge& Bridge, FString& OutError)
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
		return true;
	}

	/**
	 * The seeded opening: shipped tables mapped in, shipped scenario seeded through the
	 * rules module's own `seedFromScenario`. Returns false and fills OutError rather
	 * than asserting, on StratBridgeParity's precedent.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		if (!LoadDefsOnly(Bridge, OutError))
		{
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

	/** One question the menu can be asked, with the words to name it in a log line. */
	struct FCase
	{
		strat::Hex Hex;
		int32      Side = 0;
		FString    What;
	};

	/**
	 * Every question this board can put to the menu, enumerated from the board rather
	 * than picked.
	 *
	 * WHY EXHAUSTIVE AND NOT A CHOSEN HEX. The interesting routing failures are
	 * per-CASE: a bridge that pre-checks ownership refuses exactly the enemy-held
	 * factory and forwards the rest; one that pre-checks the build point refuses exactly
	 * the plain hex. Asking every (factory, side) pair plus a hex that is not a factory
	 * means no such failure can hide behind a case this file happened not to choose,
	 * and it means a scenario edit that moved the factories moves the case list with it.
	 *
	 * THE NON-FACTORY HEX IS FOUND, NOT WRITTEN DOWN: the first hex in the snapshot's
	 * canonical order that no `UiFactoryView` claims. Whether the module answers it "no
	 * objective at that hex" or "not a build point" -- a neutral town is the second --
	 * is the module's business and nothing here depends on which.
	 */
	static bool BuildCases(const strat::UiSnapshot& Snapshot, TArray<FCase>& OutCases, FString& OutError)
	{
		OutCases.Reset();

		if (Snapshot.factories.empty())
		{
			OutError = TEXT("the seeded scenario carries no factories, so §2.11.5 has nothing to route");
			return false;
		}

		for (const strat::UiFactoryView& Factory : Snapshot.factories)
		{
			for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
			{
				FCase Entry;
				Entry.Hex  = Factory.hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(
					TEXT("side %d at the factory %s (owner %d, builtThisTurn %d, buildWaiting %d, spawnBlocked %d)"),
					Side, *Describe(Factory.hex), static_cast<int32>(Factory.owner),
					Factory.hasBuiltThisTurn ? 1 : 0, Factory.buildWaiting ? 1 : 0,
					Factory.spawnBlocked ? 1 : 0);
				OutCases.Add(Entry);
			}
		}

		for (const strat::UiHexView& HexView : Snapshot.hexes)
		{
			bool bIsFactory = false;
			for (const strat::UiFactoryView& Factory : Snapshot.factories)
			{
				if (strat::hexEqual(Factory.hex, HexView.hex))
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
				Entry.Hex  = HexView.hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(TEXT("side %d at %s, which is not a factory"),
					Side, *Describe(HexView.hex));
				OutCases.Add(Entry);
			}
			break;
		}

		return true;
	}

	/** Seeds, projects, and enumerates the case list. Every clause opens with this. */
	static bool Prepare(FStratBridge& Bridge, strat::UiSnapshot& OutSnapshot,
	                    TArray<FCase>& OutCases, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const FStratResult Projected = Bridge.MakeUiSnapshot(OutSnapshot);
		if (!Projected.bOk)
		{
			OutError = FString::Printf(TEXT("MakeUiSnapshot refused: %s"), *Projected.Reason);
			return false;
		}

		return BuildCases(OutSnapshot, OutCases, OutError);
	}
}

// ---------------------------------------------------------------------------
// Clause 1 -- the bridge's menu IS the module's menu.
//
// Row for row, field for field, IN ORDER, for every question this board can put. The
// right-hand side is `strat::uiBuildOptions` over the bridge's own `MakeUiWorld()`, so
// both calls see one state and any difference is the bridge's doing. Nothing here
// re-prices a unit, re-compares a price against a purse, or transcribes a row out of
// `data/units.csv`.
//
// ORDER IS PART OF THE CLAIM. §2.11.5's menu is one row per §2.4 row in unit-table
// order, and `SubmitBuildAtHex` takes `defIndex` as a RAW index (see
// GATE-BRIDGE-DEFS): a bridge that forwarded the same four offers in a different order
// would draw a correct-looking menu whose buttons built the wrong unit.
//
// `defIndex` IS CHECKED AGAINST A REAL ROW FIRST, and that is not decoration. Both
// `UiBuildOption::defIndex` and `FStratBuildOptionView::DefIndex` default to -1 /
// INDEX_NONE, so a parity check alone cannot tell "copied" from "both still at their
// default". Establishing that the module's value is a real row index is what makes the
// equality below able to distinguish the two.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsMatchModuleTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.OptionsMatchTheModuleQuery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsMatchModuleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	TArray<FCase> Cases;
	FString Error;
	// On its own line and not inside the TestTrue call: it WRITES Error, and a call that
	// both fills the string and formats it has unspecified argument evaluation order.
	const bool bReady = Prepare(Bridge, Snapshot, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded world carries a unit table"), World.unitDefs != nullptr))
	{
		return false;
	}
	const int32 TableRows = static_cast<int32>(World.unitDefs->size());

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> FromBridge;
		const FStratResult Queried = Bridge.BuildOptions(Ask.Side, Ask.Hex, FromBridge);
		if (!TestTrue(*FString::Printf(TEXT("%s: BuildOptions answers"), *Ask.What), Queried.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *Queried.Reason));
			continue;
		}

		// THE EXPECTATION. The module's own answer over the bridge's own world.
		const std::vector<strat::UiBuildOption> FromModule =
			strat::uiBuildOptions(World, Ask.Side, Ask.Hex);

		if (!TestEqual(*FString::Printf(TEXT("%s: every row forwarded, none added or dropped"), *Ask.What),
			static_cast<int32>(FromBridge.size()), static_cast<int32>(FromModule.size())))
		{
			continue;
		}

		for (size_t i = 0; i < FromModule.size(); ++i)
		{
			const strat::UiBuildOption& B = FromBridge[i];
			const strat::UiBuildOption& M = FromModule[i];
			const FString Slot = FString::Printf(TEXT("%s: row %d"), *Ask.What, static_cast<int32>(i));

			TestTrue(*FString::Printf(TEXT("%s: the module names a real §2.4 row, so a copied defIndex is distinguishable from the -1 default"), *Slot),
				M.defIndex >= 0 && M.defIndex < TableRows);

			TestEqual(*FString::Printf(TEXT("%s: defIndex is the module's, and in the module's order"), *Slot),
				B.defIndex, M.defIndex);
			TestTrue(*FString::Printf(TEXT("%s: id is the module's ('%s' vs '%s')"),
					*Slot, *FromStd(B.id), *FromStd(M.id)),
				B.id == M.id);
			TestEqual(*FString::Printf(TEXT("%s: costFame is the module's, not re-priced"), *Slot),
				B.costFame, M.costFame);
			TestEqual(*FString::Printf(TEXT("%s: affordable is the module's comparison, not a second one"), *Slot),
				B.affordable, M.affordable);
			TestEqual(*FString::Printf(TEXT("%s: available is the module's verdict"), *Slot),
				B.available, M.available);
			// std::string ==, which is case-SENSITIVE where FString's is not.
			TestTrue(*FString::Printf(TEXT("%s: reason is the module's own sentence, byte for byte ('%s' vs '%s')"),
					*Slot, *FromStd(B.reason), *FromStd(M.reason)),
				B.reason == M.reason);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2 -- THE TWO CHANNELS. The single most important clause in this file.
//
// `FStratResult` says whether the query could be ASKED. `available` and `reason` on
// each row say what the rules ANSWERED. StratBridge.h states this in as many words and
// names the four cases that are ANSWERS rather than refusals: a factory this side does
// not hold, one that has already taken its build this turn, one already holding a
// waiting build, and a hex that is not a build point.
//
// WHAT MUST GO RED HERE: a bridge that folds an answer onto the refusal channel --
// `if (!Options[0].available) return Fail(...)`, or a pre-check of ownership or of the
// build point ahead of the module call. Every such mutant refuses at least one case in
// the enumerated list, and this clause asserts `bOk` on all of them.
//
// THE COVERAGE HALF IS ASSERTED, NOT ASSUMED. A clause that only said "every case
// returns Ok" would be satisfied by a board on which every case happened to be
// available, and would then be silent about the whole distinction. So it also requires
// that at least one case came back Ok WITH unavailable rows: an answered "no". If a
// scenario edit ever made every question on this board answer yes, this clause says so
// in as many words instead of quietly becoming vacuous.
//
// AND THE INVARIANT THE MODULE STATES ABOUT ITS OWN PAIRING: `reason` is present when
// `available` is false and empty when it is true. Checked as PRESENCE, never against a
// transcribed sentence -- see this file's header block on why a transcription here
// could not fail.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsAnswersRideOkTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.AnswersRideTheOkChannel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsAnswersRideOkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Snapshot, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 CasesWithAnAnsweredNo = 0;

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> Options;
		const FStratResult Queried = Bridge.BuildOptions(Ask.Side, Ask.Hex, Options);

		// THE CLAUSE. None of these questions is malformed, so none of them is refused --
		// whatever the rules answer about them.
		if (!TestTrue(*FString::Printf(
				TEXT("%s: this is a question the rules can answer, so it is answered and not refused"),
				*Ask.What), Queried.bOk))
		{
			AddError(FString::Printf(TEXT("%s: refused with '%s', which belongs on the answer channel"),
				*Ask.What, *Queried.Reason));
			continue;
		}

		bool bAnyUnavailable = false;
		for (size_t i = 0; i < Options.size(); ++i)
		{
			const strat::UiBuildOption& Row = Options[i];
			const FString Slot = FString::Printf(TEXT("%s: row %d"), *Ask.What, static_cast<int32>(i));

			if (Row.available)
			{
				TestTrue(*FString::Printf(TEXT("%s: an available row carries no reason"), *Slot),
					Row.reason.empty());
			}
			else
			{
				bAnyUnavailable = true;
				TestFalse(*FString::Printf(TEXT("%s: an unavailable row carries the module's reason"), *Slot),
					Row.reason.empty());
			}
		}

		if (bAnyUnavailable)
		{
			++CasesWithAnAnsweredNo;
		}
	}

	TestTrue(
		TEXT("at least one question on this board is answered NO on the Ok channel -- without one, "
		     "this clause could not tell a correct bridge from one that refuses every no"),
		CasesWithAnAnsweredNo > 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 3 -- a successful call is one row per §2.4 row, and never empty.
//
// StratBridge.h: `uiBuildOptions` returns the empty vector ONLY when the world carries
// no unit table, so an empty result is never an answer -- the menu has one row per §2.4
// row whatever the board looks like. The count comes from `World.unitDefs->size()`,
// which is the loaded table itself; `data/units.csv`'s four rows are nowhere in this
// file, so a table edit moves both sides of the comparison together.
//
// AND IT HOLDS ON THE UNAVAILABLE CASES TOO, which is the half that matters: a bridge
// that returned a shorter menu when the factory could not build would be deciding, one
// module out, that a player may not READ a price he cannot act on this turn.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsOneRowPerTableRowTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.SuccessIsOneRowPerUnitTableRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsOneRowPerTableRowTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Snapshot, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded world carries a unit table"), World.unitDefs != nullptr))
	{
		return false;
	}
	const int32 TableRows = static_cast<int32>(World.unitDefs->size());
	TestTrue(TEXT("the loaded unit table has rows to offer, or there is no menu to gate"),
		TableRows > 0);

	int32 UnavailableCases = 0;

	for (const FCase& Ask : Cases)
	{
		std::vector<strat::UiBuildOption> Options;
		const FStratResult Queried = Bridge.BuildOptions(Ask.Side, Ask.Hex, Options);
		if (!TestTrue(*FString::Printf(TEXT("%s: BuildOptions answers"), *Ask.What), Queried.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *Ask.What, *Queried.Reason));
			continue;
		}

		TestEqual(*FString::Printf(TEXT("%s: one row per loaded §2.4 row"), *Ask.What),
			static_cast<int32>(Options.size()), TableRows);
		TestFalse(*FString::Printf(TEXT("%s: a successful call is never an empty menu"), *Ask.What),
			Options.empty());

		bool bAnyUnavailable = false;
		for (const strat::UiBuildOption& Row : Options)
		{
			bAnyUnavailable = bAnyUnavailable || !Row.available;
		}
		if (bAnyUnavailable)
		{
			++UnavailableCases;
		}
	}

	// The full-length menu on a factory that cannot build is the half this clause is
	// really about, so it refuses to pass without having seen one.
	TestTrue(TEXT("at least one case on this board answers NO and still returns a full menu"),
		UnavailableCases > 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 4 -- the SIDE is the one malformed-question case.
//
// StratBridge.h: a `Side` outside `strat::SIDE_COUNT` is refused HERE rather than left
// to the module, because `uiBuildOptions` spells "invalid side" the same way it spells
// "not a build point" and those are not the same kind of thing. So `bOk false` means
// the question was malformed and `bOk true` means the rules answered it -- and that
// rule is only crisp if the malformed case really does refuse.
//
// THE OUT-PARAMETER IS CHECKED EMPTY, and that is the structural half: a refusal that
// left the previous factory's menu in the caller's vector would have it priced as this
// one's. It is also what makes "the module's invalid-side reason is unreachable"
// checkable without a string comparison -- see clause 5.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsRefusesBadSideTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.SideOutsideTheMatchIsRefused",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsRefusesBadSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Snapshot, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	// A factory hex the module would happily answer about, so the ONLY thing wrong with
	// the question below is the side.
	const strat::Hex& Factory = Snapshot.factories.front().hex;

	const int32 BadSides[] = { kNoSuchSide, -1 };
	for (const int32 Side : BadSides)
	{
		// Pre-loaded with a real answer, so "cleared" is distinguishable from "never
		// written": a refusal must not leave this sitting in the caller's hands.
		std::vector<strat::UiBuildOption> Options;
		const FStratResult Good = Bridge.BuildOptions(kFirstSide, Factory, Options);
		if (!TestTrue(TEXT("the good question is answered, so the vector holds a real menu first"),
			Good.bOk && !Options.empty()))
		{
			AddError(Good.Reason);
			return false;
		}

		const FStratResult Refused = Bridge.BuildOptions(Side, Factory, Options);
		TestFalse(*FString::Printf(TEXT("side %d is outside the match, so the question is refused"), Side),
			Refused.bOk);
		TestFalse(*FString::Printf(TEXT("the refusal of side %d names a reason"), Side),
			Refused.Reason.IsEmpty());
		TestTrue(*FString::Printf(TEXT("the refusal of side %d leaves no stale menu behind"), Side),
			Options.empty());
	}

	// The in-range sides are NOT refused -- otherwise the clause above would be
	// satisfied by a bridge that refused everything.
	for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
	{
		std::vector<strat::UiBuildOption> Options;
		const FStratResult Queried = Bridge.BuildOptions(Side, Factory, Options);
		TestTrue(*FString::Printf(TEXT("side %d is inside the match, so its question is answered"), Side),
			Queried.bOk);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 5 -- the module's own "invalid side" answer is UNREACHABLE through the bridge.
//
// This is StratBridge.h's stated CONSEQUENCE of clause 4, and it is a distinct claim:
// clause 4 says the bridge refuses a bad side, this one says the module's own answer
// for that side never escapes into a caller's menu. A bridge that refused AND ALSO
// filled the out-parameter would satisfy clause 4's `bOk` half while handing a widget a
// row whose reason sentence is spelled like every other refusal on this screen.
//
// THE MODULE IS SHOWN TO HAVE SUCH AN ANSWER FIRST -- otherwise this clause is
// unfalsifiable padding, green on a module that answered nothing at all. `strat::uiBuildOptions`
// is called directly with the out-of-match side, and the rows it returns are the proof
// that there IS something to be unreachable.
//
// CHECKED STRUCTURALLY AND NOT BY STRING. UE's `FString` `==`, `Contains` and
// `TestEqual` are all CASE-INSENSITIVE, so "the bridge never returns a row saying
// 'invalid side'" written as a string search would be a weaker claim than it reads as.
// "The bridge returns NO ROWS AT ALL" is stronger, needs no transcription, and cannot
// be defeated by casing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsInvalidSideUnreachableTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.TheModulesInvalidSideAnswerIsUnreachable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsInvalidSideUnreachableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	TArray<FCase> Cases;
	FString Error;
	const bool bReady = Prepare(Bridge, Snapshot, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	const strat::Hex& Factory = Snapshot.factories.front().hex;
	const strat::UiWorld World = Bridge.MakeUiWorld();

	// The module DOES answer an out-of-match side, with rows carrying its own reason.
	// Established, not assumed: without it the clause below proves nothing.
	const std::vector<strat::UiBuildOption> ModuleAnswer =
		strat::uiBuildOptions(World, kNoSuchSide, Factory);
	if (!TestTrue(
			TEXT("the module has an out-of-match-side answer for the bridge to be hiding"),
			!ModuleAnswer.empty()))
	{
		return false;
	}
	for (size_t i = 0; i < ModuleAnswer.size(); ++i)
	{
		TestFalse(*FString::Printf(
				TEXT("the module's row %d for an out-of-match side is unavailable with its own reason ('%s')"),
				static_cast<int32>(i), *FromStd(ModuleAnswer[i].reason)),
			ModuleAnswer[i].available || ModuleAnswer[i].reason.empty());
	}

	// And none of it reaches a caller of the bridge.
	std::vector<strat::UiBuildOption> ThroughBridge;
	const FStratResult Refused = Bridge.BuildOptions(kNoSuchSide, Factory, ThroughBridge);
	TestFalse(TEXT("the bridge refuses the malformed question rather than forwarding the module's answer"),
		Refused.bOk);
	TestEqual(TEXT("no row of the module's invalid-side answer reaches the caller"),
		static_cast<int32>(ThroughBridge.size()), 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 6 -- there is no menu before there is a match.
//
// The two states ahead of a seeded bridge are distinguished, on `Reachable`'s and
// `Forecast`'s precedent: definitions not loaded, and loaded-but-unseeded. Both refuse,
// and the two refusals are distinct sentences so a caller can tell which step it
// missed.
//
// AND THE OUT-PARAMETER IS CLEARED IN BOTH, which is the case the header calls out:
// `BuildOptions` clears up front precisely so that a refusal cannot leave the caller
// holding the PREVIOUS factory's menu and pricing it as this one's. A sentinel row is
// planted first so that "cleared" is distinguishable from "never touched" -- an empty
// vector is the default-constructed state, and a bridge that wrote nothing at all would
// pass a bare emptiness check for the wrong reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeBuildOptionsRefusesUnseededTest,
	"Stratocracy.StratBridge.GATE-BUILDMENU.RefusesBeforeThereIsAMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeBuildOptionsRefusesUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildOptionRouting;

	strat::Hex Anywhere;
	Anywhere.q = 0;
	Anywhere.r = 0;

	// A row no successful call could produce -- `defIndex` is a real §2.4 row index on
	// every answer, so this value marks the vector as "the caller's previous contents".
	strat::UiBuildOption Sentinel;
	Sentinel.defIndex = 4242;
	Sentinel.id = "sentinel";

	// (a) Nothing loaded at all.
	{
		FStratBridge Bare;
		std::vector<strat::UiBuildOption> Options;
		Options.push_back(Sentinel);

		const FStratResult Refused = Bare.BuildOptions(kFirstSide, Anywhere, Options);
		TestFalse(TEXT("a bridge with no definitions has no menu to offer"), Refused.bOk);
		TestFalse(TEXT("that refusal names a reason"), Refused.Reason.IsEmpty());
		TestEqual(TEXT("the refusal clears the caller's previous menu"),
			static_cast<int32>(Options.size()), 0);

		// (b) Definitions loaded, no scenario. A DIFFERENT refusal: the caller has to be
		// able to tell "I never mapped the tables" from "I never seeded".
		FString Error;
		const bool bLoaded = LoadDefsOnly(Bare, Error);
		if (!TestTrue(TEXT("definitions load without a scenario"), bLoaded))
		{
			AddError(Error);
			return false;
		}

		std::vector<strat::UiBuildOption> AfterLoad;
		AfterLoad.push_back(Sentinel);
		const FStratResult StillRefused = Bare.BuildOptions(kFirstSide, Anywhere, AfterLoad);
		TestFalse(TEXT("a loaded but unseeded bridge has no menu to offer"), StillRefused.bOk);
		TestEqual(TEXT("that refusal also clears the caller's previous menu"),
			static_cast<int32>(AfterLoad.size()), 0);

		// Distinct sentences. Compared case-INSENSITIVELY on purpose: the claim is that
		// the two steps are TOLD APART, and two sentences differing only in casing would
		// not tell them apart on any screen either.
		TestFalse(TEXT("the unseeded refusal is a different sentence from the unloaded one"),
			StillRefused.Reason.Equals(Refused.Reason, ESearchCase::IgnoreCase));
	}

	return true;
}
