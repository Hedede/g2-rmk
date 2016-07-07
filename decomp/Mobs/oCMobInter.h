class oCMobInter : public oCMob {
	Z_OBJECT(oCMobInter);
public:
	enum TMobInterDirection {
	};

	static int SetAllMobsToState(zSTRING const& name, int state)
	{
		int result = 0;
		TraverseMobs(&world->globalVobTree, name, state, &result);
		return result;
	}

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~oCMobInter();

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &) override {}
	virtual void OnMessage(zCEventMessage *,zCVob *);
	void OnTick() override
	{
		if (rewind && npcsCurrent <= 0) {
			npcStateAni = -1;
			CheckStateChange(0);
			if ( state > 0 && CanChangeState(0, state, state - 1) )
				SendStateChange(state, state - 1);
			if ( IsInState(0, 0) )
				SetSleeping(1);
		}
	}

	virtual void SetVisual(zCVisual *);
	zSTRING* GetTriggerTarget(int idx) override
	{
		if (idx > 0)
			return nullptr;
		return &triggerTarget;
	}
	zSTRING GetScemeName() override
	{
		return sceme
	}

	virtual int GetState()
	{
		return state;
	}
	virtual int GetStateNum()
	{
		return state_num;
	}

	void SetTempState(int state);
	bool SetStateToTempState();

	virtual TMobInterDirection GetDirection()
	{
		return direction;
	}
	virtual void SetDirection(TMobInterDirection dir)
	{
		direction = dir;
	}

	virtual void SetUseWithItem(zSTRING const &);
	virtual int GetUseWithItem()
	{
		if (!useWithItem.IsEmpty())
			return zparser.GetIndex(useWithItem);
		return -1;
	}

	int HasUseWithItem(oCNpc* npc)
	{
		int uitem = GetUseWithItem();
		if ( uitem <= 0 )
			return 1;
		if ( npc->HasInHand(uitem) )
			return 1;
		if ( npc->interactItem ) {
			if ( npc->interactItem->GetInstance() == uitem )
				return 1;
		}
		if ( npc->IsSelfPlayer() && npc->IsInInv(uitem, 1) )
		{
			auto item = npc->RemoveFromInv(uitem, 1);
			npc->SetInteractItem(item);
			return 1;
		}
		return 0;
	}

	virtual void Reset();
	virtual void Interact(oCNpc *,int,int,int,int,int);
	virtual void EndInteraction(oCNpc *,int);
	virtual void InterruptInteraction(oCNpc *);
	virtual void StopInteraction(oCNpc *);
	virtual void CanInteractWith(oCNpc *);
	virtual bool IsInteractingWith(oCNpc* npc)
	{
		for (auto& pos : optimalPosList) {
			if (pos.npc == npc)
				return true;
		}

		return false;
	}

	bool IsAvailable(OCNpc* npc)
	{
		if (!npc->IsSelfPlayer()) {
			if (inUseVob && inUseVob != npc, && ztimer.totalTimeFloat < timerEnd)
				return false;
			inUseVob = nullptr;
		}
		return true;
	}

	virtual bool IsOccupied()
	{
		return npcsCurrent > 0;
	}

	bool IsMultiMob() const
	{
		return npcsNeeded > 1;
	}

	virtual void AI_UseMobToState(oCNpc *,int);
	bool IsIn(int) override
	{
		return false;
	}
	virtual void IsInState(oCNpc *,int);
	virtual void StartInteraction(oCNpc *);
	virtual void StartStateChange(oCNpc *,int,int);
	virtual void CheckStateChange(oCNpc *);
	virtual void CanChangeState(oCNpc *,int,int);
	virtual void GetTransitionNames(int,int,zSTRING &,zSTRING &);

	virtual void OnBeginStateChange(oCNpc*,int,int) {}
	virtual void OnEndStateChange(oCNpc*,int,int)
	{
		if ( to == 1 ) {
			OnTrigger(this, npc);
		} else if ( to == 0 ) {
			OnUntrigger(this, npc);
		}

		if ( from < to ) {
			SendCallOnStateFunc(npc, to);
		} else if ( from == 1 && to == 0 ) {
			SendCallOnStateFunc(npc, 0);
		}
	}

	virtual void CallOnStateFunc(oCNpc *,int);
	virtual void SendCallOnStateFunc(oCNpc *,int);


	void ScanIdealPositions()
	{
	}

	virtual void SearchFreePosition(oCNpc *,float);

	bool GetFreePosition(oCNpc* npc, zVEC3& pos)
	{
		ScanIdealPositions();
		auto opt = SearchFreePosition(npc, 150.0);
		if (opt) {
			pos = opt->trafo.GetTranslation();
			return true;
		}
		return false;
	}

	void MarkAsused(float timeDelta, zCVob* vob)
	{
		inUseVob = vob;
		timerEnd = ztimer.totalTimeFloat + timeDelta;
	}

	void SetHeading(oCNpc* npc)
	{
		float azi, elev;
		npc->GetAngles(this, azi, elev);
		RotateWorld(zVEC3{0,1.0,0}, azi);
	}


	bool IsTempStateChanged() const
	{
		return tmpStateChanged;
	}


