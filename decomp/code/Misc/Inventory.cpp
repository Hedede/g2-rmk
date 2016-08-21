uint32_t Inventory_GetCategory(oCItem* item)
{
	switch (item->mainflag) {
	case ITEM_KAT_NONE:
		return zINV_MISC;
	case ITEM_KAT_ARMOR:
		return zINV_ARMOR;
	case ITEM_KAT_NF:
		return zINV_WEAPON;
	case ITEM_KAT_MAGIC;
		return zINV_MAGIC;
	case ITEM_KAR_FF:
	case ITEM_KAT_MUN:
		return zINV_WEAPON;
	case ITEM_KAT_POTIONS:
		return zINV_POTION;
	case ITEM_KAT_FOOD:
		return zINV_FOOD;
	case ITEM_KAT_DOCS:
		return zINV_DOC;
	case ITEM_KAT_RUNE:
		return zINV_RUNE;
	default:
		return zINV_MISC;
	}
}
