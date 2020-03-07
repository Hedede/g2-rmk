#include <Gothic/Game/CGameManager.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/oSaveGame.h>
#include <Gothic/Audio/zMusic.h>
#include <Gothic/Audio/zSound.h>
#include <Gothic/Menu/zMenu.h>
#include <Gothic/System/System.h>

namespace g2 {
void InitSound();
void InitMusic();

auto DefineMenuScriptFunctions = Cdecl<int()>(0x42C1D0);
void InitMenu()
{
	Log("Startup", "Initializing the menu-system");
	zoptions->ReadBool("INTERNAL", "extendedMenu" , 0);
	zCMenu::CreateParser();
	DefineMenuScriptFunctions();
}

void InitOptions();
void InitGameOptions();
void VideoOptions();

void InitConsole();
void InitRenderer(void* hwnd);
void InitGraphics();
void InitInput(void* hwnd);
} // namespace g2



#include <SDL2/SDL.h>
#include <Gothic/Game/zSession.h>
void CGameManager::Init(void* hwnd)
{
	using namespace g2;

	sysContextHandle = hwnd;

	Log("Startup", "Initializing GameManager");

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

	sysEvent();

	PreGraphicsInit();

	sysEvent();


	savegameManager = new oCSavegameManager();
	savegameManager->Reinit();

	sysEvent();

	g2::InitMenu();
	g2::VideoOptions();

	if ( zoptions->Parm("PLAYER") ) {
		std::string playerInst = zoptions->ParmValue("PLAYER");
		if ( zgameoptions )
			zgameoptions->WriteString("SETTINGS", "Player", playerInst, 1);
		else
			zoptions->WriteString("INTERNAL", "playerInstanceName", playerInst, 1);
	}

	sysEvent();
	SetEnableHandleEvent(true);

	auto& ogame = Value<oCGame*>(0xAB0884);

	gameSession = new oCGame();
	ogame = gameSession;

	gameSession->Init();
	gameSession->savegameManager = savegameManager;
	gameSession->SetGameInfo(nullptr);

	sysEvent();

	// TODO: move into InitMenu
	zCMenu::Startup();

	Log("Startup", "GameManager initializing done");
}


#include <Graphics/SplashWindow.h>
#include <Gothic/System/zFILE_VDFS.h>
#include <Gothic/Graphics/zTexture.h>
#include <Gothic/Game/oObjectFactory.h>
#include <Gothic/Menu/zView.h>
#include <Graphics/FontMan.h>
#include <Gothic/Script/zParser.h>
#include <Gothic/System/zResourceManager.h>
#include <Gothic/System/zMemPoolBase.h>
#include <Gothic/Graphics/zModelPrototype.h>
#include <Gothic/Graphics/zMorphMeshProto.h>
#include <Gothic/Graphics/zMesh.h>
void CGameManager::PreGraphicsInit()
{
	using namespace g2;

	SplashWindow splash;

	if (!splash.IsRunning())
		Warning("Startup", "Could not create splash window.");

	zCClassDef::EndStartup();

	zfactory = new oCObjectFactory;

	g2::InitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	//stub in .exe
	//zCMallocGeneric::Init(memProfiler);
	zCMemPoolBase::DisablePools(zoptions->Parm("ZNOMEMPOOL"));

	if (false)
		zFILE_VDFS::InitFileSystem();

	sysEvent();

	bool noResThread = zoptions->Parm("ZNORESTHREAD");

	zresMan = new zCResourceManager;
	zresMan->SetThreadingEnabled(!noResThread);

	zoptions->ChangeDir(DIR_SYSTEM);
	//_control87(0x9001Fu, 0xFFFFu);

	zfontman = new g2::FontMan{};

	g2::InitConsole();

	zCParser::enableParsing = zoptions->Parm("ZREPARSE");

	sysEvent();

	g2::InitRenderer(sysContextHandle);
	g2::InitGraphics();
	g2::InitSound();
	g2::InitMusic();
	g2::InitInput(sysContextHandle);

	bool texConvert = zoptions->Parm("ZTEXCONVERT");
	if ( texConvert ) {
		std::string val = zoptions->ParmValue("ZTEXCONVERT");
		zCTexture::ScanConvertTextures(val);
	}

	bool convData = zoptions->Parm("ZAUTOCONVERTDATA");

	zCModelPrototype::autoConvertAnis = convData;
	zCMesh::autoConvertMeshes = convData;
	zCMorphMeshProto::autoConvertBinaryFile = convData;

	// zfpuControler->RestoreDefaultControlWord()
	// zfpuControler->SaveCurrentControlWord();
	
	if (zoptions->ReadBool("GAME", "playLogoVideos", 1)) {
		Log("Startup", "Playing videos");
		PlayVideo("logo1.bik");
		PlayVideo("logo2.bik");
	}

	sysEvent();

	bool recalc = zoptions->ReadBool("PERFORMANCE", "recalc", 1);
	zoptions->WriteBool("PERFORMANCE", "recalc", 0, 0);

	if ( recalc )
		InitSettings();

	InitScreen_Open();
	g2::InitGameOptions();

	vidScreen = screen;
	vidScreen->SetEnableHandleEvent(1);

	sysEvent();

	Cdecl<void()> oCarsten_StartUp{0x4816F0};
	oCarsten_StartUp();

	zCMenu::EnterCallback = func<zCMenu::Callback>(0x426A80); // MenuEnterCallback;
	zCMenu::LeaveCallback = func<zCMenu::Callback>(0x426B80); // MenuLeaveCallback;

	bool convertAll = zoptions->Parm("ZCONVERTALL");
	if ( convertAll )
		Tool_ConvertData();
}

