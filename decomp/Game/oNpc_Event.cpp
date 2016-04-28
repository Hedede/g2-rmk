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


