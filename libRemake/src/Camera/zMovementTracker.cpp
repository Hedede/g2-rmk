#include <Gothic/Camera/zMovementTracker.h>
#include <Logging/Log.h>
extern bool yabzur;
zVEC3& zCMovementTracker::GetLastValidWayPoint(zTWayPoint const& type)
{
	int index;
	switch ( type ) {
	case TYPE_0:
	case TYPE_2:
		index = 0;
		break;
	case TYPE_3:
		index = 1;
		break;
	case TYPE_4:
		index = 2;
		break;
	case TYPE_1:
		index = 3;
		break;
	default:
		{
			 static zVEC3 zeroVec{ 0, 0, 0 };
			 return zeroVec;
		}
	}

	int i = __wp_pos_idx[index];
	if (yabzur) g2::Log( "ZMOVTRAC", "Type: ", (int)type );
	if (yabzur) g2::Log( "ZMOVTRAC", "ID: ", index );
	if (yabzur) g2::Log( "ZMOVTRAC", "Index: ", i );
	if ( 0.3 * 0.5 >= __wp_0305[index] ) {
		if (yabzur) g2::Log( "ZMOVTRAC", "Bolshe: da" );
		if ( i > 1 )
			return waypointPosition[i-2 + 3*index];
		if ( i > 0 )
			return waypointPosition[2 + 3*index];
		return waypointPosition[1 + 3*index];
	}

	if (yabzur) g2::Log( "ZMOVTRAC", "Bolshe: ned" );

	if ( i > 0 )
		return waypointPosition[i-1 + 3*index];
	return waypointPosition[2 + 3*index];
}
