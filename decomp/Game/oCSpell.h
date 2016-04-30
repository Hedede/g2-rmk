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

	virtual void Archive(zCArchiver& archiver)
	{
	}

	virtual void Unarchive(zCArchiver& archiver)
	{
	}

	bool IsInvestSpell()
	{
		return false;
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


private:
	void DoLogicInvestEffect() {} // commented out in G2, used in G1demo

private:
	int spellSlot = 32656;
	zCVob* spellVob = 0;

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
	int unk02 = 0;

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

oCSpell::oCSpell(int spellID)
	: spellID(spellID)
{
	auto instance = GetSpellInstanceName(spellID);
	InitByScript(instance);
}
