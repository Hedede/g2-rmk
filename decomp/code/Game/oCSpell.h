enum SpellReleaseStatus {
	SPL_DONTINVEST = 0,
	SPL_RECEIVEINVEST = 1,
	SPL_SENDCAST = 2,
	SPL_SENDSTOP = 3,
	SPL_NEXTLEVEL = 4,
	SPL_CANINVEST_NO_MANADEC = 8,
	SPL_FORCEINVEST = 1 << 16,
};

class oCSpell : public zCObject {
	Z_OBJECT(oCSpell);
public:
	oCSpell(int _spellID);
	virtual ~oCSpell();

	void Archive(zCArchiver& archiver) override { }
	void Unarchive(zCArchiver& archiver) override { }

	bool IsInvestSpell()
	{
		return false;
	}

	void SetInvestedMana(int mana)
	{
		manaInvested = mana;
		if ( !mana )
			__time_invest = 0;
	}

	zCVob* GetVob()
	{
		return spellVob;
	}

	int GetSpellID()
	{
		return spellId;
	}

	int GetLevel()
	{
		return spellLevel;
	}

	int GetSpellStatus()
	{
		return spellStatus;
	}

	int GetCategory()
	{
		return spellCat;
	}

	int GetEnergyType() const
	{
		return energyType;
	}

	bool GetEnabled() const
	{
		return isEnabled;
	}

	void SetSpellInfo(int infoValue)
	{
		spellInfo = infoValue;
	}

	bool CanTurnDuringInvest() const
	{
		return canTurnDuringInvest;
	}

	bool CanBeDeleted() const
	{
		return canBeDeleted;
	}

	// my addition
	bool IsTransformSpell() const
	{
		return spellType >= SPL_TrfFirst && spellType <= SPL_TrfLast;
	}

	bool DeleteCaster() const
	{
		return IsTransformSpell();
	}


private:
	void DoLogicInvestEffect() {} // commented out in G2, used in G1demo
	void DoTimedEffect();

	void StopTargetEffects(zCVob *vob)
	{
		zINFO("C: SPL: StopTargetEffects"); // 806
	}
private:
	int spellSlot = 32656;
	oCVisualFX *spellEffect;

	zCVob* __unk1 = 0;

	zCVob* casterVob = 0;
	oCNpc* casterNpc = 0;
	zCVob* targetVob = 0;
	oCNpc* targetNpc = 0;

	zCVob* __unkVob = 0;

	float timeInvested = 0;
	int manaInvested   = 0;

	int spellLevel  = 0;
	int spellStatus = 1;
	int spellID     = 0;
	int spellInfo   = 0;

	int isEnabled = 0;

	int unk01 = 0;

	int __timeLeft;
	int canBeDeleted = 0; // __timedEffectEnd;

	int unk1 = 0;
	int unk2 = 0;
	int unk3 = 0;
	float unk4 = 1.0;
	int energyType = ATR_MANA;

	// scripted
	float time_per_mana  = 2000.0;
	int damage_per_level = 0;
	int damagetype = DAM_MAGIC;
	int spellCategory;
	int canTurnDuringInvest = 1;
	int canChangeTargetDuringInvest = 1;
	int isMultiEffect;
	int targetCollectAlgo = 2;
	int targetCollectType = 1;
	int targetCollectRange = 10000;
	int targetCollectAzi  = 180;
	int targetCollectElev = 90;
	//end scripted
};

//------------------------------------------------------------------------------
//_carsten/oSpell.cpp
//------------------------------------------------------------------------------
oCSpell::oCSpell(int spellID)
	: spellID(spellID)
{
	auto instance = GetSpellInstanceName(spellID);
	InitByScript(instance);
}

void oCSpell::DoTimedEffect()
{
	if (__timeLeft > 0.0) {
		__timeLeft -= ztimer.frameTimeFloat;
		if (__timeLeft <= 0) {
			canBeDeleted = 1;
			EndTimedEffect();
		}
	}

	if (!in(spellType, SPL_Light, SPL_PalLight) && spellType->CanBeDeleted) {
		Release(spellEffect);
		canBeDeleted = 1;
	}
}

void oCSpell::EndTimedEffect()
{
	zINFO(8, "C: SPL: EndTimedEffect"); // 1237

	canBeDeleted = 1;
	if (IsTransformSpell()) {
		if (__npcTransform && __npcTarget) {
			zVEC3 pos = __npcTarget->GetPositionWorld(); // [3]
			__npcTransform->SetTrafoObjToWorld(__npcTarget->trafoObjToWorld);
			ogame->GetGameWorld()->AddVob(__npcTransform);
			__npcTransform->SetCollDetStat(0);
			__npcTransform->SetCollDetStat(0);

			if ( __npcTransform->SearchNpcPosition(pos) ) {
				__npcTransform->Enable(pos);
				__npcTransform->ResetRotationsWorld();

				zVEC3 dir = __npcTransform->GetHeadingAtWorld(); // [2]
				zCVob::ResetRotationsWorld(&this->__npcTransform->vtbl);
				__npcTransform->SetHeadingAtWorld(dir);
				__npcTransform->ResetXZRotationsWorld();
				__npcTransform->SetCollDetStat(1);
				__npcTransform->SetCollDetDyn(1);

				if (!__npcTarget->isInMovementMode)
					ogame->GetGameWorld()->RemoveVob(__npcTarget);

				__npcCaster->CopyTransformSpellInvariantValuesTo(__npcCaster, __npcTransform);
				__npcTransform->GetAnictrl()->SearchStandAni(0);
				__npcTransform->SetAsPlayer();

				spellEffect = CreateEffect();
				spellEffect->Init(__npcTransform, 0, 0);
				spellEffect->SearchStandAni(targetCollectType);
				spellEffect->Cast(1);
				Release(spellEffect);

				auto msg = new oCMsgConversation(EV_PLAYANI_NOOVERLAY, "T_TRFSHOOT_2_STAND");
				msg->SetHighPriority(1);
				__npcTransform->GetEM()->OnMessage(msg, __npcTransform);
				__npcTransform->CreatePassivePerception(PERC_ASSESSSURPRISE, __npcTransform, 0);
			} else {
				auto pos = __npcTarget->GetPositionWorld();
				auto fx = oCVisualFX::CreateAndPlay("TRANSFORM_NOPLACEFX", pos, 0, 1, 0.0, 0, 0);
				Release(fx);
				__npcTransform->SetCollDetStat(1);
				__npcTransform->SetCollDetDyn(1);
				__npcTransform->Disable();

				canBeDeleted = 0;
				__timeLeft = 0;
			}
		}
	}
}

