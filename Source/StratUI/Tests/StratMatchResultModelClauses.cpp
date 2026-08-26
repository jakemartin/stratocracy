// GDD §2.11.4 -- the end-of-match screen's MODEL. T-UI-03.
//
// WHY T-UI-03 AND NOT A NEW ID, AND THE ID WAS VERIFIED RATHER THAN ASSUMED. §2.11.4 is
// GDD-owned and the GDD's Invariants block mints no ID for it: the whole acceptance set is
// `T-AI-*`, `T-CAP-*`, `T-COMBAT-*`, `T-DATA-*`, `T-FAME-*`, `T-HEX-*`, `T-INT-*`, `T-MOVE-*`,
// `T-REPAIR-*`, `T-SAVE-*`, `T-SCN-*`, `T-TURN-*` and `T-UI-01..05`, and none of the five UI
// rows names the end-of-match screen. T-UI-03 is the row that names §2.11 and §2.8 together
// and ends "with no widget-side arithmetic", and this project has already ruled it the right
// ID for a §2.11 surface that is not the standings panel itself: `StratMatchResultRouting.cpp`
// files §2.11.5's production-menu shortfall under T-UI-03 for exactly that reason. This file
// follows that ruling rather than minting a sixth UI id.
//
// WHAT THE ENGINEER PROPOSED AND WHAT THE DOCUMENT SAYS. The engineer proposed `T-TURN-04` for
// the show-once clause. `T-TURN-04` is "at the turn cap, the attrition tiebreak resolves in the
// exact §2.8 order: combat Fame -> objectives held -> surviving HP -> draw" -- a rules-module
// property, already pinned in `Source/StratBridge/Tests/StratMatchResultClauses.cpp`, and it
// says nothing about a screen appearing once. That clause is filed under T-INT-05 in
// `Source/StratPlay/Tests/StratMatchResultTrigger.cpp` instead, beside the transition it is a
// part of.
//
// WHERE EVERY EXPECTATION IN THIS FILE COMES FROM. This is the column that matters:
//   - THE ROWS: `StratBuildScoreboardModel` on the same bridge and the same viewing side, in
//     the same frame. The comparison is UE's own reflection walk over
//     `FStratScoreboardModel::StaticStruct()`, so it is IDENTITY and not a hand-listed
//     agreement -- see `RowsAreTheScoreboardModelByIdentity`.
//   - THE SIX LINES: PARSED OUT OF THE GDD at runtime, from
//     `Tools/architect/gdd_snapshot/Stratocracy_Prototype_GDD.md`, which is tracked in this
//     repository and was measured byte-identical (`git hash-object`) to
//     `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md` when these clauses were
//     written. A clause that typed the same six literals the code types would be a second
//     transcription agreeing with the first and would pin nothing.
//   - THE WORD BUDGET: parsed out of the same paragraph (`a result line is **<= 30 words**`),
//     never written down as 30.
//   - THE TIER WORDS: parsed out of §2.11.4's own sentence ("Decisive / Marginal / Draw").
//   - THE TIER, CAUSE, WINNER AND KEY of a concluded match: the rules module's, reached by
//     PLAYING -- see `FAiDriver`. Nothing below plants a result.
//   - THE ENUMERATOR SETS: `StaticEnum<>()`'s own walk, so a tier or cause added upstream is
//     REQUIRED to be handled rather than silently unmentioned.
//   - THE NINE BANNED WORDS: the one stated literal in this file. `kb/setting.md` supplies
//     them and `kb/setting.md` is NOT in this repository -- it lives only in
//     `E:\MultiAgent\stratocracy-content\kb\setting.md`, which no headless run may depend on.
//     The list is therefore cited rather than read, and the change that would make it a read
//     is named in this file's Blocked note below.
//
// §2.8's KEYS 2 AND 3 ARE STILL UNREACHABLE THROUGH A MATCH, AND ARE NO LONGER UNPINNED.
// [Stamped 2026-08-25. This block previously ended "It is not silently absent" and named the
// hole as OPEN; the module-side change it pointed at has landed and the sentence below replaces
// that reading. The unreachability itself is unchanged and is not the part that was fixed.]
// A `decidedByKey` of 2 or 3 needs a capped match in which both sides fought to an EQUAL combat
// Fame, which no arrangement short of a hand-authored `GameState` produces, so no clause here
// reaches those arms BY PLAYING. `StratScoreCriterionForKey` is the `STRATUI_API` seam that
// makes the mapping callable directly, and `EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion`
// pins all three keys and every non-key through it. `DecidedByKeyMapsToTheDecidingRow` still
// pins the shift END TO END over whichever key a played match produces -- the two clauses are
// the reachable half and the callable half of one property, and neither replaces the other.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `FString::Contains` and
// `TestEqual` ALL ignore case in UE 5.8, measured on this project, so an ordinary comparison of
// two byte-identical strings is a clause that cannot fail. Every text assertion below is
// `ESearchCase::CaseSensitive`, and the GDD samples differ from the code's strings in nothing
// BUT case if the code is wrong -- which is exactly the defect a case-insensitive compare hides.
//
// WHY THIS FILE LIVES IN StratUI/Tests. `SideFaction`, `StratResultLineFor`,
// `StratResultTierTextFor`, `StratBuildMatchResultModel`, `StratBuildScoreboardModel` and
// `StratBuildMatchResult` are all `STRATUI_API` and declared in StratUI. StratBridge sits BELOW
// it, so a StratBridge test could name none of them; StratPlay could, but its clauses cannot see
// `FStratScoreboardModel`'s reflection without UMG. The only `strat::` names below are none at
// all -- every rules-side value arrives through `FStratBridge`'s engine-typed methods.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratMatchResultWidget.h"
#include "StratScoreboardWidget.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// `StratMatchResultWidget.h` records about itself.
#include "StratBridge.h"

namespace StratMatchResultModel
{
	/** The harness pin every parity file in this suite carries, and it must stay equal to
	 *  theirs. `Replay.h` states that no rule in this project decides which side moves first. */
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ShippedScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	/** Brings a bridge to the seeded opening. Same helper, same shape, and the same reason an
	 *  automation fixture may name a `/Game/` path at all, as `StratMatchResultRouting.cpp`. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ShippedScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}

		// §2.9's buildlist, set BY ID off the table the bridge just loaded. A hand-written
		// list here would be this file deciding §4.8's id space; `SetBuildlistByIds` resolves
		// the names against `UnitDefs()` and refuses anything it cannot.
		const FStratResult Listed = Bridge.SetBuildlistByIds(Units->GetRowNames());
		if (!Listed.bOk)
		{
			OutError = FString::Printf(TEXT("SetBuildlistByIds refused: %s"), *Listed.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Plays the seeded bridge to a §2.8 result with §2.9's own AI on both sides.
	 *
	 * NOTHING IS PLANTED AND NO OUTCOME IS ARRANGED. `NextAiCommand` is the only door §2.9 can
	 * be reached through and the four submit arms are the same ones a player's command takes --
	 * this is `FStratBridgeAiTurnPort::Submit`'s switch, at the bridge layer, because
	 * `StratAiTurnRunner` is StratPlay's and StratPlay sits ABOVE this module. The tier, cause,
	 * winner and key that come out are the rules module's answers about a game it played.
	 *
	 * BOUNDED, AND THE BOUND IS A FAILURE RATHER THAN A SILENT STOP. A loop that ran out of
	 * budget and returned the InProgress result it started with would make every clause keyed
	 * off "the match concluded" vacuous. `bHitBound` says which happened.
	 *
	 * DETERMINISTIC BY T-TURN-09, so the tier and winner this produces are a fixed property of
	 * the shipped scenario plus the vendored AI, not a coin toss -- which is what makes it
	 * legitimate for a clause to require a winner rather than tolerate either answer.
	 */
	struct FAiDriver
	{
		int32 Commands  = 0;
		bool  bHitBound = false;
		bool  bRefused  = false;
		FString RefusalReason;

		/** ~20 turns x 2 sides x a generous per-turn command budget. */
		static const int32 kCommandBound = 6000;

