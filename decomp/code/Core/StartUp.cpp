void zUlfi_StartUp()
{
	zfontman = new zCFontMan();
	zcon.SetPos(zcon, 0, 0);
	zcon.SetDim(zcon, 8191, 1500);
	zcon.SetAutoCompletion(zcon, 1);
	zCParser::enableParsing = zCOption::Parm(zoptions, "ZREPARSE");
}


void Trg_Init() { }

void oBert_Options()
{
	zSTRING parse;
	if ( zoptions->Parm("PARSE") )
		parse = zoptions->ParmValue( "PARSE" );

	zSTRING world;
	if ( zoptions->Parm("3D") )
		world = zoptions->ParmValue( "3D" );

	zSTRING player;
	if ( zoptions->Parm("PLAYER") )
		player = zoptions->ParmValue("PLAYER");

	if ( parse.Length() > 0 ) {
		auto file = zfactory->CreateZFile(parse);
		if ( file ) {
			auto ext = file->GetExt();
			if ( ext.Upper() == "SRC" )
				parse.DeleteRight(4);

			if ( parse.Length() ) {
				if ( zgameoptions ) {
					zgameoptions->WriteString(zOPT_SEC_FILES, "Game", parse, 1);
				} else {
					zoptions->WriteString(zOPT_SEC_INTERNAL, "gameScript", parse, 1);
				}
			}
			file->Close();
			delete file;
		}
	}
	if ( world.Length() ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString(zOPT_SEC_SETTINGS, "World", world, 1);
		} else {
			zoptions->WriteString(zOPT_SEC_INTERNAL, "gamePath", world, 1);
		}
	}
	if ( player.Length() ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString(zOPT_SEC_SETTINGS, "Player", player, 1);
		} else {
			zoptions->WriteString(zOPT_SEC_INTERNAL, "playerInstanceName", player, 1);
		}
	}

	gLogStatistics = zoptions->ReadBool(zOPT_SEC_INTERNAL, "logStatistics", 0);
}


void oBert_StartUp()
{
	zSTRING command;
	zSTRING description;

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
	zcon->AddEvalFunc(oBert_Console_EvalFunc);
	oBert_Options();
}


void zBert_StartUp()
{
	zCOption::ChangeDir(zoptions, 0);
	_control87(0x9001Fu, 0xFFFFu);

	zcon.Register("ZERR AUTHORS", "Set the author-filter for messages (as characters A-Z)");
	zcon.Register("ZERR SEARCHSPY", "Search for existing zSpy if started later than game.");
	zcon.Register("ZERR LEVEL", "Set the maximum priority-level for messages (from -1 to 10)");
	zcon.Register("ZERR STATUS", "Show error-status (current level, target ...)");
	zcon.Register("ZERR REM", "Include a remark into the error-log.");
	zcon.Register("ZERR ZVIEW", "just for internal tests");

	zcon.AddEvalFunc(zBert_Console_EvalFunc);
}

