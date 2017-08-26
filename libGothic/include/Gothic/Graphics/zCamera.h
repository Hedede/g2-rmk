#pragma once
#include <Hook/Externals.h>
struct zCView;
struct zCCamera {
	void Activate()
	{
		Thiscall<void(zCCamera*)> call{0x54A700};
		call(this);
	}
	void PreRenderProcessing()
	{
		Thiscall<void(zCCamera*)> call{0x54B7D0};
		call(this);
	}
	void PostRenderProcessing()
	{
		Thiscall<void(zCCamera*)> call{0x54B920};
		call(this);
	}
	void UpdateViewport()
	{
		Thiscall<void(zCCamera*)> call{0x54AA90};
		call(this);
	}


	char data1[0xA0];
	zCView *targetView;
	char data2[0x87C];
	zCVob *connectedVob;
	float  topBottomSin;
	float  topBottomCos;
	float  leftRightSin;
	float  leftRightCos;
};
