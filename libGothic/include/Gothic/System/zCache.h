#pragma once

struct zCCacheBase {
	static void S_ClearCaches()
	{
		Cdecl<void()> func{0x5F0340};
		func();
	}

};
