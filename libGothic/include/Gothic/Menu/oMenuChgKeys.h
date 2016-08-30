#ifndef Gothic_oMenuChgKeys_H
#define Gothic_oMenuChgKeys_H
#include <Gothic/Menu/zMenu.h>

struct oCMenuChgKeys : zCMenu {
	oCMenuChgKeys(std::string const& name)
		: zCMenu(name)
	{
		_vtab = (void*)0x830104;
	}
};

#endif//Gothic_oMenuChgKeys_H
