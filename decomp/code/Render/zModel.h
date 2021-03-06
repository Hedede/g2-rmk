struct zTMdl_StartedVobFX {
	zCVob *vob;
	int unk;
};

struct zTAniAttachment {
	int aniId;
	zCArray unk;
	float freq;
	int b;
};


class zCModel : public zCVisualAnimate {
	Z_OBEJCT(zCModel);
public:
	virtual ~zCModel();
	virtual void Render(zTRenderContext&);
	bool IsBBox3DLocal() override
	{
		return true;
	}
	zTBBox3D GetBBox3D() override
	{
		return bbox3d;
	}
	virtual void GetVisualName();
	virtual void SetVisualUsedBy(zCVob *);
	virtual void GetRenderSortKey();

	bool CanTraceRay() override
	{
		return true;
	}
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
	zSTRING* GetFileExtension(int id) override
	{
		if (id < 0 || id > 1)
			return nullptr;
		return &modelFileExt[id]
	}
	virtual void LoadVisualVirtual(zSTRING const	&);
	void StartAnimation(zSTRING const& name) override
	{
		return StartAni(name, 0);
	}
	virtual void StopAnimation(zSTRING const &);
	int IsAnimationActive(zSTRING const& name) override
	{
		if (auto ani = GetAniByName(name))
			return IsAniActive(ani);
		return 0;
	}
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

	zCModelNodeInst* SearchNode(const zSTRING& nodeName);

	int GetAniIDFromAniName(zSTRING const& name)
	{
		auto ani = _prototypes[0]->SearchAni(name);
		return ani ? ani->aniId : -1;
	}

	zCModelAni* GetAniFromAniID(int aniId);
	// v My addition, to prettify stuff
	zCModelAni* GetAniByName(zSTRING const& name)
	{
		return GetAniFromAniID(GetAniIDFromAniName(name));
	}

	bool IsAniActive(zCModelAni* ani);

	// From G1Demo, optimized out in G2A
	bool IsAniActive(int aniId)
	{
		return IsAniActive(GetAniFromAniID(aniId));
	}
	bool IsAniActive(zSTRING const& name)
	{
		return IsAniActive(GetAniByName(name));
	}

	zCModelAniActive* GetAniActive(int aniId);


	void StartAni(zCModelAni* ani, int startMode);
	void StartAni(int aniId, int startMode)
	{
		StartAni(GetAniFromAniID(aniId), startMode);
	}

	zVEC3 GetVelocityRing()
	{
		zVEC3 ret;
		for (auto& vec : velocities)
			ret += vec;
		return ret * 0.125;
	}

private:
	zTAniAttachment* SearchAniAttachList(int aniID);
	static int AniAttachmentCompare(zTAniAttachment** a1, zTAniAttachment** a2);

private:
	struct zTMdl_NodeVobAttachment {
		zCVob *Vob;
		zCModelNodeInst *NodeInst;
	};

	struct zTMeshLibEntry {
		zCModelTexAniState TexAniState;
		zCModelMeshLib *MeshLibPtr;
	};

	int active_ani_num;
	zCModelAniActive* active_anis[6];

	zCModelAniActive* guc; // pointer to array (allocated by new)

	zCArray<> alsoarray;
	zCVob* usedBy;

	zCArray<zCModelPrototype*> _prototypes;
	zCArray<zCModelNodeInst*> modelNodeInstArray;
	zCArray<zCMeshSoftSkin*> _skins;
	zCArraySort<zTAniAttachment*> aniAttachments;
	zCArray<zTMdl_NodeVobAttachment> modelNodeVobAttachments;
	zCArray<zTMdl_StartedVobFX> vobFxList;
	zCArray<> unknown_ar2;
	zCArray<zTMeshLibEntry*> meshLib;

	void *ukptr;
	zCArray unkarra;
	zTBBox3D bbox3d;
	zTBBox3D bbox3d_1;
	zTBBox3D bbox3d_2;

	int pod[2];
	float fatness;
	float __fatness; // not sure, maybe I accidentally shifted it?

	int unn7;
	int unknownffs;
	zVEC3 __vecY;
	zVEC3 __vecZ;
	zVEC3 __movement; // __vecA
	zVEC3 __vecB;
	int pat;
	int direction;
	int put[2];
	
