#include "oNpc.h"

enum {
	DAM_MODE_FIST  = 1,
	DAM_MODE_MELEE = 2,
	DAM_MODE_FAR   = 4,
	DAM_MODE_MAGIC = 8,
};

#define PRINT_DAMAGE_DEBUG(m,p,o,v) \
	if (oCNpc::isEnabledDamageDebug) { \
		zSTRING method = m, part = p, outcome = o, v = value; \
		method.Align( 2, 0x1Eu, spaceChar); \
		part.Align(   2, 0x1Eu, spaceChar); \
		outcome.Align(2, 0x28u, spaceChar); \
		value.Align(  2, 0x28u, spaceChar); \
		auto msg = method + " -> " + part + " -> " + outcome + ": " + value; \
		NotifyDamageToSpy(msg); \
	}


static void oCNpc::EnableDamageDebugOutput(zBOOL b)
{
	oCNpc::isEnabledDamageDebug = b;
}

void oCNpc::NotifyDamageToSpy(zSTRING& msg)
{
	zINFO(6, "R: DAM: " + msg); // g2a:332, g1d:267
}

int oCNpc::EV_DamageOnce(oCMsgDamage* msg)
{
	PRINT_DAMAGE_DEBUG("EV_DamageOnce()", "", "", "");
	OnDamage(msg->damDescriptor);
	return 1;
}

// moved that out of OnDamage, else too spaghetti
bool IsProcessingInfos(oCNpc* npc, oCNpc* src)
{
	if (!npc->IsSelfPlayer() && !(src && src->IsSelfPlayer()))
		return false;

	auto num_msg = npc->GetEM()->GetNumMessages();

	for (unsigned i = num_msg - 1; i >= 0; --i) {
		auto msg = npc->GetEM()->GetEventMessage(i);
		auto msgConv = dynamic_cast<oCMsgConversation>(msg);

		if ( msgConv ) {
			auto subtyp = msgConv->subType;
			if (subtyp >= EV_PROCESSINFOS &&
			    subtyp <= EV_STOPPROCESSINFOS ) {
				return true;
			}
		}
	}
	
	return false;
}

void oCNpc::OnDamage(oCNpc::oSDamageDescriptor& descriptor)
{
	PRINT_DAMAGE_DEBUG("OnDamage()", "Descriptor Version", "", "");

	descriptor.flags.once     = (descriptor.__someflags & 0x1000) != 0x1000;
	descriptor.flags.finished = !descriptor.flags.once;

	bool fxFinished = 1;

	if ( descriptor.__timer1 > 0.0 )
	{
		auto* fx = descriptor->visualFx;
		fxFinished = fx && fx->IsFinished();
	}

	descriptor.flags.finished = fxFinished;

	if ( !IsConditionValid() ) {
		if (IsProcessingInfos(npc, src)) {
			descriptor.flags.unk2 = true;
			return;
		}

		OnDamage_Hit(descriptor);
		OnDamage_Condition(descriptor);

		if ( descriptor.flags.once ) {
			OnDamage_Anim(descriptor);
			OnDamage_Effects_Start(descriptor);
			OnDamage_Script(descriptor);
			OnDamage_State(descriptor);
		}

		OnDamage_Events(descriptor);

		if ( descriptor.flags.once )
			OnDamage_Sound(descriptor);
	}

	if ( descriptor.flags.finished )
		OnDamage_Effects_End(descriptor);
}

