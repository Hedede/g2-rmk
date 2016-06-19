const int zCWorld_DIMENSION = 3; //achwas
const int zCWorld_VobHashTable_Offset = 600; //0x0258
const int VOB_HASHTABLE_SIZE = 2048;

// preliminary analysis
struct TraceFlags {
	uint32_t onlyDynamic      : 1; // 1
	uint32_t ignoreVobs       : 1; // 2
	uint32_t unk              : 2; // 4, 8
	uint32_t ignoreStatPoly   : 1; // 0x10
	uint32_t findStatic       : 1; // 0x20
	uint32_t findPortals      : 1; // 0x40
	uint32_t ignoreTranspPoly : 1; // 0x100
	uint32_t findWaterPoly    : 1; // 0x200
	uint32_t poly2Sided       : 1; // 0x400
	uint32_t ignoreCharacter  : 1; // 0x800
	uint32_t ignoreVisual     : 1; // 0x2000
	uint32_t ignoreProjetiles : 1; // 0x4000
};

class zTTraceRayReport  {
	zBOOL      foundHit;
	zCVob*     foundVob;
	zCPolygon* foundPoly;
	zVEC3      foundIntersection;
	zVEC3      foundPolyNormal;
	zCVertex*  foundVertex;
};


class zCWorld : zCObject {
	Z_OBJECT(zCWorld);
private:
	zBOOL showTextureStats;
	zBOOL s_bWaveAnisEnabled;
	zBOOL s_bFadeOutFarVerts;
public:
	static void SetShowTextureStats(int b)
	{
		zCWorld::showTextureStats = b;
	}

	static zBOOL GetShowTextureStats()
	{
		return showTextureStats;
	}

	static void SetWaveAnisEnabled(int b)
	{
		zCWorld::s_bWaveAnisEnabled = b;
	}

	static zBOOL GetWaveAnisEnabled()
	{
		return s_bWaveAnisEnabled;
	}

	static void AdvanceClock(float delta)
	{
		if ( delta >= 0.0 ) {
			ztimer.SetFrameTime(delta * 1000.0);
		} else {
			ztimer.ResetTimer();
		}
	} 

	zCWorld();
	virtual ~zCWorld();

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void ArcTraverseVobs(zCArchiver *arc, zCTree<zCVob>* node, int root);
	void UnarcTraverseVobs(zCArchiver *arc, zCTree<zCVob>* node);


	int LoadBspTree(zFILE& file, int isDyn);
	void SaveBspTree(zFILE& file);
	void SaveBspTreeMesh3DS(zSTRING* filename);

	void DebugMarkOccluderPolys();

	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);
	virtual void MergeVobSubtree(zSTRING const& fileName, zCVob* paramVob, zTWorldLoadMergeMode mergeMode);
	virtual void SaveVobSubtree(zSTRING const& fileName, zCVob* vob, int writeBinary, int saveLevelMesh);

	virtual void DisposeWorld();
	virtual int DisposeVobs(zCTree<zCVob>* vobNode);
	virtual int DisposeVobsDbg(zCTree<zCVob>* vobNode);
	virtual void DisposeStaticWorld();

	virtual zCTree<zCVob>* AddVobAsChild(zCVob* vob, zCTree<zCVob>* tree);


	virtual void RemoveVob(zCVob* vob)
	{
		if ( vob && vob->homeWorld == this ) {
			VobRemovedFromWorld(vob);
			vob->RemoveWorldDependencies(1);
		}
	}
	virtual void RemoveVobSubtree(zCVob* vob)
	{
		if ( vob ) {
			MoveVobSubtreeTo(vob, 0);
			RemoveVobSubtree_r(vob->globalVobTreeNode, 1);
		}
	}

	void RemoveVob(zCTree<zCVob>* node)
	{
		if (node)
			RemoveVob(node->GetData());
	}

	void RemoveVobSubtree(zCTree<zCVob>* node)
	{
		if (node)
			RemoveVobSubtree(node->GetData());
	}

	void RemoveVobFromLists(zCVob* vob);

	virtual void MoveVobSubtreeTo(zCVob *,zCTree<zCVob> *);
	virtual zCPlayerGroup* GetPlayerGroup()
	{
		return nullptr;
	}

	virtual zCVob* SearchVob(zCVob* vob, zCTree<zCVob>* vobNode = nullptr);
	virtual zCVob* SearchVobByID(unsigned vobId, zCTree<zCVob>* vobNode = nullptr);
	virtual zCVob* SearchVobByName(zSTRING const& name)
	{
		return SearchVobHashTable(name);
	}

	virtual void SearchVobListByName(zSTRING const& name, zCArray<zCVob*>& result)
	{
		SearchVobListHashTable(name, result);
	}
	virtual void SearchVobListByClass(zCClassDef* classDef, zCArray<zCVob*>& result, zCTree<zCVob>* vobNode);
	virtual void SearchVobListByBaseClass(zCClassDef* classDef, zCArray<zCVob*>& result, zCTree<zCVob>* vobNode);

	virtual void VobAddedToWorld(zCVob* vob)
	{
		InsertVobHashTable(vob);
		vob->ThisVobAddedToWorld(this);
		++numVobsInWorld;
	}

	virtual void VobRemovedFromWorld(zCVob* vob)
	{
		RemoveVobFromLists(vob);
		--numVobsInWorld;
	}
	virtual void RenderWaynet(zCCamera *cam);
	void Render(zCCamera* cam);

	enum zTWorldLoadMode {
		zWLD_LOAD_GAME_STARTUP,
		zWLD_LOAD_GAME_SAVED_DYN,
		zWLD_LOAD_GAME_SAVED_STAT,
		zWLD_LOAD_EDITOR_COMPILED,
		zWLD_LOAD_EDITOR_UNCOMPILED,
		zWLD_LOAD_MERGE
	};

	enum zTWorldSaveMode {
		zWLD_SAVE_GAME_SAVED_DYN,
		zWLD_SAVE_EDITOR_COMPILED,
		zWLD_SAVE_EDITOR_UNCOMPILED,
		zWLD_SAVE_COMPILED_ONLY
	};

	enum zTWorldLoadMergeMode {
		zWLD_LOAD_MERGE_ADD,
		zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL
	};

	zCBspTree& GetBspTree()
	{
		return bspTree;
	}

	zCSkyControler* GetActiveSkyControler()
	{
		if ( bspTree.bspTreeMode == 0) {
			activeSkyControler = skyControlerIndoor;
		} else {
			activeSkyControler = skyControlerOutdoor;
		}
		return activeSkyControler;
	}

	void SetSkyControlerOutdoor(zCSkyControler* ctr);
	{
		if (skyControlerOutdoor != ctr) {
			Release(skyControlerOutdoor);
			skyControlerOutdoor = ctr;
		}
	}

	void SetSkyControlerIndoor(zCSkyControler* ctr);
	{
		if (skyControlerIndoor != ctr) {
			Release(skyControlerIndoor);
			skyControlerIndoor = ctr;
		}
	}

	zCViewProgressBar* GetProgressBar()
	{
		return progressBar;
	}


	void SetOwnerSession(zCSession* owner);
	zCSession* GetOwnerSession()
	{
		return ownerSession;
	}

	void SetVobFarClipZScalability(float val)
	{
		vobFarClipZScalability = val;
	}

	void TraverseVobTree(zCVobCallback& callback, void* callbackData, zCTree<zCVob*> vobNode);
	void TraverseBsp(zCCBspTree *bsp, VobTree *node, int removeLevelCompos);

	void UpdateVobTreeBspDepencdencies(zCTree<zCVob>* vobNode);

	void PrintActiveVobs();

	int ShouldAddThisVobToBsp(zCVob *vob)
	{
		if (zDYNAMIC_CAST<zCZone>(vob))
			return addZonesToWorld;

		return true;
	}

	void AddVob(zCVob* vob)
	{
		AddVobAsChild(vob, &globalVobTree);
	}

	void AddVobAsChild(zCVob* vob, zCVob* parent)
	{
		if ( parent ) {
			auto tree = parent->globalVobTreeNode;
			if ( tree )
				AddVobAsChild(vob, tree);
		}
	}

	void MoveVobSubtreeTo(zCVob *vob, zCTree<zCVob>* vobNode);

	void MoveVobSubtreeTo(zCVob* vob, zCVob* parent)
	{
		if ( parent ) {
			auto tree = parent->globalVobTreeNode;
			if ( tree )
				MoveVobSubtreeTo(vob, tree);
		}
	}

	void MoveVobSubtreeToWorldSpace(zCVob *vob)
	{
		MoveVobSubtreeTo(vob, &globalVobTree);
	}

	void MoveVobs();

	void InsertVobHashTable(zCVob* vob);
	void RemoveVobHashTable(zCVob* vob);

	void RegisterPerFrameCallback(zCWorldPerFrameCallback* callback)
	{
		for (auto cb : perFrameCallbackList)
			if (cb == callback)
				return;

		perFrameCallbackList.Insert(callback);
	}

	void UnregisterPerFrameCallback(zCWorldPerFrameCallback *callback)
	{
		perFrameCallbackList.Remove(callback);
	}

	void UpdateZone(zCZone* zone)
	{
		zoneBoxSorter.Update(zone);
	}

	zCZone* SearchZoneDefaultByClass(zCClassDef* classDef);
	void SearchZoneListByClass(zCClassDef* classDef, zCArray<zCZone*>& result);

	int TraceRayNearestHit(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, int traceFlags);
	int TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, int traceFlags);

	int TraceRayNearestHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags);
	int TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags);

	int TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache);
	int TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache);

	int TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache);
	int TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache);

	int LightingTestRay(zVEC3 const& start, zVEC3 const& end, zVEC3& inters, zCPolygon*& hitPoly);

