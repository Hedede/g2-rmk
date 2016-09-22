struct zCSndFX_MSS : zCSoundFX {
	virtual ~zCSndFX_MSS();
	virtual void LoadResourceData();
	virtual void ReleaseResourceData();
	virtual void GetResSizeBytes();
	virtual void GetNumChannels();
	int GetNumChannelFrames(int num) override
	{
		return channels[num]->frames.GetNum();
	}
	virtual void GetChannelFrame(int,int) override { }
	virtual void SetChannelFrame(int,int);
	virtual void GetPlayingTimeMSEC();
	virtual void GetPan();
	virtual void SetPan(float);
	virtual void GetVolume();
	virtual void SetVolume(float);
	virtual void GetFrequency();
	virtual void SetFrequency(int);
	virtual void SetLooping(int);
	virtual void IsLooping();
	virtual void SetIsFixed(int);
	virtual void GetIsFixed();

	// actually no such func in .exe
	zCSndChannel* GetCurChannel()
	{
		return channels[curChannel];
	}
	// actually no such func in .exe
	zCSndFrame* GetCurFrame()
	{
		return GetCurChannel()->frames[curFrame];
	}

private:
	void ChooseVariance();

private:
	zBOOL isFixed;
	int curChannel;
	int curFrame;
	int selFrame;
	zCArray<zCSndChannel*> channels;
};

//------------------------------------------------------------------------------
void zCSndFX_MSS::ChooseVariance()
{
	auto num_frames = GetNumChannelFrames(curChannel) - 1;
	if (num_frames <= 0) {
		curFrame = 0;
	} else {
		if (selFrame == -1) {
			// XXX: appears to be some macro or inlined func
			curFrame = rand() * num_frames / 32767.0 + 0.5;
		} else {
			curFrame = selFrame;
		}
	}
}
