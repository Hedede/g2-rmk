class zCResourceManager : public zCThread {
public:
	struct zCClassCache {
		zCClassCache() = default;
		~zCClassCache() = default;

		void InsertRes(zCResource *res);
		void RemoveRes(zCResource *res);
		void TouchRes(zCResource *res);

		zCClassDef *classDef = nullptr;

		zCResource *lastRes  = nullptr;
		zCResource *firstRes = nullptr;

		int numResources   = 0;
		int totalSizeBytes = 0;

		int maxNumResources = 0;
		int maxResSizeBytes = 0;
	};

	zCResourceManager()
	{
		InitClassCache();
	}
	~zCResourceManager() override
	{
		PurgeCaches(0);
	}

	void ThreadProc() override;

	void SetThreadingEnabled(zBOOL enabled)
	{
		this->threadingEnabled = enabled;
	}

	zBOOL GetThreadingEnabled() const
	{
		return threadingEnabled;
	}

	zBOOL QueueProcess_Suspend()
	{
		if ( threadingEnabled )
			return SuspendThread();
		return false;
	}

	zBOOL QueueProcess_Resume()
	{
		if ( threadingEnabled ) {
			if ( threadSuspended ) {
				return ResumeThread();
			if ( !IsThreadRunning() )
				return BeginThread();
		}
		return ThreadProc();
	}

	zBOOL QueueProcess_Start()
	{
		if ( threadingEnabled ) {
			if ( !IsThreadRunning() )
				return BeginThread();
			return true;
		}
		if ( queueLast )
			return ThreadProc();
		return false;
	}

	void LockCacheInQueue()
	{
		if ( threadingEnabled )
			critSect.Lock(-1);
	}

	void UnlockCacheInQueue()
	{
		if ( threadingEnabled )
			critSect.Unlock();
	}


	void SetShowDebugInfo(zBOOL b)
	{
		this->showDebugInfo = b;
	}

	zBOOL SetShowDebugInfo()
	{
		return showDebugInfo;
	}

	int GetClassCacheIndex(zCClassDef* resClassDef)
	{
		unsigned idx = 0;
		for (auto& cc : classCache) {
			if (cc->classDef == resClassDef)
				return idx;
			++idx;
		}
		return -1;
	}

	zCClassCache* GetClassCache(zCResource* res)
	{
		if (res->cacheClassIndex == -1) {
			auto cd = res->_GetClassDef();
			while (!(cd->classFlags & zCLASS_FLAG_RESOURCE)) {
				cd = cd->baseClassDef;
				if (!cd) {
					cd = res->_GetClassDef();
					break;
				}
			}

			res->cacheClassIndex = GetClassCacheIndex(cd);
		}
		return classCache[res->cacheClassIndex];
	}

	void TouchTimeStamp(zCResource* res)
	{
		GetClassCache(res)->TouchRes(res);
	}

private:
	zCResource* GetNextCacheInQueue()
	{
		return RemoveCacheInQueue(0);
	}

	bool WaitForCacheIn(zCResource* res)
	{
		if ( threadingEnabled ) {
			while (in(res->cacheState, 1,2))
				SleepThread(0);
		} else {
			while (in(res->cacheState, 1,2))
				ThreadProc();
		}
		return res->cacheState == 3;
	}

private:
	zCArray<zCClassCache> classCache;

	zCResource *queueLast  = nullptr;
	zCResource *queueStart = nullptr;

	int lastUpdateTime = 0;

	zBOOL threadingEnabled = false;
	zCCriticalSection critSect;
	zBOOL queued = false;
	zBOOL loaded = false;
	zBOOL showDebugInfo = false;
	float cacheInImmedMsec = 0;
	int wha = 0;
};

//------------------------------------------------------------------------------
void zCResourceManager::RemoveCacheInQueue(zCResource *res)
{
	LockCacheInQueue();
	if (!res)
		res = queueLast;
	if ( res ) {
		if (res == queueStart)
			queueStart = res->prevRes;
		if (res == queueLast) {
			queueLast  = res->nextRes;
		} else
		if ( auto prev = res->prevRes )
			prev->nextRes = _res->nextRes;
		if ( auto next = res->nextRes )
			next->prevRes = _res->prevRes;
		res->prevRes = 0;
		res->nextRes = 0;
		--queued;
	}
	UnlockCacheInQueue();
}

void zCResourceManager::DoFrameActivity()
{
	if ( cacheInImmedMsec > 0.0 ) {
		cacheInImmedMsec -= ztimer.frameTimeFloat;
		if (cacheInImmedMsec < 0.0)
			cacheInImmedMsec = 0;
	}

	lastUpdateTime = ztimer.totalTime;
	QueueProcess_Start();
	Evict();
	if (showDebugInfo)
		PrintStatusDebug();
	loaded = 0;
}

//------------------------------------------------------------------------------
void zCResourceManager::zCClassCache::InsertRes(zCResource *res)
{
	if ( !maxResSizeBytes )
		GetCacheConfig(maxResSizeBytes, maxNumResources);

	res->prevRes = 0;
	res->nextRes = lastRes;

	if ( !firstRes )
		firstRes = res;
	if ( lastRes )
		lastRes->prevRes = res;
	lastRes = res;

	numResources   += 1;
	totalSizeBytes += res->GetResSizeBytes();
}

void zCResourceManager::zCClassCache::TouchRes(zCResource *res)
{
	// moves res to the front of the list,
	// if not already there AND older than previous
	auto prev = res->prevRes;
	if ( prev && prev->timeStamp < res->timeStamp && res != lastRes ) {
		if ( res == firstRes )
			firstRes = prev;

		if ( auto prev = res->prevRes )
			prev->nextRes = res->nextRes;
		if ( auto next = res->nextRes )
			next->prevRes = res->prevRes;

		res->prevRes = 0;
		res->nextRes = lastRes;

		if ( !firstRes )
			firstRes = res;
		if ( lastRes )
			lastRes->prevRes = res;
		lastRes = res;
	}
}

void zCResourceManager::zCClassCache::RemoveRes(zCResource *res)
{
	if ( res->cacheOutLock ) {
		if ( res == firstRes )
			firstRes = res->prevRes;
		if ( res == lastRes ) {
			lastRes = res->nextRes;
		} else
		if ( auto prev = res->prevRes )
			prev->nextRes = res->nextRes;
		if ( auto next = res->nextRes )
			next->prevRes = res->prevRes;
		res->prevRes = 0;
		res->nextRes = 0;

		--numResources;
		totalSizeBytes -= res->GetResSizeBytes();
		if ( totalSizeBytes < 0 )
			totalSizeBytes = 0;
		// hmm ^ should issue warning, because something fishy
		// going on, when you take out more than put
	}
}
