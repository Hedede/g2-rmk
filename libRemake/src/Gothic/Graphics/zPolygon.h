#ifndef Gothic_zPolygon_H
#define Gothic_zPolygon_H
#include <Hook/Externals.h>
struct zCPolygon {
	static void S_InitMorph()
	{
		Cdecl<void()> call{0x5B70F0};
		call();
	}
	static void S_ResetMorphedVerts()
	{
		Cdecl<void()> call{0x5B7130};
		call();
	}
	static void PrepareRendering()
	{
		Cdecl<void()> call{0x5B79F0};
		call();
	}
};
#endif//Gothic_zPolygon_H
