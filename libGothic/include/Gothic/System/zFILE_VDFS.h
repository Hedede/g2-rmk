#ifndef Gothic_zFILE_VDFS_H
#define Gothic_zFILE_VDFS_H
#include <Hook/Externals.h>

struct zFILE_VDFS {
	static void InitFileSystem()
	{
		Cdecl<void()> call{0x44AD60};
		call();
	}
};

#endif//Gothic_zFILE_VDFS_H
