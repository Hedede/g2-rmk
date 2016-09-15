#pragma once
#include <Gothic/Audio/zMusic.h>

struct zCMusicSys_Dummy : zCMusicSystem {
	zCMusicSys_Dummy()
	{
		vtab = reinterpret_cast<zCMusicSystem_vt*>(0x83A72C);
	}

	int unk;
};