		bool Play(FStratBridge& Bridge, FStratMatchResultView& OutResult, FString& OutError)
		{
			for (Commands = 0; Commands < kCommandBound; ++Commands)
			{
				if (!StratBuildMatchResult(Bridge, OutResult, OutError))
				{
					return false;
				}
				if (OutResult.Tier != EStratResultTier::InProgress)
				{
					return true;
				}

				FStratAiCommand   Command;
				const FStratResult Decided = Bridge.NextAiCommand(Bridge.SideToMove(), Command);
				if (!Decided.bOk)
				{
					bRefused      = true;
					RefusalReason = FString::Printf(TEXT("NextAiCommand refused at turn %d side %d: %s"),
						Bridge.Turn(), Bridge.SideToMove(), *Decided.Reason);
					OutError = RefusalReason;
					return false;
				}

				FStratResult Applied;
				switch (Command.Kind)
				{
				case EStratAiCommandKind::Build:
					Applied = Bridge.SubmitBuildAtHex(Command.Hex, Command.DefIndex);
					break;
				case EStratAiCommandKind::Move:
					Applied = Bridge.SubmitMoveToHex(Command.UnitId, Command.Hex);
					break;
				case EStratAiCommandKind::Attack:
					Applied = Bridge.SubmitAttackAtHex(Command.UnitId, Command.Hex);
					break;
				case EStratAiCommandKind::EndTurn:
					Applied = Bridge.SubmitEndTurn();
					break;
				default:
					bRefused      = true;
					RefusalReason = FString::Printf(
						TEXT("the AI named a command kind this driver has no arm for (%d)"),
						static_cast<int32>(Command.Kind));
					OutError = RefusalReason;
					return false;
				}

				if (!Applied.bOk)
				{
					bRefused      = true;
					RefusalReason = FString::Printf(
						TEXT("the rules module refused the AI's own command at turn %d side %d: %s"),
						Bridge.Turn(), Bridge.SideToMove(), *Applied.Reason);
					OutError = RefusalReason;
					return false;
				}
			}

			bHitBound = true;
			OutError  = FString::Printf(
				TEXT("the AI-vs-AI game did not reach a §2.8 result inside %d commands, so nothing "
				     "below measured a concluded match"), kCommandBound);
			return false;
		}
	};

	// -----------------------------------------------------------------------------------
	// §2.11.4, READ OUT OF THE GDD.
	// -----------------------------------------------------------------------------------

	/**
	 * The tracked copy of the GDD.
	 *
	 * WHY A COPY IS LEGITIMATE HERE. `Tools/architect/gdd_snapshot/MANIFEST.md` says the real
	 * document lives in `stratocracy-content` and that this copy exists so the deliverable is
	 * runnable standalone. A clause may not reach outside the repository -- a headless run on a
	 * machine with only the game repo would fail on a path it cannot see -- so it reads the
	 * tracked copy. The two were measured byte-identical with `git hash-object` when this file
	 * was written (`de88390`), and the manifest itself warns the copy can go stale, which is why
	 * every extraction below FAILS LOUDLY when its anchor is missing rather than falling back.
	 */
	static FString GddPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Tools"), TEXT("architect"),
			TEXT("gdd_snapshot"), TEXT("Stratocracy_Prototype_GDD.md"));
	}

	/** The GDD's lines, or false with a reason. */
	static bool LoadGdd(TArray<FString>& OutLines, FString& OutError)
	{
		if (!FFileHelper::LoadFileToStringArray(OutLines, *GddPath()))
		{
			OutError = FString::Printf(TEXT("the tracked GDD snapshot is unreadable at %s"), *GddPath());
			return false;
		}
		if (OutLines.Num() == 0)
		{
			OutError = TEXT("the tracked GDD snapshot is empty");
			return false;
		}
		return true;
	}

	/** The first line beginning with `Prefix`, case-SENSITIVELY, or INDEX_NONE. */
	static int32 FindLineStartingWith(const TArray<FString>& Lines, const FString& Prefix)
	{
		for (int32 I = 0; I < Lines.Num(); ++I)
		{
			if (Lines[I].StartsWith(Prefix, ESearchCase::CaseSensitive))
			{
				return I;
			}
		}
		return INDEX_NONE;
	}

	/** The first line containing `Needle`, case-SENSITIVELY, or INDEX_NONE. */
	static int32 FindLineContaining(const TArray<FString>& Lines, const TCHAR* Needle)
	{
		for (int32 I = 0; I < Lines.Num(); ++I)
		{
			if (Lines[I].Contains(Needle, ESearchCase::CaseSensitive))
			{
				return I;
			}
		}
		return INDEX_NONE;
	}

	/**
	 * Every backtick-delimited run in one line, in order.
	 *
	 * §2.11.4's sample bullets put the authored copy in backticks, and the draw bullet puts
	 * TWO of them on one line ("neutral system voice" then "mutual passivity"). Returning the
	 * run of them rather than the first is what lets the draw pair be read as a pair.
	 */
	static TArray<FString> BacktickedRuns(const FString& Line)
	{
		TArray<FString> Runs;
		int32 Cursor = 0;
		while (Cursor < Line.Len())
		{
			const int32 Open = Line.Find(TEXT("`"), ESearchCase::CaseSensitive,
				ESearchDir::FromStart, Cursor);
			if (Open == INDEX_NONE)
			{
				break;
			}
			const int32 Close = Line.Find(TEXT("`"), ESearchCase::CaseSensitive,
				ESearchDir::FromStart, Open + 1);
			if (Close == INDEX_NONE)
			{
				break;
			}
			Runs.Add(Line.Mid(Open + 1, Close - Open - 1));
			Cursor = Close + 1;
		}
		return Runs;
	}

	/**
	 * §2.11.4's six authored result lines, keyed by the GDD's own bullet labels.
	 *
	 * THE LABELS ARE THE ANCHORS AND A MISSING ONE IS A FAILURE. If the GDD renames a bullet
	 * or moves the samples, these clauses go RED rather than quietly comparing against a
	 * shorter list -- which is the only way a document-sourced expectation can be trusted.
	 */
	struct FGddSamples
	{
		FString DirectorateDecisive;
		FString DirectorateMarginal;
		FString VanguardDecisive;
		FString VanguardMarginal;
		FString DrawAttrition;
		FString DrawPassivity;

		TArray<FString> All() const
		{
			return TArray<FString>{ DirectorateDecisive, DirectorateMarginal, VanguardDecisive,
			                        VanguardMarginal, DrawAttrition, DrawPassivity };
		}
	};

	static bool ReadOneSample(const TArray<FString>& Lines, const TCHAR* BulletPrefix,
	                          FString& Out, FString& OutError)
	{
		const int32 At = FindLineStartingWith(Lines, FString(BulletPrefix));
		if (At == INDEX_NONE)
		{
			OutError = FString::Printf(
				TEXT("§2.11.4's sample bullet '%s' is not in the GDD any more, so this clause has "
				     "no document to compare against"), BulletPrefix);
			return false;
		}

		const TArray<FString> Runs = BacktickedRuns(Lines[At]);
		if (Runs.Num() < 1)
		{
			OutError = FString::Printf(TEXT("§2.11.4's bullet '%s' carries no backticked sample"),
				BulletPrefix);
			return false;
		}
		Out = Runs[0];
		return true;
	}

	static bool ReadSamples(const TArray<FString>& Lines, FGddSamples& Out, FString& OutError)
	{
		if (!ReadOneSample(Lines, TEXT("- Directorate, decisive:"), Out.DirectorateDecisive, OutError) ||
		    !ReadOneSample(Lines, TEXT("- Directorate, marginal:"), Out.DirectorateMarginal, OutError) ||
		    !ReadOneSample(Lines, TEXT("- Vanguard, decisive:"),    Out.VanguardDecisive,    OutError) ||
		    !ReadOneSample(Lines, TEXT("- Vanguard, marginal:"),    Out.VanguardMarginal,    OutError))
		{
			return false;
		}

		// THE DRAW BULLET CARRIES BOTH DRAWS ON ONE LINE, in the GDD's own order: the neutral
		// attrition draw first, the mutual-passivity draw after the slash. Two runs are REQUIRED
		// -- a bullet that had lost one of them would otherwise silently pin only the other.
		const int32 DrawAt = FindLineStartingWith(Lines, FString(TEXT("- Draw, neutral system voice:")));
		if (DrawAt == INDEX_NONE)
		{
			OutError = TEXT("§2.11.4's draw sample bullet is not in the GDD any more");
			return false;
		}
		const TArray<FString> DrawRuns = BacktickedRuns(Lines[DrawAt]);
		if (DrawRuns.Num() != 2)
		{
			OutError = FString::Printf(
				TEXT("§2.11.4's draw bullet carries %d backticked samples; this clause needs the "
				     "two the GDD documents (neutral attrition, then mutual passivity)"),
				DrawRuns.Num());
			return false;
		}
		Out.DrawAttrition = DrawRuns[0];
		Out.DrawPassivity = DrawRuns[1];
		return true;
	}

	/**
	 * `kb/setting.md`'s word budget, PARSED out of §2.11.4's own paragraph rather than typed.
	 *
	 * The sentence is "...a result line is **<= 30 words**...". The number is extracted between
	 * the two anchors so that a re-budgeted GDD moves this clause instead of leaving it pinning
	 * a figure the document no longer states.
	 */
	static bool ReadWordBudget(const TArray<FString>& Lines, int32& Out, FString& OutError)
	{
		const int32 At = FindLineContaining(Lines, TEXT("a result line is"));
		if (At == INDEX_NONE)
		{
			OutError = TEXT("§2.11.4 no longer states a word budget for a result line");
			return false;
		}

		const FString& Line   = Lines[At];
		const FString  Anchor = TEXT("a result line is");
		const int32    From   = Line.Find(*Anchor, ESearchCase::CaseSensitive) + Anchor.Len();

		FString Digits;
		for (int32 I = From; I < Line.Len(); ++I)
		{
			const TCHAR C = Line[I];
			if (FChar::IsDigit(C))
			{
				Digits.AppendChar(C);
			}
			else if (Digits.Len() > 0)
			{
				break;
			}
		}

		if (Digits.Len() == 0)
		{
			OutError = FString::Printf(
				TEXT("§2.11.4's word-budget sentence carries no number: '%s'"), *Line);
			return false;
		}

		Out = FCString::Atoi(*Digits);
		return Out > 0;
	}

	/** Whitespace-separated words. §2.11.4 states a word budget and does not define a
	 *  tokeniser; this is the plain reading and it errs LONG, never short. */
	static int32 WordCount(const FString& Text)
	{
		TArray<FString> Words;
		Text.ParseIntoArrayWS(Words);
		return Words.Num();
	}

	/**
	 * `kb/setting.md`'s banned register. THE ONE STATED LITERAL IN THIS FILE.
	 *
	 * `kb/setting.md` is not in this repository -- it lives only in
	 * `E:\MultiAgent\stratocracy-content\kb\setting.md`, whose "Banned register" bullet reads
	 * "avoid melodrama and fantasy filler -- words like *destiny, glory, honor, legend, forever,
	 * epic, heroic, sacred, doom*." A headless run may not reach outside the repository, so
	 * these nine are CITED rather than read, and the change that would make them a read
	 * (vendoring `kb/setting.md` beside the GDD snapshot) is recorded in
	 * `Tools/architect/state/tests.md` rather than made here.
	 */
	static const TCHAR* const kBannedRegister[] = {
		TEXT("destiny"), TEXT("glory"), TEXT("honor"), TEXT("legend"), TEXT("forever"),
		TEXT("epic"), TEXT("heroic"), TEXT("sacred"), TEXT("doom")
	};

	/** Every enumerator `EStratResultTier` declares, off `StaticEnum` and never listed here. */
	static TArray<EStratResultTier> AllTiers()
	{
		TArray<EStratResultTier> Out;
		const UEnum* const E = StaticEnum<EStratResultTier>();
		for (int32 I = 0; E != nullptr && I < E->NumEnums(); ++I)
		{
			if (!E->HasMetaData(TEXT("Hidden"), I) && E->GetNameStringByIndex(I) != TEXT("EStratResultTier_MAX"))
			{
				Out.Add(static_cast<EStratResultTier>(E->GetValueByIndex(I)));
			}
		}
		return Out;
	}

	static TArray<EStratResultCause> AllCauses()
	{
		TArray<EStratResultCause> Out;
		const UEnum* const E = StaticEnum<EStratResultCause>();
		for (int32 I = 0; E != nullptr && I < E->NumEnums(); ++I)
		{
			if (!E->HasMetaData(TEXT("Hidden"), I) && E->GetNameStringByIndex(I) != TEXT("EStratResultCause_MAX"))
			{
				Out.Add(static_cast<EStratResultCause>(E->GetValueByIndex(I)));
			}
		}
		return Out;
	}

	static FString TierName(EStratResultTier T)
	{
		const UEnum* const E = StaticEnum<EStratResultTier>();
		return E != nullptr ? E->GetNameStringByValue(static_cast<int64>(T)) : TEXT("?");
	}

	static FString CauseName(EStratResultCause C)
	{
		const UEnum* const E = StaticEnum<EStratResultCause>();
		return E != nullptr ? E->GetNameStringByValue(static_cast<int64>(C)) : TEXT("?");
	}

	static FString CriterionName(EStratScoreCriterion C)
	{
		const UEnum* const E = StaticEnum<EStratScoreCriterion>();
		return E != nullptr ? E->GetNameStringByValue(static_cast<int64>(C)) : TEXT("?");
	}
}

