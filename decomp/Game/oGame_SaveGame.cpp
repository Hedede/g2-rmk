// private
oCNpc* oCGame::RemovePlayerFromWorld()
{
	auto player = oCNpc::player;

	if ( player ) {
		AddRef(player->refCtr);
		player->groundPoly = 0;
		player->ClearPerceptionLists();


		GetGameWorld()->RemoveVob(player);
		oCNpc::player = 0;
	}

	return player;
}

void oCGame::InsertPlayerIntoWorld(oCNpc* npc, zMAT4& trafo)
{
	if ( npc && !npc->homeWorld ) {
		trafo.MakeOrthonormal();

		npc->groundPoly = 0;
		GetWorld()->AddVob(npc);

		npc->SetCollDetStat(0);
		npc->SetCollDetDyn(0);

		npc->SetTrafoObjToWorld(trafo);
		npc->SetOnFloor(trafo.GetTranslation());

		npc->SetCollDetStat(1);
		npc->SetCollDetDyn(1);

		oCNpc::player = npc;
	}
}

void oCGame::SaveWorld(zSTRING const& pwf, zCWorld::zTWorldSaveMode savemode, int savemesh, int saveBin)
{
	oCNpc* player;
	zMAT4 playerTrafo;

	if ( oCNpc::player ) {
		playerTrafo = oCNpc::player->trafoObjToWorld;
		player = RemovePlayerFromWorld();
	}

	if ( GetWorld() )
		GetWorld()->SaveWorld(pwf, savemode, savebin, savemesh);

	InsertPlayerIntoWorld(player, playerTrafo);
}

void oCGame::PreSaveGameProcessing(int saveGlobals)
{
	for (auto& npc : GetGameWorld()->voblist_npcs)
		npc->PreSaveGameProcessing();

	for (auto& vob : GetGameWorld()->voblist) {
		auto fire = zDYNAMIC_CAST<oCMobFire>(vob);
		if (fire)
			fire->PreSave();
	}

	oCVisualFX::PreSaveGameProcessing(!saveGlobals);
}

void oCGame::PostSaveGameProcessing()
{
	for (auto& npc : GetGameWorld()->voblist_npcs)
		npc->PostSaveGameProcessing();

	if ( !inLevelChange ) {
		for (auto& vob : GetGameWorld()->voblist) {
			auto fire = zDYNAMIC_CAST<oCMobFire>(vob);
			if (fire)
				fire->PostSave();
		}
	}

	oCVisualFX::PostSaveGameProcessing();
}

void oCGame::InitWorldSavegame(int& slotID, zSTRING& levelname)
{
	OpenLoadscreen(1, "");

	zINFO(5,"B: GAM: InitWorldSavegame"); // 2984,

	savegameManagero->CopyToCurrent(slotID);

	slotID = SAVEGAME_SLOT_CURRENT;
}

