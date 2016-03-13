void ScriptWarning(zSTRING msg, bool print_func = true)
{
	if ( dword_8B436C )
	{
		if ( dword_8B436C == 1 )
		{
			zERROR::Warning("C: SCRIPT: " + msg);
		}
		else
		{
			if ( !byte_AB0A7D ) {
				zERROR::Fault("C: SCRIPT: " + msg);
			}
			byte_AB0A7D = 1;
			zERROR::Warning("C: SCRIPT: " + msg);
		}

		if (print_func) {
			auto cur_func = zparser->GetCurrentFunc();
			auto sym = zparser->GetSymbol(cur_func);
			if (!sym )
				return;

			zERROR::Warning("C: SCRIPT: last parser func-name: " +
			                sym->name);
		}
	}
}


oCNpc* oldSelfNpc;
oCNpc* oldOtherNpc;
oCNpc* oldVictimNpc;

void Script_SaveNpcGlobals()
{
	oldSelfNpc = 0;
	oldOtherNpc = 0;
	oldVictimNpc = 0;

	auto* slf = (zCVob*)zparser->GetSymbol("SELF")->GetInstanceAdr();
	if (slf)
		oldSelfNpc = dynamic_cast<oCNpc*>(slf);

	auto* oth = (zCVob*)zparser->GetSymbol("OTHER")->GetInstanceAdr();
	if (oth)
		oldOtherNpc = dynamic_cast<oCNpc*>(oth);

	auto* vic = (zCVob*)zparser->GetSymbol("VICTIM")->GetInstanceAdr();
	if (vic)
		oldVictimNpc = dynamic_cast<oCNpc*>(vic);
}

void Script_RestoreNpcGlobals()
{
	zparser->SetInstance("SELF", oldSelfNpc);
	zparser->SetInstance("OTHER", oldOtherNpc);
	zparser->SetInstance("VICTIM", oldVictimNpc);
}
