#include <Logging/Log.h>
#include <Filesystem/directory_config.h>

#include <Gothic/Game/zOptions.h>

#include <aw/utility/string/case.h>
#include <aw/utility/ranges/value_range.h>
namespace g2 {

constexpr string_view dir_name[] {
	"system",
	"web",
	"savegames",
	"data",
	"anims",
	"cutscenes",
	"output_units",
	"meshes",
	"scripts",
	"sound",
	"video",
	"music",
	"tex",
	"tex_desktop",
	"world",
	"presets",
	"tools_data",
	"compiled_anims",
	"compiled_meshes",
	"compiled_scripts",
	"compiled_textures",
	"tools_config",
	"intern",
	"root",
	"executable"
};

static std::string dirs[ dir_count ];

string_view dir_config_accessor::operator[]( dir idx ) const
{
	return dirs[ aw::underlying(idx) ];
}

void read_directory_config( string_view )
{
	for (auto dir : aw::range(dir::count)) {
		auto str = zoptions->GetDirString( zTOptionPath(dir) );
		aw::string::tolower( str );
		string_view sv = str;
		if (!sv.empty()) {
			if (sv[0] == '\\')
				sv.remove_prefix(1);
		}

		auto idx =  aw::underlying(dir);
		dirs[ idx ] = fs::path(sv).generic_u8string();
		g2::Log( "FS", "Path[" + dir_name[ idx ] + "]: " + dirs[ idx ] );
	}
}

} // namespace g2