#include <Gothic/Menu/oMenuSavegame.h>
#include <Gothic/Menu/oMenuChgKeys.h>
#include <Gothic/Game/zSession.h>
void CGameManager::PreRun()
{
	using namespace g2;
	Log("Game", "PreRun");

	if ( dontStartGame )
		return;

	menu_chgkeys       = new oCMenuChgKeys("MENU_OPT_CONTROLS");
	menu_chgkeys_ext   = new oCMenuChgKeys("MENU_OPT_CONTROLS_EXTKEY");

	menu_save = new oCMenuSavegame("MENU_SAVEGAME_SAVE", oCMenuSavegame::Mode::Save);
	menu_load = new oCMenuSavegame("MENU_SAVEGAME_LOAD", oCMenuSavegame::Mode::Load);

	menu_save->saveMan = savegameManager;
	menu_load->saveMan = savegameManager;

	InitScreen_Menu();

	zoptions->WriteBool("INTERNAL", "gameStartFailed", 0, 0);

	std::string ini = zoptions->ParmValue("ini");
	if (ini.empty())
		ini = "Gothic.ini";

	zoptions->Save(ini);

	Log("Game", "PreRun Menu");
	while (!exitGame) {
		if (gameSession && gameSession->GetCamera())
			break;
		zCMenu::inGameMenu = 0;
		Menu(0);
		zCMenu::inGameMenu = 1;
	}

	InitScreen_Close();
}

bool& chapBool = Value<bool>(0x8C2954);
void CGameManager::RenderFrame()
{
	if (backLoop) {
		g2::Warning("Game", "backloop is actually used!");
	}

	if (gameSession && gameSession->GetCamera()) {
		gameSession->Render();
		gameSession->RenderBlit();
	}

	Cdecl<void()> IntroduceChapter{0x42B220};
	if (chapBool && MenuEnabled())
		IntroduceChapter();
}

void CGameManager::Run()
{
	while (!exitGame) {
		if (exitSession) {
			InitScreen_Open();
			vidScreen->SetEnableHandleEvent(1);
			InitScreen_Menu();

			while (!exitGame) {
				if (!exitSession)
					break;
				zCMenu::inGameMenu = 0;
				Menu(0);
				zCMenu::inGameMenu = 1;
			}

			InitScreen_Close();
			exitSession = 0;
		} else {
			sysEvent();
			zCInputCallback::GetInput();
			RenderFrame();
		}
	}
}


#include <Gothic/System/zCache.h>
void CGameManager::Done()
{
	Cdecl<void()> oCarsten_ShutDown{0x4839A0};
	Cdecl<void()> zCarsten_ShutDown{0x50AB30};
	Cdecl<void()> zDieter_ShutDown{0x639040};
	Cdecl<void()> zUlfi_ShutDown{0x7B43B0};
	Cdecl<void()> zDeinitOptions{0x471050};

	g2::Log("GameManager", "Exiting game.");

	if (menu_chgkeys)
		delete menu_chgkeys;
	if (menu_chgkeys_ext)
		delete menu_chgkeys_ext;
	if (menu_save)
		delete menu_save;
	if (menu_load)
		delete menu_load;

	menu_chgkeys       = 0;
	menu_chgkeys_ext   = 0;
	menu_save          = 0;
	menu_load          = 0;

	zCCacheBase::S_ClearCaches();
	// sysKillWindowsKeys() // won't reimplement

	// Always no fast exit

	GameSessionDone();
	oCarsten_ShutDown();
	zCarsten_ShutDown();
	zDieter_ShutDown();
	zUlfi_ShutDown();

	zresMan->EndThread();
	delete zresMan;

	zDeinitOptions();

	if (false)
		zFILE_VDFS::DeinitFileSystem();

	g2::Log("GameManager", "Game exit done.");
}

