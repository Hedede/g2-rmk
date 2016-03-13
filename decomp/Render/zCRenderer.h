enum zTRnd_ScreenMode
{
	zRND_SCREEN_MODE_FULLSCREEN = 0x0,
	zRND_SCREEN_MODE_WINDOWED = 0x1,
};

enum zTRnd_AlphaBlendFunc   {
	zRND_ALPHA_FUNC_MAT_DEFAULT,
	zRND_ALPHA_FUNC_NONE,
	zRND_ALPHA_FUNC_BLEND,
	zRND_ALPHA_FUNC_ADD,
	zRND_ALPHA_FUNC_SUB,

	zRND_ALPHA_FUNC_MUL,
	zRND_ALPHA_FUNC_MUL2,
	zRND_ALPHA_FUNC_TEST,
	zRND_ALPHA_FUNC_BLEND_TEST
};



class zCRenderer {
public:
	virtual ~zCRenderer();
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void FlushPolys() = 0;
	virtual void DrawPoly(zCPolygon *) = 0;
	virtual void DrawLightmapList(zCPolygon * *,int);
	virtual void DrawLine(float,float,float,float,zCOLOR) = 0;
	virtual void DrawLineZ(float,float,float,float,float,float,zCOLOR);
	virtual void SetPixel(float,float,zCOLOR);
	virtual void DrawPolySimple(zCTexture *,zTRndSimpleVertex *,int);
	virtual void SetFog(int) = 0;
	virtual int GetFog()
	{
		return 0;
	}
	virtual void SetRadialFog(int) = 0;
	virtual int GetRadialFog()
	{
		return 0;
	}
	virtual void SetFogColor(zCOLOR const &) = 0;
	virtual void GetFogColor();
	virtual void SetFogRange(float,float,zTRnd_FogMode) = 0;
	virtual void GetFogRange(float &,float &,zTRnd_FogMode &);
	virtual zTRnd_PolyDrawMode GetPolyDrawMode() const
	{
		return polyDrawMode;
	}
	virtual void SetPolyDrawMode(zTRnd_PolyDrawMode const &);
	virtual void GetSurfaceLost();
	virtual void SetSurfaceLost(int);
	virtual void GetSyncOnAmbientCol();
	virtual void SetSyncOnAmbientCol(int);
	virtual void SetTextureWrapEnabled(int);
	virtual void GetTextureWrapEnabled();
	virtual void SetBilerpFilterEnabled(int) = 0;
	virtual void GetBilerpFilterEnabled() = 0;
	virtual void SetDitherEnabled(int)
	{
	}
	virtual int GetDitherEnabled()
	{
		return 1;
	}
	virtual zTRnd_PolySortMode GetPolySortMode() const
	{
		return polySortMode;
	}
	virtual void SetPolySortMode(zTRnd_PolySortMode const &);
	virtual void GetZBufferWriteEnabled();
	virtual void SetZBufferWriteEnabled(int);
	virtual void SetZBias(int)
	{
	}
	virtual void GetZBias()
	{
		return 0;
	}
	virtual void GetZBufferCompare();
	virtual void SetZBufferCompare(zTRnd_ZBufferCmp const &);
	virtual void GetPixelWriteEnabled();
	virtual void SetPixelWriteEnabled(int);
	virtual void SetAlphaBlendSource(zTRnd_AlphaBlendSource const &)
	{
	}
	virtual zTRnd_AlphaBlendSource GetAlphaBlendSource()
	{
		return 0;
	}
	virtual void SetAlphaBlendFunc(zTRnd_AlphaBlendFunc const &)
	{
	}
	virtual zTRnd_AlphaBlendFunc GetAlphaBlendFunc()
	{
		return 0;
	}
	virtual void GetAlphaBlendFactor();
	virtual void SetAlphaBlendFactor(float const &)
	{
	}
	virtual void SetAlphaReference(ulong)
	{
	}
	virtual int GetAlphaReference()
	{
		return 0;
	}
	virtual void GetCacheAlphaPolys();
	virtual void SetCacheAlphaPolys(int);
	virtual int GetAlphaLimitReached()
	{
		return 0;
	}
	virtual void AddAlphaPoly(zCPolygon const *)
	{
	}
	virtual void FlushAlphaPolys()
	{
	}
	virtual void SetRenderMode(zTRnd_RenderMode)
	{
	}
	virtual zTRnd_RenderMode GetRenderMode()
	{
		return 0;
	}
	virtual void HasCapability(zTRnd_Capability);
	virtual void GetGuardBandBorders(float &,float &,float &,float &)
	{
	}
	virtual void ResetZTest()
	{
	}
	virtual int HasPassedZTest()
	{
		return 1;
	}
	virtual void CreateTexture() = 0;
	virtual zCTextureConvert* CreateTextureConvert()
	{
		return nullptr;
	}
	virtual void GetTotalTextureMem() = 0;
	virtual void SupportsTextureFormat(zTRnd_TextureFormat);
	virtual void SupportsTextureFormatHardware(zTRnd_TextureFormat);
	virtual int GetMaxTextureSize()
	{
		return 256;
	}
	virtual void GetStatistics(zTRnd_Stats &) = 0;
	virtual void ResetStatistics() = 0;
	virtual void Vid_Blit(int,tagRECT *,tagRECT *) = 0;
	virtual void Vid_Clear(zCOLOR &,int) = 0;
	virtual void Vid_Lock(zTRndSurfaceDesc &) = 0;
	virtual void Vid_Unlock() = 0;
	virtual void Vid_IsLocked() = 0;
	virtual void Vid_GetFrontBufferCopy(zCTextureConvert &) = 0;
	virtual void Vid_GetNumDevices() = 0;
	virtual void Vid_GetActiveDeviceNr() = 0;
	virtual void Vid_SetDevice(int) = 0;
	virtual void Vid_GetDeviceInfo(zTRnd_DeviceInfo &,int) = 0;
	virtual void Vid_GetNumModes() = 0;
	virtual void Vid_GetModeInfo(zTRnd_VidModeInfo &,int) = 0;
	virtual void Vid_GetActiveModeNr() = 0;
	virtual void Vid_SetMode(int,HWND__ * *) = 0;
	virtual void Vid_SetScreenMode(zTRnd_ScreenMode) = 0;
	virtual zTRnd_ScreenMode Vid_GetScreenMode() = 0;
	virtual void Vid_SetGammaCorrection(float,float,float) = 0;
	virtual void Vid_GetGammaCorrection() = 0;
	virtual void Vid_BeginLfbAccess() = 0;
	virtual void Vid_EndLfbAccess() = 0;
	virtual void Vid_SetLfbAlpha(int) = 0;
	virtual void Vid_SetLfbAlphaFunc(zTRnd_AlphaBlendFunc const &) = 0;
	virtual void SetTransform(zTRnd_TrafoType,zMAT4 const &);
	virtual void SetViewport(int,int,int,int);
	virtual void SetLight(ulong,zCRenderLight *);
	virtual void GetMaterial(zCRenderer::zTMaterial &);
	virtual void SetMaterial(zCRenderer::zTMaterial const &);
	virtual void SetTexture(ulong,zCTexture *);
	virtual void SetTextureStageState(ulong,zTRnd_TextureStageState,ulong);
	virtual void SetAlphaBlendFuncImmed(zTRnd_AlphaBlendFunc);
	virtual void SetRenderState(zTRnd_RenderStateType,ulong);
	virtual void GetRenderState(zTRnd_RenderStateType);
	virtual void AddAlphaSortObject(zCRndAlphaSortObject *)
	{
	}
	virtual void RenderAlphaSortList()
	{
	}
	virtual void DrawVertexBuffer(zCVertexBuffer *,int,int,ushort *,ulong) = 0;
	virtual void CreateVertexBuffer() = 0;
protected:
	zTRnd_PolySortMode polySortMode;
	zTRnd_PolyDrawMode polyDrawMode;
};
