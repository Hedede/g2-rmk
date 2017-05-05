const int VFX_NUM_USERSTRINGS = 5;

class oCVisualFX : public zCEffect {
	Z_OBJECT(oCVisualFX);
private:
	static zCParser* fxParser;
public:
	static void InitParser();
	static oCVisualFX* CreateAndPlay(zSTRING const& nameVfx,
	               zCVob const* origin, zCVob const* target,
	               int level, float damage, int damageType, int isProjectile);

	virtual Archive(zCArchiver& arc)
	{
		zCEffect::Archive(arc);
	}
	virtual Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);
	}
	virtual ~oCVisualFX();

	virtual void OnTick();
	virtual void CanThisCollideWith(zCVob *);
	virtual void Open();
	virtual void SetOrigin(zCVob *,int);
	virtual void SetTarget(zVEC3 &,int);
	virtual void SetTarget(zCVob *,int);
	virtual void SetInflictor(zCVob *);
	virtual zCVob* GetOrigin() const
	{
		return origin;
	}
	virtual zCVob* GetTarget() const
	{
		return target;
	}
	virtual zCVob* GetInflictor() const
	{
		return inflictor;
	}
	virtual void Init(zCArray<zCVob *> const &);
	virtual void Init(zCVob const *,zCVob const *,zCVob const *);
	virtual void Init(zCVob const *,zVEC3 const &);
	virtual void InvestNext();

	virtual void SetLevel(int,int);
	virtual int GetLevel() { return bitfield.level; }

	virtual void Cast(int);
	virtual void Stop(int);
	virtual void Kill();
	virtual void Play(float keyCycleTime,zMAT4 const* orgTrafo, zMAT4 const* targetTrafo) {}

	virtual bool CanBeDeleted();
	virtual bool IsFinished();
	virtual bool IsLooping();

	virtual void SetByScript(zSTRING const &);
	virtual void SetDuration(float dur)
	{
		scripted.emFXLifeSpan = dur;
	}

	virtual void Reset();
	virtual void ResetForEditing();
	virtual void ReportCollision(zCCollisionReport const &);
	virtual void SetCollisionEnabled(int);
	virtual void SetCollisionCandidates(zCArray<zCVob *>);
	virtual void GetCollisionCandidates(zCArray<zCVob *> &);
	virtual int  GetNumCollisionCandidates()
	{
		return __collisionCandidates.GetNum();
	}

	virtual void GetCollidedCandidates(zCArray<zCVob *> &);

	virtual void SetDamage(float damage) { this->damage = damage; }
	virtual void SetDamageType(int damType) { this->damageType = damType; }

	virtual float GetDamage()     const { return damage; }
	virtual int   GetDamageType() const { return damageType; }

	virtual void IsASpell() const { return scripted.sendAssessMagic; }

	virtual void SetSpellType(int type) { spellType = type; }
	virtual int  GetSpellType() const { return spellType; }

	virtual void SetSpellCat(int cat) { spellCat = cat; }
	virtual void GetSpellCat() const { return spellCat; }

	virtual void SetSpellTargetTypes(int types) { targetTypes = types; }
	virtual void GetSpellTargetTypes() const { return targetTypes; }

	virtual void GetSendsAssessMagic();
	virtual void SetSendsAssessMagic(int);

	virtual zBOOL GetIsProjectile() const { return isProjectile; }
	virtual void SetIsProjectile(zBOOL b) { isProjectile = b; }

	virtual void SetVisualByString(zSTRING const &);
	virtual void CalcTrajectory(int const &);
	virtual void Collide(int);
	virtual void CollisionResponse(zVEC3 const &,int);

	zSTRING const& GetName() const { return name; }

	void CreateBackup();
	static void PreSaveGameProcessing(bool destroyAllObjects)
	{
		// TODO: gothic1
		// (g1 has some code, in g2 it is removed)
#ifdef G1
		oCVisualFX::RemoveInstancesOfClass(&oCVisualFX::classDef, destroyAllObjects);
		oCVisualFX::RemoveInstancesOfClass(&oCVisFX_Lightning::classDef, destroyAllObjects);
		oCVisualFX::RemoveInstancesOfClass(&oCVisFX_MultiTarget::classDef, destroyAllObjects);
#endif
  ;
	}
	static void PostSaveGameProcessing()
	{
		// TODO: gothic1
	}
