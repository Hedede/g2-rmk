class zCRnd_D3D : public zCRenderer {
public:
	~zCRnd_D3D() override;
	virtual void BeginFrame();
	virtual void EndFrame();

	void FlushPolys() override
	{
		RenderAlphaSortList();
	}

	virtual void DrawPoly(zCPolygon *);
	virtual void DrawLightmapList(zCPolygon * *,int);
	virtual void DrawLine(float,float,float,float,zCOLOR);
	virtual void DrawLineZ(float,float,float,float,float,float,zCOLOR);
	virtual void SetPixel(float,float,zCOLOR);
	virtual void DrawPolySimple(zCTexture *,zTRndSimpleVertex *,int);

	void SetFog(int val) override
	{
		fogEnabled = val;
	}
	int GetFog() override
	{
		return fogEnabled;
	}

	void SetRadialFog(int val) override;
	int GetRadialFog() override
	{
		return fogRadial && GetFog();
	}

	void SetFogColor(zCOLOR const& color) override;
	zCOLOR GetFogColor() override
	{
		return fogColor;
	}
	void SetFogRange(float,float,zTRnd_FogMode) override
	void GetFogRange(float& nearz, float& farz, zTRnd_FogMode& falloff) override
	{
		nearz  = fogNearZ;
		fatz   = fogFarzZ;
		faloff = fogMode;
	}

	zTRnd_PolyDrawMode GetPolyDrawMode() const override
	{
		return polyDrawMode;
	}
	void SetPolyDrawMode(zTRnd_PolyDrawMode const& mode) override
	{
		polyDrawMode = mode;
	}

	int GetSurfaceLost() override
	{
		return surfaceLost;
	}
	void SetSurfaceLost(int val) override
	{
		surfaceLost = val;
	}

	int GetSyncOnAmbientCol() override
	{
		return syncAmbientCol;
	}
	void SetSyncOnAmbientCol(int val) override
	{
		syncAmbientCol = val;
	}

	void SetTextureWrapEnabled(int val) override
	{
		textureWrapEnabled = val;
	}
	int GetTextureWrapEnabled() override
	{
		return textureWrapEnabled;
	}

	void SetBilerpFilterEnabled(int val) override
	{
		bilerpFilterEnabled = val;
	}
	int GetBilerpFilterEnabled() override
	{
		return bilerpFilterEnabled;
	}

	void SetDitherEnabled(int val) override
	{
		ditherEnabled = val;
	}
	int GetDitherEnabled() override
	{
		return ditherEnabled;
	}

	zTRnd_PolySortMode GetPolySortMode() const override
	{
		return polySortMode;
	}
	void SetPolySortMode(zTRnd_PolySortMode const& mode) override
	{
		polySortMode = mode;
	}

	int GetZBufferWriteEnabled() override
	{
		return zBufferWriteEnabled;
	}
	void SetZBufferWriteEnabled(int enable) override;

	void SetZBias(int bias) override;
	int GetZBias() override
	{
		return zBias;
	}

	zTRnd_ZBufferCmp GetZBufferCompare() override
	{
		return zBufferCompare;
	}
	void SetZBufferCompare(zTRnd_ZBufferCmp const& cmp) override
	{
		zBufferCompare = cmp
	}

	int GetPixelWriteEnabled() override
	{
		return pixelWriteEnabled;
	}
	void SetPixelWriteEnabled(int b) override
	{
		pixelWriteEnabled = 1;
	}

	void SetAlphaBlendSource(zTRnd_AlphaBlendSource const& src) override
	{
		alphaBlendSource = src;
	}
	zTRnd_AlphaBlendSource GetAlphaBlendSource() override
	{
		return alphaBlendSource;
	}

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

	void SetAlphaReference(unsigned ref) override
	{
		alphaReference = ref;
	}
	unsigned GetAlphaReference() override
	{
		return alphaReference;
	}

	int GetCacheAlphaPolys() override
	{
		return cacheAlphaPolys;
	}
	void SetCacheAlphaPolys(int val) override
	{
		cacheAlphaPolys = val;
	}

	int GetAlphaLimitReached() override
	{
		return alphaLimitReached;
	}

	virtual void AddAlphaPoly(zCPolygon const *);
	virtual void FlushAlphaPolys();

	void SetRenderMode(zTRnd_RenderMode) override
	{
	}
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

	zCTexture* CreateTexture() override
	{
		return new zCTex_D3D();
	}

	virtual void CreateTextureConvert();
	int GetTotalTextureMem() override
	{
		return hwinfo.totalTextureMem;
	}

	bool SupportsTextureFormat(zTRnd_TextureFormat fmt) override;
	bool SupportsTextureFormatHardware(zTRnd_TextureFormat fmt) override;

	int GetMaxTextureSize() override
	{
		return hwinfo.maxTexSize;
	}

	void GetStatistics(zTRnd_Stats& out) override
	{
		out = stats;
	}
	void ResetStatistics() override
	{
		memset(&stats, 0, sizeof(stats));
	}

	virtual void Vid_Blit(int,tagRECT *,tagRECT *);
	void Vid_Clear(zCOLOR& color, int clearTarget) override;
	virtual void Vid_Lock(zTRndSurfaceDesc &);
	bool Vid_Unlock() override;
	bool Vid_IsLocked() override
	{
		return vid_isLocked;
	}
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
	unsigned Vid_GetNumModes() override
	{
		return dxDeviceModeNum[deviceNr];
	}
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
	void SetRenderState(zTRnd_RenderStateType type, unsigned val) override
	{
		if (type == 0)
			renderState[0] = val;
		else if (type == 1)
			renderState[1] = val;
	}
	void GetRenderState(zTRnd_RenderStateType type) override
	{
		if (type == 0)
			return renderState[0];
		else if (type == 1)
			return renderState[1];
		return 0;
	}

	virtual void AddAlphaSortObject(zCRndAlphaSortObject *);
	virtual void RenderAlphaSortList();
	virtual void DrawVertexBuffer(zCVertexBuffer *,int,int,ushort *,ulong);
	virtual void CreateVertexBuffer();


	int XD3D_InitPerDX(zTRnd_ScreenMode mode, unsigned x, unsigned y, int bpp, int id);
	int XD3D_EnumerateModes();
private:
	int view_xdim;
	int view_ydim;
};

