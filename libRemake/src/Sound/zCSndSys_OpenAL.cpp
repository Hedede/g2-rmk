#include <limits>
#include <Hook/type_info.h>
#include <Hook/Externals.h>

#include <Sound/zCSndSys_OpenAL.h>

#include "SoundOpenAL.h"

//#include <aw/utility/string/lazy_split.h>

//------------------------------------------------------------------------------
constexpr zTSndHandle invalid_snd_handle = zTSndHandle(-1);

//------------------------------------------------------------------------------
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
		[] (zCSoundSystem* ss, zCSoundFX* sfx, int slot) __thiscall -> zTSndHandle
		{
			if (sfx)
				return oal(ss)->PlaySound(*sfx, slot);
			g2::Warning("OpenAL", "Chto za bodyagoo ty preevolok?");
			return invalid_snd_handle;
		};

		vt.PlaySound3D2 =
		[] (zCSoundSystem* ss, zCSoundFX* sfx, zCVob* source, int slot, zTSound3DParams* params) __thiscall -> zTSndHandle
		{
			return oal(ss)->PlaySound3D(sfx, source, slot, params);
		};

		vt.StopSound =
		[] (zCSoundSystem* ss, const zTSndHandle& handle) __thiscall {
			oal(ss)->StopSound( handle );
		};

		vt.UpdateSound3D =
		[] (zCSoundSystem* ss, const zTSndHandle& handle, zTSound3DParams* params) __thiscall
		{
			oal(ss)->UpdateSound3D(handle, params);
		};

		vt.SetListener = 
		[] (zCSoundSystem* ss, zCVob* vob) __thiscall
		{
			oal(ss)->SetListener(vob);
		};

		vt.DoSoundUpdate = 
		[] (zCSoundSystem* ss) __thiscall
		{
			oal(ss)->DoSoundUpdate();
		};


		g2::Log("OpenAL", "vtable initialized");
	}
	RTTICompleteObjectLocator* rtti = &oal_rtti;
	zCSoundSystem_vt vt;
};
} // end of typeinfo

//------------------------------------------------------------------------------

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

	// FIXME
	origin_vob.resize( impl().attributes().max_sources );
}

zCSndSys_OpenAL::~zCSndSys_OpenAL()
{
	impl().~SoundOpenAL();
}

#include <Utility/String.h>
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
zTSndHandle zCSndSys_OpenAL::AllocateHandle(zCSndFX_OpenAL& sfx)
{
	sfx.CacheIn( -1.0 );
	//osfx.LoadResourceData();

	string_view sv = sfx.sound.file;
	g2::Log("SFX", "Handle requested for sfx "s, sv);

	/* FIXME: better ryr */
	int priority = 0;
	if (has_prefix( sv, "dia_" ))
		++priority;

	g2::Source source( impl().pool.request_source(0) );
	source.set_buffer( sfx.buffer );
	return (zTSndHandle)source.handle;
}

zTSndHandle zCSndSys_OpenAL::PlaySound(zCSoundFX& sfx, int slot)
{
	auto& osfx = static_cast<zCSndFX_OpenAL&>(sfx);
	auto handle = AllocateHandle(osfx);

	string_view file = osfx.sound.file;
	g2::Log("SFX", "Playing wave "s, file);

	auto src = ALuint(handle);
	SetSoundParams( handle );
	alSourcePlay( src );

	return handle;
}

void zCSndSys_OpenAL::SetSoundParams(zTSndHandle handle)
{
	// TODO: add a pool for 2D sources so this isn't needed to be done?
	auto src = ALuint(handle);
	// Reset all 3D parameters
	alSourcei( src, AL_SOURCE_RELATIVE, AL_TRUE );
	alSource3f(src, AL_POSITION, 0,10,0);
	alSource3f(src, AL_DIRECTION, 0,0,0);
	alSourcef(src, AL_MAX_DISTANCE, std::numeric_limits<float>::max());
	alSourcef(src, AL_REFERENCE_DISTANCE,100);
	alSourcef(src, AL_CONE_INNER_ANGLE, 360.0 );
	alSourcef(src, AL_CONE_OUTER_ANGLE, 360.0 );
}

