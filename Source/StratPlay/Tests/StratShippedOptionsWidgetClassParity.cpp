// `OptionsWidgetClass` ACROSS THE THREE SHIPPED GameMode BLUEPRINTS, under `GATE-TITLEMENU`.
//
// THE ID IS RULED, ON DIRECT PRECEDENT RATHER THAN BY ANALOGY.
// `Tools/architect/state/global.md`'s third acceptance-ID ruling of 2026-09-05:
// *"`Stratocracy.StratPlay.GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot` ...
// already rides `GATE-TITLEMENU` for the identical shape -- do the shipped GameMode Blueprints
// agree on one CDO default -- for `SaveSlotName`, a field with nothing to do with a screen or a
// route either. The precedent is 'cross-shipped-GameMode-default parity, when no other ID names
// the field'."* It further records that `GATE-AUDIO-SETTINGS`'s three named facets *"do not
// include a GameMode-default parity fact at all"*. No name is minted.
//
// ---------------------------------------------------------------------------------------
// THE DEBT THIS DISCHARGES, IN THE WORDS IT WAS RECORDED IN.
//
// `AStratShellGameMode::OptionsWidgetClass`'s own declaration states it: *"THE SAME ASSET IS
// NAMED BY `AStratGameMode::OptionsWidgetClass` AND NOTHING ASSERTS THE TWO AGREE ... a match
// map pointed at an older options WBP would be a second, differently-behaved volume screen
// reachable only mid-match, WHICH NOBODY WILL FILE AS A BUG. DISCHARGED BY a clause reading both
// Blueprint CDOs and asserting the two class pointers equal."* `StratOptionsPresenter.h` records
// the same cost from the owner's side, and explains why no owner shape avoids the duplication: a
// `UWorldSubsystem` has no details panel, so the class has to be handed in, and both worlds need
// the SAME widget class, so there is no split that makes the duplication go away.
//
// THREE ASSETS AND NOT TWO, WHICH IS THE ONE PLACE THIS DEPARTS FROM ITS PRECEDENT.
// `StratShellBlueprintSlotParity.cpp` names its own limit -- *"It does not pin
// `BP_StratGameMode_AiVsAi` or any other subclass. If a third GameMode ever needs to agree ...
// that is a THIRD read added to this clause"* -- and `BP_StratGameMode_AiVsAi` is a third world
// with a `UStratOptionsPresenter` of its own, so it is read here. A demo world whose options
// screen is a different asset from the match's is the same drift in a place even fewer people
// look.
//
// EFFECTIVE READS, WHICH IS WHY IT READS CDOs AND NOT `.uasset` BYTES. A property ABSENT from a
// package proves it equals the C++ default -- here, `nullptr` -- and a CDO read cannot make that
// mistake: an asset with no override hands back the C++ default, an asset with one hands back
// the override, and the comparison is over what a spawned GameMode would actually hand
// `ConfigureOptionsPanel`. That reasoning is `StratShellBlueprintSlotParity.cpp`'s and is
// re-derived here rather than cited.
//
// AND `nullptr` IS ASSERTED AGAINST EXPLICITLY, WHICH THE SAVE-SLOT PRECEDENT DID NOT NEED.
// `SaveSlotName`'s C++ default is a correct value, so a relation-only clause was sufficient
// there. `OptionsWidgetClass`'s C++ default is `nullptr`, and THREE UNSET GAMEMODES AGREE
// PERFECTLY -- an agreement clause alone would go green on a milestone where nobody had
// configured the screen at all, which is the exact state this milestone started in.
// `StratOptionsPresenter.h` calls unset *"a configuration and not an error"* for a map that
// wants no options panel, so the non-null is asserted per asset with that in mind: it is a
// statement about these three shipped maps and not about the class.
//
// WHERE THE EXPECTATIONS COME FROM. Three independent CDO reads and a relation between them.
// This clause never states which WBP the answer ought to be, so a deliberate rename or
// replacement applied to all three stays green -- which is correct, since the shipped asset is a
// designer's to choose and this clause's subject is agreement.
//
// ---------------------------------------------------------------------------------------
// A HAZARD SPECIFIC TO THIS CLAUSE, RECORDED BECAUSE IT HAS ALREADY COST THIS MILESTONE A RED
// RUN. Loading a GameMode Blueprint's generated class LOADS AND COMPILES the widget Blueprints
// its defaults reference. On 2026-09-05 a widget saved without having been compiled ensured on
// every load, and the ensure surfaced against
// `GATE-AUDIO.EveryCueInTheShippedBankHasASound` -- A SOUND BANK CLAUSE, RED FOR A WIDGET REASON
// -- because that clause loads a GameMode CDO too. AN ENSURE IS AN AUTOMATION TEST ERROR, not a
// warning, and it is attributed to whichever clause happened to load the asset FIRST. So a red
// run naming any of the three assets below may be about none of them; check for a handled ensure
// before reading the failure as a parity failure.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "StratGameMode.h"
#include "StratOptionsWidget.h"
#include "StratShellGameMode.h"