void oCNpc::OnDamage_Hit(oCNpc::oSDamageDescriptor& dam_desc)
{
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "", "", "");
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Victim:" + GetName(), "", "");
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Init:", "HP", GetAttribute(ATR_HITPOINTS));


	oCNpc* offender = dam_desc.source_npc;
	bool showFx = dam_desc.visualFx != 0;
	auto f_mode = offender->GetWeaponMode();
	bool hit = true;

	if (offender) {
		if (!offender->IsMonster() && f_mode < FMODE_2H) {
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "getting Hitchance", "Attacker is an NPC", "");

			int hc = 0;
			if (f_mode == FMODE_1H) {
				hc = offender->GetHitChance(NPC_TALENT_1H)
			} else if (f_mode == FMODE_2H) {
				hc = offender->GetHitChance(NPC_TALENT_2H)
			} else {
				if (offender->GetTrueGuild() != GIL_DRAGON &&
				    offender->GetTrueGuild() != GIL_FIREGOLEM &&
				    offender->GetTrueGuild() != GIL_ICEGOLEM) {
					PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damagechance", "First weapon- or invalid weapon Mode!", "");
				}
				hc = 100;
			}
			int roll = rand() % 100 + 1;
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damagechance", "Rolling dice" + " ( " + hc + " % )", "");

			if (hc > roll) {
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damagechance success", "Hit !!!", "");
			} else {
				hit = false;
			}
		}
	}

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "", "");

	if (dam_desc.mult != 1.0) {
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "Applying to Serepate Damages", val);
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "Multiplier", dam_desc.mult);

		unsigned i = 0;
		for (auto& val : dam_desc.damages) {
			val *= dam_desc.mult;
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "Damage[ "_s + i++ + " ] ", "");
		}

		dam_desc.damage *= dam_desc.mult;
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "Applying to Total Damages", "");
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Multiplier", "Total Damage", dam_desc.mult);
	}
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "Checking Seperate Damage", "");

	int dam_total = 0;
	unsigned i = 0;
	for (auto& val : dam_desc.damages) {
		if (HasFlag(dam_desc.type, 1 << i++)) {
			dam_total += val;
		}
	}

	bool nmst = false;
	if (offender) {
		nmst = source_npc->IsHuman()  ||
		       source_npc->IsOrc()    ||
		       source_npc->IsGoblin() ||
		       source_npc->IsSkeleton();
	}

	if (dam_total == 0) {
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "No Seperate Damage", "");

		if (offender && !nmst) {
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "Attacker is a Monster", "");
			if (dam_total->fxHit) {
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "Monster is casting a spell", "");
			} else if (dam_total->dam_total == 0.0) {
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "OnDamage_Hit()", "Split", "Using Monster's Strength", "");
				dam_desc.damage = offender->GetAttribute(ATR_STRENGTH);
			}
		}

		ApplyDamages(dam_desc.type, dam_desc.damages, dam_desc.damage);

		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "Splitting Damage Per Type", "");
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Split", "Total Damage", dam_desc.damage);
	}

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damage Boni", "", "");

	if (offender && nmst) {
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damage Boni", "Attacker is an NPC", "");

		unsigned bronny = 0;
		for (auto& val : dam_desc[damages]) {
			val += offender->GetDamageByIndex(bronny);
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Damage Boni", "Damage[ "_s + bronny++ " ] ",val);
		}
	}

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "", "");

	if (offender) {
		if(fxHit) {
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Attacker is a spell-caster", "No attribute boni added");
		}
		if (nmst) {
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Attacker is at least semi-human (fights with weapon)", "");
		}

		unsigned type = dam_desc.type;
		if (!HasFlag(type, DAM_BLUNT) &&
		    !HasFlag(type, DAM_EDGE) &&
		    !HasFlag(type, DAM_POINT) &&
		    offender->IsMonster()) {
			PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Assuming Edge Damage (No primary damage contained) !", "");
			dam_desc.type |= DAM_EDGE;
		}

		auto str = offender->GetAttribute(ATR_STRENGTH);
		auto dex = offender->GetAttribute(ATR_DEXTERITY);

		float bronnny = 0;
		if (HasFlag(dam_desc.type, DAM_BLUNT)) {
			bronnny += 1.0;
		} else if (HasFlag(dam_desc.type, DAM_EDGE)) {
			bronnny += 1.0
		} else if (HasFlag(dam_desc.type, DAM_POINT)) {
			bronnny += 1.0;
		}

		if (bronnny) {
			bronnny = 1.0 / bronnny;

			int melee = str * bronny;

			if (HasFlag(dam_desc.type, DAM_BLUNT)) {
				dam_desc.damages[1] += melee;
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Blunt Damage", dam_desc.damages[1]);
			}
			if (HasFlag(dam_desc.type, DAM_EDGE)) {
				dam_desc.damages[2] += melee;
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Edge Damage", dam_desc.damages[2]);
			}
			int ranged = dex * bronny;
			if (HasFlag(dam_desc.type, DAM_POINT)) {
				dam_desc.damages[6] += ranged;
				PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Attribute Boni", "Point Damage", dam_desc.damages[6]);
			}
		}
	}

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Protection", "", "");

	unsigned idx = 0;
	bool zeabalo = false;
	bool whasd = false;
	int damage_sum = 0;
	for (unsigned idx = 0; idx < DAM_INDEX_MAX; ++idx) {
		if (HasFlag(dam_desc.type, (1<<idx))) {
			auto prot = GetProtectionByIndex(idx);
			if (prot < 0 && zeabalo) {
				whasd = 1;
			} else {
				zeabalo = 0;
			}

			int dam = dam_desc.damages[idx] - prot;
			if (dam < 0)
				dam = 0;

			damage_sum += dam;
			dam_desc.effectiveDamages[idx] = dam;
		} else {
			dam_desc.effectiveDamages[idx] = 0;
		}
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Protection", "EffectiveDamage[ "_s + idx + " ] ", dam_desc.effectiveDamages[idx]);
	}

	dam_desc.dam_desc = damage_sum;

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Protection", "Total Damage", dam_desc.damage);
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Immortal", "Checking", "");

	if (HasFlag(NPC_FLAG_IMMORTAL)) {
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Immortal", "Vitcim is immortal !", "");
		PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Immortal", "Total Damage", dam_desc.damage);
	}

	dam_desc.effectiveDamage = dam_desc.damage;

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Update", "Effective Damage", dam_desc.effectiveDamage);

	if (dam_desc.effectiveDamage < 0) {
		dam_desc.effectiveDamage = 0;
	}

	dam_desc.realDamage = dam_desc.effectiveDamage;

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Update", "Real Damage", dam_desc.effectiveDamage);

	int damg = dam_desc.realDamage;
	if (HasFlag(dam_desc.type, DAM_BARRIER)) {
		PRINT_DAMAGE_DEBUG("OnDamage_Condition()",  "Death", "Barrier does damage", "");

		if ( GetAnictrl() && GetAnictrl()->GetWaterLevel() > 1 ) {
			PRINT_DAMAGE_DEBUG("OnDamage_Condition()", "Death", "Victim is being hit by barrier whilst swimming/diving", "");
			damg = GetAttribute(ATR_HITPOINTS_MAX);
		}
	}
	
	if ( index == -2 ) {
		index = zparser.GetIndex("NPC_MINIMAL_DAMAGE");
		auto sym = zparser.GetSymbol(index);
		if (sym)
			sym->GetValue(&min_dam, 0);

		index2 = zparser.GetIndex("NPC_MINIMAL_PERCENT");
		sym = zparser.GetSymbol(index2);
		if (sym)
			sym->GetValue(&min_perc);
		min_perc_float = min_perc * 0.01;
	}

	if (!hit)
		damg = damg * min_perc_float;

	if ( !dam_desc.visualFx && !dam_desc.fxHit && damg < min_dam ) {
		damg = min_dam;
		dam_desc.realDamage = damg;
	}

	if ( !HasFlag(NPC_FLAG_IMMORTAL) && !whasd )
		ChangeAttribute(0, -damg);

	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Update", "HP ", GetAttribute(0));
	PRINT_DAMAGE_DEBUG("OnDamage_Hit()", "Regeneration", "Resetting Values", "");

	hpHeal = GetAttribute(ATR_REGENERATE_HP) * 1000.0;
	manaHeal = GetAttribute(ATR_REGENERATE_MANA) * 1000.0;
}

