class oCSavegameInfo : public zCObject {
	Z_OBJECT(zCObject);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;
	oCSavegameInfo() = default;
	oCSavegameInfo(int slotNr) : oCSavegameInfo()
	{
		SetSlot(slotNr);
	}

	void Init()
	{
		dir.Clear();
		saveIncompatible = 0;
		playTimeSeconds = 0;
		versionMajor = APP_VERSION_MAJOR;
		versionMinor = APP_VERSION_MINOR;
		versionPoint = APP_VERSION_POINT;
		versionInt = APP_VERSION_INT;
		versionAppName = APP_NAME;
	}

	virtual ~oCSavegameInfo()
	{
		CleanResources();
	}

	bool DoesSavegameExist() const
	{
		return timeDay >= 0;
	}

	void CleanResources()
	{
		Release(thumbPic);
	}

	zSTRING GetDir() const
	{
		return dir;
	}

	zSTRING GetSaveTime() const
	{
		return saveDate;
	}

	zSTRING GetWorldName() const
	{
		return worldName;
	}

	zSTRING GetName() const
	{
		return title;
	}

private:
	zSTRING dir;
	zCTexture* thumbPic = nullptr;
	int slot = -1;
	zSTRING title;
	zSTRING worldName;
	int timeDay  = -1;
	int timeHour = 0;
	int timeMin  = 0;
	zSTRING saveDate;
	int versionMajor = APP_VERSION_MAJOR;
	int versionMinor = APP_VERSION_MINOR;
	int playTimeSeconds = 0;
	int versionPoint = APP_VERSION_POINT;
	int versionInt   = APP_VERSION_INT;
	zSTRING versionAppName{APP_NAME};
	zBOOL saveIncompatible = false;
};

zSTRING const SAVEGAME_THUMB_FILE  = "thumb.sav";
zSTRING const SAVEGAME_PIC_FILE    = "savepic.sav";
zSTRING const SAVEGAME_HEADER_FILE = "saveinfo.sav";
zSTRING const SAVEGAME_GLOBAL_FILE = "savedat.sav";
zSTRING const SAVEGAME_CURRENT_DIR = "current";
zSTRING const SAVEGAME_QUICK_DIR   = "quicksave";
zSTRING const SAVEGAME_DIR         = "savegame";
zSTRING const SAVEGAME_EXR         = "sav";

void oCSavegameInfo::Archive(zCArchiver& arc)
{
	_SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	saveDate = SystemTime.wDay + "." + SystemTime.wMonth + "." +
	           SystemTime.wYear + " - " + SystemTime.wHour + ":";

	if ( SystemTime.wMinute < 10 )
		saveDate += "0";
	saveDate += SystemTime.wMinute;

	arc.WriteString("Title", title);
	arc.WriteString("WorldName", worldName);
	arc.WriteInt("TimeDay",  timeDay);
	arc.WriteInt("TimeHour", timeHour);
	arc.WriteInt("TimeMin",  timeMin);
	arc.WriteString("SaveDate", saveDate);
	arc.WriteInt("VersionMajor", APP_VERSION_MAJOR);
	arc.WriteInt("VersionMinor", APP_VERSION_MINOR);
	arc.WriteInt("PlayTimeSeconds", playTimeSeconds);
	arc.WriteInt("VersionPoint", APP_VERSION_POINT);
	arc.WriteInt("VersionInt", APP_VERSION_INT);
	arc.WriteString("VersionAppName", APP_NAME);

	saveIncompatible = 0;
}

void oCSavegameInfo::Unarchive(zCArchiver& arc)
{
	arc.ReadString("Title", title);
	arc.ReadString("WorldName", worldName);
	arc.ReadInt("TimeDay", timeDay);
	arc.ReadInt("TimeHour", timeHour);
	arc.ReadInt("TimeMin", timeMin);
	arc.ReadString("SaveDate", saveDate);
	arc.ReadInt("VersionMajor", versionMajor);
	arc.ReadInt("VersionMinor", versionMinor);
	arc.ReadInt("PlayTimeSeconds", playTimeSeconds);

	versionPoint = -1;
	versionInt = -1;

	versionAppName.Clear();

	if ( versionMajor > 1 || versionMajor == 1 && versionMinor >= 30 ) {
		arc.ReadInt("VersionPoint", versionPoint);
		arc.ReadInt("VersionInt", versionInt);
		arc.ReadString("VersionAppName", versionAppName);
	}

	saveIncompatible = versionMajor == 1 && versionMinor <= 27;

	if ( saveIncompatible ) {
		zSTRING savVer = versionMajor + ":" + versionMinor;
		zSTRING curVer = APP_VERSION_MAJOR + ":" + APP_VERSION_MINOR;
		zWARNING("B: VER: Version of savegame \"" + title + "\" (" + savVer +
		         ") is incompatible to current version (" + curVer  ")");
	}
}

void oCSavegameInfo::ReloadResources()
{
	CleanResources();

	auto dirname = zoptions->GetDirString(DIR_SAVEGAMES) + GetDir();
	auto file = zfactory->CreateZFile(dirname + SAVEGAME_THUMB_FILE);
	if ( file->Exists2(v9) && !file->Open2(0) ) {
		thumbPic = zrenderer->CreateTexture();
		auto tex = new zCTextureFileFormatInternal;
		tex->LoadTexture(file, thumbPic);
		Relese(tex);
		file->Close();
	}
	delete file;
}

void oCSavegameInfo::UpdateThumbPic(zCTextureConvert *texCvt)
{
	if ( texCvt ) {
		zCTextureInfo texInfo;
		texCvt->GetTextureInfo(texCvt, &texInfo);
		texInfo.texFormat = zCTexture::CalcNextBestTexFormat(8);
		texInfo.sizeX = 256;
		texInfo.sizeY = 256;
		texInfo.numMipMap = 1;
		texCvt->ConvertTextureFormat(texCvt, &texInfo);
		auto tex = zrenderer->CreateTexture();
		texCvt->CopyContents(tex);

		auto dirname = zoptions->GetDirString(DIR_SAVEGAMES) + GetDir();
		auto file = zfactory->CreateZFile(dirname + SAVEGAME_THUMB_FILE);
		if ( !file->Create2() ) {
			auto texformat = new zCTextureFileFormatInternal;
			texformat->SaveTexture(file, tex);
			Release(texformat);
			file->Close(0);
		}
		Release(tex);
	}
}

void oCSavegameInfo::SetSlot(int slotNr)
{
	slot = slotNr;

	Init();

	if ( slotNr == 0 ) {
		dir = SAVEGAME_QUICK_DIR + '/';
	} else if ( slotNr >= 1 && slotNr <= 20 ) {
		dir = SAVEGAME_DIR + slotNr + '/';
	} else {
		// dir = SAVEGAME_CURRENT_DIR + '/';
	}
}

zSTRING oCSavegameInfo::GetGameTime()
{
	zSTRING ret;
	if ( timeDay >= 0 ) {
		ret += timeDay;
		ret += " - ";
		ret += timeHour;
		ret += ":";
		if ( timeMin < 10 )
			ret += "0";
		ret += timeMin;
	}
	return ret;
}
