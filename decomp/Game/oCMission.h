struct TMisStatus {
	int vobId;
	int state;
	float startTime;
};

class oCMission {
public:
	oCMission(int instanceId);
	virtual ~oCMission();
	virtual Archive(zCArchiver& arc);
	virtual Unarchive(zCArchiver& arc);
private:
	// scripted
	zSTRING name;
	zSTRING description;
	int duration;
	int important;
	int offerConditions;
	int offer;
	int successConditions;
	int success;
	int failureConditions;
	int failure;
	int obsoleteConditions;
	int obsolete;
	int running;
	// end of scripted

	int symIndex;
	int unk0;
	int unk1;
	int id;
	int av;

	zCArray<TMisStatus> info;
	TMisStatus *status;

	int unk3;
	int unk4;
};

void oCMission::Archive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		auto sym = zparser.GetSymbol(symIndex);

		arc.WriteString("name", sym->name);
		arc.WriteInt("ID", id);
		arc.WriteBool("Av", av);
		arc.WriteInt("NumInList", info.GetNumInList());

		int statusIndex = -1;
		for (int i = 0; i < info.GetNumInList(); ++i) {
			arc.WriteInt("VobIDL", info[i]->vobId);
			arc.WriteInt("StatusL", info[i]->state);
			arc.WriteFloat("startTimeL", info[i]->startTime);

			if ( info[i] == status )
				statusIndex = i;
		}

		arc.WriteInt(arc, "StatusIndex", statusIndex);
	}
}

void oCMission::Unarchive(zCArchiver& arc)
{
	unk0 = 0;
	unk1 = 0;

	if (arc.InSaveGame()) {
		info.Clear();

		arc.ReadString("name", name);

		symIndex = zparser.GetIndex(name);

		zparser.CreateInstance(symIndex, &this->name);

		arc.ReadInt("ID", id);
		arc.ReadBool("Av", av);

		int numInList;
		arc.ReadInt("NumInList", numInList);

		for (int i = 0; i < numInList; ++i) {
			auto st = new TMisStatus;

			arc.ReadInt("VobIDL", st->vobId);
			arc.ReadInt("StatusL", st->state);
			arc.ReadFloat("startTimeL", st->startTime);

			info.Insert(st);
		}

		int statusIndex;
		arc.ReadInt("StatusIndex", statusIndex);

		if ( statusIndex < 0 )
			status = 0;
		else
			status = info[statusIndex];
	}
}
