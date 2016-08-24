struct oCSavegameManager {
	oCSavegameManager();
	virtual ~oCSavegameManager();

	zCArray<oCSavegameInfo*> saves;
	oCSavegameInfo* current;
	bool initialized;
	char *unk[6];
};

zSTRING const SAVEGAME_CURRENT_DIR = "current";
zSTRING const SAVEGAME_DIR = "savegame";
zSTRING const SAVEGAME_HEADER_FILE = "saveinfo.sav";

oCSavegameInfo::~oCSavegameManager();
oCSavegameManager::oCSavegameManager()
{
	current = new oCSavegameInfo(0);

	for (int i = 1; i < 21; ++i)
		saves.InsertAtPos(new oCSavegameInfo(i), i);
	initialized = false;
}

void oCSavegameManager::Reinit()
{
	initialized = false;
	Init();
}

oCSavegameInfo* oCSavegameManager::GetSavegame(int slotNr)
{
	if ( slotNr >= 0 )
		return saves[slotNr];

	current->SetSlot(slotNr);
	return current;
}

zSTRING oCSavegameManager::GetSlotDirName(int slotNr)
{
	std::string result;
	if ( slotNr == 0 ) {
		result += SAVEGAME_DIR + '/';
	} else if ( slotNr > 0 && slotNr < 21 ) {
			result += SAVEGAME_DIR + slotNr + '/';
	} else if ( slotNr == -1 )
		result +=  SAVEGAME_CURRENT_DIR + '/';
	}
	return result;
}

void oCSavegameManager::CleanResources()
{
	for ( auto i = 0; i < saves.GetNum(); ++i )
		GetSavegame(i)->CleanResources();
}

void oCSavegameManager::ClearCurrent()
{
	zCOption::ChangeDir(zoptions, DIR_SAVEGAMES);
	Dir_Clear(SAVEGAME_CURRENT_DIR + '/', "*.*");
}

void oCSavegameManager::CopyToCurrent(int slotNr)
{
	zINFO(4, "B: SG: Copying savegame to current-dir ..."); // 407, oSavegame.cpp

	auto dir = saves[slotNr]->GetDir();
	auto cur = GetSlotDirName(-1);
	zoptions->ChangeDir(DIR_SAVEGAMES);
	Dir_Copy(dir, cur);
}

void oCSavegameManager::CopyFromCurrent(int slotNr)
{
	zINFO(4, "B: SG: Copying current-dir to savegame..."); // 422, oSavegame.cpp

	auto dir = saves[slotNr]->GetDir();
	auto cur = GetSlotDirName(-1);
	zoptions->ChangeDir(DIR_SAVEGAMES);
	Dir_Copy(cur, dir);
}

void oCSavegameManager::SetAndWriteSavegame(int slotNr, oCSavegameInfo *info)
{
	auto save = saves[slotNr];
	info->AddRef();
	Release(save);

	auto saves_dir = zoptions->GetDirString(DIR_SAVEGAMES);
	auto path = saves_dir + info->GetDir(info);

	auto file = zfactory->CreateZFile(path);
	file->DirCreate();

	path += SAVEGAME_HEADER_FILE;

	auto arc = zarcFactory.CreateArchiverWrite(file, 1, 0, 0);

	arc->WriteObject(info);
	arc->Close();
	Release(arc);
	file->Close();
	delete file;
}

void oCSavegameManager::Init(oCSavegameInfo *this)
{

	zINFO_B(3,"B: SG: Initializing savegame-manager ..."); // 277

	auto savedir = zoptions->GetDirString(DIR_SAVEGAMES);
	auto file = zfactory->CreateZFile(savedir);
	if (file) {
		file->DirCreate();
		file->Close();
		delete file;
	}

	auto currdir = zoptions->GetDirString(DIR_SAVEGAMES) + SAVEGAME_CURRENT_DIR + '/';
	file = zfactory->CreateZFile(currdir);
	if (file) {
		file->DirCreate();
		file->Close();
		delete file;
	}
	zoptions->ChangeDir(DIR_SAVEGAMES);

	for ( auto i = 0; i < saves.GetNum(); ++i ) {
		auto save = GetSavegame(i);
		if (save) {
			save->title = "---";
			save->UpdateThumbPic(0);
			auto dir = save->GetDir();
			auto path = dir + SAVEGAME_HEADER_FILE;
			auto file = zfactory->CreateZFile(path);
			if (file->Exists()) {
				file->Open(0);
				auto arc = zarcFactory.CreateArchiverRead(file, 0);
				if ( arc ) {
					arc->ReadObject(save);
					arc->Close();
					arc->Release();
				}

				file->Close();
				zINFO(5,"B: BS: Savegame "_s + i + " found: \"" + save->GetName() + "\""); // 327
			}
			delete file;
		}
	}

	initialized = 1;

	zINFO_C(3,"");
}
