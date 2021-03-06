#pragma once
#include <Gothic/Types/zTree.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zCObject.h>

struct zCBspNode;
struct zCBspLeaf;
struct zCBspBase;
struct zCBspSector;
struct zCMesh;
struct zCPolygon;
struct zCVertex;
struct zCCamera;
struct zCVob;
struct zCVobLight;

#include <Gothic/Types/zVEC3.h>
struct zTPlane {
	float distance;
	zVEC3 normal;
};

enum BspTreeMode {
	MODE_INDOOR,
	MODE_OUTDOOR
};

struct zCBspTree {
	void Render(zCCamera*)
	{
		Thiscall<void(zCBspTree*)> call{0x530080};
		call(this);
	}

	zCBspNode *actNodePtr;
	zCBspLeaf *actLeafPtr;
	zCBspBase *bspRoot;
	zCMesh *mesh;
	zCPolygon **treePolyList;
	zCBspNode *nodeList;
	zCBspLeaf *leafList;
	int numNodes;
	int numLeafs;
	int numPolys;
	zCArray<zCVob*> renderVobList;
	zCArray<zCVobLight*> renderLightList;
	zCArray<zCBspSector*> sectorList;
	zCArray<zCPolygon*> portalList;
	int bspTreeMode;
	int worldRenderMode;
	float vobFarClipZ;
	zTPlane vobFarPlane;
	int vobFarPlaneSignbits;
	int drawVobBBox3D;
	int leafsRendered;
	int vobsRendered;
	int renderedFirstTime;
	int masterFrameCtr;
	zCPolygon **actPolyPtr;
	int compiled;
};

struct zTTraceRayReport {
	int foundHit;
	zCVob *foundVob;
	zCPolygon *foundPoly;
	zVEC3 foundIntersection;
	zVEC3 foundPolyNormal;
	zCVertex *foundVertex;
};


struct zCWorld;
struct oCWorld;
struct zCSession;
struct zCCSPlayer;
struct zCWayNet;
struct zCSkyControler;
struct zCWorldPerFrameCallback;
struct zCViewProgressBar;
struct zCPlayerGroup;

enum zTWorldLoadMode : unsigned int{
	zWLD_LOAD_GAME_STARTUP,
	zWLD_LOAD_GAME_SAVED_DYN,
	zWLD_LOAD_GAME_SAVED_STAT,
	zWLD_LOAD_EDITOR_COMPILED,
	zWLD_LOAD_EDITOR_UNCOMPILED,
	zWLD_LOAD_MERGE
};

struct zCWorld_vt
{
	//zCWorld
	zCClassDef *(__thiscall *_GetClassDef)(zCWorld *);
	void (__thiscall *Archive)(zCWorld *, zCArchiver&);
	void (__thiscall *Unarchive)(zCWorld *, zCArchiver&);
	void (__thiscall *dtor)(zCWorld *, unsigned int);
	int (__thiscall *LoadWorld)(zCWorld*, const zSTRING&, zTWorldLoadMode);
	void (__thiscall *SaveWorld)(zCWorld *, const zSTRING *, unsigned int, int, int);
	zCVob *(__thiscall *MergeVobSubtree)(zCWorld *, const zSTRING *, zCVob *, int);
	void (__thiscall *SaveVobSubtree)(zCWorld *, const zSTRING *, zCVob *, int, int);
	void (__thiscall *DisposeWorld)(zCWorld *);
	int (__thiscall *DisposeVobs)(zCWorld *, zCTree<zCVob> *);
	int (__thiscall *DisposeVobsDbg)(zCWorld *, zCTree<zCVob> *);
	void (__thiscall *DisposeStaticWorld)(zCWorld *);
	int (__thiscall *AddVobAsChild)(zCWorld *, zCVob *, zCTree<zCVob> *);
	void (__thiscall *RemoveVob)(zCWorld *, zCVob *);
	void (__thiscall *RemoveVobSubtree)(zCWorld *, zCVob *);
	void (__thiscall *MoveVobSubtreeTo)(zCWorld *, zCVob *, zCTree<zCVob> *);
	zCPlayerGroup *(__thiscall *GetPlayerGroup)(zCWorld *);
	zCVob* (__thiscall *SearchVob)(zCWorld *, zCVob *, zCTree<zCVob> *);
	zCVob* (__thiscall *SearchVobByID)(zCWorld *, unsigned long, zCTree<zCVob> *);
	zCVob* (__thiscall *SearchVobByName)(zCWorld *, const zSTRING&);
	void (__thiscall *SearchVobListByName)(zCWorld*, const zSTRING&, zCArray<zCVob*>&);
	void (__thiscall *SearchVobListByClass)(zCWorld *, zCClassDef *, zCArray<zCVob*>&, zCTree<zCVob>*);
	void (__thiscall *SearchVobListByBaseClass)(zCWorld *, zCClassDef *, zCArray<zCVob*>&, zCTree<zCVob>*);
	void (__thiscall *VobAddedToWorld)(zCWorld *, zCVob *);
	void (__thiscall *VobRemovedFromWorld)(zCWorld *, zCVob *);
	void (__thiscall *RenderWaynet)(zCWorld *, zCCamera *);

