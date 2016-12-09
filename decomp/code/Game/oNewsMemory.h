class oCNewsMemory {
public:
	oCNewsMemory() = default;

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
	oCNews* GetNewsByNumber(int newsNumber);
	oCNews* GetNews(int news_id, oCNpc *offender, oCNpc *victim);

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

oCNews* oCNewsMemory::GetNewsByNumber(int newsNumber)
{
	int n = 1;
	for (auto i = iknow.next; i; i = i->next) {
		if (n == newsNumber)
			return i->Get();
		++n;
	}
	return nullptr;
}

oCNews* oCNewsMemory::GetNews(int nid, oCNpc *offender, oCNpc *victim)
{
	for (auto i = iknow.next; i; i = i->next) {
		auto n = i->Get();
		if (n->id == nid && n->offender == offender && n->victim == victim)
			return n;
	}
#if 0 // might have looked like:
	return GetNewsByNumber(SearchNews(nid, offender, victim));
#endif
	return nullptr;
}

void oCNewsMemory::Remove(oCNews *news)
{
	if ( news )
		iknow.Remove( news );
}

void oCNewsMemory::CheckSpreadNews()
{
	int num = iknow.GetNumInList();
	float time = ztimer.frameTimeFloat;
	for (int n = 0; n < num; ++n) {
		auto news = iknow.Get(n);
		if ( news->spreadTime > 0.0 ) {
			news->spreadTime -= time;
			if (news->spreadTime <= 0.0) {
				ogame->GetNewsManager()->SpreadNews(news);
			}
		}
	}
}

void oCNewsMemory::DeleteNewsByNumber(int newsID)
{
	// I've didn't de-inline this code just to show how idiotic it is
	auto n = iknow.next;
	if (n)
		return 0;
	int i = 1;
	while ( i != newsID ) {
		n = n->next;
		++i;
		if (!n)
			break;
			
	}
	if (n) {
		auto p = &iknow;
		for (auto i = iknow.next; i; p = i, i = i->next) {
			if (i->data == n->data) {
				auto d = p->next;
				p->next = d->next;
				d->next = 0;
				delete d->next;
				d->data = 0;
				break;
			}
		}
	}
	return n != 0;
}

void oCNewsMemory::Insert(oCNews *news)
{
	if ( !news )
		return;

	auto node2 = this->iknow.next;

	auto num = iknow.GetNumInList();
	if (num >= 200) {
		auto node = iknow.next;
		int j = 0;
		while ( j != 199 ) {
			node = node->next;
			++j;
			if ( !node )
				return iknow.Insert(iknow, news);
		}
		if (node->data)
			iknow.Remove(node->data);
		iknow.Insert(news);

	}
	return result;
}

oCNewsMemory::ClearNews()
{
	auto count = iknow.GetNumInList();
	for ( int i = count - 1; i >= 0; --i ) {
		auto news = iknow.Get(i);
		iknow.Remove( news ); // O(N) algorihtm turned into O(N^2)
	}
	iknow.DeleteListDatas();
}

oCNewsMemory::~oCNewsMemory()
{
	ClearNews();
}

void oCNewsMemory::ShowDebugInfos()
{
	screen->Print(0, 0, "ID");
	screen->Print(500, 0, "Witness");
	screen->Print(2000, 0, "Offender");
	screen->Print(3500, 0, "Victim");
	screen->Print(5000, 0, "Type");
	screen->Print(7500, 0, "Time");
	auto fy = 2*screen->FontY();
	int y = fy;
	for (auto news : iknow) {
		int id, g;
		oCNpc* w,o,v;
		news->GetNewsData(id,g,w,o,v);

		screen->Print(0, y, id);
		if (w)
			screen->Print(500, y, w->GetName(0));
		if (o)
			screen->Print(2000, y, o->GetName(0));
		if (v)
			screen->Print(3500, y, v->GetName(0));

		zSTRING type = [] {
		switch ( news->spreadType ) {
			case 0: return "DONT SPREAD";
			case 1: return "FR-VICTIM";
			case 2: return "FR-WITNESS";
			case 3: return "FR-OFFENDER";
			case 4: return "GUILD-VICTIM";
		}}();
		if ( gossip )
			type += "  (G)";

		screen->Print(5000, y, type);
		screen->Print(7500, y, spreadTime);

		y += fy;
	}
}
