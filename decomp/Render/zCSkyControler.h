float zSunMaxScreenBlendScale = -1.0;

class zCSkyControler : public zCObject {
	Z_OBJECT(zCSkyControler);
private:
	enum zTCamLocationHint {
		zCAM_OUTSIDE_SECTOR,
		zCAM_INSIDE_SECTOR_CANT_SEE_OUTSIDE,
		zCAM_INSIDE_SECTOR_CAN_SEE_OUTSIDE,
	};

	zCSkyControler()
		: zCObject()
	{
		if (zSunMaxScreenBlendScale == -1.0) {
			zSunMaxScreenBlendScale = zoptions.ReadReal("ENGINE", "zSunMaxScreenBlendScale", 0.8);
			cloudShadowScale = zoptions.ReadReal("ENGINE", "zCloudShadowScale", cloudShadowScale);
		}
	}

	virtual ~zCSkyControler()
	{
		Release(backgroundTexture);
	}

	virtual void SetTime(float) = 0;
	virtual void GetTime() = 0;
	virtual void ResetTime() = 0;
	virtual void SetFarZ(float) = 0;
	virtual void GetFarZ() = 0;
	virtual void SetFarZScalability(float) = 0;
	virtual void GetFarZScalability() = 0;
	virtual void SetBackgroundColor(zCOLOR) = 0;
	virtual void GetBackgroundColor() = 0;
	virtual void GetBackgroundColorDef() = 0;
	virtual void SetOverrideColor(zVEC3) = 0;
	virtual void SetOverrideColorFlag(int) = 0;
	virtual float GetCloudShadowScale()
	{
		return 0.0;
	}
	virtual void SetCloudShadowScale(float) {}
	virtual void SetFillBackground(int b)
	{
		fillBackground = b;
	}
	virtual bool GetFillBackground()
	{
		return fillBackground;
	}
	virtual void GetBackgroundColorDef() = 0;
	virtual void SetOverrideColor(zVEC3) = 0;
	virtual void SetOverrideColorFlag(int) = 0;

	virtual void SetLightDirty()
	{
		++relightCtr;
	}
	virtual void SetRelightTime(float time)
	{
		relightTime = time;
	}
	virtual void GetRelightCtr() const
	{
		return relightCtr;
	}
	virtual zCOLOR GetDaylightColorFromIntensity(int intensity)
	{
		return {intensity, intensity, intensity, -1};
	}
	virtual void RenderSkyPre() = 0;
	virtual void RenderSkyPost(int) = 0;

	virtual int GetGlobalWindVec(zVEC3 &,zTAnimationMode)
	{
		return 0;
	}
	virtual void SetGlobalSkyScale(float) {}
	virtual float GetGlobalSkyScale()
	{
		return 0.0;
	}
	virtual bool GetGlobalSkyScaleChanged()
	{
		return false;
	}
	virtual void SetCameraLocationHint(zCSkyControler::zTCamLocationHint) {}

	enum zTWeather {
		zTWEATHER_SNOW,
		zTWEATHER_RAIN
	};
	virtual void SetWeatherType(zTWeather _weather)
	{
		weather = _weather;
	}
	virtual zTWeather GetWeatherType()
	{
		return weather;
	}

	virtual void GetRenderLightning() = 0;

	void ClearBackground(zCOLOR color);

private:
	//Es wird eine Tageszeitabhängige Lookuptable für Lichtwerte bereitgestellt.
	// "colour look up table", siehe unten
	zCOLOR* polyLightCLUTPtr = nullptr;

	zREAL      cloudShadowScale = 0.0;
	zBOOL      colorChanged = false;
	zTWeather  weather      = 1;

	zCOLOR backgroundColor;
	zBOOL  fillBackground = true;
	zCTexture* backgroundTexture = nullptr;

	zTFrameCtr relightCtr = 0;
	zREAL lastRelightTime = 0.0;
	zREAL relightTime = 0.000018;
};

void zCSkyControler::ClearBackground(zCOLOR color)
{
	zCCamera::activeCam->targetView->GetViewport(ymin, xdim, xdim, ydim);
	if ( xdim < zrenderer->view_xdim || ydim < zrenderer->view_ydim ) {
		zVEC2 min{xmin, ymin};
		zVEC2 max{xmin + xdim, ymin + ydim};

		auto zbuf_cmp = zrenderer->GetZBufferCompare();
		auto abl_src  = zrenderer->GetAlphaBlendSource();
		auto abl_fac  = zrenderer->GetAlphaBlendFactor();
		auto abl_func = zrenderer->GetAlphaBlendFunc();

		if ( !GetFillBackground() ) {
			zrenderer->SetAlphaBlendFunc(2);
			zrenderer->SetAlphaBlendSource(1);
			zrenderer->SetAlphaBlendFactor(0.0);
		}

		zrenderer->SetZBufferCompare(0.0);

		if ( !backgroundTexture )
			backgroundTexture = zCTexture::Load("DEFAULT.TGA", 0);

		zVEC2 vec1, vec2;

		zrenderer->DrawTile(backgroundTexture, min, max, zCCamera::activeCam->farClipZ, vec1, vec2, 0);

		zrenderer->SetZBufferCompare(zbuf_cmp);
		zrenderer->SetAlphaBlendSource(abl_src);
		zrenderer->SetAlphaBlendFactor(abl_fac);
		zrenderer->SetAlphaBlendFunc(abl_func);
	} else {
		if ( GetAlphaTestingEnabled() )
			zrenderer->Vid_Clear(color, 3);
		else
			zrenderer->Vid_Clear(color, 2);
	}
}
