enum zTArchiveMode {
	zARC_MODE_BINARY,
	zARC_MODE_ASCII,
	zARC_MODE_ASCII_PROPS,
	zARC_MODE_BIN_SAFE,
};

class zCArchiverFactory {
private:
	static int EndOfArchive(zCBuffer *buffer, zFILE *file);
	void ReadLine(zSTRING& line, zCBuffer *buffer, zFILE *file);
	void ReadLineArg(zSTRING& line, zSTRING& arg, zCBuffer *buffer, zFILE *file);
	void ReadHeader(int arcFlags, zCBuffer *buffer, zFILE *file, zCArchiver*& arc, zTArchiveMode& arcMode, int& inSaveGame);

	static zCArchiver* CreateArchiverFromMode(zTArchiveMode mode);

	void WriteLine(const zSTRING& line, zCBuffer *buffer, zFILE *file);
	void WriteLine(const char* line, zCBuffer *buffer, zFILE *file);
	void WriteHeader(zCArchiver *arc, zTArchiveMode arcMode, int saveGame, char arcFlags, zCBuffer *buffer, zFILE *file);

public:
	virtual zCArchiver* CreateArchiverRead(zCBuffer* bufffer, int arcFlags);
	virtual zCArchiver* CreateArchiverRead(zFILE* fileRead, int arcFlags);
	virtual zCArchiver* CreateArchiverRead(zSTRING const& fileNameRead,int arcFlags);

	virtual zCArchiver* CreateArchiverWrite(zCBuffer*, zTArchiveMode arcMode, int saveGame, int arcFlags);
	virtual zCArchiver* CreateArchiverWrite(zFILE* fileWrite, zTArchiveMode arcMode, int saveGame, int arcFlags);
	virtual zCArchiver* CreateArchiverWrite(zTArchiveMode arcMode, int saveGame, int arcFlags);
	virtual zCArchiver* CreateArchiverWrite(zSTRING const& fileNameWrite, zTArchiveMode arcMode, int saveGame, int arcFlags);

};

class zCArchiver : public zCObject {
	zCLASS_DECLARATION(zCArchiver)
public:
	struct zTChunkRecord {
		zTChunkRecord() = default;
		~zTChunkRecord() = default;
		zTChunkRecord& operator(zTChunkRecord const&) = default;

		int start;
		int length;
		int16_t versionSum;
		int16_t version;
		int objectIndex;
		zSTRING className;
		zSTRING chunkName;
	};

