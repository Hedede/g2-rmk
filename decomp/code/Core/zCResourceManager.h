//Dieter/zResource.cpp
struct zCResourceManager : zCThread {
	struct zCClassCache {
		zCClassCache() = default;
		~zCClassCache() = default;

		void InsertRes(zCResource *res);
		void RemoveRes(zCResource *res);
		void TouchRes(zCResource *res);

		zCClassDef *classDef = nullptr;

		zCResource *firstRes = nullptr;
		zCResource *lastRes  = nullptr;

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

	void QueueProcess_Resume()
	{
		if ( threadingEnabled ) {
			if ( threadSuspended ) {
				return ResumeThread();
			if ( !IsThreadRunning() )
				return BeginThread();
		}
		ThreadProc();
	}

	void QueueProcess_Start()
	{
		if ( threadingEnabled ) {
			if ( !IsThreadRunning() )
				return BeginThread();
		}
		if ( queueStart )
			ThreadProc();
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
			while (in(res->cacheState,QUEUED,LOADING))
				SleepThread(0);
		} else {
			while (in(res->cacheState,QUEUED,LOADING))
				ThreadProc();
		}
		return res->cacheState == CACHED_IN;
	}

private:
	zCArray<zCClassCache> classCache;

	zCResource *queueStart = nullptr;
	zCResource *queueLast  = nullptr;

	int lastUpdateTime = 0;

