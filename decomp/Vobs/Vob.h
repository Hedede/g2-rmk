class zCVob : public zCObject {
	Z_OBJECT(zCObject);
private:
	static zBOOL s_enableAnimations;
	static zBOOL s_showHelperVisuals;
public:
	static int GetAnimationsEnabled()
	{
		return s_enableAnimations;
	}
	
	static int GetShowHelperVisuals()
	{
		return s_showHelperVisuals;
	} 

	static SetAnimationsEnabled(int b)
	{
		s_enableAnimations = b;
	}

	static SetShowHelperVisuals(int b)
	{
		s_showHelperVisuals = b;
	}

	struct zTModelLimbColl {
		zTModelLimbColl() = default;
		~zTModelLimbColl() = default;

		zCVob* vob;
		zCMaterial* unk2;
		zCList<zCModelNodeInst> nodes;
		zVEC3 center;
	};

	static void ResetIDCtr()
	{
		s_nextFreeID = 0;
	}

	static void CleanupVobSystem();

	virtual ~zCVob();

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual void OnTrigger(zCVob *,zCVob *) { }
	virtual void OnUntrigger(zCVob *,zCVob *) { }
	virtual void OnTouch(zCVob *) { }
	virtual void OnUntouch(zCVob *) { }
	virtual void OnTouchLevel() { }
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &) { }
	virtual void OnMessage(zCEventMessage *,zCVob *) { }
	virtual void OnTick() { }
	virtual void OnTimer() { }
	virtual void PostLoad() { }

	virtual uint32_t GetCharacterClass() const
	{
		return 0;
	}
	virtual void SetSleepingMode(zTVobSleepingMode);
	virtual void EndMovement(int);
	virtual bool CanThisCollideWith(zCVob* vob)
	{
		return 1;
	}
	virtual void Render(zTRenderContext &);
	virtual void SetVisual(zSTRING const &);
	virtual void SetVisual(zCVisual *);

	zCVisual GetVisual() const
	{
		return visual;
	}

	void SetBBox3DWorld(zTBBox3D const& bbox);
	void SetPositionWorld(zVEC3 const& pos);
	void SetHeadingWorld(zVEC3 const& targetPos);
	void SetHeadingWorld(zCVob* targetVob)
	{
		SetHeadingWorld(targetVob->GetPositionWorld());
	}

	void Move(float x, float y, float z);
	void MoveWorld(float x, float y, float z);
	void GetPositionWorld(float& x, float& y, float& z) const;

	zVEC3 GetPositionWorld() const;
	zVEC3 GetPositionLocal() const;

	zVEC3 GetAtVectorWorld() const;
	zVEC3 GetUpVectorWorld() const;
	zVEC3 GetRightVectorWorld() const;

	void SetRotationWorld(zCQuat const& quat);

	void RotateWorldX(float angle);
	void RotateWorldY(float angle);
	void RotateWorldZ(float angle);

	void RotateLocalX(float angle);
	void RotateLocalY(float angle);
	void RotateLocalZ(float angle);

	float GetDistanceToVob(zCVob& vob)
	{
		return (GetPositionWorld() - vob.GetPositionWorld()).Length();
	}

	float GetDistanceToVob2(zCVob& vob)
	{
		return (GetPositionWorld() - vob.GetPositionWorld()).Length2();
	}

	float GetDistanceToVobApprox(zCVob& vob)
	{
		return (GetPositionWorld() - vob.GetPositionWorld()).LengthApprox();
	}

	virtual void GetScriptInstance(zSTRING*& outname, int& outidx)
	{
		outname = 0;
		outidx = 0;
		return 0;
	}

	virtual int SetByScriptInstance(zSTRING const*, int)
	{
		return 0;
	}

	virtual uint32_t GetCSStateFlags() const
	{
		return 0;
	}
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual zSTRING const* GetTriggerTarget(int i) const
	{
		return 0;
	}

	virtual void ThisVobAddedToWorld(zCWorld* world);
	virtual void ThisVobRemovedFromWorld(zCWorld* world);

	void RemoveVobFromWorld();
	void RemoveVobSubtreeFromWorld();
	void RemoveVobFromBspTree();

	void ReleaseVobSubtree(zCTree<zCVob>* node = nullptr);

	void DoneWithTrafoLocal();

	bool HasParentVob() const;
	zCMover* GetAutoLinkParent() const; // unused?

	void AddVobToWorld_CorrectParentDependencies();
	void CorrectTrafo();
	void CreateTrafoLocal();

	zCWorld* GetHomeWorld() const
	{
		return homeWorld;
	}

	void GetEM(int doNotCreate = 0);

	bool IsOnTimer() const
	{
		return nextOnTimer < std::numeric_limits<float>::max();
	}

	void SetSleeping(int sleep)
	{
		if ( sleep )
			SetSleepingMode(0);
		else
			SetSleepingMode(1);
	}

	void SetDrawBBox3D(int b)
	{
		flags.drawBBox3D = b;
	}

	void SetStaticVob(int b)
	{
		flags.staticVob = b;
	}

	void SetPhysicsEnabled(zCVob *this, int enable)
	{
		if (!flags.staticVob) {
			flags.physicsEnabled = enable;
			if ( rigidBody )
				StopTransRot(enable);
		}
	}

	void SetCollDet(int b)
	{
		SetCollDetStat(b);
		SetCollDetDyn(b);
	}

	void SetCollisionClass(zCCollisionObjectDef* collClass);
	void CreateCollisionObject();
	void DestroyCollisionObject();
	void SetCollisionObject(zCCollisionObject* col);
	zCCollisionObject* GetCollisionObject()
	{
		return collisionObject;
	}
	zCRigidBody* GetRigidBody();

	zMAT4 GetNewTrafoObjToWorld() const
	{
		if (isInMovementMode)
			return collisionObject->trafoObjToWorld;
		return trafoObjToWorld;
	}

	void SetNewTrafoObjToWorld(zMAT4 const& newTrafo)
	{
		if (isInMovementMode)
			collisionObject->trafoObjToWorld = newTrafo;
	}

	void TouchMovement()
	{
		collisionObject->flags |= 1;
	}

	void ResetCollisionObjectState();
	void ResetToOldMovementState();

	void SetAI(zCAIBase* newai);

	bool GetDontWriteIntoArchive() const
	{
		return flags1.dontWriteIntoArchive;
	}

	bool GetIsProjectile() const
	{
		if (callback_ai)
			return callback_ai->GetIsProjectile();
		return false;
	}

	int GetVobType() const
	{
		return type;
	}

	const zSTRING* GetSectorNameVobIsIn() const
	{
		return homeWorld->bspTree.GetSectorNameVobIsIn(this);
	}

	void SetVobName(zSTRING const& name);

	void SetOnTimer(float deltaTime);

