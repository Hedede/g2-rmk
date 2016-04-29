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

int oCNpc::EV_StartFX(oCMsgConversation *msg)
{
	AddEffect(msg->name, msg->target);
	return 1;
}

int oCNpc::EV_StopProcessInfos(oCMsgConversation *msg)
{
	auto infoMan = oCInformationManager::GetInformationManager();
	infoMan.Exit();
	StopTalkingWith();
	return 1;
}

int oCNpc::EV_UseMobWithItem(oCMsgManipulate* msg)
{
	auto item = zDYNAMIC_CAST<oCItem>(msg->paramVob);

	SetInteractItem(item);
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

int oCNpc::EV_StopPointAt(oCMsgConversation* msg)
{
	if ( GetEM()->IsRunning(lastPointMsg) ) {
		lastPointMsg->Delete();
		lastPointMsg = 0;
	}
	anictrl->StopCombineAni(anictrl->s_point);
	return 1;
}

int oCNpc::EV_CanSeeNpc(oCMsgMovement *msg)
{
	if ( msg->targetMode >= 0 && CanSee(msg->targetVob, 0) ) {
		ClearEM();
		states.StartAIState(msg->targetMode, 0, 0, 0.0, 0);
		return 0;
	}
	return 1;
}
