class oCMobDoor : public oCMobLockable {
	Z_OBJECT(oCMobDoor);
public:
	oCMobDoor()
		: oCMobLockable()
	{
		sceme = "DOOR";
		addName = "FRONT";
		state_num = 1;
	}

	virtual ~oCMobDoor() = default;

	zSTRING GetScemeName() override
	{
		return sceme + "_" + addName;
	}

	TMobOptPos* SearchFreePosition(oCNpc* npc, float angle) override
	{
		auto pos = oCMobInter::SearchFreePosition(npc, angle);
		if (pos->name.Search(0, "FRONT", 1) > 0)
			addName = "FRONT";
		else
			addName = "BACK";
		return pos;
	}

	void Open(oCNpc *) override {}
	void Close(oCNpc *) override {}

private:
	zSTRING addName; // FRONT / BACK
};