void oCNpc::OnDamage_Effects(oSDamageDescriptor& desc)
{
	PRINT_DAMAGE_DEBUG("OnDamage_Effects()", "", " Values", "");
	if ( desc.flags & 2 )
		OnDamage_Effects_End(desc);
	if ( desc.flags & 1 )
		OnDamage_Effects_Start(desc);
}


void oCNpc::OnDamage_Anim(oSDamageDescriptor& descDamage)
{
	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "", "", "");

	oCAIHuman* pAIHuman = zDYNAMIC_CAST<oCAIHuman*>(callback_ai);
	oCAniCtrl* pAniCtrl = GetAnictrl();
	zCModel*   pModel   = GetModel();
	if (!pAIHuman)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "", "", "Npc has no valid HumanAI!");
		return;
	}
	if (!pAniCtrl)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "", "", "Npc has no valid Ani-Controler!");
		return;
	}
	if (!pModel)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "", "", "Npc has no valid Model!");
		return;
	}

	auto damType = descDamage.type;

	int nDamageBurn    = descDamage.effectiveDamages[DAM_INDEX_FIRE];
	int nDamageFly     = descDamage.effectiveDamages[DAM_INDEX_FLY];
	int nDamageBarrier = descDamage.effectiveDamages[DAM_INDEX_BARRIER];

	bool bBurning = damType & DAM_FIRE;
	bool bBarrier = damType & DAM_BARRIER;
	bool bFly     = damType & DAM_FLY;
	bool bFall    = damType & FAM_FALL;
	
	bool bBurningAlready = HasBodyStateModifier(BS_MOD_BURNING);
	bool bInWater        = pAniCtrl->GetWaterLevel() > 0;

	bool bFlyDamage = (nDamageFly + nDamageBarrier) > 0;
	bool bFlyAway   = bFlyDamage && (bBarrier || bFly);

	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fall", "", "");
	if (bFall)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fall", "Returning", "");
		return;
	}
	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Reset Bow", "", "");
	if (GetWeaponMode() == NPC_WEAPON_BOW)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Reset Bow", "Victim is in bow mode", "");

		auto weapon = GetWeapon();
		if (auto morphMesh = zDYNAMIC_CAST<zCMorphMesh*>(weapon))
		{
			PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Reset Bow", "Victim's weapon is a morph mesh", "Started Ani: S_RELAX");
			morphMesh->StartAni("S_RELAX", 1.0, -2.0);
		}
	}

	if (descDamage.npcAttacker)
	{
		GetAngles(descDamage.vobAttacker, descDamage.azimuth, descDamage.elevation);
	}

	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fire", "", "");

