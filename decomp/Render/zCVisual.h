class zCVisual : zCObject {
public:
	virtual zCClassDef* GetClassDef();
	virtual ~zCVisual() = default;
	virtual void unk() = 0;
	virtual void IsBBox3DLocal(void);
	virtual void unk() = 0;
	virtual void GetOBBox3D(void);
	virtual void unk() = 0;
	virtual void GetVisualDied(void);
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
