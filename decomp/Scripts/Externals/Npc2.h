int Hlp_IsItem()
{
	auto parser = zCParser::GetParser();

	int instanceItem;
	parser->GetParameter(instanceItem);

	auto item = GetItem("Hlp_IsItem", 1);

	bool ret = false;
	if ( item )
		ret = item->GetInstance() == instanceItem;

	parser->SetReturn(ret);
	return 0;
}

int Hlp_GetInstanceID()
{
	auto parser = zCParser::GetParser();

	zCVob* vob = parser->GetInstance();

	zSTRING* name = 0;
	int id = -1;
	if ( vob )
		vob->GetScriptInstance(name, id);

	parser->SetReturn(id);
	return 0;
}

int Hlp_Random()
{
	int bound;
	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(bound);
	parser->SetReturn(rand() % bound);
	return 0;
}

int Hlp_StrCmp()
{
	auto parser = zCParser::GetParser();

	zSTRING text1;
	zSTRING text2;
	parser->GetParameter(text2);
	parser->GetParameter(text1);
	text1.Upper();
	text2.Upper();

	parser->SetReturn(text1 == text2);
	return 0;
}

int Hlp_IsValidNpc()
{
	auto parser = zCParser::GetParser();

	zCVob* inst = parser->GetInstance();
	oCNpc* npc = dynamic_cast<oCNpc>(inst);
	parser->SetReturn(inst != 0);
	return 0;
}

int Hlp_IsValidItem()
{
	auto parser = zCParser::GetParser();

	zCVob* inst = parser->GetInstance();
	oCItem* item = dynamic_cast<oCItem>(inst);
	parser->SetReturn(inst != 0);
	return 0;
}

int Snd_Play()
{
	auto parser = zCParser::GetParser();

	zSTRING nameSFX;
	parser->GetParser(nameSFX);
	Game_PlaySound(nameSFX);
	return 0;
}

int Snd_Play3D()
{
	auto parser = zCParser::GetParser();

	zSTRING nameSFX;
	parser->GetParser(nameSFX);
	Game_PlaySound(nameSFX);
	return 0;
}

void Game_PlaySound(zSTRING const& nameSFX)
{
	zCSoundFX* snd = zsound->LoadSoundFXScript(nameSFX);
	if (snd) {
		zsound->PlaySound3D(snd, other, 0, 0);
		snd->Release();
	}
}

int Snd_Play3D()
{
	auto parser = zCParser::GetParser();

	zSTRING nameSFX;
	parser->GetParser(nameSFX);

	int otherIndex;
	other = parser->GetInstanceAndIndex(otherIndex);
	if ( !other ) {
		auto sym = parser->GetSymbol(otherIndex);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("Snd_Play(): illegal params: vob-name: " + name + ", sndName: " + nameSFX);
	}

	Game_PlaySound(nameSFX);
	return 0;
}

int Snd_GetDistToSource()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Snd_GetDistToSource", 1);
	int dist;
	if ( npc )
		dist = npc->GetDistToSound();
	parser->SetReturn(dist);
	return 0;
}

int Snd_IsSourceNpc()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Snd_IsSourceNpc", 1);

	oCNpc* sourceNpc = nullptr;
	if ( npc ) {
		zCVob* source = npc->GetSoundSource();
		sourceNpc = dynamic_cast<oCNpc>(source);
	}

	parser->SetInstance("OTHER", sourceNpc);
	parser->SetReturn(sourceNpc != 0);
	return 0;
}

int Snd_IsSourceItem()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Snd_IsSourceItem", 1);

	oCNpc* sourceItem = nullptr;
	if ( npc ) {
		zCVob* source = npc->GetSoundSource();
		sourceItem = dynamic_cast<oCItem>(source);
	}

	parser->SetInstance("ITEM", sourceItem);
	parser->SetReturn(sourceItem != 0);
	return 0;
}

int Npc_CanSeeSource()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_CanSeeSource", 1);

	int ret = 0;
	if ( npc ) {
		zCVob* source = npc->GetSoundSource();
		if (source)
			ret = npc->CanSee(source, 0);
	}
	parser->SetReturn(ret);
	return 0;
}

int Mis_GetStatus()
{
	auto parser = zCParser::GetParser();

	int missionNr;
	parser->GetParameter(missionNr);

	int status = misMan->GetStatus(oCNpc::player, missionNr);
	if ( status == -1 )
		Script_Warning("Mis_GetStatus() failed for nr: " + zSTRING(missionNr));
	parser->SetReturn(status);
	return 0;
}

int Mis_SetStatus()
{
	int missionNr;
	int newStatus;

	auto parser = zCParser::GetParser();
	parser->GetParameter(newStatus);
	parser->GetParameter(missionNr);

	misMan->SetStatus(oCNpc::player, missionNr, newStatus);
	return 0;
}

int Log_CreateTopic()
{
	zSTRING name;
	int section;

	auto parser = zCParser::GetParser();

	auto logMan = oCLogManager::GetLogManager();

	parser->GetParameter(section);
	parser->GetParameter(name);
	for (auto i = logMan.topics->next; i; i = i->next) {
		if (i->data && i->data->description == name)
			break;
	}

	auto topic = new oCLogTopic(name);
	logMan->topics.Insert(topic);

	return 0;
}

int Log_AddEntry()
{
	zSTRING topic;
	zSTRING entry;

	auto parser = zCParser::GetParser();

	auto logMan = oCLogManager::GetLogManager();

	parser->GetParameter(topic);
	parser->GetParameter(entry);

	for (auto i = logMan.topic->next; i; i = i->next) {
		auto log = i->data;
		if (log && log->description == topic) {
			log->AddEntry(entry);
			break;
		}
	}

	return 0;
}

int Log_SetTopicStatus()
{
	zSTRING topic;
	int status;

	auto parser = zCParser::GetParser();

	auto logMan = oCLogManager::GetLogManager();

	parser->GetParameter(topic);
	parser->GetParameter(status);

	for (auto i = logMan.topic->next; i; i = i->next) {
		auto log = i->data;
		if (log && log->description == topic) {
			topic->status = status;
			break;
		}
	}

	return 0;
}