	//oCWorld:
	zCVob *(__thiscall *CreateVob)(oCWorld *, int, int);
	void (__thiscall *InsertVobInWorld)(oCWorld *, zCVob *);
	void (__thiscall *EnableVob)(oCWorld *, zCVob *, zCVob *);
	void (__thiscall *DisableVob)(oCWorld *, zCVob *);
	void (__thiscall *TraverseVobList)(oCWorld *, void *, void *);
	void (__thiscall *DisposeVobs2)(oCWorld *);
};

struct zCWorld : zCObject {
	static zTWorldLoadMode& s_worldLoadMode;
	static zBOOL& s_bFadeOutFarVerts;

	zCSkyControler* GetActiveSkyControler()
	{
		return activeSkyControler;
	}

	void MoveVobs();

	void ProcessZones()
	{
		thiscall(0x6207F0, this);
	}

	void AddVob(zCVob* vob)
	{
		reinterpret_cast<zCWorld_vt*>(_vtab)->AddVobAsChild(this, vob, &globalVobTree);
	}

	zCVob* SearchVobByName(std::string const& name)
	{
		return reinterpret_cast<zCWorld_vt*>(_vtab)->SearchVobByName(this, name);
	}

	void DisposeWorld()
	{
		reinterpret_cast<zCWorld_vt*>(_vtab)->DisposeWorld(this);
	}

	void DisposeStaticWorld();
	void DisposeVobs(zCTree<zCVob>* root);

	bool LoadWorld(std::string_view levelpath, zTWorldLoadMode loadMode);


	zCTree<zCVob> globalVobTree;
	zTTraceRayReport traceRayReport;

	zCSession* ownerSession;
	zCCSPlayer *csPlayer;

	zSTRING levelName;

	int compiled;
	int compiledEditorMode;
	int traceRayIgnoreVobFlag;
	int isInventoryWorld;
	int worldRenderMode;

	zCWayNet *wayNet;

	int masterFrameCtr;
	float vobFarClipZ;
	float vobFarClipZScalability;

	zCArray<zCVob*> traceRayVobList;
	zCArray<zCVob*> traceRayTempIgnoreVobList;

	int renderingFirstTime;
	int showWaynet;
	int showTraceRayLines;

	zCViewProgressBar *progressBar;
	int unarchiveFileLen;
	int unarchiveStartPosVobtree;
	int numVobsInWorld;

	zCArray<zCWorldPerFrameCallback*> perFrameCallbackList;

	zCSkyControler *skyControlerIndoor;
	zCSkyControler *skyControlerOutdoor;
	zCSkyControler *activeSkyControler;

	char data1[192];

	zCBspTree bspTree;
	zCArray<zCVob*> activeVobList;
	zCArray<zCVob*> walkList;
	zCArray<zCVob*> vobHashTable[2048];
};

enum zTVobType {};
struct oCWorld : zCWorld {
	zCLASS_DECLARATION(oCWorld);
public:
	oCWorld();

	void Render(zCCamera* cam);
	bool HasLevelName();

	bool LoadWorld(std::string_view fileName, zTWorldLoadMode mode);

	zCVob* CreateVob(int vobType, int instanceId)
	{
		return reinterpret_cast<zCWorld_vt*>(_vtab)->CreateVob(this, vobType, instanceId);
	}

	zSTRING worldFilename;
	zSTRING worldName;
	char data2[12];
};

#include <Hook/size_checker.h>
CHECK_SIZE(zCWorld, 0x6258);
CHECK_SIZE(oCWorld, 0x628C);
//CHECK_SIZE(oCWorld, sizeof(zCWorld));
