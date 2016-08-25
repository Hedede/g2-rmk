#ifndef GOTHIC_OCBINKPLAYER_H
#define GOTHIC_OCBINKPLAYER_H
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>

struct zCVideoPlayer {
	zSTRING fileName;
	int unk1    = 0;
	int paused  = 0;
	int playing = 0;

	int soundEnabled  = 1;
	float soundVolume = 1.0;
	float unk2 = 1.0;
};

struct zCBinkPlayer : zCVideoPlayer {
	void* binkHandle = 0;
	int pixelFormat = -1;
	__int16 unkz1 = 0;
	__int16 unkz2 = 0;
	int unkz3 = 0;
	int handleEvents = 1;
	int fullScreen = 1;
	zSTRING unkz4;
	int unkz5;
	int unkz6[5];
};

struct oCBinkPlayer : zCBinkPlayer {

	oCBinkPlayer()
	{
		Thiscall<void(oCBinkPlayer*)> ctor{0x422BD0};
		ctor(this);
	}

	char unkb[12];
	zBOOL extendedVideoKeys;
	zBOOL disallowVideoInput;
	char unkz; 
};

#include <Hook/size_checker.h>
namespace { size_checker<oCBinkPlayer, 0x88> sc_obinkp; }
#endif//GOTHIC_OCBINKPLAYER_H
