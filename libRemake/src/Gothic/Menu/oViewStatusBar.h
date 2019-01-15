#ifndef GOTHIC_VIEWSTATUSBAR_H
#define GOTHIC_VIEWSTATUSBAR_H
#include <Gothic/Menu/zView.h>

struct zCViewStatusBar : zCView {
	int minLow;
	int maxHigh;
	int low;
	int high;
	int previewValue;
	int currentValue;
	int scale;
	zCView *range_bar;
	zCView *value_bar;
};

struct oCViewStatusBar : zCViewStatusBar {
	oCViewStatusBar()
	{
		Thiscall<void(oCViewStatusBar*)> ctor{0x43D410};
		ctor(this);
	}

	oCViewStatusBar(int x1, int y1, int x2, int y2, int viewID)
	{
		Thiscall<void(oCViewStatusBar*, int,int,int,int,int)> ctor{0x43D540};
		ctor(this,x1,y1,x2,y2,viewID);
	}

	void Init(int x, int y, float scale)
	{
		Thiscall<void(oCViewStatusBar*, int, int, float)> call{0x43D620};
		call( this, x, y, scale );
	}

	void SetTextures(
		std::string const& view,
		std::string const& rangebar,
		std::string const& valuebar,
		std::string_view // unused
	)
	{
		Thiscall<void(oCViewStatusBar*, zSTRING const&, zSTRING const&, zSTRING const&, zSTRING const&)> call{0x43D7B0};
		call(this, view, rangebar, valuebar, "");
	}

	void SetRange(float min, float max)
	{
		Thiscall<void(oCViewStatusBar*, float, float)> call{0x43DCB0};
		call(this, min, max);
	}

	void SetMaxRange(float min, float max)
	{
		Thiscall<void(oCViewStatusBar*, float, float)> call{0x43DBA0};
		call(this, min, max);
	}


	zSTRING texView;
	zSTRING texRange;
	zSTRING texValue;
};

struct zCViewProgressBar {
	void SetPercent(int percent, std::string const& text)
	{
		Thiscall<void(zCViewProgressBar*,int,zSTRING)> call{0x46EEC0};
		call(this,percent,text);
	}
};

#endif//GOTHIC_VIEWSTATUSBAR_H
