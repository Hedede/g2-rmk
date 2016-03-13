struct zCVobLightData {
	zCArray<zVALUE>       rangeAniScaleList;
	zCArray<zCOLOR>       colorAniList;
	int                   lensFlareFXNo;
	zCLensFlareFX*        lensFlareFX;

	//Alphakanal hier irrelevant
	zCOLOR        lightColor;
	zVALUE        range;
	zVALUE        rangeInv;
	zVALUE        rangeBackup;

	//Daten zur Lichtanimation
	//Zustand der Reichweitenanimation
	zVALUE          rangeAniActFrame;
	zVALUE          rangeAniFPS;

	//Zustand der Farbanimation
	zVALUE                          colorAniActFrame;
	zVALUE                          colorAniFPS;

	// spotLights? Ich kenne das Feature nicht.
	zREAL         spotConeAngleDeg;

	//Siehe Auflistung oben
	struct {
		uint32_t isStatic       : 1;
		uint32_t rangeAniSmooth : 1;
		uint32_t rangeAniLoop   : 1;
		uint32_t colorAniSmooth : 1;
		uint32_t colorAniLoop   : 1;
		uint32_t isTurnedOn     : 1;
		uint32_t lightQuality   : 4;
		uint32_t lightType      : 4;
		uint32_t canMove        : 1;
	} flags;
};

class zCVobLightPreset : public zCObject {
	Z_OBJECT(zCVobLightPreset);
public:
	virtual ~zCVobLightPreset();

	virtual void Archive(zCArchiver& arc)
	{
		arc.WriteString("presetName", presetName);
		lightData.Archive(arc):
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		presetName = arc.ReadString("presetName");
		lightData.Unarchive(arc);
	}

private:
	zCVobLightData lightData;
	zSTRING presetName;
};

class zCVobLight : public zCVob {
	Z_OBJECT(zCVobLight);
public:
	zCVobLight()
		: zCVob()
	{
		type = VOB_TYPE_LIGHT;

		SetCollDetStat(0);
		SetCollDetDyn(0);

		flags1.showVisual = 1; // why? this->flags1 & 0xFE ^ 1;
	}

	virtual ~zCVobLight();

	virtual void Archive(zCArchiver& arc)
	{
		zCVob::Archive(arc);

		arc.WriteGroupBegin("VobLight");
		arc.WriteString("lightPresetInUse", lightPreset);
		lightData.Archive(arc);
		arc.WriteGroupEnd("VobLight");
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
		lightPreset = arc.ReadString("lightPresetInUse");
		lightData.Unarchive(arc);
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void EndMovement(int);
	virtual void ThisVobAddedToWorld(zCWorld*);

private:
	zCVobLightData lightData;
	// affected polys
	zTRayTurboValMap<zCPolygon *, int> lightMap;
	zSTRING lightPreset;
};
