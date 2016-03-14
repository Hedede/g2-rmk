class zCWaypoint : public zCObject {
	Z_OBJECT(zCWaypoint);
public:
	virtual ~zCWaypoint();

	virtual void Archive(zCArchiver& archiver)
	{
		archiver.WriteString("wpName", name);
		archiver.WriteInt("waterDepth", waterDepth);
		archiver.WriteBool("underWater", underWater);
		archiver.WriteVec3("position", pos);
		archiver.WriteVec3("direction", dir);
	}

	virtual void Unarchive(zCArchiver& archiver)
	{
		archiver.ReadString("wpName", name);
		archiver.ReadInt("waterDepth", waterDepth);
		archiver.ReadBool("underWater", underWater);
		archiver.ReadVec3("position", pos);
		archiver.ReadVec3("direction", dir);
	}

	virtual void CanBeUsed(zCVob const *);

	int zCWaypoint::GetNumberOfWays() const
	{
		return wayList.GetNumInList();
	}

	zSTRING GetName()
	{
		if (wpvob)
			name = wpvob->GetObjectName();
		return name;
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
	int waterDepth;                     //int          
	int underWater;                     //zBOOL 

	zVEC3 pos;         // Position dieses Waypoints
	zVEC3 dir;         // AtVector
	zSTRING name;

	// dazugehöriger Vob in dér Welt
	zCVobWaypoint* wpvob;

	// Liste der hier beginnenden Wege ( Ways )
	zCList <zCWay>    wayList;
};

