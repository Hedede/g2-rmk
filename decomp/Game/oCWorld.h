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

private:
	//nicht ausprobiert, aber hoffentlich ist der Name Programm.
	//wie hier sortiert ist weiß ich nicht.
	zCListSort<zCVob>*   voblist;
	zCListSort<oCNpc>*   voblist_npcs;
	zCListSort<oCItem>*  voblist_items;
};

void oCWorld::Archive(zCArchiver& arc)
{
	zCWorld::Archive(arc);
	if (!arc.InSaveGame())
		return;

	int npcCount = 0;
	for (auto list : voblist_npcs) {
		if (!list->Get()->IsSelfPlayer() && !list->Get()->homeWorld)
			++npcCount;
	}
	arc.WriteInt("npcCount", npcCount);

	int nr = 0;
	for (auto list : voblist_npcs) {
		auto npc = list->Get();
		if (!npc->IsSelfPlayer() && !npc->homeWorld) {
			arc.WriteObject("npc"_s + nr, npc);
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
