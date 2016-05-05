class zCWay {
public:
	virtual ~zCWay();

	virtual void Init(zCWaypoint *,zCWaypoint *);

	virtual void CalcProperties(zCWorld *);
	virtual void GetChasm();
	virtual void CanJump();

	virtual void CanBeUsed(zCVob const *);
	virtual void IsObjectOnWay(zCVob const *);

private:
	// Kürzeste Weg Suche durch das zCWaynet
	// Diese Eigenschaften besser nicht nutzen,
	// es sind A* (Zwischen-) Ergebnisse.
	int cost;                   //int
	int usedCtr;                //int

	//Sonstige Eigenschaften
	float chasmDepth;             //zREAL
	zBOOL chasm;                  //zBOOL
	zBOOL jump;                   //zBOOL

	//die beiden anschließenden Wegpunkte
	zCWaypoint* left;
	zCWaypoint* right;
};

class oCWay : public zCWay {
public:
	virtual ~oCWay();

	virtual void Init(zCWaypoint *,zCWaypoint *);

	virtual void CanBeUsed(zCVob const *);
	virtual void IsObjectOnWay(zCVob const *);

	oCMobLadder* GetLadder() const
	{
		return ladder;
	}

private:
	//Unterklasse von zCWay.

	//Objekte im Weg?
	oCMobLadder* ladder;
	oCMobDoor* door;
};
