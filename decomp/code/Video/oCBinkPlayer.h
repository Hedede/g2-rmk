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
		// TODO
	}
}
