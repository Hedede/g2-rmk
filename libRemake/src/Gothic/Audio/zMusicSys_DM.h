#pragma once
#include <Gothic/Audio/zMusic.h>

struct zCMusicSys_DirectMusic : zCMusicSystem {
	zCMusicSys_DirectMusic()
	{
		Thiscall<void(zCMusicSys_DirectMusic*)> ctor{0x4E7450};
		ctor(this);
	}

	int unk;
};
