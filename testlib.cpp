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

template<typename T>
using func = T;

auto GetSelectedSpellNr = func<int (__thiscall*)(oCMag_Book* ptr)>(0x4777D0);
auto libExit = func<void(*)()>(0x502AB0);
auto sysEvent = func<void(*)()>(0x5053E0);

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>

void put(std::string s)
{
	fwrite(s.c_str(), s.size(), 1, stdout);
}

void put(char const* w)
{
	put(std::string(w));
}

template<typename T>
void put(T t)
{
	put(std::to_string(t));
}

template<typename... Args>
void print(Args... args)
{
	( put(args), ... );
}

template<typename... Args>
void println(Args... args)
{
	( put(args), ..., put("\n"));
}

extern "C" void aw_main()
{
	freopen("log.txt", "wb", stdout);
	oCMag_Book book;
	book.spell_no = 10;
	int spell = GetSelectedSpellNr(&book);

	//auto file = fopen("text.txt", "wb");

	print("TEST\n");
	println(spell);

	MessageBox(0, TEXT("WORKS!"), TEXT("YIPPIKIAY"),MB_OK);
	MessageBox(0, getcwd(0,0), TEXT("YIPPIKIAY"),MB_OK);

	CGameManager game;

	sysEvent();

	game.Init((void*)0x8D422C);
	game.Run();
	game.Done();

	libExit();
	exit(0);
}
