//_ulf/oGameExternal.cpp
//----------- INTERNAL HELPERS -----------
void ScriptWarning(zSTRING msg, bool print_func = true)
{
	if ( script_warn_type == 0)
		return;

	if ( script_warn_type == 1 ) {
		zERROR::Warning("C: SCRIPT: " + msg); // 252
	} else {
		static bool reported_error = false;
		if ( !reported_error )
			zERROR::Fault("C: SCRIPT: " + msg); // 256
		reported_error = true;
		zERROR::Warning("C: SCRIPT: " + msg); // 257
	}

	if (print_func) {
		auto cur_func = zparser->GetCurrentFunc();
		auto sym = zparser->GetSymbol(cur_func);
		if (!sym)
			return;

		zERROR::Warning("C: SCRIPT: last parser func-name: " + sym->name); // 262
	}
}

oCNpc* GetNpc(zSTRING& funcname, int showfunc)
{
	auto parser = zCParser::GetParser();
	if ( script_warn_type == 0)
		return dynamic_cast<oCNpc*>(parser->GetInstance());

	int idx;
	zCVob* inst = parser->GetInstanceAndIndex(&index);
	auto npc = dynamic_cast<oCNpc*>(inst);

	zCVob* offs = parser->GetSymbol(index)->GetOffset(sym);
	if ( auto npc = dynamic_cast<oCNpc*>(offs) )
		return npc;
	else if ( dynamic_cast<oCItem*>(offs) )
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is an item and not a npc. ", showfunc);
	else if ( dynamic_cast<oCMob*>(offs) ) 
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is a mob and not a npc. ", showfunc);
	else if ( dynamic_cast<zCVob*>(offs) ) 
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is invalid (bug? -> contact me!). ", showfunc);
	else
		ScriptWarning( funcname + "(): illegal param: \"" + sym->name + "\" is NULL.", showfunc);

	return npc;
}

oCItem* GetItem(zSTRING& funcname, int showfunc)
{

	auto parser = zCParser::GetParser();
	if ( script_warn_type == 0)
		return dynamic_cast<oCItem*>(parser->GetInstance());


	int idx;
	zCVob* inst = parser->GetInstanceAndIndex(&index);
	auto item = dynamic_cast<oCItem*>(inst);

	zCVob* offs = parser->GetSymbol(index)->GetOffset(sym);
	if ( auto item = dynamic_cast<oCItem*>(offs) )
		return item;
	else if ( dynamic_cast<oCNpc*>(offs) )
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is an npc and not an item. ", showfunc); // WASN'T in original code
	else if ( dynamic_cast<oCMob*>(offs) ) 
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is a mob and not an item. ", showfunc);
	else if ( dynamic_cast<zCVob*>(offs) ) 
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is invalid (bug? -> contact me!). ", showfunc);
	else
		ScriptWarning(funcname + "(): illegal param: \"" + sym->name + "\" is NULL.", showfunc);


	return item;
}

oCNpc* oldSelfNpc;
oCNpc* oldOtherNpc;
oCNpc* oldVictimNpc;

void Script_SaveNpcGlobals()
{
	oldSelfNpc = 0;
	oldOtherNpc = 0;
	oldVictimNpc = 0;

	auto* slf = (zCVob*)zparser->GetSymbol("SELF")->GetInstanceAdr();
	if (slf)
		oldSelfNpc = dynamic_cast<oCNpc*>(slf);

	auto* oth = (zCVob*)zparser->GetSymbol("OTHER")->GetInstanceAdr();
	if (oth)
		oldOtherNpc = dynamic_cast<oCNpc*>(oth);

	auto* vic = (zCVob*)zparser->GetSymbol("VICTIM")->GetInstanceAdr();
	if (vic)
		oldVictimNpc = dynamic_cast<oCNpc*>(vic);
}

void Script_RestoreNpcGlobals()
{
	zparser->SetInstance("SELF", oldSelfNpc);
	zparser->SetInstance("OTHER", oldOtherNpc);
	zparser->SetInstance("VICTIM", oldVictimNpc);
}

//----------- GAME -----------
int Game_InitGerman()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}

int Game_InitEnglish()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}

int Game_InitEngIntl()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}

int IntroduceChapter()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING chapter;
	zSTRING text;
	zSTRING fileTGA;
	zSTRING soundWAV;
	zSTRING time;
	parser->GetParameter(time);
	parser->GetParameter(soundWAV);
	parser->GetParameter(fileTGA);
	parser->GetParameter(text);
	parser->GetParameter(chapter);
	zERROR::Message("B: Introducing chapter: " + chapter + " (" + time + "ms)");
	CGameManager::IntroduceChapter(chapter, text, fileTGA, fileWAV, time);
	zERROR::Message("B: Introducing chapter ended.");
	zCParser::SetReturn(parser, 0);
	return 1;
}

int SetPercentDone(void *this)
{
	zCParser* parser = zCParser::GetParser();
	int percent;
	parser->GetParameter(percent);
	parser->SetPercentDone(percent);
	return 1;
}

int Print()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING text;
	parser->GetParameter(&text);

	ogame->GetTextView()->PrintWin(text + "");

	return 0;
}

int PrintDebug()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING text;
	parser->GetParameter(&text);

	if (zCView::GetShowDebug())
		zERROR::Message("U:Skript: " + text);

	return 0;
}

int PrintScreen()
{
	int unk;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&unk);

	if (text.IsEmpty()) {
		parser->SetReturn(1);
		// return 0;
	}

	zCView* view = ogame->array_view[0];
	if ( view ) {
		int ret = 0;
		if (!font.IsEmpty()) {
			zSTRING tmp;
			view->SetFont(font);
			view->GetFont()->GetFontName(tmp);

			ret = font == tmp;
		}

		int x1 = (int)((double)x * 81.919998);
		int y1 = (int)((double)y * 81.919998);
		double time = double(1000 * timeSec);

		if ( x == -1 && y == -1)
		{
			game->array_view[0]->PrintTimedCXY(&text, time, 0);
		}
		else if ( x == -1)
		{
			game->array_view[0]->PrintTimedCX(y1, &text, time, 0);
		}
		else if ( y == -1 )
		{
			game->array_view[0]->PrintTimedCY(x1, &text, time, 0);
		}
		else
		{
			game->array_view[0]->PrintTimed(x1, y1, &text, time, 0);
		}

		parser->SetReturn(ret);
	} else {
		parser->SetReturn(0);
	}
	return 0;
}

