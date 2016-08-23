// _bert/zVideoPlayer.cpp
struct zCVideoPlayer {
	virtual ~zCVideoPlayer() = default;
	virtual void OpenVideo(zSTRING file)
	{
		if ( (IsPlaying() ) {
			zFAULT("B: VP: Can't open a video: Another video is still playing!"); // 36
			return 0;
		}

		fileName = file;
		return 1;
	}

	virtual int CloseVideo()
	{
		if (IsPlaying()) {
			zFAULT("B: VP: Can't close video: It's still playing!"); // 50
			return 0;
		}

		paused = 0;
		playing = 1;
		return 1;
	}

	virtual int PlayInit(int frameNr)
	{
		paused = 0;
		playing = 1;
		return 1;
	}

	virtual int PlayFrame() { return 0; }
	virtual int PlayDeinit()
	{
		paused = 0;
		playing = 0;
		return 1;
	}
	virtual int Pause()
	{
		if (playing && !paused) {
			paused = 1;
			return 1;
		}
		return 0;
	}
	virtual int Unpause()
	{
		if (playing) {
			paused = 0;
			return 1;
		}
		return 0;
	}
	virtual bool IsPlaying()
	{
		return playing;
	}
	virtual int ToggleSound()
	{
		return soundEnabled = !soundEnabled;
	}
	virtual void SetSoundVolume(float vol)
	{
		soundVolume = vol;
	}
	virtual int PlayGotoNextFrame() { return 1; }
	virtual int PlayWaitNextFrame() { return 1; }
	virtual void PlayHandleEvents();

	int Play()
	{
		zerr.Separator("");

		PlayInit(frameNr);
		while ( IsPlaying() )
			PlayFrame();
		PlayDeinit();

		zerr.Separator("");

		return 1;
	}

	int Stop()
	{
		if ( playing ) {
			playing = 0;
			return 1;
		}
		return 0;
	}

	zSTRING GetVideoFilename() const
	{
		return fileName;
	}



private:
	zSTRING fileName;
	int unk1    = 0;
	int paused  = 0;
	int playing = 0;

	int soundEnabled  = 1;
	float soundVolume = 1.0;
	float unk2 = 1.0;
};
