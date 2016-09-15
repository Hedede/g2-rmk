#pragma once

struct zCVob {
	static void InitVobSystem()
	{
		Cdecl<void()> func{0x5FE6E0};
		func();
	}
};
