#ifndef Gothic_zFont_H
#define Gothic_zFont_H
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zVEC2.h>
#include <Gothic/Graphics/zTexture.h>

struct zCFont {
	zCFont(std::string const& name)
	{
		Thiscall<void(zCFont*,zSTRING const&)> ctor{0x788780};
		ctor(this, name);
	}

	~zCFont()
	{
		Thiscall<void(zCFont*)> dtor{0x788920};
		dtor(this);
	}

	std::string GetFontName() const
	{
		return std::string(fontName);
	}

	zSTRING fontName;

	int height;

	zCTexture *fontTexture;

	char width[256];
	zVEC2 unk[256];
	zVEC2 unk2[256];
};

#endif//Gothic_zFont_H
