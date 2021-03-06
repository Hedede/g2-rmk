#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/Base.h>
#include <Gothic/Types/zSTRING.h>

struct zCMusicSystem;
struct zCMusicTheme;
struct zCMusicJingle;
struct zCMusicSystem_vt {
	void (__thiscall *dtor)(zCMusicSystem *, size_t);
	void (__thiscall *PlayThemeByScript)(zCMusicSystem *, const zSTRING&, int, int *);
	void (__thiscall *LoadThemeByScript)(zCMusicSystem *, const zSTRING&);
	void (__thiscall *LoadTheme)(zCMusicSystem *, const zSTRING&);
	void (__thiscall *PlayTheme)(zCMusicSystem *, zCMusicTheme*, const float *, const int *, const int *);
	void (__thiscall *GetActiveTheme)(zCMusicSystem *);
	void (__thiscall *PlayJingleByScript)(zCMusicSystem *, const zSTRING&);
	void (__thiscall *StopJingleByScript)(zCMusicSystem *, const zSTRING&);
	void (__thiscall *LoadJingleByScript)(zCMusicSystem *, const zSTRING&);
	void (__thiscall *LoadJingle)(zCMusicSystem *, const zSTRING *);
	void (__thiscall *PlayJingle)(zCMusicSystem *, const zCMusicJingle *, const float *, const int *);
	void (__thiscall *StopJingle)(zCMusicSystem *, const int *);
	void (__thiscall *IsJingleActive)(zCMusicSystem *, const int *);
	void *pure;
	void (__thiscall *AddEventCallBackFunc)(zCMusicSystem *, const int *, void (__cdecl *)(const int *));
	void (__thiscall *RemoveEventCallBackFunc)(zCMusicSystem *, void (__cdecl *)(const int *));
	void (__thiscall *Mute)(zCMusicSystem *);
	void (__thiscall *Stop)(zCMusicSystem *);
	void (__thiscall *SetVolume)(zCMusicSystem *, float);
	void (__thiscall *GetVolume)(zCMusicSystem *);
	void (__thiscall *IsAvailable)(zCMusicSystem *, const zSTRING *);
};

auto& zmusic = Value<zCMusicSystem*>(0x8D1F14);

struct zCMusicSystem {
protected:
	static int& GetDisabled()
	{
		static int& s_musicSystemDisabled = Value<int>(0x8D1F18);
		return s_musicSystemDisabled;
	}
public:
	static void DisableMusicSystem(int b);

	void SetVolume(float vol)
	{
		vtab->SetVolume(this, vol);
	}

	void Stop()
	{
		vtab->Stop(this);
	}

protected:
	zCMusicSystem_vt* vtab;

private:
	float volume;
};

void zCMusicSystem::DisableMusicSystem(int b)
{
	GetDisabled() = b;
	if ( b ) {
		if ( zmusic )
			zmusic->Stop();
	}
}
