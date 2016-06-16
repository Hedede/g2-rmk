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

	void Draw(zCCamera* camera);

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

	
	zVEC3 GetDistancePos(float xcm, zCWaypoint* wp)
	{
		auto goal = GetGoalWaypoint(wp);
		zVEC3 vec  = wp->pos - goal->pos;
		zVEC3 vec2 = left->pos - right->pos;

		float len = 1.0 / vec2.LengthApprox();

		return wp->pos + vec * len * xcm;
	}

	void CalcProperties(zCWorld* wld);

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

void zCWay::CalcProperties(zCWorld* wld)
{
	zVEC3 pos1 = left->pos;
	zVEC3 pos2 = right->pos;
	if ( pos2.y > pos1.y )
		Alg_Swap(pos1, pos2);

	if (pos1 != pos2) {
		float len = (pos2 - pos1).Length() * 0.05;
		float invlen = 1.0 / (pos2 - pos1).Length();
		zVEC3 unit = (pos2 - pos1) * invlen * 20.0;
		// ^ так было в acме

		pos2 = GetFloor(wld, pos1);

		if ( len > 0 ) {
			float chasm1 = 0.0;
			float chasm2 = 0.0;
			float prevY = pos2.y;
			do {
				pos1 += unit;
				zVEC3 hit = GetFloor(wld, pos1);

				if ( i >= len / 2 )
					chasm1 += hit.y - prevY;
				else
					chasm2 += hit.y - prevY;
				prevY = hit.y;
			} while ( ++i < len );

			if ( chasm1 < -200.0 ) {
				chasmDepth = chasm1;
				if ( chasm1 * -0.5 < chasm2 )
					jump = 1;
			}
		}
	}
}

void zCWay::Draw(zCCamera* camera)
{
	zVEC3 left_cam  = camera->camMatrix * left->pos;
	zVEC3 right_cam = camera->camMatrix * right->pos;
	Alg_ClipAtZ0(left_cam, right_cam);

	if (left_cam.z < 0.0)
		return;
	float invz = 1.0 / left_cam.z;

	zVEC3 a1;
	a1.x = left_cam.x * camera->viewDistanceX * invz + camera->xcenter;
	a1.y = camera->ycenter - left_cam.y * camera->viewDistanceY * invz;

	if (right_cam.z < 0.0)
		return;

	invz = 1.0 / right_cam.z;

	zVEC3 a2;
	a2.x = right_cam.x * camera->viewDistanceX * invz + camera->xcenter;
	a2.y = camera->ycenter - right_cam.y * camera->viewDistanceY * invz;

	float lx = a1.x;
	float ly = a1.y; //  a1.y + a1.y - a1.y;
	float rx = a2.x;
	float ry = a2.y;

	if (screen->ClipLine(lx, ly, rx, ry)) {
		float rz = 1.0 / right_cam.z;
		float lz = 1.0 / right_cam.z;

		zrenderer->DrawLineZ(lx, ly, lz, rx, ry, rz, GFX_RED);

		zSTRING atr;
		if (chasmDepth != 0.0)
			atr = "C";
		if (jump)
			atr = "J";
		if (!atr.IsEmpty())
			atr = " [" + atr + "]";
		if (left_cam.z < 1000.0) {
			if (left->wpvob)
				left->wpvob->name = left->GetObjectName();

			zSTRING text = left->wpvob->name + atr;

			screen->Print(screen->anx(a1.x), screen->anx(a1.y), text);
		}
		if (right_cam.z < 1000.0) {
			if (right->wpvob)
				right->wpvob->name = right->GetObjectName();

			zSTRING text = right->wpvob->name + atr;

			screen->Print(screen->anx(a2.x), screen->anx(a2.y), text);
		}
	}
}

void oCWay::Init(zCWaypoint* lwp, zCWaypoint* rwp)
{
	zCWay::Init(lwp, rwp);

	zTBBox3D bbox;
	bbox.Init();
	bbox.AddPoint(left->GetPositionWorld());
	bbox.AddPoint(right->GetPositionWorld());

	zCArray<zCVob*> vobList;

	ogame->GetWorld->bspTree.bspRoot.CollectVobsInBBox3D(vobList, bbox);
	for (zCVob* vob : vobList) {
		if (auto ladder = dynamic_cast<oCMobLadder*>(vob)) {
			zVEC3 lpos = left->GetPositionWorld();
			zVEC3 rpos = right->GetPositionWorld();
			if ( lpos.y > rpos.y )
				Alg_Swap(lpos, rpos);

			zTTraceRayReport rep;
			zVEC3 ray = rpos - lpos;
			if ( ladder->TraceRay(lpos, ray, 4, rep) ) {
				float hip = (ladder->bbox3D.mins[1] + ladder->bbox3D.maxs[1]) * 0.5;
				if ( lpos.y < hip && rpos.y > hip) {
					if (this->ladder)
						zWARNING("U:WAY:Found multiple ladders on path " + left->GetName() + "/" + right->GetName()); // 226, _ulf\\oWaynet.cpp

					Release(this->ladder);
					this->ladder = ladder;
					this->ladder->AddRef();
				} else {
					zWARNING("U:WAY: Ladder highpoint error (" + left->GetName() + "/" + right->GetName()); // 236, _ulf\\oWaynet.cpp
				}
			}
		} else if (auto door = dynamic_cast<oCMobDoor*>(vob)) {
			auto dpos = door->GetPositionWorld();
			auto lpos = left->GetPositionWorld()
			auto rpos = right->GetPositionWorld()

			auto diff1 = dpos - lpos;
			auto diff2 = lpos - rpos;

			diff1.Normalize();
			diff2.Normalize();

			if ( abs(acos(diff1 * diff2) * RadToDeg) < 10 ) {
				Release(this->door);
				this->door = door;
				this->door->AddRef();
			}
		}
	}
}
