enum zMenuItemFlags {
	IT_CHROMAKEYED    = 1,
	IT_TRANSPARENT    = 2,
	IT_SELECTABLE     = 4,
	IT_MOVEABLE       = 8,
	IT_TXT_CENTER     = 16,
	IT_DISABLED       = 32,
	IT_FADE           = 64,
	IT_EFFECTS_NEXT   = 128,
	IT_ONLY_OUT_GAME  = 256,
	IT_ONLY_IN_GAME   = 512,
	IT_PERF_OPTION    = 1024,
	IT_MULTILINE      = 2048,
	IT_NEEDS_APPLY    = 4096, // die mit dem Menuepunkt verknuepfte Option wird NUR ueber ein APPLY aktiv
	IT_NEEDS_RESTART  = 8192, // die mit dem Menuepunkt verknuepfte Option wird NUR ueber ein RESTART aktiv
	IT_EXTENDED_MENU  = 16384,
}

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

	~zCMenuItem() override;
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
	virtual void SetText(zCArray<zSTRING>,int);
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
	virtual void UpdateFX();

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
		if ((flags & IT_TRANSPARENT) && zCMenu::inGameMenu)
			return 1;

		if ((flags & IT_CHROMAKEYED) && zCMenu::inGameMenu)
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
		if (!isVisible)
			return 0;

		auto flags = par.itemFlags;
		if (flags & IT_ONLY_IN_GAME && !zCMenu::inGameMenu)
			return 0;
		if (flags & IT_ONLY_OUT_GAME && zCMenu::inGameMenu)
			return 0;
		if (flags & IT_DISABLED)
			return false;
		if (dontRender)
			return false;
		// compiler bullshit?
		if ( !(flags & IT_ONLY_IN_GAME) || zCMenu::inGameMenu ) {
			if ( flags & IT_ONLY_OUT_GAME && zCMenu::inGameMenu )
				return false;
			return par.itemFlags & IT_SELECTABLE;
		}
		return false;
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

//#################################################################
//
//  Vermutlich ziemlich nutzlos, ich dachte zunächst die Klasse
//  wäre wichtiger. Alles entscheidende spielt sich
//  zumindest was das Charaktermenü angeht in den gewöhnlichen
//  zCMenuItems ab. zCMenuItemText wird (nicht ausschließlich)
//  für Auswahlboxen benutzt (In den Einstellungen: [ja|nein]-Box)
//
//#################################################################

struct zCMenuItemText : zCMenuItem {
	zSTRING GetText(int index) const
	{
		if ( index < listLines.NumInList() )
			return listLines[index];
		return zSTRING();
	}

	enum zTMenuItemTextMode {
		MODE_SIMPLE,
		MODE_ENUM,
		MODE_MULTILINE
	};
private:
	int mode; //siehe enum

	zSTRING fullText;		//zSTRING
	//Relevant für Options-Menüitems wo man zum Beispiel
	//zwischen "aus" und "an" wählen kann.
	int	numOptions;

	int topLine;          //int		 
	int viewLines;        //int		 
	int numLines;         //int		 
	zBOOL unformated;	    //zBOOL	
};
