#ifndef GOTHIC_OCBINKPLAYER_H
#define GOTHIC_OCBINKPLAYER_H
#include <Hook/func.h>

struct oCBinkPlayer {
	oCBinkPlayer()
	{
		Thiscall<void(oCBinkPlayer*)> ctor{0x422BD0};
		ctor(this);
	}
};

#endif//GOTHIC_OCBINKPLAYER_H
