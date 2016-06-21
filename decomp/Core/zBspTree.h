struct zCBspTree {
	int TraceRay(zVEC3 const& start, zVEC3 const& end, int traceFlags, zVEC3& inters, zCPolygon*& hitPoly, zCArray<zCVob *>* vobList);

private:
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

zTBBox3D ctestbbox3D;
zCArray<zCPolygon*> cfoundPolyList{2048};

bool zCBspBase::CollectPolysInBBox3D(zTBBox3D& bbox, zCPolygon **& foundPolyList, int & foundPolyNum)
{
	ctestbbox3D = bbox;
	CollectPolysInBBox3DRec();

	foundPolyList = cfoundPolyList.array;
	foundPolyNum  = cfoundPolyList.GetNum();
	return cfoundPolyList.GetNum() != 0;
}

int zCBspTree::TraceRay(zVEC3 const& start, zVEC3 const& end, int traceFlags, zVEC3& inters, zCPolygon*& hitPoly, zCArray<zCVob *>* vobList)
{
	if ( vobList )
		vobList->DeleteList = 0;

	traceRayVobList = vobList;

	traceRayIgnoreWaterPoly = (~traceFlags >> 9) & 1;
	traceRayIgnorePortals = (~traceFlags >> 6) & 1;
	traceRayIgnoreStatPoly = traceFlags & 0x10;
	traceRayIgnoreTranspPoly = traceFlags & 0x100;
	traceRayFindStatPoly = traceFlags & 0x100 | traceFlags & 0x20;
	traceRayFlags = traceFlags;

	traceRayHitPoly = 0;

	traceRayBBox.Init();
	traceRayBBoxPoint(start);
	traceRayBBoxPoint(end);

	traceRayStart = start;
	traceRayEnd   = end

	++bspCollectCtr;

	if ( s_RayCache )
		s_RayCache->StartTraceRay(start, end);

	traceRayDepth = 0;
	auto result = bspRoot->RayIntersection(start, end, inters);

	if ( s_RayCache )
		zCRayCache::StopTraceRay();

	s_RayCache = 0;

	hitPoly = traceRayHitPoly;

	++traceRayCtr;
	traceRayVobList = 0;

	return result;
}
