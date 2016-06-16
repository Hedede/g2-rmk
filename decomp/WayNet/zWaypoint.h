class zCWaypoint : public zCObject {
	Z_OBJECT(zCWaypoint);
public:
	virtual ~zCWaypoint();

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

	zSTRING GetName()
	{
		if (wpvob)
			name = wpvob->GetObjectName();
		return name;
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

private:
	// Kürzeste Weg Suche durch das zCWaynet
	// Diese Eigenschaften besser nicht nutzen,
	// es sind A* (Zwischen-) Ergebnisse.
	int routeCtr;
	int curCost;
	int estCost;
	int score;
	int curList;
	zCWay* parent;

	//Sonstige Daten
	int waterDepth;
	zBOOL underWater;

	zVEC3 pos;         // Position dieses Waypoints
	zVEC3 dir;         // AtVector
	zSTRING name;

	// dazugehöriger Vob in dér Welt
	zCVobWaypoint* wpvob;

	// Liste der hier beginnenden Wege ( Ways )
	zCList<zCWay> wayList;
};

