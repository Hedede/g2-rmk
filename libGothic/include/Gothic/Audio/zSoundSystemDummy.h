#include <Gothic/Audio/zSound.h>

struct zCSoundSystemDummy : zCSoundSystem {
	zCSoundSystemDummy()
	{
		vtab = reinterpret_cast<zCSoundSystem_vt*>(0x83A6A4);
	}
};
