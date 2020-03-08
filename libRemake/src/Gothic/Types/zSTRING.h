#pragma once
#include <string>
#include <string_view>
#include <Hook/Externals.h>
#include <Gothic/Types/Base.h>

char const spaceChar = ' ';

struct zSTRING {
	zSTRING()
		: _vtab(0x82E6F0), s{0, nullptr, 0, 0}
	{
		//thiscall(0x402AF0, this);
	}

	zSTRING(char const* str)
	{
		thiscall(0x4010C0, this, str);
	}

	zSTRING(std::string const& str)
		: zSTRING()
	{
		thiscall(0x4301B0, &s, str.data(), str.size());
	}

	zSTRING(std::string_view const& str)
		: zSTRING()
	{
		thiscall(0x4301B0, &s, str.data(), str.size());
	}

	zSTRING& operator=(char const* str)
	{
		thiscall(0x4CFAF0, this, str);
		return *this;
	}

	zSTRING& operator=(std::string const& str)
	{
		return operator=(str.c_str());
	}

	explicit operator std::string() const
	{
		if (!s.ptr) return {};
		if (s.len <= 0) return {};
		return std::string{s.ptr};
	}

	operator std::string_view() const
	{
		if (!s.ptr) return {};
		if (s.len <= 0) return {};
		return std::string_view{s.ptr, s.len};
	}

	char const* Data() const { return s.ptr; }
	size_t  Length() const { return s.len; }
	bool     Empty() const { return Length() == 0; }
	operator  bool() const { return Length() > 0; }
	bool operator!() const { return Empty(); }


	zSTRING& Upper()
	{
		thiscall(0x46AB00, this);
		return *this;
	}

private:
	uintptr_t _vtab;

	struct xstring {
		int alloc;
		char *ptr;
		size_t len;
		int res;
	} s;
};

inline unsigned ToUnsigned(std::string const& str)
{
	return std::strtol(str.data(), 0, 10);
}
