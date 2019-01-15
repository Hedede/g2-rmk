#ifndef Gothic_oCObjectFactory_H
#define Gothic_oCObjectFactory_H
#include <Hook/Externals.h>
#include <Gothic/Types/zCObject.h>
#include <Gothic/Game/zCClassDef.h>
struct zCObjectFactory : zCObject {
};
struct oCObjectFactory : zCObjectFactory {
	oCObjectFactory()
	{
		_vtab = reinterpret_cast<void*>(0x82F104);
	}

	void* operator new(size_t size)
	{
		void* mem = ::operator new(size);
		auto ptr = reinterpret_cast<oCObjectFactory*>(mem);
		auto cd = reinterpret_cast<zCClassDef*>(0xAB2FB0);
		zCClassDef::ObjectCreated(ptr, cd);
		return mem;
	}
};
static auto& zfactory = Value<zCObjectFactory*>(0x8D8DF0);
#endif//Gothic_oCObjectFactory_H
