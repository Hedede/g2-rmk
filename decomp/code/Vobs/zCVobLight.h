struct zCVobLightData {
	void SetRange(float r, int doBackup)
	{
		if (r < 0.0f)
			r = 0.0f;

		range = r;

		if (range == 0.0f)
			rangeInv = std::numeric_limits<float>::max();
		rangeInv = 1.0 / r;

		if ( doBackup )
			rangeBackup = r;
	}

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
		uint32_t isStatic       : 1; // 1
		uint32_t rangeAniSmooth : 1; // 2
		uint32_t rangeAniLoop   : 1; // 4
		uint32_t colorAniSmooth : 1; // 8
		uint32_t colorAniLoop   : 1; // 0x10
		uint32_t isTurnedOn     : 1; // 0x20
		uint32_t lightQuality   : 4; // 0x40 0x80 0x100 0x200
		uint32_t lightType      : 4; // 0x400 0x800 0x1000 0x2000
		uint32_t canMove        : 1; // 0x4000
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
private:
	static zCArray<zCVobLightPreset*> lightPresetList;
public:
	static void CleanupVobLight();

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

	void SetRange(float r, int doBackup);

private:
	zCVobLightData lightData;
	// affected polys
	zTRayTurboValMap<zCPolygon *, int> lightMap;
	zSTRING lightPreset;
};

void zCVobLight::CleanupVobLight()
{
	for (auto preset : lightPresetList)
		preset->Release();

	lightPresetList.DeleteList();
}

void zCVobLight::SetRange(float r, int doBackup)
{
	lightData.SetRange(r, doBackup);

	zTBBox3D tmp;
	tmp.mins[0] = -lightData.range;
	tmp.mins[1] = -lightData.range;
	tmp.mins[2] = -lightData.range;
	tmp.maxs[0] = lightData.range;
	tmp.maxs[1] = lightData.range;
	tmp.maxs[2] = lightData.range;

	SetBBox3DLocal(tmp);

	if ( doBackup ) {
		if ( homeWorld )
			homeWorld->bspTree.AdjustVob(this);
	}
}
