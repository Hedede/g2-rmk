#pragma once

#include <Hook/Externals.h>

#include <Gothic/Types/zCObject.h>

struct zCWay {

};

struct zCVobWaypoint;

struct zCWaypoint : zCObject
{
	zVEC3 GetPositionWorld() const
	{
		return pos;
	}

	int routeCtr;
	int curCost;
	int estCost;
	int score;
	int curList;
	zCWay *parent;
	int waterDepth;
	zBOOL underWater;
	zVEC3 pos;
	zVEC3 dir;
	zSTRING name;
	zCVobWaypoint *wpvob;
	zCList<zCWay> wayList;
};

struct zCWayNet {
	zCWaypoint* GetWaypoint(std::string const& name)
	{
		Fastcall<zCWaypoint*(zCWayNet*, zSTRING const&)> call{0x7B0330};
		return call(this,name);
	}
};


#include <Hook/size_checker.h>
CHECK_SIZE(zCWaypoint, 0x7C);
