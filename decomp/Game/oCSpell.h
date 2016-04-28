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
};
