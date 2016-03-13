//--------------------------------------
// Timer (technisch und spieltechnisch)
//--------------------------------------


const int oCWorldTimer_TicksPerHour		  = 250000;
const int oCWorldTimer_TicksPerMin_approx = 4167; //< 1 sec / Tag daneben

class oCWorldTimer {
	//250000 Ticks pro Stunde
	zREAL worldTime;    //zREAL   
	int day;          //int      
};


const int zCWorld_DIMENSION = 3; //achwas
const int zCWorld_VobHashTable_Offset = 600; //0x0258
const int VOB_HASHTABLE_SIZE = 2048;

class zCTree {
	zCTree<T>* parent;
	zCTree<T>* firstChild;
	zCTree<T>* next;
	zCTree<T>* prev;
	T*         data;
};

class zTTraceRayReport  {
	zBOOL      foundHit;               //   
	zCVob*     foundVob;               //       
	zCPolygon* foundPoly;              //
	zVEC3      foundIntersection;   //
	zVEC3      foundPolyNormal;     //
	zCVertex*  foundVertex;            //
};


class zCVobBBox3DSorter<zCZone> {
public:
	virtual ~zCVobBBox3DSorter();
	class zTBoxSortHandle  {
		void *vtbl;                  //Pointer to Method table
		zCBBox3DSorterBase* zoneActiveHandle_mySorter;              //
		zTBBox3D bbox3D;

		int zoneActiveHandle_indexBegin [zCWorld_DIMENSION]; //int
		int zoneActiveHandle_indexEnd   [zCWorld_DIMENSION]; //int

		zCArray<VOB*> activeList;
	};
	zCArray<zTBoxSortHandle *>        handles;
	zCArraySort<zTNode*> nodeList[zCWorld_DIMENSION];
	zBOOL zoneBoxSorter_sorted;                       //zBOOL
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
public:
	virtual void zCClassDef& GetClassDef();
	virtual void ~zCWorld(uint);
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
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
};

class oWorld : zCObject {
	zCTree<zCVob>     globalVobTree;  //Jedes (?) Vob in der Welt ist hier drin.

	enum                zTWorldLoadMode         {
		zWLD_LOAD_GAME_STARTUP,         
		zWLD_LOAD_GAME_SAVED_DYN,       
		zWLD_LOAD_GAME_SAVED_STAT,      
		zWLD_LOAD_EDITOR_COMPILED,      
		zWLD_LOAD_EDITOR_UNCOMPILED,    
		zWLD_LOAD_MERGE 
	};
	enum                zTWorldSaveMode         {
		zWLD_SAVE_GAME_SAVED_DYN,       
		zWLD_SAVE_EDITOR_COMPILED,      
		zWLD_SAVE_EDITOR_UNCOMPILED,    
		zWLD_SAVE_COMPILED_ONLY         
	};
	enum                zTWorldLoadMergeMode    {
		zWLD_LOAD_MERGE_ADD,                    
		zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL 
	};

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

	//Sonstige Daten:
	/*0x0060*/  zCSession*       ownerSession;      //
	/*0x0064*/  zCCSPlayer*      csPlayer;          //

	/*0x0068*/  zSTRING m_strlevelName; //zSTRING                    
	/*0x007C*/  zBOOL            compiled;
	/*0x0080*/  zBOOL            compiledEditorMode;
	/*0x0084*/  zBOOL            traceRayIgnoreVobFlag;
	/*0x0088*/  zBOOL            m_bIsInventoryWorld;
	/*0x008C*/  zTWld_RenderMode worldRenderMode;
	/*0x0090*/  zCWayNet*        wayNet;
	/*0x0094*/  zTFrameCtr       masterFrameCtr;
	/*0x0098*/  zREAL            vobFarClipZ;
	/*0x009C*/  zREAL            vobFarClipZScalability;

	zCArray<zCVob*>               traceRayVobList;
	zCArray<zCVob*>               traceRayTempIgnoreVobList;

	/*0x00B8*/  zBOOL             renderingFirstTime;         //         
	/*0x00BC*/  zBOOL             showWaynet;                 // 
	/*0x00C0*/  zBOOL             showTraceRayLines;          // 
	/*0x00C4*/  zCViewProgressBar*progressBar;                // 
	/*0x00C8*/  zDWORD            unarchiveFileLen;           //
	/*0x00CC*/  zDWORD            unarchiveStartPosVobtree;   //
	/*0x00D0*/  int               numVobsInWorld;             //              

	zCArray<zCWorldPerFrameCallback*> perFrameCallbackList;

	//Der Outdoorskycontroller ist der interessante
	//Hat eine Outdoorwelt einen Indoorskycontroller für Portalräume?
	/*0x00E0*/  zCSkyControler* skyControlerIndoor;        //
	/*0x00E4*/  zCSkyControler* skyControlerOutdoor;       //
	/*0x00E8*/  zCSkyControler* activeSkyControler;        //

	// zones                    
	zCArray<zCZone*>          zoneGlobalList;     //Defaut-Zonen sind am Anfang der Liste
	zCArraySort<zCZone*>      zoneActiveList;

	zCArraySort<zCZone*>      zoneLastClassList;

	//Drei Handle-Listen, jeweils nach einer Koordinate sortiert.
	//Um aktive Zonen für eine Welt durch Schnittmenge dreier Array Abschnitte zu bestimmen. (?)
	zCVobBBox3DSorter<zCZone> zoneBoxSorter;

	//Um die zur Zeit relevante Menge von Zonen einzugrenzen?
	zCVobBBox3DSorter<zCZone>::zTBoxSortHandle    zoneActiveHandle;

	/*0x01A0*/  zBOOL addZonesToWorld;                        //zBOOL
	/*0x01A4*/  zBOOL showZonesDebugInfo;                     //zBOOL

	//--------------------------------------
	// Binary Space Partitioning Tree
	//--------------------------------------

	zCCBspTree*cbspTree;                               //   //"construction" Bsp. Was tut der?
	zCBspTree bspTree;                    //Hier der eigentliche bsp Tree:

	zCArray<zCVob*>               activeVobList;  //aktive Vobs (Physik / AI)
	zCArray<zCVob*>               walkList; // wird im jedem Frame als Kopie der activeVobList gesetzt. Dann bekommt jedes Objekt in der Liste die Gelegenheit seinen Kram zu erledigen.
	zCArray<zCVob*>               vobHashTable[VOB_HASHTABLE_SIZE];               // for fast vob searching by name
	//Mit "array", "numAlloc" und "numInArray" also 3*VOB_HASHTABLE_SIZE Wörter.
	//Der Lexer erlaubt keine so großen Arrays, daher ist meine Deklaration semantischer Unsinn.
	//Wer mit der Hashtabelle arbeiten will, muss selbst die Offsetrechnung betreiben.
	//Siehe MEM_SearchVobByName für Benutzung.

	zSTRING worldFilename;   //zSTRING Pfad des aktuellen Levels
	zSTRING worldName;       //zSTRING Name des aktuellen Levels

	//nicht ausprobiert, aber hoffentlich ist der Name Programm.
	//wie hier sortiert ist weiß ich nicht.
	zCListSort<zCVob>*   voblist;            //
	zCListSort<oCNpc>*   voblist_npcs;       //
	zCListSort<oCItem>*  voblist_items;      //
};
