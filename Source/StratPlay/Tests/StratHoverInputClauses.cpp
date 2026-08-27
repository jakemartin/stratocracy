// Wave 0's hover input, pinned. GDD §2.11.3 / §2.11.2, filed under T-UI-01.
//
// WHAT T-UI-01 SAYS, AND WHAT THESE FIVE CLAUSES DO NOT CLAIM. T-UI-01's own GDD sentence is
// about THE FORECAST EQUALLING THE RESOLUTION -- the attack card and the combat that follows
// it agreeing -- and `Source/StratBridge/Tests/StratCombatOutcomeParity.cpp` is where that
// sentence is asserted. NOT ONE CLAUSE BELOW ASSERTS IT. There is no forecast in this wave,
// no `Forecast` call in this file, and nothing here compares a predicted outcome to a
// resolved one. The user's ruling is that wave 0 rides T-UI-01 rather than minting a new
// acceptance id, and this block is the honest statement of what that filing means: these
// clauses pin THE INPUT THE FORECAST PATH REQUIRES AND CANNOT PRODUCE FOR ITSELF -- that a
// hovered hex reaches the view model at all, and WHICH hex reaches it. A forecast card that
// draws for the wrong hex is wrong in exactly the way T-UI-01 exists to catch, and it would
// be wrong before `Forecast` was ever called. That is the whole of the connection, and it is
// a NECESSARY-CONDITION relationship rather than a restatement of the id's claim.
//
// A READER WHO WANTS THE FORECAST'S OWN PARITY SHOULD NOT LOOK HERE. See
// `Stratocracy.StratBridge.T-UI-01.CombatOutcomeAgreesWithForecast` and its neighbours.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question a reader of a test file
// should have to ask:
//   - EVERY HEX IS READ OFF A MODEL `StratBuildViewModel` BUILT from the shipped scenario
//     through a live `FStratBridge`. There is not one hex literal in this file and no hex
//     arithmetic of any kind -- no neighbour, no distance, no addition of two coordinates.
//     `FIntPoint::operator==` is the only spatial operator used, exactly as it is the only
//     one `FStratHoverState` itself uses.
//   - THE "NOT HOVERING" EXPECTATION IS A DEFAULT-CONSTRUCTED `FStratHoverView`, read field
//     for field off the module's own struct, and never a hand-typed `(0,0)` and `false`. The
//     shipped header says the cleared hex "is reset to (0,0)"; asserting a typed copy of that
//     sentence would keep passing if the struct's own default moved. Asking the struct is the
//     module-side value, so the struct is asked.
//   - EVERY BOOLEAN ASSERTED IS A SHIPPED FUNCTION'S OWN RETURN VALUE. This file computes no
//     change flag of its own -- which is the point of the third clause, since re-deriving
//     "did it move" here would be the second author `AStratPlayerController::ApplyHoverChange`
//     declines to become.
//
// NO STRING IS COMPARED ANYWHERE IN THIS FILE, AND THAT IS DELIBERATE RATHER THAN INCIDENTAL.
// `FString::operator==`, `Contains` and `TestEqual` are all `ESearchCase::IgnoreCase` in UE
// 5.8 and this project has already shipped a clause that could not fail because of it. Wave 0
// produces no text at all -- a hovered hex is two integers and a bool -- so the safest
// handling of that hazard here is to have no subject for it. If a later wave's forecast card
// puts a SENTENCE on the model, the clause that pins it must compare with
// `FString::Equals(..., ESearchCase::CaseSensitive)`.
//
// THE ASSERTIONS ARE AGAINST THE LIVE OBJECTS AND NEVER AGAINST A COPY OF THEM. The fifth
// clause drives a spawned `AStratPlayerController` and reads back through that same
// controller's own `GetHoveredHex` and its own `DecorateForPresentation`; it holds no
// `FStratHoverState` of its own and mirrors nothing. Reverting any line of the shipped hover
// path moves what these clauses read, which was proved by reverting it.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED, per
// `StratPlay.Build.cs`. `StratBridge.h` is included from this `.cpp` and from no header.
//
// WHY THIS FILE IS IN `StratPlay` AND NOT IN `StratUI`. It names `FStratHoverState` and
// `AStratPlayerController`, both `STRATPLAY_API`, alongside `FStratViewModel`, which is
// `StratUI`'s. `StratPlay -> StratUI` runs one way only, so `StratPlay` is the sole module
// that can see both sides of these comparisons; the same file in `StratUI` would be
// `LNK2019` rather than a test.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratHoverState.h"
#include "StratPlayerController.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratHoverInputClauses
{
	/** The same harness pin every parity file in this tree carries. `Replay.h` states that no
	 *  rule decides which side moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static FString Describe(FIntPoint Hex)
	{
		return FString::Printf(TEXT("axial(%d,%d)"), Hex.X, Hex.Y);
	}

	/** Seeds a bridge from the shipped tables and the shipped scenario. The two `/Game/` paths
	 *  are the automation-fixture exception the project's own rule 4 names. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Two DISTINCT board hexes, enumerated off a model the module built, at least one of which
	 * differs from the default `FStratHoverView`'s hex.
	 *
	 * ENUMERATED AND NEVER CHOSEN, on `StratProductionMenuAffordance.cpp`'s stated precedent:
	 * a clause that named `(0, 0)` and `(1, 0)` would go quiet the day the scenario's origin
	 * moved, and worse, a clause whose "hovered" hex happened to BE the default's hex could
	 * not tell a decoration apart from a struct that was never written. `OutAway` is therefore
	 * required to differ from the default, and that requirement is ASSERTED by the caller
	 * rather than assumed here.
	 *
	 * @param OutAway   a board hex that is not the default `FStratHoverView`'s hex.
	 * @param OutOther  a board hex that is neither `OutAway` nor the default's hex.
	 */
	static bool TwoDistinctBoardHexes(
		const FStratViewModel& Model, FIntPoint& OutAway, FIntPoint& OutOther, FString& OutError)
	{
		const FIntPoint DefaultHex = FStratHoverView().HoveredHex;

		TArray<FIntPoint> Candidates;
		for (const FStratHexView& Hex : Model.Hexes)
		{
			if (Hex.Hex != DefaultHex)
			{
				Candidates.Add(Hex.Hex);
			}
			if (Candidates.Num() == 2)
			{
				break;
			}
		}

		if (Candidates.Num() < 2)
		{
			OutError = FString::Printf(
				TEXT("the shipped scenario offered %d board hexes distinct from the default ")
				TEXT("hover hex %s; this clause needs two"),
				Candidates.Num(), *Describe(DefaultHex));
			return false;
		}

		OutAway  = Candidates[0];
		OutOther = Candidates[1];
		return true;
	}

	/**
	 * The board hex that IS the default `FStratHoverView`'s hex, if the scenario has one.
	 *
	 * WHY THIS ONE HEX HAS TO BE FOUND RATHER THAN AVOIDED, and it is the opposite of what
	 * `TwoDistinctBoardHexes` wants. `SetHoveredHex`'s change test is
	 * `!bHasHoveredHex || HoveredHex != Hex`, and `ClearHoveredHex` resets the coordinate as
	 * well as the flag -- so after a clear the held coordinate IS the default, and re-entering
	 * any OTHER hex already differs from it. The right-hand disjunct alone answers true for
	 * every such hex. THE ONLY HEX AT WHICH THE LEFT-HAND DISJUNCT IS OBSERVABLE AT ALL is the
	 * one the clear resets to.
	 *
	 * MEASURED, NOT REASONED. The first version of the clause below re-entered an arbitrary
	 * board hex and stayed GREEN over a mutant that deleted `!bHasHoveredHex ||` outright --
	 * a leg that could not fail, in a clause whose whole worth was that leg. The mutation was
	 * re-run against this version and reddens it.
	 */
	static bool BoardHexEqualToTheDefault(
		const FStratViewModel& Model, FIntPoint& OutHex, FString& OutError)
	{
		const FIntPoint DefaultHex = FStratHoverView().HoveredHex;
		for (const FStratHexView& Hex : Model.Hexes)
		{
			if (Hex.Hex == DefaultHex)
			{
				OutHex = DefaultHex;
				return true;
			}
		}

		OutError = FString::Printf(
			TEXT("the shipped scenario has no board hex at %s, which is the only hex at which ")
			TEXT("SetHoveredHex's `!bHasHoveredHex` disjunct is observable -- see ")
			TEXT("BoardHexEqualToTheDefault. If the scenario's origin has genuinely moved off ")
			TEXT("the default, this leg needs a seam rather than a different hex."),
			*Describe(DefaultHex));
		return false;
	}

	/**
	 * A transient world, as every actor-driving clause in this module builds one.
	 *
	 * DUPLICATED RATHER THAN SHARED, matching the ten other files in `Source/StratPlay/Tests/`
	 * that each carry their own. Lifting it into a shared header is a real improvement and is
	 * a refactor of ten files rather than a thing wave 0 does on the way past.
	 */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&)            = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};
}

