#pragma once

#include <Hook/Externals.h>

#include <Gothic/Types/zCObject.h>

struct zCCamera;

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
		const zSTRING tmpstr = name;
		return fastcall<zCWaypoint*>(0x7B0330,this,&tmpstr);
	}

	void CorrectHeight()
	{
		thiscall(0x7AD7B0, this);
	}

	void Draw(zCCamera* camera)
	{
		thiscall(0x7AD870, this, camera);
	}
};


#include <Hook/size_checker.h>
CHECK_SIZE(zCWaypoint, 0x7C);
