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


	void DeleteWaypoint(zCWaypoint* wp)
	void DeleteWay(zCWaypoint *wp1, zCWaypoint *wp2)

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
	void UpdateVobDependencies();
	void ClearVobDependencies();
	void CreateVobDependencies(zCWorld* wld);
	zSTRING MergeWaypoints();
	zSTRING CheckConsistency(bool isFault);

	int AStar(zCWaypoint* from, zCWaypoint* to, zCVob const* vob);

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

	static int SortOpenList(zCWaypoint* wp1, zCWaypoint* wp2)
	{
		return 2 * (wp1->score >= wp2->score) - 1;
	}

	static int SortWpList(zCWaypoint* wp1, zCWaypoint* wp2)
	{
		// maybe other way around?
		return wp1->GetName() < wp2->GetName() ? -1 : 1;
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

void zCWayNet::CreateWay(zCWaypoint *wp1, zCWaypoint *wp2)
{
	if ( wp1 == wp2 )
		return;

	if (!HasWaypoint(wp1))
		InsertWaypoint(wp1);
	if (!HasWaypoint(wp2))
		InsertWaypoint(wp2);

	if (!wp1->HasWay(wp2)) {
		auto way = zfactory->CreateWay();
		way->Init(wp1, wp2);
		waylist.Insert(way);
	}
}

void zCWayNet::AddWays(zCWaypoint *left, zCWaypoint *right)
{
	for (auto way : right->wayList)
		CreateWay(left, way->GetGoalWaypoint(right));
}

zCWaypoint* zCWayNet::HasWaypoint(zVEC3& pos) const
{
	for (auto wp: wplist) {
		if (wp->GetPositionWorld() == pos)
			return wp;
	return nullptr;
}

void zCWayNet::InsertWaypoint(float x, float y, float z)
{
	if (!HasWaypoint(x, y, z)) {
		auto wp = zfactory->CreateWaypoint();
		wp->pos = zVEC3{x,y,z};
		InsertWaypoint(wp);
	}
}

zCWaypoint* zCWayNet::SearchWaypoint(zCVobWaypoint* wpvob)
{
	for (auto wp : wplist) {
		if (wp->wpvob == wpvob)
			return wp;
	}
	return nullptr;
}

zCWaypoint* zCWayNet::GetWaypoint(zSTRING const& name)
{
	// Тут был бредовый binary search с индексами,
	// но потому что это linked list, оно не имеет смысла
	for (auto wp : wplist)
		if (wp->GetName() == name)
			return wp;
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

zCWaypoint* zCWayNet::GetSecNearestWaypoint(zVEC3 *pos)
{
	zCWaypoint* near1 = nullptr;
	zCWaypoint* near2 = nullptr;
	unsigned min1 = -1;
	unsigned min2 = -1;

	for (auto wp : wplist) {
		zVEC3 diff = wp->dist - pos;
		unsigned dist = abs(diff.x) + abs(diff.y) + abs(diff.z);
		if (dist < min2) {
			if (dist < min1) {
				min2 = min1;
				near2 = near1;
				min1 = dist;
				near1 = wp;
			} else {
				min2 = dist;
				near2 = wp;
			}
		}
	}

	return near2;
}

void zCWayNet::CalcProperties(zCWorld *wld)
{
	for (auto wp : wplist)
		wp->CalcProperties(wld);
	for (auto way : wayList)
		way->CalcProperties(wld);
}

zCRoute* zCWayNet::CreateRoute(zCWaypoint *to)
{
	auto route = new zCRoute{};
	zCWaypoint* wp = to;
	while (wp) {
		zCWay* parent = wp->parent;
		if ( !parent )
			break;
		route.wayList.InsertFront(parent);
		wp = parent->GetGoalWaypoint(wp);
	}

	route->target = wp; // `wp` in asm, but shouldn't it be `to`?
	route->startwp = wp;

	return route;
}

void zCWayNet::InsertWaypoint(zCWaypoint *wp1, zCWaypoint *wp2, zCWaypoint *wp3)
{
	if ( wp1 != wp2 && wp2 != wp3 && wp1 != wp3 ) {
		InsertWaypoint(wp1);
		InsertWaypoint(wp2);
		InsertWaypoint(wp3);

		for (auto way : wp1->wayList) {
			if (way->GetGoalWaypoint(wp1) == wp3) {
				wp1->wayList.Remove(way);
				wp3->wayList.Remove(way);
				this->waylist.Remove(way);
				way->Free();
				delete way;
				break;
			}
		}

		CreateWay(wp1, wp2);
		CreateWay(wp2, wp3);
	}
}

void zCWayNet::DeleteWaypoint(zCWaypoint* wp)
{
	zCWaypoint* wp1 = nullptr;
	for (auto wp2 : wplist) {
		if (wp2 == wp) {
			wp1 = wp2;
			break;
		}
	}
	
	if (!wp1)
		return;

	for (auto way : waylist) {
		if (way->left == wp || way->right == wp) {
			waylist.Remove(way);

			if (way->left)
				way->left->wayList.Remove(way);
			if (way->right)
				way->right->wayList.Remove(way);

			delete way;
		}
	}

	wplist.Remove(wp);
	wp->Free();
	Release(wp);
}

void zCWayNet::DeleteWay(zCWaypoint *wp1, zCWaypoint *wp2)
{
	zCWay* way = nullptr;
	for (auto way1 : wp1->wayList) {
		if (way1->GetGoalWaypoint(wp1) == wp2) {
			way = way1;
			break;
		}
	}

	if (!way)
		return;

	wp1->wayList.Remove(way);
	wp2->wayList.Remove(way);
	this->wayList.Remove(way);
	way->Free();
	delete way;
}

void zCWayNet::GetWPList(zCList<zVEC3>& wpl, zVEC3& pos, float mindist, float maxdist)
{
	for (auto wp : wplist) {
		float dist = (wp->GetPositionWorld() - pos).LengthApprox();
		if (mindist <= dist && dist <= maxdist) {
			if ( mindist <= v13 && v13 <= maxdist )
				wpl.Insert(wp->pos);
		}
	}
}

void zCWayNet::UpdateVobDependencies()
{
	zINFO(5,"U: WAYNET: Updating VobDependencies..."); 412,

	for (auto wp : wplist)
		wp->UpdatePositionWorld();

	zINFO(5,""); //424,
}

void zCWayNet::ClearVobDependencies()
{
	zINFO(5,"U: WAYNET: Clearing VobDependencies..."); // 432, zWaynet.cpp
	for (auto wp : wplist) {
		if (wp->wpvob) {
			auto wld = wp->wpvob->homeWorld;
			if (wld)
				wld->RemoveVob(wp->wpvob);
			Release(wp->wpvob);
		}

	}
	zINFO(5, ""); // 448,
}

void zCWayNet::CreateVobDependencies(zCWorld* wld)
{
	if (!wld)
		return;
	zINFO(5,"U: WAYNET: Creating VobDependencies"); // 457,

	for (auto wp : wplist) {
		if (!wp->wpvob) {
			wp->wpvob = new zCVobWaypoint;
			wp->wpvob->name = wp->name;
			wp->wpvob->SetPositionWorld(wp->GetPositionWorld());
			wld->AddVob(wp->wpvob);
			wp->wpvob->SetHeadingAtWorld(wp->dir);
			
		}
	}
	zINFO(5,""); // 477,
}

zSTRING zCWayNet::MergeWaypoints()
{
	zSTRING list;
	// Не стал декодить 1в1, убрал лишние GetName()'ы
	for (auto i = wplist->next; i; i = i->next) {
		auto wp1 = i->get();
		auto name1 = wp1->GetName();
		for (auto j = i->next; j; j = j->next) {
			auto wp2 = j->Get();
			auto name2 = wp2->GetName();

			if (name1 == name2) {
				zWARNING("U: WAYNET: Two Waypoints with same name detected. Merging them : " + name1); // 501
				if (!list.IsEmpty())
					list += ", ";
				list += name1;
			}

			AddWays(wp1, wp2); // for (auto way : wp2->wayList) CreateWay(wp1, way->GetGoalWaypoint(wp2));
			DeleteWaypoint(wp2);
		}
	}

	return list;
}

zSTRING zCWayNet::CheckConsistency(bool fault_on_fail)
{
	zINFO(5, "U: WAYNET: Checking consistency ..."); // 522

	zSTRING list;
	for (auto i = wplist->next; i; i = i->next) {
		auto wp1 = i->get();
		auto name1 = wp1->GetName();
		for (auto j = i->next; j; j = j->next) {
			auto wp2 = j->Get();
			auto name2 = wp2->GetName();

			if (name1 == name2) {
				zWARNING("U: WAYNET: Two Waypoints with same name detected : " + name1); // 543
				if (!list.IsEmpty())
					list += ", ";
				list += name1;
			}
		}
	}

	// was `bool check_failed`, but I'm lazy
	if (!list.IsEmpty()) {
		if (fault_on_fail) {
			zFAULT("U: WAYNET: Waynet consistency check failed. Check zSpy-Messages."); // 555
		} else {
			zINFO(5,"U: WAYNET: Waynet consistency check failed. Check zSpy-Messages."); // 557
		}
	} else {
		zINFO(5,"U: WAYNET: Consistency check ok."); // 562
	}
	zINFO(5,""); // 565,
	return list;
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

int zCWayNet::AStar(zCWaypoint* from, zCWaypoint* to, zCVob const* vob)
{
	++routeCtr;
	from->curCost = 0;

	zVEC3 frompos = from->pos;
	zVEC3 topos   = to->pos;
	zVEC3 diff = topos - frompos;
	unsigned dist = abs(diff.x) + abs(diff.y) + abs(diff.z);

	from->estCost = dist;
	from->score   = dist;
	from->parent = 0;

	openlist.DeleteList();
	InsertInOpen(from);

	while ( 1 ) {
		auto next = openlist.next;
		if ( !next )
			return 0;

		if ( openlist.GetNumInList() <= 0 )
			return 0;

		auto wp = next->Get();
		next->Remove(wp);

		if ( wp == to )
			return 1;

		for (auto way : wp->wayList) {
			if ( way->usedCtr != this->routeCtr ) {
				auto wp2 = way->GetGoalWaypoint(wp);
				if (!vob || wp2->CanBeUsed(vob) && wp->CanBeUsed(vob)) {
					way->usedCtr = this->routeCtr;
					auto cost = way->cost + wp->curCost;
					if (wp2->curCost > cost || !IsInAnyList(wp2)) {
						wp2->parent = way;
						wp2->curCost = cost;

						// I made this func up (SquareDist)
						unsigned dist2 = SquareDist(wp2->pos, to->pos);
						wp2->estCost = dist2;
						wp2->score = cost + dist2;

						if (IsInClosed(wp2))
							RemoveFromClosed(wp2);
						if (!IsInOpen(wp2))
							InsertInOpen(v15);
					}
				}
			}
		}

		InsertInClosed(wp);
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

void zCWayNet::RemoveUnusedWPVobs()
{
	if ( world ) {
		zCList<zCVobWaypoint> wpvobs;
		CreateWPVobList(wpvobs, world->globalVobTree);
		for (auto vob : wpvobs) {
			if (!SearchWaypoint(vob))
				vob->Release();
		}
	}
}
