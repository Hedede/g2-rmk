#include <Filesystem/directory_config.h>

#include <Gothic/Game/zOptions.h>

#include <aw/utility/string/case.h>
#include <aw/utility/ranges/value_range.h>
namespace g2 {
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
		directory_config[ dir ] = sv;
	}
}

} // namespace g2
