// zCAIBase : zCObject
class zCAIPlayer : public zCAIBase {
	Z_OBJECT(zCAIPlayer);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCAIPlayer();
	virtual void DoAI(zCVob *,int &);
	virtual void HostVobRemovedFromWorld(zCVob*,zCWorld *);
	virtual void StartStandAni();
	virtual void StartFallDownAni();

	bool zCAIPlayer::IsSliding()
	{
		return zMV_DO_WALL_SLIDING &&
			vob->GetRigidBody()->bitfield & 4;
	}

	void zCAIPlayer::SetCDStat(uint32_t newcdStat)
	{
		if ( cdStatOriginal )
			cdStat = newcdStat;
		else
			cdStat = 0;

		vob->SetCollDetStat(cdStat);
	}

	zCModel* zCAIPlayer::GetModel()
	{
		return model;
	}

	int CheckEnoughSpaceMoveBackward(int dontDetectWalkStopChasm)
	{
		zMAT4 const& trafo = vob->trafoObjToWorld;
		float x = trafo[0][2]; // -GetHeadingWorld()?
		float y = trafo[1][2];
		float z = trafo[2][2];
		zVEC3 dir{-x,-y,-z};
		return CheckEnoughSpaceMoveDir(dir, dontDetectWalkStopChasm);
	}


private:
	// Config
	struct zTConfig {
		//Ungefähr das, was auch in der Species.d festgelegt wird.

		zREAL   zMV_MIN_SPACE_MOVE_FORWARD;
		zREAL   zMV_DETECT_CHASM_SCAN_AT;
		zREAL   zMV_STEP_HEIGHT;
		zREAL   zMV_JUMP_UP_MIN_CEIL;
		zREAL   zMV_WATER_DEPTH_KNEE;
		zREAL   zMV_WATER_DEPTH_CHEST;
		zREAL   zMV_YMAX_SWIM_CLIMB_OUT;
		zREAL   zMV_FORCE_JUMP_UP;
		zREAL   zMV_YMAX_JUMPLOW;
		zREAL   zMV_YMAX_JUMPMID;
		zREAL   zMV_MAX_GROUND_ANGLE_WALK;
		zREAL   zMV_MAX_GROUND_ANGLE_SLIDE;
		zREAL   zMV_MAX_GROUND_ANGLE_SLIDE2;
		zREAL   zMV_DCUL_WALL_HEADING_ANGLE;
		zREAL   zMV_DCUL_WALL_HORIZ_ANGLE;
		zREAL   zMV_DCUL_GROUND_ANGLE;
	} config;

	/*
	 * Nur weil es gerade einen Berg heraufgeht, heißt das nicht, dass die
	 * Npcs genauso schiefstehen wie der Untergrund schief ist. Zumindest
	 * ist das nicht bei allen Npcs so.
	 * Dieses enum einhaltet die Konstanten, die für die Verschiedenen
	 * möglichkeiten stehen.
	 */

	enum zTSurfaceAlignMode {
		zMV_SURFACE_ALIGN_NONE  = 0,                // kein Alignment (z.B. Menschen)
		zMV_SURFACE_ALIGN_NORMAL= 1,                // wie z.B. bei Schattenläufern
		zMV_SURFACE_ALIGN_HIGH  = 2,                // Meatbugs sind z.B. mit allen Beinen am Boden festgeklebt.
	};

	//Struktur mit Infos zu erreichbaren Kletterkanten.
	struct zTLedgeInfo {
		zVEC3 point;            // Position zum anpacken
		zVEC3 normal;           // ein Normalenvektor an dem sich die Figur ausrichten muss
		zVEC3 cont;
		zREAL maxMoveForward;

		zTLedgeInfo(zCAIPlayer const*)
			: point(0,0,0), normal(0,0,0), cont(0,0,0), maxMoveForward(0)
		{}
	};

	enum zTMovementState {                          // keep in sync with bitfield!
		zMV_STATE_STAND         = 0,
		zMV_STATE_FLY           = 1,
		zMV_STATE_SWIM          = 2,
		zMV_STATE_DIVE          = 3,
	};

	zCVob*   vob;
	zCModel* model;
	zCWorld* world;

	zPOINT3  centerPos;
	zREAL    feetY;
	zREAL    headY;
	zREAL    aboveFloor;

	//0: niedrig genug, 1: schon am Waten, 2: am Schwimmen
	int waterLevel;

	zREAL velocityLen2;
	zVEC3 velocity;

	zREAL fallDownDistanceY;
	zREAL fallDownStartY;

