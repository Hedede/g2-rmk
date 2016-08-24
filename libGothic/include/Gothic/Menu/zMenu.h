#ifndef GOTHIC_ZMENU_H
#define GOTHIC_ZMENU_H
#include <Hook/func.h>

struct zCParser;

struct zCMenu {
	static zCParser*& menuParser;
	static void CreateParser();

	static void Startup()
	{
		Cdecl<void()> call{0x4D9F90};
		call();
	}
};

#endif//GOTHIC_ZMENU_H
