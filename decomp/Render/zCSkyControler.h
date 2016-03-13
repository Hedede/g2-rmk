//######################################################
//
//  Skycontroller
//      Der Outdoor Skycontroller ist der speziellste
//      aller SkyController.
//
// Update: Falsch! Noch spezieller ist der 
// oCSkyControler_Barrier. Ich werden diesen hier aber
// nicht umbenennen sondern die Barrier Eigenschaften
// einfach unten dran fügen.
//######################################################
/* eigentlich oCSkyControler_Barrier */ 
class zCSkyControler : public zCObject {
	Z_OBJECT(zCSkyControler);
private:
	virtual ~zCSkyControler() = default;
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
	virtual void GetCloudShadowScale();
	virtual void SetCloudShadowScale(float);
	virtual void SetFillBackground(int);
	virtual void GetFillBackground();
	virtual void GetBackgroundColorDef() = 0;
	virtual void SetOverrideColor(zVEC3) = 0;
	virtual void SetOverrideColorFlag(int) = 0;
	virtual void SetLightDirty();
	virtual void SetRelightTime(float);
	virtual void GetRelightCtr() const
	{
		return relightCtr;
	}
	virtual void GetDaylightColorFromIntensity(int);
	virtual void RenderSkyPre() = 0;
	virtual void RenderSkyPost(int) = 0;
	virtual void GetGlobalWindVec(zVEC3 &,zTAnimationMode);
	virtual void SetGlobalSkyScale(float);
	virtual float GetGlobalSkyScale()
	{
		return 0.0;
	}
	virtual void GetGlobalSkyScaleChanged();
	virtual void SetCameraLocationHint(zCSkyControler::zTCamLocationHint);

	enum zTWeather
	{
		zTWEATHER_SNOW,
		zTWEATHER_RAIN
	};
	virtual void SetWeatherType(zTWeather);
	virtual zTWeather GetWeatherType()
	{
		return weather;
	}
	// virtual void GetRenderLightning() = 0;
private:
	//Es wird eine Tageszeitabhängige Lookuptable für Lichtwerte bereitgestellt.
	// "colour look up table", siehe unten
	zCOLOR* polyLightCLUTPtr;

	zREAL      cloudShadowScale;
	zBOOL      m_bColorChanged;
	zTWeather  m_enuWeather;

	zCOLOR backgroundColor;
	zBOOL  fillBackground;
	zCTexture* backgroundTexture;
	int relightCtr; //zTFrameCtr
	zREAL lastRelightTime;
	zREAL m_fRelightTime;
};
