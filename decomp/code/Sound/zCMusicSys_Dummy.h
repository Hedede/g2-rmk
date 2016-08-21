struct zCMusicSys_Dummy : zCMusicSystem {
	virtual ~zCMusicSys_Dummy() = default;

	zCMusicTheme* LoadThemeByScript(zSTRING	const &) override
	{
		return new zCMusicTheme{};
	}
	zCMusicTheme* LoadTheme(zSTRING	const &) override
	{
		return new zCMusicTheme{};
	}

	zCMusicJingle* LoadJingleByScript(zSTRING const &) override
	{
		return new zCMusicJingle{};
	}
	zCMusicJingle* LoadJingle(zSTRING const &) override
	{
		return new zCMusicJingle{};
	}

	void DoMusicUpdate() override {}
};
