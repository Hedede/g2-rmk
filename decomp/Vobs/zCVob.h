//Offsets der X, Y, Z Positionen im trafoObjToWorld Array:
const int zCVob_trafoObjToWorld_X =  3;
const int zCVob_trafoObjToWorld_Y =  7;
const int zCVob_trafoObjToWorld_Z = 11;

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

	virtual ~zCVob();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

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

	zVEC3 GetPositionWorld() const
	{
		return {trafoObjToWorld[2][3],
		        trafoObjToWorld[1][3],
		        trafoObjToWorld[0][3]};
	}

	zVEC3 GetAtVectorWorld() const
	{
		return {trafoObjToWorld[2][2],
		        trafoObjToWorld[1][2],
		        trafoObjToWorld[0][2]};
	}

	zVEC3 GetUpVectorWorld() const
	{
		return {trafoObjToWorld[2][1],
		        trafoObjToWorld[1][1],
		        trafoObjToWorld[0][1]};
	}

	zVEC3 GetRightVectorWorld() const
	{
		return {trafoObjToWorld[2][0],
		        trafoObjToWorld[1][0],
		        trafoObjToWorld[0][0]};
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

	virtual void ThisVobAddedToWorld(zCWorld* world)
	{
		if ( visual )
			visual->HostVobAddedToWorld(this, world);
	}

	virtual void ThisVobRemovedFromWorld(zCWorld *);

	zCWorld* GetHomeWorld() const
	{
		return homeWorld;
	}

	void SetSleeping(int sleep)
	{
		if ( sleep )
			SetSleepingMode(0);
		else
			SetSleepingMode(1);
	}

	void GetEM(int doNotCreate = 0);

	zCCollisionObject* GetCollisionObject()
	{
		return collisionObject;
	}

	/* Unused */

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
	zCTree<zCVob>* globalVobTreeNode;

	zTFrameCtr lastTimeDrawn;
	int        lastTimeCollected;

	zCArray<zCBspLeaf*> vobLeafList;

	zMAT4 trafoObjToWorld;

	zTBBox3D    bbox3D;
	zTBSphere3D bsphere3D;

	zCArray<zCVob*> touchVobList;

	int type;
	int groundShadowSizePacked;

	zCWorld*   homeWorld;
	zCPolygon* groundPoly;

	zCAIBase* callback_ai;
	zMAT4*    trafo;
	zCVisual* visual;
	float     visualAlpha;
	float     vobFarClipZScale;

	int       aniMode;
	float     aniModeStrength;
	int       zBias;

	zCRigidBody* rigidBody;

	zCOLOR   lightColorStat;
	zCOLOR   lightColorDyn;
	zVEC3    lightDirectionStat;

	zSTRING* vobPresetName;

	zCEventManager* eventManager;

	float nextOnTimer;

	struct {
		uint32_t showVisual                : 1;
		uint32_t drawBBox3D                : 1;
		uint32_t visualAlphaEnabled        : 1;
		uint32_t physicsEnabled            : 1;
		uint32_t staticVob                 : 1;
		uint32_t ignoredByTraceRay         : 1;
		uint32_t collDetectionStatic       : 1;
		uint32_t collDetectionDynamic      : 1;
		uint32_t castDynShadow             : 2;
		uint32_t lightColorStatDirty       : 1;
		uint32_t lightColorDynDirty        : 1;
	} flags1;

	uint8_t isInMovementMode;

	struct {
		uint32_t sleepingMode              : 2;
		uint32_t mbHintTrafoLocalConst     : 1;
		uint32_t mbInsideEndMovementMethod : 1;
	} flags2;

	uint8_t visualCamAlign;

	struct {
		uint32_t collButNoMove            : 4;
		uint32_t dontWriteIntoArchive     : 1;
		uint32_t isInWater                : 1;
		uint32_t isAmbientVob             : 1;
	} flags3;

	zCCollisionObjectDef* collisionObjectClass;
	zCCollisionObject*    collisionObject;
};

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

zCEventManager* zCVob::GetEM(int doNotCreate)
{
	zCEventManager *result = eventManager;
	if ( !result && !doNotCreate )
	{
		result = (*zfactory)->CreateEventManager(this);
		this->eventManager = result;
	}
	return result;
}
