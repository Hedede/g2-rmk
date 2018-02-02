#include <Hook/type_info.h>
#include <Hook/Externals.h>
#include <Sound/SoundOpenAL.h>
#include <Sound/zCSndSys_OpenAL.h>

namespace {
struct OALTypeDescriptor : TypeDescriptor {
	const char name[22];
};

OALTypeDescriptor oal_td {
	reinterpret_cast<const void*>(0x843C68),
	0,
	".?AVzCSndSys_OpenAL@@"
};

RTTIBaseClassDescriptor oal_bcd {
	&oal_td, 1, 0, -1, 0, 0
};

auto& snd_bcd = Value<RTTIBaseClassDescriptor>(0x846C80);

RTTIBaseClassDescriptor* oal_bca[] = {
	&oal_bcd,
	&snd_bcd,
	0
};

RTTIClassHierarchyDescriptor oal_hd {
	0, 0, 2, +oal_bca
};
RTTICompleteObjectLocator oal_rtti {
	0, 0, 0, &oal_td, &oal_hd
};

struct TypeInfo {
	static auto oal(zCSoundSystem* ss) { return static_cast<zCSndSys_OpenAL*>(ss); };
	TypeInfo(zCSoundSystem_vt const& proto)
		: vt{proto}
	{
		g2::Log("OpenAL", "initializing vtable");
		vt.LoadSoundFX =
		[] (zCSoundSystem* ss, zSTRING const& name) __thiscall -> zCSoundFX*
		{
			return oal(ss)->LoadSoundFX(std::string(name));
		};

		vt.LoadSoundFXScript =
		[] (zCSoundSystem* ss, zSTRING const& name) __thiscall -> zCSoundFX*
		{
			return oal(ss)->LoadSoundFXScript(std::string(name));
		};

		vt.PlaySound2 =
		[] (zCSoundSystem* ss, zCSoundFX* sfx, int slot) __thiscall -> void
		{
			if (sfx)
				oal(ss)->PlaySound(*sfx, slot);
			else
				g2::Warning("OpenAL", "Chto za bodyagoo ty preevolok?");
		};

		vt.PlaySound3D2 =
		[] (zCSoundSystem* ss, zCSoundFX* sfx, zCVob* source, int slot, zTSound3DParams* params) __thiscall -> zCActiveSnd*
		{
			oal(ss)->PlaySound3D(sfx, source, slot, params);
			return nullptr;
		};
		g2::Log("OpenAL", "vtable initialized");
	}
	RTTICompleteObjectLocator* rtti = &oal_rtti;
	zCSoundSystem_vt vt;
};
} // end of typeinfo

#include <Filesystem.h>
#include <Gothic/Script/zParser.h>
#include <Gothic/Game/zOptions.h>

#include "zCSndFX_OpenAL.h"

zCSndSys_OpenAL::zCSndSys_OpenAL()
	: zCSoundSystemDummy()
{
	g2::Log("OpenAL", "creating sound system");
	static const TypeInfo tinfo{*vtab};
	vtab = &tinfo.vt;

	static_assert( sizeof(g2::SoundOpenAL)  <= sizeof(data)  );
	static_assert( alignof(g2::SoundOpenAL) <= alignof(data) );
	new (&data) g2::SoundOpenAL;

	g2::Log("OpenAL", "creating sfx parser");
	sfx_parser.reset( new zCParser );

	std::string sfx_path;
	if (zgameoptions)
		sfx_path = zgameoptions->ReadString("FILES", "SoundEffects", "");
	if (sfx_path.empty())
		sfx_path = "System/Sfx";

	sfx_parser->Parse( sfx_path + ".src" );
	sfx_parser->CreatePCode();
	sfx_parser->CheckClassSize( "C_SFX", sizeof(C_SFX) );
	sfx_parser->CheckClassSize( "C_SNDSYS_CFG", 40 );
}

zCSndSys_OpenAL::~zCSndSys_OpenAL()
{
	impl().~SoundOpenAL();
}

#include <Types/string_view.h>
#include <aw/utility/string/case.h>
constexpr string_view default_wave      = "default.wav"sv;
constexpr string_view non_scirpt_prefix = "%"sv;
zCSoundFX* zCSndSys_OpenAL::LoadSoundFX(std::string name)
{
	if (name.empty())
		name = default_wave;
	else
		aw::string::tolower(name); // TODO: handle case-sensitive FS
	g2::Log("SFX", "LoadSoundFX request: " + name);
	fs::path path{name};
	if (path.extension().generic_u8string() == ".wav")
	{
		auto sfx = new zCSndFX_OpenAL;
		sfx->sound.file = name.data();
		sfx->SetObjectName(non_scirpt_prefix + name);
		return sfx;
	}

	return nullptr;
}


zCSoundFX* zCSndSys_OpenAL::LoadSoundFXScript(std::string name)
{
	if (name.empty()) {
		g2::Warning("SFX", "SFX script name is empty!");
		return nullptr;
	}

	g2::Log("SFX", "LoadSoundFXScript request: " + name);
	aw::string::toupper(name);

	auto sym = sfx_parser->GetSymbol(name);
	if (!sym) {
		g2::Warning("SFX", "SFX script not found! " + name);
		return nullptr;
	}

	C_SFX sfx;
	sfx_parser->CreateInstance(name, &sfx);

	return LoadSoundFX( (std::string)sfx.file );
}

using namespace std::string_literals;
void zCSndSys_OpenAL::PlaySound(zCSoundFX& sfx, int slot)
{
	sfx.CacheIn( -1.0 );
	auto& osfx = static_cast<zCSndFX_OpenAL&>(sfx);
	//osfx.LoadResourceData();

	g2::Log("SFX", "Playing wave "s + osfx.sound.file.Data());
	impl().play( osfx.source );
}

void zCSndSys_OpenAL::PlaySound3D(zCSoundFX* sfx, zCVob *, int slot, zTSound3DParams *)
{
	if (sfx) {
		auto& osfx = static_cast<zCSndFX_OpenAL&>(*sfx);
		g2::Log("SFX", "Playing sfx "s + osfx.sound.file.Data());
		PlaySound(*sfx, slot);
	}
}
