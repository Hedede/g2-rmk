class zCResourceManager {
	virtual ~zCResourceManager();
	virtual void ThreadProc();
	void SetThreadingEnabled(zBOOL enabled)
	{
		this->threadingEnabled = enabled;
	}
};
