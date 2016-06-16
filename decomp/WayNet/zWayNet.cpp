bool zCWayNet::HasWaypoint(zCWaypoint* wp)
{
	for (auto list : wplist) {
		if (list == wp)
			return true;
	}

	return false;
}

void zCWayNet::InsertWaypoint(zCWaypoint* wp)
{
	if (!HasWaypoint(wp))
		wplist.InsertSort(wp);

	return false;
}

zCWaypoint* zCWayNet::HasWaypoint(zVEC3& pos) const
{
	for (auto wp: wplist) {
		if (wp->GetPositionWorld() == pos)
			return wp;
	return nullptr;
}

zCWaypoint* zCWayNet::SearchWaypoint(zCVobWaypoint* wpvob)
{
	for (auto wp : wplist) {
		if (wp->wpvob == wpvob)
			return wp;
	}
	return nullptr;
}

zCWaypoint* zCWayNet::GetNearestWaypoint(zVEC3 const& pos)
{
	zCWaypoint* nearest = nullptr;
	float min_dist = std::numeric_limits<float>::max();

	for (auto wp : wplist) {
		float dist = abs(wp->dist - pos);
		if (dist < min_dist) {
			nearest = wp;
			min_dist = dist;
		}
	}

	return nearest;
}

void zCWayNet::CalcProperties(zCWorld *wld)
{
	for (auto wp : wplist)
		wp->CalcProperties(wld);
	for (auto way : wayList)
		way->CalcProperties(wld);
}

void zCWayNet::ArchiveOldFormat(zCArchiver& arc)
{
	CheckConsistency(1);

	int numWaypoints = 0;
	for (auto list : wplist) {
		if (list->Get()->GetNumberOfWays() == 0)
			++numWaypoints;
	}

	arc.WriteInt("numWaypoints", int numWaypoints);

	int i = 0;
	for (auto list : wplist) {
		auto wp = list->Get();
		if (wp->GetNumberOfWays() == 0) {
			arc.WriteString("waypoint"_s + i, wp->GetName());
			++i;
		}
	}

	int numWays = wayList.GetNumInList();
	arc.WriteInt("numWays", numWays);

	for (auto way : wayList) {
		zSTRING wayl, wayr;
		if (way->left)
			wayl = way->left->GetName();
		if (way->right)
			wayr = way->right->GetName();

		arc.WriteString("way"_s + i, wayl + "/" + wayr);
	}
}

void zCWayNet::Archive(zCArchiver& arc)
{
	zINFO(5,"U:WAY: Writing Waynet."); //1453

	if ( !arc.InSaveGame() )
		CheckConsistency(1);

	arc.WriteInt("waynetVersion", 1);

	int numwp = 0;
	for (auto wp : wplist) {
		if (wp->GetNumberOfWays() == 0)
			++numwp;
	}

	arc.WriteInt("numWaypoints", numwp);

	int i = 0;
	for (auto wp : wplist) {
		if (wp->GetNumberOfWays() == 0) {
			arc.WriteObject("waypoint"_s + i, wp);
			++i;
		}
	}

	int numWays = wayList.GetNumInList();

	arc.WriteInt("numWays", numWays);

	for (auto way : wayList) {
		arc.WriteObject("wayl"_s + i, way->left);
		arc.WriteObject("wayr"_s + i, way->right);
	}

	zINFO(5, "U:WAY: Writing ok."); //1503
}

void zCWayNet::UnarchiveOldFormat(zCArchiver& arc)
{
	zINFO(5, "U:WAY: Reading Waynet (old format)."); // 1508

	int numwp = arc.ReadInt("numWaypoints");
	for (int i = 0; i < numwp; ++i) {
		auto name = arc.ReadString("waypoint"_s + i);
		name = name.Copied("[", 4).Delete("]",4);

		if ( GetWaypoint(name) )
			continue;

		auto vob = world->SearchVobByName(name);
		auto wpvob = dynamic_cast<zCVobWaypoint>(vob);

		if (!wpvob) {
			zWARNING("U: WAY: Waypoint not found : " + name); // 1528
		} else {
			auto wp = zfactory->CreateWaypoint();

			wp->Init(wpwob);
			wplist.InsertSort(wp);
		}
	}

	int numWays = arc.ReadInt("numWays");
	for (int i = 0; i < numWays; ++i) {
		zSTRING way = arc.ReadString("way"_s + i);

		zSTRING wayl = way.Copied(way, "/", 1);
		zSTRING wayr = way.Copied(way, "/", 2);

		auto left = GetWaypoint(wayl);
		if (!left) {
			auto vob = world->SearchVobByName(wayl);
			auto wpwob = dynamic_cast<zCVobWaypoint>(vob);
			if (!wpwob) {
				zWARNING("U:WAY: Waypoint not found : " + wayl); // 1551
			} else {
				auto wp = zfactory->CreateWaypoint();

				wp->Init(wpwob);
				wplist.InsertSort(wp);
			}
		}

		auto right = GetWaypoint(wayr);
		if (!right) {
			auto vob = world->SearchVobByName(wayr);
			auto wpwob = dynamic_cast<zCVobWaypoint>(vob);
			if (!wpwob) {
				zWARNING("U:WAY: Waypoint not found : " + wayr); // 1565
			} else {
				auto wp = zfactory->CreateWaypoint();

				wp->Init(wpwob);
				wplist.InsertSort(wp);
			}
		}

		auto way = zfactory->CreateWay();
		way->Init(left, right);
		wayList.Insert(way);
	}

	ClearVobDependencies();
	zINFO(5, "U:WAY: Waynet ok (old format)."); // 1602
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


void zCWayNet::Draw(zCCamera* camera)
{
	camera->Activate();

	for (auto& way : wayList)
		way->Draw(camera);
}
