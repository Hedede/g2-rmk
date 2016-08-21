struct zCBspTree {
	static bool GetShowPortals()
	{
		return s_showPortals;
	}

	static void GetShowPortals(bool b)
	{
		s_showPortals = b;
	}

	static void SetRayCache(zCRayCache* cache)
	{
		s_RayCache = cache;
	}

	static uint16_t GetFileVersion()
	{
		return 3;
	}
	
	zCBspTree()
	{
		InitTree();
	}

	~zCBspTree()
	{
		DisposeTree();
		delete[] leafList;
	}

	void InitTree()
	{
		numNodes = 0;
		numLeafs = 0;
		numPolys = 0;
		nodeList = nullptr;
		leafList = new zCBspLeaf[1];
		treePolyList = nullptr;
		mesh = nullptr;
		masterFrameCtr = 0;
		actPolyPtr = nullptr;
		actNodePtr = nullptr;
		actLeafPtr = nullptr;
		bspRoot = leafList[0];
		treePolyList = nullptr;
		bspRoot.bbox.SetMaximumBox();
		compiled = false;
		drawVobBBox3D = false;
		vobFarClipZ = 0.0;
		vobFarPlaneSignbits = 0;
		leafsRendered = 0;
		vobsRendered = 0;
		bspTreeMode = 0;
		renderVobList.Clear();
		renderLightList.Clear();
		worldRenderMode = 1;
		renderedFirstTime = 1;
	}

	int TraceRay(zVEC3 const& start, zVEC3 const& end, int traceFlags, zVEC3& inters, zCPolygon*& hitPoly, zCArray<zCVob *>* vobList);


	void RemoveVob(zCVob const* vob)
	{
		bspRoot->RemoveVobFromLeafs(vob);
	}
	void AdjustVob(zCVob const* vob);
	void AddVob(zCVob const* vob)
	{
		AdjustVob(vob);
	}

	zSTRING const* GetSectorNameVobIsIn(zCVob const* vob)
	{
		if (vob->groundPoly)
			return vob->groundPoly->material->bspSectorFront;
		return nullptr;
	}

	zCBspSector* zCBspTree::GetBspSectorByName(zSTRING& const name)
	{
		return sectorList.Search(name);
	}

	void GetPolyNeighbours(zCPolygon* sourcePoly, zCPolygon **& foundPolyList, int& foundPolyNum);

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
		s_RayCache->StopTraceRay();

	s_RayCache = 0;

	hitPoly = traceRayHitPoly;

	++traceRayCtr;
	traceRayVobList = 0;

	return result;
}

void zCBspTree::AdjustVob(zCVob *vob)
{
	if ( vob->type == VOB_TYPE_LIGHT ) {
		if ((bspFrameCtr & 1) == 1 || !vob->vobLeafList.GetNum() || ((vob->lightData.flags >> 16) & 1) == 1 ) {
			bspRoot->RemoveVobFromLeafs(vob);
			bspRoot->AddVobLightToLeafs(vob);
		}
	} else {
		bspRoot->RemoveVobFromLeafs(vob);
		bspRoot->AddVobToLeafs(vob);
	}
}

void zCBspTree::GetPolyNeighbours(zCPolygon* sourcePoly, zCPolygon **& foundPolyList, int& foundPolyNum)
{
	auto searchBox = sourcePoly->GetBBox3D();
	searchBox.Scale(1.1);

	v5 = foundPolyNum;
	bspRoot->CollectPolysInBBox3D(searchBox, foundPolyList, foundPolyNum);

	for (auto i = 0; i < foundPolyNum; ++i) {
		auto& poly = foundPolyList[i];
		if (poly == sourcePoly || poly->IsPortalPoly()) {
			foundPolyList[i--] = foundPolyList[--foundPolyNum];
		} else {
			for (auto j = i; j < foundPolyNum; ++j) {
				auto& poly2 = foundPolyList[j];
				if (poly2 == poly)
					foundPolyList[j--] = foundPolyList[--foundPolyNum];
			}
		}
	}
}
