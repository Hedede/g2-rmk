struct TMobOptPos {
	~TMobOptPos() = default;
	zMAT4 trafo;
	zBOOL isDistNode;
	oCNpc *npc;
	zSTRING name;
};

class oCMobInter : public oCMob {
	Z_OBJECT(oCMobInter);
public:
	enum TMobInterDirection {
	};

	void oCMobInter() = default;

	static void TraverseTriggerMobs( zCTree<zCVob*>* tree );
	static void TraverseTriggerMobs(zCTree<zCVob*>* tree);
	static void TraverseMobs(zCTree<zCVob*>* tree, STRING const& name, int state, int *count);
	static void TriggerAllMobsToTmpState(zCWorld* world);
	static int SetAllMobsToState(oCWorld *world, zSTRING const& name, int state);

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~oCMobInter() = default;

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &) override {}
	void OnMessage(zCEventMessage* message, zCVob* sourceVob) override;
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

	zSTRING GetScemeName() override { return sceme; }

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

	virtual void SetUseWithItem(zSTRING const& name)
	{
		useWithItem = name;
	}
	virtual int GetUseWithItem()
	{
		if (useWithItem)
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

	bool IsAvailable(oCNpc* npc)
	{
		if (npc->IsSelfPlayer() || !inUseVob)
			return true;
		if (inUseVob != npc && ztimer.totalTimeFloat < timerEnd)
			return false;
		inUseVob = nullptr;
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

	bool IsIn(int) override { return false; }

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


	void ScanIdealPositions();

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

	void MarkAsUsed(float timeDelta, zCVob* vob)
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
	//Optimale Positionen sind wohl die, zu denen der Held bei der Benutzung hinploppt.
	//Ich bin aber kein Animierer, hab wenig Ahnung davon.
	zCList<TMobOptPos> optimalPosList;

	//wohlbekannte Eigenschaften:
	zSTRING triggerTarget;
	zSTRING useWithItem;
	zSTRING sceme;
	zSTRING conditionFunc;
	zSTRING onStateFuncName;

	//Zustand des Mobs
	int state = 0;
	int state_num = 1;
	// Zustand auf den gerade "hingearbeitet wird" ?
	int state_target = 0;

	zBOOL rewind = false;

	zTModelAniID mobStateAni = -1;
	zTModelAniID npcStateAni = -1;

	uint8_t npcsMax     = 0;
	uint8_t npcsNeeded  = 0;
	uint8_t npcsCurrent = 0;

	uint8_t tmpState = 0;
	zBOOL tmpStateChanged = 0;

	//"Richtung in die das Mob benutzt wird????", (0 = none, 1 = up, 2 = down)
	TMobInterDirection direction = 0;

	zBOOL onInterruptReturnToSlotPos = 1;

	zVEC3 startPos{0,0,0};
	zREAL aniCombHeight = 0.0;
	zCVob* inUseVob = nullptr;
	zREAL timerEnd = 0.0;
};

//------------------------------------------------------------------------------
// _Ulf/oMobInter.cpp
void oCMobInter::TraverseTriggerMobs(zCTree<zCVob*> *node)
{
	// actually it was optimized by compiler into a loop
	// (it was recursive like TraverseMobs
	for (; node; node = node->next) {
		if (auto mob = zDYNAMIC_CAST<oCMobInter*>(node->Data())) {
			mob->SetStateToTempState(); // was inlined
		}
		TraverseTriggerMobs(node->firstChild);
	}
}

void oCMobInter::TraverseMobs(zCTree<zCVob*> *node, STRING const& name, int state, int *count)
{
	if (!tree)
		return;

	if (auto mob = zDYNAMIC_CAST<oCMobInter*>(node->Data())) {
		if ( mob->GetScemeName() == name ) {
			mob->SetTempState( state );
			if (mob->SetStateToTempState())
				++*count;
		}
	}

	TraverseMobs(tree->firstChild, name, state, count);
	TraverseMobs(tree->next, name, state, count);
}

void oCMobInter::TriggerAllMobsToTmpState(zCWorld* world)
{
	TraverseTriggerMobs(&world->globalVobTree);
}

int oCMobInter::SetAllMobsToState(oCWorld *world, zSTRING const& name, int state)
{
	int result = 0;
	TraverseMobs(&world->globalVobTree, name, state, &result);
	return result;
}

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

void oCMobInter::SetVisual(zCVisual *v)
{
	zCVob::SetVisual(v);
	SetCollisionClass(&zCCollObjectComplex::s_oCollObjClass);
	if ( v ) {
		auto vname = v->GetVisualName();
		sceme = vname.Copied("_", 1);
		SetCollisionClass(&zCCollObjectComplex::s_oCollObjClass);
		flags1.6 = 0;  // 100000
	}
}

void oCMobInter::SetTempState(int state)
{
	if ( state != tmpState || state != GetState() ) {
		tmpStateChanged = 1;
		tmpState = state;
	} else {
		tmpStateChanged = 0;
		tmpState = state;
	}
}

bool oCMobInter::SetStateToTempState()
{
	if ( !IsTempStateChanged() )
		return false;

	bool ret = false;
	if (GetTempState() == 1 ) {
		if (GetState() == 0) {
			GetEM()->OnTrigger(this, this);
			ret = true;
		}
	} else if ( GetTempState() == 0 ) {
		if ( GetState() == 1 ) {
			GetEM()->OnUntrigger(this, this);
			ret = true;
		}
	}

	tmpStateChanged = 0;
	return ret;
}

TMobOptPos* oCMobInter::SearchFreePosition(oCNpc *npc, float maxDist)
{
	if ( npcsCurrent >= npcsMax )
		return nullptr;

	float optDist = FLT_MAX;
	TMobOptPos* optPos = nullptr;

	bool notFound = false;
	bool wrongSide = false;

	const float maxDistSq = maxDist * maxDist;

	for (auto node : optimalPosList) {
		if (node->npc == npc)
			return pos;
		if (!node->npc) {
			auto nodePos =  pos->trafo->GetTranslation();
			auto vec = npc->GetPositionWorld() - nodePos;
			auto dist = vec.LengthSq()
			if ( dist < optDist ) {
				if ( node->isDistNode || npc->FreeLineOfSight(nodePos, 0) ) {
					if (!npc->IsSelfPlayer() || iter->isDistNode || dist < maxDistSq) {
						optDist = dist;
						optPos = node;
					} else {
						notFound = 1;
					}
				} else {
					wrongSide = 1;
				}
			}
		}
	}

	if ( !optPos && npc->IsSelfPlayer() ) {
		if ( notFound ) {
			auto msg = new oCMsgManipulate(EV_CALLSCRIPT, "PLAYER_MOB_TOO_FAR_AWAY", -1);
			npc->GetEM()->OnMessage(msg, npc);
		}
		if ( wrongSide ) {
			auto msg = new oCMsgManipulate(EV_CALLSCRIPT, "PLAYER_MOB_WRONG_SIDE", -1);
			npc->GetEM()->OnMessage(msg, npc);
		}
	}

	return optPos;
}

void oCMobInter::SetIdealPosition(oCNpc *npc)
{
	auto optPos = SearchFreePosition(npc, 150.0);
	if ( optPos ) {
		auto mov = npc->isInMovementMode;
		if (mov)
			npc->EndMovement(1);

		auto opos = opos->GetPositionWorld();

		if ( optPos->isDistNode ) {
			auto ppos = npc->GetPositionWorld();
			auto mpos = this->GetPositionWorld();

			ppos.y = 0;
			mpos.y = 0;
			opos.y = 0;

			auto optDist = (opos - mpos).Length();
			auto pdir = ppos - mpos;
			pdir.SetLength( optDist );

			npc->SetCollDetStat(0);
			npc->SetCollDetDyn(0);

			auto pos = GetPositionWorld();
			pos.y = ppos.y;
			pos += pdor;

			npc->SetPositionWorld( pos );
			npc->SetHeadingYWorld( this );

			npc->SetCollDetStat(1);
			npc->SetCollDetDyn(1);
		} else {
			npc->SetTrafoObjToWorld(optPos->trafo);
			npc->SetPositionWorld(opos);
		}

		optPos->npc = npc;

		if ( mov )
			npc->BeginMovement();
	}
}

void oCMobInter::Reset()
{
	if ( !rewind ) {
		int s = 0;
		if ( auto npc = dynamic_cast<oCNpc*>(inUseVob) ) {
			if ( state > 0 ) {
				if ( auto model = npc->GetModel() ) {
					auto name = "T_STAND_2_S" + state;
					// probably also GetAniIDFromAniName
					auto ani = model->SearchAni(name);
					if ( ani && ani->aniId != -1 )
						s = state;
				}
			}
		}

		if ( auto model = GetModel() ) {
			auto name = "S_S" + s;
			auto aniId = model->GetAniIDFromAniName( name );
			model->StartAni( aniId, 0 );
		}

		state_target = s;
		state = s;
	}
	if ( state <= 0 )
		SetDirection(0);
	else
		SetDirection(2);
}

void oCMobInter::OnTrigger(zCVob *vob, zCVob *vobInstigator)
{
	if ( vob ) {
		zINFO(7, "B: MobInter Trigger from " + vob->GetObjectName()); // 1136
	} else {
		zINFO(7, "B: MobInter Trigger"); // 1137
		if ( !vobInstigator && !>state && !IsOccupied() ) {
			state_target = 1;
			state = 1;
			if ( GetModel() )
				GetModel()->StartAni("S_S1", 0);
		}
	}

	if ( triggerTarget ) {
		zCArray<zCVob*> vobList;
		homeWorld->SearchVobListByName(triggerTarget, vobList);
		for (auto vob : vobList) {
			zINFO(7, "B: MobInter Trigger Target " + vob->GetObjectName()); // 1154
			if (vobInstigator)
				vob->GetEM()->OnTrigger(this, vobInstigator);
			else
				vob->GetEM()->OnTrigger(this, this);
		}
	}
}

void oCMobInter::OnUntrigger(zCVob *otherVob, zCVob *vobInstigator)
{
	if (otherVob) {
		zINFO(7, "B: MobInter Untrigger from " + otherVob->GetObjectName()); // 1168
	} else {
		zINFO(7, "B: MobInter Untrigger"); // 1169

		if (!vobInstigator && state == 1 && !IsOccupied()) {
			state_target = 0;
			state = 0;
			if ( GetModel() ) {
				GetModel()->StartAni( "S_S0", 0 );
			}
		}
	}

	if ( triggerTarget ) {
		zCArray<zCVob*> vobList;
		homeWorld->SearchVobListByName(triggerTarget, vobList);
		for (auto vob : vobList) {
			zINFO(7, "B: MobInter Untrigger Target " + vob->GetObjectName()); // 1186
			vob->GetEM()->OnUntrigger(vobInstigator);

		}
	}
}

void oCMobInter::SendStateChange(int from, int to)
{
	bool hasNpc = 0;
	zCVob* playerVob = nullptr;

	if ( GetEM()->GetNetVobControl(1) ) {
		auto pinfo = GetEM()->GetNetVobControl(1)->playerInfo;
		if ( pinfo )
			playerVob = pinfo->playerHostVob;
	}

	for (auto optPos : optimalPosList) {
		if (optPos->npc) {
			auto msg = new oCMobMsg(oCMobMsg::EV_STARTSTATECHANGE, optPos->npc); // 1028
			OnMessage(msg, dynamic_cast<oCNpc*>(playerVob));
			hasNpc = 1;
		}

	}

	if ( !hasNpc ) {
		auto msg = new oCMobMsg(oCMobMsg::EV_STARTSTATECHANGE,NULL); // 1037
		GetEM()->OnMessage( msg, dynamic_cast<oCNpc*>(playerVob) );
		return;
	}
}

void oCMobInter::SendEndInteraction(oCNpc *npc, int from, int to)
{
	auto msg = new oCMobMsg(EV_ENDINTERACTION, npc);

	msg->state = from;
	msg->flags = to | 0x80000000;

	GetEM()->OnMessage(msg, npc);
}

void oCMobInter::OnMessage(zCEventMessage* message, zCVob* srcVob)
{
	if (auto msg = dynamic_cast<oCMobMsg*>(message)) {
		switch ( msg->subType ) {
		case EV_STARTINTERACTION:
			StartInteraction(msg->npc);
			break;
		case EV_STARTSTATECHANGE:
			StartStateChange(msg->npc, msg->state, msg->flag);
			break;
		case EV_ENDINTERACTION:
			StopInteraction(msg->npc);
			break;
		case EV_CALLSCRIPT:
			CallOnStateFunc(msg->npc, msg->flag);
			break;
		}
	}
}

int oCMobInter::IsInState(oCNpc *npc, int snr)
{
	zCModel* model = GetModel();
	zCModel* npcModel = nullptr;
	if (npc)
		npcModel = npc->GetModel();
	else
		npcStateAni = -1;

	if ( state != snr || state_target != snr )
		return false;

	if ( mobStateAni != -1 ) {
		// all was inlined:
		auto ani = model->GetAniFromAniID(mobStateAni);
		if ( !ani )
			return false;
		if (!model->IsAniActive(ani))
			return false;
	}
	if ( npcStateAni != -1 ) {
		// all was inlined:
		/auto ani = npcModel->GetAniFromAniID(mobStateAni);
		if ( !ani )
			return false;
		if (!npcModel->IsAniActive(ani))
			return false;
	}
	return true;
}

int oCMobInter::CanChangeState(oCNpc *npc, int from, int to)
{
	if ( !this->IsInState(npc, from) )
		return 0;
	if ( npc && npc->IsSelfPlayer() ) {
		if ( from == 0 && to > 0 ) {
			if (!HasUseWithItem(npc)) // was inlined
				return 0;
		}
	}

	zSTRING mobAni, npcAni;
	GetTransitionNames(from, to, mobAni, npcAni);

	auto model = GetModel();
	auto aniId = model->GetAniIDFromAniName(mobAni);
	auto ani   = model->GetAniFromAniID(aniId);
	if (ani && (ani->bitfield & 0x3F) != 6) // TODO
		return 1;

	if (npc) {
		auto model = npc->GetModel();
		auto aniId = model->GetAniIDFromAniName(npcAni);
		auto ani   = model->GetAniFromAniID(aniId);
		if (ani && (ani->bitfield & 0x3F) != 6)
			return 1;
	}

	return 0;
}

void oCMobInter::StartStateChange(oCNpc* npc, int from, int to)
{
	auto model = GetModel();
	npcModel = npc ? npc->GetModel() : nullptr;

	zSTRING nameFrom, nameTo;
	GetTransitionNames(from, to, mobAni, npcAni);
	mobStateAni = -1;
	if ( model )
	{
		if ( npc && npc->IsAPlayer() )
			model->__aniSpeed = 1.0 / ztimer.factorMotion;

		// prototypes[0]->SearchAni(mobAni);
		if ( auto ani = model->SearchAni( mobAni ) ) {
			aniId = ani->aniId;
			if ( aniId != -1 ) {
				// zachem? (loops over anis again)
				// (all of this is quite a mess)
				auto ani1 = model->GetAniFromAniID( aniId);
				if ( !model->IsAniActive(ani1) ) {
					auto ani2 = model->GetAniFromAniID(aniId);
					auto stateAni = aniFrom->__transitionAni;
					if (!stateAni) {
						if ( ani2->aniName ) {
							stateAni = model->prototypes[ani2->aniId]->__transitionAni;
							stateAni = model->GetAniFromAniID(__transitionAni->protoAni->aniId);
						}
					}

					mobStateAni = stateAni ? stateAni->aniId : -1;
					model->StartAni(aniId, 0);
				}
			}
		}
	}

	if ( npcModel)
		StartTransitionAniNpc(npc, npcAni);

	state_target = to;
	SetDirection(0);
	if ( from >= 0 ) {
		auto cur = state_num;
		if ( from <= cur && to >= 0 && to <= cur && from != to ) {
			if ( from >= to )
				SetDirection(2);
			else
				SetDirection(1);
		}
	}

	OnBeginStateChange(npc, from, to);
}

void oCMobInter::StartTransitionAniNpc(oCNpc *npc, const zSTRING& yk)
{
	auto npcModel = oCNpc::GetModel(npc);
	auto ani = npcModel->SearchAni(yk);
	int aniId = ani ? ani->aniId : -1;

	if ( aniId != -1 ) {
		auto ani = npcModel->GetAniFromAniID(aniId);
		auto tani = ani->__transitionAni;
		if (!tani && ani->aniName)
			tani = model->prototypes->anis[ani->aniId]->__transitionAni;
		if (tani && tani->protoAni) {
			npcStateAni = tani->protoAni->aniId;
		} else {
			npcStateAni = -1;
		}

		ani = model->GetAniFromAniID(aniId);
		if ( (ani->bitfield & 0x3F) == 5 ) {
			zMAT4 trafo;
			GetTrafoModelNodeToWorld(trafo, "BIP01 FIRE");
			zVEC3 pos = npc->GetPositionWorld();
			float h = (trafo[1][3] - pos.y + 100.0) * 0.005;
			zClamp(h, 0.0, 1.0);
			aniCombHeight = h;
			model->StartAni(aniId, 0);
			model->SetCombineAniXY(aniId, 0.5, h);
		} else {
			if (!model->IsAniActive(aniId))
				model->StartAni(aniId);
		}
		if ( yk.Search("2_STAND") != -1 )
			npc->GetAnictrl()->SetNextAni(aniId, npc->GetAnictrl()->_s_walk);
	}
}

void oCMobInter::CallOnStateFunc(oCNpc *npc, int state)
{
	auto funcIdx = zparser.GetIndex( onStateFuncName + "_S" + state );
	if ( funcIdx >= 0 ) {
		zparser.SetInstance("SELF", npc);
		zparser.SetInstance("ITEM", npc->interactItem);
		zparser.CallFunc(funcIdx);
	}
}

void oCMobInter::SendCallOnStateFunc(oCNpc* npc, int state) {
	if (onStateFuncName) {
		auto msg = new oCMobMsg(oCMobMsg::EV_CALLSCRIPT,npc); // 1301
		msg->state_to = state;
		GetEm()->OnMessage(msg,this);
	}
}

void oCMobInter::InterruptInteraction(oCNpc *npc)
{
	zVEC3 pos{1.0, 0, 0};
	for (auto* node : optimalPosList) {
		if (node->npc == npc) {
			pos = node->pos;
			node->npc = nullptr;
		}
	}

	if ( npc->GetModel() && npc->GetModel()->active_ani_num > 1 )
		npc->GetModel()->StopAnisLayerRange(2, 256);

	if ( npc->interactItem ) {
		npc->PutInInv(npc->interactItem);
		npc->SetInteractItem(nullptr);
	}
	npc->SetInteractMob(nullptr);

	Reset();
	inUseVob = nullptr;

	npc->ResetXZRotationsWorld();

	if ( npc->GetModel() )
		npc->GetModel()->RemoveAllVobFX();

	if ( onInterruptReturnToSlotPos & 1 && npc->FreeLineOfSight(pos, this) ) {
		if ( npc->SearchNpcPosition(npc, pos) )
			npc->SetPositionWorld(npc, pos);
		npc->SetCollDetStat(1);
		npc->SetCollDetDyn(1);
	}

	if ( --npcsCurrent <= 0 ) {
		flags1.6 = 0; // 100000
		if ( !rewing )
			SetSleeping(1);
	}

	ignoreVobList.Remove(npc);
}

bool oCMobInter::CanInteractWith(oCNpc* npc)
{
	if ( isDestroyed )
		return false;
	if ( IsOccupied() ) {
		if (npc->IsSelfPlayer()) {
			auto msg = new oCMsgManipulate(EV_CALLSCRIPT, "PLAYER_MOB_ANOTHER_IS_USING", -1);
			npc->GetEM()->OnMessage(msg, npc);
		}
		return false;
	}

	if (flags1.3)
		return false;

	if (!npc->IsSelfPlayer() && inUseVob != npc && ztimer.totalTimeFloat < timerEnd)
		return false;
	else
		inUseVob = nullptr;

	ScanIdealPositions();
	TMobOptPos* pos = SearchFreePosition(npc, 150.0);
	if ( !pos ) {
		if ( npc->IsSelfPlayer() ) {
			if (!npc->GetModel()->IsAniActive("T_DONTKNOW")) {
#ifdef GOTHIC1 // Found this code in G1demo, but it's missing in G2

				if (zinput->GetToggled(GAMEKEY_ACTION)) {
					auto msg = new oCMsgConversation(oCMsgConversation::EV_PLAYANI,"T_DONTKNOW"); // 1463
					npc->GetEM()->OnMessage(msg, npc);
				}
			}
#endif
		}
		return false;
	}

	zVEC3 vec = pos->trafo.GetTranslation();

	bool hasItem = HasUseWithItem( npc ); // was inlined
	if ( !hasItem && !npc->IsSelfPlayer() ) {
		auto useItem = GetUseWithItem();
		bool mov = npc->isInMovementMode;
		if ( mov )
			npc->EndMovement(1);

		auto item = npc->RemoveFromInv( useItem );
		if (item) {
			item->AddRef();
		} else {
			item = ogame->GetGameWorld()->CreateVob(VOB_TYPE_ITEM, useItem);
		}
		npc->SetInteractItem(item);
		Release(item);
		hasItem = true;
	}

	if ( hasItem ) {
		if (conditionFunc) {
			auto sym = zparser.GetSymbol( "SELF" );
			auto slf = zDYNAMIC_CAST<oCNpc>(sym);

			zparser.SetInstance( "SELF", npc );
			int ret = *zparser.CallFunc( conditionFunc );
			zparser.SetInstance( "SELF", slf );

			if (!ret)
				return false;
		}

		return true;
	}

	if ( npc->IsSelfPlayer() ) {
		if (!npc->GetModel()->IsAniActive("T_DONTKNOW")) {

			if (zinput->GetToggled(GAMEKEY_ACTION)) {
				auto msg = new oCMsgConversation(oCMsgConversation::EV_PLAYANI,"T_DONTKNOW"); // 1456
				npc->GetEM()->OnMessage(msg, npc);
			}
		}
	}

	return false;
}

void oCMobInter::EndInteraction(oCNpc *npc, int playEndAni)
{
	if ( playEndAni ) {
		if ( auto model = npc->GetModel() ) {
			auto anictrl = npc->GetAnictrl();
			auto ani = model->GetAniFromAniID(anictrl->_s_walk);
			if ( !model->IsAniActive(ani) ) {
				auto name = "T_" + GetScemeName() + "_S" + state + "_2_STAND";
				StartTransitionAniNpc(npc, name);
				inUseVob = 0;
			}
		}
	} else {
		auto msg = new oCMobMsg{ oCMobMsg::EV_ENDINTERACTION, npc }; //1046
		msg->state = state;
		msg->flags = -1;
		GetEM()->OnMessage(msg, npc);
	}
	SetDirection(0);
}

void oCMobInter::StopInteraction(oCNpc *npc)
{
	if ( IsInteractingWith(npc) ) {
		// looks similar to EndInteraction()
		if ( !npc->GetAnictrl()->IsStanding() ) {
			auto name = "T_" + GetScemeName() + "_S" + state + "_2_STAND";
			StartTransitionAniNpc(npc, name);
		}

		// looks like InterruptInteraction()
		if (auto pos = optimalPosList.Search(npc)) // pseudocode
			pos->npc = 0;

		if ( npc ) {
			npc->ResetXZRotationsWorld();
			if (npc->GetModel())
				npc->GetModel()->RemoveAllVobFX();
		}
		if ( !rewind )
			SetSleeping(1);
		if ( npc->interactItem ) {
			npc->PutInInv(npc->interactItem);
			npc->SetInteractItem(0);
		}

		npc->SetInteractMob(0);
		npc->SetFocusVob(0);
		npc->CheckPutbackTorch(a2);

		if (npcsCurrent-- <= 1)
			flags1 &= 11011111b;

		ignoreVobList.Remove(npc);
		inUseVob = 0;
	}
}
