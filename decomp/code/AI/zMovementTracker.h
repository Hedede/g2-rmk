struct zCPose {
	zCPose() = default;
	~zCPose() = default;

	zCQuat __inertiaRot[100];
	zVEC3 __inertiaPos[50];
	int __inertiaPosIdx;
	int __inertiaRotIdx;
	zCQuat __rotAbs;
	zVEC3 __pos2;
	zCQuat __rot{0, 0, 0, 1};
	zVEC3 __pos = zVEC3::ZERO;
	int wat = 0;
	int __inertiaSamplesPos = 1;
	int __inertiaSamplesRot = 1;
};

enum zTAICamMsg {
	TARGET_MOVED_FORWARD  = 0x02,
	TARGET_MOVED_BACKWARD = 0x04,
	TARGET_MOVED_LEFT     = 0x08,
	TARGET_MOVED_RIGHT    = 0x10,
	TARGET_MOVED_UP       = 0x20,
	TARGET_MOVED_DOWN     = 0x40,
	TARGET_MOVED_ANY      = 0x7E,
	TARGET_ROTATED_LEFT   = 0x80,
	TARGET_ROTATED_RIGHT  = 0x100,
	//TARGET_MOVED    = 0x800,
	TARGET_STAND    = 0x1000,
	//TARGET_ROTATED  = 0x2000
	TARGET_ROT_NONE = 0x4000,
};

struct zCMovementTracker {


	// looks like it deletes only the aray itself
	// and not pointers to position keys
	~zCMovementTracker() = default;
	static zCMovementTracker* GetTracker()
	{
		zCMovementTracker theTracker;
		return theTracker;
	}

	zCMovementTracker();

	zVEC3& GetLastValidWayPoint(zTWayPoint const& type);

	zREAL GetAzimuth() const { return GetAzimuth(camPos); }
	zREAL GetElevation() const { return GetElevation(camPos); }
	zREAL GetRange() const { return GetRange(camPos); }

	zREAL GetAzimuth(zVEC3 const& testPos) const;
	zREAL GetElevation(zVEC3 const& testPos) const;
	zREAL GetRange(zVEC3 const& testPos) const;

	zVC3 ApplyAziToPoint(float const& angleDeg,  zVEC3 const& p);
	zVC3 ApplyElevToPoint(float const& angleDeg, zVEC3 const& p);
	zVC3 ApplyRangeToPoint(float const& dist, zVEC3 const& p);

	zVEC3 GetRangePos(float const& dist) const;
	zVEC3 GetIdealRangePos() const;

	void SetAzimuth(float const& angleDeg);
	void SetElevation(float const& angleDeg);
	void SetRange(float const& dist);

	void SetCamPos(zVEC3 const& newPos);

private:
	void CheckKeys() {}

	zREAL GetDistToIdealRange() const;
	bool IsIdealOrientation();
	bool IsIdealRange() const;

	void SetSampledPlayerPos(const zVEC3& newTarget)
	{
		// it is like this in the excutable
		__sampledPos = newTarget;
		__sampledPos = newTarget;
	}

private:
	zTAICamMsg __msg;
	zCArray<zCPositionKey*> __positionKeys;
	float __timeSec = 0.0;
	zVEC3 __sampledPos = zVEC3::ZERO;
	zVEC3 __unkPos     = zVEC3::ZERO;
	zVEC3 playerPos  = zVEC3::ZERO;
	zVEC3 playerPos2 = zVEC3::ZERO;
	zVEC3 __startPos = zVEC3::ZERO;
	zVEC3 camPos  = zVEC3::ZERO;
	zVEC3 camPos2 = zVEC3::ZERO;
	zVEC3 __vec_a1 = zVEC3::ZERO;
	zVEC3 __vec_a2 = zVEC3::ZERO;
	int unk1[3];

	zVEC3 __vobPos = zVEC3::ZERO;

	float __someVal0 = 0.5;
	int __targetInertia;
	int unkx1 = 2;
	int unkx2 = 2;
	zCPose __pose1;
	zCPose __pose2;
	zCPose playerPose;

	zVEC3 waypointPosition[12];
	int __wp_pos_idx[4] = { 0, 0, 0, 0 };
	float __wp_0305[4]  = { 0.0, 0.0, 0.0, 0.0 };


	zVEC3 __vec1 = zVEC3::ZERO;
	zVEC3 __vec2 = zVEC3::ZERO;
	zVEC3 __vec3 = zVEC3::ZERO;
	zVEC3 __vec4 = zVEC3::ZERO;
	zVEC3 __vec5 = zVEC3::ZERO; // possibly player movement vector
	zVEC4 __vec6 = zVEC3::ZERO;
	zVEC3 __vec7 = zVEC3::ZERO;
	zVEC3 __vec8 = zVEC3::ZERO;
	zVEC3 __vec9 = zVEC3::ZERO;
	zVEC3 __vec10 = zVEC3::ZERO;
	int unk3 = 0;

	float __ringWhat = 0.1;
	float __ringRot[4] = { 0.2, 0.1, 0.04 };

	float __azi   = 0.0;
	float __elev  = 0.0;
	float __range = 0.0;

	float __targetVelo = 0.0;

