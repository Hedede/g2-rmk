const int zCWorld_DIMENSION = 3; //achwas
const int zCWorld_VobHashTable_Offset = 600; //0x0258
const int VOB_HASHTABLE_SIZE = 2048;

class zTTraceRayReport  {
	zBOOL      foundHit;
	zCVob*     foundVob;
	zCPolygon* foundPoly;
	zVEC3      foundIntersection;
	zVEC3      foundPolyNormal;
	zCVertex*  foundVertex;
};


class zCBspTree {
	zCBspNode* actNodePtr;  //nur beim Aufbau interessant
	zCBspLeaf* actLeafPtr;  //nur beim Aufbau interessant

	zCBspBase* bspRoot;
	zCMesh*    mesh;
	zCPolygon**treePolyList;
	zCBspNode* nodeList;
	zCBspLeaf* leafList;
	int        numNodes;
	int        numLeafs;
	int        numPolys;

	zCArray<zCVob*>         renderVobList;    //Im letzten Frame gerendert
	zCArray<zCVobLight*>    renderLightList;  //Im letzten Frame gerendert

	zCArray<zCBspSector*>   sectorList;       //im letzten Frame gerendert
	zCArray<zCPolygon*>     portalList;

	enum zTBspTreeMode      {
		zBSP_MODE_INDOOR,
		zBSP_MODE_OUTDOOR
	};

	zTBspTreeMode     bspTreeMode;
	zTWld_RenderMode  worldRenderMode;
	zREAL             vobFarClipZ;
	zTPlane           vobFarPlane[4];
	int               vobFarPlaneSignbits;
	zBOOL             drawVobBBox3D;
	int               leafsRendered;
	int               vobsRendered;
	zBOOL             renderedFirstTime;
	zTFrameCtr        masterFrameCtr;

	//nur beim Aufbau interessant
	zCPolygon**   actPolyPtr;
	zBOOL         compiled;
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

	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);
	virtual void MergeVobSubtree(zSTRING const &,zCVob *,zCWorld::zTWorldLoadMergeMode);
	virtual void SaveVobSubtree(zSTRING const &,zCVob *,int,int);
	virtual void DisposeWorld();
	virtual int DisposeVobs(zCTree<zCVob>* vobNode);
	virtual int DisposeVobsDbg(zCTree<zCVob>* vobNode);
	virtual void DisposeStaticWorld();

	virtual void AddVobAsChild(zCVob *,zCTree<zCVob> *);

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

	virtual void MoveVobSubtreeTo(zCVob *,zCTree<zCVob> *);
	virtual zCPlayerGroup* GetPlayerGroup()
	{
		return nullptr;
	}

	virtual zCVob* SearchVob(zCVob* vob, zCTree<zCVob>* vobNode = nullptr);
	virtual void SearchVobByID(unsigned vobId, zCTree<zCVob>* vobNode = nullptr);
	virtual zCVob* SearchVobByName(zSTRING const &);
	virtual void SearchVobListByName(zSTRING const &,zCArray<zCVob *> &);
	virtual void SearchVobListByClass(zCClassDef *,zCArray<zCVob *> &,zCTree<zCVob> *);
	virtual void SearchVobListByBaseClass(zCClassDef *,zCArray<zCVob *> &,zCTree<zCVob> *);
	virtual void VobAddedToWorld(zCVob *);
	virtual void VobRemovedFromWorld(zCVob *);
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

	void UpdateZone(zCZone* zone)
	{
		zoneBoxSorter.Update(zone);
	}

	void SetVobFarClipZScalability(float val)
	{
		vobFarClipZScalability = val;
	}

	void TraverseVobTree(zCVobCallback& callback, void* callbackData, zCTree<zCVob*> vobNode);

	void UpdateVobTreeBspDepencdencies(zCTree<zCVob>* vobNode);

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


	void UnregisterPerFrameCallback(zCWorldPerFrameCallback *callback)
	{
		perFrameCallbackList.Remove(callback);
	}

	zCZone* SearchZoneDefaultByClass(zCClassDef* classDef);

private:
	static unsigned GetVobHashIndex(zSTRING const& name)
	{
		return zCChecksum::GetBufferCRC32(name.Data(), name.Length(), 0) & 2047;
	}

	static unsigned GetVobHashIndex(zCVob* vob)
	{
		return GetVobHashIndex(vob->GetObjectName());
	}

	static int ActiveZoneListCompare(const void* a1, const void *a2);

	void ResetCutscenePlayer();

	void RemoveAllZones();

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
	//Mit "array", "numAlloc" und "numInArray" also 3*VOB_HASHTABLE_SIZE Wörter.
	//Der Lexer erlaubt keine so großen Arrays, daher ist meine Deklaration semantischer Unsinn.
	//Wer mit der Hashtabelle arbeiten will, muss selbst die Offsetrechnung betreiben.
	//Siehe MEM_SearchVobByName für Benutzung.

	zSTRING worldFilename;   //zSTRING Pfad des aktuellen Levels
	zSTRING worldName;       //zSTRING Name des aktuellen Levels
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

void zCWorld::SetOwnerSession(zCSession* owner)
{
	ownerSession = owner;
	if ( owner ) {
		csPlayer = owner->GetCutsceneManager()->CreateCutscenePlayer(this);
	} else {
		Release(csPlayer);
	}
}


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

int zCWorld::TraverseVobTree(zCVobCallback *callback, void *callbackData, zCTree<zCVob> *vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	for (auto i = vobNode->firstChild; i; i = i->next)
		TraverseVobTree(callback, callbackData, result);

	return callback->HandleVob(callback, vobNode->Data, callbackData);
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
		vob->RemoveVobFromBspTree(v4);

		bspTree.AddVob(vob);
	}

	for (auto i = vobNode->firstChild; i; i = i->next)
		UpdateVobTreeBspDependencies(i);
}

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

		zSTRING fileName = arc.GetFile()->GetFilename();

		zINFO(5, "D: WORLD: Saving BSP: " + fileName);

		zCFileBIN fileBin;
		fileBin.BinSetFile(arcFile);

		bspTree.SaveBIN(fileBin);

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

int zCWorld::ActiveZoneListCompare(const void* a1, const void *a2)
{
	auto zone1 = *static_cast<zCZone* const*>(a1);
	auto zone2 = *static_cast<zCZone* const*>(a2);

	if (zone1->GetZoneMotherClass() >= zone2->GetZoneMotherClass())
		return zone2->GetZoneMotherClass() < zone1->GetZoneMotherClass();
	return -1;
}
