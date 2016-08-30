#ifndef Gothic_oMenuMain_H
#define Gothic_oMenuMain_H
#include <Gothic/Menu/zMenu.h>

struct oCMenuMain : zCMenu {
	oCMenuMain(std::string const& name)
		: zCMenu(name)
	{
		_vtab = (void*)0x8300B4;
	}
};

#endif//Gothic_oMenuMain_H
