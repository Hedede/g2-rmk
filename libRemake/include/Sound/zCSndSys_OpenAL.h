#pragma once
#include <string>
#include <memory>

//#include <Gothic/Audio/zSound.h>
#include <Gothic/Audio/zSoundSystemDummy.h>

namespace g2 {
struct SoundOpenAL;
} // namespace
struct zCParser;
struct zCSndSys_OpenAL : zCSoundSystemDummy {
	zCSndSys_OpenAL();
	~zCSndSys_OpenAL();

	zCSoundFX* LoadSoundFX(std::string name);
	zCSoundFX* LoadSoundFXScript(std::string name);

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
	zTSndHandle PlaySound(zCSoundFX& sfx, int slot);

	void PlaySound3D(zSTRING const &,zCVob *,int,zTSound3DParams *) {}
	zTSndHandle PlaySound3D(zCSoundFX* sfx, zCVob *,int,zTSound3DParams *);

	void StopSound(zTSndHandle handle);

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

protected:
	static constexpr size_t impl_size  = 0x40;
	static constexpr auto   impl_align = alignof(void*);

	g2::SoundOpenAL& impl()
	{
		return Value<g2::SoundOpenAL>(&data);
	}

	g2::SoundOpenAL const& impl() const
	{
		return Value<g2::SoundOpenAL>(&data);
	}

private:
	float snd3D_defaultRadius;

	std::unique_ptr<zCParser> sfx_parser;

	using storage = std::aligned_storage< impl_size >::type;
	storage data;
};
