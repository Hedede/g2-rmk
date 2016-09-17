//_carsten\\zSndMss.cpp
struct zCWaveData {
	zCWaveData() { Init(); }
	zCWaveData(zSTRING& name) : zCWaveData() { this->name = name }
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

	int GetSize()
	{
		if (sizeBytes == 0)
			CacheIn();
		return sizeBytes;
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


void zCWaveData::CacheIn()
{
	if ( !soundData ) {
		zPATH path(name);
		auto dir = zoptions->GetDirString(DIR_SOUND);

		if (path.file->SearchFile(name, dir, 1) == 5003) {
			zWARNING("C: zSndMSS.h(zCWaveData::zCWaveData): Wave not found: " + name); // 3947,
			auto dir = zoptions->GetDirString(DIR_SOUND);

			if (path.file->SearchFile(MSS_DEF_WAVE, dir, 1))
				zFATAL("C: zSndMSS.h(zCWaveData::zCWaveData): Wave not found: " + MSS_DEF_WAVE); // 3948
		}

		auto zfile = zfactory->CreateZFile(path.GetPath());

		if ( zfile->Open2(0) ) {
			zFAULT("C: zCWaveData::CacheIn(): could not open file: " + path.GetPath()); // 3986
		} else {
			sizeBytes = zfile->Size();
			soundData = new char[sizeBytes];
			zfile->Read(soundData, sizeBytes);
			zfile->Close();
			if ( !waveInfo ) {
				waveInfo = new _AILSOUNDINFO;
				AIL_WAV_info(soundData, waveInfo);
			}
			if ( !sizeBytes )
				CacheIn();

			if ( AIL_file_type(soundData, sizeBytes) == 2 ) {
				zWARNING(9,"C: SND: zCWaveData::CacheIn(): decompressing ADPCM Wave: " + GetName()); // 3972,

				char* ptr;
				int size;
				if ( !AIL_decompress_ADPCM(waveInfo, &ptr, &sizw) ) {
					zFATAL("C: zCWaveData::CacheIn(): could not decompress ADPCM file " + GetName()); // 3976,
				}

				delete[] soundData;
				soundData = ptr;
				sizeBytes = size;
				AIL_WAV_info(soundData, waveInfo);
				soundOwnedByAIL = 1;
			}
		}
		delete zfile;

		sndMemUsed += sizeBytes;
	}

	if ( playingTime != 0.0 ) {
		++usedCtr;
		return;
	}

	auto actSnd = zCActiveSnd::AllocNextFreeSnd();
	if ( actSnd->RequestChannel() ) {
		AIL_set_sample_file(actSnd->sampleHandle, soundData, 0);
		AIL_sample_ms_position(actSnd->sampleHandle, &playingTime, 0);
		AIL_release_sample_handle(actSnd->sampleHandle1);
		actSnd->Release();
		++usedCtr;
		return;
	}

	zWARNING("C: SND: (zCWaveData::CacheIn()) could not allocate sample handle for wave duration evaluation (Name:" + GetName() +  ")"); // 4003,
}
