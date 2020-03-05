const int MAX_ITEMS = 150;
const int MENU_EVENT_MAX = 9;

enum zTMenuItemSelAction {
	SEL_ACTION_UNDEF            = 0,
	SEL_ACTION_BACK             = 1,
	SEL_ACTION_STARTMENU        = 2,
	SEL_ACTION_STARTITEM        = 3,
	SEL_ACTION_CLOSE            = 4,
	SEL_ACTION_CONCOMMANDS      = 5,
	SEL_ACTION_PLAY_SOUND       = 6, //  -> console commands
	SEL_ACTION_EXECCOMMANDS     = 7  // -> dynamic build in func.
};

struct zCMenu : zCInputCallback {
	void HandleEvent(int) override;
	virtual void HandleEnterItem(zCMenuItem *);
	virtual void HandleLeaveItem(zCMenuItem *);
	virtual void HandleActivateItem(zCMenuItem *);
	virtual void Run();
	virtual void Enter();
	virtual void Leave();
	virtual void Render();
	virtual void RedrawItems()
	{
		for (auto& it : listItems) {
			it->DrawFront();
		}
	}
	virtual void HandleFrame(int);
	virtual void Activate();
	virtual ~zCMenu();
	virtual void ScreenInit();
	virtual void ScreenDone();
	virtual void HandleAction();
	virtual void HandleSelAction(zTMenuItemSelAction,zSTRING const &,zCMenuItem *);
	virtual void SetByScript(zSTRING const &);
	virtual void ExecCommand(zSTRING const &);

	static void CreateParser();
	static zCMenu* GetByName(zSTRING const& name);
	static zCMenu* Create(zSTRING const& name);

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

	enum {
		NONE, BACK, GO, FINISHED
	} exitState;

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

// ZenGin/_Carsten/zmenu.cpp
void zCMenu::CreateParser()
{
	zoptions->ChangeDir(DIR_SCRIPTS);
	if ( !menuParser )
		menuParser = new zCParser(100);
}

zCMenu* zCMenu::GetByName(zSTRING const& name)
{
	zCMenu menuCompareDummy;
	menuCompareDummy.SetName(name);

	int index = menuList.Search(menuCompareDummy);

	menuCompareDummy.SetName("TMPITEM");

	if (index < 0)
		return nullptr;
	return menuList[index];
}

zCMenu* zCMenu::Create(zSTRING const& name)
{
	zCMenu* menu = GetByName(name);

	if (!menu)
	{
		menu = new zCMenu; // 301 g1alp, 0xCC4
		menu->SetByScript(name);
	}

	return menu;
}

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

int DefineMenuScriptFunctions()
{
	auto parser = zCMenu::GetParser();
	parser.DefineExternal("Update_ChoiceBox", Update_ChoiceBox, 0, 3, 0);
	parser.DefineExternal("Apply_Options_Performance", Apply_Options_Performance, 0, 0);
	parser.DefineExternal("Apply_Options_Video", Apply_Options_Video, 0, 0);
	parser.DefineExternal("Apply_Options_Audio", Apply_Options_Audio, 0, 0);
	parser.DefineExternal("Apply_Options_Game", Apply_Options_Game, 0, 0);
	parser.DefineExternal("Apply_Options_Controls", Apply_Options_Controls, 0, 0);
	parser.DefineExternal("PlayVideo", PlayVideo, 2, 3, 0);
  return 1;
}
