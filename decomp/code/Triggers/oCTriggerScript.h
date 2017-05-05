// _bert/oTrigger.cpp
class oCTriggerScript : public zCTrigger {
	Z_OBJECT(oCTriggerScript);
public:
	oCTriggerScript() = default;
	virtual ~oCTriggerScript() = default;

	void Archive(zCArchiver& arc) override
	{
		zCTrigger::Archive(arc);
		arc.WriteString("scriptFunc", scriptFunc);
	}
	void Unarchive(zCArchiver& arc) override
	{
		zCTrigger::Unarchive(arc);
		arc.ReadString("scriptFunc", scriptFunc);
	}

	void TriggerTarget(zCVob *vobInstigator) override
	{
		zCTrigger::TriggerTarget(vobInstigator);
		zINFO(8, "B: TRIGGER: " + GetClassName() + " " + GetObjectName() + " triggered -> " + scriptFunc); // 157

		zparser.SetInstance("SELF", dynamic_cast<oCNpc*>(vobInstigator));
		zparser.SetInstance("OTHER", nullptr);
		zparser.SetInstance("ITEM", nullptr);
		zparser.CallFunc(scriptFunc);
	}
	void UntriggerTarget(zCVob* vobInstigator) override
	{
		zINFO(8, "B: TRIGGER: " + GetClassName() + " " + GetObjectName() + " untriggered -> " + scriptFunc); // 166
		zCTrigger::UntriggerTarget(vobInstigator);
	}

	void SetScriptFunc(zSTRING const& name, int calls)
	{
		scriptFunc = name;
		countCanBeActivated = calls;
		numCanBeActivated = calls;
	}

private:
	zSTRING scriptFunc;
};
