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
	virtual void GetLevel();
	virtual void Cast(int);
	virtual void Stop(int);
	virtual void Kill();
	virtual void Play(float,zMAT4 const *,zMAT4 const *);
	virtual void CanBeDeleted();
	virtual void IsFinished();
	virtual void IsLooping();
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
	virtual void GetNumCollisionCandidates();
	virtual void GetCollidedCandidates(zCArray<zCVob *> &);
	virtual void SetDamage(float damage)
	{
		this->damage = damage;
	}

	virtual void SetDamageType(int damType)
	{
		this->damageType = damType;
	}

	virtual float GetDamage() const
	{
		return damage;
	}
	virtual int GetDamageType() const
	{
		return damageType;
	}
	virtual void IsASpell();

	virtual void SetSpellType(int type)
	{
		spellType = type;
	}

	virtual int GetSpellType() const
	{
		return spellType;
	}

	virtual void SetSpellCat(int cat)
	{
		spellCat = cat;
	}
	virtual void GetSpellCat() const
	{
		return spellCat;
	}
	virtual void SetSpellTargetTypes(int types)
	{
		targetTypes = types;
	}
	virtual void GetSpellTargetTypes() const
	{
		return targetTypes;
	}
	virtual void GetSendsAssessMagic();
	virtual void SetSendsAssessMagic(int);
	virtual zBOOL GetIsProjectile() const
	{
		return isProjectile;
	}
	virtual void SetIsProjectile(zBOOL b)
	{
		isProjectile = b;
	}

	virtual void SetVisualByString(zSTRING const &);
	virtual void CalcTrajectory(int const &);
	virtual void Collide(int);
	virtual void CollisionResponse(zVEC3 const &,int);

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
