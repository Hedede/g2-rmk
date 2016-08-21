enum oEDamageType { // oETypeDamage
	DAM_BARRIER = (1 << 0),
	DAM_BLUNT   = (1 << 1),
	DAM_EDGE    = (1 << 2),
	DAM_FIRE    = (1 << 3),
	DAM_FLY     = (1 << 4),
	DAM_MAGIC   = (1 << 5),
	DAM_POINT   = (1 << 6),
	DAM_FALL    = (1 << 7),
};

enum oEIndexDamage
{
	DAM_INDEX_BARRIER = 0x0,
	DAM_INDEX_BLUNT = 0x1,
	DAM_INDEX_EDGE = 0x2,
	DAM_INDEX_FIRE = 0x3,
	DAM_INDEX_FLY = 0x4,
	DAM_INDEX_MAGIC = 0x5,
	DAM_INDEX_POINT = 0x6,
	DAM_INDEX_FALL = 0x7,
	DAM_INDEX_MAX = 0x8,
};

const int ITM_TEXT_MAX = 6;

class oCItem : public oCVob {
	Z_OBJECT(oCItem);

	static int lightingSwell;
public:
	static void SetLightingSwell(int swell)
	{
		lightingSwell = swell;
	}

	static int GetLightingSwell() const
	{
		return lightingSwell;
	}

	oCItem()
		: oCVob()
	{
		Init();
	}

	oCItem(int nr, int anz)
		: oCVob()
	{
		Init();
		InitByScript(nr, anz);
	}

	oCItem(zSTRING& inst, int anz)
		: oCItem(zparser.GetIndex(inst), anz)
	{
	}

	virtual ~oCItem()
	{
		RemoveEffect();
		zparser.ResetWithVarReferenceList(s_itemVarReferenceList, this);
	}

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual void Render(zTRenderContext &);

	void SetVisual(zCVisual* visual) override
	{
		zCVob::SetVisual(visual);
		zCVob::SetCollisionClass(zCCollObjectPoint::s_oCollObjClass);
	}

	void GetScriptInstance(zSTRING*& name, int& idx) override
	{
		name = zparser.GetSymbol(instanz)->name;
		idx = instanz;
	}

	bool SetByScriptInstance(zSTRING const* name, int index) override
	{
		if (name)
			index = zparser.GetIndex(name);

		InitByScript(index, 1);
		return 1;
	}

	void ThisVobAddedToWorld(zCWorld* world) override;
	void ThisVobRemovedFromWorld(zCWorld* world) override;

	void InitByScript(int nr, int anz);
	virtual void Init(void);

	int GetInstance() const override
	{
		return instanz;
	}

	void GetInstanceName() const override
	{
		int typ, ele;
		return zparser.GetSymbolInfo(instanz, typ, ele);
	}

	bool IsOwnedByGuild(int guild) const override
	{
		return guild > 0 && ownerGuild == guild;
	}

	bool IsOwnedByNpc(int instance) const override
	{
		return instance > 0 && owner == instance;
	}

	bool IsOwned(int instance, int guild)
	{
		return IsOwnedByNpc(instance) || IsOwnedByGuild(guild);
	}

	void GetAIVobMove() override;