#include <Gothic/Game/zConsole.h>
void g2::InitConsole()
{
	zcon.SetPos(0, 0);
	zcon.SetDim(8191, 1500);
	zcon.SetAutoCompletion(1);

	zcon.Register("ZERR AUTHORS", "Set the author-filter for messages (as characters A-Z)");
	zcon.Register("ZERR SEARCHSPY", "Search for existing zSpy if started later than game.");
	zcon.Register("ZERR LEVEL", "Set the maximum priority-level for messages (from -1 to 10)");
	zcon.Register("ZERR STATUS", "Show error-status (current level, target ...)");
	zcon.Register("ZERR REM", "Include a remark into the error-log.");
	zcon.Register("ZERR ZVIEW", "just for internal tests");

	zcon.Register("SET CLIPPINGFACTOR", "Setting the clipping-factor. Default is 1. Usually check 0.1 ... 2.0");
	zcon.Register("LIST CS", "List running cutscenes.");
	zcon.Register("LIST CS STATES", "List running cutscenes including their current status.");
	zcon.Register("LIST CS PROPS", "List running cutscenes including status and properties.");
	zcon.Register("LIST CS HISTORY", "List all cutscenes in the history-pool.");
	zcon.Register("SHOW CS", "Show information of a cutscene (add cutscene-name including extension)");
	zcon.Register("PLAY CS", "Play a cutscene (add cutscene-name including extension)");
	zcon.Register("PLAY VIDEO", "Playing a videofile");
	zcon.Register("TOGGLE CS LIST", "Toggle list of running cutscenes on screen");
	zcon.Register("TOGGLE CUTSCENES ENABLED", "Enable or disable cutscene-system");
	zcon.Register("HERO EXPORT", "Exports your hero to the given file.");
	zcon.Register("HERO IMPORT", "Imports your hero from the given file.");

	zcon.Register("ZMARK", "marks outdoor occluder polys");
	zcon.Register("ZWORLD STATUS", "prints some engine-world data");
	zcon.Register("ZWORLD ACTIVEVOBS", "prints engine-world activeVobList");
	zcon.Register("ZWORLD VOBTREE", "prints engine-world globalVobTree");
	zcon.Register("ZWORLD VOBPROPS", "prints props of vob with specified name [VOB_NAME | VOB_ID]");
	zcon.Register("ZRMODE MAT", "rendermode material/normal");
	zcon.Register("ZRMODE WMAT", "rendermode material with overlaid wireframe");
	zcon.Register("ZRMODE FLAT", "rendermode flat");
	zcon.Register("ZRMODE WIRE", "rendermode wireframe");
	zcon.Register("ZTOGGLE LIGHTSTAT", "toggles lightmaps/vertLight");
	zcon.Register("ZTOGGLE VOBBOX", "toggles bbox3D drawing of vobs");
	zcon.Register("ZTOGGLE RENDERVOB", "toggles drawing of vobs");
	zcon.Register("ZTOGGLE MODELSKELETON", "toggles drawing of all models node-hierarchies");
	zcon.Register("ZTOGGLE SMOOTHROOTNODE", "toggles smooothing of model root nodes translation");
	zcon.Register("ZTOGGLE TEXSTATS", "toggles display of scene texture statistics");
	zcon.Register("ZRNDSTAT", "renderer statistics");
	zcon.Register("ZRNDMODELIST", "enumerates the renderers available modes and devices");
	zcon.Register("ZVIDEORES", "sets video resolution");
	zcon.Register("ZLIST MAT", "enumerating materials");
	zcon.Register("ZLIST TEX", "enumerating textures");
	zcon.Register("ZLIST MESH", "enumerating meshes");
	zcon.Register("ZLIST CLASSTREE", "enumerating class hierarchy");
	zcon.Register("ZMODEL PRINTTREE", "prints a model's node hierarchy [MODEL_NAME]");
	zcon.Register("ZMOVECAMTOVOB", "[VOB_NAME | VOB_ID]");
	zcon.Register("ZSOUNDMAN DEBUG", "toggles SoundManager debug info");
	zcon.Register("ZTRIGGER", "sends trigger-message to vob [VOB_NAME | VOB_ID]");
	zcon.Register("ZUNTRIGGER", "sends untrigger-message to vob [VOB_NAME | VOB_ID]");
	zcon.Register("ZARCTEST", "tests integrity of each classes' arc/unarc funcs");
	zcon.Register("ZOVERLAYMDS APPLY", "applies overlay-.MDS to vob's model [VOB_NAME | VOB_ID] [MDS_NAME]");
	zcon.Register("ZOVERLAYMDS REMOVE", "removes overlay-.MDS from vob's model [VOB_NAME | VOB_ID] [MDS_NAME]");
	zcon.Register("ZLIST", "list every living object of class [CLASS_NAME], if the class has SHARED_OBJECTS flag");
	zcon.Register("ZTOGGLE SHOWZONES", "lists all zones the camera is currently located in (sound,reverb,fog,..)");
	zcon.Register("ZTOGGLE SHOWTRACERAY", "displays all rays traced in the world as lines");
	zcon.Register("ZTOGGLE SHOWPORTALS", "displays portals processed during occlusion during");
	zcon.Register("ZTOGGLE SHOWHELPVERVISUALS", "displays helper visuals for vobs that don't have a natural visualization (eg zCTriggers)");
	zcon.Register("ZTOGGLE PFXINFOS", "");
	zcon.Register("ZSTARTANI", "starts ani on specified vob if it has a animatable visual [VOB_NAME] [ANI_NAME]");
	zcon.Register("ZLIST VOBSWITHBIGBBOX", "lists suspicious vobs with very large bboxes");
	zcon.Register("ZLIST MESHESWITHLOTSOFMATERIALS", "lists suspicious meshes with large material counts [NUM_MIN]");
	zcon.Register("ZTOGGLE RESMANSTATS", "displays resource manager statistics (textures,sounds,..)");
	zcon.Register("ZPROGMESHLOD", "apply global strength value to all pm LOD rendering, -1(default), 0..1..x");
	zcon.Register("ZTOGGLE MARKPMESHMATERIALS", "marks vob/pmesh materials with color-code: red=1st mat, blue=2nd mat, green=3rd, yellow..white..brown..black=7th");
	zcon.Register("ZTOGGLE PMESHSUBDIV", "debug");
	zcon.Register("ZTOGGLE SHOWMEM", "displays information on heap allocations realtime onscreen");
	zcon.Register("ZTOGGLE VOBMORPH", "toggles morphing of vobs");
	zcon.Register("ZTOGGLE MATMORPH", "toggles morphing of materials");
	zcon.Register("ZTOGGLE TNL", "toggles using of hardware transform and lightning");
	zcon.Register("ZMEM DUMPHEAP BYTESPERLINE", "dumps current heap allocations sorted by bytes per line");
	zcon.Register("ZMEM DUMPHEAP BLOCKSPERLINE", "dumps current heap allocations sorted by block per line");
	zcon.Register("ZMEM CHECKHEAP", "checks consistency of current heap allocations");
	zcon.Register("ZSTARTRAIN", "starts outdoor rain effect [STRENGTH]");
	zcon.Register("ZSTARTSNOW", "starts outdoor snow effect [STRENGTH]");
	zcon.Register("ZSET VOBFARCLIPZSCALER", "adjusts far clipping plane for objects, 1 being default");
	zcon.Register("ZSET LEVELFARCLIPZSCALER", "adjusts far clipping plane for static level mesh, 1 being default");
	zcon.Register("ZHIGHQUALITYRENDER", "batch activation of high-quality render options: vob/level farClip, LevelLOD-Strength, Object-LOD, TexMaxSize");
	zcon.Register("ZTIMER MULTIPLIER", "sets factor for slow/quick-motion timing");
	zcon.Register("ZTIMER REALTIME", "resets factor for slow/quick-motion timing to realtime");
	zcon.Register("ZFOGZONE", "inserts test fog-zones");
	zcon.Register("ZTOGGLE SHOWSPLINES", "Toggles camera splines ");
	zcon.Register("ZTOGGLE TIMEDEMO", "Toggles a time demo with avg FPS Rate");
	zcon.Register("ZTOGGLE RENDERORDER", "Renders Sky last (possible better z buffer performance)");
	zcon.Register("ZTOGGLE ENVMAP", "Toggled rendering of environmental fx");
	zcon.Register("ZTOGGLE AMBIENTVOBS", "Toggles rendering of ambient vobs");
	zcon.Register("ZTOGGLE AMBIENTPFX", "Toggles rendering of ambient pfx");
	zcon.Register("ZTOGGLE RENDERPORTALS", "Toggles rendering of all portals (spacer only)");
	zcon.Register("ZSET NEARCLIPZ", "sets the value for the near clipping plane");
	zcon.Register("ZTOGGLE FLUSHONAMBIENTCOL", "toggles flushing the renderer on a ambientcol change");

	auto zBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x471520);
	auto oBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x430A70);
	zcon.AddEvalFunc( zBert_EvalFunc );
	zcon.AddEvalFunc( oBert_EvalFunc );

	auto zDieter_EvalFunc  = reinterpret_cast<zCConsole::EvalFunc*>(0x632E00);
	auto zCarsten_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x509D00);
	zcon.AddEvalFunc( zDieter_EvalFunc );
	zcon.AddEvalFunc( zCarsten_EvalFunc );
}

