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
	DIR_NUM_ENTRIES,
	// These two appear to be special ones
	DIR_ROOT,
	DIR_EXECUTABLE,
	DIR_SIZE_TOTAL
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

zSTRING const zOPT_SEC_SOUND         = "SOUND"
zSTRING const zOPT_SEC_INTERNAL      = "INTERNAL";
zSTRING const zOPT_SEC_FILES         = "FILES";
zSTRING const zOPT_SEC_VIDEO         = "VIDEO";
zSTRING const zOPT_SEC_KEYS          = "KEYS";
zSTRING const zOPT_SEC_INFO          = "INFO";
zSTRING const zOPT_SEC_MUSIC         = "MUSIC";
zSTRING const zOPT_SEC_PERFORMANCE   = "PERFORMANCE";
zSTRING const zOPT_SEC_GAME          = "GAME";
zSTRING const zOPT_SEC_SETTINGS      = "SETTINGS"
zSTRING const zOPT_SEC_OPTIONS       = "OPTIONS";
zSTRING const zOPT_SEC_KEYS_DEFAULT1 = "KEYSDEFAULT1";
zSTRING const zOPT_SEC_KEYS_DEFAULT0 = "KEYSDEFAULT0";


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

zSTRING const ZOPT_SND_3DPROV            = "";
zSTRING const ZOPT_SND_SAMPLERATE        = "";
zSTRING const ZOPT_SND_USEREVERB         = "";
zSTRING const ZOPT_SND_SPEAKERTYPE       = "";
zSTRING const ZOPT_SND_SPEECH_VOL        = "";
zSTRING const ZOPT_SND_MUSIC_VOL         = "musicVolume";
zSTRING const ZOPT_SND_SFX_VOL           = "soundVolume";
zSTRING const ZOPT_GAMEKEY_RIGHT         = "";
zSTRING const ZOPT_GAMEKEY_ACTIONRIGHT   = "";
zSTRING const ZOPT_GAMEKEY_SLOW          = "";
zSTRING const ZOPT_GAMEKEY_LAME_HEAL     = "";
zSTRING const ZOPT_GAMEKEY_STRAFELEFT    = "";
zSTRING const ZOPT_GAMEKEY_INVENTORY     = "";
zSTRING const ZOPT_GAMEKEY_SCREEN_LOG    = "";
zSTRING const ZOPT_GAMEKEY_SCREEN_MAP    = "";
zSTRING const ZOPT_GAMEKEY_LOCK_TARGET   = "";
zSTRING const ZOPT_GAMEKEY_LOOK_FP       = "";
zSTRING const ZOPT_GAMEKEY_UP            = "";
zSTRING const ZOPT_GAMEKEY_DOWN          = "";
zSTRING const ZOPT_GAMEKEY_SMOVE         = "";
zSTRING const ZOPT_GAMEKEY_PARADE        = "";
zSTRING const ZOPT_JOY_ENABLE            = "";
zSTRING const ZOPT_LANGUAGE              = "";
zSTRING const zOPT_SEC_INFO              = "";
zSTRING const zOPT_SEC_MUSIC             = "";
zSTRING const ZOPT_VID_GAMMA             = "";
zSTRING const ZOPT_DEF_LANGUAGE          = "";
zSTRING const zOPT_SEC_GAME              = "";
zSTRING const ZOPT_GAMEKEY_SNEAK         = "";
zSTRING const ZOPT_GAMEKEY_WEAPON        = "";
zSTRING const ZOPT_GAMEKEY_LOOK          = "";
zSTRING const ZOPT_GAMEKEY_STRAFERIGHT   = "";
zSTRING const ZOPT_VID_RES               = "";
zSTRING const ZOPT_GAMEKEY_LEFT          = "";
zSTRING const ZOPT_GAMEKEY_ACTION        = "";
zSTRING const ZOPT_GAMEKEY_LAME_POTION   = "";
zSTRING const ZOPT_GAMEKEY_ACTIONLEFT    = "";
zSTRING const ZOPT_GAMEKEY_SCREEN_STATUS = "";
zSTRING const ZOPT_GAMEKEY_END           = "";

struct zCOption {
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

	zFILE* directory[DIR_SIZE_TOTAL];
	zSTRING dir_string[DIR_SIZE_TOTAL];

	//zCOptions is responsible for the command line as well.
	zSTRING commandline;
};

void zInitOptions();
void zDeinitOptions();

zCOption* zoptions;
zCOption* zgameoptions;
