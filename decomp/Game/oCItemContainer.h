class oCItemContainer {
public:
	virtual void HandleEvent(int);
	virtual ~oCItemContainer();
	virtual void Open(int,int,int);
	virtual void OpenPassive(int,int,int);
	virtual void GetName();
	virtual void SetName(zSTRING &);
	virtual void GetMode();
	virtual void SetMode(int);
	virtual void Close();
	virtual void Activate();
	virtual void Deactivate();
	virtual void IsOpen();
	virtual void IsActive();
	virtual void IsEmpty();
	virtual void IsSplitScreen();
	virtual void SetContents(zCListSort<oCItem> *);
	virtual void GetContents();
	virtual void Insert(oCItem *);
	virtual void Remove(oCItem *,int);
	virtual void Remove(oCItem *);
	virtual void RemoveByPtr(oCItem *,int);
	virtual void GetSelectedItem();
	virtual void GetSelectedItemCount();
	virtual void GetSize(int &,int &);
	virtual void DisableManipulateItems(int);
	virtual void CanManipulateItems();
	virtual void DisableTransferMoreThanOneItem(int);
	virtual void CanTransferMoreThanOneItem();
	virtual void IsPassive();
	virtual void GetTransferCount();
	virtual void SetTransferCount(short);
	virtual void IncTransferCount(short);
	virtual void Archive(zCArchiver	&);
	virtual void Unarchive(zCArchiver &);
	virtual void Init(int,int,int);
	virtual void GetPosition(int &,int &);
	virtual void LoadGrafix();
	virtual void DeleteContents();
	virtual void NextItem();
	virtual void NextItemLine();
	virtual void PrevItem();
	virtual void PrevItemLine();
	virtual void CheckSelectedItem();
	virtual void TransferItem(int,int);
	virtual void Draw();
	virtual void DrawCategory();
	virtual void DrawItemInfo(oCItem *,zCWorld*);
protected:
	zCListSort<oCItem>*    contents;

	oCNpc*    npc;
	zSTRING   titleText;
	int       invMode;
	int       selectedItem;
	int       offset;
	int       maxSlotsCol;
	int       maxSlotsColScr;
	int       maxSlotsRow;
	int       maxSlotsRowScr;
	int       maxSlots;
	int       marginTop;
	int       marginLeft;
	zBOOL     frame;
	zBOOL     right;
	zBOOL     ownList;
	zBOOL     prepared;
	zBOOL     passive;
	zINT      TransferCount;
	zCView*   viewTitle;
	zCView*   viewBack;
	zCView*   viewItem;
	zCView*   viewItemActive;
	zCView*   viewItemHightlighted;
	zCView*   viewItemActiveHighlighted;
	zCView*   viewItemInfo;
	zCView*   viewItemInfoItem;
	zCView*   textView;
	zCView*   viewArrowAtTop;
	zCView*   viewArrowAtBottom;
	zCWorld*  rndWorld;
	int       posx;
	int       posy;
	zBOOL     m_bManipulateItemsDisabled;
	zBOOL     m_bCanTransferMoreThanOneItem;
};

void oCItemContainer::Archive(zCArchiver& arc)
{
	if ( !arc.InSaveGame() )
		return;

	int numItems = contents.GetNumInList();

	arc.WriteGroupBegin( "contents");
	arc.WriteInt("itemNr", numItems);

	int id = 0;
	for (auto j = contents->next; j; j = j->next, ++id) {
		zSTRING name = "item" + id;
		arc.WriteObject(name, j->data);
	}

	arc->WriteGroupEnd("contents");
}

void oCItemContainer::Unarchive(zCArchiver& arc)
{
	if ( !arc.InSaveGame() )
		return;

	int numItems = 0;
	arc.ReadInt("itemNr", &numItems);

	for (int i = 0; i < numItems; ++i) {
		zSTRING name = "item" + i;

		auto object = arc.ReadObject(name, 0);
		Insert(object);
	}
}
