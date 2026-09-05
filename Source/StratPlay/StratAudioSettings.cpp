// Two lines and one constant. Every rule this file could have restated belongs to
// `StratClampVolume`; see the header on why that authority is in `StratUI` and not here.

#include "StratAudioSettings.h"

#include "StratSoundCues.h"

const TCHAR* UStratAudioSettings::DefaultSlotName()
{
	// A FUNCTION AND NOT A `static constexpr TCHAR*` MEMBER, because a static data member of
	// pointer type needs an out-of-line definition anyway under the C++17 rules this project
	// builds with, and a function is the shape that cannot be initialised twice. It is the only
	// place this string exists in the tree -- see the header on why one author makes a literal a
	// constant rather than a copy.
	return TEXT("StratocracyAudio");
}

void UStratAudioSettings::Sanitize()
{
	// ONE AUTHORITY, ASKED THREE TIMES. `StratClampVolume` is `StratUI`'s and is also what
	// `UStratOptionsWidget`'s setters call, so the number a slider accepts and the number a slot
	// stores cannot come to different conclusions about what is legal.
	MasterVolume = StratClampVolume(MasterVolume);
	SfxVolume    = StratClampVolume(SfxVolume);
	MusicVolume  = StratClampVolume(MusicVolume);
}
