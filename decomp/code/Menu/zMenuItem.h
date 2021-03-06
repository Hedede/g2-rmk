// _carsten/zMenu_Item.cpp
enum zMenuItemFlags {
	IT_CHROMAKEYED    = 1,
	IT_TRANSPARENT    = 2,
	IT_SELECTABLE     = 4,
	IT_MOVEABLE       = 8,
	IT_TXT_CENTER     = 0x10,
	IT_DISABLED       = 0x20,
	IT_FADE           = 0x40,
	IT_EFFECTS_NEXT   = 0x80,
	IT_ONLY_OUT_GAME  = 0x100,
	IT_ONLY_IN_GAME   = 0x200,
	IT_PERF_OPTION    = 0x400,
	IT_MULTILINE      = 0x800,
	IT_NEEDS_APPLY    = 0x1000, // die mit dem Menuepunkt verknuepfte Option wird NUR ueber ein APPLY aktiv
	IT_NEEDS_RESTART  = 0x2000, // die mit dem Menuepunkt verknuepfte Option wird NUR ueber ein RESTART aktiv
	IT_EXTENDED_MENU  = 0x4000,
};


enum zMenuItemType {
	MENU_ITEM_UNDEF     = 0,
	MENU_ITEM_TEXT      = 1,
	MENU_ITEM_SLIDER    = 2,
	MENU_ITEM_INPUT     = 3,
	MENU_ITEM_CURSOR    = 4,
	MENU_ITEM_CHOICEBOX = 5,
	MENU_ITEM_BUTTON    = 6,
	MENU_ITEM_LISTBOX   = 7,
};

const int MAX_USERSTRINGS = 10;
const int MAX_SEL_ACTIONS =  5;
const int MAX_USERVARS    =  4;
const int MAX_EVENTS      = 10;

struct zCMenuItem : zCView {
	static int CompareNames(zCMenuItem* const* a, zCMenuItem* const* b)
	{
		return strcmp(a.Data(), b.Data());
	} 

	zCMenuItem* tmp;
	zCArraySort<zCMenuItem> itemList;
	static void Startup()
	{
		itemList.Compare = CompareNames;
		zCMenuItem* tmp = new zCMenuItem;
	}

	static void Shutdown()
	{
		while (!itemList.Empty())
			itemList[0]->Unregister();

		Release(zCMenuItem::tmp);
	}

	static zCMenuItem* GetByName(zSTRING const& name)
	{
		zCMenuItem::tmp->SetName(name);
		auto idx = itemList.Search(tmp);
		if (idx == -1)
			return nullptr;
		return itemList[idx];
	}

	zCMenuItem() : zCView() { Init(); }
	zCMenuItem(zSTRING const& name) : zCMenuItem();
	{
		id = name;
		id.Upper();
		SetByScript(id);
		registeredCPP = 1;
	}
	~zCMenuItem() override
	{
		font = 0;
		fontHi = 0;
		fontSel = 0;
		fontDis = 0;
		Delete(innerWindow);
	}
	int HandleEvent(int key)
	{
		if ( key == KEY_ESCAPE || key == MOUSE_BUTTONRIGHT )
			viewInitialized = 1;

		if ( par.onEventAction[5] )
			return zCMenu::GetParser()->CallFunc(action);
		return 1;
	}

	void DrawItems() override
	{
		if (!dontRender) {
			zCView::DrawItems();
			if (innerWindow)
				UpdateContent();
		}
	}

	virtual void InitMenuItem()
	{
		SetText(GetText(0), 0, 0);
	}

	virtual int ToggleValue(int step, int loop)
	{
		auto flags = par.itemFlags;
		if (flags & IT_SELECTABLE && !flags & IT_TXT_CENTER)
			zotions->WriteInt(zOPT_SEC_INTERNAL, "perfQualityIndex", 0, 0);
		return 0;
	}

