#pragma once
#include <Gothic/Types/zVEC3.h>


struct zCMovementTracker {
	enum zTWayPoint {
		TYPE_0 = 0,
		TYPE_1 = 1,
		TYPE_2 = 2,
		TYPE_3 = 3,
		TYPE_4 = 4,
	};

	zVEC3& GetLastValidWayPoint(zTWayPoint const& type);

	char unk[0x1B54];
	zVEC3 waypointPosition[12];
	int __wp_pos_idx[4];
	float __wp_0305[4];
};
