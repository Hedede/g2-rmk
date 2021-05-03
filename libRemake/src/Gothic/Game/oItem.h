#ifndef Gothic_Item_H
#define Gothic_Item_H
#include <Hook/Externals.h>
#include <Gothic/Game/zVob.h>
#include <Gothic/Game/zCClassDef.h>
class oCVisualFX;
struct oCItem : public zCVob {
	static zCClassDef* classDef;
	void* operator new(size_t size)
	{
		void* mem = ::operator new(size);

		auto object   = reinterpret_cast<oCItem*>(mem);
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

	std::string GetInstanceName() const
	{
		zSTRING ret;
		Thiscall<void(oCItem const*, zSTRING*)> call(0x713D30);
		call(this, &ret);
		return std::string(ret);
	}


	int id;
	zSTRING name;
	zSTRING nameID;
	int hp;
	int hp_max;
	int mainflag;
	int flags;
	int weight;
	int value;
	int damageType;
	int damageTotal;
	int damage[8];
	int wear;
	int protection[8];
	int nutrition;
	int cond_atr[3];
	int cond_value[3];
	int change_atr[3];
	int change_value[3];
	int magic;
	int on_equip;
	int on_unequip;
	int on_state[4];
	int owner;
	int ownerGuild;
	int disguiseGuild;
	zSTRING item_visual;
	zSTRING visual_change;
	zSTRING effect;
	int visual_skin;
	zSTRING scemeName;
	int material;
	int munition;
	int spell;
	int range;
	int mag_circle;
	zSTRING description;
	zSTRING text[6];
	int count[6];
	int inv_zbias;
	int inv_rotx;
	int inv_roty;
	int inv_rotz;
	int inv_animate;
	int amount;
	int instanz;
	int c_manipulation;
	zREAL last_manipulation;
	int magic_value;
	oCVisualFX *effectVob;
	oCItem *next;
};

CHECK_SIZE(oCItem, 0x348);
#endif//Gothic_Item_H
