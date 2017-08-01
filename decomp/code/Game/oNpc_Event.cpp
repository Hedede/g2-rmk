int oCNpc::EV_AttackMagic(oCMsgAttack*)
{
	return 0;
}

int oCNpc::EV_Defend(oCMsgAttack *msg)
{
	if ( didParade ) {
		isDefending = 0;
		didParade = 0;
		return 1;
	}

	isDefending = 1;
	return 0;
}

int oCNpc::EV_AimAt(oCMsgAttack *msg)
{
	oCNpc* target = 0;
	zBOOL drawn = 0;
	zBOOL ammo = 0;

	InitAim(msg, &target, &drawn, &ammo, 1);

	if ( !target ) {
		FinalizeAim(0, 1);
		return 1;
	}

	if ( drawn ) {
		if ( !ammo ) {
			FinalizeAim(1, 1);
			return 1;
		}

		if ( TransitionAim(human_ai->_s_walk, human_ai->_s_aim) ) {
			if ( human_ai )
				human_ai->StopTurnAnis();
			InterpolateAim(target);
		}
	}
	return 0;
}

int oCNpc::EV_StopAim(oCMsgAttack*)
{
	FinalizeAim(0, 1);
	return 1;
}

int oCNpc::EV_ShootAt(oCMsgAttack* msg)
{
	if ( !didShoot ) {
		wasAiming = FinalizeAim(0, 0);

		if ( !hasLockedEnemy ) {
			EV_AimAt(msg);
			return 0;
		}

		DoShootArrow(1);
		didShoot = 1;
	}

	if ( !didHit )
		didHit = TransitionAim(human_ai->_s_aim, human_ai->_s_hitf);

	if ( !didShoot || !didHit )
		return 0;

	DoDoAniEvents();

	if ( !TransitionAim(human_ai->_s_hitf, human_ai->_s_aim) )
		return 0;

	didShoot = 0;
	didHit = 0;
	if ( !wasAiming ) {
		StandUp(0, 1);
		return 1;
	}

	auto msg1 = new oCMsgAttack(EV_AIMAT, msg->paramVob, 0);
	GetEM()->OnMessage(msg1, msg->paramVob);
	wasAiming = 0;
	return 1;
}

int oCNpc::EV_EquipBestWeapon(oCMsgWeapon* msg)
{
	EquipBestWeapon(msg->targetMode);
	return 1;
}

int oCNpc::EV_EquipBestArmor(oCMsgWeapon* msg)
{
	EquipBestArmor();
	return 1;
}

int oCNpc::EV_UnequipArmor(oCMsgWeapon*)
{
	EquipArmor( GetEquippedArmor() );
	return 1;
}

int oCNpc::EV_UnequipWeapons(oCMsgWeapon *msg)
{
	UnequipItem(GetEquippedMeleeWeapon());
	UnequipItem(GetEquippedRangedWeapon());
	return 1;
}

int oCNpc::EV_RemoveWeapon1(oCMsgWeapon* msg)
{
	if ( IsMonster() )
		return 1;

	DoDoAniEvents();

	anictrl->StopTurnAnis();

	if (anictrl->RemoveWeapon1()) {
		if (IsAPlayer() && zinput->KeyPressed(KEY_SPACE)
		                && anictrl->wmode_selected >= FMODE_MAGIC )
		{
			auto msg1 = new oCMsgMagic(0, anictrl->wmode_selected, 0);
			msg1->removeSymbol = 1;

			GetEM()->OnMessage(msg1, this);
		}
		return 1;
	}

	return GetWeaponMode() == FMODE_NONE && anictrl->IsStanding();
}

int oCNpc::EV_StartFX(oCMsgConversation *msg)
{
	AddEffect(msg->name, msg->target);
	return 1;
}

int oCNpc::EV_StopFX(oCMsgConversation* msg)
{
	RemoveEffect(msg->name);
	return 1;
}

