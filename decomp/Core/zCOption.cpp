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

	void AddParameters(zSTRING params);
	void ChangeDir(zTOptionPaths dirId);

	zSTRING& zCOption::GetDirString(zTOptionPaths dirId)
	{
		return dir_string[dirId];
	}

	uint32_t zCOption::GetNumSections() const
	{
		return sectionList.GetNumInList();
	}

	int Parm(zSTRING const& parm);
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

void zDeinitOptions();

void zInitOptions()
{
	if ( zoptions )
		return;

	zoptions = new zCOption();

	zoptions->Init(sysCommandLine, 1);
	zoptions->ChangeDir(DIR_SYSTEM);

	zSTRING ini = zoptions->ParmValue("ini");

	if (ini.IsEmpty())
		ini = "Gothic.ini";

	zoptions->Load(ini);

	int version = zoptions->ReadInt(zOPT_SEC_GAME, "PATCHVERSION", 0);
	if (version < 0 )
		version = 0;

	switch (version) {
	case 0:
		zoptions->WriteBool(zOPT_SEC_GAME, "enableJoystick", 0, 0);
		++version;
	case 1:
		zoptions->WriteReal(zOPT_SEC_GAME, "SHORTKEY1FARPLANEDIST", 0.8, 0);
		zoptions->WriteReal(zOPT_SEC_GAME, "SHORTKEY2FARPLANEDIST", 1.2, 0);
		zoptions->WriteReal(zOPT_SEC_GAME, "SHORTKEY3FARPLANEDIST", 2.0, 0);
		zoptions->WriteReal(zOPT_SEC_GAME, "SHORTKEY4FARPLANEDIST", 3.0, 0);
		zoptions->WriteBool("RENDERER_D3D", "zSyncAmbientCol", 0, 0);

		zoptions->WriteBool(zOPT_SEC_INTERNAL, "logStatistics", 0, 0);
		zoptions->WriteBool(zOPT_SEC_INTERNAL, "extendedMenu", 0, 0);
		zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameAbnormalExit", 0, 0);

		zoptions->WriteReal("ENGINE", "zInventoryItemsDistanceScale", 1.3, 0);
		++version;
	case 2:
		zoptions->WriteBool("RENDERER_D3D", "radeonHackAmbientColBug", 0, 0);
		++version;
	case 3:
		zoptions->WriteString(zOPT_SEC_INTERNAL, "gameScript", "", 0);
		zoptions->WriteString(zOPT_SEC_INTERNAL, "playerInstanceName", "", 0);
		++version;
	case 4:
		zoptions->WriteBool("RENDERER_D3D", "geforce3HackWBufferBug", 1, 0);
		zoptions->WriteBool("ENGINE", "zSkyRenderFirst", 1, 0);
		++version;
	default:
		break;
	}

	zoptions->WriteInt("GAME", "PATCHVERSION", version, 0);

	if (zoptions->ReadBool(zOPT_SEC_INTERNAL, "gameStartFailed", 0))
		zWARNING("B: GOTHIC: Start of gothic failed before."); // zzBert.cpp 146

	if (zoptions->ReadBool(zOPT_SEC_INTERNAL, "gameAbnormalExit", 0)) {
		zWARNING("B: GOTHIC: Abnormal exit of gothic detected."); // 152
		zWARNING("N: Force Initializing VDFS after Crash."); // 163

		bool devmode = zoptions->Parm("DEVMODE");
		if ( devmode ) {
			zWARNING("N: Forced Initialization of VDFS canceled (-devmode)") // 165
		} else {
			char filename[260];
			memset(filename, 0, 260);

			GetModuleFileNameA(0, filename, 0x104u);

			auto i = &filename[lstrlenA(filename)];
			for (; i >= filename ; --i) {
				if (*i == '\\')
					break;
				if (*i == '/')
					break;
			}
			if (i > filename) {
				*i-- = 0;
				for (; i >= filename ; *i-- = 0) {
					if (*i == '\\')
						break;
					if (*i == '/')
						break;
					
				}
				lstrcatA(filename, "VDFS.DMP");
				DeleteFileA(filename);
			}
		}
	}

	zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameAbnormalExit", 1, 0);
	zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameStartFailed", 1, 0);

	int gameStarts = zoptions->ReadInt(zOPT_SEC_INTERNAL, "gameStarts", 0);
	zoptions->WriteInt(zOPT_SEC_INTERNAL, "gameStarts", v8 + 1, 0);

	zoptions->RemoveEntry(zOPT_SEC_INTERNAL, "testmode");
	zoptions->RemoveEntry(zOPT_SEC_GAME, "weatherEffects");
	zoptions->RemoveEntry(zOPT_SEC_INTERNAL, "zFastUnsafeSaveGames");

	zoptions->Save(ini);

	zSTRING game = zoptions->ParmValue("GAME");

	if ( game.Length() > 0 ) {
		zoptions->ChangeDir(DIR_SYSTEM);
		zFILE* file = zfactory->CreateZFile(game);

		if (!file || !file->Exists()) {
			game += ".ini";
			file = zfactory->CreateZFile(game);
		}

		if (file && file->Exists())
			zgameoptions = new zCOption();
			zgameoptions->Init("", 0);
			zgameoptions->Load(game);
			zSTRING forceparams = zgameoptions->ReadString(zOPT_SEC_OPTIONS, "force_Parameters", 0);
			if (forceparams.Length() > 0)
				AddParameters(forceparams);

			file->Close();
			delete file;
		}
	}

	if ( zoptions->Parm("VDFS") ) {
		zSTRING vdfs = zoptions->ParmValue("VDFS");
		if (vdfs ==  "PHYSICALFIRST")
			zFILE_VDFS::s_physical_first = 1;
	}

	bool recalc = zoptions->ReadBool(zOPT_SEC_PERFORMANCE, "recalc", 1);
}

void zDeinitOptions()
{
	if ( zoptions ) {
		zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameAbnormalExit", 0, 0);
		zoptions->ChangeDir(0);

		zSTRING ini = zoptions->ParmValue("ini");

		if (ini.IsEmpty())
			ini = "Gothic.ini";

		zoptions->Save(ini);

		Delete(zoptions);
		Delete(zgameoptions);
	}
}

void zCOption::AddParameters(zSTRING params)
{
	commandline = params + " " + commandline;
	commandline.Upper();
}

void zCOption::ChangeDir(int dirId)
{
	auto dir = directory[dirId];
	if ( dir ) {
		dir->ChangeDir(0);
	} else {
		zFAULT("B: (zCOption::ChangeDir) directory-entry not defined!"); // 900
	}
}

bool zCOption::EntryExists(zSTRING const& secName, const char* name)
{
	auto sec = GetSectionByName(secName, 0);
	if ( sec )
		return GetSectionByName(sec, name, 0) != 0;
	return false;
}

bool zCOption::SectionExists(zSTRING const& secName)
{
  return GetSectionByName(secName, 1) != 0;
}

int zCOption::Parm(zSTRING const& parm)
{
	zSTRING search = "-" + parm;
	search.Upper();
	commandline.Upper();

	return commandline.Search(0, search, 1u) >= 0;
}

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
	ReadString(rawstr, secName, optName, 0);

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
