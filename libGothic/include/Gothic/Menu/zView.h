#pragma once
#include <Hook/Externals.h>
#include <Gothic/Input/zCInputCallback.h>

struct zCView;
struct zCOLOR;
struct zCView_vt
{
	int (__thiscall *anx)(zCView *, int);
	int (__thiscall *any)(zCView *, int);
	void (__thiscall *nax)(zCView *, int);
	void (__thiscall *nay)(zCView *, int);
	void (__thiscall *ClipLine)(zCView *, int *, int *, int *, int *);
	void (__thiscall *Line)(zCView *, int, int, int, int, const zCOLOR *);
	void (__thiscall *GetViewport)(zCView *, int *, int *, int *, int *);
	void (__thiscall *FillZBuffer)(zCView *);
	void (__thiscall *GetCode)(zCView *, int, int);
	void (__thiscall *dtor)(zCView *, unsigned);
	void (__thiscall *Blit)(zCView *);
	void (__thiscall *DrawItems)(zCView *);
};


struct zCViewBase {
	zCView_vt *vtab;
};


struct zCView : zCViewBase, zCInputCallback {
	static void SetMode(int x, int y, int b, void*& w)
	{
		Cdecl<void(int,int,int,void*&)> call{0x7ABDB0};
		call(x,y,b,w);
	}

	void InsertItem(zCView* child, int front)
	{
		Thiscall<void(zCView*, zCView*, int)> call{0x7ABAD0};
		call(this, child, front);
	}

	void RemoveItem(zCView* item)
	{
		Thiscall<void(zCView*, zCView*)> call{0x7ABD10};
		call(this, item);
	}

	void SetPos(int vx, int vy)
	{
		Thiscall<void(zCView*, int, int)> call{0x7A75B0};
		call(this, vx, vy);
	}

	void GetSize(int& x, int& y)
	{
		Thiscall<void(zCView*, int&, int&)> call{ 0x7A7810 };
		call(this, x, y);

		//*x = vsizex;
		//*y = vsizey;
	}

	int FontY()
	{
		Thiscall<int(zCView*)> call{0x7A99F0};
		return call(this);
	}

	char data[0x100-8];
};

inline zCView*& screen = Value<zCView*>(0xAB6468);