int PrintDialog()
{
	int dialogNr;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&dialogNr);
	if ( !ogame->array_view[0] )
	{
		zCParser::SetReturn(parser, 0);
		return 0;
	}

	if ( oCNpc::player )
	{
		auto* omsg = new oCMsgConversation(21, oCNpc::player);
		omsg->posX = x;
		omsg->posY = y;
		omsg->timeSec = (double)timeSec;
		omsg->dialogNr = dialogNr;
		omsg->font = font;
		omsg->text = text;

		evtmgr = oCNpc::player->GetEM(0);
		evtmgr->OnMessage(omsg, oCNpc::player);
	}
	zCParser::SetReturn(parser, 1);
	return 0;
}

int PrintDialog()
{
	int dialogNr;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&dialogNr);
	if ( !ogame->array_view[dialogNr] ) {
		zCParser::SetReturn(parser, 0);
		return 0;
	}
	int retval = 0;
	oCGame::OpenView(ogame, dialogNr);
	auto* view = ogame->array_view[dialogNr];
	if ( font.size() != 0 )
	{
		view->SetFont(&font);
		auto vfont = view->GetFont();

		retval = (vfont.GetFontName() == font);
	}
	xf = (double)x * 81.919998;
	yf = (double)x * 81.919998;
	zCOLOR color;
	color.color[2] = -1;
	color.color[1] = -1;
	color.color[0] = -1;
	color.color[3] = -1;
	double time = 1000 * timeSec;
	if ( x == -1 && y == -1)
	{
		view->DialogCXY(&text, time, color);
	}
	else if ( y == -1 )
	{
		view->DialogCY(xf, &text, time, color);
	}
	else
	{
		view->Dialog(xf, yf, &text, time, color);
	}
	zCParser::SetReturn(parser, fontfound);
	return 0;
}

int PrintDebugInst()
{
	zCParser *parser = zCParser::GetParser();
	auto* self = parser->GetSymbol("SELF");
	zVob* selfinst = nullptr;
	if (self)
		selfinst = (zCVob *)self.GetInstanceAdr();
	zSTRING text;
	parser->GetParameter(&text);
	if (ogame->IsDebuggingAllInstances() ||
	    ogame->IsDebuggingInstance(slfinst) )
	{
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + msg);
		}
	}
	return 0;
}

int PrintDebugInstCh()
{
	zCParser *parser = zCParser::GetParser();
	auto* self = parser->GetSymbol("SELF");
	zVob* selfinst = nullptr;
	if (self)
		selfinst = (zCVob *)self.GetInstanceAdr();
	zSTRING text;
	int channel;
	parser->GetParameter(&text);
	parser->GetParameter(&channel);
	if ((ogame->IsDebuggingAllInstances() ||
	     ogame->IsDebuggingInstance(slfinst)) &&
	    (ogame->IsDebuggingChannel(channel)) {
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + text);
		}
	}
	return 0;
}

int PrintDebugCh()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING text;
	int channel;
	parser->GetParameter(&text);
	parser->GetParameter(&channel);
	if (ogame->IsDebuggingChannel(channel)) {
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + text);
		}
	}
	return 0;
}


int PrintMulti()
{
	zSTRING strings[5];
	zCParser* parser = zCParser::GetParser();
	v3 = 5;
	for (int i = 5; i; --i)
		zCParser::GetParameter(parser, strings[i]);
	int k = 0;
	while (k < 5) {
		if ( strings[k].Length() <= 0 )
			break;
		++k;
	}
	zCView* view = ogame->vtbl->GetTextView();
	view->Printwin(strings[rand() % k]);
	return 0;
}

int ExitGame()
{
	if (gameMan)
		gameMan->ExitGame();
	return 1;
}

int ExitGame()
{
	if (gameMan)
		gameMan->ExitSession();
	return 1;
}

int PlayVideo()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING videoBIK;
	parser->GetParameter(videoBIK);
	int ret = gameMan->PlayVideo(videoBIK);
	parser->SetReturn(ret);
}

int PlayVideoEx()
{
	zCParser* parser = zCParser::GetParser();
	int exitSession;
	int blendTime;
	zSTRING videoBIK;
	parser->GetParameter(exitSession);
	parser->GetParameter(blendTime);
	parser->GetParameter(videoBIK);
	int ret = gameMan->PlayVideoEx(videoBIK, blendTime, exitSession);
	parser->SetReturn(ret);
}


//----------- TYPE CONVERSIONS -----------
int IntToString()
{
	static zSTRING result;
	zCParser* parser = zCParser::GetParser();

	int value;
	parser.GetParameter(&value);
	result = zSTRING(value);
	parser.SetReturn(&result);
	return 0;
}

int FloatToString()
{
	static zSTRING result;
	zCParser* parser = zCParser::GetParser();

	float value;
	parser.GetParameter(&value);
	result = zSTRING(value);
	parser.SetReturn(&result);
	return 0;
}

int FloatToInt()
{
	float value;
	parser.GetParameter(&value);
	parser.SetReturn(int(value));
	return 0;
}

int IntToFloat()
{
	int value;
	parser.GetParameter(&value);
	parser.SetReturn(float(value));
	return 0;
}

int ConcatStrings()
{
	static zSTRING text1;
	zCParser* parser = zCParser::GetParser();

	zSTRING text2;
	parser.GetParameter(&text2);
	parser.GetParameter(&text1);
	text1 += text2;
	parser.SetReturn(&text1);
	return 0;
}

//----------- GUILDS -----------
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

//----------- MEMORY -----------
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

//----------- STATES -----------
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

int AI_SetNpcsToState()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING funcname = "AI_SetNpcsToState";
	int state = 0;
	int radius = 0;
	parser->GetParameter(&radius);
	parser->GetParameter(&state);

	oCNpc* npc = GetNpc(funcname, 1);

	if ( npc )
	{
		auto msgState = new oCMsgState(2, state);
		msgState->radius = radius;
		npc->GetEM()->OnMessage(msgState, npc);
	}
	return 0;
}

