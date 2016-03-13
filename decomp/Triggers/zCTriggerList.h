class zCTriggerList : public zCTrigger {
	Z_OBJECT(zCTriggerList);
private:
	virtual ~zCTriggerList();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	virtual void OnTimer();
	virtual void GetTriggerTarget(int);
	virtual void TriggerTarget(zCVob *);
	virtual void UntriggerTarget(zCVob *);
private:
	zSTRING triggerTarget[6];
	float fireDelay[6];
	int listProcess;
	uint8_t actTarget;
	zBOOL sendOnTrigger;
};

void zCTriggerList::Archive(zCArchiver& arc)
{
	zCTrigger::Archive(arc);

	arc.WriteGroupBegin("TriggerList");

	arc.WriteEnum("listProcess", "LP_ALL;LP_NEXT_ONE;LP_RAND_ONE", listProcess);

	int numTarget = 6;
	if ( !arc.InProperties() ) {
		for (int i = 6; i > 0; --i) {
			if (triggerTarget[i].Length() > 0) {
				numTarget = i;
				break;
			}
		}

		arc.WriteByte("numTarget", numTarget);
	}

	for (int i = 0; i < numTarget; ++i) {
		arc.WriteString("triggerTarget"_s + i, triggerTarget[i]);
		arc.WriteFloat("fireDelay"_s + i, fireDelay[i]);
	}

	if ( arc.InSaveGame() ) {
		arc.WriteByte("actTarget", actTarget);
		arc.WriteBool("sendOnTrigger", sendOnTrigger);
	}

	arc.WriteGroupEnd("TriggerList");
}


void zCTriggerList::Unarchive(zCArchiver& arc)
{
	zCTrigger::Unarchive(arc);

	listProcess = 0;
	actTarget = 0;
	sendOnTrigger = 1;

	int numTarget = 6;
	for (int i = 0; i < numTarget; ++i) {
		triggerTarget[i].Clear();
		fireDelay[i] = 0;
	}


	listProcess = arc.ReadEnum("listProcess");

	if ( !arc.InProperties(arc) )
		numTarget = arc.ReadByte("numTarget");

	for (int i = 0; i < numTarget; ++i) {
		arc.ReadString("triggerTarget"_s + i, triggerTarget[i]);
		arc.ReadFloat("fireDelay"_s + i, fireDelay[i]);
		triggerTarget[i].Upper();
	}

	if ( arc.InSaveGame() ) {
		actTarget = arc.ReadByte("actTarget");
		arc.ReadBool("sendOnTrigger", sendOnTrigger);
	}
}

