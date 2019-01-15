#ifndef Gothic_zVertex_H
#define Gothic_zVertex_H
#include <Hook/Externals.h>
struct zCVertex {
	static void ResetVertexTransforms()
	{
		Cdecl<void()> call{0x50BBD0};
		call();
	}
};
#endif//Gothic_zVertex_H
