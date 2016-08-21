#ifndef G2_CGAMEMANAGER_H
#define G2_CGAMEMANAGER_H
#include <Hook/func.h>
#include <Gothic/Types/zSTRING.h>

struct oCGame;
struct oCSavegameManager;
struct zCView;
struct oCBinkPlayer;
struct zCMenu;
struct oCMenuSavegame;

struct CGameManager;
CGameManager*& gameMan = *(CGameManager**)0x8C2958;

struct CGameManager {
	CGameManager()
	{
		Thiscall<void(CGameManager*)> ctor{0x4244E0};
		ctor(this);
	}

	~CGameManager()
	{
		Thiscall<void(CGameManager*)> dtor{0x424730};
		dtor(this);
	}

	void Init(void* hwnd)
	{
		Thiscall<void(CGameManager*,void*)> func{0x424C70};
		func(this, hwnd);
	}

	void Run()
	{
		Thiscall<void(CGameManager*)> func{0x425830};
		func(this);
	}

	void Done()
	{
		Thiscall<void(CGameManager*)> func{0x4254E0};
		func(this);
	}

	void* _vbtl;
	int oldAlphaBlendFunc;
	void* sysContextHandle;
	oCGame *gameSession = 0;
	oCGame *backLoop  = 0;
	zBOOL exitGame    = 0;
	zBOOL exitSession = 0;
	zBOOL gameIdle = 1;
	int lastWorldWasGame = 0;
	oCSavegameManager *savegameManager = 0;
	zSTRING *lastDatFileList_array = 0;
	int lastDatFileList_numAlloc   = 0;
	int lastDatFileList_numInArray = 0;
	zSTRING *lastWorldList_array   = 0;
	int lastWorldList_numAlloc     = 0;
	int lastWorldList_numInArray   = 0;
	zSTRING backWorldRunning;
	zSTRING backDatFileRunning;
	zCView *vidScreen;
	zCView *initScreen;
	int introActive;
	int dontStartGame;
	oCBinkPlayer *videoPlayer;
	int videoPlayInGame;
	zCMenu *menu_chgkeys;
	zCMenu *menu_chgkeys_ext;
	oCMenuSavegame *menu_save_savegame;
	oCMenuSavegame *menu_load_savegame;
	int playTime;
};
#endif
