#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/System/zThread.h>
#include <Gothic/System/zCriticalSection.h>

struct zCClassDef;
struct zCResource;
struct zCResourceManager : zCThread {
	zCResourceManager()
	{
		Thiscall<void(zCResourceManager*)> ctor{0x5DC8C0};
		ctor(this);
	}

	~zCResourceManager()
	{
		Thiscall<void(zCResourceManager*)> dtor{0x5DC9A0};
		dtor(this);
		//PurgeCaches();
	}

	void SetThreadingEnabled(int enabled)
	{
		threadingEnabled = enabled;
	}

	void EndThread()
	{
		vtab->EndThread(this);
	}

	void PurgeCaches(zCClassDef* cd = nullptr)
	{
		Thiscall<void(zCResourceManager*,zCClassDef*)> func{0x5DCA30};
		func(this, cd);
	}

	void DoFrameActivity()
	{
		Thiscall<void(zCResourceManager*)> func{0x5DD4F0};
		func(this);
	}

	void CacheIn(zCResource* res, float priority)
	{
		Thiscall<void(zCResourceManager*,zCResource*,float)> call{0x05DD040};
		call(this, res, priority);
	}

	struct zCClassCache {};

// private:
	zCArray< zCClassCache > classCache;

	zCResource *queueStart;
	zCResource *queueLast;

	int lastUpdateTime;

	zBOOL threadingEnabled;

	zCCriticalSection critSect;

	int queued;
	int loaded;

	zBOOL showDebugInfo;
	float cacheInImmedMsec;

	int purged;
};

#include <Hook/size_checker.h>
CHECK_SIZE(zCResourceManager, 0x64);

inline auto& zresMan = Value<zCResourceManager*>(0x99AB30);
