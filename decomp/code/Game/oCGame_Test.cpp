void Game_OpenConsole()
{
	if ( !game_con )
		Game_InitConsole();
	zcon.Toggle();
}

int oCGame::TestKeys(int key)
{
	if ( !oCNpc::player )
		return 0;

	if ( game_editwaynet )
		return IA_TestWay(key);

	switch ( key ) {
	case KEY_MULTIPLY: {
		float gamma = zrenderer->Vid_GetGammaCorrection() + 0.1;
		if ( gamma > 2.0 )
			gamma = 2.0;

		zoptions->WriteReal(zOPT_SEC_VIDEO, "zVidGamma", gamma, 0);

		zrenderer->Vid_SetGammaCorrection(gamma, 0.5, 0.5);

		zCView::PrintDebug("< Gamma " + zSTRING(gamma) + ">\n");

		return 1;
	}
	case KEY_DIVIDE: {
		float gamma = zrenderer->Vid_GetGammaCorrection() - 0.1;
		if ( gamma < 0.0 )
			gamma = 0.0;

		zoptions->WriteReal(zOPT_SEC_VIDEO, "zVidGamma", gamma, 0);

		zrenderer->Vid_SetGammaCorrection(gamma, 0.5, 0.5);

		zCView::PrintDebug("< Gamma " + zSTRING(gamma) + ">\n");

		return 1;
	}
	case KEY_F8:
		if ( zinput->KeyPressed(KEY_LSHIFT) ) {
			oCNpc::player->ResetPos(this->startpos);
			zTAICamMsg msg = 0x8000;
			GetCameraAI()->ReceiveMsg(msg);
		} else {
			zVEC3 playerPos;
			oCNpc::player->trafoObjToWorld->GetTranslation(playerPos);
			playerPos.y += 150.0;
			oCNpc::player->ResetPos(playerPos);
		}
		oCInformationManager::GetInformationManager()->Exit();
		return 1;

	case KEY_F2:
		Game_OpenConsole();
		return 1;
	case KEY_5:
		cliprange = GetCamera()->farClipZ - 100.0;
		SetRanges();

		zCView::PrintDebug("< FarClip " + zSTRING(GetCamera()->farClipZ) + " >\n");
		return 1;
	case KEY_6:
		cliprange = GetCamera()->farClipZ + 100.0;
		SetRanges();

		zCView::PrintDebug("< FarClip " + zSTRING(GetCamera()->farClipZ) + " >\n");
		return 1;
	case KEY_7: {
		float tmp1, tmp2;
		zrenderer->GetFogRange(fogrange, tmp1, tmp2);

		fogrange -= 100.0;
		if ( fogrange < 0.0 )
			fogrange = 0.0;

		SetRanges();
	 
		zCView::PrintDebug("< Fog " + zSTRING(fogrange) + ">\n");

		return 1;
	}
	case KEY_8: {
		float tmp1, tmp2;
		zrenderer->GetFogRange(fogrange, tmp1, tmp2);

		fogrange += 100.0;
		if ( fogrange < 0.0 )
			fogrange = 0.0;

		SetRanges();
	 
		zCView::PrintDebug("< Fog " + zSTRING(fogrange) + ">\n");

		return 1;
	}
	case KEY_9:
		if (zinput->KeyPressed(KEY_LSHIFT) ||
		    zinput->KeyPressed(KEY_RSHIFT) )
		{
			playerLightInt -= 100;

			if ( playerLightInt < 0 )
				playerLightInt = 0;

			if ( zCSkyControler::s_activeSkyControler )
				zCSkyControler::s_activeSkyControler->SetLightDirty();

			zCView::PrintDebug("< CamLight: " + zSTRING(playerLightInt) + ">\n");
		}
		else
		{
			pl_lightval -= 100.0;

			if ( pl_lightval < 0.0 )
				pl_lightval = 0.0;

			pl_lightval->SetRange(pl_lightval, 1);
			if ( zCSkyControler::s_activeSkyControler )
				zCSkyControler::s_activeSkyControler->SetLightDirty();

			zCView::PrintDebug("< PlayerLight: " + zSTRING(pl_lightval) ">\n");
		}
		return 1;
	case KEY_0:
		if (zinput->KeyPressed(KEY_LSHIFT) ||
		    zinput->KeyPressed(KEY_RSHIFT) )
		{
			playerLightInt += 100;

			if ( zCSkyControler::s_activeSkyControler )
				zCSkyControler::s_activeSkyControler->SetLightDirty();

			zCView::PrintDebug("< CamLight: " + zSTRING(playerLightInt) + ">\n");
		}
		else
		{
			pl_lightval += 100.0;

			pl_lightval->SetRange(pl_lightval, 1);
			if ( zCSkyControler::s_activeSkyControler )
				zCSkyControler::s_activeSkyControler->SetLightDirty();

			zCView::PrintDebug("< PlayerLight: " + zSTRING(pl_lightval) ">\n");
		}
		return 1;
	case KEY_Y: {
		zVEC3 atvec;
		zVEC3 pos;
		oCNpc::player->trafoObjToWorld.GetAtVector(atvec);
		oCNpc::player->trafoObjToWorld.GetTranslation(pos);

		auto msg = new oCMsgMovement(0, pos + atvec*1000.0)

		oCNpc::player->GetEM(0)->OnMessage(msg, 0);
	}
	return 1;
	case KEY_C:
	// XXX: LOOK AT THIS LATER, MIGHT BE INLINED CODE
		if ( zinput->KeyPressed(KEY_LSHIFT) ) {
			oCNpc* focus = oCNpc::player->GetFocusNpc();
			if ( focus ) {
				zCParser->SetInstance("SELF", focus);
				zCParser->SetInstance("OTHER", oCNpc::player);
				focus->states.StartAIState("ZS_FOLLOW_PC", 0, 0, 0.0, 0);
			}
		}
		else
		{
			oCNpc* focus = oCNpc::player->GetFocusNpc();
			if ( focus )
			{
				focus->SetEnemy(oCNpc::player);
				focus->states.StartAIState("ZS_ATTACK", 0, 0, 0.0, 0);
			}
		}
		return 1;
	case KEY_F3:
		Game_ToggleScreenMode();
		return 1;
	case KEY_F4:
		if ( zinput->KeyPressed(KEY_LSHIFT) ) {
			sysIdle(100);
			oCInformationManager::GetInformationManager();->ToggleStatus();
		} else {
			zCVob* camera = GetCameraVob();
			camera->SetCollDetStat(0);
			camera->SetCollDetDyn(0);
			GetWorld()->RemoveVob(camera);
			GetWorld()->AddVob(camera);
		}

		zCView::PrintDebug("< Game Camera.>\n");

		GetCameraVob()->SetAI(GetCameraAI());
		GetCameraVob()->SetSleeping(0);
		freeLook = 0;

		if ( oCNpc::player )
			oCNpc::player->SetSleeping(0);
		return 1;
	case KEY_F5:
		if ( zinput->KeyPressed(KEY_LCONTROL))
			return 1;

		oCNpc::player->bitfield0 |= 1;
		oCNpc::player->SetSleeping(0);

		zCView::PrintDebug("< Move Player - Model.>\n");

		GetCameraVob()->SetSleeping(1);
		return 1;
	case KEY_F6: {
		zCVob* camera = GetCameraVob();
		camera->SetCollDetStat(0);
		camera->SetCollDetDyn(0);

		GetWorld()->RemoveVob(camera);
		GetWorld()->AddVob(camera);

		camera->SetHeadingYWorld(oCNpc::player);
		auto ai = new oCAICamera;
		camera->SetAI(ai);
		camera->callback_ai->Release();
		camera->SetSleeping(0);
		oCNpc::player->bitfield0 |= 1;
		oCNpc::player->SetSleeping(1);

		freeLook = 1;
		return 1;
	}
	case KEY_F7: {
		zCVob* camera = GetCameraVob();
		camera->SetCollDetStat(0);
		camera->SetCollDetDyn(0);

		oCNpc::player->bitfield0 |= 1;

		GetWorld()->RemoveVob(camera);
		camera->ResetRotationsWorld();
		GetWorld()->AddVobAsChild(oCNpc::player, camera);

		zCView::PrintDebug("< Third Person.>\n");

		camera->SetSleeping(1);
		oCNpc::player.SetSleeping(0);
		return 1;
	}
	case KEY_F9:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			zresMan.PurgeCaches(zresMan, 0);
			return 1;
		}
		if ( !zinput->KeyPressed(KEY_LCONTROL) ) {
			singleStep = !singleStep;
			if ( singleStep ) {
				timeStep = 0.0;
				zCView::PrintDebug("< Single-step an >\n");
			} else {
				timeStep = -1.0;
				zCView::PrintDebug("< Single-step aus >\n");
			}
			return 1;
		}
		break;
	case KEY_F10:
		if ( singleStep ) {
			timeStep = 0.04;
			zERROR::Message("step");
		}
		return 1;
	case KEY_F12:
		if ( zinput->KeyPressed(zinput, KEY_LMENU) ) {
			auto selfPlayer = dynamic_cast<oCNpc*>(oCGame::GetSelfPlayerVob());
			auto focus = selfPlayer->GetFocusNpc();
			if ( focus ) {
				auto msg = new oCMsgConversation(16, focus);
				selfPlayer->GetEM(0)->OnMessage(msg, selfPlayer);
			}
		} else {
			++dword_AB0BAC;
		}
		return 1;
	case KEY_G:
		if (zinput->KeyPressed(KEY_LCONTROL)) {
			oCNpc::player->GetFocusNpc()->ToggleShowStateDebug();
		} else {
			oCNpc* focus;
			if (zinput->KeyPressed(KEY_LSHIFT)) {
				focus = oCNpc::player->GetFocusNpc();
			} else {
				focus = oCNpc::player;
			}

			auto npclist = ogame->GetGameWorld()->voblist_npcs->next;
			for (; npclist; npclist = npclist->next) {
				if ( npclist->data != focus ) {
					oCAIHuman* ai = npclist->data->human_ai;
					if ( ai )
						ai->SetShowAI(0);
				}
			}

			if ( focus ) {
				oCAIHuman* ai = focus->human_ai;
				if ( ai )
					ai->SetShowAI(!ai->GetShowAI());
			}
		}
		return 1;
	case KEY_F11:
		// probably return to first
		if ( dword_AB0B84 ) {
			int v113 = dword_AB0B84->bitfield2 & 3;
			dword_AB0B84->SetAsPlayer();
			dword_AB0B84->SetVobName("ABCD");
			dword_AB0B84->callback_ai-<SetShowAI(0);
			this->HandleEvent(KEY_F4);
			dword_AB0B84->SetSleepingMode(v113);
			dword_AB0B84 = 0;
		}
		return 1;
	case KEY_HOME:
		SwitchCamToNextNpc();
		return 1;
	case KEY_BACK:
		if ( oCNpc::player->human_ai->bitfield & oCAIHuman_sprintActive ) {
			oCNpc::player->human_ai->SetSprintMode(0);
		} else {
			if ( oCNpc::player->GetWeaponMode() >= 1 ) {
				auto msg = new oCMsgWeapon(3, FMODE_NONE, 0);
				oCNpc::player->GetEM(0)->OnMessage(msg, oCNpc::player);
			}
			oCNpcFocus::SetFocusMode(0);

			oCAIHuman::SetCamMode(CamModNormal, 0);
			oCNpc::player->human_ai->SetSprintMode(1);
		}
		return 1;
	case KEY_T:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			GetWorld()->PrintStatus();
		} else {
			oCNpc* npc;
			if ( zinput->KeyPressed(KEY_LSHIFT) )
				npc = oCNpc::player;
			else
				npc = oCNpc::player->GetFocusVob();

			if ( npc )
				npc->ToggleShowDebug();
		}
		return 1;
	case KEY_ADD:
		playerLightInt += 100;

		fogrange  += 100.0;
		cliprange += 100.0;

		SetRanges();

		zCView::PrintDebug("< Light: " + zSTRING(playerLightInt) + " Fog: " + zSTRING(fogrange) + ">\n");
		return 1;
	case KEY_SUBTRACT:
		playerLightInt -= 100;

		fogrange  -= 100.0;
		cliprange -= 100.0;

		SetRanges();

		zCView::PrintDebug("< Light: " + zSTRING(playerLightInt) + " Fog: " + zSTRING(fogrange) + ">\n");
		return 1;
	case KEY_P:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			float y, x, z;
			oCNpc::player->GetPositionWorld(y, x, z);
			zSTRING text = "Position :";
			text += y;
			text += "/";
			text += x;
			text += "/"
			text += z;
			text += "\n"

			game_text->Printwin(text);
		}
		return 1;
	case KEY_O:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			zCView::SetShowDebug(!zCView::GetShowDebug());
			if ( zCView::GetShowDebug() ) {
				game_text->Printwin("Debug Infos an.\n");
			} else {
				game_text->Printwin("Debug Infos aus.\n");
			}
		} else if ( oCNpc::player->GetFocusNpc() ) {
			oCNpc::player->GetFocusNpc()->SetAsPlayer();
		}
		return 1;
	case KEY_U:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			game_showwaynet = !game_showwaynet;
		} else if ( zinput->KeyPressed(KEY_LCONTROL) ) {
			zCView::SetShowDebug(1);
			zCView::PrintDebug("< Waynet - Mode an >\n");
			game_showwaynet = 1;
			game_editwaynet = 1;
		} else if ( zinput->KeyPressed(KEY_LSHIFT) ) {
			GetWorld()->wayNet->CorrectHeight();
			zCView::PrintDebug("< Waynet - Correction.>\n");
		}
		return 1;
	case KEY_K:
		if ( zinput->KeyPressed(KEY_LMENU) ) {
			Game_OpenFightConsole();
		} else {
			oCNpc::player->SetCollDetStat(0);
			oCNpc::player->SetCollDetDyn(0);
			zVEC3 atvec;
			zVEC3 pos;
			oCNpc::player->trafoObjToWorld.GetAtVector(atvec);
			oCNpc::player->trafoObjToWorld.GetTranslation(pos);

			oCNpc::player->SetPositionWorld(pos + atvec*300.0);

			oCNpc::player->SetCollDetStat(1);
			oCNpc::player->SetCollDetDyn(1);

			ogame->GetCameraAI()->ReceiveMsg(0x8000);
		}
		return 1;
	case KEY_E:
		if ( zinput->KeyPressed(zinput, KEY_LMENU) ) {
			Game_OpenSpeciesConsole();
		} else {
			oCNpc* npc = (oCNpc *)oCGame::GetSelfPlayerVob();
			zCModel* model = npc->GetModel();
			model->_protoypes[0]->DescribeTree(0u, 0);
		}
		return 1;
	case KEY_NUMPAD4:
		if ( oCNpc::player->GetTalentSkill(NPC_TALENT_SNEAK) == 1) {
			oCNpc::player->SetTalentSkill(NPC_TALENT_SNEAK, 0);
			zCView::PrintDebug("< Schleichen aus. >\n");
		} else {
			oCNpc::player->SetTalentSkill(, NPC_TALENT_SNEAK, 1);
			zCView::PrintDebug("< Schleichen an. >\n");
		}
		return 1;
	case KEY_LBRACKET:
		zCAICamera::bCamChanges = !zCAICamera::bCamChanges;
		if ( zCAICamera::bCamChanges ) {
			zCView::PrintDebug("< Camera-Changes an. >\n");
		} else {
			zCView::PrintDebug("< Camera-Changes aus. >\n");
		}
		return 1;
	case KEY_H:
		if ( zinput->KeyPressed(KEY_LSHIFT) ) {
			zCVob* focus = oCNpc::GetFocusVob(oCNpc::player);
			if ( focus )
			{
				zVEC3 pos;
				pos[0] = focus->trafoObjToWorld.m[2][3];
				pos[1] = focus->trafoObjToWorld.m[1][3];
				pos[2] = focus->trafoObjToWorld.m[0][3];
				focus->OnDamage(oCNpc::player, oCNpc::player, 5.0, DAM_FIRE, pos);
			}
			oCNpc::player->SetFocusVob(0);
		} else if ( zinput->KeyPressed(KEY_LCONTROL) ) {
			zVEC3 pos;
			pos[0] = oCNpc::player->trafoObjToWorld.m[2][3];
			pos[1] = oCNpc::player->trafoObjToWorld.m[1][3];
			pos[2] = oCNpc::player->trafoObjToWorld.m[0][3];
			oCNpc::Player->GetEM(0)->OnDamage(0, 0, 10.0, DAM_MAGIC|DAM_FIRE, pos);
		} else {
			zVEC3 pos;
			pos[0] = oCNpc::player->trafoObjToWorld.m[2][3];
			pos[1] = oCNpc::player->trafoObjToWorld.m[1][3];
			pos[2] = oCNpc::player->trafoObjToWorld.m[0][3];
			oCNpc::Player->GetEM(0)->OnDamage(0, 0, 10.0, DAM_EDGE, pos);
		}
		return 1;
	case KEY_J:
		if ( oCNpc::player )
			showRoutineNpc = oCNpc::player->GetFocusNpc();
	case KEY_V:
	default:
		return 1;
	}
	return 0;
}