int Mis_AddMissionEntry()
{
	int missionInst;
	zSTRING entry;

	auto parser = zCParser::GetParser();

	parser->GetParameter(entry);
	parser->GetParameter(missionInst);

	auto mission = misMan->GetMission(missionInst);
	if ( mission ) {
		mission->AddEntry(entry);
		mission->AddEntry(entry);
	} else {
		auto sym = parser->GetSymbol(missionInst);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("S: Mission " + name + " not found !");
	}
	return 0;
}

int Mis_RemoveMission()
{
	int missionInst;

	auto parser = zCParser::GetParser();

	parser->GetParameter(missionInst);

	auto mission = misMan->GetMission(missionInst);
	if ( mission ) {
		mission->mis[31] = 1;
	} else {
		auto sym = parser->GetSymbol(missionInst);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("S: Mission " + name + " not found !");
	}
	return 0;
}

int Npc_CheckAvailableMission()
{
	int state;
	int important = 0;

	auto parser = zCParser::GetParser();

	parser->GetParameter(important);
	parser->GetParameter(state);

	auto npc = GetNpc("Npc_CheckAvailableMissions", 1);

	int ret = 0
	if ( npc ) {
		oCNpc* player = (oCNpc*)(oCGame::GetSelfPlayerVob());
		auto mission = misMan.GetAvailableMission(npc, player, important);
		if ( mission ) {
			int misStatus = mission->GetStatus();
			ret = misStatus == state;
		} else {
			zSTRING name = npc->GetName(0);
			Script_Warning("Npc_CheckAvailableMission(): illegal mission: nsc-name: " + name);
		}
	}
	parser->SetReturn(ret);
	return 0;
}


