class oCTriggerScript : public zCTrigger {
	Z_OBJECT(oCTriggerScript);
public:
	virtual void Archive(zCArchiver& arc);
	{
		zCTrigger::Archive(arc);
		arc.WriteString("scriptFunc", scriptFunc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		zCTrigger::Unarchive(arc);
		arc.ReadString("scriptFunc", scriptFunc);
	}
	virtual ~oCTriggerScript();
	virtual void TriggerTarget(zCVob *);
	virtual void UntriggerTarget(zCVob *);
private:
	zSTRING scriptFunc;
};
