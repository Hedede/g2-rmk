class oCWorld : public zCWorld {
	Z_OBJECT(oCWorld);
public:
	virtual ~oCWorld();
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);
	virtual void DisposeWorld();
	virtual void AddVobAsChild(zCVob *,zCTree<zCVob> *);
	virtual void RemoveVob(zCVob *);
	virtual void SearchVob(zCVob *,zCTree<zCVob> *);
	virtual void SearchVobByID(ulong,zCTree<zCVob> *);
	virtual void SearchVobByName(zSTRING const &);
	virtual void SearchVobListByName(zSTRING const &,zCArray<zCVob *> &);
	virtual void CreateVob(zTVobType,int);
	virtual void InsertVobInWorld(zCVob *);
	virtual void EnableVob(zCVob *,zCVob *);
	virtual void DisableVob(zCVob *);
	virtual void TraverseVobList(zCVobCallback &,void *);
	virtual void DisposeVobs();
};

void oCWorld::Archive(zCArchiver& arc)
{
	zCWorld::Archive(arc);
	if (!arc.InSaveGame())
		return;

	int npcCount = 0;
	for (auto j = voblist_npcs->next; j; j = j->next) {
		if (!j->data->IsSelfPlayer() && !j->data->homeWorld)
			++npcCount;
	}
	arc.WriteInt("npcCount", npcCount);

	int nr = 0;
	for (auto j = voblist_npcs->next; j; j = j->next) {
		auto npc = j->data;
		if (!npc->IsSelfPlayer() && !npc->homeWorld) {
			arc.WriteObject("npc"+nr, npc);
			++nr;
		}
	}

	if ( ogame )
		ogame->GetSpawnManager()->Archive(arc);
}

void oCWorld::Unarchive(zCArchiver& arc)
{
	zCWorld::Unarchive(arc);
	if (!arc.InSaveGame())
		return;

	int npcCount = arc.ReadInt("npcCount");

	for (int i = 0; i < npcCount; ++i) {
		auto npc = arc.ReadObject("npc" + i);
		InsertInLists(npc);
	}

	if ( ogame )
		ogame->GetSpawnManager()->Unarchive(arc);
}
