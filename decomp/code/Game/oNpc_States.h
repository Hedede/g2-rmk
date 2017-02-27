class oCNpc_States {
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void PackState(zCBuffer&);
	virtual void UnpackState(zCBuffer&);

	static zSTRING strLastError;
	static zSTRING GetLastError()
	{
		return strLastError;
	}

	void SetOwner(oCNpc* o)
	{
		this->npc = o;
	}

	zVEC3& GetAIStatePosition()
	{
		return this->aiStatePosition;
	}

	int GetLastState() const
	{
		return this->lastAIState;
	}

	int GetLastRoutineState()
	{
		if (rtnNow)
			return rtnNow->GetState();
		return 0;
	}

	zSTRING GetRoutineName() const
	{
		if ( npc->daily_routine > 0 )
		{
			auto sym = zparser.GetSymbol(npc->daily_routine);
			if (sym)
				return sym->name;
		}
		return "";
	}

	int GetState()
	{
		if ( !curState.valid )
			return 0;
		int result = curState.prgIndex;
		if ( result >= -1 )
			result = curState.index;
		return result;
	}

	int IsInState(int funcInst)
	{
		if ( curState.valid ) {
			if (curState.index == funcInst || curState.prgIndex == funcInst)
				return 1;
		} else if (nextState.valid) {
			if (nextState.index == funcInst || nextState.prgIndex == funcInst)
				return 1;
		}
		return 0;
	}

	int GetStateTime()
	{
		if ( curState.valid )
			return curState.stateTime / 1000;
		return 0;
	}

	void SetStateTime(int seconds)
	{
		if ( curState.valid )
			curState.stateTime = timeSec * 1000.0;
	}

	bool IsScriptStateActive()
	{
		return curState.valid && curState.name;
	}

	bool IsAIStateDriven()
	{
		return !hasRoutine && npc->startAIState > 0;
	}

	bool IsInRoutine()
	{
		// don't get this
		if ( curState.valid ) {
			if ( curState.isRtnState )
				return 1;
			if ( curState.valid )
				return 0;
		}
		if ( nextState.valid )
			return 0;
		return 1;
	}

	bool IsInCutscene()
	{
		return npc->GetEM()->GetCutsceneMode();
	}

	void StartOutputUnit(int csNumber)
	{
		zCWorld* wld = npc->homeWorld;
		if ( wld )
			wld->csPlayer->StartOutputUnit(csNumber, npc);
	}

	void StartCutscene(zSTRING const& csName, zSTRING const& roleName)
	{
		zCWorld* wld = npc->homeWorld;
		if ( wld )
			wld->csPlayer->StartCutscene(csName, npc);
	}

	void EndCurrentState()
	{
		if ( curState.valid ) {
			curState.phase = 1;
			return DoAIState();
		}
		return 0;
	}

public:
	class TNpcAIState {
		TNpcAIState() = default;
		~TNpcAIState() = default;
		int      index = 0;
		int      loop  = 0;
		int      end   = 0;
		int      timeBehaviour = 0;
		zREAL    restTime = 0;
		int      phase = -1;
		zBOOL    valid = 0;
		zSTRING  name;
		zREAL    stateTime = 0;
		int      prgIndex = -1;
		zBOOL    isRtnState = 0;
	};

private:
	zSTRING name;
	oCNpc*  npc;

	TNpcAIState   curState;
	TNpcAIState   nextState;
	int           lastAIState;
	zBOOL         hasRoutine;
	zBOOL         rtnChanged;
	oCRtnEntry*   rtnBefore;
	oCRtnEntry*   rtnNow;
	zCRoute*      rtnRoute;
	zBOOL         rtnOverlay;
	int           rtnOverlayCount;
	int           walkmode_routine;
	zBOOL         weaponmode_routine;
	zBOOL         startNewRoutine;
	int           aiStateDriven;
	zVEC3         aiStatePosition;
	oCNpc*        parOther;
	oCNpc*        parVictim;
	oCItem*       parItem;
	int           rntChangeCount;
};

//==============================================================================
// _ulf/oNpcStates.cpp
//==============================================================================
void oCNpc_States::~oCNpc_States()
{
	CloseCutscenes();
	rtnMan.RemoveRoutine(npc);
	Delete(rtnRoute);
	rtnBefore = 0;
	rtnNow = 0;
	npc = 0;
	ClearParserReferences(); // was inlined
}

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

void oCNpc_States::PackState(zCBuffer& buf)
{
	buf.Write(&curState, 0x3C);
	buf.Write(&nextState, 0x3C);
	buf.WriteInt(lastAIState, 32);
}

void oCNpc_States::UnpackState(zCBuffer& buf)
{
	buf.Read(&curState, 0x3C);
	buf.Read(&nextState, 0x3C);
	lastAIState = buf.ReadInt(32);
}

void oCNpc_States::InitRoutine()
{
	rtnBefore = 0;
	rtnNow = 0;
	if ( npc->daily_routine ) {
		zparser.SetInstance("SELF", npc);
		zparser.CallFunc(npc->daily_routine);
		if ( ogame->GetWorld()->wayNet )
			rtnMan.CreateWayBoxes(npc);
	}
}

void oCNpc_States::RemoveOverlay()
{
	rtnOverlayCount = 0;
	rtnMan.RemoveOverlay(npc);
	InitRoutine();
	rtnNow = 0;
	rtnMan.UpdateSingleRoutine(npc);
}

