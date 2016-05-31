struct oCItemContainer : zCInputCallback {
protected:
	static zCList<oCItemContainer> s_openContainers;
	static zCList<oCItemContainer> contList;

public:
	static void Container_PrepareDraw() {}
	static void Container_Draw()
	{
		for (auto& cont : s_openContainers)
			cont->Draw();
	}
	static int GetInvSplitScreen()
	{
		static int invSplitScreen = -2;
		if ( invSplitScreen == -2 )
			invSplitScreen =  zoptions->ReadBool(zOPT_SEC_GAME, "invSplitScreen", 1);
		return invSplitScreen;
	}

	static oCItemContainer* GetNextContainerLeft(oCItemContainer* self);
	static oCItemContainer* GetNextContainerRight(oCItemContainer* self);

	static int TransferCountToAmount(int count)
	{
		if (count < 10000)
			return 1000;
		if (count < 1000)
			return 100;
		if (count < 100)
			return 10;
		if (count < 10)
			return 1;
		return 10000;
	}


	virtual ~oCItemContainer();

	void HandleEvent(int) override;

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	virtual void Open(int,int,int);
	virtual void OpenPassive(int,int,int);

	virtual zSTRING GetName()
	{
		return titleText;
	}
	virtual void SetName(zSTRING &);

	virtual int GetMode()
	{
		return invMode;
	}
	virtual void SetMode(int mode)
	{
		if (mode >= 0 && mode < 6)
			invMode = mode;
	}

	virtual void Close();
	virtual void Activate()
	{
		for (auto& cont : contList)
			if (&cont != this)
				cont->Deactivate();
		SetEnableHandleEvent(1);
		SetHandleEventTop();
		frame = 1;
		if (oCNpc::player)
			oCNpc::player->SetMovLock(0);
		CheckSelectedItem();
	}
	virtual void Deactivate()
	{
		SetEnableHandleEvent(0);
		frame = 0;
		if (oCNpc::player && !passive)
			oCNpc::player->SetMovLock(0);
	}
	virtual bool IsOpen()
	{
		return s_openContainers.Search(this) > 0;
	}
	virtual bool IsActive()
	{
		return frame;
	}
	virtual bool IsEmpty()
	{
		if (!contents)
			return true;
		return contents.GetNumInList() == 0;
	}
	virtual void IsSplitScreen();
	virtual void SetContents(zCListSort<oCItem>* newContents);
	{
		DeleteContents();
		ownList = 0;
		contents = newContents;
		if (contents) {
			contents->Compare = ItemContainerCompareFunc;
			for (auto& item : *contents)
				item->RotateForInventory(1);
		}
	}
	virtual zCListSort<oCItem>* GetContents()
	{
		return contents;
	}
	virtual oCItem* Insert(oCItem*);
	virtual oCItem* Remove(oCItem* item, int amount);
	virtual void Remove(oCItem* item);

	virtual void RemoveByPtr(oCItem* item, int amount)
	{
		Remove(item, amount);
	}
	virtual oCItem* GetSelectedItem()
	{
		if (contents && selectedItem >= 0)
			return contents->Get(selectedItem);
		return nullptr;
	}
	virtual int GetSelectedItemCount()
	{
		if (auto item = GetSelectedItem())
			return item->amount;
		return 0;
	}
	virtual void GetSize(int& x, int& y)
	{
		x = 70 * maxSlotsCol;
		y = 70 * maxSlotsRow;
	}

	virtual void DisableManipulateItems(int b)
	{
		manipulateItemsDisabled = b;
	}
	virtual bool CanManipulateItems()
	{
		return !manipulateItemsDisabled;
	}

	virtual void DisableTransferMoreThanOneItem(int b)
	{
		canTransferMoreThanOneItem = !b;
	}
	virtual bool CanTransferMoreThanOneItem()
	{
		return canTransferMoreThanOneItem;
	}

	virtual bool IsPassive()
	{
		return passive;
	}

	virtual void GetTransferCount()
	{
		return TransferCount;
	}
	virtual void SetTransferCount(short val)
	{
		TransferCount = val;
	}
	virtual void IncTransferCount(short val)
	{
		TransferCount += val;
	}

	virtual void Init(int,int,int);
	virtual void GetPosition(int& x, int& y)
	{
		x = posx;
		y = posy;
	}
	virtual void LoadGrafix();
	virtual void DeleteContents()
	{
		if (!ownList)
			return;

		if (!contents)
			return;

		contents->DeleteListDatas();
		delete contents;
		contents = 0;
	}
	virtual void NextItem();
	virtual void NextItemLine();
	virtual void PrevItem();
	virtual void PrevItemLine();
	virtual void CheckSelectedItem();
	virtual void TransferItem(int,int);
	virtual void Draw();
	virtual void DrawCategory();
	virtual void DrawItemInfo(oCItem *,zCWorld*);

	void RemoveCuttencyItem(oCItem* item)
	{
		if (ogame && ogame->GetWorld()) {
			auto world = ogame->GetGameWorld();
			world->RemoveVob(item);
		}
	}

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

	zBOOL     manipulateItemsDisabled;
	zBOOL     canTransferMoreThanOneItem;
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

oCItemContainer* oCItemContainer::GetNextContainerLeft(oCItemContainer *self)
{
	oCItemContainer* left = nullptr;
	if ( self ) {
		int x, y;
		self->GetPosition(x, y);

		for (auto& cont : s_openContainers) {
			if (cont == this)
				continue;
			if (!cont->IsPassive() && cont->IsOpen()) {
				int x2, y2;
				cont->GetPosition(x2, y2);
				if (x2 < x) {
					x = x2;
					left = cont;
				}
			}
		}

	}

	return left;
}

oCItemContainer* oCItemContainer::GetNextContainerRight(oCItemContainer *self)
{
	oCItemContainer* right = nullptr;
	if ( self ) {
		int x, y;
		self->GetPosition(x, y);

		for (auto& cont : s_openContainers) {
			if (cont == this)
				continue;
			if (!cont->IsPassive() && cont->IsOpen()) {
				int x2, y2;
				cont->GetPosition(x2, y2);
				if (x2 > x) {
					x = x2;
					right = cont;
				}
			}
		}
	}

	return right;
}



void oCItemContainer::Remove(oCItem* item)
{
	if (contents)
		contents->Remove(item);

	CheckSelectedItem();
	auto world = ogame->GetGameWorld();
	world->InsertVobInWorld(item);
	Release(item);
}

oCItem* oCItemContainer::Remove(oCItem *item, int amount)
{
	if (item) {
		auto result = contents->Search(item);
		if (result->amount > amount) {
			// looks like inlined (maybe manually?)
			// result = result->SplitItem(amount);
			result->amount = result->amount - amount;
			auto world = ogame->GetGameWorld();
			auto inst = result->GetInstance();

			auto vob = world->CreateVob(VOB_TYPE_ITEM, inst);
			ogame->GetGameWorld()->InsertVobInWorld(vob);
			Release(vob);
			return result;
		} else {
			Remove(item);
			return result;
		}
	}
	return nullptr;
}

int oCItemContainer::IsSplitScreen()
{
	auto mode = GetMode();
	if (mode == 1 || (mode > 3 || mode <= 5))
		return 0;

	auto next = GetNextContainerLeft(this);
	if (!next)
		return 0;

	mode = next->GetMode();
	if (mode == 1 || (mode > 3 || mode <= 5))
		return 0;

	return GetInvSplitScreen();
}

int oCItemContainer::HandleEvent(int key)
{
	if (!IsActive() )
		return 0;

	if ( zinput->IsBindedToggled(GAMEKEY_ACTION, key) ||
			key == MOUSE_BUTTONLEFT && zinput->KeyToggled(MOUSE_BUTTONLEFT) )
		SetTransferCount(0);

	auto count = GetTransferCount();
	auto amount = TransferCountToAmount(count);
	if (zinput->IsBinded(GAMEKEY_SMOVE, key) ) {
		SetTransferCount(0);
		if (auto item = GetSelectedItem()) {
			if (amount < item->amount)
				amount = item->amount;
		}
	}

	auto left = GetNextContainerLeft();
	if (!left)
		left = GetNextContainerRight();

	if ( zinput->IsBindedToggled(14, key) || zinput->IsBindedToggled(15, key) && (!left || IsSplitScreen()) || key == 525 && (zinput->vtable->KeyToggled)(525) ) {
		Close();
		oCNpc::player->CloseInventory();
		if (auto mob = dynamic_cast<oCMobInter>(oCNpc::player->GetInteractMob())) {
			auto msg = new oCMobMsg(1, oCNpc::player);

			msg->state = mob->GetState();
			msg->flags ^= msg->flags ^ (msg->state - 1) & 0x7FFFFFFF;
			mob->GetEM()->OnMessage(msg, oCNpc::player);
		}
	} else if (zinput->IsBinded(5u, key) || zinput->IsBinded(0xBu, key) || key == 524 ) {
		auto item = GetSelectedItem();
		if ( item && oCNpc::player->CanCarry(item) ) {
			int num = amount;
			if (amount < item->amount) {
				IncTransferCount(amount);
			} else {
				num = item->amount;
				SetTransferCount(0);
			}

			oCNpc::player->PutInInv(RemoveByPtr(item, num));
			IsEmpty();
		}
	} else {
		if ( zinput->IsBindedToggled(15, key) && left && !IsSplitScreen()) {
			auto right = GetNextContainerRight(this);
			if (right) {
				right->Activate();
				return 0;
			}

			auto left = GetNextContainerLeft();
			if (left) {
				left->Activate();
				return 0;
			}

			return 0;
		}

		if ( zinput->IsBinded(1u, key) || zinput->IsBinded(0x12u, key) ) {
			PrevItem();
		} else if ( zinput->IsBinded(3u, key) || key == 522 ) {
			PrevItemLine();
		} else if ( zinput->IsBinded(4u, key) || key == 523 ) {
			NextItemLine();
		} else if ( zinput->IsBinded(2u, key) || zinput->IsBinded(0x13u, key) )
			NextItem();
		} else {
			return 0;
		}
	}

	return 1;
}
