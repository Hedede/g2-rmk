#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zCObject.h>
#include <Gothic/System/zCriticalSection.h>

struct zCResource;
struct zCResource_vt {
	zCClassDef *(__thiscall *_GetClassDef)(zCResource *);
	void (__thiscall *Archive)(zCResource *, zCArchiver *);
	void (__thiscall *Unarchive)(zCResource *, zCArchiver *);
	void (__thiscall *dtor)(zCResource *, unsigned);
	void (__thiscall *LoadResourceData)(zCResource *);
	void (__thiscall *ReleaseResourceData)(zCResource *);
	int (__thiscall *GetResSizeBytes)(zCResource *);
	void (__thiscall *GetCacheConfig)(zCResource *, unsigned *, unsigned *);
};


struct zCResource : zCObject {
	void LoadResourceData()
	{
		vtab()->LoadResourceData(this);
	}

	int CacheIn(float priority);

	void TouchTimeStamp()
	{
		Thiscall<void(zCResource*)> call{0x5DC810};
		call(this);
	}

	void TouchTimeStampLocal();


protected:
	zCResource_vt* vtab()
	{
		return reinterpret_cast<zCResource_vt*>(_vtab);
	}

public:

	zCResource *nextRes;
	zCResource *prevRes;
	int timeStamp;
	zCCriticalSection stateChangeGuard;
	uint8_t cacheState : 2;
	uint8_t cacheClassIndex;
	uint8_t cacheOutLock;
	uint8_t managedByResMan;
	int16_t cacheInPriority;
	int16_t canBeCachedOut;
};

