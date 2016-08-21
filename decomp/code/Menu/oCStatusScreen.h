void HandleResultString(zSTRING cheatstr)
{
	zCView *view = ogame->array_view[0];

	oCNpc* focusNpc = oCNpc::player->GetFocusNpc();
	oCNpc* npc = focusNpc;

	zSTRING focusName = focusNpc ? focusNpc->GetName(0) : "";

	zSTRING::Upper(&cheatstr);

	int hash = 0;
	for (size_t i = 0, len = cheatstr.Length(); i < len; ++i) {
		hash += 11 * len * cheatstr[i];
	}
	zERROR::Message("B: TEST: Result: " + cheatstr + "(" + hash + ")");

	oCWorld* world = dynamic_cast<oCWorld*>(ogame->GetWorld());

	switch (hash) {
	case 22374:
		oCNpc::godmode = !oCNpc::godmode;
		if ( !view )
			break;
		if ( oCNpc::godmode )
		{
			view->PrintTimed(100, 300," GUARDIAN-MODE", 3000.0, 0);
			view->PrintTimed(100, 600, "Ja, ich wei-, woher ich stamme!", 4500.0, 0);
			view->PrintTimed(100, 800, "Ungesättigt gleich der Flamme", 5000.0, 0);
			view->PrintTimed(100, 1000, "glühe und verzehr' ich mich.", 5500.0, 0);
			view->PrintTimed(100, 1400, "Licht wird alles, was ich fasse", 6000.0, 0);
			view->PrintTimed(100, 1600, "Kohle alles, was ich lasse, ", 6500.0, 0);
			view->PrintTimed(100, 1900, "Flamme bin ich sicherlich!", 7000.0, 0);
		} else {
			view->PrintTimed(100, 300, "GUARDIAN-MODE off", 3000.0, 0);
		}
		break;
	case 4774: {
		int hpmax = ocnpc::player->getattribute(atr_hitpoints_max);
		int manamax = ocnpc::player->getattribute(atr_mana_max);
		ocnpc::player->setattribute(atr_hitpoints, hpmax);
		ocnpc::player->setattribute(atr_mana, manamax);
		if ( view ) {
			view->PrintTimed(100, 300, "TAKI makes you feal fine", 3000.0, 0);
		}
		break;
	}
	case 8360:
	case 18425:
		   oCNpc::player->PutInInv("ITMI_NUGGET", 1);
		   if ( view ) {
			   view->PrintTimed(100, 300, "some more ore ...", 3000.0, 0);
		   }
		   break;
	case 23112:
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->GetModel()->SetModelScale(zVEC3(0.4,0.4,0.4));
			}
		}
		if ( view ) {
			view->PrintTimed(100, 300, "KINGKONG-POINT-OF-VIEW", 3000.0, 0);
		}
		break;
	case 22748:
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->GetModel()->SetModelScale(zVEC3(5.0,5.0,5.0));
			}
		}
		if ( view )
		{
			view->PrintTimed(100, 300, "GODZILLA MUTATION", 3000.0, 0);
			view->PrintTimed(100, 600, "Kampf der Saurier Mutanten!", 6000.0, 0);
		}
		break;
	case 18128:
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->GetModel()->SetModelScale(zVEC3(0.2,1.3,1.3));
			}
		}
		if (view) {
			view->PrintTimed(100, 300, "GROMMIT-AFTER-CAR-ACCIDENT-MODE", 3000.0, 0);
		}
		break;
	case 12100:
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->SetFatness(-6.0);
			}
		}
		if ( view )
		{
			view->PrintTimed(100, 600, "Deep in the shady sadness of a vale", 4500.0, 0);
			view->PrintTimed(100, 800, "Far sunken from the healthy breath of morn,", 5000.0, 0);
			view->PrintTimed(100, 1000, "Far from the fiery noon, and eve s  one star,", 5500.0, 0);
			view->PrintTimed(100, 1200, "Sat gray-haired  Saturn, quiet as a stone,", 6000.0, 0);
			view->PrintTimed(100, 1400, "Still as the silence round about his lair;", 6500.0, 0);
			view->PrintTimed(100, 1600, "Forest on forest hung above his head", 7000.0, 0);
			view->PrintTimed(100, 1800, "like cloud on cloud ...", 7500.0, 0);
			view->PrintTimed(100, 2000, "-John Keats-", 8000.0, 0);
		}
		break;
	case 22088:
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->SetFatness(6.0);
			}
		}
		if (view) {
			view->PrintTimed(100, 300, "GARFIELD-MODE", 3000.0, 0);
		}
		break;
	case 30282:
		ztimer.factorMotion = 3.0;
		if ( view )
			view->PrintTimed(100, 300, "SOUTHPARK", 3000.0, 0);
		break;
	case 12925:
		ztimer.factorMotion = 0.40;
		if ( view )
			view->PrintTimed(100, 300, "CLERKS", 3000.0, 0);
		break;
	case 9152:
		if ( !npc )
			break;
		npc->GetModel()->__anispeed = 0.4;
		if ( view )
		{
			view->PrintTimed(100, 600, "Kalt sei Hand, Herz und Gebein,", 4500.0, 0);
			view->PrintTimed(100, 800, "Kalt der Schlaf unterm Stein:,", 5000.0, 0);
			view->PrintTimed(100, 1000, "Nimmer steh vom Bette auf,", 5500.0, 0);
			view->PrintTimed(100, 1200, "Eh nicht endet der Sonn und des Mondes Lauf,", 6000.0, 0);
			view->PrintTimed(100, 1400, "Die Sterne zersplittern im schwarzen Wind,", 6500.0, 0);
			view->PrintTimed(100, 1600, "Und fallen herab und liegen hier blind,", 7000.0, 0);
			view->PrintTimed(100, 1800, "Bis der dunkle Herrscher hebt seine Hand", 7500.0, 0);
			view->PrintTimed(100, 2000, "-ber tote See und verdorrtes Land.", 8000.0, 0);
			view->PrintTimed(100, 2000, "- J.R.R.Tolkien -", 8500.0, 0);
		}
		break;
	case 36234:
		if ( !npc )
			break;
		if ( view )
			view->PrintTimed(100, 300, "HIGHLANDER", 3000.0, 0);
		npc->GetModel()->__anispeed = 0.4;
		break;
	case 22990:
		if (world) {
			auto skyctrl = dynamic_cast<zCSkyControler_Outdoor>(world->GetActiveSkyControler());
			if (!skyctrl)
				break;
			skyctrl->SetRainFXWeight(0.0, 0.1);
			view->PrintTimed(100, 300, "MOBY DICKS ELEMENT", 3000.0, 0);
		}
	case 8173:
		if ( !ogame )
			break;
		ogame->SwitchCamToNextNpc(ogame);
		if ( view ) {
			view->PrintTimed(100, 300, "FROM EARTH TO NEXT NPC", 3000.0, 0);
		}
		break;
	case 7942:
		if (!npc)
			break;
		npc->SetAsPlayer();

		if (view) {
			view->PrintTimed(view, 100, 300, "YOU ARE NOW " + focusName, 3000.0, 0);
		}
		break;
	case 550:
		ztimer.factorMotion = 1.0;
		oCNpc::player::GetModel()->__anispeed = 1.0;
		oCNpc::player->SetAsPlayer();
		if ( zrenderer )
			zrenderer->SetPolyDrawMode(0);// TODO: enum
		Game_ToggleTestMode(0);
		oCNpc::godmode = 0;
		if ( world ) {
			auto list = world->voblist_npcs;
			for (size_t i = 0, num = list->NumInList(); i < num; ++i )
			{
				oCNpc* npc = list->Get(i);
				if ( npc == oCNpc::player )
					continue;
				npc->SetFatness(1.0);
				npc->GetModel()->SetModelScale(zVEC3(1.0, 1.0, 1.0));
			}
		}
		if (view) {
			view->PrintTimed(view, 100, 300, "WHAT WAS THE QUESTION?", 3000.0, 0);
		}
		ogame->game_frameinfo = 0;
		ogame->game_showtime = 0;
		break;
	case 12859:
		Game_ToggleTestMode(1);
		if ( view )
			view->PrintTimed(100, 300, "MARVIN-MODE", 3000.0, 0);
		break;

	case 36278:
		zrenderer->SetPolyDrawMode(2);
		if ( view )
			view->PrintTimed(100, 300, "CAIPI-Mode", 3000.0, 0);
		break;
	case 11968:
		zrenderer->SetPolyDrawMode(3);
		if ( view )
			view->PrintTimed(100, 300, "MAITAI-Mode", 3000.0, 0);
		break;
	case 71852:
		if (oCNpc::player->IsInInv("ITSC_SUMMONMUD", 1))
			break;
		oCNpc::player->PutInInv("ITSC_SUMMONMUD", 1);
		if ( view ) {
			view->PrintTimed(100, 300, "THE QUEEN IS DEAD", 3000.0, 0);
		}
		break;
	case 22803:
		gameMan->ShowRealPlayTime();
		break;
	case 2838:
		ShowMoreInfos(npc, oCNpc::player);
		break;
	case 8140:
		ogame->game_frameinfo = !ogame->game_frameinfo;
	break;
	case 8085:
		ogame->game_showtime = !ogame->game_showtime;
		if ( !view )
			break;
		if ( ogame->game_showtime ) {
			view->PrintTimed(10, 300, "CLOCK TICK TOCK", 3000.0, 0);
		} else {		
			view->PrintTimed(10, 300, "CLOCK OFF", 3000.0, 0);
		}
		break;
	case 8448:
		if (world) {
			oCVob* vob = world->SearchVobByName(world, "VLK_449_LARES");
			oCNpc* lares = dynamic_cast<oCNpc*>(vob);

			if ( !lares )
				break;

			zSTRING ornament = "ITMI_ORNAMENT_ADDON_VATRAS";
			if (!lares->IsInInv(ornament, 1))
				break;

			lares->RemoveFromInv(ornament, 1);
			oCNpc::player->PutInInv(ornament, 1);
			if ( view )
				view->PrintTimed(100, 300, "Ornament von Lares erhalten", 0.0, 0);
		}
	}
}

class oCStatusScreen {
public:
	oCStatusScreen();
	virtual ~oCStatusScreen();
	void Show();
private:
	oCMenu_Status* menu;
};

oCStatusScreen::oCStatusScreen()
{
	zERROR::Message("B: STAT: Init Status-Screen ...");
	menu = new oCMenu_Status("MENU_STATUS");
	CheatString.Clear();
	zERROR::Message("");
}

oCStatusScreen::~oCStatusScreen()
{
	zERROR::Message("B: STAT: Deinit Status-Screen ...");
	menu->Release();
	HandleResultString(CheatString);
	zERROR::Message("");
}

void oCStatusScreen::Show()
{
	zERROR::Message("B: STAT: Entering Status-Screen ...");
	zinput->ResetRepeatKey(1);
	if ( menu )
	{
		if ( ogame )
			ogame->Pause(0);
		menu->InitForDisplay();
		menu->Run();
		if ( ogame )
			ogame->Unpause();
	}
	zinput->vtable->ResetRepeatKey(zinput, 1);
	zERROR::Message("");
}
