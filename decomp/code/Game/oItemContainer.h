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
		static int invSplitScreen = zoptions->ReadBool(zOPT_SEC_GAME, "invSplitScreen", 1);
		return invSplitScreen;
	}

	static int GetInvMaxRows()
	{
		static int invMaxRows = -1;
		if (invMaxRows <= 0)
			invMaxRows = zoptions->ReadInt(zOPT_SEC_GAME, "invMaxRows", 0);

		if (invMaxRows <= 0)
			return 1024;
		
		return invMaxRows;
	}

	static int GetInvMaxColumns()
	{
		static int invMaxRows = -1;
		if (invMaxRows <= 0)
			invMaxRows = zoptions->ReadInt(zOPT_SEC_GAME, "invMaxRows", 0);

		if (invMaxRows <= 0)
			return 1024;
		
		return invMaxRows;
	}

	static oCItemContainer* GetNextContainerLeft(oCItemContainer* self);
	static oCItemContainer* GetNextContainerRight(oCItemContainer* self);

	static int GetCurrencyInstance()
	{
		static int currencyInstance = -1;
		if (currencyInstance == -1) {
			auto name = GetCurrencyInstanceName();
			currencyInstance = zparser.GetIndex(name);
		}
	}

	static oCItem* CreateCurrencyItem(int amount);

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
	virtual oCItem* Insert(oCItem* item);
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

			auto left = GetNextContainerLeft(this);
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
		} else if ( zinput->IsBinded(2u, key) || zinput->IsBinded(0x13u, key) ) {
			NextItem();
		} else {
			return 0;
		}
	}

	return 1;
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

oCItem* oCItemContainer::Insert(oCItem *item)
{
	if ( !item )
		return 0;

	if ( !contents )
		return item;

	if (item->MultiSlot()) {
		for (auto i = contents->next; i; i = i->next) {
			auto item2 = i->Get();
			if ( item2->MultiSlot() ) {
				auto inst = item2->GetInstance()
				if ( inst == item->GetInstance() ) {
					item2->amount += item->amount;
					ogame->GetGameWorld()->RemoveVob(item);
					return item2;
				}
			}
		}
	}

	contents->InsertSort(item);

	CheckSelectedItem();

	item->AddRef();
	ogame->GetGameWorld()->RemoveVob(item);

	return item;
}

oCItem* oCItemContainer::CreateCurrencyItem(int amount)
{
	if ( ogame && ogame->GetWorld() ) {
		zSTRING inst = GetCurrencyInstanceName();
		auto vob = ogame->GetGameWorld()->CreateVob(VOB_TYPE_ITEM, inst);
		auto item = static_cast<oCItem*>(vob);
		if ( item )
			item->amount = amount;
	}
	return item;
}


