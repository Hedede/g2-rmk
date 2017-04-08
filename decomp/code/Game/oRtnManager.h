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
	int instance = -1;
	int unk1 = 0;
	oCNpc* npc = nullptr;
	oCRtnCutscene* cs = nullptr;
	int overlay = 0;
//0x3C
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
	oCRtnManager()
	{
		for(int i = 0; i < 3; ++i) {
			rtn0[i] = 0;
			rtn0[i + 3] = 0;
		}
		rtnEntries.compare  = oCRtnManager::Sort_Routine;
		wayBoxes[0].compare = oCRtnManager::Sort_WayBoxX;
		wayBoxes[1].compare = oCRtnManager::Sort_WayBoxY;
		wayBoxes[2].compare = oCRtnManager::Sort_WayBoxZ;
	}
	~oCRtnManager() = default;

	void SetWorld(zCWorld* wld)
	{
		world = wld;
	}

	int GetNumOfWayBoxes() const
	{
		return wayBoxes[0].GetNumInList();
	}

	void Insert(oCNpc *npc, oCRtnEntry *rtn)
	{
		if ( rtn ) {
			if ( npc ) {
				rtn->npc = npc;
				rtnEntries.InsertSort(rtn);
			}
		}
	}

	void DrawWayBoxes();

	struct TRtn_WayBox { zTBBox3D bbox; };
	struct TRtn_WayBoxLimit {
		int type;
		zTBBox3D bbox;
		float limit;
		oCNpc *npc;
	};
	
private:
	static int Sort_WayBoxX(TRtn_WayBox *a, TRtn_WayBox *b)
	{
		return (a->bbox.mins[0] > b->bbox.mins[0]) ? 1 : -1;
	}
	static int Sort_WayBoxY(TRtn_WayBox *a, TRtn_WayBox *b)
	{
		return (a->bbox.mins[1] > b->bbox.mins[1]) ? 1 : -1;
	}
	static int Sort_WayBoxZ(TRtn_WayBox *a, TRtn_WayBox *b)
	{
		return (a->bbox.mins[2] > b->bbox.mins[2]) ? 1 : -1;
	}
	static int Sort_WayBoxLimit(TRtn_WayBoxLimit *a, TRtn_WayBoxLimit *b)
	{
		return (a->limit > b->limit[2]) ? 1 : -1;
	}

private:
	char rtn0[24];
	zCWorld *world = nullptr;
	zCListSort<oCRtnEntry> *nextEntry = nullptr;
	zCListSort<oCRtnEntry> rtnEntries;
	zCListSort<TRtn_WayBox> wayBoxes[3];
	zCList<oCNoc*> npcs;
	zCArraySort<TRtn_WayBoxLimit*> wayBoxLimits[3];
//0x88
};

// _Ulf/oRoutine.cpp
void oCRtnManager::InitWayBoxes()
{
	auto npcs = ogame->GetGameWorld()->voblist_npcs;
	for ( oCNpc* npc : npcs ) {
		if (!npc->IsDead() || npc->IsAPlayer())
			CreateWayBoxes(npc);
	}
}

void oCRtnManager::DrawWayBoxes()
{
	for ( auto box : wayBoxes ) {
		zCOLOR color;
		// don't remeber, is it rgba or bgra?
		color[2] = 0;
		color[1] = 0;
		color[0] = -1;
		color[3] = -1;
		box.Draw(color);
	}
}

// private
void oCRtnManager::InsertWaybox(oCNpc *npc, zVEC3& mins, zVEC3& maxs)
{
	// very messy with stupid compiler shortcut pointer manipulation
	// "mins[i]" is compiled as as "maxs[i] + (mins - maxs)" (all pointers)
	int i = 3;
	while (i --> 0) {
		auto min = new TRtn_WayBoxLimit;
		min->type = 1;
		min->npc = npc;
		min->bbox.mins = mins;
		min->bbox.maxs = maxs;
		min->limit = mins[i];
		wayBoxLimits[i].InsertSort(min);

		auto max = new TRtn_WayBoxLimit;
		max->type = 0;
		max->npc = npc;
		max->bbox.mins = mins;
		max->bbox.maxs = maxs;
		max->limit = maxs[i];
		wayBoxLimits[i].InsertSort(max);
	}
}
