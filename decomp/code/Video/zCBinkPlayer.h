struct zCBinkPlayer : zCVideoPlayer {
	virtual ~zCBinkPlayer();
	virtual void OpenVideo(zSTRING);
	virtual void CloseVideo();
	virtual void PlayInit(int);
	virtual void PlayFrame();
	virtual void PlayDeinit();
	virtual void Pause()
	{
		BinkPause(binkHandle, 1);
		return zCVideoPlayer::Pause();
	}
	virtual void Unpause();
	{
		BinkPause(binkHandle, 0);
		return zCVideoPlayer::Unpause();
	}
	virtual void IsPlaying();
	virtual void ToggleSound()
	{
		zCVideoPlayer::ToggleSound();
		if ( soundEnabled ) {
			SetSoundVolume(1.0);
		} else {
			SetSoundVolume(0.0);
		}
		return soundEnabled;
	}
	virtual void SetSoundVolume(float vol)
	{
		BinkSetVolume(binkHandle, 0, (vol * 65536.0));
	}

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
