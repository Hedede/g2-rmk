#pragma once

struct zCVisual {
	static void InitVisualSystem()
	{
		Cdecl<void()> func{0x6068F0};
		func();
	}
};
