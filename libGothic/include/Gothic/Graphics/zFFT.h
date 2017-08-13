#pragma once

struct zCFFT {
	static void S_Init()
	{
		Cdecl<void()> func{0x559C40};
		func();
	}
};