private:
	struct {
		zSTRING visName;
		zSTRING visSize;
		float   visAlpha;
		zSTRING visAlphaBlendFunc;
		float   visTexAniFPS;
		int     visTexAniIsLooping;
		// none, target, line, spline, random
		zSTRING emTrjMode;
		zSTRING emTrjOriginNode;
		zSTRING emTrjTargetNode;
		float   emTrjTargetRange;
		float   emTrjTargetAzi;
		float   emTrjTargetElev;
		int     emTrjNumKeys;
		int     emTrjNumKeysVar;
		float   emTrjAngleElevVar;
		float   emTrjAngleHeadVar;
		float   emTrjKeyDistVar;
		zSTRING emTrjLoopMode;
		zSTRING emTrjEaseFunc;
		float   emTrjEaseVel;
		float   emTrjDynUpdateDelay;
		int     emTrjDynUpdateTargetOnly;
		zSTRING emFXCreate;
		zSTRING emFXInvestOrigin;
		zSTRING emFXInvestTarget;
		float   emFXTriggerDelay;
		int     emFXCreatedOwnTrj;
		// CREATE, BOUNCE, CREATEONCE, NORESP, COLLIDE
		zSTRING  emActionCollDyn;
		// CREATE, BOUNCE, CREATEONCE, NORESP, COLLIDE, CREATEQUAD
		zSTRING  emActionCollStat;
		zSTRING emFXCollStat;
		zSTRING emFXCollDyn;
		zSTRING  emFXCollDynPerc;
		// TRAJECTORY, COLLISIONNORMAL
		zSTRING emFXCollStatAlign;
		zSTRING emFXCollDynAlign;
		float   emFXLifeSpan;
		int     emCheckCollision;
		int     emAdjustShpToOrigin;
		float   emInvestNextKeyDuration;
		float   emFlyGravity;
		zSTRING emSelfRotVel;
		zSTRING userString[VFX_NUM_USERSTRINGS];
		zSTRING lightPresetName;
		zSTRING sfxID;
		int     sfxIsAmbient;
		int     sendAssessMagic;
		float   secsPerDamage;
	} scripted;

	int unk0;
	zVEC3 __visSize0;

	int __trjModeFlags;
	int colDynFlags;
	int colStatFlags;

	zVEC3 __rotVel;

	int __easeModeFunc;
	int __loopModeFlags;

	int __state;

	oCVisualFX *_vfx1;
	oCVisualFX *_vfx2;

	oCVisualFX* __investFxOrigin;
	oCVisualFX* __investFxTarget;

	zCAIBase *ai;

	zCArray<oCVisualFX*> __children;
	zCArray<oCVisualFX*> __children2;
	zCArray<oCEmitterKey*> __emitterKeys;

	zCArray<zCVob*> __someVobArr;
	zCArray<zCVob*> __ignoreCollision;
	zCArray<zCVob*> __collisionCandidates;
	zCArray<zCVob*> __collidedCandidates;
	zCArray<> __queuedCollisions;

	zCArray<zCPositionKey*> poskeys;

	int int2[3];
	zMAT4 __mat;
	int __ink3;
	zCVob *__unkvob1;
	zCVobScreenFX *blendEffect;
	float __blendTimer;

	zBOOL __aBool;

	zCModelNodeInst *__originNode;
	zCModelNodeInst *__targetNode;
	char unk2[4];
	zCVob *origin;
	zCVob *inflictor;
	zCVob *target;
	zCVobLight* __light;
	int what[1];
	zCSoundFX *sfxAmbient;
	zCSoundFX *sfx;
	zSTRING name;

	oCEmitterKey *__someKey1;
	oCEmitterKey *__someKey2;
	oCEmitterKey *activeKey;

	float __frameTimeSec;
	char unk3_1[4];
	float __something;
	float __damageTimer;

	zVEC3 targetPos;

	char unk4[12];
	zVEC3 __visSize1;
	zVEC3 __visSize2;
	zVEC3 vec4;

	float __investTimer;
	int unkno;
	float __delayTrigTimer;
	float __dynUpdatetimer;
	char unk4_0[4];
	float __unkflt;
	float __totalTime;
	float __fxTimer;
	char unk4_1[4];
	float fovx;
	float fovy;

	struct {
		unsigned unknown0     : 3;
		unsigned __finish     : 1; // 0x8
		unsigned unknown1     : 1; // 0x10
		unsigned canBeDeleted : 1; // 0x20
		unsigned unknown2 : 7;
		unsigned level    : 5; // bitfield << 14 >> 27
		unsigned unknown3 : 14;
	} bitfield;

	float damage;
	int damageType;
	int spellType;
	int spellCat;
	int targetTypes;

	int __emitter_ppsValue;
	zVEC2 __someVec2;

	zVEC3 vecs[10];
	char unk6[8];
	zBOOL isProjectile;
	zBOOL __someBool;
	zBOOL __someBool0;
	float __timeRelated;
};

