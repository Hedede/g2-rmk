//_carsten\\zSndMss.cpp
struct zCWaveData {
	zCWaveData() { Init(); }
	~zCWaveData()
	{
		if ( refCtr ) {
			zFATAL("C: zSndMSS.h(zCWaveData::~zCWaveData()) refCtr not zero!"); // 3890
		}

		CacheOut();
		Delete(waveInfo);
	}

	void Release()
	{
		if (!--refCtr)
			delete this;
	}

	zSTRING GetName() const
	{
		return name;
	}

	void CacheOut()
	{
		if (!--usedCtr && soundData) {
			if ( soundOwnedByAIL ) {
				AIL_mem_free_lock(soundData);
				delete waveInfo;
				waveInfo = 0;
			} else {
				delete(soundData);
			}
			soundData = 0;
			sndMemUsed -= sizeBytes;
		}
	}

	void CacheIn();

	int CheckForCorrectFormat(int is3D)
	{
		if ( !sizeBytes )
			CacheIn();
		if ( sizeBytes )
			return 0;
		if ( !is3D )
			return 1;

		if ( waveInfo->channels == 2 ) {
			zWARNING("C: SND: PlaySound3D: SoundFile " + name + " has to be a mono file"); // 4057,
			return 0;
		}

		switch ( waveInfo->format ) {
		default:
			zWARNING("C: PlaySound3D: SoundFile " + name + " is a unknown format (did you try to play a compressed file as non-ambient 3d sound ?)"); // 4069
			return 0;
		case 2:
		case 3:
		case 7:
			zWARNING("C: SND: PlaySound3D: SoundFile " + name + " has to be a mono file"); // 4074
			return 0;
		case 0:
		case 1:
		case 5:
			return 1;
		}
	}

private:
	void Init()
	{
		waveInfo = 0;
		refCtr = 1;
		soundData = 0;
		sizeBytes = 0;
		playingTime = 0;
		usedCtr = 0;
		soundOwnedByAIL = 0;
	}

	int sizeBytes;
	void *soundData;
	int playingTime;
	_AILSOUNDINFO *waveInfo;
	zSTRING name;
	int refCtr;
	int usedCtr;
	int soundOwnedByAIL;
};

