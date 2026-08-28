// The match-setup gate on §2.9's difficulty handicap -- `EStratDifficulty`,
// `StratDifficultyFameDelta`, `StratHandicappedSide`, and the call site inside
// `UStratMatchSubsystem::StartMatchInternal`. GDD §2.9, §2.7, §2.11.6, §2.13, under
// T-FAME-02 and T-SAVE-06.
//
// WHY THESE CLAUSES RIDE EXISTING IDS AND MINT NOTHING. §2.9's handicap has no acceptance ID
// of its own. The user ruling recorded 2026-08-28 in `Tools/architect/state/global.md`'s
// `## NEXT` splits W7 across three EXISTING upstream ids by subject; this file carries two:
//
//   - `T-FAME-02` -- the tier -> delta map, the player-side-only application, and the
//     inertness arms. The ID's own stub NAMES these numbers and closes with an instruction
//     addressed to a gate: "so the gate asserts each side's CONFIGURED VALUE and never a
//     literal 200". Its HEADLINE subject is income accrual, and no clause here asserts any
//     accrual or is written as though that half covered the handicap.
//   - `T-SAVE-06` -- the replayed-state-hash refusal. The last clause asserts that a slot
//     written at one tier is refused at another and carries that id. It asserts NOTHING about
//     the hash algorithm being correct; that is `StratBridgeRestoreParity.cpp`'s.
//
// `T-AI-07` IS NOT AN ID. It appears in the GDD and §4.11's row-6 note records that
// `spec/ai_spec.md` declines to mint it -- the self-play run rides `GATE-AI-SMOKE`. Nothing
// here reaches for it.
//
// WHERE EVERY EXPECTATION COMES FROM, and NO CLAUSE HERE WRITES 200, 350 OR 100.
//   - THE OPENING BASELINE IS MEASURED, PER SIDE, FROM A HOT-SEAT MATCH -- the configuration
//     in which `StratHandicappedSide` returns `INDEX_NONE` and the scenario's own
//     `startingFame` therefore stands. It is read off `FStratViewModel::Sides[i].FameTotal`,
//     which mirrors `strat::UiSideView::fameTotal`. Every handicapped reading below is
//     asserted as `baseline + delta`, per side, and never against a number in this file.
//     `StratBridge`'s `T-FAME-02.HandicapMovesTheScenariosConfiguredFameByTheDelta` is what
//     pins that baseline to `strat::Scenario::startingFame` itself; this module cannot see
//     that struct without naming a `strat::` type, which no test file in `StratPlay` does.
//   - THE TWO ARMS CROSS-CHECK EACH OTHER, AND THAT IS DELIBERATE. If the inertness broke and
//     a hot seat WERE handicapped, the baseline would shift by the same delta the handicapped
//     match shifted by, and the difference this file asserts would collapse to zero. So
//     `HandicapMovesThePlayersOpeningFameAtEveryTier` goes red on exactly the regression
//     `HandicapIsInertWithoutASinglePlayerOpponent` exists to catch, rather than silently
//     absorbing it into its own baseline.
//   - EVERY DELTA IS `StratDifficultyFameDelta(Tier)` -- the module's own mapping -- EXCEPT
//     in the one clause whose subject IS that mapping. There the expectation is a GDD literal
//     and is cited as one: §2.9 gives `Easy = player +150 opening Fame; Normal = even
//     (200/200); Hard = player -100`, and `global.md`'s ruling restates it. There is nothing
//     module-side to read a tier number from -- the enum carries no payload, deliberately
//     (`StratMatchSubsystem.h`: "THREE VALUES AND NO `Custom`") -- so a literal is the only
//     honest expectation, and it comes from the specification rather than from the function.
//   - THE SIDE COUNT IS `FStratViewModel::Sides.Num()`. A literal 2 would be a second author
//     of it, and `strat::SIDE_COUNT` is not nameable here.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW, on the
// standing of every other test file in this module -- LNK2019 outside `StratBridge` and
// `StratRules`, measured 8 x. `FStratBridge::StateHash`, `SubmitEndTurn` and
// `RecordedCommandCount` are methods on the exported facade.
//
// A CONSEQUENCE WORTH KNOWING BEFORE READING ANY ASSERTION. `StartMatch` can return FALSE in a
// fixture with no tile meshes while `IsMatchLive()` returns TRUE -- `StratMatchSubsystem.h`
// splits all-or-nothing on the rules side from not-all-or-nothing on the presentation side.
// So the return value is never how a clause here asks whether a match started; `IsMatchLive()`
// is, and on the load path the bridge's own hash is.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratDifficultyHandicapMatchClauses
{
	static const int32 kFirstSide   = 0;

	/** The seat the screen is drawn for, and -- in a single-player configuration -- the seat
	 *  §2.7's handicap moves. `StratHandicappedSide` reads this field and not `FirstSide`. */
	static const int32 kViewingSide = 0;

	/** The seat handed to the AI in every single-player configuration below. It is NOT
	 *  `kViewingSide`, which is the whole content of `StratHandicappedSide`'s second arm. */
	static const int32 kAiSide = 1;

	/** This file's own slot, never the configured default. `FStratMatchConfig::SaveSlotName`
	 *  is the player's slot and a gate that wrote there would delete a developer's game --
	 *  `StratSaveSlotClauses.cpp` records the reasoning. */
	static const TCHAR* kTestSlotName = TEXT("StratocracyAutomation_DifficultyHandicap");

	/** §2.11 is hot-seat: one logged-in user, and the subsystem hardcodes the index. */
	static const int32 kUserIndex = 0;

	/** Supplied on both the write and the read, and never read from a manifest -- see
	 *  `StratSaveSlotClauses.cpp` on why feeding these the vendored values would pin
	 *  `T-INT-01` by accident and go red on a re-vendor. */
	static const TCHAR* kRulesCommit = TEXT("automation-rules-commit");
	static const TCHAR* kDataHash    = TEXT("automation-data-hash");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchLifecycle.cpp` for the same helper and the reason it is duplicated. */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = kViewingSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.RulesCommit     = kRulesCommit;
		Out.DataHash        = kDataHash;
		Out.SaveSlotName    = kTestSlotName;
		return true;
	}

	/** See `StratSaveSlotClauses.cpp`: the world first and the context second. */
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

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/** Deletes this file's slot on the way in AND on the way out -- `StratSaveSlotClauses.cpp`
	 *  records why both ends. */
	struct FSlotScope
	{
		FSlotScope()  { Clear(); }
		~FSlotScope() { Clear(); }

		static void Clear()
		{
			if (UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex))
			{
				UGameplayStatics::DeleteGameInSlot(kTestSlotName, kUserIndex);
			}
		}

		FSlotScope(const FSlotScope&) = delete;
		FSlotScope& operator=(const FSlotScope&) = delete;
	};

	/** The three tiers, in the order §2.9 names them. Written out because the enum carries no
	 *  iteration surface and `EStratDifficulty` is deliberately payload-free. */
	static TArray<EStratDifficulty> AllTiers()
	{
		return { EStratDifficulty::Easy, EStratDifficulty::Normal, EStratDifficulty::Hard };
	}

	static const TCHAR* TierName(EStratDifficulty Tier)
	{
		switch (Tier)
		{
		case EStratDifficulty::Easy:   return TEXT("Easy");
		case EStratDifficulty::Normal: return TEXT("Normal");
		case EStratDifficulty::Hard:   return TEXT("Hard");
		}
		return TEXT("<unnamed tier>");
	}

	/**
	 * Starts a match and reads every side's opening `FameTotal` off the projected model.
	 *
	 * THE READING IS THE VIEW MODEL'S AND NOT THE BRIDGE'S, because `FStratSideView::FameTotal`
	 * is what a scoreboard binds to and is therefore the value a player would see be wrong.
	 * `StratViewModelParity.cpp` already pins it as a mirror of `strat::UiSideView::fameTotal`.
	 */
	static bool OpeningFame(UStratMatchSubsystem& Subsystem,
	                        const FStratMatchConfig& Config,
	                        TArray<int32>& OutFameTotal,
	                        TArray<int32>& OutFameCombat,
	                        FString& OutError)
	{
		OutFameTotal.Reset();
		OutFameCombat.Reset();

		FString StartReason;
		Subsystem.StartMatch(Config, StartReason);
		if (!Subsystem.IsMatchLive())
		{
			OutError = FString::Printf(TEXT("the match did not go live: %s"), *StartReason);
			return false;
		}

		FStratViewModel Model;
		if (!Subsystem.BuildViewModel(Model, OutError))
		{
			return false;
		}
		for (const FStratSideView& Side : Model.Sides)
		{
			OutFameTotal.Add(Side.FameTotal);
			OutFameCombat.Add(Side.FameCombat);
		}
		return OutFameTotal.Num() > 0;
	}
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the tier -> delta mapping is §2.9's three numbers.
//
// THE EXPECTATION HERE IS A SPECIFICATION LITERAL AND IS THE ONE PLACE IN THIS WAVE THAT IS.
// Everywhere else a module-side value exists to read and is read. Here there is none by
// design: `EStratDifficulty` carries no payload -- `StratMatchSubsystem.h` refuses a `Custom`
// value precisely so there is exactly one place saying what "Hard" means -- so the only thing
// to compare `StratDifficultyFameDelta` against is the document. §2.9: "Easy = player +150
// opening Fame; Normal = even (200/200); Hard = player -100", restated in `global.md`'s
// 2026-08-28 ruling and in `T-FAME-02`'s own stub. A clause comparing the function to itself
// would pin nothing at all.
//
// AND THE THREE MUST BE DISTINCT IN THE RIGHT DIRECTIONS, which is the half a transcription
// error survives: a mapping that returned +150 for both Easy and Hard would satisfy three
// equalities against a mistyped expectation and still be caught here.
//
// NOTE WHAT THIS CLAUSE DOES NOT PIN. It says nothing about §2.9's OTHER claim -- that the
// baseline AI routine is identical at every tier. That is a property of code this file cannot
// observe; see the record entry for why it is unpinned rather than half-pinned.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDifficultyTierDeltasTest,
	"Stratocracy.StratPlay.T-FAME-02.DifficultyTierDeltasAreSection29sThreeNumbers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDifficultyTierDeltasTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapMatchClauses;

	// §2.9, quoted in `global.md`'s 2026-08-28 ruling: Easy is the player +150, Normal is even,
	// Hard is the player -100. These are DELTAS on the scenario's configured value and never
	// absolutes -- 350 and 100 are what the shipped scenario's 200 REACHES, not what §2.9 says.
	TestEqual(TEXT("T-FAME-02: §2.9's Easy tier opens the player +150 above the configured value"),
		StratDifficultyFameDelta(EStratDifficulty::Easy), 150);
	TestEqual(TEXT("T-FAME-02: §2.9's Normal tier is even -- the configured value stands"),
		StratDifficultyFameDelta(EStratDifficulty::Normal), 0);
	TestEqual(TEXT("T-FAME-02: §2.9's Hard tier opens the player 100 below the configured value"),
		StratDifficultyFameDelta(EStratDifficulty::Hard), -100);

	// The directions, so a mapping that returned one number for two tiers is caught even if the
	// three equalities above were transcribed wrong together.
	TestTrue(TEXT("T-FAME-02: Easy is a HELP -- strictly above Normal"),
		StratDifficultyFameDelta(EStratDifficulty::Easy)
			> StratDifficultyFameDelta(EStratDifficulty::Normal));
	TestTrue(TEXT("T-FAME-02: Hard is a HINDRANCE -- strictly below Normal"),
		StratDifficultyFameDelta(EStratDifficulty::Hard)
			< StratDifficultyFameDelta(EStratDifficulty::Normal));

	// The shipped default, which §2.11.6 names: "the first match runs on the one shipped
	// scenario at Easy by default". Read off a default-constructed config, not asserted about
	// the enum's underlying value.
	const FStratMatchConfig Defaults;
	TestTrue(TEXT("T-FAME-02: §2.11.6's shipped default tier is Easy"),
		Defaults.Difficulty == EStratDifficulty::Easy);

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the handicap is INERT unless the match is single-player, and a hot seat opens on
// the scenario's configured value at every tier including Easy.
//
// THE RULING THIS PINS IS THE `coordinator`'S AND IS MARKED AS SUCH IN BOTH `global.md` AND
// `StratMatchSubsystem.h`: §2.7 calls it "Single-player difficulty", so with no AI seat there
// is no player-versus-opponent asymmetry for a starting-Fame handicap to express. It is a
// reading of two words and not a GDD quotation. THE CLAUSE IS WRITTEN SO THAT OVERTURNING THE
// RULING FAILS EXACTLY HERE rather than in twenty clauses that assumed a 200/200 opening: the
// shipped hot seat is the default configuration, and applying the handicap to every human side
// would move it from 200/200 to 350/350 at Easy.
//
// THE PURE FUNCTION IS ASSERTED DIRECTLY, WHICH IS WHY IT EXISTS. `StratHandicappedSide` holds
// the whole ruling in one place so a clause can pin the ruling rather than infer it from an
// opening purse -- and inferring it from a purse is exactly what Normal tier makes impossible,
// since an inert handicap and a zero delta are the same number.
//
// AND THE THIRD ARM IS THE CONTROL. Without "AiSides = {the OTHER side} returns ViewingSide",
// both `INDEX_NONE` assertions would pass on a function that returned `INDEX_NONE`
// unconditionally -- which is to say, on a handicap that had been deleted.
//
// THE LIVE HALF USES EASY AND NOT NORMAL, deliberately. Normal's delta is zero, so a hot seat
// at Normal cannot tell an inert handicap from an applied one. `StratMatchSubsystem.h` makes
// the same point about why the field's C++ default is `Easy` rather than `Normal`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapInertWithoutOpponentTest,
	"Stratocracy.StratPlay.T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapInertWithoutOpponentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapMatchClauses;

	// The board's own sentence about a fixture with no Blueprint defaults. Declared rather
	// than suppressed; `StratMatchReconcile.cpp` records why it is a presentation split and
	// not a failed match.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// ---- the ruling, asserted on the function that holds it -----------------
	{
		FStratMatchConfig HotSeat;
		HotSeat.ViewingSide = kViewingSide;
		TestEqual(TEXT("no AiSides is no AI seat, so §2.7's handicap has no side to move"),
			HotSeat.AiSides.Num(), 0);
		TestEqual(
			TEXT("T-FAME-02: a hot seat is handicapped on NO side -- the shipped configuration"),
			StratHandicappedSide(HotSeat), static_cast<int32>(INDEX_NONE));

		FStratMatchConfig ScreenIsAi;
		ScreenIsAi.ViewingSide = kViewingSide;
		ScreenIsAi.AiSides     = { kViewingSide };
		TestEqual(
			TEXT("T-FAME-02: the screen opening on an AI seat is handicapped on NO side -- "
			     "either both sides are AI or the configuration is inverted"),
			StratHandicappedSide(ScreenIsAi), static_cast<int32>(INDEX_NONE));

		FStratMatchConfig BothAi;
		BothAi.ViewingSide = kViewingSide;
		BothAi.AiSides     = { kViewingSide, kAiSide };
		TestEqual(
			TEXT("T-FAME-02: phase D's AI-vs-AI configuration is handicapped on NO side, so "
			     "that gate cannot move"),
			StratHandicappedSide(BothAi), static_cast<int32>(INDEX_NONE));

		// THE CONTROL. Every assertion above would hold of a function that returned INDEX_NONE
		// unconditionally.
		FStratMatchConfig SinglePlayer;
		SinglePlayer.ViewingSide = kViewingSide;
		SinglePlayer.AiSides     = { kAiSide };
		TestEqual(
			TEXT("CONTROL: a single-player match IS handicapped, on the viewing seat -- "
			     "otherwise the three INDEX_NONEs above would pass on a deleted handicap"),
			StratHandicappedSide(SinglePlayer), kViewingSide);
	}

	// ---- and the live hot seat, at the tier whose delta is not zero ---------
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// Easy, and `AiSides` left at its empty default: the shipped configuration §2.11.6 names.
	Config.Difficulty = EStratDifficulty::Easy;
	TestTrue(TEXT("the tier under test has a NON-ZERO delta, so an applied handicap would be "
	              "visible if the inertness broke"),
		StratDifficultyFameDelta(Config.Difficulty) != 0);

	TArray<int32> Easy, EasyCombat;
	if (!TestTrue(TEXT("a hot-seat match at Easy goes live"),
			OpeningFame(*Subsystem, Config, Easy, EasyCombat, Error)))
	{
		AddError(Error);
		return false;
	}

	// Normal is the tier whose delta is zero, so its opening is the scenario's configured
	// value whether the handicap is inert or not. That makes it the reference reading, and
	// the assertion is that Easy did not differ from it on ANY side.
	FStratMatchConfig Even = Config;
	Even.Difficulty = EStratDifficulty::Normal;
	TestEqual(TEXT("the reference tier's delta is zero, which is what makes it a reference"),
		StratDifficultyFameDelta(Even.Difficulty), 0);

	TArray<int32> Reference, ReferenceCombat;
	if (!TestTrue(TEXT("a hot-seat match at Normal goes live"),
			OpeningFame(*Subsystem, Even, Reference, ReferenceCombat, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestEqual(TEXT("both readings describe the same number of sides"),
			Easy.Num(), Reference.Num()))
	{
		return false;
	}

	for (int32 Side = 0; Side < Easy.Num(); ++Side)
	{
		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-02: side %d's hot-seat opening at Easy is the scenario's CONFIGURED ")
				TEXT("value -- the same number the zero-delta tier opens on, and never a literal"),
				Side),
			Easy[Side], Reference[Side]);
		TestEqual(
			*FString::Printf(TEXT("T-FAME-01: side %d opens with no combat fame at either tier"),
				Side),
			EasyCombat[Side], ReferenceCombat[Side]);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- in a single-player match, every tier moves the PLAYER's opening Fame by that
// tier's delta and leaves the AI's on the scenario's configured value.
//
// THE BASELINE IS MEASURED AND NOT WRITTEN DOWN. It is the hot-seat opening -- the
// configuration in which `StratHandicappedSide` is `INDEX_NONE` -- read per side off the view
// model. `T-FAME-02`'s stub instructs a gate to assert "each side's configured value and never
// a literal 200", and this is that instruction obeyed: nothing here knows what the scenario
// configured and nothing here needs to. Re-author `startingFame` in the scenario and this
// clause follows it.
//
// THE CROSS-CHECK WITH THE INERTNESS CLAUSE IS NOT AN ACCIDENT. If the handicap ever applied to
// a hot seat, the baseline read here would shift by the same delta the handicapped match shifts
// by, and `Player == Baseline + Delta` would go RED rather than absorbing the bug. See the
// header block.
//
// THE AI SIDE IS THE SHARPER HALF. §2.9 says "the AI opens on 200 at every tier"; in this
// file's terms, the AI side equals its own hot-seat baseline at all three tiers. A handicap
// that moved both sides -- the most likely way to get this wrong -- passes every player-side
// assertion and fails here.
//
// AND `fameCombat` IS PINNED AT THE OPENING FOR BOTH SIDES, at every tier, as the match-level
// half of the bridge clause's non-movement property. It is a WEAKER reading than that clause's
// and is labelled as such: at the opening both counters are zero, so this arm cannot tell "left
// alone" from "zeroed". `Stratocracy.StratBridge.T-FAME-01.HandicapLeavesANonZeroFameCombatUnmoved`
// is the one that can, and it is the clause to read for that property.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapMovesPlayersOpeningFameTest,
	"Stratocracy.StratPlay.T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapMovesPlayersOpeningFameTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the baseline: a hot seat, where the handicap is inert --------------
	TArray<int32> Baseline, BaselineCombat;
	if (!TestTrue(TEXT("the baseline hot-seat match goes live"),
			OpeningFame(*Subsystem, Config, Baseline, BaselineCombat, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the projection describes more than one side, so 'the other side' means "
	                   "something"),
			Baseline.Num() > 1))
	{
		return false;
	}
	if (!TestTrue(TEXT("the scenario configured a positive opening purse, so a delta composes "
	                   "with something and the Hard arm is not a clamp"),
			Baseline[kViewingSide] > 0 && Baseline[kAiSide] > 0))
	{
		return false;
	}

	// ---- every tier, single-player -----------------------------------------
	FStratMatchConfig SinglePlayer = Config;
	SinglePlayer.AiSides = { kAiSide };
	TestEqual(TEXT("this configuration IS handicapped, and on the viewing seat"),
		StratHandicappedSide(SinglePlayer), kViewingSide);

	for (const EStratDifficulty Tier : AllTiers())
	{
		FStratMatchConfig Tiered = SinglePlayer;
		Tiered.Difficulty = Tier;

		const int32 Delta = StratDifficultyFameDelta(Tier);

		TArray<int32> Opening, Combat;
		if (!TestTrue(*FString::Printf(TEXT("the %s match goes live"), TierName(Tier)),
				OpeningFame(*Subsystem, Tiered, Opening, Combat, Error)))
		{
			AddError(Error);
			return false;
		}
		if (!TestEqual(*FString::Printf(TEXT("the %s reading describes the same sides as the "
		                                     "baseline"), TierName(Tier)),
				Opening.Num(), Baseline.Num()))
		{
			continue;
		}

		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-02: at %s the PLAYER's side opens on the scenario's configured ")
				TEXT("value %+d -- baseline %d read from the inert hot seat, delta from ")
				TEXT("StratDifficultyFameDelta"),
				TierName(Tier), Delta, Baseline[kViewingSide]),
			Opening[kViewingSide], Baseline[kViewingSide] + Delta);

		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-02: at %s the AI's side opens on its OWN configured value, ")
				TEXT("unmoved -- §2.9: 'the AI opens on 200 at every tier'"),
				TierName(Tier)),
			Opening[kAiSide], Baseline[kAiSide]);

		for (int32 Side = 0; Side < Opening.Num(); ++Side)
		{
			TestEqual(
				*FString::Printf(
					TEXT("T-FAME-01: at %s side %d's opening fameCombat is where the baseline ")
					TEXT("left it (see the block: this arm is the weaker reading)"),
					TierName(Tier), Side),
				Combat[Side], BaselineCombat[Side]);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a slot written at one difficulty tier is REFUSED when loaded at another.
//
// WHY THIS IS A CLAUSE AND NOT A COMMENT. `global.md`'s ruling names it as a hazard: the
// handicap is not a scenario field, so `scenarioHash` cannot carry it, and it is not a
// `strat::SaveCommand`, so the §4.10 log cannot either. `RestoreFromSaveText` replays the log
// onto the freshly seeded state and compares `strat::canonicalStateHash`, whose preimage
// carries `fT` per side -- so a tier change moves the seed the replay starts from and the
// hashes cannot agree. IT FAILS SAFE. What it must not do is fail SILENTLY, and what it must
// not do instead is SUCCEED -- which is what applying the handicap AFTER the restore would
// produce: a slot that loaded "successfully" into a match worth 150 Fame more than the one
// that was saved. `StartMatchInternal` orders the handicap before the restore for that reason,
// and this clause is what holds the ordering.
//
// THE CONTROL IS THE SAME-TIER LOAD AND IT IS THE HALF THAT MAKES THE REFUSAL MEAN SOMETHING.
// Restarting a match and loading a slot into it is itself a route that could refuse for
// reasons having nothing to do with difficulty. So the identical sequence is run first at the
// SAME tier and is required to SUCCEED, reaching the hash the save was taken at.
//
// THE ID IS COMPARED CASE-SENSITIVELY. `FString::operator==`, `Contains` and `TestEqual` are
// all case-insensitive in this engine and that has already produced a clause in this project
// that could not fail.
//
// WHAT IS PINNED ABOUT THE PROSE, AND WHAT IS NOT. The refusal must carry `T-SAVE-06`, and it
// must name the handicap as a cause -- that second half is the whole content of the engineer's
// change to the message, whose stated purpose is that a reader not be sent to blame the log.
// The word `handicap` is what the message would lose if that change were reverted; the rest of
// the sentence is not pinned, because a rewording with identical behaviour is not a regression.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCrossTierLoadRefusedTest,
	"Stratocracy.StratPlay.T-SAVE-06.ASlotWrittenAtOneTierIsRefusedAtAnother",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCrossTierLoadRefusedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// `StartMatchInternal` logs the rules-side teardown at Error, and this clause CAUSES one
	// on purpose. Declared rather than suppressed: an undeclared Error fails the clause, and
	// suppressing it would hide the message the clause is about.
	AddExpectedMessagePlain(TEXT("No match this session"), ELogVerbosity::Error,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// `LoadMatchFromSlot`'s own report of the same refusal, at Warning. Declared for the same
	// reason and with the same force: `Occurrences = 0` means "at least one", so this is a
	// CLAIM that the load path says something about a refused load rather than failing mute.
	AddExpectedMessagePlain(TEXT("Load of slot"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope       Slot;
	FTestWorldScope  Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Written;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Written, Error)))
	{
		AddError(Error);
		return false;
	}
	Written.AiSides    = { kAiSide };
	Written.Difficulty = EStratDifficulty::Easy;

	// The premises this clause rests on, asserted rather than assumed: the handicap is armed
	// in this configuration, and the two tiers below are genuinely different openings.
	TestEqual(TEXT("the saved match IS handicapped, on the viewing seat"),
		StratHandicappedSide(Written), kViewingSide);
	const EStratDifficulty OtherTier = EStratDifficulty::Hard;
	TestNotEqual(
		TEXT("the two tiers give DIFFERENT deltas, so a cross-tier load really does seed a "
		     "different state"),
		StratDifficultyFameDelta(Written.Difficulty), StratDifficultyFameDelta(OtherTier));

	// ---- write the slot at the first tier -----------------------------------
	FString StartReason;
	Subsystem->StartMatch(Written, StartReason);
	if (!TestTrue(TEXT("the match to be saved is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratBridge* const Live = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match has a bridge"), Live))
	{
		return false;
	}

	// Off the opening, so the restore has a log to replay and the control below measures the
	// replay rather than the seed. `SubmitEndTurn` is the shortest §4.9 command that names no
	// coordinate.
	if (!TestTrue(TEXT("one EndTurn is recorded through the rules module"),
			Live->SubmitEndTurn().bOk))
	{
		return false;
	}
	const FString SavedHash  = Live->StateHash();
	const int32   SavedCount = Live->RecordedCommandCount();
	TestTrue(TEXT("the save has a command to replay"), SavedCount > 0);

	if (!TestTrue(TEXT("the handicapped match saves to this file's slot"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- THE CONTROL: the same route, the same tier, and it must SUCCEED ----
	{
		Subsystem->StartMatch(Written, StartReason);
		if (!TestTrue(TEXT("CONTROL: a fresh match at the SAME tier is live before the load"),
				Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		FString LoadReason;
		Subsystem->LoadMatchFromSlot(FString(), LoadReason);
		if (!TestTrue(
				*FString::Printf(TEXT("CONTROL: loading the slot at the tier it was written at "
				                      "leaves a live match; got '%s'"), *LoadReason),
				Subsystem->IsMatchLive()))
		{
			return false;
		}

		const FStratBridge* const Reloaded = Subsystem->GetBridge();
		if (!TestNotNull(TEXT("CONTROL: the loaded match has a bridge"), Reloaded))
		{
			return false;
		}
		TestEqual(
			TEXT("CONTROL: it reaches the canonical state hash the save was taken at, so the "
			     "refusal below is caused by the TIER and not by this route"),
			Reloaded->StateHash(), SavedHash);
		TestEqual(TEXT("CONTROL: and carries the same recorded log length"),
			Reloaded->RecordedCommandCount(), SavedCount);
	}

	// ---- the cross-tier load ------------------------------------------------
	FStratMatchConfig Loading = Written;
	Loading.Difficulty = OtherTier;

	Subsystem->StartMatch(Loading, StartReason);
	if (!TestTrue(TEXT("a fresh match at the OTHER tier is live before the load"),
			Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FString Refusal;
	const bool bLoaded = Subsystem->LoadMatchFromSlot(FString(), Refusal);

	TestFalse(
		TEXT("T-SAVE-06: a slot written at one tier does not load at another -- the handicap "
		     "moves the seed the log replays onto, and the canonical hashes cannot agree"),
		bLoaded);
	TestFalse(
		TEXT("T-SAVE-06: and the refusal is a TEARDOWN, not a presentation complaint: no match "
		     "is live afterwards, so nothing is on screen wearing the saved match's name"),
		Subsystem->IsMatchLive());
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-06: the refusal carries the id T-SAVE-06 exactly (case-sensitive); "
			     "got '%s'"), *Refusal),
		Refusal.Contains(TEXT("T-SAVE-06"), ESearchCase::CaseSensitive));
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-06: and it NAMES the handicap as a cause, so a reader is not sent to "
			     "blame the log, the definitions or the seeding side; got '%s'"), *Refusal),
		Refusal.Contains(TEXT("handicap"), ESearchCase::CaseSensitive));

	return true;
}
