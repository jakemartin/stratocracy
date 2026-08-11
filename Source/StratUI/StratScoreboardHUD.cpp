// GDD §4.9 part 2 / §4.7 Stub 8 / §2.11.4 -- the runtime host, implementation side.
//
// WHAT IS HERE THAT COULD NOT BE IN THE HEADER, which is the whole reason this file
// has a comment block of its own:
//
//   1. `#include "StratBridge.h"`. StratScoreboardHUD.h declares a UCLASS, so UHT
//      parses it, and StratBridge.h pulls in the vendored `strat` headers -- exactly
//      the thing StratBridge.h's own note ("declares no reflected types on purpose")
//      exists to keep out of the header tool's way. A .cpp is not parsed by UHT, so
//      this is the one place in this class where the include is legal. It must not
//      migrate upward.
//
//   2. The constructor and destructor bodies. `TUniquePtr` instantiates its deleter
//      where the destructor is DEFINED, and a deleter cannot be instantiated against
//      an incomplete type -- so both are declared in the header and land here, after
//      the include. Writing `= default` on either declaration in the header breaks the
//      build with an error that points at TUniquePtr instead of at this arrangement.
//
//   3. The `ViewingSide` range check. The header records that it cannot name
//      SIDE_COUNT without the forbidden include; this file can, because it has the
//      include. Reading a compile-time constant out of a vendored header is NOT the
//      cross-module `strat::` call that constraint forbids -- nothing is emitted and
//      no symbol has to be exported from UnrealEditor-StratBridge.dll for it to link.
//      Every actual entry point below goes through a method on `FStratBridge`, which
//      is the only thing that DLL exports.
//
// THE SEQUENCE, and why it is four separate steps rather than one setup function.
// Seed the bridge, create the widget, add it to the viewport, refresh it. Each step
// fails with the refusing layer's own words, so a log line names which of the four
// went wrong. A single `Setup()` returning one bool would collapse "DT_Units is not
// assigned" and "the WBP asset was deleted" into the same sentence, and those have
// different fixes.
//
// WHY A FAILED FIRST REFRESH TAKES THE WIDGET BACK DOWN. §2.11.4 exists because of
// revision §1.5-#1 -- the tiebreak must never be a hidden win condition -- and
// "both sides have destroyed nothing" is a REAL state with a banner and a draw
// attached to it. UStratScoreboardWidget::Refresh is documented to leave `Model`
// untouched on refusal precisely so a transient failure cannot blank correct
// standings; on the FIRST refresh, untouched means the default-constructed model,
// which is a panel of zeroes. Leaving that on screen would counterfeit the passivity
// state and defeat the widget's refusal from above, so the host removes the widget
// instead. This is the one place the host makes a display decision, and it is a
// decision to display nothing.
//
// NOTHING BELOW IS A GAMEPLAY VALUE OR AN ASSET PATH. The tables, the scenario file
// name, the first side and the viewing side are read from the EditDefaultsOnly
// properties; the turn cap, the objective total and both sides' standings arrive from
// the scenario and the tables through the bridge's projection and are never named
// here. Source/StratUI/Tests/StratScoreboardParity.cpp keeps the same property for the
// same reason -- a scenario edit must not require a C++ edit -- and this file resolves
// its scenario path the way that pass does, against FPaths::ProjectDir(), so the
// running game and the gate open the same file when configured to.

#include "StratScoreboardHUD.h"

#include "StratScoreboardWidget.h"
#include "StratUI.h"

// See item 1 of the block above. This line is legal here and nowhere else in this
// class.
#include "StratBridge.h"

#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Paths.h"

namespace
{
	/**
	 * Renders a bridge refusal as one log-ready sentence, tag included when the rules
	 * module supplied one.
	 *
	 * THE REASON IS FORWARDED VERBATIM and is never replaced with a friendlier
	 * sentence of this module's own. FStratBridge's header states that every refusal it
	 * returns is a refusal the rules module returned; rewording it here would put this
	 * file's guess about the cause in front of the failing layer's own account of it,
	 * which is how a diagnostic starts lying. Only the surrounding context -- which
	 * call refused, over which file -- is added.
	 */
	FString DescribeRefusal(const FString& Call, const FStratResult& Result)
	{
		if (Result.Id.IsEmpty())
		{
			return FString::Printf(TEXT("%s refused: %s"), *Call, *Result.Reason);
		}
		return FString::Printf(TEXT("%s refused [%s]: %s"), *Call, *Result.Id, *Result.Reason);
	}