private:
	static unsigned GetVobHashIndex(zSTRING const& name)
	{
		return zCChecksum::GetBufferCRC32(name.Data(), name.Length(), 0) & 2047;
	}

	static unsigned GetVobHashIndex(zCVob* vob)
	{
		return GetVobHashIndex(vob->GetObjectName());
	}

	zCVob* SearchVobHashTable(zSTRING const& name);
	void SearchVobListHashTable(zSTRING const& name, zCArray<zCVob *>& result);

	void RemoveVobSubtree_r(zCTree<zCVob>* node, int firstVob);
	void MakeVobLightingDirty();
	void GenerateStaticVertexLighting();

	static int ActiveZoneListCompare(const void* a1, const void *a2);

	void ResetCutscenePlayer();

	void RemoveAllZones();
	void AddZone(zCZone* zone);
	void RemoveZone(zCZone* zone);

private:
	//Jedes (?) Vob in der Welt ist hier drin.
	zCTree<zCVob>     globalVobTree;

	zTTraceRayReport  traceRayReport;

	enum zTStaticWorldLightMode {
		zWLD_LIGHT_VERTLIGHT_ONLY,
		zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_LOW_QUAL,
		zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_MID_QUAL,
		zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_HI_QUAL
	};

	enum zTWld_RenderMode {
		zWLD_RENDER_MODE_VERT_LIGHT,
		zWLD_RENDER_MODE_LIGHTMAPS
	};

	zCSession*       ownerSession = nullptr;
	zCCSPlayer*      csPlayer     = nullptr;

	zSTRING          levelName;
	zBOOL            compiled = false;
	zBOOL            compiledEditorMode = false;
	zBOOL            traceRayIgnoreVobFlag = false;
	zBOOL            isInventoryWorld = false;
	zTWld_RenderMode worldRenderMode = zWLD_RENDER_MODE_LIGHTMAPS;
	zCWayNet*        wayNet;
	zTFrameCtr       masterFrameCtr = 0;
	zREAL            vobFarClipZ            = 5000.0;
	zREAL            vobFarClipZScalability = 1.0;

	zCArray<zCVob*>  traceRayVobList;
	zCArray<zCVob*>  traceRayTempIgnoreVobList;

	zBOOL            renderingFirstTime = true;
	zBOOL            showWaynet         = false;
	zBOOL            showTraceRayLines  = false;

	zCViewProgressBar* progressBar = nullptr;

	zDWORD unarchiveFileLen         = 0;
	zDWORD unarchiveStartPosVobtree = 0;

	int numVobsInWorld = 0;

	zCArray<zCWorldPerFrameCallback*> perFrameCallbackList;

	//Der Outdoorskycontroller ist der interessante
	//Hat eine Outdoorwelt einen Indoorskycontroller für Portalräume?
	zCSkyControler* skyControlerIndoor  = nullptr;
	zCSkyControler* skyControlerOutdoor = nullptr;
	zCSkyControler* activeSkyControler  = nullptr;

	//Defaut-Zonen sind am Anfang der Liste
	zCArray<zCZone*>          zoneGlobalList;
	zCArraySort<zCZone*>      zoneActiveList;

	zCArraySort<zCZone*>      zoneLastClassList;

	//Drei Handle-Listen, jeweils nach einer Koordinate sortiert.
	//Um aktive Zonen für eine Welt durch Schnittmenge dreier Array Abschnitte zu bestimmen. (?)
	zCVobBBox3DSorter<zCZone> zoneBoxSorter;

	//Um die zur Zeit relevante Menge von Zonen einzugrenzen?
	zCVobBBox3DSorter<zCZone>::zTBoxSortHandle    zoneActiveHandle;

	zBOOL addZonesToWorld    = false;
	zBOOL showZonesDebugInfo = false;

	//--------------------------------------
	// Binary Space Partitioning Tree
	//--------------------------------------

	//"construction" Bsp. Was tut der?
	zCCBspTree* cbspTree = nullptr;
	//Hier der eigentliche bsp Tree:
	zCBspTree   bspTree;

	//aktive Vobs (Physik / AI)
	zCArray<zCVob*>               activeVobList;
	// wird im jedem Frame als Kopie der activeVobList gesetzt. Dann bekommt jedes Objekt in der Liste die Gelegenheit seinen Kram zu erledigen.
	zCArray<zCVob*>               walkList;
	// for fast vob searching by name
	zCArray<zCVob*>               vobHashTable[VOB_HASHTABLE_SIZE];
};

