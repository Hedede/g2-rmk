struct zCSndFrame {
	void CacheOut()
	{
		zCWavePool::GetPool().CacheOut(waveData);
	}

	int CalcPitchVariance()
	{
		if (pitchVar == 0.0)
			return pitch;

		auto min = pow(0.9438743, pitchVar) * pitch;
		auto max = pow(1.0594631, pitchVar) * pitch;
		// XXX: appears to be some macro or inlined func
		return (rand() / 32767 * (max - min) + min + 0.5)
	}

	zSTRING file;
	int pitchOff;
	int pitchVar;
	int vol;
	int loop;
	int loopStartOffset;
	int loopEndOffset;
	float reverbLevel;
	zSTRING pfxName;
	int unk2;
	zSTRING instanceName;
	int volume;
	int pitch;
	int pan;
	zCWaveData *waveData;
};
