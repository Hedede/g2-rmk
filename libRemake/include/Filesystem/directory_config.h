#pragma once
#include <Filesystem.h>
#include <Types/string_view.h>
#include <aw/types/enum.h>

namespace g2 {
enum class dir {
	system,
	web,
	savegames,
	data,
	anims,
	cutscenes,
	output_units,
	meshes,
	scripts,
	sound,
	video,
	music,
	tex,
	tex_desktop,
	world,
	presets,
	tools_data,
	compiled_anims,
	compiled_meshes,
	compiled_scripts,
	compiled_textures,
	tools_config,
	intern,
	root,
	executable,
	count
};

inline dir& operator++(dir& d) { d = dir(aw::underlying(d) + 1); return d; }

constexpr size_t dir_count = aw::underlying(dir::count);

struct {
	fs::path& operator[]( dir idx )
	{
		return dirs[ aw::underlying(idx) ];
	}

private:
	fs::path dirs[ dir_count ];
} directory_config;

void read_directory_config( [[maybe_unused]] string_view );

} // namespace g2
