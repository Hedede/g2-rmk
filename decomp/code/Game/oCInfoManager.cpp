class oCInfoManager : public zCObject {
	Z_OBJECT(oCInfoManager);
public:
	virtual void Archive(zCArchiver& archiver)
	{
		if (!archiver.InSaveGame())
			return;

		int num = infoList.GetNumInList();
		archiver.WriteInt("NumOfEntries", num);

		for (auto j = infoList.next; j; j = j->next) {
			oCInfo* info = j->data;
			if ( info )
				info->Archive(archiver);
		}
	}

	virtual void  Unarchive(zCArchiver& archiver)
	{
		if (!archiver.InSaveGame(archiver))
			return;

		int numEntries = 0;
		archiver.ReadInt("NumOfEntries", numEntries);

		infoList.DeleteList();

		for (int i = 0; i < numEntries; ++i) {
			oCInfo* info = new oCInfo();
			info->Unarchive(archiver);

			infoList.Insert(info);
		};
	}

	virtual void RestoreParserInstances()
	{
		for (auto j = infoList->next; j; j = j->next)
			if (j->Get())
				j->Get()->RestoreParserInstance();
	}

private:
	//Liste aller oCInfos. Geordnet nach...? M¸sste man mal nachschauen.
	zCListSort< oCInfo > infoList;    

	//Parser.
	zCParser* parser;
};

class oCInformationManager 
{
	typedef enum zEInformationManagerMode
	{
		INFO_MGR_MODE_IMPORTANT ,
		INFO_MGR_MODE_INFO      ,
		INFO_MGR_MODE_CHOICE    ,
		INFO_MGR_MODE_TRADE
	} zTInfoMgrMode;

	zSTRING LastMethod           ;  //zSTRING                         
	zCViewDialogChoice* DlgStatus;  //   //das sind Views. Das heiﬂt vermutlich nur Anzeige
	oCViewDialogTrade*  DlgTrade ;  //   //das sind Views. Das heiﬂt vermutlich nur Anzeige
	zCViewDialogChoice* DlgChoice;  //   //das sind Views. Das heiﬂt vermutlich nur Anzeige
	oCNpc*       Npc                     ;  //                
	oCNpc*       Player                  ;  //                
	oCInfo*      Info                    ;  //                   
	zBOOL        IsDone                  ;  //        //== InfoManager_HasFinished         
	zBOOL        IsWaitingForEnd         ;  //                 
	zBOOL        IsWaitingForScript      ;  //                 
	zBOOL        IsWaitingForOpen        ;  //                 
	zBOOL        IsWaitingForClose       ;  //                 
	zBOOL        IsWaitingForSelection   ;  //                 
	zBOOL        MustOpen                ;  //                 
	int          IndexBye                ;  //                      
	int          ImportantCurrent        ;  //                      
	int          ImportantMax            ;  //                      
	zTInfoMgrMode Mode                    ;  //           
};
