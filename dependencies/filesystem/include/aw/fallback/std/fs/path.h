/*
 * Copyright (C) 2016  Hedede <hededrk@gmail.com>
 *
 * License LGPLv3 or later:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifdef aw_utility_fallback_filesystem_h
#include <string>
#include <string_view>

namespace awstd {
namespace filesystem {
struct path {
#if (AW_PLATFORM == AW_PLATFORM_WIN32)
	static constexpr char preferred_separator = '\\';
#else
	static constexpr char preferred_separator = '/';
#endif

	path() {}
	path(path const& other) : p(other.p) {}
#if __cplusplus >= 201103L
	path(path&& other) : p{std::move(other.p)} {}
#endif

	path(std::string_view source)
		: p(std::string(source))
	{ }

	path(std::string const& source)
		: p(source)
	{ }

	path(std::wstring const& source);

	template<class CharT, size_t N>
	path(CharT const (&source)[N])
		: path(std::basic_string<CharT>(source))
	{ }

	path& swap(path& other)
	{
		p.swap(other.p);
		return *this;
	}

	path& make_preferred();

	path filename() const  { return filename_view(); }
	//! Filename without extension: "file.txt" -> "file"
	path stem() const      { return stem_view(); }
	//! Extract extension: "file.txt" -> ".txt"
	path extension() const { return extension_view(); }

	std::string generic_string() const;
	std::string generic_u8string() const;

	std::string string()   const  { return p; }
	std::string u8string() const  { return p; }

	std::wstring wstring() const;

	std::string generic() const { return generic_u8string(); }
	std::wstring native() const;

	bool empty() const { return p.empty(); }

protected:
	std::string_view path_view() const;
	std::string_view filename_view() const;
	std::string_view stem_view() const;
	std::string_view extension_view() const;

private:
	std::string p;
};
} // namespace filesystem
} // namespace awstd
#endif//aw_utility_fallback_filesystem_h