	zVEC3 slidePolyNormal;
	zREAL checkWaterCollBodyLen;
	zCModelNodeInst* modelHeadNode;

	//Zur Bedeutung siehe obenstehende Struktur.
	struct {    // 27 Bit
		// zTSurfaceAlignMode
		zUINT8          zMV_DO_SURFACE_ALIGN            : 2; // 1,2
		zUINT8          zMV_DO_DETECT_WALK_STOP_CHASM   : 1; // 4
		zUINT8          zMV_DO_WALL_SLIDING             : 1; // 8
		zUINT8          zMV_DO_HEIGHT_CORRECTION        : 1; // 16

		zUINT8          cdStatOriginal                  : 1;
		zUINT8          cdStat                          : 1;
		zUINT8          startFXModelLandDust            : 1;
		zUINT8          forceModelHalt                  : 1;
		zUINT8          autoRollEnabled                 : 1;
		zUINT8          modelAnisInPlace                : 1;
		zTMovementState oldState                        : 3;
		zTMovementState state                           : 3;
		zUINT8          oldWaterLevel                   : 2; //wieder 0, 1 oder 2 mit oben genannten Bedeutungen
		zCCollObjectCharacter::zEConfigState m_eCollObjectState     : 4;
		zCCollObjectCharacter::zEConfigState m_eCollObjectStateOld  : 4;
	}  flags2;


	//Bluteffekte
	struct zTBloodVobTrack {
		zCVob           *bloodVob;
		zREAL           alpha;
	};

	zCArray<zTBloodVobTrack>  bloodVobList;

	zREAL  bleedingPerc;
	zVEC3  bleedingLastPos;
	zREAL  bleedingNextDist;

	// Weapon Trail effects
	zCVob* weaponTrailVob;

	// WaterRing effects
	zCVob*  waterRingVob;
	zREAL   waterRingTimer;

	// autoRolling
	zREAL  autoRollPos;
	zREAL  autoRollPosDest;
	zREAL  autoRollSpeed;
	zREAL  autoRollMaxAngle;

	zBYTE dummyLastVar;
	zSTRING bloodDefaultTexName;
};

int zCAIPlayer::CreateLedgeInfo()
{
	void const* key = this;
	return ledgeCache[key];
}

auto zCAIPlayer::GetLedgeInfo() -> zTLedgeInfo*
{
	void const* key = this;
	if (ledgeCache.GetDataDangerous(key))
		return ledgeCache[key];

	return nullptr;
}

zCAIPlayer zCAIPlayer::zCAIPlayer()
{
	bleedingPerc = 0;
	bloodDefaultTexName = "ZBLOODSPLAT2.TGA";
	autoRollPos = 0;
	autoRollPosDest = 1.0;
	autoRollSpeed = 0.003;
	autoRollMaxAngle = 10.0;
	flags2.autoRollEnabled = 1;
	return this;
}

void zCAIPlayer::Archive(zCArchiver& arc)
{
	if (!arc.InSaveGame())
		return;

	if (!vob)
		return;

	auto col = vob->GetCollisionObject()

	if ( col ) {
		arc.WriteInt("waterLevel", waterLevel);
		arc.WriteFloat("floorY", floorY);
		arc.WriteFloat("waterY", waterY);
		arc.WriteFloat("ceilY", ceilY);
		arc.WriteFloat("feetY", feetY);
		arc.WriteFloat("headY", headY);
		arc.WriteFloat("fallDistY", fallDownDistanceY);
		arc.WriteFloat("fallStartY", fallDownStartY);
	}
}

virtual void Unarchive(zCArchiver& arc)
{
	if (!arc.InSaveGame())
		return;

	auto parent = zDYNAMIC_CAST<zCVob>(arc.GetParentObject());

	if (!parent)
		return;

	zCVob* tmp = this->vob;
	this->vob = parent;
	if ( !parent->GetCollisionObject() )
		parent->CreateCollisionObject();

	auto col = parent->GetCollisionObject();
	arc.ReadInt("waterLevel", waterLevel);
	arc.ReadFloat("floorY", col->floorY);
	arc.ReadFloat("waterY", col->waterY);
	arc.ReadFloat("ceilY",  col->ceilY);
	arc.ReadFloat("feetY", feetY);
	arc.ReadFloat("headY", headY);
	arc.ReadFloat("fallDistY",  fallDownDistanceY);
	arc.ReadFloat("fallStartY", fallDownStartY);
	this->vob = tmp;
}
