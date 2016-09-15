#pragma once
#include <Gothic/Types/zCOLOR.h>
struct zCRenderer;
struct zCPolygon;
struct zCTexture;
struct zCTextureConvert;
struct zCRenderLight;
struct zCRndAlphaSortObject;
struct zTRnd_Stats;
struct zTRnd_VidModeInfo;
enum zTRnd_ScreenMode : int;
struct zCVertexBuffer;
struct tagRECT;
struct zCRenderer_vt {
	void (__thiscall *dtor)(zCRenderer *, size_t);
	void (__thiscall *BeginFrame)(zCRenderer *);
	void (__thiscall *EndFrame)(zCRenderer *);
	void (__thiscall *FlushPolys)(zCRenderer *);
	void (__thiscall *DrawPoly)(zCRenderer *, zCPolygon *);
	void (__thiscall *DrawLightmapList)(zCRenderer *, zCPolygon **, int);
	void (__thiscall *DrawLine)(zCRenderer *, float, float, float, float, zCOLOR);
	void (__thiscall *DrawLineZ)(zCRenderer *, float, float, float, float, float, float, zCOLOR);
	void (__thiscall *SetPixel)(zCRenderer *, float, float, zCOLOR);
	void (__thiscall *DrawPolySimple)(zCRenderer *, zCTexture *, int *, int);
	void (__thiscall *SetFog)(zCRenderer *, int);
	int (__thiscall *GetFog)(zCRenderer *);
	void (__thiscall *SetRadialFog)(zCRenderer *, int);
	void (__thiscall *GetRadialFog)(zCRenderer *);
	void (__thiscall *SetFogColor)(zCRenderer *, const zCOLOR *);
	void (__thiscall *GetFogColor)(zCRenderer *);
	void (__thiscall *SetFogRange)(zCRenderer *, float, float, int);
	float (__thiscall *GetFogRange)(zCRenderer *, float *, float *, int);
	void (__thiscall *GetPolyDrawMode)(zCRenderer *);
	void (__thiscall *SetPolyDrawMode)(zCRenderer *, int);
	void (__thiscall *GetSurfaceLost)(zCRenderer *);
	void (__thiscall *SetSurfaceLost)(zCRenderer *, int);
	void (__thiscall *GetSyncOnAmbientCol)(zCRenderer *);
	void (__thiscall *SetSyncOnAmbientCol)(zCRenderer *, int);
	void (__thiscall *SetTextureWrapEnabled)(zCRenderer *, int);
	void (__thiscall *GetTextureWrapEnabled)(zCRenderer *);
	void (__thiscall *SetBilerpFilterEnabled)(zCRenderer *, int);
	void (__thiscall *GetBilerpFilterEnabled)(zCRenderer *);
	void (__thiscall *SetDitherEnabled)(zCRenderer *, int);
	void (__thiscall *GetDitherEnabled)(zCRenderer *);
	void (__thiscall *GetPolySortMode)(zCRenderer *);
	void (__thiscall *SetPolySortMode)(zCRenderer *, int);
	void (__thiscall *GetZBufferWriteEnabled)(zCRenderer *);
	void (__thiscall *SetZBufferWriteEnabled)(zCRenderer *, int);
	void (__thiscall *SetZBias)(zCRenderer *, int);
	void (__thiscall *GetZBias)(zCRenderer *);
	void *(__thiscall *GetZBufferCompare)(zCRenderer *);
	void (__thiscall *SetZBufferCompare)(zCRenderer *, int);
	void (__thiscall *GetPixelWriteEnabled)(zCRenderer *);
	void (__thiscall *SetPixelWriteEnabled)(zCRenderer *, int);
	void (__thiscall *SetAlphaBlendSource)(zCRenderer *, int);
	int (__thiscall *GetAlphaBlendSource)(zCRenderer *);
	void (__thiscall *SetAlphaBlendFunc)(zCRenderer *, int);
	int (__thiscall *GetAlphaBlendFunc)(zCRenderer *);
	float (__thiscall *GetAlphaBlendFactor)(zCRenderer *);
	void (__thiscall *SetAlphaBlendFactor)(zCRenderer *, const float *);
	void (__thiscall *SetAlphaReference)(zCRenderer *, unsigned);
	void (__thiscall *GetAlphaReference)(zCRenderer *);
	void (__thiscall *GetCacheAlphaPolys)(zCRenderer *);
	void (__thiscall *SetCacheAlphaPolys)(zCRenderer *, int);
	void (__thiscall *GetAlphaLimitReached)(zCRenderer *);
	void (__thiscall *AddAlphaPoly)(zCRenderer *, const zCPolygon *);
	void (__thiscall *FlushAlphaPolys)(zCRenderer *);
	void (__thiscall *SetRenderMode)(zCRenderer *, int);
	void (__thiscall *GetRenderMode)(zCRenderer *);
	int (__thiscall *HasCapability)(zCRenderer *, int);
	void (__thiscall *GetGuardBandBorders)(zCRenderer *, float *, float *, float *, float *);
	void (__thiscall *ResetZTest)(zCRenderer *);
	void (__thiscall *HasPassedZTest)(zCRenderer *);
	zCTexture *(__thiscall *CreateTexture)(zCRenderer *);
	zCTextureConvert *(__thiscall *CreateTextureConvert)(zCRenderer *);
	int (__thiscall *GetTotalTextureMem)(zCRenderer *);
	void (__thiscall *SupportsTextureFormat)(zCRenderer *, int);
	void (__thiscall *SupportsTextureFormatHardware)(zCRenderer *, int);
	void (__thiscall *GetMaxTextureSize)(zCRenderer *);
	void (__thiscall *GetStatistics)(zCRenderer *, zTRnd_Stats *);
	void (__thiscall *ResetStatistics)(zCRenderer *);
	void (__thiscall *Vid_Blit)(zCRenderer *, int, tagRECT *, tagRECT *);
	void (__thiscall *Vid_Clear)(zCRenderer *, zCOLOR *, int);
	int (__thiscall *Vid_Lock)(zCRenderer *, int);
	void (__thiscall *Vid_Unlock)(zCRenderer *);
	void (__thiscall *Vid_IsLocked)(zCRenderer *);
	void (__thiscall *Vid_GetFrontBufferCopy)(zCRenderer *, zCTextureConvert *);
	void (__thiscall *Vid_GetNumDevices)(zCRenderer *);
	int (__thiscall *Vid_GetActiveDeviceNr)(zCRenderer *);
	void (__thiscall *Vid_SetDevice)(zCRenderer *, int);
	void (__thiscall *Vid_GetDeviceInfo)(zCRenderer *, int);
	int (__thiscall *Vid_GetNumModes)(zCRenderer *);
	int (__thiscall *Vid_GetModeInfo)(zCRenderer *, zTRnd_VidModeInfo *, int);
	int (__thiscall *Vid_GetActiveModeNr)(zCRenderer *);
	void (__thiscall *Vid_SetMode)(zCRenderer *, int, void*&);
	void (__thiscall *Vid_SetScreenMode)(zCRenderer *, zTRnd_ScreenMode);
	zTRnd_ScreenMode (__thiscall *Vid_GetScreenMode)(zCRenderer *);
	void (__thiscall *Vid_SetGammaCorrection)(zCRenderer *, float, float, float);
	float (__thiscall *Vid_GetGammaCorrection)(zCRenderer *);
	void (__thiscall *Vid_BeginLfbAccess)(zCRenderer *);
	void (__thiscall *Vid_EndLfbAccess)(zCRenderer *);
	void (__thiscall *Vid_SetLfbAlpha)(zCRenderer *, int);
	void (__thiscall *Vid_SetLfbAlphaFunc)(zCRenderer *, int);
	void (__thiscall *SetTransform)(zCRenderer *, int);
	void (__thiscall *SetViewport)(zCRenderer *, int, int, int, int);
	void (__thiscall *SetLight)(zCRenderer *, unsigned, zCRenderLight *);
	void (__thiscall *GetMaterial)(zCRenderer *, int *);
	void (__thiscall *SetMaterial)(zCRenderer *, const int *);
	void (__thiscall *SetTexture)(zCRenderer *, unsigned, zCTexture *);
	void (__thiscall *SetTextureStageState)(zCRenderer *, unsigned, int);
	void (__thiscall *SetAlphaBlendFuncImmed)(zCRenderer *, int);
	void (__thiscall *SetRenderState)(zCRenderer *, int);
	void (__thiscall *GetRenderState)(zCRenderer *, int);
	void (__thiscall *AddAlphaSortObject)(zCRenderer *, zCRndAlphaSortObject *);
	void (__thiscall *RenderAlphaSortList)(zCRenderer *);
	void (__thiscall *DrawVertexBuffer)(zCRenderer *, zCVertexBuffer *, int, int, unsigned short *, unsigned);
	void (__thiscall *CreateVertexBuffer)(zCRenderer *);
};


struct zCRenderer {
	void Vid_SetDevice(int nr)
	{
		vtab->Vid_SetDevice(this, nr);
	}

	void Vid_SetScreenMode(int mode)
	{
		vtab->Vid_SetScreenMode(this, zTRnd_ScreenMode(mode));
	}

	int Vid_SetMode(int newXDim, int newYDim, int newBpp, void *&hwnd)
	{
		Thiscall<void(zCRenderer*,int,int,int,void*&)> func{0x5D3C20};
		func(this,newXDim, newYDim, newBpp, hwnd);
	}

	void Vid_SetMode(int mode, void*& hwnd)
	{
		vtab->Vid_SetMode(this, mode, hwnd);
	}

	void Vid_Clear(zCOLOR color, int v)
	{
		vtab->Vid_Clear(this, &color, v);
	}
	
	void Vid_SetGammaCorrection(float g, float c, float b)
	{
		vtab->Vid_SetGammaCorrection(this, g, c, b);
	}

	zCRenderer_vt *vtab;
	int polySortMode;
	int polyDrawMode;
};

zCRenderer*& zrenderer = Value<zCRenderer*>(0x982F08);
