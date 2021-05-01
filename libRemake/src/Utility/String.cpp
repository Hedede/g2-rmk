#include <Utility/String.h>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <charconv>

template<>
bool try_parse(string_view in, double& out)
{
	char const* begin = in.data();
	char* end;
	out = strtod(begin, &end);
	return (out != 0) || (end != begin);
}

template<>
bool try_parse(string_view in, int& out)
{
	char const* begin = in.data();
	char* end;
	out = strtol(begin, &end, 0);
	return (out != 0) || (end != begin);
}

template<>
bool try_parse(string_view in, long& out)
{
	auto result = std::from_chars(in.data(), in.data() + in.size(), out);
	return result.ec == std::errc();
}

template<>
bool try_parse(string_view in, unsigned long& out)
{
	char const* begin = in.data();
	char* end;
	out = strtoul(begin, &end, 0);
	return (out != 0) || (end != begin);
}

template<>
bool try_parse(string_view in, float& out)
{
	static float min = std::numeric_limits<float>::min();
	static float max = std::numeric_limits<float>::max();
	double tmp;
	if ( !try_parse(in, tmp) )
		return false;
	if (tmp == 0.0 || tmp == -0.0) {
		out = tmp;
		return true;
	}
	double abstmp = abs(tmp);
	if (abstmp < min) {
		out = tmp < 0 ? -min : min;
	} else if (abstmp > max) {
		out = tmp < 0 ? -max : max;
	} else {
		out = float(tmp);
	}
	return true;
}



template<>
int parse(string_view in, int def)
{
	static const long min = std::numeric_limits<int>::min();
	static const long max = std::numeric_limits<int>::max();
	long tmp;
	if( !try_parse(in, tmp) || tmp > max || tmp < min )
		return def;
	return int(tmp);
}

template<>
unsigned parse(string_view in, unsigned def)
{
	static const unsigned long max = std::numeric_limits<int>::max();
	unsigned long tmp;
	if( !try_parse(in, tmp) || tmp > max )
		return def;
	return unsigned(tmp);
}

namespace {
bool find(string_view s, string_view subs)
{
	return s.find(subs) != s.npos;
}
} // namespace

#include <aw/utility/string/case.h>
template<>
bool parse(string_view s, bool def)
{
	static const/*expr*/ size_t longest = 5;

	auto nonspace = [] (char c) { return !std::isspace(c); };

	auto in = std::string(s);
	aw::string::tolower( in );

	if (in.empty()) return def;
	if (find(in,"true"))  return true;
	if (find(in,"false")) return false;

	unsigned long val = parse<unsigned long>(s, def);
	if (val == 1) return true;
	if (val == 0) return false;
	return def;
}
