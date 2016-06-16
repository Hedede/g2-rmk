class zCWayNet : public zCObject {
	Z_OBJECT(zCWayNet);
public:
	static int EstimateCost(zCWaypoint* from, zCWaypoint* to)
	{
		zVEC3 diff = from.pos - to.pos;
		return abs(diff.x) + abs(diff.y) + abs(diff.z);
	}

	zCWayNet()
		: zCObject()
	{
		openList.Compare = SortOpenList;
	}

	zCWayNet(zCWorld* wld)
		: zCWayNet(), world(wld)
	{ }

	virtual ~zCWayNet()
	{
		wplist.DeleteListDatas();
		wayList.DeleteListDatas();
	}

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void ArchiveOldFormat(zCArchiver& arc);
	void UnarchiveOldFormat(zCArchiver& arc);

	void Draw(zCCamera* camera);

	bool HasWaypoint(zCWaypoint* wp) const;
	void InsertWaypoint(zCWaypoint* wp);

	zCWaypoint* HasWaypoint(zVEC3& pos) const;
	zCWaypoint* HasWaypoint(float x, float y, float z) const
	{
		return HasWaypoint(zVEC3{x,y,z});
	}
	void InsertWaypoint(float x, float y, float z);

	zCWayPoint* SearchWaypoint(zCVobWaypoint* wpvob);
	zCWayPoint* GetWaypoint(zSTRING const& name);
	zCWayPoint* GetNearestWaypoint(zVEC3 const& pos);
	zCWayPoint* GetSecNearestWaypoint(zVEC3 const& pos);

	void CreateWay(zCWayPoint* wp1, zCWayPoint* wp2);
	void AddWays(zCWayPoint* wp1, zCWayPoint* wp2);


	void CalcProperties(zCWorld* wld);

	zCRoute* CreateRoute(zCWaypoint* to);

	zCRoute* FindRoute(zCWaypoint* from, zCWaypoint* to, zCVob* vob)
	{
		if ( AStar(from, to, vob) )
			return CreateRoute(to);
		return nullptr;
	}

	zCRoute* FindRoute(zSTRING const& from, zSTRING const& to, zCVob* vob)
	{
		auto wp1 = GetWaypoint(from);
		auto wp2 = GetWaypoint(to);
		return FindRoute(wp1, wp2, vob);
	}

	zCRoute* FindRoute(zVEC3 const& from, zCWaypoint* to, zCVob* vob)
	{
		auto wp1 = GetNearestWaypoint(from);
		return FindRoute(wp1, to, vob);
	}

	zCRoute* FindRoute(zVEC3 const& from, zSTRING const& to, zCVob* vob)
	{
		auto wp1 = GetNearestWaypoint(from);
		auto wp2 = GetWaypoint(to);
		return FindRoute(wp1, wp2, vob);
	}

	zCRoute* FindRoute(zVEC3 const& from, zVEC3 const& to, zCVob* vob)
	{
		auto wp1 = GetNearestWaypoint(from);
		auto wp2 = GetNearestWaypoint(to);
		return FindRoute(wp1, wp2, vob);
	}

	void CorrectHeight()
	{
		for (auto wp : wplist)
			wp->CorrectHeight(world);
	}

	void RemoveUnusedWPVobs();

private:
	void InsertInClosed(zCWaypoint* wp)
	{
		wp->curList = WAY_LIST_CLOSED;
		wp->routeCtr = this->routeCtr;
	}

	void RemoveFromClosed(zCWaypoint* wp)
	{
		wp->curList = WAY_LIST_NONE;
	}

	void InsertInOpen(zCWaypoint* wp)
	{
		openList.InsertSort(wp);
		wp->curList = WAY_LIST_OPEN;
		wp->routeCtr = this->routeCtr;
	}

	bool IsInClosed(zCWaypoint* wp)
	{
		return wp->routeCtr == this->routeCtr && wp->curList == 2;
	}

	bool IsInOpen(zCWaypoint* wp)
	{
		return wp->routeCtr == this->routeCtr && wp->curList == 1;
	}

	bool IsInAnyList(zCWaypoint* wp)
	{
		return wp->routeCtr == this->routeCtr && (in(wp->curList, 1, 2));
	}

	static int SortOpenList(zCWaypoint* wp1, zCWaypoint wp2)
	{
		return 2 * (wp1->score >= wp2->score) - 1;
	}

	static void CreateWPVobList(zCList<zCVobWaypoint>& list, zCTree<zCVob>* tree)
	{
		for (auto i = tree; i; i = i->next) {
			if ( i->data->type == VOB_TYPE_WAYPOINT )
				list.Insert(i->data);
			CreateWPVobList(list, i->firstChild);
		}
	}

private:
	zCWorld* world = nullptr;

	// Alle Waypoints   im Netz
	zCListSort<zCWaypoint> wplist;
	// Alle Ways im Netz
	zCList<zCWay>     wayList;

	enum {
		WAY_LIST_NONE,
		WAY_LIST_OPEN,
		WAY_LIST_CLOSED
	};

	zCListSort<zCWaypoint> openList;

	int routeCtr = 0;
};
