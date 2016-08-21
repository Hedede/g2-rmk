class zCRoute {
	virtual ~zCRoute();
private:
	zCList<zCWay>       wayList;

	zCWaypoint*         startwp;
	zCWaypoint*         target;

	zCWay*              way;
	zCList<zCWay>*      waynode;
};
