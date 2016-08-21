class zCSndSys_MSS : public zCSoundSystem {
public:
	virtual void ~zCSndSys_MSS();
	virtual void LoadSoundFX(zSTRING const &);
	virtual void LoadSoundFXScript(zSTRING const &);
	virtual void GetSFXParser();
	virtual void GetPlayingTimeMSEC(zSTRING const &);


	virtual void SetMasterVolume(float);
	virtual void GetMasterVolume();
	virtual void PlaySound(zCSoundFX *,int,int,float,float);
	virtual void PlaySound(zCSoundFX *,int);
	virtual void PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams *);
	virtual void PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *);
	virtual void StopSound(int const &);
	virtual void StopAllSounds();
	virtual void GetSound3DProps(int const &,zCSoundSystem::zTSound3DParams &);
	virtual void UpdateSound3D(int const &,zCSoundSystem::zTSound3DParams *);
	virtual void GetSoundProps(int const &,int &,float &,float &);
	virtual void UpdateSoundProps(int const &,int,float,float);
	virtual void IsSoundActive(int const &);
	virtual void DoSoundUpdate();
	virtual void SetListener(zCVob *);
	virtual void SetGlobalReverbPreset(int,float);
	virtual void SetReverbEnabled(int);
	virtual void GetReverbEnabled();
	virtual void GetNumProvider();
	virtual void GetProviderName(int);
	virtual void SetProvider(int);
	virtual void SetSpeakerType(zCSoundSystem::zTSpeakerType);
	virtual void GetSpeakerType();
	virtual void SetGlobalOcclusion(float);
	virtual void GetCPULoad();
};
