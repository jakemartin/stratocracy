// THE STANDING CLAIM ABOUT THIS FILE, on `StratTransientReceipts.cpp`'s precedent and in the
// same spirit: THERE IS NOT ONE ARITHMETIC OPERATION IN IT. No subtraction, no sum, no ratio,
// no distance. Every decision below is an EQUALITY or an ORDER comparison between two readings
// of the same field -- `Hex != Hex`, `Hp < Hp`, `(Turn, SideToMove) != (Turn, SideToMove)` --
// and an id set membership. That is deliberate and it is what keeps this file out of
// T-UI-03's way: a hex DISTANCE would be movement arithmetic standing in for a rules answer,
// which is the substitution T-UI-02 exists to catch, and an HP DELTA would be the second
// subtraction `StratTransientReceipts.cpp`'s own claim already forbids. Neither is here, and
// neither is needed: a cue asks WHETHER something changed and never BY HOW MUCH.
//
// AND THE ONE-PER-KIND COLLAPSE IS A `bool[7]`, NOT A SORT, A DEDUPE OR A SET. The emissions
// land in a documented order and the first of each kind wins; nothing is compared to anything
// after it. That makes the header's rule 5 readable off ten lines of code rather than derived
// from a container's semantics.
//
// THE CLAIM SURVIVED `StratClampVolume` JOINING THIS FILE, AND IT IS RE-CHECKED HERE RATHER THAN
// ASSUMED TO STILL HOLD. That function is a volume rule rather than a cue rule -- its declaration
// records why one authority for it has to sit below both `StratUI` and `StratPlay`, and why this
// header, already included by `StratSoundBank.h`, is where it costs nothing. What matters for the
// sentence above is that it contains NO ARITHMETIC EITHER: `FMath::IsNaN` and `FMath::Clamp` are
// comparisons and a select, with no operand ever added to, subtracted from or scaled by another.
// THE PERCENT CONVERSION -- the one place a volume IS multiplied -- IS DELIBERATELY NOT HERE; it
// is in `StratOptionsWidget.cpp`, which owns the display side and says so in its own standing
// claim. Two files, two claims, one arithmetic expression between them.

#include "StratSoundCues.h"

#include "StratViewModel.h"

FStratSoundMark StratSoundMarkFromView(const FStratViewModel& Model)
{
	FStratSoundMark Mark;

	// SEEDED FIRST AND UNCONDITIONALLY, so that no early return below can leave a mark whose
	// contents were written and whose flag was not. There is no early return in this function
	// today; the ordering is what keeps that from mattering if one is ever added.
	Mark.bSeeded    = true;
	Mark.Turn       = Model.Match.Turn;
	Mark.SideToMove = Model.Match.SideToMove;

	Mark.Units.Reserve(Model.Units.Num());
	for (const FStratUnitView& Unit : Model.Units)
	{
		// FOUR FIELDS, NAMED ONE AT A TIME. See `FStratUnitSoundMark`: a struct copy would
		// silently enrol every future `FStratUnitView` field as an input to a cue decision.
		// `Side` is written here and compared nowhere below; that struct says why.
		FStratUnitSoundMark& Entry = Mark.Units.AddDefaulted_GetRef();
		Entry.UnitId = Unit.UnitId;
		Entry.Side   = Unit.Side;
		Entry.Hex    = Unit.Hex;
		Entry.Hp     = Unit.Hp;
	}

	return Mark;
}

