#ifndef ZCINPUTCALLBACK_H
#define ZCINPUTCALLBACK_H
#include <Hook/value.h>
#include <Gothic/Types/zCList.h>
struct zCInputCallback {
	static void GetInput();
private:
	// TODO:
	static callbacksList = Value<zCList<zCInputCallback>>(0);
};
#endif//ZCINPUTCALLBACK_H