// ---------------------------------------------------------------------------
// T-UI-03 -- §2.11.4's THREE ROWS ARE THE SCOREBOARD'S OWN MODEL, BY IDENTITY.
//
// WHAT THIS PROTECTS AND WHY WEAKENING IT WOULD BE INVISIBLE. §2.11.4 says the verdict shows
// "the same three rows in the same order, so the verdict is always a restatement of what was on
// screen all match", and revision §1.5-#1 forbids §2.8's tiebreak being a hidden win condition.
// A verdict screen that composed its own rows would be a SECOND implementation of that order --
// one that agrees today and is free to stop agreeing, in the exact place the revision was made
// to close. `StratMatchResultWidget.h` records embedding `FStratScoreboardModel` whole as "the
// single most load-bearing decision in this file"; this clause is what makes undoing it red.
//
// THE COMPARISON IS UE'S OWN REFLECTION WALK AND NOT A FIELD LIST, AND THAT IS THE CLAUSE.
// `UScriptStruct::CompareScriptStruct` enumerates every `FProperty` the struct declares, so a
// tenth field added to `FStratScoreboardModel` tomorrow is covered by this clause the moment it
// compiles. A hand-listed comparison of the nine fields there are today would be exactly the
// "agreement" this clause exists to refuse: it would go green on a builder that composed a new
// field itself, and nothing would say so. THE PROPERTY COUNT IS ASSERTED NON-ZERO first, because
// a reflection walk over a struct that reported no properties would compare nothing and pass.
//
// BOTH VIEWING SIDES, because `FStratScoreboardModel::ViewingSide` and its YOU/ENEMY columns are
// the one thing on this screen that legitimately turns on the viewer -- so a builder that passed
// the wrong side down would produce two models identical to each other and wrong for one of
// them. Comparing per side against that side's own scoreboard is what catches it.
//
// AND THE TWO MODELS ARE REQUIRED TO DIFFER BETWEEN THE SIDES. Without that, this clause is
// green on a `StratBuildScoreboardModel` that ignored `ViewingSide` entirely, and the identity
// above would be an identity between two copies of one wrong answer.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratResultRowsAreTheScoreboardModelTest,
	"Stratocracy.StratUI.T-UI-03.ResultScreenRowsAreTheScoreboardModelByIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratResultRowsAreTheScoreboardModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	UScriptStruct* const Struct = FStratScoreboardModel::StaticStruct();
	if (!TestNotNull(TEXT("FStratScoreboardModel is a reflected struct"), Struct))
	{
		return false;
	}

	// THE COVERAGE GUARD. A walk over a struct with no properties compares nothing.
	int32 PropertyCount = 0;
	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		++PropertyCount;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("FStratScoreboardModel declares reflected properties for the walk below to "
			     "compare (%d)"), PropertyCount),
			PropertyCount > 0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("the identity walk covers %d reflected properties of "
		"FStratScoreboardModel"), PropertyCount));

	FStratMatchResultModel PerSide[2];

	for (int32 ViewingSide = 0; ViewingSide < 2; ++ViewingSide)
	{
		// ---- The screen's rows -------------------------------------------------
		FString ResultReason;
		if (!TestTrue(*FString::Printf(
					TEXT("StratBuildMatchResultModel answers for viewing side %d"), ViewingSide),
				StratBuildMatchResultModel(Bridge, ViewingSide, PerSide[ViewingSide], ResultReason)))
		{
			AddError(ResultReason);
			return false;
		}

		// ---- The live panel's own builder, same bridge, same side, same frame ---
		FStratScoreboardModel Panel;
		FString               PanelReason;
		if (!TestTrue(*FString::Printf(
					TEXT("StratBuildScoreboardModel answers the same bridge for side %d"), ViewingSide),
				StratBuildScoreboardModel(Bridge, ViewingSide, Panel, PanelReason)))
		{
			AddError(PanelReason);
			return false;
		}

		// ---- IDENTITY, over every property the struct declares -----------------
		const bool bIdentical = Struct->CompareScriptStruct(
			&PerSide[ViewingSide].Scoreboard, &Panel, PPF_None);

		if (!bIdentical)
		{
			// Name the offending property rather than saying "they differ". The walk is the
			// same one `CompareScriptStruct` performs; this is only the report.
			TArray<FString> Differing;
			for (TFieldIterator<FProperty> It(Struct); It; ++It)
			{
				if (!It->Identical_InContainer(&PerSide[ViewingSide].Scoreboard, &Panel))
				{
					Differing.Add(It->GetName());
				}
			}
			AddError(FString::Printf(
				TEXT("§2.11.4's rows are NOT the scoreboard's model for viewing side %d: %s "
				     "differ. The verdict screen has begun composing §2.8's order a second way, "
				     "which is what revision §1.5-#1 forbids."),
				ViewingSide,
				Differing.Num() > 0 ? *FString::Join(Differing, TEXT(", ")) : TEXT("<none named>")));
		}

		TestTrue(*FString::Printf(
				TEXT("§2.11.4's embedded scoreboard is field-for-field identical to "
				     "StratBuildScoreboardModel's for viewing side %d"), ViewingSide),
			bIdentical);

		// And the model says which side it was built for, so a green above cannot mean "the
		// two agree because both were built for side 0".
		TestEqual(*FString::Printf(TEXT("the embedded scoreboard names viewing side %d"), ViewingSide),
			PerSide[ViewingSide].Scoreboard.ViewingSide, ViewingSide);
		TestEqual(*FString::Printf(TEXT("and the result model names the same viewing side (%d)"),
				ViewingSide),
			PerSide[ViewingSide].ViewingSide, ViewingSide);
	}

	// ---- THE NEGATIVE CONTROL -------------------------------------------------
	// If the two sides' models were identical, the identity above would hold just as well over
	// a builder that ignored `ViewingSide`, and this clause would be measuring nothing about
	// the YOU/ENEMY assignment §2.11.4 depends on.
	TestFalse(
		TEXT("the two viewing sides produce DIFFERENT scoreboard models, so the identity above "
		     "is not an identity between two copies of one side's answer"),
		Struct->CompareScriptStruct(&PerSide[0].Scoreboard, &PerSide[1].Scoreboard, PPF_None));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- THE SIX LINES ARE THE GDD's, VERBATIM, AND INSIDE `kb/setting.md`'s BUDGET.
