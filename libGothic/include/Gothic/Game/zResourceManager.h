#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/System/zThread.h>
//#include <Gothic/System/zCriticalSection.h>
//

struct zCClassDef;
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

	struct zCClassCache {};

// private:
	zCArray< zCClassCache > classCache;
	void* resource[2];
	int unk;

	zBOOL threadingEnabled;

	char data[0x34];
	//zCCriticalSection critSect;
	//int queued;
	//int loaded;
	//zBOOL showDebugInfo;
	//float cacheInImmedMsec;
	//int wha;
};
static auto& zresMan = Value<zCResourceManager*>(0x99AB30);