int oCGame::IA_TestWay(int key)
{
	static zCWaypoint* mwp1 = 0;
	static zCWaypoint* mwp2 = 0;
	static zSTRING vobname = "A";

	switch ( key ) {
	case KEY_N: {
		auto vob = new zCVobWaypoint();
		vob->SetVobName(vobname);

		GetWorld()->AddVob(vob);

		vob->SetCollDetStat(0);
		vob->SetCollDetDyn(0);

		vob->SetPositionWorld(oCNpc::player->GetPositionWorld());
		vob->SetHeadingAtWorld(oCNpc::player->GetHeadingAtWorld());

		auto wp = zfactory->CreateWaypoint();

		wp->Init(vob);
		wp->SetName(vobname);

		++vobname[0];

		GetWorld()->wayNet->InsertWaypoint(wp);
		return 1;
	}
	case KEY_1: {
		auto wp = GetWorld()->wayNet->GetNearestWaypoint(oCNpc::player::GetPositionWorld());
		if ( wp )
			zCView::PrintDebug("< Select WP1 " + wp->GetName() + ">\n");

		mwp1 = wp;
		return 1;
	}
	case KEY_2: {
		auto wp = GetWorld()->wayNet->GetNearestWaypoint(oCNpc::player::GetPositionWorld());
		if ( wp )
			zCView::PrintDebug("< Select WP2 " + wp->GetName() + ">\n");

		mwp2 = wp;
		return 1;
	}
	case KEY_C:
		if ( mwp1 ) {
			auto vob = new zCVobWaypoint();
			vob->SetVobName(vobname);

			GetWorld()->AddVob(vob);

			vob->SetCollDetStat(0);
			vob->SetCollDetDyn(0);

			vob->SetPositionWorld(oCNpc::player->GetPositionWorld());
			vob->SetHeadingAtWorld(oCNpc::player->GetHeadingAtWorld());


			mwp2 = zfactory->CreateWaypoint();

			mwp2->Init(vob);
			mwp2->SetName(vobname);

			++vobname[0];

			GetWorld()->wayNet->InsertWaypoint(mwp2);
			GetWorld()->wayNet->CreateWay(mwp1, mwp2);
			mwp1 = mwp2;
		}
		return 1;
	case KEY_W:
		if (zinput->KeyPressed(KEY_LCONTROL)) {
			zCView::PrintDebug("< Waynet - Mode aus >\n");
			game_editwaynet = 0;
		} else if ( mwp1 && mwp2 ) {
			GetWorld()->wayNet->CreateWay(mwp1, mwp2);
		}
		return 1;
	case KEY_B:
		if ( mwp1 ) {
			auto view = new zCView(0, 0, 2048, screen->FontY(), 2);

			screen->InsertItem(view, 0);

			auto name = mwp1->GetName();
			name = view->Input(30, name);
			mwp1->SetName(name);

			auto vob = mwp1->GetVob();
			if (vob)
				vob->SetVobName(name);

			screen->RemoveItem(view);

			Delete(view);
		}
		return 1;
	case KEY_E:
		if ( mwp1 ) {
			auto view = new zCView(0, 0, 2048, screen->FontY(), 2);

			screen->InsertItem(view, 0);

			auto str = view->Input(8, str);

			screen->RemoveItem(view);

			Delete(view);
		}
		return 1;
	case KEY_D:
		if (zinput->KeyPressed(KEY_LCONTROL)) {
			GetWorld()->wayNet->DeleteWaypoint(mwp1);
		} else if (zinput->KeyPressed(KEY_LSHIFT)) {
			GetWorld()->wayNet->DeleteWay(mwp1, mwp2);
		}
		return 1;
	case KEY_A:
		GetWorld()->wayNet->CorrectHeight();
		return 1;
	default:
		break;
	}
	return 0;
}
