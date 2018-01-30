class zCCSManager : public zCObject {
	Z_OBJECT(zCCSManager);
public:
	void Archive(zCArchiver& arc)   override;
	void Unarchive(zCArchiver& arc) override;

	zCCSManager()
	{
		zCCSCutsceneContext::playDisabled = zoptions->ReadBool(zOPT_SEC_INTERNAL, "cutscenesDisabled", 0);
	}
	virtual ~zCCSManager()
	{
		for (auto item : poolItems)
			Release(item);
		for (auto player : playerList)
			Release(player);
		Release( csLib );
		Delete(unk0);
	}
	virtual zCEventMessage* CreateMessage(int) { return nullptr; }
	virtual zCEventMessage* CreateOuMessage() { return nullptr; }
	virtual zCCSPlayer* CreateCutscenePlayer(zCWorld* world)
	{
		auto player = new zCCSPlayer;
		player->SetOwnerWorld( world );
		InsertPlayerInList( player );
		return player;
	}
	virtual zCCSProps* CreateProperties() { return new zCCSProps; } // 537
	virtual void RemoveCutscenePlayer(zCCSPlayer* player)
	{
		RemovePlayerFromList( player );
	}

	virtual bool LibIsLoaded()
	{
		return csLib->IsLoaded();
	}
	virtual zCCSBlock* LibGet(int n)
	{
		LibCheckLoaded(3);
		__lastGet = n;
		if ( n < csLib->blocks.num )
			return csLib->GetOU(n);
		return nullptr;
	}

	virtual void LibGetFirst(int& n)
	{
		// it was probably implemented the otehr way around
		auto result = LibGetFirst();
		if (result)
			n = __lastGet;
		return result;
	}

	virtual void LibGetFirst();
	{
		LibCheckLoaded(3);

		for (__lastGet = 0; __lastGet < csLib->blocks.num; ++__lastGet) {
			if ( csLib->GetOU( __lastGet ) )
				break;
		}

		if ( __lastGet < csLib->blocks.num )
			return csLib->GetOU( __lastGet );
		return nullptr;
	}

	virtual void LibGetNext(int& n)
	{
		auto result = LibGetNext();
		if (result)
			n = __lastGet;
		return result;
	}
	virtual zCCSBlock* LibGetNext();
	{
		if (!LibIsLoaded())
			return nullptr;

		csLib = this->csLib;
		while( ++__lastGet < csLib->blocks.num ) {
			if ( csLib->GetOU( __lastGet ) )
				break;
		}

		if ( __lastGet < csLib.blocks.num )
			return csLib->GetOU( __lastGet );
		return nullptr;
	}

	virtual void LibAddOU(zCCSBlock* block);
	{
		LibCheckLoaded(3);
		return csLib->Add(block);
	}
	virtual void LibDelOU(int n)
	{
		LibCheckLoaded(3);
		csLib->RemoveFromLib(n, 1);
	}
	virtual void LibNullOU(int id)
	{
		LibCheckLoaded();
		csLib->RemoveFromLib(id, 0);
	}
	virtual void LibLoad(int flags);
	virtual void LibStore(int flags);
	virtual int LibValidateOU(zSTRING& token)
	{
		LibCheckLoaded(3);
		return csLib->ValidateToken(token);
	}

	virtual bool LibValidateOU(int ref)
	{
		LibCheckLoaded(3);
		return csLib->blocks.num < ref && csLib->GetOU(ref);
	}

	virtual zSTRING LibGetSvmModuleName(int id)
	{
		if ( auto lbl = LibGet(id) ) {
			if (lbl->GetRoleName().PickWord(1, "_", "_") == "SVM") {
				return lbl->GetRoleName()->PickWord(3, "_", "_");
		}
		return "";
	}
	virtual int LibIsSvmModuleRunning(int id)
	{
		auto svmName = LibGetSvmModuleName(id);
		if (svmName)
			return svmLib.Search( svmName ) >= 0;
		return 0;
	}
	virtual int LibSvmModuleStart(int id)
	{
		auto svmName = LibGetSvmModuleName(id);
		if (svmName.IsEmpty())
			return 1;
		if (svmLib.Search(svmLib) >= 0)
			return 0;
		svmLib.InsertEnd( svmName );
		return 1;
	}
	virtual int LibSvmModuleStop(int id)
	{
		auto svmName = LibGetSvmModuleName(id);
		if (svmName.IsEmpty())
			return 1;
		auto idx = svmLib.Search(svmLib);
		if (idx < 0)
			return 0;
		svmLib.RemoveIndex(idx);
		return 1;
	}

	virtual void InsertPlayerInList(zCCSPlayer *);
	virtual void RemovePlayerFromList(zCCSPlayer *);

	virtual void LibCheckLoaded(int flags)
	{
		if ( !csLib->IsLoaded() )
			LibLoad(flags);
	}

	//----------------------------------------
	void PoolResetByDay()
	{
		for (auto csitem : poolItems)
			if (csitem->GetRunBehaviour() == 3)
				csitem->numRuns = 0;
	}

	void PoolResetByHour()
	{
		for (auto csitem : poolItems)
			if (csitem->GetRunBehaviour() == 2)
				csitem->numRuns = 0;
	}

	void PoolResetAll()
	{
		for (auto csitem : poolItems)
			csitem->numRuns = 0;
	}

	virtual zCCSPoolItem* PoolFindItem(zSTRING& csname);
	{
		if ( csname.Search(0, ".", 1u) <= 0 )
			csname += ".CS";
		csname.Upper();

		return poolItems.Search(csname);
	}
	virtual void PoolInsertItem(zCCSPoolItem* item) { poolItems.Insert(item); }

	int PoolAllowedToStart(zSTRING& csname)
	{
		if ( auto csitem = PoolFindItem(csname) )
			return csitem->IsAllowedToPlay() != 0;
		return -1;
	}

	int PoolNumPlayed(zSTRING& csname)
	{
		if ( auto cutscene = PoolFindItem(csname) )
			return cutscene->numPlayed;
		return 0;
	}

	void PoolSetFlags(zSTRING& csname, int flags)
	{
		if ( auto csitem = PoolFindItem(csname) )
			csitem->SetFlags(flags);
	}

	void PoolClrFlags(zSTRING& csname, int flags)
	{
		if ( auto csitem = PoolFindItem(csname) )
			csitem->ClrFlags(flags);
	}

	zBOOL PoolHasFlags(zSTRING& csname, int flags)
	{
		if ( auto csitem = PoolFindItem(csname) )
			return csitem->HasFlags( flags )
		return false;
	}

	void PoolCountStop(zSTRING& csname)
	{
		if ( auto csitem = PoolFindItem(csname) )
			csitem->TestedDeactivation();
	}

	void PoolCountStart(zSTRING& csname)
	{
		if ( auto csitem = PoolFindItem(csname) ) {
			++csitem->numPlayed;
			++csitem->numRuns;
		}
	}

	zBOOL IsDeactivated(zSTRING& csname)
	{
		if ( auto csitem = PoolFindItem(csname) )
			return csitem->deactivated;
		return false;
	}

	//----------------------------------------
	zCCSCutsceneContext* CreateNewCutscene(zSTRING& csname);
	zCCSCutsceneContext* CreateNewOutputUnit(int ref)
	{
		if ( LibValidateOU(ref) ) {
			auto lbl = LibGet( ref );
			if ( lbl )
				return new zCCSCutsceneContext(lbl,ref,TRUE); // 154
		}
		return nullptr;
	}

	void InitiateOUwithContext(zCCSCutsceneContext* context)
	{
		if ( auto ref = context->reference; ref >= 0 ) {
			context->UseThisSubBlock(LibGet(ref));
		}
	}

	zCCSCutsceneContext* InitiateCSwithContext(zCCSCutsceneContext *context);

	void RemovePlayerFromList(zCCSPlayer *player)
	{
		player->SetOwnerManager(0);
		// doesn't seem to check whether player is actually in list
		// — there's even a jump from inline function straight to Release(),
		// when player isn't found in list
		playerList.Remove( player );

		player->Release();
	}

	void InsertPlayerInList(zCCSPlayer *player)
	{
		player->SetOwnerManager(this);
		player.InsertEnd( player );
		player->AddRef();
	}

	//----------------------------------------
	int GetNumOfShortCom() const
	{
		return shortComs.GetNum();
	}

	zSTRING GetShortCom(int z)
	{
		if (z < 0 || z >= GetNumOfShortCom()) {
			zWARNING("A: (zCCSManager::GetShortCom)"); // 316
			return "NO";
		}
		return shortComs[z];
	}

	void AddCommand(zSTRING& comm)
	{
		shortComs.Insert(comm);
	}


private:
	zCArray<zSTRING> svmLib;
	int unk[4];
	zCArray<zCCSPoolItem*> poolItems;
	zCArray<zSTRING> shortComs;
	zCArray<zCCSPlayer*> playerList;
	int unk4;
	zCCSLib* csLib = new zCCSLib;
	int __lastGet;
};


//--------------------------------------------------------------------------------
//_andre/zCCsManager.cpp
void zCCSManager::Archive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		int poolCount = poolItems.GetNumInList();

		arc.WriteInt("poolCount", poolCount);

		for (int i = 0; i < poolCount; ++i)
			arc.WriteObject("poolItem" + i, poolItems[i]);
	}
}

