#pragma once
#include <Hook/Externals.h>
#include <Gothic/Input/zCInputCallback.h>

struct zCViewBase {
	void *vtab;
};


struct zCView : zCViewBase, zCInputCallback {
	static void SetMode(int x, int y, int b, void*& w)
	{
		Cdecl<void(int,int,int,void*&)> call{0x7ABDB0};
		call(x,y,b,w);
	}
};

zCView*& screen = Value<zCView*>(0xAB6468);