int oCNpc::EV_SndPlay(oCMsgConversation *msg)
{
	if ( msg->IsInUse() )
		return 1;

	msg->SetInUse(1);

	auto snd = zsound->LoadSoundFX(msg->name);
	if ( !snd )
		return 1;

	
	msg->timeSec = snd->GetPlayingTimeMSEC();
	msg->posX = 0;
	if ( msg->posY ) // maybe not posY?
	{
		if ( auto target = msg->target ) {
			msg->voiceHandle = zsound->PlaySound3D(snd, target, 0, 0);
			msg->posX = snd->GetPlayingTimeMSEC(snd);
			listOfVoiceHandles.Insert(msg->voiceHandle);
		}
	}

	zsound->PlaySound(snd, 0);
	snd->Release();
	return 1;
}

int oCNpc::EV_PlayAniSound(oCMsgConversation *msg)
{
	auto model = GetModel();

	if (!msg->IsInUse())
		msg->aniId = StartDialogAni();

	if ( EV_PlaySound(msg) ) {
		model->FadeOutAni(msg->aniId);
		return 1;
	}

	if ( !msg->IsInUse() )
		msg->SetInUse(1);
	return 0;
}

int oCNpc::EV_PlayAniFace(oCMsgConversation* msg)
{
	auto model = GetModel();
	if ( model ) {
		auto node = model->SearchNode(zMDL_NODE_NAME_HEAD);
		auto head = zDYNAMIC_CAST<zCMorphMesh>(node);
		if ( head )
			head->StartAni(msg->name, 1.0, -2.0);
	}

	return 1;
}

int oCNpc::EV_PlayAni(oCMsgConversation *msg)
{
	auto model = GetModel();
	if ( !msg->IsInUse() ) {
		auto& name = msg->name;
		if ( model->IsAniActive(anictrl->_s_walk) && name.Search(0, "_2_STAND", 1u) > 0 ) {
			model->StartAni(anictrl->_s_walk, 0);
			return 1;
		}

		if ( name[0] == 'R' && name[1] == '_')
			model->direction = 0;
		else
			model->direction = 1;

		auto ani_id = model->GetAniIDFromAniName(msg->name);
		auto anim = model->GetAniFromAniID(aniId);
		msg->talkingWith = anim;
		if ( msg->talkingWith )
			msg->aniId = anim->aniId;
		if ( msg->aniId == -1 )
			model->StartAni(msg->name, 0);
		else
			model->StartAni(msg>aniId, 0);
		if ( interactMob )
			interactMob->SetMobBodyState(this);
		else
			SetBodyState(msg->number & 0x7F);
		msg->SetInUse(1);
	}

	aniMessageRunning = !model->IsAniActive(msg->aniId);
	return aniMessageRunning;
}

int oCNpc::EV_WaitForQuestion(oCMsgConversation *msg)
{
	msg->timeSec -= ztimer.frameTimeFloat;
	if ( msg->timeSec > 0.0 )
		return 0;
	zparser.SetInstance("SELF", this);
	zparser.CallFunc("B_NPCBye");
	return 1;
}

int oCNpc::EV_ProcessInfos(oCMsgConversation *msg)
{
	auto infoman = oCInformationManager::GetInformationManager();
	if ( msg->IsInUse() ) {
		infoman.Update();
		return infoman.HasFinished();
	} else {
		if (GetEM()->GetCutsceneMode())
			return 1;

		if (target && target->GetEM()->GetCutsceneMode())
			return 1;

		infoman.SetNpc(target);
		msg->SetInUse(1);
	}
	return 0;
}

int oCNpc::EV_StopProcessInfos(oCMsgConversation *msg)
{
	auto infoMan = oCInformationManager::GetInformationManager();
	infoMan.Exit();
	StopTalkingWith();
	return 1;
}