// ------------------- CPP --------------------------
zCWorld::zCWorld()
	: zCObject(), activeVobList(128)
{
	wayNet = new zCWayNet(this);

	SetSkyControlerIndoor(new zCSkyControler_Indoor());
	SetSkyControlerOutdoor(new zCSkyControler_Outdoor());

	GetActiveSkyControler();

	bspTreeMode.worldRenderMode = worldRenderMode;
	zoneBoxSorter.InsertHandle(zoneActiveHandle);

	if (zVobFarClipZScale <= 0.0) {
		zVobFarClipZScale = zoptions->ReadReal("ENGINE", "zVobFarClipZScale", 4.0);
		zClamp(zVobFarClipZScale, 0.0001, 9999999.0);

		vobFarClipZScalability = zVobFarClipZScale * 0.22142857 + 0.4;

		s_bFadeOutFarVerts = zoptions->ReadBool("ENGINE", "zFarClipAlphaFade", 1);
		s_bWaveAnisEnabled = zoptions->ReadBool("ENGINE", "zWaterAniEnabled", 1);
	}
}

zCWorld::~zCWorld()
{
	DisposeWorld();
	Release(wayNet);
	Release(csPlayer);
	Release(skyControlerIndoor);
	Release(skyControlerOutdoor);
	zCPolygon::S_DeleteMorphedVerts();
	Delete(cbspTree);
}

void zCWorld::SetOwnerSession(zCSession* owner)
{
	ownerSession = owner;
	if ( owner ) {
		csPlayer = owner->GetCutsceneManager()->CreateCutscenePlayer(this);
	} else {
		Release(csPlayer);
	}
}

