// The LIVE half of GATE-AUDIO -- the recording player itself, and the properties of the audio
// call sites that a world-free clause structurally cannot reach. The local ID is authorized for
// this file by the 2026-09-04 ruling in `Tools/architect/state/global.md`, which mints no
// acceptance ID and claims none; see `Source/StratUI/Tests/StratSoundCueClauses.cpp` for the
// one-sentence form of why no `T-` ID owns this surface.
//
// WHY THIS FILE IS IN `StratPlay/Tests` AND NOT BESIDE THE DECIDER. Everything asserted below
// is a property of `UStratSoundDirector`, `UStratMatchSubsystem` or `AStratPlayerController`:
// the ORDER of two statements inside `ApplyView`, the lifetime of the mark, the tour gate, the
// conclusion latch, and the fact that a refused button still clicks. `StratUI` is BELOW
// `StratPlay` and the arrow does not reverse, so a clause there could not name any of them.
// Everything that can be asserted without a world already is, next door; this file deliberately
// holds only the residue. Same split as
// `StratTransientReceiptClauses.cpp` / `StratTransientReceiptCallSite.cpp`.
//
// ---------------------------------------------------------------------------------------
// THE ONE PROPERTY THE WHOLE FILE RESTS ON, AND IT IS ASSERTED FIRST FOR THAT REASON.
//
// NO SOUND ASSET IS SET ANYWHERE IN THIS PROJECT. No `UStratSoundBank` exists yet -- phase C
// authors the assets and phase D pins what the Blueprints carry -- so every cue this suite ever
// observes is recorded with `EStratSoundDisposition::NoBank`. That is not a limitation of the
// fixture; it is the SHIPPED state, and `UStratSoundDirector` was written so that the shipped
// state is fully observable: `EmitCue` RECORDS FIRST AND PLAYS SECOND, with no early return on
// any path, and every outcome gets a named disposition.
//
// SO A SINGLE `if (Bank == nullptr) { return; }` AT THE TOP OF `EmitCue` WOULD MAKE EVERY OTHER
// CLAUSE IN THIS FILE VACUOUS AT ONCE -- a green suite over a feature that records nothing, and
// one line that reads like defensive hygiene. `EmitCueRecordsWithNoBank` is written first,
// placed first, and is the clause to read first, because it is the one that keeps the other
// eight non-vacuous. This project's record already carries two measured defects of that exact
// species: a shipped zero default that made every clause vacuous, and a real default that could
// not signal unset.
// ---------------------------------------------------------------------------------------
//
// WHERE THE EXPECTATIONS COME FROM. Every cue count below is compared against the director's
// own published record -- `GetEmissions`, `GetEmitCallCount`, `GetApplyViewObservationCount` --
// and every unit id, side, hex and turn is read off `UStratMatchSubsystem::GetViewModel()`, the
// model the subsystem itself applied. There is not one unit id, side index, hex, HP figure or
// turn number written in this file. The one interval that IS written is a HARNESS interval and
// is not a pace -- see `kHarnessPlaybackInterval`.
//
// SILENCE IS NEVER ASSERTED ALONE, AND THAT IS THE DISCIPLINE THIS FILE ADDS OVER THE OTHER.
// An empty emission list has two causes that look identical: every cue was correctly
// suppressed, or the seam is DEAD -- deleted, mis-ordered or gated off. This project has a
// named defect class for exactly that ("a reflected verb with no caller reads as built": check
// the ROUTE, not the mechanism). So every clause below that asserts a silence pairs it with a
// positive control: `GetApplyViewObservationCount()` where `ApplyView` is the subject, an
// actor population that demonstrably changed where the tour is, and a return value where a verb
// is.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. Seeding goes through `FStratMatchConfig`
// and the subsystem; a `strat::` call here is `LNK2019`, measured 8x.
//
// THESE FIXTURES HAVE NO TILE MESHES, and say so rather than papering over it. `StartMatch`
// spawns and draws the board in one call, so a headless gate has no moment in which to hand it
// the Blueprint defaults the content lane authors. The call therefore returns FALSE while
// `IsMatchLive()` returns true -- the split `StratMatchSubsystem.h` states in terms -- and the
// board's warning is declared expected, on `StratMatchReconcile.cpp`'s precedent.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSoundCues.h"
#include "StratSoundDirector.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratSoundDirectorCallSite
{
	/** The same harness pin every parity file in this tree carries. No rule in this project
	 *  decides which side moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	/**
	 * A HARNESS INTERVAL AND NOT A PACE, on `StratTourExistenceHoldClauses.cpp`'s reasoning
	 * verbatim. These worlds are created with `bInformEngineOfWorld = false` and are never
	 * ticked, so nothing advances a tour except a clause that asks it to. §2.11.2's 0.5 is not
	 * written here, on `AiPlaybackStepSeconds`' own recorded ruling that the pace has one home
	 * and a test is not it. What this value selects is only that `WillAiPlaybackRun()` is TRUE,
	 * which is the condition that raises the tour gate this file is about.
	 */
	static const float kHarnessPlaybackInterval = 600.0f;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchConclusion.cpp` on why `InitializeActorsForPlay` is not decoration and
	 *  why the world is destroyed before its context. Duplicated from the other clause files in
	 *  this directory rather than shared, which is this project's standing posture for a test
	 *  helper -- sharing would put a header inside a `Tests/` directory. */
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
				World->InitializeActorsForPlay(FURL());
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
		Out.ViewingSide     = kFirstSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();

		// NO `SoundBank`, AND THAT IS THE SHIPPED CONFIGURATION RATHER THAN AN OMISSION. See
		// the header block: with no bank every record lands on `NoBank`, which is a full named
		// answer and is what these clauses assert against.
		return true;
	}

	/**
	 * Both sides AI, so a whole §2.8 game resolves inside one synchronous `RunAiTurnsNow`.
	 *
	 * THE SIDES ARE READ OFF THE SCENARIO'S OWN VIEW MODEL and are not written here, on
	 * `StratTourExistenceHoldClauses.cpp`'s pattern: it starts a PROBE match, reads which sides
	 * the scenario actually deploys units for, and hands back a config the caller starts for
	 * real. A hand-written `{0, 1}` would be this file deciding the scenario's roster.
	 */
	static bool MakeAiVsAiConfig(UStratMatchSubsystem& Match, const float PlaybackStepSeconds,
	                             FStratMatchConfig& Out, FString& OutError)
	{
		if (!MakeConfig(Out, OutError))
		{
			return false;
		}

		FStratMatchConfig Probe = Out;
		Match.StartMatch(Probe, OutError);
		if (!Match.IsMatchLive())
		{
			return false;
		}

		FStratViewModel Model;
		if (!Match.BuildViewModel(Model, OutError))
		{
			return false;
		}

		TArray<int32> Sides;
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		if (Sides.Num() < 2)
		{
			OutError = TEXT("the shipped scenario deploys units for fewer than two sides");
			return false;
		}

		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));

		Out.AiSides               = Sides;
		Out.AiBuildlistUnitIds    = Units != nullptr ? Units->GetRowNames() : TArray<FName>();
		Out.AiPlaybackStepSeconds = PlaybackStepSeconds;
		return true;
	}

	static const TCHAR* CueWord(const EStratSoundCue Cue)
	{
		switch (Cue)
		{
		case EStratSoundCue::ButtonClick:      return TEXT("ButtonClick");
		case EStratSoundCue::TurnEnded:        return TEXT("TurnEnded");
		case EStratSoundCue::UnitMoved:        return TEXT("UnitMoved");
		case EStratSoundCue::UnitAttacked:     return TEXT("UnitAttacked");
		case EStratSoundCue::UnitDestroyed:    return TEXT("UnitDestroyed");
		case EStratSoundCue::FactoryBuiltUnit: return TEXT("FactoryBuiltUnit");
		case EStratSoundCue::MatchEnded:       return TEXT("MatchEnded");
		}
		return TEXT("<unknown>");
	}

	static const TCHAR* DispositionWord(const EStratSoundDisposition Disposition)
	{
		switch (Disposition)
		{
		case EStratSoundDisposition::Played:               return TEXT("Played");
		case EStratSoundDisposition::NoBank:               return TEXT("NoBank");
		case EStratSoundDisposition::NoSoundConfigured:    return TEXT("NoSoundConfigured");
		case EStratSoundDisposition::SuppressedByCooldown: return TEXT("SuppressedByCooldown");
		case EStratSoundDisposition::NoWorld:              return TEXT("NoWorld");
		}
		return TEXT("<unknown>");
	}

	static FString Describe(const TArray<FStratSoundEmissionRecord>& Records, const int32 From = 0)
	{
		TArray<FString> Parts;
		for (int32 Index = From; Index < Records.Num(); ++Index)
		{
			const FStratSoundEmissionRecord& Record = Records[Index];
			Parts.Add(FString::Printf(TEXT("{cue=%s side=%d unit=%d turn=%d disposition=%s}"),
				CueWord(Record.Cue), Record.Side, Record.UnitId, Record.Turn,
				DispositionWord(Record.Disposition)));
		}
		return Parts.Num() == 0 ? FString(TEXT("<empty>")) : FString::Join(Parts, TEXT(", "));
	}

	static int32 CountOfCue(const TArray<FStratSoundEmissionRecord>& Records,
	                        const EStratSoundCue                     Cue,
	                        const int32                              From = 0)
	{
		int32 Count = 0;
		for (int32 Index = From; Index < Records.Num(); ++Index)
		{
			if (Records[Index].Cue == Cue)
			{
				++Count;
			}
		}
		return Count;
	}

	/** Every unit id in the model the subsystem last APPLIED. `GetViewModel()` and not
	 *  `BuildViewModel`, which is the distinction the subsystem's own header draws: the audio
	 *  seam sits inside `ApplyView` and reads the applied model, so this does too. */
	static TSet<int32> AppliedUnitIds(const UStratMatchSubsystem& Match)
	{
		TSet<int32> Ids;
		for (const FStratUnitView& View : Match.GetViewModel().Units)
		{
			Ids.Add(View.UnitId);
		}
		return Ids;
	}

	/**
	 * Declares the two log lines an AI-vs-AI hand-over on a fixture world always emits.
	 *
	 * DECLARED FROM A MEASURED RUN AND NOT SPECULATIVELY. `AddExpectedMessagePlain` with
	 * `Occurrences 0` FAILS a clause where the message never fires, so a defensive declaration
	 * is itself an assertion -- the point `StratAiPlaybackClauses.cpp` and
	 * `StratTourExistenceHoldClauses.cpp` both record about these same two lines on this same
	 * fixture shape.
	 */
	static void DeclareHandoverNoise(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
		Test.AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- `EmitCue` RECORDS WITH NO BANK, AND THAT IS WHAT KEEPS THIS PHASE OBSERVABLE.
//
// READ THE HEADER BLOCK. This is the clause the other eight rest on: no sound asset exists
// anywhere in this project, so if a null bank produced no record then every audio clause in the
// suite would be asserting over an empty list and would stay green over a deleted feature.
//
// THE FOUR ASSERTIONS AND WHAT EACH MUTANT DOES TO THEM.
//   - `GetSoundBank() == nullptr` FIRST, as the precondition. Without it the clause could be
//     green over a fixture that had somehow acquired a bank, which is a different code path.
//   - ONE RECORD. An `if (SoundBank == nullptr) { return; }` before the append leaves the list
//     EMPTY and is red here.
//   - `Disposition == NoBank`. A disposition left at whatever the struct defaults to, or
//     collapsed into a single `bool bPlayed`, cannot say WHY the request was silent -- and
//     "no sound came out" is not a finding a clause can use.
//   - `GetEmitCallCount() == 1`. The count is kept separately from the record so it stays true
//     if the record ever grows a cap, and asserting both is what would separate "the record was
//     trimmed" from "the call never happened".
//
// IT NEEDS NO MATCH AND STARTS NONE, deliberately. `UStratSoundDirector` is a `UWorldSubsystem`
// and not a member of `UStratMatchSubsystem` precisely so the title map -- which has no match
// and never will -- can still click; a fixture that needed a live match to reach the director
// would be asserting the shape that design rejected.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundEmitRecordsWithNoBankTest,
	"Stratocracy.StratPlay.GATE-AUDIO.EmitCueRecordsWithNoBank",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundEmitRecordsWithNoBankTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(
			TEXT("GATE-AUDIO: a Game world acquires a UStratSoundDirector with no match started "
			     "-- the title map has no match and its buttons must still click"),
			Director))
	{
		return false;
	}

	// THE PRECONDITION, AND IT IS THE SHIPPED STATE OF THIS PROJECT rather than a fixture
	// choice: no `UStratSoundBank` asset exists yet.
	if (!TestNull(TEXT("CONTROL: no sound bank is adopted, which is the shipped configuration "
	                   "and the state every other clause in this file runs in"),
			Director->GetSoundBank()))
	{
		return false;
	}
	if (!TestEqual(TEXT("CONTROL: the director starts with an empty record"),
			Director->GetEmissions().Num(), 0))
	{
		return false;
	}

	Director->EmitCue(EStratSoundCue::TurnEnded, /*Side*/ INDEX_NONE, /*UnitId*/ INDEX_NONE,
	                  /*Turn*/ 0);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: `EmitCue` RECORDS FIRST AND PLAYS SECOND, so a request made with "
			     "no bank leaves evidence -- an early return before the append is red on this "
			     "line and would make every other audio clause vacuous: %s"),
			*Describe(Director->GetEmissions())),
			Director->GetEmissions().Num(), 1))
	{
		return false;
	}

	TestEqual(TEXT("GATE-AUDIO: the record carries the cue it was asked for"),
		static_cast<int32>(Director->GetEmissions()[0].Cue),
		static_cast<int32>(EStratSoundCue::TurnEnded));

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and the NAMED disposition `NoBank` -- not merely 'silent', which "
			     "would not distinguish a correctly configured quiet director from one nobody "
			     "wired up (was %s)"),
			DispositionWord(Director->GetEmissions()[0].Disposition)),
		static_cast<int32>(Director->GetEmissions()[0].Disposition),
		static_cast<int32>(EStratSoundDisposition::NoBank));

	TestEqual(TEXT("GATE-AUDIO: and the call count agrees with the record it is kept separately "
	               "from"),
		Director->GetEmitCallCount(), 1);

	// THE RESET SEAM, ASSERTED HERE BECAUSE FOUR CLAUSES BELOW DEPEND ON IT. A `ResetEmissions`
	// that cleared the array and not the counters would make every "reset, then act, then
	// count" fixture in this file read a stale number.
	Director->ResetEmissions();
	TestEqual(TEXT("GATE-AUDIO: `ResetEmissions` clears the record"),
		Director->GetEmissions().Num(), 0);
	TestEqual(TEXT("GATE-AUDIO: and the call count with it"), Director->GetEmitCallCount(), 0);
	TestEqual(TEXT("GATE-AUDIO: and the observation count with it"),
		Director->GetApplyViewObservationCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- THE FIRST `ApplyView` OF A MATCH EMITS NOTHING, AND THE SEAM STILL RAN.
//
// THE LIVE FORM OF `AnUnseededMarkEmitsNothing`. The world-free clause pins that the DECIDER is
// silent on an unseeded mark; this one pins that the match's opening actually takes that path
// -- that `SoundMark` is genuinely unseeded when `StartMatch`'s own `ApplyView` runs, rather
// than having been seeded by something earlier. Ferrum Crossing deploys ten units, so getting
// this wrong opens every match with a factory cue that sounds exactly like a correct one.
//
// AND IT IS THE CLAUSE THAT SHOWS WHY `NoteApplyViewObserved` IS NOT DECORATION. Asserting the
// silence alone would be green over a `ApplyView` whose whole audio seam had been deleted --
// silence and absence are the same observation. The observation counter separates them: a
// seam that ran and chose to emit nothing advances it, and a seam that is not there does not.
// This project has a named defect class for exactly that ("a reflected verb with no caller
// reads as built": check the ROUTE, not the mechanism).
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundFirstApplyViewIsSilentTest,
	"Stratocracy.StratPlay.GATE-AUDIO.TheFirstApplyViewOfAMatchEmitsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundFirstApplyViewIsSilentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
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

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// THE CONTROL FOR THE SILENCE, AND IT COMES FIRST. Without it the assertion below is
	// satisfied by a deleted seam.
	AddInfo(FString::Printf(
		TEXT("StartMatch's own reconciliation observed the seam %d time(s) and produced: %s"),
		Director->GetApplyViewObservationCount(), *Describe(Director->GetEmissions())));

	if (!TestTrue(
			TEXT("CONTROL: `ApplyView`'s audio seam RAN during StartMatch -- the observation "
			     "counter is what separates 'suppressed' from 'the seam is dead', and zero here "
			     "would make the silence below meaningless"),
			Director->GetApplyViewObservationCount() > 0))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the opening applied %d units, every one of which an unseeded-mark "
			     "bug would have reported as newly built"),
			Match->GetViewModel().Units.Num()),
			Match->GetViewModel().Units.Num() > 0))
	{
		return false;
	}

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a match's FIRST refresh has no predecessor and is therefore "
			     "SILENT -- the seam ran and chose to emit nothing: %s"),
			*Describe(Director->GetEmissions())),
		Director->GetEmissions().Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- `ApplyView` DECIDES AGAINST THE PREVIOUS MARK, THEN RE-MARKS.
//
// WHY THE ORDER OF TWO ADJACENT LINES IS WORTH A CLAUSE, AND IT IS THE SAME ARGUMENT
// `StratTransientReceiptCallSite.cpp` makes about the receipts' identical pair. Swap
// `StratDecideSoundCues` and `StratSoundMarkFromView` and every refresh compares the model
// against ITSELF: no cue can ever fire again, for the life of the game. AND THAT FAILURE IS
// SILENT IN THE MOST LITERAL POSSIBLE SENSE -- a match that makes no noise is indistinguishable
// from a match with no sound assets configured, which is also the shipped state today. Nothing
// goes red, nothing logs, and the feature is simply absent.
//
// THE THREE APPLIES ARE WHAT MAKES THE ORDER SEPARABLE, and no shorter fixture does it.
//   1. `StartMatch` runs its own `ApplyView` and leaves the mark seeded at the opening. That is
//      `TheFirstApplyViewOfAMatchEmitsNothing`'s subject and is assumed here.
//   2. A model carrying a planted MOVE is applied. The cue must fire, and must name the unit
//      that moved. Re-marking first would have compared the model with itself: no cue, an empty
//      list, and a green suite.
//   3. The SAME model applied again must be QUIET. Without that half the clause would pass over
//      a call site that decided correctly and never re-marked at all -- which would report the
//      same move for the rest of the match, once per mouse movement.
//
// THE PLANT IS ANOTHER UNIT'S HEX OFF THE APPLIED MODEL, never a hex this file invented, and
// the clause asserts the two differ before relying on it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundApplyViewOrderTest,
	"Stratocracy.StratPlay.GATE-AUDIO.ApplyViewDecidesBeforeItReMarks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundApplyViewOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
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

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const FStratViewModel Opening = Match->GetViewModel();
	if (!TestTrue(TEXT("CONTROL: the opening applied at least two units, so one has somewhere to "
	                   "be moved TO that this file did not invent"),
			Opening.Units.Num() >= 2))
	{
		return false;
	}

	Director->ResetEmissions();

	// ---- 2. a planted move, applied through the live path ----------------------------
	FStratViewModel Moved = Opening;
	Moved.Units[0].Hex = Opening.Units[1].Hex;
	if (!TestTrue(TEXT("CONTROL: the destination really differs from the origin"),
			Moved.Units[0].Hex != Opening.Units[0].Hex))
	{
		return false;
	}

	Match->ApplyView(Moved);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: `ApplyView` decided against the PREVIOUS mark, so the move was "
			     "heard -- swapping the decide and the re-mark silences the feature forever and "
			     "silence is indistinguishable from a quiet match: %s"),
			*Describe(Director->GetEmissions())),
			CountOfCue(Director->GetEmissions(), EStratSoundCue::UnitMoved), 1))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: naming the unit that moved, off the model that was applied"),
		Director->GetEmissions()[0].UnitId, Opening.Units[0].UnitId);
	TestEqual(TEXT("GATE-AUDIO: and recorded with the shipped disposition"),
		static_cast<int32>(Director->GetEmissions()[0].Disposition),
		static_cast<int32>(EStratSoundDisposition::NoBank));

	// ---- 3. and the mark moved afterwards --------------------------------------------
	Director->ResetEmissions();
	Match->ApplyView(Moved);

	if (!TestTrue(TEXT("CONTROL: the second apply's seam ran"),
			Director->GetApplyViewObservationCount() > 0))
	{
		return false;
	}
	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: so re-applying the SAME model is quiet, and the move is not "
			     "re-announced on every refresh for the rest of the match: %s"),
			*Describe(Director->GetEmissions())),
		Director->GetEmissions().Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- THE MARK DIES WITH THE APPLIED MODEL.
//
// A MARK SURVIVING A TEARDOWN HAS A LIVE SUCCESSOR TO BE COMPARED AGAINST: the next match's
// first model. Every one of the OLD match's units would then read as an id in the mark and
// absent from the model -- destroyed -- and every one of the NEW match's as an id in the model
// and absent from the mark -- built. Plus a `TurnEnded` for a turn that never began, since a
// fresh match opens at a different (Turn, SideToMove) pair than wherever the old one stopped.
// So the next match opens with three wrong cues, on `StratTransientReceiptCallSite.cpp`'s
// clause-2 shape exactly.
//
// THE PATH THIS DRIVES IS `TearDownPresentation`, which is private and runs unconditionally at
// the top of `StartMatchInternal`. A SECOND `StartMatch` in one session is the reachable route
// to it, and it is also the real one -- a restart and a load both go through there.
//
// `Deinitialize`'S COPY OF THE SAME LINE IS **NOT** DRIVEN HERE, AND THAT GAP IS STATED RATHER
// THAN LEFT TO BE DISCOVERED. It runs as the world dies, after which nothing is left to observe
// the mark with -- and the director dies with the same world, so even the record is gone.
// Asserting it would need a probe this lane may not add. `StratTransientReceiptCallSite.cpp`
// records the identical gap about the identical line, and `Tools/architect/state/tests.md`
// carries it as an unpinned half together with the seam that would close it.
//
// THE FIXTURE MUST MOVE THE PAIR FIRST, OR THE CLAUSE IS INERT. Two fresh matches both open at
// the same turn and the same side, so with no divergence a SURVIVING mark's pair would agree
// with the new match's and the turn arm would stay silent for the RIGHT answer for the WRONG
// reason. The clause therefore applies a model with a moved pair, and asserts the divergence
// explicitly before it asserts the silence.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundMarkResetTest,
	"Stratocracy.StratPlay.GATE-AUDIO.TheMarkDiesWithTheAppliedModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundMarkResetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
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

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the first match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// ---- drive the first match's mark away from a fresh opening ----------------------
	FStratViewModel Advanced = Match->GetViewModel();
	if (!TestTrue(TEXT("CONTROL: the opening model carries at least two sides and some units"),
			Advanced.Sides.Num() >= 2 && Advanced.Units.Num() > 0))
	{
		return false;
	}

	const int32 StaleTurn = Advanced.Match.Turn + 1;
	const int32 StaleSide = Advanced.Sides.Num() - 1 - Advanced.Match.SideToMove;
	Advanced.Match.Turn       = StaleTurn;
	Advanced.Match.SideToMove = StaleSide;

	Match->ApplyView(Advanced);

	// ---- restart, which runs TearDownPresentation on its way in ----------------------
	Director->ResetEmissions();

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the second match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const FStratViewModel& Fresh = Match->GetViewModel();

	// THE FIXTURE-VALIDITY CONTROL. Without this the clause would be green over a surviving
	// mark whose pair happened to agree with the new match's.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the stale mark's pair (turn %d, side %d) differs from the new match's "
			     "(turn %d, side %d), so a SURVIVING mark would have fired a TurnEnded"),
			StaleTurn, StaleSide, Fresh.Match.Turn, Fresh.Match.SideToMove),
			StaleTurn != Fresh.Match.Turn || StaleSide != Fresh.Match.SideToMove))
	{
		return false;
	}
	if (!TestTrue(
			TEXT("CONTROL: and the restart's own audio seam RAN, so the silence below is a "
			     "decision and not a dead call site"),
			Director->GetApplyViewObservationCount() > 0))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("the restart's first refresh produced: %s"),
		*Describe(Director->GetEmissions())));

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a restart is a FIRST observation, so the new match's opening "
			     "refresh is silent -- a surviving mark would report %d units destroyed, %d "
			     "built and a turn that never began"),
			Advanced.Units.Num(), Fresh.Units.Num()),
		Director->GetEmissions().Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- AN AI HAND-OVER IS NOT VOICED TWICE.
//
// THE MEASUREMENT THIS WHOLE DESIGN TURNS ON. A whole AI hand-over is ONE `ApplyView`:
// `RunAiTurnsNow` loops turns synchronously and refreshes exactly once, after the loop, so the
// diff that refresh takes spans every command the AI played. When a TOUR is armed, the
// per-command surface is `AdvanceAiPlaybackOneStep`, which voices each step from the REEL. The
// reel is a RECORDING and `ApplyView` is a DIFF: they describe the same events by different
// means, so with no gate every AI move, attack and build would sound TWICE -- once from the
// diff, once on the step that shows it.
//
// THE OBSERVATION COUNTER IS THE CONTROL AND IT IS NOT OPTIONAL. Asserting emptiness alone
// would be green over a dead seam, and that is the failure mode this whole class was designed
// against. So this clause asserts BOTH halves in both directions:
//   - GATE UP: zero emissions AND `GetApplyViewObservationCount()` DOES NOT MOVE. The counter
//     lives INSIDE the gate deliberately -- on a gated refresh the seam did not emit, and
//     counting it would make the counter agree with a dead seam.
//   - GATE DOWN: the same planted diff DOES emit AND the counter DOES move. Without this half
//     the clause is satisfied by an `ApplyView` whose audio seam was deleted outright.
//
// THE GATE IS RAISED THROUGH THE SHIPPING PATH AND NOT SET BY THIS FILE. `bTourExistenceHeld`
// is private and is raised by `BeginAiPlayback` under `WillAiPlaybackRun()`; the fixture
// therefore configures a positive playback interval and runs a real AI-vs-AI hand-over, and
// lowers the gate again through the shipping verb `SkipAiPlayback`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundHandoverNotVoicedTwiceTest,
	"Stratocracy.StratPlay.GATE-AUDIO.AnAiHandOverIsNotVoicedTwice",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundHandoverNotVoicedTwiceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	DeclareHandoverNoise(*this);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the interval is POSITIVE, so `WillAiPlaybackRun` is true and "
	                   "the gate is raised at all"),
			Config.AiPlaybackStepSeconds > 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, Match->GetAiPlaybackStepCount()));

	if (!TestTrue(TEXT("CONTROL: the hand-over recorded a reel, so a tour exists to gate against"),
			Match->GetAiPlaybackStepCount() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and the tour is running, which is what holds the gate up"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	// A PLANTED DIFF THAT WOULD CERTAINLY SOUND IF THE GATE WERE DOWN: the (Turn, SideToMove)
	// pair moved. Both halves are derived from the applied model.
	FStratViewModel Diff = Match->GetViewModel();
	if (!TestTrue(TEXT("CONTROL: the applied model carries at least two sides"),
			Diff.Sides.Num() >= 2))
	{
		return false;
	}
	Diff.Match.Turn       = Diff.Match.Turn + 1;
	Diff.Match.SideToMove = Diff.Sides.Num() - 1 - Diff.Match.SideToMove;

	// ---- GATE UP ----------------------------------------------------------------------
	Director->ResetEmissions();
	Match->ApplyView(Diff);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: with a tour armed, `ApplyView`'s DIFF is silent -- the reel voices "
			     "these events step by step and a second voice would sound every AI action "
			     "twice: %s"),
			*Describe(Director->GetEmissions())),
			Director->GetEmissions().Num(), 0))
	{
		return false;
	}
	if (!TestEqual(
			TEXT("GATE-AUDIO: and the observation counter DID NOT move, because the seam is "
			     "inside the gate -- counting a gated refresh would make the counter agree with "
			     "a dead seam"),
			Director->GetApplyViewObservationCount(), 0))
	{
		return false;
	}

	// ---- GATE DOWN, through the shipping verb -----------------------------------------
	if (!TestTrue(TEXT("CONTROL: the tour is skippable, which is how the gate comes down"),
			Match->SkipAiPlayback()))
	{
		return false;
	}

	FStratViewModel SecondDiff = Match->GetViewModel();
	SecondDiff.Match.Turn       = SecondDiff.Match.Turn + 1;
	SecondDiff.Match.SideToMove = SecondDiff.Sides.Num() - 1 - SecondDiff.Match.SideToMove;

	Director->ResetEmissions();
	Match->ApplyView(SecondDiff);

	TestEqual(
		TEXT("GATE-AUDIO: with the tour over the gate is DOWN and the same seam observes again "
		     "-- which is what proves the silence above was the gate and not a deleted call"),
		Director->GetApplyViewObservationCount(), 1);

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and the identical planted diff is now VOICED, so the gate "
			     "suppresses rather than mutes: %s"),
			*Describe(Director->GetEmissions())),
		CountOfCue(Director->GetEmissions(), EStratSoundCue::TurnEnded), 1);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A SKIP IS SILENT.
//
// WHY THE RELEASE MUST MAKE NO NOISE, AND IT IS A SPECIFIC AND UGLY FAILURE. During a tour a
// killed unit is HIDDEN by `ApplyTourExistenceAtCursor` and DESTROYED later by
// `EndAiPlaybackTour`, so the death cue is placed at the step that SHOWS the death. Put it in
// the release's destroy loop instead -- which is where a reader tidying the audio code would
// naturally reach for it, because that is where the actors actually die -- and pressing skip
// plays every death of the hand-over at once, as a single burst. Ferrum Crossing's hand-overs
// kill several units.
//
// THE CONTROL IS THAT THE DESTROY LOOP GENUINELY RAN, and it is derived rather than asserted
// from a private flag. The units the hand-over killed are RETAINED as corpses while the tour is
// held -- the departed set has actors before the skip -- and are destroyed by the release. So
// the clause takes the departed population as a roster DIFFERENCE off two readings of
// `GetViewModel()`, requires at least one departed unit still to have an actor before the skip,
// and requires it to have none after. Only then does it read the emission list. Without that
// the silence would be green over a release that did nothing at all.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundSkipIsSilentTest,
	"Stratocracy.StratPlay.GATE-AUDIO.ASkipIsSilent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundSkipIsSilentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	DeclareHandoverNoise(*this);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const TSet<int32> Before = AppliedUnitIds(*Match);
	if (!TestTrue(TEXT("CONTROL: the opening roster is non-empty"), Before.Num() > 0))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, Match->GetAiPlaybackStepCount()));

	const TSet<int32> After = AppliedUnitIds(*Match);

	TArray<int32> Departed;
	for (const int32 Id : Before)
	{
		if (!After.Contains(Id))
		{
			Departed.Add(Id);
		}
	}
	Departed.Sort();

	if (!TestTrue(TEXT("CONTROL: the hand-over killed at least one unit, so the release has "
	                   "corpses to destroy and the mutant has something to voice"),
			Departed.Num() > 0))
	{
		return false;
	}

	TArray<int32> Retained;
	for (const int32 Id : Departed)
	{
		if (Match->FindUnitActor(Id) != nullptr)
		{
			Retained.Add(Id);
		}
	}
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: %d of the %d departed units are still RETAINED as corpses while the "
			     "tour holds them, so the release's destroy loop has work to do"),
			Retained.Num(), Departed.Num()),
			Retained.Num() > 0))
	{
		return false;
	}

	// ---- the skip ---------------------------------------------------------------------
	Director->ResetEmissions();

	if (!TestTrue(TEXT("CONTROL: the skip was accepted"), Match->SkipAiPlayback()))
	{
		return false;
	}

	for (const int32 Id : Retained)
	{
		if (!TestNull(*FString::Printf(
				TEXT("CONTROL: the release destroyed retained corpse %d, so the destroy loop "
				     "really ran -- which is the loop the death cue must NOT be in"), Id),
				Match->FindUnitActor(Id)))
		{
			return false;
		}
	}

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: releasing the tour is SILENT -- the death cue lives at the step "
			     "that SHOWS the death, so a skip does not play %d deaths at once: %s"),
			Retained.Num(), *Describe(Director->GetEmissions())),
		Director->GetEmissions().Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A TOUR STEP VOICES ITS OWN EVENT, AND THE TOUR IS NOT SILENT.
//
// THE MUTANT THIS IS WRITTEN AGAINST, AND IT IS THE SHARPEST ONE IN THE FILE. The per-step
// cues sit AFTER `PlayMoveSlideForStep` in `AdvanceAiPlaybackOneStep`. Gate them on that
// function's RETURN -- which reads like an obvious tidy, since the sound belongs with the
// animation -- and the whole tour goes silent at `AStratUnitActor::MoveTweenSeconds <= 0`,
// which is the shipped default and is EVERY headless fixture. A move with no slide still
// happened. That mutant makes the total emission count zero and is red on this clause's first
// assertion.
//
// THE COUNTING RULE THAT MAKES THIS CLAUSE WORK, AND IT WAS MEASURED WRONG FIRST. Two DIFFERENT
// call sites emit during one `AdvanceAiPlaybackOneStep`: the three command cues from the switch
// this clause is about, and `UnitDestroyed` from `ApplyTourExistenceAtCursor`, which the same
// function calls and which the slide gate does NOT sit in front of. So a "the tour emitted
// something" assertion that counts all four kinds together is satisfied by the DEATH cues alone
// -- and this clause's first draft did exactly that and let the named mutant through:
// `M16-step-cues-gated-on-the-slide` was run in place and the suite came back 408/408, ZERO red.
// The counting below therefore separates the two populations and the "not silent" assertion
// counts only the THREE COMMAND CUES. That is the whole of the repair, it is recorded here
// rather than quietly fixed, and it is a fresh instance of a defect class this project already
// carries: a clause whose control was satisfied through a channel other than the one under test.
//
// THE FOUR PROPERTIES, EACH WITH THE MUTANT IT CATCHES.
//   (1) STEPPING THE WHOLE TOUR EMITS AT LEAST ONE COMMAND CUE. This is what kills the slide
//       gate. `AStratUnitActor::MoveTweenSeconds <= 0` IS the C++ default and is what every
//       headless fixture runs at, so `PlayMoveSlideForStep` refuses on EVERY step here and a
//       gate on its return silences all three arms at once.
//   (2) EVERY EMISSION IS ONE OF THE FOUR DIEGETIC KINDS. `TurnEnded`, `ButtonClick` and
//       `MatchEnded` are owned elsewhere -- the turn edge in particular is `ApplyView`'s, from
//       the one refresh that follows the whole hand-over, and voicing it per step would sound
//       one `TurnEnded` per AI TURN inside a tour the player experiences as a single hand-over.
//   (3) NO SINGLE STEP EMITS MORE THAN ONE COMMAND CUE, and AT LEAST ONE STEP EMITS NONE. The
//       second half is what pins the `default:` arm: §4.10 has four command kinds and `EndTurn`
//       is deliberately silent, so a mutant that added an `EndTurn` arm would leave no step
//       emitting none, and is red here.
//   (4) THE KINDS THE HAND-OVER ACTUALLY CONTAINED ARE THE KINDS THAT SOUND, and the expectation
//       is a ROSTER DIFFERENCE off two readings of `GetViewModel()` rather than a list this file
//       composed. A unit that APPEARED can only have arrived by a §2.7 Build, so a build cue
//       must have sounded; a unit that DEPARTED can only have gone by combat, so an attack cue
//       must have sounded. Each half is asserted only when its population is non-empty, so the
//       clause never asserts about the empty set.
//
// WHAT THIS CLAUSE STILL DOES **NOT** PIN, STATED RATHER THAN LEFT TO BE DISCOVERED: that each
// INDIVIDUAL step sounds its own kind. Property (4) is a statement about the tour as a whole, so
// a mutant that swapped the Move and Attack arms would still produce both kinds somewhere and is
// invisible here. Pinning it per step needs a step's KIND, and `UStratMatchSubsystem::AiPlaybackReel`
// is private with no accessor -- `GetAiPlaybackStepCount` and `GetAiPlaybackCursor` are counts,
// not contents. `Tools/architect/state/tests.md` records the seam that would close it.
//
// AND THE BUILD CUE'S `UnitId` IS DELIBERATELY NOT ASSERTED. A §4.10 Build command carries a
// `defIndex` in its `unitId` field -- `.agents/ue-project-context.md` states that and
// `strat::applyCommand` indexes with it -- so the id on a build step is a UNIT DEFINITION and not
// a unit. Comparing it against a roster id would be comparing two different kinds of number.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundStepVoicesItsOwnEventTest,
	"Stratocracy.StratPlay.GATE-AUDIO.AStepVoicesItsOwnEventAndTheTourIsNotSilent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundStepVoicesItsOwnEventTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	DeclareHandoverNoise(*this);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// THE PRE-HAND-OVER ROSTER, for property (4)'s expectation.
	const TSet<int32> Before = AppliedUnitIds(*Match);
	if (!TestTrue(TEXT("CONTROL: the opening roster is non-empty"), Before.Num() > 0))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	const int32 StepCount = Match->GetAiPlaybackStepCount();
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, StepCount));

	if (!TestTrue(TEXT("CONTROL: the hand-over recorded a reel to step through"), StepCount > 0))
	{
		return false;
	}

	const TSet<int32> After = AppliedUnitIds(*Match);

	int32 Appeared = 0;
	for (const int32 Id : After)
	{
		if (!Before.Contains(Id))
		{
			++Appeared;
		}
	}
	int32 Departed = 0;
	for (const int32 Id : Before)
	{
		if (!After.Contains(Id))
		{
			++Departed;
		}
	}
	if (!TestTrue(TEXT("CONTROL: and the tour is running"), Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	Director->ResetEmissions();

	int32 StepsTaken        = 0;
	int32 StepsWithNoOwnCue = 0;
	int32 WorstPerStep      = 0;

	// COUNTED SEPARATELY FROM THE DEATH CUES ON PURPOSE -- see the block above, where counting
	// them together is recorded as having let the named mutant through.
	int32 CommandCues = 0;
	int32 MoveCues    = 0;
	int32 AttackCues  = 0;
	int32 BuildCues   = 0;
	int32 DeathCues   = 0;

	// THE GUARD IS THE REEL'S OWN LENGTH PLUS A MARGIN, so a stepping bug is a bounded failure
	// rather than a hung suite.
	for (int32 Guard = 0; Guard < StepCount + 2; ++Guard)
	{
		const int32 Mark = Director->GetEmissions().Num();
		if (!Match->AdvanceAiPlaybackOneStep())
		{
			break;
		}
		++StepsTaken;

		int32 OwnCues = 0;
		for (int32 Index = Mark; Index < Director->GetEmissions().Num(); ++Index)
		{
			const FStratSoundEmissionRecord& Record = Director->GetEmissions()[Index];

			// ---- (2) ----------------------------------------------------------------
			const bool bDiegetic =
				Record.Cue == EStratSoundCue::UnitMoved ||
				Record.Cue == EStratSoundCue::UnitAttacked ||
				Record.Cue == EStratSoundCue::FactoryBuiltUnit ||
				Record.Cue == EStratSoundCue::UnitDestroyed;

			if (!TestTrue(*FString::Printf(
					TEXT("GATE-AUDIO: a tour step voices only what happened on it -- `%s` is "
					     "owned by a latch elsewhere and must not come from a step"),
					CueWord(Record.Cue)),
					bDiegetic))
			{
				return false;
			}

			switch (Record.Cue)
			{
			case EStratSoundCue::UnitMoved:        ++MoveCues;   ++OwnCues; break;
			case EStratSoundCue::UnitAttacked:     ++AttackCues; ++OwnCues; break;
			case EStratSoundCue::FactoryBuiltUnit: ++BuildCues;  ++OwnCues; break;
			default:                               ++DeathCues;             break;
			}
		}

		CommandCues += OwnCues;

		WorstPerStep = FMath::Max(WorstPerStep, OwnCues);
		if (OwnCues == 0)
		{
			++StepsWithNoOwnCue;
		}
	}

	AddInfo(FString::Printf(
		TEXT("stepped %d of %d recorded steps; %d command cues (%d move, %d attack, %d build) "
		     "and %d death cues; %d step(s) voiced no command cue; the busiest voiced %d; the "
		     "hand-over's roster gained %d unit(s) and lost %d"),
		StepsTaken, StepCount, CommandCues, MoveCues, AttackCues, BuildCues, DeathCues,
		StepsWithNoOwnCue, WorstPerStep, Appeared, Departed));

	if (!TestTrue(TEXT("CONTROL: the fixture actually stepped the tour"), StepsTaken > 0))
	{
		return false;
	}

	// ---- (1) ---------------------------------------------------------------------------
	// COUNTED OVER THE COMMAND CUES ALONE. `Director->GetEmissions().Num() > 0` was the first
	// draft's form and is satisfied by `ApplyTourExistenceAtCursor`'s death cues, which the
	// slide gate does not sit in front of -- measured, and recorded in the block above.
	TestTrue(
		TEXT("GATE-AUDIO: stepping the tour voices its COMMANDS -- the per-step cues do not gate "
		     "on `PlayMoveSlideForStep`'s return, which refuses at the C++ default "
		     "MoveTweenSeconds of zero on EVERY step here and would make the whole feature "
		     "unobservable in exactly the configuration the suite runs in"),
		CommandCues > 0);

	// ---- (3) ---------------------------------------------------------------------------
	TestTrue(*FString::Printf(
			TEXT("GATE-AUDIO: no step voices more than one command cue (the busiest voiced %d)"),
			WorstPerStep),
		WorstPerStep <= 1);

	TestTrue(
		TEXT("GATE-AUDIO: and at least one step voices NONE of them -- §4.10's EndTurn is "
		     "deliberately silent, because the turn edge is the ONE refresh after the whole "
		     "hand-over and voicing it per step would sound one TurnEnded per AI turn"),
		StepsWithNoOwnCue > 0);

	// ---- (4) ---------------------------------------------------------------------------
	// THE EXPECTATION IS A ROSTER DIFFERENCE AND NOT A LIST THIS FILE COMPOSED. Each half is
	// asserted only where its population is non-empty, so neither can be green over nothing.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the hand-over changed the roster (%d gained, %d lost), so at least "
			     "one of the two assertions below has content"),
			Appeared, Departed),
			Appeared > 0 || Departed > 0))
	{
		return false;
	}

	if (Appeared > 0)
	{
		TestTrue(*FString::Printf(
				TEXT("GATE-AUDIO: %d unit(s) arrived during the hand-over, and §2.7's Build is "
				     "the only thing in this project that creates one, so the tour voiced the "
				     "build cue (%d times)"),
				Appeared, BuildCues),
			BuildCues > 0);
	}

	if (Departed > 0)
	{
		TestTrue(*FString::Printf(
				TEXT("GATE-AUDIO: %d unit(s) left the roster during the hand-over, which takes "
				     "an attack, so the tour voiced the attack cue (%d times)"),
				Departed, AttackCues),
			AttackCues > 0);
	}

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- `MatchEnded` FIRES ONCE, AND FROM THE LATCH.
//
// WHY THIS CUE IS NOT THE DECIDER'S, WHICH IS THE WHOLE OF WHY IT NEEDS A LIVE CLAUSE. A match
// ending IS visible in the view model, so a `bHasResult` arm in `StratDecideSoundCues` would
// look reasonable. It would be a SECOND answer to "has this match already ended", and a
// concluded match refreshes many times -- so the cost of the second answer is not one extra
// sound, it is a victory sting on every mouse move for the rest of the session.
// `ConcludeMatchIfEnded` already owns a once-per-match latch, `bMatchConclusionAnnounced`, and
// the cue is sounded inside it.
//
// THE FIXTURE APPLIES A CONCLUDED MODEL TWICE, which is the minimum that separates "latched"
// from "fires on a result". The result is planted, and the plant is validated through the
// module's own predicate `StratMatchIsConcluded` rather than by reading the bool back -- that
// function is the one field the subsystem consults and is `STRATPLAY_API` for this reason.
//
// THE SCREEN'S REFUSAL IS NOT DECLARED, AND THAT WAS MEASURED RATHER THAN ASSUMED. A first
// pass declared `AddExpectedMessagePlain(TEXT("No end-of-match screen this match"), ...)` on
// `StratMatchResultTrigger.cpp`'s precedent and the clause went RED with *"Expected suppressed
// ('Warning') level log message or higher matching 'No end-of-match screen this match' did not
// occur"* -- because that declaration with `Occurrences 0` is itself an ASSERTION, and this
// fixture spawns no `AStratScoreboardHUD`, so nothing ever reaches the line that refuses. The
// declaration is therefore absent. This clause runs over the ordering `ConcludeMatchIfEnded`
// states -- the cue BEFORE the screen, so a match that ends with no screen is still audible --
// and this fixture is exactly the "no screen" half of it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundMatchEndedLatchTest,
	"Stratocracy.StratPlay.GATE-AUDIO.MatchEndedFiresOnceAndFromTheLatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundMatchEndedLatchTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
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

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const FStratViewModel Opening = Match->GetViewModel();
	if (!TestFalse(TEXT("CONTROL: the opening is not concluded, measured through the module's "
	                    "own predicate"),
			StratMatchIsConcluded(Opening)))
	{
		return false;
	}

	FStratViewModel Concluded = Opening;
	Concluded.Match.bHasResult = true;
	if (!TestTrue(TEXT("CONTROL: and the planted model IS concluded by that same predicate"),
			StratMatchIsConcluded(Concluded)))
	{
		return false;
	}

	Director->ResetEmissions();

	Match->ApplyView(Concluded);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a match reaching a result sounds `MatchEnded`: %s"),
			*Describe(Director->GetEmissions())),
			CountOfCue(Director->GetEmissions(), EStratSoundCue::MatchEnded), 1))
	{
		return false;
	}

	// THE SECOND APPLY IS THE CLAUSE. A concluded match refreshes on every mouse move.
	Match->ApplyView(Concluded);

	if (!TestTrue(TEXT("CONTROL: the second refresh's audio seam ran"),
			Director->GetApplyViewObservationCount() >= 2))
	{
		return false;
	}

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and it fires ONCE PER MATCH, from `bMatchConclusionAnnounced` -- "
			     "an emission outside that latch would sting on every refresh of a finished "
			     "match: %s"),
			*Describe(Director->GetEmissions())),
		CountOfCue(Director->GetEmissions(), EStratSoundCue::MatchEnded), 1);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A BUTTON CLICKS EVEN WHEN ITS VERB REFUSES.
//
// THE CLICK ACKNOWLEDGES THE INPUT AND NEVER THE OUTCOME, and that is the whole reason the
// emission sits at each verb's ENTRY, above every legality check and regardless of the return.
// A refused control that makes no sound reads as a DEAD control: the player learns nothing
// about whether the click registered and presses harder. Move the emission below the success
// check -- which is where it would land if someone treated it as feedback about the ACTION --
// and every refusal in the game becomes indistinguishable from a broken button.
//
// `RequestEndTurn` WITH NO MATCH IS THE CHEAPEST REFUSAL IN THE PROJECT and needs no board, no
// scenario and no bridge: `HandleSelectionEvent` finds no live match and returns false. So the
// clause asserts the REFUSAL and the CLICK in the same breath, which is the pairing that makes
// it a statement about placement rather than about either half alone.
//
// THE SIDE AND UNIT ARE ASSERTED ABSENT, ON THE CALL SITE'S OWN STATED CONTRACT. A click is
// about the interface and not about the match; inventing a side or a unit for it would put
// numbers in a record that nothing measured.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundRefusedButtonStillClicksTest,
	"Stratocracy.StratPlay.GATE-AUDIO.AButtonClickIsEmittedEvenWhenTheVerbRefuses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundRefusedButtonStillClicksTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundDirectorCallSite;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a sound director"), Director))
	{
		return false;
	}

	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("AStratPlayerController spawned"), Controller))
	{
		return false;
	}

	// NO MATCH IS STARTED, DELIBERATELY -- that is the refusal this clause needs.
	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match))
	{
		return false;
	}
	if (!TestFalse(TEXT("CONTROL: no match is live, so the verb below is certain to refuse"),
			Match->IsMatchLive()))
	{
		return false;
	}

	Director->ResetEmissions();

	FString    Reason;
	const bool bAccepted = Controller->RequestEndTurn(Reason);

	if (!TestFalse(*FString::Printf(
			TEXT("CONTROL: the verb REFUSED, which is the condition this clause is about "
			     "('%s')"), *Reason),
			bAccepted))
	{
		return false;
	}

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and the button clicked ANYWAY -- the click acknowledges the INPUT "
			     "and never the outcome, so a refusal must not read as a dead control: %s"),
			*Describe(Director->GetEmissions())),
			Director->GetEmissions().Num(), 1))
	{
		return false;
	}

	TestEqual(TEXT("GATE-AUDIO: it is the click cue"),
		static_cast<int32>(Director->GetEmissions()[0].Cue),
		static_cast<int32>(EStratSoundCue::ButtonClick));
	TestEqual(TEXT("GATE-AUDIO: recorded with the shipped disposition"),
		static_cast<int32>(Director->GetEmissions()[0].Disposition),
		static_cast<int32>(EStratSoundDisposition::NoBank));
	TestEqual(TEXT("GATE-AUDIO: a click is about the interface and names no side"),
		Director->GetEmissions()[0].Side, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("GATE-AUDIO: and no unit"),
		Director->GetEmissions()[0].UnitId, static_cast<int32>(INDEX_NONE));

	return true;
}
