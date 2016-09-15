struct zTPMEdge {
	short unk[2];
};
struct zTPMWedge {
	int unk[6];
};
struct zTPMVertexUpdate {
	int unk;
};
struct zTPMTriangle {
	short unk[3];
};
struct zTPMTriangleEdges {
	short unk[3];
};

// Empty class (no fields)
struct zCProgMeshBuilder {
	int BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags);
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

		  int unk1;
		  int numTris;

		  zCArrayAdapt<zTPMWedge> wedges;
		  zCArrayAdapt<float> unk2;
		  zCArrayAdapt<uint16_t> unk3;
		  zCArrayAdapt<zTPlane> unk4;
		  zCArrayAdapt<uint16_t> unk5;
		  zCArrayAdapt<zTPMVertexUpdate> vertexUpdates;
		  zCArrayAdapt<zTPMTriangleEdges> triangleEdges;
		  zCArrayAdapt<float> unk6;

		  int unk7;
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
	int unk0;
	int numVerts;

	zCArrayAdapt<VEC3> unk1;

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
