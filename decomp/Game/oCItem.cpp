enum oEDamageType {
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

class oCItem : public zCVob {
	CLASSDEF_DEFINE;
public:
	virtual ~oCItem();
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual void Render(zTRenderContext &);
	virtual void SetVisual(zCVisual *);
	virtual void GetScriptInstance(zSTRING * &,int &);
	virtual void SetByScriptInstance(zSTRING const *,int);
	virtual void ThisVobAddedToWorld(zCWorld *);
	virtual void ThisVobRemovedFromWorld(zCWorld *);
	virtual void Init(void);
	virtual void GetInstance(void);
	virtual void GetInstanceName(void);
	virtual void IsOwnedByGuild(int);
	virtual void IsOwnedByNpc(int);
	virtual void GetAIVobMove(void);
	virtual void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium &,oTSndMaterial &,int);
	virtual void GetSoundMaterial(void);

	void ClearFlag(int flag)
	{
		flags &= 0xFFFFFFFF - flag;
	}

	void GetDamages(uint32_t *damages)
	{
		if ( damages )
			memcpy(damages, this->damage, 0x20u);
	}

	int GetDamageByIndex(oEIndexDamage index)
	{
		return damage[index];
	}

	int GetDamageTypes() const
	{
		return damageType;
	}

	bool IsDeadly() const
	{
		return damageType & (DAM_BLUNT | DAM_EDGE);
	}

	int GetWeight() const
	{
		if ( amount > 1 )
			return weight * amount;
		return weight;
	}

	void Identify() { }
	void UseItem() { }
private:
	INT		id;				
	zSTRING  name,nameID;
	INT		hp,hp_max;

	int mainflag,flags;		//	Hauptflag und weitere Flags
	int weight,value;			

	// Für Waffen								
	int damageType;		//	Welche Schadensarten
	int damageTotal;
	int damage			[DAM_INDEX_MAX];

	// Für Rüstungen
	int wear;
	int protection		[PROT_INDEX_MAX];

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

	zSTRING 	scemeName;
	int material;	
	// zSTRING	pfx;		//	Magic Weapon PFX
	int munition;		//	Instance of Munition

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
