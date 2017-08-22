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
	DIR_NUM_ENTRIES,
	DIR_ROOT,
	DIR_EXECUTABLE,
	DIR_TOTAL_ENTRIES
};

struct zCOptionEntry {

};

struct zFILE;
struct zCOptions {
	zCOptions()
	{
		Thiscall<void(zCOptions*)> ctor{0x460350};
		ctor(this);

	}

	void Init(std::string const& cmdLine)
	{
		Thiscall<void(zCOptions*, zSTRING, int)> Init{0x463C20};
		Init(this, cmdLine, 1);
	}

	bool RemoveEntry(std::string const& sec, std::string const& ent)
	{
		Thiscall<int(zCOptions*, const zSTRING&, const char*)> call{0x462A70};
		return call(this, zSTRING{sec}, ent.data());
	}

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

	void AddParameters(std::string const& params)
	{
		Thiscall<void(zCOptions*, zSTRING)> call{0x463B00};
		call( this, zSTRING{params} );
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

	auto ReadString(std::string const& sec, std::string const& opt, const char* defval) -> std::string
	{
		zSTRING tmp;
		Thiscall<zSTRING*(zCOptions*, zSTRING*, zSTRING const&, const char*, const char*)> call{0x4627E0};
		zSTRING* ptr = call(this, &tmp, zSTRING{sec}, opt.data(), defval);
		return std::string(tmp);;
	}

	int ReadInt(std::string const& sec, std::string const& opt, int defval)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, int)> call{0x462390};
		return call(this, zSTRING{sec}, opt.data(), defval);
	}

	unsigned ReadDWord(std::string const& sec, std::string const& opt, unsigned defval)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, unsigned)> call{0x4624F0};
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

	bool WriteInt(std::string const& sec, std::string const& opt, int value, bool temp)
	{
		WriteString(sec, opt, std::to_string(value), temp);
	}

	bool WriteReal(std::string const& sec, std::string const& opt, float value, bool temp)
	{
		Thiscall<int(zCOptions*, zSTRING const&, const char*, float, int)> call{0x461F90};
		return call(this, zSTRING{sec}, opt.data(), value, temp);
	}

	bool WriteBool(std::string const& sec, std::string const& opt, bool value, bool temp)
	{
		WriteString(sec, opt, value ? "1" : "0", temp);
	}

	bool Load(std::string const& filename)
	{
		Thiscall<int(zCOptions*, zSTRING filename)> call{0x4607B0};
		return call(this, zSTRING{filename});
	}

	bool Save(std::string const& filename)
	{
		Thiscall<int(zCOptions*, zSTRING)> func{0x4616C0};
		return func(this, filename);
	}

private:
	void* vtable;
	zBOOL readTemp;

	zCOptionEntry **array;
	int numAlloc;
	int numInArray;
	
	zFILE*  directory[DIR_TOTAL_ENTRIES];
	zSTRING dir_string[DIR_TOTAL_ENTRIES];
	zSTRING commandline;
};

#include <Hook/size_checker.h>
CHECK_SIZE(zCOptions, 0x298);

static auto& zoptions     = Value<zCOptions*>(0x8CD988);
// TODO: rename to modoptions?
static auto& zgameoptions = Value<zCOptions*>(0x8CD98C);

#endif//GOTHIC_ZOPTIONS_H
