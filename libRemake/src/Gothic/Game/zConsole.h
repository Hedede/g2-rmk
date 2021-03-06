#pragma once
#include <Gothic/Types/Base.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Hook/Externals.h>

struct zCWorld;
struct zCParser;
struct zCView;

struct zCConDat;

constexpr int zCON_MAX_EVAL = 15;

struct zCConsole {
	zCConsole()
	{
		thiscall(0x781EA0, this);
	}

	zCConsole(int x, int y, std::string const& name)
	{
		checked::thiscall<void(zCConsole*, int,int,zSTRING const&)>(0x7820B0, this, x, y, name);
	}

	void Register(std::string const& cmd, std::string const& desc, int param)
	{
		const zSTRING s = cmd;
		const zSTRING h = desc;
		thiscall(0x7829C0, this, &s, &h, param);
	}

	void Register(std::string const& cmd, std::string const& desc)
	{
		const zSTRING s = cmd;
		const zSTRING h = desc;
		thiscall(0x782AE0, this, &s, &h);
	}

	using EvalFunc = zBOOL( const zSTRING& s, zSTRING& msg );
	void AddEvalFunc(EvalFunc* func)
	{
		if ( evalcount + 1 < zCON_MAX_EVAL )
			evalfunc[evalcount++] = func;
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
	EvalFunc* evalfunc[zCON_MAX_EVAL];

	void (*changedfunc)(const zSTRING& s);
	zCWorld *world;
	zCParser* parser;
	int edit_index;
	void *edit_adr;
};

static zCConsole& zcon = Value<zCConsole>(0xAB3860);
