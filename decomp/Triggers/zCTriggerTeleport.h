class zCTriggerTeleport : public zCTriggerBase {
	Z_OBJECT(zCTriggerTeleport);
public:
	virtual ~zCTriggerTeleport();

	virtual void Archive(zCArchiver& arc)
	{
		zCTriggerBase::Archive(arc);
		arc.WriteString("sfxTeleport", soundTeleport);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCTriggerBase::Archive(arc);
		arc.ReadString("sfxTeleport", soundTeleport);
		Release(sfxTeleport);
		sfxTeleport = zsound->LoadSoundFX(soundTeleport);
		SetCollisionClass(zCCollObjectBoxPassThrough::s_oCollObjClass);
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);

private:
	zSTRING soundTeleport;
	zCSoundFX* sfxTeleport;
};