private:
	struct zTCollisionContext {
		~zTCollisionContext() = default;

		zCArray<zCCollisionObject*> collObjects;
		zCArray<zCVob*> vobList;
	};

	// const? бред
	void CleanupCollisionContext(zTCollisionContext const& context);
	void CollectCollisionContext(zTCollisionContext& context);

private:
	static void DeleteGroundShadowMesh();
	void SetGroundShadowSize(zVEC2 const& dimensions);
	zVEC2 GetGroundShadowSize();

	void ProcessOnTimer();

	void SetInMovement(int inMovement);
	void SetInMovementMode(zCVob::zTMovementMode mode);

public:/* Unused */
	static uint32_t GetNextFreeVobID()
	{
		return zCVob::s_nextFreeID;
	}

	/* Gothic 1 demo, удалено в релизной версии
	uint32_t GetVobID() const
	{
		return id;
	}
	*/

	void SetVobID(uint32_t const& id)
	{
		/* Gothic 1 demo, удалено в релизной версии
		this->id = id;
		if (id >= s_nextFreeID)
			s_nextFreeID = id + 1;
			*/
	}

	void SetVobPresetName(zSTRING const& presetName) { }
	zSTRING& GetVobPresetName() const
	{
		if (!presetName)
			return zSTR_EMPTY;
		return *vobPresetName;
	}


private:
	zCTree<zCVob>* globalVobTreeNode = nullptr;

	zTFrameCtr lastTimeDrawn     = -1;
	int        lastTimeCollected = 0;

	zCArray<zCBspLeaf*> vobLeafList;

	zMAT4 trafoObjToWorld;

	zTBBox3D    bbox3D;
	zTBSphere3D bsphere3D;

	zCArray<zCVob*> touchVobList;

	int type;
	int groundShadowSizePacked = 0;

	zCWorld*   homeWorld  = nullptr;
	zCPolygon* groundPoly = nullptr;

	zCAIBase* callback_ai = nullptr;
	zMAT4*    trafo  = nullptr;
	zCVisual* visual = nullptr;
	float     visualAlpha = 1.0f;
	float     vobFarClipZScale = 1.0f;

	int       aniMode = 0;
	float     aniModeStrength = 0.0;
	int       zBias;

	zCRigidBody* rigidBody;

	zCOLOR   lightColorStat{0};
	zCOLOR   lightColorDyn{0};
	zVEC3    lightDirectionStat;

	zSTRING* vobPresetName = nullptr;

	zCEventManager* eventManager;

	float nextOnTimer = std::numeric_limits<float>::max();

	struct {
		uint32_t showVisual                : 1; // 1
		uint32_t drawBBox3D                : 1; // 2
		uint32_t visualAlphaEnabled        : 1; // 4
		uint32_t physicsEnabled            : 1; // 8
		uint32_t staticVob                 : 1; // 0x10
		uint32_t ignoredByTraceRay         : 1; // 0x20
		uint32_t collDetectionStatic       : 1; // 0x40
		uint32_t collDetectionDynamic      : 1; // 0x80
		uint32_t castDynShadow             : 2; // 0x100
		uint32_t lightColorStatDirty       : 1; // 0x200 / 4
		uint32_t lightColorDynDirty        : 1; // 0x400 / 8
	} flags1;

	uint8_t isInMovementMode : 2;

	struct {
		uint32_t sleepingMode              : 2;
		uint32_t mbHintTrafoLocalConst     : 1; // 4
		uint32_t mbInsideEndMovementMethod : 1; // 8
	} flags2;

	uint8_t visualCamAlign;

	struct {
		uint32_t collButNoMove            : 4; // 8 4 2 1
		uint32_t dontWriteIntoArchive     : 1; // 0x10
		uint32_t isInWater                : 1;
		uint32_t isAmbientVob             : 1;
	} flags3;

	zCCollisionObjectDef* collisionObjectClass = &zCCollObjectUndef::s_oCollObjClass;
	zCCollisionObject*    collisionObject      = nullptr;
};

// private static
void zCVob::LoadGroundShadowMesh()
{
	zCVob::s_shadowMesh = zCMesh::Load("_intern\\groundShadow.3ds", 0);
	if ( zCVob::s_shadowMesh ) {
		zCVob::s_shadowMesh->polyList[0]->material->alphaFunc = 5;
		zCVob::s_shadowMesh->SetStaticLight({-1,-1,-1});
		zCVob::s_shadowMesh->ResetLightDynToLightStat();
	}
}

// static
void zCVob::InitVobSystem()
{
	LoadGroundShadowMesh();

	zCVob::s_poCollisionDetector = new zCCollisionDetector();
	zCCollObjectBase::S_RegisterCollisionTestFuncs(s_poCollisionDetector);
}

void zCVob::CleanupVobSystem()
{
	Release( zCVob::s_shadowMesh );

	s_helperVisualMap.clear(); // don't know, bunch of nonsense

	Delete( zCVob::s_poCollisionDetector );
}

zCVob* zCVob::zCVob()
	// vobLeafList — preallcoate 16
	: vobLeafList(16),
{
	trafoObjToWorld = Alg_Identity3D();

	flags1.showVisual = 1;
	flags1.drawBBox3D = 1;
	flags1.visualAlphaEnabled = 1;

	SetCollDetStat(0);
	SetCollDetDyn(0);
	SetPhysicsEnabled(0);

	bsphere3D.radius = std::numeric_limits<float>::max();
}

void zCVob::~zCVob()
{
	if ( refCtr > 0 )
		zFAULT("D: tried to delete zCVob with refCtr>0 !"); // 402

	if ( homeWorld )
		homeWorld->RemoveVob(this);

	if ( globalVobTreeNode ) {
		// maybe just Delete(globalVobTreeNode);
		globalVobTreeNode->RemoveSubtree();
		Delete(globalVobTreeNode);
	}

	Release(visual);
	Release(callback_ai);
	Release(eventManager);
	Delete(vobPresetName);
	Delete(rigidBody);
	Delete(collisionObject);
}

