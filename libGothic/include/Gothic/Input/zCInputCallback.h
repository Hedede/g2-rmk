#ifndef ZCINPUTCALLBACK_H
#define ZCINPUTCALLBACK_H
#include <Types/zCList.h>
struct zCInputCallback {
	static void GetInput();
private:
	zCList<zCInputCallback> callbacksList;
};
#endif//ZCINPUTCALLBACK_H
