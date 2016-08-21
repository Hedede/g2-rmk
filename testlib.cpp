#include <Game/CGameManager.h>

struct zVEC3 {
	float x[3];
};

struct SpellArray {
	int x[3];
};

struct ItemArray {
	int x[3];
};

struct zCVob;
struct zCWorld;
struct zCModel;

struct oCMag_Book {
	SpellArray spells;
	ItemArray spell_itmes;
	zCWorld *owner_world;
	zCVob *owner;
	zCModel *owner_model;
	int spell_no;
	float mag_height;
	zBOOL active;
	zBOOL remove_all;
	int inMovement;
	zBOOL showHandSymbol;
	float step;
	int action;
	int unused0;
	zBOOL open;
	zBOOL open_delay_timer;
	zBOOL show_particles;
	float targetdir;
	float t1;
	zVEC3 targetPos;
	zVEC3 startPos;
	int nextRegisterAt;
	int keys;
};

auto GetSelectedSpellNr = func<int (__thiscall*)(oCMag_Book* ptr)>(0x4777D0);
