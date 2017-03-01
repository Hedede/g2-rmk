struct oCRtnEntry {
	oCRtnEntry() = default;
	oCRtnEntry(int h1, int m1, int h2, int m2, int func, ZSTRING const& wp, int i)
	{
		hour_start = h1;
		min_start = m1;
		hour_end = h2;
		min_end = m2;
		state = func;
		waypoint = wp;
		unk0 = i;
	}

	~oCRtnEntry()
	{
		if (cs)
			delete cs;
	}

	int GetState() const { return state; }
	zSTRING GetWaypoint() const { return waypoint; }

	int GetTime(int& h1, int& m1, int& h2, int& m2) const
	{
		h1 = hour_start;
		m1 = min_start;
		h2 = hour_end;
		m2 = min_end;
	}
	void SetTime(int h1, int m1, int h2, int m2)
	{
		hour_start = h1;
		min_start = m1;
		hour_end = h2;
		min_end = m2;
	}


	int hour_start = 0;
	int min_start = 0;
	int hour_end = 0;
	int min_end = 0;
	int state = 0;
	zSTRING waypoint;
	int unk0 = -1;
	int unk1 = 0;
	oCNpc* npc = nullptr;
	oCRtnCutscene* cs = nullptr;
	int unk4 = 0;
	int unk[2];
};

struct oCRtnCutscene {
	oCRtnCutscene() = default;
	virtual ~oCRtnCutscene()
	{
		ResetPlayState();
	}

	void ResetPlayState()
	{
		if ( ogame ) {
			auto csMan = ogame->GetCutsceneManager();
			csMan->PoolClrFlags(csName, 2);
		}
	}


	oCNpc *npc = nullptr;
	zSTRING csName;
	zSTRING roleName;
	int unk0 = 0;
	int unk1;
// 0x38
};

struct oCRtnManager {
	char rtn0[24];
	zCWorld *world;
	zCListSort *nextEntry;
	zCListSort rtnEntries;
	char rtn1[8];
	zCListSort wayBoxes[3];
	zCArraySort wayBoxLimits[3];
//0x88
};
