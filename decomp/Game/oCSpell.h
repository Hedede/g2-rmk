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
		return level;
	}

	int GetSpellStatus()
	{
		return spellStatus;
	}

	int GetCategory()
	{
		return spellCat;
	}

	void SetSpellInfo(int infoValue)
	{
		spellInfo = infoValue;
	}

	bool CanBeDeleted() const
	{
		return canBeDeleted;
	}

private:
	void DoLogicInvestEffect() {} // commented out in G2, used in G1demo

private:
	int spellSlot;
	zCVob *spellVob;

	int __unk1;

	zCVob *casterVob;
	oCNpc *__npcCaster;
	zCVob *targetVob;
	oCNpc *__npcTarget;

	zCVob* __unkVob;

	float __timeInvested;
	int manaInvested;
	char __investLevel;
	int releaseStatus;

	int spellType;
	int level;
	int spellStatus;
	int spellId;
	int spellInfo;

	int canBeDeleted; // __timedEffectEnd;
	char unk1[20];
	int __invest_attribute;

	// scripted
	float time_per_mana;
	int damage_per_level;
	int damagetype;
	int spellCategory;
	int canTurnDuringInvest;
	int canChangeTargetDuringInvest;
	int isMultiEffect;
	int targetCollectAlgo;
	int targetCollectType;
	int targetCollectRange;
	int targetCollectAzi;
	int targetCollectElev;
	//end scripted

	char unk2[80];
};
