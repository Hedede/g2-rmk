class zCVisual : zCObject {
	Z_OBEJCT(zCVisual);
public:
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
