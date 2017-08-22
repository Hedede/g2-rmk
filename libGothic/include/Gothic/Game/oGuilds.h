#ifndef GOTHIC_GUILDS_H
#define GOTHIC_GUILDS_H
enum GuildAttitude {
	ATT_HOSTILE = 0,
	ATT_ANGRY = 1,
	ATT_NEUTRAL = 2,
	ATT_FRIENDLY = 3,
};

struct oCGuilds {
	oCGuilds() {
		Thiscall<void(oCGuilds*, zSTRING const&)> InitGuildTable{0x700FA0};
		InitGuildTable( this, "GIL_ATTITUDES" );
	}

	int maxGuilds  = 66;
	int *attitudes = new int[maxGuilds*maxGuilds]{ATT_FRIENDLY};
};


#endif//GOTHIC_GUILDS_H
