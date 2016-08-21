class zCResourceManager : public zCThread {
public:
	struct zCClassCache {
		~zCClassCache() = default;
	};

	virtual ~zCResourceManager();
	virtual void ThreadProc();

	void SetThreadingEnabled(zBOOL enabled)
	{
		this->threadingEnabled = enabled;
	}

	zBOOL GetThreadingEnabled() const
	{
		return threadingEnabled;
	}

	void SetShowDebugInfo(zBOOL b)
	{
		this->showDebugInfo = b;
	}

	zBOOL SetShowDebugInfo()
	{
		return showDebugInfo;
	}

private:
	zCResource* GetNextCacheInQueue()
	{
		return RemoveCacheInQueue(0);
	}
private:

};
