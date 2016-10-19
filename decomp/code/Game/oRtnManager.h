struct oCRtnEntry {
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
	int unk2 = 0;
	int unk3 = 0;
	int unk4 = 0;
	int unk[2];
};

struct oCRtnManager {
	char rtn[136];
};
