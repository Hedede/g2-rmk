#ifndef Gothic_zCClassDef_H
#define Gothic_zCClassDef_H
#include <new>
#include <Hook/Externals.h>
#include <Gothic/Types/zCObject.h>

struct zCClassDef {
	static ObjectCreated(zCObject* obj, zCClassDef* def)
	{
		Cdecl<void(zCObject*, zCClassDef*)> call{0x5AAEB0};
		call(obj, def);
	}
};

#endif//Gothic_zCClassDef_H
