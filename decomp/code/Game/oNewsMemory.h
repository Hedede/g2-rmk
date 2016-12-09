class oCNewsMemory {
public:
	virtual void Archive(zCArchiver& arc)
	{
		if (!arc.InSaveGame())
			return;

		int numEntries = iknow.GetNumInList();
		arc.WriteInt("NumOfEntries", numEntries);
		for (int i = 0; i < numEntries; ++i) {
			iknow.Get(i).Archive(arc);
		}
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		if (!arc.InSaveGame())
			return;

		ClearNews();

		int numEntries = arc.ReadInt("NumOfEntries");
		for (int i = 0; i < numEntries; ++i) {
			auto news = new oCNews();
			news->Unarchive(arc);
			iknow.Insert(news);
		}
	}

	int SearchNews(int newsId, oCNpc* offender, oCNpc* victim);
private:
	zCList<oCNews> iknow;
};

int oCNewsMemory::SearchNews(int newsId, oCNpc *offender, oCNpc *victim)
{
	int id;
	int gossip;
	oCNpc *witness;
	oCNpc *offen;
	oCNpc *vict;

	oCNews* news;
	zCList<oCNews>* next = iknow.next;

	for (int i = 1; next; ++i)
	{
		news = next->data;
		next = next->next;
		news->GetNewsData(&id, &gossip, &witness, &offen, &vict);
		if (id == newsId &&
		    (!offender || offender == offen) &&
		    (!victim   || victim == vict))
			return i;
	}

	return 0;
}

