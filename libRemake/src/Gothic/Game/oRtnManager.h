#pragma once

#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Gothic/Types/zBBox.h>

struct oCNpc;
struct zCWorld;
struct oCRtnEntry;

struct oCRtnManager {

	struct TRtn_WayBox {
		zTBBox3D bbox;
	};

	struct TRtn_WayBoxLimit {
		int type;
		zTBBox3D bbox;
		float limit;
		oCNpc *npc;
	};

	void SetWorld(zCWorld* world)
	{
		this->world = world;
	}
	
private:
	int indexStart[3];
	int indexEnd[3];
	zCWorld *world = nullptr;
	zCListSort<oCRtnEntry>* nextEntry = nullptr;
	zCListSort<oCRtnEntry> rtnEntries;
	zCListSort<TRtn_WayBox> wayBoxes[3];
	zCList<oCNpc*> activeNpcList;
	zCArraySort<TRtn_WayBoxLimit*> wayBoxLimits[3];
};

extern oCRtnManager& rtnMan;
