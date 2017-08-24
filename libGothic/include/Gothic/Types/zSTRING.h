#pragma once
#include <string>
#include <Hook/Externals.h>
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

	explicit operator std::string() const
	{
		return std::string(ptr, len);
	}

	char const* Data() const { return ptr; }
	size_t  Length() const { return len; }
	bool     Empty() const { return Length() == 0; }
	operator  bool() const { return Length() > 0; }
	bool operator!() const { return Empty(); }


	zSTRING& Upper()
	{
		Thiscall<void(zSTRING*)> func{0x46AB00};
		func(this);
	}

private:
	void* _vtab;
	char alloc;
	char *ptr;
	size_t len;
	int res;
};

inline unsigned ToUnsigned(std::string const& str)
{
	return std::strtol(str.data(), 0, 10);
}
