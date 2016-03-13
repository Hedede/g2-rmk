//Offsets der X, Y, Z Positionen im trafoObjToWorld Array:
const int zCVob_trafoObjToWorld_X =  3;
const int zCVob_trafoObjToWorld_Y =  7;
const int zCVob_trafoObjToWorld_Z = 11;

class zCVob : public zCObject {
	Z_OBJECT(zCObject);
public:
	static void SetVobID(uint32_t const&)
	{
	}

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	virtual ~zCVob();
	virtual void OnTrigger(zCVob *,zCVob *)
	{
	}
	virtual void OnUntrigger(zCVob *,zCVob *)
	{
	}
	virtual void OnTouch(zCVob *)
	{
	}
	virtual void OnUntouch(zCVob *)
	{
	}
	virtual void OnTouchLevel()
	{
	}
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &)
	{
	}
	virtual void OnMessage(zCEventMessage *,zCVob *)
	{
	}
	virtual void OnTick()
	{
	}
	virtual void OnTimer()
	{
	}
	virtual void PostLoad()
	{
	}
	virtual void GetCharacterClass(void);
	virtual void SetSleepingMode(zTVobSleepingMode);
	virtual void EndMovement(int);
	virtual void CanThisCollideWith(zCVob *);
	virtual void Render(zTRenderContext &);
	virtual void SetVisual(zSTRING const &);
	virtual void SetVisual(zCVisual *);

	virtual void GetScriptInstance(zSTRING*& outname, int& outidx)
	{
		outname = 0;
		outidx = 0;
		return 0;
	}

	virtual int SetByScriptInstance(zSTRING const *,int)
	{
		return 0;
	}

	virtual void GetCSStateFlags(void);
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void GetTriggerTarget(int);

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

	void GetEM(int doNotCreate);
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

	int bitfield[5];
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
		uint32_t collButNoMove             : 4;
		uint32_t dontWriteIntoArchive      : 1;
		uint32_t bIsInWater                : 1;
		uint32_t bIsAmbientVob             : 1;
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
