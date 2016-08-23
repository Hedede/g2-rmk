#ifndef ZCINPUTCALLBACK_H
#define ZCINPUTCALLBACK_H
#include <Hook/value.h>
#include <Gothic/Types/zCList.h>
struct zCInputCallback {
	zCInputCallback()
	{
		_vtab = reinterpret_cast<void*>(0x83E2FC);
	}

	static void GetInput();

protected:
	void* _vtab;
private:
	// TODO:
	static constexpr auto& callbacksList = Value<zCList<zCInputCallback>>(0);
};
#endif//ZCINPUTCALLBACK_H
