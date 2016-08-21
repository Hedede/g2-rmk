class oCWorld : public zCWorld {
	Z_OBJECT(oCWorld);
public:
	oCWorld();
	~oCWorld() override;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	int LoadWorld(zSTRING const& fileName, zCWorld::zTWorldLoadMode loadMode) override;
	void SaveWorld(zSTRING const& fileName, zCWorld::zTWorldSaveMode saveMode, int writeBinary, int saveLevelMesh) override;

	void DisposeWorld() override
	{
		DisposeVobs();
		zCWorld::DisposeWorld();
	}

	virtual void AddVobAsChild(zCVob *,zCTree<zCVob> *);
	virtual void RemoveVob(zCVob *);
	virtual void SearchVob(zCVob *,zCTree<zCVob> *);
	virtual void SearchVobByID(ulong,zCTree<zCVob> *);
	virtual void SearchVobByName(zSTRING const &);
	virtual void SearchVobListByName(zSTRING const &,zCArray<zCVob *> &);

	virtual void CreateVob(zTVobType type, int instanceId)
	{
		zINFO(10, "U: (oCWorld::CreateVob) "_s + instanceId); // 315, oWorld.cpp
		if ( type == VOB_TYPE_ITEM ) {
			auto ofactory = dynamic_cast<oCObjectFactory*>(zfactory);
			ofactory->CreateItem(instanceId);
		} else if ( type == VOB_TYPE_NPC ) {
			auto ofactory = dynamic_cast<oCObjectFactory*>(zfactory);
			ofactory->CreateNpc(instanceId);
		}
	}

	virtual void InsertVobInWorld(zCVob* vob)
	{
		if (vob)
			InsertInLists(vob);
	}
	virtual void EnableVob(zCVob *,zCVob *);
	virtual void DisableVob(zCVob *);
	virtual void TraverseVobList(zCVobCallback& cb, void* cb_data)
	{
		for (auto vob : voblist)
			if (vob)
				cb->HandleVob(vob, cb_data);
	}

	void ClearNpcPerceptionVobLists()
	{
		for (auto npc : voblist_npcs) {
			if (!npc->IsSelfPlayer()) {
				npc->ClearPerceptionLists();
				npc->SetEnemy(0);
			}
		}
	}

	virtual void DisposeVobs();

	void InsertInLists(zCVob* vob);

	zSTRING GetWorldFilename()
	{
		return worldFilename;
	}

	zSTRING GetWorldName()
	{
		return worldName;
	}


private:
	zSTRING worldFilename;   //Pfad des aktuellen Levels
	zSTRING worldName;       //Name des aktuellen Levels

	//nicht ausprobiert, aber hoffentlich ist der Name Programm.
	//wie hier sortiert ist weiß ich nicht.
	zCListSort<zCVob>*   voblist;
	zCListSort<oCNpc>*   voblist_npcs;
	zCListSort<oCItem>*  voblist_items;
};

oCWorld::oCWorld()
	: zCWorld()
{
	zINFO(9, "U: (oCWorld) Construction"); // 262, _ulf/oWorld.cpp

	oCCollObjectCharacter::S_RegisterCollisionTestFuncs();

	voblist = new zCListSort<zCVob>;
	voblist_npcs = new zCListSort<oCNpc>;
	voblist_items = new zCListSort<oCItem>;

	voblist->Compare = CompareVobsByID;
	voblist_npcs->compare = CompareNpcsByID;
	voblist_items->compare = CompareItemsByID;

	SetSkyControlerOutdoor(new oCSkyControler_Barrier);
}

oCWorld::~oCWorld()
{
	DisposeVobs();
	DisposeWorld();

	zCPlayerGroup::allPlayerGroup::ResetPlayerList();

	Delete(voblist);
	Delete(voblist_npcs);
	Delete(voblist_items);

	zINFO(9,"U: (oCWorld) Destruction"); // 298
}

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

void oCWorld::SaveWorld(zSTRING const& fileName, zCWorld::zTWorldSaveMode saveMode, int writeBinary, int saveLevelMesh)
{
	zINFO(9,"U: (oCWorld::SaveWorld) " + fileName); // 401, _ulf\\oWorld.cpp
	if ( wayNet )
		wayNet->ClearVobDependencies();
	return zCWorld::SaveWorld(fileName, saveMode, writeBinary, saveLevelMesh);
}
	
int oCWorld::LoadWorld(zSTRING const& fileName, zCWorld::zTWorldLoadMode mode)
{
	zINFO(9,"U: (oCWorld::LoadWorld) " + fileName); // 420, _ulf/oWorld.cpp

	auto fname = fileName;
	fname.Upper();

	if ( mode != 1 ) {
		worldFilename = fname;

		zFILE_FILE file(worldFilename);

		worldName = file.GetFilename();
	}

	if ( fname.Search(0, ".3DS", 1u) <= 0 ) {
		zoptions->ChangeDir(DIR_WORLD);
		return zCWorld::LoadWorld(fname, mode);
	}

	auto compo = new zCVobLevelCompo();
	compo->SetVobName("Level-Vob");
	compo->SetVisual(fname);
	AddVob(compo);
	Release(compo);

	return 1;
}
