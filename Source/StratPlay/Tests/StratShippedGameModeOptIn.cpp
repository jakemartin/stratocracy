// The gate on the SHIPPED opt-in -- `BP_StratGameMode`'s `MatchConfig` default for
// `bRecordCompletionOnMatchEnd` -- under T-UI-03. GDD §2.11.6.
//
// WHY THIS FILE EXISTS, AND WHY IT IS NOT IN `StratMatchCompletionRecording.cpp`. That file
// gates the WRITER: whether `UStratMatchSubsystem::RecordMatchCompletionOnSave` and the
// `ApplyView` hook that calls it do the right thing WHEN ASKED. Every clause in it assembles its
// own `FStratMatchConfig` in code and sets the opt-in by hand, so every one of them would stay
// green in a shipping build where nothing ever asks. This clause is the other half and it has a
// different subject: the ASSET default that decides whether the shipped game asks at all. It
// needs no world, no fixture, no DataTables and no save slot, and it writes nothing -- so it
// does not belong inside a file whose every other clause runs a match.
//
// WHAT GOES WRONG IF THIS IS RED. `UStratSaveGame::bHasCompletedAMatch` is never written in the
// packaged game, so §2.11.6's guided opening re-arms for every player, every match, forever. The
// C++ side is entirely correct in that state and every writer clause is green: the defect lives
// only in a Blueprint class default, which is a place no C++-only clause can see. That is the
// whole reason this file is worth its length.
//
// THE TRAP THIS CLAUSE IS SHAPED AROUND, AND IT IS THE REASON IT READS A CDO RATHER THAN BYTES.
// A property ABSENT from a `.uasset` proves it equals the C++ default. The C++ default here is
// FALSE -- `FStratMatchConfig::bRecordCompletionOnMatchEnd`'s member initialiser, pinned
// separately by `Stratocracy.StratPlay.T-SAVE-06.TheOptInDefaultsOffInCpp` -- and FALSE IS THE
// DEFECT. So "the property was not found in the asset, therefore it is unchanged, therefore
// fine" is exactly backwards, and a clause that extracted the asset's name table and looked for
// the property would read the broken state as the healthy one. Resolving the Blueprint-generated
// class and reading the EFFECTIVE value off its class default object cannot make that mistake:
// an asset with no override hands back the C++ default, which is false, which fails.
//
// EVERY WAY THE LOOKUP CAN FAIL IS A FAILURE, DELIBERATELY AND WITHOUT EXCEPTION. If the package
// does not load, if the generated class does not resolve, if the CDO is null, or if the class is
// not a `AStratGameMode` -- the clause goes RED and stops. None of those paths is allowed to
// reach a "well, we could not tell, so pass". A clause about a shipped default that cannot find
// the shipped default has not observed anything, and the one thing it must never do is report
// that as agreement.
//
// WHERE THE EXPECTATION COMES FROM. `true` is a literal, and this is one of the shapes in which
// a literal is correct: there is no module-side value that can produce it. No `strat::` API, no
// `FStratBridge` and no `FStratViewModel` knows what a shipped Blueprint ought to opt into --
// the requirement is §2.11.6's, that a completed match must be remembered so the guided opening
// retires, and the only place the shipping game can say yes is this asset's class default. The
// C++ default is deliberately NOT used as the expectation: parity against it would invert the
// clause, since the C++ default is the value this asset exists to override.
//
// AND THE C++ DEFAULT IS ASSERTED FALSE FIRST, AS THIS CLAUSE'S OWN PREMISE. If a later
// "convenience" flip turned the member initialiser true, this clause would go green on an asset
// that overrode nothing at all -- it would stop being a gate on `BP_StratGameMode` while still
// carrying its name. So the premise is checked, and the clause stops rather than interpreting a
// true it cannot attribute. This shares a subject with
// `Stratocracy.StratPlay.T-SAVE-06.TheOptInDefaultsOffInCpp` and that duplication is intended:
// that clause reports the flip, this one reports that the flip made this clause blind.
//
// WHY THE `/Game/` LITERAL IS HERE AND IS NOT A VIOLATION. The project rule forbids `/Game/`
// path literals in GAMEPLAY C++, and names automation-test fixtures as a standing exception. It
// has to be an exception here: the asset path is not incidental to this clause, it IS the
// subject. There is no property, no `EditDefaultsOnly` indirection and no bridge call that can
// name "the Blueprint the shipped game's World Settings point at" -- naming it is the test.
//
// WHAT THIS CLAUSE DOES **NOT** PIN, said plainly, because each of these has been mistaken for
// it before:
//   - It does not pin `BP_StratGameMode_AiVsAi`. That asset deliberately leaves the opt-in FALSE
//     -- an automated AI-vs-AI run must not write a completion onto a human player's save -- and
//     nothing here asserts over it or would force it true. If a later clause ever wants that
//     asymmetry pinned, it is a SECOND clause requiring FALSE there, not a widening of this one.
//   - It does not pin `SaveSlotName` or `AiMaxCommandsPerTurn`. Both were measured as INHERITED
//     rather than overridden in this asset -- they are C++ defaults showing through, so a clause
//     reading them here would silently be a clause about `StratMatchSubsystem.h`.
//   - It does not pin that any map's World Settings actually SELECT this GameMode, nor that
//     `DefaultGameMode` in `Config/DefaultEngine.ini` names it. Those are a different subject in
//     a different lane; this clause says only that IF this GameMode runs the match, the match
//     is recorded.
//   - It does not pin the writer. That is `StratMatchCompletionRecording.cpp`'s six clauses.
//
// NO STRING COMPARISON DECIDES ANYTHING BELOW. `FString::operator==`, `Contains` and
// `TestEqual(FString)` all ignore case in UE 5.8 and this project has already shipped one clause
// that could not fail because of it. The class names that appear in the messages are DIAGNOSTIC
// text inside `FString::Printf` only; every assertion is over a pointer, a class flag, an
// `IsChildOf`, or a bool.
//
// NOTHING HERE TOUCHES A SAVE SLOT. No `FStratMatchConfig` is handed to a subsystem, no match is
// started, no §2.8 result is reached and `UGameplayStatics` is not included. The standing slot
// hygiene rule for this suite does not apply because this clause has no path to a writer --
// `Saved/SaveGames/` is neither read nor written.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "StratGameMode.h"
#include "StratMatchSubsystem.h"

