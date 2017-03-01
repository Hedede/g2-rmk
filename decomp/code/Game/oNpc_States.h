int playerStatesEnabled[9] = {-1,-1,-1,-1,-1,-1,0,0,0};
zSTRING strPlayerStatesEnabled[] = {
	"ZS_ASSESSMAGIC", "ZS_ASSESSSTOPMAGIC", "ZS_MAGICFREEZE",
	"ZS_WHIRLWIND", "ZS_SHORTZAPPED", "ZS_ZAPPED",
	"ZS_PYRO", "ZS_MAGICSLEEP"
};

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
		if (curState.isRtnState)
			return 1;
		return !curState.valid && !nextState.valid;
		/* don't get this
		if ( curState.valid ) {
			if ( curState.isRtnState )
				return 1;
			if ( curState.valid )
				return 0;
		}
		if ( nextState.valid )
			return 0;
		return 1;*/
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
		void Clear()
		{
			valid = 0;
			index = -1;
			prgIndex = -1;
			phase = -1;
			name.Clear();
		}
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
	oCNpc*  npc = nullptr;

	TNpcAIState   curState;
	TNpcAIState   nextState;
	int           lastAIState = 0;
	zBOOL         hasRoutine = false;
	zBOOL         rtnChanged = false;
	oCRtnEntry*   rtnBefore = nullptr;
	oCRtnEntry*   rtnNow    = nullptr;
	zCRoute*      rtnRoute  = nullptr;
	zBOOL         rtnOverlay = false;
	int           rtnOverlayCount = 0;
	int           walkmode_routine = false;
	zBOOL         weaponmode_routine = false;
	zBOOL         startNewRoutine = false;
	int           aiStateDriven = false;
	zVEC3         aiStatePosition;
	oCNpc*        parOther  = nullptr;
	oCNpc*        parVictim = nullptr;
	oCItem*       parItem   = nullptr;
	int           rntChangeCount = 0;
};

//==============================================================================
// _ulf/oNpcStates.cpp
//==============================================================================
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
		if (prgIndex < -1 && prgIndex > -7)
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
void oCNpc_States()
{
	if ( parOtherIndex < 0 )
		parOtherIndex = zparser.GetIndex("OTHER");
	if ( parVictimIndex < 0 )
		parVictimIndex = zparser.GetIndex("VICTIM");
	if ( parItemIndex < 0 )
		parItemIndex = zparser.GetIndex("ITEM");
	if ( playerStatesEnabled[0] == -1 ) {
		for (auto& [i, s] = pairs(playerStatesEnabled, strPlayerStatesEnabled))
			i = zparser.GetIndex(s);
	}
	return this;
}

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

void oCNpc_States::SetRoutine(oCRtnEntry *nowTA, oCRtnEntry *beforeTA)
{
	if ( rtnNow == nowTA ) {
		if ( beforeTA ) {
			if ( rtnBefore == beforeTA )
				return;
		}
	}

	if ( beforeTA )
		this->rtnBefore = beforeTA;
	else
		this->rtnBefore = rtnNow;

	if ( rtnNow ) {
		if ( rtnNow->cs )
			rtnNow->cs->ResetPlayState();
	}

	Delete(rtnRoute);
	rtnNow = nowTA;

	if (rtnBefore && rtnNow) {
		auto wpNow = rtnNow->GetWaypoint();
		auto wpBefore = rtnBefore->GetWaypoint();
		rtnRoute = ogame->GetWorld()->wayNet->FindRoute(wpBefore, wpNow, npc);
	}
	rntChangeCount = 0;
	if ( rtnNow )
		npc->wpname = rtnNow->GetWaypoint();
}

void oCNpc_States::ChangeRoutine(zSTRING const& routineName)
{
	auto func = "RTN_" + routineName + "_" + npc->idx;
	ChangeRoutine(zparser.GetIndex(func));
}

