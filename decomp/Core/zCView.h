enum zEViewFX
{
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

enum zTviewID { VIEW_SCREEN,VIEW_VIEWPORT,VIEW_ITEM };
class zCView : public zCInputCallback {
public:
	virtual void HandleEvent(int);
	virtual void anx(int);
	virtual void any(int);
	virtual void nax(int);
	virtual void nay(int);
	virtual void ClipLine(int &,int &,int &,int &);
	virtual void Line(int,int,int,int,zCOLOR const &);
	virtual void GetViewport(int &,int &,int &,int &);
	virtual void GetCode(int,int);
	virtual ~zCView();
	virtual void Blit();
	virtual void DrawItems();
private:
	void* _zCInputCallBack_vtbl; //Noch eine _vtbl, weil zCView von zwei Klassen erbt.


	zBOOL m_bFillZ;               //zBOOL
	zCView* next;

	zTviewID viewID;
	int flags;                  //int
	int intflags;               //int
	zBOOL ondesk;                 // Flag if in list

	zTRnd_AlphaBlendFunc alphafunc;
	zCOLOR color;                  //zCOLOR b, g, r, a
	int alpha;                  //int

	// Childs
	zList <zCView>            childs

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

int zCView::GetShowDebug()
{
  return zCView::showDebug;
}