	virtual int Run()
	{
		SetIsActive(1);
		SetEnableHandleEvent(1);

		Enter();

		leaveItem = 0;

		do {
			zCMenu::GetActive()->HandleFrame(1);
		} while ( !leaveItem );

		Leave();

		SetEnableHandleEvent(0);
		SetIsActive(0);

		return 0;
	}

	virtual zSTRING GetText(int index)
	{
		if (index < listLines.GetNum())
			return listLines[index];
		return "";
	}
	virtual void SetText(zCArray<zSTRING> textLines, int draw_now)
	{
		zCMenuItem *v4; // ebx@1
		int v5; // edi@1
		zSTRING *v6; // eax@2
		const zSTRING *v7; // eax@2
		void **v8; // ecx@7
		char *v9; // esi@7
		zSTRING *v10; // ebp@10
		int v11; // ebx@10
		signed int v12; // eax@11
		int v13; // eax@15
		zSTRING *v14; // ecx@15
		std::string *v15; // eax@16
		int result; // eax@20
		zCMenuItem *v17; // [sp+10h] [bp-3Ch]@1
		zSTRING a1; // [sp+18h] [bp-34h]@2
		zSTRING v19; // [sp+2Ch] [bp-20h]@2
		int v20; // [sp+44h] [bp-8h]@22
		int v21; // [sp+48h] [bp-4h]@1
		zSTRING *retaddr; // [sp+4Ch] [bp+0h]@10

		v4 = 0;
		v5 = 0;
		v17 = 0;
		v21 = 0;

		zINFO(7,"B: MenuItem " + id + " text: textarray"); //543

		listLines.Clear();
		// appears to be bug in original code
		for (auto line : listLines) {
			listLines.InsertEnd(line);
			FormatText(listLines.Back()); // .Back() is pseudocode
		}

		DrawFront();
		if ( textLines.GetNum() > 0 )
			DrawItems();
	}

	virtual void SetText(zSTRING const &,int,int);
	virtual void SetMinValue(float) {}
	virtual void SetMaxValue(float) {}
	virtual void Enter() {}
	virtual void Leave() {}
	virtual void Draw()
	{
		if ( !dontRender ) {
			DrawFront();
			DrawItems();
		}
	}

	virtual void DrawFront();
	{
		if ( owner && innerWindow && !dontRender ) {
			auto text = GetText(text, 0);
			UpdateFX();

			ClrPrintwin(innerWindow);
			auto flags = par.itemFlags;
			if ( flags & IT_MULTILINE8 ) {
				innerWindow->Printwin(text + "\n");
			} else {
				if (flags & IT_TXT_CENTER)
					innerWindow->PrintCXY(text);
				else
					innerWindow->Print(0, 0, text);
			}
		}
	}

	virtual bool Show()
	{
		if (orgWin) {
			orgWin->InsertItem(this, 0);
			this->InsertItem(innerWindow, 0);
			return 1;
		}
		return 0;
	}
	virtual bool Hide()
	{
		if (orgWin) {
			if (innerWindow)
				this->RemoveItem(innerWindow);
			orgWin->RemoveItem(this);
			return 1;
		}
		return 0;
	}
	virtual bool HasBeenCanceled() { return false; }
	virtual void Open()
	{
		open = 1;
		opened = 0;
		close = 0;
		closed = 0;
		openDelayTimer = 0;
	}
	virtual void Input(int,zSTRING &);
	virtual void InsertInWin(zCView *);
	virtual void RemoveFromWin()
	{
		Delete( innerWindow );
		if ( orgWin )
			orgWin->RemoveItem(this);
		opened = 0;
		open = 0;
		close = 0;
		closed = 0;
		active = 0;
		activeTimer = 0.0;
		fxTimer = 0.0;
		openDelayTimer = 0.0;
	}
	virtual void UpdateContent() {}
	virtual void UpdateFX()
	{
		if ( isVisible )
		{
			auto flags = this->par.itemFlags;
			bool disabled = GetIsDisabled();

			zCOLOR col;
			zCFont* fnt;
			if ( active ) {
				col = GFX_WHITE;
				fnt = fontSel;
			} else if ( disabled ) {
				col = zCOLOR{-81, -81, -81, 220};
				fnt = fontDis;
			} else {
				col = GFX_WHITE;
				fnt = fontM;
			}

			innerWindow->SetFont(fnt);
			innerWindow->SetFontColor(col);
			innerWindow->SetTransparency(par.alpha);
			UpdateSize();
		}
	}