	void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium& med, oTSndMaterial& mat,int) override
	{
		med = 1;
		mat = GetSoundMaterial(this);
	}

	int GetSoundMaterial() override
	{
		return material;
	}

	zSTRING& GetDescription()
	{
		return description;
	}

	bool HasFlag(int flag) const
	{
		return (flags & flag) == flag;
	}
	
	void SetFlag(int flag)
	{
		flags |= flag;
	}

	void ClearFlag(int flag)
	{
		flags &= 0xFFFFFFFF - flag;
	}

	bool TwoHanded() const
	{
		return HasFlag(ITEM_2HD_AXE) || HasFlag(ITEM_2HD_SWD);
	}

	bool IsOneHandWeapon() const
	{
		return HasFlag(ITEM_AXE) || HasFlag(ITEM_SWD);
	}

	bool SplitSlot() const
	{
		if (HasFlag(ITEM_NSPLIT))
			return 0;
		return HasFlag(ITEM_KAT_NF) || HasFlag(ITEM_KAT_FF);
	}

	void GetDamages(uint32_t* damages)
	{
		if ( damages )
			memcpy(damages, this->damage, 0x20u);
	}

	int GetDamageByMode(unsigned mode) const;
	int GetDamageByType(oEDamageType type);
	int GetDamageByIndex(oEIndexDamage index)
	{
		return damage[index];
	}

	int GetDamageTypes() const
	{
		return damageType;
	}

	bool HasDamageType(int dam) const
	{
		return (dam & damageType) == dam;
	}

	int GetFullDamage() const
	{
		int total = 0;
		for (auto dam : damage) {
			total += dam;
		}
		return total;
	}

	int GetFullProtection() const
	{
		int total = 0;
		for (auto prot : protection) {
			total += prot;
		}
		return total;
	}

	int GetProtectionByMode(unsigned mode) const;
	int GetProtectionByType(oEDamageType type);
	int GetProtectionByIndex(oEIndexDamage index)
	{
		return protection[index];
	}

	bool IsDeadly() const
	{
		return damageType & (DAM_BLUNT | DAM_EDGE);
	}

	void CopyDamage(oCItem* other) const
	{
		if ( other ) {
			other->damageType  = damageType;
			other->damageTotal = damageTotal;
			memcpy(other->damage, this->damage, sizeof(other->damage));
		}
	}

	oCItem* SplitItem(int removeAmount)
	{
		oCItem* result = nullptr;

		if ( removeAmount < amount) {
			auto world =  ogame->GetGameWorld();
			auto inst = GetInstance();

			result = world->CreateVob(VOB_TYPE_ITEM, inst);
			if (result) {
				result->amount = removeAmount;
				this->amount -= removeAmount;
			}
		} else {
			result = this;
		}

		return result;
	}

	int GetWeight() const
	{
		if ( amount > 1 )
			return weight * amount;
		return weight;
	}

	int GetCount(int idx) const
	{
		if ( idx < 0 || idx >= 6 )
			return 0;
		return count[idx];
	}

	zSTRING& GetText(int index)
	{
		zSTRING empty = "";
		if (index < 0 || index >= 6)
			return empty;

		return text[index];
	}

	int GetValue()
	{
		double val = (value + magic_value);
		if ( hp_max > 0 )
			// hp probably should be converted to double
			// but it is int
			val *= (hp / hp_max);
		return ceil(val);
	}

	zSTRING GetStatus() const;

	int GetHealMode(int& value) const;

	int GetStateEffectFunc(int stateNr)
	{
		if ( stateNr < 0 || stateNr >= 4)
			return -1;

		int func = on_state[stateNr];

		return func > 0 ? func : -1;
	}


	int GetDisguiseGuild() const
	{
		return disguiseGuild;
	}

	zSTRING GetVisualChange() const
	{
		zSTRING tmp = visual_change;
		tmp.Upper();
		int ext = tmp.Search(0, ".ASC", 1);
		if ( ext > 0 )
			tmp.Delete(ext, 4);
		return tmp;
	}

	void RotateInInventory();
	void CreateVisual()
	{
		if (!visual && (item_visual.Search(0, ".ZEN", 1) < 0))
			zCVob::SetVisual(item_visual);
	}

	void RemoveEffect()
	{
		if ( effectVob ) {
			effectVob->Kill();
			Release(effectVob);
		}
	}

	void InsertEffect();

	zSTRING GetEffectName() const
	{
		return effect;
	}

	zSTRING GetName(int identified) const
	{
		return name;
	}

	zSTRING GetSchemeName() const
	{
		return scemeName;
	}

	void AddManipulation();
	void Identify() { }
	void UseItem() { }

