class oCInfoManager : public zCObject {
	Z_OBJECT(oCInfoManager);
public:
	void Archive(zCArchiver& archiver) override
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

	void Unarchive(zCArchiver& archiver) override
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
	//Liste aller oCInfos. Geordnet nach...? Müsste man mal nachschauen.
	zCListSort< oCInfo > infoList;    

	//Parser.
	zCParser* parser;
};

class oCInformationManager {
	oCInformationManager& GetInformationManager()
	{
		static oCInformationManager informationManager;
		return informationManager;
	}

	bool WaitingForEnd() const { return IsWaitingForEnd; }
	bool HasFinished() const { return IsDone; }

	typedef enum zEInformationManagerMode
	{
		INFO_MGR_MODE_IMPORTANT ,
		INFO_MGR_MODE_INFO      ,
		INFO_MGR_MODE_CHOICE    ,
		INFO_MGR_MODE_TRADE
	} zTInfoMgrMode;

	zSTRING LastMethod;

	//das sind Views. Das heißt vermutlich nur Anzeige
	zCViewDialogChoice* DlgStatus;
	oCViewDialogTrade*  DlgTrade;
	zCViewDialogChoice* DlgChoice;

	oCNpc*  Npc;
	oCNpc*  Player;
	oCInfo* Info;

	zBOOL         IsDone;
	zBOOL         IsWaitingForEnd;
	zBOOL         IsWaitingForScript;
	zBOOL         IsWaitingForOpen;
	zBOOL         IsWaitingForClose;
	zBOOL         IsWaitingForSelection;
	zBOOL         MustOpen;
	int           IndexBye;
	int           ImportantCurrent;
	int           ImportantMax;
	zTInfoMgrMode Mode;
};
