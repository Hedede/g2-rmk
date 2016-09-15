#ifndef Gothic_zTexture_H
#define Gothic_zTexture_H
#include <Hook/Externals.h>
#include <Gothic/Types/Base.h>
struct zCTexture {
	static zBOOL& s_globalLoadTextures;

	static void ScanConvertTextures(std::string const& dir)
	{
		Cdecl<void(zSTRING)> call{0x5F7F40};
		call(zSTRING{dir});
	}

	static void AutoDetectRendererTexScale()
	{
		Cdecl<void()> call{0x5F46E0};
		call();
	}
};
#endif//Gothic_zTexture_H
