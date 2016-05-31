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

	zVEC3 GetVelocityRing()
	{
		zVEC3 ret;
		for (auto& vec : velocities)
			ret += vec;
		return ret * 0.125;
	}

private:
	int ani_num;
	zCModelAni **ani_array;
	int unn2;
	int unn3;
	int unn4;
	int unn5;
	int unn6;
	void **guc;
	zCArray alsoarray;
	zCVob *usedBy;
	ModelPrototypeArray _prototypes;
	zCArray modelNodeInstArray;
	zCArray_zCMeshSoftSkin _skins;
	zCArraySort aniAttachments;
	zCArray __modelNodes;
	zCArray unknown_ar1;
	zCArray unknown_ar2;
	ObjectArray meshLib;
	void *ukptr;
	zCArray unkarra;
	zTBBox3D bbox3d;
	zTBBox3D bbox3d_1;
	zTBBox3D bbox3d_2;
	int pod[3];
	float __fatness;
	int unn7;
	int unknownffs;
	zVEC3 __vecY;
	zVEC3 __vecZ;
	zVEC3 __vecA;
	zVEC3 __vecB;
	int put[8];
	zVEC3 __vecC;
	int dunno;
	zVEC3 velocities[8];
	char flags;
	int unkno;
	zCModelAni **showAniList;
};
