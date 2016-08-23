// _bert/zBinkPlayer.cpp
struct zCBinkPlayer : zCVideoPlayer {
	zCBinkPlayer() : zCVideoPlayer()
	{
		zINFO(3,"B: VP: Initialize zBinkPlayer"); // 45, 
	}
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
	virtual void IsPlaying()
	{
		return zCVideoPlayer::IsPlaying && binkHandle &&
		       (unk1 || bink->Frames > bink->FrameNum);
	}
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

	int PlayGotoNextFrame() override
	{
		BinkNextFrame(binkHandle);
		return 1;
	}
	void PlayWaitNextFrame() override
	{
		while ( IsPlaying() ) {
			if ( !BinkWait(binkHandle) )
				break;
			PlayHandleEvents();
		}
		return 1;
	}
	bool PlayHandleEvents() override
	{
		return handleEvents == 0;
	}

private:
	BINK* binkHandle = 0;
	int pixelFormat = -1;
	__int16 unkz1 = 0;
	__int16 unkz2 = 0;
	int unkz3 = 0;
	int handleEvents = 1;
	int fullScreen = 1;
	zSTRING unkz3;
	int unkz4;
	int unkz5[5];
};
