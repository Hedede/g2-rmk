class oCMobLadder : public oCMobInter {
	Z_OBJECT(oCMobLadder);
public:
	virtual ~oCMobLadder();
	virtual void DoFocusCheckBBox();
	virtual void Interact(oCNpc *,int,int,int,int,int);
	virtual void EndInteraction(oCNpc *,int);
	virtual void CanInteractWith(oCNpc *);
	virtual void StartInteraction(oCNpc	*);
	virtual void CanChangeState(oCNpc *,int,int);
	virtual void SearchFreePosition(oCNpc *,float);
};