void zCCSManager::Unarchive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		for (int i = 0; i < poolItems.GetNumInList(); ++i) {
			poolItems[i]->Release();
			poolItems[i] = 0;
		}

		int poolCount = 0;
		arc->ReadInt("poolCount", poolCount);

		for (int i = 0; i < poolCount; ++i) {
			auto obj = arc.ReadObject("poolItem" + i, 0);

			auto poolItem = dynamic_cast<zCCSPoolItem>(obj);
			poolItems.Insert(poolItem);
		}
	}
}

//--------------------------------------------------------------------------------
void zCCSManager::LibStore(int flags)
{
	zSTRING name = "";
	if ( zgameoptions )
		name = zgameoptions->ReadString(zgameoptions, zOPT_SEC_FILES, "OutputUnits", 0);
	if ( !name )
		name = "OU";
	if ( LibIsLoaded() ) {
		auto oudir = zoptions->GetDirString(DIR_OUTPUTUNITS);
		if ( flags & 1 )
			zINFO(4, "B: CSL: Writing ascii version of Cutscene-Library (OutputUnits) to " + name + ".csl ..."); // 533

			auto fileName = oudir + name + ".csl";
			auto arc = zarcFactory.CreateArchiverWrite(fileName, 1, 0, 0);

			if (arc) {
				arc->WriteObject( csLib );
				arc->Close();
				arc->Release();
			}
		}
		if ( flags & 2 ) {
			zINFO(4,  "B: CSL: Writing binary version of Cutscene-Library (OutputUnits) to " + name + ".bin ..."); //546

			auto fileName = oudir + name + ".bin";
			auto arc = zarcFactory.CreateArchiverWrite( fileName, 3, 0, 0 );

			if (arc) {
				arc->WriteObject( csLib );
				arc->Close();
				arc->Release();
			}
		}
	}
}

