struct zCSndFX_MSS : zCSoundFX {
	zCSndFX_MSS() = default;
	virtual ~zCSndFX_MSS();
	virtual void LoadResourceData();
	virtual void ReleaseResourceData();
	virtual void GetResSizeBytes();
	int GetNumChannels() override
	{
		return channels.GetNum();
	}
	int GetNumChannelFrames(int num) override
	{
		return channels[num]->frames.GetNum();
	}
	void GetChannelFrame(int,int) override { }
	void SetChannelFrame(int channel, int frameNr) override
	{
		selFrame = frameNr;
		curChannel = channel;

		int numFrames = GetNumChannelFrames(channel);
		if ( numFrames <= 1 ) {
			frameNr = 0;
		} else if ( frameNr == -1)
			frameNr = zRandF() * (numFrames - 1) + 0.5;
		}
		curFrame = frameNr;
		channels[curChannel]->actFrame = curFrame;;
	}

	double GetPlayingTimeMSEC() override
	{
		CacheIn( -1.0 );
		return channels[curChannel]->frames[curFrame]->waveData->playingTime;
	}

	void GetPan() override { return pan; }
	virtual void SetPan(float);
	void GetVolume() override { return volume; }
	void SetVolume(float vol) override
	{
		for (auto* chan : channels) {
			for (auto* frame : chan->frames)
				frame->volume = frame->defaultVol * vol;
		}
		volume = vol
	}
	int GetFrequency() override { return frequency; }
	virtual void SetFrequency(int);
	void SetLooping(int b) override
	{
		for (auto* chan : channels) {
			for (auto* frame : chan->frames)
				frame->loop = b;
		}
	}
	bool IsLooping() override
	{
		return channels[0]->frames[0]->loop;
	}
	void SetIsFixed(int b) override { isFixed = b; }
	bool GetIsFixed() override { return isFixed; }

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
	void CacheInActChannel();
	void CacheInActFrame()
	{
		channels[curChannel]->frames[curFrame]->CacheIn();
	}
	void CacheOutActFrame()
	{
		channels[curChannel]->frames[curFrame]->CacheOut();
	}
	void SetDefaultProperties()
	{
		for (auto* chan : channels) {
			for (auto* frame : chan->frames)
				frame->SetDefaultProperties();
		}
	}

private:
	zBOOL isFixed  = false;
	int curChannel = 0;
	int curFrame   = 0;
	int selFrame   = -1;
	zCArray<zCSndChannel*> channels;
};

//------------------------------------------------------------------------------
// _carsten/zSndMss.cpp
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



// private
void zCSndFX_MSS::CacheInActChannel()
{
	if (auto chan = channels[curChannel]) {
		for (auto& frame : chan->frames)
			frame->waveData->CacheIn();
	}
}

int zCSndFX_MSS::LoadResourceData()
{
	zINFO( 8, "C: zCSndFX_MSS :: LoadResourceData() for " + GetObjectName()); //2671

	for ( int i = 0; i < GetNumChannels(this); ++i ) {
		auto channel = channels[i];
		for (auto frame : channel->frames) {
			frame->CacheIn(); // was inlined
		}
	}
	return 1;
}
