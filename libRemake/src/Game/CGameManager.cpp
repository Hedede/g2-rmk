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

void InitConsole();
void InitRenderer(void* hwnd);
void InitGraphics();
} // namespace g2



#include <SDL2/SDL.h>
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
	g2::InitOptions();

	if ( zoptions->Parm("PLAYER") ) {
		std::string playerInst = zoptions->ParmValue("PLAYER");
		if ( zgameoptions )
			zgameoptions->WriteString("SETTINGS", "Player", playerInst, 1);
		else
			zoptions->WriteString("INTERNAL", "playerInstanceName", playerInst, 1);
	}

	sysEvent();
	SetEnableHandleEvent(true);

	GameSessionInit();

	sysEvent();

	// TODO: move into InitMenu
	zCMenu::Startup();

	Log("Startup", "GameManager initializing done");
}


#include <Graphics/SplashWindow.h>
#include <Gothic/System/zFILE_VDFS.h>
#include <Gothic/Graphics/zTexture.h>
#include <Gothic/Game/oObjectFactory.h>
#include <Gothic/Game/zResourceManager.h>
#include <Gothic/Menu/zView.h>
#include <Graphics/FontMan.h>
#include <Gothic/Script/zParser.h>
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

	Cdecl<void()> zInitOptions{0x4701F0};
	zInitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	//stub in .exe
	//zCMallocGeneric::Init(memProfiler);
	zCMemPoolBase::DisablePools(zoptions->Parm("ZNOMEMPOOL"));

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
	g2::InitSound();
	g2::InitMusic();



	Cdecl<void(void*&)> zCarsten_StartUp{0x509580};
	zCarsten_StartUp(sysContextHandle);

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

	while (!exitGame) {
		if (gameSession && gameSession->GetCamera())
			break;
		zCMenu::inGameMenu = 0;
		Menu(0);
		zCMenu::inGameMenu = 1;
	}

	InitScreen_Close();
}

void CGameManager::RenderFrame()
{
	if (backLoop) {
		backLoop->Render();
		backLoop->RenderBlit();
	} else if (gameSession && gameSession->GetCamera()) {
		gameSession->Render();
		gameSession->RenderBlit();
	}
}

bool& chapBool = Value<bool>(0x8C2954);
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

			Cdecl<void()> IntroduceChapter{0x42B220};
			if (chapBool && MenuEnabled())
				IntroduceChapter();
		}
	}
}

void CGameManager::Done()
{
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

	Thiscall<void(CGameManager*)> _g_Done{0x4254E0};
	_g_Done(this);
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
	zcon.Register("Imports your hero from the given file.", "HERO IMPORT");

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


	auto zBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x471520);
	auto oBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x430A70);
	zcon.AddEvalFunc( zBert_EvalFunc );
	zcon.AddEvalFunc( oBert_EvalFunc );

	auto zDieter_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x632E00);
	zcon.AddEvalFunc( zDieter_EvalFunc );
}

//#include <aw/utility/filesystem.h>
#include <Gothic/Types/zCOLOR.h>
#include <Gothic/Game/zTimer.h>
#include <Gothic/Graphics/zRnd_D3D.h>
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
		Log("No renderer specified, initializing default: D3D7");
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
			resX = ToUnsigned( vals[0] );
		if (vals.size() > 1)
			resY = ToUnsigned( vals[1] );
		if (vals.size() > 2)
			bpp  = ToUnsigned( vals[2] );
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

	Log("Startup", "Renderer is initialized");
}

#include <Gothic/Bsp/zRayTurbo.h>
#include <Gothic/Game/zVob.h>
#include <Gothic/Game/zVobLight.h>
#include <Gothic/Graphics/zVisual.h>
#include <Gothic/Graphics/zMapDetailTexture.h>
#include <Gothic/Graphics/zProgMeshProto.h>
#include <Gothic/Graphics/zLensFlareFX.h>
#include <Gothic/Graphics/zDecal.h>
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
}

#include <Gothic/Audio/zSoundSystemDummy.h>
#include <Gothic/Audio/zSoundSys_MSS.h>
#include <Gothic/Audio/zSoundManager.h>
void g2::InitSound()
{
	Log("Startup", "Initializing Sound");

	if ( zoptions->Parm("ZNOSOUND") )
		zoptions->WriteBool("SOUND", "soundEnabled", 0, 1);

	auto soundEnabled = zoptions->ReadBool("SOUND", "soundEnabled", 1);

	if ( soundEnabled ) {
		zsound = new zCSndSys_MSS;
	} else {
		zsound = new zCSoundSystemDummy;
	}

	zsndMan = new zCSoundManager;

	float masterVol = zoptions->ReadReal("SOUND", "soundVolume", 1.0);
	if ( zsound )
		zsound->SetMasterVolume(masterVol);
}

#include <Gothic/Audio/zMusicSys_Dummy.h>
#include <Gothic/Audio/zMusicSys_DM.h>
auto ChangeMusicEnabled = func<zCOptions::ChangeHandler>(0x42D240);
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

	zoptions->InsertChangeHandler("SOUND", "musicEnabled", ChangeMusicEnabled);
	float musicVol = zoptions->ReadReal("SOUND", "musicVolume", 0.8);
	if ( zmusic )
		zmusic->SetVolume(musicVol);
}

#include <aw/utility/string/case.h>
void g2::InitOptions()
{
	Log("Startup", "Initializing options");

	zoptions->ReadInt("PERFORMANCE", "sightValue" , 4);
	zoptions->ReadReal("PERFORMANCE", "modelDetail" , 0.5);
	zoptions->ReadReal("INTERNAL", "texDetailIndex" , 0.6);
	zoptions->ReadInt("GAME", "skyEffects" , 1);
	zoptions->ReadInt("GAME", "bloodDetail" , 2);
	zoptions->ReadReal("VIDEO", "zVidBrightness" , 0.5);
	zoptions->ReadReal("VIDEO", "zVidContrast"   , 0.5);
	zoptions->ReadReal("VIDEO", "zVidGamma"      , 0.5);
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