private:
	int      id;
	zSTRING  name, nameID;
	int      hp, hp_max;

	int mainflag, flags;    // Hauptflag und weitere Flags
	int weight, value;

	// Für Waffen
	int damageType;         // Welche Schadensarten
	int damageTotal;
	int damage[DAM_INDEX_MAX];

	// Für Rüstungen
	int wear;
	int protection[DAM_INDEX_MAX];

	// Für Nahrung
	int nutrition;		//	HP-Steigerung bei Nahrung

	// Benötigte Attribute zum Benutzen des Items
	int cond_atr		[3];
	int cond_value		[3];

	// Attribute, die bei anlegen des Items verändert werden
	int change_atr		[3];
	int change_value	[3];

	// Parserfunktionen
	int magic;		//	Parserfunktion zum "Magie Header"
	int on_equip;		//	Parserfunktion, wenn Item equipped wird.
	int on_unequip;		//	Parserfunktion, wenn Item unequipped wird.
	int on_state		[4];		//	

	// Besitzer									
	int owner;		//	Besitzer : Instanz-Name
	int ownerGuild;		//	Besitzer : Gilde
	int disguiseGuild;		//	Zur Schau getragene Gilde durch Verkleidung

	// Die 3DS-Datei
	zSTRING	visual;

	// Veränderung des NSC-Meshes beim Anlegen dieses Gegenstandes
	zSTRING  visual_change;  // ASC - File
	zSTRING  effect;         // Effekt Instanz

	int visual_skin;

	zSTRING scemeName;

	int material;
	zSTRING pfx;            // Magic Weapon PFX
	int munition;           // Instance of Munition

	int spell;
	int range;

	int mag_circle;

	zSTRING	description;
	zSTRING	text[ITM_TEXT_MAX];
	int count[ITM_TEXT_MAX];

	// inventory darstellungs geschichten, wird nur benutzt, falls von 0 abweichend
	int inv_zbias;								//  wie weit ist das item im inventory richtung far plane verschoben (integer scale 100=1)
	int inv_rotx;								//  wieviel grad um die x achse ist das item im inv gedreht
	int inv_roty;								//  wieviel grad um die y achse ist das item im inv gedreht
	int inv_rotz;								//  wieviel grad um die z achse ist das item im inv gedreht
	int inv_animate;							//  soll das item in inv rotiert werden

	int amount;							//int Wieviele Items sind das (Itemstapel)
	int instanz;						//int Symbolindex
	int c_manipulation;					//int ?
	zREAL last_manipulation;				//zREAL ?
	int magic_value;					//int ?
	oCVisualFX* effectVob;						//oCVisualFX*
	oCItem* next;							//oCItem* sind wohl einfach verkettete Listen.
};

bool oCItem::MultiSlot(oCItem *this)
{
	// Weapons stack in Gothic2, in gothic1, only
	// MULTI, KAT_MUN and KAT_FODD are stackable
	return (flags & ITEM_MULTI) == ITEM_MULTI ||
	       (flags & ITEM_KAT_MUN) == ITEM_KAT_MUN ||
	       (flags & ITEM_KAT_FOOD) == ITEM_KAT_FOOD ||
	       (flags & ITEM_NSPLIT) != ITEM_NSPLIT &&
	       ((flags & ITEM_KAT_NF) == ITEM_KAT_NF ||
		(flags & ITEM_KAT_FF) == ITEM_KAT_FF) &&
	        (flags & ITEM_ACTIVE) != ITEM_ACTIVE;
}

void oCItem::Archive(zCArchiver& arc)
{
	bool no_zen = 0 item_visual.Search(0, ".ZEN", 1) < 0;

	zCVisual* visual = this->visual;
	if ( no_zen ) {
		if ( visual )
			visual.AddRef();
		SetVisual(0);
	}

	oCVob::Archive(archive);

	zSTRING instName = GetInstanceName();

	arc.WriteString("itemInstance", instName);

	if ( no_zen ) {
		SetVisual(visual);
		if ( visual )
			visual.Release();
	}

	if ( arc.InSaveGame() ) {
		WriteInt("amount", amount);
		WriteInt("flags",  flags);
	}
}

void oCItem::Unarchive(zCArchiver& arc)
{
	oCVob::Unarchive(arc);

	arc.ReadString("itemInstance", name);

	int index = zparser.GetIndex(name);
	if ( index >= 0 ) {
		bool tmp = s_LoadingGame;
		s_LoadingGame = true;

		InitByScript(index, 1);

		s_LoadingGame = tmp;
	}

	if ( arc.InSaveGame(arc) ) {
		arc.ReadInt("amount", amount);
		arc.ReadInt("flags", flags);
	}
}

