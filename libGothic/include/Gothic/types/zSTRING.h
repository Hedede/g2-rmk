#ifndef G2_zSTRING_h
#define G2_zSTRING_h
#include <string>
#include <ostream>
#include <Gothic/types/common.h>

char const spaceChar = ' ';

class zSTRING {
public:
	zSTRING() = default;
	zSTRING(const char *str);
	zSTRING(char c);
	zSTRING(zSTRING const& other);
	zSTRING(zSTRING const* other);
	zSTRING(int32_t i);
	zSTRING(uint32_t i);
	zSTRING(double f, uint8_t decimals);
	zSTRING(float f, uint8_t decimals);

	virtual ~zSTRING() = default;

	zSTRING& operator=(zSTRING const& other)
	{
		data = other.data;
		return *this;
	}

	zSTRING& operator+=(zSTRING const& other)
	{
		data += other.data;
		return *this;
	}

	zSTRING& operator+=(char const* str)
	{
		data += str;
		return *this;
	}

	zSTRING& operator+=(char c)
	{
		data += c;
		return *this;
	}

	char const* operator[](size_t index)
	{
		if (index >= data.size())
			return &spaceChar;
		return &data[index];
	}

	void Init()
	{
	}

	bool IsEmpty() const
	{
		return data.empty();
	}

	int Length() const
	{
		return data.size();
	}

	void Clear()
	{
		data.clear();
	}
	
	zSTRING& Lower();
	zSTRING& Upper();
	zSTRING& UpperFirstLowerRest();

	int Search(int begin, char const* substring, size_t nr) const;
	int Search(char const* substring, size_t nr) const
	{
		return Search(0, substring, nr);
	}
	int Search(zSTRING const& substring, size_t nr) const
	{
		return Search(0, substring.data.c_str(), nr);
	}

	char const*
	PickWordPos(size_t nr, zSTRING const& delim1, zSTRING const& delim2) const;
	zSTRING PickWord(size_t nr, zSTRING const& delim1, zSTRING const& delim2) const;
	zSTRING PickWord_Old(size_t nr, zSTRING const& delim) const
	{
		return PickWord(nr, delim, " ");
	}

	int32_t ToInt() const;
	float ToFloat() const;
	char ToChar() const;
private:
	std::string data;
};

zSTRING operator+(zSTRING const& str, zSTRING const& str2)
{
	zSTRING tmp(str);
	tmp += str2;
	return tmp;
}

zSTRING operator+(zSTRING const& str, char const* str2)
{
	zSTRING tmp(str);
	tmp += str2;
	return tmp;
}

zSTRING operator+(char const* str, zSTRING const& str2)
{
	zSTRING tmp(str);
	tmp += str2;
	return tmp;
}

zSTRING operator+(zSTRING const& str, char c)
{
	zSTRING tmp(str);
	tmp += c;
	return tmp;
}

zSTRING operator+(char c, zSTRING const& str)
{
	zSTRING tmp(c);
	tmp += str;
	return tmp;
}

zSTRING operator ""_s (const char* str)
{
	return str;
}
#endif//G2_zSTRING_h
