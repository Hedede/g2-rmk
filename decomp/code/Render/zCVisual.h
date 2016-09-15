class zCVisual : zCObject {
	Z_OBEJCT(zCVisual);
public:
	static zCArray<zCVisual*> s_visualClassList;
	static void InitVisualSystem();

	virtual ~zCVisual() = default;

	virtual void Render(zTRenderContext&) = 0;

	virtual bool IsBBox3DLocal();
	virtual zTBBox3D  GetBBox3D() = 0;
	virtual zCOBBox3D GetOBBox3D();

	virtual void GetVisualName() = 0;
	virtual void GetVisualDied();
	virtual void SetVisualUsedBy(zCVob *);
	virtual void DynLightVisual(zCArray<zCVobLight *> const &,zMAT4 *);
	virtual void GetRenderSortKey(void);
	virtual void CanTraceRay(void);
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
	virtual void HostVobAddedToWorld(zCVob *,zCWorld *);
	virtual void GetFileExtension(int);
	virtual void GetLODVisualAndAlpha(float,zCVisual * &,float &);
	virtual void GetAlphaTestingEnabled(void);
	virtual void SetAlphaTestingEnabled(int);
	virtual void LoadVisualVirtual(zSTRING const &);

private:
	zCVisual*   nextLODVisual;
	zCVisual*   prevLODVisual;
	zREAL       lodFarDistance;
	zREAL       lodNearFadeOutDistance;
};

void zCVisual::InitVisualSystem()
{
	auto& classDefList = *zCClassDef::classDefList;
	size_t num = classDefList.size();
	for (size_t i = 0; i < num; ++i) {
		auto cd = classDefList.GetSafe(i);
		if (!cd)
			continue;
		if (!cd.IsDerivedFrom(&zCVisual::classDef)) // pseudocode
			continue;
		if (!cd.IsAbstract())
			continue;

		auto vis = cd.CreateNewInstance();
		if ( !vis->GetFileExtension(0) ) {
			Release(vis);
			continue;
		}
		s_visualClassList.InsertEnd(vis);
	}
}
