class oCWorld : public zCWorld {
	Z_OBJECT(oCWorld);
public:
	oCWorld();
	~oCWorld() override;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	int LoadWorld(zSTRING const& fileName, zCWorld::zTWorldLoadMode loadMode) override;
	void SaveWorld(zSTRING const& fileName, zCWorld::zTWorldSaveMode saveMode, int writeBinary, int saveLevelMesh) override;

	void DisposeWorld() override;


	zCTree<zCVob>* AddVobAsChild(zCVob * childVob,zCTree<zCVob> * tree) override;
	void RemoveVob(zCVob* vob) override;
	zCVob* SearchVob(zCVob* vob, zCTree<zCVob> * vobNode) override;
	zCVob* SearchVobByID(ulong,zCTree<zCVob> *) override;
	zCVob* SearchVobByName(zSTRING const &) override;
	zCVob* SearchVobListByName(zSTRING const &,zCArray<zCVob *> &) override;

	virtual zCVob* CreateVob(zTVobType type, int instanceId);
	zCVob* CreateVob(zTVobType type, zSTRING const& name);

	virtual void InsertVobInWorld(zCVob* vob);

	virtual void EnableVob(zCVob* vob, zCVob * parent);
	virtual void DisableVob(zCVob* vob);
	virtual void TraverseVobList(zCVobCallback& cb, void* cb_data);
	void ClearNpcPerceptionVobLists();


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

//------------------------------------------------------------------------------
//Gothic/_ulf/oWorld.cpp
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

zCVob* oCWorld::CreateVob(zTVobType type, int instanceId)
{
	zINFO(10, "U: (oCWorld::CreateVob) "_s + instanceId); // 315, oWorld.cpp

	zCVob* vob = nullptr;
	if ( type == VOB_TYPE_ITEM ) {
		auto ofactory = dynamic_cast<oCObjectFactory*>(zfactory);
		vob = ofactory->CreateItem(instanceId);
	}
	if ( type == VOB_TYPE_NPC ) {
		auto ofactory = dynamic_cast<oCObjectFactory*>(zfactory);
		vob = ofactory->CreateNpc(instanceId);
	}
	return vob;
}

zCVob* oCWorld::CreateVob(zTVobType type, zSTRING const& name)
{
	zINFO( 10, "U: (oCWorld::CreateVob) " + name); // 307 in g2, 281 in g1demo
	int instanceId = zparser.GetIndex(name);
	if ( instanceId < 0 )
		zFAULT("U: OBJ: Script-object " + name + " not found ! Please correct instancename !"); // 309, 283
	return CreateVob(type, instanceId);
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

zCTree<zCVob> * oCWorld::AddVobAsChild(zCVob * childVob, zCTree<zCVob> * tree)
{
	if ( !childVob )
		return 0;
	InsertVobInWorld(childVob);
	auto node = AddVobAsChild(childVob, tree);
	zINFO(10, "U: (oCWorld::AddVobAsChild) " + childVob->GetObjectName() + " (" + 1 + ")"); // 466
	return node;
}

void oCWorld::RemoveVob(zCVob *vob)
{
	if ( vob ) {
		zINFO( "U: (oCWorld::RemoveVob) " + vob->GetObjectName() + " (" + 1 + ")"); // 475
		RemoveFromLists(vob);
		if ( vob->homeWorld )
			zCWorld::RemoveVob(vob);
	}
}

void oCWorld::InsertVobInWorld(zCVob* vob)
{
	if (vob)
		InsertInLists(vob);
}

void oCWorld::EnableVob(zCVob *vob, zCVob *papa)
// 'papa' is the name in g1demo executable
{
	if ( !vob->homeWorld ) {
		zINFO( 9, "U: (oCWorld::EnableVob) " + vob->GetObjectName() ); // 496
		if (auto npc = dynamic_cast<oCNpc*>(vob))
			vob->globalVobTreeNode = 0;
		if ( parent )
			zCWorld::AddVobAsChild(vob, parent);
		else
			zCWorld::AddVobAsChild(vob, &globalVobTree);
		vob->SetSleeping(0);
	}
}

void oCWorld::DisableVob(zCVob *vob)
{
	if ( vob->homeWorld ) {
		voblist.RemoveOrder( vob );

		zINFO("U: (oCWorld::DisableVob) " + vob->GetObjectName() ); // 525
		zCWorld::RemoveVobSubtree(vob);
		vob->SetSleeping(1);
	}
}

zCVob* oCWorld::SearchVob(zCVob *vob, zCTree<zCVob> * vobNode)
{
	zCVob *result = zCWorld::SearchVob(vob, vobNode);
	if ( !result && !vobNode )
		result = voblist? voblist->Search( vob ) : nullptr;
	return result;
}

zCVob* oCWorld::SearchVobByID(unsigned int vobId, zCTree<zCVob> * vobNode)
{
	zCVob *result = zCWorld::SearchVobByID(vobId, vobNode);
	if ( !result && !vobNode )
		// pseudocode
		result = voblist? voblist->Search( vobId ) : nullptr;
	return result;
}

zCVob* oCWorld::SearchVobByName(zSTRING const& vobName)
{
	zCVob *result = zCWorld::SearchVobByName(vobName);
	if ( !result )
		// pseudocode
		result = voblist? voblist->Search( vobName ) : nullptr;
	return result;
}

void oCWorld::SearchVobListByName(zSTRING const& name, zCArray<zCVob *>& resultVobList)
{
	zCWorld::SearchVobListByName( name, resultVobList );
	if (!voblist)
		return;
	for (zCVob* vob : voblist)
		if (vob->GetObjectName() == name)
			resultVobList.InsertEnd( vob );
}


void oCWorld::InsertInLists(zCVob *vob)
{
	if (!vob)
		return;

	if ( !CheckInheritance(&oCVob::classDef, vob) )
		return;

	if (!voblist.Search(vob)) {
		voblist.InsertSort( vob );
		vob->AddRef();
	}

	if (auto npc = zDYNAMIC_CAST<oCNpc>(vob))
	{
		if (!voblist_npcs.Search(npc)) {
			voblist_npcs.InsertSort( npc );
			npc->AddRef();
			return;
		}
	}

	if (auto item = zDYNAMIC_CAST<oCItem>(vob))
	{
		if (!voblist_items.Search(item)) {
			voblist_items.InsertSort( item );
			item->AddRef();
		}
	}
}

void oCWorld::TraverseVobList(zCVobCallback& cb, void* cb_data)
{
	for (auto vob : voblist)
		if (vob)
			cb->HandleVob(vob, cb_data);
}

void oCWorld::ClearNpcPerceptionVobLists()
{
	for (auto npc : voblist_npcs) {
		if (!npc->IsSelfPlayer()) {
			npc->ClearPerceptionLists();
			npc->SetEnemy(0);
		}
	}
}

void oCWorld::RemoveFromLists(zCVob *vob)
{
	if ( vob->refDtr <= 0 ) {
		zWARNING( "RemoveFromList(vob) called from vob destructor!\n" ); // 673
		return;
	}

	vob->AddRef();

	if ( !CheckInheritance(&oCVob::classDef, vob) )
		return;

	if (voblist.Search(vob)) {
		voblist.RemoveOrder( vob );
		vob->Release();
	}

	// Really inefficient, traverses the linked list twice
	// I have no idea how this code was originally structured
	// why does check 'else' branch if npc isn't found in the list?
	// it is already established that it is npc, it can't be item
	// it would even look simpler without that jump, am I missing something?
	auto npc = zDYNAMIC_CAST<oCNpc>(vob);
	if (npc && voblist_npcs.Search(npc)) {
		voblist_npcs.RemoveOrder( npc );
		npc->CleanUp();
		npc->Release();
	} else if (auto item = zDYNAMIC_CAST<oCItem>(vob)) {
		if (voblist_items.Search(item)) {
			voblist_items.RemoveOrder( item );
			item->Release();
		}
	}

	vob->Release();
}

void oCWorld::DisposeVobs()
{
	auto list = &voblist_npcs;
	for ( auto i = list->next; i; i = list->next ) {
		auto npc = i->Get();

		npc->CleanUp();
		npc->DestroySpellBook();
		npc->RemoveFromAllSlots(0);
		npc->Release();

		voblist_npcs->Remove( npc );
	}

	list = &voblist_items;
	for ( auto i = list->next; i; i = list->next ) { {
		auto item = i->Get();
		list->Remove( item );
		item->Release();
	}

	list = &voblist;
	for ( auto i = list->next; i; i = list->next ) {
		auto vob = i->Get();
		list->Remove( vob );
		vob->Release();
	}
	zCWorld::DisposeVobs(0);
}

void oCWorld::DisposeWorld()
{
	DisposeVobs();
	zCWorld::DisposeWorld();
}

//------------------------------------------------------------------------------
int CompareVobsByID(zCVob* vob1, zCVob* vob2)
{
	return a - b;
	//return vob1->GetVobID() - vob2->GetVobID();
}

int CompareItemsByID(oCItem *vob1, oCItem *vob2)
{
	return CompareVobsByID(vob1, vob2);
}

int CompareNpcsByID(oCNpc *vob1, oCNpc *vob2)
{
	return CompareVobsByID(vob1, vob2);
}


