#pragma once

struct zCMapDetailTexture {
	static void S_Init()
	{
		Cdecl<void()> func{0x5DAF00};
		func();
	}
};
