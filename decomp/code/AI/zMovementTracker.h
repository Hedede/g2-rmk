struct zCPose {
	~zCPose() = default;

	zCQuat __inertiaRot[100];
	zVEC3 __inertiaPos[50];
	int __inertiaPosIdx;
	int __inertiaRotIdx;
	zCQuat __rotAbs;
	zVEC3 __pos2;
	zCQuat __rot;
	zVEC3 __pos;
	int wat;
	int __inertiaSamplesPos;
	int __inertiaSamplesRot;
};

struct zCMovementTracker {

	zVEC3& GetLastValidWayPoint(zTWayPoint const& type);

private:
	void CheckKeys() {}

	bool IsIdealOrientation();

private:
	zTAICamMsg __msg;
	zCArray<zCPositionKey*> __positionKeys;
	float __timeSec;
	zVEC3 __sampledPos;
	zVEC3 __unkPos;
	zVEC3 playerPos;
	zVEC3 playerPos2;
	zVEC3 __startPos;
	zVEC3 camPos;
	zVEC3 camPos2;

	int unk1[9];

	zVEC3 __vobPos;

	float __someVal0;
	int __targetInertia;
	int unkx1;
	int unkx2;
	zCPose __pose1;
	zCPose __pose2;
	zCPose playerPose;

	zVEC3 waypointPosition[12];
	int __wp_pos_idx[4];
	float __wp_0305[4];


	zVEC3 __vec1;
	zVEC3 __vec2;
	zVEC3 __vec3;
	zVEC3 __vec4;
	zVEC3 __vec5;
	zVEC4 __vec6;
	zVEC3 __vec7;
	int unk3[3];
	zVEC3 __vec9;
	int unk33[4];

	float __ringWhat;
	float __ringRot[4];

	float __azi;
	float __elev;
	float __range;

	int unk3_[3];

	zMAT4 __mat0inverse;
	zMAT4 __mat0;

	zMAT4 __mat1;
	int unko4[16];
	zMAT4 __mat2;
	zMAT4 __mat3;
	zMAT4 __mat4;
	zMAT4 __mat5;
	zMAT4 __mat6;
	zMAT4 __mat7;
	zMAT4 __mat8;

	int unk4[3];
	int rotateEnabled;
	int unk5;
	float __someVal1;
	float __someVal2;

	oCNpc *player;
	zCVob *__vob;
	zCAICamera *aiCam;
	zCPathSearch *pathSearch;
};


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
