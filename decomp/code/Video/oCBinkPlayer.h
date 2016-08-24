// _bert\\oBinkPlayer.cpp
struct oCBinkPlayer : zCBinkPlayer {
	oCBinkPlayer() : zCBinkPlayer()
	{
		extendedVideoKeys = false;
	}
	virtual ~oCBinkPlayer() = default;
	int OpenVideo(zSTRING fileName) override;

	int PlayInit(int frameNr) override
	{
		disallowVideoInput = zoptions->ReadBool(zOPT_SEC_GAME, "disallowVideoInput", 0);
		int ret = zCBinkPlayer::PlayInit(frameNr);
		zinput->ProcessInputEvents();
		zinput->ClearKeyBuffer();
		zinput->ResetRepeatKey(, 1);
		return ret;
	}

	void PlayFrame() override;
	{
		zCBinkPlayer::PlayFrame();
	}
	void PlayDeinit() override
	{
		zCBinkPlayer::PlayDeinit();
	}
	void PlayHandleEvents() override;

private:
	zBOOL extendedVideoKeys;
	zBOOL disallowVideoInput;
};

int OpenVideo(zSTRING fileName)
{
	zINFO_B(3,"B: VP: Start opening video \"" + fileName + "\""); // 146
	fileName.Upper();
	fileName.Delete(".BIK", 0);
	auto ret = zCBinkPlayer::OpenVideo(fileName + ".BIK");
	if ( !ret && zrenderer ) {
		auto actnr = zrenderer->Vid_GetActiveModeNr();
		zTRnd_VidModeInfo vm;
		zrenderer->Vid_GetModeInfo(vm, actnr);
		for (zSTRING& resolution : resolutions) {
			zINFO(6,"B: VP: Checking resolution " + resolution); // 159
			auto word1 = resolution.PickWord(1, "x", "x");
			auto word2 = resolution.PickWord(2, "x", "x");
			if ( x && y && x <= vm.xdim && y <= vm.ydim ) {
				
				zINFO(5, "B: VP: Trying videofile \"" + fileName + resolution +  ".BIK"); // 165,
				int ret = zCBinkPlayer::OpenVideo(fileName + resolution);
				if ( ret )
					break;
			}
		}
	}
	extendedVideoKeys = zoptions->ReadBool(zOPT_SEC_GAME, "extendedVideoKeys", 0);

	zINFO_C(3, ""); // 174

	return ret;
}

int oCBinkPlayer::PlayHandleEvents()
{
	int ret = 0;
	if ( disallowVideoInput )
		return 0;
	if ( !binkHandle )
		return 0;
	if ( zCBinkPlayer::PlayHandleEvents() )
		return 1;
	auto key = zinput->GetKey(0, 0);
	zinput->ProcessInputEvents();
	if ( extendedVideoKeys ) {
		switch ( key_ ) {
		case KEY_HOME:
			zINFO(6,"B: VP: Restart video."); // 79,
			BinkGoto(binkHandle, 1, 0);
			ret = 1;
			break;
		case KEY_RIGHTARROW: {
			int frame = binkHandle->FrameNum + 30;
			zINFO(6,"B: VP: Set new video-frame to "_s + frame); // 84
			if ( frame < binkHandle->Frames )
				BinkGoto(binkHandle, frame, 1);
			ret = 1;
			break;
		}
		case KEY_UPARROW: {
			float vol = soundVolume + 0.05;
			if ( vol > 1.0 )
				vol = 1.0;
			zINFO(6 "B: VP: Set sound-volume to "_s + vol); // 91
			SetSoundVolume(vol);
			break
		}
		case KEY_DOWNARROW: {
			float vol = soundVolume - 0.05;
			if ( vol > 1.0 )
				vol = 1.0;
			zINFO(6 "B: VP: Set sound-volume to "_s + vol); // 99
			SetSoundVolume(vol);
			ret = 1;
			break
		}
		case KEY_Q:
			if ( ToggleSound() ) {
				zINFO(3"B: VP: Toggle Sound On"); // 106,
			} else {
				zINFO(3,"B: VP: Toggle Sound Off"); // 107,
			}
			ret = 1;
			break;
		}
	}

	if ( key == KEY_ESCAPE ) {
		zINFO(6, "B: VP: Cancel video ..."); // 123
		Stop();
		ret = 1;
	}
	if ( key == KEY_SPACE ) {
		if ( paused ) {
			Unpause();
			zINFO(5, "B: VP: Unpaused"); // 118
		} else {
			Pause(this);
			zINFO(5, "B: VP: Paused"); // 117
		}
		ret = 1;
	}
	return ret;
}