// ---------- HashTable
zCVob* zCWorld::SearchVobHashTable(zSTRING const& name)
{
	if (name.IsEmpty())
		return nullptr;

	auto hash = GetVobHashIndex(name);
	auto bucket = vobHashTable[hash];

	for (auto vob : bucket) {
		if (vob->GetObjectName() == name)
			return vob;

	return nullptr;
}

void zCWorld::SearchVobListHashTable(zSTRING const& name, zCArray<zCVob *>& result)
{
	if (name.IsEmpty())
		return;

	auto hash = GetVobHashIndex(name);
	auto bucket = vobHashTable[hash];

	for (auto vob : bucket)
		if (vob->GetObjectName() == name)
			result.Insert(vob);
}

void zCWorld::InsertVobHashTable(zCVob *vob)
{
	auto vobName = vob->GetObjectName();
	if (vobName) {
		auto bucket = vobHashTable[GetVobHashIndex(vob)];
		bucket.Insert(vob);
	}
}

void zCWorld::RemoveVobHashTable(zCVob *vob)
{
	auto vobName = vob->GetObjectName();
	if (vobName) {
		auto bucket = vobHashTable[GetVobHashIndex(vob)];
		bucket.Remove(vob);
	}
}


// --------- Search
zCVob* zCWorld::SearchVob(zCVob* vob, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &globalVobTree;

	if ( vobNode->data == vob )
		return vob;

	zCVob* result = nullptr;
	for (auto i = vobNode->firstChild; i; i = i->next) {
		result = SearchVob(vob, i);
		if (result)
			break;
	}

	return result;
}

zCVob* zCWorld::SearchVobByID(unsigned vobId, VobTree *vobNode)
{
	if ( !vobNode ) {
		if (!vobId)
			return nullptr;

		vobNode = &globalVobTree;
	}

	zCVob* result = vobNode->data;
	if ( result && vobId == 1 )
		return result;

	result = nullptr;
	for (auto i = vobNode->firstChild; i; i = i->next) {
		result = SearchVobByID(vobId, i);
		if (result)
			break;
	}

	return result;
}

void zCWorld::SearchVobListByClass(zCClassDef* classDef, zCArray<zCVob *>& result, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if (vob->GetClassDef() == classDef)
		result.Insert(vob);

	for (auto i = vobNode->firstChild; i; i = i->next) {
		SearchVobListByClass(classDef, result, i);
}

void zCWorld::SearchVobListByBaseClass(zCClassDef* classDef, zCArray<zCVob *>& result, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if (zCObject::CheckInheritance(vob->GetClassDef(), classDef))
		result.Insert(vob);

	for (auto i = vobNode->firstChild; i; i = i->next) {
		SearchVobListByClass(classDef, result, i);
}

// --------- Traversal
int zCWorld::TraverseVobTree(zCVobCallback *callback, void *callbackData, zCTree<zCVob> *vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	for (auto i = vobNode->firstChild; i; i = i->next)
		TraverseVobTree(callback, callbackData, result);

	return callback->HandleVob(callback, vobNode->Data, callbackData);
}

int zCWorld::TraverseBsp(zCCBspTree *bsp, VobTree *node, int removeLevelCompos)
{
	if ( !node )
		return 0;

	auto vob = node->data;
	if (auto compo = dynamic_cast<zCVobLevelCompo*>(vob)) {
		// visual->GetClassDef() ==  &zCMesh::classDef )
		if (auto mesh = dynamic_cast<zCMesh*>(vob->visual))
			bsp->AddMesh(bsp, mesh, vob->trafoObjToWorld);
	}

	node = node->firstChild;
	while ( node ) {
		if ( TraverseBsp(bsp, node, removeLevelCompos) == 1 )
			node = node->firstChild;
		else
			node = node->next;
	}

	if (auto compo = dynamic_cast<zCVobLevelCompo*>(vob)) {
		if ( removeLevelCompos ) {
			RemoveVob(compo);
			return 1;
		}
		compo->flags1.showVisual = false;
	}

	return 0;
}

void MoveVobs()
{
	/* walkList.Resize(activeVobList.GetNumInList());
	for (unsigned i = 0; i < activeVobList.GetNumInList(); ++i)
		walkList[i] = activeVobList[i];*/
	walkList = activeVobList;

	for (auto vob : walkList) {
		if (vob)
			vob->DoFrameActivity();
	}

	walkList.DeleteList();
}

void zCWorld::MoveVobSubtreeTo(zCVob *vob, zCTree<zCVob>* vobNode)
{
	if ( vob ) {
		if ( vob->globalVobTreeNode )
			vob->globalVobTreeNode->RemoveSubtree();
		if ( vobNode )
			vobNode->AddChild(vob->globalVobTreeNode);
		else
			vob->globalVobTreeNode->parent = 0;

		auto inMovement = vob->isInMovementMode;
		if ( !inMovement )
			vob->BeginMovement();
		vob->TouchMovement();
		vob->Move(0.0, 0.0, 0.0);
		if ( !inMovement )
			vob->EndMovement(1);
	}
}


void zCWorld::UpdateVobTreeBspDependencies(VobTree *vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if ( vob ) {
		vob->groundPoly = 0;
		vob->CalcGroundPoly();

		vob->vobLeafList.DeleteList();
		vob->RemoveVobFromBspTree();

		bspTree.AddVob(vob);
	}

	for (auto i = vobNode->firstChild; i; i = i->next)
		UpdateVobTreeBspDependencies(i);
}

void zCWorld::PrintActiveVobs()
{
	zINFO("D: *** World: ActiveVobList ************************************");

	unsigned i = 0;
	for (auto vob : activeVobList) {
		auto info = vob->GetVobInfo();

		zINFO("D: " + (i++) + ": " + info);
	}
}

// --------- Save/Load
int zCWorld::MergeVobSubtree(zSTRING const& fileName, zCVob* paramVob, zTWorldLoadMergeMode mergeMode)
{
	zCWorld::s_worldMergeMode = mergeMode;
	if ( mergeMode ) {
		if ( !paramVob )
			return 0;
		s_firstVobMergeVobSubtree = paramVob;
		s_unarcTraverseVobsFirstParent = 0;
	} else {
		zCTree<zCVob>* tree = nullptr;
		if (paramVob)
			tree = paramVob->globalVobTreeNode;
		if (!tree)
			tree = &globalVobTree;

		s_firstVobMergeVobSubtree = 0;
		s_unarcTraverseVobsFirstParent = tree;
	}

	zCWorld::s_worldLoadMode = zWLD_LOAD_MERGE;
	zoptions->ChangeDir(DIR_WORLD);

	auto arc = zCArchiverFactory::CreateArchiverRead(fileName, 0);
	if ( arc ) {
		arc->ReadObject(this);
		arc->Close();
		Release(arc);
	}

	s_unarcTraverseVobsFirstParent = 0;
	return std::exchange(s_firstVobMergeVobSubtree, 0);
}

int zCWorld::SaveVobSubtree(zSTRING *fileName, zCVob *vob, int writeBinary, int saveLevelMesh)
{
	if ( vob && vob->homeWorld == this ) {
		zCWorld::s_worldSaveMode = 2;

		s_saveLevelMesh = saveLevelMesh;
		s_firstVobSaveWorld = vob->globalVobTreeNode;
		s_saveWayNet = 0;

		zoptions->ChangeDir(DIR_WORLD);

		auto arc = zarcFactory.CreateArchiverWrite(fileName, writeBinary ? 3 : 1, 0, 0);
		if ( arc ) {
			arc->WriteObject(this);
			arc->Close();
			arc->Release();
		}

		s_firstVobSaveWorld = 0;

		return arc != 0;
	}

	return 0;
}

int zCWorld::LoadBspTree(zFILE& file, int isDyn)
{
	zCFileBIN fileBin;
	fileBin.BinSetFile(file);

	auto load = bspTree.LoadBIN(fileBin, isDyn);

	if ( !isDyn ) {
		if ( bspTree.mesh ) {
			if ( bspTree.mesh->hasLightmaps ) {
				worldRenderMode = 1;
				bspTree.worldRenderMode = 1;
			} else {
				worldRenderMode = 0;
				bspTree.worldRenderMode = 0;
			}
		}
		compiled = load;
	}
	return load;
}

void zCWorld::SaveBspTree(zCFile& file)
{
	zSTRING fileName = file.GetFilename();

	zINFO(5, "D: WORLD: Saving BSP: " + fileName);

	zCFileBIN fileBin;
	fileBin.BinSetFile(arcFile);

	bspTree.SaveBIN(fileBin);
}

void zCWorld::SaveBspTreeMesh3DS(zSTRING *filename)
{
	zoptions->ChangeDir(DIR_MESHES);
	zCFile3DS::Save3DS(filename, bspTree.mesh);
}


// ---------- Dispose
void zCWorld::ResetCutscenePlayer();
{
	if ( csPlayer )
		csPlayer->ResetCutscenePlayer();

	Release(csPlayer);

	if (ownerSession) {
		auto csman = ownerSession->GetCutsceneManager();
		csPlayer = csman->CreateCutscenePlayer(this);
	}
}

void zCWorld::RemoveAllZones()
{
	zoneBoxSorter.Clear();
	for (auto zone : zoneGlobalList)
		zone->ThisVobRemovedFromWorld(this);

	zoneActiveList.DeleteList();
	zoneLastClassList.DeleteList();
	zoneGlobalList.DeleteList();
	zoneBoxSorter.Clear();
}

int zCWorld::DisposeVobs(zCTree<zCVob> *vobNode)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs();

	if ( !vobNode ) {
		Release(wayNet);
		wayNet = new zCWayNet(this);
		ResetCutscenePlayer();
		RemoveAllZones();

		vobNode = &this->globalVobTree;
		renderingFirstTime = 1;
	}

	for (auto i = vobNode->firstChild; i;) {
		if (DisposeVobs(i) == 1)
			i = vobNode->firstChild;
		else
			i = i->next;
	}

	if ( vobNode->data ) {
		RemoveVob(vobNode->data);
		return 1;
	}

	return 0;
}

int zCWorld::DisposeVobsDbg(zCTree<zCVob> *vobNode)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs();

	if ( !vobNode ) {
		Release(wayNet);
		wayNet = new zCWayNet(this);
		ResetCutscenePlayer();
		RemoveAllZones();

		vobNode = &this->globalVobTree;
		renderingFirstTime = 1;
	}

	for (auto i = vobNode->firstChild; i;) {
		if (DisposeVobsDbg(i) == 1)
			i = vobNode->firstChild;
		else
			i = i->next;
	}

	if ( vobNode->data ) {
		RemoveVob(vobNode->data);
		return 1;
	}

	return 0;
}

void zCWorld::DisposeStaticWorld()
{
	if ( bspTree.numPolys != 0 ) {
		auto skyctrl = zDYNAMIC_CAST<zCSkyControler_Outdoor>(zCSkyControler::s_activeSkyControler);
		if (skyctrl)
			skyctrl->levelChanged = true;
	}

	zCPolygon::S_DeleteMorphedVerts();
	bspTree.DisposeTree();

	compiled = 0;
	compiledEditorMode = 0;
	renderingFirstTime = 1;

	UpdateVobTreeBspDependencies(&globalVobTree);
}

void zCWorld::DisposeWorld()
{
	zINFO(8, "D: WORLD: Disposing zCWorld.."); // 1674, _dieter\\zWorld.cpp

	DisposeVobs(0);
	DisposeStaticWorld();

	zINFO(8, "D: WORLD: ..zCWorld disposed."); // 1682,
}


// ---------- Zones
int zCWorld::ActiveZoneListCompare(const void* a1, const void *a2)
{
	auto zone1 = *static_cast<zCZone* const*>(a1);
	auto zone2 = *static_cast<zCZone* const*>(a2);

	if (zone1->GetZoneMotherClass() >= zone2->GetZoneMotherClass())
		return zone2->GetZoneMotherClass() < zone1->GetZoneMotherClass();
	return -1;
}

zCZone* SearchZoneDefaultByClass(zCClassDef* cd)
{
	for (auto zone : zoneGlobalList) {
		if (zone->GetClassDef() == cd)
			return zone;
		if (zone->GetClassDef() == zone->GetDefaultZoneClass())
			continue;
	}

	return nullptr;
}

void zCWorld::SearchZoneListByClass(zCClassDef *classDef, zCArray<zCZone*>& results)
{
	for (auto zone : zoneGlobalList) {
		if (zone->GetClassDef() == classDef)
			results.Insert(zone);
	}
}

void zCWorld::RemoveZone(zCZone *zone)
{
	if ( zone ) {
		auto defclass = zone->GetDefaultZoneClass();
		auto theclass = zone->GetClassDef();
		if ( theclass == defclass); {
			zoneGlobalList.RemoveOrder(zone);
		} else {
			zoneGlobalList.Remove(zone);
			zoneBoxSorter.Remove(zone);
		}
	
		zoneLastClassList.Remove(zone);
		zoneActiveList.Remove(zone);
	}
}

void zCWorld::AddZone(zCZone* zone)
{
	if ( zone ) {
		if (!zoneGlobalList.IsInList(zone)) {
			auto defclass = zone->GetDefaultZoneClass();
			auto theclass = zone->GetClassDef();

			if (defclass == theclass) {
				zoneGlobalList.InsertFront(zone);
			} else {
				zoneGlobalList.InsertEnd(zone);
				zoneBoxSorter.Insert(zone);
			}
		}
	}
}


// ---------- Render
void zCWorld::RenderWaynet(zCCamera *cam)
{
	if ( showWaynet ) {
		if ( wayNet )
			wayNet->Draw(cam);
	}
}

void zCWorld::Render(zCCamera *cam)
{
	if (!cam)
		return;
	if (!cam->connectedVob)
		return;

	if ( cam->connectedVob->homeWorld != this)
		return;

	int hasLevelName = 0;
	if (!levelName.IsEmpty()) {
		hasLevelName = 1;
	} else if (auto oworld = zDYNAMIC_CAST<oCWorld>(this)) {
		hasLevelName = !oworld->worldFilename.IsEmpty();
	}

	zCCacheBase::S_PurgeCaches();

	if ( hasLevelName && zCWorld::s_bWaveAnisEnabled )
		zCFFT::S_SetFrameCtr(masterFrameCtr);

	cam->PreRenderProcessing();
	cam->UpdateViewport();
	cam->Activate();

	GetActiveSkyControler();

	if ( renderingFirstTime ) {
		zresMan->cacheInImmedMsec = 1500.0;
		renderingFirstTime = 0;
	}

	if ( hasLevelName ) {
		zsound->SetListener(zCCamera::activeCam->connectedVob);
		ProcessZones();
		zsound->DoSoundUpdate();
	}

	zCPolygon::PrepareRendering();

	bspTree.vobFarClipZ = vobFarClipZScalability * vobFarClipZ;
	bspTree.Render(/*cam*/);
	cam->PostRenderProcessing();
	zrenderer->FlushPolys();

	zCPolygon::S_ResetMorphedVerts();

	if ( hasLevelName ) {
		zlineCache.Flush(cam->targetView);
		RenderWaynet(cam);
		ShowZonesDebugInfo();
		ShowTextureStats();
	}

	cam->Activate();

	MoveVobs();

	if ( hasLevelName ) {
		zCEventManager::DoFrameActivity();
		zresMan->DoFrameActivity();

		if ( csPlayer )
			csPlayer->Process();

		for (auto callback : perFrameCallbackList)
			callback(this, cam);
	}

	++masterFrameCtr;
	zCVertex::ResetVertexTransforms();
	++zCTexAniCtrl::masterFrameCtr;
}

// ----------- Ray tracing
int zCWorld::TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, int traceFlags)
{
	auto end = start + ray;
	if ( showTraceRayLines )
		zlineCache.Line3D(start, end, GFX_LBLUE, 0);

	traceRayVobList.DeleteList();

	traceRayReport = zTTraceRayReport{};

	zCArray<zCVob*>* traceRayList = nullptr;
	if (!traceFlags.ignoreVobs)
		traceRayList = &traceRayVobList;

	traceRayReport.foundHit = bspTree.TraceRay(
	                start, end, traceFlags | 0x1000,
	                &traceRayReport.foundIntersection,
	                &traceRayReport.foundPoly,
	                &traceRayList);

	if ( traceFlags.ignoreStatPoly )
		traceRayReport.foundHit = 0;

	if ( traceRayReport.foundHit ) {
		if ( traceFlags < 0 ) {
			auto foundPoly = traceRayReport.foundPoly;
			if ( foundPoly )
				traceRayReport.foundPolyNormal = foundPoly.polyPlane.normal;
		}
		return 1;
	}

	if (traceFlags.ignoreVobs)
		return 0;

	for (auto vob : traceRayVobList) {
		bool hasVisual =  vob->visual && vob->flags1.showVisual;
		bool ignVisual = !vob->visual && traceFlags.ignoreVisual;

		if (!(hasVisual || ignVisual))
			continue;
		if (!traceRayIgnoreVobFlag && vob->flags1.ignoredByTraceRay)
			continue;
		if (ignoreList && ignoreList->IsInList(vob))
			continue;
		if (traceFlags.ignoreCharacter && vob->GetCharacterClass())
			continue;
		if (traceFlags.ignoreProjectiles && vob->GetIsProjectile())
			continue;
		if (traceFlags.requireDynamic && !vob->GetCollDetDyn())
			continue;

		return vob->TraceRay(start, ray, traceFlags, &traceRayReport);
	}

	return 0;
}

