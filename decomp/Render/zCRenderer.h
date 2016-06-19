enum zTRnd_ScreenMode {
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

struct zTRnd_VidModeInfo {
	int xdim;
	int ydim;
	int bpp;
};

struct zCRenderer {
	static zTRnd_TexFormatInfo* GettexFormatInfo(zTRnd_TexFormat idx)
	{
		return texFormatInfoList[idx]; // 0x24
	}
	virtual ~zCRenderer();
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void FlushPolys() = 0;
	virtual void DrawPoly(zCPolygon *) = 0;
	virtual void DrawLightmapList(zCPolygon**, int) { }
	virtual void DrawLine(float,float,float,float,zCOLOR) = 0;
	virtual void DrawLineZ(float,float,float,float,float,float,zCOLOR) { }
	virtual void SetPixel(float,float,zCOLOR);
	virtual void DrawPolySimple(zCTexture *,zTRndSimpleVertex *,int) { }

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
	virtual void GetFogColor()
	{
		return {0,0,0,-1}; // RGBA, not real ctor
	}
	virtual void SetFogRange(float,float,zTRnd_FogMode) = 0;
	virtual void GetFogRange(float &,float &,zTRnd_FogMode &) { }

	virtual zTRnd_PolyDrawMode GetPolyDrawMode() const
	{
		return polyDrawMode;
	}
	virtual void SetPolyDrawMode(zTRnd_PolyDrawMode const& val)
	{
		polyDrawMode = val;
	}

	virtual int GetSurfaceLost()
	{
		return 0;
	}
	virtual void SetSurfaceLost(int) { }

	virtual int GetSyncOnAmbientCol()
	{
		return 0;
	}
	virtual void SetSyncOnAmbientCol(int) { }
	virtual void SetTextureWrapEnabled(int) { }
	virtual void GetTextureWrapEnabled()
	{
		return true;
	}
	virtual void SetBilerpFilterEnabled(int) = 0;
	virtual void GetBilerpFilterEnabled() = 0;
	virtual void SetDitherEnabled(int) { }

	virtual int GetDitherEnabled()
	{
		return 1;
	}

	virtual zTRnd_PolySortMode GetPolySortMode() const
	{
		return polySortMode;
	}
	virtual void SetPolySortMode(zTRnd_PolySortMode const& val)
	{
		polySortMode = val;
	}

	virtual void GetZBufferWriteEnabled()
	{
		return true;
	}
	virtual void SetZBufferWriteEnabled(int) { }
	virtual void SetZBias(int) { }
	virtual void GetZBias()
	{
		return 0;
	}
	virtual void GetZBufferCompare()
	{
		return 2;
	}
	virtual void SetZBufferCompare(zTRnd_ZBufferCmp const &) { }

	virtual void GetPixelWriteEnabled()
	{
		return true;
	}
	virtual void SetPixelWriteEnabled(int) { }

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
	virtual float GetAlphaBlendFactor()
	{
		return 1.0;
	}
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
	virtual void GetCacheAlphaPolys()
	{
		return true;
	}
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
	virtual void HasCapability(zTRnd_Capability)
	{
		return false;
	}
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
	virtual void SupportsTextureFormat(zTRnd_TextureFormat)
	{
		return false;
	}
	virtual void SupportsTextureFormatHardware(zTRnd_TextureFormat)
	{
		return false;
	}
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

	virtual void SetTransform(zTRnd_TrafoType,zMAT4 const &)
	{
		return 0;
	}
	virtual void SetViewport(int,int,int,int)
	{
		return 0;
	}
	virtual void SetLight(ulong,zCRenderLight *)
	{
		return 0;
	}
	virtual void GetMaterial(zCRenderer::zTMaterial &)
	{
		return 0;
	}
	virtual void SetMaterial(zCRenderer::zTMaterial const &)
	{
		return 0;
	}
	virtual int SetTexture(ulong,zCTexture *)
	{
		return 0;
	}
	virtual int SetTextureStageState(ulong,zTRnd_TextureStageState,ulong)
	{
		return 0;
	}
	virtual int SetAlphaBlendFuncImmed(zTRnd_AlphaBlendFunc)
	{
		return 0;
	}

	virtual void SetRenderState(zTRnd_RenderStateType,ulong) { }
	virtual void GetRenderState(zTRnd_RenderStateType)
	{
		return 0;
	}

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

int zCRenderer::Vid_SetMode(int newXDim, int newYDim, int newBpp, HWND *initContextHandle)
{
	int modeNr = -1;
	int mindiff = 99999999;

	if (Vid_GetNumModes() <= 0)
		return 0;

	for (int i = 0; i < Vid_GetNumModes(); ++i) {
		zTRnd_VidModeInfo info;
		Vid_GetModeInfo(info, i);

		if ( info.bpp == newBpp ) {
			int xdiff = info.xdim - newXDim;
			int ydiff = info.ydim - newYDim;
			if (ydiff*ydiff + xdim*xdim < mindiff) {
				modeNr = i;
				mindiff = ydiff*ydiff + xdim*xdim;
			}
		}
	}

	if ( modeNr >= 0 ) {
		Vid_SetMode(modeNr, initContextHandle);
		return 1;
	}

	return 0;
}

zTRnd_AlphaBlendFunc zCRenderer::AlphaBlendFuncStringToType(zSTRING& name)
{
	if ( name.Search(0, "MAT_DEFAULT", 1u) > 0 )
		return zRND_ALPHA_FUNC_MAT_DEFAULT;
	if ( name.Search(0, "NONE", 1u) > 0 )
		return zRND_ALPHA_FUNC_NONE;
	if ( name.Search(0, "BLEND", 1u) > 0 )
		return zRND_ALPHA_FUNC_BLEND;
	if ( name.Search(0, "ADD", 1u) > 0 )
		return zRND_ALPHA_FUNC_ADD;
	if ( name.Search(0, "SUB", 1u) > 0 )
		return zRND_ALPHA_FUNC_SUB;
	if ( name.Search(0, "MUL", 1u) > 0 )
		return zRND_ALPHA_FUNC_MUL;
	if ( name.Search(0, "MUL2", 1u) > 0 )
		return zRND_ALPHA_FUNC_MUL2;
	return zRND_ALPHA_FUNC_NONE;
}
