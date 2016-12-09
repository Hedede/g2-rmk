// Gothic\\_ulf\\oNews.cpp
struct oCNews {
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	static int news_counter;
	static int GetRemainingNews() { return news_counter; }
	static zCArray<oCNews*> news_list;
	static void CheckRemainingNews();

	oCNews();
	oCNews(int nid, int g, oCNpc* w, oCNpc *o, oCNpc *v, int gv)
	~oCNews();

	int GetID() const { return id; }

	bool IsGossip() const { return gossip; }
	void SetGossip(zBOOL g)
	{
		gossip = g;
		if (gossip)
			spreadTime = 0;
	}


	bool IsGuildVictim() const { return guildvictim; }

	float GetSpreadTime() const { return spreadTime; }
	void SetSpreadTime(float time)
	{
		spreadTime = time;
		if (time < 0.0)
			spreadTime = 0;
	}

	void GetNewsData(int& id, int& gossip, oCNpc*& w, oCNpc*& o, oCNpc*& v);

	bool IsIdentical(int nid,  oCNpc *o, oCNpc *v) const
	{
		return nid == id && o == offender && v = victim;
	}


private:
	zBOOL told = false;
	float spreadTime = 0.0f;
	int spreadType = 0;
	int id = -1;
	zBOOL gossip = false;
	oCNpc* witness  = nullptr;
	oCNpc* offender = nullptr;
	oCNpc* victim   = nullptr;
	zSTRING witnessName;
	zSTRING offenderName;
	zSTRING victimName;
	zBOOL guildvictim = false;
};

void oCNews::Archive(zCArchiver& arc)
{
	arc.WriteBool("told", told);
	arc.WriteFloat("spread_time", spreadTime);
	arc.WriteInt("spreadType", spreadType);
	arc.WriteInt("news_id", id);
	arc.WriteBool("gossip", gossip);
	arc.WriteBool("guildvictim", guildvictim);

	zSTRING witnessName;
	if ( witness ) {
		witnessName = witness->GetInstanceName();

		if (witness == oCNpc::dontArchiveThisNpc) {
			witness->Release();
			witness = 0;
		}
	}
	arc.WriteString("witnessName", witnessName);

	zSTRING offenderName;
	if ( offender ) {
		offenderName = offender->GetInstanceName();
		if (offender == oCNpc::dontArchiveThisNpc) {
			offender->Release();
			offender = 0;
		}
	}
	arc.WriteString("offenderName", offenderName);

	zSTRING victimName;
	if ( victim ) {
		victimName = victim->GetInstanceName();
		if (victim == oCNpc::dontArchiveThisNpc) {
			victim->Release();
			victim = 0;
		}
	}
	arc.WriteString("victimName", victimName);
}

void oCNews::Unarchive(zCArchiver& arc)
{
	arc.ReadBool("told", told);
	arc.ReadFloat("spread_time", spreadTime);
	arc.ReadInt("spreadType", spreadType);
	arc.ReadInt("news_id", id);
	arc.ReadBool("gossip", gossip);
	arc.ReadBool("guildvictim", guildvictim);

	if (witness) {
		witness->Release();
		witness = 0;
	}

	if (offender) {
		offender->Release();
		offender = 0;
	}

	if (victim) {
		victim->Release();
		victim = 0;
	}

	witnessName = arc.ReadString();
	offenderName = arc.ReadString();
	victimName = arc.ReadString();
}

void oCNews::GetNewsData(int& id, int& gossip, oCNpc*& w, oCNpc*& o, oCNpc*& v)
{
	id = this->id;
	gossip = this->gossip;
	if ( !witness ) {
		if ( witnessName.Length() ) {
			auto vob = ogame->GetGameWorld()->SearchVobByName(witnessName);
			witness = dynamic_cast<oCNpc*>(vob);
			if (witness)
				++witness->refCtr;
		}
	}
	if ( !offender ) {
		if ( offenderName.Length() ) {
			auto vob = ogame->GetGameWorld()->SearchVobByName(offenderName);
			offender = dynamic_cast<oCNpc*>(vob);
			if (offender)
				++offender->refCtr;
		}
	}
	if ( !victim ) {
		if ( victimName.Length() ) {
			auto vob = ogame->GetGameWorld()->SearchVobByName(victimName);
			victim = dynamic_cast<oCNpc*>(vob);
			if (victim)
				++victim->refCtr;
		}
	}
	w = this->witness;
	o = this->offender;
	v = this->victim;
}

void oCNews::CheckRemainingNews()
{
	for (auto news : news_list) {
		if (news) {
			int id, g;
			oCNpc* w,o,v;
			news->GetNewsData(id, g, w, o, v);

			auto msg = "U: NEWS: Found illegal news : "_s + "ID:" + id + "/Gossip:" + g + "/";
			if (w)
				msg += "Witness:" + w->GetInstanceName() + "/";
			if (o)
				msg += "Offender:" + w->GetInstanceName() + "/";
			if (v)
				msg += "Victim:" + w->GetInstanceName() + "/";
			zWARNING(msg); // 516

		}
	}
}

oCNews::oCNews()
{
	++news_counter;
	news_list.Insert( this );
}

oCNews::oCNews(int nid, int g, oCNpc *npc, oCNpc *offen, oCNpc *vic, int gv)
{
	++oCNews::news_counter;

	gossip = g;

	id = nid;
	witness = npc;
	if ( npc )
		++npc->refCtr;
	offender = offen;
	if ( offen )
		++offen->refCtr;
	victim = vic;
	if ( vic )
		++vic->refCtr;

	guildvictim = gv;

	news_list.Insert(this);

	zINFO("U: NEWS: News created: "_s + id); // 559
}

void oCNews::~oCNews()
{
	news_list.Remove( this );

	Release(witness);
	Release(offender);
	Release(victim);

	--news_counter;
}
