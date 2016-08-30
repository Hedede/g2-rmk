#ifndef GOTHIC_ZOPTIONS_H
#define GOTHIC_ZOPTIONS_H
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>

enum zTOptionPath {
	DIR_SYSTEM,
	DIR_WEB,
	DIR_SAVEGAMES,
	DIR_DATA,
	DIR_ANIMS,
	DIR_CUTSCENES,
	DIR_OUTPUTUNITS,
	DIR_MESHES,
	DIR_SCRIPTS,
	DIR_SOUND,
	DIR_VIDEO,
	DIR_MUSIC,
	DIR_TEX,
	DIR_TEX_DESKTOP,
	DIR_WORLD,
	DIR_PRESETS,
	DIR_TOOLS_DATA,
	DIR_COMPILED_ANIMS,
	DIR_COMPILED_MESHES,
	DIR_COMPILED_SCRIPTS,
	DIR_COMPILED_TEXTURES,
	DIR_TOOLS_CONFIG,
	SUBDIR_INTERN,
	DIR_UNKNOWN_,
	DIR_ROOT,
	DIR_EXECUTABLE,
	DIR_NUM_ENTRIES
};

struct zCOptionEntry {

};

struct zCOptions {
	void ChangeDir(zTOptionPath dirId)
	{
		Thiscall<void(zCOptions*, zTOptionPath)> call{0x465160};
		call(this, dirId);
	}

	using ChangeHandler = int(*)(zCOptionEntry* optentry);
	void InsertChangeHandler(std::string const& sec, std::string name, ChangeHandler ccb)
	{
		Thiscall<void(zCOptions*,zSTRING const&,char const*,ChangeHandler)> call{0x0463310};

		call(this, zSTRING{sec}, name.data(), ccb);
	}

	bool Parm(std::string const& name)
	{
		Thiscall<int(zCOptions*, zSTRING const&)> call{0x465270};
		zSTRING tmp{name};
		return call(this, tmp);
	}

	auto ParmValue(std::string const& name) -> std::string
	{
		Thiscall<zSTRING*(zCOptions*,zSTRING*,zSTRING const&)> call{0x465380};

		zSTRING tmp{name};
		zSTRING ret;

		zSTRING* ptr = call(this, &ret, tmp);

		return std::string(ret);
	}

	int ReadInt(std::string const& sec, std::string const& opt, int defval)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, int)> call{0x462390};
		return call(this, zSTRING{sec}, opt.data(), defval);
	}

	float ReadReal(std::string const& sec, std::string const& opt, float defval)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, float)> call{0x462650};
		return call(this, zSTRING{sec}, opt.data(), defval);
	}

	bool ReadBool(std::string const& sec, std::string const& opt, bool defval)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, int)> call{0x462160};
		return call(this, zSTRING{sec}, opt.data(), defval);
	}

	bool WriteString(std::string const& sec, std::string const& opt, std::string const& value, bool temp)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, zSTRING, int)> call{0x461FD0};
		return call(this, zSTRING{sec}, opt.data(), zSTRING{value}, temp);
	}


	bool WriteBool(std::string const& sec, std::string const& opt, bool value, bool temp)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, int, int)> call{0x461DE0};
		return call(this, zSTRING{sec}, opt.data(), value, temp);
	}

	bool Save(std::string const& filename)
	{
		Thiscall<int(zCOptions*, zSTRING)> func{0x4616C0};
		return func(this, filename);
	}
};

static auto& zoptions     = Value<zCOptions*>(0x8CD988);
// TODO: rename to modoptions?
static auto& zgameoptions = Value<zCOptions*>(0x8CD98C);

#endif//GOTHIC_ZOPTIONS_H
