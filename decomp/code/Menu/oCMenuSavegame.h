// _bert\\oMenuSavegame.cpp
struct oCMenuSavegame : oCMenu_Main {
	enum oTMenuSavegameMode {};

	oCMenuSavegame(zSTRING const& name, oTMenuSavegameMode mode)
		: oCMenu_Main(name), mode(mode)
	{ }
	~oCMenuSavegame() override
	{
		Release(thumbPic);
	}

	static int GetMenuItemSlotNr(zCMenuItem* item);

	void HandleEvent(int key) override
	{
		auto ret = oCMenu_Main::HandleEvent(key);
		int sel = selectedSlotId;
		if (mainSel >= 0 && mainSel < listItems.GetNum()) {
			int selItem = mainSel;
			int selectedSlotId = GetMenuItemSlotNr(listItems[mainSel]);
		}
		if (selectedSlotId != sel)
			HandleSlotChange(sel);
		return ret;
	}

	void Activate() override { zCMenu::Activate(); }

	void ScreenInit() override;
	void ScreenDone() override;

	virtual void HandleSlotChange(int oldSel);
	virtual int GetSelectedSlot()
	{
		return selectedSlotId;
	}

	void SetScreenShot(zCTextureConvert* texCvt)
	{
		Release(thumbPic);
		if (texCvt) {
			zCTextureInfo info;
			texCvt->GetTextureInfo(info);

			info.texFormat = zTexture::CalcNextBestTextFormat(8);
			info.sizeX = 256;
			info.sizeY = 256;
			info.numMipMap = 1;

			texCvt->ConvertTextureFormnat(info);
			thumbPic = zrenderer->CreateTexture();

			zCTextureExchange::CopyContents(texCvt, thumbPic);
		}
	}

private:
	oTMenuSavegameMode mode;
	oCSavegameManager* saveMan = nullptr;
	int selectedSlotId = -1;
	int selItem        = -1;
	zCTexture *thumbPic = nullptr;
	zCMenuItem *itemPlayTime  = nullptr;
	zCMenuItem *itemDateTime  = nullptr;
	zCMenuItem *itemLevelName = nullptr;
	zCMenuItem *itemGameTime  = nullptr;
	zCMenuItem *itemThumbPic  = nullptr;
	zCView *view = nullptr;
};

int oCMenuSavegame::GetMenuItemSlotNr(zCMenuItem* item)
{
	if ( !item )
		return -1;

	auto name = item->GetName();
	if ( name.Search(0, "_SLOT", 1u) > 0 ) {
		name.Delete("_SLOT", 3);

		int id = name.ToInt();

		if ( id >= SAVEGAME_SLOT_QUICK && id <= SAVEGAME_SLOT_MAX )
			return id;
	}

	return -1;
}

void oCMenuSavegame::ScreenDone()
{
	zINFO_B(5,"B: MSG: Closing Savegame-Menu"); // 214,

	for (auto i = 0; i < listItems.GetNum(); ++i) {
		zINFO(6, "B: MSG: Set " + listItems[i]->GetName()); //217
		auto slotNr = GetMenuItemSlotNr(listItems[i]);
		if ( slotNr < 0 )
			continue;
		auto save = saveMan->GetSavegame(slotNr);
		if ( !save )
			continue;
		save->title = listItems[i]->GetText();
		zINFO(5, "B: MSG: Savegame "_s + slotNr + ": " + save->GetName()); // 225
	}

	oCMenu_Main::ScreenDone();
	if ( saveMan )
		saveMan->CleanResources();
	Delete(View);
	Release(itemThumbPic);
	zinput->ClearKeyBuffer();

	zINFO_C(5,""); // 239,
}

