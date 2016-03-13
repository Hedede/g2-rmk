int Wld_SendTrigger()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING vobName;
	zCParser::GetParameter(&vobName);
	zCWorld* world = ogame->GetWorld();
	zCVob* vob = world->SearchVobByName(vobName);
	if ( vob )
	{
		vob.GetEM()->OnTrigger();
	}
	else
	{
		zSTRING msg = "Script tries to send trigger to " + vobName +
		              ". Failure : vob not found.";

		ScriptWarning(msg);
	}
	return 0;
}

int Wld_SendUntrigger()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING vobName;
	zCParser::GetParameter(&vobName);
	zCWorld* world = ogame->GetWorld();
	zCVob* vob = world->SearchVobByName(vobName);
	if ( vob )
	{
		vob.GetEM()->OnUntrigger();
	}
	else
	{
		zSTRING msg = "Script tries to send untrigger to " + vobName +
		              ". Failure : vob not found.";

		ScriptWarning(msg);
	}
	return 0;
}

void Warning_Wld_Detect(int aistate, int instance)
{
	auto sym = zparser->GetSymbol(aistate);
	zSTRING aistate_str = sym ? sym->name : "[UNKNOWN]";

	sym = zparser->GetSymbol(instance);
	zSTRING npc_name = sym ? sym->name : "[UNKNOWN]";

	ScriptWarning("... aiState: " + aistate_str +
			", npc-name: " + npc_name);
}

int Wld_DetectItem()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Wld_DetectItem";

	int flags;
	parser->GetParameter(&flags);
	oCNpc *npc = GetNpc(funcname, 1);

	oCItem* item = 0;
	if ( npc )
	{
		item = npc->DetectItem(flags, 0);
		zparser->SetInstance("ITEM", item);
	}
	parser->SetReturn(item != 0);
	return 0;
}

int Wld_DetectPlayer()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Wld_DetectPlayer";

	oCNpc *npc = GetNpc(funcname, 1);

	oCNpc* player = 0;
	if ( npc )
	{
		player = npc->DetectPlayer();
		zparser->SetInstance("OTHER", player);
	}
	parser->SetReturn(player != 0);
	return 0;
}

int Wld_DetectNpc()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Wld_DetectNpc";

	int instance;
	int guild;
	int aistate;

	parser->GetParameter(&guild);
	parser->GetParameter(&aistate);
	parser->GetParameter(&instance);
	oCNpc* npc = GetNpc(funcname, 0);
	oCNpc* other = nullptr;
	if ( npc )
	{
		other = npc->FindNpc(instance, guild, aistate, 1);
		if ( other )
		{
			parser->SetInstance("OTHER", other);
		}
	}
	else
	{
		Warning_Wld_Detect(aistate, instance);
	}
	parser->SetReturn(other != 0);
	return 0;
}

int Wld_DetectNpcEx()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Wld_DetectNpcEx";

	int instance;
	int guild;
	int aistate;
	int detectplayer;

	parser->GetParameter(&detectplayer);
	parser->GetParameter(&guild);
	parser->GetParameter(&aistate);
	parser->GetParameter(&instance);
	oCNpc* npc = GetNpc(funcname, 0);
	oCNpc* other = nullptr;
	if ( npc )
	{
		other = npc->FindNpcEx(instance, guild, aistate, 1, !detectplayer, 1);
		if ( other )
		{
			parser->SetInstance("OTHER", other);
		}
	}
	else
	{
		Warning_Wld_Detect(aistate, instance);
	}
	parser->SetReturn(other != 0);
	return 0;
}

