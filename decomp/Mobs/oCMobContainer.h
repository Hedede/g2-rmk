class oCMobContainer : public oCMobLockable {
	Z_OBJECT(oCMobContainer);
public:
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual ~oCMobContainer();
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void Destroy();
	virtual void Reset();
	virtual void IsIn(int);
	virtual void Open(oCNpc *);
	virtual void Close(oCNpc *);
	virtual void Insert(oCItem *);
	virtual void Remove(oCItem *,int);
	virtual void Remove(oCItem *);
	virtual void CreateContents(zSTRING const &);
private:
    //oCMobContainer
    zSTRING contains;                //zSTRING //Scriptinstanzen der Items. Nicht aktuelle gehalten! Original Spacereinstellung!
    
    oCItemContainer* items;
    zCListSort<oCItem>    containList; //sollte die richtigen Daten beinhalten
};

void oCMobContainer::Archive(zCArchiver& arc)
{
	oCMobLockable::Archive(arc);

	arc.WriteGroupBegin("Container");
	arc.WriteString("contains", contains);
	arc.WriteGroupEnd("Container");

	if ( arc.InSaveGame() ) {
		int numEntries = containList.GetNumInList();
		arc.WriteInt("NumOfEntries", numEntries);

		for (int i = 0 ; i < numEntries; ++i)
			arc.WriteObject(Get(i));
	}
}

void oCMobContainer::Unarchive(zCArchiver& arc)
{
	oCMobLockable::Unarchive(arc);

	arc.ReadString("contains", contains);
	pickLockStr.Upper();

	if ( arc.InSaveGame() ) {
		for (auto i = containList.next; i; i = i->next)
			i->data->Release();

		containList.DeleteListDatas();

		int numEntries = 0;
		arc.ReadInt("NumOfEntries", numEntries);

		for (int i = 0; i < numEntries; ++i) {
			auto obj = arc.ReadObject(0);
			auto item = dynamic_cast<oCItem>(obj);

			if ( item ) {
				containList.Insert(item);
				item->AddRef();
			}

			if ( ogame ) {
				auto world = dynamic_cast<oCWorld>(ogame->GetWorld());
				if ( world )
					 world->InsertVobInWorld(item);
			}

			if ( item )
				item->Release();
		}
	}
	else
	{
		CreateContents(contains);
	}
}
