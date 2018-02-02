class zCVobSound : public zCZone {
	Z_OBJECT(zCVobSound);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;
	virtual ~zCVobSound();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void EndMovement(int);
	virtual void ThisVobAddedToWorld(zCWorld*);
	virtual void ThisVobRemovedFromWorld(zCWorld*);
	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone	*> const &,zCWorld *);
	virtual void GetDebugDescString();
	virtual void DoSoundUpdate(float);

	void StopSound();
	void SetSoundRadius(float radius);

	void zCVobSound::SetSoundVolume(float volume)
	{
		sndVolume = volume;
	}

private:
	int unk1;

	zSTRING sndName;
	float sndRadius;
	uint32_t sndMode;

	struct {
		uint32_t sndStartOn        : 1;
		uint32_t sndAmbient3D      : 1;
		uint32_t sndObstruction    : 1;
		uint32_t sndVolType        : 1;
		uint32_t sndIsRunning      : 1;
		uint32_t sndIsAllowedToRun : 1;
	} sndFlags;

	float sndRandDelay;
	float sndRandDelayVar;
	float sndVolume;
	float sndConeAngle;

	zCSoundFX *sndFx;
	int sndHandle;

	float unk2;
	zVEC3 vec2;
};

void zCVobSound::Archive(zCArchiver& arc)
{
	zCZone::Archive(arc);

	arc.WriteGroupBegin("Sound");
	arc.WriteFloat("sndVolume", sndVolume);
	arc.WriteEnum("sndMode", "LOOPING;ONCE;RANDOM", sndMode);
	arc.WriteFloat("sndRandDelay", sndRandDelay);
	arc.WriteFloat("sndRandDelayVar", sndRandDelayVar);
	arc.WriteBool("sndStartOn", sndFlags.sndStartOn);
	arc.WriteBool("sndAmbient3D", sndFlags.sndAmbient3D);
	arc.WriteBool("sndObstruction", sndFlags.sndObstruction);
	arc.WriteFloat("sndConeAngle", sndConeAngle);
	arc.WriteEnum("sndVolType", "SPHERE;ELLIPSOID", sndFlags.sndVolType);
	arc.WriteFloat("sndRadius", sndRadius);
	arc.WriteString("sndName", sndName);
	arc.WriteGroupEnd("Sound");

	if ( arc.InSaveGame() ) {
		arc.WriteBool("soundIsRunning", sndFlags.sndIsRunning);
		arc.WriteBool("soundAllowedToRun", sndFlags.sndIsAllowedToRun);
	}
}

void zCVobSound::Unarchive(zCArchiver& arc)
{
	zCZone::Unarchive(arc);

	StopSound();

	arc.ReadFloat("sndVolume", sndVolume);
	sndMode = arc.ReadEnum("sndMode" );
	arc.ReadFloat("sndRandDelay", sndRandDelay);
	arc.ReadFloat("sndRandDelayVar", sndRandDelayVar);
	sndFlags.sndStartOn     = arc.ReadBool("sndStartOn");
	sndFlags.sndAmbient3D   = arc.ReadBool("sndAmbient3D");
	sndFlags.sndObstruction = arc.ReadBool("sndObstruction",);

	arc.ReadFloat("sndConeAngle", sndConeAngle);

	sndFlags.sndVolume      = arc.ReadEnum("sndVolType");

	float radius = arc.ReadFloat("sndRadius");
	SetSoundRadius(radius);

	sndName = arc.ReadString("sndName", sndName);

	zCSoundFX* fx = 0;
	if (zsound) {
		if (sndName.Search(0, ".", 1u) == -1)
			fx = zsound->LoadSoundFXScript(sndName);
		else
			fx = zsound->LoadSoundFX(sndName);
	}

	Release(sndFx);
	sndFx = fx;

	if ( arc.InSaveGame() ) {
		sndFlags.sndIsRunning       = arc.ReadBool("soundIsRunning");
		sndFlags.sndIsAllowedToRun  = arc.ReadBool("soundAllowedToRun");
	}
}
