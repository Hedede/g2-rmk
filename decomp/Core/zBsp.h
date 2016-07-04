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

		int result = 0;
		if ( left )
			result += left->CountNodes();
		if ( right )
			result += right->CountNodes();
		return result;
	}

private:
	zCBspNode* parent = nullptr;
	zTBBox3D   bbox3D;

	zCPolygon** polyList = nullptr;
	int         numPolys = 0;

	zTBspNodeType nodeType = 0;

	int unk0[3];
	int leafVobs;

	zCBspNode* left  = nullptr;
	zCBspNode* right = nullptr;

	int leafLights;
	int numLeafs = 0;
	int unk1;
};

struct zCBspNode : zCBspBase {
	zCBspNode()
		: zCBspBase()
	{
		nodeType = zBSP_NODE;
	}

};

struct zCBspLeaf : zCBspBase {
	zCBspLeaf()
		: zCBspBase()
	{
		nodeType = zBSP_LEAF;
		numLeafs = -1;
	}
}