//#include <aw/utility/filesystem.h>
#include <Gothic/Types/zCOLOR.h>
#include <Gothic/Game/zTimer.h>
#include <Gothic/Graphics/zRnd_D3D.h>
#include <Gothic/Graphics/zRenderManager.h>
#include <aw/utility/string/split.h>
void g2::InitRenderer(void* hwnd)
{
	Log("Initializing renderer.");

	if ( zoptions->Parm("ZMAXFRAMERATE")) {
		auto val = zoptions->ParmValue("ZMAXFRAMERATE");
		ztimer.LimitFPS( ToUnsigned(val) );
	}

	int deviceOverride = -1;
	if ( zoptions->Parm("ZRND") ) {
		auto val = zoptions->ParmValue("ZRND");
		if (val.find("D3D") != std::string::npos) {
			zrenderer = new zCRnd_D3D; // 0x82E7C
			if ( val.find("1") != std::string::npos )
				deviceOverride = 1;
		}
	}

	if ( !zrenderer ) {
		Log("Init", "No renderer specified, initializing default: D3D7");
		zrenderer = new zCRnd_D3D;
	}

	auto resX       = zoptions->ReadInt( "VIDEO", "zVidResFullscreenX", 800);
	auto resY       = zoptions->ReadInt( "VIDEO", "zVidResFullscreenY", 600);
	auto bpp        = zoptions->ReadInt( "VIDEO", "zVidResFullscreenBPP",16);
	auto deviceNo   = zoptions->ReadInt( "VIDEO", "zVidDevice", 0);
	auto gamma      = zoptions->ReadReal("VIDEO", "zVidGamma", 0.5);
	auto contrast   = zoptions->ReadReal("VIDEO", "zVidContrast", 0.5);
	auto brightness = zoptions->ReadReal("VIDEO", "zVidBrightness", 0.5);

	if ( zoptions->Parm("ZRES") ) {
		auto val = zoptions->ParmValue("ZRES");
		auto vals = aw::string::split(val, ",");
		if (vals.size() > 0)
			resX = ToUnsigned( (std::string)vals[0] );
		if (vals.size() > 1)
			resY = ToUnsigned( (std::string)vals[1] );
		if (vals.size() > 2)
			bpp  = ToUnsigned( (std::string)vals[2] );
	}

	if ( deviceOverride >= 0 )
		deviceNo = deviceOverride;

	auto windowed = zoptions->ReadBool("VIDEO", "zStartupWindowed", 0);
	if ( zoptions->Parm("ZWINDOW") )
		windowed = 1;
	
	zrenderer->Vid_SetDevice(deviceNo);
	zrenderer->Vid_SetScreenMode(windowed != 0);
	zrenderer->Vid_SetMode(resX, resY, bpp, hwnd);
	zrenderer->Vid_Clear(colors::black, 3);
	zrenderer->Vid_SetGammaCorrection(gamma, contrast, brightness);

	zCTexture::AutoDetectRendererTexScale();

	zCTexture::s_globalLoadTextures = !zoptions->Parm("ZNOTEX");
	zCView::SetMode(resX, resY, bpp, hwnd);

	zrenderMan.InitRenderManager();

	Log("Startup", "Renderer is initialized");
}

