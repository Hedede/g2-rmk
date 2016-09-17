//_carsten\\zSndMss.cpp
struct zCWavePool {
	static zCWavePool& GetPool()
	{
		static zCWavePool thePool;
		return thePool;
	}

	zCWavePool()
	{
		waveList.Compare = CompareWaves;
		lastData = new zCWaveData;
	}

	void CacheIn(zCWaveData* wave)
	{
		if (!wave)
			return;

		zINFO(10,"C: SND: zCWavePool::CacheIn() for wave: " + wave->GetName()); // 3766,
		critSect.Lock(-1);
		wave->CacheIn();
		critSect.Unlock();
	}

	void CacheOut(zCWaveData *wave)
	{
		if (!wave)
			return;
		zINFO(10,"C: SND: zCWavePool::CacheOut() for wave: " + wave->GetName()); // 3775,
		critSect.Lock(-1);
		wave->CacheOut();
		critSect.Unlock();
	}

	void ReleaseWave(zCWaveData *wave)
	{
		if ( !wave )
			return;

		critSect.Lock(-1);

		waveList.RemoveOrder(wave);

		wave->Release();

		critSect.Unlock();
	}

	// Present in G1demo, not present in G2
	void DebugInfo();
	void DebugInfo2();

private:
	zCArraySort<zCWaveData*> waveList;
	zCWaveData *lastData;
	zCCriticalSection critSect;

	void *wtf = 0;
	int i1    = 0;
	int i2    = 0;
};
