#include <Types/string_view.h>

constexpr bool has_prefix(string_view str, string_view prefix)
{
	auto plen = prefix.size();
	if (str.size() < plen)
		return false;
	return str.substr(0, plen) == prefix;
}
