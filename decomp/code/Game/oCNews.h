struct oCNews {
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	int GetID() const { return id; }

	bool IsGossip() const { return gossip; }

	bool IsGuildVictim() const { return guildvictim; }

	float GetSpreadTime() const { return spreadTime; }

	bool IsIdentical(int nid,  oCNpc *o, oCNpc *v) const
	{
		return nid == id && o == offender && v = victim;
	}

private:
	zBOOL told;
	float spreadTime;
	int spreadType;
	int id;
	zBOOL gossip;
	oCNpc *witness;
	oCNpc *offender;
	oCNpc *victim;
	zSTRING witnessName;
	zSTRING offenderName;
	zSTRING victimName;
	zBOOL guildvictim;
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
