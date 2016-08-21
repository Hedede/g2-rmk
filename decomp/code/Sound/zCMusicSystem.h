class zCMusicSystem {
	using EventCallback = void(zTMus_EventType const&);

	static bool musicSystemDisabled;
	static void DisableMusicSystem(bool b)
	{
		musicSystemDisabled = b;
		if (musicSystemDisabled)
			if (zmusic)
				zmusic->Stop();
	}

	virtual ~zCMusicSystem() = default;

	virtual void PlayThemeByScript(zSTRING const &,int,int *) {}
	virtual bool LoadThemeByScript(zSTRING const&)
	{
		return nullptr;
	}
	virtual bool LoadTheme(zSTRING const&)
	{
		return nullptr;
	}
	virtual void PlayTheme(zCMusicTheme *,float const &,zTMus_TransType const &,zTMus_TransSubType const &) {}
	virtual zCMusicTheme* GetActiveTheme()
	{
		return nullptr;
	}

	virtual bool PlayJingleByScript(zSTRING const &)
	{
		return false;
	}
	virtual void StopJingleByScript(zSTRING const &) {}
	virtual zCMusicJingle* LoadJingleByScript(zSTRING const &)
	{
		return nullptr;
	}
	virtual zCMusicJingle* LoadJingle(zSTRING const &)
	{
		return nullptr;
	}
	virtual bool PlayJingle(zCMusicJingle const *,float const &,zTMus_TransSubType const &)
	{
		return false;
	}
	virtual void StopJingle(int const&) {}
	virtual bool IsJingleActive(int const&)
	{
		return false;
	}

	virtual void AddEventCallBackFunc(zTMus_EventType const&, EventCallback*) {}
	virtual void RemoveEventCallBackFunc(EventCallback*) {}
	virtual void Mute() {}
	virtual void Stop() {}
	virtual void SetVolume(float volume)
	{
		this->volume = volume;
	}
	virtual float GetVolume()
	{
		return volume;
	}
	virtual bool IsAvailable(zSTRING const&)
	{
		return false;
	}

private:
	int volume;
};
