struct oCShrinkHelper
{
	oCShrinkHelper(oCNpc* npc)
		: npc(npc)
	{
		npc->SetVisual(nullptr);
		npc->UnShrink();
	}

	~oCShrinkHelper()
	{
		if ( npc )
			npc->Shrink(); //was inlined

	}

	bool Ask()
	{
		if (!npc)
			return true;
		if (!npc->homeWorld)
			return true;
		if (!npc->GetModel())
			return true;
		if (npc->GetModel()->_prototypes.IsEmpty())
			return true;
		if (npc->GetModel()->_prototypes.At(0).refCtr <= 1)
			return true;
		return false;
	}

	oCNpc *npc;
};
