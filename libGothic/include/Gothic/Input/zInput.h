#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>

struct zSKeyMapping {
	uint16_t id;
	zCArray<uint16_t> keys;
	int unk;
};

struct zCInput {
	zCInput()
	{
		vtab = reinterpret_cast<void*>(0x830AD4);
		Cdecl<void()>{0x4CC640}();
		mappings.Compare = +[] (zSKeyMapping* const& a, zSKeyMapping* const& b)
		{
			return a->id - b->id;
		};
	}

private:
	void* vtab;
	zCArraySort<zSKeyMapping*> mappings;
};

struct zCInput_Win32 {
	zCInput_Win32(void* hwnd)
	{
		Thiscall<void(zCInput_Win32*, void*)>{0x4D4A10}(this, hwnd);
	}
	char data[0x58];
};

extern zCInput*& zinput;