	void Release()
	{
		if ( --refCtr <= 0)
			if (registeredCPP)
				delete this;
	}

	void Close()
	{
		close = 1;
		closed = 0;
	}

	int GetType()
	{
		return par.type;
	}

	bool IsRegistered()
	{
		return itemList.Search(this) != -1;
	}

	void Unregister()
	{
		if (!itemList.Empty()) {
			itemList.Remove(this);
			this->Release();
		}
	}

	void Register()
	{
		if (!IsRegistered()) {
			itemList.InsertSort(this);
			++refCtr;
		}
	}

	zCMenuItem* GetSelItem()
	{
		for (auto i = par.onSelAction; *i; ++i) {
			if (*i == 3)
				return GetByName(par.onSelAction_S);
		}
		return nullptr;
	}

	void GetCoords(int& x0, int& y0, int& x1, int& y1)
	{
		zCView::GetScreenExtends(x0, y0, x1, y1);
	}

	void SetIsVisisble(bool b)
	{
		isVisible = b;
	}

	bool GetIsVisisble() const
	{
		return isVisible;
	}

	void SetIsDisabled(int b)
	{
		// optimizer cut out code?
		par.itemFlags &= IT_DISABLED;
		DrawFront();
	}

	int GetIsDisabled() const
	{
		if (!GetIsVisisble())
			return 1;

		auto flags = par.itemFlags;
		if ((flags & IT_ONLY_IN_GAME) && !zCMenu::inGameMenu)
			return 1;
		if ((flags & IT_ONLY_OUT_GAME) && zCMenu::inGameMenu)
			return 1;
		return par.itemFlags & IT_DISABLED;
	}

	void SetIsSelectable(int b)
	{
		if (b) {
			par.itemFlags |= IT_SELECTABLE;
		} else {
			par.itemFlags &= ~IT_SELECTABLE;
			if (active)
				zCMenu::GetActive()->NextOption();
		}
		DrawFront();
	}

	int GetIsSelectable()
	{
		if (GetIsDisabled())
			return false;
		if (dontRender)
			return false;

		auto flags = par.itemFlags;
		if ( (flags & IT_ONLY_IN_GAME) && !zCMenu::inGameMenu )
			return false;
		if ( flags & IT_ONLY_OUT_GAME && zCMenu::inGameMenu )
			return false;
		return par.itemFlags & IT_SELECTABLE;
	}

	void SetName(zSTRING& name)
	{
		id = name;
		id.Upper();
	}

	void SetName(zSTRING const& name)
	{
		id = name;
	}

	zSTRING const& GetName() const
	{
		return id;
	}

	int GetEventFunc(zTMenuItemEvent evt) const
	{
		return par.onSelAction[evt];
	}

	int GetSelActionType(int idx) const
	{
		return par.onSelAction[idx];
	}

	zSTRING const& GetSelActionString(int idx) const
	{
		return par.onSelAction_S[idx];
	}

	int GetDimY(int parDimY)
	{
		if ( parDimY < 0 ) {
			zCView* inWin = innerWindow;
			if ( !inWin )
				inWin = this;
			auto font = inWin->GetFont();
			if ( font && owner ) {
				auto y = nay(par.frameSizeY);
				auto fontY = font->GetFontY();
				return owner->any(y + fontY);
			}
			return 0;
		}
		return parDimY;
	}

	int GetDimX(int parDimX)
	{
		if ( parDimX < 0 ) {
			zCView* inWin = innerWindow;
			if ( !inWin )
				inWin = this;
			auto font = inWin->GetFont();
			if ( font && owner ) {
				auto x = nax(par.frameSizeX);
				auto fontX = font->GetFontX(GetText(0));
				return owner->anx(x + fontX);
			}
			return 0
		}
		return parDimX;
	}

