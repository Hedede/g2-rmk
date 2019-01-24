const int zSKY_NUM_LAYER = 2;

enum zESkyLayerMode {
	zSKY_MODE_POLY,
	zSKY_MODE_BOX
};

struct zCSkyLayerData {
	zCSkyLayerData();
	~zCSkyLayerData();

	zESkyLayerMode skyMode;
	zCTexture *tex  = nullptr;
	zSTRING texName;
	zREAL texAlpha = 1.0;
	zREAL texScale = 0.0;
	zVEC2 texSpeed = { 1.0, 0.0 };
};

class zCSkyLayer {
	void RenderSkyLayer(zCSkyState *skyLayerState)
	{
		if ( skyPoly->material->color.Alpha() == 0.0 )
			return;
		if ( skyMode == zSKY_MODE_POLY )
			RenderSkyPoly(skyLayerState);
		else
			//RenderSkyBox(skyLayerState); // in g1
			RenderSkyDome(skyLayerState);
	}

	zCMesh*        skyPolyMesh;
	zCPolygon*     skyPoly;
	zVEC2          skyTexOffs;
	zCMesh*        skyDomeMesh;
	zESkyLayerMode skyMode;
};

class zCSkyState {
	zCSkyState() = default;
	~zCSkyState() = default;

	void PresetDay0();
	void PresetDay1();
	void PresetDay2();
	void PresetEvening();
	void PresetNight0();
	void PresetNight1();
	void PresetNight2();
	void PresetDawn();


	zREAL time;
	zVEC3 polyColor;
	zVEC3 fogColor;
	zVEC3 domeColor1;
	zVEC3 domeColor0;
	zREAL fogDist = 0.0;
	zBOOL sunOn   = true;
	int   cloudShadowOn = 0;

	zCSkyLayerData layer[zSKY_NUM_LAYER];
};

enum zTPlanet {
	SUN = 0,
	MOON = 1,
	NUM_PLANETS = 2;
};

class zCSkyPlanet {
	zCMesh* mesh;
	zVEC4   color0;
	zVEC4   color1;
	zREAL   size;
	zVEC3   pos;
	zVEC3   rotAxis;
};

// Der Outdoor Skycontroller ist der speziellste aller SkyController.
class zCSkyControler_Outdoor : public zCSkyControler_Mid {
	Z_OBJECT(zCSkyControler_Outdoor);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~zCSkyControler();

	void SetTime(float newTime) override;
	virtual void GetTime();
	virtual void ResetTime();

	virtual void SetFarZ(float);
	float GetFarZ()
	{
		return userFogFar;
	}

	virtual void SetFarZScalability(float);
	float GetFarZScalability() override
	{
		return userFarZScalability;
	}

	void SetBackgroundColor(zCOLOR) override;
	virtual void GetBackgroundColor();
	virtual void GetBackgroundColorDef();
	virtual void SetOverrideColor(zVEC3);
	virtual void SetOverrideColorFlag(int);
	virtual void GetCloudShadowScale();
	virtual void SetCloudShadowScale(float);
	virtual void UpdateWorldDependencies();
	virtual void GetDaylightColorFromIntensity(int);
	virtual void RenderSkyPre();
	virtual void RenderSkyPost(int);
	virtual void GetGlobalWindVec(zVEC3 &,zTAnimationMode);

	void SetGlobalSkyScale(float scale) override
	{
		if (skyScale != scale) {
			skyScale = scale;
			skyScaleChanged = true;
		}
	}
	float GetGlobalSkyScale() override
	{
		return skyScale;
	}
	bool GetGlobalSkyScaleChanged() override
	{
		return skyScaleChanged;
	}

	void SetCameraLocationHint(zCSkyControler::zTCamLocationHint hint) override
	{
		rainFX.camLocationHint = hint;
	}

	virtual void SetWeatherType(zTWeather);
	bool GetRenderLightning() override
	{
		return rainFX.renderLightning;
	}
	
private:
	//ab hier: Outdoor spezifisch!
	zBOOL initDone;           //
	zREAL masterTime;         // //Outdoorsky hat eine Zeit
	zREAL masterTimeLast;     //


	zCSkyState masterState;
	zCSkyState* state[zSKY_NUM_LAYER];
	zCArray<zCSkyState*>      stateList;

	////Farbtabelle für Beleuchtung. Abhängig von der Tageszeit
	/*  Its basically a rolled function that maps an intensity between
	 *  0 and 255 to a certain colour. It affects static lighting only.
	 *  polyLightCLUT is updated regularly by the Sky Controller from
	 *  certain presets for different parts of the day (red in the evening,
	 *  blue in the morning...).
	 *  In other words: At any given point it is a linear interpolation
	 *  between black and the current sunlight.
	 */
	zCOLOR polyLightCLUT[256];

