#include "zCSndFX_OpenAL.h"
#include <Sound/zCSndSys_OpenAL.h>

#include <Gothic/Game/zOptions.h>

#include <Hook/type_info.h>
#include <Hook/Externals.h>
#include <Sound/SoundOpenAL.h>
#include <Types/string_view.h>
#include <aw/utility/string/case.h>

#include <Types/string_view.h>
#include <aw/io/input_file_stream.h>
#include <aw/fileformat/wav/reader.h>
#include <Filesystem/directory_config.h>
#include <Filesystem.h>

namespace {
struct TypeInfo {
	TypeInfo(zCSoundFX_vt const& proto)
		: vt{proto}
	{
		g2::Log("zCSndFX_OpenAL", "initializing vtable");
		vt.LoadResourceData =
		[] (zCSoundFX* sfx) __thiscall -> void
		{
			reinterpret_cast<zCSndFX_OpenAL*>(sfx)->LoadResourceData();
		};
		g2::Log("zCSndFX_OpenAL", "vtable initialized");
	}
	zCSoundFX_vt vt;
};
} // namespace

zCSndFX_OpenAL::zCSndFX_OpenAL()
	: zCSoundFX{}
{
	static TypeInfo tinfo{*zCSoundFX::vtab()};
	_vtab = static_cast<void*>(&tinfo.vt);
}

void zCSndFX_OpenAL::LoadResourceData()
{
	using namespace g2;
	g2::Log("SFX", "Loading wave " + (std::string)sound.file);

	auto result = find_file_recursive( directory_config[dir::sound], sound.file);
	if (!result) {
		g2::Log("SFX", "Couldn't find SFX file " + (std::string)sound.file);
		return;
	}

	using namespace std::string_literals;
	auto path = *result;

	aw::io::input_file_stream file{ path };
	if (!file.is_open()) {
		g2::Warning("SFX", "Can't open file " + path.generic_u8string());
		return;
	}

	auto wav = aw::wav::read( file );
	if (!wav) {
		g2::Warning("SFX", "Failed to load sound " + path.generic_u8string());
		return;
	}

	buffer.set_data( *wav );
	source.set_buffer( buffer );
}
