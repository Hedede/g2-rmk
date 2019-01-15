#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>

struct zERROR {
	void Init(std::string cmdLine) {}

	void SetFilterLevel(int max_level)
	{
		filter_level = max_level;
	}

	void SetTarget(int target)
	{
		this->target = target;
	}

	void SetFilterAuthors(std::string const& authors)
	{
		filter_authors = authors;
	}

	/*
	{
		Thiscall<void(zERROR*, zSTRING)> func{0x44BFA0};
		func(this, cmdLine);
	}*/

private:
	void* vtab;
	void (*onexit)();

	zSTRING filter_authors;
	int     filter_flag  = 0;
	int     filter_level = -1;

	int target = 2;

	int ack_type = 0;
	void* log_file = 0;
	int indent_depth = 0;

	void* spyHandle;
	void* spyMutex;
};

static zERROR& zerr = Value<zERROR>(0x8CDCD0);

#include <Hook/size_checker.h>
CHECK_SIZE(zERROR, 0x3C);
