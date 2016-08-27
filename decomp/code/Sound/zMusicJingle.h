class zCMusicJingle {
	zCMusicJingle() = default;
	virtual ~zCMusicJingle() = default;
	virtual void AddRef()
	{
		if (this)
			delete this;
	}
	virtual void Release()
	{
		++refCount;
	}
	virtual void IsPlaying() { return 0; }
	virtual void StopPlaying() {}
	virtual void CacheIn() {}
	virtual void CacheOut() {}

private:
	zSTRING name;
	int loop;
	float vol;
	int transSubType;
	char unk[4];
	int refCount = 1;
	zSTRING name2;
};
