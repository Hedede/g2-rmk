class zCTriggerBase : public zCVob {
	Z_OBJECT(zCTriggerBase);
public:
	virtual void Archive(zCArchiver& arc);
	{
		zCVob::Archive(arc);
		arc.WriteString("triggerTarget", triggerTarget);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
		arc.ReadString("triggerTarget", triggerTarget);
		triggerTarget.Upper();
	}

	virtual ~zCTriggerBase();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void GetTriggerTarget(int);
};
