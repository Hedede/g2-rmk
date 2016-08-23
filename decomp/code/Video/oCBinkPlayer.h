struct oCBinkPlayer : zCBinkPlayer {
	virtual ~oCBinkPlayer();
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

	virtual void PlayFrame();
	virtual void PlayDeinit();
	virtual void PlayHandleEvents();

private:
	bool disallowVideoInput;
};