	void UpdateSize()
	{
		int dimY = GetDimY(par.dimY); // was inlined
		int dimX = GetDimX(par.dimX);
		SetSize(dimX, dimY);
		if (innerWindow) {
			innerWindow->SetPos(par.frameSizeX, par.frameSizeY);
			innerWindow->SetSize(2*(4096 - par.frameSizeX), 2*(4096 - par.frameSizeY));
		}
	}

	void InitView()
	{
		if ( !viewInitialized ) {
			int dimX = par.dimX;
			int dimY = par.dimY;
			if ( dimX == -1 || dimY == -1 ) {
				auto text = GetText(0);
				Init(0, 0, text, 2);
			} else {
				Init(0, 0, dimX, dimY, 2);
			}
			SetPos(par.posX, par.posY);
			viewInitialized = 1;
		}
	}

	void SetIsActive(int b)
	{
		active = b;
		if ( zCMenu::GetActive() ) {
			if ( active ) {
				zCMenu::GetActive()->PrintInformation(par.text[1]);
			} else {
				zCMenu::GetActive()->PrintInformation("");
			}

		}
		DrawFront();
	}

	void SetByScript(zSTRING& name);

public:
	// m_parYYYY in original code
	struct {
		zSTRING fontName;
		zSTRING text      [MAX_USERSTRINGS];
		zSTRING backPic;
		zSTRING alphaMode;

		int alpha;
		int type;

		int     onSelAction     [MAX_SEL_ACTIONS];
		zSTRING onSelAction_S   [MAX_SEL_ACTIONS];

		zSTRING onChgSetOption;
		zSTRING onChgSetOptionSection;

		int onEventAction[MAX_EVENTS];

		int posX;
		int posY;
		int dimX;
		int dimY;
		int sizeStartScale;
		int itemFlags;
		int openDelayTime;
		int openDuration;

		int     userFloat  [MAX_USERVARS];
		zSTRING userString [MAX_USERVARS];

		int frameSizeX;
		int frameSizeY;
		zSTRING hideIfOptionSectionSet;
		zSTRING hideIfOptionSet;
		int hideOnValue;
	} par;

private:
	int refCtr;
	zCView*                   innerWindow;
	zCFont*                   pFont;
	zCFont*                   pFontHi;
	zCFont*                   pFontSel;
	zCFont*                   pFontDis;

	zBOOL                     viewInitialized;
	zBOOL                     leaveItem;
	zBOOL                     isVisible;
	zBOOL                     dontRender;

	zCArray<zSTRING> listLines;

	zSTRING id;
	int inserted;
	int changed;
	int active;
	int open;
	int close;
	int opened;
	int closed;
	int disabled;

	zCView*  orgWin;

	float fxTimer;
	float openDelayTimer;

	float activeTimer;
	zBOOL registeredCPP;
	zBOOL firstTimeInserted;
};

struct zCMenuItemInput : zCMenuItem {
	void Leave() override { }
}

//--------------------------------------------------------------------------------
void zCMenuItem::SetByScript(zSTRING& name)
{
	SetName(name);

	if ( zCMenu::GetParser()->GetSymbol(name) )
		zCMenu::GetParser()->CreateInstance(name, (void*)&par);
	par.backPic.Upper();
	SetAlphaBlendFunc(zCRenderer::AlphaBlendFuncStringToType(par.alphaMode));

	auto fontName = par.fontName;
	auto fnt = zfontman->Load(fontName);
	font = zfontman->GetFont(fnt);

	if ( fontName.Search(0, "_hi", 1u) < 0 ) {
		auto pos = fontName.Search(0, ".", 1);
		fontName.Insert(pos, "_hi");
	}

	fnt = zfontman->Load(fontName);
	font = zfontman->GetFont(fnt);
	fontDis = font;
	fontHi = v11;
	fontSel = v11;

	Register();

	int num = -1;
	for (int i = 0; i < MAX_USERSTRINGS; ++i) {
		if (text[i].NotEmpty())
			num = i;
	}

	for (int i = 0; i < num; ++i)
		listLines.Insert(text[i]);
}