int zCWorld::TraceRayNearestHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags)
{
	if ( !ignoreVob )
		return TraceRayNearestHit(start, ray, (zCArray<zCVob>*)nullptr, traceFlags);

	traceRayTempIgnoreVobList.Insert(ignoreVob);

	auto result = TraceRayNearestHit(start, ray, &traceRayTempIgnoreVobList, traceFlags);
	traceRayTempIgnoreVobList.DeleteList();
	return result;
}

int zCWorld::TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags)
{
	if ( !ignoreVob )
		return TraceRayFirstHit(start, ray, (zCArray<zCVob>*)nullptr, traceFlags);

	traceRayTempIgnoreVobList.Insert(ignoreVob);

	auto result = TraceRayFirstHit(start, ray, &traceRayTempIgnoreVobList, traceFlags);
	traceRayTempIgnoreVobList.DeleteList();
	return result;
}

void ShowTraceRayLine(zVEC3 const& start, zVEC3 const& ray)
{
	// I made this function to reduce duplicate code
	if ( showTraceRayLines ) {
		int effort = zCRayCache::LastTraceRayEffort() * 512.0;
		if ( effort > 255 )
			effort = 255;

		zCOLOR color = (GFX_RED * effort) / 256 + GFX_DGREEN;

		zVEC3 end = start + ray;
		zlineCache.Line3D(start, end, color, 0);
	}
}

