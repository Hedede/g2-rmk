const int MAX_ITEMS = 150;
const int MENU_EVENT_MAX = 9;

struct zCMenu : zCInputCallback {
	void HandleEvent(int) override;
	virtual void HandleEnterItem(zCMenuItem *);
	virtual void HandleLeaveItem(zCMenuItem *);
	virtual void HandleActivateItem(zCMenuItem *);
	virtual void Run();
	virtual void Enter();
	virtual void Leave();
	virtual void Render();
	virtual void RedrawItems();
	virtual void HandleFrame(int);
	virtual void Activate();
	virtual ~zCMenu();
	virtual void ScreenInit();
	virtual void ScreenDone();
	virtual void HandleAction();
	virtual void HandleSelAction(zTMenuItemSelAction,zSTRING const &,zCMenuItem *);
	virtual void SetByScript(zSTRING const &);
	virtual void ExecCommand(zSTRING const &);

	static void CreateParser()
	{
		zoptions->ChangeDir(8);
		if ( !menuParser )
			menuParser = new zCParser(100);
	}
private:
	static zCParser             menuParser;
	static zCArraySort<zCMenu*> menuList;

	static zCTimer* timer;
	static int inGameMenu;
	static (void(*)()) FrameCallback;

	zSTRING backPic;
	zSTRING backWorld;
	int posx;
	int posy;
	int dimx;
	int dimy;
	int alpha;
	zSTRING musicThemeName;
	int eventTimerUpdateMSec;

	zSTRING itemID[MAX_ITEMS];

	int     flags;
	int     defaultOutGame;
	int     defaultInGame;

	zCViewWindow*  pWindow;
	zCViewWindow*  pInnerWindow;

	enum { NONE, BACK, GO, FINISHED } exitState;

	zSTRING name;

	zCMusicTheme* musicTheme;
	int mainSel;
	zBOOL screenInitDone;

	int use_dimx;
	int use_dimy;
	int use_posx;
	int use_posy;

	zCView*  pViewInfo;
	zCVob*   cursorVob;

	zBOOL eventOccured[MENU_EVENT_MAX];
	zBOOL cursorEn;
	zBOOL noneSelectable;
	zBOOL registeredCPP;

	int updateTimer;            //int
	float fxTimer;                //float

	enum zTMenuItemSelAction {
		SEL_ACTION_UNDEF            = 0,
		SEL_ACTION_BACK             = 1,
		SEL_ACTION_STARTMENU        = 2,
		SEL_ACTION_STARTITEM        = 3,
		SEL_ACTION_CLOSE            = 4,
		SEL_ACTION_CONCOMMANDS      = 5,
		SEL_ACTION_PLAY_SOUND       = 6,
		SEL_ACTION_EXECCOMMANDS     = 7 
	};
	zTMenuItemSelAction forceSelAction;

	zSTRING forceSelAction_S;
	zCMenuItem* forceSelActionItem;     //
	zBOOL forcedSelAction;        //zBOOL

	zCArray <zCMenuItem *> listItems;
};

//#################################################################
//
//  zCView: Basis für viele Anzeigeelemente
//
//#################################################################

//------------------------------------------------
//  Textzeile von zCView:
//------------------------------------------------
class zCViewText {
	void* _vtbl;
	
	int posx;
	int posy;
	
	zSTRING text;	//zSTRING //Die Entscheidende Eigenschaft.
	zCFont* font;		//
	zREAL timer;		//zREAL //übrige Zeit für PrintScreen anzeigen die nur eine bestimmte Zeit dauern?
	zBOOL inPrintWin; //zBOOL //vermutlich für anzeigen mit "Print", die nach oben weggeschoben werden.

	zCOLOR color;
	zBOOL timed;
	zBOOL colored;	//klingt interessant. Vielleicht kann man hiermit was anfangen.
};

//siehe Konstanten für Adresse vom globalen zCView screen.

/* zCViews kapseln zweidimensionale Objekte auf dem Bildschirm.
 * Dazu zählen zum Beispiel Texte oder Menüelemente.
 * 
 * Bei Menüelemente verhält es sich so:
 * -Ein Menüelement wird generiert, wenn es das erste mal gebraucht wird. Es lebt dann bis zum beenden von Gothic.
 * -Ein Menüelement speichert den Text, der zu ihm gehört in zCMenuItem.listLines.
 * -Sobald ein Menüelement etwas anzeigen muss, erzeugt es ein "InnerWindow" um speichert sich eine Referenz
 *  auf dieses InnerWindow in zCMenuItem.pInnerWindow.
 * -Das InnerWindow ist ein zCView und bekommt eine Kopie des anzuzeigenden Textes vom Menüelement.
 * -Das InnerWindow speichert sich alle Textzeilen (meistens eine) in zCView.textLines. Das "nullte" Element von solchen Liste bleibt stets leer.
 * -Der Text wird vom InnerWindow dann in jedem Frame gezeichnet. Sobald das Menüitem entscheidet, dass es nichts mehr anzeigen muss wird das InnerWindow zerstört.
 */

