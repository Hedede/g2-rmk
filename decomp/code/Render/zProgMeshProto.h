struct zTPMEdge {
	short wedge[2];
};

struct zTPMWedge {
	zVEC3 normal;
	zVEC2 uv;
	int position;
};

struct zTPMVertexUpdate {
	short numNewTri;
	short numNewWedge;
};
struct zTPMTriangle {
	short edge[3];
};
struct zTPMTriangleEdges {
	short edge[3];
};

struct zCProgMeshBuilder {
	int BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags);

private:
	// Empty class (no fields)
};


class zCProgMeshProto : zCVisual {
	Z_OBJECT(zCProgMeshProto);
public:
	static zVEC3 s_posCacheList[2048];
	static uint16_t* s_wedgeRemap[2048];

	static int BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags);
	static void SetLODParamStrength(float val)
	{
		zCProgMeshProto::s_lodParamStrengthOverride = val;
	}

	struct zCSubMesh {
		zCMaterial* material;
		zCArrayAdapt<zTPMTriangle> triList;
		zCArrayAdapt<zTPMWedge> wedgeList;
		zCArrayAdapt<float> colorList;
		zCArrayAdapt<VERTEX_INDEX> triPlaneIndexList;
		zCArrayAdapt<zTPlane> triPlaneList;
		zCArrayAdapt<zTPMTriangleEdges> triEdgeList;
		zCArrayAdapt<zTPMEdge> edgeList;
		zCArrayAdapt<float> edgeScoreList;

		zCArrayAdapt<VERTEX_INDEX> wedgeMap;
		zCArrayAdapt<zTPMVertexUpdate> vertexUpdates;

		int vbStartIndex;
	};

	~zCProgMeshProto();

	virtual void Render(zTRenderContext&);
	bool IsBBox3DLocal() override
	{
		return true;
	}
	zTBBox3D GetBBox3D() override
	{
		return bbox3d;
	}
	zCOBBox3D& GetOBBox3D() override
	{
		return obbox3d;
	}
	zSTRING GetVisualName() override
	{
		return GetObjectName();
	}

	void DynLightVisual(zCArray<zCVobLight *> const&, zMAT4*) override {}

	int GetRenderSortKey() override
	{
		return sortKey;
	}

	bool CanTraceRay() override
	{
		return true;
	}
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);

	zSTRING* GetFileExtension(int idx)
	{
		if (idx > 0)
			return nullptr;
		return &pmeshFileExt;
	}

	bool GetAlphaTestingEnabled() override
	{
		return alphaTestingEnabled;
	}
	void SetAlphaTestingEnabled(int b) override
	{
		alphaTestingEnabled = b;
	}

	virtual void LoadVisualVirtual(zSTRING const& visualName)
	{
		return Load(visualName);
	}
	virtual void Load(zCFileBIN&);
	virtual void Save(zCFileBIN&);

private:
	zCArrayAdapt<VEC3> vertList;
	zCArrayAdapt<VEC3> normalList;

	zTBBox3D bbox3d;
	zCOBBox3D obbox3d;

	void* unk2;

	zCSubMesh* subMeshes;
	int numSubMeshes;

	int unk3[3];
	zBOOL alphaTestingEnabled;

	float unk4[4];
	int unk5;
	int protoId;
	int sortKey;
};

int zCProgMeshProto::BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags)
{
	// I'm lazy
	auto builder = std::make_unique<zCProgMeshBuilder>();

	return builder->BuildProgMeshProto(sourceMesh, destProgMesh, posListRemap, buildFlags);
}

void zCProgMeshProto::InitProgMeshProto()
{
	memset(&s_posCacheList, 0, sizeof(s_posCacheList));
	memset(&s_wedgeRemap, 0, sizeof(s_wedgeRemap));
	s_frameCtr = 0;
	s_vertBufferNormal = new zCVertexBufferDyn(0x15u, 0x1000u);

	s_autoSubdivEnabled = zoptions->ReadBool("ENGINE", "zSubdivSurfacesEnabled", 0);
	zCModel::s_bSmoothRootNode = zoptions->ReadBool("ENGINE", "zSmoothModelRootNode", zCModel::s_bSmoothRootNode);
}
