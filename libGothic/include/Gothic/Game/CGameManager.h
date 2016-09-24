#ifndef G2_CGAMEMANAGER_H
#define G2_CGAMEMANAGER_H
#include <Hook/Externals.h>
#include <Logging/Log.h>
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
static auto& gameMan = Value<CGameManager*>(0x8C2958);

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

	void Init(void* hwnd);

	void GameInit();
	void PreGraphicsInit();

	void InitSettings()
	{
		Thiscall<void(CGameManager*)> call{0x426430};
		call(this);
	}

	void PreRun();
	void Run();

	void RenderFrame();

	void Tool_ConvertData()
	{
		Thiscall<void(CGameManager*)> func{0x4248F0};
		func(this);

	}

	void GameSessionInit()
	{
		Thiscall<void(CGameManager*)> func{0x426E10};
		func(this);
	}

	void Done();

	void InitScreen_Open()
	{
		Thiscall<void(CGameManager*)> func{0x425F60};
		func(this);
	}

	void InitScreen_Menu()
	{
		Thiscall<void(CGameManager*)> func{0x426210};
		func(this);
	}

	void InitScreen_Close()
	{
		Thiscall<void(CGameManager*)> func{0x426330};
		func(this);
	}

	void Menu(int a)
	{
		Thiscall<void(CGameManager*, int)> func{0x4292D0};
		func(this, a);
	}

	bool MenuEnabled()
	{
		Thiscall<int(CGameManager*, int&)> func{0x42AAF0};
		int a = 0;
		return func(this, a);
	}

	void PlayVideo(std::string const& fileName)
	{
		Thiscall<int(CGameManager*, zSTRING)> call{0x42B940};
		call(this, zSTRING{fileName});
	}

	void GameSessionDone()
	{
		Thiscall<void(CGameManager*)> _g_GameSessionDone{0x426F70};
		_g_GameSessionDone(this);
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
	oCMenuSavegame* menu_save          = nullptr;
	oCMenuSavegame* menu_load          = nullptr;

	//wird selten (?) aktualisiert. Mindestens aber beim Speichern und Laden.
	int playTime = 0;
};
#endif
