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
	virtual void LibIsLoaded();
	virtual void LibGet(int);
	virtual void LibGetFirst(int&);
	virtual void LibGetFirst();
	virtual void LibGetNext(int&);
	virtual void LibGetNext();
	virtual void LibAddOU(zCCSBlock	*);
	virtual void LibDelOU(int);
	virtual void LibNullOU(int);
	virtual void LibLoad(int);
	virtual void LibStore(int);
	virtual void LibValidateOU(zSTRING &);
	virtual void LibValidateOU(int);
	virtual void LibGetSvmModuleName(int);
	virtual void LibIsSvmModuleRunning(int);
	virtual void LibSvmModuleStart(int);
	virtual void LibSvmModuleStop(int);
	virtual void InsertPlayerInList(zCCSPlayer *);
	virtual void RemovePlayerFromList(zCCSPlayer *);
	virtual void LibCheckLoaded(int);
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
