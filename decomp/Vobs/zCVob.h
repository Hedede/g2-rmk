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