oCAIVobMove* oCItem::GetAIVobMove()
{
	if ( HasFlag(ITEM_TORCH)) {
		auto movTorch = dynamic_cast<oCAIVobMoveTorch>(callback_ai);
		if ( !movTorch ) {
			auto movTorch = new oCAIVobMoveTorch();
			SetAI(movTorch);
			Release(movTorch);
		}
		return movTorch;
	}

	return oCVob::GetAIVobMove();
}

void oCItem::Init()
{
	memset(&this->id, 0, 0x20Cu);

	instanz = -1;
	mainflag = -1;
	amount = 1;
	c_manipulation = 0;
	last_manipulation = 0;
	magic_value = 0;
	effectVob = 0;
	next = 0;
	description = "";
	damageType = 0;
	damageTotal = 0;
	memset(damage, 0, sizeof(damage));

	for (auto& val : count)
		val = 0;
	for (auto& str : text)
		str = "";
	disguiseGuild = 0;

	type = VOB_TYPE_ITEM;

	zCVob::SetCollisionClass(zCCollObjectPoint::s_oCollObjClass);
	inv_zbias = 0;
	inv_rotx = 0;
	inv_roty = 0;
	inv_rotz = 0;
	inv_animate = 0;
	zBias = 1;

	static bool reflist = 0;
	if ( !reflist ) {
		zparser.CreateVarReferenceList("C_ITEM", s_itemVarReferenceList);
		reflist = 1;
	}
}

void oCItem::InitByScript(int nr, int anz)
{
	static class_index = -1;
	if ( class_index < 0 ) {
		class_index = zparser.GetIndex("C_ITEM");
		zparser.CheckClassSize(class_index, 524);
	}

	instanz = nr;
	zparser.CreateInstance(nr, this);

	flags |= mainflag;
	amount = anz;

	if ( damage[0] ) {
		zWARNING("R: ITEM: item \"" + name + "\" causes DAM_INDEX_BARRIER damage. Most likely it contains an assignment to DAMAGE instead of DAMAGETOTAL !!!"); // 412, Ulf
	}

	ApplyDamages(damageType, damage, damageTotal);

	item_visual.Upper();

	if ( !item_visual.IsEmpty()) {
		if ( item_visual.Search(0, ".ZEN", 1u) > 0 && !s_LoadingGame ) {
			SetVisual(0);
			SetCollDetStat(0);
			SetCollDetDyn(0);

			zoptions.ChangeDir(DIR_WORLD);

			ogame->GetGameWorld()->AddVob(this);
			SetPositionWorld({0,0,0});

			auto t = ogame->GetWorld()->MergeVobSubtree(&item_visual, 0, 0);
			if ( t ) {
				t->SetCollDetStat(0);
				t->SetCollDetDyn(0);
				t->SetPositionWorld({0,0,0});
				SetTrafoObjToWorld(t->trafoObjToWorld);

				t->AddRefVobSubtree(0, 1);

				ogame->GetWorld()->RemoveVobSubtree(t);
				ogame->GetWorld()->AddVobAsChild(t, this);
				ogame->GetWorld()->RemoveVobSubtree(this);
			} else {
				zWARNING("C: oCItem::InitByScript(): could not merge subtree for item " + name + " possible cause: visual " + item_visual + " does not exist or is corrupt!"); // 445
			}
		}
	}

	type = VOB_TYPE_ITEM;
	int a, b;
	auto name = zparser.GetSymbolInfo(nr, a, b);

	SetVobName(name);
}

void oCItem::ThisVobAddedToWorld(zCWorld* world)
{
	struct zCRigidBody *v3; // eax@7

	CreateVisual();

	zCVob::ThisVobAddedToWorld(world);

	SetCollDetDyn(1);
	SetCollDetStat( 1);
	SetSleeping(1);

	flags1 &= 0xEFu;

	SetPhysicsEnabled(0);

	if ( !world->isInventoryWorld )
		InsertEffect();

	if ( GetRigidBody() ) {
		GetRigidBody()->flags |= 1;
	}
}

