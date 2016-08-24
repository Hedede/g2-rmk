#ifndef Gothic_zResourceManager_H
#define Gothic_zResourceManager_H
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>

// TODO: merge with other zthread
struct zCThreadB {
	void* _vtab;
	void*    threadHandle = 0;
	unsigned threadId     = 0;

	zBOOL threadSuspended      = 0;
	zBOOL isRunning            = 0;
	zBOOL terminationRequested = 0;
};

struct zCResourceManager : zCThreadB {
	zCResourceManager()
	{
		Thiscall<void(zCResourceManager*)> ctor{0x5DC8C0};
		ctor(this);
	}

	void SetThreadingEnabled(int enabled)
	{
		threadingEnabled = enabled;
	}

	zCArray< void* > classCache;
	void* resource[2];
	int unk;

	zBOOL threadingEnabled;
	char data[0x34];
};
static auto& zresMan = Value<zCResourceManager*>(0x99AB30);
#endif//Gothic_zResourceManager_H