namespace StratShippedOptionsWidgetClassParity
{
	/**
	 * One shipped GameMode Blueprint to read.
	 *
	 * The `_C` suffix is the generated CLASS inside the package, as against the `UBlueprint`
	 * asset object carrying the editor-only graph. It is the class a map's World Settings
	 * resolve and the one whose CDO carries the defaults a spawned GameMode starts from.
	 */
	struct FShippedGameMode
	{
		const TCHAR* Label;
		const TCHAR* ClassPath;
	};

	/** The title map's GameMode -- `AStratShellGameMode`'s Blueprint. */
	const TCHAR* const kShellPath =
		TEXT("/Game/StratPlay/BP_StratShellGameMode.BP_StratShellGameMode_C");

	/** The match map's. */
	const TCHAR* const kMatchPath =
		TEXT("/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C");

	/** The AI-vs-AI demo's, which is a third world with a presenter of its own. */
	const TCHAR* const kAiVsAiPath =
		TEXT("/Game/StratPlay/BP_StratGameMode_AiVsAi.BP_StratGameMode_AiVsAi_C");
}

// ---------------------------------------------------------------------------------------
// All three shipped GameModes name one options widget class.
//
// MUTANTS (all of them asset edits, which is why this clause exists at all -- no C++ change
// produces any of them):
//   - clear `OptionsWidgetClass` on `BP_StratGameMode` -- red on that asset's non-null, naming
//     it, and red again on the agreement.
//   - point `BP_StratGameMode_AiVsAi` at a second, older options WBP -- red on the agreement
//     only, printing both class names.
//   - clear it on all three -- red on three non-nulls; an agreement-only clause would be GREEN.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedOptionsWidgetClassParityTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedOptionsWidgetClassParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedOptionsWidgetClassParity;

	// The three reads, each independent, each red on any failure. "Could not tell" must never
	// be reported as "fine" -- `StratShellBlueprintSlotParity.cpp`'s stated rule for this shape.
	struct FRead
	{
		const TCHAR*                     Label = nullptr;
		TSubclassOf<UStratOptionsWidget> Panel;
	};

	auto ReadOne = [this](const TCHAR* const Label, const TCHAR* const Path,
	                      UClass* const NativeBase, FRead& Out) -> bool
	{
		UClass* const Generated = StaticLoadClass(NativeBase, nullptr, Path);
		if (!TestNotNull(*FString::Printf(
				TEXT("%s's generated class loads from '%s'. If this failed the asset is "
				     "missing, renamed or moved -- and this clause CANNOT conclude anything "
				     "about agreement, so it fails rather than passing."), Label, Path),
				Generated))
		{
			return false;
		}

		// THE SUBJECT IS THE ASSET'S CLASS, NOT THE NATIVE ONE. If a later edit pointed this
		// path at the C++ class the read would silently become a read of the constructor's
		// `nullptr`, and the clause would fail for a reason that names the wrong thing.
		if (!TestTrue(*FString::Printf(
				TEXT("%s resolves to a Blueprint-generated class and not a native C++ one "
				     "(read: '%s')"), Label, *Generated->GetName()),
				!Generated->HasAnyClassFlags(CLASS_Native)))
		{
			return false;
		}

		const AGameModeBase* const Cdo = Generated->GetDefaultObject<AGameModeBase>();
		if (!TestNotNull(*FString::Printf(TEXT("%s's class default object exists"), Label), Cdo))
		{
			return false;
		}

		// READ THROUGH THE REFLECTED PROPERTY AND NOT THROUGH A CAST TO ONE NATIVE BASE,
		// because the three assets do not share one: two derive from `AStratGameMode` and one
		// from `AStratShellGameMode`, and the two `OptionsWidgetClass` members are separate
		// declarations on separate classes -- which is the very duplication this clause is
		// about. A reflected read asks each CDO for its own property by name and cannot
		// accidentally read one class's member off the other.
		const FProperty* const Property =
			Generated->FindPropertyByName(TEXT("OptionsWidgetClass"));
		const FClassProperty* const AsClassProperty = CastField<FClassProperty>(Property);
		if (!TestNotNull(*FString::Printf(
				TEXT("%s declares an OptionsWidgetClass property. If this is null the member "
				     "has been renamed or removed and every comparison below is blind"), Label),
				AsClassProperty))
		{
			return false;
		}

		Out.Label = Label;
		Out.Panel = Cast<UClass>(AsClassProperty->GetPropertyValue_InContainer(Cdo));
		return true;
	};

	FRead Shell;
	FRead Match;
	FRead AiVsAi;

	if (!ReadOne(TEXT("the title map's GameMode Blueprint"), kShellPath,
			AStratShellGameMode::StaticClass(), Shell)
		|| !ReadOne(TEXT("the match map's GameMode Blueprint"), kMatchPath,
			AStratGameMode::StaticClass(), Match)
		|| !ReadOne(TEXT("the AI-vs-AI demo's GameMode Blueprint"), kAiVsAiPath,
			AStratGameMode::StaticClass(), AiVsAi))
	{
		return false;
	}

	// ---- 1. EACH ONE NAMES A CLASS AT ALL --------------------------------------------------
	//
	// SEE THE HEADER. Three unset GameModes agree perfectly, so an agreement-only clause would
	// be green on a build where nobody had configured the volume screen at all.
	const FRead* const All[] = { &Shell, &Match, &AiVsAi };
	for (const FRead* const One : All)
	{
		TestNotNull(*FString::Printf(
			TEXT("%s names an OptionsWidgetClass. Unset here means a player who reaches the "
			     "Options route on that map sees nothing at all -- which is a legitimate "
			     "configuration for a map that wants no volume screen, and is NOT what these "
			     "three shipped maps want"), One->Label),
			One->Panel.Get());
	}

	// ---- 2. AND IT IS THE SAME CLASS ON ALL THREE ------------------------------------------
	TestEqual(*FString::Printf(
		TEXT("the title map and the match map name the SAME options widget class ('%s' vs "
		     "'%s'). Disagreement here is two differently-behaved volume screens, one reachable "
		     "from the title and one only mid-match, which nobody will file as a bug"),
		*GetNameSafe(Shell.Panel.Get()), *GetNameSafe(Match.Panel.Get())),
		Shell.Panel.Get(), Match.Panel.Get());

	TestEqual(*FString::Printf(
		TEXT("and the AI-vs-AI demo names it too ('%s' vs '%s')"),
		*GetNameSafe(AiVsAi.Panel.Get()), *GetNameSafe(Match.Panel.Get())),
		AiVsAi.Panel.Get(), Match.Panel.Get());

	// ---- 3. AND IT IS AN OPTIONS SCREEN ----------------------------------------------------
	//
	// `TSubclassOf` enforces this at edit time, so this is belt to that braces -- and it is
	// cheap insurance against a reflected read that resolved a differently-typed property of
	// the same name on some future subclass.
	if (Shell.Panel.Get() != nullptr)
	{
		TestTrue(*FString::Printf(
			TEXT("the agreed class derives from UStratOptionsWidget (it is '%s')"),
			*GetNameSafe(Shell.Panel.Get())),
			Shell.Panel->IsChildOf(UStratOptionsWidget::StaticClass()));
	}

	return true;
}