	zREAL dayCounter;     //sehr nutzlos

	zCArray<zVEC3> fogColorDayVariations;
	zCArray<zVEC3> fogColorDayVariations2;

	zREAL skyScale;
	zBOOL skyScaleChanged;

	zVEC3 overrideColor[3];
	zBOOL overrideColorFlag;
	zBOOL dontRain;
	zBOOL levelChanged;
	zBOOL darkSky;    //Oldworld

	//fog
	zREAL  resultFogScale;
	zREAL  heightFogMinY;
	zREAL  heightFogMaxY;
	zREAL  userFogFar;
	zREAL  resultFogNear;
	zREAL  resultFogFar;
	zREAL  resultFogSkyNear;
	zREAL  resultFogSkyFar;
	zCOLOR resultFogColor;
	zCOLOR resultFogColorOverride;
	zREAL  userFarZScalability;

	zCSkyState* skyLayerState[2];

	zCSkyLayer skyLayer[2];
	zCSkyLayer skyLayerRainClouds;

	zCTexture*     skyCloudLayerTex;

	zCSkyPlanet planets[NUM_PLANETS];

	zBOOL sunVisible;
	zREAL fadeScale;

	// sky-pfx
	zCVob*    vobSkyPFX;
	zREAL     skyPFXTimer;

	// wind
	// gibts überhaupt noch andere als den Hauptcontroller?
	zBOOL isMainControler;

	zVEC3 windVec;

	struct zTRainFX {
		zCOutdoorRainFX*  outdoorRainFX;
		zTCamLocationHint camLocationHint;
		zREAL             outdoorRainFXWeight; // 0..1
		zREAL             soundVolume;         // 0..1
		zREAL             timerInsideSectorCantSeeOutside;// msec
		zREAL             timeStartRain;
		zREAL             timeStopRain;
		zBOOL             renderLightning;
		zBOOL             m_bRaining;
		//Anzahl der bisherigen Regenperioden.
		int               m_iRainCtr;
	} rainfx;
};

//------------------------------------------------------------------------------
void zCSkyLayerData::zCSkyLayerData()
{
	skyMode = zoptions->ReadBool("SKY_OUTDOOR", skyDome, true);
}

void zCSkyLayerData::~zCSkyLayerData()
{
	DELETE( tex );
}

void zCSkyState::PresetDawn()
{
	time = 0.7;
	polyColor = { 190.0, 160.0, 255.0 };
	fogColor  = { 80.0, 60.0, 105.0 };
	domeColor0 = { 80.0, 60.0, 105.0 };
	domeColor1 = { 255.0, 255.0, 255.0 };
	layer[0].texAlpha = 128.0;
	layer[1].texAlpha = 128.0;
	fogDist = 0.5;
	sunOn = 1;
}

void zCSkyState::PresetDay0()
{
	time = 0.75;
	polyColor = { 255.0, 250.0, 235.0 };
	fogColor  = { 120.0, 140.0, 180.0 };
	domeColor1 = { 255.0, 255.0,  255.0 };
	domeColor0 = fogColor;
	fogDist = 0.2;
	sunOn = 1;

	// confusing
	layer[0].texName = "SKYDAY_LAYER1_A0.TGA";
	layer[1].texName = "SKYDAY_LAYER0_A0.TGA";
	layer[0].texAlpha = 0.0;
	layer[1].texAlpha = 255.0;
	layer[1].texSpeed.x *= 0.2;
	layer[1].texSpeed.y *= 0.2;
}

int zCSkyState::PresetDay1()
{
	time = 0.0;
	polyColor = { 255.0, 250.0, 235.0 };
	fogColor  = { 120.0, 140.0, 180.0 };
	domeColor1 = { 255.0, 255.0, 255.0 };
	domeColor0 = fogColor;
	fogDist = 0.05;
	sunOn = 1;

	layer[0].texName = "SKYDAY_LAYER1_A0.TGA";
	layer[0].texAlpha = 215.0;
	layer[1].texAlpha = 255.0;
}

void __thiscall zCSkyState::PresetDay2(zCSkyState *this)
{
	zVEC3 *v1; // edx
	zVEC3 *v2; // edi
	zVEC3 *v3; // edi
	zVEC3 *v4; // ecx

	time = 0.25;
	polyColor = { 255.0, 250.0, 235.0 };
	fogColor  = { 120.0, 140.0, 180.0 };
	domeColor1 = { 255.0, 255.0, 255.0 };
	domeColor0 = { 120.0, 140.0, 180.0 };
	fogDist = 0.05;
	sunOn = 1;
	layer[0].texAlpha = 0.0;
	layer[1].texAlpha = 255.0;
}

