struct zCWay {
	static zVEC3 GetFloor(zCWorld* wld, zVEC3 const& pos)
	{
		zVEC3 ray{0, -1000.0, 0};
		if ( wld->TraceRayNearestHit(pos, ray, 0, 32) && wld->foundPoly )
			return wld->foundIntersection;

		return {pos.x, pos.y - 1000.0, ray.z};
	}

	zCWay(zCWaypoint* left, zCWaypoint* right)
	{
		Init(left, right);
	}

	virtual ~zCWay() = default;

	virtual void Init(zCWaypoint* left, zCWaypoint* right);
	{
		chasm = 0;
		chasmDepth = 0;
		jump = 0;
		usedCtr = 0;

		this->left = left;
		this->right = right;

		left->wayList.Insert(this);
		right->wayList.Insert(this);

		EstimateCost();
	}

	virtual void CalcProperties(zCWorld *);
	virtual float GetChasm()
	{
		return -chasmDepth;
	}
	virtual bool CanJump()
	{
		return jump;
	}

	virtual bool CanBeUsed(zCVob const *)
	{
		return true;
	}
	virtual bool IsObjectOnWay(zCVob const *)
	{
		return false;
	}

	int GetCost()
	{
		return cost;
	}

	float GetLength()
	{
		return (right->pos - left->pos).LengthApprox();
	}

	float GetJumpDistance()
	{
		zVEC3 diff = right->pos - left->pos;
		diff.y = 0.0;
		return diff.LengthApprox();
	}

	zCWaypoint* GetGoalWaypoint(zCWaypoint* wp)
	{
		return wp == left ? right : left
	}

	zCWaypoint* GetGoalWaypoint(zCWaypoint* wp)
	{
		return wp == left ? right : left
	}

	void EstimateCost()
	{
		cost = zCWayNet::EstimateCost(left, right);
	}

	void Free()
	{
		left->wayList.Remove(this);
		right->wayList.Remove(this);
	}

private:
	// Kürzeste Weg Suche durch das zCWaynet
	// Diese Eigenschaften besser nicht nutzen,
	// es sind A* (Zwischen-) Ergebnisse.
	int cost = 0;
	int usedCtr = 0;

	//Sonstige Eigenschaften
	zREAL chasmDepth = 0.0;
	zBOOL chasm = false;
	zBOOL jump  = false;

	//die beiden anschließenden Wegpunkte
	zCWaypoint* left  = nullptr;
	zCWaypoint* right = nullptr;
};

struct oCWay : public zCWay {
	oCWay() = default;
	virtual ~oCWay()
	{
		Release(ladder);
		Release(door);
	}

	virtual void Init(zCWaypoint *,zCWaypoint *);

	virtual bool CanBeUsed(zCVob const* vob)
	{
		if (auto npc = dynamic_cast<oCNpc const*>(vob)) {
			if ( ladder )
				return !npc->IsMonster();

			if ( CanJump() ) {
				if ( GetJumpDistance() > npc->GetJumpRange() ) {
					if ( GetChasm() > npc->GetFallDownHeight() )
						return 0;
					if ( GetChasm() > oCNpc::GetClimbRange())
						return 0;
				}
			} else {
				if ( GetChasm() > 0.0 ) {
					if ( GetChasm() > npc->GetFallDownHeight())
						return 0;
				}
			}
		}
		return 1;
	}

	virtual bool IsObjectOnWay(zCVob const* vob)
	{
		return ladder == vob || door == vob;
	}

	oCMobLadder* GetLadder() const
	{
		return ladder;
	}

private:
	//Unterklasse von zCWay.

	//Objekte im Weg?
	oCMobLadder* ladder = nullptr;
	oCMobDoor* door = nullptr;
};