	/**
	 * Turns the configured scenario string into a path the bridge can open.
	 *
	 * RELATIVE TO THE PROJECT DIRECTORY, not to Content/. The shipped scenario is a
	 * plain file under Data/ rather than a cooked asset, and the Automation pass reaches
	 * it as FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), ...). Resolving against
	 * a different root here would let the game and the gate read different files while
	 * both looked correctly configured.
	 *
	 * THE SUBDIRECTORY IS NOT PASTED ON HERE. The property carries the whole relative
	 * path ("Data/ferrum_crossing.json") because "Data" is as much a layout decision as
	 * the file name is, and half a path in C++ is the kind of hardcoded path that
	 * survives a review by being too small to notice.
	 *
	 * No existence check. Whether the file can be read is the bridge's answer to give,
	 * and asking first would be a second opinion that can also race the actual open.
	 */
	FString ResolveScenarioPath(const FString& Configured)
	{
		if (FPaths::IsRelative(Configured))
		{
			return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), Configured));
		}
		return Configured;
	}
}

AStratScoreboardHUD::AStratScoreboardHUD()
{
	// NO TICK. The header records that reacting to state changes automatically is not
	// in this round: there is no change notification to subscribe to, and a tick-driven
	// poll would make the scoreboard's update rate a frame-rate property instead of a
	// turn property. Refreshes are requested, by whoever accepted the command.
	//
	// This HUD also draws no canvas of its own -- §2.11.4's panel is UMG -- so nothing
	// else here wants the tick either.
	PrimaryActorTick.bCanEverTick = false;
}

// Out of line and with a body, per item 2 of the header block: this is where
// TUniquePtr<FStratBridge> instantiates its deleter, and FStratBridge is complete only
// below the include above. The body is empty because TUniquePtr and TObjectPtr each
// release what they hold; EndPlay does the same work earlier, where it is observable.
AStratScoreboardHUD::~AStratScoreboardHUD() = default;

void AStratScoreboardHUD::BeginPlay()
{
	Super::BeginPlay();

	// A fresh attempt owns a fresh verdict. Carrying a previous reason forward would
	// let a stale sentence explain a new success.
	LastFailureReason.Reset();

	FString FailureReason;

	if (!SeedBridge(FailureReason))
	{
		LastFailureReason = FailureReason;

		// A half-loaded bridge must not be reachable through GetBridge(). Definitions
		// mapped but no scenario seeded is a state the projection refuses anyway, but
		// handing it out invites a caller to treat "not seeded" as "seeded and empty",
		// which is the exact confusion FStratBridge::MakeUiSnapshot refuses to permit.
		Bridge.Reset();

		UE_LOG(LogStratUI, Error, TEXT("No scoreboard this session: %s"), *LastFailureReason);
		return;
	}

	if (ScoreboardWidgetClass == nullptr)
	{
		// LEGITIMATE CONFIGURATION, NOT A FAILURE -- the header says so about this
		// property. A level that wants the bridge seeded but no panel on screen leaves
		// the class unset. The reason is still recorded, because "why is there no
		// scoreboard" is a question someone asks later and a Log line is gone by then;
		// it is logged at Log rather than Error because nothing went wrong.
		LastFailureReason = TEXT("no ScoreboardWidgetClass is set on this HUD's Blueprint defaults");
		UE_LOG(LogStratUI, Log, TEXT("Bridge seeded; no scoreboard requested: %s"), *LastFailureReason);
		return;
	}

	if (!CreateScoreboardWidget(FailureReason))
	{
		LastFailureReason = FailureReason;
		UE_LOG(LogStratUI, Error, TEXT("No scoreboard this session: %s"), *LastFailureReason);
		return;
	}

	// The first refresh is the one that decides whether a panel stays up. See the
	// header block on why a refusal here removes the widget rather than leaving a
	// default-constructed model visible.
	if (!RefreshScoreboard(FailureReason))
	{
		LastFailureReason = FailureReason;

		if (Scoreboard != nullptr)
		{
			Scoreboard->RemoveFromParent();
			Scoreboard = nullptr;
		}

		UE_LOG(LogStratUI, Error,
			TEXT("Scoreboard created but never had data, so it was taken back down: %s"),
			*LastFailureReason);
		return;
	}

	UE_LOG(LogStratUI, Log,
		TEXT("Scoreboard live: seeded from '%s' (first side %d), drawn for side %d."),
		*ScenarioFile, FirstSide, ViewingSide);
}

void AStratScoreboardHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Torn down explicitly rather than left to destruction order. The widget holds no
	// pointer into the bridge -- UStratScoreboardWidget::Refresh takes it by reference
	// per call and copies engine-typed values out of the snapshot -- so the ORDER here
	// is not load-bearing. It is written out anyway so that a later change which does
	// introduce a dependency has an obvious place to be wrong in, instead of being
	// silently correct until the destructor order shifts.
	if (Scoreboard != nullptr)
	{
		Scoreboard->RemoveFromParent();
		Scoreboard = nullptr;
	}

	// The authoritative GameState dies with the map, which is the lifetime it actually
	// has and the reason this is an AHUD and not a GameInstance subsystem.
	Bridge.Reset();

	// The situation the reason described is over with the world.
	LastFailureReason.Reset();

	Super::EndPlay(EndPlayReason);
}

bool AStratScoreboardHUD::SeedBridge(FString& OutFailureReason)
{
	// SIDE_COUNT is reachable here and not in the header -- item 3 of the header block.
	//
	// This check duplicates one the model builder also makes, and that is deliberate
	// rather than overlooked: checking here turns a misconfigured Blueprint into a
	// named refusal during setup instead of a refusal on every refresh forever after.
	// It is NOT a second authority on the range. If the two ever disagree, the builder
	// is right, because it sits nearer the data it indexes.
	if (ViewingSide < 0 || ViewingSide >= static_cast<int32>(strat::SIDE_COUNT))
	{
		OutFailureReason = FString::Printf(
			TEXT("ViewingSide %d is outside [0, %d) -- it names which strat side is the \"YOU\" column"),
			ViewingSide, static_cast<int32>(strat::SIDE_COUNT));
		return false;
	}

	// FirstSide is NOT range-checked here. Replay.h states that no rule in this project
	// decides which side moves first, so seeding owns whatever it will accept; checking
	// it here would make this file a second author of a decision the rules module
	// declined to make. An out-of-range value comes back as LoadScenarioFromFile's own
	// refusal below, in its words.

	if (UnitTable == nullptr || TerrainTable == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("definition tables are not assigned on this HUD's Blueprint defaults (UnitTable %s, TerrainTable %s)"),
			UnitTable == nullptr ? TEXT("unset") : TEXT("set"),
			TerrainTable == nullptr ? TEXT("unset") : TEXT("set"));
		return false;
	}

	if (ScenarioFile.IsEmpty())
	{
		OutFailureReason = TEXT("ScenarioFile is empty on this HUD's Blueprint defaults");
		return false;
	}

	// Constructed here rather than in the constructor: a bridge that exists before its
	// inputs have been checked is a bridge GetBridge() could hand out unseeded, and
	// BeginPlay is where the failure path can say why it did not survive.
	Bridge = MakePimpl<FStratBridge>();

	// ROW ORDER IS LOAD-BEARING and is not this class's business -- `defIndex` indexes
	// the vectors the bridge builds from these tables, and a §4.10 Build command
	// carries a defIndex. The bridge's header records that the order is taken from the
	// table and then asserted equal to the headless loader's by the editor pass. This
	// call passes the configured assets through and asserts nothing about them.
	const FStratResult Loaded = Bridge->LoadDefinitions(UnitTable, TerrainTable);
	if (!Loaded.bOk)
	{
		OutFailureReason = DescribeRefusal(TEXT("LoadDefinitions"), Loaded);
		return false;
	}

	const FString ScenarioPath = ResolveScenarioPath(ScenarioFile);

	const FStratResult Seeded = Bridge->LoadScenarioFromFile(ScenarioPath, FirstSide);
	if (!Seeded.bOk)
	{
		// The resolved path is added to the sentence because the configured string and
		// the file actually opened differ by a root, and a "not found" without the
		// resolved path sends the reader looking in the wrong directory.
		OutFailureReason = FString::Printf(TEXT("%s (resolved path: %s)"),
			*DescribeRefusal(TEXT("LoadScenarioFromFile"), Seeded), *ScenarioPath);
		return false;
	}

	return true;
}

