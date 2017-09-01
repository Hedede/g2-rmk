#include "zCSndFX_OpenAL.h"
#include "zCSndSys_OpenAL.h"

#include <Gothic/Game/zOptions.h>

#include <Hook/type_info.h>
#include <Hook/Externals.h>
#include <Sound/SoundOpenAL.h>
#include <Types/string_view.h>
#include <aw/utility/string/case.h>

#include <aw/io/input_file_stream.h>
#include <aw/fileformat/wav/reader.h>


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
	g2::Log("SFX", "Loading wave " + sound.file);

	// TODO: organize it such that it queried only once
	auto dir = zoptions->GetDirString(DIR_SOUND);
	for (auto& c : dir)
		if (c == '\\') c = '/';

	using namespace std::string_literals;
	auto path = (dir.data()+1) + "sfx/"s + (std::string)sound.file;
	aw::string::tolower(path);

	aw::io::input_file_stream file{ path };
	if (!file.is_open()) {
		g2::Warning("SFX", "Can't open file " + path);
		return;
	}

	auto wav = aw::wav::read( file );
	if (!wav) {
		g2::Warning("SFX", "Failed to load sound " + path);
		return;
	}

	buffer.set_data( *wav );
	source.set_buffer( buffer );

}
