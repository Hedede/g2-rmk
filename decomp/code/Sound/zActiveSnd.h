struct zCActiveSnd {
	static unsigned lastHandle = 0;
	static unsigned GetNextFreeHandle()
	{
		return ++lastHandle;
	}

	static void RemoveSoundsByFrame(zCSndFrame *frame)
	{
		for (int i = 0; i < activeSndList.GetNum(); ++i ) {
			if ( activeSndList[i]->frame == frame )
				RemoveSound(activeSndList[i--]);
		}
	}

	zCActiveSnd() { Initialize(); }
	~zCActiveSnd() { Release(); }

	int HasBeenUpdatedThisFrame() const
	{
		if ( listener && listener->homeWorld )
			return updateFrameCtr == listener->homeWorld->masterFrameCtr;
		return true;
	}

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
	char bitField;
	char unko;
	char __volume;
	int frequency;
	int __lastUpdate;
	int updateFrameCtr;
	int unk[4];
	zCVob *sourceVob;
	zCSndFrame *frame;
	zCSoundFX *sndFx;
};
