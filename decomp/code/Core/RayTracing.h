struct zCMeshOctreeNode {
	zCMeshOctreeNode* children[8];
	zTBBox3D bbox;
	char flags;
	PolyArray polys;
};

struct zCRayTurbo {
	zCBspNode* bspNode;
	zCMeshOctreeNode otreeNode;
};


struct zCRayTurboAdmin {
	static int s_iPolyTreshold;
	static int CanHaveSpatialInfo(zCBspNode const* node)
	{
		if ( s_iPolyTreshold > 0 )
			return node->numPolys > s_iPolyTreshold;
		return false;
	}
	static int GetPolyTreshold()
	{
		return s_iPolyTreshold;
	}
	static void SetPolyTreshold(int v)
	{
		s_iPolyTreshold = v;
	}
	zCRayTurboAdmin& GetInstance()
	{
		static zCRayTurboAdmin rayTurboAdmin;
		return rayTurboAdmin;
	}

private:
	zTRayTurboValMap<zCBspNode*, zCRayTurbo*> map;
};