private:
	//Optimale Positionen sind wohl die, zu denen der Held bei der Benutzung hinploppt. Ich bin aber kein Animierer, hab wenig Ahnung davon.
	zCList<TMobOptPos>    optimalPosList; // List of all Optimal Positions

	//wohlbekannte Eigenschaften:
	zSTRING triggerTarget;
	zSTRING useWithItem;
	zSTRING sceme;
	zSTRING conditionFunc;
	zSTRING onStateFuncName;

	//Zustand des Mobs
	int state;
	int state_num;
	// Zustand auf den gerade "hingearbeitet wird" ?
	int state_target;

	zBOOL rewind;

	zTModelAniID  mobStateAni;
	zTModelAniID  npcStateAni;

	uint8_t npcsMax;
	uint8_t npcsNeeded;
	uint8_t npcsCurrent;

	uint8_t tmpState;
	zBOOL tmpStateChanged;

	//"Richtung in die das Mob benutzt wird????", (0 = none, 1 = up, 2 = down)
	TMobInterDirection direction;

	zBOOL onInterruptReturnToSlotPos;

	zVEC3 startPos;
	zREAL aniCombHeight;
	zCVob* inUseVob;
	zREAL timerEnd;
};

void oCMobInter::Archive(zCArchiver& arc)
{
	oCMob::Archive(arc);

	if ( !arc.InProperties(arc) )
		arc.WriteInt("stateNum", state_num);

	arc.WriteString("triggerTarget", triggerTarget);
	arc.WriteString("useWithItem",   useWithItem);
	arc.WriteString("conditionFunc", conditionFunc);
	arc.WriteString("onStateFunc",   onStateFuncName);
	arc.WriteBool("rewind", rewind);
}

void oCMobInter::Unarchive(zCArchiver& arc)
{
	oCMob::Unarchive(arc);

	if ( !arc.InProperties(arc) )
		arc.ReadInt("stateNum", state_num);

	arc.ReadString("triggerTarget", triggerTarget);
	arc.ReadString("useWithItem",   useWithItem);
	arc.ReadString("conditionFunc", conditionFunc);
	arc.ReadString("onStateFunc",   onStateFuncName);
	arc.ReadBool("rewind", rewind);

	Reset();
}

void oCMobInter::SetTempState(int state)
{
	if ( state != this->tmpState || state != this->state ) {
		this->tmpStateChanged = 1;
		this->tmpState = state;
	} else {
		this->tmpStateChanged = 0;
		this->tmpState = state;
	}
}

bool oCMobInter::SetStateToTempState()
{
	ret = tmpStateChanged;
	if ( ret ) {
		if ( tmpState != 1 || GetState() )
		{
			ret = tmpState;
			if ( !ret ) {
				ret = GetState();
				if ( ret > 0 ) {
					ret = GetEM()->OnUntrigger(this, this);
				}
			}
			tmpStateChanged = 0;
		} else {
			ret = GetEM()->OnTrigger(this, this);
			tmpStateChanged = 0;
		}
	}
	return ret;
}
