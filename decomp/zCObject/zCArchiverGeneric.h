class zCArchiverGeneric : public zCArchiver {
	Z_OBJECT(zCArchiverGeneric);
private:
	static zSTRING stringEOL;
public:
	static bool IsStringValid(const char* str)
	{
		for (auto ptr = str; *ptr; ++ptr) {
			if (in(*ptr, '=', '%', '§', '[', ']'))
				return false;
		}
		return true;
	}

	static zSTRING GetStringEOL()
	{
		return stringEOL;
	}

public:
	virtual ~zCArchiverGeneric();
	virtual void WriteInt(char const *,int);
	virtual void WriteByte(char const *,uchar);
	virtual void WriteWord(char const *,ushort);
	virtual void WriteFloat(char const *,float);
	virtual void WriteBool(char const *,int);
	virtual void WriteString(char const *,zSTRING const &);
	virtual void WriteVec3(char	const *,zVEC3 const &);
	virtual void WriteColor(char const *,zCOLOR const	&);
	virtual void WriteRaw(char	const *,void *,ulong);
	virtual void WriteRawFloat(char const	*,void *,ulong);
	virtual void WriteEnum(char	const *,char const *,int);

	virtual void WriteObject(char const* name, zCObject& obj);
	virtual void WriteObject(char const* name, zCObject* obj);
	virtual void WriteObject(zCObject* obj)
	{
		WriteObject(this, "", obj);
	}

	virtual void WriteChunkStart(char const *,char const*,ushort,ulong);
	virtual void WriteChunkStart(char const *,ushort);
	virtual void WriteChunkEnd();
	virtual void WriteGroupBegin(char const	*);
	virtual void WriteGroupEnd(char const *);
	virtual void ReadInt(char const	*);
	virtual void ReadInt(char const *,int &);
	virtual void ReadByte(char const *);
	virtual void ReadByte(char const *,uchar &);
	virtual void ReadWord(char const *);
	virtual void ReadWord(char const *,ushort &);
	virtual void ReadFloat(char const *);
	virtual void ReadFloat(char const *,float &);
	virtual void ReadBool(char const *);
	virtual void ReadBool(char const *,int &);
	virtual void ReadString(char const *);
	virtual void ReadString(char const *,zSTRING &);
	virtual void ReadVec3(char const *);
	virtual void ReadVec3(char const *,zVEC3 &);
	virtual void ReadColor(char	const *);
	virtual void ReadColor(char const *,zCOLOR	&);
	virtual void ReadEnum(char const *);
	virtual void ReadEnum(char const *,int &);
	virtual void ReadRaw(char const *,void *,ulong);
	virtual void ReadRawFloat(char	const *,void *,ulong);

	virtual void ReadObject(char const* name, zCObject* obj);
	virtual void ReadObject(zCObject* obj)
	{
		return ReadObject("", obj);
	}

	virtual void ReadChunkStart(char const *);
	virtual void ReadChunkStart(zSTRING&, ushort&);
	virtual void ReadChunkStartNamed(char const *,ushort &);
	virtual void SkipOpenChunk();
	virtual void GetCurrentChunkVersion();
	virtual zFILE* GetFile();
	virtual void GetBufferString(zSTRING &);
	virtual void GetBuffer();
	virtual void EndOfArchive();
	virtual void Close();
	virtual void SetStringEOL(zSTRING const &);
	virtual void GetStringEOL();
	virtual bool IsStringValid(char const* str)
	{
		return IsStringValid(str);
	}
	virtual int GetChecksumEnabled() const
	{
		return checksumEnabled;
	}
	virtual void SetChecksumEnabled(int enabled)
	{
		checksumEnabled = enabled;
	}
	virtual SetNoReadSearchCycles(int val)
	{
		noReadSearchCycles = val;
	}
	virtual void InProperties();
	virtual bool InSaveGame() const
	{
		return inSaveGame;
	}
	virtual void InBinaryMode();
	virtual void GetParentObject();
	virtual void OpenWriteBuffer(zCBuffer *,zTArchiveMode,int,int,int);
	virtual void OpenWriteFile(zFILE *,zTArchiveMode,int,int,int);
	virtual void WriteHeader(int);
	virtual void WriteHeaderNumObj();
	virtual void WriteASCIILine(char const *,char const	*,zSTRING const	&);
	virtual void StoreBuffer(void *,ulong);
	virtual void StoreString(char const	*);
	virtual void StoreStringEOL(char	const *);
	virtual void StoreGetPos();
	virtual void StoreSeek(ulong);
	virtual void OpenReadBuffer(zCBuffer &,zTArchiveMode,int,int);
	virtual void OpenReadFile(zFILE	*,zTArchiveMode,int,int,int);
	virtual void GetClassDefByString(zSTRING const &);
	virtual void CreateObject(zSTRING const &);
	virtual void SkipChunk(int);
	virtual void ReadChunkStartASCII(char const *,zSTRING &);
	virtual void ReadChunkEnd();
	virtual void ReadEntryASCII(char const *,zSTRING &);
	virtual void ReadHeader();
	virtual void RestoreBuffer(void *,ulong);
	virtual void RestoreStringEOL(zSTRING	&);
	virtual void RestoreString0(zSTRING &);
	virtual void RestoreGetPos();
	virtual void RestoreSeek(ulong);
	virtual void DeleteBuffer();
private:
	int DebugMessage(zSTRING const& msg);
	void CheckObjectListSize(int numRequired);

};