int oCNpc::EV_PointAt(oCMsgConversation *msg)
{
	if ( HasBodyStateFreeHands() ) {
		if ( !msg->target && !msg->vtbl->IsInUse() ) {
			auto wp = homeWorld->wayNet->GetWaypoint( msg->name );
			if ( wp ) {
				msg->positionWorld = wp->GetPositionWorld();
			} else {
				msg->target = homeWorld->SearchVobByName( msg->name );
			}
			msg->SetInUse(1);
		}

		if ( msg->target ) {
			msg->positionWorld = msg->target->GetPositionWorld();
		}

		v13 = &msg_->positionWorld;

		lastPointMsg = msg;

		float azi, elev;
		GetAngles(msg->positionWorld, azi, elev);
		if ( abs(azi) > 90.0 || abs(elev) > 45.0 ) {
			anictrl->StopCombineAni(anictrl->s_point);
			return 0;
		}

		if (msg->target)
			anictrl->StartCombineAni(msg->target, anictrl->s_point, 45.0, 90.0);
		else
			anictrl->StartCombineAni(msg->positionWorld, anictrl->s_point, 45.0, 90.0);

		return 0;
	}
	return 1;
}

int oCNpc::EV_StopPointAt(oCMsgConversation* msg)
{
	if ( GetEM()->IsRunning(lastPointMsg) ) {
		lastPointMsg->Delete();
		lastPointMsg = 0;
	}
	anictrl->StopCombineAni(anictrl->s_point);
	return 1;
}

int oCNpc::EV_StopLookAt(oCMsgConversation* msg)
{
	if ( GetEM()->IsRunning(lastLookMsg) ) {
		lastLookMsg->Delete();
		lastLookMsg = 0;
	}

	if ( GetEM()->GetNumMessages() > 0 ) {
		for (unsigned i = 0; i < GetEM()->GetNumMessages(); ++i)
			auto msg = GetEM()->GetEventMessage(i);
			auto msgconv = 0;
			if (msgconv = zDYNAMIC_CAST<oCMsgConversation>(msg)) {
				if ( msgconv->subType == EV_LOOKAT )
					msgconv->Delete();
			}
		}
	}

	anictrl->StopLookAtTarget();
	return 1;
}

int oCNpc::EV_Ask(oCMsgConversation *msg)
{
	auto player = oCGame::GetSelfPlayerVob();
	if (player->GetFocusNpc() == this &&
	    IsInPerceptionRange(PERC_ASSESSTALK, this, player) )
	{
		msg->SetInUse();
		return 0;
	}

	if ( !msg->IsInUse() || zinput->GetState(5u) != 0.0 )
		return 0;

	ogame->array_view[4]->Close();
	return 1;
}

int oCNpc::EV_WaitTillEnd(oCMsgConversation *msg)
{
	auto target = zDYNAMIC_CAST<oCNpc>(msg->target);
	if ( target != 0 ) {
		if ( human_ai ) {
			human_ai->StopTurnAnis()
			if ( IsSelfPlayer() )
				StandUp(msg, 0, 1);
		}

		if ( !target->GetEM()->IsRunning(msg->talkingWith) ) {
			if ( !target->GetEM()->GetCutsceneMode() )
				return 1;
		}

		return 0;
	}
	return 1;
}

bool oCNpc::IsWaitingForAnswer()
{
	auto msg = dynamic_cast<zCEventMessage*>(GetEM()->GetActiveMessage());
	if (!msg)
		return 0;

	auto type = msg->subType;
	return type == EV_ASK || type == EV_WAITFORQUESTION;
}

int oCNpc::EV_RobustTrace(oCMsgMovement *msg)
{
	if ( !msg->IsInUse() ) {
		RbtReset();
		RbtUpdate(msg->targetPos, msg->targetVob);

		rbt.flags.standIfTargetReached = (msg->targetMode != 0);

		msg->SetInUse(1);
	}

	return RobustTrace();
}

int oCNpc::EV_CanSeeNpc(oCMsgMovement* msg)
{
	if ( msg->targetMode >= 0 && CanSee(msg->targetVob, 0) ) {
		ClearEM();
		states.StartAIState(msg->targetMode, 0, 0, 0.0, 0);
		return 0;
	}
	return 1;
}

