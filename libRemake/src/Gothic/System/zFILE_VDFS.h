#pragma once
#include <Hook/Externals.h>

struct zFILE_VDFS {
	static void InitFileSystem()
	{
		Cdecl<void()> call{0x44AD60};
		call();
	}

	static void DeinitFileSystem()
	{
		Cdecl<void()> call{0x471050};
		call();
	}
};
