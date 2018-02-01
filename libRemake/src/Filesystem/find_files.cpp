#include <Filesystem.h>

#include <Logging/Log.h>

#include <aw/utility/unicode/convert.h>
#include <aw/utility/string/split.h>
#include <aw/utility/string/case.h>
#include <aw/utility/on_scope_exit.h>

#include <experimental/filesystem>

#include <windows.h>

using namespace aw::unicode;

/*
 * This is code from Havogrek.
 * fs::recursive_directory_iterator is kinda slow on mingw32,
 * in part because of SJLJ eh
 */
namespace {
enum DirentType {
	ENT_FILE,
	ENT_DIRECTORY
};

struct DirectoryEntry {
	DirentType   type;
	std::wstring name;
};

DirectoryEntry ConvertToDirent(_WIN32_FIND_DATAW const& ffd)
{
	DirectoryEntry e;
	if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		e.type = ENT_DIRECTORY;
	else
		e.type = ENT_FILE;
	e.name = ffd.cFileName;
	return e;
}

template<typename DirentFilter>
bool FindFileRecursiveW( std::wstring const& path, DirentFilter& Filter, DirectoryEntry& out )
{
	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW( (path + L"/*").data(), &ffd );
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	aw::on_scope_exit guard( [&] { FindClose(hFind); } );

	do {
		DirectoryEntry dent = ConvertToDirent(ffd);
		std::wstring dpath = path + L'/' + dent.name;
		if (dent.type == ENT_DIRECTORY) {
			if (dent.name == L"." || dent.name == L"..")
				continue;
			if (FindFileRecursiveW(dpath, Filter, out))
				return true;
		}
		if (Filter(dent)) {
			dent.name = dpath;
			out = dent;
			return true;
		}
	} while(FindNextFileW(hFind, &ffd));
	return false;
}

std::string DirentName( DirectoryEntry const& ent )
{
	return narrow( ent.name );
}

bool IsFile(DirectoryEntry const& ent) { return ent.type == ENT_FILE; }
bool IsDir(DirectoryEntry const& ent)  { return ent.type == ENT_DIRECTORY; }
} // namespace

namespace g2 {

std::optional<fs::path> find_file_recursive( string_view path, string_view mask )
{
	auto name = widen(mask);
	auto filter = [&] (DirectoryEntry& ent) {
		if (ent.type != ENT_FILE)
			return false;
		auto fname = aw::string::tolower(ent.name);
		if (fname == name)
			g2::Log("ZOROZO", narrow(ent.name) + " " + std::string(mask));
		return fname == name;
	};
	DirectoryEntry dent;
	if (FindFileRecursiveW(widen(path), filter, dent)) {
		g2::Log("ZOROZO", narrow(dent.name));
		return dent.name;
	}
	return {};
}


std::optional<fs::path> find_file_recursive_std(string_view dir, string_view name)
try {
	for (fs::path const& path : fs::recursive_directory_iterator{ dir }) {
		auto file = path.filename().generic_u8string();
		aw::string::tolower( file );
		if (file == name)
			return path;
	}
	return {};
} catch( fs::filesystem_error& e ) {
	g2::Error("SFX", e.what());
	g2::Error("SFX", dir);
	g2::Error("SFX", e.path1().generic_u8string());
	g2::Error("SFX", e.path2().generic_u8string());
	return {};
}
} //namespace g2
