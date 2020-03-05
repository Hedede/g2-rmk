#ifndef Gothic_oSaveGame_H
#define Gothic_oSaveGame_H
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>

struct oCSavegameManager_vt;

struct oCSavegameInfo;
struct oCSavegameManager {
	oCSavegameManager_vt *vtab;

	oCSavegameManager()
	{
		Thiscall<void(oCSavegameManager*)> ctor{0x4386F0};
		ctor(this);
	}

	void Init()
	{
		Thiscall<void(oCSavegameManager*)> call{0x0438990};
		call(this);
	}

	void Reinit()
	{
		initialized = false;
		Init();
	}

	void ClearCurrent()
	{
		Thiscall<void(oCSavegameManager*)> call{0x439670};
		call(this);
	}

	zCArray<oCSavegameInfo*> saves;
	oCSavegameInfo*          current;
	bool initialized;
	int unk[6];
};

const int SAVEGAME_SLOT_NONE    = -3;
const int SAVEGAME_SLOT_NEW     = -2;
const int SAVEGAME_SLOT_CURRENT = -1;
const int SAVEGAME_SLOT_QUICK   = 0;
const int SAVEGAME_SLOT_MIN     = 1;
const int SAVEGAME_SLOT_MAX     = 20;

#endif//Gothic_oSaveGame_H
