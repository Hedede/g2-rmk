struct zCBinkPlayer : zCVideoPlayer {
	virtual ~zCBinkPlayer();
	virtual void OpenVideo(zSTRING);
	virtual void CloseVideo();
	virtual void PlayInit(int);
	virtual void PlayFrame();
	virtual void PlayDeinit();
	virtual void Pause();
	virtual void Unpause();
	virtual void IsPlaying();
	virtual void ToggleSound();
	virtual void SetSoundVolume(float);
	virtual void PlayGotoNextFrame();
	virtual void PlayWaitNextFrame()
	{
		BinkNextFrame(binkHandle);
		return 1;
	}
	virtual bool PlayHandleEvents()
	{
		return handleEvents == 0;
	}

private:
	BINK* binkHandle;
};
