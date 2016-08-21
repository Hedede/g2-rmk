class zCCodeMaster : public zCTriggerBase {
	Z_OBJECT(zCCodeMaster);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCodeMaster();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob*);
	virtual void OnUntouch(zCVob *);
	virtual void OnMessage(zCEventMessage	*,zCVob	*);
	virtual void GetTriggerTarget(int);

	void Reset()
	{
		for (auto& slave : slaveTriggered) {
			if (slave)
				slave->Release();
			slave = 0;
		}
		numSlavesTriggered = 0;
	}

private:
	void Init()
	{
		for (auto& slave : slaveVobName)
			slave.Clear();

		triggerTargetFailure.Clear();

		orderRelevant       = 0;
		untriggerCancels    = 0;
		firstFalseIsFailure = 0;

		Reset();
	}

	zSTRING slaveVobName[6];
	zSTRING triggerTargetFailure;

	uint32_t orderRelevant       : 1;
	uint32_t untriggerCancels    : 1;
	uint32_t firstFalseIsFailure : 1;

	zCObject* slaveTriggered[6];
	int numSlavesTriggered;
};

void zCCodeMaster::Archive(zCArchiver& arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteGroupBegin("CodeMaster");

	arc.WriteBool("orderRelevant", orderRelevant);

	arc.WriteGroupBegin("OrderRelevant");
	arc.WriteBool("firstFalseIsFailure", firstFalseIsFailure);
	arc.WriteString("triggerTargetFailure", triggerTargetFailure);
	arc.WriteGroupEnd("OrderRelevant");

	arc.WriteGroupBegin("OrderNotRelevant");
	arc.WriteBool("untriggerCancels", untriggerCancels);
	arc.WriteGroupEnd("OrderNotRelevant");

	int numSlaves = 6;
	if (!arc.InProperties()) {
		for (; numSlaves > 0; --numSlaves) {
			if (!slaveVobName.IsEmpty())
				break;
		}
	}
	arc.WriteByte("numSlaves", numSlaves);

	for (int i = 0; i < numSlaves; ++i)
		arc.WriteString("slaveVobName"_s + i, slaveVobName[i]);

	if (arc.InSaveGame()) {
		arc.WriteByte("numSlavesTriggered", numSlavesTriggered);

		for (int i = 0; i < numSlavesTriggered; ++i)
			arc.WriteObject("slaveTriggered"_s + i, slaveTriggered[i]);
	}

	arc.WriteGroupEnd("CodeMaster");
}

void __thiscall zCCodeMaster::Unarchive(zCCodeMaster *this, zCArchiver_vt **a3)
{
	zCTriggerBase::Unarchive(arc);

	Init();

	orderRelevant = arc.ReadBool("orderRelevant");
	firstFalseIsFailure = arc.ReadBool("untriggerCancels");

	arc.ReadString("triggerTargetFailure", triggerTargetFailure);
	triggerTargetFailure.Upper();

	untriggerCancels = arc.ReadBool("orderRelevant");

	int numSlaves = 6;
	if ( !arc.InProperties() )
		numSlaves = arc.ReadByte("numSlaves");

	for (int i = 0; i < numSlaves; ++i) {
		arc.ReadString("slaveVobName"_s + i, slaveVobName[i]);
		slaveVobName[i].Upper();
	}

	if ( arc.InSaveGame() ) {
		int numSlavesTriggered = arc.ReadByte("numSlavesTriggered");
		for (int i = 0; i < numSlavesTriggered; ++i)
			slaveTriggered[i] = arc.ReadObject("slaveTriggered"_s + i, 0);
	}
}
