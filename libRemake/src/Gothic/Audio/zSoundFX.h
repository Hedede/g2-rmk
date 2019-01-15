#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/System/zResource.h>

struct zCSoundSystem;
struct zCArchiver;
struct zCClassDef;
struct zCSoundFX;
struct zCSoundFX_vt {
	zCClassDef *(__thiscall *_GetClassDef)(zCSoundFX *);
	void (__thiscall *Archive)(zCSoundFX *, zCArchiver *);
	void (__thiscall *Unarchive)(zCSoundFX *, zCArchiver *);
	void (__thiscall *dtor)(zCSoundFX *, unsigned);
	void (__thiscall *LoadResourceData)(zCSoundFX *);
	void (__thiscall *ReleaseResourceData)(zCSoundFX *);
	int (__thiscall *GetResSizeBytes)(zCSoundFX *);
	void (__thiscall *GetCacheConfig)(zCSoundFX *, unsigned *, unsigned *);
	int (__thiscall *GetNumChannels)(zCSoundFX *);
	int (__thiscall *GetNumChannelFrames)(zCSoundFX *, int);
	void (__thiscall *GetChannelFrame)(zCSoundFX *, int, int);
	void (__thiscall *SetChannelFrame)(zCSoundFX *, int, int);
	void (__thiscall *GetPlayingTimeMSEC)(zCSoundFX *);
	void (__thiscall *GetPan)(zCSoundFX *);
	void (__thiscall *SetPan)(zCSoundFX *, float);
	void (__thiscall *GetVolume)(zCSoundFX *);
	void (__thiscall *SetVolume)(zCSoundFX *, float);
	void (__thiscall *GetFrequency)(zCSoundFX *);
	void (__thiscall *SetFrequency)(zCSoundFX *, int);
	void (__thiscall *SetLooping)(zCSoundFX *, int);
	void (__thiscall *IsLooping)(zCSoundFX *);
	void (__thiscall *SetIsFixed)(zCSoundFX *, int);
	int (__thiscall *GetIsFixed)(zCSoundFX *);
};


struct zCSoundFX : zCResource {
	zCSoundFX()
	{
		Thiscall<void(zCSoundFX*)> ctor{0x4EDF50};
		ctor(this);
	}
	
protected:
	zCSoundFX_vt* vtab()
	{
		return static_cast<zCSoundFX_vt*>(_vtab);
	}

};
