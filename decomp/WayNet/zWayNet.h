class zCWayNet : public zCObject {
	Z_OBJECT(zCWayNet);
public:
	virtual ~zCWayNet();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	bool HasWaypoint(zCWaypoint* wp) const;

private:
	zCWorld* world;

	// Alle Waypoints   im Netz
	zCListSort  <zCWaypoint> wplist;
	// Alle Ways im Netz
	zCList<zCWay>     waylist;

	enum {
		WAY_LIST_NONE,
		WAY_LIST_OPEN,
		WAY_LIST_CLOSED
	};

	zCListSort    <zCWaypoint>        openList;

	int routeCtr;

};