void oCGame::WriteSavegame(int slotnr, int saveGlobals)
{
	if ( saveGlobals )
		OpenSavescreen(1);

	if ( progressBar )
		progressBar->SetPercent(0, "");

	zoptions->ChangeDir(2);

	auto current = oCSavegameManager::GetSlotDirName(SAVEGAME_SLOT_CURRENT);
	auto slotDir = oCSavegameManager::GetSlotDirName(slotnr);

	if (GetWorld()) {
		if ( progressBar )
			progressBar->SetPercent(5, "");

		if ( slotnr >= SAVEGAME_SLOT_QUICK && slotnr <= SAVEGAME_SLOT_MAX )
			savegameManager->CopyFromCurrent(slotnr);

		if ( progressBar )
			progressBar->SetPercent(10, "");

		auto worldName = GetGameWorld()->GetWorldFilename();

		{
		auto file = zfactory->CreateZFile(slotDir + "savehdr.sav");
		file->Create();
		file->Write(worldName + "\n");
		delete file;
		}

		if ( progressBar )
			progressBar->SetPercent(12, "");

		zPATH wpath = {worldName};

		zSTRING worldFile = zoptions->GetDirString(DIR_SAVEGAMES);
		worldFile += slotDir + wpath.GetFilename() + "." + SAVEGAME_EXT;

		if ( progressBar )
			progressBar->SetRange(12, 92);

		PreSaveGameProcessing();

		GetWorld()->SaveWorld(worldFile, 0, 1, 0);

		PostSaveGameProcessing();

		if ( progressBar ) {
			// PROBABLY INLINED, WERENT UNDER SAME IF
			progressBar->ResetRange();
			progressBar->SetPercent(92, "");
		}

		SetCameraPosition();
	}

	if ( saveGlobals ) {
		if ( progressBar )
			progressBar->SetPercent(95, "");

		auto file = zoptions->GetDirString(DIR_SAVEGAMES);
		file += slotDir + SAVEGAME_GLOBAL_FILE;

		auto arc = zarcFactory->CreateArchiverWrite(file, 3, 1, 0);

		int hour, min;
		wldTimer->GetTime(hour, min);
		int day = wldTimer->GetDay();

		arc->WriteInt("day", day);
		arc->WriteInt("hour", hour);
		arc->WriteInt("min", min);

		infoman->Archive(*arc);
		misMan->Archive(*arc);
		oCLogManager::GetLogManager()->Archive(*arc);

		arc->WriteObject(GetCutsceneManager());

		zparser.SaveGlobalVars(*arc);
		guilds->SaveGuildTable(*arc);

		arc->Close();
		delete arc;

		if ( progressBar )
			progressBar->SetPercent(98, "");
	}

	SetCameraPosition();

	if ( gLogStatistics ) {
		auto dir = zoptions->GetDirString(DIR_SAVEGAMES) + slotDir;
		LogStatisticsWrite(dir);
	}
	if ( progressBar )
		progressBar->SetPercent(100, "");

	if ( saveGlobals )
		CloseSavescreen();
}

void oCGame::LoadGame(int slotID, zSTRING const& levelpath)
{
	ClearGameState();

	OpenLoadscreen(1, levelpath);

	if ( progressBar )
		progressBar->SetPercent(0, "");
	if ( progressBar )
		progressBar->SetRange( 0, 92);

	LoadWorld(slotID, levelpath);

	if ( progressBar )
		progressBar->ResetRange();
	if ( progressBar )
		progressBar->SetRange(92, 100);

	EnterWorld(0, 1, "");

	if ( progressBar )
		progressBar->ResetRange();

	if ( slotID == SAVEGAME_SLOT_NEW )
		InitNpcAttitudes();

	SetTime(0, initial_hour, initial_minute);

	if ( progressBar )
		progressBar->SetPercent(100, "");

	CloseLoadscreen();
}

