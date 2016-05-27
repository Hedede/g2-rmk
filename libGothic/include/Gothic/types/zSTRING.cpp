#include <cstring>
#include "zSTRING.h"

zSTRING::zSTRING(char const* str)
	: data(str)
{
}

zSTRING::zSTRING(char c)
	: data(1, c)
{
}

zSTRING::zSTRING(zSTRING const& other)
	: data(other.data)
{
}

zSTRING::zSTRING(zSTRING const* other)
	: data(other->data)
{
}

zSTRING::zSTRING(int32_t i)
	: data(std::to_string(i))
{
}

zSTRING::zSTRING(uint32_t i)
	: data(std::to_string(i))
{
}

// OK, that's not really from the engine
constexpr size_t zSTRING_MAX_DOUBLE = sizeof("-2.22507386e-308") + 1;
zSTRING::zSTRING(double f, uint8_t decimals)
	//: data(std::to_string(f))
{
	char dest[zSTRING_MAX_DOUBLE];
	char format[6];
	if (0 > decimals || decimals > 9)
		decimals = 9;

	strcpy(format, "%.9g");
	format[2] = decimals + 48;
	snprintf(dest, sizeof(dest), format, f);
	data = std::string(dest);
}

zSTRING::zSTRING(float f, uint8_t decimals)
	//: data(std::to_string(f))
{
	char dest[zSTRING_MAX_DOUBLE];
	char format[5];
	if (0 > decimals || decimals > 9)
		decimals = 9;

	strcpy(format, "%.9g");
	format[2] = decimals + 48;
	snprintf(dest, sizeof(dest), format, f);
	data = std::string(dest);
}

int zSTRING::Search(int begin, char const* substring, size_t nr) const
{
	if ( !substring || !nr )
		return -1;

	int cur = begin;
	int result = -1;

	size_t finds = 0;

	while ( 1 )
	{
		result = data.find(substring, cur);

		if ( result == std::end(data) )
			break;

		cur = result + 1;

		if ( ++finds >= nr )
			return result;
	}

	return -1;
}

char const*
zSTRING::PickWordPos(size_t nr, zSTRING const& delim1, zSTRING const& delim2) const
{
	if ( IsEmpty() )
		return 0;

	bool skip = false;
	for (size_t i = 0; i < Length(); ++i) {
		char c = data[i];
		if (delim2.data.find(c) == std::string::npos)
		{
			if (delim1.data.find(c) == std::string::npos) {
				if ( !skip ) {
					skip = true;
					if ( !--nr )
						return &data[i];
				}
				continue;
			}

			if ( !--nr )
				return &data[i];
		}
		skip = false;
	}
	return 0;
}

zSTRING zSTRING::PickWord(size_t nr, zSTRING const& delim1, zSTRING const& delim2) const
{
	if ( IsEmpty() )
		return "";

	char const* wpos = PickWordPos(nr, delim1, delim2);
	if ( !wpos )
		return "";

	zSTRING ret;
	if ( delim1.data.find(*wpos) != std::string::npos )
		ret += *wpos;

	for (; *wpos; ++wpos)
	{
		if ( delim2.data.find(*wpos) != std::string::npos )
			break;
		if ( delim1.data.find(*wpos) != std::string::npos )
			break;
		ret += *wpos;
	}

	return ret;
}

zSTRING Characters(char c, unsigned anzahl)
{
	return std::string(c, anzahl).c_str();
}

zSTRING Spaces(unsigned num)
{
	return Characters(' ', num);
}

zSTRING& zSTRING::Lower()
{
	for (char& c: data)
		c = std::tolower(c);
	return *this;
}

zSTRING& zSTRING::Upper()
{
	for (char& c: data)
		c = std::toupper(c);
	return *this;
}

zSTRING& zSTRING::UpperFirstLowerRest()
{
	if (IsEmpty())
		return *this;

	auto it = std::begin(data);
	*it = std::toupper(*it);

	for (it = it + 1; it != std::end(data); ++it)
		*it = std::tolower(*it);
	return *this;
}

unsigned long zSTRING::ToUint() const
{
	return stoul(data);
}

long zSTRING::ToInt() const
{
	return stol(data);
}

double zSTRING::ToFloat() const
{
	return stod(data);
}

char zSTRING::ToChar() const
{
	return data[0];
}