void oCItemContainer::OpenPassive(int x, int y, int max_items)
{
	auto xdim = zrenderer->view_xdim / 800.0;
	auto ydim = zrenderer->view_ydim / 600.0;
	if ( xdim > 1.0 ) xdim = 1.0;
	if ( ydim > 1.0 ) ydim = 1.0;
	inv_inf_sizex   = screen->anx(300) * xdim;
	inv_inf_sizey   = screen->any(32)  * ydim;
	inv_item_dx     = screen->anx(10)  * xdim;
	inv_item_sizex  = screen->anx(70)  * xdim;
	inv_item_sizey  = screen->any(70)  * ydim;
	inv_item_width  = screen->anx(70)  * xdim;
	inv_item_height = screen->any(70)  * ydim;

	RELEASE( rndWorld );

	rndWorld = new zCWorld();
	rndWorld->isInventoryWorld = true;

	if ( inv_item_width + x > 0x2000 )
		x = 0x2000 - inv_item_width;

	posy = y;
	posx = x;

	right = x > 4095;

	Init(x, y, max_items);

	viewArrowAtTop = new zCView( 0, 0, screen->anx(16), screen->any(16), 2 );
	screen->InsertItem( viewArrowAtTop, 0 );
	viewArrowAtTop->InsertBack(TEX_INV_ARROW_TOP);
	viewArrowAtTop->SetAlphaBlendFunc(2);
	viewArrowAtTop->SetTransparency(96);
	screen->RemoveItem(viewArrowAtTop);

	viewArrowAtBottom = new zCView( 0, 0, screen->anx(16), screen->any(16), 2 );
	screen->InsertItem( viewArrowAtBottom, 0 );
	viewArrowAtBottom->InsertBack(TEX_INV_ARROW_BOTTOM);
	viewArrowAtBottom->SetAlphaBlendFunc(2);
	viewArrowAtBottom->SetTransparency(96);
	screen->RemoveItem(viewArrowAtBottom);

	
	viewTitle = new zCView(0, 0, 10, 10, 2);
	screen->InsertItem( viewTitle, 0 );
	viewTitle->InsertBack(TEX_INV_TITLE);
	viewTitle->SetAlphaBlendFunc(TEX_INV_TITLE_BLEND);
	screen->RemoveItem(viewTitle);



	viewBack = new zCView(0, 0, 10, 10, 2);
	screen->InsertItem( viewBack, 0 );
	viewBack->InsertBack(TEX_INV_BACK);
	viewBack->SetAlphaBlendFunc(TEX_INV_BACK_BLEND);
	screen->RemoveItem(viewBack);


	viewItem = new zCView(0, 0, inv_item_sizex, inv_item_sizey, 2);
	screen->InsertItem( viewItem, 0 );
	viewItem->InsertBack(TEX_INV_ITEM);
	viewItem->SetAlphaBlendFunc(TEX_INV_ITEM_BLEND);
	viewItem->SetTransparency(255);
	screen->RemoveItem(viewItem);


	viewItemActive = new zCView(0, 0, inv_item_sizex, inv_item_sizey, 2);
	screen->InsertItem( viewItemActive, 0 );
	viewItemActive->InsertBack(TEX_INV_ITEM_ACTIVATED);
	viewItemActive->SetAlphaBlendFunc(TEX_INV_ITEM_ACTIVATED_BLEND);
	viewItemActive->SetTransparency(255);
	screen->RemoveItem(viewItemActive);

	viewItemHightlighted = new zCView(0, 0, inv_item_sizex, inv_item_sizey, 2);
	screen->InsertItem( viewItemHightlighted, 0 );
	viewItemHightlighted->InsertBack(TEX_INV_ITEM_HIGHLIGHTED);
	viewItemHightlighted->SetAlphaBlendFunc(TEX_INV_ITEM_HIGHLIGHTED_BLEND);
	viewItemHightlighted->SetTransparency(255);
	screen->RemoveItem(viewItemHightlighted);

	viewItemActiveHighlighted = new zCView(0, 0, inv_item_sizex, inv_item_sizey, 2);
	screen->InsertItem( viewItemActiveHighlighted, 0 );
	viewItemActiveHighlighted->InsertBack(TEX_INV_ITEM_ACTIVATED_HIGHLIGHTED);
	viewItemActiveHighlighted->SetAlphaBlendFunc(TEX_INV_ITEM_ACTIVATED_HIGHLIGHTED_BLEND);
	viewItemActiveHighlighted->SetTransparency(255);
	screen->RemoveItem(viewItemActiveHighlighted);

	textView = new zCView(0, 0, inv_item_sizex, inv_item_sizey, 2);
	screen->InsertItem( screen, textView, 0 );
	textView->SetAlphaBlendFunc(TEX_INV_ITEM_ACTIVATED_BLEND);
	textView->SetTransparency(255);
	screen->RemoveItem(textView);

	viewItemInfo = new zCView(0, 0, inv_inf_sizex, inv_inf_sizey, 2);
	screen->InsertItem( viewItemInfo, 0 );
	viewItemInfo->InsertBack(INV_ITEM_DESC);
	viewItemInfo->SetAlphaBlendFunc(INV_ITEM_DESC_BLEND);
	viewItemInfo->SetTransparency(255);
	screen->RemoveItem(viewItemInfo);

	viewItemInfoItem = new zCView(0, 0, inv_inf_sizex, inv_inf_sizey, 2);
	viewItemInfo->InsertItem( viewItemInfoItem, 0 );
	viewItemInfoItem->SetAlphaBlendFunc(INV_ITEM_DESC_BLEND);
	viewItemInfoItem->SetTransparency(255);
	viewItemInfo->RemoveItem(viewItemInfo);

	if ( !IsOpen() )
		s_openContainers.Insert( this );
	if ( m_bManipulateItemsDisabled )
		CheckSelectedItem();
	passive = 1;
}