const int MAX_USERSTRINGS = 10;
const int MAX_SEL_ACTIONS =  5;
const int MAX_USERVARS    =  4;
const int MAX_EVENTS      = 10;

class zCMenuItem : public zCView {
public:
	virtual ~zCMenuItem();
	virtual void DrawItems();
	virtual void InitMenuItem();
	virtual void ToggleValue(int,int);
	virtual void Run();
	virtual void GetText(int);
	virtual void SetText(zCArray<zSTRING>,int);
	virtual void SetText(zSTRING const &,int,int);
	virtual void SetMinValue(float);
	virtual void SetMaxValue(float);
	virtual void Enter();
	virtual void Leave();
	virtual void Draw();
	virtual void DrawFront();
	virtual void Show();
	virtual void Hide();
	virtual void HasBeenCanceled();
	virtual void Open();
	virtual void Input(int,zSTRING &);
	virtual void InsertInWin(zCView *);
	virtual void RemoveFromWin();
	virtual void UpdateContent();
	virtual void UpdateFX();
    //Parser Start

    zSTRING parFontName;
    zSTRING parText      [MAX_USERSTRINGS];
    zSTRING parBackPic;
    zSTRING parAlphaMode;
    int parAlpha;
    int parType;
    int parOnSelAction         [MAX_SEL_ACTIONS];
    zSTRING parOnSelAction_S   [MAX_SEL_ACTIONS];
    zSTRING parOnChgSetOption;
    zSTRING parOnChgSetOptionSection;
    int parOnEventAction  [MAX_EVENTS];
    int parPosX;
    int parPosY;
    int parDimX;
    int parDimY;
    int parSizeStartScale;
    int parItemFlags;
    int parOpenDelayTime;
    int parOpenDuration;
    int parUserFloat          [MAX_USERVARS];
    zSTRING parUserString     [MAX_USERVARS];
    int parFrameSizeX;
    int parFrameSizeY;
    zSTRING parHideIfOptionSectionSet;
    zSTRING parHideIfOptionSet;
    int parHideOnValue;
private:
    //Parser End
    int iRefCtr;
    zCView*                   pInnerWindow;           //
    zCFont*                   pFont;                  //
    zCFont*                   pFontHi;                //
    zCFont*                   pFontSel;               //
    zCFont*                   pFontDis;               //
    zBOOL                     bViewInitialized;       //
    zBOOL                     bLeaveItem;             //
    zBOOL                     bVisible;               //
    zBOOL                     bDontRender;            //
    zCArray<zSTRING> listLines;

    zSTRING id;                     //zSTRING            
    int inserted;                  //zBOOL            
    int changed;                   //zBOOL            
    int active;                    //zBOOL            
    int open;                      //zBOOL            
    int close;                     //zBOOL            
    int opened;                    //zBOOL            
    int closed;                    //zBOOL            
    int disabled;                  //zBOOL            
    zCView*             orgWin;                    //
    float fxTimer;                   //float            
    float openDelayTimer;            //float            
    
    float activeTimer;               //float
	zBOOL registeredCPP;             //zBOOL
	zBOOL firstTimeInserted;         //zBOOL
};

//#################################################################
//
//  Auch ein zCView, aber ein anderes:
//
//#################################################################

class oCViewStatusBar : zCView
{    
    zREAL  minLow, maxHigh;         //
    zREAL  low, high;               //
    zREAL  previewValue;            //
    zREAL  currentValue;            //
    
    float     scale;                   //
    zCView*   range_bar;               //
    zCView*   value_bar;               //
    zSTRING texView;              //zSTRING 
    zSTRING texRange;             //zSTRING 
    zSTRING texValue;             //zSTRING 
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

class zCMenuItemText : zCMenuItem {
public:
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

int zCMenu::ExecCommand(zSTRING& const cmdstr)
{
	zSTRING str1 = cmdstr.PickWord(1, " ", " ");
	zSTRING str2 = cmdstr.PickWord(2, " ", " ");
}

void zCMenu::Render()
{
	int numItems = this->listItems.numInArray;
	for ( int i = 0; i < numItems; ++i ) {
		// I have no idea what's that loop for
		// probably commented out code
	}

	for ( int i = 0; i < numItems; ++i ) {
	{
		zCMenuItem* menuItem = listItems[i];
		if ( menuItem->parOpenDuration != 0xBF800000 ) // -1.0
			menuItem->Open();
	}
	if ( !zCMenu::inGameMenu )
		Sleep(50);

	if ( zCMenu::FrameCallback )
	{
		zCMenu::FrameCallback(menuItem);
		zCTimer::ResetTimer(zCMenu::timer);
	}
	else
	{
		zCOLOR black;
		zrenderer->Vid_Clear(black, 3);
		zrenderer->BeginFrame();
		screen->Render;
		zrenderer->EndFrame();
		zrenderer->Vid_Blit(1, 0, 0);
		zsound->DoSoundUpdate();
		timer->ResetTimer();
	}
}