#if G1DEMO
	if (nDamageBurn > 1 && bBurning && !bBurningAlready && !bInWater)
	{
		auto msg = new oCMsgConversation(oCMsgConversation::EV_PLAYANI_NOOVERLAY, "S_FIRE_VICTIM"); // g1d:1393
		msg->SetHighPpriority(1);
		GetEM(0)->OnMessage(msg, this);
	}
	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fire", "Fire Animation Message sent", "");
#endif

	PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Stumble", "", "");

	if (descDamage.realDamage > 1 && !bFlyAway && !descDamage.flags.isDead && !descDamage.flags.isUnconscious)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Stumble", "Damage causes stumble", "");

		oCMsgConversation* msg;

		if (IsBodyStateInterruptable())
		{
			ClearEM();
			Interrupt(0);
			SetBodyState(BS_STUMBLE);

			auto prefix = GetInterruptPrefix();
			if (descDamage.fxHit)
			{
				auto sym = zparser.GetSymbol("spellFXAniLetters");
				if (sym)
				{
					auto spellType = descDamage.fxHit->GetSpellType();
					sym->GetValue(prefix, spellType);
				}
				else
				{
					zWARNING("R: NPC: OnDamage_Anim() -> NPC = \"" + GetName() + "\" : SpellAni not found!"); // g1d:1435
				}
				
			}

			auto aniId = pModel->GetAniFromAniID("T_" + prefix + "STUMBLE");
			if (!pModel->GetAniFromId(aniId))
			{
				prefix = GetInterruptPrefix();
			}

			auto aniName = "T_" + prefix + "STUMBLE";

			if (descDamage.azimuth <= 90.0)
			{
				aniName += "B";
			}

			msg = new oCMsgConversation( oCMsgConversation::EV_PLAYANI_NOOVERLAY, aniName ); // g1d:1455
		}
		else
		{
			msg = new oCMsgConversation( oCMsgConversation::EV_PLAYANI, "T_GOTHIT" ); // g1d:1463
		}

		msg->SetHighPpriority(1);
		GetEM(0)->OnMessage(msg, this);

		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Stumble", "Stumble Message sent", "");
	}
	else
	{
		bool spellCast = 0;
		if ( GetWeaponMode() == FMODE_MAGIC )
		{
			if ( GetSpellBook() )
			{
				if ( GetSpellBook()->GetSelectedSpell() )
				{
					if ( GetSpellBook()->GetSelectedSpell()->GetSpellStatus() == SPL_SENDCAST )
						spellCast = 1;
				}
			}
		}
		if (GetAnictrl()->GetWeaponMode() != FMODE_FIST ||
		    spellCast ||
		    inventory.IsOpen() ||
		    IsAPlayer() && (pAniCtrl->IsInPreHit() || pAniCtrl->IsInCombo()))
		{
			PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "", "");
			if (bFlyDamage && !bInWater && GetBodyState != BS_LIE)
			{
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Damage causes fly", "");

				zVEC3 vecDirection;
				if (bBarrier)
				{
					PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Using direction", "");
					vecDirection = descDamage.direction;
				}
				else
				{
					PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Using location", "");

					zVEC3 vecPosition;

					auto vob = descDamage.vobAttacker;
					if (!vob)
						vob = descDamage.vobHit;
					if (vob)
					{
						vecPosition = vob->GetPositionWorld();
					}
					else
					{
						vecPosition = GetAtVectorWorld() * 100 + GetPositionWorld();
					}

					vecDirection = GetPositionWorld();
					vecDirection -= vecPosition;
					vecDirection.NormalizeApprox();
				}

				overrideFallDownHeight = 1;
				pAIHuman->StartFlyDamage(nDamageFly + nDamageBarrier, vecDirection);
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Starting Fly Animation", "");
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Vector [X]", vecDirection[0]);
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Vector [Y]", vecDirection[1]);
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Fly", "Vector [Z]", vecDirection[2]);
			}
			PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Facial Animation", "", "");
			if (zsound && !descDamage.flags.isDead && !descDamage.flags.isUnconscious)
			{
				StartFaceAni(NPC_TALK_ANI, 1.0, -2.0);
				PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Facial Animation", "Started", "");
			}
		}
	}
	if (descDamage.flags.isDead)
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Death", "", "");
		zSTRING aniName = "T_";
		if (pAniCtrl->GetActionMode() == 6 || pAniCtrl->GetActionMode() == 5)
		{
			aniName += "DIVE_2_DROWNED";
		}
		else
		{
			aniName += "DEAD";
			if (descDamage.azimuth <= 90.0)
				azimuth += "B";
		}
		pModel->StartAni(aniName);
		PRINT_DAMAGE_DEBUG("OnDamage_Anim()", "Death", "Started: ", aniName);
	}
}

