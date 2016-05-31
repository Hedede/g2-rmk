enum zINV_CATEGORY {
	zINV_WEAPON = 1,
	zINV_ARMOR = 2,
	zINV_RUNE = 3,
	zINV_MAGIC = 4,
	zINV_FOOD = 5,
	zINV_POTION = 6,
	zINV_DOC = 7,
	zINV_MISC = 8,
	zINV_CAT_MAX = 9,
};

class oCNpcInventory : public oCItemContainer {
public:
	virtual void ~oCNpcInventory();;
	virtual void Open(int,int,int);
	virtual void Close(void);
	virtual void Insert(oCItem *);
	virtual void Remove(oCItem *,int);
	virtual void Remove(oCItem *);
	virtual void RemoveByPtr(oCItem *,int);
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual void Draw(void);
	virtual void DrawCategory(void);
	virtual void Remove(zSTRING const &,int);
	virtual void Remove(int,int);

	virtual void IsIn(int,int);
	virtual void IsIn(oCItem *,int);
	virtual void IsIn(zSTRING const& name, int amount)
	{
		int instIndex = zCParser::GetIndex(&zparser, name);
		return IsIn(instIndex, amount);
	}

	virtual void IsEmpty(int,int);

	void SetOwner(oCNpc* npc)
	{
		owner = npc;
	}

	oCNpc* GetOwner()
	{
		return owner;
	}

private:
	oCNpc*    owner;
	zBOOL     packAbility;
	zCListSort<oCItem> inventory;
	zSTRING packString;
	int     maxSlots;
};

void oCNpcInventory::Archive(zCArchiver& arc)
{
	PackItemsInCategory(0);

	arc.WriteString("packed", packString);

	int numInv = inventory.GetNumInList();

	arc.WriteGroupBegin("contents");
	arc.WriteInt("itemCount", numInv);

	int i = 0;
	for (auto j = inventory.next; j; ++j) {
		arc.WriteObject("item" + i++);
		if (j->data->mainflag == ITEM_KAT_RUNE ) {
			int key = 0;
			if ( owner && owner->GetSpellBook() )
				key = owner->GetSpellBook()->GetKeyByItem(j->data);

			arc.WriteInt("shortKey" + i, key);
		}
		j = j->next;
	}

	arc.WriteGroupEnd("contents");
}


void oCNpcInventory::Unarchive(zCArchiver& arc)
{
	if (arc.InSaveGame())
		return;

	arc.ReadString("packed", packString);

	int itemCount = 0;
	arc.ReadInt("itemCount", itemCount);
	for (int i = 0; i < itemCount; ++i) {
		auto obj = arc.ReadObject("item" + i, 0);

		oCItem* item = obj;
		if (!item) {
			zWARNING("C: could not read inventory item");
			break;
		}

		int key = -1;
		if ( item->mainflag == ITEM_KAT_RUNE ) {
			arc.ReadInt("shortKey" + i, key);

			if (owner && owner->GetSpellBook())
				owner->GetSpellBook()->NextRegisterAt(key);
		}

		if (item->HasFlag(ITEM_ACTIVE))
			item->ClearFlag(ITEM_ACTIVE);

		Insert(item);

		if (item->HasFlag(ITEM_ACTIVE)) {
			if (Inventory_GetCategory(item) == zINV_WEAPON ||
			    Inventory_GetCategory(item) == zINV_MAGIC  ||
			    Inventory_GetCategory(item) == zINV_ARMOR)
			{
				if (item->mainflag < ITEM_KAT_ARMOR &&
				    item->mainflag < ITEM_KAT_NF    &&
				    item->mainflag == ITEM_KAT_MAGIC )
					item->SetFlag(ITEM_ACTIVE);
			} else {
				owner->Equip(item);
			}
		}

		item->Release();
	}
}


oCItem* oCNpcInventory::CreateFromPackString(zSTRING const& packstr)
{
	if ( !packAbility )
		return nullptr;

	oCItem* result = nullptr;

	int amount;
	int unk2;

	if ( GetPackedItemInfo(packstr, 1, amount, unk2) ) {
		auto world = ogame->GetGameWorld();
		auto item_vob = (oCItem *)world->CreateVob(VOB_TYPE_ITEM, packstr);
		if ( item_vob ) {
			item_vob->amount = amount;
			result = Insert(item_vob);
			item_vob.Release();
		}
	}

	return result;
}

oCItem* oCNpcInventory::IsIn(int inst, int amount)
{
	oCItem *res = nullptr;
	for (auto i = inventory.next; i; i = i->next ) {
		res = i->data;
		if ( res->GetInstance() == inst && res->amount >= amount )
			return res;
	}

	if ( packAbility ) {
		auto* sym = zparser->GetSymbol(inst);
		if ( sym )
		{
			res = CreateFromPackString(sym->name);
			if ( res && res->amount >= amount )
				return res;
		}
	}

	return nullptr;
}

oCItem* oCNpcInventory::IsIn(oCItem *item, int amount)
{
	oCItem *res = nullptr;;
	if (!item)
		return res;

	for (auto i = inventory.next; i; i = i->next ) {
		res = i->data;
		if (res == item && res->amount >= amount)
			return res;
	}

	return nullptr;
}

oCItem* oCNpcInventory::Remove(int instanceId, int amount)
{
	oCItem* item = nullptr;
	zCListSort<oCItem>* node = inventory.next;
	for (; node; node = node->next) {
		item = node->data;
		if ( item->GetInstance() == instanceId )
			return Remove(item, amount);
	}

	if ( packAbility ) {
		sym = zparser.GetSymbol(instanceId);

		auto* sym = zparser->GetSymbol(inst);

		if ( sym )
		{
			item = CreateFromPackString(sym->name);
			if ( item && item->amount >= amount )
				return Remove(item);
		}
	}

	return 0;
}
