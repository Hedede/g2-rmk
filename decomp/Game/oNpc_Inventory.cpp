oCItem* oCNpc::RemoveFromInv(oCItem* item, int amount)
{
	if (item) {
		item = inventory.Remove(item, amount);
		if (item && item->HasFlag(ITEM_ACTIVE)) {
			Equip(item);
		}
	}
	return item;
}

oCItem* oCNpc::RemoveFromInv(int inst, int amount)
{
	oCItem* removed = inventory.Remove(inst, amount);
	if ( removed && removed->HasFlag(ITEM_ACTIVE) )
		Equip(removed);
	return removed;
}
