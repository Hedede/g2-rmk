#include <string>
#include <string_view>
using std::string_view;
using namespace std::string_view_literals;

inline std::string operator+( string_view sv, std::string const& str )
{
	return std::string{sv} + str;
}

inline std::string operator+( std::string str, string_view sv )
{
	str += sv;
	return str;
}