	zCQuat __quat;
	zVEC3 __vecC;
	int dunno;
	zVEC3 velocities[8];
	struct {
		uint8_t unk : 1; // 1
		uint8_t unk : 1; // 2
		uint8_t unk : 1; // 4
		uint8_t unk : 1; // 8
		uint8_t unk : 1; // 0x10
		uint8_t yawOnly : 1; // 0x20
	} flags;
	int unkno;

	zCModelAni **showAniList;
};

zCModelNodeInst* zCModel::SearchNode(const zSTRING& nodeName)
{
	for (auto* node : modelNodeInstArray) {
		if (nodeName == node->protoNode->name)
			return node;
	}
	return nullptr;
}


zTAniAttachment* zCModel::SearchAniAttachList(int aniID)
{
	static zTAniAttachment aniAttachSearchDummy;
	aniAttachSearchDummy.aniId = aniID;
	int index = aniAttachments.Search(&aniAttachSearchDummy);
	if (index < 0)
		return nullptr;
	return aniAttachments[index];
}

int zCModel::AniAttachmentCompare(zTAniAttachment** a1, zTAniAttachment** a2)
{
	int id1 = (*a1)->aniId;
	int id2 = (*a2)->aniId;
	if ( id1 > id2 )
		return 1;
	if ( id1 == id2 )
		return 0;
	return -1;
}


zCModelAniActive* zCModel::GetActiveAni(int aniId)
{
	for (unsigned i = 0; i < active_ani_num; ++i)
		if (active_anis[i]->ani->aniId == aniId)
			return active_anis[i];
	return nullptr;
}

zCModelAniActive* zCModel::GetActiveAni(zCModelAni *ani)
{
	if (!ani)
		return nullptr;

	for (unsigned i = 0; i < active_ani_num; ++i)
		if (active_anis[i]->protoAni == ani)
			return active_anis[i];
	return nullptr;
}

bool zCModel::IsAniActive(zCModelAni *ani)
{
	return GetActiveAni(ani) != nullptr;
}

zCModelAni* zCModel::GetAniFromAniID(int aniId)
{
	if ( aniId == -1 )
		return nullptr;

	for (auto proto : reverse(_prototypes))
		if (proto->anis[aniId])
			return proto->anis[aniId];

	return nullptr;
}

int zCModel::GetNumMaterials()
{
	if (_skins.GetNum() <= 0)
		return 0;
	if (_skins.GetNum() == 1)
		return skins[0]->numSubMeshes;
	return skins[1]->numSubMeshes;
}

void zCModel::SetRandAniFreq( int aniID, float freq)
{
	GetCreateAniAttachment(aniID)->freq = freq;
}

zCVob* zCModel::GetAttachedNodeVob(zCModelNodeInst* node)
{
	for (auto [v, n] : modelNodeVobAttachments)
		if (n == node)
			return v;
	return nullptr;
}

void zCModel::RemoveAllChildVobsFromNode()
{
	for (auto [v, n] : modelNodeVobAttachments)
		v->Release();
	modelNodeVobAttachments.Clear();
}

void zCModel::StopAnisLayerRange(int layerLow, int layerHi)
{
	i = 0;
	for (int i = 0; i < active_ani_num; ++i) {
		auto* ani = active_anis[i];
		if ( within(ani->protoAni->layer, layerLow, layerHi) ) {
			StopAni( ani );
			if (ani != active_anis[i])
				--i;
		}
	}
}

void zCModel::RemoveAllVobFX()
{
	for (int i = vobFxList.GetNum() - 1; i >= 0; --i ) {
		auto* vob = vobFxList.array[i].vob;
		if ( vob ) {
			auto visual = vob->visual;
			// pesudocode, it checks classdef and makes static cast
			if ( auto pfx = zCAST<zCParticleFX*>(visual) )
				pfx->StopEmitterOutput();
		}

		vobFxList.RemoveIndex( i );
		Release(vob);
	}
}

void zCModel::RemoveMeshLibAll()
{
	for ( zTMeshLibEntry* entry : reverse(meshLib) )
		RemoveMeshLib( entry->MeshLibPtr->GetObjectName() );
}
