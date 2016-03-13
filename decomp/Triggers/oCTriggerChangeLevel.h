class oCTriggerChangeLevel : public zCTrigger {
	Z_OBJECT(oCTriggerChangeLevel);
public:
	virtual ~oCTriggerChangeLevel();

	virtual void Archive(zCArchiver& arc)
	{
		zCTrigger::Archive(arc);
		arc.WriteString("levelName", levelName);
		arc.WriteString("startVobName", startVob);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCTrigger::Unachive(arc);
		arc.ReadString("levelName", levelName);
		arc.ReadString("startVobName", startVob);
	}

	virtual void TriggerTarget(zCVob*);
	virtual void UntriggerTarget(zCVob*);

private:
	zSTRING levelName;
	zSTRING startVob;
};
