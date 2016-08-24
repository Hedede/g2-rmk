#ifndef Gothic_oSaveGame_H
#define Gothic_oSaveGame_H
#include <Hook/func.h>
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

	zCArray<oCSavegameInfo*> saves;
	oCSavegameInfo*          current;
	bool initialized;
	int unk[6];
};


#endif//Gothic_oSaveGame_H
