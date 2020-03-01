#ifndef Gothic_oItem_H
#define Gothic_oItem_H
#include <Hook/Externals.h>
#include <Gothic/Game/zVob.h>
#include <Gothic/Game/zCClassDef.h>
struct oCItem : public zCVob {
	void* operator new(size_t size)
	{
		void* mem = ::operator new(size);

		auto object   = reinterpret_cast<oCItem*>(mem);
		auto classDef = reinterpret_cast<zCClassDef*>(0xAB1168);
		zCClassDef::ObjectCreated(object, classDef);

		return mem;
	}

	oCItem()
	{
		Thiscall<void(oCItem*)> ctor(0x711290);
		ctor(this);
	}

	void InitByScript(int instanceId, int amount)
	{
		Thiscall<void(oCItem*, int, int)> call(0x711BD0);
		call(this, instanceId, amount);
	}

	static oCItem* CreateItem(int instanceId)
	{
		auto item = new oCItem;
		if (instanceId != -1)
			item->InitByScript(instanceId, 1);
		return item;
	}
};
#endif