	float __savedAzi   = 0.0;
	float __savedElev  = 0.0;

	zMAT4 __mat0inverse;
	zMAT4 __mat0;

	zMAT4 __mat1;
	int unko4[16];
	zMAT4 __mat2 = zMAT4::s_identity;
	zMAT4 __mat3 = zMAT4::s_identity;
	zMAT4 __mat4 = zMAT4::s_identity;
	zMAT4 __mat5 = zMAT4::s_identity;
	zMAT4 __mat6;
	zMAT4 __mat7;
	zMAT4 __mat8 = zMAT4::s_identity;

	float unk5 = 100000.0;
	int unk5_1 = 0;
	int unk5_2 = 0;
	int rotateEnabled;
	int unk6 = 0;
	float __someVal1 = 0.0;
	float __someVal2 = 0.0;

	oCNpc *player   = nullptr;
	zCVob *__camVob = nullptr;
	zCAICamera *aiCam;
	zCPathSearch *pathSearch = zCPathSearch::GetSearch();
};

//------------------------------------------------------------------------------
zCMovementTracker::zCMovementTracker()
{
	ClearMessages();

	zMAT4 mat;
	mat[0][0] = 0.0;
	mat[0][1] = 0.0;
	mat[0][2] = 0.0;
	mat[0][3] = 1.0;
	mat[1][0] = 0.0;
	mat[1][1] = 0.0;
	mat[1][2] = 0.0;
	mat[1][3] = 1.0;
	mat[2][0] = 0.0;
	mat[2][1] = 0.0;
	mat[2][2] = 0.0;
	mat[2][3] = 1.0;
	mat[3][0] = 0.0;
	mat[3][1] = 0.0;
	mat[3][2] = 0.0;
	mat[3][3] = 1.0;

	__mat6 = mat;
	__mat7 = mat;

	zVEC3 fillPos = zVEC3::ZERO;
	ResetWayPoints( fillPos, 0 );
	ResetWayPoints( fillPos, 1 );
}

zVEC3 zCMovementTracker::GetRangePos(float const& dist, zVEC3 const& point) const
{
	auto dir = (point - playerPos); // was inlined
	dir.NormalizeSafe(); // was inlined
	return playerPos + dir * dist; // ws inlined
}

zREAL zCMovementTracker::GetRange(zVEC3 const& testPos)
{
	return (playerPos - testPos).Length(); // was inlined
}

zVEC3 zCMovementTracker::GetRangePos(float const& dist) const
{
	return ApplyRangeToPoint( dist, camPos );
}

zVEC3 zCMovementTracker::GetIdealRangePos() const
{
	auto dist = aicam->GetBestRange() * 100.0;
	return GetRangePos( dist ); // was inlined
}

zREAL zCMovementTracker::GetDistToIdealRange() const
{
	return GetIdealRangePos().Length(); // was inlined
}

bool zCMovementTracker::IsIdealRange() const
{
	return GetDistToIdealRange() < 0.1; // was inlined
}

void zCMovementTracker::SetRange(float const& dist)
{
	camPos = ApplyRangeToPoint( dist, camPos ); // was inlined
}

void zCMovementTracker::SetAzimuth(float const& angleDeg)
{
	camPos = ApplyAziToPoint( angleDeg, camPos );
}

void zCMovementTracker::SetElevation(float const& angleDeg)
{
	camPos = ApplyElevToPoint( angleDeg, camPos );
}

void zCMovementTracker::SetCamPos(zVEC3 const& newPos)
{
	camPos = newPos;
	if ( __msg & 0x8000 ) // sbyte1 < 0
		ResetWayPoints(newPos, 1);
}

void zCMovementTracker::ClearMessages()
{
	__msg = TARGET_ROT_NONE|TARGET_STAND;
}

void zCMovementTracker::UpdateMessages()
{
	zTAICamMsg v1; // eax

	if ( __msg & TARGET_MOVED_ANY )
		__msg = __msg & ~TARGET_STAND | 0x800;
	if ( (__msg & TARGET_ROTATED_LEFT) || (__msg & TARGET_ROTATED_RIGHT) )
		__msg = __msg & ~TARGET_ROT_NONE | 0x2000;
}

zVEC3& zCMovementTracker::GetLastValidWayPoint(zTWayPoint const& type)
{
	int index;
	switch ( type ) {
	case 0:
	case 2:
		index = 0;
		break;
	case 3:
		index = 1;
		break;
	case 4:
		index = 2;
		break;
	case 1:
		index = 3;
		break;
	default:
		return { 0, 0, 0 };
	}

	int i = __wp_pos_idx[0];
	if ( 0.3 * 0.5 >= __wp_0305[0] ) {
		if ( i > 1 )
			return waypointPosition[i-2 + 3*index];
		if ( i > 0 )
			return waypointPosition[2 + 3*index];
		return waypointPosition[1 + 3*index];
	}

	if ( i > 0 )
		return waypointPosition[i-1 + 3*index];
	return waypointPosition[2 + 3*index];
}

bool zCMovementTracker::IsIdealOrientation()
{
	return fabs(GetAzimuth(camPos) - bestAzimuth) < 1.5 &&
	       fabs(GetElevation(camPos) - bestElevation) < 1.5;
}