void zCSkyState::PresetEvening()
{
	time = 0.30000001;
	polyColor  = { 255.0, 185.0, 170.0 };
	fogColor   = { 170.0, 70.0, 50.0 };
	domeColor1 = { 255.0, 255.0, 255.0 };
	domeColor0 = { 170.0, 70.0, 50.0 };
	layer[0].texAlpha = 128.0;
	layer[1].texAlpha = 128.0;
	sunOn = 1;
	fogDist = 0.2;
}

void zCSkyState::PresetNight0()
{
	time = 0.35;
	polyColor  = { 105.0, 105.0, 195.0 };
	fogColor   = { 20.0, 20.0, 60.0 };
	domeColor1 = { 55.0, 55.0, 155.0 };

	sunOn = 0;
	fogDist = 0.1;

	layer[0].texName = "SKYNIGHT_LAYER0_A0.TGA";
	layer[1].texName = "SKYNIGHT_LAYER1_A0.TGA";

	layer[0].texScale *= 4.0;
	layer[0].texSpeed.y = 0.0;
	layer[0].texSpeed.x = 0.0;
	layer[0].texAlpha = 255.0;
	layer[1].texAlpha = 0.0;

	domeColor0 = fogColor;
	cloudShadowOn = 0;
}

void zCSkyState::PresetNight1()
{
	time = 0.5;
	polyColor  = { 40.0, 60.0, 210.0 };
	fogColor   = { 5.0, 5.0, 20.0 };
	domeColor1 = { 55.0, 55.0, 155.0 };

	fogDist = 0.05;
	sunOn = 0;

	layer[0].texAlpha = 255.0;
	layer[1].texName = "SKYNIGHT_LAYER1_A0.TGA"1;
	layer[1].texAlpha = 215.0;

	domeColor0 = fogColor;
}

void zCSkyState::PresetNight2()
{
	time = 0.65;
	polyColor = {40.0, 60.0, 210.0 };
	fogColor = {5.0, 5.0, 20.0 };
	domeColor1 = {55.0, 55.0, 155.0 };
	fogDist = 0.2;
	sunOn = 0;
	layer[0].texAlpha = 255.0;
	layer[1].texName = "SKYDAY_LAYER0_A0.TGA";
	layer[1].texAlpha = 0.0;
	domeColor0 = fogColor;
}



//------------------------------------------------------------------------------
void zCSkyControler_Outdoor::Archive(zCArchiver& archiver)
{
	if (archiver.InSaveGame()) {
		archiver.WriteFloat("masterTime",     masterTime);
		archiver.WriteFloat("rainWeight",     rainFX.outdoorRainFXWeight);
		archiver.WriteFloat("rainStart",      rainFX.timeStartRain);
		archiver.WriteFloat("rainStop",       rainFX.timeStopRain);
		archiver.WriteFloat("rainSctTimer",   rainFX.timerInsideSectorCantSeeOutside);
		archiver.WriteFloat("rainSndVol",     rainFX.soundVolume);
		archiver.WriteFloat("dayCtr",         dayCounter);
		archiver.WriteFloat("fadeScale",      fadeScale);
		archiver.WriteBool("renderLightning", rainFX.renderLightning);
		archiver.WriteBool("isRaining\t\t ",  rainFX.m_bRaining);
		archiver.WriteInt("rainCtr\t\t ",     rainFX.m_iRainCtr);
	}
}

void zCSkyControler_Outdoor::Unarchive(zCArchiver& archiver)
{
	if (archiver.InSaveGame()) {
		archiver.ReadFloat("masterTime",     masterTime);
		archiver.ReadFloat("rainWeight",     rainFX.outdoorRainFXWeight);
		archiver.ReadFloat("rainStart",      rainFX.timeStartRain);
		archiver.ReadFloat("rainStop",       rainFX.timeStopRain);
		archiver.ReadFloat("rainSctTimer",   rainFX.timerInsideSectorCantSeeOutside);
		archiver.ReadFloat("rainSndVol",     rainFX.soundVolume);
		archiver.ReadFloat("dayCtr",         dayCounter);
		archiver.ReadFloat("fadeScale",      fadeScale);
		archiver.ReadBool("renderLightning", rainFX.renderLightning);
		archiver.ReadBool("isRaining\t\t ",  rainFX.m_bRaining);
		archiver.ReadInt("rainCtr\t\t ",     rainFX.m_iRainCtr);
	}
}

void zCSkyControler_Outdoor::SetTime(float newTime)
{
	lastRelightTime = lastRelightTime - newTime;
	masterTime = newTime;
	if ( fabs(lastRelightTime) > relightTime ) {
		++relightCtr;
		lastRelightTime = newTime;
	}
}