void oCItem::ThisVobRemovedFromWorld(zCWorld* world)
{
	if ( !world->isInventoryWorld )
		RemoveEffect();

	zCVob::ThisVobRemovedFromWorld(world);
	zCVob::SetAI(0);
}

void oCItem::RotateInInventory()
{
	if ( inv_animate ) {
		zVEC3 axis;
		if ( visual && visual->GetOBBox3D() ) {
			axis = visual->GetOBBox3D()->GetAxis();
		} else {
			unsigned index = 0;
			float min = -1.0;
			for (unsigned i = 0; i < 3; ++i) {
				float diff = bbox3D.maxs[i] - bbox3D.mins[i];
				if (diff > min) {
					min = diff;
					index = i;
				}
			}

			axis[index] = 1.0;
		}

		RotateLocal(axis, ztimer.totalTimeFloat * 0.02);
	}
}

int oCItem::GetDamageByType(oEDamageType type)
{
	if ( (type & DAM_BARRIER) == DAM_BARRIER )
		return damage[DAM_INDEX_BARRIER];

	if ( (type & DAM_BLUNT) == DAM_BLUNT )
		return damage[DAM_INDEX_BLUNT];

	if ( (type & DAM_EDGE) == DAM_EDGE )
		return damage[DAM_INDEX_EDGE];

	if ( (type & DAM_FIRE) == DAM_FIRE )
		return damage[DAM_INDEX_FIRE];

	if ( (type & DAM_FLY) == DAM_FLY )
		return damage[DAM_INDEX_FLY];

	if ( (type & DAM_MAGIC) == DAM_MAGIC )
		return damage[DAM_INDEX_MAGIC];

	if ( (type & DAM_POINT) == DAM_POINT )
		return damage[DAM_INDEX_POINT];

	if ( (type & DAM_FALL) == DAM_FALL )
		return damage[DAM_INDEX_FALL];

	return 0;
}

int oCItem::GetDamageByMode(unsigned mode) const
{
	int result = 0;
	if ( (mode & DAM_BARRIER) == DAM_BARRIER )
		result  = damage[DAM_INDEX_BARRIER];
	if ( (mode & DAM_BLUNT) == DAM_BLUNT )
		result += damage[DAM_INDEX_BLUNT];
	if ( (mode & DAM_EDGE) == DAM_EDGE )
		result += damage[DAM_INDEX_EDGE];
	if ( (mode & DAM_FIRE) == DAM_FIRE )
		result += damage[DAM_INDEX_FIRE];
	if ( (mode & DAM_FLY) == DAM_FLY )
		result += damage[DAM_INDEX_FLY];
	if ( (mode & DAM_MAGIC) == DAM_MAGIC )
		result += damage[DAM_INDEX_MAGIC];
	if ( (mode & DAM_POINT) == DAM_POINT )
		result += damage[DAM_INDEX_POINT];
	if ( (mode & DAM_FALL) == DAM_FALL )
		result += damage[DAM_INDEX_FALL];
	return result;
}

int oCItem::GetProtectionByType(oEDamageType type)
{
	if ( (type & DAM_BARRIER) == DAM_BARRIER )
		return protection[DAM_INDEX_BARRIER];

	if ( (type & DAM_BLUNT) == DAM_BLUNT )
		return protection[DAM_INDEX_BLUNT];

	if ( (type & DAM_EDGE) == DAM_EDGE )
		return protection[DAM_INDEX_EDGE];

	if ( (type & DAM_FIRE) == DAM_FIRE )
		return protection[DAM_INDEX_FIRE];

	if ( (type & DAM_FLY) == DAM_FLY )
		return protection[DAM_INDEX_FLY];

	if ( (type & DAM_MAGIC) == DAM_MAGIC )
		return protection[DAM_INDEX_MAGIC];

	if ( (type & DAM_POINT) == DAM_POINT )
		return protection[DAM_INDEX_POINT];

	if ( (type & DAM_FALL) == DAM_FALL )
		return protection[DAM_INDEX_FALL];

	return 0;
}

