void GiveFatal()
{
	zFATAL("D: This method should be overriden! Illegal zCResource class setup!");
}

class zCResource : public zCObject {
	Z_OBJECT(zCResource);
public:
	zCResource()
	{
		SetCacheInPriority(0.5);
	}

	virtual ~zCResource()
	{
		nextRes = nullptr;
		prevRes = nullptr;
	}

	virtual void LoadResourceData()    { GiveFatal(); }
	virtual void ReleaseResourceData() { GiveFatal(); }

	virtual size_t GetResSizeBytes()   { GiveFatal(); return 0; }

	virtual void GetCacheConfig(ulong &,ulong &) { GiveFatal(); }

	int CacheIn(float priority)
	{
		if ( cacheState == 3 ) {
			TouchTimeStamp();
		} else {
			TouchTimeStampLocal();
			zresMan.CacheIn(this, priority);
		}
		return cacheState;
	}

	void CacheOut()
	{
		if (!cacheState)
			zresMan.CacheOut(this);
	}

	int LockStateChange()
	{
		return stateChangeGuard.Unlock(-1);
	}

	int UnlockStateChange()
	{
		return stateChangeGuard.Unlock();
	}

	void TouchTimeStampLocal()
	{
		timeStamp = ztimer.totalTime;
	}

	void SetCacheInPriority(float perc)
	{
		cacheInPriority = perc * numeric_limits<uint16_t>::max();
	}

private:
	zCResource*  nextRes = nullptr;
	zCResource*  prevRes = nullptr;
	uint32_t     timeStamp = 0;

	zCCriticalSection stateChangeGuard;

	uint8_t cacheState : 2;
	uint8_t cacheClassIndex = -1;
	bool cacheOutLock = false;
	bool managedByResMan;

	uint16_t cacheInPriority;
	short    canBeCachedOut = 1;
};
