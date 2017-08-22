#ifndef GOTHIC_INFO_H
#define GOTHIC_INFO_H

#include <Gothic/Types/zCList.h>
struct zCParser;
struct oCInfo;
struct oCInfoManager {
	oCInfoManager(zCParser& par)
	{
		Thiscall<void(oCInfoManager*, zCParser&)> ctor{0x7023F0};
		ctor(this, par);
	}
	void*_vtbl;
	zCListSort<oCInfo> infoList;
	zCParser *parser;
};

#endif//GOTHIC_INFO_H
