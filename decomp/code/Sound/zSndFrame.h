struct zCSndFrame {
	zCSndFrame() = default;
	~zCSndFrame()
	{
		zCActiveSnd:::RemoveSoundsByFrame( this );
		zCWavePool::GetPool()->ReleaseWave(waveData);
	}

	void CacheOut()
	{
		zCWavePool::GetPool().CacheOut(waveData);
	}

	void CacheIn()
	{
		if (waveData) {
			zCWavePool::GetPool().CacheIn(waveData);
		} else {
			zCWavePool::GetPool().CacheIn(file);
		}
	}

	int CalcPitchVariance()
	{
		if (pitchVar == 0.0)
			return pitch;

		auto min = pow(0.9438743, pitchVar) * pitch;
		auto max = pow(1.0594631, pitchVar) * pitch;
		return (zRandF() * (max - min) + min + 0.5)
	}

	void CalcPitchOffset(float offset)
	{
		pitchOff = offset;
		pitch = waveData->waveInfo->rate;
		if ( offset != -999999.0 ) {
			if (offset == 0.0)
				return;

			float power = offset > 0.0 ? 1.0594631 : 0.9438743;
			offset = fabs(offset);
			pitch *= pow(power, offset);
		}
	}

	void SetDefaultProperties()
	{
		volume = defaultVol;
		pan = 64;
		if (!waveData->sizeBytes)
			waveData->CacheIn();
		if ( waveData->sizeBytes ) {
			CalcPitchOffset( pitchOff );
		} else {
			pitch = 0;
		}
	}


	zSTRING file;
	float pitchOff = 0;
	int pitchVar   = 0;
	int defaultVol = 127;
	int loop = 0;
	int loopStartOffset = 0;
	int loopEndOffset   = -1;
	float reverbLevel = 0.5;
	zSTRING pfxName;
	int unk2;
	zSTRING instanceName;
	int volume;
	int pitch;
	int pan;
	zCWaveData *waveData;
};