int oCNpc::EV_Jump(oCMsgMovement* msg)
{
	bool inUse = msg->IsInUse(msg);
	if ( !inUse ) {
		anictrl->JumpForward();
		msg->SetInUse(1);
		return 0;
	}

	if ( !anictrl->IsStanding() )
		return 0;

	SetCollDetStat(1);
	SetCollDetDyn(1);
	return 1;
}

int oCNpc::EV_Turn(oCMsgMovement *msg)
{
	float turnSpeed = IsAPlayer() ? 0.1 : speedTurn;
	float turnAngle = ztimer.frameTime * turnSpeed;

	float angle = msg->angle;
	if ( msg->angle >= 0.0 ) {
		if ( msg->angle >= turnAngle ) {
			angle = turnAngle;
		}
	} else {
		if (msg->angle <= -turnAngle)
			angle = -turnAngle;
	}

	msg->angle -= angle;

	anictrl->TurnDegrees(angle, 1);

	if ( msg->angle == 0.0 ) {
		anictrl->StopTurnAnis(anictrl);
		return 1;
	}

	return 0;
}

int oCNpc::EV_TurnAway(oCMsgMovement *msg)
{
	if ( msg->targetVob ) {
		if (GetBodyState())
			return 1;

		if ( !anictrl->IsStanding() )
			StandUp(0, 1);
		auto tpos = msg->targetVob->GetPositionWorld();
		auto pos  = GetPositionWorld();
		msg->targetPos = pos + (pos-tpos); // wtf?
		float elev;
		GetAngles(msg->targetPos, msg->angle, elev);
		return EV_Turn(msg);
	}

	zWARNING("U: NPC: EV_TURNAWAY : No targetVob found.") // 2402, _roman/oNpc_Move.cpp
	return 1;
}

int oCNpc::EV_StandUp(oCMsgMovement *msg)
{
	if ( !msg->IsInUse() ) {
		StandUp(0, msg->targetMode != 0);
		msg->SetInUse(1);
	}

	if ( anictrl->actionMode == 5 )
		return 1;

	if ( anictrl->actionMode == 6 )
		return 1;

	if ( GetTrueGuild() == GIL_SKELETON_MAGE )
		return 1;

	auto model = GetModel();
	// was inlined v
	auto ani = model->GetAniFromAniID(anictrl->_s_walk);

	return model->IsAniActive(ani);
}


int oCNpc::EV_GotoPos(oCMsgMovement* msg)
{
	StandUp(1, 1);

	if ( !msg->IsInUse(msg) ) {
		RbtReset();
		RbtUpdate(msg->targetPos, 0);

		rbt.flags.standIfTargetReached = true;
		rbt.maxTargetDist = 10000.0;
		msg->SetInUse(1);
	}

	msg->targetMode = 1;
	return EV_RobustTrace(msg);
}

int oCNpc::EV_GotoVob(oCMsgMovement* msg)
{
	if ( msg->targetVob ) {
		rbt.timer = 500.0
		rbt.targetVob = msg->targetVob;
		rbt.targetPos = msg->targetPos->GetPositionWorld();
		rbt.flags.standIfTargetReached = true;
		rbt.maxTargetDist = 40000.0;
		return RbtGotoFollowPosition();
	}

	zWARNING("U: NPC: EV_GOTOVOB : targetVob not found.") // 2146, oNpc_Move.cpp
	return 1;
}

bool oCNpc::EV_Wait(oCMsgState *msg)
{
	if ( human_ai ) {
		if (human_ai->IsWalking() || in(human_ai->actionMode, 5, 6))
			human_ai->_Stand();
		human_ai->StopTurnAnis();
	}

	msg->time -= ztimer.frameTimeFloat;
	return msg->time < 0.0;
}

