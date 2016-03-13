enum GuildAttitude {
	ATT_HOSTILE = 0,
	ATT_ANGRY = 1,
	ATT_NEUTRAL = 2,
	ATT_FRIENDLY = 3,
};

class oCGuilds {
public:
	static zSTRING GetGuildName(int guild)
	{
		zCPar_Symbol* txt_guilds = zparser->GetSymbol("TXT_GUILDS");
		if ( txt_guilds ) {
			zSTRING guildName;
			txt_guilds->GetValue(guildName, guild);
			return guildName;
		}

		return "Error : Missing guild-names !";
	}

	oCGuilds()
	{
		attitudes = new uint8_t[numGuilds * numGuilds]();
		InitGuildTable("GIL_ATTITUDES");
	}

	~oCGuilds()
	{
		delete[] attitudes;
	}

	int GetAttitude(int g1, int g2)
	{
		if ( g1 >= 66 || g2 >= 66 ) {
			zWARNING("U:GIL:Unknown Guild " + zSTRING(g1) + ":" + zSTRING(g2));
			return ATT_NEUTRAL;
		}
		return attitudes[g1 + numGuilds * g2];
	}

	void SetAttitude(int g1, int g2, uint8_t att)
	{
		if (g1 < 66 && g2 < 66 )
			attitudes[g1 + g2 * numGuilds] = att;
	}

	void InitGuildTable(zSTRING const& tableName);

	void SaveGuildTable(zCArchiver& arc)
	{
		arc.WriteRaw("guildTable", attitudes, numGuilds * numGuilds);
	}

	void LoadGuildTable(zCArchiver& arc)
	{
		arc.ReadRaw("guildTable", attitudes, numGuilds * numGuilds);
	}
private:
	int numGuilds = 66;
	uint8_t* attitudes;
};


void oCGuilds::InitGuildTable(zSTRING const& tableName)
{
	int index = zparser.GetIndex(tableName);
	if ( index >= 0 ) {
		auto sym = zparser.GetSymbol(index);
		if ( (sym->bitfield & 0xFFF) != 256 ) {
			zWARNING("U:GUILDS:Guild-attitude-table size mismatch : Size has to be " + 66);
		}

		for (int i = 0; i < 16; ++i) {
			for (int j = 0; j < 16; ++j) {
				int val;
				sym->GetValue(val, i * 16 + j);
				attitude[i + j * maxGuilds] = val;
			}
		}

		zINFO(5, "U:GUILDS:Guild-attitude-table changed : " + tableName);
	} else {
		zWARNING("U:GUILDS:Guild-Attitudes not found :" + tableName);
	}
}

