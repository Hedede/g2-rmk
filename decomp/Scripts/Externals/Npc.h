int Npc_GetGuildAttitude()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetGuildAttitude";
	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc = GetNpc(&funcname, 1);
	if ( npc && other ) {
		auto trueGuild = oCNpc::GetTrueGuild(other);
		auto attitude = oCNpc::GetGuildAttitude(npc, trueGuild);
		parser->SetReturn(attitude);
	}
	return 0;
}

// Memory
int Npc_MemoryEntry()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_MemoryEntry";
	oCNpc* npc;
	int source;
	oCNpc* offender;
	int newsId;
	oCNpc* victim;

	victim = GetNpc(funcname, 1);
	parser->GetParameter(&newsId);
	offender = GetNpc(&funcname, 1);
	parser->GetParameter(&source);
	npc = GetNpc(funcname, 1);

	ogame->GetNewsManager().CreateNews(newsId, source, npc, offender, victim, 0);
	return 0;
}

int Npc_MemoryEntryGuild()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_MemoryEntryGuild";
	oCNpc* npc;
	int source;
	oCNpc* offender;
	int newsId;
	oCNpc* victim;

	victim = GetNpc(funcname, 1);
	parser->GetParameter(&newsId);
	offender = GetNpc(&funcname, 1);
	parser->GetParameter(&source);
	npc = GetNpc(funcname, 1);

	ogame->GetNewsManager().CreateNews(newsId, source, npc, offender, victim, 1);
	return 0;
}

int Npc_HasNews()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_HasNews";
	oCNpc* victim = GetNpc(funcname, 0);
	oCNpc* offender = GetNpc(funcname, 0);
	int newsId;
	parser->GetParameter(&newsID);
	oCNpc* witness = GetNpc(funcname, 0);

	oCNews* news = 0;
	if ( victim && offender && witness) {
		news = witness->knownNews->SearchNews(newsID, offender, victim);
	}
	else
	{
		Script_Warning("... newsID: " + newsID, 1),
	}
	parser->SetReturn(news);
	return 0;
}


int Npc_GetNewsWitness()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetNewsWitness";

	uint32_t newsId;
	parser->GetParameter(&newsId);
	oCNpc* npc = GetNpc(funcname, 0);

	int id;
	int gossip;
	oCNpc* witness = 0;
	oCNpc* offender = 0;
	oCNpc* victim = 0;


	if ( npc )
	{
		auto news = npc->knownNews->GetNewsByNumber(newsId);
		if ( news )
			news->GetNewsData(news, id, gossip, witness, offender, victim);
	}
	else
	{
		Script_Warning(" ... newsnumber: " + newsId, 1);
	}
	parser->SetReturn(witness);
	return 0;
}

int Npc_GetNewsOffender()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetNewsOffender";

	uint32_t newsId;
	parser->GetParameter(&newsId);
	oCNpc* npc = GetNpc(funcname, 0);

	int id;
	int gossip;
	oCNpc* witness = 0;
	oCNpc* offender = 0;
	oCNpc* victim = 0;


	if ( npc )
	{
		auto news = npc->knownNews->GetNewsByNumber(newsId);
		if ( news )
			news->GetNewsData(news, id, gossip, witness, offender, victim);
	}
	else
	{
		Script_Warning(" ... newsnumber: " + newsId, 1);
	}
	parser->SetReturn(offender);
	return 0;
}

int Npc_GetNewsVictim()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetNewsVictim";

	uint32_t newsId;
	parser->GetParameter(&newsId);
	oCNpc* npc = GetNpc(funcname, 0);

	int id;
	int gossip;
	oCNpc* witness = 0;
	oCNpc* offender = 0;
	oCNpc* victim = 0;

	if ( npc )
	{
		auto news = npc->knownNews->GetNewsByNumber(newsId);
		if ( news )
			news->GetNewsData(news, id, gossip, witness, offender, victim);
	}
	else
	{
		Script_Warning(" ... newsnumber: " + newsId, 1);
	}
	parser->SetReturn(victim);
	return 0;
}

int Npc_GetNewsGossip()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetNewsGossip";

	uint32_t newsId;
	parser->GetParameter(&newsId);
	oCNpc* npc = GetNpc(funcname, 0);

	int id;
	int gossip;
	oCNpc* witness = 0;
	oCNpc* offender = 0;
	oCNpc* victim = 0;

	if ( npc )
	{
		auto news = npc->knownNews->GetNewsByNumber(newsId);
		if ( news )
			news->GetNewsData(news, id, gossip, witness, offender, victim);
	}
	else
	{
		Script_Warning(" ... newsnumber: " + newsId, 1);
	}
	parser->SetReturn(gossip);
	return 0;
}

int Npc_DeleteNews()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_DeleteNews";

	int newsID = 0;
	parser->GetParameter(&newsID);

	oCNpc* npc = GetNpc(funcname, 1);
	bool ok = 0;
	if ( npc )
		ok = npc->knownNews->DeleteNewsByNumber(newsID);
	zCParser::SetReturn(parser, ok);
	return 0;
}

// combat

int Npc_GetActiveSpell()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetActiveSpell";
	oCNpc *npc = GetNpc(funcname, 1);
	oCSpell* spell = 0;
	if ( npc )
		spell = npc->GetActiveSpellNr();
	zCParser::SetReturn(parser, spell);
	return 0;
}

int Npc_GetActiveSpellCat()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetActiveSpellCat";
	oCNpc *npc = GetNpc(funcname, 1);
	int spellCat = 0;
	if ( npc )
		spellCat = npc->GetActiveSpellCategory();
	zCParser::SetReturn(parser, spellCat);
	return 0;
}

int Npc_GetLastHitSpellID()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetLastHitSpellID";
	oCNpc* npc = GetNpc(funcname, 1);
	if ( npc )
		zCParser::SetReturn(parser, npc->lastHitSpellID);
	return 0;
}

int Npc_GetLastHitSpellCat()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetLastHitSpellCat";
	oCNpc* npc = GetNpc(funcname, 1);
	if ( npc )
		zCParser::SetReturn(parser, npc->lastHitSpellCat);
	return 0;
}

int Npc_GetActiveSpellLevel()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetActiveSpellLevel";
	oCNpc *npc = GetNpc(funcname, 1);
	int spellLevel = 0;
	if ( npc )
		spellLevel = npc->GetActiveSpellLevel();
	zCParser::SetReturn(parser, spellLevel);
	return 0;
}

int Npc_GetActiveSpellIsScroll()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetActiveSpellIsScroll";
	oCNpc *npc = GetNpc(funcname, 1);
	int isScroll = 0;
	if ( npc )
		isScroll = npc->GetActiveSpellIsScroll();
	zCParser::SetReturn(parser, isScroll);
	return 0;
}

int Npc_SetActiveSpellInfo()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetActiveSpellInfo";
	oCNpc *npc = GetNpc(funcname, 1);
	int value = 0;
	parser->GetParameter(&value);
	if ( npc )
		npc->SetActiveSpellInfo(value);
	return 0;
}

int Npc_CreateSpell()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CreateSpell";
	int spell = 0;
	parser->GetParameter(&spell);
	oCNpc *npc = GetNpc(funcname, 1);
	if ( npc )
		npc->CreateSpell(spell);
	return 0;
}

int Npc_LearnSpell()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_LearnSpell";
	int spell = 0;
	parser->GetParameter(&spell);
	oCNpc *npc = GetNpc(funcname, 1);
	if ( npc )
		npc->LearnSpell(spell);
	return 0;
}

// inventory
int Npc_HasItems()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_LearnSpell";
	int instanceItem = 0;
	parser->GetParameter(&instanceItem);
	oCNpc *npc = GetNpc(funcname, 1);

	int amount = 0;
	if ( npc )
		amount = npc->inventory2->GetAmount(instance);
	zCParser::SetReturn(parser, amount);
	return 0;
}

int Npc_GiveItem()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GiveItem";
	oCNpc* oth = GetNpc(funcname, 1);
	int item = 0;
	parser->GetParameter(&item);
	oCNpc* slf = GetNpc(funcname, 1);
	if ( slf && oth )
	{
		auto sym = parser->GetSymbol(item);
		if ( sym && sym->name == "ITEM") {
			zCVob* adr = (zCVob*)sym->GetInstanceAdr();
			oCItem* item_vob = dynamic_cast<oCItem*>(adr);
			if ( item_vob )
				item = item_vob->GetInstance();
		}
		if ( item >= 0 )
		{
			oCItem* item_vob = self->GetTradeItem();
			if ( item_vob )
			{
				auto inst = item_vob->GetInstance();
				if ( inst == item )
				{
					slf->CloseTradeContainer();
					oth->CloseTradeContainer();
				}
			}

			item_vob = slf->RemoveFromInv(slf, item, 1);
			oth->PutInInv(item_vob);
		}
	}
	return 0;
}