int oCNpc::EV_DoState(oCMsgState *msg)
{
	if (msg->waypoint)
		wpname = msg->waypoint;

	zparser.SetInstance("OTHER", msg->other);
	zparser.SetInstance("VICTIM", msg->victim);
	ClearEM();
	if ( auto func = msg->hour_or_func )
	{
		// flags << 31 >> 31 - oldRtnState
		// flags << 30 >> 31 - isRtnState
		states.StartAIState(func, msg->flags & 1, 0, 0.0, msg.flags & 2)
	}
	else
	{
		states.curState.valid  = 0;
		states.nextState.valid = 0;
		states.StartAIState(0);
	}
	return 1;
}


int oCNpc::EV_EquipArmor(oCMsgWeapon* msg)
{
	auto name = msg->GetObjectName();
	auto item = inventory.IsIn1(name, 1);
	if ( item && CanUse(item) && item->HasFlag(ITEM_KAT_ARMOR) )
		EquipArmor(item);
	return 1;
}

int oCNpc::EV_EquipItem(oCMsgManipulate *msg)
{
	if ( !msg->paramVob )
		msg->paramVob = inventory.IsIn1(msg->schemeName, 1);

	auto item = zDYNAMIC_CAST<oCItem>(msg->paramVob);

	if ( item->HasFlag(ITEM_ACTIVE) || CanUse(item) || this != oCNpc::player ) {
		Equip(item);
	} else {
		DisplayCannotUse();
	}
	return 1;
}

int oCNpc::EV_UseMobWithItem(oCMsgManipulate* msg)
{
	auto item = zDYNAMIC_CAST<oCItem>(msg->paramVob);

	SetInteractItem(item);
	return 1;
}

int oCNpc::EV_RemoveInteractItem(oCMsgManipulate*)
{
	SetInteractItem(nullptr);
	return 1;
}

int oCNpc::EV_PlaceInteractItem(oCMsgManipulate* msg)
{
	auto inter = zDYNAMIC_CAST<oCMobItemSlot>(interactMob);
	if (inter) {
		if ( interactItem ) {
			SetInteractItem(nullptr);
			if (inventory.IsIn(interactItem, 1))
				RemoveFromInv(interactItem);

			inter->PlaceItem(interactItem);
		} else {
			auto item = inter->RemoveItem();
			PutInInv(item);
		}
	} else {
		if (interactItem) {
			auto inst = interactItem->GetInstance();
			SetInteractItem(nullptr);

			auto item = RemoveFromInv(inst, 1);
			if ( item && homeWorld )
				homeWorld->RemoveVob(item);
		}
	}

	return 1;
}

int oCNpc::EV_ExchangeInteractItem(oCMsgManipulate* msg)
{
	if ( interactItem ) {
		TNpcSlot* slot = GetInvSlot(interactItem);
#if 0
		TNpcSlot* interactSlot = nullptr;
		for (auto& slot : invSlots) {
			// ИЩЕТ ЭТОТ ЖЕ СЛОТ ЕЩЁ РАЗ,
			// видимо разраб не знал что у них
			// есть функция GetInvSlot(zCVob*)
			auto item = GetSlotItem(slot->name);
			if (item == interactItem) {
				interactSlot = slot;
				break;
			}
		}
#endif

		RemoveFromSlot(slot, 0, 1);
		homeWorld->RemoveVob(interactItem);
		Release(interactItem);
	}

	auto world = ogame->GetGameWorld();
	auto item = world->CreateVob(VOB_TYPE_ITEM, msg->schemeName);
	if ( item ) {
		PutInInv(item);
		auto inst = item->GetInstance();
		SetInteractItem(RemoveFromInv(inst, 1));
		EV_InsertInteractItem(msg);
		Release(item);
	}
	return 1;
}

