class zCProgMeshProto : zCVisual {
	Z_OBJECT(zCProgMeshProto);
public:
	~zCProgMeshProto();

	virtual void Render(zTRenderContext	&);
	virtual void IsBBox3DLocal();
	virtual void GetBBox3D();
	virtual zCOBBox3D& GetOBBox3D()
	{
		return obbox3d;
	}
	virtual void GetVisualName();
	virtual void DynLightVisual(zCArray<zCVobLight *> const	&,zMAT4	*);
	virtual void GetRenderSortKey();
	virtual void CanTraceRay();
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void GetFileExtension(int);
	virtual void GetAlphaTestingEnabled();
	virtual void SetAlphaTestingEnabled(int);
	virtual void LoadVisualVirtual(zSTRING const	&);
	virtual void Load(zCFileBIN	&);
	virtual void Save(zCFileBIN	&);
};