void RemoveOverlayEntry(oCRtnEntry *entry)
{
	if ( entry && entry->rtnOverlay )
	{
		oCRtnManager::RemoveEntry(&rtnMan, entry);
		if ( --rtnOverlayCount <= 0) {
			InitRoutine();
			rtnNow = 0;
			rtnMan.UpdateSingleRoutine(npc);
		} else if ( rtnNow == entry ) {
			rtnNow = 0;
		}
	}
}

void oCNpc_States::BeginInsertOverlayRoutine()
{
	if ( rtnOverlayCount > 0 ) {
		rtnOverlayCount = 0;
		rtnMan.RemoveOverlay(npc);
		InitRoutine();
		rtnNow = 0;
		rtnMan.UpdateSingleRoutine(npc);
	}
	rtnMan.RemoveRoutine(npc);
	rtnOverlayCount = 0;
	rtnNow = 0;
	rtnBefore = 0;
	rtnOverlay = 1;
}

void oCNpc_States::StopInsertOverlayRoutine()
{
	if ( rtnBefore ) {
		auto h = rtnBefore->hour_end;
		auto m = rtnBefore->min_end;
		auto rtn = new oCRtnEntry(h, m, h, m, -1, rtnBefore->GetWatpoint(), -1); // 0x3C
		rtn->npc = npc;
		rtn->rtnOverlay = rtnOverlay;
		rtnMan.Insert(npc, rtn);
	}
	rtnMan.CreateWayBoxes(npc);
	rtnMan.UpdateSingleRoutine(npc);
	rtnOverlay = 0;
}

void oCNpc_States::InsertRoutine(int h1, int m1, int h2, int m2, int func, zSTRING const& s, int instance)
{
	hasRoutine = 1;
	if ( rtnOverlay ) {
		auto timer = ogame->GetWorldTimer();
		int hour1, min1;
		timer->GetTime(hour1, min1);
		int hour2 = hour1;
		int min2 = min1;
		oCWorldTimer::AddTime(hour1, min1, h1, m1);
		m1 = hour1;
		h1 = min1;
		oCWorldTimer::AddTime(hour2, min2, h2, m2);
		h2 = hour2;
		m2 = min2;
		++rtnOverlayCount;
	}

	auto rtn = new oCRtnEntry(h1, m1, h2, m2, func, s, -1); // 0x3C
	rtn->rtnOverlay = rtnOverlay;
	rtnMan.Insert(npc, rtn);
	rtnBefore = rtn;
	if ( s_bCheckRoutines )
		rtnMan.CheckConsistency(npc);
}

int oCNpc_States::StartRtnState(int force)
{
	if ( npc->IsSelfPlayer() )
		return 1;
	startNewRoutine = 0;
	int result = ActivateRtnState(force);
	++rntChangeCount;
	startNewRoutine = result == 0;
	return result;
}

int oCNpc_States::StartAIState(zSTRING const& name, int endOldState, int timeBehaviour, float timed, int isRtnState)
{
	auto idx = zparser.GetIndex(name);
	if (idx > 0)
		return StartAIState(idx, endOldState, timeBehaviour, timed, 0);

	zWARNING("U:AI: Scriptstate " + name + " not found."); // 541
	return 0;
}

int playerStatesEnabled[9];
int oCNpc_States::CanPlayerUseAIState(TNpcAIState const& state)
{
	if ( !state.valid )
		return 0;
	if ( state.prgIndex >= -1 ) {
		for (auto s : playerStatesEnabled) {
			if (state.index == s)
				return 1;
		}
		return 0;
		/*
		 * this confused me at first:
		 * (where int*p = playerStatesEnabled)
		 * because I thought it compared *p, not p)
		 * (that string is located in memory directly after array above)
		 * if ( p >= "ZS_MAGICSLEEP" )
		 * 	return 0;
		 */
	}
	return state.prgIndex == -3 || state.prgIndex == -4;
}

void oCNpc_States::CloseCutscenes()
{
	if ( npc->GetEM()->GetCutsceneMode() )
		npc->StopFaceAni("VISEME");
	auto cs = npc->GetEM()->GetCutscene();
	if ( cs )
		cs->Stop();
	auto wld = npc->homeWorld;
	if ( wld ) {
		auto cs = wld->csPlayer->GetStartedCutscene(npc);
		if ( cs )
			cs->Stop();
	}
}

void oCNpc_States::SetParserInstanceItem(oCItem *item)
{
	if ( item != parItem )
	{
		zCParser.SetInstance(parItemIndex, item);
		Release(parItem);
		parItem = item;
		if ( item )
			++item->refCtr;
	}
}

void oCNpc_States::SetParserInstanceNpc(oCNpc *npc)
{
	if ( index == parOtherIndex ) {
		zCParser.SetInstance(index, npc);
		Release(parOther);
		parOther = npc;
		AddRef(npc);
	} else if ( index == parVictimIndex ) {
		zCParser.SetInstance(index, npc);
		Release(parVictim);
		parVictim = npc;
		AddRef(npc);
	}
}

void oCNpc_States::ClearParserReferences()
{
	SetParserInstanceNpc(parOtherIndex, 0);
	SetParserInstanceNpc(parVictimIndex, 0);
	if ( parItem ) {
		zCParser.SetInstance(parItemIndex, 0);
		Release(parItem);
	}
}
