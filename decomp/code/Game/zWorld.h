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
	uint32_t polyNormal       : 1; // 0x80
	uint32_t ignoreTranspPoly : 1; // 0x100
	uint32_t findWaterPoly    : 1; // 0x200
	uint32_t poly2Sided       : 1; // 0x400
	uint32_t ignoreCharacter  : 1; // 0x800
	uint32_t firstHit         : 1; // 0x1000
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

	virtual int LoadWorld(zSTRING const& filename, zTWorldLoadMode mode);
	virtual int SaveWorld(zSTRING const& filename, zTWorldSaveMode mode, int writeBinary, int saveLevelMesh);

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

	void CompileWorld(zTBspTreeMode const& bspMode, float optimize, int removeLevelCompos, int editorMode, zCArray<zCPolygon *>* leakPolyList);

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
	int  TraverseBspAddVobsToMesh(zCCBspTree* bspTree, zCTree<zCVob> *node);

	void UpdateVobTreeBspDepencdencies(zCTree<zCVob>* vobNode);

	void PrintActiveVobs();
	void PrintGlobalVobTree(zCTree<zCVob>* node, int indent);
	void ShowZonesDebugInfo();
	void ShowTextureStats();
	void PrintStatus();

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

	static void GetPhongNormal(zCPolygon* poly, zVEC3 const& pos);
	static int GetSurfaceLightmapBBox2D(zCArray<zCPolygon>& surface, zTBBox2D& lmBox, int* realDim);
	void LightPatchMap(zCPatchMap* patchMap);
	void LightWorldStaticCompiled();
	void LightWorldStaticUncompiled(zCTree<zCVob>* node);
	void MakeVobLightingDirty();
	void MakeTransfers();
	zCPatchMap* GeneratePatchMapFromSurface(zCArray<zCPolygon *>& surface)
	void GenerateSurfaces(int doRayTracing, zTBBox3D* updateBBox3D);
	void GenerateLightmapFromPatchMap(zCPatchMap* patchMap);
	void GenerateLightmapsRadiosity(zTBBox3D* bbox);
	void GenerateLightmaps(zTStaticWorldLightMode const& lightMode, zTBBox3D* updateBBox3D);
	void GenerateStaticVertexLighting();
	void GenerateStaticWorldLighting(zTStaticWorldLightMode const& lightMode, zTBBox3D *updateBBox3D);

	static int ActiveZoneListCompare(const void* a1, const void *a2);

	void ResetCutscenePlayer();

	void RemoveAllZones();
	void AddZone(zCZone* zone);
	void RemoveZone(zCZone* zone);
	void ProcessZones();

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
