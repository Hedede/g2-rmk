class zCWayNet : public zCObject {
	Z_OBJECT(zCWayNet);
public:
	static int EstimateCost(zCWaypoint* from, zCWaypoint* to)
	{
		zVEC3 diff = from.pos - to.pos;
		return abs(diff.x) + abs(diff.y) + abs(diff.z);
	}

	virtual ~zCWayNet();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	void ArchiveOldFormat(zCArchiver& arc);
	void UnarchiveOldFormat(zCArchiver& arc);

	bool HasWaypoint(zCWaypoint* wp) const;

	void Draw(zCCamera* camera);

private:
	zCWorld* world;

	// Alle Waypoints   im Netz
	zCListSort  <zCWaypoint> wplist;
	// Alle Ways im Netz
	zCList<zCWay>     wayList;

	enum {
		WAY_LIST_NONE,
		WAY_LIST_OPEN,
		WAY_LIST_CLOSED
	};

	zCListSort    <zCWaypoint>        openList;

	int routeCtr;
};
