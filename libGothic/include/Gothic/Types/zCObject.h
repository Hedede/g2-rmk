#pragma once
#include <Gothic/Types/zSTRING.h>
struct zCObject {
	void Release()
	{
		Thiscall<void(zCObject*)> call{0x40C310};
		call(this);
	}

	void* _vtab;

	int refCtr    = 1;
	int hashIndex = -1;
	int hashNext  = 0;
	zSTRING objectName;
};
