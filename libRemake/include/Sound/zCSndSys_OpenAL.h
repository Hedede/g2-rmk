#pragma once
struct zCParser;
struct zCSndSys_OpenAL : zCSoundSystemDummy {
	zCSndSys_OpenAL();

	zCSoundFX* LoadSoundFX(std::string name);

	zCSoundFX* LoadSoundFXScript(std::string const& name)
	{
		g2::Log("SFX", "LoadSoundFXScript request: " + name);
		return nullptr;
	}

	zCParser& GetSFXParser()
	{
		return *sfx_parser;
	}

	float GetPlayingTimeMSEC(zSTRING const &)
	{
		return 0.0;
	}

	void SetSound3DDefaultRadius(float radius)
	{
		snd3D_defaultRadius = radius;
	}
	float GetSound3DDefaultRadius()
	{
		return snd3D_defaultRadius;
	}

	void SetMasterVolume(float) {}
	float GetMasterVolume()
	{
		return 1.0;
	}

	void PlaySound(zCSoundFX *,int,int,float,float) {}
	void PlaySound(zCSoundFX& sfx, int slot);

	void PlaySound3D(zSTRING const &,zCVob *,int,zTSound3DParams *) {}
	void PlaySound3D(zCSoundFX *,zCVob *,int,zTSound3DParams *);

	void StopSound(int const &) {}
	void StopAllSounds() {}

	void GetSound3DProps(int const &,zTSound3DParams &) {}
	void UpdateSound3D(int const &,zTSound3DParams *) {}
	void GetSoundProps(int const &,int &,float &,float &) {}
	void UpdateSoundProps(int const &,int,float,float) {}
	bool IsSoundActive(int const &)
	{
		return false;
	}
	void SetListener(zCVob *) {}
	void SetGlobalReverbPreset(int,float) { }
	void SetReverbEnabled(int) {}
	bool GetReverbEnabled()
	{
		return true;
	}
	int GetNumProvider()
	{
		return 1;
	}
	zSTRING* GetProviderName(int)
	{
		return nullptr;
	}
	int SetProvider(int)
	{
		return 1;
	}
	void SetSpeakerType(zTSpeakerType) {}
	zTSpeakerType GetSpeakerType()
	{
		return {};
	}
	void SetGlobalOcclusion(float) {}
	float GetCPULoad()
	{
		return -1.0;
	}

private:
	float snd3D_defaultRadius;
	g2::SoundOpenAL impl;

	std::unique_ptr<zCParser> sfx_parser;
};
