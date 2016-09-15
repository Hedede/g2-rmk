#include <Gothic/Audio/zSound.h>

struct zCSndSys_MSS : zCSoundSystem {
	zCSndSys_MSS()
	{
		Thiscall<void(zCSndSys_MSS*)> ctor{0x4EAAF0};
		ctor(this);
	}

	int unk1;
	int unk2;
	int unk3;
};
