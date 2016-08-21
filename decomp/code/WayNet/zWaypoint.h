class zCWaypoint : public zCObject {
	Z_OBJECT(zCWaypoint);
public:
	zCWaypoint() = default;
	virtual ~zCWaypoint() = default;

	void Archive(zCArchiver& arc) override
	{
		arc.WriteString("wpName", name);
		arc.WriteInt("waterDepth", waterDepth);
		arc.WriteBool("underWater", underWater);
		arc.WriteVec3("position", pos);
		arc.WriteVec3("direction", dir);
	}

	void Unarchive(zCArchiver& arc) override
	{
		arc.ReadString("wpName", name);
		arc.ReadInt("waterDepth", waterDepth);
		arc.ReadBool("underWater", underWater);
		arc.ReadVec3("position", pos);
		arc.ReadVec3("direction", dir);
	}

	virtual bool CanBeUsed(zCVob const *)
	{
		return true;
	}

	int GetNumberOfWays() const
	{
		return wayList.GetNumInList();
	}

	void SetName(zSTRING& name)
	{
		this->name = name;
		if (wpvob)
			wpvob->SetVobName(name);
	}

	zSTRING GetName()
	{
		if (wpvob)
			name = wpvob->GetObjectName();
		return name;
	}

	void Init(float x, float y, float z)
	{
		Init(zVEC3{x,y,z});
	}

	void Init(zVEC3& pos)
	{
		this->wpvob = nullptr
		this->pos = pos;
		this->dir = zVEC3{1.0, 0, 0};
	}

	void Init(zCVobWaypoint* wpwob)
	{
		this->wpvob = wpvob;

		name = wpvob->GetObjectName();
		name.Upper();

		pos = wpvop->trafoObjToWorld.GetTranslation();
		dir = wpvob->trafoObjToWorld.GetAtVector();

		AddRef(wpvob);
	}

	void Draw() {}

	zVEC3 const& GetPositionWorld()
	{
		return pos;
	}

	zCList<zCWay>& GetWayList()
	{
		return wayList;
	}

	zCVobWaypoint* GetVob()
	{
		return wpvob;
	}

	void SetWaypointVob(zCVobWaypoint* vob)
	{
		Release(wpvob);
		wpvob = vob;
		AddRef(wpvob)
	}

	void InsertWay(zCWay* way)
	{
		wayList.Insert(way);
	}

	void RemoveWay(zCWay* way)
	{
		wayList.Remove(way);
	}

	bool HasWay(zCWaypoint* other)
	{
		for (auto way : wayList) {
			if (way.GetGoalWaypoint(this) == other)
				return true;
		}
		return false;
	}

	void Free()
	{
		for (auto way : wayList)
			if (way)
				delete way;

		parent = nullptr;
		if (wpvob && wpvob->homeWorld)
			wpvob->homeWorld->RemoveVob(wpvob);
		Release(wpvob);
	}

	void UpdatePositionWorld()
	{
		if (wpvob) {
			pos = wpvob->GetPositionWorld();
			dir = wpvob->GetHeadingAtWorld();
		}
	}

	void CorrectHeight(zCWorld* wld)
	{
		zVEC3 ray{0, -1000.0, 0};
		if ( wld->TraceRayNearestHit(pos, ray, wpvob, 0x821) && (wld->foundPoly || wld->foundVob) ) {
			pos = wld->foundIntersection;
			pos.y += 50.0;

			if ( wpvob )
				wpvob->SetPositionWorld(pos);
		}
	}

	void CalcProperties(zCWorld* wld);

private:
	// Kürzeste Weg Suche durch das zCWaynet
	// Diese Eigenschaften besser nicht nutzen,
	// es sind A* (Zwischen-) Ergebnisse.
	int routeCtr = 0;
	int curCost  = 0;
	int estCost  = 0;
	int score    = 0;
	int curList  = 0;
	zCWay* parent = nullptr;

	//Sonstige Daten
	int waterDepth = 0;
	zBOOL underWater = false;

	// Position dieses Waypoints
	zVEC3 pos{0,0,0};
	// AtVector
	zVEC3 dir{1.0,0,0};
	zSTRING name;

	// dazugehöriger Vob in dér Welt
	zCVobWaypoint* wpvob = nullptr;

	// Liste der hier beginnenden Wege ( Ways )
	zCList<zCWay> wayList;
};

void zCWaypoint::CalcProperties(zCWorld *wld)
{
	waterDepth = 0;
	float dist = 0.0;

	zVEC3 start = pos;
	zVEC3 ray{0, -1000.0, 0};
	if ( wld->TraceRayNearestHit(start, ray, wpvob, 0xA21) ) {
		auto foundPoly = wld->foundPoly;
		if (foundPoly && foundPoly->material) {
			dist = pos.y - wld->foundIntersection.y;
			if ( foundPoly->material->matGroup == MAT_WATER ) {
				start = wld->foundIntersection;
				if ( wld->TraceRayNearestHit(start, ray, wpvob, 0x821) ) {
					if ( wld->foundPoly ) {
						waterDepth += start.y - wld->foundIntersection.y;
					} else {
						zWARNING("U: WAY: Incorrect Waterdepth at WP " + GetName()); // 948, _ulf\\zWaynet.cpp
					}
				}
			}
		}
	}

	ray = zVEC3{0, 1000, 0};
	if ( wld->TraceRayNearestHit(start, ray, wpvob, 0xA21) ) {
		if ( wld->foundPoly && wld->foundPoly->material ) {
			if ( wld->foundPoly->material->matGroup == MAT_WATER ) {
				waterDepth = wld->foundIntersection.y - pos.y + dist;
				underWater = 1;
			}
		}
	}
}
