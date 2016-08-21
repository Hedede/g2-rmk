class oCMissionManager {
public:
	oCMissionManager()
	{
		missions.Compare = SortMissions;
	}
	virtual ~oCMissionManager();
	virtual void Archive(zCArchiver& arc)
	{
		arc.WriteInt("NumOfEntries", missions.GetNumInList());

		for (int i = 0; i < missions.GetNumInList(); ++i)
			missions[i]->Archive(arc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		int numMissions = 0;
		arc.ReadInt("NumOfEntries", numMissions);

		missions.Clear();
		for (int i = 0; i < numMissions; ++i) {
			auto mission = new oCMission;
			mission->Unarchive(arc);
			missions.InsertSort(mission);
		}
	}
private:
	zCArraySort<oCMission*> missions;
};
