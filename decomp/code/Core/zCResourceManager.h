class zCResourceManager : public zCThread {
public:
	struct zCClassCache {
		zCClassCache() = default;
		~zCClassCache() = default;

		zCClassDef *classDef = nullptr;

		zCResource *lastRes  = nullptr;
		zCResource *firstRes = nullptr;

		int numResources   = 0;
		int totalSizeBytes = 0;

		int maxNumResources = 0;
		int maxResSizeBytes = 0;
	};

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

private:
	zCResource* GetNextCacheInQueue()
	{
		return RemoveCacheInQueue(0);
	}

private:
	zCArray<zCClassCache> classCache;

	zCResource *resource1; // ?
	zCResource *resource2;

	int unk;

	zBOOL threadingEnabled;
	zCCriticalSection critSect;
	int queued;
	int loaded;
	zBOOL showDebugInfo;
	float cacheInImmedMsec;
	int wha;
};
