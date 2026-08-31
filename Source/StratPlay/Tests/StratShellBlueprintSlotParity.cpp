// The gate on the SHIPPED SAVE SLOT AGREEMENT between the two GameMode Blueprints --
// `BP_StratShellGameMode`'s `SaveSlotName` and `BP_StratGameMode`'s
// `MatchConfig.SaveSlotName`. GATE-TITLEMENU. GDD §2.11.5.
//
// WHY THIS FILE EXISTS, AND IT WAS RECORDED AS OWED BEFORE THE ASSETS EXISTED.
// `Source/StratPlay/StratShellGameMode.h` names this clause in terms, under `SaveSlotName`'s
// "WHAT IT DOES NOT FIX": *"A designer who overrides EITHER property on EITHER Blueprint
// re-opens the drift, and no C++ can see it ... The discharge is unchanged -- a clause reading
// both Blueprint CDOs and asserting the two strings equal, once the editor lane has created
// them."* `Stratocracy.StratPlay.GATE-TITLEMENU.TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn`
// closed the C++ half -- the shell's constructor derives its default from `FStratMatchConfig`
// rather than restating the literal -- and closed nothing on the asset side, saying so in its
// own "does not pin" block. W6's asset tail created `BP_StratShellGameMode` and the debt became
// payable. This file pays it.
//
// WHAT GOES WRONG IF THIS IS RED, IN THE PLAYER'S TERMS. The title screen's `Continue` asks
// whether ITS slot holds a restorable match; the match map writes THE OTHER slot. Every save the
// player makes lands in a slot the title screen never reads, and `Continue` greys with
// *"No saved match."* -- which is also the ordinary, correct first-run sentence. There is no
// error, no log line a player would ever see, and no C++ that can tell the two apart: the shell
// asks `UStratMatchSubsystem::DoesSlotHoldARestorableMatch` for a slot that genuinely holds
// nothing, and gets a truthful answer to the wrong question. The defect lives entirely in two
// Blueprint class defaults, which is a place no C++-only clause can see.
//
// THE TRAP THIS CLAUSE IS SHAPED AROUND, AND IT DECIDES THE INSTRUMENT: `FString` COMPARISON IS
// CASE-INSENSITIVE IN THIS ENGINE. `operator==`, `Contains` and `TestEqual(FString)` all ignore
// case in UE 5.8, measured in this project, which has already shipped one clause that could not
// fail for exactly that reason. And case matters to the subject: the slot name is handed to
// `UGameplayStatics::DoesSaveGameExist` and `LoadGameFromSlot`, which resolve it to a file name
// under `Saved/SaveGames/`, so a differently-cased pair is a real drift and not a cosmetic one.
// Every comparison below is `FString::Equals(..., ESearchCase::CaseSensitive)`, and the clause
// proves its own instrument can see case before it uses it -- see the first premise. That
// premise is a fixed two-character control, not derived from the data, so it cannot go vacuous
// on a slot name that happens to be uniformly cased.
//
// THE VACUITY GUARD, WITHOUT WHICH THIS CLAUSE WOULD BE WORTHLESS. Two empty strings are equal
// under any comparison. If both Blueprints ever cleared the property the equality would hold
// while the game was unconfigured, so both values are asserted NON-EMPTY first.
//
// EVERY WAY THE LOOKUP CAN FAIL IS A FAILURE, following `StratShippedGameModeOptIn.cpp`'s
// precedent exactly. If a package does not load, a generated class does not resolve, a CDO is
// null, or a class is native rather than Blueprint-generated -- this clause goes RED and stops.
// A clause about two shipped defaults that cannot find one of them has observed nothing, and the
// one thing it must never do is report that as agreement.
//
// WHY THE `/Game/` LITERALS ARE HERE AND ARE NOT A VIOLATION. `CLAUDE.md`'s rule 4 forbids
// `/Game/` path literals in GAMEPLAY C++ and names automation-test fixtures as one of its two
// standing exceptions. It has to be an exception here for the same reason it was in
// `StratShippedGameModeOptIn.cpp`: the asset paths are not incidental to this clause, they ARE
// the subject. No property, no `EditDefaultsOnly` indirection and no bridge call can name "the
// two Blueprints the shipped game's World Settings point at" -- naming them is the test.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NOT A TYPED STRING. Neither side of the comparison
// is authored here. Both are read from module-side objects: one from
// `BP_StratShellGameMode_C`'s CDO, one from `BP_StratGameMode_C`'s. The clause asserts a
// RELATION between two independent reads and never states what the slot ought to be called, so
// it stays green over a deliberate rename that is applied to both -- which is the correct
// behaviour, since the shipped name is a designer's to choose and this clause's subject is
// agreement, not identity with any particular string.
//
// THESE ARE EFFECTIVE READS, WHICH IS WHY IT READS CDOs AND NOT `.uasset` BYTES. A property
// ABSENT from a package proves it equals the C++ default -- and `SaveSlotName` is deliberately
// absent from `BP_StratShellGameMode`'s saved bytes, because the shell's constructor derives the
// right value and an override would give the string a second author. A byte-level clause would
// therefore find nothing and have to guess what nothing meant. A CDO read cannot make that
// mistake: an asset with no override hands back the C++ default, an asset with one hands back
// the override, and the comparison is over what a spawned GameMode would actually use.
//
// WHAT THIS CLAUSE DOES **NOT** PIN, said plainly:
//   - It does not pin either value against `FStratMatchConfig`'s C++ default. That is
//     `Stratocracy.StratPlay.GATE-TITLEMENU.TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn`,
//     which pins the NATIVE defaults and is a different subject. Asserting it here as well would
//     redden on a designer who renamed the slot correctly on both assets, which is a change this
//     clause must permit.
//   - It does not pin `MatchLevel`, `TitleLevel`, or any map's World Settings. A separate drift
//     with a separate symptom.
//   - It does not pin `BP_StratGameMode_AiVsAi` or any other subclass. If a third GameMode ever
//     needs to agree about the slot, that is a THIRD read added to this clause or a clause of
//     its own -- not something a reader may assume is already covered.
//   - It does not pin that a save is ever WRITTEN to the agreed slot, nor that `Continue`
//     behaves correctly once it is. Those are `StratSaveSlotClauses.cpp`'s and
//     `StratShellRouteClauses.cpp`'s.
//
// NOTHING HERE TOUCHES A SAVE SLOT ON DISK. No `UGameplayStatics` call is made, no match is
// started and `Saved/SaveGames/` is neither read nor written. The two slot NAMES are read as
// data and compared. The standing slot-hygiene rule for this suite has no subject here.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "StratGameMode.h"
#include "StratMatchSubsystem.h"
#include "StratShellGameMode.h"

