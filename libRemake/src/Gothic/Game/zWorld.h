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


struct zCSession;
struct zCCSPlayer;
struct zCWayNet;
struct zCSkyControler;
struct zCWorldPerFrameCallback;
struct zCViewProgressBar;
struct zCWorld : zCObject {
	zCSkyControler* GetActiveSkyControler()
	{
		return activeSkyControler;
	}

	void MoveVobs();

	void ProcessZones()
	{
		Thiscall<void(zCWorld*)> ProcessZones{0x6207F0};
		ProcessZones(this);
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


struct oCWorld : zCWorld {
	void Render(zCCamera* cam);
	bool HasLevelName();

	zSTRING worldFilename;
	zSTRING worldName;
	char data2[12];
};

#include <Hook/size_checker.h>
CHECK_SIZE(zCWorld, 0x6258);
CHECK_SIZE(oCWorld, 0x628C);
//CHECK_SIZE(oCWorld, sizeof(zCWorld));
