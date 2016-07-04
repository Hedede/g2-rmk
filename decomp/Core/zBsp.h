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

	void CollectVobsInBBox3D(zCArray<zCVob> const& vobs, zTBBox3D const& bbox)
	{
		++bspCollectCtr;
		CollectVobsInBBox3D_I(vobs, bbox);
	}

	bool IsLeaf()
	{
		return NodeType == zBSP_LEAF;
	}

	bool IsNode()
	{
		return NodeType == zBSP_NODE;
	}

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

private:
	int lastTimeLighted;
	zCArray<zCVob*>      leafVobList;
	zCArray<zCVobLight*> leafLightList;
	int unk[6];
}

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
