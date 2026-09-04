// GDD §2.7 / §2.9 / §2.11.6 -- what `BP_StratGameMode` AUTHORS into its `MatchConfig`. T-FAME-02.
//
// WHY THIS FILE EXISTS, AND IT IS A GAP TWO LANES FOUND INDEPENDENTLY ON THE SAME DAY.
// On 2026-09-03 a user ruling settled that the shipped game is HUMAN-VERSUS-AI and not a hot
// seat: `BP_StratGameMode`'s `MatchConfig` carries `AiSides=(1)` with `ViewingSide` 0, so
// `StratHandicappedSide` takes its THIRD arm and Easy's +150 lands on the human's side --
// intended, and ruled intended. Comments in
// `Source/StratPlay/Tests/StratAiMatchClauses.cpp` and
// `Source/StratPlay/Tests/StratDifficultyHandicapMatchClauses.cpp` were corrected to say so.
//
// **BUT NOT ONE CLAUSE IN THIS TREE READ THAT AUTHORED VALUE.** Every existing clause on the
// subject constructs its own `FStratMatchConfig` in C++ -- correctly, because each is pinning
// `StratHandicappedSide`'s arms or the subsystem's behaviour on a GIVEN config, not the asset.
// The consequence is that a content pass could re-author `BP_StratGameMode` back to an empty
// `AiSides` tomorrow, the ENTIRE suite would stay green, and every corrected comment in both
// files above would silently become false again. This clause is the one that would go red.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NOT A NUMBER THIS FILE INVENTED. `AiSides=(1)`
// and `ViewingSide` 0 were read off the LIVE EDITOR by the `coordinator` on 2026-09-03, with
// controls -- `BP_StratGameMode_AiVsAi` read `(0,1)`, `struct_properties("StratMatchConfig")`
// gave the C++ default as empty, and `BP_StratShellGameMode` carried no `MatchConfig` at all.
// The user then RULED that reading intended. So the expectation here is a literal, and it is
// one of the two shapes in which a literal is legitimate in this tree: a value the module
// cannot compute because the module is not the author of it. The asset is the author, and a
// test that asked the asset what it says and then asserted the same thing would be asserting
// nothing. This is the same posture, and the same justification, as
// `StratShippedGameModeOptIn.cpp`'s `true`.
//
// WHY THE `/Game/` LITERALS ARE HERE AND ARE NOT A VIOLATION. `CLAUDE.md`'s rule binds
// GAMEPLAY C++, and names automation-test fixtures as one of its two standing exceptions. This
// clause's SUBJECT is a named asset; there is no Blueprint default that could hold a property
// pointing at it, and a soft reference would make a renamed asset resolve to null and report as
// "not configured" instead of "not found". Naming the path IS the test.
//
// EVERY FAILURE PATH IS RED AND NONE OF THEM SKIPS. A clause about an authored default that
// cannot find the asset has observed nothing, and reporting "could not tell" as "fine" would
// leave it green while pinning nothing at all -- strictly worse than not existing, because it
// also occupies the space where a real gate would go. So a missing package, a renamed class, a
// path that resolves to the NATIVE class, and a null CDO are each an explicit failure with its
// own message naming what to fix.
//
// AND THE CONTROL IS NOT OPTIONAL, BECAUSE READING A BLUEPRINT DEFAULT IS ASYMMETRIC EVIDENCE.
// A CDO read is an EFFECTIVE read: it reports the asset's override if there is one and the C++
// default if there is not, and the two are indistinguishable at the call site. So a reader that
// had quietly stopped seeing overrides -- a wrong path, a stale package, a CDO taken off the
// native class -- would answer the C++ default to everything, and a clause whose expectation
// happened to equal the C++ default would pass for entirely the wrong reason. Two things
// discharge that here, and both are assertions rather than remarks:
//
//   1. THE C++ DEFAULT IS ASSERTED TO DIFFER FROM THE EXPECTATION, first and fatally.
//      `FStratMatchConfig::AiSides` initialises EMPTY; the expectation is one element. So the
//      value this clause looks for is one the C++ default CANNOT supply, and a reader stuck on
//      the C++ default fails rather than passes. If a later edit ever made `AiSides` default to
//      `{1}` in C++, this clause could no longer tell an override from an inheritance -- and it
//      says so and fails, rather than continuing to wear a name it no longer earns.
//   2. THE SAME READER IS RUN AGAINST A SECOND ASSET AND MUST COME BACK DIFFERENT.
//      `BP_StratGameMode_AiVsAi` authors `AiSides=(0,1)`. The reader must return `(1)` for one
//      package and `(0,1)` for the other -- three distinct answers counting the C++ default's
//      empty -- which is what makes it a reader OF THE ASSET rather than a function that
//      returns a constant.
//
// WHAT THIS CLAUSE DOES **NOT** PIN, said plainly so it is not inferred from the above:
//
//   - IT DOES NOT PIN `ViewingSide` AS AN OVERRIDE, and the assertion below says so in its own
//     message. `FStratMatchConfig::ViewingSide` initialises to 0 and the shipped asset also
//     reads 0, so on THAT field the effective read cannot distinguish "the designer set it" from
//     "nobody touched it". The assertion is still worth its place -- a shipped `ViewingSide` of
//     1 would put the human on the AI's seat and is exactly the inversion `StratHandicappedSide`
//     arm two exists to refuse -- but it is a pin on the VALUE, not on the authorship, and
//     conflating the two is the defect this paragraph exists to prevent.
//   - IT DOES NOT PIN ANY BEHAVIOUR. That a config of `ViewingSide` 0 with `AiSides` `{1}`
//     handicaps side 0 is `T-FAME-02.HandicapIsInertWithoutASinglePlayerOpponent`'s control arm
//     and `T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier`; both construct that config
//     in C++ and neither reads an asset. This clause is the missing half that ties those two to
//     the shipped package, and it is useless without them.
//   - IT DOES NOT PIN `Difficulty`. That field is now the subject of the SECOND clause in
//     this file, `T-FAME-02.ShippedGameModeRunsAtEasy`, and that clause is a
//     WEAKER instrument than this one for exactly the reason the `ViewingSide` bullet above
//     gives: `FStratMatchConfig::Difficulty` initialises to `EStratDifficulty::Easy` in C++,
//     `Easy` is the enum's first value and therefore 0, and the shipped asset also reads
//     `Easy`. So the effective read there pins the VALUE and cannot tell an authored tier
//     from an untouched one. Read that clause's own header before citing it: it establishes
//     that the shipped game runs at Easy, and it establishes NOTHING about whether a designer
//     put that tier there.
//   - IT DOES NOT PIN `SaveSlotName`, `AiPlaybackStepSeconds`, `ScenarioFile`, `FirstSide`,
//     the definition tables, the buildlist OR ANY OTHER FIELD of the same struct. Each is a
//     separate claim and would need its own assertion.
//   - IT DOES NOT PIN THAT `BP_StratGameMode` IS THE MAP'S GAMEMODE. That binding is a single
//     unguarded line in `Config/DefaultEngine.ini` and is a different subject entirely.
// ---------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "StratGameMode.h"
#include "StratMatchSubsystem.h"

