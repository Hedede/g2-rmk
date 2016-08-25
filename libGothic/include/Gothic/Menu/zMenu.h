#ifndef GOTHIC_ZMENU_H
#define GOTHIC_ZMENU_H
#include <Hook/Externals.h>

struct zCParser;

struct zCMenu {
	static zCParser*& menuParser;
	static void CreateParser();

	static void Startup()
	{
		Cdecl<void()> call{0x4D9F90};
		call();
	}

	using Callback = int(*)();
	static Callback& EnterCallback;
	static Callback& LeaveCallback;
};

#endif//GOTHIC_ZMENU_H
