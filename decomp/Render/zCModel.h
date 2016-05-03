class zCModel : public zCVisualAnimate {
	Z_OBEJCT(zCModel);
public:
	virtual ~zCModel();
	virtual void Render(zTRenderContext&);
	bool IsBBox3DLocal() override
	{
		return true;
	}
	virtual void GetBBox3D();
	virtual void GetVisualName();
	virtual void SetVisualUsedBy(zCVob *);
	virtual void GetRenderSortKey();

	bool CanTraceRay() override
	{
		return true;
	}
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
	virtual void GetFileExtension(int);
	virtual void LoadVisualVirtual(zSTRING const	&);
	virtual void StartAnimation(zSTRING	const &);
	virtual void StopAnimation(zSTRING const &);
	virtual void IsAnimationActive(zSTRING const &);
	virtual void GetAnyAnimation();

	zTBBox3D GetBBox3DNodeWorld(zCModelNodeInst* node)
	{
		CalcNodeListBBoxWorld();
		return node->bbox;
	}

	zVEC3 GetNodePositionWorld(zCModelNodeInst* node)
	{
		CalcNodeListBBoxWorld();
		return node->trafoWorld->GetTranslation();
	}

	int GetAniIDFromAniName(zSTRING const& name)
	{
		auto ani = _prototypes[0]->SearchAni(name);
		return ani ? ani->aniId : -1;
	}

private:
	zTBBox3D bbox3d[3];
};
