struct oCBinkPlayer : zCBinkPlayer {
	oCBinkPlayer() : zCBinkPlayer()
	{
		extendedVideoKeys = false;
	}
	virtual ~oCBinkPlayer() = default;
	virtual void OpenVideo(zSTRING);

	virtual int PlayInit(int frameNr)
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
	virtual void PlayHandleEvents();

private:
	zBOOL extendedVideoKeys;
	zBOOL disallowVideoInput;
};