int Npc_CreateInvItem()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CreateInvItem";
	int itemInstance = 0;
	parser->GetParameter(&itemInstance);

	oCNpc* slf = GetNpc(funcname, 1);

	if ( slf && ogame && !ogame->inLoadSaveGame )
	{
		Script_SaveNpcGlobals();
		auto world = ogame->GetGameWorld();
		auto item_vob = world->ThisVobAddedToWorld(world, 129, itemInstance);

		if ( item_vob )
		{
			int inst = slf->PutInInv(item_vob);
			zparser->SetInstance("ITEM", inst);
			item_vob->Release();
		}
		else
		{
			zSTRING tmpstr;
			auto sym = parser->GetSymbol(itemInstance);
			if ( !sym )
				tmpstr = "[UNKNOWN]";
			else
				tmpstr = sym->name;

			Script_Warning("Npc_CreateInvItem(): item could not be created. illegal item: " + tmpstr);
		}
		Script_RestoreNpcGlobals();
	}
	return 0;
}

int Npc_CreateInvItems()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CreateInvItems";
	int itemInstance = 0;
	int amount = 0;
	parser->GetParameter(&amount);
	parser->GetParameter(&itemInstance);

	oCNpc* slf = GetNpc(funcname, 1);

	if ( slf && ogame && !ogame->inLoadSaveGame )
	{
		Script_SaveNpcGlobals();
		slf->CreateItems(itemInstance, amount);
		Script_RestoreNpcGlobals();
	}
	return 0;
}

int Npc_GetInvItem()
{
	zCParser *parser = zCParser::GetParser();
	int itemInstance = 0;
	parser->GetParameter(&itemInstance);

	auto inst = (zCVob *)parser->GetInstance();
	slf = dynamic_cast<oCNpc*>(inst);
	if ( slf )
		item = slf->IsInInv(itemInstance, 1);
	zparser->SetInstance("ITEM", item);

	parser->SetReturn(item != 0);
	return 0;
}

int Npc_GetInvItemBySlot()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetInvItemBySlot";
	int slot = 0;
	int category = 0;
	parser->GetParameter(&slot);
	parser->GetParameter(&category);

	oCNpc* slf = GetNpc(funcname, 1);
	oCItem* item = nullptr;
	if ( slf )
		item = slf->GetItem(category, slot);
	zparser->SetInstance("ITEM", item);
	int amount = 0;
	if ( item )
		amount = item->amount;
	parser->SetReturn(amount);
	return 0;
}

int Npc_RemoveInvItem()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_RemoveInvItem";
	int inst = 0;
	parser->GetParameter(&inst);

	oCNpc* npc = GetNpc(funcname, 1);

	int ret = 0;
	oCItem* isIn = 0;
	if ( npc )
		isIn = npc->IsInInv(inst, 1);
	if ( isIn )
	{
		Script_SaveNpcGlobals();
		oCNpc::RemoveFromInv(npc, inst, 1);
		ret = 1;
		Script_RestoreNpcGlobals();
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_RemoveInvItems()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_RemoveInvItems";
	int instanceItem = 0;
	int amount = 0;
	parser->GetParameter(&amount);
	parser->GetParameter(&instanceItem);

	oCNpc* npc = GetNpc(funcname, 1);

	int ret = 0;
	oCItem* item = 0;
	if ( npc )
		item = npc->IsInInv(instanceItem, 1);

	if ( item ) {
		Script_SaveNpcGlobals();
		if (item->MultiSlot() && npc->IsInInv(instanceItem, amount))
		{
			npc->RemoveFromInv(instanceItem, amount);
		}
		else
		{
			for ( i = 0; i < amount; ++i ) {
				if ( !npc->RemoveFromInv(instanceItem, 1) )
					break;
			}
		}
		ret = 1;
		Script_RestoreNpcGlobals();
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_ClearInventory()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_ClearInventory";

	oCNpc* npc = GetNpc(funcname, 1);
	if ( npc )
	{
		auto inv = npc->inventory;
		if ( inv->IsEmpty2(0, 0) )
			return 0;

		Script_SaveNpcGlobals();
		int numItems = inv->GetNumItemsInCategory();
		for (int i = 0; i < numItems; ++i) {
			auto item = inv->GetItem(i);
			if ( !item )
				continue;

			int inst = item->GetInstance();
			int amount = inv->GetAmount(inst);
			if ( amount > 0 )
			{
				if ( item->HasFlag(item, ITEM_MISSION) ||
				     item->HasFlag(item, ITEM_ACTIVE) ) {
					++i;
					continue;
				}

				npc->RemoveFromInv(item, amount);
			}
		}
		Script_RestoreNpcGlobals();
	}
	return 0;
}

// AI
int Npc_IsInState()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsInState";
	int state = 0;
	parser->GetParameter(&state);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
	{
		parser->SetReturn(npc->IsAIState(state));
	}
	else
	{
		parser->SetReturn(0);
	}
	return 0;
}

int Npc_WasInState()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_WasInState";
	int state = 0;
	parser->GetParameter(&state);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
	{
		parser->SetReturn(npc->states->GetLastState() == state);
	}
	else
	{
		parser->SetReturn(0);
	}
	return 0;
}

int Npc_IsInRoutine()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsInRoutine";
	int state = 0;
	parser->GetParameter(&state);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
	{
		parser->SetReturn(npc->states->GetLastRoutineState() == state);
	}
	else
	{
		parser->SetReturn(0);
	}
	return 0;
}

int Npc_HasDetectedNpc()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_HasDetectedNpc";

	oCNpc* oth = GetNpc(funcname, 1);
	oCNpc* slf = GetNpc(funcname, 1);

	if ( slf && oth )
	{
		parser->SetReturn(slf->HasVobDetected(oth));
	}
	else
	{
		parser->SetReturn(0);
	}
	return 0;
}

// Attitudes
int Npc_SetAttitude()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetAttitude";
	int attitude = 0;
	parser->GetParameter(&attitude);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
		npc->SetAttitude(attitude);
	return 0;
}

int Npc_SetTempAttitude()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetTempAttitude";
	int attitude = 0;
	parser->GetParameter(&attitude);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
		npc->SetTmpAttitude(attitude);
	return 0;
}

int Npc_GetAttitude()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetAttitude";

	oCNpc* oth = GetNpc(funcname, 1);
	oCNpc* slf = GetNpc(funcname, 1);

	if ( slf && oth )
	{
		int att = slf->GetAttitude(oth);
		parser->SetReturn(att);
	}
	return 0;
}

int Npc_GetPermAttitude()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetPermAttitude";

	oCNpc* oth = GetNpc(funcname, 1);
	oCNpc* slf = GetNpc(funcname, 1);

	if ( slf && oth )
	{
		int att = slf->GetPermAttitude(oth);
		parser->SetReturn(att);
	}
	return 0;
}

int Npc_ChangeAttribute()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_ChangeAttribute";
	int attribute = 0;
	int value = 0;
	parser->GetParameter(&value);
	parser->GetParameter(&attribute);

	oCNpc* npc = GetNpc(funcname, 1);
	if ( npc )
		npc->ChangeAttribute(attribute, value);

	return 0;
}

int Npc_GetComrades()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetComrades";

	int hasComrades = 0;
	oCNpc* npc = GetNpc(funcname, 1);
	if ( npc )
		hasComrades = npc->GetComrades();
	parser->SetReturn(hasComrades);

	return 0;
}

int Npc_IsNear()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsNear";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc = GetNpc(funcname, 1);

	bool ret = 0;

	if ( npc && other )
		ret = npc->IsNear(other);

	parser->SetReturn(ret);
	return 0;
}

int Npc_GetDistToNpc()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetDistToNpc";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc = GetNpc(funcname, 1);

	float ret;

	if ( npc && other )
		ret = npc->GetDistanceToVob(other);
	else
		ret = std::numeric_limits<float>::max; // 0x7FFF'FFFF

	parser->SetReturn(ret);
	return 0;
}


int Npc_GetHeightToNpc()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetHeightToNpc";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc = GetNpc(funcname, 1);

	float ret;

	if ( npc && other ) {
		zVEC3 slf_pos(npc->trafoObjToWorld.m[0][3],
		              npc->trafoObjToWorld.m[1][3],
		              npc->trafoObjToWorld.m[2][3]);
		ret = fabs(slf_pos.y - oth->trafoObjToWorld.m[1][3]);
	} else {
		ret = std::numeric_limits<float>::max; // 0x7FFF'FFFF
	}

	parser->SetReturn(ret);
	return 0;
}