void zCSndSys_OpenAL::SetSound3DParams(zTSndHandle handle, zTSound3DParams& params)
{
	auto src = ALuint(handle);
	alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
	alSourcef(src,AL_MAX_DISTANCE,params.radius);
	alSourcef(src,AL_REFERENCE_DISTANCE,params.radius*0.3);
	float cone = (params.coneAngle != 0) ? params.coneAngle : 360.0;
	alSourcef(src,AL_CONE_INNER_ANGLE, 360.0 - cone);
	alSourcef(src,AL_CONE_OUTER_ANGLE, cone);

	if (params.__obstruction > 0.0) {
		static ALuint filter = [] {
			ALuint filter;
			alGenFilters(1, &filter);
			alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter, AL_LOWPASS_GAIN, 0.7);
			alFilterf(filter, AL_LOWPASS_GAINHF, 0.3);
			return filter;
		}();

		alSourcei(src, AL_DIRECT_FILTER, filter);
	} else {
		alSourcei(src, AL_DIRECT_FILTER, AL_FILTER_NULL);
		alSource3i(src, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
	}
}

void zCSndSys_OpenAL::SetSoundDefault3DParams(zTSndHandle handle)
{
	auto src = ALuint(handle);
	alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
	alSourcef(src,AL_MAX_DISTANCE,3500.0);
	alSourcef(src,AL_REFERENCE_DISTANCE,1200.0);
}

#include <Gothic/Game/zVob.h>
zTSndHandle zCSndSys_OpenAL::PlaySound3D(zCSoundFX* sfx, zCVob* source, int slot, zTSound3DParams* params)
{
	if (sfx) {
		auto& osfx = static_cast<zCSndFX_OpenAL&>(*sfx);
		auto handle = AllocateHandle(osfx);
		
		if (handle != invalid_snd_handle)
			origin_vob[ size_t(handle) ] = source;

		//UpdateSound3D( handle, params );
		auto src = ALuint(handle);
		auto pos = source->GetPositionWorld();
		auto dir = source->GetAtVectorWorld();
		alSource3f(src,AL_POSITION,pos.x,pos.y,pos.z);
		alSource3f(src,AL_DIRECTION,dir.x,dir.y,dir.z);
		if (params)
			SetSound3DParams( handle, *params );
		else
			SetSoundDefault3DParams( handle );

		string_view file = osfx.sound.file;
		g2::Log("SFX", "Playing sfx (3D) "s + osfx.sound.file);
		alSourcePlay(src);

		return handle;
	}

	return invalid_snd_handle;
}

void zCSndSys_OpenAL::StopSound(zTSndHandle handle)
{
	alSourceStop( ALuint(handle) );
}

void zCSndSys_OpenAL::UpdateSound3D(zTSndHandle handle, zTSound3DParams* params)
{
	auto vob = origin_vob[ size_t(handle) ];
	if (vob) {
		auto on = vob->GetObjectName();
		if (!on) return;
		g2::Log("SFX", "UpdateSound3D by vob: "s + on.Data());
		auto pos = vob->GetPositionWorld();
		auto dir = vob->GetAtVectorWorld();
		if (params) SetSound3DParams( handle, *params );

		auto src = ALuint(handle);
		alSource3f(src,AL_POSITION,pos.x,pos.y,pos.z);
		alSource3f(src,AL_DIRECTION,dir.x,dir.y,dir.z);

		g2::Log("SFX", "UpdateSound3D pos: ", pos.x,',',pos.y,',',pos.z );
		g2::Log("SFX", "Distance: ", (pos - listener->GetPositionWorld()).Length() );
	}
}

// TODO: move to listener
void zCSndSys_OpenAL::UpdateListener()
{
	if (!listener) {
		g2::Log("SFX", "No listener...");
		return;
	}

	auto pos = listener->GetPositionWorld();
	alListener3f(AL_POSITION,  pos.x, pos.y, pos.z );
	g2::Log("SFX", "Listener pos: ", pos.x,',',pos.y,',',pos.z );

	auto at = listener->GetAtVectorWorld();
	auto up = listener->GetUpVectorWorld();
	ALfloat ori[] = { at.x, at.y, at.z, up.x, up.y, up.z };
	alListenerfv(AL_ORIENTATION, ori);
}

void zCSndSys_OpenAL::SetListener(zCVob* vob)
{
	if (!vob) {
		listener = nullptr;
		return;
	}

	auto nameb = vob->GetObjectName();
	g2::Log("SFX", "New listener: "s + nameb);

	// TODO: release and stuff
	listener = vob;
	UpdateListener();
}

void zCSndSys_OpenAL::DoSoundUpdate()
{
	g2::Log("SFX", "Doing sound update...");
	UpdateListener();
}
