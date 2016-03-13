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
