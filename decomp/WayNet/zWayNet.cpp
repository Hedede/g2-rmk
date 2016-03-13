bool zCWayNet::HasWaypoint(zCWaypoint* wp)
{
	zCList<zCWaypoint>* node = wplist;

	for (auto list : wplist) {
		if (list->Get() == wp)
			return true;
	}

	return false;
}

void zCWayNet::Archive(zCArchiver& arc)
{
	zINFO(5,"U:WAY: Writing Waynet."); //1453

	if ( !arc.InSaveGame() )
		CheckConsistency(1);

	arc.WriteInt("waynetVersion", 1);

	int numwp = 0;
	for (auto list : wplist) {
		auto wp = list->Get();

		if (wp->wayList->GetNumInList() == 0)
			++numwp;
	}

	arc.WriteInt("numWaypoints", numwp);

	int i = 0;
	for (auto list : wplist) {
		auto wp = list->Get();

		if (wp->wayList->GetNumInList() == 0) {
			arc.WriteObject("waypoint"_s + i, wp);
			++i;
		}
	}

	int numWays = wayList.GetNumInList();

	arc.WriteInt("numWays", numWays);

	for (auto list : wayList) {
		auto way = list->Get();

		arc.WriteObject("wayl"_s + i, way->left);
		arc.WriteObject("wayr"_s + i, way->right);
	}

	zINFO(5, "U:WAY: Writing ok."); //1503
}

void zCArchiver::UnarchiveOldFormat(zCArchiver& arc)
{
}

void zCWayNet::Unarchive(zCArchiver& arc)
{
	zINFO(5,"U:WAY: Reading Waynet."); //1607

	int waynetVersion = ReadInt("waynetVersion");
	if (waynetVersion == 0) {
		UnarchiveOldFormat(arc);
	} else {
		int numwp = arc.ReadInt("numWaypoints");
		for (int i = 0; i < numwp;) {
			auto obj = arc.ReadObject("waypoint"_s + i);
			zCWaypoint* wp = static_cast<zCWaypoint*>(obj);
			if ( !HasWaypoint(obj) ) {
				wplist.InsertSort(wp);
				++i;
			}
		}

		int numWays = ReadInt("numWays");

		for (int i = 0; i < numWays; ++i) {
			auto obj = arc.ReadObject("wayl"_s + i);
			auto wayl = static_cast<zCWaypoint*>(obj);
			if (wayl->RefCount() <= 2) {
				wplist.InsertSort(wayl);
			} else {
				wayl->Release();
			}

			obj = arc.ReadObject("wayr"_s + i);
			auto wayr = static_cast<zCWaypoint*>(obj);
			if (wayr->RefCount() <= 2) {
				wplist.InsertSort(wayr);
			} else {
				wayr->Release();
			}

			auto way = zfactory->CreateWay();
			way->Init(wayl, wayr);
			wayList.Insert(way);
		}
	}

	zINFO(5, "U:WAY: Waynet ok."); // 1661
}
