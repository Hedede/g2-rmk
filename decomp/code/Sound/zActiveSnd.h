struct zCActiveSnd {
	static unsigned lastHandle = 0;
	static unsigned GetNextFreeHandle()
	{
		return ++lastHandle;
	}

	static void InitPreAllocedData()
	{
		for (int i = 50; i; --i)
			preAllocedSndList.InsertEnd(new zCActiveSnd);
	}

	static void CleanUpPreAllocedData()
	{
		for (auto& snd : preAllocedSndList)
			delete snd;
		preAllocedSndList.DeleteList();
	}

	static void AddSound(zCActiveSnd* snd)
	{
		activeSndList.InsertSort(snd);
	}

	static int GetNumActive() { return activeSndList.GetNum(); }

	static int GetNumAmbientSamplesUsed()
	{
		int count = 0;
		for (auto& snd : activeSndList) {
			if (!snd->bitfield.is3d && snd->sampleHandle)
				++count;
		}
		return count;
	}

	static int GetNum3DSamplesUsed()
	{
		int count = 0;
		for (auto& snd : activeSndList) {
			if (snd->bitfield.is3d && snd->sampleHandle3d)
				++count;
		}
		return count;
	}

	static void RemoveSoundsByFrame(zCSndFrame *frame)
	{
		for (int i = 0; i < GetNumActive(); ++i ) {
			if ( activeSndList[i]->frame == frame )
				RemoveSound(activeSndList[i--]);
		}
	}

	static bool GetSoundActive(zCActiveSnd* snd)
	{
		if (activeSndList.Search(snd) != -1)
			return snd.bitfield.playing;
		return false;
	}

	static int GetHandleIndex(int handle)
	{
		static zCActiveSnd tmpsnd;
		tmpsnd.handle = handle;
		return activeSndList.Search(&tmpsnd);
	}

	static zCActiveSnd* GetHandleSound(int handle)
	{
		auto idx = GetHandleIndex(handle);
		if (idx == -1)
			return nullptr;
		return activeSndList.GetSafe(idx);
	}


	static bool GetHandleActive(int handle)
	{
		if (auto snd = GetHandleSound(handle)) {
			return snd->IsActive();
		}
		return nullptr;
	}

	static bool GetFrameActive(zCSndFrame *frame)
	{
		if (!frame)
			return false;

		for (auto& snd : activeSndList) {
			if (snd->bitfield.playing && snd->frame == frame)
				return true;
		}
		return false;
	}

	static zCActiveSnd* AllocNextFreeSnd();

	zCActiveSnd() { Initialize(); }
	~zCActiveSnd()
	{
		Release();
		nextFreeSnd = nullptr;
	}

	void Release()
	{
		Release(sourceVob);
		Release(sndFx);

		if ( sampleHandle3d )
			AIL_release_3D_sample_handle(sampleHandle3d);
		if ( sampleHandle )
			AIL_release_sample_handle(sampleHandle);

		Initialize();
		nextFreeSnd = this;
	}

	int HasBeenUpdatedThisFrame() const
	{
		if ( listener && listener->homeWorld )
			return updateFrameCtr == listener->homeWorld->masterFrameCtr;
		return true;
	}

	// no such funk in .exe
	bool IsActive()
	{
		if (bitfield.playing)
			return true;
		if (sampleHandle)
			return true;
		if (sampleHandle3d)
			return true;
		return false;
	}

	int CalcListenerPan() { return 64; }

private:
	int handle;
	int sampleHandle;
	int sampleHandle3d;
	int __updateCtr;
	int unk1[1];
	float radius;
	int reverbLevel;
	int unk2[2];
	float unk3_;
	int unk4[3];
	struct {
		uint8_t playing : 1; // 1
		uint8_t loop    : 1; // 2
		uint8_t unk1    : 1; // 4
		uint8_t is3d    : 1; // 8
		uint8_t in_use  : 1; // 16
		uint8_t slot    : 3;
	} bitfield;
	char unko;
	char volume;
	int frequency; // pitch?
	int __lastUpdate;
	int updateFrameCtr;
	int unk[4];

	zCVob *sourceVob;
	zCSndFrame *frame;
	zCSoundFX *sndFx;
};

