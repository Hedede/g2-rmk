#include <Types/string_view.h>

constexpr bool has_prefix(string_view str, string_view prefix)
{
	auto plen = prefix.size();
	if (str.size() < plen)
		return false;
	return str.substr(0, plen) == prefix;
}

template<typename T>
bool try_parse(string_view in, T& out);

template<> bool try_parse(string_view in, double& out);
template<> bool try_parse(string_view in, long& out);
template<> bool try_parse(string_view in, unsigned long& out);

template<> bool try_parse(string_view in, int& out);
template<> bool try_parse(string_view in, float& out);

template<typename T>
T parse(string_view in, T def = T())
{
	T tmp; if (!try_parse(in, tmp)) return def; return tmp;
}

template<> int parse(string_view in, int def);
template<> unsigned parse(string_view in, unsigned def);
template<> bool parse(string_view in, bool def);
