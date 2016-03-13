class zCPFXControler : public zCEffect {
	Z_OBJECT(zCPFXControler);
public:
	virtual ~zCPFXControler();

	virtual void Archive(zCArchiver& arc)
	{
		zCEffect::Archive(arc);
		arc.WriteString("pfxName", pfxName);
		arc.WriteBool("killVobWhenDone", killVobWhenDone);
		arc.WriteBool("pfxStartOn", pfxStartOn);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);
		arc.ReadString("pfxName", pfxName);
		killVobWhenDone = arc.ReadBool("killVobWhenDone");
		pfxStartOn = arc.ReadBool("pfxStartOn");

		if ( arc.InSaveGame() )
			pfxStartOn = 0;

		if ( pfxName.Search(0, ".PFX", 1u) == -1 )
			pfxName += ".PFX";
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void PostLoad();

private:
	zSTRING pfxName;
	uint32_t killVobWhenDone : 1;
	uint32_t pfxStartOn      : 1;
};

