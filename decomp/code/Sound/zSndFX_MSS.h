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
	void GetPan() override { return pan; }
	virtual void SetPan(float);
	void GetVolume() override { return volume; }
	virtual void SetVolume(float);
	int GetFrequency() override { return frequency; }
	virtual void SetFrequency(int);
	virtual void SetLooping(int);
	virtual void IsLooping();
	void SetIsFixed(int b) override { isFixed = b; }
	void GetIsFixed() override { return isFixed; }

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