namespace StratShippedMatchConfigClauses
{
	/**
	 * The shipped GameMode Blueprint's generated class, by path.
	 *
	 * The `_C` suffix is the generated CLASS inside the package, as against the `UBlueprint`
	 * asset object that carries the editor-only graph. It is the class an `AGameModeBase`
	 * spawn resolves and the one whose CDO carries the class defaults, so it is the only
	 * object that can answer what the shipped game is configured to do.
	 */
	const TCHAR* const kShippedGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C");

	/** The CONTROL asset. Authors `AiSides=(0,1)`; see this file's header on why it is here. */
	const TCHAR* const kAiVsAiGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode_AiVsAi.BP_StratGameMode_AiVsAi_C");

	/** `(0,1)` -- so a failure message names the value that was actually read. */
	FString Describe(const TArray<int32>& Sides)
	{
		FString Out = TEXT("(");
		for (int32 Index = 0; Index < Sides.Num(); ++Index)
		{
			if (Index > 0)
			{
				Out += TEXT(",");
			}
			Out += FString::FromInt(Sides[Index]);
		}
		return Out + TEXT(")");
	}

	/**
	 * `Easy` -- the reflected name, so a failure message names the tier that was read.
	 *
	 * ASKED OF THE `UENUM` RATHER THAN SWITCHED OVER IN THIS FILE. A hand-written switch here
	 * would be a second author of the tier names and would print a stale one the day a tier is
	 * added or renamed. This is display only; nothing is asserted about the string.
	 */
	FString DescribeDifficulty(const EStratDifficulty Tier)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratDifficulty>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Tier));
		}
		return FString::Printf(TEXT("<unreflected tier %d>"), static_cast<int32>(Tier));
	}

	/**
	 * A GameMode Blueprint's class default object, or `nullptr` WITH THE TEST ALREADY FAILED.
	 *
	 * Never returns a "could not tell" that a caller might read as benign: every early exit
	 * has already recorded a failure through `Test`, so a null here means the clause is
	 * already red and the caller's only correct move is to stop.
	 */
	const AStratGameMode* ResolveCdoOrFail(FAutomationTestBase& Test, const TCHAR* const Path)
	{
		UClass* const Generated =
			StaticLoadClass(AStratGameMode::StaticClass(), nullptr, Path);

		if (!Test.TestNotNull(
				FString::Printf(
					TEXT("the GameMode Blueprint's generated class loads from '%s'. If this "
					     "failed the asset is missing, renamed, or moved -- and this clause "
					     "CANNOT conclude anything about what it authors, so it FAILS rather "
					     "than passing. Fix the asset or fix this path; do not soften this "
					     "into a skip."),
					Path),
				Generated))
		{
			return nullptr;
		}

		if (!Test.TestTrue(
				FString::Printf(
					TEXT("'%s' resolved to a Blueprint-generated class and not to native C++ "
					     "(read: '%s'). This clause's subject is an ASSET's class default, so "
					     "a native class here means the path no longer points at the asset and "
					     "every value below would be the C++ default wearing the asset's name."),
					Path, *Generated->GetName()),
				!Generated->HasAnyClassFlags(CLASS_Native)))
		{
			return nullptr;
		}

		if (!Test.TestTrue(
				FString::Printf(
					TEXT("'%s' derives from AStratGameMode (read: '%s'), so its CDO actually "
					     "carries an FStratMatchConfig"),
					Path, *Generated->GetName()),
				Generated->IsChildOf(AStratGameMode::StaticClass())))
		{
			return nullptr;
		}

		const AStratGameMode* const Cdo = Generated->GetDefaultObject<AStratGameMode>();
		if (!Test.TestNotNull(
				FString::Printf(
					TEXT("'%s' has a class default object -- the object carrying the defaults a "
					     "spawned GameMode starts from"),
					Path),
				Cdo))
		{
			return nullptr;
		}

		return Cdo;
	}
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the shipped GameMode authors ONE AI side, and the human opens on side 0.
//
// The clause that makes "the shipped game is human-versus-AI" a fact about the package rather
// than a sentence in six comments. See this file's header for the control and for the four
// properties this deliberately does not pin.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedGameModeAuthorsOneAiSideTest,
	"Stratocracy.StratPlay.T-FAME-02.ShippedGameModeAuthorsOneAiSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedGameModeAuthorsOneAiSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedMatchConfigClauses;

	// ---- PREMISE: the C++ default CANNOT supply the expectation -------------------------
	//
	// Checked first and fatal. If `AiSides` ever initialises non-empty in C++, a reader that
	// had stopped seeing asset overrides could pass this clause by accident, still wearing the
	// name "ShippedGameModeAuthorsOneAiSide". It fails instead. Do not "fix" a failure here by
	// deleting the check -- restore the empty initialiser in
	// `Source/StratPlay/StratMatchSubsystem.h`.
	{
		const FStratMatchConfig CppDefaults;
		if (!TestEqual(
				TEXT("PREMISE: FStratMatchConfig::AiSides is EMPTY in C++. This clause exists "
				     "to prove BP_StratGameMode OVERRIDES that default; if the C++ default is "
				     "non-empty, the CDO read below can no longer tell an override from an "
				     "inheritance and this is not a gate on the asset at all."),
				CppDefaults.AiSides.Num(), 0))
		{
			return false;
		}
	}

	// ---- the shipped asset ---------------------------------------------------------------
	const AStratGameMode* const Shipped = ResolveCdoOrFail(*this, kShippedGameModeClassPath);
	if (Shipped == nullptr)
	{
		return false;
	}

	const TArray<int32>& ShippedAiSides = Shipped->MatchConfig.AiSides;

	TestEqual(
		*FString::Printf(
			TEXT("§2.7: BP_StratGameMode authors EXACTLY ONE AI side (read: %s). If this "
			     "failed with zero sides the shipped game has become a hot seat, and every "
			     "comment in StratAiMatchClauses.cpp and StratDifficultyHandicapMatchClauses.cpp "
			     "that says otherwise is now false. USER RULING 2026-09-03: human-versus-AI is "
			     "intended. The fix is in Content/StratPlay/BP_StratGameMode.uasset's class "
			     "defaults (strat-editor-builder's lane), not in this test."),
			*Describe(ShippedAiSides)),
		ShippedAiSides.Num(), 1);

	if (ShippedAiSides.Num() == 1)
	{
		TestEqual(
			*FString::Printf(
				TEXT("§2.7: the AI seat BP_StratGameMode authors is side 1, the Vanguard "
				     "(read: %s). Side 0 is the Directorate and is the human's."),
				*Describe(ShippedAiSides)),
			ShippedAiSides[0], 1);
	}

	// NOT A PIN ON AUTHORSHIP, and the message says so. `ViewingSide` initialises to 0 in C++
	// and the asset reads 0, so this cannot distinguish a set property from an untouched one.
	// It pins the VALUE, which is still worth pinning: a shipped `ViewingSide` of 1 would open
	// the human on the AI's seat, which is `StratHandicappedSide`'s second arm and would make
	// §2.9's handicap silently inert in the shipped game.
	TestEqual(
		*FString::Printf(
			TEXT("§2.7: BP_StratGameMode opens the screen on side 0, the human's seat (read: "
			     "%d). NOTE: 0 is ALSO FStratMatchConfig's C++ default, so this assertion pins "
			     "the VALUE and cannot tell an authored 0 from an untouched one -- unlike the "
			     "AiSides assertions above, which the C++ default cannot satisfy."),
			Shipped->MatchConfig.ViewingSide),
		Shipped->MatchConfig.ViewingSide, 0);

	// ---- THE CONTROL: the same reader, a different package, a DIFFERENT answer -----------
	//
	// Without this, every assertion above would pass on a reader that had silently stopped
	// seeing asset overrides and answered the same thing to everything.
	const AStratGameMode* const AiVsAi = ResolveCdoOrFail(*this, kAiVsAiGameModeClassPath);
	if (AiVsAi == nullptr)
	{
		return false;
	}

	const TArray<int32>& ControlAiSides = AiVsAi->MatchConfig.AiSides;

	TestEqual(
		*FString::Printf(
			TEXT("CONTROL: BP_StratGameMode_AiVsAi authors TWO AI sides (read: %s) -- phase "
			     "D's both-sides-AI gate"),
			*Describe(ControlAiSides)),
		ControlAiSides.Num(), 2);

	TestNotEqual(
		*FString::Printf(
			TEXT("CONTROL: the two packages give this reader DIFFERENT answers -- shipped %s "
			     "against AI-vs-AI %s. If these were equal the reader is not reading the "
			     "asset at all, and every assertion above passed for the wrong reason."),
			*Describe(ShippedAiSides), *Describe(ControlAiSides)),
		Describe(ShippedAiSides), Describe(ControlAiSides));

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the shipped GameMode runs at Easy.
//
// A PIN ON THE VALUE AND NOT ON THE AUTHORSHIP, AND THAT IS THE FIRST THING TO KNOW ABOUT IT.
// This is the `ViewingSide` situation from this file's header, NOT the `AiSides` situation, and
// the two must not be conflated. `FStratMatchConfig::Difficulty` initialises to
// `EStratDifficulty::Easy` in `Source/StratPlay/StratMatchSubsystem.h`; `Easy` is the FIRST
// value of `UENUM EStratDifficulty` and is therefore 0. The 2026-09-03 live-editor measurement
// recorded in this file's header found `ViewingSide` 0 and `Difficulty` `Easy` ABSENT from both
// the native and the Blueprint export -- which is what a zero-valued field sitting at its C++
// default looks like. So the CDO read below CANNOT distinguish an authored `Easy` from an
// untouched one, and there is deliberately NO premise block here asserting that the C++ default
// cannot supply the expectation: on this field that premise is FALSE, and asserting it would
// fail. The assertion's own message says all of this, so a reader who arrives at a failure
// without reading this block still learns it.
//
// WHY THE VALUE IS WORTH PINNING ANYWAY. Sec 2.11.6: "The first match runs on the one shipped
// scenario at **Easy** by default (player +150 opening Fame, Sec 2.9)". Easy is the only tier
// whose delta is non-zero in the player's favour -- `StratDifficultyFameDelta` gives +150 / 0 /
// -100 -- so a shipped `Normal` would make Sec 2.9's handicap ARITHMETICALLY unobservable in the
// shipped game while every existing handicap clause stayed green, because all of them construct
// their own tier in C++. That is the same shape of gap this file's first clause was written to
// close, and it is worth a clause even at the weaker warrant.
//
// WHERE THE EXPECTATION COMES FROM. The GDD sentence quoted above, at Sec 2.11.6. It is a
// literal because the module is not the author of what the shipped package configures. That the
// C++ default happens to agree with it is a coincidence of this field and is precisely what
// costs this clause its authorship warrant -- it is not the source of the expectation.
//
// THE LIVENESS CONTROL, AND ITS LIMIT SAID PLAINLY. A reader that had silently stopped seeing
// asset overrides would answer the C++ default to everything -- and on THIS field the C++
// default IS the expectation, so the `Difficulty` assertion alone proves nothing about whether
// the reader is alive. The control below discharges that by running the SAME `ResolveCdoOrFail`
// over the AI-vs-AI package and requiring the two packages to disagree on `AiSides`, where they
// are known to differ (`(1)` shipped against `(0,1)` control). THE LIVENESS EVIDENCE THEREFORE
// COMES FROM A DIFFERENT FIELD THAN THE ONE BEING PINNED. That is a strictly weaker warrant than
// `ShippedGameModeAuthorsOneAiSide` enjoys: it proves the reader reads assets, and it does not
// prove that THIS field's answer came from the asset rather than from the C++ default underneath
// it. No arrangement of assertions can close that gap while `Easy` is also the C++ default; only
// a different C++ default, or a reader that reports whether a property was overridden, could.
//
// WHAT IT DOES NOT PIN. Not authorship, per the whole of the above. Not any behaviour: that Easy
// moves the player's opening Fame by +150 is
// `T-FAME-02.HandicapMovesThePlayersOpeningFameAtEveryTier`, which constructs its own config in
// C++ and reads no asset. Not `SaveSlotName`, `AiPlaybackStepSeconds` or any other field.
//
// WHY THE NAME SAYS `RunsAt` AND NOT `Authors`. Renamed 2026-09-04 on the user's ruling, after
// `strat-integration-reviewer` observed that the prior name's verb was `Authors` -- sitting
// beside `ShippedGameModeAuthorsOneAiSide`, where that verb is earned -- and so claimed the exact
// warrant the five paragraphs above spend their length disclaiming. THE NAME IS THE ONE ARTIFACT
// THAT TRAVELS WITHOUT ITS HEADER: into `Saved/AutomationReport/index.json`, a CI summary line, a
// grep hit and a record file's "+1 added" line. A reader who meets only the name would conclude
// authorship, which is the thing this clause cannot establish. Correct disclaimers do not repair
// an overclaiming name. `RunsAt` states the value and claims nothing about who set it.
// The prior name is written out in full, stamped, in `Tools/architect/state/tests.md`, which is
// where a reader arriving from that gate report's four citations of it should be sent. It is
// deliberately NOT repeated here: a tree-wide census of the old identifier scoped to `Source/`
// must come back empty, and a comment would be an indistinguishable hit.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedGameModeRunsAtEasyTest,
	"Stratocracy.StratPlay.T-FAME-02.ShippedGameModeRunsAtEasy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedGameModeRunsAtEasyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedMatchConfigClauses;

	const AStratGameMode* const Shipped = ResolveCdoOrFail(*this, kShippedGameModeClassPath);
	if (Shipped == nullptr)
	{
		return false;
	}

	const EStratDifficulty ShippedTier = Shipped->MatchConfig.Difficulty;

	TestEqual(
		*FString::Printf(
			TEXT("Sec 2.11.6: the shipped game runs at Easy -- \"the first match runs on the one "
			     "shipped scenario at Easy by default (player +150 opening Fame, Sec 2.9)\" (read: "
			     "%s). A shipped Normal would make Sec 2.9's handicap arithmetically unobservable, "
			     "since Normal's delta is 0 and Easy's is the only one non-zero in the player's "
			     "favour. NOTE, AND IT IS THE POINT OF THIS CLAUSE'S HEADER: Easy is ALSO "
			     "FStratMatchConfig's C++ default and is the enum's zero value, so this assertion "
			     "pins the VALUE and CANNOT tell an authored Easy from an untouched one -- unlike "
			     "ShippedGameModeAuthorsOneAiSide's AiSides assertions, which the C++ default "
			     "cannot satisfy. Do not cite this clause as evidence that a designer set the "
			     "tier. The fix for a red here is in Content/StratPlay/BP_StratGameMode.uasset's "
			     "class defaults (strat-editor-builder's lane), not in this test."),
			*DescribeDifficulty(ShippedTier)),
		ShippedTier, EStratDifficulty::Easy);

	// ---- THE CONTROL: the reader is alive -- MEASURED ON A DIFFERENT FIELD ----------------
	//
	// The assertion above cannot be its own liveness check, because a reader that had stopped
	// seeing asset overrides would answer the C++ default and the C++ default is the expectation.
	// So liveness is measured on `AiSides`, where the two packages are known to differ. THIS IS A
	// WEAKER WARRANT THAN `ShippedGameModeAuthorsOneAiSide` HAS, and deliberately so: it proves
	// this reader reads ASSETS, not that the `Difficulty` value above came from the asset.
	const AStratGameMode* const AiVsAi = ResolveCdoOrFail(*this, kAiVsAiGameModeClassPath);
	if (AiVsAi == nullptr)
	{
		return false;
	}

	TestNotEqual(
		*FString::Printf(
			TEXT("LIVENESS CONTROL, ON A DIFFERENT FIELD: the same reader gives the two packages "
			     "DIFFERENT answers for AiSides -- shipped %s against AI-vs-AI %s. If these were "
			     "equal the reader is not reading assets at all, and the Difficulty assertion "
			     "above passed for the wrong reason, because Easy is what the C++ default would "
			     "have answered too."),
			*Describe(Shipped->MatchConfig.AiSides), *Describe(AiVsAi->MatchConfig.AiSides)),
		Describe(Shipped->MatchConfig.AiSides), Describe(AiVsAi->MatchConfig.AiSides));

	return true;
}