#include <Gothic/Bsp/zRayTurbo.h>
#include <Gothic/Game/zVob.h>
#include <Gothic/Game/zVobLight.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Graphics/zVisual.h>
#include <Gothic/Graphics/zMapDetailTexture.h>
#include <Gothic/Graphics/zProgMeshProto.h>
#include <Gothic/Graphics/zLensFlareFX.h>
#include <Gothic/Graphics/zDecal.h>
#include <Gothic/Graphics/zFFT.h>
#include <Gothic/Graphics/zPolygon.h>
#include <Gothic/PFX/zParticleFX.h>
void g2::InitGraphics()
{
	Log("Startup", "Initializing graphics subsystems");

	zCMapDetailTexture::S_Init();
	zCVisual::InitVisualSystem();
	zCVob::InitVobSystem();
	zCLensFlareFX::LoadLensFlareScript();
	zCVobLight::LoadLightPresets();
	zCDecal::CreateDecalMeshes();
	zCParticleFX::InitParticleFX();
	zCProgMeshProto::InitProgMeshProto();

	auto polyTreshold = zCRayTurboAdmin::GetPolyTreshold();
	polyTreshold = zoptions->ReadInt("ENGINE", "zRayTurboPolyTreshold", polyTreshold);
	zCRayTurboAdmin::SetPolyTreshold(polyTreshold);

	zCFFT::S_Init();
	zCPolygon::S_InitMorph();

	auto maxFps = zCTimer::S_GetMaxFPS();
	maxFps = zoptions->ReadDWord("ENGINE", "zMaxFPS", maxFps);
	zCTimer::S_SetMaxFPS(maxFps);

	int& zCParticleFX_s_bAmbientPFXEnabled  = Value<int>(0x8A4E48);
	int& zCAIPlayer_s_bShowWeaponTrails     = Value<int>(0x89EC90);
	int& zCWorld_s_bAmbientVobsEnabled      = Value<int>(0x8A8AC0);
	int& zCWorld_s_bEnvMappingEnabled       = Value<int>(0x8A8AC4);
	int& zCWorld_s_bAlternateRenderOrder    = Value<int>(0x9A4424);
	zCParticleFX_s_bAmbientPFXEnabled = zoptions->ReadBool("ENGINE", "zAmbientPFXEnabled", 1);
	zCWorld_s_bAmbientVobsEnabled     = zoptions->ReadBool("ENGINE", "zAmbientVobsEnabled", 1);
	zCWorld_s_bEnvMappingEnabled      = zoptions->ReadBool("ENGINE", "zEnvMappingEnabled", 1);
	zCWorld_s_bAlternateRenderOrder   = !zoptions->ReadBool("ENGINE", "zSkyRenderFirst", 1);
	zCAIPlayer_s_bShowWeaponTrails    = zoptions->ReadBool("GAME", "zShowWeaponTrails", 1);

	ztimer.SetMaxFPS(zCTimer::S_GetMaxFPS());
}

