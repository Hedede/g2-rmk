#ifndef GOTHIC_ZERROR_H
#define GOTHIC_ZERROR_H
#include <Hook/value.h>
#include <Types/zSTRING.h>

struct zERROR {
	void Init(std::string const& cmdLine)
	{
		Thiscall<void(zERROR*, zSTRING)> func{0x44BFA0};
		func(this, cmdLine);
	}
};

zERROR& zerr = Value<zERROR>(0x8CDCD0);

#endif//GOTHIC_ZERROR_H
