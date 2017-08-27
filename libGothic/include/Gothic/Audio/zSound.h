#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>

struct zCSoundSystem;
struct zCSoundFX;
struct zCActiveSnd;
struct zTSound3DParams;
struct zCVob;

enum zTSpeakerType {};

struct zCSoundSystem_vt {
	void (__thiscall *dtor)(zCSoundSystem *);
	zCSoundFX *(__thiscall *LoadSoundFX)(zCSoundSystem *, const zSTRING&);
	zCSoundFX *(__thiscall *LoadSoundFXScript)(zCSoundSystem *, const zSTRING&);
	void (__thiscall *GetSFXParser)(zCSoundSystem *);
	float (__thiscall *GetPlayingTimeMSEC)(zCSoundSystem *, const zSTRING *);
	void (__thiscall *SetSound3DDefaultRadius)(zCSoundSystem *, float);
	void (__thiscall *GetSound3DDefaultRadius)(zCSoundSystem *);
	void (__thiscall *SetMasterVolume)(zCSoundSystem *, float);
	void (__thiscall *GetMasterVolume)(zCSoundSystem *);
	void (__thiscall *PlaySound1)(zCSoundSystem *, zCSoundFX *, int, int, float, float);
	void (__thiscall *PlaySound2)(zCSoundSystem *, zCSoundFX *, int);
	void (__thiscall *PlaySound3D1)(zCSoundSystem *, const zSTRING *, zCVob *, int, zTSound3DParams *);
	zCActiveSnd *(__thiscall *PlaySound3D2)(zCSoundSystem *, zCSoundFX *, zCVob *, int, zTSound3DParams *);
	void (__thiscall *StopSound)(zCSoundSystem *, const int *);
	void (__thiscall *StopAllSounds)(zCSoundSystem *);
	int (__thiscall *GetSound3DProps)(zCSoundSystem *, const int *, zTSound3DParams *);
	void (__thiscall *UpdateSound3D)(zCSoundSystem *, const int *, zTSound3DParams *);
	void (__thiscall *GetSoundProps)(zCSoundSystem *, const int *, int *, float *, float *);
	void (__thiscall *UpdateSoundProps)(zCSoundSystem *, const int *, int, float, float);
	int (__thiscall *IsSoundActive)(zCSoundSystem *, const int *);
	void (__thiscall *DoSoundUpdate)(zCSoundSystem *);
	void (__thiscall *SetListener)(zCSoundSystem *, zCVob *);
	void (__thiscall *SetGlobalReverbPreset)(zCSoundSystem *, int, float);
	void (__thiscall *SetReverbEnabled)(zCSoundSystem *, int);
	bool (__thiscall *GetReverbEnabled)(zCSoundSystem *);
	void (__thiscall *GetNumProvider)(zCSoundSystem *);
	void (__thiscall *GetProviderName)(zCSoundSystem *, int);
	void (__thiscall *SetProvider)(zCSoundSystem *, int);
	void (__thiscall *SetSpeakerType)(zCSoundSystem *, zTSpeakerType);
	void (__thiscall *GetSpeakerType)(zCSoundSystem *);
	void (__thiscall *SetGlobalOcclusion)(zCSoundSystem *, float);
	void (__thiscall *GetCPULoad)(zCSoundSystem *);
};


struct zCSoundSystem {
	void SetMasterVolume(float vol)
	{
		vtab->SetMasterVolume(this, vol);
	}

protected:
	zCSoundSystem_vt const* vtab;

private:
	float volume;
};

static auto& zsound = Value<zCSoundSystem*>(0x99B03C);
