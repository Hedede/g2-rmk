class oCMobBed : public oCMobInter {
	Z_OBJECT(oCMobBed);
public:
	oCMobBed()
		: oCMobInter()
	{
		sceme = "BED";
		addName = "FRONT";
		state_num = 1;
	}
	virtual ~oCMobBed() = default;
	zSTRING GetScemeName() override
	{
		return sceme + "_" + addName;
	}

	void StartInteraction(oCNpc* npc) override
	{
		oCMobInter::StartInteraction(npc);
	}

	void OnBeginStateChange(oCNpc*,int,int) override {}
	void OnEndStateChange(oCNpc* npc, int from, int to) override
	{
		if ( to == 1 ) {
			OnTrigger(this, npc);
		} else if ( to == 0 ) {
			OnUntrigger(this, npc);
		}

		if ( from < to ) {
			SendCallOnStateFunc(npc, to);
		} else if ( from == 1 && to == 0 ) {
			SendCallOnStateFunc(npc, 0);
		}
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

private:
	zSTRING addName;
};
