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

int oBert_Console_EvalFunc(const zSTRING& cmdString, zSTRING& out)
{
	zSTRING word1 = cmdString.PickWord(1, " ", zSTR_SKIP);
	zSTRING word2 = cmdString.PickWord(2, " ", zSTR_SKIP);
	zSTRING word3 = cmdString.PickWord(3, " ", zSTR_SKIP);
	zSTRING word4 = cmdString.PickWord(4, " ", zSTR_SKIP);


	out = "ok";

	if (word1 == "SET") {
		if (word2 == "CLIPPINGFACTOR") {
			float factor = word3.ToFloat();
			
			if (factor < 0.05)
				factor = 0.05;
			if (factor > 10.0)
				factor = 10.0;

			zCWorld* world = ogame->GetWorld();
			if (world) {
				zCSkyControler skyctrl = world->GetActiveSkyControler();
				if (skyctrl)
					skyctrl->SetFarZScalability(factor);
			}
		}
		return 0;
	}
	if ( word1 == "LIST") {
		if (word2 == "CS") {
			zCWorld* world = ogame->GetWorld();

			world->csPlayer->PrintListOfCutscenes(word3);

			out = "toggled cutscene-debuginfos";
			return 1;
		}
		return 0;
	}
	if ( word1.data == "HERO" )
	{
		if ( word2.data == "EXPORT" && oCNpc::player ) {
			if ( word3.IsEmpty())
				word3 = "hero_export.zen";

			if ( word3.Search(0, ".zen", 1) < 0 )
				word3 += ".zen";

			zSTRING dir = zoptions->GetDirString(2);
			zFILE_FILE file(dir + word3);

			if ( file.Exists() )
				file.FileDelete();

			if ( file.Create() != 0 ) {
				out = "\"" + file.GetFullPath() + "\" could not be created.";
				return 0;
			}

			auto archiver = zarcFactory->CreateArchiverWrite(file, 1, 1, 0);

			archiver->WriteObject(oCNpc::player);
			archiver->Close()

			out = "Exported your hero to filee \"" + file.GetFullPath() + "\".";
			return 1;
		}
		if ( word2.data == "IMPORT" && oCNpc::player ) {
			if ( word3.IsEmpty())
				word3 = "hero_export.zen";

			if ( word3.Search(0, ".zen", 1) < 0 )
				word3 += ".zen";

			zSTRING dir = zoptions->GetDirString(2);
			zFILE_FILE file(dir + word3);

			if ( !file.Exists() || file->Open(0) != 0)
			{
				out = "\"" + file.GetFullPath() + "\" does not exist.";
				return 1;
			}

			auto* archiver = zCArchiverFactory::CreateArchiverRead(file, 0);
			zCObject* obj;
			archiver->ReadObject(obj);
			oCNpc* hero = dynamic_cast<oCNpc*>(obj);

			archiver->Close();

			if ( !hero || !ogame )
				return 1;

			oCNpc* player = oCNpc::player;

			zVEC3 translation;
			zVEC3 atvec;
			player->trafoObjToWorld.GetTranslation(translation);
			player->trafoObjToWorld.GetAtVector(atvec);

			player->RemoveVobFromWorld();
			hero->SetPositionWorld(trafoObjToWorld);
			hero->SetHeadingAtWorld(atvec);

			zCWorld* world = ogame->GetWorld();
			world->AddVob(hero);
			hero->SetAsPlayer();

			out = "Imported your hero from file \"" + file.GetFullPath() + "\".";
			return 1;
		}
		return 0;
	}
	if ( word1.data == "TOGGLE" )
	{
		if ( word2.data == "CUTSCENES" && word3.data == "ENABLED" )
		{
			zCCSCutsceneContext::DisablePlayCutscenes(dword_8C218C == 0);
			if ( dword_8C218C )
			{
				out = "Cutscene-System disabled ";
				zCWorld* world = ogame->GetWorld();
				world->csPlayer->StopAllCutscenes(0);
				return 1;
			}
			output = "Cutscene-System enabled";
			return 1;
		}
		if ( word2.data == "CS" && word3.data == "LIST" )
		{
			zCWorld* world = ogame->GetWorld();
			world->csPlayer->ToggleDebugInfo();
			output = "toggled cutscene-debuginfos";
		}
		return 0;
	}
	if ( word1.data == "SHOW" ) {
		if ( word2.data != "CS" )
			return 0;

		zINFO(1, "B: CS: Informations about cutscene \"" + word3 +"\"");
		zCCSManager* csman = ogame->GetCutsceneManager();

		int numplayed = csman->PoolNumPlayed(word3);
		if ( numplayed >= 0 ) {
			zINFO(1, "B: CS: CSPool: Has been started.");
			zINFO(1, "B: CS: CSPool: Has been played " + zSTRING(numplayed) + " times");
		}
		else
		{
			zINFO(1, "B: CS: CSPool: It wasn't played yet.");
		}

		zCCSCutsceneContext* cscontext = ogame->GetWorld()->csPlayer->GetStartedCutscene(word3);
		if ( cscontext ) {
			if ( cscontext->IsPlaying() )
				zINFO(1, "B: CS: CSPlayer: Cutscenes is playing right now.");
			else
				zINFO(1, "B: CS: CSPlayer: Cutscenes is sleeping.");
		}
		zINFO(1, "");
		return 1;
	}

	if ( word1 != "PLAY")
		return 0;

	if (word2 == "CS") {
		oCNpc* npc = oCNpc::player;
		if ( oCNpc::player->GetFocusNpc() )
			npc = oCNpc::player->GetFocusNpc();

		if ( npc )
		{
			auto msg = oCMsgConversation::Create(6, word3, npc->GetObjectName())
			npc->GetEM(0)->OnMessage(msg, npc);
			out = "Started playing cutscene for npc \"" + npc->GetObjectName() + "\"";
			return 1;
		}
	} else if ( word2 == "VIDEO") {
		gameMan->PlayVideo(gameMan, word3);
		return 1;
	}
	return 0;
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

