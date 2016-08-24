#ifndef Gothic_zTexture_H
#define Gothic_zTexture_H
#include <Hook/Externals.h>
struct zCTexture {
	static void ScanConvertTextures(std::string const& dir)
	{
		Cdecl<void(zSTRING)> call{0x5F7F40};
		call(zSTRING{dir});
	}
};
#endif//Gothic_zTexture_H
