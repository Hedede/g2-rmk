void oCNpc::OnDamage(zCVob *source, zCVob *offender, float amount, int type, zVEC3 const& pos)
{
	oCNpc::oSDamageDescriptor dam;
	memset(&dam, 0, sizeof(dam));
	dam.__flt = 1.0;
	dam.damage = amount;
	dam.pos  = pos;
	dam.pos2 = pos;

	dam.offender = offender;
	dam.source = source;

	dam.npc = zDYNAMIC_CAST<oCNpc>(offender);
	dam.weapon = zDYNAMIC_CAST<oCItem>(source);
	oCVisualFX vfx = zDYNAMIC_CAST<oCVisualFX>(source);
	dam.SetFXHit(vfx);
	if ( dam.fxHit ) {
		dam.spellType  = dam.fxHit->GetSpellType();
		dam.spellCat   = dam.fxHit->GetSpellCat();
		dam.spellLevel = dam.fxHit->GetLevel();
	} else {
		dam.spellType  = -1;
		dam.spellCat   = -1;
		dam.spellLevel = -1;
	}
	dam.mode = 0;
	dam.type = 0;
	if ( dam.npc ) {
		switch (dam.npc->GetWeaponMode()) {
		case FMODE_FIST:
			dam.mode = 1;
			type |= dam.npc->damagetype;
			break;
		case FMODE_MELEE:
		case 3:
		case 4:
			dam.mode |= 2u;
			break;
		case FMODE_FAR:
		case 6:
			dam.mode |= 4u;
			break;
		case FMODE_MAGIC:
			dam.mode |= 8u;
			break;
		}

		oCItem* weap = dam.npc->GetWeapon();
		if ( dam.weapon ) {
			if ( weap != dam.weapon )
				zINFO(5, "R: NPC: OnDamage() - Weapon is NOT equal to NPC's weapon !!!");
		} else {
			dam.weapon = weap;
		}
	}

	if ( dam.weapon ) {
		if ( dam.weapon->HasFlag(ITEM_KAT_NF) )
			dam.mode |= 2;
		if ( dam.weapon->HasFlag(ITEM_KAT_FF) )
			dam.mode |= 4;
		if ( dam.weapon->HasFlag(ITEM_KAT_MUN) )
			dam.mode |= 4;
		if ( dam.weapon->HasFlag(ITEM_KAT_RUNE) )
			dam.mode |= 8
	}
	dam.type = type;

	if ( !type && (dam.weapon || dam.npc) ) {
		if ( (dam.mode & 4) == 4 )
			dam.type = DAM_POINT;
		if ( (dam.mode & 8) == 8 )
			dam.type |= DAM_MAGIC;
	}

	OnDamage(dam);
}
