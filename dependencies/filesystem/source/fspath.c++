#include <aw/fallback/std/filesystem>

#include <aw/utility/unicode/convert.h>
namespace awstd {
namespace filesystem {
namespace {
#if (AW_PLATFORM == AW_PLATFORM_WIN32)
constexpr std::string_view _path_separators("/\\",2);
#else
constexpr std::string_view _path_separators("/",1);
#endif

template <typename CharT>
bool is_path_separator(CharT c)
{
#if (AW_PLATFORM == AW_PLATFORM_WIN32)
	return c == CharT('/') || c == CharT('\\');
#else
	return c == CharT('/');
#endif
}

std::string& conv_generic(std::string& p)
{
#if __cplusplus >= 201103L
	for (char& c : p) {
		if (is_path_separator(c))
			c = '/';
	}
#else
	for (size_t i = 0, e = p.size(); i < e; ++i) {
		if (is_path_separator(p[i]))
			p[i] = '/';
	}
#endif
	return p;
}

template <typename CharT>
std::basic_string<CharT>& conv_preferred(std::basic_string<CharT>& p)
{
#if __cplusplus >= 201103L
	for (CharT& c : p) {
		if (is_path_separator(c))
			c = static_cast<CharT>(path::preferred_separator);
	}
#else
	for (size_t i = 0, e = p.size(); i < e; ++i) {
		if (is_path_separator(p[i]))
			p[i] = static_cast<CharT>(path::preferred_separator);
	}
#endif
	return p;
}
} // namespace

path::path(std::wstring const& source)
	: p{ aw::unicode::narrow(source) }
{}

path& path::make_preferred()
{
	conv_preferred(p);
	return *this;
}

std::string_view path::path_view() const
{
	return { p };
}

std::string_view path::filename_view() const
{
	std::string_view pv = path_view();
	size_t pos = pv.find_last_of(_path_separators);
	if (pos == pv.npos) return "";
	return pv.substr( pos + 1 );
}

namespace {
struct stem_and_ext {
	stem_and_ext(std::string_view fn) : stem(fn) {}
	stem_and_ext(std::string_view fn, std::string_view ext) : stem(fn), ext(ext) {}
	std::string_view stem;
	std::string_view ext;
};

stem_and_ext split_filename(std::string_view fn)
{
	if (fn.empty() || fn == "." || fn == "..")
		return stem_and_ext(fn);
	size_t pos = fn.rfind('.');
	if (pos == fn.npos)
		return stem_and_ext(fn);
	return stem_and_ext( fn.substr(0, pos), fn.substr(pos) );
}
} // namespace

std::string_view path::stem_view() const
{
	return split_filename(filename_view()).stem;
}

std::string_view path::extension_view() const
{
	return split_filename(filename_view()).ext;
}

std::string path::generic_string() const
{
	std::string s = string();
	return conv_generic(s);
}

std::string path::generic_u8string() const
{
	return generic_string();
}

std::wstring path::wstring() const
{
	return aw::unicode::widen(p);
}

std::wstring path::native() const
{
	std::wstring str = wstring();
	return conv_preferred(str);
}

} // namespace awstd
} // namespace filesystem
