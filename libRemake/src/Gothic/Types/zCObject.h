#ifndef ZenGin_Object_H
#define ZenGin_Object_H
#include <Gothic/Types/zSTRING.h>

struct zCObject;
struct zCClassDef;
struct zCArchiver;

struct zCObject_vt
{
	zCClassDef *(__thiscall *GetClassDef)(zCObject *);
	void (__thiscall *Archive)(zCObject *, zCArchiver *);
	void (__thiscall *Unarchive)(zCObject *, zCArchiver *);
	void (__thiscall *dtor)(int);
};

struct zCObject {
	zCClassDef* GetClassDef()
	{
		return reinterpret_cast<zCObject_vt*>(_vtab)->GetClassDef(this);
	}

	void Release()
	{
		if (1 >= refCtr--)
			delete this;
	}

	zSTRING const& GetObjectName() const
	{
		return objectName;
	}

	int SetObjectName(std::string const& name)
	{
		Thiscall<int(zCObject*, zSTRING const&)> call{0x5A9CE0};
		call( this, zSTRING{name} );
	}

	void* _vtab;

	int refCtr    = 1;
	int hashIndex = -1;
	int hashNext  = 0;
	zSTRING objectName;
};
#endif//ZenGin_Object_H
