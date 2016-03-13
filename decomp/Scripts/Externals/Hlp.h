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