bool AStratScoreboardHUD::CreateScoreboardWidget(FString& OutFailureReason)
{
	// Unset is handled by BeginPlay as a configuration, not an error; reaching here with
	// it null means someone called this directly, and that IS an error.
	if (ScoreboardWidgetClass == nullptr)
	{
		OutFailureReason = TEXT("ScoreboardWidgetClass is unset");
		return false;
	}

	// The owning player is the widget's outer, so the panel belongs to this local
	// player rather than to the world. A scoreboard is client-local and per-player --
	// the third of the header block's reasons for this being an AHUD at all.
	APlayerController* const OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer == nullptr)
	{
		OutFailureReason = TEXT("this HUD has no owning player controller to parent the widget to");
		return false;
	}

	UStratScoreboardWidget* const Created =
		CreateWidget<UStratScoreboardWidget>(OwningPlayer, ScoreboardWidgetClass);
	if (Created == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("CreateWidget returned null for ScoreboardWidgetClass '%s'"),
			*GetNameSafe(ScoreboardWidgetClass));
		return false;
	}

	// Z-order is presentation and comes from the property, so a later overlay can be
	// layered over or under this panel without touching this file.
	Created->AddToViewport(ScoreboardZOrder);

	// Assigned only after it is on screen, so `Scoreboard != nullptr` and "there is a
	// panel" never disagree -- IsScoreboardLive() below reports on exactly that.
	Scoreboard = Created;

	// DELIBERATELY NOT REFRESHED HERE. The header says so: BeginPlay refreshes once
	// afterwards, so "the widget was created" and "the widget has data" fail separately
	// and the log names which of the two went wrong.
	return true;
}

bool AStratScoreboardHUD::RefreshScoreboard(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	FStratBridge* const Live = GetBridge();
	if (Live == nullptr)
	{
		// The ROOT reason is forwarded when there is one. A caller told only "there is
		// no bridge" would have to go find the BeginPlay log to learn that DT_Units was
		// unassigned; the sentence that explains it already exists, so hand it back.
		if (LastFailureReason.IsEmpty())
		{
			OutFailureReason = TEXT("there is no bridge: this HUD's setup has not run");
		}
		else
		{
			OutFailureReason = LastFailureReason;
		}
		return false;
	}

	if (Scoreboard == nullptr)
	{
		if (LastFailureReason.IsEmpty())
		{
			OutFailureReason = TEXT("there is no scoreboard widget to refresh");
		}
		else
		{
			OutFailureReason = LastFailureReason;
		}
		return false;
	}

	// Whether the bridge is seeded is NOT checked here. FStratBridge::MakeUiSnapshot
	// refuses an unseeded bridge in its own words, and StratBuildScoreboardModel carries
	// those words out unchanged -- re-asking the question here would either duplicate
	// that judgement or, worse, answer it differently.
	//
	// LastFailureReason is left alone on a refusal from this point on, and that is the
	// distinction IsScoreboardLive() exists to expose: a refused refresh over a live
	// scoreboard is a different situation from a HUD that never came up, and
	// overwriting the setup verdict with a transient one would erase the second.
	return Scoreboard->Refresh(*Live, ViewingSide, OutFailureReason);
}

bool AStratScoreboardHUD::IsScoreboardLive() const
{
	// All three clauses, because each answers a different half of "is there something
	// to project and somewhere to put it": a bridge that exists but never seeded has
	// nothing to project, and a widget that was removed from the viewport has nowhere
	// to put it even though the pointer is still valid.
	return Bridge.IsValid()
		&& Bridge->IsSeeded()
		&& Scoreboard != nullptr
		&& Scoreboard->IsInViewport();
}
