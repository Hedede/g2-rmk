// Empty class (no fields)
struct zCProgMeshBuilder {
	int BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags);
};

class zCProgMeshProto : zCVisual {
	Z_OBJECT(zCProgMeshProto);
public:
	static int BuildProgMeshProto(zCMesh *sourceMesh, zCProgMeshProto* destProgMesh, zCArray<int>* posListRemap, int buildFlags);
	static void SetLODParamStrength(float val)
	{
		zCProgMeshProto::s_lodParamStrengthOverride = val;
	}

	struct zCSubMesh {
		char data[88];
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
	int unk1[4];

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
