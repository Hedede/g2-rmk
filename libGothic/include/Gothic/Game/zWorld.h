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
struct zCVob;
struct zCVobLight;

#include <Gothic/Types/zVEC3.h>
struct zTPlane {
	float distance;
	zVEC3 normal;
};


struct zCBspTree {
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
	int vobFarClipZ;
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


struct zCSession;
struct zCCSPlayer;
struct zCWayNet;
struct zCSkyControler;
struct zCWorldPerFrameCallback;
struct zCCamera;
struct zCWorld : zCObject {
	zCSkyControler* GetActiveSkyControler()
	{
		// TODO: is this necessary?
		if ( bspTree.bspTreeMode == 0 )
			activeSkyControler = skyControlerIndoor;
		else
			activeSkyControler = skyControlerOutdoor;
		return activeSkyControler;
	}

	void Render(zCCamera* cam)
	{
		Thiscall<void(zCWorld*, zCCamera*)> call{0x621700};
		call(this,cam);
	}

	void MoveVobs()
	{
		Thiscall<void(zCWorld*)> call{0x626050};
		call(this);
	}

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

	zCWorldPerFrameCallback **perFrameCallbackList_array;
	int perFrameCallbackList_numAlloc;
	int perFrameCallbackList_numInArray;

	zCSkyControler *skyControlerIndoor;
	zCSkyControler *skyControlerOutdoor;
	zCSkyControler *activeSkyControler;
	char data1[192];

	zCBspTree bspTree;
	zCArray<zCVob*> activeVobList;
	zCArray<zCVob*> walkList;
	zCArray<zCVob*> vobHashTable[2048];
};


struct oCWorld : zCWorld {
	char data2[52];
};

CHECK_SIZE(zCWorld, 0x6258);
CHECK_SIZE(oCWorld, 0x628C);
//CHECK_SIZE(oCWorld, sizeof(zCWorld));
