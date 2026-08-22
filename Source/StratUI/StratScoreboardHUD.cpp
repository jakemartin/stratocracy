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
//   2. The constructor and destructor bodies -- BY CHOICE, and not because a deleter
//      here needs a complete type. `OwnedBridge` is a `TPimplPtr<FStratBridge>`, which
//      type-erases its deleter at the `MakePimpl` call site in `SeedBridge`, so no
//      destructor of this class ever needs `FStratBridge` complete; `= default` in the
//      header would compile today. They are kept out of line anyway because the
//      constructor has a body worth reading and a declared constructor/destructor pair
//      is easier to keep honest together than apart.
//
//      THAT IS NOT AN INVITATION TO RESTORE TUniquePtr. `TUniquePtr` instantiates its
//      deleter wherever the owning destructor is instantiated, and defining the
//      destructor down here does not cover every such site: UHT emits a vtable-helper
//      constructor into Module.StratUI.gen.cpp, a translation unit that includes
//      StratScoreboardHUD.h and never includes StratBridge.h, which no definition in
//      this file can reach. Measured as error C4150, "deletion of pointer to incomplete
//      type 'FStratBridge'", reported from UniquePtr.h and compiled from that .gen.cpp.
//      TPimplPtr is the fix; this arrangement never was one. The same account, from the
//      other end, sits on the destructor definition below.
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

