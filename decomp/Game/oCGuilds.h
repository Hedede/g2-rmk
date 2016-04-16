enum GuildAttitude {
	ATT_HOSTILE = 0,
	ATT_ANGRY = 1,
	ATT_NEUTRAL = 2,
	ATT_FRIENDLY = 3,
};

enum GuildId {
	GIL_NONE = 0,
	GIL_HUMAN = 1,
	GIL_PAL = 1,
	GIL_MIL = 2,
	GIL_VLK = 3,
	GIL_MAG = 4,
	GIL_NOV = 5,
	GIL_DJG = 6,
	GIL_SLD = 7,
	GIL_BAU = 8,
	GIL_BDT = 9,
	GIL_STRF = 10,
	GIL_DMT = 11,
	GIL_OUT = 12,
	GIL_PIR = 13,
	GIL_KDW = 14,
	GIL_EMPTY_D = 15,
	GIL_PUBLIC = 15,
	GIL_SEPERATOR_HUM = 16,
	GIL_MEATBUG = 17,
	GIL_SHEEP = 18,
	GIL_GOBBO = 19,
	GIL_GOBBO_SKELETON = 20,
	GIL_SUMMONED_GOBBO_SKELETON = 21,
	GIL_SCAVENGER = 22,
	GIL_Giant_Rat = 23,
	GIL_GIANT_BUG = 24,
	GIL_BLOODFLY = 25,
	GIL_WARAN = 26,
	GIL_WOLF = 27,
	GIL_SUMMONED_WOLF = 28,
	GIL_MINECRAWLER = 29,
	GIL_LURKER = 30,
	GIL_SKELETON = 31,
	GIL_SUMMONED_SKELETON = 32,
	GIL_SKELETON_MAGE = 33,
	GIL_ZOMBIE = 34,
	GIL_SNAPPER = 35,
	GIL_SHADOWBEAST = 36,
	GIL_SHADOWBEAST_SKELETON = 37,
	GIL_HARPY = 38,
	GIL_STONEGOLEM = 39,
	GIL_FIREGOLEM = 40,
	GIL_ICEGOLEM = 41,
	GIL_SUMMONED_GOLEM = 42,
	GIL_DEMON = 43,
	GIL_SUMMONED_DEMON = 44,
	GIL_TROLL = 45,
	GIL_SWAMPSHARK = 46,
	GIL_DRAGON = 47,
	GIL_MOLERAT = 48,
	GIL_Alligator = 49,
	GIL_SWAMPGOLEM = 50,
	GIL_Stoneguardian = 51,
	GIL_Gargoyle = 52,
	GIL_Empty_A = 53,
	GIL_SummonedGuardian = 54,
	GIL_SummonedZombie = 55,
	GIL_EMPTY_B = 56,
	GIL_EMPTY_C = 57,
	GIL_SEPERATOR_ORC = 58,
	GIL_ORC = 59,
	GIL_FRIENDLY_ORC = 60,
	GIL_UNDEADORC = 61,
	GIL_DRACONIAN = 62,
	GIL_EMPTY_X = 63,
	GIL_EMPTY_Y = 64,
	GIL_EMPTY_Z = 65,
	GIL_MAX = 66,
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
		if (g1 < GIL_MAX && g2 < GIL_MAX )
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
	int numGuilds = GIL_MAX;
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

