class zCCSPoolItem : public zSTRING, public zCObject {
	Z_OBJECT(zCCSPoolItem);
public:
	zCCSPoolItem(zCCSCutsceneContext *context)
	{
		if (context)
			zSTRING::operator=(context->GetCSName())

		Init();

		zSTRING::Upper();
		return v2;
	}

	virtual ~zCCSPoolItem() = default;
	virtual PrintDebugInfo();

	void Archive(zCArchiver& arc) override
	{
		arc.WriteString("itemName", *this);
		arc.WriteEnum("runBehaviour", "RUN_ALWAYS;RUN_TIMES;RUN_PERHOUR;RUN_PERDAY", runBehaviour);
		arc.WriteInt("runBehaviourValue", runBehaviourValue);
		arc.WriteInt("numPlayed", numPlayed);
		arc.WriteInt("deactivated", deactivated);
		arc.WriteInt("flags", flags);
	}

	void Unarchive(zCArchiver& arc) override
	{
		arc.ReadString("itemName", *this);
		this->Upper();

		arc.ReadEnum("runBehaviour", runBehaviour);
		arc.ReadInt("runBehaviourValue", runBehaviourValue);
		arc.ReadInt("numPlayed", numPlayed);
		arc.ReadInt("deactivated", deactivated);
		arc.ReadInt("flags", flags);
	}

	int GetRunBehaviour() const
	{
		return runBehaviour;
	}

	int GetRunBehaviour(int& value) const
	{
		value = runBehaviourValue;
		return runBehaviour;
	}

	void SetRunBehaviour(zCCSProps::zTCSRunBehaviour beh, int& value)
	{
		runBehaviour = beh;
		runBehaviourValue = value;
	}

	void SetFlags(int flag)
	{
		flags |= flag;
	}

	void ClrFlags(int flag)
	{
		flags &= ~flag;
	}

	bool HasFlags(int flag)
	{
		return (flag & this->flags) == flag;
	}

	bool IsAllowedToPlay()
	{
		return runBehaviour == 0 || runBehaviourValue > numRuns || runBehaviour == 0; // [sic]
	}

	void TestedDeactivation()
	{
		if ( runBehaviour == 1 && runBehaviourValue >= numRuns )
			deactivated = 1;
	}

protected:
	zCCSPoolItem()
	{
		Init();
	}

	void Init()
	{
		deactivated = 0;
		numPlayed = 0;
		unk = 0;
		flags = 0;
	}


private:
	int deactivated;
	int runBehaviourValue;
	int runBehaviour;
	int numPlayed;
	int unk;
	int flags;
};

//------------------------------------------------------------------------------
//_andre/zCCsPool.cpp
void zCCSPoolItem::PrintDebugInfo()
{
	zINFO_BEGIN(1, "B: CS: Cutscene \"" + this + "\" in history-pool"); // 93
	if (deactivated)
		zINFO(1, "B: CS: *** deactivated ***"); // 95
	zINFO(1, "B: CS: num of runs: " + numRuns); // 97
	zINFO_END(1, ""); // 104
}