int zCMenuItem::Input(int len, zSTRING& ls)
{
	zSTRING s = ls;
	SetText(s + "_", 0, 0);

	int key = -1;
	int num = 0;
	while ( 1 ) {
		zCMenu::GetActive()->HandleFrame(1);
		key = zinput->GetKey(1, 1);
		switch (key) {
		case KEY_ESCAPE:
		case KEY_RETURN:
		case MOUSE_BUTTONLEFT:
		case MOUSE_BUTTONRIGHT:
			break;

		case KEY_DELETE:
			s.Clear();
			SetText(s + "_");
			continue;

		case KEY_BACK:
			if (num > 0) {
				s.Delete(s.Length() - 1, 1);
				SetText(s + "_");
				--num;
			}
			continue;
		}

		if ( key > KEY_DELETE ) {
			int shift = (zinput->KeyPressed(KEY_RSHIFT) || zinput->KeyPressed(KEY_LSHIFT)) ? KEY_LSHIFT : 0;
			int alt   = zinput->KeyPressed(KEY_RMENU) ? KEY_RMENU : 0;

			char c = GetCharFromDIK(key, shift, alt);
			if ( num < len && c >= ' ' && c <= '\xFF' ) {
				s += c;
				SetText(s + "_", 0, 0);
				++num;
			}
		}
	}

	ls = s;
	SetText(s);
	return key == KEY_RETURN || key == MOUSE_BUTTONLEFT;
}

void zCMenuItem::Init()
{
	memset(par.onEventAction, 0, 0x24u);
	for (auto& s : par.onSelAction_S)
		s.Clear();
	inserted = 0;
	changed = 1;
	active = 0;
	activeTimer = 0;
	orgWin = 0;
	open = 0;
	close = 0;
	opened = 0;
	closed = 0;
	fxTimer = 0;
	openDelayTimer = 0;
	par.alpha = 254;
	par.type = 0;
	par.openDuration = -1.0;
	par.sizeStartScale = 1.0;
	par.itemFlags = 0;
	par.openDelayTime = 0;
	par.posX = 0;
	par.posY = 0;
	par.dimX = -1;
	par.dimY = -1;
	par.frameSizeX = 0;
	par.frameSizeY = 0;
	disabled = 0;
	registeredCPP = 0;
	isVisible = 1;
	dontRender = 0;
	viewInitialized = 0;
	refCtr = 1;
	innerWindow = 0;
	fontM = 0;
	fontHi = 0;
	fontSel = 0;
	fontDis = 0;
	firstTimeInserted = 1;
	par.fontName.data.Clear();
	par.backPic.data.Clear();
	par.alphaMode.data.Clear();
	par.onChgSetOption.data.Clear();
	id.Clear();
	for (auto& s : par.text)
		s.Clear();
	for (auto& f : par.userFloat)
		f = 0;
	for (auto& s : par.userString)
		s.Clear();

	par.hideIfOptionSet.Clear();
	par.hideIfOptionSectionSet.Clear();
	par.hideOnValue = 0;
}

void zCMenuItem::SetText(zSTRING const& txt, int line, int draw_now)
{
	if ( line < 0 )
		return;
	zINFO(7,"B: MenuItem " + id + " text: " + txt + " (line " + line + ")"); //514

	while (listLines.GetNum() <= line)
		listLines.InsertEnd("");

	listLines[line] = txt;

	FormatText(listLines[line]);
	UpdateSize();
	DrawFront();
	if ( draw_now )
		DrawItems();
}

	int GetDimX(int parDimX)
	{
		if ( parDimX < 0 ) {
			zCView* inWin = innerWindow;
			if ( !inWin )
				inWin = this;
			auto font = inWin->GetFont();
			if ( font && owner ) {
				auto x = nax(par.frameSizeX);
				auto fontX = font->GetFontX(GetText(0));
				return owner->anx(x + fontX);
			}
			return 0
		}
		return parDimX;
	}