int Npc_IsDrawingWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_IsDrawingWeapon", 1);

	bool ret = false;
	auto em = npc->GetEM();
	for (int i = 0, num = em->GetNumMessages(); i < num; ++i) {
		auto msg = em->GetEventMessage(i);
		auto msgWeap  = zDYNAMIC_CAST<oCMsgWeapon>(msg);
		auto msgMagic = zDYNAMIC_CAST<oCMsgMagic>(msg);
		if (msgWeap  && msgWeap->subType < 3 ||
		    msgMagic && msgMagic->subType == 9) {
			ret = true;
			break;
		}
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_IsDrawingSpell()
{
	auto parser = zCParser::GetParser();

	int ret = -1;
	auto npc = GetNpc("Npc_IsDrawingSpell", 1);
	if ( npc ) {
		for (int i = 0, num = em->GetNumMessages(); i < num; ++i) {
			auto msg = em->GetEventMessage(i);
			auto msgMagic = zDYNAMIC_CAST<oCMsgMagic>(msg);
			if (msgMagic && msgMagic->subType == 9) {
				ret = msgMagic->spellId;
				break;
			}
		}
	}
	parser->SetReturn(ret);
	return 0;
}


int Npc_CheckRunningMission()
{
	int important;

	auto parser = zCParser::GetParser();

	parser->GetParameter(important);

	auto npc = GetNpc("Npc_CheckRunningMission", 1);

	int ret = 0;

	if ( npc ) {
		oCNpc* player = (oCNpc*)(oCGame::GetSelfPlayerVob());

		auto mission = misMan.GetAvailableMission(npc, player, important);
		if ( mission ) {
			if ( mission->GetStatus() )
				ret = mission->CheckMission();
		} else {
			zSTRING name = npc->GetName(0);
			Script_Warning("Npc_CheckRunningMission(): illegal mission: nsc-name: " + name);
		}
	}
	parser->SetReturn(ret);
	return 0;
}


int Npc_CheckOfferMission()
{
	int important;

	auto parser = zCParser::GetParser();

	parser->GetParameter(important);

	auto npc = GetNpc("Npc_CheckOfferMission", 1);

	bool ret = false;
	if ( npc ) {
		oCNpc* player = (oCNpc*)(oCGame::GetSelfPlayerVob());

		auto mission = misMan.GetAvailableMission(npc, player, important);
		if ( mission ) {
			if ( mission->GetStatus() ) {
				mission->Offer()
				ret = true;
			}
		} else {
			zSTRING name = npc->GetName(0);
			Script_Warning("Npc_CheckOfferMission(): illegal mission: nsc-name: " + name);
		}
	}
	parser->SetReturn(ret);
	return 0;
}

int Mdl_SetVisual()
{
	int fileMDS;

	auto parser = zCParser::GetParser();

	parser->GetParameter(fileMDS);

	auto npc = GetNpc("Mdl_SetVisual", 1); // "Npc_CanSeeSource" originally

	if ( npc ) {
		npc->mds_name = fileMDS;
	} else {
		Script_Warning("... visual: " + fileMDS);
	}
	return 0;
}

int Mdl_SetVisualBody()
{
	zSTRING bodyMesh;
	int bodyTex;
	int skinColor;
	zSTRING headMMS;
	int headTex;
	int toothTex;
	int armor;

	auto parser = zCParser::GetParser();

	parser.GetParameter(armor);
	parser.GetParameter(toothTex);
	parser.GetParameter(headTex);
	parser.GetParameter(headMMS);
	parser.GetParameter(skinColor);
	parser.GetParameter(bodyTex);
	parser.GetParameter(bodyMesh);

	auto npc = GetNpc("Mdl_SetVisualBody", 1);
	if ( npc ) {
		npc->SetAdditionalVisuals(bodyMesh, bodyTex, skinColor, headMMS, headTex, toothTex, armor);
	} else {
		auto sym = zparser->GetSymbol(armor);
		if ( sym ) {
			Script_Warning("... armor: " + name);
		} else {
			Script_Warning("... armor: [UNKNOWN]");
		}
	}
	return 0;
}

int Mdl_ApplyOverlayMds()
{
	auto parser = zCParser::GetParser();

	zSTRING fileMDS;

	parser->GetParameter(fileMDS);

	auto npc = GetNpc("Mdl_ApplyOverlayMds", 0);
	if ( npc ) {
		npc->ApplyOverlay(fileMDS);
		if ( npc->GetAnictrl() )
			npc->GetAnictrl()->InitAnimations();
	} else {
		Script_Warning("... ani: " + fileMDS);
	}
	return 0;
}

int Mdl_ApplyOverlayMdsTimed()
{
	auto parser = zCParser::GetParser();

	zSTRING fileMDS;
	int time;

	parser->GetParameter(time);
	parser->GetParameter(fileMDS);

	auto npc = GetNpc("Mdl_ApplyOverlayMdsTimed", 0);
	if ( npc ) {
		npc->ApplyTimedOverlayMds(fileMDS, time);
	} else {
		Script_Warning("... overlay: " + fileMDS);
	}
	return 0;
}

int Mdl_RemoveOverlayMds()
{
	auto parser = zCParser::GetParser();

	zSTRING fileMDS;

	parser->GetParameter(fileMDS);

	auto npc = GetNpc("Mdl_RemoveOverlayMds", 0);
	if ( npc ) {
		npc->RemoveOverlay(fileMDS);
		if ( npc->GetAnictrl() )
			npc->GetAnictrl()->InitAnimations();
	} else {
		Script_Warning("... overlay: " + fileMDS);
	}
	return 0;
}

int Mdl_ApplyRandomAni()
{
	auto parser = zCParser::GetParser();

	zSTRING alias;
	zSTRING ani;

	parser->GetParameter(alias);
	parser->GetParameter(aniname);
	alias.Upper();
	ani.Upper();

	auto npc = GetNpc("Mdl_ApplyRandomAni", 0);
	if ( npc ) {
		if ( npc->GetModel() ) {
			auto prototype = npc->GetModel()->prototypes[0];
			auto ani = prototype->SearchAni(aniname);

			aniId = ani ? ani->aniId : -1;
			
			auto aniId2 = npc->GetModel()->GetAniIDFromAniName(alias);
			if ( aniId && aniId2 ) {
				npc->GetModel()->InsertRandAni(aniId, aniId2, 1);
			} else {
				Script_Warning("C: Mdl_ApplyRandomAni(): could not apply random ani with name " + ani + ". name2: " + alias);
			}
		}
	} else {
		Script_Warning("... ani: " + ani + ". ani2: " + alias);
	}
	return 0;
}

int Mdl_ApplyRandomAniFreq()
{
	zSTRING aniname;
	float freq;

	auto parser = zCParser::GetParser();

	parser->GetParameter(freq);
	parser->GetParameter(aniname);
	aniname.Upper();

	auto npc = GetNpc("Mdl_ApplyRandomAniFreq", 0);

	if ( npc ) {
		if ( npc->GetModel() ) {
			auto prototype = oCNpc::GetModel(npc)->prototypes[0];
			auto ani = prototype->SearchAni(aniname);
			if (ani && ani->aniId != -1) {
				npc->GetModel()->SetRandAniFreq(ani->aniId, freq);
			} else {
				zWARNING("C: Mdl_ApplyRandomAniFreq(): could not apply random ani with name " + ani);
			}
		}
	} else {
		Script_Warning("... ani: " + ani);
	}
	return 0;
}

int Mdl_SetModelScale()
{
	auto parser = zCParser::GetParser();

	float x, y, z;

	parser->GetParameter(z);
	parser->GetParameter(y);
	parser->GetParameter(x);

	auto npc = GetNpc("Mdl_SetModelScale", 1);
	if ( npc ) {
		zVEC3 scale(x, y, z);
		npc->SetModelScale(scale);
		if ( npc->GetAnictrl() )
			npc->GetAnictrl()->SetVob(npc);
	}
	return 0;
}

int Mdl_SetModelFatness()
{
	auto parser = zCParser::GetParser();

	float fatness;
	parser->GetParameter(fatness);

	auto npc = GetNpc("Mdl_SetModelFatness", 1);
	if ( npc )
		npc->SetFatness(fatness);
	return 0;
}

int Mdl_StartFaceAni()
{
	zSTRING aniName;
	float intensity;
	float holdTime;

	auto parser = zCParser::GetParser();

	parser->GetParameter(holdTime);
	parser->GetParameter(intensity);
	parser->GetParameter(aniName);

	auto npc = GetNpc("Mdl_StartFaceAni", 0);
	if ( npc ) {
		npc->StartFaceAni(aniName, intensity, holdTime);
	} else {
		Script_Warning("... face Ani: " + aniName);
	}
	return 0;
}


int Mdl_ApplyRandomFaceAni()
{
	zSTRING ani;
	float timeStart;
	float timeStartVar;
	float timeStop;
	float timeStopVar;
	float probability;

	auto parser = zCParser::GetParser();

	parser->GetParameter(probability);
	parser->GetParameter(timeStopVar);
	parser->GetParameter(timeStop);
	parser->GetParameter(timeStartVar);
	parser->GetParameter(timeStart);
	parser->GetParameter(ani);

	auto npc = GetNpc("Mdl_ApplyRandomFaceAni", 0);
	if ( npc ) {
		auto node = npc->GetModel->SearchNode(zMDL_NODE_NAME_HEAD);
		if ( node ) {
			auto mm = dynamic_cast<zCMorphMesh>(mm);
			if ( mm ) {
				auto anim = mm->SearchAni(ani);
				if ( anim )
					mm->AddRandAni(anim, timeStart, timeStartVar, timeStop, timeStopVar, probability);
			}
		}
	} else {
		Script_Warning("... face Ani: " + ani);
	}
	return 0;
}

int Wld_IsTime()
{
	int hourLow;
	int minLow;
	int hourHigh;
	int minHigh;

	auto parser = zCParser::GetParser();

	parser->GetParameter(minHigh);
	parser->GetParameter(hourHigh);
	parser->GetParameter(minLow);
	parser->GetParameter(hourLow);

	auto timer = ogame->GetWorldTimer();
	bool ret = timer->IsTimeBetween(hourLow, minLow, hourHigh, minHigh);
	parser->SetReturn(ret);
	return 0;
}

int Wld_GetDay()
{
	auto parser = zCParser::GetParser();

	int ret = ogame->GetWorldTimer()->GetDay();
	parser->SetReturn(ret);
	return 0;
}

int Wld_IsRaining()
{
	auto parser = zCParser::GetParser();

	auto activeCtrl = zCSkyControler::s_activeSkyControler;
	auto skyCtrl = dynamic_cast<zCSkyControler_Outdoor>(skyCtrl)
	bool ret = skyCtrl && skyCtrl->rainFX.outdoorRainFXWeight > 0.3;

	parser->SetReturn(ret);
	return 0;
}

int Wld_SetTime()
{
	int day;
	int hour;
	int min;

	auto parser = zCParser::GetParser();

	ogame->GetTime(day, hour, min);
	parser->GetParameter(min);
	parser->GetParameter(hour);
	ogame->SetTime(day, hour, min);
	return 0;
}

int Wld_SpawnNpcRange()
{
	int instanceNpc;
	int number;
	float distance;

	auto parser = zCParser::GetParser();

	auto spawnMgr = ogame->GetSpawnManager();

	parser.GetParameter(distance);
	parser.GetParameter(number);
	parser.GetParameter(instanceNpc);

	auto npc = GetNpc("Wld_SpawnNpcRange", 1);
	if (!npc)
		return 0;

	zVEC3 npcpos = npc->trafoObjToWorld.GetTranslation();
	zVEC3 atvec  = npc->trafoObjToWorld.GetAtVector();

	for (int i = 0; i < number; ++i) {
		zMAT4 spawnTrafo = npc->trafoObjToWorld;
		zVEC3 spawnpos   = npcpos + npcatvec * 150.0;

		float angle = 0.0;
		float dir   = 1.0;

		for (int j = 0; j < 20; ++j) {
			auto summon = spawnMgr->SummonNpc(instanceNpc, spawnpos, 0.0);
			if ( summon ) {
				summon->GetModel()->StartAni("T_SPAWN", 0);
				break;
			}

			float rnd = rand() * 0.000030518509 * distance + 100.0;
			zVEC3 offset = spawnTrafo.GetAtVector() * rnd;

			spawnTrafo.SetTranslation(npcpos + offset);

			float delta = dir * 18.0;

			spawnTrafo.PostRotateY(delta);

			angle += delta;
			if ( angle >= 90.0 ) {
				angle = -18.0;
				dir   = -1.0;

				spawnTrafo = npc->trafoObjToWorld;
				spawnTrafo.PostRotateY(-18.0);
			}

			spawnpos = spawnTrafo.GetTranslation();
		}
	}

	return 0;
}

int Wld_PlayEffect()
{
	zSTRING nameVfx;
	zCVob* origin;
	zCVob* target;
	int effectLevel;
	int damage;
	int damageType;
	int isProjectile;

	auto parser = zCParser::GetParser();

	parser.GetParameter(isProjectile);
	parser.GetParameter(damageType);
	parser.GetParameter(damage);
	parser.GetParameter(effectLevel);
	target = dynamic_cast<zCVob>(parser->GetInstance());
	origin = dynamic_cast<zCVob>(parser->GetInstance());
	parser->GetParameter(nameVfx);
	if ( origin ) {
		auto visFx = oCVisualFX::CreateAndPlay(
		                 nameVfx, origin, target,
		                 effectLevel, damage,
		                 damageType, isProjectile);
		if ( visFx )
			visFx->Release();`
	} else {
		Script_Warning("C: Wld_PlayEffect: Origin Script Instance is NULL!",);
	}
	return 0;
}

int Wld_StopEffect()
{
	auto parser = zCParser::GetParser();

	zSTRING nameVfx;
	parser->GetParameter(nameVfx);
	if ( oCNpc::player ) {
		zCArray<zCVob*> voblist;
		auto world = oCNpc::player->homeWorld;
		world->SearchVobListByClass(&oCVisualFX::classDef, voblist);

		if (voblist.IsEmpty())
			return 0;

		for (int i = 0; i < voblist.GetNumInList(); ++i) {
			auto visFx = zDYNAMIC_CAST<oCVisualFX>(voblist[i]);
			if (visFx && visFx->name == nameVfx) {
				visFx->Stop(1);
				break;
			}
		}
	}
	return 0;
}

int AI_PlayFX()
{
	oCNpc* npc;
	oCVob* target;
	zSTRING nameVfx;

	auto parser = zCParser::GetParser();

	auto inst = parser->GetInstance();
	target = zDYNAMIC_CAST(inst);
	npc = GetNpc("AI_PlayFX", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(22);
		msg->name = nameVfx;

		npc->GetEM()->OnMessage(msg, target);
	} else {
		Script_Warning("C: Wld_PlayEffect: Origin Script Instance is NULL!",);
	}
	return 0;
}

int AI_StopFX()
{
	oCNpc* npc;
	zSTRING nameVfx;

	auto parser = zCParser::GetParser();

	npc = GetNpc("AI_StopFX", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(23);
		msg->name = nameVfx;

		npc->GetEM()->OnMessage(msg, target);
	}
	return 0;
}

int Wld_RemoveNpc()
{
	int instanceNpc;

	auto parser = zCParser::GetParser();

	auto spawnMgr = ogame->GetSpawnManager();
	parser->GetParameter(instanceNpc);

	auto sym = parser->GetSymbol(instanceNpc);
	if ( sym && (sym->Type() == zPAR_TYPE_INSTANCE)) {
		auto world = ogame->GetWorld();
		auto vob   = world->SearchVobByName(sym->name);
		auto npc = dynamic_cast<oCNpc>(vob);

		if ( spawnMgr && npc ) {
			spawnMgr->DeleteNpc(npc);
		} else {
			Script_Warning("Wld_RemoveNpc(): npc could not be removed, no spawnmanager or instance is not an npc (name:" + sym-name + ")");
		}
	} else {
		Script_Warning("Wld_RemoveNpc(): npc could not be removed (illegal nsc ?)");
	}
	return 0;
}

int Wld_InsertNpcAndRespawn()
{
	int instanceNpc;
	int spawnPoint;
	int spawnDelay;

	auto parser = zCParser::GetParser();

	parser.GetParameter(spawnDelay);
	parser.GetParameter(spawnPoint);
	parser.GetParameter(instanceNpc);

	auto spawnMgr = ogame->GetSpawnManager();
	auto npc = spawnMgr->SpawnNpc(instanceNpc, &spawnPoint, 0.0);
	if ( npc ) {
		npc->bitfield0 |= 0x10;
		npc->spawnDelay = spawnDelay;
		if ( ogame && !ogame->inScriptStartup && npc->states.hasRoutine )
			rtnMan.UpdateSingleRoutine(npc);
	} else {
		auto sym = parser->GetSymbol(instanceNpc);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("Wld_InsertNpcAndRespawn(): npc could not be spawned (illegal nsc ? not enough place ?): spawnpoint: " + spawnPoint + ", npcName: " + name);
	}
	return 0;
}


int Wld_InsertItem()
{
	int instanceItem;
	int insertPoint;

	auto parser = zCParser::GetParser();

	parser.GetParameter(insertPoint);
	parser.GetParameter(instanceItem);

	auto world = ogame->GetGameWorld();
	auto vob = world->CreateVob(VOB_TYPE_ITEM, instanceItem);
	if ( vob ) {
		world = ogame->GetWorld();

		zVEC3 pos;
		zCWaypoint* wp = world->wayNet->GetWaypoint(insertPoint);
		if ( wp ) {
			pos = wp->GetPositionWorld();
		} else {
			auto posVob = world->SearchVobByName(insertPoint);
			if ( posVob ) {
				zMAT4 mat = posVob->trafoObjToWorld;
				pos = mat->GetTranslation();
			} else {
				zWARNING(&v24, "U:WLD: Position-Vob " + insertPoint + " not found.");
			}
		}
		vob->SetPositionWorld(pos);
		world->AddVob(vob);
		vob->Release();
	} else {
		auto sym = parser->GetSymbol(instanceItem);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("Wld_InsertItem(): item could not be inserted (illegal item ? not enough place ?): itemname: " + name + " ,position: " + insertPoint);
	}
	return 0;
}

int Wld_InsertObject()
{
	zSTRING objectName;
	zSTRING insertPoint;

	auto parser = zCParser::GetParser();

	parser.GetParameter(objectName);
	parser.GetParameter(insertPoint);

	auto world = ogame->GetWorld();
	do {
		auto wp = world->wayNet->GetWaypoint(wpName);
		if ( wp ) {
			auto pos = wp->GetPositionWorld();
			break;
		}

		auto posVob = world->SearchVobByName(insertPoint);
		if ( posVob ) {
			zMAT4 mat = posVob->trafoObjToWorld;
			pos = mat->GetTranslation();
			break;
		}

		Script_Warning("Wld_InsertObject(): Spawnpoint " + wpName + " not found. Object " + objName + " cannot be spawned.");
		return 0;
	} while (0);

	auto mob = new oCMob;
	mob->SetVisual(objName);
	mob->SetCollDetStat(0);
	mob->SetCollDetDyn(0);
	oCNpc::player->homeWorld->AddVob(mob);
	mob->SetOnFloor(pos);
	mob->SetCollDetStat(1);
	mob->SetCollDetDyn(1);
	mob->SetPhysicsEnabled(1);
	mob->SetSleeping(1);
	mob->Release();
	return 0;
}

int Wld_RemoveItem()
{
	auto parser = zCParser::GetParser();

	bool ret = 0;
	auto inst = parser->GetInstance();
	auto item = dynamic_cast<oCItem>(inst);
	if ( item ) {
		auto list = ogame->GetGameWorld()->voblist_items;
		for (auto i = list->next; i; i = i->next) {
			if (i->data == item) {
				ogame->GetGameWorld()->RemoveVob(item);
				ret = 1;
				break;
			}
		}
	}
	parser->SetReturn(ret);
	return 0;
}

int Wld_ExchangeGuildAttitudes()
{
	zSTRING tableName;

	auto item = dynamic_cast<oCItem>(inst);

	parser->GetParameter(tableName);

	auto guilds = ogame->GetGuilds();
	guilds->InitGuildTable(tableName);
	ogame->InitNpcAttitudes();
	return 0;
}

int Wld_SetObjectRoutine()
{
	zSTRING name;
	int hour;
	int min;
	int state;

	auto parser = zCParser::GetParser();

	parser->GetParameter(state);
	parser->GetParameter(name);
	parser->GetParameter(min);
	parser->GetParameter(hour);

	auto world = ogame->GetGameWorld();
	if ( !world->SearchVobByName(name) )
		Script_Warning("Wld_SetObjectRoutine(): could not find a vob with name: " + name);

	ogame->InsertObjectRoutine(1, name, hour, min, state);
	return 0;
}

int Wld_SetMobRoutine()
{
	zSTRING name;
	int hour;
	int min;
	int state;

	auto parser = zCParser::GetParser();

	parser->GetParameter(state);
	parser->GetParameter(name);
	parser->GetParameter(min);
	parser->GetParameter(hour);

	auto world = ogame->GetGameWorld();
	if ( !world->SearchVobByName(name) )
		Script_Warning("Wld_SetMobRoutine(): could not find a vob with name: " + name);

	ogame->InsertObjectRoutine(0, name, hour, min, state);
	return 0;
}

int Npc_OwnedByNpc()
{
	bool ret = false;

	auto parser = zCParser::GetParser();

	auto npc  = GetNpc("Npc_OwnedByNpc", 1);
	auto item = GetItem("Npc_OwnedByNpc", 1);
	if ( npc && item ) {
		int npcInstance = npc->GetInstance();
		ret = item->IsOwnedByNpc(npcInstance);
	}

	parser->SetReturn(ret);
	return 0;
}

int Npc_OwnedByGuild()
{
	bool ret = false;

	int guild;

	auto parser = zCParser::GetParser();

	auto item = GetItem("Npc_OwnedByGuild", 1);
	parser->GetParameter(guild);

	if ( item )
		ret = item->IsOwnedByGuild(guild);

	parser->SetReturn(ret);
	return 0;
}

int AI_TakeMob()
{
	oCNpc* npc;
	zSTRING mobName;

	auto parser = zCParser::GetParser();

	parser->GetParser(mobName);
	npc = GetNpc("AI_TakeMob", 0);

	if ( npc ) {
		auto msg = new oCMsgManipulate(16, mobName, 0);
		npc->GetEM()->OnMessage(msg, 0);
	} else {
		Script_Warning("... mobSchemeName: " + mobName);
	}
	return 0;
}

int AI_DropMob()
{
	oCNpc* npc;

	auto parser = zCParser::GetParser();

	npc = GetNpc("AI_DropMob", 1);
	if ( npc ) {
		auto msg = new oCMsgManipulate(17, "", 0);
		npc->GetEM()->OnMessage(msg, 0);
	}
	return 0;
}

int AI_MobInteract()
{
	oCNpc* npc;
	zSTRING mobName;
	int state;

	auto parser = zCParser::GetParser();

	parser->GetParameter(state);
	parser->GetParameter(mobName);

	auto npc = GetNpc("AI_MobInteract", 0);

	if ( npc ) {
		auto msg = new oCMsgManipulate(4, mobName, state);
		npc->GetEM()->OnMessage(msg, 0);
	} else {
		Script_Warning("... mobSchemeName: " + mobName);
	}
	parser->SetReturn(0);
	return 1;
}

int Mob_CreateItems()
{
	zSTRING mobName;
	int itemInstance;
	int amount;

	auto parser = zCParser::GetParser();

	parser->GetParameter(amount);
	parser->GetParameter(itemInstance);
	parser->GetParameter(mobName);

	auto world = ogame->GetWorld();

	auto vob = world->SearchVobByName(mobName);
	auto container = dynamic_cast<oCMobContainer>(vob);
	if ( !container ) {
		zSTRING msg = "Mob_CreateItems() : No Mob found with specified Vobname : " + mobName;
		Script_Warning(msg, 1);
		return 0;
	}

	for ( i = 0; i < amount; ++i ) {
		auto vob = world->CreateVob(VOB_TYPE_ITEM, mobName);
		auto item = dynamic_cast<oCItem>(vob);
		if ( item ) {
			container->Insert(item);
			if ( item->MultiSlot() ) {
				item->amount = amount;
				break;
			}
			item->Release();
		} else {
			zSTRING msg = "Mob_CreateItems() : No instance found, mobName: " + mobName;
			auto sym = parser->GetSymbol(itemInstance);
			if ( sym ) {
				Script_Warning(msg + ", instance-name:" + sym->name);
			} else {
				Script_Warning(msg + ", illegal instance");
			}
		}
	}
	return 0;
}

int Mob_HasItems()
{
	int ret = 0;

	zSTRING mobName;
	int itemInstance;

	auto parser = zCParser::GetParser();

	parser->GetParameter(itemInstance);
	parser->GetParameter(mobName);

	auto world = ogame->GetWorld();
	auto vob = world->SearchVobByName(mobName);
	auto mobInter = dynamic_cast<oCMobInter>(vob);
	if ( mobInter ) {
		ret = mobInter->IsIn(itemInstance);
	} else {
		Script_Warning("Mob_HasItems() : No Mob found with specified Vobname : " + mobName);
		ret = itemInstance; //wtf?
	}
	parser->SetReturn(ret);
	return 0;
}

int Doc_Create()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	auto doc = docMan->CreateDocument();

	zparser->SetReturn(doc);
	return 0;
}

int Doc_CreateMap()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	auto doc = docMan->CreateMap();

	zparser->SetReturn(doc);
	return 0;
}

int Doc_SetPages()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle = -1;
	int pages  = -1;

	auto parser = zCParser::GetParser();

	parser->GetParameter(pages);
	parser->GetParameter(handle);

	docMan->SetPages(handle, pages);
	return 0;
}

int Doc_SetPage()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle;
	int page;
	zSTRING pageImageTGA;
	int scale;

	auto parser = zCParser::GetParser();

	parser->GetParameter(scale);
	parser->GetParameter(pageImageTGA);
	parser->GetParameter(page);
	parser->GetParameter(handle);

	docMan->SetPage(andle, page, pageImageTGA, scale);
	return 0;
}

int Doc_SetFont()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle;
	int page;
	zSTRING fontTGA;

	auto parser = zCParser::GetParser();

	parser->GetParameter(fontTGA);
	parser->GetParameter(page);
	parser->GetParameter(handle);

	docMan->SetFont(handle, page, fontTGA);
	return 0;
}

int Doc_SetLevel()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle;
	zSTRING worldZEN;

	auto parser = zCParser::GetParser();

	parser->GetParameter(worldZEN);
	parser->GetParameter(handle);

	docMan->SetLevel(handle, worldZEN);
	return 0;
}

int Doc_SetLevelCoords()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle = -1;
	int left   = 0;
	int top    = 0;
	int right  = 0;
	int bottom = 0;

	auto parser = zCParser::GetParser();

	parser->GetParameter(bottom);
	parser->GetParameter(right);
	parser->GetParameter(top);
	parser->GetParameter(left);
	parser->GetParameter(handle);

	docMan->SetLevelCoords(handle, left, top, right, bottom);
	return 0;
}

int Doc_SetMargins()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle = -1;
	int page   = -1;
	int top    = -1;
	int left   = -1;
	int right  = -1;
	int bottom = -1;
	int pixel  =  1;

	parser->GetParameter(pixel);
	parser->GetParameter(bottom);
	parser->GetParameter(right);
	parser->GetParameter(top);
	parser->GetParameter(left);
	parser->GetParameter(page);
	parser->GetParameter(handle);

	auto parser = zCParser::GetParser();

	zCPosition tl(left, top);
	zCPosition br(right, bottom);

	docMan->SetMargins(handle, page, tl, br, pixel);
	return 0;
}

int Doc_PrintLine()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle = -1;
	int page   = -1;
	zSTRING text;

	auto parser = zCParser::GetParser();

	parser->GetParameter(text);
	parser->GetParameter(page);
	parser->GetParameter(handle);

	docMan->PrintLine(handle, page, &text);
	return 0;
}

int Doc_PrintLines()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int page   = -1;
	int handle = -1;
	zSTRING text;

	parser->GetParameter(text);
	parser->GetParameter(page);
	parser->GetParameter(handle);

	docMan->PrintLines(handle, page, text);
	return 0;
}

int Doc_Show()
{
	static auto docMan = oCDocumentManager::GetDocumentManager();

	int handle;

	auto parser = zCParser::GetParser();

	parser->GetParameter(handle);

	docMan->Show(handle);
	return 0;
}

int Doc_Open()
{
	zSTRING picName;

	auto parser = zCParser::GetParser();

	parser->GetParameter(picName);

	if ( !picName.IsEmpty() ) {
		if ( document )
			delete document;

		document = new oCDoc(picName);
		document->Show();
	} else {
		Script_Warning("Dop_Open(): invalid param!");
	}
	return 0;
}

int Doc_Font()
{
	zSTRING fontName;

	auto parser = zCParser::GetParser();

	parser->GetParameter(fontName);

	if ( document ) {
		document->SetFont(fontName);
	} else {
		Script_Warning("Doc_Font(): no document to set font: " + fontName);
	}
	return 0;
}

int Doc_Print()
{
	zSTRING text;

	auto parser = zCParser::GetParser();

	parser->GetParameter(text);
	if ( document ) {
		document->Printwin(text + "\n");
	} else {
		Script_Warning("Doc_Print(): no document to print text: " + text);
	}
	return 0;
}

int Doc_MapCoordinates()
{
	zSTRING levelName;
	float leftLevel;
	float topLevel;
	float leftMap;
	float topMap;
	float rightLevel;
	float bottomLevel;
	float rightMap;
	float bottomMap;

	auto parser = zCParser::GetParser();

	parser->GetParameter(bottomMap);
	parser->GetParameter(rightMap);
	parser->GetParameter(bottomLevel);
	parser->GetParameter(rightLevel);
	parser->GetParameter(topMap);
	parser->GetParameter(leftMap);
	parser->GetParameter(topLevel);
	parser->GetParameter(leftLevel);
	parser->GetParameter(levelName);

	if ( document ) {
		document->MapInitCoordinates(
		                leftLevel, topLevel,
		                leftMap, topMap,
		                rightLevel, bottomLevel,
		                rightMap, bottomMap);
		document->SetLevelName(levelName);
		player = oCGame::GetSelfPlayerVob();

		if ( player ) {
			zVEC3 playerPos playerPos->trafoObjToWorld.GetTranslation();
			document->MapDrawCoordinates(playerPos.x, playerPos.z);
		}
	} else {
		Script_Warning("Doc_Mapcoordinates(): no document to set coords. levelname: " + levelName);
	}
	return 0;
}

int TA()
{
	int startHour;
	int stopHour;
	int state;
	zSTRING waypoint;

	auto parser = zCParser::GetParser();

	parser->GetParameter(waypoint);
	parser->GetParameter(state);
	parser->GetParameter(stopHour);
	parser->GetParameter(startHour);

	auto npc = GetNpc("TA", 0);

	if ( npc ) {
		npc->states->InsertRoutine(startHour, 0, stopHour, 0, state, waypoint, -1);
	} else {
		Script_Warning("... wp-name: " + waypoint);
	}
	return 0;
}

int TA_Min()
{
	int startHour;
	int startMin;
	int stopHour;
	int stopMin;
	int state;
	zSTRING waypoint;

	auto parser = zCParser::GetParser();

	parser->GetParameter(waypoint);
	parser->GetParameter(state);
	parser->GetParameter(stopMin);
	parser->GetParameter(stopHour);
	parser->GetParameter(startMin);
	parser->GetParameter(startHour);

	auto npc = GetNpc("TA_Min", 0);

	if ( npc ) {
		npc->states->InsertRoutine( startHour, startMin, stopHour, stopMin, state, waypoint, -1);
	} else {
		Script_Warning("... wp-name: " + waypoint);
	}
	return 0;
}

int TA_CS()
{
	zSTRING csName;
	zSTRING roleName;

	auto parser = zCParser::GetParser();

	parser->GetParameter(roleName);
	parser->GetParameter(csName);

	auto npc = GetNpc("TA_Min", 0);

	if ( npc ) {
		npc->states->InsertRoutineCS(csName, roleName);
	} else {
		Script_Warning("... cs-name: " + csName + ", role-name: " + roleName);
	}
	return 0;
}

int TA_BeginOverlay()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("TA_BeginOverlay", 0);
	if ( npc )
		npc->states->BeginInsertOverlayRoutine();
	return 0;
}

int TA_EndOverlay()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("TA_EndOverlay", 0);
	if ( npc )
		npc->states->StopInsertOverlayRoutine();
	return 0;
}

int TA_RemoveOverlay()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("TA_EndOverlay", 0);
	if ( npc )
		npc->states->RemoveOverlay();
	return 0;
}

int Npc_ExchangeRoutine()
{
	zSTRING routine;

	auto parser = zCParser::GetParser();

	parser->GetParameter(routine);

	auto npc = GetNpc("Npc_ExchangeRoutine", 0);
	if ( npc ) {
		Script_SaveNpcGlobals();
		npc->states->ChangeRoutine(routine);
		Script_RestoreNpcGlobals();
	} else {
		Script_Warning("... routineName: " + routine);
	}
	return 0;
}

int Rtn_Exchange()
{
	bool ret = false;

	zSTRING newRoutine;
	zSTRING oldRoutine;

	auto parser = zCParser::GetParser();

	parser->GetParameter(newRoutine);
	parser->GetParameter(oldRoutine);
	newRoutine.Upper();
	oldRoutine.Upper();

	auto voblist = ogame->GetGameWorld()->voblist_npcs;

	for (auto i = voblist->next; i; i = i->next) {
		auto npc = i->data;
		if ( npc && !npc->IsSelfPlayer() && npc->states.hasRoutine ) {
			auto states = npc->states;
			auto rtnname = states->GetRoutineName();
			if (!(rtnname.Search(0, oldRoutine, 1u) & 0x80000000)) {
				Script_SaveNpcGlobals();
				states->ChangeRoutine(newRoutine);
				Script_RestoreNpcGlobals();
				ret = true;
			}
		}
	}

	if ( !ret ) {
		Script_Warning("Rtn_Exchange(): no npc found with routine: " + oldRoutine);
	}
	return 0;
}

int Perc_SetRange()
{
	int percID;
	float range;

	auto parser = zCParser::GetParser();

	parser->GetParameter(range);
	parser->GetParameter(percID);
	oCNpc::SetPerceptionRange(percID, range);
	return 0;
}

int Npc_SetPercTime()
{
	float time;

	auto parser = zCParser::GetParser();

	parser->GetParameter(time);

	auto npc = GetNpc("Npc_SetPercTime", 1);
	if ( npc ) {
		npc->SetPerceptionTime(time * 1000.0);
	}
	return 0;
}

int Npc_PercEnable()
{
	int perc;
	int action;

	auto parser = zCParser::GetParser();

	parser->GetParameter(action);
	parser->GetParameter(perc);

	auto npc = GetNpc("Npc_SetPercTime", 1);
	if ( npc )
		npc->EnablePerception(perc, action);
	return 0;
}

int Npc_PercDisable()
{
	int perc;

	auto parser = zCParser::GetParser();

	parser->GetParameter(perc);

	auto npc = GetNpc("Npc_PercDisable", 1);
	if ( npc )
		npc->:DisablePerception(perc);
	return 0;
}

int Npc_SendPassivePerc()
{
	int perc;

	auto parser = zCParser::GetParser();
	
	auto offender = GetNpc("Npc_SendPassivePerc", 1);
	auto victim = GetNpc("Npc_SendPassivePerc", 1);

	parser->GetParameter(perc);

	auto npc = GetNpc("Npc_SendPassivePerc", 1);
	if ( npc ) {
		Script_SaveNpcGlobals();
		npc->CreatePassivePerception(perc, offender, victim);
		Script_RestoreNpcGlobals();
	}
	return 0;
}

int Npc_SendSinglePerc()
{
	int perc;

	auto parser = zCParser::GetParser();
	
	auto target = GetNpc("Npc_SendSinglePerc", 1);
	auto npc = GetNpc("Npc_SendSinglePerc", 1);
	if ( npc && target )
	{
		Script_SaveNpcGlobals();
		zparser.SetInstance("SELF", target);
		zparser.SetInstance("OTHER", npc);

		int func = target->GetPerceptionFunc(perc);
		if (func >= 0)
			target->states->StartAIState(func, 0, 0, 0.0, 0);
		Script_RestoreNpcGlobals();
	}
	return 0;
}

int Npc_PerceiveAll()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_PerceiveAll", 1);
	if ( npc )
		npc->PerceiveAll();
	return 0;
}

int Wld_AssignRoomToGuild()
{
	zSTRING room;
	int guild;

	auto parser = zCParser::GetParser();

	parser->GetParameter(guild);
	parser->GetParameter(room);

	auto roomMgr = ogame->GetPortalRoomManager();
	if ( roomMgr )
		roomMgr->AssignPortalToGuild(room, guild);
	return 0;
}

int Wld_AssignRoomToNpc()
{
	zSTRING room;

	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Wld_AssignRoomToNpc", 0);

	parser->GetParameter(room);
	if ( !npc ) {
		Script_Warning("... roomName: " + room);
		return 0;
	}

	auto roomMgr = ogame->GetPortalRoomManager();
	if ( roomMgr ) {
		auto npcInst = npc->GetInstanceName();
		roomMgr->AssignPortalToNpc(room, npcInst);
	}
	return 0;
}

int Wld_GetPlayerPortalOwner()
{
	auto parser = zCParser::GetParser();
	auto roomMgr = ogame->GetPortalRoomManager();

	if ( roomMgr ) {
		oCNpc* owner = roomMgr->GetCurPlayerPortalRoomOwner();

		auto voblist = ogame->GetGameWorld()->voblist_npcs;

		for (auto i = voblist->next; i; i = i->next) {
			auto npc = i->data;
			if ( npc == owner ) {
				parser->SetReturn(owner);
				return 0;
			}
		}
	}

	parser->SetReturn(nullptr);
	return 0;
}

int Wld_GetPlayerPortalGuild()
{
	int ret = -1;

	auto parser = zCParser::GetParser();
	auto roomMgr = ogame->GetPortalRoomManager();

	if ( roomMgr )
		ret = roomMgr->GetCurPlayerPortalRoomGuild();
	parser->SetReturn(ret);
	return 0;
}

int Wld_GetFormerPlayerPortalOwner()
{
	auto parser = zCParser::GetParser();
	auto roomMgr = ogame->GetPortalRoomManager();

	if ( roomMgr ) {
		oCNpc* owner = roomMgr->GetFormerPlayerPortalRoomOwner();

		auto voblist = ogame->GetGameWorld()->voblist_npcs;

		for (auto i = voblist->next; i; i = i->next) {
			auto npc = i->data;
			if ( npc == owner ) {
				parser->SetReturn(owner);
				return 0;
			}
		}
	}

	parser->SetReturn(nullptr);
	return 0;
}


int Wld_FormerPlayerPortalGuild()
{
	int ret = -1;

	auto parser = zCParser::GetParser();
	auto roomMgr = ogame->GetPortalRoomManager();

	if ( roomMgr )
		ret = roomMgr->GetFormerPlayerPortalRoomGuild();
	parser->SetReturn(ret);
	return 0;
}

int Npc_IsPlayerInMyRoom()
{
	bool ret = false;

	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_IsPlayerInMyRoom", 1);
	if ( npc ) {
		auto roomMgr = ogame->GetPortalRoomManager();
		if ( roomMgr )
			ret = roomMgr->IsPlayerInMyRoom(npc);
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_IsInPlayersRoom()
{
	bool ret = false;

	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_IsInPlayersRoom", 1);
	if ( npc )
		auto roomMgr = ogame->GetPortalRoomManager();
		if ( roomMgr )
			ret = oCPortalRoomManager::IsNSCInPlayerRoom(npc);
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_WasPlayerInMyRoom()
{
	bool ret = false;

	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_WasPlayerInMyRoom", 1);
	if ( npc ) {
		auto roomMgr = ogame->GetPortalRoomManager();
		if ( roomMgr )
			ret = roomMgr->WasPlayerInMyRoom(npc);
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetPortalOwner()
{
	auto parser = zCParser::GetParser();
	auto roomMgr = ogame->GetPortalRoomManager();

	auto npc = GetNpc("Npc_GetPortalOwner", 1);

	if ( npc && roomMgr ) {
		oCNpc* owner = roomMgr->GetCurNpcPortalRoomOwner(npc);

		auto voblist = ogame->GetGameWorld()->voblist_npcs;

		for (auto i = voblist->next; i; i = i->next) {
			auto npc = i->data;
			if ( npc == owner ) {
				parser->SetReturn(owner);
				return 0;
			}
		}
	}

	parser->SetReturn(nullptr);
	return 0;
}

int Npc_GetPortalGuild()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetPortalGuild", 1);

	if ( npc ) {
		auto roomMgr = ogame->GetPortalRoomManager();
		if (roomMgr)
			oCNpc* owner = roomMgr->GetCurNpcPortalRoomGuild(npc);
	}

	parser->SetReturn(nullptr);
	return 0;
}
