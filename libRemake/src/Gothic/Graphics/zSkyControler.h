#ifndef Gothic_zSkyCtl_H
#define Gothic_zSkyCtl_H
#include <Gothic/Types/zVEC3.h>

enum zTWeather : int;
struct zCOLOR;
struct zCSkyControler;
struct zCArchiver;
struct zCSkyCtrl_vt {
	void (__thiscall *_GetClassDef)(zCSkyControler *);
	void (__thiscall *Archive)(zCSkyControler *, zCArchiver *);
	void (__thiscall *Unarchive)(zCSkyControler *, zCArchiver *);
	void (__thiscall *dtor)(zCSkyControler *, unsigned);
	void (__thiscall *SetTime)(zCSkyControler *, float);
	float (__thiscall *GetTime)(zCSkyControler *);
	void (__thiscall *ResetTime)(zCSkyControler *);
	void (__thiscall *SetFarZ)(zCSkyControler *, float);
	void (__thiscall *GetFarZ)(zCSkyControler *);
	void (__thiscall *SetFarZScalability)(zCSkyControler *, float);
	void (__thiscall *GetFarZScalability)(zCSkyControler *);
	void (__thiscall *SetBackgroundColor)(zCSkyControler *, zCOLOR);
	void (__thiscall *GetBackgroundColor)(zCSkyControler *);
	void (__thiscall *GetBackgroundColorDef)(zCSkyControler *);
	void (__thiscall *SetOverrideColor)(zCSkyControler *, zVEC3);
	void (__thiscall *SetOverrideColorFlag)(zCSkyControler *, int);
	void (__thiscall *GetCloudShadowScale)(zCSkyControler *);
	void (__thiscall *SetCloudShadowScale)(zCSkyControler *, float);
	void (__thiscall *SetFillBackground)(zCSkyControler *, int);
	int (__thiscall *GetFillBackground)(zCSkyControler *);
	void (__thiscall *SetUnderwaterFX)(zCSkyControler *, int);
	void (__thiscall *GetUnderwaterFX)(zCSkyControler *);
	void (__thiscall *UpdateWorldDependencies)(zCSkyControler *);
	void (__thiscall *SetLightDirty)(zCSkyControler *);
	void (__thiscall *SetRelightTime)(zCSkyControler *, float);
	void (__thiscall *GetRelightCtr)(zCSkyControler *);
	void (__thiscall *GetDaylightColorFromIntensity)(zCSkyControler *, int);
	void (__thiscall *RenderSkyPre)(zCSkyControler *);
	void (__thiscall *RenderSkyPost)(zCSkyControler *, int);
	void (__thiscall *GetGlobalWindVec)(zCSkyControler *, zVEC3 *, int);
	void (__thiscall *SetGlobalSkyScale)(zCSkyControler *, float);
	void (__thiscall *GetGlobalSkyScale)(zCSkyControler *);
	void (__thiscall *GetGlobalSkyScaleChanged)(zCSkyControler *);
	void (__thiscall *SetCameraLocationHint)(zCSkyControler *, int);
	void (__thiscall *SetWeatherType)(zCSkyControler *, zTWeather);
	void (__thiscall *GetWeatherType)(zCSkyControler *);
	void (__thiscall *GetRenderLightning)(zCSkyControler *);
};

struct zCSkyControler {
	zCSkyCtrl_vt* vtab;
	void SetTime(float time)
	{
		vtab->SetTime(this, time);
	}
};


#endif//Gothic_zSkyCtl_H
