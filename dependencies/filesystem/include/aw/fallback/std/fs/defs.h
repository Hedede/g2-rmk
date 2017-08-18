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
#include <aw/types/enum.h>

namespace awstd {
namespace filesystem {

/* I "stole" those enums from libcxx, so thanks to them */
enum class file_type : signed char {
	none       = 0,
	not_found  = -1,
	regular    = 1,
	directory  = 2,
	symlink    = 3,
	block      = 4,
	character  = 5,
	fifo       = 6,
	socket     = 7,
	unknown    = 8
};

enum class perms : unsigned {
	none         = 0,
	unknown      = 0xFFFF,

	owner_read   = 0400,
	owner_write  = 0200,
	owner_exec   = 0100,
	owner_all    = 0700,

	group_read   = 040,
	group_write  = 020,
	group_exec   = 010,
	group_all    = 070,

	others_read  = 04,
	others_write = 02,
	others_exec  = 01,
	others_all   = 07,

	all          = 0777,

	set_uid      = 04000,
	set_gid      = 02000,
	sticky_bit   = 01000,
	mask         = 07777,

	add_perms        = 0x10000,
	remove_perms     = 0x20000,
	symlink_nofollow = 0x40000
};

constexpr perms operator&(perms a, perms b) { return perms(aw::underlying(a) & aw::underlying(b)); }
constexpr perms operator|(perms a, perms b) { return perms(aw::underlying(a) | aw::underlying(b)); }
constexpr perms operator^(perms a, perms b) { return perms(aw::underlying(a) ^ aw::underlying(b)); }

constexpr perms operator~(perms a) { return perms(~aw::underlying(a)); }

inline perms& operator&=(perms& a, perms b) { return a = a & b; }
inline perms& operator|=(perms& a, perms b) { return a = a | b; }
inline perms& operator^=(perms& a, perms b) { return a = a ^ b; }


struct file_status {
	file_status() noexcept = default;
	explicit file_status(file_type ft, perms prms = perms::unknown) noexcept
		: ft{ft}, prms{prms}
	{}

	~file_status() = default;

	file_status(const file_status&) noexcept = default;
	file_status(file_status&&)      noexcept = default;

	file_status& operator=(const file_status&) noexcept = default;
	file_status& operator=(file_status&&)      noexcept = default;

	file_type type()        const noexcept { return ft; }
	perms     permissions() const noexcept { return prms; }

	void type(file_type ft)      noexcept { this->ft = ft; }
	void permissions(perms prms) noexcept { this->prms = prms; }

private:
	file_type ft = file_type::none;
	perms prms   = perms::unknown;
};

inline bool status_known(file_status s) noexcept
{
	return s.type() != file_type::none;
}

inline bool exists( file_status s ) noexcept
{
	return status_known(s) && s.type() != file_type::not_found;
}
} // namespace filesystem
} // namespace awstd
#endif//aw_utility_fallback_filesystem_h
