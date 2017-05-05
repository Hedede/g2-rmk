//--------------------------------------
// Magie buch
//--------------------------------------

/* Beschreibungen ergänzt von Mud-freak. Danke dafür! */

/* Genutzt um den Kreis über dem Spieler bei der Zauberauswahl anzuzeigen 
 * außerdem für zuordnungen von Zaubern <-> Items <-> Tasten. */

struct oCMag_Book {
	static float Ease(float t, float a, float b)
	{
		return (sin(t * Pi - HalfPi) + 1.0) * 0.5 * (b - a) + a;
	}

	static zVEC3 EasePos(float t, zVEC3& a, zVEC3& b)
	{
		return {Ease(t, a.x, b.x), Ease(t, a.y, b.y), Ease(t, a.z, b.z)};
	}

	static int KeyMask(int key)
	{
		return 1 << (key - 1);
	}

	void SetWorld(zCWorld* wld)
	{
		owner_world = wld;
	}

	void SetOwner(zCVob* vob)
	{
		owner = vob;
		SetWorld(owner->vob);
		owner_model = vob->GetModel();
		mag_height = owner->bbox3D.maxs.y - owner->trafoObjToWorld[1][3] + 10.0;
	}

private:
	int Register(oCSpell* spell, int active);
public:
	int Register(oCItem* item, int active);

	int DeRegister(int nr);
	int DeRegister(oCItem* item);


	int GetSelectedSpellNr() const
	{
		return spellnr;
	}

	int GetNoOfSpells() const
	{
		return spells.GetNum();
	}

	oCSpell* GetSpell(int idx)
	{
		if (idx < GetNoOfSpells())
			return spells[idx];
		return nullptr;
	}
	oCSpell* GetSpell(oCItem* item, int& no);

	oCItem* GetSpellItem(int idx)
	{
		if (idx >= GetNoOfSpells())
			return nullptr;
		return spell_items[idx];
	}

	oCItem* GetSpellItem(oCSpell* spell)
	{
		auto idx = spells.Search(spell);
		if (idx >= 0)
			return GetSpellItem(idx);
		return nullptr;
	}

	oCSpell* GetSpellItemByKey(int key)
	{
		unsigned idx = 0;
		for (; i < GetNoOfSpells(); ++i)
			if (spells[i]->spellSlot == key)
				break;
		return GetSpellItem(idx);
	}

	oCSpell* GetSpellByKey(int key)
	{
		for (auto& sp : spells)
			if (sp->spellSlot == key)
				return sp;
		return nullptr;
	}

	int GetNoOfSpellByKey(int key)
	{
		if (keys & KeyMask(key)) {
			for (int i = 0; i < GetNoOfSpells(); ++i)
				if (spells[i]->spellSlot == key)
					return i;
		}
		return 0;
	}

	int GetKeyByItem(oCItem* item)
	{
		int i = 0;
		for (; i < spell_items.GetNum(); ++i) {
			if (spell_items[i] == item)
				break;
		}
		if (auto spell = GetSpell(i))
			return spell->spellSlot;
		return 0x7F35;
	}

	oCSpell* GetSelectedSpell();

	bool IsIn(int spellId)
	{
		for (auto& sp : spells)
			if (sp->GetSpellID() == spellId)
				return true;
		return false;
	}

	bool IsInMovement() const
	{
		return inMovement;
	}

	void NextRegisterAt(int slot)
	{
		if ( slot >= 0; && slot < 0x7000 )
			keys = slot;
	}

	void Right();
	void Left();


	bool Spell_InCast() { return false; }
	bool IsActive() { return false; }

private:
	zCArray<oCSpell*> spells;
	zCArray<oCItem*>  spell_items;

