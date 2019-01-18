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
	// very repetitive, I suspect compiler might've unrolled this
	int idx;
	switch ( type ) {
	case 0:
	case 2:
		idx = __wp_pos_idx[0];
		if ( 0.3 * 0.5 >= __wp_0305[0] ) {
			if ( idx > 1 )
				return waypointPosition[idx-2];
			if ( idx == 1 )
				return waypointPosition[2];
			return waypointPosition[1];
		} else {
			if ( idx > 0 ) {
				return waypointPosition[idx-1];
			return waypointPosition[2];
		}
	case 3:
		idx = __wp_pos_idx[1];
		if ( 0.3 * 0.5 >= __wp_0305[1] ) {
			if ( idx > 1 )
				return waypointPosition[idx+1];
			if ( idx == 1 )
				return waypointPosition[5];
			return waypointPosition[4];
		} else {
			if ( idx > 0 ) {
				return waypointPosition[idx+2];
			return waypointPosition[5];
		}
	case 4:
		idx = __wp_pos_idx[2];
		if ( 0.3 * 0.5 >= __wp_0305[2] ) {
			if ( idx > 1 )
				return waypointPosition[idx+4];
			if ( idx == 1 )
				return waypointPosition[8];
			return waypointPosition[7];
		} else {
			if ( idx > 0 ) {
				return waypointPosition[idx+5];
			return waypointPosition[8];
		}
		break;
	case 1:
		idx = __wp_pos_idx[3];
		if ( 0.3 * 0.5 >= __wp_0305[3] ) {
			if ( idx > 1 )
				return waypointPosition[idx+7];
			if ( v2 == 1 )
				return waypointPosition[11];
			return waypointPosition[10];
		} else {
			if ( idx > 0 ) {
				return waypointPosition[idx+8];
			return waypointPosition[11];
		}
	default:
		return { 0, 0, 0 };
	}
}
