class zCMusicSys_DirectMusic : zCMusicSystem {
	virtual ~zCMusicSys_DirectMusic();

	virtual void PlayThemeByScript(zSTRING	const &,int,int	*);
	virtual void LoadThemeByScript(zSTRING const &);
	virtual void LoadTheme(zSTRING const &);
	virtual void PlayTheme(zCMusicTheme *,float const &,zTMus_TransType const &,zTMus_TransSubType const &);
	zCMusicTheme* GetActiveTheme() override { return activeTheme; }

	virtual void PlayJingleByScript(zSTRING const &);
	void StopJingleByScript(zSTRING const &) override {}
	virtual void LoadJingleByScript(zSTRING const &);
	virtual void LoadJingle(zSTRING const &);
	virtual void PlayJingle(zCMusicJingle const *,float const &,zTMus_TransSubType const &);
	void StopJingle(int const &) override {}
	virtual void IsJingleActive(int const &);

	void DoMusicUpdate() override {}

	virtual void Mute();
	virtual void Stop();
	virtual void SetVolume(float);
	float GetVolume() override { return vol }
	virtual void IsAvailable(zSTRING	const &);

private:
	int unk0;
	float vol;
	int what[6];
	zCMusicTheme *activeTheme;
	int unk;
};