oCVisualFX* oCVisualFX::CreateAndPlay(
                zSTRING const& nameVfx, zCVob const* origin, zCVob const* target,
                int level, float damage, int damageType, int isProjectile)
{
	zSTRING name = nameVfx;
	name.Upper();
	zSTRING::Upper(&name);
	if (name.IsEmpty())
		return nullptr;

	auto sym = fxParser->GetSymbol(name);
	if (!sym) {
		zWARNING("C: oCVisualFX::CreateAndPlay(): VisualFX ID " + nameVfx +
		         " not found in visualfxinst.d");
		return nullptr;
	}

	auto visFx = new oCVisualFX();
	visFx->SetIsProjectile(isProjectile);

	visFx->_vfx1 = visFx;
	visFx->_vfx2 = visFx;

	zSTRING lev = name + "_L" + level;
	if ( fxParser->GetSymbol(lev) )
		name = lev;

	visFx->SetByScript(name);
	visFx->SetDamage(damage);
	visFx->SetDamageType(damageType);
	visFx->SetLevel(level, 0);
	visFx->Init(origin, target, 0);
	visFx->Cast(1);

	return visFx;
}

// _carsten\\oVisFx.cpp
void oCVisualFX::InitParser()
{
	if ( oCVisualFX::fxParser ) {
		fxParser->Reset();
	} else {
		zINFO(5, "C: *** Visual FX Implementation " + "v0.5");
		fxParser = new zCParser(100);
	}

	bool oldEnable = zCParser::enableParsing;
	zCParser::enableParsing = oldEnable || zoptions->Parm("REPARSEVIS");
	zSTRING path;
	if ( zgameoptions )
		path = zgameoptions->ReadString(zOPT_SEC_FILES, "VisualEffects", 0);

	if ( !path )
		path = "System\\VisualFx";

	zINFO(4, "N: VFX: Loading file " + path + ".src or .dat"); // 863

	oCVisualFX::fxParser->Parse(path + ".src");
	oCVisualFX::fxParser->CreatePCode();

	oCEmitterKey key;

	oCVisualFX::fxParser->CheckClassSize("C_PARTICLEFXEMITKEY", 352);

	auto vfx = new oCVisualFX();
	Release(vfx)
	oCVisualFX::fxParser->CheckClassSize("CFX_BASE", 616);
	zCParser::enableParsing = oldEnable;
}


bool oCVisualFX::CanBeDeleted()
{
	if (!bitfield.canBeDeleted)
		return false;
	for (auto* vfx : __children) {
		if (!vfx->bitfield.canBeDeleted)
			return true;
	}

	return true;
}

bool oCVisualFX::IsFinished()
{
	if (bitfield.__finish || bitfield.canBeDeleted)
		return true;

	if (!visual) {
		if (scripted.visName || __unkvob1 || blendEffect)
			return false; // I removed redundant if (visual)
		return true;
	}

	if (auto visual = zDYNAMIC_CAST<zCParticleFX>(this->visual))
		return visual->CalcIsDead();

	return visual->GetVisualDied();
}

bool oCVisualFX::IsLooping()
{
	if (IsFinished())
		return false;

	if (auto visual = zDYNAMIC_CAST<zCParticleFX>(this->visual)) {
		if (visual->flags.8)
			return false;
	}

	return scripted.emFXLifeSpan == -1;
}

void oCVisualFX::SetupEmitterKeysByVisual()
{
	// looks wrong to me, but this is what is written in assembly
	// also it does xor ecx, ecx before the call, which is bogus
	// (maybe I am decompiling damaged exe?)
	if ( !visual && !visual->GetVisualDied() )
	{
		for (auto key : __emitterKeys )
			key->SetDefaultByFX(this);
		for (auto vfx : __children)
			vfx->SetupEmitterKeysByVisual();
		for (auto vfx : __children2)
			vfx->SetupEmitterKeysByVisual();
	}
}
