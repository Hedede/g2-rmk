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