#include "StratGuidanceWidget.h"

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

	/**
	 * The ViewingSide range check, in one place because it is now asked from three:
	 * `SeedBridge` (the owned path's setup), `BeginPlay` (the adopted path's setup, which
	 * skips SeedBridge) and `SetViewingSide` (every hot-seat hand-over).
	 *
	 * SIDE_COUNT is reachable in this file and not in the header -- item 3 of the header
	 * block. Reading a compile-time constant out of a vendored header is not the
	 * cross-module `strat::` CALL the constraint forbids: nothing is emitted and no symbol
	 * has to be exported from UnrealEditor-StratBridge.dll for it to link.
	 *
	 * STILL NOT THE AUTHORITY ON THE RANGE. `StratBuildScoreboardModel` checks the same
	 * thing against the snapshot's own side array, which sits nearer the data it indexes;
	 * this exists so a misconfiguration is a named refusal at the moment it is made rather
	 * than a refusal on every refresh forever after. If the two ever disagree, the builder
	 * is right.
	 */
	bool ViewingSideInRange(int32 Side, FString& OutFailureReason)
	{
		if (Side < 0 || Side >= static_cast<int32>(strat::SIDE_COUNT))
		{
			OutFailureReason = FString::Printf(
				TEXT("ViewingSide %d is outside [0, %d) -- it names which strat side is the \"YOU\" column"),
				Side, static_cast<int32>(strat::SIDE_COUNT));
			return false;
		}

		OutFailureReason.Reset();
		return true;
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

// Out of line, per item 2 of the header block, and NOT because a deleter is instantiated
// here -- it is not. `OwnedBridge` is a `TPimplPtr<FStratBridge>`, which type-erases its
// deleter at the `MakePimpl` call site in `SeedBridge` below, so no destructor of this
// class ever needs `FStratBridge` to be complete. That is the whole point of the
// TPimplPtr-not-TUniquePtr decision the header block measures as C4150: with TUniquePtr
// this destructor WAS load-bearing and still did not save the build, because UHT emits a
// vtable-helper constructor into Module.StratUI.gen.cpp, a translation unit that never
// sees StratBridge.h. Do not restore TUniquePtr on the strength of this definition
// existing.
//
// It is kept out of line anyway because a declared constructor and a declared destructor
// are easier to keep honest as a pair than as a lone one, and the constructor below has a
// body worth reading.
//
// DEFAULTED, AND THE TWO BRIDGE MEMBERS ARE WHY IT CAN BE. `OwnedBridge` (TPimplPtr) frees
// what it holds; `AdoptedBridge` is a raw observing pointer into somebody else's lifetime
// and this class frees NOTHING through it, here or anywhere. At most one is ever non-null.
// EndPlay does the same two acts earlier, where they are observable -- a reset and a clear
// -- so by the time this runs there is nothing left to release in a live world; this
// definition only covers a HUD destroyed without EndPlay having run.
AStratScoreboardHUD::~AStratScoreboardHUD() = default;

void AStratScoreboardHUD::BeginPlay()
{
	Super::BeginPlay();

	// A fresh attempt owns a fresh verdict. Carrying a previous reason forward would
	// let a stale sentence explain a new success.
	LastFailureReason.Reset();

	FString FailureReason;

	// AN ADOPTED BRIDGE IS NOT RESEEDED, and this branch is the whole of phase 3's path
	// through BeginPlay. Its owner -- a world subsystem, per this class's header block --
	// already mapped the tables and seeded the scenario into it, and running SeedBridge
	// here would allocate a SECOND `strat::GameState` for the same map and then draw from
	// whichever one won. The configured tables and scenario file are deliberately ignored
	// in this case rather than checked for agreement: they are this HUD's fallback
	// configuration, not a second opinion about the match somebody else is running.
	if (AdoptedBridge == nullptr && !SeedBridge(FailureReason))
	{
		LastFailureReason = FailureReason;

		// A half-loaded bridge must not be reachable through GetBridge(). Definitions
		// mapped but no scenario seeded is a state the projection refuses anyway, but
		// handing it out invites a caller to treat "not seeded" as "seeded and empty",
		// which is the exact confusion FStratBridge::MakeUiSnapshot refuses to permit.
		//
		// Only the OWNED member is reset. Reaching here means nothing was adopted, so
		// there is nothing borrowed to drop, and a reset of the adopted pointer would be
		// a line that only ever runs when it cannot matter.
		OwnedBridge.Reset();

		UE_LOG(LogStratUI, Error, TEXT("No scoreboard this session: %s"), *LastFailureReason);
		return;
	}

	// The ViewingSide range check normally happens inside SeedBridge, which the branch
	// above skips for an adopted bridge. It is re-asked here so that a misconfigured
	// Blueprint is a named refusal during setup on BOTH paths -- otherwise adopting would
	// quietly buy a HUD out of a check that the owned path fails loudly.
	if (!ViewingSideInRange(ViewingSide, FailureReason))
	{
		LastFailureReason = FailureReason;
		UE_LOG(LogStratUI, Error, TEXT("No scoreboard this session: %s"), *LastFailureReason);
		return;
	}

	// THE GUIDANCE STRIP IS CREATED BEFORE THE SCOREBOARD BLOCK, AND THE ORDER IS THE
	// WHOLE POINT. Everything below this line can return early -- an unset
	// `ScoreboardWidgetClass` returns, a failed create returns, a refused first refresh
	// returns -- and each of those is a statement about the SCOREBOARD. Sitting after any
	// of them would make GDD Sec 2.11.6's strip silently conditional on Sec 2.11.4's panel
	// being configured, which is two surfaces sharing one failure mode for no reason
	// either of them gives.
	//
	// IT IS AFTER THE BRIDGE AND VIEWING-SIDE CHECKS, though, and that IS a real
	// dependency: those two returns mean no match will be drawn at all, and a directive
	// strip over a board that never seeded would be instructing the player about a game
	// that is not running.
	if (GuidanceWidgetClass == nullptr)
	{
		// Logged at Log and not recorded in `LastFailureReason`. That member answers "why
		// is there no scoreboard", and overwriting it here would have a guidance sentence
		// explain a scoreboard that is about to come up perfectly well.
		UE_LOG(LogStratUI, Log,
			TEXT("No guided-opening strip requested: no GuidanceWidgetClass is set on this HUD's Blueprint defaults."));
	}
	else if (!CreateGuidanceWidget(FailureReason))
	{
		// NOT A RETURN. A missing strip is not a reason to abandon the scoreboard, and the
		// converse holds below. Reported at Warning rather than Error because the match
		// remains fully playable without it -- Sec 2.11.6's guidance is an opening aid,
		// and Sec 2.11.6-B's End Turn gate lives on the selection machine rather than on
		// this widget, so nothing becomes unreachable when the strip is absent.
		UE_LOG(LogStratUI, Warning, TEXT("No guided-opening strip this session: %s"), *FailureReason);
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

	// The two paths log differently because they are different situations, and a log line
	// naming a ScenarioFile this HUD never opened would send the next reader to the wrong
	// configuration to explain the wrong board.
	if (AdoptedBridge != nullptr)
	{
		UE_LOG(LogStratUI, Log,
			TEXT("Scoreboard live on an adopted bridge (this HUD seeded nothing), drawn for side %d."),
			ViewingSide);
	}
	else
	{
		UE_LOG(LogStratUI, Log,
			TEXT("Scoreboard live: seeded from '%s' (first side %d), drawn for side %d."),
			*ScenarioFile, FirstSide, ViewingSide);
	}
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

	// The strip goes down the same way and for the same reason. It holds no pointer into
	// the bridge either -- `PushGuidance` copies a reflected struct in -- so the order
	// between the two widgets is not load-bearing and is written out only so that a later
	// change which does introduce a dependency has an obvious place to be wrong in.
	if (GuidanceStrip != nullptr)
	{
		GuidanceStrip->RemoveFromParent();
		GuidanceStrip = nullptr;
	}

	// TWO MEMBERS, TWO DIFFERENT ACTS, and the asymmetry is the whole content of the
	// ownership split recorded in the header block.
	//
	// The OWNED bridge is destroyed here. The authoritative GameState dies with the map,
	// which is the lifetime it actually has and the reason this is an AHUD and not a
	// GameInstance subsystem.
	OwnedBridge.Reset();

	// The ADOPTED bridge is dropped, NOT destroyed. It belongs to whoever handed it over
	// -- phase 3's `UStratMatchSubsystem` -- and that owner outlives this actor. Deleting
	// it here would be a double free the moment the subsystem tore down its own copy, and
	// the two members are separate precisely so that this line cannot be written as a
	// delete by accident: there is no `delete` that could be spelled on a raw pointer this
	// class never allocated.
	AdoptedBridge = nullptr;

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
	if (!ViewingSideInRange(ViewingSide, OutFailureReason))
	{
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
	//
	// THE OWNED MEMBER, AND ONLY EVER THE OWNED MEMBER. This function is the one place in
	// the project that allocates a bridge for this HUD, which is what makes "OwnedBridge
	// is non-null exactly when this HUD allocated it" checkable by reading rather than by
	// hoping. BeginPlay does not call it at all when a bridge has been adopted.
	OwnedBridge = MakePimpl<FStratBridge>();
	FStratBridge* const Fresh = OwnedBridge.Get();

	// ROW ORDER IS LOAD-BEARING and is not this class's business -- `defIndex` indexes
	// the vectors the bridge builds from these tables, and a §4.10 Build command
	// carries a defIndex. The bridge's header records that the order is taken from the
	// table and then asserted equal to the headless loader's by the editor pass. This
	// call passes the configured assets through and asserts nothing about them.
	const FStratResult Loaded = Fresh->LoadDefinitions(UnitTable, TerrainTable);
	if (!Loaded.bOk)
	{
		OutFailureReason = DescribeRefusal(TEXT("LoadDefinitions"), Loaded);
		return false;
	}

	const FString ScenarioPath = ResolveScenarioPath(ScenarioFile);

	const FStratResult Seeded = Fresh->LoadScenarioFromFile(ScenarioPath, FirstSide);
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

bool AStratScoreboardHUD::CreateGuidanceWidget(FString& OutFailureReason)
{
	// Same shape as CreateScoreboardWidget above, deliberately, so that a reader who has
	// understood one has understood both. Unset is handled by BeginPlay as a
	// configuration rather than an error; reaching here with it null means someone called
	// this directly, and that IS an error.
	if (GuidanceWidgetClass == nullptr)
	{
		OutFailureReason = TEXT("GuidanceWidgetClass is unset");
		return false;
	}

	APlayerController* const OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer == nullptr)
	{
		OutFailureReason = TEXT("this HUD has no owning player controller to parent the guidance strip to");
		return false;
	}

	UStratGuidanceWidget* const Created =
		CreateWidget<UStratGuidanceWidget>(OwningPlayer, GuidanceWidgetClass);
	if (Created == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("CreateWidget returned null for GuidanceWidgetClass '%s'"),
			*GetNameSafe(GuidanceWidgetClass));
		return false;
	}

	Created->AddToViewport(GuidanceZOrder);

	// Assigned only after it is on screen, so `GuidanceStrip != nullptr` and "there is a
	// strip" never disagree.
	GuidanceStrip = Created;

	// DELIBERATELY NOT REFRESHED FROM A SOURCE HERE, and the reason differs from the
	// scoreboard's. The scoreboard could be refreshed at this point and is not, so that two
	// failures stay distinguishable. This one CANNOT be: the value it wants is a field of a
	// view model that only exists once `UStratMatchSubsystem` has seeded a match and built
	// one, and this HUD has no way to ask for that without becoming a second thing that runs
	// matches. All of that is unchanged.
	//
	// WHAT IS CHANGED IS THE SENTENCE THAT USED TO FOLLOW IT, retracted in place:
	// RETRACTED> "The strip draws its own defaults until the first `ApplyView`."
	// It assumed a LATER `ApplyView`, and there is not one. Measured in five fresh PIE
	// sessions on 2026-08-21: `AStratPlayerController::BeginPlay` reached `ApplyView` with the
	// session's only decorated model before this function had run, every time, and nothing
	// reconciles again without player input -- so the strip drew its own defaults for the
	// whole session and Sec 2.11.6's opening was never seen.
	//
	// SO THE NEW STRIP IS BROUGHT UP TO THE LAST VALUE THIS HUD WAS HANDED, if it has been
	// handed one. That is the same act `UStratMatchSubsystem::ApplyView` performs for a unit
	// actor it has just spawned: a surface appearing mid-flight is reconciled to the latest
	// model rather than left blank until something else happens to move. It assumes NO
	// ordering -- a push arriving after this point takes the ordinary route through
	// `PushGuidance`, one that arrived before it is delivered here, and a session with no push
	// at all delivers nothing and leaves the strip on its defaults.
	DeliverLatestGuidance();

	return true;
}

void AStratScoreboardHUD::PushGuidance(const FStratGuidanceView& InGuidance)
{
	// RECORDED FIRST, UNCONDITIONALLY, AND WITHOUT COMPARING. The strip may not exist yet --
	// at match start it does not, which is the whole of the first delivery defect -- so the
	// value is kept before the forward rather than only when the forward can happen. Storing
	// it on both branches is what makes `DeliverLatestGuidance` able to say the same thing
	// later that this call would have said now.
	//
	// NO EQUALITY TEST AGAINST THE STORED VALUE, and no early-out on "unchanged". That would
	// be the delta-shaped thinking `UStratMatchSubsystem::ApplyView` refuses at the other end
	// of this same call, arrived at from the widget's side.
	LastPushedGuidance  = InGuidance;
	bGuidanceEverPushed = true;

	// THE REST IS A NULL CHECK AND A FORWARD, and the header records why that is the design:
	// no strip is a configuration, not a refusal, so there is nothing to report and nothing
	// to log. A log line here would fire once per reconcile for the whole of a session that
	// was configured exactly as intended.
	//
	// NO BRANCH ON THE VALUE. This function does not read `InGuidance.bActive`, does not
	// show or hide the widget, and does not decide anything about the strip's appearance.
	// `FStratGuidanceView` carries `bActive` precisely so the widget can bind visibility
	// to it; a `RemoveFromParent` here would be this file forming a second opinion about
	// when guidance is over, and Sec 2.11.6's window closing is not this class's fact.
	if (GuidanceStrip != nullptr)
	{
		GuidanceStrip->PushGuidance(InGuidance);
	}
}

bool AStratScoreboardHUD::DeliverLatestGuidance()
{
	// TWO CONDITIONS, AND THEY ARE DIFFERENT QUESTIONS. `GuidanceStrip` answers "is there
	// anywhere to deliver to"; `bGuidanceEverPushed` answers "is there anything to deliver".
	// The second cannot be read off `LastPushedGuidance` itself -- a default-constructed
	// `FStratGuidanceView` is a real state and not an absence, see the member's declaration --
	// and delivering one anyway would fire `OnGuidanceRefreshed` at a Widget Blueprint to
	// announce a reconcile that never happened.
	if (GuidanceStrip == nullptr || !bGuidanceEverPushed)
	{
		return false;
	}

	// THE FLAG IS NOT CLEARED. This is a replay of the current value, not the consumption of
	// a queued event; see the declaration on why idempotence is the property wanted here.
	GuidanceStrip->PushGuidance(LastPushedGuidance);
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
	//
	// ASKED THROUGH GetBridge() AND NOT OFF A MEMBER, so an adopted bridge counts as
	// live exactly as an owned one does. Liveness is a question about whether there is a
	// seeded match to draw; whose match it is has no bearing on it.
	const FStratBridge* const Live = GetBridge();

	return Live != nullptr
		&& Live->IsSeeded()
		&& Scoreboard != nullptr
		&& Scoreboard->IsInViewport();
}

bool AStratScoreboardHUD::AdoptBridge(FStratBridge& InBridge, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// REFUSED BEFORE ANYTHING IS RELEASED, which is what makes the failure path leave this
	// HUD exactly as it found it. Checking first and mutating second is the same
	// all-or-nothing construction StratScoreboardWidget.cpp uses on its model, and it
	// matters more here: a refusal that had already reset OwnedBridge would leave a HUD
	// with no bridge at all and no way back to the one it destroyed.
	//
	// An unseeded bridge is refused in this class's own words rather than being allowed
	// through to fail on every refresh forever after. See the declaration.
	if (!InBridge.IsSeeded())
	{
		OutFailureReason = TEXT(
			"the offered bridge is not seeded -- seed the scenario into it before handing it over, "
			"so that \"nothing is loaded\" cannot be drawn as \"a match in which nothing has happened\"");
		return false;
	}

	// Adopting the bridge this HUD is already reading is a no-op and is reported as
	// success, not as an error. A subsystem that hands the same bridge to the same HUD
	// twice -- on a re-entered BeginPlay, on a seamless travel, on a retry -- has done
	// nothing wrong, and the alternative is a refusal a caller would have to special-case.
	if (GetBridge() == &InBridge)
	{
		return true;
	}

	// THE ORDER HERE IS THE WHOLE OWNERSHIP TRANSITION AND IS NOT INTERCHANGEABLE.
	//
	// The owned bridge is destroyed FIRST. Nothing reads it after this line -- GetBridge()
	// is about to prefer the adopted pointer -- and holding it alive underneath would leak
	// a whole `strat::GameState` per handover, with nothing to point at when someone
	// noticed. Reaching here means either OwnedBridge or AdoptedBridge was set, never
	// both, so exactly one of these two lines does anything.
	OwnedBridge.Reset();
	AdoptedBridge = &InBridge;

	// DELIBERATELY NOT REFRESHED. See the declaration: adopting is a change of source, not
	// of state, and this class does not refresh on its own initiative anywhere else
	// either. The panel keeps showing the last model it successfully built until whoever
	// performed the handover asks for a rebuild.
	UE_LOG(LogStratUI, Log,
		TEXT("Bridge adopted: this HUD now draws a match it does not own and will not destroy."));

	return true;
}

bool AStratScoreboardHUD::SetViewingSide(int32 InViewingSide, FString& OutFailureReason)
{
	// Range-checked before the assignment, so a refused call changes nothing at all. A
	// clamp was the rejected alternative and it is the worst available behaviour in a
	// hot-seat game: it renders a coherent, correct-looking screen for the wrong player.
	if (!ViewingSideInRange(InViewingSide, OutFailureReason))
	{
		return false;
	}

	// THE ENTIRE MUTATION. One int32 on this actor. No command is submitted, the bridge is
	// not touched, and nothing the rules module holds can change as a result -- a view of
	// a match is not a move in it. That property is what lets a turn loop call this at the
	// hand-over without the hand-over becoming part of the match.
	ViewingSide = InViewingSide;

	// Nothing to rebuild is not a failure. A HUD configured to seed a bridge without
	// putting a panel on screen (a legitimate configuration -- see BeginPlay) stores the
	// new side, and the next successful refresh, if there ever is one, uses it.
	if (Scoreboard == nullptr)
	{
		return true;
	}

	// PRESENTATION IS REBUILT, NOT PATCHED. The scoreboard is not told "the side changed";
	// it is rebuilt from the view the bridge projects for the new side, which is the same
	// path every other refresh takes. There is no second code path here that could drift
	// from the first.
	//
	// A refusal here reports the refresh's failure and does NOT roll the side back. The
	// declaration records why: the widget leaves its Model alone on refusal, so the panel
	// still shows the previous side's correct standings, and a later retry gets the new
	// side without anyone having to set it twice. Rolling back would make a failed
	// hand-over silently stay with the previous player, which is the one outcome a
	// hot-seat game must not produce quietly.
	return RefreshScoreboard(OutFailureReason);
}

int32 AStratScoreboardHUD::GetViewingSide() const
{
	// ONE READ AND NOTHING ELSE, and the absence of everything else is the content of this
	// function. No range check on the way out: `ViewingSide` is an EditDefaultsOnly
	// property, so a Blueprint can set it out of range without going through
	// `SetViewingSide` at all, and BeginPlay is where that is caught and named. Clamping
	// here would hide a misconfiguration behind a plausible number and would also break the
	// one property this reader exists to make testable -- that a refused `SetViewingSide`
	// leaves the stored side EXACTLY as it was, whatever it was.
	//
	// It could have been an inline `FORCEINLINE` in the header, like `GetBridge()`. It is
	// defined here instead because it is a UFUNCTION and `GetBridge()` cannot be one: the
	// reflected ones are kept together on this side of the include so that a later reader
	// looking for "what Blueprint can call" finds them in one file.
	return ViewingSide;
}