void oCGame::LoadSavegame(int slotnr, int loadGlobals)
{
	ClearGameState();

	OpenLoadscreen(this, 1, "");

	if ( progressBar )
		progressBar->SetPercent(0, "");

	auto slotDir = oCSavegameManager::GetSlotDirName(slotnr);

	if ( progressBar )
		progressBar->SetRange(0, 5);

	zSTRING levelname;

	InitWorldSavegame(slotnr, levelname);

	if ( progressBar )
		progressBar->ResetRange();

	auto current = oCSavegameManager::GetSlotDirName(SAVEGAME_SLOT_CURRENT);

	auto savehdr = current + "savehdr.sav";

	auto file = zfactory->CreateZFile(savehdr);

	file->Open(0);
	file->read(levelname);

	delete file;

	if ( progressBar )
		progressBar->SetPercent(5, "");

	auto playerInfo = dynamic_cast<oCPlayerInfo>(zCPlayerInfo::GetActivePlayer());
	if ( playerInfo )
		playerInfo->SetPlayerVob(0);

	auto worldName = GetGameWorld()->worldFilename;

	bool deleteVisuals;
	if (worldFilename != "") {
		GetGameWorld()->DisposeWorld();
	} else {
		deleteVisuals = true;
		CacheVisualsIn();
		GetGameWorld()->DisposeVobs();
	}

	CheckObjectConsistency(1);

	if ( progressBar )
		progressBar->SetPercent(8, "");

	oCNpc::player = 0;

	int day, hour, min;
	wldTimer->GetTime(hour, min);

	zCArchiver* arc;
	if ( loadGlobals ) {
		auto dir = zoptions->GetDirString(DIR_SAVEGAMES);
		auto global = dir + slotDir + SAVEGAME_GLOBAL_FILE;

		arc = zCArchiverFactory::CreateArchiverRead(global, 0);

		arc->ReadInt("day", day);
		arc->ReadInt("hour", hour);
		arc->ReadInt("min", min);

		wldTimer->SetDay(day);
		wldTimer->SetTime(hour, min);
	}

	if ( progressBar )
		progressBar->SetRange(10, 92);

	LoadWorld(slotnr, levelname);

	if ( progressBar )
		progressBar->ResetRange();

	if ( deleteVisuals ) {
		for (auto visual : visualList)
			Release(visual);

		visualList.DeleteList();
	}

	if ( progressBar )
		progressBar->SetRange(92, 98);

	if ( !oCNpc::player )
		zFATAL("U: GAME: No player found. Did you try to load an incompatible savgame ?"); // 2245,

	EnterWorld(oCNpc::player, 0, "");

	if ( progressBar )
		progressBar->ResetRange();

	if ( loadGlobals ) {
		infoman->Unarchive(*arc);
		misMan.Unarchive(*arc);
		oCLogManager::GetLogManager()->Clear();
		oCLogManager::GetLogManager()->Unarchive(*arc);

		auto csMan = GetCutsceneManager();

		arc->ReadObject(csMan);
		zparser.LoadGlobalVars(*arc);
		guilds->LoadGuildTable(*arc);

		arc->Close();

		rtnMan.UpdateGlobalRoutineEntry();
	}

	CallScriptInit();

	if ( gLogStatistics )
		LogStatisticsRead(zoptions->GetDirString(DIR_SAVEGAMES) + slotDir);

	SetCameraPosition();

	for (auto& npc : GetGameWorld()->voblist_npcs) {
		if (npc && npc->homeWorld) { {
			if ( npc->IsDead() && npc->GetAnictrl() && npc->GetModel() ) {
				if ( npc->GetAnictrl()->actionMode == 5 ||
				     npc->GetAnictrl()->actionMode == 6 )
				{
					npc->GetModel()->StartAni("S_DROWNED", 0);
					npc->SetPhysicsEnabled(0);
				} else {
					rand();

					std::string::assign(&v95.data, , strlen("S_DEADB"));
					npc->GetModel()->StartAni("S_DEADB", 0);
					npc->GetModel()->AdvanceAnis();

					auto sa = npc->GetAnictrl()->flags2.zMV_DO_SURFACE_ALIGN;
					npc->GetAnictrl()->flags2.zMV_DO_SURFACE_ALIGN = 1;

					npc->GetAnictrl()->Begin(npc);
					npc->GetAnictrl()->End(npc);

					npc->GetAnictrl()->flags2.zMV_DO_SURFACE_ALIGN = sa;
				}
			} else {
				zCVob* vob = nullptr;
				zVEC3 pos = npc->trafoObjToWorld->GetTranslation();
				npc->GetFloorPositionWithVob(pos, vob);

				auto dyn = npc->GetCollDetDyn();
				auto stat = npc->GetCollDetStat();

				if ( vob ) {
					auto mob = zDYNAMIC_CAST<oCMobInter>(vob);
					if (mob && mob->GetFreePosition(npc, pos))
						npc->SearchNpcPosition(pos);
				}

				npc->SetPositionWorld(pos);
				npc->SetCollDetDyn(dyn)
				npc->SetCollDetStat(stat);
			}
		}
	}

	SetObjectRoutineTimeChange(0, 0, hour, min);

	if ( progressBar )
		progressBar->SetPercent(100, "");

	CloseLoadscreen();
}