void zCVob::ArchiveVerbose(zCArchiver& arc)
{
	arc.WriteGroupBegin("Internals");

	arc.WriteInt("pack", 0);

	arc.WriteString("presetName", vobPresetName ? *vobPresetName : ""_s);

	arc.WriteRawFloat("bbox3DWS", bbox3D, 24);

	arc.WriteRaw("trafoOSToWSRot", trafoObjToWorld.ExtractRotation());
	arc.WriteVec3("trafoOSToWSPos", trafoOSToWSPos.GetTranslation());
	arc.WriteGroupEnd("Internals");

	arc.WriteGroupBegin("Vob");

	arc.WriteString("vobName", GetObjectName());

	arc.WriteString("visual", visual ? visual->GetVisualName() : "");

	arc.WriteBool("showVisual", flags.showVisual);

	arc.WriteEnum("visualCamAlign", "NONE;YAW;FULL", visualCamAlign);
	arc.WriteEnum("visualAniMode", "NONE;WIND;WIND2", aniMode);

	arc.WriteFloat("visualAniModeStrength", aniModeStrength);
	arc.WriteFloat("vobFarClipZScale", vobFarClipZScale);

	arc.WriteBool("cdStatic", flags1.collDetectionStatic);
	arc.WriteBool("cdDyn", flags1.collDetectionDynamic);
	arc.WriteBool("staticVob", flags1.staticVob);
	arc.WriteEnum("dynShadow", "DS_NONE;DS_BLOB", flags1.castDynShadow);

	arc.WriteInt("zbias", zBias);
	arc.WriteBool("isAmbient", flags3.isAmbientVob);
	arc.WriteGroupEnd("Vob");

	arc.WriteObject("visual", visual);
	arc.WriteObject("ai", callback_ai);

	if ( arc.InSaveGame() ) {
		arc.WriteObject(eventManager);
		arc.WriteBool("physicsEnabled", flags1.physicsEnabled);
	}
}

void zCVob::ArchivePacked(zCArchiver& arc)
{
	char dataPacked[83];

	arc.WriteInt("pack", 1);

	aw::copy_raw(bbox3D, &dataPacked[0]);

	auto pos = trafoObjToWorld.GetTranslation();
	aw::copy_raw(pos[0][3], dataPacked[ 24 ]);
	aw::copy_raw(pos[1][3], dataPacked[ 28 ]);
	aw::copy_raw(pos[2][3], dataPacked[ 32 ]);

	auto rot = trafoObjToWorld.ExtractRotation();
	aw::copy_raw(rot[0][0], dataPacked[ 36 ]);
	aw::copy_raw(rot[0][1], dataPacked[ 40 ]);
	aw::copy_raw(rot[0][2], dataPacked[ 44 ]);
	aw::copy_raw(rot[1][0], dataPacked[ 48 ]);
	aw::copy_raw(rot[1][1], dataPacked[ 52 ]);
	aw::copy_raw(rot[1][2], dataPacked[ 56 ]);
	aw::copy_raw(rot[2][0], dataPacked[ 60 ]);
	aw::copy_raw(rot[2][1], dataPacked[ 64 ]);
	aw::copy_raw(rot[2][2], dataPacked[ 68 ]);

	dataPacked[72] = 0;

	dataPacked[72] |= flags1.showVisual             * 1;
	dataPacked[72] |= (visualCamAlign & 3)          * 2;
	dataPacked[72] |= (flags1.collDetectionStatic)  * 8;
	dataPacked[72] |= (flags1.collDetectionDynamic) * 16;
	dataPacked[72] |= (flags1.ignoredByTraceRay)    * 32;
	dataPacked[72] |= (flags1.castDynShadow)        * 64;

	bool has_preset  = vobPresetName && vobPresetName->Length() > 0;
	bool has_vobname = !GetObjectName().IsEmpty();
	bool has_visname = !GetVisualName().IsEmpty();
	bool has_visual  = visual       != 0;
	bool has_ai      = callback_ai  != 0;
	bool has_em      = eventManager != 0;

	dataPacked[73]  = 0;
	dataPacked[73] |= has_preset   * 0x01;
	dataPacked[73] |= has_vobname  * 0x02;
	dataPacked[73] |= has_visname  * 0x04;
	dataPacked[73] |= has_visual   * 0x08;
	dataPacked[73] |= has_ai       * 0x10;
	dataPacked[73] |= has_em       * 0x20;

	dataPacked[74]  = 0;
	dataPacked[74] |= (aniMode & 3)       * 0x01;
	dataPacked[74] |= (zBias & 0x1F)      * 0x02;
	dataPacked[74] |= flags3.isAmbientVob * 0x40;

	aw::copy_raw(aniModeStrength, &dataPacked[75]);
	aw::copy_raw(vobFarClipZScale, &dataPacked[79]);

	arc.WriteRaw("dataRaw", dataPacked, 83);

	if ( dataPacked[73] & 1 )
		arc.WriteString("presetName", *vobPresetName);
	if ( dataPacked[73] & 2 )
		arc.WriteString("vobName", GetObjectName());
	if ( dataPacked[73] & 4 )
		arc.WriteString("visual", GetVisualName());
	if ( dataPacked[73] & 8 )
		arc.WriteObject("visual", visual);
	if ( dataPacked[73] & 0x10 )
		arc.WriteObject("ai", callback_ai);
	if ( arc.InSaveGame() && dataPacked[73] & 0x20 )
		arc.WriteObject(eventManager);

}

