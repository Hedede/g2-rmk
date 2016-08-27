struct zCMusicTheme_DM : zCMusicTheme {
	~zCMusicTheme() override
	{
		Delete(handle);
	}
	bool IsActive() override { return 0; }

private:
	UNK handle;
};
