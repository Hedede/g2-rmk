#ifndef Gothic_zCObject_H
#define Gothic_zCObject_H
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

#endif//Gothic_zCObject_H
