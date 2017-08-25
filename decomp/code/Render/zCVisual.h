class zCVisual : zCObject {
	Z_OBEJCT(zCVisual);
public:
	static zCArray<zCVisual*> s_visualClassList;
	static void InitVisualSystem();

	virtual ~zCVisual() = default;

	virtual void Render(zTRenderContext&) = 0;

	virtual bool IsBBox3DLocal() { return true; }
	virtual zTBBox3D   GetBBox3D() = 0;
	virtual zCOBBox3D* GetOBBox3D() { return nullptr; }

	virtual zSTRING* GetVisualName() = 0;

	virtual bool GetVisualDied() { return false; }

	virtual void SetVisualUsedBy(zCVob *) { }
	virtual void DynLightVisual(zCArray<zCVobLight*> const &,zMAT4 *) {}

	virtual void* GetRenderSortKey() { return this; }

	virtual void CanTraceRay() { return false; }
	virtual int TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &)
	{
		return 1;
	}

	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *) {}
	virtual void HostVobAddedToWorld(zCVob *,zCWorld *) {}

	virtual zSTRING* GetFileExtension(int);
	virtual void GetLODVisualAndAlpha(float,zCVisual * &,float &);
	virtual bool GetAlphaTestingEnabled() { return false; }
	virtual void SetAlphaTestingEnabled(int) {}
	virtual zCVisual* LoadVisualVirtual(zSTRING const &) { return nullptr; }

private:
	zCVisual*   nextLODVisual = nullptr;
	zCVisual*   prevLODVisual = nullptr;
	zREAL       lodFarDistance = 50000.0;
	zREAL       lodNearFadeOutDistance = 49000.0;
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
