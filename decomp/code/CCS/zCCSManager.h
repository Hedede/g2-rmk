class zCCSManager : public zCObject {
	Z_OBJECT(zCCSManager);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCSManager();
	virtual void CreateMessage(int);
	virtual void CreateOuMessage();
	virtual void CreateCutscenePlayer(zCWorld *);
	virtual void CreateProperties();
	virtual void RemoveCutscenePlayer(zCCSPlayer *);

	virtual bool LibIsLoaded()
	{
		return csLib->IsLoaded();
	}
	virtual void LibGet(int);
	virtual void LibGetFirst(int&);
	virtual void LibGetFirst();
	virtual void LibGetNext(int&);
	virtual void LibGetNext();
	virtual void LibAddOU(zCCSBlock* block);
	{
		LibCheckLoaded(3);
		return csLib->Add(block);
	}
	virtual void LibDelOU(int);
	virtual void LibNullOU(int id)
	{
		LibCheckLoaded();
		csLib->RemoveFromLib(id, 0);
	}
	virtual void LibLoad(int);
	virtual void LibStore(int);
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

	virtual void LibGetSvmModuleName(int);
	virtual void LibIsSvmModuleRunning(int);
	virtual void LibSvmModuleStart(int);
	virtual void LibSvmModuleStop(int);

	virtual void InsertPlayerInList(zCCSPlayer *);
	virtual void RemovePlayerFromList(zCCSPlayer *);

	virtual void LibCheckLoaded(int flags)
	{
		if ( !csLib->IsLoaded() )
			LibLoad(flags);
	}

	virtual void PoolFindItem(zSTRING &);
	virtual void PoolInsertItem(zCCSPoolItem *);
};

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
