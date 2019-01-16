enum zTBspNodeType {
	zBSP_NODE = 0,
	zBSP_LEAF = 1
};

enum zTBspMode {
	zBSP_MODE_INDOOR = 0,
	zBSP_MODE_OUTDOOR = 1
};

struct zCBspBase {
	zCBspBase() = default;

	bool IsLeaf()
	{
		return NodeType == zBSP_LEAF;
	}

	bool IsNode()
	{
		return NodeType == zBSP_NODE;
	}

	void DescribeTree(int indent);

	int CountNodes()
	{
		if (nodeType == zBSP_LEAF)
			return 1;

		auto self = static_cast<zCBspNode*>(this);

		int result = 0;
		if (self->left)
			result += self->left->CountNodes();
		if (self->right)
			result += self->right->CountNodes();
		return result;
	}

	void RenderIndoor()
	{
		// note: I've omitted static casts for brevity
		auto clipFlags = 63;
		auto inFrustum = zCCamera::activeCam->BBox3DInFrustum(bbox3D, clipFlags);
		if ( inFrustum ) {
			auto wtf = inFrustum - 1;
			if ( wtf && wtf == 1 ) {
				if ( IsLeaf() ) {
					RenderLeafIndoor(clipFlags);
				} else {
					if ( left )
						left->RenderIndoor(clipFlags);
					if ( right )
						right->RenderIndoor(clipFlags);
				}
			}
		} else {
			RenderTrivInIndoor();
		}
	}

	void RenderTrivInIndoor()
	{
		if ( IsLeaf() ) {
			auto self = static_cast<zCBspLeaf*>(this);
			self->RenderLeafIndoor(0);
		} else {
			auto self = static_cast<zCBspNode*>(this);
			for (auto i = 0; i < self->numLeafs; ++i)
				self->leafList[i]->RenderLeafIndoor(0);
		}
	}

	int CollectVobsSubtree(zCArray<zCVob *>& result)
	{
		++bspCollectCtr;
		return CollectLightVobsSubtree_I(result);
	}

	int CollectLightVobsSubtree(zCArray<zCVob *>& result)
	{
		++bspCollectCtr;
		return CollectLightVobsSubtree_I(result);
	}

	void CollectVobsInBBox3D(zCArray<zCVob> const& vobs, zTBBox3D const& bbox)
	{
		++bspCollectCtr;
		CollectVobsInBBox3D_I(vobs, bbox);
	}

private:
	zCBspNode* parent = nullptr;
	zTBBox3D   bbox3D;

	zCPolygon** polyList = nullptr;
	int         numPolys = 0;

	zTBspNodeType nodeType = 0;
};

struct zCBspNode : zCBspBase {
	zCBspNode()
		: zCBspBase()
	{
		nodeType = zBSP_NODE;
	}

	void CalcPlaneSignbits(zCBspTree*)
	{
		if ( plane.normal.x != 0.0 )
			planeSignbits = 0;
		else if ( plane.normal.y != 0.0 )
			planeSignbits = 1;
		else
			planeSignbits = 2;
	}

private:
	zTPlane plane;
	zCBspNode* left  = nullptr;
	zCBspNode* right = nullptr;
	zCBspLeaf* leafList = nullptr;
	int numLeafs = 0;
	char planeSignbits;
};

struct zCBspLeaf : zCBspBase {
	zCBspLeaf()
		: zCBspBase()
	{
		nodeType = zBSP_LEAF;
	}

	void RenderLeafIndoor(int clipFlags);

private:
	int lastTimeLighted;
	zCArray<zCVob*>      leafVobList;
	zCArray<zCVobLight*> leafLightList;

	int __frameCtr = -1; // unk0
	short sectorNum = 0;
	short unk2 = 0;
	int   unk3 = 0;
	zVEC3 unk4{-99,-99,-99};
}

struct zCBspSector {
private:
	static zCArray<zCBspSector*> s_activeSectorList;
	static void RenderActiveSectorList()
	{
		for (auto sector : s_activeSectorList)
			sector->RenderSector();
		s_activeSectorList.Clear();
	}

private:
	zSTRING sectorName;

	<zCBspBase*> sectorNodes;

	int sectorIndex;

	zCArray<zCPolygon*> sectorPortals;

	zCArray<zTPortalInfo> sectorPortalInfo;

	zBOOL activated;
	int frameCtr;

	zTBBox2D activePortal;

	zVEC3 sectorCenter;

	bool hasBigNoFade;
};

void zCBspBase::DescribeTree(int indent)
{
	auto msg = "D: BSP: "_s + Spaces(2 * indent);

	if ( nodeType == zBSP_LEAF ) {
		msg += "- Leaf";
	} else {
		msg += "- Node";
	}

	if (nodeType == zBSP_NODE ) {
		auto self = static_cast<zCBspNode*>(this);
		msg += ", numLeafs: " + self->numLeafs;
	}

	if (nodeType == zBSP_LEAF) {
		auto self = static_cast<zCBspLeaf*>(this);
		msg += ", leafVobs: "   + self->leafVobList.GetNum();
		msg += ", leafLights: " + self->leafLightList.GetNum();
	}

	zINFO(3,msg); // 996,_dieter\\zBsp.cpp

	if (nodeType == zBSP_NODE) {
		auto self = static_cast<zCBspNode*>(this);
		if (self->left)
			self->left->DescribeTree(indent + 1);
		if (self->right)
			self->right->DescribeTree(indent + 1);
	}
}

void zCBspLeaf::RenderLeafIndoor(int clipFlags)
{
	if (unk0 == bspFrameCtr ) {
		zTBBox2D* actPortal;
		if ( sectorNum == -1 ) {
			actPortal = nullptr;
		} else {
			auto sector = actBspTree->sectorList[sectorNum];
			actPortal = &sector->activePortal;

			if (sector && !zCBspTree::s_showPortals) {
				if (!zCCamera::activeCam->ScreenProjectionTouchesPortalRough(bbox3D, actPortal))
					return;
			}
		}

		AddVobsToRenderList(actPortal);
		CollectNodeLights();

		for (auto i = 0; i < numPolys; ++i ) {
			auto poly = polyList[i];
			if (poly->IsPortalPoly())
				break;
			poly->RenderPoly(clipFlags & 0xF);
		}

		++drawnLeafs;
	}
}

void zCBspSector::RenderSector()
{
	if ( frameCtr != bspFrameCtr ) {
		frameCtr = bspFrameCtr;
		++zCBspSector::s_sectorsRendered;
		for (auto* node : sectorNodes) {
			auto clipFlags = 63;
			if ( zCCamera::activeCam->BBox3DInFrustum(node->bbox3D, clipFlags) != 1 ) {
				node->CollectNodeLights();
				for (auto* poly : reverse(node->polyList)) {
					if ( poly->flags & 3 || !(poly->flags & 8) )
						break;
					if ( poly->material->bspSectorFront == this )
						poly->RenderPoly(clipFlags);
				}
			}
		}
		for ( int i = 0; i < sectorPortals.numInArray; ++i )
		{
			auto alpha = sectorPortalInfo[i].alpha;
			auto poly = sectorPortals[i];
			if ( alpha > 1u ) {
				poly->material->color[3] = alpha;
				zrenderer->DrawPoly(poly);
			}
		}
	}
}