void zCVob::UnarchiveVerbose(zCArchiver& arc)
{
	arc.ReadString("presetName");

	arc.ReadRawFloat("bbox3DWS", bbox3D, 24);

	zMAT3 rot;
	zVEC3 pos;
	arc.ReadRaw("trafoOSToWSRot", rot, 36);
	arc.ReadVec3("trafoOSToWSPos", pos);

	trafoObjToWorld.SetRotation(rot);
	trafoObjToWorld.SetTranslation(pos);

	if (!homeWorld) {
		if ( trafo )
			trafo = trafoObjToWorld;
	}

	auto name = arc.ReadString("vobName");

	if ( homeWorld )
		homeWorld->RemoveVobHashTable(this);

	SetObjectName(name);

	if ( homeWorld )
		homeWorld->InsertVobHashTable(this);

	auto visName = arc.ReadString("visual");
	if (visName.Length() > 0) {
		auto tmp = zCMaterial::s_matUsageDefault;
		if ( this->type == 3 )
			zCMaterial::s_matUsageDefault = 0;

		SetVisual(visName);
		zCMaterial::s_matUsageDefault = tmp;

		auto pfx = zDYNAMIC_CAST<zCParticleFX>(visual);
		if ( pfx ) {
			bbox3D.mins[0] = trafoObjToWorld[0][3];
			bbox3D.mins[0] = trafoObjToWorld[0][3];
			bbox3D.mins[1] = trafoObjToWorld[1][3];
			bbox3D.maxs[1] = trafoObjToWorld[1][3];
			bbox3D.mins[2] = trafoObjToWorld[2][3];
			bbox3D.maxs[2] = trafoObjToWorld[2][3];
		}
	}


	flags1.showVisual = arc.ReadBool("showVisual");

	visualCamAlign = arc.ReadEnum("visualCamAlign");
	aniMode = arc.ReadEnum("visualAniMode");

	aniModeStrength  = arc.ReadFloat("visualAniModeStrength");
	vobFarClipZScale = arc.ReadFloat("vobFarClipZScale");

	SetCollDetStat(arc.ReadBool("cdStatic"));
	SetCollDetDyn(arc.ReadBool("cdDyn"));

	flags1.staticVob = arc.ReadBool("staticVob");
	flags1.dynShadow = arc.ReadEnum("dynShadow");

	arc.ReadInt("zbias", zBias);

	flags3.isAmbientVob = arc.ReadBool("isAmbient");

	auto obj1 = arc.ReadObject("visual", visual);
	auto obj2 = arc.ReadObject("ai", callback_ai);

	if (!visual) {
		SetVisual(static_cast<zCVisual*>(obj1));

		obj1->Release();
	}

	if ( !callback_ai ) {
		if (obj2) {
			callback_ai = obj2;
			obj2->AddRef();
			obj2->Release();
		}
	}


	if ( visual ) {
		if (zDYNAMIC_CAST<zCParticleFX>(visual))
			visual = zDYNAMIC_CAST<zCParticleFX>(visual);
	}

	if ( arc.InSaveGame() ) {
		eventManager = zfactory->CreateEventManager(this);
		auto obj = arc.ReadObject(eventManager);
		if (!obj)
			Release(eventManager);

		eventManager = static_cast<zCEventManager*>(obj);

		SetPhysicsEnabled(arc.ReadBool("physicsEnabled"));
	}
}

void zCVob::UnarchivePacked(zCArchiver& arc)
{
	char dataPacked[83];

	arc.ReadRaw("dataRaw", dataPacked, 83);

	memcpy(&bbox3D, dataPacked, sizeof(bbox3D));

	zMAT4 mat;

	mat[0][3] = aw::reinterpret<float>(dataPacked[ 24 ]);
	mat[1][3] = aw::reinterpret<float>(dataPacked[ 28 ]);
	mat[2][3] = aw::reinterpret<float>(dataPacked[ 32 ]);

	mat[0][0] = aw::reinterpret<float>(dataPacked[ 36 ]);
	mat[0][1] = aw::reinterpret<float>(dataPacked[ 40 ]);
	mat[0][2] = aw::reinterpret<float>(dataPacked[ 44 ]);
	mat[1][0] = aw::reinterpret<float>(dataPacked[ 48 ]);
	mat[1][1] = aw::reinterpret<float>(dataPacked[ 52 ]);
	mat[1][2] = aw::reinterpret<float>(dataPacked[ 56 ]);
	mat[2][0] = aw::reinterpret<float>(dataPacked[ 60 ]);
	mat[2][1] = aw::reinterpret<float>(dataPacked[ 64 ]);
	mat[2][2] = aw::reinterpret<float>(dataPacked[ 68 ]);

	trafoObjToWorld = mat;

	if (!homeWorld) {
		if ( trafo )
			trafo = trafoObjToWorld;
	}

	auto flags_pack1 = aw::reinterpret<int>(dataPacked[72]);
	auto flags_pack2 = aw::reinterpret<int>(dataPacked[73]);
	auto flags_pack3 = aw::reinterpret<int>(dataPacked[74]);

	if ( flags_pack2 & 1 ) {
		arc.ReadString("presetName");
	}

	if ( flags_pack2 & 2 ) {
		auto name = arc.ReadString("vobName");

		if ( homeWorld )
			homeWorld->RemoveVobHashTable(this);

		SetObjectName(name);

		if ( homeWorld )
			homeWorld->InsertVobHashTable(this);
	}

	if ( flags_pack2 & 4 ) {
		auto visName = arc.ReadString("visual");
		if (visName.Length() > 0) {
			auto tmp = zCMaterial::s_matUsageDefault;
			if ( this->type == 3 )
				zCMaterial::s_matUsageDefault = 0;

			SetVisual(visName);
			zCMaterial::s_matUsageDefault = tmp;

			auto pfx = zDYNAMIC_CAST<zCParticleFX>(visual);
			if ( pfx ) {
				bbox3D.mins[0] = trafoObjToWorld[0][3];
				bbox3D.mins[0] = trafoObjToWorld[0][3];
				bbox3D.mins[1] = trafoObjToWorld[1][3];
				bbox3D.maxs[1] = trafoObjToWorld[1][3];
				bbox3D.mins[2] = trafoObjToWorld[2][3];
				bbox3D.maxs[2] = trafoObjToWorld[2][3];
			}
		}
	}

	flags1.showVisual        = (flags_pack1 >> 0) & 1;
	visualCamAlign           = (flags_pack1 >> 1) & 3;
	SetCollDetStat           ( (flags_pack1 >> 3) & 1);
	SetCollDetDyn            ( (flags_pack1 >> 4) & 1);
	flags1.ignoredByTraceRay = (flags_pack1 >> 5) & 1;
	flags1.castDynShadow     = (flags_pack1 >> 6) & 3;

	aniMode             = (flags_pack3 >> 0) & 3;
	zBias               = (flags_pack3 >> 2) & 0x1F;
	flags3.isAmbientVob = (flags_pack3 >> 7) & 1;

	aniModeStrength  = aw::reinterpret<int>(dataPacked[75]);
	vobFarClipZScale = aw::reinterpret<int>(dataPacked[79]);

	if ( flags_pack2 & 8 ) {
		auto obj = arc.ReadObject("visual", visual);
		if (!visual) {
			SetVisual(static_cast<zCVisual*>(obj));

			obj->Release();
		}
	}

	if ( flags_pack2 & 0x10 ) {
		auto obj = arc.ReadObject("ai", callback_ai);
		if ( !callback_ai ) {
			if (obj) {
				callback_ai = obj;
				obj->AddRef();
				// in assembly it looks like Release()
				// but uses inc instead of dec
			}
		}
	}

	if ( visual ) {
		if (zDYNAMIC_CAST<zCParticleFX>(visual))
			visual = zDYNAMIC_CAST<zCParticleFX>(visual);
	}

	if ( arc.InSaveGame() ) {
		if ( flags_pack2 & 0x20 ) {
			if ( eventManager ) {
				eventManager = arc.ReadObject(eventManager);
			} else {
				eventManager = zfactory->CreateEventManager(this);
				auto obj = arc.ReadObject(eventManager);
				eventManager = static_cast<zCEventManager*>(obj);
				Release(obj);
			}
		}

		SetPhysicsEnabled((flags_pack2 >> 6) & 1);
	}
}