void oCNpc_States::ChangeRoutine(int newRoutineFunc)
{
	if ( newRoutineFunc <= 0 ) {
		auto name = npc->GetInstanceName();
		zWARNING("U:RTN:Routine change failed:" + name);
	} else {
		rtnMan.RemoveRoutine(npc);
		npc->daily_routine = newRoutineFunc;
		InitRoutine();
		rtnMan.UpdateSingleRoutine(npc);

		auto name = npc->GetInstanceName();
		zINFO("U:RTN:Routine change :" + name);
	}
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

void oCNpc_States::InitAIStateDriven(zVEC3 const& pos)
{
	aiStatePosition = pos;
	auto wp = ogame->GetWorld()->wayNet->GetNearestWaypoint(pos);
	if ( wp )
		npc->wpname = wp->GetName();
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

void oCNpc_States::InsertRoutineCS(zSTRING const& csName, zSTRING const& roleName)
{
	if ( rtnBefore ) {
		auto cs = new oCRtnCutscene(); // 0x38
		cs->csName = csName;
		cs->csName.Upper();
		if ( cs->csName.Search(0, ".", 1u) <= 0 )
			cs->csName += ".CS";
		cs->roleName = roleName;
		cs->roleName.Upper();
		cs->npc = this->npc;
		cs->unk1 = 0;
		rtnBefore->cs = cs;
	}
}

int oCNpc_States::ActivateRtnState(int force)
{
	if ( npc->IsSelfPlayer() || !oCNpc::IsScriptStateAIEnabled() )
		return 0;
	if ( !npc->IsConditionValid() ) {
		strLastError = "Condition not valid.";
		return 0;
	}
	if (npc->GetEM()->GetCutsceneMode()) {
		if ( !force ) {
			strLastError = "In Cutscene Mode.";
			return 0;
		}

		if (npc->GetEM()->GetCutscene())
			npc->GetEM()->GetCutscene()->Stop();
	}

	if ( hasRoutine ) {
		if ( !rtnNow ) {
			strLastError = "No current Routine.";
			return 0;
		}

		if ( !force ) {
			if (!npc->GetEM()->IsEmpty(1)) {
				strLastError = "EM not empty.";
				return 0;
			}

			/*int idx = 0;
			if ( curState.valid ) {
				idx = curState.prgIndex;
				if ( idx >= -1 )
					idx = curState.index;
			}*/

			if ( GetState() == rtnNow->GetState() )
				return 1;

			if (!IsInRoutine()) {
				strLastError = "No routine state currently running.";
				return 0;
			}
		}

		npc->wpname = rtnNow->GetWaypoint();

		if (!rtnBefore) {
			zWARNING("U: NPC: rtnBefore unknown"); // 501
		} else {
			auto wld = npc->homeWorld;
			if (wld) {
				auto cs = wld->csPlayer->GetStartedCutscene();
				if (cs && rtnBefore->cs) {
					// zCCSCutsceneContext::AreU
					if (cs->AreU(rtnBefore->cs->csName)) {
						cs->Stop();
					}
				}
			}
		}

		rtnChanged = 0;
		auto state = rtnNow->GetState();
		return StartAIState(state, 1, 0, 0.0, 1);
	}

	if ( npc->startAIState <= 0 )
		return 0;

	if ( force ) {
		curState.valid = 0;
		nextState.valid = 0;
	}
	if ( curState.valid || nextState.valid )
		return 0;
	return StartAIState(npc->startAIState, 1, 0, 0.0, 0);
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

int oCNpc_States::CanPlayerUseAIState()
{
	bool can = CanPlayerUseAIState(curState);
	if (!can)
		ClearAIState();
	return can;
}

void oCNpc_States::ClearAIState()
{
	curState.Clear();
	nextState.Clear();
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

zSTRING oCNpc_States::GetStateInfo()
{
	zSTRING out = curState.name;
	if (curState.phase == 0)
		out += "_LOOP";
	if (curState.phase == 1)
		out += "_END";
	if (curState.phase == 2)
		out += "_INTERRUPT";
	out += "/";
	//if (curState.isRtnState || (!curState.valid && !nextState.valid))
	if (IsInRoutine())
		out += "R/";
	if (npc->GetEM()->GetCutsceneMode())
		out += "CS/";
	out += "(" + GetStateTime() + ")"; // was inlined
	out += " Ref:" + npc->refCtr;
	return out;
}

int oCNpc_States::DoAIState()
{
	// was very messy sphagetti
	if ( curState.valid && !curState.phase )
		curState.stateTime += ztimer.frameTimeFloat;
	if (!oCNpc::IsScriptStateAIEnabled())
		return 0;
	if ( npc->GetEM()->GetCutscene() ) {
		if (npc->GetEM()->GetCutscene()->IsCutscene())
			return 0;
	}
	if (npc->GetEM()->IsEmpty(1)) {
		if ( startNewRoutine ) {
			if (IsInRoutine()) {
				if ( !npc->IsSelfPlayer() && !npc->IsSelfPlayer())
					StartRtnState(0);
			}
		}

		if ( !curState.valid ) {
			if ( nextState.valid ) {
				lastAIState = curState.index;
				curState.valid = 1;
				// basically curState = nextState
				curState.index = nextState.index;
				curState.phase = nextState.phase;
				curState.loop = nextState.loop;
				curState.end = nextState.end;
				curState.name = nextState.name;
				curState.isRtnState = nextState.isRtnState;
				curState.prgIndex = nextState.prgIndex;
				curState.stateTime = 0;
				nextState.valid = 0;
				npc->ClearPerception();
			} else if ( !npc->IsSelfPlayer() ) {
				StartRtnState(0);
			}
		}

		if ( npc->IsSelfPlayer(npc) ) {
			if (!CanPlayerUseAIState())
				return 0;
		}

		if ( curState.valid ) {
			zparser.SetInstance("SELF", npc);
			zparser.SetInstance(parOtherIndex, parOther);
			zparser.SetInstance(parVictimIndex, parVictim);
			zparser.SetInstance(parItemIndex, parItem);

			switch(curState.phase) {
			case -1:
				npc->SetPerceptionTime(5000.0);
				if ( curState.index > 0 )
					zparser.CallFunc(curState.index);
				curState.phase = 0;
				break;
			case 0:
				int loopEnd;
				if (curState.loop <= 0)
					loopEnd = 1;
				else
					loopEnd = *zparser.CallFunc(curState.loop);
				if (curState.prgIndex < 0) {
					if (curState.prgIndex == -4)
						npc->CheckUnconscious();
					if (curState.prgIndex == -6) {
						npc->Follow();
						break;
					}
				}
				if (loopEnd)
					curState.phase = 1;
				break;
			case 1:
				if (curState.end > 0)
					zparser.CallFunc(curState.end);
				curState.phase = 2;
				curState.valid = 0;
				break;

			}
		}
	}

	if ( curState.valid && !curState.phase ) {
		if ( curState.isRtnState )
		{
			if ( rtnNow && rtnNow->cs ) {
				auto cs = rtnNow->cs;
				if ( npc->homeWorld ) {
					auto csMan = ogame->GetCutsceneManager();
					bool flag2 = csMan->PoolHasFlags(cs->csName, 2);
					if (!csMan->PoolHasFlags(cs->csName, 1)) {
						if (cs->unk0 || !flag2) {
							auto mov = npc->isInMovementMode != 0;
							if (mov)
								npc->EndMovement(1);
							auto ccs = npc->homeWorld->csPlayer->StartCutscene(cs->csName, npc);
							if (ccs)
								cs->unk = ccs->ReplayCutscene();
							if (mov)
								npc->BeginMovement();
						}

					}
				}
			}
		}
		if ( curState.timeBehaviour == 1 || curState.timeBehaviour == 2)
		{
			curState.restTime -= ztimer.frameTimeFloat;
			if (curState.restTime < 0.0)
				curState.phase = 1;
		}
	}
	return 1;
}

int oCNpc_States::StartAIState(int index, int oldRtnState, int timeBehaviour, float timed, int isRtnState)
{
	if ( !oCNpc::IsScriptStateAIEnabled() )
		return 0;

	if (npc->GetEM()->GetCutscene()) {
		if (npc->GetEM()->GetCutscene()->IsCutscene()) {
			strLastError = "Cutscene mode active.";
			return 0;
		}
		CloseCutscenes();
	}

	if ( index < 0 ) {
		if ( index >= -1 || index <= -7 ) {
			auto name = npc->GetInstanceName();
			zWARNING("U:NPC: " + name + ": Illegal AI-State" + index + " not found."); // 607
			return 0;
		}

		nextState.prgIndex = index;
		nextState.name = npcStatesPrg[-index];
		nextState.index = zparser.GetIndex(nextState.name);
		nextState.loop = zparser.GetIndex(nextState.name + "_LOOP");
		nextState.end = zparser.GetIndex(nextState.name + "_END");
	} else {
		int typ, ele;
		auto name = zparser.GetSymbolInfo(index, typ, ele)
		if ( name.Search(0, "ZS_", 1u) ) {
			int tmp = curState.isRtnState;
			curState.isRtnState = isRtnState;
			zparser.SetInstance("SELF", npc);
			zparser.CallFunc(index);
			curState.isRtnState = tmp;
			return 1;
		}

		nextState.prgIndex = 0;
		nextState.name = name;
		nextState.index = index;
		nextState.loop = zparser.GetIndex(nextState.name + "_LOOP");
		nextState.end = zparser.GetIndex(nextState.name + "_END");
	}

	oCNpc* other = 0;
	if ( auto sym = zparser.GetSymbol(parOtherIndex) )
		other = dynamic_cast<oCNpc*>(sym->GetInstanceAdr());
	SetParserInstanceNpc(parOtherIndex, other);
	
	oCNpc* victim = 0;
	if ( auto sym = zparser.GetSymbol(parVictimIndex) )
		victim = dynamic_cast<oCNpc*>(sym->GetInstanceAdr());
	SetParserInstanceNpc(parVictimIndex, victim);

	oCItem* item = 0;
	if ( auto sym = zparser.GetSymbol(parItemIndex) )
		item = dynamic_cast<oCItem*>(sym->GetInstanceAdr());

	SetParserInstanceItem( item ); // was inlined


	nextState.timeBehaviour = timeBehaviour;
	nextState.phase = -1;
	nextState.valid = 1;
	nextState.isRtnState = isRtnState;
	if ( oldRtnState ) {
		curState.phase = 1;
	} else {
		curState.phase = 2;
		curState.valid = 0;
		if ( !npc->GetInteractMob() ) {
			if ( !npc->interactItem ) {
				if ( !npc->IsAPlayer() || CanPlayerUseAIState(nextState) ) {
					npc->GetModel()->StopAnisLayerRange(2,256);
					npc->Interrupt(0, 1);
					npc->ClearEM();
				}
			}
		}
	}
	if ( npc->sleepingMode && npc->homeWorld )
		return DoAIState();
	return 0;
}
