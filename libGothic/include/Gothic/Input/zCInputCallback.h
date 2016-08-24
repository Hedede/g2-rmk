#ifndef ZCINPUTCALLBACK_H
#define ZCINPUTCALLBACK_H
#include <Hook/value.h>
#include <Hook/func.h>
#include <Gothic/Types/zCList.h>
struct zCInputCallback {
	zCInputCallback()
	{
		_vtab = reinterpret_cast<void*>(0x83E2FC);
	}

	void SetEnableHandleEvent(bool enable)
	{
		Thiscall<void(zCInputCallback*, int)> call{0x7A54E0};
		call(this, enable);
	}

	static void GetInput();

protected:
	void* _vtab;
private:
	// TODO:
	static constexpr auto& callbacksList = Value<zCList<zCInputCallback>>(0);
};
#endif//ZCINPUTCALLBACK_H