	zBOOL threadingEnabled = false;
	zCCriticalSection critSect;
	zBOOL queued = false;
	zBOOL loaded = false;
	zBOOL showDebugInfo = false;
	float cacheInImmedMsec = 0;
	int cacheInLock = 0;
};

//------------------------------------------------------------------------------
void zCResourceManager::RemoveCacheInQueue(zCResource *res)
{
	LockCacheInQueue();
	if (!res)
		res = queueStart;
	if ( res ) {
		if (res == queueLast)
			queueLast = res->prevRes;
		if (res == queueStart) {
			queueStart  = res->nextRes;
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

void zCResourceManager::Evict()
{
	int maxres  = 0;
	int maxsize = 0;
	for (auto& _class : classCache) {
		maxres  += _class.maxNumResources;
		maxsize += _class.maxResSizeBytes;
	}

	for (auto& _class : classCache) {
		auto maxr = _class.maxResSizeBytes;
		auto* res  = _class.lastRes;

		// I have no idea wtf is going on,
		int govno = 0;
		if (lastUpdateTime > maxr)
			govno = lastUpdateTime - maxr;
		if (!res) continue;

		if (res->timeStamp >= govno || !res->canBeCachedOut) {
			if (_class.totalSizeBytes <= _class.maxNumResources || maxsize < maxres)
				continue;
			maxsize -= res->GetResSizeBytes();
		}
		if ( !res->locked )
			zresMan->CacheOut(res);
	}
}

void zCResourceManager::PurgeCaches(zCClassDef *classDef)
{
	cacheInLock = true;

	while ( IsThreadRunning(this) && !threadSuspended );

	while( auto res = RemoveCacheInQueue(0) )
		res->CacheOut();

	for (auto& clas : classCache) {
		if (clas->classDef == classDef) {
			auto res = clas->firstRes;
			while ( res ) {
				zASSERT(res->GetRefCtr()>=1); // 312
				if ( res->locked ) {
					res = res->nextRes;
				} else {
					zresMan.CacheOut(res);
					res = clas->firstRes;
				}
			}
		}
	}

	cacheInLock = false;
}

void zCResourceManager::InitClassCache()
{
	zCArray<zCClassDef *> classList;
	for (auto& classDef : zCClassDef::classDefList) {
		// was using GetSafe, but I hidden it in ^
		if (classDef->classFlags & zCLASS_FLAG_RESOURCE) {
			// CheckInheritance is pseudocode
			if (!CheckInheritance(classFlags, &zCResource::classDef )) {
				zFATAL("D: Class has zCLASS_FLAG_RESOURCE but does not inherit zCResource!"); // 353
				continue;
			}
			classList.InsertEnd(classDef);
		}
	}

	classCache.AllocAbs( classCache.GetNum() );

	zSTRING classes;
	for (auto [i, clas] : ipairs(classList)) {
		classCache[i] = clas;
		classes += " " + clas->className;
	}

	zINFO(3, "D: RESMAN: Classes registered: " + classes); // 366

}

void zCResourceManager::InsertCacheInQueue(zCResource *res, float priority)
{
	res->cacheInPriority = priority * 65535.0;

	LockCacheInQueue();

	zCResource* next = nullptr;
	zCResource* prev   = queueLast;
	for ( ; prev; prev = prev->prevRes ) {
		if ( prev->cacheInPriority <= res->cacheInPriority )
			break;
		next = prev;
	}
	res->prevRes = node;
	res->nextRes = next;
	if ( next )
		next->prevRes = res;
	else
		queueLast = res;
	if ( prev )
		prev->nextRes = res;
	else
		queueStart = res;
	++queued;

	res->cacheState = QUEUED;

	QueueProcess_Resume();

	UnlockCacheInQueue();
}

void zCResourceManager::LoadResource(zCResource *res)
{
	if ( !in(cacheState, LOADING, CACHED_IN) ) {
		res->cacheState = LOADING;
		res->LoadResourceData();
		res->cacheState = CACHED_IN;

		GetClassCache(res)->InsertRes(res);

		++loaded;
	}
}

unsigned zCResourceManager::ThreadProc()
{
	if ( threadingEnabled )
		SetThreadPriority(threadHandle, 0);

	if ( GetTerminationRequested() )
		return 0;

	while ( threadingEnabled || loaded < 99999 ) {
		zCResource* res = nullptr;
		if (!cacheInLock)
			res = RemoveCacheInQueue(0);
		if (!res) {
			if (!QueueProcess_Suspend())
				return 0;
		}

		LoadResource(res);

		if ( GetTerminationRequested() )
			return 0;
	}

	SuspendThread();
	return 0;
}

int zCResourceManager::CacheIn(zCResource *res, float priority)
{
	if (res->GetObjectName().IsEmpty())
		return -1;
	if ( cacheInImmedMsec > 0.0 )
		priority = -1.0;

	if (res->cacheState == CACHED_OUT) {
		res->cacheOutLock = true;
		res->refCtr += 1;
		if ( priority >= 0.0 ) {
			InsertCacheInQueue(res, priority);
		} else if ( !threadingEnabled || threadSuspended ) {
			LoadResource(res);
		} else {
			cacheInLock = true;
			while ( IsThreadRunning() && !threadSuspended ) ;

			LoadResource(res);

			cacheInLock = false;

			QueueProcess_Resume();
		}
	} else if (res->cacheState != CACHED_IN && priority < 0.0)
		if ( threadingEnabled ) {
			SetThreadPriority(threadHandle, 2);
			WaitForCacheIn(res);
			SetThreadPriority(threadHandle, 0);
		} else {
			WaitForCacheIn(res);
			zWARNING("X: CacheIn: Immediate resource cached in via worker thread: " + res->GetObjectName() + " !");
		}
	}
	return res->cacheState;
}

void zCResourceManager::CacheOut(zCResource *res)
{
	res->stateChangeGuard.Lock();

	if ( res->cacheState == LOADING)
		WaitForCacheIn(res);

	if ( res->cacheState == QUEUED ) {
		RemoveCacheInQueue(res);
	} else if ( res->cacheState == CACHED_IN ) {
		GetClassCache(res)->RemoveRes(res);
		res->ReleaseResourceData();
	}

	res->cacheState = CACHED_OUT;

	res->stateChangeGuard.Unlock();

	if ( res->cacheOutLock )
		res->Release();
	if ( res->refCtr == 0 )
		res->cacheOutLock = 0;
}

//------------------------------------------------------------------------------
void zCResourceManager::zCClassCache::InsertRes(zCResource *res)
{
	if ( !maxResSizeBytes )
		GetCacheConfig(maxResSizeBytes, maxNumResources);

	res->prevRes = 0;
	res->nextRes = firstRes;

	if ( !lastRes )
		lastRes = res;
	if ( firstRes )
		firstRes->prevRes = res;
	firstRes = res;

	numResources   += 1;
	totalSizeBytes += res->GetResSizeBytes();
}

void zCResourceManager::zCClassCache::TouchRes(zCResource *res)
{
	// moves res to the front of the list,
	// if not already there AND older than previous
	auto prev = res->prevRes;
	if ( prev && prev->timeStamp < res->timeStamp && res != firstRes ) {
		if ( res == lastRes )
			lastRes = prev;

		if ( auto prev = res->prevRes )
			prev->nextRes = res->nextRes;
		if ( auto next = res->nextRes )
			next->prevRes = res->prevRes;

		res->prevRes = 0;
		res->nextRes = firstRes;

		if ( !lastRes )
			lastRes = res;
		if ( firstRes )
			firstRes->prevRes = res;
		firstRes = res;
	}
}

void zCResourceManager::zCClassCache::RemoveRes(zCResource *res)
{
	if ( res->cacheOutLock ) {
		if ( res == lastRes )
			lastRes = res->prevRes;
		if ( res == firstRes ) {
			firstRes = res->nextRes;
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

//------------------------------------------------------------------------------
void zCResourceManager::PrintStatusDebug()
{
	int y = 1600;
	if (cacheInImmedMsec > 0.0)
		screen->Print(0, y, "cacheInImmedMsec: "_s + cacheInImmedMsec);

	auto fonty = screen->FontY();
	y += fonty;
	if ( !threadingEnabled )
		screen->Print(0, y, "threading DISABLED");

	y += fonty;

	if ( queued > 0 )
		screen->Print(0, y, "queued: "_s + queued);

	y += fonty;
	if ( loaded > 0 )
		screen->Print(0, y, "loaded: "_s + loaded);

	y += fonty;

	for (auto& clas : classCache) {
		if (clas->numResources > 0) {
			zSTRING info;
			info += clas->classDef->className;
			info += ": num: " + clas->numResources;
			info += "/" + clas->maxNumResources / 1000;
			info += ", kb: " + (clas->totalSizeBytes / 1024);
			// ((274877907 * totalSizeBytes) >> 6) >> 32

			int locked = 0;
			for (auto res = clas->firstRes; res; res = res->nextRes)
				if (res->locked) ++locked;

			if (locked > 0)
				info += ", locked: "_s + locked;

			screen->Print(0, y, info);
		}
		y += fonty;
	}

	LockCacheInQueue();

	auto res = queueStart;
	y = 4000;
	while ( res && y < 0x2000 ) {
		zSTRING info;
		auto prio = res->cacheInPriority / 65535.0;
		info += res->_GetClassDef()->className + ", ";
		info += zSTRING(prio, 2) + ", ";
		info += res->GetObjectName();

		res = res->nextRes;
		y += 200;
	}
	UnlockCacheInQueue();
	if ( zinput->KeyToggled(KEY_8) )
		PurgeCaches(0);
}
