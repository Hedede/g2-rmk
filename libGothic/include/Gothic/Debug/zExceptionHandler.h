#ifndef GOTHIC_ZCEXCEPTIONHANDLER
#define GOTHIC_ZCEXCEPTIONHANDLER
#include <Hook/func.h>
#include <Gothic/Types/Base.h>

zBOOL& zExHandler = *reinterpret_cast<zBOOL*>(0x89D9F4);

struct zCExceptionHandler {
	static void SetIsActive(bool active)
	{
		Cdecl<void(int)> func{0x4C8860};

		zExHandler = active;
		func(active);
	}
};

#endif//GOTHIC_ZCEXCEPTIONHANDLER
