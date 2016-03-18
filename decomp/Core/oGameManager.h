//######################################################
//
//	Game Manager: Kümmert sich darum
//  Game Sessions zu starten zu speichern usw.
//
//######################################################

class CGameManager {
public:
	virtual int HandleEvent(int key);
	virtual ~CGameManager();
	CGameManager();

	void InsertMenuWorld(zSTRING& backWorld, zSTRING& backDatFile)
	{
	}

	void RemoveMenuWorld()
	{
	}

	oCGame* GetGame() const
	{
		return this->gameSession;
	}
private:
	zTRnd_AlphaBlendFunc   oldAlphaBlendFunc;
	zTSystemContextHandle  sysContextHandle;

	oCGame*            gameSession;        //
	oCGame*            backLoop;           //
	zBOOL              exitGame;	    //
	zBOOL              exitSession;	    //
	zBOOL              gameIdle;           //
	zBOOL              lastWorldWasGame;   //
	oCSavegameManager* savegameManager;    //

	zCArray<zSTRING>	lastDatFileList;
	zCArray<zSTRING>	lastWorldList;

	zSTRING backWorldRunning;    //zSTRING
	zSTRING backDatFileRunning;  //zSTRING

	zCView*        vidScreen;              //
	zCView*        initScreen;             //

	zBOOL          introActive;            //
	zBOOL          dontStartGame;          //

	oCBinkPlayer*  videoPlayer;            //
	zBOOL          videoPlayInGame;        //

	zCMenu*        menu_chgkeys;           //
	zCMenu*        menu_chgkeys_ext;       //
	oCMenuSavegame*menu_save_savegame;     //
	oCMenuSavegame*menu_load_savegame;	//

	int playTime;	 //wird selten (?) aktualisiert. Mindestens aber beim Speichern und Laden.
};

bool chapBool;
zSTRING chapName;
zSTRING chapTime;
zSTRING chapTGA;
zSTRING chapWAV;
float chapTime;
