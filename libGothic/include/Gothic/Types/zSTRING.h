#pragma once
#include <string>
#include <string_view>
#include <Hook/Externals.h>
#include <Gothic/Types/Base.h>

char const spaceChar = ' ';

struct zSTRING {
	zSTRING()
	{
		Thiscall<void(zSTRING*)> ctor{0x402AF0};
		ctor(this);
	}

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
		if (!ptr) return {};
		if (len <= 0) return {};
		return std::string{ptr};
	}

	operator std::string_view() const
	{
		if (!ptr) return {};
		if (len <= 0) return {};
		return std::string_view{ptr};
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

	int alloc;
	char *ptr;
	int len;
	int res;
};

inline unsigned ToUnsigned(std::string const& str)
{
	return std::strtol(str.data(), 0, 10);
}
