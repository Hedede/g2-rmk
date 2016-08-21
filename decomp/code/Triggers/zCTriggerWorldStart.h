class zCTriggerWorldStart : public zCTriggerBase {
	Z_OBJECT(zCTriggerWorldStart);
public:
	virtual ~zCTriggerWorldStart();

	virtual void Archive(zCArchiver& arc)
	{
		zCTriggerBase::Archive(arc);

		arc.WriteBool("fireOnlyFirstTime", fireOnlyFisrtTime);
		if ( arc.InSaveGame() )
			arc.WriteBool("hasFired", hasFired);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCTriggerBase::Unarchive(arc);

		fireOnlyFisrtTime = arc.ReadBool("fireOnlyFirstTime");
		if ( arc.InSaveGame() )
			hasFired = arc.ReadBool("hasFired");
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void PostLoad();
private:
	bool fireOnlyFisrtTime;
	bool hasFired;
};
