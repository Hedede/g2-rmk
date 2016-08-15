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
		if (keys & (1 << (key - 1))) {
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

	void SetWorld(zCWorld* wld)
	{
		owner_world = wld;
	}

	void NextRegisterAt(int slot)
	{
		if ( slot >= 0; && slot < 0x7000 )
			keys = slot;
	}

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
