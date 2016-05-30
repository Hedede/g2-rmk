
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
	zCBspNode* actNodePtr;           ////nur beim Aufbau interessant
	zCBspLeaf* actLeafPtr;           ////nur beim Aufbau interessant

	zCBspBase* bspRoot;
	zCMesh*    mesh;
	zCPolygon**treePolyList;
	zCBspNode* nodeList;
	zCBspLeaf* leafList;
	int        numNodes;
	int        numLeafs;
	int        numPolys;

	zCArray<zCVob*>         renderVobList;    //Im letzten Frame gerendert
	zCArray<zCVobLight*>        renderLightList;        //Im letzten Frame gerendert

	zCArray<zCBspSector*>       sectorList; //im letzten Frame gerendert
	zCArray<zCPolygon*>     portalList;

	enum zTBspTreeMode      {
		zBSP_MODE_INDOOR,
		zBSP_MODE_OUTDOOR
	};

	zTBspTreeMode     bspTreeMode;         //
	zTWld_RenderMode  worldRenderMode;     //
	zREAL             vobFarClipZ;         //
	zTPlane           vobFarPlane[4];      //
	int               vobFarPlaneSignbits; //
	zBOOL             drawVobBBox3D;       //
	int               leafsRendered;       //
	int               vobsRendered;        //
	zBOOL             m_bRenderedFirstTime;//
	zTFrameCtr        masterFrameCtr;      //
	//nur beim Aufbau interessant
	zCPolygon**   actPolyPtr;          //
	zBOOL         compiled;            //
};

class zCWorld : zCObject {
	Z_OBJECT(zCWorld);
private:

	zBOOL showTextureStats;
	zBOOL s_bWaveAnisEnabled;
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

	virtual ~zCWorld();

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);
	virtual void MergeVobSubtree(zSTRING const &,zCVob *,zCWorld::zTWorldLoadMergeMode);
	virtual void SaveVobSubtree(zSTRING const &,zCVob *,int,int);
	virtual void DisposeWorld(void);
	virtual void DisposeVobs(zCTree<zCVob> *);
	virtual void DisposeVobsDbg(zCTree<zCVob> *);
	virtual void DisposeStaticWorld(void);
	virtual void AddVobAsChild(zCVob *,zCTree<zCVob> *);
	virtual void RemoveVob(zCVob *);
	virtual void RemoveVobSubtree(zCVob *);
	virtual void MoveVobSubtreeTo(zCVob *,zCTree<zCVob> *);
	virtual void GetPlayerGroup(void);
	virtual void SearchVob(zCVob *,zCTree<zCVob> *);
	virtual void SearchVobByID(ulong,zCTree<zCVob> *);
	virtual zCVob* SearchVobByName(zSTRING const &);
	virtual void SearchVobListByName(zSTRING const &,zCArray<zCVob *> &);
	virtual void SearchVobListByClass(zCClassDef *,zCArray<zCVob *> &,zCTree<zCVob> *);
	virtual void SearchVobListByBaseClass(zCClassDef *,zCArray<zCVob *> &,zCTree<zCVob> *);
	virtual void VobAddedToWorld(zCVob *);
	virtual void VobRemovedFromWorld(zCVob *);
	virtual void RenderWaynet(zCCamera *);

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

	zCViewProgressBar* GetProgressBar()
	{
		return progressBar;
	}


	void SetOwnerSession(zCSession* owner);
	zCSession* GetOwnerSession()
	{
		return ownerSession;
	}

	zCPlayerGroup* GetPlayerGroup()
	{
		return nullptr;
	}

	void UpdateZone(zCZone* zone)
	{
		zoneBoxSorter.Update(zone);
	}

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

	zCSession*       ownerSession;
	zCCSPlayer*      csPlayer;

	zSTRING          levelName;
	zBOOL            compiled;
	zBOOL            compiledEditorMode;
	zBOOL            traceRayIgnoreVobFlag;
	zBOOL            isInventoryWorld;
	zTWld_RenderMode worldRenderMode;
	zCWayNet*        wayNet;
	zTFrameCtr       masterFrameCtr;
	zREAL            vobFarClipZ;
	zREAL            vobFarClipZScalability;

	zCArray<zCVob*>               traceRayVobList;
	zCArray<zCVob*>               traceRayTempIgnoreVobList;

	zBOOL             renderingFirstTime;
	zBOOL             showWaynet;
	zBOOL             showTraceRayLines;

	zCViewProgressBar* progressBar;

	zDWORD unarchiveFileLen;
	zDWORD unarchiveStartPosVobtree;

	int numVobsInWorld;

	zCArray<zCWorldPerFrameCallback*> perFrameCallbackList;

	//Der Outdoorskycontroller ist der interessante
	//Hat eine Outdoorwelt einen Indoorskycontroller für Portalräume?
	zCSkyControler* skyControlerIndoor;
	zCSkyControler* skyControlerOutdoor;
	zCSkyControler* activeSkyControler;

	//Defaut-Zonen sind am Anfang der Liste
	zCArray<zCZone*>          zoneGlobalList;
	zCArraySort<zCZone*>      zoneActiveList;

	zCArraySort<zCZone*>      zoneLastClassList;

	//Drei Handle-Listen, jeweils nach einer Koordinate sortiert.
	//Um aktive Zonen für eine Welt durch Schnittmenge dreier Array Abschnitte zu bestimmen. (?)
	zCVobBBox3DSorter<zCZone> zoneBoxSorter;

	//Um die zur Zeit relevante Menge von Zonen einzugrenzen?
	zCVobBBox3DSorter<zCZone>::zTBoxSortHandle    zoneActiveHandle;

	zBOOL addZonesToWorld;
	zBOOL showZonesDebugInfo;

	//--------------------------------------
	// Binary Space Partitioning Tree
	//--------------------------------------

	//"construction" Bsp. Was tut der?
	zCCBspTree* cbspTree;
	//Hier der eigentliche bsp Tree:
	zCBspTree   bspTree;

	zCArray<zCVob*>               activeVobList;  //aktive Vobs (Physik / AI)
	zCArray<zCVob*>               walkList; // wird im jedem Frame als Kopie der activeVobList gesetzt. Dann bekommt jedes Objekt in der Liste die Gelegenheit seinen Kram zu erledigen.
	zCArray<zCVob*>               vobHashTable[VOB_HASHTABLE_SIZE];               // for fast vob searching by name
	//Mit "array", "numAlloc" und "numInArray" also 3*VOB_HASHTABLE_SIZE Wörter.
	//Der Lexer erlaubt keine so großen Arrays, daher ist meine Deklaration semantischer Unsinn.
	//Wer mit der Hashtabelle arbeiten will, muss selbst die Offsetrechnung betreiben.
	//Siehe MEM_SearchVobByName für Benutzung.

	zSTRING worldFilename;   //zSTRING Pfad des aktuellen Levels
	zSTRING worldName;       //zSTRING Name des aktuellen Levels
};