	virtual ~zCArchiver() = default;
	virtual void WriteInt(char const *,int) = 0;
	virtual void WriteByte(char const *,uchar) = 0;
	virtual void WriteWord(char const *,ushort) = 0;
	virtual void WriteFloat(char const *,float) = 0;
	virtual void WriteBool(char const *,int) = 0;
	virtual void WriteString(char const *,zSTRING const &) = 0;
	virtual void WriteVec3(char	const *,zVEC3 const &) = 0;
	virtual void WriteColor(char const *,zCOLOR const	&) = 0;
	virtual void WriteRaw(char const*,void *,ulong) = 0;
	virtual void WriteRawFloat(char const*,void *,ulong) = 0;
	virtual void WriteEnum(char const *,char const *,int) = 0;
	virtual void WriteObject(char const *,zCObject	&) = 0;
	virtual void WriteObject(char const *,zCObject	*) = 0;
	virtual void WriteObject(zCObject*) = 0;
	virtual void WriteChunkStart(char const *,char const *,ushort,ulong) = 0;
	virtual void WriteChunkStart(char const *,ushort) = 0;
	virtual void WriteChunkEnd() = 0;
	virtual void WriteGroupBegin(char const	*) = 0;
	virtual void WriteGroupEnd(char const *) = 0;
	virtual void ReadInt(char const	*) = 0;
	virtual void ReadInt(char const *,int &) = 0;
	virtual void ReadByte(char const *) = 0;
	virtual void ReadByte(char const *,uchar &) = 0;
	virtual void ReadWord(char const *) = 0;
	virtual void ReadWord(char const *,ushort &) = 0;
	virtual void ReadFloat(char const *) = 0;
	virtual void ReadFloat(char const *,float &) = 0;
	virtual void ReadBool(char const *) = 0;
	virtual void ReadBool(char const *,int &) = 0;
	virtual void ReadString(char const *) = 0;
	virtual void ReadString(char const *,zSTRING &) = 0;
	virtual void ReadVec3(char const *) = 0;
	virtual void ReadVec3(char const *,zVEC3 &) = 0;
	virtual void ReadColor(char	const *) = 0;
	virtual void ReadColor(char const *,zCOLOR	&) = 0;
	virtual void ReadEnum(char const *) = 0;
	virtual void ReadEnum(char const *,int &) = 0;
	virtual void ReadRaw(char const *,void *,ulong) = 0;
	virtual void ReadRawFloat(char	const *,void *,ulong) = 0;
	virtual void ReadObject(char const *,zCObject *) = 0;
	virtual void ReadObject(zCObject *) = 0;
	virtual void ReadObjectAccount(char const *,int,char const *,zCObject *);
	virtual void ReadObjectAccount(char const *,int,zCObject *);
	virtual void ReadChunkStart(char const *) = 0;
	virtual void ReadChunkStart(zSTRING &,ushort&) = 0;
	virtual void ReadChunkStartNamed(char const *,ushort &) = 0;
	virtual void SkipOpenChunk() = 0;
	virtual void GetCurrentChunkVersion() = 0;
	virtual void GetFile() = 0;
	virtual void GetBufferString(zSTRING &) = 0;
	virtual void GetBuffer() = 0;
	virtual void EndOfArchive() = 0;
	virtual void Close() = 0;
	virtual void SetStringEOL(zSTRING	const &) = 0;
	virtual void GetStringEOL() = 0;
	virtual void IsStringValid(char const *) = 0;
	virtual void GetChecksumEnabled() = 0;
	virtual void SetChecksumEnabled(int) = 0;
	virtual void SetNoReadSearchCycles(int) = 0;
	virtual bool InProperties() = 0;
	virtual bool InSaveGame() = 0;
	virtual bool InBinaryMode() = 0;
	virtual void GetParentObject() = 0;
	virtual void OpenWriteBuffer(zCBuffer *,zTArchiveMode,int,int,int) = 0;
	virtual void OpenWriteFile(zFILE *,zTArchiveMode,int,int,int) = 0;
	virtual void WriteHeader(int) = 0;
	virtual void WriteHeaderNumObj() = 0;
	virtual void WriteASCIILine(char const *,char const	*,zSTRING const	&) = 0;
	virtual void StoreBuffer(void *,ulong) = 0;
	virtual void StoreString(char const *) = 0;
	virtual void StoreStringEOL(char const *) = 0;
	virtual void StoreGetPos() = 0;
	virtual void StoreSeek(ulong) = 0;
	virtual void OpenReadBuffer(zCBuffer &,zTArchiveMode,int,int) = 0;
	virtual void OpenReadFile(zFILE	*,zTArchiveMode,int,int,int) = 0;
	virtual void GetClassDefByString(zSTRING const &) = 0;
	virtual void CreateObject(zSTRING const &) = 0;
	virtual void SkipChunk(int) = 0;
	virtual void ReadChunkStartASCII(char const *,zSTRING &) = 0;
	virtual void ReadChunkEnd() = 0;
	virtual void ReadEntryASCII(char const *,zSTRING &) = 0;
	virtual void ReadHeader() = 0;
	virtual void RestoreBuffer(void *,ulong) = 0;
	virtual void RestoreStringEOL(zSTRING	&) = 0;
	virtual void RestoreString0(zSTRING &) = 0;
	virtual void RestoreGetPos() = 0;
	virtual void RestoreSeek(ulong) = 0;
	virtual void DeleteBuffer() = 0;

private:
	int useBuffer;
	int mode;
};