int Wld_DetectNpcExAtt()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Wld_DetectNpcExAtt";

	int instance;
	int guild;
	int aistate;
	int detectplayer;
	int attitude;

	parser->GetParameter(&attitude);
	parser->GetParameter(&detectplayer);
	parser->GetParameter(&guild);
	parser->GetParameter(&aistate);
	parser->GetParameter(&instance);
	oCNpc* npc = GetNpc(funcname, 0);
	oCNpc* other = nullptr;
	if ( npc )
	{
		other = npc->FindNpcExAtt(instance, guild, aistate, 1, !detectplayer, 1, attitude);
		if ( other )
		{
			parser->SetInstance("OTHER", other);
		}
	}
	else
	{
		Warning_Wld_Detect(aistate, instance);

	}
	parser->SetReturn(other != 0);
	return 0;
}

// guilds
int Wld_SetGuildAttitude()
{
	zCParser* parser = zCParser::GetParser();

	int guild1;
	int guild2;
	int attitude;

	parser->GetParameter(&guild2);
	parser->GetParameter(&attitude);
	parser->GetParameter(&guild1);

	ogame->GetGuilds()->SetAttitude(guild1, guild2, attitude);
	return 0;
}

int Wld_SetGuildAttitude()
{
	zCParser* parser = zCParser::GetParser();

	int guild1;
	int guild2;

	parser->GetParameter(&guild2);
	parser->GetParameter(&guild1);

	int attitude = ogame->GetGuilds()->GetAttitude(guild1, guild2);
	parser->SetReturn(attitude);
	return 0;
}

// mob
int Wld_IsMobAvailable()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Wld_IsMobAvailable";

	zSTRING mobName;
	parser->GetParameter(&mobName);
	zSTRING::Upper(&mobName);

	oCNpc* npc = GetNpc(funcname, 0);
	if ( !npc )
	{
		ScriptWarning("... mobName: " + mobName);
	}

	int isAvail = 0;
	if ( npc )
	{
		oCMobInter* mob = npc->FindMobInter(mobName);
		if ( mob )
		{
			if ( mob->IsAvailable(npc) )
				isAvail = 1;
		}
	}
	parser->SetReturn(isAvail);
	return 1;
}

int Wld_IsFPAvailable()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Wld_IsFPAvailable";

	zSTRING fpName;
	parser->GetParameter(&fpName);
	zSTRING::Upper(&fpName);

	oCNpc* npc = GetNpc(funcname, 0);
	if ( !npc )
	{
		ScriptWarning("... fpName: " + fpName);
	}

	zCVobSpot* spot = nullptr;
	if ( npc )
	{
		spot = npc->FindSpot(fpName, 1, 700.0);
	}

	parser->SetReturn(spot != nullptr);
	return 1;
}

int Wld_IsNextFPAvailable()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Wld_IsNextFPAvailable";

	zSTRING fpName;
	parser->GetParameter(fpName);
	fpName.Upper();

	oCNpc* npc = GetNpc(funcname, 0);
	if ( !npc )
	{
		ScriptWarning("... fpName: " + fpName);
	}

	zCVobSpot* spot = nullptr;
	if ( npc )
	{
		spot = npc->FindSpot(fpName, 0, 700.0);
	}

	parser->SetReturn(spot != nullptr);
	return 1;
}

int Wld_InsertNpc()
{
	zCParser* parser = GetParser();

	int instanceNpc;
	zSTRING spawnPoint;

	parser->GetParameter(spawnPoint);
	parser->GetParameter(instanceNpc);

	instanceNpc = inst;
	oCSpawnManager* spawnManager = ogame->GetSpawnManager();
	npc = spawnManager->SpawnNpc(instanceNpc, spawnPoint, 0.0);
	if ( !npc )
	{
		auto* symbol = zparser->GetSymbol(instanceNpc);
		zSTRING name = symbol ? symbol->name : "[UNKNOWN]";

		ScriptWarning("Wld_InsertNpc(): called with illegal index. instance: " +name + ", position: " + spawnPoint, true);
		return 0;
	}

	npc->flags &= 0xFFFFFFEF;
	if ( !ogame->inScriptStartup && npc->states.hasRoutine )
		rtnMan.UpdateSingleRoutine(npc);
	return 0;
}
