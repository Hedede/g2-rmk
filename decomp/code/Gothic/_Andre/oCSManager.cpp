#include <oCSManager.h>

zOBJECT_CLASSDEF(oCCSManager, zCCSManager, 0, 0);

oCCSManager::oCCSManager()
	: zCCSManager();
{
	{
		auto event = new zCEventCore(zCEventCore::zEVENT_TRIGGER,0); // 35
		AddCommand(eventCore->MD_GetSubTypeString(TRIGGER));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new oCMsgMovement(oCMsgMovement::EV_ROBUSTTRACE);
		AddCommand(event->MD_GetSubTypeString(EV_GOTOVOB));
		AddCommand(event->MD_GetSubTypeString(EV_GOROUTE));
		AddCommand(event->MD_GetSubTypeString(EV_GOTOFP));
		AddCommand(event->MD_GetSubTypeString(EV_TURNTOVOB));
		AddCommand(event->MD_GetSubTypeString(EV_SETWALKMODE));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new oCMsgConversation(oCMsgConversation::EV_PLAYANISOUND,zSTRING(),zSTRING()); // 51
		AddCommand(event->MD_GetSubTypeString(EV_PLAYANISOUND));
		AddCommand(event->MD_GetSubTypeString(EV_PLAYANI));
		AddCommand(event->MD_GetSubTypeString(EV_PLAYSOUND));
		AddCommand(event->MD_GetSubTypeString(EV_LOOKAT));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new oCMsgManipulate(oCMsgManipulate::EV_USEMOB,0); //60
		AddCommand(event->MD_GetSubTypeString(EV_USEMOB));
		AddCommand(event->MD_GetSubTypeString(EV_USEITEM));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new zCEventScreenFX(zCEventScreenFX::zEVENT_BLEND_FADEIN,0,0,0); //67
		AddCommand(event->MD_GetSubTypeString(BLEND_FADE_IN));
		AddCommand(event->MD_GetSubTypeString(BLEND_FADE_OUT));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new zCCSCamera_EventMsgActivate(); // 74
		AddCommand(event->MD_GetSubTypeString(EV_CAM_SETPOSREFERENCE));
		AddCommand(event->MD_GetSubTypeString(EV_CAM_SETTARGETREFERENCE));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
	{
		auto event = new zCCSCamera_EventMsg; // 81
		AddCommand(event->MD_GetSubTypeString(EV_CAM_PLAY));
		AddCommand(event->MD_GetSubTypeString(EV_CAM_STOP));
		AddCommand(zMENU_SEPARATOR);
		zRELEASE(event);
	}
}



zCCSPlayer* oCCSManager::CreateCutscenePlayer(zCWorld *ownerWorld)
{
	auto csPlayer = new oCCSPlayer; // g1alpha: 93

	csPlayer->SetOwnerWorld(ownerWorld);
	InsertPlayerInList(csPlayer);

	return csPlayer;
}

zCEventMessage* oCCSManager::CreateOuMessage()
{
	return CreateMessage( 8 );
}


zCEventMessage* oCCSManager::CreateMessage(int msgType)
{
	switch(msgType)
	{
	case 0: return zCEventCore(zCEventCore::zTEventCoreSubType(zCEventCore::zEVENT_TRIGGER)); // 111
	//case 1: return new ???;
	case 2: return new oCMsgMovement(oCMsgMovement::EV_GOTOVOB,0); // 113
	case 3: return new oCMsgMovement(oCMsgMovement::EV_GOROUTE,0); // 114
	case 4: return new oCMsgMovement(oCMsgMovement::EV_GOTOFP,0); // 115
	case 5: return new oCMsgMovement(oCMsgMovement::EV_TURNTOVOB,0); // 116
	case 6: return new oCMsgMovement(oCMsgMovement::EV_SETWALKMODE,0); // 117
	//case 7: return new ???;
	case 8: return new oCMsgConversation(oCMsgConversation::TConversationSubType(oCMsgConversation::EV_PLAYANISOUND)); // 119
	case 9: return new oCMsgConversation(oCMsgConversation::TConversationSubType(oCMsgConversation::EV_PLAYANI));
	case 10: return new oCMsgConversation(oCMsgConversation::TConversationSubType(oCMsgConversation::EV_PLAYSOUND)) // 121
	case 11: return new oCMsgConversation(oCMsgConversation::TConversationSubType(oCMsgConversation::EV_LOOKAT)); // 122
	//case 12: return new ???;
	case 13: return new oCMsgManipulate(oCMsgManipulate::EV_USEMOB); // 124
	case 14: return new oCMsgManipulate(oCMsgManipulate::EV_USEITEM); // 125
	//case 15: return new ???;
	case 16: return new zCEventScreenFX(zCEventScreenFX::zEVENT_BLEND_FADEIN,0,0,0); // 127
	case 17: return new zCEventScreenFX(zCEventScreenFX::zEVENT_BLEND_FADEOUT,0,0,0); // 128
	//case 18: return new ???;
	case 19: return new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE); // 130
	case 20: return new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE); // 131
	//case 21: return new ???;
	case 22: return new zCCSCamera_EventMsg(zCCSCamera_EventMsg::EV_PLAY); // 133
	case 23: return new zCCSCamera_EventMsg(zCCSCamera_EventMsg::EV_STOP); // 134
	default: return nullptr;
}



zCCSProps* oCCSManager::CreateProperties()
{
	return new oCCSProps; // 142
}
