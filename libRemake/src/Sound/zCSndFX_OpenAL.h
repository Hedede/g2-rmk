#pragma once
#include <Gothic/Types/zSTRING.h>

struct C_SFX {
	zSTRING    file;                   // sfx filename
	int        pitchOff = 0;           // pitchoffset in semitones
	int        pitchVar = 0;           // semitone-variance
	int        vol = 127;              // 0..1
	int        loop = 0;               // 0/1
	int        loopStartOffset = 0;
	int        loopEndOffset = -1;
	float      reverbLevel = 1;        // 0..1
	zSTRING    pfxName;
};

#include <Sound/OpenALSource.h>
#include <Gothic/Audio/zSoundFX.h>
struct zCSndFX_OpenAL : zCSoundFX {
	zCSndFX_OpenAL( );
	void LoadResourceData();

	// TODO: check whether this is necessary {
	float volume;
	float pan;

	int frequency;

	int unkz;

	float timeStart;
	// }

	C_SFX sound;

	g2::Buffer buffer;
};
