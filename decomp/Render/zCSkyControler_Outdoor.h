const int zSKY_NUM_LAYER = 2;

struct zCSkyLayerData
{
	zESkyLayerMode skyMode;
	zCTexture *tex;
	zSTRING texName;
	zREAL texAlpha;
	zREAL texScale;
	zVEC2 texSpeed;
};

class zCSkyLayer {
	zCMesh*        skyPolyMesh;
	zCPolygon*     skyPoly;
	zVEC2          skyTexOffs;
	zCMesh*        skyDomeMesh;
	zESkyLayerMode skyMode;
};

class zCSkyState {
	zREAL time;
	zVEC3 polyColor;
	zVEC3 fogColor;
	zVEC3 domeColor1;
	zVEC3 domeColor0;
	zREAL fogDist;
	zBOOL sunOn;
	int   cloudShadowOn;

	zCSkyLayerData layer[zSKY_NUM_LAYER];
};

enum zTPlanet {
	NUM_PLANETS = 2;
};

class zCSkyPlanet {
	zCMesh* mesh;        //
	zVEC4   color0;   //  
	zVEC4   color1;   //  
	zREAL   size;        //  
	zVEC3   pos;      //  
	zVEC3   rotAxis;  //  
};

class zCSkyControler_Outdoor : public zCSkyControler_Mid {
	Z_OBJECT(zCSkyControler_Outdoor);
public:
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual  ~zCSkyControler(uint);
	virtual void SetTime(float);
	virtual void GetTime();
	virtual void ResetTime();
	virtual void SetFarZ(float);
	virtual void GetFarZ();
	virtual void SetFarZScalability(float);
	virtual void GetFarZScalability();
	virtual void SetBackgroundColor(zCOLOR);
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
	virtual void SetGlobalSkyScale(float);
	virtual float GetGlobalSkyScale()
	{
		return skyScale;
	}
	virtual void GetGlobalSkyScaleChanged();
	virtual void SetCameraLocationHint(zCSkyControler::zTCamLocationHint);
	virtual void SetWeatherType(zTWeather);
	// virtual void GetRenderLightning();
private:
	//ab hier: Outdoor spezifisch!
	zBOOL initDone;           //
	zREAL masterTime;         // //Outdoorsky hat eine Zeit
	zREAL masterTimeLast;     //

	enum zESkyLayerMode { zSKY_MODE_POLY, zSKY_MODE_BOX };

	zCSkyState masterState;
	zCSkyState* state[zSKY_NUM_LAYER];
	zCArray<zCSkyState*>      stateList;

	zCOLOR polyLightCLUT[256];
	////Farbtabelle für Beleuchtung. Abhängig von der Tageszeit
	/*  Its basically a rolled function that maps an intensity between 0 and 255 to a certain colour. It affects static lighting only.
	    polyLightCLUT is updated regularly by the Sky Controller from certain presets for different parts of the day (red in the evening, blue in the morning...).
	    In other words: At any given point it is a linear interpolation between black and the current sunlight.
	 */

	zREAL dayCounter;     //sehr nutzlos

	zCArray<zVEC3> fogColorDayVariations;
	zCArray<zVEC3> fogColorDayVariations2;

	zREAL m_fSkyScale;             //
	zBOOL m_bSkyScaleChanged;      //
	zVEC3 m_overrideColor[3];      //
	zBOOL m_bOverrideColorFlag;    //
	zBOOL m_bDontRain;             //
	zBOOL m_bLevelChanged;         //
	zBOOL m_bDarkSky;              // //Oldworld

	//fog
	zREAL  resultFogScale;          //            
	zREAL  heightFogMinY;           //            
	zREAL  heightFogMaxY;           //            
	zREAL  userFogFar;              //            
	zREAL  resultFogNear;           //            
	zREAL  resultFogFar;            //            
	zREAL  resultFogSkyNear;        //            
	zREAL  resultFogSkyFar;         //            
	zCOLOR resultFogColor;          //           
	zCOLOR resultFogColorOverride;  //           
	zREAL  userFarZScalability;     //            

	zCSkyState* skyLayerState[2];

	zCSkyLayer skyLayer[2];    
	zCSkyLayer skyLayerRainClouds;

	zCTexture*     skyCloudLayerTex;        //

	// planets
	//enum { NUM_PLANETS = 2 }; //Sonne:0, Mond:1

	zCSkyPlanet planets[NUM_PLANETS];

	zBOOL m_bSunVisible;        //
	zREAL m_fFadeScale;         //

	// sky-pfx
	zCVob*    vobSkyPFX;            //
	zREAL     skyPFXTimer;          //

	// wind
	zBOOL m_bIsMainControler;   // //gibts überhaupt noch andere als den Hauptcontroller?

	zVEC3 m_bWindVec;        //

	enum zTCamLocationHint {
		zCAM_OUTSIDE_SECTOR,
		zCAM_INSIDE_SECTOR_CANT_SEE_OUTSIDE,
		zCAM_INSIDE_SECTOR_CAN_SEE_OUTSIDE,
	};
};

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