void oCarsten_StartUp()
{
	oCVisualFX::InitParser();

	zcon.Register("EDIT FX", "edits a special effect");
	zcon/AddEvalFunc(oCVisualFX::FxConsole_EvalFunc);

	zinput.BindKeys(1);
	zinput.BindKeys(0);

	zCExceptionHandler::AddUnhandledExceptionInfoCallback(ExceptionInfoCallback);

	zcon.Register("FIRSTPERSON", "EXPERIMENTAL FIRST PERSON CAMERA");
	zcon.Register("GOTO CAMERA", "SETS THE PLAYER POSITION TO THE CAMERA POSITION");
	zcon.Register("GOTO POS", "SETS THE PLAYER POSITION TO THE GIVEN POSITION (X,Y,Z)");
	zcon.Register("SPAWNMASS", "spawns max [num] random monsters around the player");
	zcon.Register("SPAWNMASS GIGA", "spawns max [num] random giga monsters around the player");
	zcon.Register("DEBUG DAMAGE", "enables/disabled (0/1) damage debug output");
	zcon.Register("REPARSE", "reparses game scripts, warning: you have to know what you are doing (you are probably not)"
	zcon.Register("PLAY FX", "plays a special fx"
	zcon.Register("STOP FX", "stops the previously played fx");

	zCConsole::AddEvalFunc(&zcon, oCarsten_Console_EvalFunc);

	s_bCheckRoutines = zoptions.Parm("CHECKROUTINES");

	oCNpcFocus::s_iHightlightMeleeFocus = zoptions.ReadInt("GAME", "highlightMeleeFocus", 0);
	oCNpcFocus::s_bHighlightInteractFocus = zoptions::ReadBool("GAME", "highlightInteractFocus", 0);

	oCGame::s_fUseSpeechReverb = zoptions.ReadReal("GAME", "useSpeechReverbLevel", 1.0);
	oCGame::s_bUsePotionKeys = zoptions.ReadBool("GAME", "usePotionKeys", 0);
	oCGame::s_bUseQuickSave = zoptions.ReadBool("GAME", "useQuickSaveKeys", 0);
	oCItem::s_fGlobalItemZBiasScale = zoptions.ReadReal("ENGINE", "zInventoryItemsDistanceScale", 1.3);
	oCItem::s_bItemEffectEnabled = zoptions::ReadBool("GAME", "itemEffects", 1);

	if ( zrenderer ) {
		bool hasFog = zoptions.ReadBool(, "RENDERER_D3D", "zFogRadial", 1);
		zrenderer->SetRadialFog(hasFog);
	}
}

void zCarsten_StartUp(HWND* wnd)
{
	zoptions.InsertChangeHandler(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, zOpt_Sound_ChangeFXVol);
	zoptions.InsertChangeHandler(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, zOpt_Sound_ChangeMusicVol);
	zoptions.InsertChangeHandler(zOPT_SEC_VIDEO, "zVidBrightness", zOpt_Video_ChangeVidOptions);
	zoptions.InsertChangeHandler(zOPT_SEC_VIDEO, "zVidContrast", zOpt_Video_ChangeVidOptions);
	zoptions.InsertChangeHandler(zOPT_SEC_VIDEO, "zVidGamma", zOpt_Video_ChangeVidOptions);

	auto sfxVol = zoptions.ReadReal(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, 1.0);
	zoptions.WriteReal(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, sfxVol, 0);

	auto musVol = zoptions.ReadReal(zOPT_SEC_SOUND, ZOPT_SND_MUSIC_VOL, 0.8);
	zoptions.WriteReal(zOPT_SEC_SOUND, ZOPT_SND_MUSIC_VOL, musVol, 0);

	if ( zinput )
		delete zinput;

	zinput = new zCInput_Win32(wnd);

	zcon.Register("ZTOGGLE SHOWSPLINES", "Toggles camera splines ");
	zcon.Register("ZTOGGLE TIMEDEMO", "Toggles a time demo with avg FPS Rate");
	zcon.Register("ZTOGGLE RENDERORDER", "Renders Sky last (possible better z buffer performance)");
	zcon.Register("ZTOGGLE ENVMAP", "Toggled rendering of environmental fx");
	zcon.Register("ZTOGGLE AMBIENTVOBS", "Toggles rendering of ambient vobs");
	zcon.Register("ZTOGGLE AMBIENTPFX", "Toggles rendering of ambient pfx");
	zcon.Register("ZTOGGLE RENDERPORTALS", "Toggles rendering of all portals (spacer only)");
	zcon.Register("ZSET NEARCLIPZ", "sets the value for the near clipping plane");
	zcon.Register("ZTOGGLE FLUSHONAMBIENTCOL", "toggles flushing the renderer on a ambientcol change");

	zcon.AddEvalFunc(zCarsten_Console_EvalFunc);

	zCFFT::S_Init();
	zCPolygon::S_InitMorph();

	auto maxFps = zCTimer::S_GetMaxFPS();
	maxFps = zoptions.ReadDWord("ENGINE", "zMaxFPS", maxFps);
	zCTimer::S_SetMaxFPS(maxFps);

	zCParticleFX::s_bAmbientPFXEnabled = zoptions.ReadBool("ENGINE", "zAmbientPFXEnabled", 1);

	// original:
	// zCWorld::s_bEnvMappingEnabled = zoptions.ReadBool("ENGINE", "zAmbientVobsEnabled", 1);
	zCWorld::s_bAmbientVobsEnabled = zoptions.ReadBool("ENGINE", "zAmbientVobsEnabled", 1);
	zCWorld::s_bEnvMappingEnabled = zoptions.ReadBool("ENGINE", "zEnvMappingEnabled", 1);
	zCWorld::s_bAlternateRenderOrder = !zoptions.ReadBool("ENGINE", "zSkyRenderFirst", 1);
	zCAIPlayer::s_bShowWeaponTrails = zoptions.ReadBool("GAME", "zShowWeaponTrails", 1);

	ztimer.SetMaxFPS(zCTimer::S_GetMaxFPS());
}


void zDieter_StartUp(HWND* initContextHandle)
{
	Trg_Init();

	if ( zoptions->Parm("ZMAXFRAMERATE"))
		ztimer.LimitFPS(zoptions->ParmValue("ZMAXFRAMERATE").ToLong());

	zCMemPoolBase::DisablePools(zoptions->Parm("ZNOMEMPOOL"));

	int deviceOverride = -1;
	if ( zoptions->Parm("ZRND") ) {
		auto val = zoptions->ParmValue("ZRND");
		if (val.Search(0,"D3D",1) != -1) {
			zrenderer = new zCRnd_D3D; // 0x82E7C
			if ( val.Search(0, "1", 1u) != -1 )
				deviceOverride = 1;
		}
	} else if ( !zrenderer ) {
		zINFO("D: RENDERER: no renderer specified, trying D3D (dev0)");
		zrenderer = new zCRnd_D3D;
	}
	auto resX       = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenX", 800);
	auto resY       = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenY", 600);
	auto bpp        = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenBPP",16);
	auto deviceNo   = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidDevice", 0);
	auto gamma      = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidGamma", 0.5);
	auto contrast   = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidContrast", 0.5);
	auto brightness = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidBrightness", 0.5);

	if ( zoptions->Parm("ZRES") ) {
		auto val = zoptions->ParmValue("ZRES");
		resX = val.PickWord(1, ",", ",").ToLong();
		resY = val.PickWord(2, ",", ",").ToLong();
		bpp  = val.PickWord(3, ",", ",").ToLong();
	}
	if ( deviceOverride >= 0 )
		deviceNo = deviceOverride;

	auto windowed = zoptions->ReadBool(zOPT_SEC_VIDEO, "zStartupWindowed", 0);
	if ( zoptions->Parm("ZWINDOW") )
		windowed = 1;

	zrenderer->Vid_SetDevice(deviceNo);
	zrenderer->Vid_SetScreenMode(windowed != 0);
	zrenderer->Vid_SetMode(resX, resY, bpp, initContextHandle);
	zCOLOR clearcolor = 0;
	zrenderer->Vid_Clear(&clearcolor, 3);
	zrenderer->Vid_SetGammaCorrection(gamma, contrast, brightness);

	zCTexture::AutoDetectRendererTexScale();

	zCTexture::s_globalLoadTextures = zoptions->Parm("ZNOTEX") == 0;
	zCView::SetMode(resX, resY, bpp, initContextHandle);

	if ( zoptions->Parm("ZNOMUSIC") )
		zoptions->WriteBool(zOPT_SEC_SOUND, "musicEnabled", 0, 1);
	auto music = zoptions->ReadBool(zOPT_SEC_SOUND, "musicEnabled", 1);
	zCMusicSystem::DisableMusicSystem(!music);
	if ( !zmusic ) {
		if ( zoptions->Parm("ZNOMUSIC") ) {
			zmusic = new zCMusicSys_Dummy;
		} else {
			zmusic = new zCMusicSys_DirectMusic;
		}
	}

	if ( zoptions->Parm("ZNOSOUND") )
		zoptions->WriteBool(zOPT_SEC_SOUND, "soundEnabled", 0, 1);
	auto sound = zoptions->ReadBool(zOPT_SEC_SOUND, "soundEnabled", 1);
	if ( !zsound ) {
		if ( sound ) {
			zsound = new zCSndSys_MSS; // 0x14
		} else {
			zsound = new zCSoundSystemDummy;
		}
	}

	zsndMan = new zCSoundManager;



	zCModelPrototype::s_autoConvertAnis = convData;
	zCMesh::s_autoConvertMeshes = convData;
	zCMorphMeshProto::autoConvertBinaryFile = convData;

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
	zcon.Register("ZLIST", "list every living object of class [CLASS_NAME], if the class has SHARED_OBJECTS flag",
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

	zcon.AddEvalFunc(zDieter_EvalFunc);

	zrenderMan.InitRenderManager();

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
