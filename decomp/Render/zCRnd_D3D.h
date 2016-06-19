class zCRnd_D3D : public zCRenderer {
public:
	virtual ~zCRnd_D3D();
	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void FlushPolys();
	virtual void DrawPoly(zCPolygon *);
	virtual void DrawLightmapList(zCPolygon * *,int);
	virtual void DrawLine(float,float,float,float,zCOLOR);
	virtual void DrawLineZ(float,float,float,float,float,float,zCOLOR);
	virtual void SetPixel(float,float,zCOLOR);
	virtual void DrawPolySimple(zCTexture *,zTRndSimpleVertex *,int);
	virtual void SetFog(int);
	virtual void GetFog();
	virtual void SetRadialFog(int);
	virtual void GetRadialFog();
	virtual void SetFogColor(zCOLOR const &);
	virtual void GetFogColor();
	virtual void SetFogRange(float,float,zTRnd_FogMode);
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
	virtual void SetBilerpFilterEnabled(int);
	virtual void GetBilerpFilterEnabled();
	virtual void SetDitherEnabled(int);
	virtual void GetDitherEnabled();
	virtual zTRnd_PolySortMode GetPolySortMode() const
	{
		return polySortMode;
	}
	virtual void SetPolySortMode(zTRnd_PolySortMode const &);
	virtual void GetZBufferWriteEnabled();
	virtual void SetZBufferWriteEnabled(int);
	void SetZBias(int) override
	int GetZBias() override
	{
		return zBias;
	}
	virtual void GetZBufferCompare();
	virtual void SetZBufferCompare(zTRnd_ZBufferCmp const &);
	virtual void GetPixelWriteEnabled();
	virtual void SetPixelWriteEnabled(int);
	virtual void SetAlphaBlendSource(zTRnd_AlphaBlendSource const &);
	virtual void GetAlphaBlendSource();
	void SetAlphaBlendFunc(zTRnd_AlphaBlendFunc const& func) override
	{
		alphaBlendFunc = func;
	}
	zTRnd_AlphaBlendFunc GetAlphaBlendFunc() override
	{
		return alphaBlendFunc;
	}
	float GetAlphaBlendFactor() override
	{
		return alphaBlendFactor;
	}
	void SetAlphaBlendFactor(float const& f) override
	{
		alphaBlendFactor = f;
	}
	virtual void SetAlphaReference(ulong);
	virtual void GetAlphaReference();
	virtual void GetCacheAlphaPolys();
	virtual void SetCacheAlphaPolys(int);
	virtual void GetAlphaLimitReached();
	virtual void AddAlphaPoly(zCPolygon const *);
	virtual void FlushAlphaPolys();
	void SetRenderMode(zTRnd_RenderMode);
	zTRnd_RenderMode GetRenderMode() override
	{
		return renderMode;
	}
	virtual void HasCapability(zTRnd_Capability);
	virtual void GetGuardBandBorders(float &,float &,float &,float &);
	void ResetZTest() override {}
	bool HasPassedZTest() override
	{
		return false;
	}
	virtual void CreateTexture();
	virtual void CreateTextureConvert();
	int GetTotalTextureMem() override
	{
		return totalTextureMem;
	}
	virtual void SupportsTextureFormat(zTRnd_TextureFormat);
	virtual void SupportsTextureFormatHardware(zTRnd_TextureFormat);
	int GetMaxTextureSize() override
	{
		return maxTexSize;
	}
	virtual void GetStatistics(zTRnd_Stats &);
	virtual void ResetStatistics();
	virtual void Vid_Blit(int,tagRECT *,tagRECT *);
	void Vid_Clear(zCOLOR& color, int clearTarget) override;
	virtual void Vid_Lock(zTRndSurfaceDesc &);
	virtual void Vid_Unlock();
	virtual void Vid_IsLocked();
	virtual void Vid_GetFrontBufferCopy(zCTextureConvert &);
	int Vid_GetNumDevices() override
	{
		return numDevices;
	}
	int Vid_GetActiveDeviceNr() override
	{
		return deviceNr;
	}
	virtual void Vid_SetDevice(int);
	virtual void Vid_GetDeviceInfo(zTRnd_DeviceInfo &,int);
	virtual void Vid_GetNumModes();
	virtual void Vid_GetModeInfo(zTRnd_VidModeInfo &,int);
	int Vid_GetActiveModeNr() override
	{
		return modeNr;
	}
	virtual void Vid_SetMode(int,HWND__ * *);
	virtual void Vid_SetScreenMode(zTRnd_ScreenMode);
	zTRnd_ScreenMode Vid_GetScreenMode() override
	{
		return screenMode;
	}
	virtual void Vid_SetGammaCorrection(float,float,float);
	float Vid_GetGammaCorrection() override
	{
		return gammaCorrection;
	}
	virtual void Vid_BeginLfbAccess();
	virtual void Vid_EndLfbAccess() {}
	void Vid_SetLfbAlpha(int) override {}
	void Vid_SetLfbAlphaFunc(zTRnd_AlphaBlendFunc const &) override {}

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
	virtual void AddAlphaSortObject(zCRndAlphaSortObject *);
	virtual void RenderAlphaSortList();
	virtual void DrawVertexBuffer(zCVertexBuffer *,int,int,ushort *,ulong);
	virtual void CreateVertexBuffer();
};

void zCRnd_D3D::Vid_Clear(zCOLOR& color, int clearTarget)
{
	int res;
	zClamp(clearTarget,1,3);
	if ( zCRnd_D3D::xd3d_pd3dx ) {
		res = zCRnd_D3D::xd3d_pd3dx->SetClearDepth(1.0);
		DXTryWarning(res, "X: [RND3D]Vid_Clear: Can't set clear depth !");
		res = zCRnd_D3D::xd3d_pd3dx->SetClearColor(&color);
		DXTryWarning(res, "X: [RND3D]Vid_Clear: Can't set clear color !");
		res = zCRnd_D3D::xd3d_pd3dx->Clear(clearTarget);
		DXTryWarning(res, "X: [RND3D]Vid_Clear: Can't clear render target !");
	}
}