namespace
{
	/**
	 * The shipped GameMode Blueprint's generated class, by path.
	 *
	 * The `_C` suffix is the generated CLASS inside the package, as against the
	 * `UBlueprint` asset object that carries the editor-only graph. It is the class an
	 * `AGameModeBase` spawn resolves and the one whose CDO carries the class defaults, so
	 * it is the only object that can answer what the shipped game is configured to do.
	 */
	const TCHAR* const kShippedGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C");
}

// ---------------------------------------------------------------------------
// T-UI-03 -- the shipped GameMode opts in to recording match completion.
//
// The one clause standing between a correct writer and a game that never calls it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedGameModeOptsInTest,
	"Stratocracy.StratPlay.T-UI-03.TheShippedGameModeOptsIn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedGameModeOptsInTest::RunTest(const FString& /*Parameters*/)
{
	// ---- PREMISE: the C++ default is false, so a true below is the ASSET's ---------------
	//
	// Checked first and fatal. If the member initialiser ever flips, this clause would pass
	// on an asset that overrode nothing, still wearing the name "TheShippedGameModeOptsIn".
	// It says so instead. See the header block.
	{
		const FStratMatchConfig CppDefaults;
		if (!TestFalse(
				TEXT("PREMISE: FStratMatchConfig::bRecordCompletionOnMatchEnd is FALSE in C++. "
				     "This clause exists to prove BP_StratGameMode OVERRIDES that default; if "
				     "the C++ default is true, this clause can no longer tell an override from "
				     "an inheritance and is not a gate on the asset at all. Do not 'fix' this "
				     "by deleting the check -- restore the false initialiser in "
				     "Source/StratPlay/StratMatchSubsystem.h, which "
				     "Stratocracy.StratPlay.T-SAVE-06.TheOptInDefaultsOffInCpp also pins."),
				CppDefaults.bRecordCompletionOnMatchEnd))
		{
			return false;
		}
	}

	// ---- Resolve the shipped Blueprint's generated class ---------------------------------
	//
	// EVERY FAILURE PATH FROM HERE DOWN IS RED. A clause about a shipped default that cannot
	// find the shipped default has observed nothing, and "could not tell" must never be
	// reported as "fine" -- the state it would be hiding IS the defect.
	UClass* const ShippedClass = StaticLoadClass(
		AStratGameMode::StaticClass(), nullptr, kShippedGameModeClassPath);

	if (!TestNotNull(
			FString::Printf(
				TEXT("the shipped GameMode Blueprint's generated class loads from '%s'. If this "
				     "failed the asset is missing, renamed, or moved -- and this clause CANNOT "
				     "conclude anything about the opt-in, so it fails rather than passing. "
				     "Fix the asset or fix this path; do not soften this into a skip."),
				kShippedGameModeClassPath),
			ShippedClass))
	{
		return false;
	}

	// ---- The subject is the ASSET's class, not the native one ----------------------------
	//
	// Asserted rather than assumed, and it is the mirror image of the assertion in
	// `StratPlayerControllerTick.cpp`: that clause pins a C++ constructor and refuses to read
	// a Blueprint; this one pins a Blueprint class default and must refuse to read C++. If a
	// later edit ever pointed this path at the native class, the read would silently become a
	// read of the member initialiser -- which is false, so the clause would go red, but with
	// a message blaming the asset for a path bug. It names the real thing instead.
	if (!TestTrue(
			FString::Printf(
				TEXT("the resolved class is Blueprint-generated and not native C++ (read: "
				     "'%s'). This clause's subject is BP_StratGameMode's class default, so a "
				     "native class here means the path no longer points at the asset."),
				*ShippedClass->GetName()),
			!ShippedClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	if (!TestTrue(
			FString::Printf(
				TEXT("the resolved class derives from AStratGameMode (read: '%s'), so its CDO "
				     "actually carries an FStratMatchConfig"),
				*ShippedClass->GetName()),
			ShippedClass->IsChildOf(AStratGameMode::StaticClass())))
	{
		return false;
	}

	const AStratGameMode* const Cdo = ShippedClass->GetDefaultObject<AStratGameMode>();
	if (!TestNotNull(
			TEXT("the shipped GameMode Blueprint's class default object exists -- this is the "
			     "object that carries the class defaults a spawned GameMode starts from"),
			Cdo))
	{
		return false;
	}

	// ---- The clause -----------------------------------------------------------------------
	//
	// This is an EFFECTIVE read: the CDO reports the asset's override if there is one and the
	// C++ default if there is not. An asset that never opted in therefore arrives here as
	// false and fails, which is the correct reading of an absent property and the reason this
	// clause does not go anywhere near the `.uasset` bytes.
	TestTrue(
		TEXT("§2.11.6: BP_StratGameMode's MatchConfig sets bRecordCompletionOnMatchEnd = true. "
		     "If this failed: the shipped game never records that a match was completed, so "
		     "UStratSaveGame::bHasCompletedAMatch stays false forever and the guided opening "
		     "re-arms for every player on every match, permanently. The C++ writer would be "
		     "entirely correct and every clause in StratMatchCompletionRecording.cpp would be "
		     "green -- nothing ever asks it to run. NOTE: a false here can mean the checkbox "
		     "was cleared OR that the property was never set at all; an unset property reports "
		     "the C++ default, which is false. Both are the same defect. The fix is in "
		     "Content/StratPlay/BP_StratGameMode.uasset's class defaults (strat-editor-builder's "
		     "lane), not in this test and not in the C++ default."),
		Cdo->MatchConfig.bRecordCompletionOnMatchEnd);

	return true;
}
