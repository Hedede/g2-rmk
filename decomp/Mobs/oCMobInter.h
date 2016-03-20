class oCMobInter : public oCMob {
	Z_OBJECT(oCMobInter);
public:
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual ~oCMobInter();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void OnTick();
	virtual void SetVisual(zCVisual *);
	virtual void GetTriggerTarget(int);
	virtual void GetScemeName();
	virtual void GetState();
	virtual void GetStateNum();
	virtual void GetDirection();
	virtual void SetDirection(oCMobInter::TMobInterDirection);
	virtual void SetUseWithItem(zSTRING const &);
	virtual void GetUseWithItem();
	virtual void Reset();
	virtual void Interact(oCNpc *,int,int,int,int,int);
	virtual void EndInteraction(oCNpc *,int);
	virtual void InterruptInteraction(oCNpc *);
	virtual void StopInteraction(oCNpc *);
	virtual void CanInteractWith(oCNpc *);
	virtual void IsInteractingWith(oCNpc *);
	virtual void IsOccupied();
	virtual void AI_UseMobToState(oCNpc *,int);
	virtual void IsIn(int);
	virtual void IsInState(oCNpc *,int);
	virtual void StartInteraction(oCNpc *);
	virtual void StartStateChange(oCNpc *,int,int);
	virtual void CheckStateChange(oCNpc *);
	virtual void CanChangeState(oCNpc *,int,int);
	virtual void GetTransitionNames(int,int,zSTRING &,zSTRING &);
	virtual void OnBeginStateChange(oCNpc *,int,int);
	virtual void OnEndStateChange(oCNpc *,int,int);
	virtual void CallOnStateFunc(oCNpc *,int);
	virtual void SendCallOnStateFunc(oCNpc *,int);
	virtual void SearchFreePosition(oCNpc *,float);
private:
	//Optimale Positionen sind wohl die, zu denen der Held bei der Benutzung hinploppt. Ich bin aber kein Animierer, hab wenig Ahnung davon.
	zCList<TMobOptPos>    optimalPosList; // List of all Optimal Positions

	//wohlbekannte Eigenschaften:
	zSTRING triggerTarget;               //zSTRING
	zSTRING useWithItem;                 //zSTRING
	zSTRING sceme;                       //zSTRING
	zSTRING conditionFunc;               //zSTRING
	zSTRING onStateFuncName;             //zSTRING

	//Zustand des Mobs
	int state;                          //int
	int state_num;                      //int
	int state_target;                   //int Zustand auf den gerade "hingearbeitet wird" ?

	zBOOL rewind;                         //zBOOL

	zTModelAniID  mobStateAni;
	zTModelAniID  npcStateAni;

	uint8_t npcsMax;
	uint8_t npcsNeeded;
	uint8_t npcsCurrent;

	uint8_t tmpState;
	zBOOL tmpStateChanged;

	//"Richtung in die das Mob benutzt wird????", (0 = none, 1 = up, 2 = down)
	TMobInterDirection  Direction;

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
