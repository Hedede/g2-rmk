class oCMobItemSlot : public oCMobInter {
	Z_OBJECT(oCMobItemSlot);
public:
	virtual void Archive(zCArchiver&);
	virtual void Unarchive(zCArchiver&);
	virtual ~oCMobItemSlot();
	virtual void GetUseWithItem();
	virtual void CanInteractWith(oCNpc*);
	virtual void IsIn(int);
	virtual void GetInsertedItem();
	virtual void PlaceItem(oCItem*);
	virtual void RemoveItem();
private:
	oCItem* insertedItem;
	zBOOL itemRemoveable;
};

void oCMobItemSlot::Archive(zCArchiver& arc)
{
	oCMobInter::Archive(arc);

	arc.WriteBool("itemRemoveable", itemRemoveable);
	if ( !arc.InProperties() )
		arc.WriteObject("insertedItem", insertedItem);
}

void oCMobItemSlot::Unarchive(zCArchiver& arc)
{
	oCMobInter::Unarchive(arc);

	arc.ReadBool("itemRemoveable", itemRemoveable);
	if ( !arc.InProperties() )
		insertedItem = static_cast<oCItem*>(arc.ReadObject("insertedItem", 0));
}
