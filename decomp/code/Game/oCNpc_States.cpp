void oCNpc_States::Archive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		arc.WriteBool("curState.valid", curState.valid);
		arc.WriteString("curState.name", curState.name);
		arc.WriteInt("curState.prgIndex", curState.prgIndex);
		arc.WriteBool("curState.isRtnState", curState.isRtnState);
		arc.WriteBool("nextState.valid", nextState.valid);
		arc.WriteString("nextState.name", nextState.name);
		arc.WriteInt("nextState.prgIndex", nextState.prgIndex);
		arc.WriteBool("nextState.isRtnState", nextState.isRtnState);
		arc.WriteInt("lastAIState", lastAIState);
		arc.WriteBool("hasRoutine", hasRoutine);
		arc.WriteBool("rtnChanged", rtnChanged);
		arc.WriteBool("rtnOverlay", rtnOverlay);
		arc.WriteInt("rtnOverlayCount", rtnOverlayCount);
		arc.WriteInt("walkmode_routine", walkmode_routine);
		arc.WriteBool("weaponmode_routine", weaponmode_routine);
		arc.WriteBool("startNewRoutine", startNewRoutine);
		arc.WriteInt("aiStateDriven", aiStateDriven);
		arc.WriteVec3("aiStatePos", aiStatePosition);

		if ( rtnOverlayCount > 0 )
			rtnMan.WriteRoutine(arc, npc, 1);
	}
}

void oCNpc_States::Unarchive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		curState.phase = -1;
		arc.ReadBool("curState.valid", curState.valid);
		arc.ReadString("curState.name", curState.name);
		arc.ReadInt("curState.prgIndex", curState.prgIndex);
		arc.ReadBool("curState.isRtnState", curState.isRtnState);

		int prgIndex = curState.prgIndex;
		if (prgIndex < -1 && prgIndex > -7) {
			curState.name = npcStatesPrg[-prgIndex];


		curState.index = zparser.GetIndex(curState.name);
		curState.loop = zparser.GetIndex(curState.name + "_LOOP");
		curState.end  = zparser.GetIndex(curState.name + "_END");

		if ( curState.valid && curState.index == -1 ) {
			zSTRING msg = "U:NPC: Cannot reinitialize AI-State of " +
			               npc->GetInstanceName() + ". ScriptState : " +
			               curState.name
			zFAULT(msg);
		}

		nextState.phase = -1;
		arc.ReadBool("nextState.valid", nextState.valid);
		arc.ReadString("nextState.name", nextState.name);
		arc.ReadInt("nextState.prgIndex", nextState.prgIndex);
		arc.ReadBool("nextState.isRtnState", nextState.isRtnState);

		prgIndex = nextState.prgIndex;
		if (prgIndex < -1 && prgIndex > -7)
			nextState.name = npcStatesPrg[-prgIndex];

		nextState.index = zparser.GetIndex(nextState.name);
		nextState.loop = zparser.GetIndex(nextState.name + "_LOOP");
		nextState.end  = zparser.GetIndex(nextState.name + "_END");

		arc.ReadInt("lastAIState", lastAIState);
		arc.ReadBool("hasRoutine", hasRoutine);
		arc.ReadBool("rtnChanged", rtnChanged);
		arc.ReadBool("rtnOverlay", rtnOverlay);
		arc.ReadInt("rtnOverlayCount", rtnOverlayCount);
		arc.ReadInt("walkmode_routine", walkmode_routine);
		arc.ReadBool("weaponmode_routine", weaponmode_routine);
		arc.ReadBool("startNewRoutine", startNewRoutine);
		arc.ReadInt("aiStateDriven", aiStateDriven);
		arc.ReadVec3("aiStatePos", aiStatePosition);

		if ( rtnOverlayCount > 0 )
			rtnMan.ReadRoutine(arc, npc);

		rtnMan.FindRoutine(npc, rtnNow, rtnBefore);
	}
}