int oCNpc::EV_UseMob(oCMsgManipulate *msg)
{
	if (!msg->paramVob)
		msg->paramVob = interactMob;

	if ( msg->paramVob ) {
		if (auto mob = zDYNAMIC_CAST<oCMobInter>(msg->paramVob)) {
			if (mob->IsInteractingWith(this) || mob->CanInteractWith(this))
				return mob->AI_UseMobToState(this, msg->targetState);
			return 1;

		}
		return 0;
	}

	msg->schemeName.Upper();
	msg->paramVob = FindMobInter(msg->schemeName);
	if ( auto mob = msg->paramVob ) {
		zVEC3 pos;
		mob->GetFreePosition(this, pos);
		auto msg1 = new oCMsgMovement(1, pos);
		msg1->SetHighPriority(1);
		GetEM()->OnMessage(msg1, 0);
		mob->MarkAsUsed(20000.0, this);
		return 0;
	}

	zWARNING("U: NPC: EV_USEMOB : No Mobsi with specified schemeName ("+ msg->schemeName +") found."); // 13267, oCNpc.cpp
	return 1;
}

int oCNpc::EV_InsertInteractItem(oCMsgManipulate *msg)
{
	if ( !interactItem )
		return 1;

	if ( msg->itemSlot ) {
		auto item = GetSlotItem( msg->itemSlot );
		if ( item != interactItem ) {
			if (inventory.IsIn(interactItem, 1)) {
				SetInteractItem(RemoveFromInv(interactItem, 1));
			}

			auto slot = GetInvSlot(msg->itemSlot);
			PutInSlot( slot, interactItem);
		}
	} else {
		zWARNING("U: ITEMI: No slot found :"+ msg->itemSlot); // 13300, oNpc.cpp
		// I think this is wrong, because adding empty string makes no sense
		// but it is clear that it jumps to here only if msg->itemSlot has length of 0
	}
	return 1;
}

int oCNpc::EV_DestroyInteractItem(oCMsgManipulate *msg)
{
	int inst = -1;
	if ( interactItem )
		inst = interactItem->GetInstance();
	SetInteractItem(0);
	if ( inst >= 0 ) {
		if ( auto item = RemoveFromInv(inst, 1) ) {
			if ( item->GetEM()->GetCutsceneMode() ) {
				zFAULT("U: ITEMI: Item (" + item->GetInstanceName() + ") used in cutscene cannot be destroyed. This is evil, please correct the cutscene using the AI_USEITEM-command !"); // 13413, _ulf/oNpc.cpp
			}

			ogame->GetWorld()->RemoveVob(item);
		}
	}
	return 1;
}

int oCNpc::EV_Cutscene(oCMsgConversation *msg)
{
	if ( auto& name = msg->name ) {
		if (name.Search(0, ".", 1) < 0)
			name += ".CS";
		states.StartCutscene(name, msg->text);
		return 1;
	}

	zWARNING("U: NPC: Tried to play Cutscene (or Output-Unit) with no name. That is evil !"); // 14643, oNpc.cpp
	return 1;
}

int oCNpc::EV_TakeMob(oCMsgManipulate* msg)
{
	auto mi = FindMobInter(msg->schemeName);
	if ( mi ) {
		auto msg = new oCMsgManipulate(0, mi);
		msg->SetHighPriority(1);
		GetEM()->OnMessage(msg, this);
	}
	return 1;
}

int oCNpc::EV_DropMob(oCMsgManipulate* msg)
{
	auto mi = zDYNAMIC_CAST<oCMobInter>(carry_vob);
	if ( mi ) {
		auto mm = new oCMsgManipulate(EV_DROPVOB, carry_vob);

		mm->SetHighPriority(1);
		GetEM()->OnMessage(msg, this);
	}
	return 1;
}

int oCNpc::EV_CallScript(oCMsgManipulate *msg)
{
	zparser.SetInstance("SELF", this);
	zparser.SetInstance("OTHER", oCNpc::player);

	if (msg->targetState < -1) {
		if ( !msg->schemeName.IsEmpty() )
			zparser.CallFunc(msg->schemeName);
	} else {
		zparser.CallFunc(msg->targetState);
	}
	return 1;
}
