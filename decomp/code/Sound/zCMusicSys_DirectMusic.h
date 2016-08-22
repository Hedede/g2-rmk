class zCMusicSys_DirectMusic : zCMusicSystem {
	virtual ~zCMusicSys_DirectMusic();

	virtual void PlayThemeByScript(zSTRING	const &,int,int	*);
	virtual void LoadThemeByScript(zSTRING const &);
	virtual void LoadTheme(zSTRING const &);
	virtual void PlayTheme(zCMusicTheme *,float const &,zTMus_TransType const &,zTMus_TransSubType const &);
	virtual void GetActiveTheme();
	virtual void PlayJingleByScript(zSTRING const &);
	virtual void StopJingleByScript(zSTRING const &);
	virtual void LoadJingleByScript(zSTRING const &);
	virtual void LoadJingle(zSTRING const &);
	virtual void PlayJingle(zCMusicJingle const *,float const &,zTMus_TransSubType const &);
	virtual void StopJingle(int const &);
	virtual void IsJingleActive(int const &);

	void DoMusicUpdate() override {}

	virtual void Mute();
	virtual void Stop();
	virtual void SetVolume(float);
	virtual void GetVolume();
	virtual void IsAvailable(zSTRING	const &);
};