void zCMenuItem::InsertInWin(zCView* orgWindow)
{
	if ( owner )
		RemoveFromWin();
	if ( isVisible ) {
		orgWin = orgWindow;
		orgWin->InsertItem(this, 0);
		SetPos(par.posX, par.posY);
		SetTransparency(par.alpha);
		SetFont(par.fontName);

		auto x = GetDimY(par.dimY);
		auto y = GetDimX(par.dimX);
		SetSize(x, y);

		if ( par.backPic.Search(0, ".TGA", 1u) > 0 ) {
			zSTRING backPic = par.BackPic;
			if ( par.backPic.Search(0, "|", 1u) > 0 ) {
				backPic = backPic.PickWord(inGameMenu ? 2 : 1, "|", "|");
			}
			if (!backPic.IsEmpty())
				InsertBack(&backPic);
			else
				InsertBack(nullptr);
		}

		innerWindow = new zCView(
				par.frameSizeX,
				par.frameSizeY,
				0x2000 - par.frameSizeX,
				0x2000 - par.frameSizeY,
				2);

		v23 = this->par.alpha;
		v47 = this->fontColor;
		v47.color[3] = v23;
		v24 = operator new(0x100u);
		orgWindow = v24;
		v56 = 5;
		if ( v24 )
			v25 = zCView::zCView(
		else
			v25 = 0;
		v56 = -1;

		InsertItem(innerWindow, 0);
		SetPos(innerWindow, par.frameSizeX, par.frameSizeY);
		innerWindow->SetSize(2 * (4096 - par.frameSizeX), 2 * (4096 - par.frameSizeY));
		innerWindow->SetFont(par.fontName);
		innerWindow->SetFontColor(fontColor);
		innerWindow->SetAlphaBlendFunc(zCRenderer::AlphaBlendFuncStringToType(par.alphaMode);

		if ( par.itemFlags & IT_TXT_CENTER )
			innerWindow->Setflags(512);
		else
			innerWindow->ClrFlags(512);

		inserted = 1;
		UpdateFX();

		// SetSize(GetDimX(par.dimX), GetDimY(par.dimY));
		UpdateSize();

		ClrPrintwin();
		DrawFront();
		return;
	}
	v10 = 0;
}

zCMenuItem* zCMenuItem::Create(zSTRING const& name)
{
	if (name.IsEmpty())
		return nullptr;

	zSTRING _name = name;
	_name.Upper();

	auto item = GetByName(_name);
	if (item) {
		++item->refCtr;
		return item;
	}

	zCMenuItem* result;
	if ( zCMenu::GetParser()->GetSymbol(_name) ) {
		zCMenu::GetParser()->CreateInstance(_name, &zCMenuItem::tmp->par);
		switch (  zCMenuItem::tmp->par.type ) {
		case MENU_ITEM_TEXT:
			result = new zCMenuItemText(_name);
			break;
		case MENU_ITEM_INPUT:
			result = new zCMenuItemInput(_name);
			break;
		case MENU_ITEM_SLIDER:
			result = new zCMenuItemSlider(_name);
			break;
		case MENU_ITEM_CHOICEBOX:
			result = new zCMenuItemChoice(_name);
			break;
		case MENU_ITEM_LISTBOX:
			result = new zCMenuItemList(_name);
			break;
		case MENU_ITEM_BUTTON:
			result = new zCMenuItemButton(_name);
			break;
		default:
			zFATAL("C: zmenu_item.cpp(zCMenuItem::Create): Illegal item type: "_s + tmp->par.type); // 97
			return nullptr;
		}
	} else {
		zFATAL("C: zmenu_item.cpp(zCMenuItem::Create): MenuItem not known: " + _name); // 100
	}

	result->InitView();
	result->InitMenuItem();

	return result;
}