namespace
{
	/**
	 * The title map's GameMode Blueprint's generated class, by path.
	 *
	 * The `_C` suffix is the generated CLASS inside the package, as against the `UBlueprint`
	 * asset object carrying the editor-only graph. It is the class `Lvl_Title`'s World
	 * Settings resolve and the one whose CDO carries the class defaults a spawned GameMode
	 * starts from.
	 */
	const TCHAR* const kShellGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratShellGameMode.BP_StratShellGameMode_C");

	/** The match map's GameMode Blueprint's generated class, same shape, same reasoning. */
	const TCHAR* const kMatchGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C");
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- the two shipped GameMode Blueprints name one save slot.
//
// The clause `StratShellGameMode.h` recorded as owed, now that both assets exist.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellBlueprintSlotParityTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellBlueprintSlotParityTest::RunTest(const FString& /*Parameters*/)
{
	// ---- PREMISE: the instrument can see case ---------------------------------------------
	//
	// A FIXED CONTROL AND NOT A TRANSFORM OF THE SUBJECT, deliberately: deriving it by
	// upper-casing the real slot name would collapse to a tautology on a name that is already
	// uniformly cased. If this ever fails, `FString::Equals(..., CaseSensitive)` has stopped
	// discriminating and every assertion below is blind -- so the clause stops rather than
	// reporting an agreement it could not have measured.
	{
		const FString Lower(TEXT("a"));
		if (!TestFalse(
				TEXT("PREMISE: FString::Equals(..., ESearchCase::CaseSensitive) distinguishes "
				     "'a' from 'A'. This clause's whole instrument is that comparison -- UE's "
				     "default FString comparison, Contains and TestEqual all IGNORE case, and "
				     "this project has already shipped a clause that could not fail because of "
				     "it. If this premise is red, the comparison below cannot tell two "
				     "differently-cased slot names apart and the clause is not a gate at all."),
				Lower.Equals(TEXT("A"), ESearchCase::CaseSensitive)))
		{
			return false;
		}
	}

	// ---- Resolve the title map's GameMode Blueprint ---------------------------------------
	//
	// EVERY FAILURE PATH FROM HERE DOWN IS RED. See the header block: "could not tell" must
	// never be reported as "fine".
	UClass* const ShellClass = StaticLoadClass(
		AStratShellGameMode::StaticClass(), nullptr, kShellGameModeClassPath);

	if (!TestNotNull(
			FString::Printf(
				TEXT("the title map's GameMode Blueprint's generated class loads from '%s'. If "
				     "this failed the asset is missing, renamed or moved -- and this clause "
				     "CANNOT conclude anything about slot agreement, so it fails rather than "
				     "passing. Fix the asset or fix this path; do not soften this into a skip."),
				kShellGameModeClassPath),
			ShellClass))
	{
		return false;
	}

	// THE SUBJECT IS THE ASSET'S CLASS, NOT THE NATIVE ONE, on `StratShippedGameModeOptIn.cpp`'s
	// precedent. If a later edit pointed this path at `AStratShellGameMode` itself the read
	// would silently become a read of the C++ constructor -- which derives the correct value, so
	// the clause would go GREEN while pinning nothing about any asset. That is the one direction
	// this must never fail in, so it is asserted rather than assumed.
	if (!TestTrue(
			FString::Printf(
				TEXT("the resolved shell class is Blueprint-generated and not native C++ (read: "
				     "'%s'). This clause's subject is BP_StratShellGameMode's class default; a "
				     "native class here means the path no longer points at the asset, and the "
				     "clause would pass by reading the C++ default it exists to look past."),
				*ShellClass->GetName()),
			!ShellClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	const AStratShellGameMode* const ShellCdo =
		ShellClass->GetDefaultObject<AStratShellGameMode>();
	if (!TestNotNull(
			TEXT("the title GameMode Blueprint's class default object exists -- this is the "
			     "object carrying the defaults a spawned shell GameMode starts from, and the "
			     "value BeginPlay hands to UStratShellSubsystem::ConfigureMatchDestination"),
			ShellCdo))
	{
		return false;
	}

	// ---- Resolve the match map's GameMode Blueprint ---------------------------------------
	UClass* const MatchClass = StaticLoadClass(
		AStratGameMode::StaticClass(), nullptr, kMatchGameModeClassPath);

	if (!TestNotNull(
			FString::Printf(
				TEXT("the match map's GameMode Blueprint's generated class loads from '%s'. Same "
				     "rule as above: a clause that cannot find one of its two subjects has "
				     "observed nothing."),
				kMatchGameModeClassPath),
			MatchClass))
	{
		return false;
	}

	if (!TestTrue(
			FString::Printf(
				TEXT("the resolved match class is Blueprint-generated and not native C++ (read: "
				     "'%s'). A native class here would turn this half of the comparison into a "
				     "read of FStratMatchConfig's member initialiser, which the shell derives "
				     "from -- so the clause would agree with itself and pin nothing."),
				*MatchClass->GetName()),
			!MatchClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	const AStratGameMode* const MatchCdo = MatchClass->GetDefaultObject<AStratGameMode>();
	if (!TestNotNull(
			TEXT("the match GameMode Blueprint's class default object exists -- this is the "
			     "object carrying the FStratMatchConfig a spawned match GameMode starts from, "
			     "and the slot UStratMatchSubsystem actually writes"),
			MatchCdo))
	{
		return false;
	}

	// ---- The two effective values ----------------------------------------------------------
	//
	// TWO INDEPENDENT READS FROM TWO DISTINCT OBJECTS. Neither is authored here and neither is
	// derived from the other; the clause asserts only that they agree.
	const FString ShellSlot = ShellCdo->SaveSlotName;
	const FString MatchSlot = MatchCdo->MatchConfig.SaveSlotName;

	// ---- THE VACUITY GUARD, BEFORE THE COMPARISON ------------------------------------------
	//
	// Two empty strings are equal. Without this, a game with the slot cleared on both assets
	// would report agreement -- and `DoesSaveGameExist("")` is false for every player forever.
	if (!TestFalse(
			TEXT("PREMISE: the title GameMode Blueprint's effective SaveSlotName is not empty. "
			     "An empty slot on both assets would satisfy the equality below while Continue "
			     "was permanently greyed, so the comparison is not allowed to run over it."),
			ShellSlot.IsEmpty()))
	{
		return false;
	}

	if (!TestFalse(
			TEXT("PREMISE: the match GameMode Blueprint's effective MatchConfig.SaveSlotName is "
			     "not empty either -- same reason."),
			MatchSlot.IsEmpty()))
	{
		return false;
	}

	// ---- The clause -------------------------------------------------------------------------
	//
	// SAME, so the comparison is case-sensitive: the stricter reading, and the necessary one on
	// an engine whose default `FString` comparison is not. The slot name becomes a file name
	// under `Saved/SaveGames/`, so a case-only difference is a real drift.
	TestTrue(
		FString::Printf(
			TEXT("§2.11.5: BP_StratShellGameMode's SaveSlotName and BP_StratGameMode's "
			     "MatchConfig.SaveSlotName are the same string, character for character. Read: "
			     "title = '%s', match = '%s'. If this failed, every save the player makes lands "
			     "in a slot the title screen never reads: Continue greys with \"No saved "
			     "match.\", which is ALSO the correct first-run sentence, so the failure is "
			     "silent and permanent and no log line reports it. Both C++ sides are correct in "
			     "that state -- AStratShellGameMode's constructor derives its default from "
			     "FStratMatchConfig and TheShellsDefaultSlotIsDerivedFromTheMatchConfigsOwn stays "
			     "green -- because the defect is a Blueprint class default overriding one of "
			     "them. The fix is in Content/StratPlay/BP_StratShellGameMode.uasset or "
			     "Content/StratPlay/BP_StratGameMode.uasset (strat-editor-builder's lane): clear "
			     "the override, or set both to the same string. It is NOT in this test and NOT "
			     "in the C++ defaults. NOTE: either value may be an inherited C++ default rather "
			     "than an override -- this reads the EFFECTIVE value, which is what a spawned "
			     "GameMode uses, and both cases are the same subject."),
			*ShellSlot, *MatchSlot),
		ShellSlot.Equals(MatchSlot, ESearchCase::CaseSensitive));

	return true;
}
