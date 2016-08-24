class oCSavegameInfo : public zCObject {
	Z_OBJECT(zCObject);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;
	virtual ~oCSavegameInfo();

	bool DoesSavegameExist() const
	{
		return timeDay >= 0;
	}
private:
	zSTRING dir;
	zCTexture* thumbPic;
	int slot;
	zSTRING title;
	zSTRING worldName;
	int timeDay;
	int timeHour;
	int timeMin;
	zSTRING saveDate;
	int versionMajor;
	int versionMinor;
	int playTimeSeconds;
	int versionPoint;
	int versionInt;
	zSTRING versionAppName;
	zBOOL saveIncompatible;
};

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
