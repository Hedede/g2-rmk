#pragma once
#include <Gothic/Graphics/zRenderer.h>
struct zCRnd_D3D : zCRenderer {
	zCRnd_D3D()
	{
		Thiscall<void(zCRnd_D3D*)> ctor{0x649800};
		ctor(this);
	}
	char data[0x82E7C - sizeof(zCRenderer)];
};