// ---------------------------------------------------------------------------
// CLAUSE 1 -- the hovered hex reaches the model, and WHICH hex does.
// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHoveredHexReachesTheViewModelTest,
	"Stratocracy.StratPlay.T-UI-01.HoveredHexReachesTheViewModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `FStratHoverState::SetHoveredHex` then `DecorateViewModel` puts `bHasHoveredHex == true` and
 * THE SAME HEX on `FStratViewModel::Hover`.
 *
 * IT PINS THE IDENTITY AND NOT MERELY THE ARRIVAL. Two distinct board hexes are driven through
 * the same state in sequence and each is asserted to be on the model AND the other asserted
 * not to be. A clause that only checked `bHasHoveredHex` would stay green over a
 * `DecorateViewModel` that wrote a constant hex, which is precisely the defect a forecast card
 * drawing for the wrong tile would be made of.
 *
 * THIS IS NOT T-UI-01's OWN SENTENCE. See the file header: no forecast is computed here and no
 * resolution is compared to one. This is the necessary condition underneath that sentence.
 */
bool FStratHoveredHexReachesTheViewModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHoverInputClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Built;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Built, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint Away;
	FIntPoint Other;
	if (!TestTrue(TEXT("the scenario offers two board hexes to hover"),
			TwoDistinctBoardHexes(Built, Away, Other, Error)))
	{
		AddError(Error);
		return false;
	}
	// The precondition the clause's own strength rests on, asserted rather than assumed.
	if (!TestNotEqual(TEXT("the two enumerated board hexes differ from each other"),
			Away, Other))
	{
		return false;
	}

	FStratHoverState Hover;

	// --- The first hex ------------------------------------------------------
	Hover.SetHoveredHex(Away);

	FStratViewModel First;
	Hover.DecorateViewModel(First);

	TestTrue(
		*FString::Printf(TEXT("after SetHoveredHex(%s), the model reports hovering"),
			*Describe(Away)),
		First.Hover.bHasHoveredHex);
	TestEqual(
		*FString::Printf(TEXT("the model carries the hex that was hovered, %s"),
			*Describe(Away)),
		First.Hover.HoveredHex, Away);
	TestNotEqual(
		*FString::Printf(TEXT("the model does NOT carry the other board hex %s"),
			*Describe(Other)),
		First.Hover.HoveredHex, Other);

	// --- The second hex, through the same state -----------------------------
	// THE SAME OBJECT AND NOT A FRESH ONE, so that a decorator writing a constant, or a state
	// that latched its first hex, is caught here rather than passing on a second instance's
	// first write.
	Hover.SetHoveredHex(Other);

	FStratViewModel Second;
	Hover.DecorateViewModel(Second);

	TestTrue(
		*FString::Printf(TEXT("after SetHoveredHex(%s), the model still reports hovering"),
			*Describe(Other)),
		Second.Hover.bHasHoveredHex);
	TestEqual(
		*FString::Printf(TEXT("the model now carries %s"), *Describe(Other)),
		Second.Hover.HoveredHex, Other);
	TestNotEqual(
		*FString::Printf(TEXT("the model no longer carries the first hex %s"), *Describe(Away)),
		Second.Hover.HoveredHex, Away);

	// The first model is re-read AFTER the second decoration: `DecorateViewModel` is `const`
	// and takes its target by reference, so a model decorated earlier must be untouched by a
	// later decoration of a different one.
	TestEqual(
		TEXT("the earlier model is unchanged by the later decoration"),
		First.Hover.HoveredHex, Away);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 2 -- a cleared hover leaves no stale hex behind.
// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratClearedHoverLeavesNoStaleHexTest,
	"Stratocracy.StratPlay.T-UI-01.ClearedHoverLeavesNoStaleHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * After `ClearHoveredHex`, a decorated model's `Hover` equals a DEFAULT-CONSTRUCTED
 * `FStratHoverView` field for field -- flag false and hex back to the default.
 *
 * WHY BOTH FIELDS AND NOT JUST THE FLAG. `FStratViewModel` is documented as a complete
 * statement and never a delta, so a cleared hover must be a complete statement too: a stale
 * coordinate surviving behind a false flag is readable by any consumer that forgets to check
 * the flag, and `FStratHoverView`'s own block says the default hex is where it must land so
 * that such a consumer draws something obviously wrong at a corner rather than something
 * plausible in the middle.
 *
 * THE EXPECTATION IS THE MODULE'S OWN DEFAULT AND NOT A TYPED `(0,0)`. `FStratHoverView()` is
 * asked for it. A literal here would keep passing if the struct's default ever moved, which is
 * the shape of test that tests itself.
 *
 * THE POSITIVE CONTROL COMES FIRST. The same state is shown carrying a real board hex onto a
 * model BEFORE the clear, so the not-hovering reading afterwards is a change this clause
 * caused rather than a struct that was never written -- an absence needs a control.
 *
 * WHAT THIS CLAUSE PINS IS A CONJUNCTION, AND THAT WAS MEASURED RATHER THAN REASONED. The
 * shipped code defends "no stale hex" in THREE places -- `ClearHoveredHex` resets the
 * coordinate, `DecorateViewModel` masks it behind the flag, and `GetHoveredHex` masks it
 * again -- and any two of them cover for the third. Deleting the reset in `ClearHoveredHex`
 * ALONE was run against this clause and left it GREEN, together with the other four; only
 * deleting the reset AND both masks reddens it. So a reader must not take a green here as
 * evidence that the reset line is live. It is evidence that no stale coordinate is READABLE
 * through either public reader, which is the property a consumer actually depends on and the
 * strongest one reachable without a seam into the private field.
 */
bool FStratClearedHoverLeavesNoStaleHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHoverInputClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Built;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Built, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint Away;
	FIntPoint Other;
	if (!TestTrue(TEXT("the scenario offers a board hex distinct from the default hover hex"),
			TwoDistinctBoardHexes(Built, Away, Other, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE MODULE-SIDE EXPECTATION, asked for rather than typed.
	const FStratHoverView Default;

	if (!TestNotEqual(
			TEXT("the hex about to be hovered differs from the default, so a stale copy of it "
			     "would be visible"),
			Away, Default.HoveredHex))
	{
		return false;
	}

	FStratHoverState Hover;

	// --- POSITIVE CONTROL: the state is shown able to speak ------------------
	Hover.SetHoveredHex(Away);

	FStratViewModel Hovering;
	Hover.DecorateViewModel(Hovering);
	if (!TestTrue(TEXT("control: the state puts a hover on a model before the clear"),
			Hovering.Hover.bHasHoveredHex))
	{
		return false;
	}
	if (!TestEqual(TEXT("control: and it is the board hex that was set"),
			Hovering.Hover.HoveredHex, Away))
	{
		return false;
	}

	// --- The clear ----------------------------------------------------------
	Hover.ClearHoveredHex();

	FStratViewModel Cleared;
	Hover.DecorateViewModel(Cleared);

	TestFalse(TEXT("after ClearHoveredHex, the decorated model reports no hover"),
		Cleared.Hover.bHasHoveredHex);
	TestEqual(
		*FString::Printf(
			TEXT("and the hex is back to the default %s rather than the stale %s"),
			*Describe(Default.HoveredHex), *Describe(Away)),
		Cleared.Hover.HoveredHex, Default.HoveredHex);

	// THE SAME STATEMENT MADE THROUGH THE READ ACCESSOR, because `GetHoveredHex`'s own
	// declaration promises the out-parameter is zeroed on the false path so that it "cannot be
	// told apart from `HexUnderCursor`" by a caller that ignores the return value. A model
	// assertion alone would leave that promise unpinned.
	FIntPoint ReadBack = Away;
	const bool bStillHovering = Hover.GetHoveredHex(ReadBack);
	TestFalse(TEXT("GetHoveredHex reports no hover after the clear"), bStillHovering);
	TestEqual(
		TEXT("GetHoveredHex zeroes its out-parameter on the false path rather than leaving the "
		     "caller's stale value standing"),
		ReadBack, Default.HoveredHex);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 3 -- a hover change is reported only when the hover actually moves.
// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHoverChangeIsReportedOnlyWhenItMovesTest,
	"Stratocracy.StratPlay.T-UI-01.HoverChangeIsReportedOnlyWhenItMoves",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * The setters' `bool` return is the de-duplication the whole refresh policy rests on: true on
 * a hex that is new, false on the hex already held, true again on a clear that had something
 * to clear, false on a clear that did not, and TRUE on the same hex re-entered after a clear.
 *
 * THE LAST OF THOSE IS THE ONE WITH A NAMED DEFECT BEHIND IT. `SetHoveredHex`'s change test is
 * `!bHasHoveredHex || HoveredHex != Hex`, and the shipped comment says why the first disjunct
 * is there: a cursor dragged across a unit's own mesh -- over which `HexUnderCursor` answers
 * false -- and back onto the tile underneath it would otherwise report "no change" and leave
 * the card dark until the player moved to a different tile. A clause that only tested
 * new-hex-versus-same-hex would stay green over a change test that compared the coordinate
 * alone. The clear-then-same-hex leg is therefore not a garnish; it is the reason this clause
 * is worth more than the two above it.
 *
 * EVERY BOOLEAN ASSERTED IS THE SHIPPED FUNCTION'S OWN RETURN VALUE. This file computes no
 * change flag, which is the same refusal `ApplyHoverChange` makes for the same reason.
 */
bool FStratHoverChangeIsReportedOnlyWhenItMovesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHoverInputClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Built;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Built, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint Away;
	FIntPoint Other;
	if (!TestTrue(TEXT("the scenario offers two board hexes to hover"),
			TwoDistinctBoardHexes(Built, Away, Other, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratHoverState Hover;

	// --- A first hex is news ------------------------------------------------
	TestTrue(
		*FString::Printf(TEXT("the first hover onto %s is reported as a change"),
			*Describe(Away)),
		Hover.SetHoveredHex(Away));

	// --- The same hex again is not ------------------------------------------
	TestFalse(
		*FString::Printf(TEXT("hovering %s again is reported as no change"), *Describe(Away)),
		Hover.SetHoveredHex(Away));

	// --- A different hex is ------------------------------------------------
	TestTrue(
		*FString::Printf(TEXT("moving to %s is reported as a change"), *Describe(Other)),
		Hover.SetHoveredHex(Other));
	TestFalse(
		*FString::Printf(TEXT("and %s again is not"), *Describe(Other)),
		Hover.SetHoveredHex(Other));

	// --- A clear with something to clear is news ----------------------------
	TestTrue(TEXT("clearing a live hover is reported as a change"), Hover.ClearHoveredHex());
	TestFalse(TEXT("clearing an already-clear hover is not"), Hover.ClearHoveredHex());

	// --- THE LEG WITH THE DEFECT BEHIND IT ----------------------------------
	// The cursor left the board and came back onto the very hex it left. The coordinate the
	// state now holds is unchanged by the re-entry; the HOVER changed, and the flag is the
	// only witness to that.
	//
	// IT MUST BE THE HEX THE CLEAR RESETS TO, and `BoardHexEqualToTheDefault` says why at
	// length: at any other hex the coordinate comparison alone answers true and the leg
	// cannot fail. An earlier version of this clause used an arbitrary board hex here and was
	// measured green over a mutant that deleted the disjunct.
	FIntPoint Origin;
	if (!TestTrue(TEXT("the scenario has a board hex at the default hover hex"),
			BoardHexEqualToTheDefault(Built, Origin, Error)))
	{
		AddError(Error);
		return false;
	}

	// The state is walked back to that hex and then cleared, so the coordinate it holds going
	// into the re-entry is the same one the re-entry passes in.
	TestTrue(*FString::Printf(TEXT("hovering %s is a change"), *Describe(Origin)),
		Hover.SetHoveredHex(Origin));
	TestTrue(TEXT("and clearing it is a change"), Hover.ClearHoveredHex());

	TestTrue(
		*FString::Printf(
			TEXT("re-entering %s after a clear is reported as a change, even though the ")
			TEXT("coordinate the state holds did not move"),
			*Describe(Origin)),
		Hover.SetHoveredHex(Origin));

	// And the state that leg left behind is the hovering one, so the leg above cannot be
	// passing on a state that failed to record anything.
	FStratViewModel Reentered;
	Hover.DecorateViewModel(Reentered);
	TestTrue(TEXT("the re-entered hover is on the model"), Reentered.Hover.bHasHoveredHex);
	TestEqual(TEXT("and it is the hex that was re-entered"),
		Reentered.Hover.HoveredHex, Origin);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 4 -- an undecorated model is not hovering.
// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAnUndecoratedModelIsNotHoveringTest,
	"Stratocracy.StratPlay.T-UI-01.AnUndecoratedModelIsNotHovering",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `StratBuildViewModel` alone leaves `FStratViewModel::Hover` at its default.
 *
 * WHY THIS IS A PROPERTY AND NOT A TAUTOLOGY. `FStratViewModel::Hover`'s own block calls the
 * default load-bearing rather than incidental: every model built for a hand-over, a gate, an
 * AI turn or a reconcile that no controller started says "not hovering", which is the truth
 * for all of them. If the builder ever grew an opinion about the cursor -- and it has a
 * bridge, so it could -- there would be two authors of the hovered hex and they would disagree
 * on the first frame a hand-over rebuilt a model behind a live cursor.
 *
 * THE ABSENCE HAS A CONTROL, in this project's standing sense: the SAME built model is then
 * decorated by a live `FStratHoverState` and shown to change. Without that leg, a
 * `StratBuildViewModel` that had silently begun refusing -- or a `Hover` field that had been
 * deleted and replaced by something the compiler defaulted the same way -- would read exactly
 * like a clean pass.
 */
bool FStratAnUndecoratedModelIsNotHoveringTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHoverInputClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// The model really was built, so "not hovering" below is a statement about a populated
	// model and not about a struct nobody filled in.
	if (!TestTrue(TEXT("the built model carries board hexes"), Model.Hexes.Num() > 0))
	{
		return false;
	}

	const FStratHoverView Default;

	TestFalse(TEXT("a model built by StratBuildViewModel alone is not hovering"),
		Model.Hover.bHasHoveredHex);
	TestEqual(TEXT("and its hovered hex is the struct's own default"),
		Model.Hover.HoveredHex, Default.HoveredHex);

	// --- THE CONTROL --------------------------------------------------------
	FIntPoint Away;
	FIntPoint Other;
	if (!TestTrue(TEXT("the scenario offers a board hex distinct from the default hover hex"),
			TwoDistinctBoardHexes(Model, Away, Other, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratHoverState Hover;
	Hover.SetHoveredHex(Away);
	Hover.DecorateViewModel(Model);

	TestTrue(
		TEXT("control: the same model becomes hovering once a hover state decorates it, so "
		     "the reading above was a real absence and not an unreachable field"),
		Model.Hover.bHasHoveredHex);
	TestEqual(TEXT("control: and it carries the hex the state was given"),
		Model.Hover.HoveredHex, Away);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 5 -- the controller's own hover route reaches the model.
// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratControllerHoverRouteReachesTheModelTest,
	"Stratocracy.StratPlay.T-UI-01.ControllerHoverRouteReachesTheModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * A LIVE `AStratPlayerController`: `SetHoveredHex` is read back through that controller's own
 * `GetHoveredHex`, and that controller's own `DecorateForPresentation` carries the hex onto a
 * model. `ClearHoveredHex` takes it off again.
 *
 * WHY THE CONTROLLER LEG EXISTS AT ALL WHEN CLAUSE 1 ALREADY PINS THE STATE. The four clauses
 * above are about `FStratHoverState`, which is a plain struct nothing on screen consults. The
 * ROUTE is the part that can be wired wrong while every one of them stays green: a
 * `DecorateForPresentation` that never called `Hover.DecorateViewModel`, or a public setter
 * that wrote a different member, would leave the struct perfect and the screen blind. This
 * project has recorded a correct, clause-pinned, reflected verb that no player could reach;
 * the lesson was to check the ROUTE and not the mechanism, and this clause is that check.
 *
 * IT ASSERTS AGAINST THE LIVE ACTOR AND HOLDS NO COPY. It constructs no `FStratHoverState` of
 * its own, and every value it reads comes back out of the spawned controller. Reverting the
 * `Hover.DecorateViewModel(Model)` line in `AStratPlayerController::DecorateForPresentation`
 * moves what this clause reads, which was proved by reverting it.
 *
 * `BeginPlay` IS NOT DISPATCHED, on `StratProductionMenuAffordance.cpp`'s stated precedent, so
 * `SetupInputComponent` never runs and `HoverAction`'s BINDING is deliberately out of scope
 * here -- see "WHAT THIS CLAUSE DOES NOT PIN" below. `UpdateHoverFromCursor` is likewise out
 * of scope: it calls `HexUnderCursor`, which calls `APlayerController::GetHitResultUnderCursor`
 * and needs a viewport, a local player and a cursor position no automation test has. The
 * shipped `SetHoveredHex` exists as the headless seam for exactly that reason and its own
 * declaration says so; this clause takes it at its word rather than reshaping anything.
 *
 * WHAT THIS CLAUSE DOES NOT PIN, stated rather than left to be discovered:
 *   - THAT `HoverAction` IS BOUND, or that it is bound to `ETriggerEvent::Triggered`. That
 *     needs `SetupInputComponent` over a `UEnhancedInputComponent`, and the asset itself is
 *     null in C++ by this project's rule 4 and is authored on a Blueprint in a separate
 *     editor batch. Until that batch lands there is no asset for a clause to name.
 *   - THAT A MOUSE MOVE PRODUCES A HOVER. Nothing in this suite drives `UPlayerInput`.
 *   - THAT `ApplyHoverChange` REFRESHES. `RefreshFromMachine` refuses in this fixture -- no
 *     match is live -- and the shipped code logs that at `Log` and returns true anyway,
 *     deliberately. Pinning the refresh needs a live match and belongs with the wave that has
 *     a reader for the refreshed field.
 */
bool FStratControllerHoverRouteReachesTheModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHoverInputClauses;

	// The hexes come from the scenario, through a bridge this clause owns and the controller
	// never sees. They are DATA and not a fixture opinion; see `TwoDistinctBoardHexes`.
	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Built;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Built, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint Away;
	FIntPoint Other;
	if (!TestTrue(TEXT("the scenario offers two board hexes to hover"),
			TwoDistinctBoardHexes(Built, Away, Other, Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratHoverView Default;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratPlayerController* const Controller =
		Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("AStratPlayerController spawned"), Controller))
	{
		return false;
	}

	// --- A fresh controller is not hovering ---------------------------------
	FIntPoint FreshHex = Away;
	TestFalse(TEXT("a freshly spawned controller reports no hovered hex"),
		Controller->GetHoveredHex(FreshHex));
	TestEqual(TEXT("and it zeroes the out-parameter rather than leaving the caller's value"),
		FreshHex, Default.HoveredHex);

	// --- The set, through the controller's own public seam -------------------
	TestTrue(
		*FString::Printf(TEXT("the controller reports the first hover onto %s as a change"),
			*Describe(Away)),
		Controller->SetHoveredHex(Away));

	FIntPoint ReadBack = Other;
	TestTrue(TEXT("the controller now reports a hovered hex"),
		Controller->GetHoveredHex(ReadBack));
	TestEqual(
		*FString::Printf(TEXT("and it is %s, read back off the controller itself"),
			*Describe(Away)),
		ReadBack, Away);

	// --- The route to the model, through the controller's own decorator ------
	FStratViewModel Decorated;
	Controller->DecorateForPresentation(Decorated);

	TestTrue(TEXT("DecorateForPresentation puts the controller's hover on the model"),
		Decorated.Hover.bHasHoveredHex);
	TestEqual(
		*FString::Printf(TEXT("and the model carries %s and not the other board hex %s"),
			*Describe(Away), *Describe(Other)),
		Decorated.Hover.HoveredHex, Away);

	// --- Moving the controller's hover moves the model's --------------------
	// The identity leg, at the controller's level: a route that wrote a constant would pass
	// everything above.
	TestTrue(
		*FString::Printf(TEXT("moving the controller's hover to %s is a change"),
			*Describe(Other)),
		Controller->SetHoveredHex(Other));

	FStratViewModel Moved;
	Controller->DecorateForPresentation(Moved);
	TestEqual(
		*FString::Printf(TEXT("the next decorated model carries %s"), *Describe(Other)),
		Moved.Hover.HoveredHex, Other);

	// --- The clear, through the controller ----------------------------------
	TestTrue(TEXT("the controller reports the clear as a change"),
		Controller->ClearHoveredHex());
	TestFalse(TEXT("and a second clear as no change"), Controller->ClearHoveredHex());

	FStratViewModel Cleared;
	Controller->DecorateForPresentation(Cleared);

	TestFalse(TEXT("a model decorated after the clear reports no hover"),
		Cleared.Hover.bHasHoveredHex);
	TestEqual(TEXT("and its hex is the struct's own default, not the stale one"),
		Cleared.Hover.HoveredHex, Default.HoveredHex);

	return true;
}