void zCRnd_D3D::SetFogColor(zCOLOR const& color)
{
	fogColor = color;

	int d3d_fogColor = color[0] | (color[1] << 8) | (color[2] << 16);
	if ( xd3d_state.fogColor != d3d_fogColor ) {
		auto res = zCRnd_D3D::xd3d_pd3dDevice7->SetRenderState(D3DRENDERSTATE_FOGCOLOR, d3d_fogColor);
		if ( DXTryWarning(res, "X: [RND3D]XD3D_SetRenderState: Set render state failed!") )
			xd3d_state.fogColor = d3d_fogColor;
	}
}

void zCRnd_D3D::SetZBias(int bias)
{
	if ( bias == zBias )
		return;

	if ( xd3d_state.zBias != bias ) {
		auto res = zCRnd_D3D::xd3d_pd3dDevice7->SetRenderState(D3DRENDERSTATE_ZBIAS, bias);
		if ( DXTryWarning(res, "X: [RND3D]XD3D_SetRenderState: Set render state failed!") )
			xd3d_state.zBias = bias;
	}
	zBias = bias;
}

void zCRnd_D3D::SetZBufferWriteEnabled(int enable)
{
	zBufferWriteEnabled = enable;
	if ( xd3d_state.zWriteEnable != enable ) {
		auto res = zCRnd_D3D::xd3d_pd3dDevice7->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, enable);
		if ( DXTryWarning(res, "X: [RND3D]XD3D_SetRenderState: Set render state failed!") )
			xd3d_state.zWriteEnable = enable;
	}
}

int zCRnd_D3D::SupportsTextureFormatHardware(zTRnd_TextureFormat fmt)
{
	switch ( fmt ) {
	case 6:
	case 7:
	case 8:
		return 1;
	case 10:
		return supportDXTC;
	case 12:
		return val_9D2A90;
	default:
		return 0;
	}
}

bool zCRnd_D3D::Vid_Unlock()
{
	if ( vid_isLocked ) {
		xd3d_backBuffer->Unlock(0);
		vid_isLocked = 0;
		return 1;
	}

	return 0;
}

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
