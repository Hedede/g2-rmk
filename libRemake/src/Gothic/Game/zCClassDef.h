#ifndef Gothic_zCClassDef_H
#define Gothic_zCClassDef_H
#include <new>
#include <Hook/Externals.h>
#include <Gothic/Types/zCObject.h>

struct zCClassDef {
	static void ObjectCreated(zCObject* obj, zCClassDef* def)
	{
		Cdecl<void(zCObject*, zCClassDef*)> call{0x5AAEB0};
		call(obj, def);
	}

	static void EndStartup()
	{
		Cdecl<void()> call{0x5AA7A0};
		call();
	}
};

#include <Logging/Log.h>

template<typename T>
T* zSTATIC_CAST(zCObject* object)
{
	g2::Log("zSTATIC_CAST", (void*)object);
	if (!object)
		return nullptr;
	g2::Log("zSTATIC_CAST", (void*)object->_vtab);
	g2::Log("zSTATIC_CAST", (void*)object->GetClassDef());
	g2::Log("zSTATIC_CAST", (void*)T::classDef);
	if (object->GetClassDef() == T::classDef)
		return static_cast<T*>(object);
	return nullptr;
}

#endif//Gothic_zCClassDef_H
