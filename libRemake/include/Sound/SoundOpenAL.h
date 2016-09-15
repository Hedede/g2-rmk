#ifndef G2Remake_SoundOpenAL_H
#define G2Remake_SoundOpenAL_H
#include <string>
#include <Logging/Log.h>
//#include <Gothic/Audio/zSound.h>
#include <Gothic/Audio/zSoundSystemDummy.h>


namespace g2 {


struct SoundOpenAL : zCSoundSystemDummy {
	struct TypeInfo {
		TypeInfo(zCSoundSystem_vt const& proto)
			: vt{proto}
		{
			vt.LoadSoundFX =
			[] (zCSoundSystem* ss, zSTRING const& name) __thiscall -> zCSoundFX*
			{
				auto _this = static_cast<SoundOpenAL*>(ss);
				return _this->LoadSoundFX(std::string(name));
			};

			vt.LoadSoundFXScript =
			[] (zCSoundSystem* ss, zSTRING const& name) __thiscall -> zCSoundFX*
			{
				auto _this = static_cast<SoundOpenAL*>(ss);
				return _this->LoadSoundFXScript(std::string(name));
			};
		}

		void* rtti = reinterpret_cast<void*>(0x846CF8);
		zCSoundSystem_vt vt;
	};

	SoundOpenAL() : zCSoundSystemDummy()
	{
		static const TypeInfo tinfo{*vtab};
		vtab = &tinfo.vt;
	}

	~SoundOpenAL();

	zCSoundFX* LoadSoundFX(std::string const& name)
	{
		Log("SFX", "LoadSoundFX requst: " + name);
		return nullptr;
	}

	zCSoundFX* LoadSoundFXScript(std::string const& name)
	{
		Log("SFX", "LoadSoundFXScript request: " + name);
		return nullptr;
	}

	zCParser* GetSFXParser()
	{
		return nullptr;
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
	void PlaySound(zCSoundFX *,int) {}
	void PlaySound3D(zSTRING const &,zCVob *,int,zTSound3DParams *) {}
	void PlaySound3D(zCSoundFX *,zCVob *,int,zTSound3DParams *) {}
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
};

}

#endif//G2Remake_SoundOpenAL_H
