#pragma once
#include <Gothic/Types/zSTRING.h>

struct zCClassDef;
struct zCArchiver;
struct zCObject {
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
