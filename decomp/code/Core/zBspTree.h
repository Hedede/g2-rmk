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

zTBBox3D* zCBspTree::CalcLeafRealBBox3D(zVEC3 const& point)
{
	auto node = &bspRoot->parent
	zTBBox3D bbox = node->bbox3D;
	while ( 1 ) {
		if ( node->nodeType == zBSP_LEAF )
			break;

		int sign = node->planeSignbits;
		auto dist = node->plane.DistanceToPoint(point);
		if ( dist > 0.001 || dist >= -0.001 ) {
			bbox.mins[sign] = node->plane.distance;
			node = node->left;
		} else {
			bbox.maxs[sign] = node->plane.distance;
			node = node->right;
		}

		if ( !node )
			break;
	}

	return bbox;
}

void zCBspTree::RenderLightList()
{
	for (auto* light : renderLightList.GetNum() ) {
		light->DoAnimation();
		auto flare = light->lightData.lensFlareFX;
		if ( flare )
			flare->RenderLensFlares(light);
		if ( zCVobLight::renderLightVisuals )
			renderVobList.InsertEnd( light );
	}
}

void zCBspTree::Render()
{
	// globals
	s_morphedVerts = 0;
	drawnLeafs = 0;
	drawnVobs = 0;
	bspFrameCtr = masterFrameCtr;
	actBspTree = this;
	numDrawnFarPolys = 0;
	zrenderer->SetZBias(0);

	renderVobList.DeleteList();
	renderLightList.DeleteList();

	zBOOL altOrder = 0;
	if ( bspTreeMode == 0 ) {
		zCSkyControler::s_activeSkyControler->RenderSkyPre();
		doScanHorizon = 0;
		zCBspSector* startSector = 0;

		auto connVob = zCCamera::activeCam->connectedVob;
		auto start = connVob->GetPositionWorld();
		auto end = start;
		end.y -= 500000.0;
		zVEC3 inters;
		zCPolygon* hitPoly = 0;

		TraceRay(start, end, 0x40|0x20, inters, hitPoly, 0); 
		if ( hitPoly ) {
			if ( hitPoly->flags & 3 ) {
				auto end = start;
				end.y += 500000.0;

				TraceRay(start, end, 0x40|0x20, inters, hitPoly, 0);
			}
			if ( hitPoly ) {
				auto sectorIndex = hitPoly->sectorIndex;
				if ( sectorIndex != 0xFFFF ) {
					// not sure why 0x8000,
					// but there are two portals, inner and outer
					if ( !(hitPoly->flags & 3) && sectorIndex >= 0x8000 ) {
						auto portalIndex = 2 * (sectorIndex & 0x7FFF);
						auto portal = portalList[portalIndex];
						// * == dot product
						if ( start * portal->polyPlane.normal - portal->polyPlane.distance >= 0.0 )
							portal = portalList[portalIndex + 1];
						sectorIndex = portal->sectorIndex;
					}
					startSector = sectorList[sectorIndex];
				}
				if ( zCBspTree::s_showPortals )
					screen_>Print(0, 4000, "startSector: " + sectorIndex);
			}
		}

		zCBspSector::s_startPointOutdoor = 0;
		zCBspSector::s_activeOutdoorPortals.numInArray = 0;
		zCBspSector::s_sectorsRendered = 0;
		zCBspSector::s_activeSectorList.numInArray = 0;
		if ( startSector ) {
			zTBBox2D bbox;
			bbox.min.x = zCCamera::activeCam->xminFloat;
			bbox.min.y = zCCamera::activeCam->yminFloat;
			bbox.max.x = zCCamera::activeCam->xmaxFloat;
			bbox.max.y = zCCamera::activeCam->ymaxFloat;
			startSector.ActivateSectorRecIndoor(0, bbox, 0, 0);
		} else {
			for (int i = 0; i < numLeafs; ++i) {
				auto* leaf = leafList[i];
				leaf->__frameCtr = bspFrameCtr;
				leaf->sectorNum = -1;
			}

			if ( numLeafs == 0 ) {
				if ( bspRoot ) {
					if ( bspRoot->nodeType == zBSP_LEAF )
						static_cast<zCBspLeaf*>(bspRoot)->__frameCtr = bspFrameCtr;
					// doesn't quite make sense
					static_cast<zCBspLeaf*>(bspRoot)->sectorNum = -1;
				}
			}
		}
		if ( zrenderer->HasCapability(5) && zCRenderManager::s_useRendererTNL || worldRenderMode == 0 ) {
			zrenderer->SetRenderMode(0);
			bspRoot->RenderIndoor(63); // was inlined
		} else {
			zrenderer->SetRenderMode(2);
			bspRoot->RenderIndoor(63); // was inlined
		}

		zrenderMan.Flush();
		zrenderer->SetRenderMode(4);
		RenderLightList(); // was inlined
		RenderVobList();
	} else if ( bspTreeMode == 1 ) {
		zrenderer->vtab->SetRenderMode(2);
		auto cam = zCCamera::activeCam;
		zVEC3 dir;
		dir.z = (cam->camMatrixInv[2][1] + cam->camMatrixInv[2][0]) * 0.0 + vobFarClipZ * cam->camMatrixInv[2][2] + cam->camMatrixInv[2][3];
		dir.y = (cam->camMatrixInv[1][1] + cam->camMatrixInv[1][0]) * 0.0 + vobFarClipZ * cam->camMatrixInv[1][2] + cam->camMatrixInv[1][3];
		dir.x = (cam->camMatrixInv[0][1] + cam->camMatrixInv[0][0]) * 0.0 + vobFarClipZ * cam->camMatrixInv[0][2] + cam->camMatrixInv[0][3];

		auto transp = cam->camMatrix.Transpose();

		vobFarPlane.normal = transp * zVEC3{0,0,-1};
		vobFarPlane.distance = dir * vobFarPlane.normal;
		vobFarPlaneSignbits = vobFarPlane.CalcSignbits();
		numSectorsVisited = 0; // global

		auto start = cam->connectedVob->GetPositionWorld();
		auto end = start;
		end.y -= 50000.0;
		zVEC3 inters;

		zCPolygon* hitPoly = 0;
		TraceRay(start, end, 0x40|0x20, inters, hitPoly, 0);

		// it was two variables, but they are used in same way
		zCBspSector* bspSector = 0;
		zBOOL startOutdoor = 1;
		if ( hitPoly ) {
			if ( hitPoly->flags & 8 ) {
				if ( !(hitPoly->flags & 3) || hitPoly->material->bspSectorFront ) {
					auto bspSector = hitPoly->material->bspSectorFront;
					if ( bspSector ) {
						startOutdoor = 0;
						// secondVar = 0;
					}
				}
			}
		}

		zCBspSector::s_startPointOutdoor = startOutdoor;
		zCBspSector::s_activeOutdoorPortals.numInArray = 0;
		zCBspSector::s_sectorsRendered = 0;
		zCBspSector::s_activeSectorList.numInArray = 0;
		if ( startOutdoor )
		{
			horizonXDim = zCCamera::activeCam->xdim / 4.0;
			horizonYMin = (int)zCCamera::activeCam->ymaxFloat;

			if ( horizonXDim > 0 ) {
				// Not sure if I got this correctly
				std::fill_n(horizonZ, size_t(4 * horizonXDim) / 4, -99999.0);
				std::fill_n(horizonY, size_t(4 * horizonXDim) / 4, horizonYMin);
			}

			altOrder = 1;

			zCSkyControler::s_activeSkyControler->RenderSkyPre();

			doScanHorizon = 1;
			zTBBox3D bbox(bspRoot->bbox3D);
			bspRoot->RenderOutdoor(63, bbox, 1);
			doScanHorizon = 0;
			zsound->etGlobalReverbPreset(0, 0.0);
			zCSkyControler::s_activeSkyControler->SetCameraLocationHint(0);
		} else {
			doScanHorizon = 0;
			zTBBox2D bbox;
			bbox.mins[0] = zCCamera::activeCam->xminFloat;
			vbox.mins[1] = zCCamera::activeCam->yminFloat;
			vbox.maxs[0] = zCCamera::activeCam->xmaxFloat;
			vbox.maxs[1] = zCCamera::activeCam->ymaxFloat;
			bspSector->ActivateSectorRec(bbox, 0, 0);
			if ( zCBspSector::s_activeOutdoorPortals.numInArray <= 0 )
			{
				if ( renderedFirstTime )
					zCSkyControler::s_activeSkyControler->RenderSkyPre(s_activeSkyControler);
				auto bgColor = zCSkyControler::s_activeSkyControler->GetBackgroundColor();
				int clearTarget;
				if ( zrenderer->GetPolyDrawMode() == 3 ) {
					bgColor = BLACK:
					clearTarget = 3;
				} else {
					clearTarget = 2;
				}
				zrenderer->Vid_Clear(bgColor, clearTarget);
				for (auto* sec : zCBspSector::s_activeSectorList)
					sec->RenderSector();
				zCBspSector::s_activeSectorList.DeleteList() = 0;
			} else {
				zCBspSector::InitHorizonByOutdoorPortals();
				zCSkyControler::s_activeSkyControler->RenderSkyPre();
				for (auto* sec : zCBspSector::s_activeSectorList)
					sec->RenderSector();
				zCBspSector::s_activeSectorList.numInArray = 0;
				doScanHorizon = 1;
				zTBBox3D bbox(bspRoot->bbox3D);
				bspRoot->RenderOutdoor(63, bbox, 1);
				doScanHorizon = 0;
			}
			zsound->SetGlobalReverbPreset(2, 0.6);
			if ( zCBspSector::s_activeOutdoorPortals.numInArray <= 0 )
				zCSkyControler::s_activeSkyControler->SetCameraLocationHint(1);
			else
				zCSkyControler::s_activeSkyControler->SetCameraLocationHint(2);
		}

		zrenderMan.Flush();
		zrenderer->SetRenderMode(4);
		RenderLightList(); // was inlined
		RenderVobList();
		if ( zCWorld::s_bAlternateRenderOrder && startOutdoor && drawTwice > 0 ) {
			--drawTwice;
			zCSkyControler::s_activeSkyControler->colorChanged = 1;
			
			zCSkyControler::s_activeSkyControler->SetGlobalSkyScale(zCSkyControler::s_activeSkyControler, 0.89999998);
			zCSkyControler::s_activeSkyControler->SetGlobalSkyScale(zCSkyControler::s_activeSkyControler, 1.0);
		}
	}

	zCSkyControler::s_activeSkyControler->RenderSkyPost();
	leafsRendered = drawnLeafs;
	vobsRendered = drawnVobs;
	renderedFirstTime = 0;
	masterFrameCtr = masterFrameCtr + 1;
}