int oCItem::GetProtectionByMode(unsigned mode) const
{
	int result = 0;
	if ( (mode & DAM_BARRIER) == DAM_BARRIER )
		result  = protection[DAM_INDEX_BARRIER];
	if ( (mode & DAM_BLUNT) == DAM_BLUNT )
		result += protection[DAM_INDEX_BLUNT];
	if ( (mode & DAM_EDGE) == DAM_EDGE )
		result += protection[DAM_INDEX_EDGE];
	if ( (mode & DAM_FIRE) == DAM_FIRE )
		result += protection[DAM_INDEX_FIRE];
	if ( (mode & DAM_FLY) == DAM_FLY )
		result += protection[DAM_INDEX_FLY];
	if ( (mode & DAM_MAGIC) == DAM_MAGIC )
		result += protection[DAM_INDEX_MAGIC];
	if ( (mode & DAM_POINT) == DAM_POINT )
		result += protection[DAM_INDEX_POINT];
	if ( (mode & DAM_FALL) == DAM_FALL )
		result += protection[DAM_INDEX_FALL];
	return result;
}

int GetHealMode(int& value)
{
	if (!HasFlag(ITEM_KAT_FOOD))
		return 10;

	value = nutrition;
	if ( nutrition <= 0 ) {
		unsigned i = 0;
		for (auto& atr : change_atr) {
			// original code contained 3 duplicate if clauses
			if (!(atr == ATR_HITPOINTS  ||
			      atr == ATR_MANA       ||
			      atr == ATR_STRENGTH   ||))
			{
				continue
			}

			value = change_value[i++];
			if (value > 0)
				return atr;
		}
	}
	return 0;
}

zSTRING oCItem::GetStatus() const
{
	int hp_perc = 100 * hp / hp_max;
	int status = 0;
	if (hp_perc > 0)
		 status = (hp_perc - 1) / 10;
	if (status > 9)
		status = 9;

	return zparser.GetText(oCText::TXT_ATR_HP, status);
}

void oCItem::AddManipulation()
{
	if ( c_manipulation > 0 ) {
		auto timer = ogame->GetWorldTimer();
		if ( timer->GetPassedTime(last_manipulation) > 60 )
			c_manipulation = 0;
	}

	auto timer = ogame->GetWorldTimer();
	last_manipulation = timer->GetFullTime();
	++c_manipulation;
}

void oCItem::InsertEffect()
{
	if ( oCItem::s_bItemEffectEnabled && !effectVob && !effect.IsEmpty() ) {
		effectVob = oCVisualFX::CreateAndPlay(effect, this, 0, 1, 0.0, 0, 0);
		if ( effectVob ) {
			if ( visual )
				effectVob->SetPFXShapeVisual(visual, 0);
		} else {
			zWARNING("C: effect" + effect + " could not be started for item " + GetName()); // 850, oItem.cpp, Ulf
		}
	}
}

void oCItem::RenderItem(zCWorld* world, zCViewBase* viewItem, float addon)
{
	auto active = zCCamera::activeCam;
	auto light  = playerLightInt;

	zCCamera cam;
	auto vob = new zCVob();

	SetPositionWorld({0,0,0});

	if ( addon == 0.0 )
		RotateForInventory(1.0);
	else
		RotateInInventory();

	groundPoly = 0;

	world->AddVob(this);
	world->AddVob(vob);

	cam.connectedVob = vob;

	playerLightInt = 5000;

	RenderItemPlaceCamera(cam, addon);

	cam.SetRenderTarget(viewItem);
	cam.SetFarClipZ(FLT_MAX);

	world->bspTree.drawVobBBox3D = 0;

	lastTimeDrawn = -1;

	bspTree.bspTreeMode = 0;

	world->GetActiveSkyControler()->Render(0);

	zCEventManager::disableEventManagers = 1;

	int abf = 1;
	zrenderer->SetAlphaBlendFunc(&abf);

	world->Render(cam);

	zCEventManager::disableEventManagers = 0;

	world->RemoveVob(vob);
	world->RemoveVobSubtree(this);

	Release(vob);
	zCCamera::activeCam = active;
	playerLightInt = light;
}