int Npc_GetHeightToItem()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetHeightToItem";

	oCItem* item = GetItem(funcname, 1);
	oCNpc* npc = GetNpc(funcname, 1);

	float ret;

	if ( npc && other ) {
		zVEC3 slf_pos(npc->trafoObjToWorld.m[0][3],
		              npc->trafoObjToWorld.m[1][3],
		              npc->trafoObjToWorld.m[2][3]);
		ret = fabs(slf_pos.y - item->trafoObjToWorld.m[1][3]);
	} else {
		ret = std::numeric_limits<float>::max; // 0x7FFF'FFFF
	}

	parser->SetReturn(ret);
	return 0;
}


int Npc_GetDistToPlayer()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetDistToPlayer";

	oCNpc* npc = GetNpc(funcname, 1);
	zCVob* player = oCGame::GetSelfPlayerVob();

	float ret = std::numeric_limits<float>::max; // 0x7FFF'FFFF

	if ( npc && player )
		ret = npc->GetDistanceToVobApprox(player);

	parser->SetReturn(ret);
	return 0;
}



int Npc_GetDistToItem()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetDistToItem";

	oCItem* item = GetItem(funcname, 1);
	oCNpc* npc = GetNpc(funcname, 1);

	float ret = std::numeric_limits<float>::max; // 0x7FFF'FFFF

	if ( npc && other )
		ret = npc->GetDistanceToVob(item);

	parser->SetReturn(ret);
	return 0;
}


int Npc_GetDistToWP()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetDistToWP";

	zSTRING wp;
	parser->GetParameter(wp);
	oCNpc* npc = GetNpc(funcname, 1);

	float ret = std::numeric_limits<double>::quiet_NaN();

	if (npc && npc->homeWorld) {
		auto world = ogame->GetWorld();
		auto waypoint = world->waynet->GetWaypoint(wp);
		if (waypoint) {
			zVEC3 npc_pos = npc->trafoObjToWorld.GetTranslation();
			zVEC3 wp_pos = waypoint->GetPositionWorld();
			ret = (npc_pos - wp_pos).LengthApprox();
		} else {
			Script_Warning("Npc_GetDistToWP(): Waypoint " + wp + " not found.");
		}
	} else {
		Script_Warning("... wp-name: " + wp);
	}

	parser->SetReturn(ret);
	return 0;
}

int Npc_CanSeeNpc()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CanSeeNpc";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	bool ret = false;

	if ( npc && other )
		ret = npc->CanSee(other, false);

	parser->SetReturn(ret);
	return 0;
}

int Npc_CanSeeNpcFreeLOS()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CanSeeNpcFreeLOS";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	bool ret = false;

	if ( npc && other )
		ret = npc->CanSee(other, true);

	parser->SetReturn(ret);
	return 0;
}

int Npc_CanSeeItem()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_CanSeeItem";

	oCItem* item = GetItem(funcname, 1);
	oCNpc*  npc  = GetNpc(funcname, 1);

	bool ret = false;

	if ( npc && other )
		ret = npc->CanSee(item, false);

	parser->SetReturn(ret);
	return 0;
}

int Npc_IsPlayer()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsPlayer";
	oCNpc* npc   = GetNpc(funcname, 1);

	bool ret = npc && npc->IsAPlayer();

	parser->SetReturn(ret);
	return 0;
}

int Npc_KnowsPlayer()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_KnowsPlayer";

	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	bool ret = false;

	if ( npc && player ) {
		uint32_t num = player->GetPlayerNumber();
		ret = npc->KnowsPlayer(num);
	}

	parser->SetReturn(ret);
	return 0;
}


int Npc_SetKnowsPlayer()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetKnowsPlayer";

	oCNpc* player = GetNpc(funcname, 1);
	oCNpc* npc    = GetNpc(funcname, 1);

	if ( npc && player ) {
		uint32_t num = player->GetPlayerNumber();
		ret = npc->SetKnowsPlayer(num);
	}

	parser->SetReturn(ret);
	return 0;
}

int Npc_IsInFightMode()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsInFightMode";

	int mode = 0;
	parser->GetParameter(&mode);
	oCNpc* npc = GetNpc(funcname, 1);

	bool ret = false;
	if ( npc )
		npc->IsInFightMode_S(mode);

	parser->SetReturn(ret);
	return 0;
}

int Npc_IsAiming()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsAiming";
	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	bool ret = false;
	if ( npc )
		ret = npc->IsAiming_S(other);

	parser->SetReturn(ret);
	return 0;
}

int Npc_StartItemReactModules()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_StartItemReactModules";
	oCItem* item = GetItem(funcname, 1);
	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	int ret = 0;
	if ( npc && other && item ) {
		if ( npc->IsAPlayer() )
			ret = npc->CheckItemReactModule(other, item);
		else
			ret = other->CheckItemReactModule(npc, item);
	}

	parser->SetReturn(ret);
	return 0;
}


int Npc_HasOffered()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "Npc_HasOffered";
	int instanceItem;
	parser->GetParameter(instanceItem);
	oCNpc* other = GetNpc(funcname, 1);
	oCNpc* npc   = GetNpc(funcname, 1);

	int ret = 0;
	if (npc && other) {
		oCItem* tradeItem = other->GetTradeItem() != 0;
		if (item && other->IsSelfPlayer())
			ret = tradeInst->GetInstance() == instanceItem;
	}

	parser->SetReturn(ret);
	return 0;
}

int EquipItem()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "EquipItem";
	int itemInstance;
	parser->GetParameter(itemInstance);
	oCNpc* npc = GetNpc(funcname, 1);

	if (npc && ogame && !ogame->inLoadSaveGame) {
		oCWorld* world = ogame->GetGameWorld();
		oCVob* itemVob = world->CreateVob(VOB_TYPE_ITEM, itemInstance);
		oCItem* item = (oCItem*)itemVob;

		if ( itemVob ) {
			item->SetReturn(ITEM_ACTIVE);
			npc->PutInInv(item);
			npc->AddItemEffects(item);
			item->Release();
		} else {
			auto sym = parser->GetSymbol(itemInstance);
			zSTRING name = sym ? sym->name : "[UNKNOWN]";
			Script_Warning("EquipItem(): item could not be created. illegal item: " + name);
		}
	}
	return 0;
}

int Npc_SetToFistMode()
{
	auto parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetToFistMode";
	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc ) {
		npc->SetWeaponMode(FMODE_FIST);
		auto item = dynamic_cast<oCItem>(npc->GetRightHand());
		if ( item ) {
			npc->RemoveFromSlot(NPC_NODE_RIGHTHAND, 1, 1);
			npc->RemoveFromInv(item, 1);
			ogame->GetGameWorld()->RemoveVob(item);
		}
	}
	return 0;
}

int Npc_SetToFightMode()
{
	auto parser = zCParser::GetParser();
	zSTRING funcname = "Npc_SetToFightMode";

	int weapon;
	parser->GetParameter(weapon);
	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc ) {
		auto rightHand = dynamic_cast<oCItem>(npc->GetRightHand());

		if (rightHand) {
			oCWorld* world = ogame->GetGameWorld();
			oCVob* itemVob = world->CreateVob(VOB_TYPE_ITEM, itemInstance);
			oCItem* item = (oCItem*)itemVob;

			if ( item ) {
				npc->PutInInv(item);
				npc->PutInSlot("ZS_RIGHTHAND", item, 1);
				int guild = npc->GetTrueGuild();
				if ( !trueGuild )
					guild = npc->GetGuild();

				if (guild >= GIL_SEPARATOR_HUM)
					npc->SetWeaponMode(FMODE_FIST);
				else
					npc->SetWeaponMode(FMODE_1H);

				npc->Release();
			} else {
				auto sym = parser->GetSymbol(weapon);
				zSTRING name = sym ? sym->name : "[UNKNOWN]";

				Script_Warning("Npc_SetToFightMode(): item could not be created for fight mode. illegal item: " + name);
			}
		}
	}
	return 0;
}


int Npc_IsDead()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_IsDead";

	oCNpc* npc = GetNpc(funcname, 1);

	int ret = 1;
	if ( npc )
		ret = npc->IsDead();

	parser->SetReturn(ret);

	return 0;
}


int AI_StartState()
{
	zCParser* parser = zCParser::GetParser();

	int state;
	int behaviour;
	int waypoint;
	parser->GetParameter(waypoint);
	parser->GetParameter(behaviour);
	parser->GetParameter(state);

	npc = GetNpc("AI_StartState", 0);
	if ( npc ) {
		auto msg = new oCMsgState(0, state, behaviour, &waypoint);

		auto npcother = zparser->GetSymbol("OTHER");
		if ( npcother ) {
			zCVob* obj = npcother->GetInstanceAdr();
			msg->other = dynamic_cast<oCNpc>(obj);
		}

		auto npcvictim = zparser->GetSymbol("VICTIM");
		if ( npcvictim ) {
			zCVob* obj = npcvictim->GetInstanceAdr();
			msg->victim = dynamic_cast<oCNpc>(obj);
		}

		uint32_t inRoutine = npc->states->IsInRoutine();
		msg->flags ^= (msg->flags ^ (2 * inRoutine)) & 2;

		npc->GetEM()->OnMessage(msg, npc);
	} else {
		sym = zparser->GetSymbol(state);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";

		Script_Warning("... aiState: " + name + ", wp-name: " + waypoint);
	}
	return 1;
}

