class zCSoundSystem {
	static zSTRING reverbPresetName[26];
public:
	static zSTRING const& GetReverbPresetName(int idx)
	{
		return reverbPresetName[idx];
	}

	static int GetNumReverbPresets()
	{
		return 26;
	}

	enum zTSpeakerType {
		ST_2_SPEAKERS,
		ST_HEADPHONES,
		ST_SURROUND,
		ST_4_SPEAKERS,
		ST_5_1_SPEAKERS,
		ST_7_1_SPEAKERS
	};

	struct zTSound3DParams {
		void SetDefaults()
		{
			__obstructon = 0.0;
			volume = 1.0;
			radius = -1.0;
			__isLooping = 0;
			coneAngle = 1.0;
			reverbLevel = 0;
			unk2 = 0;
			pitchOf = -999999.0;
		}

		float __obstructon;
		float volume; // volScale
		float radius;
		int __isLooping;
		float coneAngle;
		float reverbLevel;
		float unk2;
		float pitchOff;
	};

public:
	virtual ~zCSoundSystem();
	virtual zCSoundFX* LoadSoundFX(zSTRING const &)
	{
		return nullptr;
	}
	virtual zCSoundFX* LoadSoundFXScript(zSTRING const &)
	{
		return nullptr;
	}
	virtual zCParser* GetSFXParser()
	{
		return nullptr;
	}
	virtual float GetPlayingTimeMSEC(zSTRING const &)
	{
		return 0.0;
	}

	virtual void SetSound3DDefaultRadius(float radius)
	{
		snd3D_defaultRadius = radius;
	}
	virtual float GetSound3DDefaultRadius()
	{
		return snd3D_defaultRadius;
	}

	virtual void SetMasterVolume(float) {}
	virtual float GetMasterVolume()
	{
		return 1.0;
	}

	virtual void PlaySound(zCSoundFX *,int,int,float,float) = 0;
	virtual void PlaySound(zCSoundFX *,int) = 0;
	virtual void PlaySound3D(zSTRING const &,zCVob *,int,zTSound3DParams *) = 0;
	virtual void PlaySound3D(zCSoundFX *,zCVob *,int,zTSound3DParams *) = 0;
	virtual void StopSound(int const &) {}
	virtual void StopAllSounds() {}

	virtual void GetSound3DProps(int const &,zTSound3DParams &) = 0;
	virtual void UpdateSound3D(int const &,zTSound3DParams *) = 0;
	virtual void GetSoundProps(int const &,int &,float &,float &) {}
	virtual void UpdateSoundProps(int const &,int,float,float) {}
	virtual bool IsSoundActive(int const &)
	{
		return false;
	}
	virtual void SetListener(zCVob *) {}
	virtual void SetGlobalReverbPreset(int,float) { }
	virtual void SetReverbEnabled(int) {}
	virtual bool GetReverbEnabled()
	{
		return true;
	}
	virtual int GetNumProvider()
	{
		return 1;
	}
	virtual zSTRING* GetProviderName(int)
	{
		return nullptr;
	}
	virtual int SetProvider(int)
	{
		return 1;
	}
	virtual void SetSpeakerType(zCSoundSystem::zTSpeakerType) {}
	virtual zTSpeakerType GetSpeakerType()
	{
		return 0;
	}
	virtual void SetGlobalOcclusion(float) = 0;
	virtual float GetCPULoad()
	{
		return -1.0;
	}

private:
	float snd3D_defaultRadius;
};

zSTRING zCSoundSystem::reverbPresetName[26] = {
	"GENERIC", "PADDEDCELL", "ROOM", "BATHROOM",
	"LIVINGROOM", "STONEROOM", "AUDITORIUM", "CONCERTHALL",
	"CAVE", "ARENA", "HANGAR", "CARPETEDHALLWAY",
	"HALLWAY", "STONECORRIDOR", "ALLEY", "FOREST",
	"CITY", "MOUNTAINS", "QUARRY", "PLAIN",
	"PARKINGLOT", "SEWERPIPE", "UNDERWATER", "DRUGGED",
	"DIZZY", "PSYCHOTIC"
};
