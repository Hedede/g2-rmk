#ifndef GOTHIC_INFO_H
#define GOTHIC_INFO_H

#include <Gothic/Types/zCList.h>
struct zCParser;
struct oCInfo;
struct oCInfoManager {
	oCInfoManager(zCParser& par)
	{
		Thiscall<void(oCInfoManager*, zCParser&)> ctor{0x7023F0};
		ctor(this, par);
	}
	void*_vtbl;
	zCListSort<oCInfo> infoList;
	zCParser *parser;
};

struct oCNpc;

typedef enum zEInformationManagerMode
{
	INFO_MGR_MODE_IMPORTANT,
	INFO_MGR_MODE_INFO,
	INFO_MGR_MODE_CHOICE,
	INFO_MGR_MODE_TRADE
} zTInfoMgrMode;

struct oCInformationManager {
	static oCInformationManager& GetInformationManager()
	{
		Cdecl<oCInformationManager&()> call{0x65F790};
		return call();
	}

	bool WaitingForEnd() const { return IsWaitingForEnd; }
	void Update()
	{
		Thiscall<void(oCInformationManager*)> call{0x660BB0};
		call(this);
	}	

	zSTRING LastMethod;

	void* DlgStatus;
	void* DlgTrade;
	void* DlgChoice;

	oCNpc*  Npc;
	oCNpc*  Player;
	oCInfo* Info;

	zBOOL         IsDone;
	zBOOL         IsWaitingForEnd;
	zBOOL         IsWaitingForScript;
	zBOOL         IsWaitingForOpen;
	zBOOL         IsWaitingForClose;
	zBOOL         IsWaitingForSelection;
	zBOOL         MustOpen;
	int           IndexBye;
	int           ImportantCurrent;
	int           ImportantMax;
	zTInfoMgrMode Mode;
};

#endif//GOTHIC_INFO_H
