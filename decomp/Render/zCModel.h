class zCModel : public zCVisualAnimate {
	Z_OBEJCT(zCModel);
public:
	virtual ~zCModel();
	virtual void Render(zTRenderContext	&);
	virtual void IsBBox3DLocal();
	virtual void GetBBox3D();
	virtual void GetVisualName();
	virtual void SetVisualUsedBy(zCVob *);
	virtual void GetRenderSortKey();
	virtual void CanTraceRay();
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
	virtual void GetFileExtension(int);
	virtual void LoadVisualVirtual(zSTRING const	&);
	virtual void StartAnimation(zSTRING	const &);
	virtual void StopAnimation(zSTRING const &);
	virtual void IsAnimationActive(zSTRING const &);
	virtual void GetAnyAnimation();
};