int zCWorld::TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayNearestHit(start, ray, ignoreList, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayFirstHit(start, ray, ignoreList, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayNearestHit(start, ray, ignoreVob, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayFirstHit(start, ray, ignoreVob, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

// ----------- Lighting
int zCWorld::LightingTestRay(zVEC3 const& start, zVEC3 const& end, zVEC3& inters, zCPolygon*& hitPoly)
{
	float junk;

	if ( traceRayLastPoly ) {
		zVEC3 ray = end = start;
		if (traceRayLastPoly->CheckRayPolyIntersection(start, ray, inters, &junk)) {
			*hitPoly = traceRayLastPoly;
			return 1;
		}
		traceRayLastPoly = 0;
	}

	if ( bspTree.TraceRay(start, end, 0x120, inters, hitPoly, 0) ) {
		traceRayLastPoly = *hitPoly;
		return 1;
	}

	return 0;
}

void TraverseMakeVobLightingDirty(zCTree<zCVob>* tree)
{
	auto vob = tree->GetData();
	if (vob) {
		vob->flags1.lightColorStatDirty = true;
		vob->flags1.lightColorDynDirty = true;
	}

	for (auto i = tree->firstChild; i; i = i->next)
		TraverseMakeVobLightingDirty(i);
}

void zCWorld::MakeVobLightingDirty()
{
	zINFO(7,"D: WLD: Make Vob-Lighting dirty"); // 1922,

	TraverseMakeVobLightingDirty(&globalVobTree);

	zINFO("D: WLD: ... finished"); // 1924, _dieter\\zWorldLight.cpp
}

void TraverseCollectLights(VobTree *node)
{
	auto vob = node->GetData();

	if (vob && vob->type == VOB_TYPE_LIGHT)
		if (static_cast<zCVobLight*>(vob)->lightData.flags.isStatic)
			lightVobList.Insert(vob);
	for (auto i = node->firstChild; i; i = i->next )
		TraverseCollectLights(i);
}

void zCWorld::GenerateStaticVertexLighting()
{
	zINFO("D: WORLD: Calculating static Vertex Light for world ...");

	bspTree.mesh->CreateListsFromArrays();

	TraverseCollectLights(&globalVobTree);

	int numLights = lightVobList.GetNumInList();

	zINFO("D: WORLD: NumLights: "_s + numLights);

	if ( compiled )
		LightWorldStaticCompiled();
	else
		LightWorldStaticUncompiled(&globalVobTree);

	lightVobList.DeleteList();

	zINFO("D: WORLD: ... Finished.");
}

// ----------- Debug
void zCWorld::DebugMarkOccluderPolys()
{
	if ( compiled ) {
		auto mat = new zCMaterial("Occluder_Poly_Mat");

		zCOLOR color{50,50,-1};
		bspTree.mesh->CreateListsFromArrays();

		// for here is pseudocode,
		// range-based for is not possible, because
		// polyList is jsut a pointer
		for (auto poly : bspTree.mesh->polyList) {
			if (poly->flags.occluder && !(poly.flags.portalPoly))
				poly->SetMaterial(mat);
		}
	}
}

// ----------- Vob Subtree
zCTree<zCVob>* zCWorld::AddVobAsChild(zCVob *vob, zCTree<zCVob> *tree)
{
	if ( vob && tree && !vob->homeWorld ) {
		vob->homeWorld = this;
		if (auto zone = zDYNAMIC_CAST<zCZone>(vob))
			AddZone(zone);

		auto vobNode = vob->globalVobTreeNode;
		if ( vobNode ) {
			if ( !vobNode->parent )
				tree->AddChild(vobNode);

			for (auto i = vobNode->firstChild; i; i = i->next )
				AddVobAsChild(i->data, vobNode);
		} else {
			vob->globalVobTreeNode = tree->AddChild(vob);
		}

		vob->AddVobToWorld_CorrectParentDependencies();

		auto stat = vob->GetCollDetStat();
		auto dyn = vob->GetCollDetDyn();
		vob->SetCollDetStat(0);
		vob->SetCollDetDyn(0);
		vob->BeginMovement();
		vob->TouchMovement();
		vob->EndMovement(1);
		vob->SetCollDetDyn(dyn);
		vob->SetCollDetStat(stat);
		vob->AddRef();

		if (!zDYNAMIC_CAST<zCZone>(vob) || addZonesToWorld )
			bspTree.AddVob(vob);

		if ( vob->flags2.sleepingMode ) {
			if (!activeVobList.IsInList(vob))
				activeVobList.Insert(vob);
		}

		if ( vob->callback_ai )
			vob->callback_ai->HostVobAddedToWorld(vob, this);
		if ( vob->visual )
			vob->visual->HostVobAddedToWorld(v20, vob, this);
		VobAddedToWorld(vob);
		return vob->globalVobTreeNode;
	}
	return nullptr;
}

void zCWorld::RemoveVobSubtree_r(zCTree<zCVob>* node, int firstVob)
{
	if ( node ) {
		auto vob = node->GetData();
		if (vob) {
			VobRemovedFromWorld(vob);
			vob->AddRef();
			bool b = !node->firstChild() && firstVob;
			vob->RemoveWorldDependencies(b);
		}

		for (auto i = node->firstChild; i; i = i->next)
			RemoveVobSubtree_r(i, 0);
		if ( vob && vob->refCtr == 1 ) {
			auto vnode = vob->globalVobTreeNode;
			if (vnode && vnode->firstChild) {
				zFAULT("D: WLD: zCVobs leaking, uneven refCtrs in vobSubTree! (Removing vobSubtree from world; Release() deletes parentVob but not children"); // 2016
			}

			Release(vob);
		}
	}
}

void RemoveVobFromLists(zCVob* vob)
{
	bspTree.renderVobList.Remove(vob);
	walkList.Remove(vob);
	if (auto zone = zDYNAMIC_CAST<zCZone>(vob))
		RemoveZone(zone);
	RemoveVobHashTable(vob);
}

// ----------- Archive
void zCWorld::Archive(zCArchiver& arc)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs(this);

	auto arcFile = arc.GetFile();
	auto progressBar = GetProgressBar();

	if ( progressBar && arcFile )
		progressBar->SetPercent(0, "");

	auto saveMode = zCWorld::s_worldSaveMode;

	bool modeCompiled = (saveMode == zWLD_SAVE_COMPILED_ONLY || saveMode == zWLD_SAVE_EDITOR_COMPILED);
	bool compiled = this->compiled && modeCompiled;

	if (s_firstVobSaveWorld == &this->globalVobTree && compiled) {
		arc.WriteChunkStart("MeshAndBsp", 0);

		if ( !dword_9A4428 ) // zCWorld::s_inUnarc
			bspTree.mesh->ShareFeatures();

		SaveBspTree(arcFile);

		arc.WriteChunkEnd();
	}

	bool hasZones = zoneGlobalList.GetNumInList() > 0;
	bool firstVob = s_firstVobSaveWorld && (s_firstVobSaveWorld->data || s_firstVobSaveWorld->firstChild);
	bool modeNotOnlyCompiled = saveMode != zWLD_SAVE_COMPILED_ONLY;

	if ((hasZones || firstVob) && modeNotOnlyCompiled ) {
		if ( progressBar )
			progressBar->SetRange(10, 80);

		arc.WriteChunkStart("VobTree", 0);

		int tmp = arcNumChildCtr;
		arcNumChildCtr = 0;

		ArcTraverseVobs(s_firstVobSaveWorld, 1);

		arc.WriteChunkEnd();

		arcNumChildCtr = tmp;

		if ( progressBar )
			progressBar->ResetRange();
	}

	if ( arc.InSaveGame() && csPlayer ) {
		arc.WriteChunkStart("CutscenePlayer", 0);
		arc.WriteObject(csPlayer);
		arc.WriteChunkEnd();
	}

	if ( arc.InSaveGame() ) {
		arc.WriteChunkStart("SkyCtrl", 0);

		if ( bspTree.bspTreeMode > 0 )
			activeSkyControler = skyControlerOutdoor;
		else
			activeSkyControler = skyControlerIndoor;

		arc.WriteObject(activeSkyControler);

		arc.WriteChunkEnd();
	}

	if ( s_saveWayNet ) {
		arc.WriteChunkStart("WayNet", 0);
		arc.WriteObject(wayNet);
		arc.WriteChunkEnd();
	}

	arc.WriteChunkStart("EndMarker", 0);
	arc.WriteChunkEnd();

	if ( progressBar )
		progressBar->SetPercent(100, "");
}

void zCWorld::Unarchive(zCArchiver& arc)
{
	dword_9A4428 = 0;

	auto arcFile = arc.GetFile();
	auto progressBar = GetProgressBar();

	unarchiveFileLen = 0;
	unarchiveStartPosVobtree = 0;

	if ( progressBar && arcFile ) {
		unarchiveFileLen = arcFile->Size();
		progressBar->SetPercent(0, "");
	}

	auto loadMode = zCWorld::s_worldSaveMode;

	bool load_dyn = (loadMode == zWLD_LOAD_GAME_SAVED_DYN    ||
	                 loadMode == zWLD_LOAD_EDITOR_UNCOMPILED ||
	                 loadMode == zWLD_LOAD_MERGE);

	bool load_static = loadMode == zWLD_LOAD_GAME_SAVED_STAT;

	zSTRING chunkName;
	uint16_t version;

	auto loadMeshAndBsp = [&] () {
		if ( !load_dyn )
			CreateWorld();

		zCFileBIN fileBin;
		fileBin.BinSetFile(arcFile);

		bool bspLoaded = bspTree.LoadBIN(fileBin, load_dyn);

		if ( !load_dyn ) {
			auto polyList = bspTree.mesh;
			if ( polyList ) {
				if ( polyList->hasLightmaps ) {
					worldRenderMode = 1;
					bspTree.worldRenderMode = 1;
				} else {
					worldRenderMode = 0;
					bspTree.worldRenderMode = 0;
				}
			}

			compiled = bspLoaded;
		}

		if (load_static && !bspLoaded )
			zWARNING("D: loadMode==zWLD_LOAD_GAME_SAVED_STAT, but could not load static World"); // 2495
	};

	auto loadVobTree = [&] () {
		if (zCWorld::s_worldLoadMode != zWLD_LOAD_GAME_SAVED_DYN &&
		    !load_dyn && !compiled )
		{
			CreateWorld();
			zCWorld::s_worldLoadMode = zWLD_LOAD_EDITOR_UNCOMPILED;
		}

		if ( progressBar ) {
			if ( arcFile )
				unarchiveStartPosVobtree = arcFile->Pos();

			unarchiveFileLen -= unarchiveStartPosVobtree;
			progressBar->SetRange(10, 80);
		}

		zINFO(5 "D: WORLD: Loading VobTree ..");

		int tmp = arcNumChildCtr;
		arcNumChildCtr = 0;

		s_UnarcVobList.Clear();

		UnarcTraverseVobs(0);

		arc.SkipOpenChunk();

		arcNumChildCtr = tmp;

		if (progressBar)
			progressBar->ResetRange();


		for (auto vob : s_UnarcVobList)
			vob->PostLoad();

		s_UnarcVobList.Clear();
	};

	do {
		arc.ReadChunkStart(chunkName, version);

		if ( chunkName == "MeshAndBsp" ) {
			if ( arcFile )
				loadMeshAndBsp();
			arc.SkipOpenChunk();
		}

		if (chunkName == "VobTree") {
			if ( !load_static )
				loadVobTree;
			arc.SkipOpenChunk();
		}

		if (chunkName == "CutscenePlayer") {
			if ( csPlayer )
				csPlayer = arc.ReadObject(csPlayer);
			arc.SkipOpenChunk();
		}

		if (chunkName == "SkyCtrl") {
			if ( bspTree.bspTreeMode > 0 )
				activeSkyControler = skyControlerOutdoor;
			else
				activeSkyControler = skyControlerIndoor;

			activeSkyControler = arc.ReadObject(activeSkyControler);
			arc.SkipOpenChunk();
		}

		if (chunkName == "WayNet") {
			if ( !load_static )
				wayNet = arc.ReadObject(wayNet);
			arc.SkipOpenChunk();
		}
	} while (chunkName != "EndMarker");

	arc.SkipOpenChunk();

	if ( arc.GetCurrentChunkVersion() < 1u ) {
		if ( load_static ) {
			arc.ReadChunkStart(chunkName, version);
			arc.SkipOpenChunk();

			if ( !wayNet ) {
				wayNet = new zCWayNet(this);
			}
		} else {
			wayNet = arc.ReadObject(wayNet);
		}
	}

	if ( progressBar )
		progressBar->SetPercent(100, "");
}

void zCWorld::ArcTraverseVobs(zCArchiver *arc, zCTree<zCVob>* node, int root)
{
	if (!node )
		return;

	if ( world->progressBar && !(arcNumChildCtr & 7) ) {
		auto percent = arcNumChildCtr / (world->numVobsInWorld + 0.1) * 100.0;
		GetProgressBar()->SetPercent(percent, "");
	}

	unsigned numChilds = 0;
	for (auto i = node->firstChild; i; i = i->next) {
		if (!i->GetData()->GetDontWriteIntoArchive())
			++numChilds;
	}

	if ( root ) {
		int num = arcNumChildCtr++;
		
		auto str = "childs"_s + num;
		if ( node->GetData() ) {

			arc->WriteInt(str, 1);
			zASSERT(!node->GetData()->GetDontWriteIntoArchive()); //_dieter\\zWorld.cpp, 2773
		} else {
			arc->WriteInt(str, numChilds);
		}
	}

	auto obj = node->GetData();
	if ( obj && !obj->GetDontWriteIntoArchive()) {
		arc->WriteObject(obj);
		auto visual = obj->visual;
		if ( s_saveLevelMesh && obj->type == VOB_TYPE_LEVELCOMPO) {
			//if (visual && visual->GetClassDef() == &zCMesh::classDef) {
			if (auto mesh = zSTRICT_CAST<zCMesh>(visual)) { // madeupcast
				zoptions->ChangeDir(DIR_MESHES);
				zCFile3DS::Save3DS(visual->meshName, visual);
			}
		}

		int num = arcNumChildCtr++;
		arc->WriteInt(arc, "childs"_s + num, numChilds);
	}

	for (auto i = node->firstChild; i; i = i->next )
		world->ArcTraverseVobs(arc, i, 0);
}

void zCWorld::UnarcTraverseVobs(zCArchiver *arc, zCTree<zCVob> *node)
{
	if ( progressBar && !(arcNumChildCtr & 7) ) {
		auto file = arc->GetFile();
		auto pos = file->Pos() - unarchiveStartPosVobtree;
		float percent = pos / (unarchiveFileLen + 0.1) * 100.0
		GetProgressBar()->SetPercent(percent, "");
	}

	int ignoreVisuals = 0;
	zCTree<zCVob>* tree = nullptr;
	bool b = false;
	if ( node ) {
		if (s_worldLoadMode == zWLD_LOAD_GAME_STARTUP ||
		    s_worldLoadMode == zWLD_LOAD_GAME_SAVED_DYN1)
			ignoreVisuals = 1;
		zCVobLevelCompo::unarcIgnoreVisuals = ignoreVisuals;

		auto obj = arc->ReadObject(0);
		if ( obj ) {
			if ( s_worldLoadMode == zWLD_LOAD_MERGE ) {
				if ( s_worldMergeMode == zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL ) {
					s_worldMergeMode = zWLD_LOAD_MERGE_ADD;

					if ( !s_firstVobMergeVobSubtree )
						goto LABEL_50;

					s_firstVobMergeVobSubtree->SetVisual(obj->visual);

					obj->Release();
					obj = s_firstVobMergeVobSubtree;

					b = true;
				} else {
					zCVob::SetVobID(zCVob::GetNextFreeVobID());
					if ( !s_firstVobMergeVobSubtree )
						s_firstVobMergeVobSubtree = obj;
				}
			}

			auto compo = dynamic_cast<zCVobLevelCompo*>(obj);
			if (compo) {
				if (s_worldLoadMode == zWLD_LOAD_EDITOR_COMPILED ||
				    s_worldLoadMode == zWLD_LOAD_EDITOR_UNCOMPILED )
					obj->flags1.showVisual = 0;
			}

			if (b == 0) {
				if ( compo && ignoreVisuals ) {
					Release(obj);
					tree = node;
				} else {
					tree = AddVobAsChild(obj, node);
					if (!tree)
						tree = node;
					Release(obj);
				}
			} else {
				tree = obj->globalVobTreeNode;
			}

			if (obj) {
				if (obj->type = VOB_TYPE_LIGHT)
					obj->SetRange(obj->lightData.range, 1);

				s_unarcVobList.Insert(obj);
			}
		}
	} else if ( s_unarcTraverseVobsFirstParent ) {
		tree = s_unarcTraverseVobsFirstParent;
	} else {
		tree = &globalVobTree;
	}

LABEL_50:
	int numChilds = arc->ReadInt("childs"_s + arcNumChildCtr++);
	while (numChilds --> 0)
		UnarcTraverseVobs(arc, tree);
}