void oCNpc::OnDamage_Events(oSDamageDescriptor& desc)
{
	auto model = GetModel();
	if ( model )
	{
		if ( desc.flags & 8 )
		{
			PRINT_DAMAGE_DEBUG("OnDamage_Events()", "Unconsciouseness", "Dropping unconscious", "");
			DropUnconscious(desc.rest, desc.source_npc);
		}
		if ( desc.flags & 4 )
		{
			PRINT_DAMAGE_DEBUG("OnDamage_Events()", "Death", "Dieing", "");
			if ( !desc->source_npc  )
			{
				PRINT_DAMAGE_DEBUG("OnDamage_Events()", "Death", "ATTENTION: Attacker is invalid! No XPs will be given!", "");
			}

			auto aniId = model->GetAniIDFromAniName("S_FIRE_VICTIM");
			auto ani = model->GetAniFromAniID();
			model->StopAni(ani);
			DoDie(desc.source_npc);
		}
	
		if (auto magBook = GetSpellBook())
		{
			if (desc.realDamage >= 1.0 && !IsAPlayer() && !bodyStateInterruptableOverride) {
				magBook->StopSelectedSpell();
			}
		}
	}
	else
	{
		PRINT_DAMAGE_DEBUG("OnDamage_Effects_End()", "No valid Model!", "", "");
	}
}