void zCCSManager::LibLoad(int flags)
{
	if ( csLib ) {
		DeleteLib(csLib);

		int loaded = 0;
		int binTried = 0;

		zSTRING name = "";
		if ( zgameoptions )
			name = zgameoptions->ReadString(zgameoptions, zOPT_SEC_FILES, "OutputUnits", 0);

		if ( !name )
			name = "OU";

		// was duplicated in each branch
		auto oudir = zoptions->GetDirString(DIR_OUTPUTUNITS);

		if ( flags & 2 ) {
			binTried = 1;

			auto fileName = oudir + name + ".bin";
			auto file = zfactory->CreateZFile( fileName );

			if ( file->Exists() ) {
				zINFO(5, "B: SPC: Loading binary output-library " + file->GetFile() + "..."); // 387
				file->Open(0);
				arc->CreateArchiverRead(file, 0);
				if (arc) {
					arc->ReadObject( csLib );
					arc->Close();
					arc->Release();
				}

				file->Close();
				loaded = 1;
			}

			delete file;
		}

		if ( loaded )
			return;

		if ( flags & 1 ) {
			auto fileName = oudir + name + ".csl";

			auto file = zfactory->CreateZFile( fileName );

			if ( file->Exists() ) {
				zINFO(5, "B: SPC: Loading ascii output-library " + file->GetFile() + "..."); // 414
				file->Open(0);
				arc->CreateArchiverRead(file, 0);
				if (arc) {
					arc->ReadObject( csLib );
					arc->Close();
					arc->Release();
				}

				file->Close();
				loaded = 1;
			}

			// not sure about condition, because stack is messed up in decompiler
			if ( binTried ) {
				zCCSManager->LibStore( 2 );
			}

			delete file;

			if ( !loaded ) {
				csLib->loaded = 1;
				zFAULT( "N: CS: File for output-units not found: " + name ".bin or " + name + ".csl");
			}
		}
	}
}
//--------------------------------------------------------------------------------
zCCSCutsceneContext* zCCSManager::InitiateCSwithContext(zCCSCutsceneContext *context);
{
	auto csname = context->GetCSName();
	auto lbl = PoolFindItem(csname);

	if ( !lbl ) {
		lbl = new zCCSPoolItem( context ); // 103

		auto* props = newContext->properties;
		auto rb  = props->runBehviour;
		auto rbv = props->runBehaviourValue;
		lbl->SetRunBehaviour(lbl, rb, rbv);

		PoolInsertItem(lbl);
		context->SetPoolItem(lbl);
	} else if ( lbl->IsAllowedToPlay() && !lbl->deactivated ) {
		context->allowedToPlay = lbl->IsAllowedToPlay();
		context->SetPoolItem(lbl);
	}

	return context;
}

zCCSCutsceneContext* zCCSManager::CreateNewCutscene(zSTRING& csname)
{
	auto lbl = PoolFindItem(csname);
	if ( lbl ) {
		if (!lbl->IsAllowedToPlay())
			return nullptr;
		if (lbl->IsDeactivated())
			return nullptr;
	}

	auto newContext = new zCCSCutsceneContext( csname ); // 132
	if ( lbl ) {
		newContext->allowedToPlay = lbl->IsAllowedToPlay();
	} else {
		lbl = new zCCSPoolItem(newContext); // 136
		auto* props = newContext->properties;
		auto rb  = props->runBehviour;
		auto rbv = props->runBehaviourValue;
		lbl->SetRunBehaviour(rb, rbv);
		PoolInsertItem(lbl);
	}
	newContext->SetPoolItem(lbl);
	return newContext;
}