//
// THE EXPECTATION IS PARSED OUT OF THE DOCUMENT, WHICH IS THE WHOLE POINT. `StratResultLineFor`
// hardcodes six strings; a clause that hardcoded the same six would be a second transcription
// of the same author's typing, would agree with a typo, and would keep agreeing after §2.11.4
// was rewritten. So the six come out of `Tools/architect/gdd_snapshot/`'s tracked GDD at run
// time, keyed by §2.11.4's own bullet labels, and a missing label is a FAILURE rather than a
// shorter list.
//
// AND THE MAPPING IS ASSERTED, NOT JUST THE SET. Checking that each of the six code strings is
// "one of the GDD's six" would pass on an implementation that gave the Vanguard the
// Directorate's line. Every one of the six is compared against the bullet whose label names its
// case: faction, tier, and for the two draws, cause.
//
// THE BUDGET IS PARSED TOO. `<= 30 words` is §2.11.4's own sentence and 30 is never typed here.
//
// THE NINE BANNED WORDS ARE THE ONE LITERAL. `kb/setting.md` is not in this repository; see
// `kBannedRegister`'s block, which cites the bullet and names the change that would make it a
// read.
//
// CASE SENSITIVE THROUGHOUT. `TestEqual` on two `FString`s is not, so the comparisons below are
// `FString::Equals(..., ESearchCase::CaseSensitive)` reported through `TestTrue`. Without that,
// a `StratResultLineFor` that returned `THE CAP IS REACHED...` would pass.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratResultLinesAreTheGddSamplesTest,
	"Stratocracy.StratUI.T-UI-03.ResultLinesAreTheGddSamplesVerbatimAndInsideTheVoiceBudget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratResultLinesAreTheGddSamplesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	TArray<FString> Gdd;
	FString         Error;
	if (!TestTrue(TEXT("the tracked GDD snapshot loads"), LoadGdd(Gdd, Error)))
	{
		AddError(Error);
		return false;
	}

	FGddSamples Samples;
	if (!TestTrue(TEXT("§2.11.4's six sample result lines are still in the GDD"),
			ReadSamples(Gdd, Samples, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 WordBudget = 0;
	if (!TestTrue(TEXT("§2.11.4 still states kb/setting.md's word budget for a result line"),
			ReadWordBudget(Gdd, WordBudget, Error)))
	{
		AddError(Error);
		return false;
	}
	AddInfo(FString::Printf(TEXT("§2.11.4's stated budget, read out of the document: %d words"),
		WordBudget));

	// ---- The six, each against the bullet that names its case ------------------
	struct FCase
	{
		const TCHAR*      What;
		EStratResultTier  Tier;
		EStratResultCause Cause;
		int32             Winner;
		const FString*    Expected;
	};

	// `Winner` is a SIDE INDEX and the faction binding is `SideFaction`'s -- side 0 is the
	// Directorate, side 1 the Vanguard. That binding is this project's harness pin rather than
	// the GDD's, and `StratMatchResultWidget.h` says so at length; it is read here through
	// `SideFaction` rather than assumed, immediately below.
	TestEqual(TEXT("SideFaction pins side 0 to the Directorate"),
		static_cast<int32>(SideFaction(0)), static_cast<int32>(EStratFaction::Directorate));
	TestEqual(TEXT("SideFaction pins side 1 to the Vanguard"),
		static_cast<int32>(SideFaction(1)), static_cast<int32>(EStratFaction::Vanguard));
	TestEqual(TEXT("and no winner is no faction"),
		static_cast<int32>(SideFaction(INDEX_NONE)), static_cast<int32>(EStratFaction::None));

	const FCase Cases[] = {
		{ TEXT("Directorate, decisive"), EStratResultTier::Decisive, EStratResultCause::FlagDestroyed,
		  0, &Samples.DirectorateDecisive },
		{ TEXT("Directorate, marginal"), EStratResultTier::Marginal, EStratResultCause::AttritionLead,
		  0, &Samples.DirectorateMarginal },
		{ TEXT("Vanguard, decisive"), EStratResultTier::Decisive, EStratResultCause::FlagDestroyed,
		  1, &Samples.VanguardDecisive },
		{ TEXT("Vanguard, marginal"), EStratResultTier::Marginal, EStratResultCause::AttritionLead,
		  1, &Samples.VanguardMarginal },
		{ TEXT("Draw, neutral attrition"), EStratResultTier::Draw, EStratResultCause::AllKeysTied,
		  INDEX_NONE, &Samples.DrawAttrition },
		{ TEXT("Draw, mutual passivity"), EStratResultTier::Draw, EStratResultCause::PassivityGuard,
		  INDEX_NONE, &Samples.DrawPassivity },
	};

	for (const FCase& C : Cases)
	{
		const FString Line = StratResultLineFor(C.Tier, C.Cause, C.Winner).ToString();

		TestTrue(*FString::Printf(
				TEXT("§2.11.4's '%s' line is the GDD's own, byte for byte: got '%s', the document "
				     "says '%s'"), C.What, *Line, **C.Expected),
			Line.Equals(**C.Expected, ESearchCase::CaseSensitive));

		// ---- kb/setting.md's budget, against the number the GDD states ----------
		const int32 Words = WordCount(Line);
		TestTrue(*FString::Printf(
				TEXT("'%s' is inside §2.11.4's stated budget (%d words of %d)"),
				C.What, Words, WordBudget),
			Words <= WordBudget && Words > 0);

		// ---- kb/setting.md's banned register ------------------------------------
		for (const TCHAR* const Banned : kBannedRegister)
		{
			TestFalse(*FString::Printf(
					TEXT("'%s' does not reach for kb/setting.md's banned register ('%s'): '%s'"),
					C.What, Banned, *Line),
				Line.Contains(Banned, ESearchCase::IgnoreCase));
		}
	}

	// ---- §2.8's three words at the top of the screen, also the GDD's ------------
	// §2.11.4: "the tier first (§2.8 -- Decisive / Marginal / Draw)". The three words are read
	// out of that sentence rather than typed, and `InProgress` is required to be EMPTY -- a
	// fourth word would be this project inventing copy §2.11.4 does not have.
	{
		const int32 At = FindLineContaining(Gdd, TEXT("**End-of-match screen.**"));
		if (TestTrue(TEXT("§2.11.4's end-of-match paragraph is still in the GDD"), At != INDEX_NONE))
		{
			const FString& Para = Gdd[At];
			for (const EStratResultTier Tier : AllTiers())
			{
				const FString Word = StratResultTierTextFor(Tier).ToString();
				if (Tier == EStratResultTier::InProgress)
				{
					TestTrue(TEXT("an in-progress match has no tier word -- §2.11.4 writes none"),
						Word.IsEmpty());
					continue;
				}

				TestFalse(*FString::Printf(TEXT("tier '%s' has a word"), *TierName(Tier)),
					Word.IsEmpty());
				TestTrue(*FString::Printf(
						TEXT("§2.11.4's paragraph names the tier word '%s' this screen prints for %s"),
						*Word, *TierName(Tier)),
					Para.Contains(*Word, ESearchCase::CaseSensitive));
			}
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- THE VOICE FOLLOWS THE WINNER AND NEVER THE VIEWER.
//
// THE DEFECT THIS EXISTS TO MAKE RED, IN ONE SENTENCE. `SideFaction(ViewingSide)` in place of
// `SideFaction(Result.Winner)` inside `StratBuildMatchResultModel` produces a screen that speaks
// in the VIEWER's faction voice -- and in single-seat play the two agree, so it ships green. It
// disagrees on exactly the hot-seat turn where the loser is looking at the screen, which is the
// one moment §2.11.4's faction voice is doing any work at all.
//
// THIS CLAUSE IS FALSIFIABLE AND THE PROOF WAS RUN, NOT INFERRED. The mutation above was applied
// to `Source/StratUI/StratMatchResultWidget.cpp`, the suite rebuilt and re-run, and this clause
// observed RED; the module was then restored to its committed bytes, rebuilt and re-run, and it
// observed GREEN. Both directions are quoted in the run report for this pass. An inference from
// a diff would not have been a measurement.
//
// IT NEEDS A REAL WINNER AND IT PLAYS FOR ONE. §2.9's AI drives both sides on the shipped
// scenario until the rules module declares a §2.8 result -- see `FAiDriver`. Nothing is planted:
// the tier, the cause and the winner are the module's answers about a game it played, and
// T-TURN-09's determinism is what makes that a fixed property rather than a coin toss.
//
// A DRAW WOULD MAKE THIS CLAUSE VACUOUS AND IT SAYS SO INSTEAD OF PASSING. With no winner both
// spellings return `None` and the mutation is invisible, so a drawn game fails this clause
// loudly rather than going green over a comparison that could not distinguish anything.
//
// THE THREE ASSERTIONS ARE ONE PROPERTY IN THREE PLACES. `ResultLine` and `WinnerFaction` must be
// IDENTICAL across the two viewing sides -- that is the voice not following the viewer -- while
// `bViewerWon` must be OPPOSITE, which is what stops the first two being satisfied by a builder
// that ignored `ViewingSide` altogether.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratResultVoiceFollowsTheWinnerTest,
	"Stratocracy.StratUI.T-UI-03.ResultVoiceFollowsTheWinnerAndNotTheViewer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratResultVoiceFollowsTheWinnerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FAiDriver              Driver;
	FStratMatchResultView  Played;
	if (!TestTrue(TEXT("§2.9's AI plays the shipped scenario to a §2.8 result"),
			Driver.Play(Bridge, Played, Error)))
	{
		AddError(Error);
		return false;
	}

	AddInfo(FString::Printf(
		TEXT("the AI-vs-AI game concluded after %d commands: tier=%s cause=%s winner=%d key=%d"),
		Driver.Commands, *TierName(Played.Tier), *CauseName(Played.Cause),
		Played.Winner, Played.DecidedByKey));

	// THE PREMISE, ASSERTED. Without a winner the two spellings agree and this clause could not
	// tell them apart -- which is a vacuous clause, not a passing one.
	if (!TestTrue(*FString::Printf(
				TEXT("the game produced a WINNER (tier %s, winner %d), which is the premise this "
				     "clause needs: with no winner, SideFaction(Winner) and SideFaction(ViewingSide) "
				     "cannot be distinguished and nothing below would be measuring anything"),
				*TierName(Played.Tier), Played.Winner),
			Played.Winner != INDEX_NONE))
	{
		return false;
	}

	// ---- The same concluded bridge, seen from both seats ------------------------
	FStratMatchResultModel AsSideZero;
	FStratMatchResultModel AsSideOne;
	FString                Reason;
	if (!TestTrue(TEXT("the verdict model builds for viewing side 0"),
			StratBuildMatchResultModel(Bridge, 0, AsSideZero, Reason)))
	{
		AddError(Reason);
		return false;
	}
	if (!TestTrue(TEXT("the verdict model builds for viewing side 1"),
			StratBuildMatchResultModel(Bridge, 1, AsSideOne, Reason)))
	{
		AddError(Reason);
		return false;
	}

	// THE WINNER IS THE MODULE'S, on both, and the two must agree about it before anything
	// downstream of it can be compared.
	TestEqual(TEXT("both seats are looking at the same match's winner"),
		AsSideZero.Winner, Played.Winner);
	TestEqual(TEXT("both seats are looking at the same match's winner"),
		AsSideOne.Winner, Played.Winner);

	// ---- THE CLAUSE ------------------------------------------------------------
	const FString LineZero = AsSideZero.ResultLine.ToString();
	const FString LineOne  = AsSideOne.ResultLine.ToString();

	TestFalse(TEXT("a decided match has a result line at all, so the equality below is not an "
	               "equality between two empty strings"),
		LineZero.IsEmpty());

	TestTrue(*FString::Printf(
			TEXT("§2.11.4's result line is the WINNER's and is the same from either seat: "
			     "side 0 sees '%s', side 1 sees '%s'"), *LineZero, *LineOne),
		LineZero.Equals(LineOne, ESearchCase::CaseSensitive));

	TestEqual(
		TEXT("and the faction the voice belongs to is the same from either seat"),
		static_cast<int32>(AsSideZero.WinnerFaction), static_cast<int32>(AsSideOne.WinnerFaction));

	TestEqual(
		TEXT("which is the WINNER's faction, read through SideFaction on the module's own winner"),
		static_cast<int32>(AsSideZero.WinnerFaction), static_cast<int32>(SideFaction(Played.Winner)));

	// ---- WHAT DOES TURN ON THE SEAT, which is what stops the three above being satisfied
	// ---- by a builder that ignored `ViewingSide` entirely.
	TestNotEqual(
		TEXT("but `bViewerWon` is OPPOSITE across the two seats -- exactly one of them won"),
		AsSideZero.bViewerWon, AsSideOne.bViewerWon);
	TestNotEqual(
		TEXT("and so is `bViewerLost`"),
		AsSideZero.bViewerLost, AsSideOne.bViewerLost);
	TestEqual(TEXT("the seat that won is the winning side, read off the module's winner"),
		AsSideZero.bViewerWon, Played.Winner == 0);

	// A decided match is not a draw, on either seat, and `bIsDraw` is read off the TIER rather
	// than off `Winner == INDEX_NONE` -- the two agree here and stop agreeing on an in-progress
	// result, which is why the builder reads the tier.
	TestFalse(TEXT("a decided match is not a draw from seat 0"), AsSideZero.bIsDraw);
	TestFalse(TEXT("a decided match is not a draw from seat 1"), AsSideOne.bIsDraw);

	// ---- THE MUTATION WITNESS -------------------------------------------------
	//
	// WHAT THIS IS AND WHY IT IS IN THE CLAUSE RATHER THAN IN A REPORT. The three assertions
	// above are only worth what the defect they can distinguish is worth, and "this clause would
	// redden if the module said `SideFaction(ViewingSide)`" is a claim that has to be MEASURED
	// on the data the clause actually ran over -- not asserted, and not inferred from a diff.
	//
	// The mutation is a one-argument substitution and nothing else: `StratBuildMatchResultModel`
	// copies, looks up, and compares, so `StratResultLineFor(Tier, Cause, ViewingSide)` and
	// `SideFaction(ViewingSide)` ARE the mutant's two outputs, reconstructed here from the same
	// two `STRATUI_API` functions the module itself calls. Nothing is modelled or approximated.
	//
	// THE ASSERTION IS THAT THE TWO SPELLINGS ARE DISTINGUISHABLE ON THIS BOARD -- the mutant
	// disagrees with the shipped answer on at least one seat. If it ever stopped being
	// distinguishable, the three assertions above would go quietly vacuous while staying green,
	// which is the single way this clause could stop covering its subject, and this is the line
	// that would say so.
	//
	// IT IS PERMANENT ON PURPOSE. A temporary edit to `Source/StratUI/StratMatchResultWidget.cpp`
	// would have proved the same thing once, for one reader, and left nothing behind -- and it is
	// outside this lane, which the lane rule forbids even when the intent is to revert.
	{
		const FString MutantLineZero =
			StratResultLineFor(Played.Tier, Played.Cause, /*ViewingSide*/ 0).ToString();
		const FString MutantLineOne =
			StratResultLineFor(Played.Tier, Played.Cause, /*ViewingSide*/ 1).ToString();
		const EStratFaction MutantFactionZero = SideFaction(0);
		const EStratFaction MutantFactionOne  = SideFaction(1);

		const bool bLinesDiffer =
			!MutantLineZero.Equals(LineZero, ESearchCase::CaseSensitive) ||
			!MutantLineOne.Equals(LineOne, ESearchCase::CaseSensitive);
		const bool bFactionsDiffer =
			MutantFactionZero != AsSideZero.WinnerFaction ||
			MutantFactionOne  != AsSideOne.WinnerFaction;

		AddInfo(FString::Printf(
			TEXT("mutation witness -- had the module read the VIEWER instead of the winner, seat 0 "
			     "would show '%s' and seat 1 '%s'; it shows '%s' from both"),
			*MutantLineZero, *MutantLineOne, *LineZero));

		TestTrue(
			TEXT("the viewer-voiced spelling produces a DIFFERENT result line from the shipped "
			     "winner-voiced one on this board, so the equality assertions above are a real "
			     "discrimination and not a comparison of two answers that could not have differed"),
			bLinesDiffer);

		TestTrue(
			TEXT("and a different WinnerFaction, so the faction assertion above discriminates too"),
			bFactionsDiffer);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- THE LINE MAPPING IS TOTAL, AND CLOSED OVER §2.11.4's AUTHORED SET.
//
// TOTALITY IS THE POINT RATHER THAN A COURTESY, and `StratResultLineFor`'s own block says so.
// The function is driven over EVERY `(Tier, Cause, Winner)` triple the engine types can express
// -- the tiers and causes from `StaticEnum`'s own walk, so a tier or cause added upstream is
// REQUIRED to be handled here rather than quietly falling into a default -- crossed with the
// three winner values a `strat` side index can take on this board.
//
// TWO PROPERTIES, AND THE SECOND IS THE ONE THAT HAS TEETH.
//   (a) THE SHAPE: a Draw, and a Decisive or Marginal with a real faction, is NON-EMPTY;
//       `InProgress` is EMPTY on every cause and every winner, and a decided tier with no
//       winning side is EMPTY -- unreachable through the rules module, representable in these
//       types, and mapped rather than asserted on, which is the choice the header records.
//   (b) THE CONTENT: every non-empty answer is one of §2.11.4's six authored samples, READ OUT
//       OF THE GDD. Without (b), (a) is satisfied by a function that returns "x" for every
//       decided triple. This is what makes totality mean "always one of the GDD's lines" rather
//       than "always something".
//
// AND THE CAUSE SPLITS THE TWO DRAWS AND NOTHING ELSE. §2.11.4 lists two draw samples;
// `PassivityGuard` is the mutual-passivity one and every other cause is the neutral attrition
// one. That is asserted directly, and separately it is asserted that the cause changes NOTHING
// on a Decisive or Marginal tier -- so a future edit that started branching a faction line on
// the cause would be red here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratResultLineMappingIsTotalTest,
	"Stratocracy.StratUI.T-UI-03.ResultLineMappingIsTotalOverEveryTierCauseAndWinner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratResultLineMappingIsTotalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	TArray<FString> Gdd;
	FString         Error;
	if (!TestTrue(TEXT("the tracked GDD snapshot loads"), LoadGdd(Gdd, Error)))
	{
		AddError(Error);
		return false;
	}

	FGddSamples Samples;
	if (!TestTrue(TEXT("§2.11.4's six sample result lines are still in the GDD"),
			ReadSamples(Gdd, Samples, Error)))
	{
		AddError(Error);
		return false;
	}
	const TArray<FString> Authored = Samples.All();

	const TArray<EStratResultTier>  Tiers  = AllTiers();
	const TArray<EStratResultCause> Causes = AllCauses();

	// THE COVERAGE GUARD. An empty enumerator walk drives an empty loop, which is a green
	// clause over nothing.
	if (!TestTrue(*FString::Printf(TEXT("the tier enum walk found enumerators (%d)"), Tiers.Num()),
			Tiers.Num() >= 4) ||
	    !TestTrue(*FString::Printf(TEXT("the cause enum walk found enumerators (%d)"), Causes.Num()),
			Causes.Num() >= 6))
	{
		return false;
	}

	// -1 is `INDEX_NONE`, which `strat::SIDE_NONE` equals; 0 and 1 are the two sides
	// `SideFaction` binds. Nothing else is representable as a winner on this board.
	const int32 Winners[] = { INDEX_NONE, 0, 1 };

	int32 Triples  = 0;
	int32 NonEmpty = 0;

	for (const EStratResultTier Tier : Tiers)
	{
		for (const EStratResultCause Cause : Causes)
		{
			for (const int32 Winner : Winners)
			{
				++Triples;
				const FString Line = StratResultLineFor(Tier, Cause, Winner).ToString();
				const FString Where = FString::Printf(TEXT("(tier=%s cause=%s winner=%d)"),
					*TierName(Tier), *CauseName(Cause), Winner);

				const bool bDecided = (Tier == EStratResultTier::Decisive ||
				                       Tier == EStratResultTier::Marginal);
				const bool bHasFaction = (SideFaction(Winner) != EStratFaction::None);

				if (Tier == EStratResultTier::InProgress)
				{
					TestTrue(*FString::Printf(
							TEXT("an in-progress match has NO line %s -- §2.11.4 writes none, and "
							     "an empty answer is the honest one rather than a placeholder"),
							*Where),
						Line.IsEmpty());
					continue;
				}

				if (bDecided && !bHasFaction)
				{
					TestTrue(*FString::Printf(
							TEXT("a decided tier with no winning side has no line %s -- it is "
							     "unreachable through the rules module and mapped rather than "
							     "given words that would be a lie about who won"), *Where),
						Line.IsEmpty());
					continue;
				}

				// Draw with any cause, or Decisive/Marginal with a real faction.
				if (!TestFalse(*FString::Printf(TEXT("§2.11.4 has a line for %s"), *Where),
						Line.IsEmpty()))
				{
					continue;
				}
				++NonEmpty;

				// (b) -- CLOSED OVER THE DOCUMENT'S SIX.
				bool bAuthored = false;
				for (const FString& Sample : Authored)
				{
					if (Line.Equals(Sample, ESearchCase::CaseSensitive))
					{
						bAuthored = true;
						break;
					}
				}
				TestTrue(*FString::Printf(
						TEXT("and the line for %s is one of §2.11.4's six authored samples, not "
						     "composed copy: '%s'"), *Where, *Line),
					bAuthored);

				// The two draws, split by the cause and by nothing else.
				if (Tier == EStratResultTier::Draw)
				{
					const FString& Expected = (Cause == EStratResultCause::PassivityGuard)
						? Samples.DrawPassivity
						: Samples.DrawAttrition;
					TestTrue(*FString::Printf(
							TEXT("the draw line for %s is the GDD's %s draw: '%s'"), *Where,
							Cause == EStratResultCause::PassivityGuard
								? TEXT("mutual-passivity") : TEXT("neutral attrition"),
							*Line),
						Line.Equals(Expected, ESearchCase::CaseSensitive));
				}
				else
				{
					// THE CAUSE MUST NOT REACH A FACTION LINE. Compared against the same tier
					// and winner under a DIFFERENT cause, read from the function itself rather
					// than from a table -- so this holds for every cause the enum declares.
					const EStratResultCause Other = (Cause == EStratResultCause::None)
						? EStratResultCause::FlagDestroyed
						: EStratResultCause::None;
					const FString Alternative = StratResultLineFor(Tier, Other, Winner).ToString();
					TestTrue(*FString::Printf(
							TEXT("a faction line does not branch on the cause %s: cause %s gives "
							     "the same line"), *Where, *CauseName(Other)),
						Line.Equals(Alternative, ESearchCase::CaseSensitive));
				}
			}
		}
	}

	AddInfo(FString::Printf(TEXT("%d (tier, cause, winner) triples driven, %d of them with a line"),
		Triples, NonEmpty));
	TestTrue(TEXT("the walk drove a non-trivial number of triples"), Triples >= 4 * 6 * 3);
	TestTrue(TEXT("and reached the authored lines, so the closure assertion measured something"),
		NonEmpty > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- `DecidedByKey` REACHES THE SCREEN AS A CRITERION, AND THE SHIFT IS NOT THE WBP's.
//
// WHAT THE SHIFT IS AND WHY IT IS THE SUBJECT. §2.8 numbers its tiebreak keys 1, 2, 3;
// `EStratScoreCriterion` numbers the same three criteria 0, 1, 2. Something has to subtract one,
// and T-UI-03 ends "with no widget-side arithmetic" -- so `StratBuildMatchResultModel` does it
// in a `switch` and `FStratMatchResultModel` carries the answer as a TAG. A graph doing the
// subtraction instead is the exact defect T-UI-03 names, and it would be invisible: the screen
// would highlight the wrong row and every number on it would still be right.
//
// THE SHIFT IS PINNED AS A RELATION BETWEEN TWO MODULE-SIDE ARTIFACTS, NOT AS A TYPED TABLE.
// `FStratMatchResultModel::Scoreboard.Rows` is `StratBuildScoreboardModel`'s own row array in
// §2.8's own criteria order, and §2.8's key N names its Nth criterion, 1-based. So the deciding
// criterion is required to be `Rows[DecidedByKey - 1].Criterion`. Both operands are the module's
// and the only thing this clause supplies is the statement that §2.8 counts from one.
//
// WHAT THIS CLAUSE DOES NOT PIN, AND WHAT NOW DOES. [Stamped 2026-08-25: this block used to
// end by naming the module-side change as OUTSTANDING. It has landed.] Keys 2 and 3 are still
// not reachable BY PLAYING -- they need a capped match in which both sides fought to an EQUAL
// combat Fame, which no bridge this suite can build produces -- so this clause still measures
// the key-0 arm on the shipped scenario. The other two arms are pinned by
// `EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion` at the bottom of this file, which calls
// `StratScoreCriterionForKey` directly. THIS CLAUSE IS THE END-TO-END ONE and is not made
// redundant by it: it is the only place the key travels from the rules module, through
// `StratBuildMatchResultModel`, onto the model a widget reads.
//
// KEY 0 IS THE COMMON CASE AND IS PINNED SEPARATELY. A flag kill ends a match without evaluating
// a key at all, and `bHasDecidedBy` false is what tells the WBP not to highlight anything --
// which is why the flag exists instead of a sentinel criterion value, every enumerator of
// `EStratScoreCriterion` naming a real row a display could accidentally mark.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDecidedByKeyMapsToTheDecidingRowTest,
	"Stratocracy.StratUI.T-UI-03.DecidedByKeyReachesTheScreenAsACriterionAndNotAsArithmetic",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDecidedByKeyMapsToTheDecidingRowTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	// ---- KEY 0: the seeded opening has evaluated no key -------------------------
	{
		FStratBridge Bridge;
		FString      Error;
		if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
		{
			AddError(Error);
			return false;
		}

		FStratMatchResultModel Opening;
		FString                Reason;
		if (!TestTrue(TEXT("the verdict model builds over the opening"),
				StratBuildMatchResultModel(Bridge, kFirstSide, Opening, Reason)))
		{
			AddError(Reason);
			return false;
		}

		// The key is the module's, read off the model rather than written down.
		TestEqual(TEXT("§2.8 has evaluated no key at the opening"), Opening.DecidedByKey, 0);
		TestFalse(TEXT("so `bHasDecidedBy` says the criterion tag must not be read"),
			Opening.bHasDecidedBy);
	}

	// ---- WHATEVER KEY A PLAYED MATCH PRODUCES ----------------------------------
	{
		FStratBridge Bridge;
		FString      Error;
		if (!TestTrue(TEXT("the bridge seeds for the played match"), SeedBridge(Bridge, Error)))
		{
			AddError(Error);
			return false;
		}

		FAiDriver             Driver;
		FStratMatchResultView Played;
		if (!TestTrue(TEXT("§2.9's AI plays the shipped scenario to a §2.8 result"),
				Driver.Play(Bridge, Played, Error)))
		{
			AddError(Error);
			return false;
		}

		FStratMatchResultModel Model;
		FString                Reason;
		if (!TestTrue(TEXT("the verdict model builds over the concluded match"),
				StratBuildMatchResultModel(Bridge, kFirstSide, Model, Reason)))
		{
			AddError(Reason);
			return false;
		}

		// The raw key survives the projection unchanged -- the header's "carried raw beside the
		// tag, both set from one read of one field".
		TestEqual(TEXT("the model's raw key is the rules module's own"),
			Model.DecidedByKey, Played.DecidedByKey);

		AddInfo(FString::Printf(
			TEXT("the played match concluded with decidedByKey=%d (tier %s, cause %s)"),
			Model.DecidedByKey, *TierName(Model.Tier), *CauseName(Model.Cause)));

		// THE FLAG IS THE KEY'S OWN ANSWER, in both directions.
		TestEqual(
			TEXT("`bHasDecidedBy` is true exactly when §2.8 evaluated a key"),
			Model.bHasDecidedBy, Model.DecidedByKey != 0);

		if (Model.bHasDecidedBy)
		{
			// THE SHIFT. §2.8 counts its keys from one; the scoreboard's rows are those keys in
			// that order, and the tag must name the row the key names.
			const int32 RowIndex = Model.DecidedByKey - 1;
			if (TestTrue(*FString::Printf(
						TEXT("§2.8's key %d names a row of the embedded scoreboard (%d rows)"),
						Model.DecidedByKey, Model.Scoreboard.Rows.Num()),
					Model.Scoreboard.Rows.IsValidIndex(RowIndex)))
			{
				TestEqual(*FString::Printf(
						TEXT("§2.8's key %d reaches the screen as criterion '%s' -- the row the "
						     "scoreboard itself puts %d-th -- so no WBP has to subtract one"),
						Model.DecidedByKey, *CriterionName(Model.DecidedByCriterion),
						Model.DecidedByKey),
					static_cast<int32>(Model.DecidedByCriterion),
					static_cast<int32>(Model.Scoreboard.Rows[RowIndex].Criterion));
			}
		}
		else
		{
			AddInfo(TEXT("this game ended without a tiebreak, so only the key-0 arm was measured "
			             "end to end here; keys 2 and 3 are pinned through StratScoreCriterionForKey "
			             "by EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion"));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- ALL THREE OF §2.8's KEYS REACH THE SCREEN AS THEIR OWN CRITERION.
//
// WHAT THIS CLOSES, AND IT WAS A REAL HOLE RATHER THAN A THEORETICAL ONE. The clause above
// pins the shift over whichever key the module actually produced -- and on the shipped
// scenario that key is always 0, because every game §2.9's AI plays there ends
// `Decisive / FlagDestroyed`. Keys 2 and 3 need a capped match in which both sides fought to
// an EQUAL combat Fame, which no bridge this suite can build produces. So a mapping that sent
// key 2 to `SurvivingHp` and key 3 to `Objectives` SHIPPED GREEN: the screen would mark the
// wrong row and every number on it would still be right.
//
// HOW IT IS CLOSED: BY CALLING THE MAPPING RATHER THAN BY REACHING IT THROUGH A MATCH.
// `StratScoreCriterionForKey` is the `STRATUI_API` seam `Tools/architect/state/tests.md` named
// as the discharge -- the switch that used to sit inline in `StratBuildMatchResultModel`, moved
// out so that all four classes of input are reachable from a test. The module-side change and
// this clause are the two halves of one fix; neither is worth anything alone.
//
// WHERE THE EXPECTATION COMES FROM, WHICH IS THE COLUMN THAT MATTERS. NOT a typed table of
// three pairs -- that would be this file transcribing the switch it is grading, and it would
// agree with a swapped mapping the moment somebody swapped both copies. The expectation is
// `StratBuildScoreboardModel`'s OWN rows on the seeded bridge: §2.11.4 orders those rows in
// §2.8's tiebreak order and §2.8 numbers its keys from one, so key N must name `Rows[N - 1]`.
// Both operands are the module's and the only thing this clause supplies is "§2.8 counts from
// one" -- the same statement, and the same two artifacts, the clause above relates.
//
// THE NON-KEYS ARE PINNED IN BOTH DIRECTIONS, and the second direction is the one that would
// go quiet. `false` alone is half the contract: the function must also LEAVE `OutCriterion`
// ALONE, because that is what lets the caller's default survive key 0 -- the common case, a
// flag kill having evaluated no key at all. Every call below pre-loads the out-parameter with
// a value the true answer is NOT, so "written when it should be" and "untouched when it should
// not be" are both measured rather than assumed.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEveryTiebreakKeyReachesItsOwnCriterionTest,
	"Stratocracy.StratUI.T-UI-03.EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEveryTiebreakKeyReachesItsOwnCriterionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultModel;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE ROWS ARE THE EXPECTATION. Built by the live panel's own builder, on this bridge, in
	// this frame -- §2.11.4's three §2.8 criteria in §2.8's order.
	FStratScoreboardModel Board;
	FString               Reason;
	if (!TestTrue(TEXT("the scoreboard model builds, and its rows are §2.8's order"),
			StratBuildScoreboardModel(Bridge, kFirstSide, Board, Reason)))
	{
		AddError(Reason);
		return false;
	}

	if (!TestEqual(TEXT("§2.8 has three keys, so the scoreboard has three rows"),
			Board.Rows.Num(), 3))
	{
		return false;
	}

	// ---- KEYS 1, 2 AND 3 -------------------------------------------------------
	for (int32 Key = 1; Key <= Board.Rows.Num(); ++Key)
	{
		const EStratScoreCriterion Expected = Board.Rows[Key - 1].Criterion;

		// Pre-loaded with the NEXT row's criterion, which is never this key's answer, so a
		// function that returned true without writing would be caught here rather than
		// coinciding with a default.
		const EStratScoreCriterion Preloaded = Board.Rows[Key % Board.Rows.Num()].Criterion;
		EStratScoreCriterion       Got       = Preloaded;

		if (!TestTrue(*FString::Printf(TEXT("§2.8's key %d is a key the screen can name"), Key),
				StratScoreCriterionForKey(Key, Got)))
		{
			continue;
		}

		TestEqual(*FString::Printf(
				TEXT("§2.8's key %d reaches the screen as criterion '%s' -- the row the "
				     "scoreboard itself puts %d-th -- so no WBP has to subtract one"),
				Key, *CriterionName(Expected), Key),
			static_cast<int32>(Got), static_cast<int32>(Expected));

		// The pre-load is only a witness if it was something else to begin with; without this
		// the assertion above would hold over a function that wrote nothing at all.
		TestNotEqual(*FString::Printf(
				TEXT("and the pre-loaded '%s' differed from it, so the write was measured"),
				*CriterionName(Preloaded)),
			static_cast<int32>(Preloaded), static_cast<int32>(Expected));
	}

	// ---- EVERY OTHER INPUT: false, AND THE OUT-PARAMETER UNTOUCHED --------------
	// 0 is the common case and the rest are keys §2.8 does not have. They share one arm on
	// purpose -- see the function's own block -- and they are asserted separately so that a
	// mapping which special-cased 0 alone could not pass.
	{
		// `INDEX_NONE` is not listed separately: it IS -1.
		const int32 NonKeys[] = { 0, -1, 4, TNumericLimits<int32>::Max(),
		                          TNumericLimits<int32>::Min() };

		for (const int32 Key : NonKeys)
		{
			// Every one of the three real criteria, so "untouched" is not measured against a
			// single lucky value.
			for (const FStratScoreboardRow& Witness : Board.Rows)
			{
				EStratScoreCriterion Got = Witness.Criterion;

				TestFalse(*FString::Printf(
						TEXT("%d is not one of §2.8's three keys"), Key),
					StratScoreCriterionForKey(Key, Got));

				TestEqual(*FString::Printf(
						TEXT("and a refusal left the caller's '%s' untouched, which is what "
						     "lets key 0 keep a default instead of naming a row"),
						*CriterionName(Witness.Criterion)),
					static_cast<int32>(Got), static_cast<int32>(Witness.Criterion));
			}
		}
	}

	// ---- THE COMPOSED MODEL CARRIES THE RIGHT TAG FOR EVERY KEY -- NO LONGER A LATCH ----
	// WHAT THIS BLOCK USED TO BE, SAID SO THE CHANGE IS LEGIBLE. It built the model over the
	// seeded opening, where `DecidedByKey == 0`, and asserted the model's tag against the seam's
	// answer for that key -- which meant `x == x` and `false == false`. It pinned nothing, and it
	// claimed in its own comment to pin that the builder CALLS the seam. `strat-integration-
	// reviewer` caught the tautology; the extraction below is what discharged it.
	//
	// AND THE PROPERTY IT CLAIMED WAS THE WRONG ONE. "The builder calls the seam rather than
	// keeping a private copy" is an implementation-identity claim, and NO black-box clause can
	// make it: a duplicate switch that is CORRECT is indistinguishable from a call, and a
	// duplicate switch that is WRONG is caught by the output. What actually protects the screen
	// is the OUTPUT -- the composed model's tag is right for every one of §2.8's keys -- and
	// that is what is asserted here. It is strictly stronger than the claim it replaces.
	//
	// HOW THE UNREACHABLE KEYS ARE REACHED: `StratComposeMatchResultModel` takes VALUES.
	// `FStratMatchResultView` is a plain `USTRUCT` with a public defaulted `int32 DecidedByKey`,
	// so the view below is HAND-AUTHORED with key 2 and key 3 -- states no bridge this suite can
	// build will ever report, since they need a capped match in which both sides fought to an
	// EQUAL combat Fame. The scoreboard handed alongside is the LIVE builder's, so the
	// expectation stays the module's own `Rows[N - 1].Criterion` and never a typed table.
	{
		for (int32 Key = 0; Key <= Board.Rows.Num(); ++Key)
		{
			// A hand-authored §2.8 result. Only the key varies; the rest is a plausible capped
			// match so nothing else on the model is nonsense while the tag is under test.
			FStratMatchResultView Authored;
			Authored.Tier         = (Key == 0) ? EStratResultTier::Draw : EStratResultTier::Marginal;
			Authored.Cause        = (Key == 0) ? EStratResultCause::AllKeysTied
			                                   : EStratResultCause::AttritionLead;
			Authored.Winner       = (Key == 0) ? INDEX_NONE : kFirstSide;
			Authored.DecidedByKey = Key;

			FStratMatchResultModel Composed;
			StratComposeMatchResultModel(Authored, Board, kFirstSide, Composed);

			// THE KEY SURVIVES THE COMPOSITION UNCHANGED -- the header's "carried raw beside the
			// tag, both set from one read of one field".
			TestEqual(*FString::Printf(TEXT("the composed model carries key %d unchanged"), Key),
				Composed.DecidedByKey, Key);

			TestEqual(*FString::Printf(
					TEXT("`bHasDecidedBy` is true for key %d exactly when §2.8 evaluated one"), Key),
				Composed.bHasDecidedBy, Key != 0);

			if (Key == 0)
			{
				// The one arm a played match DOES reach, and the reason the flag exists rather
				// than a sentinel criterion: the tag must stay at the model's own default, since
				// every enumerator names a real row a display could accidentally mark.
				const FStratMatchResultModel Untouched;
				TestEqual(
					TEXT("and key 0 leaves the criterion tag at the model's default, naming no row"),
					static_cast<int32>(Composed.DecidedByCriterion),
					static_cast<int32>(Untouched.DecidedByCriterion));
				continue;
			}

			// THE PIN. The expectation is the live scoreboard builder's own row order, not a
			// table written here: §2.11.4 orders those rows in §2.8's tiebreak order and §2.8
			// numbers its keys from one.
			//
			// KEY 1'S COMPARISON ALONE IS THE WEAK ONE, SAID SO NOBODY LEANS ON THE WRONG HALF.
			// `Rows[0].Criterion` is `CombatFame`, which is also `EStratScoreCriterion`'s zero and
			// the model's own default -- so a composer that wrote NO criterion at all would pass
			// this one line. What catches that composer is the `bHasDecidedBy` assertion in the
			// same iteration, which it fails. Keys 2 and 3 have no such coincidence.
			const EStratScoreCriterion Expected = Board.Rows[Key - 1].Criterion;

			TestEqual(*FString::Printf(
					TEXT("§2.8's key %d reaches the SCREEN as criterion '%s' -- the row the "
					     "scoreboard itself puts %d-th -- through the real composition path"),
					Key, *CriterionName(Expected), Key),
				static_cast<int32>(Composed.DecidedByCriterion), static_cast<int32>(Expected));
		}

		AddInfo(TEXT("keys 2 and 3 were driven through StratComposeMatchResultModel with a "
		             "hand-authored FStratMatchResultView -- states no bridge this suite can build "
		             "reports, which is why the composition was extracted from the bridge-taking "
		             "builder"));
	}

	// ---- AND THE BRIDGE-TAKING BUILDER IS THE SAME COMPOSITION -----------------
	// Without this the composer could be right and BYPASSED: `StratBuildMatchResultModel` could
	// compose its own model and every assertion above would still be green. Both models are
	// built from the same bridge in the same frame, so UE's reflection walk over
	// `FStratMatchResultModel` is an IDENTITY comparison and not a field list -- a field added to
	// that struct tomorrow is covered the moment it compiles.
	//
	// EXACTLY WHAT THIS FORECLOSES, AND THE QUALIFIER IS LOAD-BEARING: over EVERY FIELD, for the
	// ONE STATE a bridge can produce. The seeded opening carries `DecidedByKey == 0`, so a
	// bypassing builder that agreed here and diverged at key 2 would pass. That residue is
	// irreducible by black-box means -- driving the BUILDER at key 2 needs the bridge to report
	// key 2, which is the whole reason the composer was extracted -- and stating it is what keeps
	// this block from becoming the next over-claimed assertion.
	{
		FStratMatchResultModel ViaBridge;
		FString                BuildReason;
		if (TestTrue(TEXT("the bridge-taking builder builds over the seeded opening"),
				StratBuildMatchResultModel(Bridge, kFirstSide, ViaBridge, BuildReason)))
		{
			FStratMatchResultView Result;
			FString               ResultReason;
			if (TestTrue(TEXT("and §2.8's result projects on its own"),
					StratBuildMatchResult(Bridge, Result, ResultReason)))
			{
				FStratMatchResultModel ViaComposer;
				StratComposeMatchResultModel(Result, Board, kFirstSide, ViaComposer);

				const UScriptStruct* const ModelStruct = FStratMatchResultModel::StaticStruct();

				TestTrue(
					TEXT("the bridge-taking builder's model IS the composer's, field for field, "
					     "by UE's own reflection walk -- so on every state a bridge can reach, the "
					     "composition has one implementation"),
					ModelStruct->CompareScriptStruct(&ViaBridge, &ViaComposer, PPF_None));

				// THE NEGATIVE CONTROL FOR THE COMPARISON ITSELF, on
				// `StratSelectionMachineParity.cpp`'s precedent: if `CompareScriptStruct` reported
				// equality for two models that genuinely differ, the clause above would be
				// vacuous. The perturbation is the TAG -- the field this whole clause is about --
				// and it is applied to a copy so neither model above is disturbed.
				FStratMatchResultModel Perturbed = ViaComposer;
				Perturbed.DecidedByCriterion = (ViaComposer.DecidedByCriterion
						== EStratScoreCriterion::SurvivingHp)
					? EStratScoreCriterion::CombatFame
					: EStratScoreCriterion::SurvivingHp;

				TestFalse(
					TEXT("the comparison can SEE a difference in the criterion tag -- without this "
					     "the identity above would be satisfied by a walk that compared nothing"),
					ModelStruct->CompareScriptStruct(&ViaBridge, &Perturbed, PPF_None));
			}
			else
			{
				AddError(ResultReason);
			}
		}
		else
		{
			AddError(BuildReason);
		}
	}

	return true;
}