void oCNpc::OnDamage_State(oCNpc::oSDamageDescriptor& desc)
{
	auto anictrl = GetAnictrl();

	bool has_fire_dam = desc.type & DAM_FIRE;
	int  dam_fire = desc.effectiveDamages[DAM_INDEX_FIRE];

	bool burning = HasBodyStateModifier(BS_MOD_BURNING);
	bool in_water = anictrl ? anictrl->GetWaterLevel() > 0 : 0;

	if (dam_fire > 1 && has_fire_dam && !burning && !in_water)
		ModifyBodyState(BS_MOD_BURNING, 0);
}

//zCVob *pVobHit, zCVob *pVobAttacker, float damage, int damageType, const zVEC3 *hitLocation
void oCNpc::OnDamage(zCVob *pVobHit, zCVob *pVobAttacker, float amount, int damageType, zVEC3 const& hitLocation)
{
	oCNpc::oSDamageDescriptor dam;
	memset(&dam, 0, sizeof(dam));
	dam.mult = 1.0;
	dam.damage = damage;

	dam.pos  = hitLocation;
	dam.pos2 = hitLocation;

	dam.vobAttacker = pVobAttacker;
	dam.source      = pVobHit;

	dam.npcAttacker = zDYNAMIC_CAST<oCNpc>(pVobAttacker);
	dam.weapon      = zDYNAMIC_CAST<oCItem>(pVobHit);

	oCVisualFX* vfx = zDYNAMIC_CAST<oCVisualFX>(pVobHit);
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
		switch (dam.offender_npc->GetWeaponMode()) {
		case NPC_WEAPON_FIST:
			dam.mode = DAM_MODE_FIST;
			type |= dam.pNpcAttacker->damagetype;
			break;
		case NPC_WEAPON_DAG:
		case NPC_WEAPON_1HS:
		case NPC_WEAPON_2HS:
			dam.mode |= DAM_MODE_FAR;
			break;
		case NPC_WEAPON_BOW:
		case NPC_WEAPON_CBOW:
			dam.mode |= DAM_MODE_FAR;
			break;
		case NPC_WEAPON_MAG:
			dam.mode |= DAM_MODE_MAGIC;
			break;
		}

		oCItem* weap = dam.npc->GetWeapon();
		if ( dam.weapon ) {
			if ( weap != dam.weapon )
				zINFO(5, "R: NPC: OnDamage() - Weapon is NOT equal to NPC's weapon !!!"); // 2109
		} else {
			dam.weapon = weap;
		}
	}

	if ( dam.weapon ) {
		if ( dam.weapon->HasFlag(ITEM_KAT_NF) )
			dam.mode |= DAM_MODE_MELEE;
		if ( dam.weapon->HasFlag(ITEM_KAT_FF) )
			dam.mode |= DAM_MODE_FAR;
		if ( dam.weapon->HasFlag(ITEM_KAT_MUN) )
			dam.mode |= DAM_MODE_FAR;
		if ( dam.weapon->HasFlag(ITEM_KAT_RUNE) )
			dam.mode |= DAM_MODE_MAGIC:
	}
	dam.type = damageType;

	if ( !type && (dam.weapon || dam.npc) ) {
		if ( dam.mode & DAM_MODE_FAR )
			dam.type = DAM_POINT;
		if ( dam.mode & DAM_MODE_MAGIC )
			dam.type |= DAM_MAGIC;
	}

	OnDamage(dam);
}


