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

int oCNpc::EV_RemoveInteractItem(oCMsgManipulate*)
{
	SetInteractItem(nullptr);
	return 1;
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
	unsigned num = invSlots.GetNumInList();
	for (unsigned i = 0; i < num; ++i)
		if (invSlots[i] == NPC_NODE_TORSO) {
			auto item = zDYNAMIC_CAST<oCItem>(invSlots[i]->object);
			if (item) {
				EquipArmor(item);
			}
		}
	}
	return 1;
}

int oCNpc::EV_StartFX(oCMsgConversation *msg)
{
	AddEffect(msg->name, msg->target);
	return 1;
}

int oCNpc::EV_PlayAniSound(oCMsgConversation *msg)
{
	auto model = zDYNAMIC_CAST<zCModel>(visual);

	if (model)
		AvoidShrink(1000);

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


int oCNpc::EV_UseMobWithItem(oCMsgManipulate* msg)
{
	auto item = zDYNAMIC_CAST<oCItem>(msg->paramVob);

	SetInteractItem(item);
	return 1;
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


