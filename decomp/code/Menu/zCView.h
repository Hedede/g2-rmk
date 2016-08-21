enum zEViewFX {
	VIEW_FX_NONE,
	VIEW_FX_ZOOM,
	VIEW_FX_MAX
};

enum zTViewFX {
	VIEW_FX_NONE        = 0,
	VIEW_FX_ZOOM        = 1,
	VIEW_FX_FADE        = VIEW_FX_ZOOM << 1,
	VIEW_FX_BOUNCE      = VIEW_FX_FADE << 1,

	VIEW_FX_FORCE_DWORD = 0xffffffff
};

enum zTviewID {
	VIEW_SCREEN,
	VIEW_VIEWPORT,
	VIEW_ITEM
};

struct zCView : zCInputCallback {
	static int GetShowDebug()
	{
		return zCView::showDebug;
	}

	static void SetShowDebug(int val)
	{
		zCView::showDebug = val;
	}

	static void StdPrintwin(zSTRING const& text)
	{
		if ( zCView::stdoutput )
			zCView::stdoutput->Printwin(text);
	}

	static void PrintDebug(zSTRING const& text)
	{
		if ( zCView::showDebug )
			StdPrintwin(text);
	}

	void HandleEvent(int key) override;

	virtual int anx(int x);
	{
		float nx = float(x * 8192) / psizex;
		return nx + (nx > 0.0) ? (0.5) : (-0.5);
	}

	virtual int any(int y);
	{
		float ny = float(y * 8192) / psizey;
		return ny + (ny > 0.0) ? (0.5) : (-0.5);
	}

	virtual int nax(int x)
	{
		float ax = float(x * psizex) / 8192;
		return ax + (ax > 0.0) ? (0.5) : (-0.5);
	}

	virtual int nay(int y)
	{
		float ay = float(y * psizey) / 8192;
		return ay + (ay > 0.0) ? (0.5) : (-0.5);
	}

	virtual void ClipLine(int &,int &,int &,int &);
	virtual void Line(int,int,int,int,zCOLOR const &);
	virtual void GetViewport(int &,int &,int &,int &);
	virtual void GetCode(int,int);
	virtual ~zCView();
	virtual void Blit();
	virtual void DrawItems()
	{
		Blit();
		for (auto child : childs)
			child->DrawItems();
		zrenderer->SetViewport(0, 0, zrenderer->view_xdim, zrenderer->view_ydim);
	}

	void Render()
	{
		DrawItems();
	}

	void SetTransparency(int val)
	{
		alpha = val;
	}

	void SetAlphaBlendFunc(zTRnd_AlphaBlendFunc const& func)
	{
		alphafunc = func;
	}

	void SetColor(zCOLOR const& _color)
	{
		color = _color;
	}

	zCOLOR GetColor() const
	{
		return color;
	}

	void SetFontColor(zCOLOR const& color)
	{
		fontColor = color;
	}

	void SetFont(zCFont* f)
	{
		if ( !f )
			f = zfontman->GetFont(f);
		font = f;
	}

	zCFont* GetFont() const
	{
		return font;
	}

	void SetFlags(int f)
	{
		flags |= f;
	}

	void GetSize(int& vx, int& vy)
	{
		vx = vsizex;
		vy = vsizey;
	}

	void GetPos(int& vx, int& vy)
	{
		vx = vposx;
		vy = vposy;
	}

	void GetPixelPos(int& ppx, int& ppy)
	{
		ppx = pposx;
		ppy = pposy;
	}

	void ClrFlags(int f)
	{
		flags &= ~f;
	}

	bool HasFlag(int f)
	{
		return (flags & f) > 0;
	}

	zSTRING PatchMenuItemPrintwin()
	{
		return GetFirstText();
	}

	void ChangeMode() {}
	void SetNextMode() {}

private:
	//Noch eine _vtbl, weil zCView von zwei Klassen erbt.
	void* _zCInputCallBack_vtbl; 

	zBOOL m_bFillZ;
	zCView* next;

	zTviewID viewID;
	int flags;                  //int
	int intflags;               //int
	zBOOL ondesk;                 // Flag if in list

	zTRnd_AlphaBlendFunc alphafunc;
	zCOLOR color;                  //zCOLOR b, g, r, a
	int alpha;                  //int

	// Childs
	zList <zCView>  childs;

	zCView* owner;
	zCTexture* backTex;

	//Das Menüzeug nutzt oft virtuelle Koordinaten.
	int vposx;              //int
	int vposy;              //int
	int vsizex;             //int
	int vsizey;             //int

	//Aber auch "echt" Pixelpositionen
	int pposx;
	int pposy;
	int psizex;
	int psizey;

	//Font
	zCFont* font;
	zCOLOR fontColor;

	//Das Textfenster:
	int px1;                //int
	int py1;                //int
	int px2;                //int
	int py2;                //int

	int winx;               //int // Position in Text-Win
	int winy;               //int

	zCList <zCViewText>       textLines;

	zREAL scrollMaxTime;      //zREAL
	zREAL scrollTimer;        //zREAL


	zTViewFX              fxOpen;
	zTViewFX              fxClose;
	zREAL timeDialog;
	zREAL timeOpen;
	zREAL timeClose;
	zREAL speedOpen;
	zREAL speedClose;
	zBOOL isOpen;
	zBOOL isClosed;
	zBOOL continueOpen;
	zBOOL continueClose;
	zBOOL removeOnClose;
	zBOOL resizeOnOpen;
	int maxTextLength;
	zSTRING textMaxLength;
	zVEC2 posCurrent[2];
	zVEC2 posOpenClose[2];
};

zCView::~zCView()
{
	if ( owner )
		owner->RemoveItem(this);

	Release(backTex);
	for (auto& tl : textLines)
		Release(tl);
}