int Npc_GetStateTime()
{
	zCParser* parser = zCParser::GetParser();

	oCNpc* npc = GetNpc("Npc_GetStateTime", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetAIStateTime();
	parser->SetReturn(ret);
	return 0;
}

int Npc_SetStateTime()
{
	zCParser* parser = zCParser::GetParser();

	int seconds;
	parser->GetParameter(seconds);

	oCNpc* npc = GetNpc("Npc_GetStateTime", 1);


	if ( npc )
		npc->SetAIStateTime(seconds);

	return 0;
}

int AI_Output()
{
	auto parser = zCParser::GetParser();

	zSTRING nameWAV;
	parser->GetParameter(nameWAV);

	auto target = GetNpc("AI_Output", 1);
	auto npc = GetNpc("AI_Output", 1);

	if ( npc ) {
		if ( target ) {
			oCMsgConversation talkingMsg = target.GetTalkingWithMessage(npc);
			// CHECK FOR INLINED
			if ( talkingMsg ) {
				auto waitMsg = new oCMsgConversation(EV_WAITTILLEND, target);
				waitMsg->talkingWith = talkingMsg;
				npc->GetEM()->OnMessage(waitMsg, npc);
			}
		}
		
		auto talkingMsg = new oCMsgConversation(EV_OUTPUT, nameWAV);
		talkingMsg->target = target;

		npc->GetEM()->OnMessage(talkingMsg, npc);
		if ( talkingMsg ) {
			auto waitMsg = new oCMsgConversation(EV_WAITTILLEND, npc);
			waitMsg->talkingWith = talkingMsg;
			npc->GetEM()->OnMessage(waitMsg, npc);
		}
	}
	return 1;
}

int AI_OutputSVM()
{
	auto parser = zCParser::GetParser();

	zSTRING nameSVM;
	parser->GetParameter(nameSVM);

	auto target = GetNpc("AI_OutputSVM", 1);
	auto npc = GetNpc("AI_OutputSVM", 1);

	if ( npc ) {
		if ( other ) {
			auto talkingMsg = oCNpc::GetTalkingWithMessage(other, npc);
			if ( talkingMsg ) {
				auto waitMsg = oCMsgConversation(EV_WAITTILLEND, target);
				waitMsg->talkingWith = talkingMsg;
				npc->GetEM()->OnMessage(waitMsg, npc);
			}
		}

		auto outputMsg = new oCMsgConversation(EV_OUTPUTSVM, nameSVM);
		outputMsg->target = target;
		npc->GetEM()->OnMessage(outputMsg, npc);
	}
	return 1;
}

int AI_OutputSVM_Overlay()
{
	auto parser = zCParser::GetParser();

	zSTRING nameSVM;
	parser->GetParameter(nameSVM);

	auto target = GetNpc("AI_OutputSVM_Overlay", 1);
	auto npc = GetNpc("AI_OutputSVM_Overlay", 1);

	if ( npc ) {
		if ( other ) {
			auto talkingMsg = oCNpc::GetTalkingWithMessage(other, npc);
			if ( talkingMsg ) {
				auto waitMsg = oCMsgConversation(EV_WAITTILLEND, target);
				waitMsg->talkingWith = talkingMsg;
				npc->GetEM()->OnMessage(waitMsg, npc);
			}
		}

		auto outputMsg = new oCMsgConversation(EV_OUTPUTSVM_OVERLAY, nameSVM);
		outputMsg->target = target;
		npc->GetEM()->OnMessage(outputMsg, npc);
	}
	return 1;
}

int AI_PlayCutscene()
{
	auto parser = zCParser::GetParser();

	zSTRING nameCs;
	parser->GetParameter(nameCs);

	auto npc = GetNpc("AI_PlayCutscene", 1);

	if ( npc ) {
		auto csMsg = new oCMsgConversation(EV_CUTSCENE, nameCs, npcName);
		zSTRING name = npc->GetObjectName();
		npc->GetEM()->OnMessage(csMsg, npc);
	}

	return 1;
}

int AI_CanSeeNpc()
{
	auto parser = zCParser::GetParser();

	int unkparam;
	parser->GetParameter(unkparam);

	auto other = GetNpc("AI_CanSeeNpc", 1);
	auto npc = GetNpc("AI_CanSeeNpc", 1);

	if ( npc && other ) {
		auto msg = new oCMsgMovement(EV_CANSEENPC, other);
		msg->unki[12] = unkparam;
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_Wait()
{
	auto parser = zCParser::GetParser();
	float timeSec;
	parser->GetParameter(timeSec);

	oCNpc* npc = GetNpc("AI_Wait", 1);

	if ( npc ) {
		auto msg = new oCMsgState(EV_WAIT, timeSec * 1000.0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_WaitMS()
{
	auto parser = zCParser::GetParser();
	float msec;
	parser->GetParameter(msec);

	oCNpc* npc = GetNpc("AI_WaitMS", 1);

	if ( npc ) {
		auto msg = new oCMsgState(EV_WAIT, msec);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_WaitTillEnd()
{
	auto parser = zCParser::GetParser();

	oCNpc* other = GetNpc("AI_WaitTillEnd", 1);
	oCNpc* npc = GetNpc("AI_WaitTillEnd", 1);
	if ( npc && other ) {
		auto msg = new oCMsgConversation(EV_WAITTILLEND, npc);
		auto em = npc->GetEM();

		oCMsgConversation* talkingWith;
		for (int i = 0, num = em->GetNumMessages(); i < num; ++i) {
			auto talkingWith = em->GetEventMessage(i);
			if (!talkingWith->IsOverlay())
				break;
		}

		msg->talkingWith = talkingWith;

		em->OnMessage(msg, npc);
	}
	return 1;
}


int AI_AlignToWP()
{
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("AI_AlignToWP", 1);
	if ( npc ) {
		auto homeWorld = npc->homeWorld;
		if ( homeWorld ) {
			zVEC3 npcPos(npc->trafoObjToWorld.m[0][3],
			             npc->trafoObjToWorld.m[1][3],
			             npc->trafoObjToWorld.m[2][3]);

			auto nearest = homeWorld->wayNet->GetNearestWaypoint(npcPos);
			if ( nearest ) {
				float x = npcpos[0] + nearest->dir.x * 200.0;
				float y = npcpos[1] + nearest->dir.y * 200.0;
				float z = npcpos[2] + nearest->dir.z * 200.0;

				zVEC3 alignment(x, y, z);
				auto msg = new oCMsgMovement(EV_TURNTOPOS, alignment);
				npc->GetEM()->OnMessage(msg, npc);
			}
		}
	}
	return 1;
}

int AI_SetWalkMode()
{
	auto parser = zCParser::GetParser();
	int walkMode;
	parser->GetParameter(walkMode);
	auto npc = GetNpc("AI_SetWalkMode", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_SETWALKMODE, walkMode);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_PlayAni()
{
	auto parser = zCParser::GetParser();

	zSTRING ani;
	parser->GetParameter(ani);
	auto npc = GetNpc("AI_PlayAni", 1);

	if ( npc ) {
		auto msg = new oCMsgConversation(EV_PLAYANI_NOOVERLAY, ani);
		msg->number = 0;
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_PlayAniBS()
{
	auto parser = zCParser::GetParser();

	zSTRING ani;
	int state;

	parser->GetParameter(state);
	parser->GetParameter(ani);
	auto npc = GetNpc("AI_PlayAniBS", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_PLAYANI_NOOVERLAY, ani);
		msg->number = state;
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_GotoWP()
{
	auto parser = zCParser::GetParser();

	zSTRING waypoint;
	parser->GetParameter(waypoint);
	auto npc = GetNpc("AI_GotoWP", 1);
	if ( npc ) {
		if ( !waypoint.IsEmpty() ) {
			auto msg = new oCMsgMovement(EV_GOROUTE, waypoint);
			npc->GetEM()->OnMessage(msg, npc);
		} else {
			zWARNING("U:NPC:AI_GotoWP Waypoint is empty ! Ignored.message : " + npc->GetInstanceName());
		}
	}
	return 1;
}

int AI_Teleport()
{
	auto parser = zCParser::GetParser();

	zSTRING waypoint;
	parser->GetParameter(waypoint);
	auto npc = GetNpc("AI_Teleport", 1);

	if ( npc ) {
		if (npc->IsSelfPlayer() || npc->homeWorld ||
		    npc->GetEM()->GetCutscene())
		{
			new msg = oCMsgMovement(EV_BEAMTO, waypoint);
			npc->GetEM()->OnMessage(msg, npc);
		} else {
			npc->BeamTo(waypoint);
		}
	}
	return 1;
}

int AI_GotoItem()
{
	auto parser = zCParser::GetParser();

	auto item = GetItem("AI_GotoItem", 1);
	auto npc = GetNpc("AI_GotoItem", 1);

	if (npc && item) {
		auto msg = new oCMsgMovement(EV_GOTOVOB, item);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_GotoNpc()
{
	auto parser = zCParser::GetParser();

	auto other = GetItem("AI_GotoNpc", 1);
	auto npc = GetNpc("AI_GotoNpc", 1);

	if (npc && other) {
		auto msg = new oCMsgMovement(EV_GOTOVOB, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_AlignToFP()
{
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("AI_AlignToFP", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_ALIGNTOFP, "");
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int Npc_IsOnFP()
{
	auto parser = zCParser::GetParser();

	zSTRING freepoint;
	parser->GetParameter(freepoint);

	auto npc = GetNpc("AI_GotoFP", 1);

	bool ret;
	if ( npc ) {
		spot = npc->FindSpot(freepoint, 1, 100.0);
		ret = spot && spot->npc == npc;
	} else {
		Script_Warning("... fpName: " + freepoint);
	}
	parser->SetReturn(ret);
	return 1;
}


int AI_GotoFP()
{
	auto parser = zCParser::GetParser();
	zSTRING freepoint;
	parser->GetParameter(freepoint);
	freepoint.Upper();

	auto npc = GetNpc("AI_GotoFP", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_GOTOFP, freepoint);
		msg->targetMode = 1;
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		Script_Warning("... fpName: " + freepoint);
	}
	return 1;
}

int AI_GotoNextFP()
{
	auto parser = zCParser::GetParser();

	zSTRING freepoint;
	parser->GetParameter(freepoint);

	auto npc = GetNpc("AI_GotoNextFP", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_GOTOFP, freepoint);
		msg->targetMode = 0;
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		Script_Warning("... fpName: " + freepoint);
	}
	return 1;
}

int AI_GotoSound()
{
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("AI_GotoNextFP", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_GOTOPOS, npc->soundPosition)
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_TakeItem()
{
	auto parser = zCParser::GetParser();
	auto item = GetItem("AI_TakeItem", 1);
	auto npc = GetNpc("AI_TakeItem", 1);
	if ( npc && item ) {
		auto gotoItem = new oCMsgMovement(EV_GOTOVOB, item);
		npc->GetEM()->OnMessage(gotoItem, npc);

		auto turnToitem = new oCMsgMovement(EV_TURNTOVOB, item);
		npc->GetEM()->OnMessage(turnToitem, npc);

		auto takeItem = new oCMsgManipulate(EV_TAKEVOB, item);
		npc->GetEM()->OnMessage(takeItem, npc);
	}
	return 1;
}

int AI_DropItem()
{
	auto parser = zCParser::GetParser();

	int instanceItem;
	parser->GetParameter(instanceItem);
	auto npc = GetNpc("AI_DropItem", 1);
	if ( npc ) {
		int globalItem = parser->GetIndex("ITEM");
		if ( globalItem == instanceItem ) {
			auto sym = parser->GetSymbol(globalItem);
			oCItem* adr = sym->GetInstanceAdr();
			if ( adr )
				instanceItem = adr->GetInstance(adr);
		}

		oCItem* item = npc->RemoveFromInv(instanceItem, 1);
		if ( item ) {
			if ( item->HasFlag(ITEM_ACTIVE)  &&
			    (item->HasFlag(ITEM_KAT_NF)  ||
			     item->HasFlag(ITEM_KAT_FF)  ||
			     item->HasFlag(ITEM_KAT_MUN) ||
			     item->HasFlag(ITEM_KAT_RUNE)))
			{
				auto msgWeap = new oCMsgWeapon(3, 0, 0);
				npc->GetEM()->OnMessage(msgWeap, npc);
			}

			auto dropItem = new oCMsgManipulate(EV_DROPITEM, item);
			npc->GetEM()->OnMessage(dropItem, npc);
		}
	}
	return 1;
}


int AI_DrawWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_DrawWeapon", 1);
	if ( npc ) {
		auto msg = new oCMsgWeapon(0, 0, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_RemoveWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_DrawWeapon", 1);

	if ( npc ) {
		oCItem* weap = mpc->GetWeapon();

		if ( weap && weap->HasFlag(ITEM_KAT_FF) ) {
			auto msg = new oCMsgAttack(8, 0, 1);
			npc->GetEM()->OnMessage(msg, npc);
		}

		auto msg = new oCMsgWeapon(3, 0, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int Npc_HasRangedWeaponWithAmmo()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasRangedWeaponWithAmmo", 1);

	bool ret = false;
	if ( ret )
		ret = npc->HasRangedWeaponAndAmmo();
	parser->SetReturn(ret);
	return 0;
}

int Npc_HasEquippedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasEquippedWeapon", 1);

	oCItem* weap;
	if ( npc ) {
		weap = npc->GetEquippedMeleeWeapon();
		if ( !weap )
			weap = npc->GetEquippedRangedWeapon();
	}
	parser->SetReturn(weap != 0);
	return 0;
}

int Npc_HasEquippedMeleeWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasEquippedMeleeWeapon", 1);

	oCItem* weap;
	if ( npc )
		weap = npc->GetEquippedMeleeWeapon();

	parser->SetReturn(weap != 0);
	return 0;
}

int Npc_HasEquippedRangedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasEquippedRangedWeapon", 1);

	oCItem* weap;
	if ( npc ) {
		weap = npc->GetEquippedRangedWeapon();
	}
	parser->SetReturn(weap != 0);
	return 0;
}

int Npc_HasEquippedArmor()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasEquippedArmor", 1);

	oCItem* armor;
	if ( npc ) {
		armor = npc->GetEquippedArmor();
	}
	parser->SetReturn(armor != 0);
	return 0;
}

int Npc_HasReadiedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasReadiedWeapon", 1);

	oCItem* weap;
	if ( npc )
		weap = npc->GetWeapon();
	parser->SetReturn(weap != 0);
	return 0;
}

int Npc_HasReadiedMeleeWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasReadiedMeleeWeapon", 1);

	bool ret;
	if ( npc ) {
		auto weap = npc->GetWeapon();
		if (weap)
			ret = weap->HasFlag(ITEM_KAT_NF).
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_HasReadiedRangedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_HasReadiedRangedWeapon", 1);

	bool ret;
	if ( npc ) {
		auto weap = npc->GetWeapon();
		if (weap)
			ret = weap->HasFlag(ITEM_KAT_FF).
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetTrueGuild()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetTrueGuild", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetTrueGuild();
	parser->SetReturn(ret);
	return 0;
}

int Npc_SetTrueGuild()
{
	auto parser = zCParser::GetParser();

	int guild;
	parser->GetParameter(guild);
	auto npc = GetNpc("Npc_SetTrueGuild", 1);

	if ( npc ) {
		npc->SetTrueGuild(guild);
		if (npc->IsAPlayer())
			game->InitNpcAttitudes();
	} else {
		Script_Warning("U:NPC: SetTrueGuild failed.");
	}
	return 0;
}

int Npc_AreWeStronger()
{
	auto parser = zCParser::GetParser();

	auto other = GetItem("Npc_AreWeStronger", 1);
	auto npc = GetNpc("Npc_AreWeStronger", 1);

	bool ret = false;
	if (npc && other)
		ret = other->AreWeStronger(npc);
	parser->SetReturn(ret);
}

int Npc_GetTarget()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetTarget";
	oCNpc* npc = GetNpc(funcname, 1);
	oCNpc* enemy = nullptr;
	if ( npc ) {
		enemy = npc->enemy;
		parser->SetInstance("OTHER", enemy);
	}
	parser->SetReturn(enemy != 0);
	return 0;
}

int Npc_GetNextTarget()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING funcname = "Npc_GetNextTarget";
	oCNpc* npc = GetNpc(funcname, 1);
	oCNpc* enemy = nullptr;
	if ( npc )
		enemy = npc->getNextEnemy();

	zparser.SetInstance("OTHER", enemy);
	parser->SetReturn(enemy != 0);
	return 0;
}

int Npc_GetLookAtTarget()
{
	zCParser* parser = zCParser::GetParser();
	oCNpc* npc = GetNpc("Npc_GetLookAtTarget", 1);
	oCNpc* ret = nullptr;
	if ( npc ) {
		zCVob* target = npc->GetAnictrl(npc)->targetVob;
		ret = zDYNAMIC_CAST<oCNpc>(target);
	}

	parser->SetReturn(ret);
	return 0;
}

int Npc_IsNextTargetAvailable()
{
	zCParser* parser = zCParser::GetParser();
	oCNpc* npc = GetNpc("Npc_IsNextTargetAvailable", 1);
	oCNpc* enemy = nullptr;
	if ( npc )
		enemy = npc->getNextEnemy();
	parser->SetReturn(enemy != 0);
	return 0;
}

int Npc_SetTarget()
{
	auto parser = zCParser::GetParser();
	auto target = GetNpc("Npc_SetTarget", 1);
	auto npc = GetNpc("Npc_SetTarget", 1);
	if ( npc ) {
		if ( !target )
			zWARNING("C: skript: Npc_SetTarget(): target is not valid. self is:" + npc->GetInstanceName());

		npc->SetEnemy(target);
	}
	return 0;
}

int Npc_GetBodyState()
{
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("Npc_GetBodyState", 1);
	int ret = 0;
	if ( npc )
		ret = npc->GetFullBodyState();
	parser->SetReturn(ret);
	return 0;
}

int Npc_HasBodyFlag()
{
	auto parser = zCParser::GetParser();

	oCNpc* npc;
	int bodyFlag;

	parser->GetParameter(bodyFlag);
	npc = GetNpc("Npc_HasBodyFlag", 1);

	int ret = 0;
	if ( npc ) {
		auto bodyState = npc->GetFullBodyState();

		ret = (bodyState & bodyFlag) > 0;
	}
	parser->SetReturn(ret);
	return 0;
}


int Npc_IsInCutscene()
{
	auto parser = zCParser::GetParser();

	oCNpc* npc = GetNpc("Npc_IsInCutscene", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetEM()->GetCutsceneMode();
	parser->SetReturn(ret);
	return 0;
}


int Npc_IsVoiceActive()
{
	auto parser = zCParser::GetParser();

	oCNpc* npc = GetNpc("Npc_IsVoiceActive", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetEM()->GetCutsceneMode() || npc->IsVoiceActive();
	parser->SetReturn(ret);
	return 0;
}

int Npc_StopAni()
{
	auto parser = zCParser::GetParser();

	oCNpc* npc;
	zSTRING ani;
	parser->GetParameter(ani);
	oCNpc* npc = GetNpc("Npc_StopAni", 1);

	if (npc) {
		zCModel* model = npc->GetModel();
		zCModelAni* modelAni = model->prototypes[0]->SearchAni(ani);
		int aniId = modelAni ? modelAni->aniId : -1;
		
		modelAni = model->GetAniFromID(aniId);
		model->StopAni(modelAni);

		auto em = npc->GetEM();
		for (int i = 0, num = em->GetNumMessages(); i < num; ++i) {
			zCEventMessage* msg = em->GetEventMessage(i);
			auto* msgConv = zDYNAMIC_CAST<zCMsgConversation>(msg);
			if (msgConv &&
			    msgConv->subType == EV_PLAYANI_NOOVERLAY &&
			    msgConv->name == ani)
				msgConv->Delete();
		}
	}
	return 0;
}

int Npc_GetDetectedMob()
{
	static zSTRING retstr;
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("Npc_GetDetectedMob");
	if ( npc ) {
		oCMob* interMob = npc->GetInteractMob();
		// dynamic_cast<oCMobInter>
		if (!interMob)
			interMob = zDYNAMIC_CAST<oCMobInter>(npc->GetRbtObstacleVob());

		if ( interMob ) {
			zSTRING scene = interMob->GetSceneName();
			word = scene.PickWord(1, "_", "_")
		}
	}
	parser->SetReturn(retstr);
	return 0;
}

int Npc_PlayAni()
{
	auto parser = zCParser::GetParser();

	oCNpc* npc;
	zSTRING ani;
	parser->GetParameter(ani);
	auto npc = GetNpc("Npc_PlayAni", 1);

	if ( npc )
		npc->GetModel()->StartAni(ani, 0);
	return 1;
}

int Npc_IsDetectedMobOwnedByNpc()
{
	auto parser = zCParser::GetParser();

	auto owner = GetNpc("Npc_IsDetectedMobOwnedByNpc", 1);
	auto npc   = GetNpc("Npc_IsDetectedMobOwnedByNpc", 1);

	bool ret = false;
	if ( npc && owner ) {
		oCMob* interMob = npc->GetInteractMob();
		if (!interMob)
			interMob = zDYNAMIC_CAST<oCMobInter>(npc->GetRbtObstacleVob());

		if (interMob)
			ret = interMob->IsOwnedByNpc(owner->GetInstance());
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_IsDetectedMobOwnedByGuild()
{
	auto parser = zCParser::GetParser();

	int ownerguild;
	parser->GetParameter(ownerguild);
	auto npc = GetNpc("Npc_IsDetectedMobOwnedByNpc", 1);

	bool ret = false;
	if ( npc ) {
		oCMob* interMob = npc->GetInteractMob();
		if (!interMob)
			interMob = zDYNAMIC_CAST<oCMobInter>(npc->GetRbtObstacleVob());

		
		if (interMob)
			ret = interMob->IsOwnedByGuild(ownerguild);
	}
	parser->SetReturn(ret);
	return 0;
}

int Wld_GetMobState()
{
	auto parser = zCParser::GetParser();

	zSTRING mobName;
	parser->GetParameter(mobName);
	auto npc = GetNpc("Wld_GetMobState", 1);

	int state = -1;
	if ( npc ) {
		oCMobInter* mobInter;
		if ( npc->GetInteractMob() )
			mobInter = npc->GetInteractMob();
		else
			mobInter = npc->FindMobInter(mobName);

		if ( mobInter )
			state = mobInter->GetState();
	}
	parser->SetReturn(state);
	return 0;
}

int Npc_ClearAIQueue()
{
	auto parser = zCParser::GetParser();
	auto npc = GetNpc("Npc_ClearAIQueue", 1);
	if ( npc )
		npc->ClearEM();
	return 0;
}

int InfoManager_HasFinished()
{
	auto parser = zCParser::GetParser();
	auto infoMan = oCInformationManager::GetInformationManager();

	parser->SetReturn(infoMan->HasFinished());
	return 0;
}

int AI_StopProcessInfos()
{
	auto parser = zCParser::GetParser();

	auto player = dynamic_cast<oCNpc>(oCGame::GetSelfPlayerVob());
	auto npc = GetNpc("AI_StopProcessInfos", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_STOPPROCESSINFOS, npc);
		npc->GetEM()->OnMessage(msg, player);
	}
	return 0;
}

int AI_ProcessInfos()
{
	auto parser = zCParser::GetParser();

	oCNpc* player = dynamic_cast<oCNpc>(oCGame::GetSelfPlayerVob());
	oCNpc* npc = GetNpc("AI_ProcessInfos", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_STOPPROCESSINFOS, npc);
		npc->GetEM()->OnMessage(msg, player);
	}
	auto infoMan = oCInformationManager::GetInformationManager();
	if ( infoMan->HasFinished() && npc ) {
		auto msg = new oCMsgConversation(EV_PROCESSINFOS, npc);
		npc->GetEM()->OnMessage(msg, player);
	}
	return 0;
}

int Info_AddChoice()
{
	auto parser = zCParser::GetParser();

	int menu;
	zSTRING text;
	int action;

	parser->GetParameter(action);
	parser->GetParameter(text);
	parser->GetParameter(menu);

	auto infoMan = ogame->GetInfoManager();
	auto info = infoMan.GetInformation(menu);

	if ( info ) {
		info->AddChoice(text, action);
	} else {
		auto sym = zparser.GetSymbol(menu);
		zSTRING name = sym ? sym->name : zSTRING(menu);
		Script_Warning("Info_AddChoice(): wrong or non-existant info with instance-id: " + name + ", with text: " + text);
	}
	return 0;
}

int Info_ClearChoices()
{
	auto parser = zCParser::GetParser();

	int menu;
	parser->GetParameter(menu);

	auto infoMan = ogame->GetInfoManager();
	auto info = infoMan.GetInformation(menu);

	if ( info ) {
		info->RemoveAllChoices();
	} else {
		auto sym = zparser.GetSymbol(menu);
		zSTRING name = sym ? sym->name : zSTRING(menu);
		Script_Warning("Info_AddChoice(): wrong or non-existant info with instance-id: " + name);
	}
	return 0;
}

int Npc_KnowsInfo()
{
	auto parser = zCParser::GetParser();

	int instanceInfo;
	parser->GetParameter(instanceInfo);
	auto npc = GetNpc("Npc_KnowsInfo", 1);

	int ret = 0;
	if ( npc ) {
		ret = ogame->GetInfoManager()->InformationTold(instanceInfo);
	} else {
		auto sym = zparser.GetSymbol(menu);
		zSTRING name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("... infoinstance: " + name);
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_CheckInfo()
{
	auto parser = zCParser::GetParser();

	int important;
	parser->GetParameter(important);

	auto npc = GetNpc("Npc_CheckInfo", 1);

	oCInfo* info = nullptr;
	if ( npc ) {
		auto player = oCGame::GetSelfPlayerVob();
		auto infoMan = ogame->GetInfoManager();
		info = infoMan->GetInformation(player, npc, important);
	}
	parser->SetReturn(info != 0);
	return 0;
}

int Npc_GiveInfo()
{
	auto parser = zCParser::GetParser();

	int important;
	parser->GetParameter(important);

	auto npc = GetNpc("Npc_GiveInfo", 1);

	bool ret = false;
	if ( npc ) {
		auto player = oCGame::GetSelfPlayerVob();
		auto infoMan = ogame->GetInfoManager();
		oCInfo* info = oCInfoManager::GetInformation(player, npc, important);
		if ( info ) {
			info->Info();
			ret = true;
		}
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetTalentSkill()
{
	auto parser = zCParser::GetParser();

	int talent;
	parser->GetParameter(talent);

	auto npc = GetNpc("Npc_GetTalentSkill", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetTalentSkill(talent);
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetTalentValue()
{
	auto parser = zCParser::GetParser();

	int talent;
	parser->GetParameter(talent);

	auto npc = GetNpc("Npc_GetTalentValue", 1);

	int ret = 0;
	if ( npc )
		ret = npc->GetTalentValue(talent);
	parser->SetReturn(ret);
	return 0;
}

int Npc_SetTalentSkill()
{
	auto parser = zCParser::GetParser();

	int talent;
	int level;
	parser->GetParameter(level);
	parser->GetParameter(talent);

	auto npc = GetNpc("Npc_SetTalentSkill", 1);

	if ( npc )
		ret = npc->SetTalentSkill(talent, level);
	parser->SetReturn(npc != 0);
	return 0;
}

int Npc_SetTalentValue()
{
	auto parser = zCParser::GetParser();

	int talent;
	int value;
	parser->GetParameter(value);
	parser->GetParameter(talent);

	auto npc = GetNpc("Npc_SetTalentValue", 1);

	if ( npc )
		npc->SetTalentValue(talent, value);
	parser->SetReturn(npc != 0);
	return 0;
}

int Tal_Configure()
{
	auto parser = zCParser::GetParser();

	int talent;
	int value;
	parser->GetParameter(value);
	parser->GetParameter(talent);

	auto npc = GetNpc("Tal_Configure", 1);

	parser->SetReturn(npc != 0);
	return 0;
}

int Npc_GetNearestWP()
{
	static zSTRING retstr;
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetNearestWP", 1);
	if (npc && npc->homeWorld) {
		auto wayNet = npc->homeWorld->wayNet;
		auto wp = wayNet->GetNearestWaypoint(npc->GetPos());
		retstr = wp ? wp->GetName() : "";
	}
	parser->SetReturn(retstr);
	return 0;
}

int Npc_GetNextWP()
{
	static zSTRING retstr;
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetNextWP", 1);
	if (npc && npc->homeWorld) {
		auto wayNet = npc->homeWorld->wayNet;
		auto wp = wayNet->GetSecNearestWaypoint(npc->GetPos());
		retstr = wp ? wp->GetName() : "";
	}
	parser->SetReturn(retstr);
	return 0;
}

int Npc_IsWayBlocked()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_IsWayBlocked", 1);
	bool ret = false;
	if ( npc ) {
		zTBBox3D bbox = npc->GetModel()->GetBBox3D();
		float out = (bbox.maxs[2] - bbox.mins[2]) * 0.5 + 100.0;

		zVEC3 pos = npc->GetPos();
		zVEC3 dir = pos * out;
		zVEC3 normal;

		zCVob* obstacle;
		ret = npc->RbtGetObstacle(pos, dir, obstacle, normal);
		if ( !ret ) {
			auto obstnpc = dynamic_cast<oCNpc>(obstacle);

			if (obstnpc) {
				ret = true;
			} else {
				auto ai = dynamic_cast<oCAIHuman>(npc->callback_ai);
				if (ai)
					ret = ai->CheckEnoughSpaceMoveForward(0) == 0;
			}
		}
	}
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetReadiedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetReadiedWeapon", 1);

	oCItem* ret = nullptr;
	if ( npc )
		ret = npc->GetWeapon();
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetEquippedMeleeWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetEquippedMeleeWeapon", 1);

	oCItem* ret = nullptr;
	if ( npc )
		ret = npc->GetEquippedMeleeWeapon();
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetEquippedRangedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetEquippedRangedWeapon", 1);

	oCItem* ret = nullptr;
	if ( npc )
		ret = npc->GetEquippedRangedWeapon();
	parser->SetReturn(ret);
	return 0;
}

int Npc_GetEquippedArmor()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_GetEquippedArmor", 1);

	oCItem* ret = nullptr;
	if ( npc )
		ret = npc->GetEquippedArmor();
	parser->SetReturn(ret);
	return 0;
}

int AI_EquipBestRangedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_EquipBestRangedWeapon", 1);
	if (npc) {
		auto msg = new oCMsgWeapon(9, 4, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_EquipBestArmor()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_EquipBestArmor", 1);
	if (npc) {
		auto msg = new oCMsgWeapon(10, 0, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}


int AI_UnequipWeapons()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_UnequipWeapons", 1);
	if (npc) {
		auto msg = new oCMsgWeapon(11, 0, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_UnequipArmor()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_UnequipArmor", 1);
	if (npc) {
		auto msg = new oCMsgWeapon(12, 0, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_EquipArmor()
{
	auto parser = zCParser::GetParser();

	int instanceItem;
	parser->GetParameter(instanceItem);

	auto npc = GetNpc("AI_EquipArmor", 1);
	if ( npc ) {
		auto sym = parser->GetSymbol(instanceItem);
		zSTRING name = sym ? sym->name : "";
		if (name == "ITEM") {
			zVob* vob = sym->GetInstanceAdr();
			auto item = dynamic_cast<oCItem>(vob);
			if ( item )
				name = item->GetInstanceName();
		}
		if ( !name.IsEmpty() ) {
			auto msg = new oCMsgWeapon(13, 0, 0);
			msg->SetObjectName(name);
			npc->GetEM()->OnMessage(msg, npc);
		}
	}
	return 1;
}

int AI_ReadyMeleeWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_ReadyMeleeWeapon", 1);
	if ( npc ) {
		auto msg = new oCMsgWeapon(0, 1, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	parser->SetReturn(0);
	return 1;
}

int AI_ReadyRangedWeapon()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_ReadyRangedWeapon", 1);
	if ( npc ) {
		auto msg = new oCMsgWeapon(0, 5, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	parser->SetReturn(0);
	return 1;
}

int AI_TurnAway()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_TurnAway", 1);
	auto npc = GetNpc("AI_TurnAway", 1);
	if ( npc && other ) {
		auto msg = new oCMsgMovement(EV_TURNAWAY, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_TurnToNPC()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_TurnToNPC", 1);
	auto npc = GetNpc("AI_TurnToNPC", 1);
	if ( npc && other ) {
		auto msg = new oCMsgMovement(EV_TURNTOVOB, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_WhirlAround()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_WhirlAround", 1);
	auto npc = GetNpc("AI_WhirlAround", 1);
	if ( npc && other ) {
		auto msg = new oCMsgMovement(EV_WHIRLAROUND, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_WhirlAroundToSource()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_WhirlAroundToSource", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_WHIRLAROUND, npc->soundPosition);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_TurnToSound()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_TurnToSound", 1);
	if ( npc ) {
		auto msg = new oCMsgMovement(EV_TURNTOPOS, npc->soundPosition);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_QuickLook()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_QuickLook", 1);
	auto npc = GetNpc("AI_QuickLook", 1);
	if ( npc && other ) {
		auto msg = new oCMsgConversation(EV_QUICKLOOK, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 0;
}

int AI_LookAt()
{
	auto parser = zCParser::GetParser();

	zSTRING point;
	parser->GetParameter(point);

	auto npc = GetNpc("AI_LookAt", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_LOOKAT, point);
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		Script_Warning("... name: " + point);
	}
	return 0;
}

int AI_LookAtNpc()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_LookAtNpc", 1);
	auto npc = GetNpc("AI_LookAtNpc", 1);
	if ( npc && other ) {
		auto msg = new oCMsgConversation(EV_LOOKAT, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 0;
}

int AI_StopLookAt()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_StopLookAt", 1);
	if ( npc ) {
		auto msg = oCMsgConversation(EV_STOPLOOKAT);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 0;
}

int AI_PointAt()
{
	auto parser = zCParser::GetParser();

	zSTRING point;
	parser->GetParameter(point);

	auto npc = GetNpc("AI_PointAt", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_POINTAT, point);
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		Script_Warning("... name: " + point);
	}
	return 0;
}

int AI_PointAtNpc()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_PointAtNpc", 1);
	auto npc = GetNpc("AI_PointAtNpc", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_POINTAT, other);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 0;
}

int AI_StopPointAt()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_StopPointAt", 1);
	if ( npc ) {
		auto msg = oCMsgConversation(EV_POINTLOOKAT);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 0;
}

int AI_StandUp()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_StandUp", 1);
	if ( npc ) {
		auto msg = oCMsgMovement(EV_STANDUP, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_StandUpQuick()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_StandupQuick", 1);
	if ( npc ) {
		auto msg = oCMsgMovement(EV_STANDUP, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_Flee()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_Flee", 1);
	if ( npc ) {
		if ( npc->GetEM->IsEmpty(1) )
			npc->Fleeing();
	}
	return 1;
}

int AI_AimAt()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_AimAt", 1);
	auto npc = GetNpc("AI_AimAt", 1);

	auto msg = new oCMsgAttack(6, other, 0);

	npc->GetEM()->OnMessage(msg, npc);
	return 0;
}

int AI_StopAim()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_StopAim", 1);

	auto msg = new oCMsgAttack(8, 0, 1);

	npc->GetEM()->OnMessage(msg, npc);
	return 0;
}

int AI_ShootAt()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_AimAt", 1);
	auto npc = GetNpc("AI_AimAt", 1);

	auto msg = new oCMsgAttack(7, other, 0);

	npc->GetEM()->OnMessage(msg, npc);
	return 0;
}

int AI_Defend()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_Defenc", 1);
	auto em = npc->GetEM(0);

	for (int i = 0, num = em->GetEventMessage(i); i < num; ++i) {
		auto msg = dynamic_cast<oCMsgAttack>(em->GetEventMessage(i));
		if (msg && msg->subType == 9)
			return 0;
	}

	auto msg = new oCMsgAttack(9, 0, 1);
	em->OnMessage(msg, npc);

	return 0;
}

int AI_CombatReactToDamage()
{
	return 1;
}

int AI_ReadySpell()
{
	auto parser = zCParser::GetParser();

	int spell;
	int investMana;
	parser->GetParameter(investMana);
	parser->GetParameter(spell);

	auto npc = GetNpc("AI_ReadySpell", 1);
	if ( npc ) {
		auto msg = new oCMsgMagic(9);
		msg->spellId = spell;
		msg->investMana = investMana;
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_UnreadySpell()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_UnreadySpell", 1);
	if ( npc ) {
		auto msg = new oCMsgMagic(10);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int Npc_HasSpell()
{
	auto parser = zCParser::GetParser();

	int spell;
	parser->GetParameter(spell);

	auto npc = GetNpc("Npc_HasSpell", 1);
	oCItem* item = nullptr;
	if ( npc )
		item = npc->GetSpellItem(spell);
	parser->SetReturn(item != 0);
	return 1;
}

int AI_Attack()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_Attack", 1);
	if (npc && npc->GetEM(0)->IsEmpty(1))
		npc->Fighting();
	return 1;
}

int AI_FinishingMove()
{
	auto parser = zCParser::GetParser();

	auto other = GetNpc("AI_FinishingMove", 1);
	auto npc = GetNpc("AI_FinishingMove", 1);
	if ( npc && other && npc->GetAnictrl() && npc->GetModel() ) {
		int weapMode = npc->GetWeaponMode();
		zSTRING hitStr = oCAniCtrl_Human::GetWeaponHitString(weapMode);
		zSTRING aniStr = "T_" + hitStr + "FINISH";

		zCModelPrototype* proto = npc->GetModel()->prototypes[0];
		zCModelAni* ani = proto->SearchAni(aniStr);

		int aniId = ani ? ani->aniId : -1;

		auto msg = new oCMsgAttack(4, aniId, 1);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_Dodge()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_Dodge", 1);
	if ( npc ) {
		auto msg = oCMsgMovement(EV_DODGE, 0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_UseItem()
{
	auto parser = zCParser::GetParser();

	int instanceItem;
	parser->GetParameter(instanceItem);

	auto npc = GetNpc("AI_Dodge", 1);

	if ( npc ) {
		auto sym = parser->GetSymbol(instanceItem);
		auto name = sym ? sym->name ? "";
		if (name == "ITEM") {
			zCVob* vob = sym->GetInstanceAdr();
			oCItem* item = dynamic_cast<oCItem>(vob);
			if ( item )
				name = item->GetInstanceName();
		}
		if (!name.IsEmpty()) {
			auto msg = new oCMsgManipulate(name, -1);
			npc->GetEM()->OnMessage(msg, npc);
		}
	} else {
		auto sym = parser->GetSymbol(instanceItem);
		auto name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("... item-name: " + name);
	}
	return 1;
}

int AI_UseItemToState()
{
	auto parser = zCParser::GetParser();

	int state;
	int instanceItem;
	parser->GetParameter(instanceItem);
	parser->GetParameter(state);

	auto npc = GetNpc("AI_UseItemToState", 0)
	if ( npc ) {
		auto sym = parser->GetSymbol(instanceItem);
		auto name = sym ? sym->name ? "";
		if (name == "ITEM") {
			zCVob* vob = sym->GetInstanceAdr();
			oCItem* item = dynamic_cast<oCItem>(vob);
			if ( item )
				name = item->GetInstanceName();
		}
		if (!name.IsEmpty()) {
			auto msg = new oCMsgManipulate(15, name, state);
			npc->GetEM()->OnMessage(msg, npc);
		}
	} else {
		auto sym = parser->GetSymbol(instanceItem);
		auto name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning("... item-name: " + name);
	}
	return 1;
}

int Npc_RefuseTalk()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("Npc_CanTalk", 1);
	bool ret = false;
	if ( npc )
		ret = npc->CanTalk() == 0;
	parser->SetReturn(ret);
	return 0;
}

int Npc_SetRefuseTalk()
{
	auto parser = zCParser::GetParser();

	int timeSec;
	parser->GetParameter(timeSec);
	auto npc = GetNpc("Npc_SetCanTalk", 1);
	if ( npc )
		npc->SetCanTalk(timeSec);
	return 0;
}

int AI_Ask()
{
	auto parser = zCParser::GetParser();

	int funcYes;
	int funcNo;
	parser->GetParameter(funcNo);
	parser->GetParameter(funcYes);
	auto npc = GetNpc("AI_Ask", 1);
	if (npc) {
		auto msg = new oCMsgConversation(EV_ASK, funcYes, funcNo, 20000.0);
		msg->name = "Nein";
		msg->text = "Ja";
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_AskText()
{
	auto parser = zCParser::GetParser();

	int funcYes;
	int funcNo;
	zSTRING textYes;
	zSTRING textNo;
	parser->GetParameter(textNo);
	parser->GetParameter(textYes);
	parser->GetParameter(funcNo);
	parser->GetParameter(funcYes);

	auto npc = GetNpc("AI_AskText", 1);
	if ( npc ) {
		auto msg = new CMsgConversation(EV_ASK, funcYes, funcNo, 20000.0);
		msg->name = textNo;
		msg->text = textYes;

		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_WaitForQuestion()
{
	auto parser = zCParser::GetParser();

	int action;
	parser->GetParser(action);

	auto npc = GetNpc("AI_WaitForQuestion", 1);
	if ( npc ) {
		auto msg = new oCMsgConversation(EV_WAITFORQUESTION, action, action, 20000.0);
		npc->GetEM()->OnMessage(msg, npc);
	}
	return 1;
}

int AI_Snd_Play()
{
	auto parser = zCParser::GetParser();

	zSTRING snd;
	parser->GetParameter(snd);

	auto npc = GetNpc("AI_Snd_Play", 1);
	if ( npc ) {
		auto msg = oCMsgConversation(EV_SNDPLAY, snd);
		msg->posY = 0;
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		Script_Warning("... sndName: ", snd);
	}
	return 0;
}

int AI_Snd_Play3D()
{
	auto parser = zCParser::GetParser();

	zSTRING snd;
	parser->GetParameter(snd);

	int otherIndex;
	zCVob* other = parser->GetInstanceAndIndex(otherIndex);
	auto npc = GetNpc("AI_Snd_Play3D",, 0);

	if ( npc && other ) {
		auto msg = oCMsgConversation(EV_SNDPLAY, snd);
		msg->target = other;
		msg->posY = 1;
		npc->GetEM()->OnMessage(msg, npc);
	} else {
		auto sym = parser->GetSymbol(otherIndex);
		auto name = sym ? sym->name : "[UNKNOWN]";
		Script_Warning(" ...emitterName: " + name);
	}
	return 0;
}