int AI_ContinueRoutine()
{
	auto parser = zCParser::GetParser();

	auto npc = GetNpc("AI_ContinueRoutine", 1);
	if ( npc ) {
		auto msg1 = new oCMsgMovement(EV_STANDUP, 0);
		npc->GetEM()->OnMessage(msg1, npc);

		auto msg2 = new oCMsgConversation(EV_STOPLOOKAT);
		npc->GetEM()->OnMessage(msg2, npc);

		auto msg3 = new oCMsgConversation(EV_STOPPOINTAT);
		npc->GetEM()->OnMessage(msg3, npc);

		auto msg4 = new oCMsgWeapon(3, 0, 0);
		npc->GetEM()->OnMessage(msg4, npc);

		auto msg5 = new oCMsgState(0, 0);
		npc->GetEM()->OnMessage(msg5, npc);
	}
	return 0;
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

int Hlp_CutscenePlayed()
{
	zCParser *parser = zCParser::GetParser();
	zCCSManager *csman = ogame->GetCutsceneManager();

	zSTRING csName;
	parser->GetParameter(&csName);
	int numplayed = csman->PoolNumPlayed(csName);
	parser->SetReturn(numplayed);
	return 0;
}

int Hlp_GetNpc()
{
	zCParser *parser = zCParser::GetParser();

	oCNpc* npc = nullptr;

	int instanceId;
	parser->GetParameter(instanceId);
	zCPar_Symbol* symbol = parser->GetSymbol(instanceId);

	if ( symbol ) {
		if (symbol->type == zPAR_TYPE_INSTANCE) {
			zCVob* adr = (zCVob*)symbol->GetInstanceAdr();
			npc = dynamic_cast<oCNpc*>(adr);
		}

		if (!npc) {
			zCWorld* world = ogame->GetGameWorld();
			zCVob* vob = world->SearchVobByName(symbol->name);

			npc = zDYNAMIC_CAST<oCNpc>(npc);
		}
	}

	zCParser::SetReturn(parser, npc);
	return 0;
}

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
	target = zDYNAMIC_CAST<oCNpc>(inst);
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

void oCGame::DefineExternals_Ulfi(zCParser* parser)
{
	parser->DefineExternal("IntToString", IntToString, 3, 2, 0);
	parser->DefineExternal("FloatToString", FloatToString, 3, 1, 0);
	parser->DefineExternal("FloatToInt", FloatToInt, 2, 1, 0);
	parser->DefineExternal("IntToFloat", IntToFloat, 1, 2, 0);
	parser->DefineExternal("ConcatStrings", ConcatStrings, 3, 3, 3, 0);
	parser->DefineExternal("Print", Print, 0, 3, 0);
	parser->DefineExternal("PrintDebug", PrintDebug, 0, 3, 0);
	parser->DefineExternal("PrintScreen", PrintScreen, 2, 3, 2, 2, 3, 2, 0);
	parser->DefineExternal("AI_PrintScreen", AI_PrintScreen, 2, 3, 2, 2, 3, 2, 0);
	parser->DefineExternal("PrintDialog", PrintDialog, 2, 2, 3, 2, 2, 3, 2, 0);
	parser->DefineExternal("PrintDebugInst", PrintDebugInst, 0, 3, 0);
	parser->DefineExternal("PrintDebugInstCh", PrintDebugInstCh, 0, 2, 3, 0);
	parser->DefineExternal("PrintDebugCh", PrintDebugCh, 0, 2, 3, 0);
	parser->DefineExternal("PrintMulti", PrintMulti, 0, 3, 3, 3, 3, 3, 0);
	parser->DefineExternal("ExitGame", ExitGame, 0, 0);
	parser->DefineExternal("ExitSession", ExitSession, 0, 0);
	parser->DefineExternal("PlayVideo", PlayVideo, 2, 3, 0);
	parser->DefineExternal("PlayVideoEx", PlayVideoEx, 2, 3, 2, 2, 0);
	parser->DefineExternal("SetPercentDone", SetPercentDone, 0, 2, 0);
	parser->DefineExternal("IntroduceChapter", IntroduceChapter, 0, 3, 3, 3, 3, 2, 0);
	parser->DefineExternal("Npc_MemoryEntry", Npc_MemoryEntry, 0, 7, 2, 7, 2, 7, 0);
	parser->DefineExternal("Npc_MemoryEntryGuild", Npc_MemoryEntryGuild, 0, 7, 2, 7, 2, 7, 0);
	parser->DefineExternal("Npc_HasNews", Npc_HasNews, 2, 7, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetNewsWitness", Npc_GetNewsWitness, 7, 7, 2, 0);
	parser->DefineExternal("Npc_GetNewsOffender", Npc_GetNewsOffender, 7, 7, 2, 0);
	parser->DefineExternal("Npc_GetNewsVictim", Npc_GetNewsVictim, 7, 7, 2, 0);
	parser->DefineExternal("Npc_IsNewsGossip", Npc_IsNewsGossip, 2, 7, 2, 0);
	parser->DefineExternal("Npc_DeleteNews", Npc_DeleteNews, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GetGuildAttitude", Npc_GetGuildAttitude, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetActiveSpell", Npc_GetActiveSpell, 2, 7, 0);
	parser->DefineExternal("Npc_GetLastHitSpellID", Npc_GetLastHitSpellID, 2, 7, 0);
	parser->DefineExternal("Npc_GetLastHitSpellCat", Npc_GetLastHitSpellCat, 2, 7, 0);
	parser->DefineExternal("Npc_GetActiveSpellCat", Npc_GetActiveSpellCat, 2, 7, 0);
	parser->DefineExternal("Npc_GetActiveSpellLevel", Npc_GetActiveSpellLevel, 2, 7, 0);
	parser->DefineExternal("Npc_GetActiveSpellIsScroll", Npc_GetActiveSpellIsScroll, 2, 7, 0);
	parser->DefineExternal("Npc_SetActiveSpellInfo", Npc_SetActiveSpellInfo, 2, 7, 2, 0);
	parser->DefineExternal("Wld_DetectItem", Wld_DetectItem, 2, 7, 2, 0);
	parser->DefineExternal("Wld_DetectPlayer", Wld_DetectPlayer, 2, 7, 0);
	parser->DefineExternal("Wld_DetectNpc", Wld_DetectNpc, 2, 7, 2, 5, 2, 0);
	parser->DefineExternal("Wld_DetectNpcEx", Wld_DetectNpcEx, 2, 7, 2, 5, 2, 2, 0);
	parser->DefineExternal("Wld_DetectNpcExAtt", Wld_DetectNpcExAtt, 2, 7, 2, 5, 2, 2, 2, 0);
	parser->DefineExternal("Wld_SetGuildAttitude", Wld_SetGuildAttitude, 0, 2, 2, 2, 0);
	parser->DefineExternal("Wld_GetGuildAttitude", Wld_GetGuildAttitude, 2, 2, 2, 0);
	parser->DefineExternal("Wld_IsMobAvailable", Wld_IsMobAvailable, 2, 7, 3, 0);
	parser->DefineExternal("Wld_IsFPAvailable", Wld_IsFPAvailable, 2, 7, 3, 0);
	parser->DefineExternal("Wld_IsNextFPAvailable", Wld_IsNextFPAvailable, 2, 7, 3, 0);
	parser->DefineExternal("Npc_CreateSpell", Npc_CreateSpell, 0, 7, 2, 0);
	parser->DefineExternal("Npc_LearnSpell", Npc_LearnSpell, 0, 7, 2, 0);
	parser->DefineExternal("Npc_HasItems", Npc_HasItems, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GiveItem", Npc_GiveItem, 0, 7, 2, 7, 0);
	parser->DefineExternal("CreateInvItem", Npc_CreateInvItem, 0, 7, 2, 0);
	parser->DefineExternal("CreateInvItems", Npc_CreateInvItems, 0, 7, 2, 2, 0);
	parser->DefineExternal("Npc_GetInvItem", Npc_GetInvItem, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GetInvItemBySlot", Npc_GetInvItemBySlot, 2, 7, 2, 2, 0);
	parser->DefineExternal("Npc_RemoveInvItem", Npc_RemoveInvItem, 2, 7, 2, 0);
	parser->DefineExternal("Npc_RemoveInvItems", Npc_RemoveInvItems, 2, 7, 2, 2, 0);
	parser->DefineExternal("Npc_ClearInventory", Npc_ClearInventory, 0, 7, 0);
	parser->DefineExternal("Npc_IsInState", Npc_IsInState, 2, 7, 5, 0);
	parser->DefineExternal("Npc_WasInState", Npc_WasInState, 2, 7, 5, 0);
	parser->DefineExternal("Npc_IsInRoutine", Npc_IsInRoutine, 2, 7, 5, 0);
	parser->DefineExternal("AI_SetNpcsToState", AI_SetNpcsToState, 0, 7, 5, 2, 0);
	parser->DefineExternal("Npc_HasDetectedNpc", Npc_HasDetectedNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_SetAttitude", Npc_SetAttitude, 0, 7, 2, 0);
	parser->DefineExternal("Npc_SetTempAttitude", Npc_SetTempAttitude, 0, 7, 2, 0);
	parser->DefineExternal("Npc_GetAttitude", Npc_GetAttitude, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetPermAttitude", Npc_GetPermAttitude, 2, 7, 7, 0);
	parser->DefineExternal("Npc_ChangeAttribute", Npc_ChangeAttribute, 0, 7, 2, 2, 0);
	parser->DefineExternal("Npc_GetComrades", Npc_GetComrades, 2, 7, 0);
	parser->DefineExternal("Npc_IsNear", Npc_IsNear, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetDistToNpc", Npc_GetDistToNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetHeightToNpc", Npc_GetHeightToNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetHeightToItem", Npc_GetHeightToItem, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetDistToPlayer", Npc_GetDistToPlayer, 2, 7, 0);
	parser->DefineExternal("Npc_GetDistToItem", Npc_GetDistToItem, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetDistToWP", Npc_GetDistToWP, 2, 7, 3, 0);
	parser->DefineExternal("Npc_CanSeeNpc", Npc_CanSeeNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_CanSeeNpcFreeLOS", Npc_CanSeeNpcFreeLOS, 2, 7, 7, 0);
	parser->DefineExternal("Npc_CanSeeItem", Npc_CanSeeItem, 2, 7, 7, 0);
	parser->DefineExternal("Npc_IsPlayer", Npc_IsPlayer, 2, 7, 0);
	parser->DefineExternal("Npc_KnowsPlayer", Npc_KnowsPlayer, 2, 7, 7, 0);
	parser->DefineExternal("Npc_SetKnowsPlayer", Npc_SetKnowsPlayer, 0, 7, 7, 0);
	parser->DefineExternal("Npc_IsInFightMode", Npc_IsInFightMode, 2, 7, 2, 0);
	parser->DefineExternal("Npc_IsAiming", Npc_IsAiming, 2, 7, 7, 0);
	parser->DefineExternal("Npc_StartItemReactModules", Npc_StartItemReactModules, 2, 7, 7, 7, 0);
	parser->DefineExternal("Npc_HasOffered", Npc_HasOffered, 2, 7, 7, 2, 0);
	parser->DefineExternal("EquipItem", EquipItem, 0, 7, 2, 0);
	parser->DefineExternal("Npc_SetToFistMode", Npc_SetToFistMode, 0, 7, 0);
	parser->DefineExternal("Npc_SetToFightMode", Npc_SetToFightMode, 0, 7, 2, 0);
	parser->DefineExternal("Npc_IsDead", Npc_IsDead, 2, 7, 0);
	parser->DefineExternal("AI_StartState", AI_StartState, 0, 7, 5, 2, 3, 0);
	parser->DefineExternal("AI_ContinueRoutine", AI_ContinueRoutine, 0, 7, 0);
	parser->DefineExternal("Npc_GetStateTime", Npc_GetStateTime, 2, 7, 0);
	parser->DefineExternal("Npc_SetStateTime", Npc_SetStateTime, 0, 7, 2, 0);
	parser->DefineExternal("AI_Output", AI_Output, 0, 7, 7, 3, 0);
	parser->DefineExternal("AI_OutputSVM", AI_OutputSVM, 0, 7, 7, 3, 0);
	parser->DefineExternal("AI_OutputSVM_Overlay", AI_OutputSVM_Overlay, 0, 7, 7, 3, 0);
	parser->DefineExternal("AI_PlayCutscene", AI_PlayCutscene, 0, 7, 3, 0);
	parser->DefineExternal("AI_CanSeeNpc", AI_CanSeeNpc, 0, 7, 7, 5, 0);
	parser->DefineExternal("AI_Wait", AI_Wait, 0, 7, 1, 0);
	parser->DefineExternal("AI_Waitms", AI_WaitMS, 0, 7, 2, 0);
	parser->DefineExternal("AI_WaitTillEnd", AI_WaitTillEnd, 0, 7, 7, 0);
	parser->DefineExternal("AI_AlignToWP", AI_AlignToWP, 0, 7, 0);
	parser->DefineExternal("AI_SetWalkMode", AI_SetWalkMode, 0, 7, 2, 0);
	parser->DefineExternal("AI_PlayAni", AI_PlayAni, 0, 7, 3, 0);
	parser->DefineExternal("AI_PlayAniBS", AI_PlayAniBS, 0, 7, 3, 2, 0);
	parser->DefineExternal("AI_GotoWP", AI_GotoWP, 0, 7, 3, 0);
	parser->DefineExternal("AI_Teleport", AI_Teleport, 0, 7, 3, 0);
	parser->DefineExternal("AI_GotoItem", AI_GotoItem, 0, 7, 7, 0);
	parser->DefineExternal("AI_GotoNpc", AI_GotoNpc, 0, 7, 7, 0);
	parser->DefineExternal("AI_AlignToFP", AI_AlignToFP, 0, 7, 0);
	parser->DefineExternal("AI_GotoFP", AI_GotoFP, 0, 7, 3, 0);
	parser->DefineExternal("Npc_IsOnFP", Npc_IsOnFP, 2, 7, 3, 0);
	parser->DefineExternal("AI_GotoNextFP", AI_GotoNextFP, 0, 7, 3, 0);
	parser->DefineExternal("AI_GotoSound", AI_GotoSound, 0, 7, 0);
	parser->DefineExternal("AI_TakeItem", AI_TakeItem, 0, 7, 7, 0);
	parser->DefineExternal("AI_DropItem", AI_DropItem, 0, 7, 2, 0);
	parser->DefineExternal("AI_DrawWeapon", AI_DrawWeapon, 0, 7, 0);
	parser->DefineExternal("AI_RemoveWeapon", AI_RemoveWeapon, 0, 7, 0);
	parser->DefineExternal("Npc_HasRangedWeaponWithAmmo", Npc_HasRangedWeaponWithAmmo, 2, 7, 0);
	parser->DefineExternal("Npc_HasEquippedWeapon", Npc_HasEquippedWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_HasEquippedMeleeWeapon", Npc_HasEquippedMeleeWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_HasEquippedRangedWeapon", Npc_HasEquippedRangedWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_HasEquippedArmor", Npc_HasEquippedArmor, 2, 7, 0);
	parser->DefineExternal("Npc_HasReadiedWeapon", Npc_HasReadiedWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_HasReadiedMeleeWeapon", Npc_HasReadiedMeleeWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_HasReadiedRangedWeapon", Npc_HasReadiedRangedWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_GetTrueGuild", Npc_GetTrueGuild, 2, 7, 0);
	parser->DefineExternal("Npc_SetTrueGuild", Npc_SetTrueGuild, 2, 7, 2, 0);
	parser->DefineExternal("Npc_AreWeStronger", Npc_AreWeStronger, 2, 7, 7, 0);
	parser->DefineExternal("Npc_GetTarget", Npc_GetTarget, 2, 7, 0);
	parser->DefineExternal("Npc_GetNextTarget", Npc_GetNextTarget, 2, 7, 0);
	parser->DefineExternal("Npc_GetLookAtTarget", Npc_GetLookAtTarget, 7, 7, 0);
	parser->DefineExternal("Npc_IsNextTargetAvailable", Npc_IsNextTargetAvailable, 2, 7, 0);
	parser->DefineExternal("Npc_SetTarget", Npc_SetTarget, 0, 7, 7, 0);
	parser->DefineExternal("Npc_GetBodyState", Npc_GetBodyState, 2, 7, 0);
	parser->DefineExternal("Npc_HasBodyFlag", Npc_HasBodyFlag, 2, 7, 2, 0);
	parser->DefineExternal("Npc_IsInCutscene", Npc_IsInCutscene, 2, 7, 0);
	parser->DefineExternal("Npc_IsVoiceActive", Npc_IsVoiceActive, 2, 7, 0);
	parser->DefineExternal("Npc_StopAni", Npc_StopAni, 0, 7, 3, 0);
	parser->DefineExternal("Npc_GetDetectedMob", Npc_GetDetectedMob, 3, 7, 0);
	parser->DefineExternal("Npc_PlayAni", Npc_PlayAni, 0, 7, 3, 0);
	parser->DefineExternal("Npc_IsDetectedMobOwnedByNpc", Npc_IsDetectedMobOwnedByNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_IsDetectedMobOwnedByGuild", Npc_IsDetectedMobOwnedByGuild, 2, 7, 2, 0);
	parser->DefineExternal("Wld_GetMobState", Wld_GetMobState, 2, 7, 3, 0);
	parser->DefineExternal("Npc_ClearAIQueue", Npc_ClearAIQueue, 0, 7, 0);
	parser->DefineExternal("InfoManager_HasFinished", InfoManager_HasFinished, 2, 0);
	parser->DefineExternal("AI_StopProcessInfos", AI_StopProcessInfos, 0, 7, 0);
	parser->DefineExternal("AI_ProcessInfos", AI_ProcessInfos, 0, 7, 0);
	parser->DefineExternal("Info_AddChoice", Info_AddChoice, 0, 2, 3, 5, 0);
	parser->DefineExternal("Info_ClearChoices", Info_ClearChoices, 0, 2, 0);
	parser->DefineExternal("Npc_KnowsInfo", Npc_KnowsInfo, 2, 7, 2, 0);
	parser->DefineExternal("Npc_CheckInfo", Npc_CheckInfo, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GiveInfo", Npc_GiveInfo, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GetTalentSkill", Npc_GetTalentSkill, 2, 7, 2, 0);
	parser->DefineExternal("Npc_GetTalentValue", Npc_GetTalentValue, 2, 7, 2, 0);
	parser->DefineExternal("Npc_SetTalentSkill", Npc_SetTalentSkill, 0, 7, 2, 2, 0);
	parser->DefineExternal("Npc_SetTalentValue", Npc_SetTalentValue, 0, 7, 2, 2, 0);
	parser->DefineExternal("Tal_Configure", Tal_Configure, 0, 2, 2, 0);
	parser->DefineExternal("Npc_GetNearestWP", Npc_GetNearestWP, 3, 7, 0);
	parser->DefineExternal("Npc_GetNextWP", Npc_GetNextWP, 3, 7, 0);
	parser->DefineExternal("Npc_IsWayBlocked", Npc_IsWayBlocked, 2, 7, 0);
	parser->DefineExternal("Npc_GetReadiedWeapon", Npc_GetReadiedWeapon, 7, 7, 0);
	parser->DefineExternal("Npc_GetEquippedMeleeWeapon", Npc_GetEquippedMeleeWeapon, 7, 7, 0);
	parser->DefineExternal("Npc_GetEquippedRangedWeapon", Npc_GetEquippedRangedWeapon, 7, 7, 0);
	parser->DefineExternal("Npc_GetEquippedArmor", Npc_GetEquippedArmor, 7, 7, 0);
	parser->DefineExternal("AI_EquipBestMeleeWeapon", AI_EquipBestMeleeWeapon, 2, 7, 0);
	parser->DefineExternal("AI_EquipBestRangedWeapon", AI_EquipBestRangedWeapon, 2, 7, 0);
	parser->DefineExternal("AI_EquipBestArmor", AI_EquipBestArmor, 0, 7, 0);
	parser->DefineExternal("AI_UnequipWeapons", AI_UnequipWeapons, 0, 7, 0);
	parser->DefineExternal("AI_UnequipArmor", AI_UnequipArmor, 0, 7, 0);
	parser->DefineExternal("AI_EquipArmor", AI_EquipArmor, 0, 7, 2, 0);
	parser->DefineExternal("AI_ReadyMeleeWeapon", AI_ReadyMeleeWeapon, 0, 7, 0);
	parser->DefineExternal("AI_ReadyRangedWeapon", AI_ReadyRangedWeapon, 0, 7, 0);
	parser->DefineExternal("AI_TurnAway", AI_TurnAway, 0, 7, 7, 0);
	parser->DefineExternal("AI_TurnToNPC", AI_TurnToNPC, 0, 7, 7, 0);
	parser->DefineExternal("AI_WhirlAround", AI_WhirlAround, 0, 7, 7, 0);
	parser->DefineExternal("AI_WhirlAroundToSource", AI_WhirlAroundToSource, 0, 7, 0);
	parser->DefineExternal("AI_TurnToSound", AI_TurnToSound, 0, 7, 0);
	parser->DefineExternal("AI_QuickLook", AI_QuickLook, 0, 7, 7, 0);
	parser->DefineExternal("AI_LookAt", AI_LookAt, 0, 7, 3, 0);
	parser->DefineExternal("AI_LookAtNpc", AI_LookAtNpc, 0, 7, 7, 0);
	parser->DefineExternal("AI_StopLookAt", AI_StopLookAt, 0, 7, 0);
	parser->DefineExternal("AI_PointAt", AI_PointAt, 0, 7, 3, 0);
	parser->DefineExternal("AI_PointAtNpc", AI_PointAtNpc, 0, 7, 7, 0);
	parser->DefineExternal("AI_StopPointAt", AI_StopPointAt, 0, 7, 0);
	parser->DefineExternal("AI_Standup", AI_Standup, 0, 7, 0);
	parser->DefineExternal("AI_StandupQuick", AI_StandUpQuick, 0, 7, 0);
	parser->DefineExternal("AI_Flee", AI_Flee, 0, 7, 0);
	parser->DefineExternal("AI_AimAt", AI_AimAt, 0, 7, 7, 0);
	parser->DefineExternal("AI_StopAim", AI_StopAim, 0, 7, 0);
	parser->DefineExternal("AI_ShootAt", AI_ShootAt, 0, 7, 7, 0);
	parser->DefineExternal("AI_Defend", AI_Defend, 0, 7, 0);
	parser->DefineExternal("AI_CombatReactToDamage", AI_CombatReactToDamage, 0, 7, 0);
	parser->DefineExternal("AI_ReadySpell", AI_ReadySpell, 0, 7, 2, 2, 0);
	parser->DefineExternal("AI_UnreadySpell", AI_UnreadySpell, 0, 7, 0);
	parser->DefineExternal("Npc_HasSpell", Npc_HasSpell, 2, 7, 2, 0);
	parser->DefineExternal("AI_Attack", AI_Attack, 0, 7, 0);
	parser->DefineExternal("AI_FinishingMove", AI_FinishingMove, 0, 7, 7, 0);
	parser->DefineExternal("AI_Dodge", AI_Dodge, 0, 7, 0);
	parser->DefineExternal("AI_UseItem", AI_UseItem, 0, 7, 2, 0);
	parser->DefineExternal("AI_UseItemToState", AI_UseItemToState, 0, 7, 2, 2, 0);
	parser->DefineExternal("Npc_RefuseTalk", Npc_RefuseTalk, 2, 7, 0);
	parser->DefineExternal("Npc_SetRefuseTalk", Npc_SetRefuseTalk, 0, 7, 2, 0);
	parser->DefineExternal("AI_Ask", AI_Ask, 0, 7, 5, 5, 0);
	parser->DefineExternal("AI_AskText", AI_AskText, 0, 7, 5, 5, 3, 3, 0);
	parser->DefineExternal("AI_WaitForQuestion", AI_WaitForQuestion, 0, 7, 5, 0);
	parser->DefineExternal("Hlp_IsItem", Hlp_IsItem, 2, 7, 2, 0);
	parser->DefineExternal("Hlp_GetInstanceID", Hlp_GetInstanceID, 2, 7, 0);
	parser->DefineExternal("Hlp_Random", Hlp_Random, 2, 2, 0);
	parser->DefineExternal("Hlp_StrCmp", Hlp_StrCmp, 2, 3, 3, 0);
	parser->DefineExternal("Hlp_GetNpc", Hlp_GetNpc, 7, 2, 0);
	parser->DefineExternal("Hlp_IsValidNpc", Hlp_IsValidNpc, 2, 7, 0);
	parser->DefineExternal("Hlp_IsValidItem", Hlp_IsValidItem, 2, 7, 0);
	parser->DefineExternal("AI_Snd_Play", AI_Snd_Play, 0, 7, 3, 0);
	parser->DefineExternal("AI_Snd_Play3D", AI_Snd_Play3D, 0, 7, 7, 3, 0);
	parser->DefineExternal("Snd_Play", Snd_Play, 0, 3, 0);
	parser->DefineExternal("Snd_Play3d", Snd_Play3d, 0, 7, 3, 0);
	parser->DefineExternal("Snd_GetDistToSource", Snd_GetDistToSource, 2, 7, 0);
	parser->DefineExternal("Snd_IsSourceNpc", Snd_IsSourceNpc, 2, 7, 0);
	parser->DefineExternal("Snd_IsSourceItem", Snd_IsSourceItem, 2, 7, 0);
	parser->DefineExternal("Npc_CanSeeSource", Npc_CanSeeSource, 2, 7, 0);
	parser->DefineExternal("Mis_GetStatus", Mis_GetStatus, 2, 2, 0);
	parser->DefineExternal("Mis_SetStatus", Mis_SetStatus, 0, 2, 2, 0);
	parser->DefineExternal("Mis_OnTime", Mis_SetStatus, 2, 2, 0);
	parser->DefineExternal("Log_CreateTopic", Log_CreateTopic, 0, 3, 2, 0);
	parser->DefineExternal("Log_AddEntry", Log_AddEntry, 0, 3, 3, 0);
	parser->DefineExternal("Log_SetTopicStatus", Log_SetTopicStatus, 0, 3, 2, 0);
	parser->DefineExternal("Mis_AddMissionEntry", Mis_AddMissionEntry, 0, 7, 3, 0);
	parser->DefineExternal("Mis_RemoveMission", Mis_RemoveMission, 0, 7, 0);
	parser->DefineExternal("Npc_CheckAvailableMission", Npc_CheckAvailableMission, 2, 7, 2, 2, 0);
	parser->DefineExternal("Npc_CheckRunningMission", Npc_CheckRunningMission, 2, 7, 2, 0);
	parser->DefineExternal("Npc_IsDrawingWeapon", Npc_IsDrawingWeapon, 2, 7, 0);
	parser->DefineExternal("Npc_IsDrawingSpell", Npc_IsDrawingSpell, 2, 7, 0);
	parser->DefineExternal("Npc_CheckOfferMission", Npc_CheckOfferMission, 2, 7, 2, 0);
	parser->DefineExternal("Mdl_SetVisual", Mdl_SetVisual, 0, 7, 3, 0);
	parser->DefineExternal("Mdl_SetVisualBody", Mdl_SetVisualBody, 0, 7, 3, 2, 2, 3, 2, 2, 2, 0);
	parser->DefineExternal("Mdl_ApplyOverlayMds", Mdl_ApplyOverlayMds, 0, 7, 3, 0);
	parser->DefineExternal("Mdl_ApplyOverlayMdsTimed", Mdl_ApplyOverlayMdsTimed, 0, 7, 3, 2, 0);
	parser->DefineExternal("Mdl_RemoveOverlayMds", Mdl_RemoveOverlayMds, 0, 7, 3, 0);
	parser->DefineExternal("Mdl_ApplyRandomAni", Mdl_ApplyRandomAni, 0, 7, 3, 3, 0);
	parser->DefineExternal("Mdl_ApplyRandomAniFreq", Mdl_ApplyRandomAniFreq, 0, 7, 3, 1, 0);
	parser->DefineExternal("Mdl_SetModelScale", Mdl_SetModelScale, 0, 7, 1, 1, 1, 0);
	parser->DefineExternal("Mdl_SetModelFatness", Mdl_SetModelFatness, 0, 7, 1, 0);
	parser->DefineExternal("Mdl_StartFaceAni", Mdl_StartFaceAni, 0, 7, 3, 1, 1, 0);
	parser->DefineExternal("Mdl_ApplyRandomFaceAni", Mdl_ApplyRandomFaceAni, 0, 7, 3, 1, 1, 1, 1, 1, 0);
	parser->DefineExternal("Wld_IsTime", Wld_IsTime, 2, 2, 2, 2, 2, 0);
	parser->DefineExternal("Wld_GetDay", Wld_GetDay, 2, 0);
	parser->DefineExternal("Wld_IsRaining", Wld_IsRaining, 2, 0);
	parser->DefineExternal("Wld_SetTime", Wld_SetTime, 0, 2, 2, 0);
	parser->DefineExternal("Wld_InsertNpc", Wld_InsertNpc, 0, 2, 3, 0);
	parser->DefineExternal("Wld_SpawnNpcRange", Wld_SpawnNpcRange, 0, 7, 2, 2, 1, 0);
	parser->DefineExternal("Wld_PlayEffect", Wld_PlayEffect, 0, 3, 7, 7, 2, 2, 2, 2, 0);
	parser->DefineExternal("Wld_StopEffect", Wld_StopEffect, 0, 3, 0);
	parser->DefineExternal("AI_PlayFX", AI_PlayFX, 0, 7, 7, 3, 0);
	parser->DefineExternal("AI_StopFX", AI_StopFX, 0, 7, 3, 0);
	parser->DefineExternal("Wld_RemoveNpc", Wld_RemoveNpc, 0, 2, 0);
	parser->DefineExternal("Wld_InsertNpcAndRespawn", Wld_InsertNpcAndRespawn, 0, 2, 3, 2, 0);
	parser->DefineExternal("Wld_InsertItem", Wld_InsertItem, 0, 2, 3, 0);
	parser->DefineExternal("Wld_InsertObject", Wld_InsertObject, 0, 3, 3, 0);
	parser->DefineExternal("Wld_RemoveItem", Wld_RemoveItem, 2, 7, 0);
	parser->DefineExternal("Wld_ExchangeGuildAttitudes", Wld_ExchangeGuildAttitudes, 0, 3, 0);
	parser->DefineExternal("Wld_SetObjectRoutine", Wld_SetObjectRoutine, 0, 2, 2, 3, 2, 0);
	parser->DefineExternal("Wld_SetMobRoutine", Wld_SetMobRoutine, 0, 2, 2, 3, 2, 0);
	parser->DefineExternal("Wld_SendTrigger", Wld_SendTrigger, 0, 3, 0);
	parser->DefineExternal("Wld_SendUntrigger", Wld_SendUntrigger, 0, 3, 0);
	parser->DefineExternal("Npc_OwnedByNpc", Npc_OwnedByNpc, 2, 7, 7, 0);
	parser->DefineExternal("Npc_OwnedByGuild", Npc_OwnedByGuild, 2, 7, 2, 0);
	parser->DefineExternal("AI_TakeMob", AI_TakeMob, 0, 7, 3, 0);
	parser->DefineExternal("AI_DropMob", AI_DropMob, 0, 7, 0);
	parser->DefineExternal("AI_UseMob", AI_MobInteract, 2, 7, 3, 2, 0);
	parser->DefineExternal("Mob_CreateItems", Mob_CreateItems, 0, 3, 2, 2, 0);
	parser->DefineExternal("Mob_HasItems", Mob_HasItems, 2, 3, 2, 0);
	parser->DefineExternal("Doc_Create", Doc_Create, 2, 0);
	parser->DefineExternal("Doc_CreateMap", Doc_CreateMap, 2, 0);
	parser->DefineExternal("Doc_SetPages", Doc_SetPages, 0, 2, 2, 0);
	parser->DefineExternal("Doc_SetPage", Doc_SetPage, 0, 2, 2, 3, 2, 0);
	parser->DefineExternal("Doc_SetFont", Doc_SetFont, 0, 2, 2, 3, 0);
	parser->DefineExternal("Doc_SetLevel", Doc_SetLevel, 0, 2, 3, 0);
	parser->DefineExternal("Doc_SetLevelCoords", Doc_SetLevelCoords, 0, 2, 2, 2, 2, 2, 0);
	parser->DefineExternal("Doc_SetMargins", Doc_SetMargins, 0, 2, 2, 2, 2, 2, 2, 2, 0);
	parser->DefineExternal("Doc_PrintLine", Doc_PrintLine, 0, 2, 2, 3, 0);
	parser->DefineExternal("Doc_PrintLines", Doc_PrintLines, 0, 2, 2, 3, 0);
	parser->DefineExternal("Doc_Show", Doc_Show, 0, 2, 0);
	parser->DefineExternal("Doc_Open", Doc_Open, 0, 3, 0);
	parser->DefineExternal("Doc_Font", Doc_Font, 0, 3, 0);
	parser->DefineExternal("Doc_Print", Doc_Print, 0, 3, 0);
	parser->DefineExternal("Doc_MapCoordinates", Doc_MapCoordinates, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0);
	parser->DefineExternal("TA", TA, 0, 7, 2, 2, 5, 3, 0);
	parser->DefineExternal("TA_Min", TA_Min, 0, 7, 2, 2, 2, 2, 5, 3, 0);
	parser->DefineExternal("TA_CS", TA_CS, 0, 7, 3, 3, 0);
	parser->DefineExternal("TA_BeginOverlay", TA_BeginOverlay, 0, 7, 0);
	parser->DefineExternal("TA_EndOverlay", TA_EndOverlay, 0, 7, 0);
	parser->DefineExternal("TA_RemoveOverlay", TA_RemoveOverlay, 0, 7, 0);
	parser->DefineExternal("Npc_ExchangeRoutine", Npc_ExchangeRoutine, 0, 7, 3, 0);
	parser->DefineExternal("Rtn_Exchange", Rtn_Exchange, 0, 3, 3, 0);
	parser->DefineExternal("Perc_SetRange", Perc_SetRange, 0, 2, 2, 0);
	parser->DefineExternal("Npc_SetPercTime", Npc_SetPercTime, 0, 7, 1, 0);
	parser->DefineExternal("Npc_PercEnable", Npc_PercEnable, 0, 7, 2, 5, 0);
	parser->DefineExternal("Npc_PercDisable", Npc_PercDisable, 0, 7, 2, 0);
	parser->DefineExternal("Npc_SendPassivePerc", Npc_SendPassivePerc, 0, 7, 2, 7, 7, 0);
	parser->DefineExternal("Npc_SendSinglePerc", Npc_SendSinglePerc, 0, 7, 7, 2, 0);
	parser->DefineExternal("Npc_PerceiveAll", Npc_PerceiveAll, 0, 7, 0);
	parser->DefineExternal("Wld_AssignRoomToGuild", Wld_AssignRoomToGuild, 0, 3, 2, 0);
	parser->DefineExternal("Wld_AssignRoomToNpc", Wld_AssignRoomToNpc, 0, 3, 7, 0);
	parser->DefineExternal("Wld_GetPlayerPortalOwner", Wld_GetPlayerPortalOwner, 7, 0);
	parser->DefineExternal("Wld_GetPlayerPortalGuild", Wld_GetPlayerPortalGuild, 2, 0);
	parser->DefineExternal("Wld_GetFormerPlayerPortalOwner", Wld_GetFormerPlayerPortalOwner, 7, 0);
	parser->DefineExternal("Wld_GetFormerPlayerPortalGuild", Wld_GetFormerPlayerPortalGuild, 2, 0);
	parser->DefineExternal("Npc_IsPlayerInMyRoom", Npc_IsPlayerInMyRoom, 2, 7, 0);
	parser->DefineExternal("Npc_IsInPlayersRoom", Npc_IsInPlayersRoom, 2, 7, 0);
	parser->DefineExternal("Npc_WasPlayerInMyRoom", Npc_WasPlayerInMyRoom, 2, 7, 0);
	parser->DefineExternal("Npc_GetPortalOwner", Npc_GetPortalOwner, 7, 7, 0);
	parser->DefineExternal("Npc_GetPortalGuild", Npc_GetPortalGuild, 2, 7, 0);
	parser->DefineExternal("Hlp_CutscenePlayed", Hlp_CutscenePlayed, 2, 3, 0);
	parser->DefineExternal("Game_InitGerman", Game_InitGerman, 2, 0);
	parser->DefineExternal("Game_InitEnglish", Game_InitEnglish, 2, 0);
	parser->DefineExternal("Game_InitEngIntl", Game_InitEngIntl, 2, 0);
}

//------------------------------------------------------------------------------
void  NotifyInfoToSpy(zSTRING& str)
{
	zINFO(8, "R: INF: " + str); // 842
}
