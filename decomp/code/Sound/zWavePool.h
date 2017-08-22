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
		tmpWave = new zCWaveData;
	}

	zCWaveData* CacheIn(zSTRING& fileName)
	{
		critSect.Lock(-1);

		tmpWave->name = fileName;

		zCWaveData* wave;
		auto idx = waveList.Search(tmpWave);
		if (idx == -1) {
			wave = new zCWaveData(fileName);
			waveList.InsertSort(wave);
		}

		wave->CacheIn();

		critSect.Unlock();
		return wave;
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

	void DebugInfo();
	// Used in G1demo, but unused in G2
	void DebugInfo2();

	// seems to be bullshit and not used
	void NewFrame();

private:
	zCArraySort<zCWaveData*> waveList;
	zCWaveData *tmpWave;
	zCCriticalSection critSect;

	// used by NewFrame(), i.e. unused
	zCArray<zCWaveData*> waveList2;
};


int zCWavePool::NewFrame()
{
	waveList2.DeleteList();
	auto num = zCActiveSnd::activeSndList.GetNum();
	for (auto i = 0; i < num; ++i) {
		auto* frame = zCActiveSnd::activeSndList[i]->frame;
		if (!frame)
			continue;
		auto* wave = frame->waveData;
		if (!wave || !wave->soundData)
			continue;
		if (waveList2.Search(wave) == -1)
			waveList2.Insert(wave);
	}
	return num;
}
