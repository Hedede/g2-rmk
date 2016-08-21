// oCMob was named oCMOB in original code
class oCMob : public oCVob {
	Z_OBJECT(oCMob);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	oCMob()
		: oCVob()
	{
		flags1.showVisual = 1;
		hitpoints = 0xA;
		type = VOB_TYPE_MOB;

		SetCollisionClass(&zCCollObjectComplex::s_oCollObjClass);
	}

	virtual ~oCMob() = default;

	void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &) override {}
	void OnMessage(zCEventMessage *,zCVob *) override {}

	void CanThisCollideWith(zCVob* vob) override
	{
		return ignoreVobList.Search(vob) > 0;
	}

	void SetVisual(zCVisual* visual) override
	{
		zCVob::SetVisual(visual);
		SetCollisionClass(&zCCollObjectComplex::s_oCollObjClass);
	}

	void IsOwnedByGuild(int guild) override
	{
		return ownerGuild == guild;
	}
	void IsOwnedByNpc(int instance) override
	{
		return owner == instance;
	}

	void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium& med, oTSndMaterial& mat,int) override
	{
		med = 2;
		mat = GetSoundMaterial();
	}
	void SetSoundMaterial(oTSndMaterial mat) override
	{
		sndMat = mar;
	}
	oTSndMaterial GetSoundMaterial() override
	{
		return sndMat;
	}

	virtual void SetName(zSTRING const& name)
	{
		focusNameIndex = zparser.GetIndex(name);
	}

	virtual zSTRING GetName()
	{
		zSTRING tmp;

		auto sym = zparser.GetSymbol(focusNameIndex);
		if (sym)
			sym->GetValue(tmp);

		return tmp;

	}
	virtual zCModel* void GetModel()
	{
		return dynamic_cast<zCModel*>(visual);
	}
	virtual zSTRING GetScemeName()
	{
		zSTRING ret;

		if (visual) {
			zSTRING visName = visual->GetVisualName();
			ret = visName.Copied("_", 1);
		}

		return ret;
	}

	virtual void Destroy();

	virtual bool AllowDiscardingOfSubtree()
	{
		return true;
	}

	bool IsOwner(oCNpc* npc)
	{
		return IsOwnedByNpc(npc->GetInstance()) ||
		       IsOwnedByGuild(npc->GetTrueGuild());
	}

	void SetOwner(int owner, int ownerGuild)
	{
		this->owner = owner;
		this->ownerGuild = ownerGuild;
	}

	void SetOwner(zSTRING const& o, zSTRING const& og)
	{
		ownerStr = o;
		ownerGuildStr = og;

		auto npc = zparser.GetIndex(o);
		auto sym = zparser.GetSymbol(og);

		int guild = -1;
		if (sym)
			sym->GetValue(guild, 0);

		SetOwner(npc, guild);
	}

	void SetMoveable(bool b)
	{
		moveable = b;
	}

	bool IsMoveable() const
	{
		return moveable;
	}

	void InsertInIgnoreList(zCVob* vob)
	{
		if (!ignoreVobList.Search(vob))
			ignoreVobList.Insert(vob);
	}

	void RemoveFromIgnoreList(zCVob* vob)
	{
		ignoreVobList.Remove(vob);
	}

	void Hit(int dam) {}

private:
	//Mobeigenschaften:
	// Symbolischer Name, z.B. "MOBNAME_DOOR"
	zSTRING name;

	uint32_t hitpoints      : 12;
	uint32_t damage         : 12;
	uint32_t isDestroyed    : 1;
	uint32_t moveable       : 1;
	uint32_t takeable       : 1;
	uint32_t focusOverride  : 1;
	//oTSndMaterial
	uint32_t sndMat         : 3;

	zSTRING visualDestroyed;
	zSTRING ownerStr;
	zSTRING ownerGuildStr;

	int owner      = -1;
	int ownerGuild = -1;
	
	//Index des Parsersymbols, dass den Namen enthält,
	//zum Beispiel Index des Symbols MOBNAME_DOOR.
	int focusNameIndex = -1;

	zCList<zCVob>     ignoreVobList;
};

void oCMob::Destroy()
{
	if (isDestroyed)
		return;

	zsndMan->StartDestructionSound(this, GetSoundMaterial());

	auto pfx = new zCParticleFX();
	auto pfx_vob = new zCVob();

	pfx_vob->SetCollDetStat(0);
	pfx_vob->SetCollDetDyn(0);
	pfx_vob->SetPositionWorld(GetPositionWorld());
	pfx_vob->SetVisual(pfx);
	pfx_vob->flags3.dontWriteIntoArchive = 1;

	pfx->SetEmitter("PFX_MOBDESTROY", 0);

	pfx->emitter->visualName = "OC_MOB_CHEST_LARGE.3DS";
	pfx->emitter->UpdateInternals();

	pfx->CreateParticles();

	homeWorld->AddVob(pfx_vob);

	isDestroyed = 1;

	if ( visualDestroyed.Length() > 0 ) {
		SetVisual(visualDestroyed);
		UpdateVisualDependencies(1);
	} else {
		RemoveVobFromWorld();
	}
}
