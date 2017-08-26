#pragma once
#include <Gothic/Types/zTree.h>
//#include <Gothic/Types/zMAT4.h>
struct zCVob : zCObject {
	static void InitVobSystem()
	{
		Cdecl<void()> func{0x5FE6E0};
		func();
	}

	char data[0x94];
	zCWorld *homeWorld;
};
