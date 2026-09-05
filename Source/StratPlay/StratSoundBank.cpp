// Two lookups and no logic. Everything this file could usefully decide is decided in
// `StratSoundCues.h` (what happened) or `StratSoundDirector.cpp` (whether to play it); a
// branch here would be a third opinion.

#include "StratSoundBank.h"

#include "Sound/SoundBase.h"

USoundBase* UStratSoundBank::SoundFor(const EStratSoundCue Cue) const
{
	// A SWITCH WITH NO `default:` ARM, DELIBERATELY. The missing default is what makes a future
	// eighth enumerator a compiler diagnostic here rather than a silent null at runtime. The
	// `return nullptr` below the switch is what MSVC needs to see a value on every path; it is
	// unreachable for every enumerator that has an arm.
	switch (Cue)
	{
	case EStratSoundCue::ButtonClick:      return ButtonClick;
	case EStratSoundCue::TurnEnded:        return TurnEnded;
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
