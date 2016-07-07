class oCMobLadder : public oCMobInter {
	Z_OBJECT(oCMobLadder);
public:
	virtual ~oCMobLadder();

	int DoFocusCheckBBox() override
	{
		return true;
	}

	virtual void Interact(oCNpc *,int,int,int,int,int);
	virtual void EndInteraction(oCNpc *,int);
	int CanInteractWith(oCNpc* npc) override
	{
		return oCMobInter::CanInteractWith(npc);
	}
	virtual void StartInteraction(oCNpc	*);

	int CanChangeState(oCNpc* npc, int from, int to) override
	{
		if ( oCMobInter::CanChangeState(npc, from, to) ) {
			if ( to != -1 || state  == 0 || state == state_num )
				return 1;
		}
		return 0;
	}

	virtual void SearchFreePosition(oCNpc *,float);
};
