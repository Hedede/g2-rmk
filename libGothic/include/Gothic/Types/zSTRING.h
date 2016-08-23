#ifndef G2_zSTRING_h
#define G2_zSTRING_h
#include <string>
#include <Hook/func.h>
#include <Gothic/Types/Base.h>

char const spaceChar = ' ';

struct zSTRING {
	zSTRING() = default;

	zSTRING(char const* str)
	{
		Thiscall<void(zSTRING*, char const*)> ctor{0x4010C0};
		ctor(this, str);
	}

	zSTRING(std::string const& str)
		: zSTRING(str.c_str())
	{ }

	zSTRING& operator=(char const* str)
	{
		Thiscall<void(zSTRING*, char const*)> assign{0x4CFAF0};
		assign(this, str);
		return *this;
	}

	zSTRING& operator=(std::string const& str)
	{
		return operator=(str.c_str());
	}

private:
	void* _vtab;
	void* data[4];
};
#endif//G2_zSTRING_h
