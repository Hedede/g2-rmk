//#################################################################
//
//  Ini File Interface: zCOption
//
//#################################################################
/*
   The sense of "Temporary values" is probably, that the developers
   thought of an "apply" button. If that "apply" button is used,
   temporary values would have been made persistent.

   Doesn't seem like this feature is used anywhere.
 */
//************************************
//  zCOptions is the interface
//  to access ini files.
//************************************

zSTRING[] zOPTION_PATHNAMES = {
	"DIR_SYSTEM", "DIR_WEB", "DIR_SAVEGAMES", "DIR_DATA", "DIR_ANIMS",
	"DIR_CUTSCENES", "DIR_OUTPUTUNITS", "DIR_MESHES", "DIR_SCRIPTS",
	"DIR_SOUND", "DIR_VIDEO", "DIR_MUSIC", "DIR_TEX", "DIR_TEX_DESKTOP",
	"DIR_WORLD", "DIR_PRESETS", "DIR_TOOLS_DATA", "DIR_COMPILED_ANIMS",
	"DIR_COMPILED_MESHES", "DIR_COMPILED_SCRIPTS", "DIR_COMPILED_TEXTURES",
	"DIR_TOOLS_CONFIG", "SUBDIR_INTERN", "", "DIR_ROOT", "DIR_EXECUTABLE",
}

enum zOPTION_PATH {
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

class zCOptionSection {
	//name of the section. For example "GAME"
	zSTRING secName;
	//the entries in this section.
	zCArray<zCOptionEntry*> entryList;
};

class zCOptionEntry {
	zBOOL changed;            //zBOOL     //unused?

	zCArray <EntryChangeHandler> ccbList;
	//engine functions that want to be informed, whenever this option changes. Quite useless for modders.

	//  Variable-Data
	zSTRING varName;         //name of the variable
	zSTRING varValue;        //value of the variable (everything is converted to a string)
	zSTRING varValueTemp;    //temporary value of the variable. See above. Useless as far as I can see.
	int varFlag;           //int        //flags of the variable. The engine doesn't seem to care about these flags however.
};

const int NUM_ENTRIES = 26;

class zCOption {
public:
	virtual ~zCOption();

	int ReadRaw(zSTRING const& secName, char const* optName, char *& outArray,
	        char* orArray, int orArraySize);
private:
	zBOOL m_bReadTemp;           //read "temporary" value instead of normal value.

	zCArray<zCOptionSection*> sectionList; //an ini files consists of sections

	zFILE* directory[NUM_ENTRIES];
	zSTRING dir_string[NUM_ENTRIES];
	//zCOptions is responsible for the command line as well.
	zSTRING commandline;
};

int zCOption::ReadRaw(zSTRING const& secName, char const* optName, char *& outArray,
                      char* orArray, int orArraySize)
{
	zCOptionSection* section = GetSectionByName(secName, 0);
	zCOptionEntry* entry = GetEntryByName(section, optName, 0);

	if (!section || !entry) {
		if ( orArray )
			WriteRaw(secName, optName, orArray, orArraySize, 0);
	}

	zSTRING rawstr;
	ReadString(this, rawstr, secName, optName, 0);

	int len = rawstr.Length();

	outArray = new char[len/2];
	int size = 0;

	for (int i = 0; i < len; i += 2) {
		uint32_t tmp;
		sscanf(rawstr[i], "%2x", &tmp);
		outArray[idx] = tmp;
		size += 1;
	}
	return size;
}