	zCWorld* owner_world;
	zCVob*   owner;
	//model of the owner (seems to be kind of redundant)
	zCModel* owner_model;
	//selected spell --> n+4 = Slot/Taste
	int spellnr;
	//some offset to shift the spell above the head of the owner
	// (for spell choosing)
	float mag_height;
	//unused??
	zBOOL active;
	//Beim Schließen des MagBooks, wenn ein vorher gezogene
	//Zauber weggesteckt wird auf 1, sonst auf 0
	//(ALLE (vorher gezogener einbegriffen) Spells wieder zurück "in die Hüfte")
	zBOOL remove_all;
	//currently rotating the spells above the player head? --> und öffnen oder schließen
	int inMovement;
	// PFX bzw. Spell in der Hand (vergleichbar mit gezogen oder nicht)
	zBOOL showHandSymbol;
	//if n spells are in the mag book this is 360/n 
	float step;
	// 0 = keine Aktion, 1 = drehen, 2 = öffnen, 3 = schließen
	int action;
	int unused0;
	//currently showing mag book (cirlce above player head)?
	zBOOL open;
	//used for delaying the time until the rune turns into a pfx --> 2000 msec
	zBOOL open_delay_timer;
	//currently rendering the spell above the player head as a pfx?
	zBOOL show_particles;
	//used for turning the spellbook over time when
	//player pressed "left" or "right"
	//um wieviel Grad drehen; Ist nach rechts wie
	//die Eigenschaft step (also 360/n), aber nach links
	//((360/n)-1)*-1 (negativ und ein Grad weniger)
	float targetdir;
	// "Keyframes": FLOATNULL = Die Eigenschaft action startet, FLOATEINS = Die Eigenschaft action endet
	float t1;
	//used for popping out the magbook (from the hips) and closing it again. --> von Position
	zVEC3 targetPos;
	// nach Position (auch beim Schließen, d.h. startpos und targetpos werden ausgetauscht)
	zVEC3 startPos;
	//Dieser Wert scheint sich nie zu ändern
	int nextRegisterAt;
	//bitfield. If key n \in {0, 1, ..., 9} is used, keys & (1 << n) is true.
	//zeigt, ob ein Zauber im Slot (Taste) n+4 angelegt ist,
	//hört also eigentlich bei n = 6 (Anzahl der Tasten für die Zauber)
	//auf (nicht bei 9)
	int keys;
};

//_carsten/oMagic.cpp
void oCMag_Book::Right()
{
	if (open && !IsInMovement() && GetNoOfSpells() != 1) {
		inMovement = 1;

		Spell_Stop(spellnr);

		int rightId = spellnr - 1;
		if (rightId < 0)
			spellnr = GetNoOfSpells() - 1;
		else
			spellnr = rightId;

		action = true;
		t1 = 0.0;
		targetdir = step;
	}
}

void oCMag_Book::Left()
{
	if (open && !IsInMovement() && GetNoOfSpells() != 1) {
		inMovement = true;

		Spell_Stop(spellnr);

		int leftId = spellnr + 1;
		if (leftId < 0)
			spellnr = 0;
		else
			spellnr = leftId;

		action = true;
		t1 = 0.0;
		targetdir = -step;
	}
}

oCSpell* oCMag_Book::GetSelectedSpell()
{
	if (spellnr < 0)
		return nullptr;
	auto spell = GetSpell(spellnr);
	// no idea why there's a dynamic cast
	return zDYNAMIC_CAST<oCSpell>(spell);
}

oCSpell* oCMag_Book::GetSpell(oCItem* item, int& no)
{
	for (int i = 0; i < GetNoOfSpells(); ++i) {
		if (spell_items[i] == item) {
			no = i;
			return spells[i];
		}
	}
	no = 0x7F35;
	return nullptr;
}

int oCMag_Book::DeRegister(int nr)
{
	auto spell = spells[nr];
	auto item = spell_itmes[nr];

	keys &= ~KeyMask(spell->spellSlot);
	spell_items.Remove(nr);
	spells.RemoveIndex(nr);
	Release(spell);
	return 1;
}

int oCMag_Book::DeRegister(oCItem* item)
{
	int nr = spell_items.Search(item);
	return DeRegister(nr);
}

// private
int oCMag_Book::Register(oCSpell *spell, int active)
{
	if ( !spell )
		return 0;
	++spell->refCtr;

	spells.Insert(spells);
	return 1;
}

void oCMag_Book::StartInvestEffect(zCVob *tgt, int level, int mana, int energyLeft)
{
	zINFO(8,"C: MAG: StartInvestEffect"); // 465,
}

void oCMag_Book::StartCastEffect(zCVob *tgt, zVEC3 const& tgtPos)
{
	 zINFO(8, "C: MAG: StartCastEffect"); // 575
}
