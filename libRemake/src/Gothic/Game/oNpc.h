#ifndef Gothic_Npc_H
#define Gothic_Npc_H

#include <Gothic/Game/zVob.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zArray.h>


struct TNpcSlot
{
	zSTRING name;
	int inInv;
	int __someNum;
	zSTRING nume;
	zCVob *object;
	int tree;
};


struct oCNpc : oCVob {
	static oCNpc*& player;

	/*TNpcSlot* GetInvSlot(std::string const& slotName)
	{
		Thiscall<TNpcSlot*(oCNpc*, const zSTRING&)>(0x749AE0);
	}*/

	int junk[548];
	zCArray<TNpcSlot*> invSlots;
};

#endif//Gothic_Npc_H
