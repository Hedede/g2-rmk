/*
   The sense of "Temporary values" is probably, that the developers
   thought of an "apply" button. If that "apply" button is used,
   temporary values would have been made persistent.

   Doesn't seem like this feature is used anywhere.
 */

zSTRING[] zOPTION_PATHNAMES = {
	"DIR_SYSTEM", "DIR_WEB", "DIR_SAVEGAMES", "DIR_DATA", "DIR_ANIMS",
	"DIR_CUTSCENES", "DIR_OUTPUTUNITS", "DIR_MESHES", "DIR_SCRIPTS",
	"DIR_SOUND", "DIR_VIDEO", "DIR_MUSIC", "DIR_TEX", "DIR_TEX_DESKTOP",
	"DIR_WORLD", "DIR_PRESETS", "DIR_TOOLS_DATA", "DIR_COMPILED_ANIMS",
	"DIR_COMPILED_MESHES", "DIR_COMPILED_SCRIPTS", "DIR_COMPILED_TEXTURES",
	"DIR_TOOLS_CONFIG", "SUBDIR_INTERN", "", "DIR_ROOT", "DIR_EXECUTABLE",
}

enum zTOptionPaths {
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
	_UNKNOWN_,
	DIR_ROOT,
	DIR_EXECUTABLE,
};

using EntryChangeHanlder = int (*)(struct zCOptionEntry *);

class zCOptionSection {
	zCOptionSection(zSTRING const& secName)
		: secName(secName);
	{
		secName.TrimLeft(' ');
		secName.TrimRight(' ');
		secName.TrimLeft('[');
		secName.TrimRight(']');
		secName.Upper();
	}

	~zCOptionSection()
	{
		for (auto& entry : entryList) {
			delete entry;
			entry = 0;
		}
	}

	//name of the section. For example "GAME"
	zSTRING secName;
	//the entries in this section.
	zCArray<zCOptionEntry*> entryList;
};

class zCOptionEntry {
	zCOptionEntry(zSTRING const& name, zSTRING const& value) // _name, _value
		: varName(name), varValue(value)
	{
		varName.TrimLeft(' ');
		varName.TrimRight(' ');
		varValue.TrimLeft(' ');
		varValue.TrimRight(' ');
		varValueTemp = varValue;
	}

	~zCOptionEntry() = default;

	void zCOptionEntry::SetValue(zSTRING s, zBOOL temp)
	{
		varValueTemp = s;
		if ( !temp )
			varValue = s;
	}

	zBOOL changed = 0;            //unused?

	//engine functions that want to be informed, whenever this option changes.
	zCArray <EntryChangeHandler> ccbList;

	//name of the variable
	zSTRING varName;
	//value of the variable (everything is converted to a string)
	zSTRING varValue;
	//temporary value of the variable. See above. Useless as far as I can see.
	zSTRING varValueTemp;
	//flags of the variable. The engine doesn't seem to care about these flags however.
	int varFlag = 0;
};

const int NUM_ENTRIES = 26;

class zCOption {
public:
	zCOption();

	static zCOptionEntry* GetEntry(zCOptionSection* section, int index)
	{
		if (!section)
			return nullptr;

		if (index < 0 || index > section->entryList.GetNumInList())
			return nullptr;

		return section->entryList.GetSafe(index);
	}

	static uint32_t GetNumEntries(zCOptionSection* section)
	{
		if (section)
			return section->entryList.GetNumInList();
		return 0;
	}

	virtual ~zCOption();

	void ChangeDir(zTOptionPaths dirId);

	zSTRING& GetDirString(zTOptionPaths dirId)
	{
		return dir_string[dirId];
	}

	void AddParameters(zSTRING params);
	int Parm(zSTRING const& parm);

	uint32_t GetNumSections() const
	{
		return sectionList.GetNumInList();
	}

	zCOptionSection* GetSection(int index)
	{
		if (index < 0 || index > sectionList.GetNumInList())
			return nullptr;
		
		return sectionList.GetSafe(index);
	}


	int ReadRaw(zSTRING const& secName, char const* optName, char *& buffer,
	            char* defBuffer, int defSize);

	/*
	int ReadRaw(zSTRING const& secName, zSTRING const& name, void* buffer, int max_size)
	{
		return ReadRaw(secName, name, buffer, max_size);
	}*/

private:
	//read "temporary" value instead of normal value.
	zBOOL readTemp = 1;

	//an ini files consists of sections
	zCArray<zCOptionSection*> sectionList;

	zFILE* directory[NUM_ENTRIES];
	zSTRING dir_string[NUM_ENTRIES];

	//zCOptions is responsible for the command line as well.
	zSTRING commandline;
};

void zInitOptions();
void zDeinitOptions();
