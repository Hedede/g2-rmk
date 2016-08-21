zCOption::zCOption()
{
	for (auto& dir : directory) {
		dir = new zFILE_FILE();
		// also assigns string some value,
		// looks like assigns sdir_strings to itself
	}
}

zCOption::~zCOption()
{
	for (auto& sec : sectionList)
		delete sec;
	for (auto& dir : directory)
		delete dir;
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

zCOptionSection* zCOption::GetSectionByName(zSTRING const& secName, int create)
{
	zSTRING name = secName;
	name.TrimLeft('[');
	name.TrimRight(']');
	name.Upper();


	for (auto& section : sectionList) {
		if (section->secName == secName)
			return section;
	}

	if (create) {
		auto section = new zCOptionSection(secName);
		sectionList.Insert(section);
		return section;
	}

	return nullptr;
 }

bool zCOption::SectionExists(zSTRING const& secName)
{
	return GetSectionByName(secName, 1) != 0;
}

int zCOption::RemoveSection(zSTRING const& secName)
{
	auto section = GetSectionByName(secName, 1); //BUG! should be 0

	if (!section)
		return 0;

	sectionList.Remove(section);
	return 1;
}

zCOptionEntry* zCOption::GetEntryByName(zCOptionSection* section, zSTRING const& name, int create)
{
	if ( !section )
		return 0;

	// int anz = section->entryList.GetNumInList();
	for (auto& entry : section->entryList) {
		if (entry->varName == name)
			return entry;
	}

	if (create) {
		auto entry = new zCOptionEntry(name, "");
		section->entryList.Insert(entry);
		return entry;
	}

	return nullptr;
}

bool zCOption::EntryExists(zSTRING const& secName, const char* name)
{
	auto sec = GetSectionByName(secName, 0);
	if ( sec )
		return GetEntryByName(sec, name, 0) != 0;
	return false;
}

int zCOption::RemoveEntry(zSTRING const& secName, char const* name)
{
	auto section = GetSectionByName(secName, 1);
	auto entry = GetEntryByName(section, name, 1);

	if (entry) {
		section->entryList.Remove(entry);
		delete entry;
	}
	return 1;
}

void zCOption::SetFlag(zSTRING const& secName, const char* entryName, int flag)
{
	auto section = GetSectionByName(secName, 1);
	auto entry = GetEntryByName(section, entryName, 1);
	entry->varFlag |= flag;
}

int zCOption::Parm(zSTRING const& key)
{
	zSTRING param = "-" + key;
	param.Upper();
	commandline.Upper();

	return commandline.Search(0, param, 1u) >= 0;
}

zSTRING zCOption::ParmValue(zSTRING const& key)
{
	zSTRING param = "-" + key + ":";
	param.Upper();

	int pos = commandline.Search(0, param, 1u);
	if ( pos >= 0 ) {
		int start = param.Length() + pos;
		int end = start;
		while(commandline[end] && commandline[end] != ' ')
			++end;

		return commandline.Copied(start, end - start);
	}

	return "";
}

float zCOption::ParmValueFloat(zSTRING const& key)
{
	zSTRING parmValue = ParmValue(key);
	if (parmValue.IsEmpty)
		return -1.0;
	return parmValue.ToFloat();
}

int zCOption::ParmValueInt(zSTRING const& key)
{
	zSTRING parmValue = ParmValue(key);
	if (parmValue.IsEmpty)
		return -1;
	return parmValue.ToInt();
}

zSTRING zCOption::ParmValueRaw(zSTRING const& key)
{
	return ParmValue(key);
}

void zCOption::InsertChangeHandler(zSTRING const& secName, const char* name, EntryChangeHandler ccb)
{
	auto section = GetSectionByName(secName, 1);
	auto entry   = GetEntryByName(setion, name, 1);
	entry->ccbList.Insert(ccb);
}

int zCOption::Load(zSTRING filename)
{
	zINFO(8,"B: INI: Loading " + filename + " ..."); // 229
	filename.Lower();

	sectionList.Clear();

	ChangeDir(DIR_SYSTEM);

	auto file = new zFILE_FILE(filename);
	if ( !file )
		return 0;

	if ( !file->Exists() ) {
		zWARNING("B: INI: File \"" + filename + "\" not found."); // 240
		delete file;
		return 0;
	}

	file->Open(0);

	zSTRING secName = "HEAD";
	auto head = new zCOptionSection(secName);
	sectionList.Insert(head);

	zINFO(8, "B: INI: Section: " + secName); // 254

	zSTRING trash;
	while ( !file->Eof() ) {
		zSTRING readstr;
		file->Read(readstr);

		readstr.TrimLeft(' ');
		readstr.TrimRight(' ');
		
		if (readstr.IsEmpty() || readstr[0] == ';') {
			trash += readstr;

			if ( file->Eof(file) )
				continue;

			trash += "\r\n";
		} else {
			if (!trash.IsEmpty()) {
				auto entry = new zCOptionEntry("", trash);

				entryList.Insert(entry);
				zINFO(9,  "B: INI: Trash: ", trash); // 274
			}

			trash.Clear();

			if (readstr[0] == '[') {
				zINFO(8, ""); // 281

				readstr.TrimLeft('[');
				readstr.TrimRight(']');

				auto sec = GetSectionByName(readstr, 1); // BUG! should be ,0
				if (sec) {
					zINFO(8, "B: INI: Section(cont): " + sec->secName); // 289
				} else {
					sec = new zCOptionSection(readstr);

					sectionList.Insert(sec);

					zINFO(8, "B: INI: Section: " + sec->secName); // 295
				}

				continue;
			}

			if (readstr.Search(0, "=", 1u) <= 0) {
				zWARNING("B: INI: Unknown line: " + readstr); // 310
				continue;
			}

			auto varName = readstr.Deleted("=", 3);
			readstr.Delete("=", 4);

			auto entry = new zCOptionEntry(varName, readstr);

			entryList.Insert(entry);

			zINFO(9, "B: INI: Entry: " + entry->varName + "=" + entry->varValue); // 306
		}
	}

	if (!trash.IsEmpty()) {
		auto entry = new zCOptionEntry("", trash);

		entryList.Insert(entry);
		zINFO(9,  "B: INI: Trash: ", trash); // 274
	}

	if (!trash.IsEmpty()) {
		auto entry = new zCOptionEntry("", trash);

		entryList.Insert(entry);
		zINFO(9,  "B: INI: Trash: ", trash); // 320
	}

	file->Close();

	char buffer[16];
	int  size = 16;
	GetComputerNameA(buffer, &size);

	WriteString(zOPT_SEC_INTERNAL, "idComputerName", buffer, 0);

	size = 16;
	GetUserNameA(buffer, &size);
	WriteString(zOPT_SEC_INTERNAL, "idUserName", buffer, 0);

	zINFO(8, ""); // 346
	zINFO(8, ""); // 347

	delete file;
	return 1;
}

int zCOption::Save(zSTRING filename)
{
	auto file = zfactory->CreateZFile(filename);
	if (!file )
		return 0;

	zINFO(6, "B: INI: Saving " + filename + " ..."); // 374

	ChangeDir(DIR_SYSTEM);

	filename.Lower();

	file->Create(filename);
	int numSections = sectionList.GetNumInList();
	for (auto section : sectionList) {
		if (section->secName != "HEAD") {
			auto secName = '[' + section->secName + ']' + "\r\n";
			file->Write(secName);
		}
		zINFO(8, "B: INI: Section: ", section->secName); // 390

		for (auto entry : section->entryList) {
			if ((entry->varFlag & 1) != 1) {
				if (entry->varName.IsEmpty())
					continue;

				auto var = entry->varName + "=" + entry->varValue + "\r\n";
				file->Write(var);
			}

			zINFO(8,""); //407
		}
	}

	delete file;

	zINFO(6, ""); // 413

	return 1;
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

zSTRING zCOption::ReadString(zSTRING const& secName, const char* optName, const char* defval)
{
	auto section = GetSectionByName(secName, 1);
	auto option = GetEntryByName(section, optName, 1);

	if (option->varValueTemp.IsEmpty() && defVal) {
		varValueTemp = defVal;
		varValue = defVal;
	}

	if ( !readTemp )
		return optName->varValue;

	return optName->varValueTemp;
}

zSTRING zCOption::ReadString(zSTRING const& secName, zSTRING const& optName, char const* defval)
{
	return ReadString(secName, optName.Data(), defval);
}

bool zCOption::ReadBool(zSTRING const& secName, const char* optName, bool defval)
{
	zSTRING val;
	if (defval == true)
		val = ReadString(secName, optName, "1");
	else
		val = ReadString(secName, optName, "0");

	val.Upper();

	if (val == "1" || val == "TRUE")
		return true;

	if (val == "0" || val == "FALSE")
		return false;

	return 0;
}

bool zCOption::ReadBool(zSTRING const& secName, zSTRING const& optName, bool defval)
{
	return ReadBool(secName, optName.Data(), defval);
}

int zCOption::ReadInt(zSTRING const& secName, const char* optName, int defval)
{
	zSTRING def = defval;
	auto val = ReadString(secName, optName, def);
	return val.ToInt();
}

int zCOption::ReadInt(zSTRING const& secName, zSTRING const& optName, int defval)
{
	return ReadInt(secName, optName.Data(), defval);
}

zDWORD zCOption::ReadDWord(zSTRING const& secName, const char* optName, zDWORD defval)
{
	zSTRING def = defval;
	auto val = ReadString(secName, optName, def);
	return val.ToUint();
}

zDWORD zCOption::ReadDWord(zSTRING const& secName, zSTRING const& optName, zDWORD defval)
{
	return ReadDWord(secName, optName.Data(), defval);
}

float zCOption::ReadReal(zSTRING const& secName, const char* optName, float defval)
{
	zSTRING def = defval;
	auto val = ReadString(secName, optName, def);
	return val.ToFloat();
}

float zCOption::ReadReal(zSTRING const& secName, zSTRING const& optName, float defval)
{
	return ReadReal(secName, optName.Data(), defval);
}

int zCOption::WriteString(zSTRING const& section, const char* option, zSTRING value, int temp)
{
	auto sec = GetSectionByName(section, 1);
	auto entry = GetEntryByName(sec, option, 1);


	zSTRING& str = temp ? varValueTemp : varValue;

	bool notify = str == value;

	varValueTemp = value;
	if (!temp)
		varValue = value;

	if (notify) {
		for (auto func : ccbList) {
			if (func())
				break;
		}
	}
	return 1;
}

int zCOption::WriteString(zSTRING const& secName, zSTRING const& entry, zSTRING value, int temp)
{
	return WriteString(secName, entry.Data(), value, temp);
}

int zCOption::WriteBool(zSTRING const& secName, const char* entry, int value, int temp)
{
	return WriteString(secName, entry, value ? "1" : "0", temp);
}

int zCOption::WriteBool(zSTRING const& secName, zSTRING const& entry, int value, int temp)
{
	return WriteBool(secName, entry.Data(), value, temp);
}

int zCOption::WriteDWord(zSTRING const& secName, const char* entry, uint32_t value, int temp)
{
	return WriteString(secName, entry, value, temp);
}

int zCOption::WriteDWord(zSTRING const& secName, zSTRING const& entry, uint32_t value, int temp)
{
	return WriteDWord(secName, entry.Data(), value, temp);
}

int zCOption::WriteInt(zSTRING const& secName, const char* entry, int32_t value, int temp)
{
	return WriteString(secName, entry, value, temp);
}

int zCOption::WriteInt(zSTRING const& secName, zSTRING const& entry, int32_t value, int temp)
{
	return WriteInt(secName, entry.Data(), value, temp);
}

int zCOption::WriteReal(zSTRING const& secName, const char* entry, float value, int temp)
{
	return WriteString(secName, entry, value, temp);
}

int zCOption::WriteReal(zSTRING const& secName, zSTRING const& entry, float value, int temp)
{
	return WriteReal(secName, entry.Data(), value, temp);
}

int zCOption::WriteRaw(zSTRING const& secName, const char* entry, void *buffer, int size, int temp)
{
	zSTRING value;
	for (int i = 0; i < size: ++i) {
		char temp[3];
		sprintf(temp, "%02x", ((char*)buffer)[i]);
		value += temp;
	}
	WriteString(secName, entry, value, temp);
	return 1;
}

int zCOption::WriteRaw(zSTRING const& secName, zSTRING const& entry, void* buffer, int size, int temp)
{
	return WriteRaw(secName, entry.Data(), buffer, size, temp);
}

int zCOption::Init(zSTRING parmlist, bool do_init)
{
	if ( !do_init )
		return 0;

	zINFO(3,  "B: zOPT: Initialize configurations "); // 758

	commandline += parmlist;
	commandline.Upper();

	char Filename[260];

	GetModuleFileNameA(0, Filename, 0xFFu);

	zSTRING filename = Filename;

	int pos = filename.SearchRev("/",1);
	filename.Delete(pos, -1);

	_getcwd(Filename, 254);
	if ( _access(pathsd, 0) == -1 ) {
		zFATAL("B: zOPT: Could not find file " + pathsd); //781
		return 0;
	}

	zINFO(3,"B: zOPT: Found file " + pathsd); //785

	zINFO(5,"B: zOPT: Initialising Parser"); // 789

	zparser.Reset();

	zINFO(5, "B: zOPT: Parsing " + pathsd + " ..."); // 792
	zparser.ParseFile(pathsd);

	zINFO(5,  "B: zOPT: Analysing " + pathsd + " ..."); // 795

	int idx = zparser.GetIndex(zOPTION_PATHNAMES[24]);
	if (idx < 0)
		zFATAL("B: zOPT: Entry missed in " + pathsd + ": " + zOPTION_PATHNAMES[24]); // 799

	auto text = zparser.GetText(idx, 0);

	chdir(text);
	_getcwd(Filename, 254);

	auto root = zFILE::SetRootDirectory(Filename);

	dir_string[24] = root;

	zINFO(6,"B: zOPT: Set root-directory to \"" + dir_string[24] + "\""); //806

	zFILE_FILE file;

	for (int i = 0; i < 24; ++i) {
		if (zOPTION_PATHNAMES[i] == DIR_ROOT) // what the fuck?
			break;

		int idx = zparser.GetIndex(zOPTION_PATHNAMES[i]);
		if (idx < 0) {
			zFATAL("B: zOPT: Entry missed in " + pathd + ": " + zOPTION_PATHNAMES[i]); // 851
		} else {
			auto text = zparser.GetText(idx, 0);
			auto game = zoptions->ParmValue("game");
			if (!game.IsEmpty()) {
				auto file = zfactory->CreateZFile(game);
				if (file) {
					zSTRING filename;
					file->GetFilename(filename);
					filename.Lower();
					if (filename == "gothicgame") {
						while (text[filename.Length()] == '\\')
							text.DeleteRight(1);
						text += "_" + filename + '\\';
						text.Lower();
					}
					file->Close();
					delete file;
				}
			}

			text.Upper();
			if (text.Search(0, "$DATA$", 1) > 0) {
				int pos = text.Search(0, "$DATA$", 1);
				text.Delete(pos, 0);
				text.Insert(pos, dir_string[3]);
				pos = text.Search(0, "\\\\", 1);
				if (pos >= 0)
					text.Delete(pos, 1);
			}

			directory[i]->SetPath(text);

			zINFO(6,"B: zOPT: " + zOPTION_PATHNAMES[i] " = " + text); // 867
		}

		if ( !text.Search(0, "DIR_", 1u) ) {
			file.SetPath(text);
			auto path = file.GetFullPath();

			if ( _access(path.Data(), 0) ) {
				zFAULT("B: zOPT: The value of <" tetx "> in file \"" + pathsd + "\" is wrong:\r\n Directory \"" path "\" does not exist."); // 877
			}
		}
	}


	root.Delete(dir_string[24], 0);
	root += '\\';
	root += dir_string[25];
	directory[DIR_EXECUTABLE]->SetPath(root);

	zINFO(3,"B: zOPT: DIR_EXECUTABLE = " + root); // 887

	zparser->Reset();

	zINFO(3,""); // 891

	return 0;
}

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
