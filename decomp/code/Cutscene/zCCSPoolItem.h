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

	virtual Archive(zCArchiver& arc)
	{
		arc.WriteString("itemName", *this);
		arc.WriteEnum("runBehaviour", "RUN_ALWAYS;RUN_TIMES;RUN_PERHOUR;RUN_PERDAY", runBehaviour);
		arc.WriteInt("runBehaviourValue", runBehaviourValue);
		arc.WriteInt("numPlayed", numPlayed);
		arc.WriteInt("deactivated", deactivated);
		arc.WriteInt("flags", flags);
	}

	virtual Unarchive(zCArchiver& arc)
	{
		arc.ReadString("itemName", *this);
		zSTRING::Upper();

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
