#pragma once
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Hook/Externals.h>

struct zCWorld;
struct zCParser;
struct zCView;

struct zCConDat;

struct zCConsole {
	zCConsole()
	{
		Thiscall<void(zCConsole*)> ctor{0x781EA0};
		ctor(this);
	}

	zCConsole(int x, int y, std::string const& name)
	{
		Thiscall<void(zCConsole*, int,int,zSTRING const&)> ctor{0x7820B0};
		ctor(this, x, y, zSTRING{name});
	}

	void SetPos(int x, int y)
	{
		px = x;
		py = y;
	}

	void SetDim(int x, int y)
	{
		lx = x;
		ly = y;
		dynsize = 0;
	}
	
	void SetAutoCompletion(bool b)
	{
		autocomplete = b;
	}

	void *vtable;
	zSTRING id;
	zSTRING instr;
	zSTRING savemsg;
	zCArray<zSTRING> lastcommand;
	int lastCommandPos;
	int px;
	int py;
	int lx;
	int ly;
	int showmax;
	int skip;
	int dynsize;
	int var;
	int autocomplete;
	zList<zCConDat> list;
	zCView *conview;
	int evalcount;
	int (*evalfunc[15])(const zSTRING& s, zSTRING& msg);
	void (*changedfunc)(const zSTRING& s);
	zCWorld *world;
	zCParser *cparser;
	int edit_index;
	void *edit_adr;
};

static zCConsole& zcon = Value<zCConsole>(0xAB3860);
