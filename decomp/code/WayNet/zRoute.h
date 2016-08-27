struct zCRoute {
	virtual ~zCRoute() = default;

	void SetStart(zCWaypoint* wp)
	{
		target = wp;
		startwp = wp;
	}

	int GetNumberOfWaypoints() const
	{
		return wayList.GetNumInList();
	}

	bool IsEmpty() const
	{
		return GetNumberOfWaypoints() <= 0;
	}

	zCWay* GetCurrentWay()
	{
		return way;
	}

	zCWaypoint* GetTargetWP()
	{
		return target;
	}

	bool IsLastRarget() const
	{
		if (waynode)
			return waynode->Next() == nullptr;
		return true;
	}

private:
	zCList<zCWay>       wayList;

	zCWaypoint*         startwp = nullptr;
	zCWaypoint*         target  = nullptr;

	zCWay*              way     = nullptr;
	zCList<zCWay>*      waynode = nullptr;
};

zCWaypoint* zCRoute::GetNextWP()
{
	if ( waynode ) {
		waynode = waynode->next;
	} else {
		auto num = GetNumberOfWaypoints();
		if (IsEmpty()) {
			target = startwp;
			startwp = 0;
			return target;
		}

		waynode = wayList.Next();
	}

	if ( waynode ) {
		way = waynode->Get();

		if ( way->left == target )
			target = way->right;
		else
			target = way->left;
		return target;
	}

	return nullptr;
}