void zCVob::Archive(zCArchiver& arc)
{
	if ( arc.InSaveGame() || arc.InBinaryMode() )
		ArchivePacked(arc);
	else
		ArchiveVerbose(arc);

	if ( arc.InSaveGame() ) {
		arc.WriteByte("sleepMode", flags.sleepingMode);

		float nextOnTimer = this->nextOnTimer;
		if ( nextOnTimer < std::numeric_limits<float>::max) // 3.4028235e38
			nextOnTimer -= _unk_starttime;

		arc.WriteFloat("nextOnTimer", nextOnTimer);

		if ( flags.physicsEnabled )
			GetRigidBody()->Archive(arc);
	}
}

void zCVob::Unarchive(zCArchiver& arc)
{
	nextOnTimer = std::numeric_limits<float>::max;

	int pack = 0;
	arc.ReadInt("pack", pack);

	if ( pack )
		UnarchivePacked(arc);
	else
		UnarchiveVerbose(arc);

	if ( arc.InSaveGame() ) {
		int sleepMode = arc.ReadByte("sleepMode");
		SetSleepingMode(sleepMode);

		arc.ReadFloat("nextOnTimer", nextOnTimer);
		if (nextOnTimer < std::numeric_limits<float>::max)
			nextOnTimer += _unk_starttime;

		if ( flags.physicsEnabled )
			GetRigidBody()->Unarchive(arc);
	}

	if ( visual ) {
		if ( !isInMovementMode )
			BeginMovement();

		if ( visual->IsBBox3DLocal() ) {
			zTBBox3D trans;
			visual->GetBBox3D()->Transform(trafoObjToWorld, trans)
		} else {
			bbox3D = visual->GetBBox3D();
			TouchMovement();
		}

		if ( !isInMovementMode )
			EndMovement(1);
	}
}

void zCVob::PackStateBinary(zCBuffer *buffer)
{
	buffer->SetMode(1);
	auto pos = trafoObjToWorld.GetTranslation();
	auto rot = trafoObjToWorld.GetEulerAngles();
	buffer->WriteFloat(pos.x, 32);
	buffer->WriteFloat(pos.y, 32);
	buffer->WriteFloat(pos.z, 32);
	buffer->WriteFloat(rot.x, 32);
	buffer->WriteFloat(rot.y, 32);
	buffer->WriteFloat(rot.z, 32);
}

void zCVob::UnpackStateBinary(zCBuffer *buffer)
{
	zCBuffer::SetMode(buffer, 0);

	zVEC3 pos, rot;
	pos.x = zCBuffer::ReadFloat(buffer, 32);
	pos.y = zCBuffer::ReadFloat(buffer, 32);
	pos.z = zCBuffer::ReadFloat(buffer, 32);
	rot.x = zCBuffer::ReadFloat(buffer, 32);
	rot.y = zCBuffer::ReadFloat(buffer, 32);
	rot.z = zCBuffer::ReadFloat(buffer, 32);

	BeginMovement();

	SetPositionWorld(pos);
	trafoObjToWorld.SetByEulerAngles(rot);
	SetTrafoObjToWorld(trafoObjToWorld);

	EndMovement(1);
}

void zCVob::SetBBox3DWorld(zTBBox3D const& bbox)
{
	if ( !visual ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		this->bbox3D = bbox;
		collisionObject->flags |= 1u;

		if (!movmode)
			EndMovement(0);
	}
}

void zCVob::SetBBox3DLocal(zTBBox3D const& bbox)
{
	if ( !visual ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		bbox3D = bbox;
		bbox3d.mins += GetPositionWorld();
		bbox3d.maxs += GetPositionWorld();

		collisionObject->flags |= 1u;

		if (!movmode)
			EndMovement(0);
	}
}

zTBBox3D zCVob::GetBBox3DLocal()
{
	if (visual && visual->IsBBox3DLocal())
		return visual->GetBBox3D();

	auto bb = bbox;
	bb.mins -= trafoObjToWorld.GetTranslation();
	bb.maxs -= trafoObjToWorld.GetTranslation();
	return bb;
}

void zCVob::SetTrafoObjToWorld(zMAT4 const& intrafo)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	collisionObject->trafoObjToWorld = intrafo;
	collisionObject->flags |= 1u;
	collisionObject->flags |= 2u;

	if (!movmode)
		EndMovement(1);
}

void zCVob::SetTrafo(zMAT4 const& newTrafo)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto parent = globalVobTreeNode->parent;
	if (parent && parent->data) {
		auto mat = parent->data->trafoObjToWorld * newTrafo;
		collisionObject->trafoObjToWorld = mat;
	} else {
		collisionObject->trafoObjToWorld = newTrafo;
	}

	collisionObject->flags |= 1u;
	collisionObject->flags |= 2u;

	if (!movmode)
		EndMovement(1);
}

