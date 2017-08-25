class zCMesh : public zCVisual {
	Z_OBJECT(zCMesh);
public:
	virtual ~zCMesh();
	virtual void Render(zTRenderContext &);
	virtual void GetBBox3D();
	zCOBBox3D* GetOBBox3D() override { return &obbox3D; }
	virtual void GetVisualName();

	void DynLightVisual(zCArray<zCVobLight *> const& vobLightList, zMAT4* trafoObjToWorld) override {}
	void DynLightScalePrelit(zCOLOR scaleColor) { }

	virtual void CanTraceRay();
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport	&);
	virtual void GetFileExtension(int);
	virtual void GetAlphaTestingEnabled();
	virtual void SetAlphaTestingEnabled(int);
	virtual void LoadVisualVirtual(zSTRING	const &);

	zCPolygon* SharePoly(int i)
	{
		if (polyArray)
			return &polyArray[i];

		return polyList[i];
	}

	void LightMesh(zCVobLight& light, zMAT4& meshTrafoObjToWorld, zCWorld* wld);

private:
	int numPoly;
	int numVert;
	int numFeat;

	zCVertex**      vertList;
	zCPolygon**     polyList;
	zCVertFeature** featList;

	zCVertex*      vertArray;
	zCPolygon*     polyArray;
	zCVertFeature* featArray;

	zTBBox3D    bbox3D;

	zCOBBox3D   obbox3D;

	int masterFrameCtr;

	zCMesh*  next;
	zCMesh*  prev;

	zSTRING meshName;
	zBOOL  hasLightmaps;
	zBOOL  m_bUsesAlphaTesting;

	int    numVertAlloc;
	int    numPolyAlloc;
};

void zCMesh::LightMesh(zCVobLight *light, zMAT4 *meshTrafoObjToWorld, zCWorld *world)
{
	auto lpos = light.GetPositionWorld() * meshTrafoObjToWorld.InverseLinTrafo();

	for (unsigned i = 0; i < numVert; ++i)
		vertList[i]->MyIndex = 0;
	for (unsigned i = 0; i < numPoly; ++i)
		polyList[i]->LightStatic(light, lpos, meshTrafoObjToWorld, world);
}