#include <Gothic/Audio/zSoundSystemDummy.h>
#include <Gothic/Audio/zSoundSys_MSS.h>
#include <Sound/SoundOpenAL.h>
#include <Sound/zCSndSys_OpenAL.h>
#include <Gothic/Audio/zSoundManager.h>
auto ChangeFxVol = func<zCOptions::ChangeHandler>(0x509370);
void g2::InitSound()
{
	Log("Startup", "Initializing Sound");

	if ( zoptions->Parm("ZNOSOUND") )
		zoptions->WriteBool("SOUND", "soundEnabled", 0, 1);

	auto soundEnabled = zoptions->ReadBool("SOUND", "soundEnabled", 1);

	if ( soundEnabled ) {
		//zsound = new zCSndSys_MSS;
		zsound = new zCSndSys_OpenAL;
	} else {
		zsound = new zCSoundSystemDummy;
	}

	zsndMan = new zCSoundManager;

	Log("Startup", "Adding sound options");
	zoptions->InsertChangeHandler("SOUND", "soundVolume",   ChangeFxVol);
	auto sfxVol = zoptions->ReadReal("SOUND", "soundVolume", 1.0);
	zoptions->WriteReal("SOUND", "soundVolume", sfxVol, 0);
}

#include <Gothic/Audio/zMusicSys_Dummy.h>
#include <Gothic/Audio/zMusicSys_DM.h>
auto ChangeMusicEnabled = func<zCOptions::ChangeHandler>(0x42D240);
auto ChangeMusicVol = func<zCOptions::ChangeHandler>(0x5093E0);

void g2::InitMusic()
{
	Log("Startup", "Initializing Music");
	
	if ( zoptions->Parm("ZNOMUSIC") )
		zoptions->WriteBool("SOUND", "musicEnabled", 0, 1);

	auto musicEnabled = zoptions->ReadBool("SOUND", "musicEnabled", 1);
	zCMusicSystem::DisableMusicSystem(!musicEnabled);

	if ( musicEnabled ) {
		zmusic = new zCMusicSys_DirectMusic;
	} else {
		zmusic = new zCMusicSys_Dummy;
	}

	Log("Startup", "Adding music options");
	zoptions->InsertChangeHandler("SOUND", "musicEnabled",  ChangeMusicEnabled);
	zoptions->InsertChangeHandler("SOUND", "musicVolume",   ChangeMusicVol);

	auto musVol = zoptions->ReadReal("SOUND", "musicVolume", 0.8);
	zoptions->WriteReal("SOUND", "musicVolume", musVol, 0);
}