int zCActiveSnd::RequestChannel()
{
	if ( bitfield.is3d )
		return RequestChannel3D();

	sampleHandle = handleManager.AllocSample();
	if ( sampleHandle )
		return 1;

	// in original it is index based, but i chose ptrs
	zCActiveSnd* reusable  = nullptr;
	zCActiveSnd* unusedest = activeSndList[0];
	for (auto& snd : activeSndList) {
		if (snd->sampleHandle) {
			if (snd->GetVolume() < 0.05) {
				reusable = snd;
				break;
			}
			if (snd->__updateCtr <= reusable->__updateCtr) {
				reusable  = snd;
				unusedest = snd;
			}
		}
	}

	if ( !reusable->sampleHandle ) {
		for (auto& snd : activeSndList) {
			if (snd->sampleHandle)
				reusable = snd;
		}
	}

	if ( !reusable->sampleHandle ) {
		zWARNING("C: ARGLKABARGL: (zCActiveSnd::RequestChannel): no sample found"); // 3367,
		return 0;
	}

	AIL_release_sample_handle(reusable->sampleHandle);
	reusable->sampleHandle = 0;
	reusable->playing = false;
	sampleHandle = AIL_allocate_sample_handle(sndDrv);
	if ( sampleHandle )
		return true;

	zWARNING("C: ARGLKABARGL2: (zCActiveSnd::RequestChannel): no sample found"); // 3380
	return 0;
}

int zCActiveSnd::RequestChannel3D()
{
	sampleHandle3d = handleManager.Alloc3DSample();
	if ( sampleHandle3d )
		return true;

	if (activeSndList.IsEmpty()) {
		zWARNING("C: ARGLKABARGL: (zCActiveSnd::RequestChannel3D): no sample3d found"); // 3411
		return 0;
	}

	zCActiveSnd* reusable  = nullptr;
	zCActiveSnd* unusedest = activeSndList[0];
	for (auto& snd : activeSndList) {
		if (snd->sampleHandle3d) {
			if (snd->GetVolume() < 0.05) {
				reusable = snd;
				break;
			}
			if (snd->__updateCtr <= reusable->__updateCtr) {
				reusable  = snd;
				unusedest = snd;
			}
		}
	}

	if ( !reusable->sampleHandle ) {
		for (auto& snd : activeSndList) {
			if (snd->sampleHandle3d)
				reusable = snd;
		}
	}
	if ( !reusable->sampleHandle3d ) {
		zWARNING("C: ARGLKABARGL: (zCActiveSnd::RequestChannel3D): no sample3d found"); // 3437,
		return 0;
	}

	AIL_release_3D_sample_handle(reusable->sampleHandle3d);
	reusable->sampleHandle3d   = 0;
	reusable->bitfield.playing = 0;
	sampleHandle3d = AIL_allocate_3D_sample_handle(act3dProvider);
	if ( sampleHandle3d )
		return 1;

	zWARNING("C: ARGLKABARGL: (zCActiveSnd::RequestChannel3D): no sample3d found"); // 3449,
	return 0;
}

zCActiveSnd* zCActiveSnd::AllocNextFreeSnd()
{
	auto result = nextFreeSnd;
	if (result) {
		result->bitfield.in_use = true;
		nextFreeSnd = nullptr;
		return result;
	}

	for (auto& snd : preAllocedSndList) {
		if (!snd->bitfield.in_use) {
			snd->bitfield.in_use = true;
			return snd;
		}
	}

	result = new zCActiveSnd;
	preAllocedSndList.InsertEnd(result);
	result->bitfield.in_use = true;
	return result;
}