void zCVob::SetPositionWorld(zVEC3 const& pos)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	collisionObject->trafoObjToWorld.SetTranslation(pos);
	collisionObject->flags |= 1u;

	if (!movmode)
		EndMovement(1);
}

void zCVob::SetPositionLocal(zVEC3 const& vec)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto inv = collisionObject->trafoObjToWorld.Inverse();
	collisionObject->trafoObjToWorld.SetTranslation(vec * inv);
	collisionObject->flags |= 1u;

	if (!movmode)
		EndMovement(1);
}

void zCVob::SetHeadingWorld(zVEC3 const& vec)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject.trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtWorld(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::SetHeadingLocal(zVEC3 const& vec)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject->trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtLocal(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::SetHeadingYLocal(zVEC3 const& targetPos)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject->trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtLocal(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}

	ResetXZRotationsLocal();
}

zVEC3 zCVob::GetPositionWorld() const
{
	return {trafoObjToWorld[2][3], trafoObjToWorld[1][3], trafoObjToWorld[0][3]};
}

zVEC3 zCVob::GetAtVectorWorld() const
{
	return {trafoObjToWorld[2][2], trafoObjToWorld[1][2], trafoObjToWorld[0][2]};
}

zVEC3 zCVob::GetUpVectorWorld() const
{
	return {trafoObjToWorld[2][1], trafoObjToWorld[1][1], trafoObjToWorld[0][1]};
}

zVEC3 zCVob::GetRightVectorWorld() const
{
	return {trafoObjToWorld[2][0], trafoObjToWorld[1][0], trafoObjToWorld[0][0]};
}

void zCVob::GetPositionWorld(float& x, float& y, float& z)
{
	x = trafoObjToWorld.m[0][3];
	y = trafoObjToWorld.m[1][3];
	z = trafoObjToWorld.m[2][3];
}

void zCVob::GetPositionLocal(zCVob *this, float& x, float& y, float& z)
{
	if ( !trafo )
		CreateTrafoLocal();

	x = (*trafo)[0][3];
	y = (*trafo)[1][3];
	z = (*trafo)[2][3];
}

void zCVob::SetRotationWorld(zCQuat const& quat)
{
	int movmode = isInMovementMode;
	if ( !movmode )
		BeginMovement();

	quat.QuatToMatrix4(collisionObject->trafoObjToWorld);
	collisionObject->flags |= 2u;

	if ( !movmode )
		EndMovement(1);
}


void zCVob::ResetRotationsWorld()
{
	auto movmode = isInMovementMode;
	if ( !movmode )
		BeginMovement();

	auto pos = GetPositionWorld();
	collisionObject->trafoObjToWorld = zMAT4::s_identity;
	collisionObject->trafoObjToWorld.SetTranslation(pos); // didn't look up, was inlined

	collisionObject->flags |= 2u;

	if ( !movmode )
		EndMovement(1);
}

void zCVob::UpdateVisualDependencies(int calcGround)
{
	if ( visual ) {
		auto movmode = isInMovementMode;
		if ( !movmode )
			BeginMovement();

		bool isLocal = visual->IsBBox3DLocal();
		if ( isLocal ) {
			auto bbox = visual->GetBBox3D();
			bbox.Transform(this->trafoObjToWorld, this->bbox3D);
		} else {
			this->bbox3d = visual->GetBBox3D();
			TouchMovement();
		}

		if ( !movmode )
			EndMovement(1);
	}
}

zCEventManager* zCVob::GetEM(int dontCreate)
{
	zCEventManager *result = eventManager;
	if ( !result && !doNotCreate ) {
		result = (*zfactory)->CreateEventManager(this);
		this->eventManager = result;
	}
	return result;
}

void zCVob::ThisVobAddedToWorld(zCWorld* world)
{
	if ( visual )
		visual->HostVobAddedToWorld(this, world);
}

void zCVob::ThisVobRemovedFromWorld(zCWorld* world)
{
	if ( visual )
		visual->HostVobRemovedFromWorld(this, world);
}

void zCVob::RemoveVobFromWorld()
{
	if ( homeWorld )
		homeWorld->RemoveVob(this);
}

void zCVob::RemoveVobSubtreeFromWorld()
{
	if ( homeWorld )
		homeWorld->RemoveVobSubtree(this);
}

void zCVob::RemoveVobFromBspTree()
{
	if ( homeWorld )
		homeWorld->bspTree.RemoveVob(this);
}

void zCVob::ReleaseVobSubtree(zCTree<zCVob>* node)
{
	if ( node || node = globalVobTreeNode) {
		auto child = node->child;

		for (; child; child = child->next) {
			ReleaseVobSubtree(child);

		Release(node->data);
	}
}

void zCVob::AddRefVobSubtree(zCTree<zCVob>* node, int addTree)
{
	if (node || node = globalVobTreeNoe) {
		auto data = node->data;
		if ( data ) {
			if ( addTree ) {
				if (auto light = zDYNAMIC_CAST<zCVobLight>(data)) {
					light->lightData.flags.isStatic = 1
					// flags = flags & 0xFE ^ 1;
				}
			}
			data->AddRef();
		}

		for (auto i = node->firstChild; i; i = i->next )
			AddRefVobSubtree(i, 1);
	}
}

void zCVob::RemoveVobFromGlobalVobTree()
{
	if (!globalVobTreeNode)
		return;

	auto node = globalVobTreeNode->firstChild;
	for (; node; node = node->firstChild)
		// I reordered to make code look neater
		// // I reordered to make code look neater
		auto parent = globalVobTreeNode->parent;

		node->data->DoneWithTrafoLocal();
		node->RemoveSubtree();

		if (parent)
			parent->AddChild(node);
		else
			node->parent = 0;

		globalVobTreeNode->parent;
	}

	if (globalVobTreeNode)
		delete globalVobTreeNode;

	globalVobTreeNode = 0;
}

void zCVob::SetVobName(zSTRING const& name)
{
	if ( homeWorld )
		homeWorld->RemoveVobHashTable(this);

	SetObjectName(name);

	if ( homeWorld )
		homeWorld->InsertVobHashTable(this);
}

void zCVob::DoneWithTrafoLocal(zCVob *this)
{
	if ( trafo ) {
		if (!HasParentVob()) {
			trafoObjToWorld = trafo;
			Delete(trafo);
		}
	}
}

void zCVob::HasParentVob() const
{
	if ( globalVobTreeNode ) {
		auto parent = globalVobTreeNode->parent;
		if ( parent ) {
			if ( parent->data )
				return true;
		}
	}
	return false;
}

void zCVob::AddVobToWorld_CorrectParentDependencies()
{
	if (HasParentVob()) // was inlined
		CreateTrafoLocal();
}

void zCVob::CreateTrafoLocal()
{
	if ( !trafo )
		trafo = new zMAT4{};

	CorrectTrafo(); // was inlined
}

void zCVob::CorrectTrafo()
{
	if ( trafo ) {
		auto parent = globalVobTreeNode->parent;
		if (parent && parent->data) {
			auto& ptrafo = parent->data->trafoObjToWorld;
			*trafo = ptrafo.InverseLinTrafo() * trafoObjToWorld
		} else {
			*trafo = trafoObjToWorld;
		}
	}
}

void zCVob::SetCollisionClass(zCCollisionObjectDef* collClass)
{
	if (collClass != collisionObjectClass) {
		if ( collisionObject && collClass )
			SetCollisionObject(nullptr);
	}
	collisionObjectClass = collClass;
}

void zCVob::SetCollisionObject(zCCollisionObject* col)
{
	if ( collisionObject )
		delete collisionObject;
	collisionObject = col;
}

void zCVob::CreateCollisionObject()
{
	if ( !collisionObject ) {
		collisionObject = collisionObjectClass->CreateNewInstance();
		collisionObject->parent = this;
	}
}

void zCVob::DestroyCollisionObject(int force)
{
	auto cc = collisionObjectClass;
	if ( cc && (force || cc->isVolatile) )
		Delete(collisionObject);
}

void zCVob::ResetCollisionObjectState()
{
	collisionObject->trafo = trafoObjToWorld;
	collisionObject->trafoObjToWorld = trafoObjToWorld;
	collisionObject->flags &= 0xFCu;
}

void zCVob::CollectCollisionContext(zTCollisionContext& context)
{
	zTBBox3D bbox1;
	collisionObject->GetLargestBBox3DLocal(bbox1);

	zTBBox3D bbox2;
	bbox1.Transform(collisionObject->trafo, bbox2);
	bbox1.Transform(collisionObject->trafoObjToWorld, bbox1);

	bbox2.CalcGreaterBBox3D(bbox1);

	context->collObjects.DeleteList(); // не помню имя метода

	CollectCollisionContext_Vobs(bbox2, context);
	CollectCollisionContext_Static(bbox2, context);
}

void zCVob::CleanupCollisionContext(zTCollisionContext const& context)
{
	for (auto& vob : context.vobList) {
		if (!vob->isInMovementMode)
			vob->DestroyCollisionObject(1);
		Release(vob);
	}
}

void zCVob::CalcWaterVob()
{
	if ( !GetCharacterClass() && !flags3.dontWriteIntoArchive) {
		zVEC3 start{
			(bbox3D.maxs[0] + bbox3D.mins[0]) * 0.5,
			 bbox3D.maxs[1]
			(bbox3D.maxs[2] + bbox3D.mins[2]) * 0.5
		};
		zVEC3 ray{0, 20000.0, 0};

		homeWorld->TraceRayNearestHit(start, ray, 0, 0x222);

		flags3.isInWater = !!homeWorld->foundPoly;
	}
}

zCRigidBody* zCVob::GetRigidBody()
{
	if ( !rigidBody )
		rigidBody = new zCRigidBody();
	return rigidBody;
}

// private
void zCVob::NotifyCollisionPair(zCCollisionReport *report, zCArray<zCCollisionReport*> const& collReportList, int simulateFurtherDummy)
{
	auto col1 = report->col1;
	auto col2 = report->col2;
	auto vob1 = col1->parent;
	auto vob2 = col2->parent;

	if ( col1->GetCollObjClass() == &zCCollObjectLevelPolys::s_oCollObjClass ) {
		vob1->GetEM()->OnTouchLevel();
	}

	if ( vob1 && vob2 ) {
		zCVob* vob = vob1;
		if ( vob1 == this )
			vob = vob2;

		int dummy;
		vob->collisionObject->CollisionResponse(collReportList, dummy);

		if ( !simulateFurtherDummy ) {
			vob1->GetEM()->OnTouch(vob2);
			vob2->GetEM()->OnTouch(vob1);
			vob2->GetEM()->OnUntouch(vob1);
			vob1->GetEM()->OnUntouch(vob2);
		}
	}

	if ( vob1 && vob1->callback_ai )
		vob1->callback_ai->ReportCollisionToAI(report);

	if ( vob2 && vob2->callback_ai )
		vob2->callback_ai->ReportCollisionToAI(report);
}

// private
void zCVob::ExecuteCollisionResponse(zCArray<zCCollisionReport*> const& collReportList, int& simulateFurther)
{
	int sf = simulateFurther;
	simulateFurther = 0;

	collReportList[0]->col1->CollisionResponse(collReportList, sf);

	response.field_0 = simulateFurther;
	std::string::_Tidy(&response, 0);
	std::string::assign(&response, "response", strlen("response"));
	std::string::_Tidy(&response, 1);
	v7 = 0;
	simulateFurther = 0;

	for (auto& rep : collReportList) {
		auto vob1 = rep->col1->parent;
		auto vob2 = rep->col2->parent;
		if ( vob1 && vob2 ) {
			if ( vob1 != this )
				vob2 = vob1;

			vob2->collisionObject->CollisionResponse(collReportList, simulateFurther);
		}

		NotifyCollisionPair(rep, collReportList, 0);
	}

	flags2.mbHintTrafoLocalConst = true;
}

void zCVob::SetInMovement(int inMovement)
{
	SetInMovementMode(inMovement != 0);
}

void zCVob::SetInMovementMode(zCVob::zTMovementMode mode)
{
	if ( isInMovementMode == 2 || mode == 2 ) {
		isInMovementMode = mode;
	} else {
		isInMovementMode = mode;
		zCVob::s_vobAlreadyInMovement = mode == 1;
	}
}

void zCVob::ResetToOldMovementState();
{
	if ( isInMovementMode ) {
		collisionObject->trafoObjToWorld = trafoObjToWorld;
		collisionObject->flags &= 0xFCu;
	}
}

zCOLOR zCVob::GetLightColorStatGroundPoly()
{
	zCOLOR unused;
	if (flags1.visualAlphaEnabled)
		 unused = GetLightColorStat();

	return lightColorStat;
}

zCOLOR zCVob::CalcLightSampleAtOrigin()
{
	if ( homeWorld ) {
		auto sta = GetLightColorStat();
		auto dyn = GetLightColorDyn();

		// dunno what's going on, some kind of
		// optimization?
		auto what = (dyn & 0xFEFEFF) + (sta & 0xFEFEFF);
		auto what2 = what & 0x1010100;

		return what | (what2 - (what2 >> 8));
	}

	// RGBA, лень смотреть как на самом деле
	return {0,0,0,0xff};
}

void zCVob::SetOnTimer(zCVob *this, float deltaTime)
{
	nextOnTimer = ztimer.totalTimeFloat + deltaTime;

	if ( !flags2.sleepingMode )
		SetSleepingMode(2);
}

void zCVob::ProcessOnTimer()
{
	if ( ztimer.totalTimeFloat >= nextOnTimer ) {
		nextOnTimer = std::numeric_limits<float>::max();
		OnTimer();
	}
}

void zCVob::DeleteGroundShadowMesh()
{
	Release(zCVob::s_shadowMesh);
}

void zCVob::SetGroundShadowSize(zVEC2 const& dimensions)
{
	int x = dimensions->x * 8.0;
	int y = dimensions->y * 8.0;
	groundShadowSizePacked = x | (y << 16);
	return groundShadowSizePacked;
}

zVEC2 zCVob::GetGroundShadowSize()
{
	auto x = groundShadowSizePacked & 0xFFFF;
	auto y = (groundShadowSizePacked & 0xFFFF0000) >> 16;
	return {x * 0.125, y * 0.125};
}

zMAT4 zCVob::GetTrafoModelNodeToWorld(zCModelNodeInst* modelNode)
{
	if (auto model = zDYNAMIC_CAST<zCModel>(visual))
		return trafoObjToWorld * model->GetTrafoNodeToModel(modelNode);

	return trafoObjToWorld; // was weird loop
}

zMAT4 zCVob::GetTrafoNodeToModel(zSTRING const& modelNodeName)
{
	if (auto model = zDYNAMIC_CAST<zCModel>(visual)) {
		auto node = model->SearchNode(modelNodeName);
		if (node)
			return GetTrafoNodeToModel(node);
	}

	return trafoObjToWorld;
}

zCMover* zCVob::GetAutoLinkParent(zCVob* vob)
{
	if (!HasParentVob())
		return nullptr;

	auto tree = vob->globalVobTreeNode;
	auto parent = dynamic_cast<zCMover>(tree->parent->data);

	if (!parent->autoLinkEnabled)
		return nullptr;

	return parent;
}

// static
int zCVob::CanAutoLinkWith(zCVob *childVob, zCVob *parentVob)
{
	bool b = childVob->GetCharacterClass() && !(childVob->flags1.physicsEnabled) ||
	        !childVob->GetCharacterClass() &&   childVob->flags1.physicsEnabled;
	if (!b)
		return 0;
	if (!childVob->HasParentVob())
		return 0;

	if (childVob->flags1.staticVob)
		return 0;

	zVEC3 start = childVob->GetPositionWorld();
	start.y = childVob->bbox3D.maxs[1];

	zVEC3 ray{0, -100000.0, 0};
	if ( parentVob ) {
		zTTraceRayReport report;
		int hit = parentVob->TraceRay(start, ray, 0, report);
		return hit && report.result; // TODO: actual name
	}

	childVob->homeWorld->TraceRayNearestHit(start, ray, childVob, 17);
	if (auto mover = dynamic_cast<zCMover*>(childVob->homeWorld->foundVob))
		return mover->autoLinkEnabled;

	return 0;
}

void zCVob::SetAI(zCAIBase* newai)
{
	if ( callback_ai != newai ) {
		Release(callback_ai);
		callback_ai = newai;
		AddRef(newai);
	}
}

// private
void zCVob::AdoptCollObjResults()
{
	if ( collisionObject->flags & 3 ) {
		trafoObjToWorld = collisionObject->trafoObjToWorld;
		if (!flags2.mbHintTrafoLocalConst)
			CorrectTrafo();
	}

	DestroyCollisionObject(0);
}

zVEC3 zCVob::GetVelocity()
{
	if ( flags1.physicsEnabled)
		return GetRigidBody()->vel;

	if (auto model = zDYNAMIC_CAST<zCModel>(visual)) {
		auto vel = model->GetVelocityRing();

		return vel * trafoObjToWorld;
	}

	return {0.0f, 0.0f, 0.0f};
}

void zCVob::CalcDestinationBBox3DWorld(zTBBox3D *outbbox)
{
	if ( visual && visual->IsBBox3DLocal() ) {
		auto bbox = visual->GetBBox3D();
		bbox.Transform(collisionObject->trafoObjToWorld, outbbox);
	} else {
		auto vec1 = collisionObject->trafoObjToWorld.GetTranslation();
		auto vec2 = collisionObject->trafo.GetTranslation();
		zVEC3 tmp = vec1 - vec2;
		outbbox = bbox3d;
		outbbox.mins += tmp;
		outbbox.maxs += tmp;
	}
}

int zCVob::DetectCollision(zMAT4 *trafoObjToWorldNew)
{
	int old_flag = 0;
	if ( trafoObjToWorldNew ) {
		old_flag = flags2.sleepingMode;
		SetPhysicsEnabled(1); // was inlined
		SetSleepingMode(1);
	}

	BeginMovement();
	if ( traf )
		SetTrafoObjToWorld(*trafoObjToWorldNew); // inlined

	bool isColliding = IsColliding();
	if ( trafoObjToWorldNew && isInMovementMode) {
		collisionObject->trafoObjToWorld = trafoObjToWorld;
		collisionObject->flags &= 0xFCu;
	}

	EndMovement(1);
	if ( trafoObjToWorldNew ) {
		SetPhysicsEnabled(old_flag); // was inlined
		SetSleepingMode(old_flag);
	}

	return isColliding;
}
