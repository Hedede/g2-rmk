#ifndef G2_CGAMEMANAGER_H
#define G2_CGAMEMANAGER_H
#include <Hook/func.h>
#include <Hook/value.h>
#include <Hook/log.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Input/zCInputCallback.h>
#include <Gothic/Video/oBinkPlayer.h>

struct oCGame;
struct oCSavegameManager;
struct zCView;
struct oCBinkPlayer;
struct zCMenu;
struct oCMenuSavegame;

struct CGameManager;
auto& gameMan = Value<CGameManager*>(0x8C2958);

struct CGameManager : zCInputCallback {
	CGameManager()
		: zCInputCallback()
	{
		_vtab = reinterpret_cast<void*>(0x82F0EC);
		gameMan = this;
	}

	~CGameManager()
	{
		Thiscall<void(CGameManager*)> dtor{0x424730};
		dtor(this);
	}

	void Init(void* hwnd)
	{
		println("CGameManager::Init(): hwnd: ", uintptr_t(hwnd));

		Thiscall<void(CGameManager*,void**)> func{0x424C70};
		func(this, &hwnd);
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

	int    oldAlphaBlendFunc;
	void*  sysContextHandle;

	oCGame*            gameSession = nullptr;
	oCGame*            backLoop    = nullptr;
	zBOOL              exitGame    = false;
	zBOOL              exitSession = false;
	zBOOL              gameIdle    = true;
	zBOOL              lastWorldWasGame = false;
	oCSavegameManager* savegameManager = nullptr;

	zCArray<zSTRING>	lastDatFileList;
	zCArray<zSTRING>	lastWorldList;

	zSTRING backWorldRunning;
	zSTRING backDatFileRunning;

	zCView*        vidScreen  = nullptr;
	zCView*        initScreen = nullptr;

	zBOOL          introActive   = false;
	zBOOL          dontStartGame = false;

	oCBinkPlayer*  videoPlayer = new oCBinkPlayer{};
	zBOOL          videoPlayInGame = false;

	zCMenu*         menu_chgkeys       = nullptr;
	zCMenu*         menu_chgkeys_ext   = nullptr;
	oCMenuSavegame* menu_save_savegame = nullptr;
	oCMenuSavegame* menu_load_savegame = nullptr;

	//wird selten (?) aktualisiert. Mindestens aber beim Speichern und Laden.
	int playTime = 0;
};
#endif