void oCMenuSavegame::HandleSlotChange(int prevSlot)
{
	auto save = saveMan->GetSavegame(selectedSlotId);

	if ( itemPlayTime ) {
		auto sec = save->playTimeSeconds;
		auto hrs = sec / 3600;
		auto min = src / 60 - 60 * hrs;

		zSTRING text = hrs + "h "_s + min + "min";
		itemPlayTime->SetText(text, 0, 0);
	}

	if ( itemLevelName ) {
		zSTRING world;
		if ( save && !save->saveIncompatible )
			world = save->GetWorldName();
		world.UpperFirstLowerRest();
		if ( save && !save->saveIncompatible && world.Length() > 0) {

			if (save->versionMajor != APP_VERSION_MAJOR
			||  save->versionMinor != APP_VERSION_MINOR
			||  save->versionPoint != APP_VERSION_POINT
			||  save->versionInt   != APP_VERSION_INT) {
				zSTRING maj = save->versionMajor;
				zSTRING min = save->versionMinor;

				zSTRING text = " (" + maj + "." + min;
				if ( save->versionPoint >= 0 ) {
					text += "."_s + save->versionPoint;
					if ( save->versionInt >= 0 ) {
						text += "."_s + save->versionInt;
					}
				}
				text += ")";
				world += text;
			}
		}
		itemLevelName->SetText(world, 0, 1);
	}

	if ( itemGameTime ) {
		zSTRING text;
		if ( save && !save->saveIncompatible )
			text = save->GetGameTime();
		itemGameTime->SetText(text, 0, 1);
	}

	if ( itemDateTime ) {
		zSTRING date;
		if ( save && !save->saveIncompatible )
			date = save->GetSaveTime();

		itemDateTime->SetText(date, 0, 1);
	}

	if ( itemThumbPic ) {
		auto pic = 0;
		if ( !save || save->saveIncompatible ) {
			if ( thumbPic )
				pic = thumbPic;
			else
				save->ReloadResources();
		}
		if ( save && !save->saveIncompatible )
			pic = save->thumbPic;

		if ( pic )
			itemThumbPic->InsertBack(pic);
		else
			itemThumbPic->InsertBack(0);
	}
}

void oCMenuSavegame::ScreenInit()
{
	zINFO_B(5,"B: MSG: Initializing Savegame-Menu"); // 151,

	oCMenu_Main::ScreenInit();
	itemThumbPic  = zCMenuItem::GetByName(zMENUITEM_LOADSAVE_THUMBPIC);
	itemDateTime  = zCMenuItem::GetByName(zMENUITEM_LOADSAVE_DATETIME_VALUE);
	itemLevelName = zCMenuItem::GetByName(zMENUITEM_LOADSAVE_LEVELNAME_VALUE);
	itemGameTime  = zCMenuItem::GetByName(zMENUITEM_LOADSAVE_GAMETIME_VALUE);
	itemThumbPic  = zCMenuItem::GetByName(zMENUITEM_LOADSAVE_PLAYTIME_VALUE);

	int x0, y0, x1, y1;
	if ( itemThumbPic && thumbPic ) {
		itemThumbPic->GetCoords(x0, y0, x1, x1);
		view = new zCView(x0, y0 - y1 + 8000, x1, 8000, 2);

		view->InsertBack(thumbPic);
		screen->InsertItem(view, 0);
	}

	for (auto i = 0; i < listItems.GetNum(); ++i) {
		zINFO(6, "B: MSG: Get " + listItems[i].GetName()); // 178,

		auto slot = GetMenuItemSlotNr(listItems[i]);

		if ( slot < SAVEGAME_SLOT_QUICK || slot > SAVEGAME_SLOT_MAX )
			continue;

		zSTRING title;
		auto save = saveMan->GetSavegame(slot);
		if ( save ) {
			zINFO(5,"B: MSG: Savegame "_s + slot + " found: " + save->GetName()); // 185
			if ( save->saveIncompatible ) {
				title = "xxx";
			} else {
				title = save->GetName();
			}

			if ( title.IsEmpty() )
				title = "???";

			if ( slot != SAVEGAME_SLOT_QUICK )
				listItems[i]->SetText(title);
		} else if ( slot == SAVEGAME_SLOT_QUICK ) {
			listItems[i]->SetIsDisabled(0);
			listItems[i]->SetIsSelectable(!mode || save);
		} else {
			listItems[i]->SetText("---");
		}
	}

	auto sel_slot = oCMenuSavegame::GetMenuItemSlotNr(listItems[mainSel]);
	selectedSlotId = sel_slot;
	HandleSlotChange(-1);

	zINFO_C(5,"");// 204,
}