void StratDecideSoundCues(const FStratSoundMark& Mark,
                          const FStratViewModel& Model,
                          TArray<FStratSoundEmission>& Out)
{
	// EMPTIED FIRST AND ON EVERY PATH, INCLUDING THE UNSEEDED ONE. The output describes THIS
	// refresh; a caller handed the same array twice must not find the first call's answer
	// still in it. See the declaration.
	Out.Reset();

	// RULE 1. An unseeded mark has no predecessor and therefore no edges. See
	// `FStratSoundMark`: without this line Ferrum Crossing's ten seeded units all read as
	// newly built on the first reconcile.
	if (!Mark.bSeeded)
	{
		return;
	}

	// THE ONE-PER-KIND GATE, AS A FIXED ARRAY INDEXED BY THE ENUM. `ButtonClick` and
	// `MatchEnded` have entries and are never set, which is cheaper than an arm that would
	// have to explain itself; the header block states why neither is decided here.
	bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1] = {};

	const auto Emit = [&Out, &bEmitted](const EStratSoundCue Cue,
	                                    const int32 Side,
	                                    const int32 UnitId,
	                                    const int32 Turn)
	{
		const int32 Slot = static_cast<int32>(Cue);
		if (bEmitted[Slot])
		{
			return;
		}
		bEmitted[Slot] = true;

		FStratSoundEmission& Emission = Out.AddDefaulted_GetRef();
		Emission.Cue    = Cue;
		Emission.Side   = Side;
		Emission.UnitId = UnitId;
		Emission.Turn   = Turn;
	};

	// RULE 2. THE PAIR AND NEVER EITHER HALF. `FStratReceiptMark::SideToMove` gives the two
	// counterexamples that make each half alone wrong, and they are not restated here.
	//
	// IT CARRIES THE MARK'S TURN AND THE MARK'S SIDE -- the turn that ENDED. The model's pair
	// describes the turn that is BEGINNING, and a cue named `TurnEnded` carrying the incoming
	// turn would be a field that reads correct and means the opposite.
	if (Mark.Turn != Model.Match.Turn || Mark.SideToMove != Model.Match.SideToMove)
	{
		Emit(EStratSoundCue::TurnEnded, Mark.SideToMove, INDEX_NONE, Mark.Turn);
	}

	// THE JOIN IS BY ID AND NOT BY POSITION. `FStratViewModel::Units` is rebuilt every refresh
	// and one death shifts every entry after it, so index i in the mark and index i in the
	// model are routinely different units. This is the identity `ApplyView` reconciles actors
	// on, which is what makes a cue and an actor agree about which unit they mean.
	TMap<int32, const FStratUnitSoundMark*> Marked;
	Marked.Reserve(Mark.Units.Num());
	for (const FStratUnitSoundMark& Entry : Mark.Units)
	{
		Marked.Add(Entry.UnitId, &Entry);
	}

	// RULE 3. One pass, model order.
	TSet<int32> Living;
	Living.Reserve(Model.Units.Num());
	for (const FStratUnitView& Unit : Model.Units)
	{
		Living.Add(Unit.UnitId);

		const FStratUnitSoundMark* const* const Found = Marked.Find(Unit.UnitId);
		if (Found == nullptr || *Found == nullptr)
		{
			// AN ID IN THE MODEL AND NOT IN THE MARK. Sec 2.7's Build is the only thing in this
			// project that creates a unit, so this is a factory delivering one. It is NOT
			// emitted at `SubmitProductionChoice`: Sec 2.7 can HOLD a build
			// (`FStratFactoryView::bBuildWaiting`), so a sound at submit time would announce a
			// unit that has not spawned.
			Emit(EStratSoundCue::FactoryBuiltUnit, Unit.Side, Unit.UnitId, Model.Match.Turn);
			continue;
		}

		const FStratUnitSoundMark& Before = **Found;

		if (Before.Hex != Unit.Hex)
		{
			Emit(EStratSoundCue::UnitMoved, Unit.Side, Unit.UnitId, Model.Match.Turn);
		}

		// STRICTLY LOWER, AND A RISE IS NOT AN EVENT HERE. See the declaration: HP falls under
		// Sec 2.6 combat, rises under Sec 2.7 repair, and arrives fresh at `hpMax` on a Sec 2.7
		// spawn, so only the FALL has exactly one cause. This is the whole of why the
		// comparison is `<` and not `!=`.
		if (Unit.Hp < Before.Hp)
		{
			Emit(EStratSoundCue::UnitAttacked, Unit.Side, Unit.UnitId, Model.Match.Turn);
		}
	}

	// RULE 4. One pass, mark order. An id that was on the board and is not is a unit that died
	// -- `ApplyView`'s destroy loop is the authority for `Units` being every LIVING unit, and
	// this file makes the same inference the actor reconciliation already makes.
	//
	// THE SIDE COMES OFF THE MARK AND NOT OFF THE MODEL, AND IT IS THE ONLY CUE FOR WHICH THAT
	// IS TRUE. The unit is gone from `Model.Units`, so the model cannot say whose it was; the
	// mark is the last place that fact exists. `FStratUnitSoundMark::Side` is stored for
	// exactly this line and is compared nowhere.
	for (const FStratUnitSoundMark& Entry : Mark.Units)
	{
		if (!Living.Contains(Entry.UnitId))
		{
			Emit(EStratSoundCue::UnitDestroyed, Entry.Side, Entry.UnitId, Model.Match.Turn);
		}
	}
}

float StratClampVolume(const float Volume)
{
	// NaN FIRST, BECAUSE THE CLAMP CANNOT SEE IT. `FMath::Clamp` is two comparisons, and every
	// comparison against a NaN is false, so a NaN passes through a clamp unchanged and reaches
	// `SetSoundMixClassOverride` as a gain with no defined behaviour. `FMath::IsNaN` is the
	// only instrument here that can answer; see the declaration for why the answer is 0 rather
	// than 1.
	if (FMath::IsNaN(Volume))
	{
		return 0.0f;
	}

	// AN INFINITY IS *NOT* SPECIAL-CASED, WHICH IS THE OTHER HALF OF THE SAME MEASUREMENT AND
	// IS EASY TO GET WRONG BY BEING TIDY. `FMath::IsFinite` would have caught both in one
	// line -- and would have mapped `+INF` to SILENCE, which is the opposite of what it means.
	// Comparisons against an infinity are well defined, so the clamp below answers `+INF -> 1`
	// and `-INF -> 0` correctly on its own. Only the NaN needed a hand.

	// THE ONE ARITHMETIC-FREE RULE IN THIS FILE, AND IT IS DELIBERATELY NOT A SCALE. There is
	// no percentage here, no decibel conversion and no curve: `[0, 1]` is the range
	// `UGameplayStatics::SetSoundMixClassOverride` documents for a class override's volume, and
	// anything that reshaped it would be a mix decision taken in a clamp.
	return FMath::Clamp(Volume, 0.0f, 1.0f);
}
