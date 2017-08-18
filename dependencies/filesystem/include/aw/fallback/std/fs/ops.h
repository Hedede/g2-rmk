/*
 * Copyright (C) 2016  Hedede <hededrk@gmail.com>
 *
 * License LGPLv3 or later:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifdef aw_utility_fallback_filesystem_h
#include <system_error>

namespace awstd {
namespace filesystem {

struct filesystem_error : std::system_error {
	filesystem_error( std::string const& what_arg, std::error_code ec )
		: system_error{ec, what_arg}
	{}

	filesystem_error( std::string const& what_arg,
	                  path const& p1,
	                  std::error_code ec )
		: system_error{ec, what_arg}, p{p1}
	{}

	filesystem_error( std::string const& what_arg,
	                  path const& p1,
	                  path const& p2,
	                  std::error_code ec )
		: system_error{ec, what_arg}, p{p1, p2}
	{}

	path const& path1() const { return p[0]; }
	path const& path2() const { return p[1]; }

	char const* what() const noexcept { return what_str.c_str(); }

private:
	std::string what_str = [this] {
		std::string str = std::system_error::what();
		if (!path1().empty())
			str += " " + path1().string();
		if (!path2().empty())
			str += " " + path2().string();
		return str;
	}();
	path p[2];
};


namespace _impl {
namespace win32 {
uintmax_t   file_size( path const& p, std::error_code* ec = nullptr );
bool        remove( path const& p,    std::error_code* ec = nullptr );
file_status status( path const& p,    std::error_code* ec = nullptr );
} // win32
using win32::file_size;
using win32::remove;
using win32::status;

inline bool exists( path const& p, std::error_code* ec) noexcept
{
	auto s = _impl::status(p, ec);
	if (status_known(s))
		ec->clear();
	return exists(s);
}
} // _impl

inline auto file_size(path const& p, std::error_code& ec) noexcept
{
	return _impl::file_size(p, &ec);
}

inline auto file_size(path const& p)
{
	return _impl::file_size(p);
}

inline auto remove(path const& p, std::error_code& ec) noexcept
{
	return _impl::remove(p, &ec);
}

inline auto remove(path const& p)
{
	return _impl::remove(p);
}

inline bool exists( path const& p, std::error_code& ec) noexcept
{
	return _impl::exists(p, &ec);
}

inline bool exists( path const& p ) noexcept
{
	return _impl::exists(p, nullptr);
}
} // namespace filesystem
} // namespace awstd
#endif//aw_utility_fallback_filesystem_h
