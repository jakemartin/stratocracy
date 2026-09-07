// Two lookups and no logic. Everything this file could usefully decide is decided in
// `StratSoundCues.h` (what happened) or `StratSoundDirector.cpp` (whether to play it); a
// branch here would be a third opinion.

#include "StratSoundBank.h"

#include "Sound/SoundBase.h"

USoundBase* UStratSoundBank::SoundFor(const EStratSoundCue Cue) const
{
	// A SWITCH WITH NO `default:` ARM, DELIBERATELY -- BUT SEE THE RETRACTION BELOW BEFORE
	// RELYING ON IT. The omission is still right: a `default: return nullptr;` would make an
	// unhandled cue INDISTINGUISHABLE from a configured null, and `EveryCueInTheShippedBankHasASound`
	// reads exactly that answer. What the omission does NOT buy is a compiler diagnostic.
	// The `return nullptr` below the switch is what MSVC needs to see a value on every path; it
	// is reached today only by an enumerator with no arm.
	//
	//   RETRACTED> "The missing default is what makes a future eighth enumerator a compiler
	//   RETRACTED>  diagnostic here rather than a silent null at runtime."
	//   MEASURED FALSE 2026-09-06 over base commit `f7da9ca`, twice, by adding an eighth
	//   enumerator to `EStratSoundCue` and building `StratocracyEditor Win64 Development`: this
	//   file recompiled -- it is step 40 of 55 in that build's own log -- and MSVC emitted
	//   NOTHING. Both diagnostics that would have fired are level-4 and OFF BY DEFAULT (C4062,
	//   "enumerator in switch of enum is not handled"; C4061, its with-default sibling), and
	//   nothing in `StratPlay.Build.cs` or either `.Target.cs` promotes them -- the only warning
	//   any `Build.cs` in this tree touches is `ShadowVariableWarningLevel`.
	//   THIS IS A GUARD THAT PRINTED GREEN WHILE BLIND, and it is worth naming as that rather
	//   than as a stale comment: the sentence did not merely describe a mechanism that had gone
	//   away, it asserted a mechanism that never existed on this toolchain, and a reader adding
	//   an eighth cue would have been entitled to expect the build to stop them. It did not.
	//   WHAT ACTUALLY CATCHES AN EIGHTH CUE HERE is a clause and not the compiler:
	//   `Stratocracy.StratPlay.GATE-AUDIO.EveryCueInTheShippedBankHasASound` asks this function
	//   for every enumerator the reflected enum reports and reddens on the null. That is weaker
	//   -- it needs a suite run -- and it is what there is.
	//   NOT ONE EXECUTABLE BYTE MOVED FOR THIS. The switch is unchanged; only the prose is.
	//   ADDING `/we4062` TO THIS MODULE WOULD MAKE THE OLD SENTENCE TRUE and is deliberately not
	//   done in this pass: it would promote a warning across every switch in `StratPlay`, which
	//   is a module-wide build-configuration decision with its own blast radius and does not
	//   belong in a pass about one array bound. Recorded as a debt, not as an oversight.
	//
	//   AND THE MEASUREMENT WAS CASHED IN ON 2026-09-07, WHICH IS WORTH RECORDING BECAUSE IT IS
	//   THE CASE THE RETRACTION ABOVE PREDICTED. `EStratSoundCue::PlayerTurnBegan` was added
	//   that day and the arm below was written BY HAND, from the enum, because nothing in this
	//   toolchain would have said a word had it been forgotten. The debt is unchanged and
	//   `/we4062` was still not taken, for the reason immediately above.
	switch (Cue)
	{
	case EStratSoundCue::ButtonClick:      return ButtonClick;
	case EStratSoundCue::TurnEnded:        return TurnEnded;
	case EStratSoundCue::PlayerTurnBegan:  return PlayerTurnBegan;
	case EStratSoundCue::UnitMoved:        return UnitMoved;
	case EStratSoundCue::UnitAttacked:     return UnitAttacked;
	case EStratSoundCue::UnitDestroyed:    return UnitDestroyed;
	case EStratSoundCue::FactoryBuiltUnit: return FactoryBuiltUnit;
	case EStratSoundCue::MatchEnded:       return MatchEnded;
	}

	return nullptr;
}

float UStratSoundBank::MinSecondsBetweenFor(const EStratSoundCue Cue) const
{
	// ABSENT AND NON-POSITIVE COLLAPSE TO THE SAME ANSWER, HERE, ONCE. Both mean "no minimum",
	// and leaving the two spellings for the caller to reconcile would be an invitation for one
	// call site to handle only the first. See the property's own doc.
	const float* const Found = MinSecondsBetween.Find(Cue);
	return (Found != nullptr && *Found > 0.0f) ? *Found : 0.0f;
}
