int oCNpc::IsMessageAIEnabled()
{
	if ( oCNpc::ai_messagesSkip == 0.0 )
		return 1;
	return oCNpc::ai_messagesSkip >= 0.0 &&
	       oCNpc::ai_messagesSkipTimer > oCNpc::ai_messagesSkip;
}


int oCNpc::IsScriptStateAIEnabled()
{
	if ( oCNpc::ai_scriptStateSkip == 0.0 )
		return 1;
	return oCNpc::ai_scriptStateSkip >= 0.0 &&
	       oCNpc::ai_scriptStateSkipTimer > oCNpc::ai_scriptStateSkip;
}

void oCNpc::SetNpcAIDisabled(int b)
{
	oCNpc::ai_disabled = b;
	oCNpc::ai_baseEnabled = 1;

	oCNpc::ai_scriptStateSkipTimer = 0;
	oCNpc::ai_messagesSkipTimer = 0;
	oCNpc::ai_messagesSkip = 0;
	oCNpc::ai_scriptStateSkip = 0;

	if ( b )
		oCNpc::ai_scriptStateSkip = -1.0;
}

void oCNpc::DeleteHumanAI()
{
	Release(human_ai);
	SetAI(0);
	anictrl = 0;
}


bool oCNpc::IsMoreImportant(zCVob* vob1, zCVob* vob2)
{
	if (!vob1)
		return 0;
	if (!vob2)
		return 1;

	if (GetDistanceToVob2(vob2) > GetDistanceToVob2(vob1))
		return 1;

	return 0;
}