#include <aw/utility/string/case.h>
#include <Filesystem/directory_config.h>
void g2::InitOptions()
{
	Log("Startup", "Initializing options");
	auto& sysCommandLine = Value<char*>(0x8D3D2C);
	zoptions = new zCOptions{};
	zoptions->Init(sysCommandLine);
	zoptions->ChangeDir(DIR_SYSTEM);

	// TODO: parameter is a dummy, it just copies paths from zoptions
	read_directory_config( "system/paths.d" );

	std::string ini = zoptions->ParmValue("ini");
	if ( ini.empty() )
		ini = "Gothic.ini";

	zoptions->Load(ini);

	int version = zoptions->ReadInt("GAME", "PATCHVERSION", 0);
	if (version < 0)
		version = 0;

	switch (version) {
	case 0:
		zoptions->WriteBool("GAME", "enableJoystick", 0, 0);
		++version;
	case 1:
		zoptions->WriteReal("GAME", "SHORTKEY1FARPLANEDIST", 0.8, 0);
		zoptions->WriteReal("GAME", "SHORTKEY2FARPLANEDIST", 1.2, 0);
		zoptions->WriteReal("GAME", "SHORTKEY3FARPLANEDIST", 2.0, 0);
		zoptions->WriteReal("GAME", "SHORTKEY4FARPLANEDIST", 3.0, 0);
		zoptions->WriteBool("RENDERER_D3D", "zSyncAmbientCol", 0, 0);

		zoptions->WriteBool("INTERNAL", "logStatistics", 0, 0);
		zoptions->WriteBool("INTERNAL", "extendedMenu", 0, 0);
		zoptions->WriteBool("INTERNAL", "gameAbnormalExit", 0, 0);

		zoptions->WriteReal("ENGINE", "zInventoryItemsDistanceScale", 1.3, 0);
		++version;
	case 2:
		zoptions->WriteBool("RENDERER_D3D", "radeonHackAmbientColBug", 0, 0);
		++version;
	case 3:
		zoptions->WriteString("INTERNAL", "gameScript", "", 0);
		zoptions->WriteString("INTERNAL", "playerInstanceName", "", 0);
		++version;
	case 4:
		zoptions->WriteBool("RENDERER_D3D", "geforce3HackWBufferBug", 1, 0);
		zoptions->WriteBool("ENGINE", "zSkyRenderFirst", 1, 0);
		++version;
	default:
		break;
	}

	zoptions->WriteInt("GAME", "PATCHVERSION", version, 0);

	if (zoptions->ReadBool("INTERNAL", "gameStartFailed", 0))
		Warning("Previous startup has failed");

	if (zoptions->ReadBool("INTERNAL", "gameAbnormalExit", 0))
		Warning("Previous game has exited uncleanly");

	zoptions->WriteBool("INTERNAL", "gameAbnormalExit", 1, 0);
	zoptions->WriteBool("INTERNAL", "gameStartFailed", 1, 0);

	int gameStarts = zoptions->ReadInt("INTERNAL", "gameStarts", 0);
	zoptions->WriteInt("INTERNAL", "gameStarts", gameStarts + 1, 0);

	zoptions->RemoveEntry("INTERNAL", "testmode");
	zoptions->RemoveEntry("GAME", "weatherEffects");
	zoptions->RemoveEntry("INTERNAL", "zFastUnsafeSaveGames");

	zoptions->Save(ini);

	std::string game = zoptions->ParmValue("GAME");
	if ( !game.empty() ) {
		zoptions->ChangeDir(DIR_SYSTEM);
		if ( !fs::exists(game) ) {
			game += ".ini";
		}

		if ( fs::exists(game) ) {
			zgameoptions = new zCOptions{};
			// no init because it's no-op anyway
			zgameoptions->Load(game);
			auto forceparams = zgameoptions->ReadString("OPTIONS", "force_Parameters", 0);
			if ( !forceparams.empty() )
				zoptions->AddParameters(forceparams);
		}
	}
}

auto ChangeVidOptions = func<zCOptions::ChangeHandler>(0x509450);
void g2::VideoOptions()
{
	Log("Startup", "Adding video options");

	zoptions->ReadInt("PERFORMANCE", "sightValue" , 4);
	zoptions->ReadReal("PERFORMANCE", "modelDetail" , 0.5);
	zoptions->ReadReal("INTERNAL", "texDetailIndex" , 0.6);
	zoptions->ReadInt("GAME", "skyEffects" , 1);
	zoptions->ReadInt("GAME", "bloodDetail" , 2);

	zoptions->ReadReal("VIDEO", "zVidBrightness" , 0.5);
	zoptions->ReadReal("VIDEO", "zVidContrast"   , 0.5);
	zoptions->ReadReal("VIDEO", "zVidGamma"      , 0.5);
	zoptions->InsertChangeHandler("VIDEO", "zVidBrightness", ChangeVidOptions);
	zoptions->InsertChangeHandler("VIDEO", "zVidContrast",   ChangeVidOptions);
	zoptions->InsertChangeHandler("VIDEO", "zVidGamma",      ChangeVidOptions);
}

