#include <Gothic/System/zResource.h>
#include <Gothic/System/zResourceManager.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/zTimer.h>

enum ResourceState {
	CACHED_OUT = 0,
	QUEUED = 1,
	LOADING = 2,
	CACHED_IN = 3
};

int zCResource::CacheIn(float priority)
{
	if (cacheState == CACHED_IN) {
		TouchTimeStamp();
	} else {
		TouchTimeStampLocal();
		zresMan->CacheIn(this, priority);
	}
	return cacheState;
}

void zCResource::TouchTimeStampLocal()
{
	timeStamp = ztimer.GetTotalTime();
}
