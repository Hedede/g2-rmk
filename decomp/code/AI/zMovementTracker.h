struct zCPose {
	~zCPose() = default;
};

struct zCMovementTracker {

	zVEC3& GetLastValidWayPoint(zTWayPoint const& type);

private:
	void CheckKeys() {}

private:
	zTAICamMsg __msg;
	float unk0[4];
	zVEC3 __sampledPos;
	float unk00[3];
	zVEC3 playerPos;
	zVEC3 playerPos2;
	zVEC3 __startPos;
	zVEC3 __testPos;
	zVEC3 __camPos2;
	int unk2[13];
	int __targetInertia;
	int unkx1;
	int unkx2;
	zCPose __pose1;
	zCPose __pose2;
	zCPose __pose3;
	zVEC3 waypointPosition[12];
	int __wp_pos_idx[4];
	float __wp_0305[4];
	int unk3[32];
	float __ringRot[4];
	int unk3_[22];
	zMAT4 __mat;
	int unko4[32];
	zMAT4 __mat2;
	zMAT4 __mat3;
	zMAT4 __mat4;
	zMAT4 __mat5;
	zMAT4 __mat6;
	zMAT4 __mat7;
	zMAT4 __mat8;
	int unk4[3];
	int rotateEnabled;
	int unk5[3];
	oCNpc *player;
	zCPathSearch *pathSearch;
	zCAICamera *aiCam;
	zCPathSearch *unkoooo;
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