void g2::InitGameOptions()
{
	Log("Startup", "Initializing game options");

	std::string parse;
	if ( zoptions->Parm("PARSE") )
		parse = zoptions->ParmValue( "PARSE" );

	std::string world;
	if ( zoptions->Parm("3D") )
		world = zoptions->ParmValue( "3D" );

	std::string player;
	if ( zoptions->Parm("PLAYER") )
		player = zoptions->ParmValue("PLAYER");

	if ( parse.size() > 0 ) {
		std::string ext = parse.substr(parse.size() - 5);
		// aw::fs::path path(parse);
		// std::string ext = path.extension();
		if ( aw::string::tolower(ext) == ".src" )
			parse.resize(parse.size() - 4);

		if ( parse.size() > 0 ) {
			if ( zgameoptions ) {
				zgameoptions->WriteString("FILES", "Game", parse, 1);
			} else {
				zoptions->WriteString("INTERNAL", "gameScript", parse, 1);
			}
		}
	}
	if ( world.size() > 0 ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString("SETTINGS", "World", world, 1);
		} else {
			zoptions->WriteString("INTERNAL", "gamePath", world, 1);
		}
	}
	if ( player.size() > 0 ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString("SETTINGS", "Player", player, 1);
		} else {
			zoptions->WriteString("INTERNAL", "playerInstanceName", player, 1);
		}
	}

	static int& gLogStatistics = Value<int>(0x8C2B50);
	gLogStatistics = zoptions->ReadBool("INTERNAL", "logStatistics", 0);
}

#include <Gothic/Input/zInput.h>
void g2::InitInput(void* hwnd)
{
	if ( zinput )
		delete zinput;
	zinput = (zCInput*)new zCInput_Win32(hwnd);
}

void CGameManager::GameSessionReset()
{
	if ( !gameSession )
		g2::Warning("Game", "reset, gameSession == nullptr ???");

	savegameManager->ClearCurrent();
	gameSession->SetGameInfo(nullptr);
}

void CGameManager::Menu(int inGame)
{
	g2::Log("Game", "Menu called");

	zinput->ResetRepeatKey(1);
	gameSession->SetShowPlayerStatus(0);

	zoptions->WriteString("internal", "menuAction", "RESUME_GAME", 0);
	
	if (exitGame)
	{
		g2::Log("Game", "Menu exitGame");
		return;
	}

	auto noMenu = zoptions->Parm("NOMENU");

	PreMenu();

	if ( gameSession && gameSession->GetCamera() ) {
		if ( noMenu ) {
			exitGame = 1;
			return;
		}

		g2::Log("Menu", "started");

		if ( gameSession )
			gameSession->Pause(exitSession);

		auto inGameMenu = zCMenu::inGameMenu;
		if ( !inGame )
			zCMenu::inGameMenu = 0;

		auto gameMenu = zCMenu::Create("MENU_MAIN");

		gameMenu->Run();
		gameMenu->Release();

		zCMenu::inGameMenu = inGameMenu;

		if ( gameSession )
			gameSession->Unpause();

		g2::Log("Menu", "finished");
	} else {
		if ( noMenu ) {
			zoptions->WriteString("internal", "menuAction", "NEW_GAME", 0);
		} else {
			g2::Log("Menu", "started");

			auto mainmenu = zCMenu::Create("MENU_MAIN");

			mainmenu->Run();
			mainmenu->Release();

			g2::Log("Menu", "finished");
		}
	}

	if ( zrenderer ) {
		screen->DrawItems();
		zrenderer->Vid_Blit(0, 0, 0);
	}

	auto menuAction = zoptions->ReadString("internal", "menuAction", 0);

	g2::Log("Menu", menuAction); // 1430

	if ( menuAction == "LEAVE_GAME")
	{
		exitGame = true;
		zoptions->WriteBool("internal", "gameAbnormalExit", 0, 0);
	}
	else if (menuAction == "NEW_GAME")
	{
		std::string world;
		if ( zgameoptions )
			world = zgameoptions->ReadString("settings", "World", 0);
		else
			world = zoptions->ReadString("internal", "gamePath", 0);

		if (world.empty())
			world = "NewWorld\\NewWorld.zen";

		GameSessionReset();
		exitSession = 0;

		g2::Log("Menu", "Loading new world");

		gameSession->LoadGame(SAVEGAME_SLOT_NEW, world);

		if ( zmusic )
			zmusic->Stop();

		g2::Log("Menu", "Loading done");

		playTime = 0;
	}
	else if (menuAction == "SAVEGAME_LOAD")
	{
		Read_Savegame(menu_load->GetSelectedSlot());
	}
	else if (menuAction == "SAVEGAME_SAVE")
	{
		Write_Savegame(menu_save->GetSelectedSlot());
	}
	else if (menuAction == "RESUME_GAME")
	{
		//
	}

	g2::Log("Menu", "Finished processing action");

	auto ini = zoptions->ParmValue("ini");
	if (ini.empty())
		ini = "Gothic.ini";

	zoptions->Save(ini);

	introActive = 0;

	Thiscall<void(CGameManager*)> ApplySomeSettings{0x4276B0};

	ApplySomeSettings(this);

	if ( gameSession )
		gameSession->UpdateScreenResolution();

	zinput->ProcessInputEvents();
	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);

	if ( !exitSession )
		gameSession->SetShowPlayerStatus(1);

	zinput->ProcessInputEvents();
	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);

	g2::Log("Menu", "Menu return");
}
