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
public:
	zCArchiverGeneric();
	virtual ~zCArchiverGeneric();

	virtual void WriteInt(char const *,int);
	virtual void WriteByte(char const *,uchar);
	virtual void WriteWord(char const *,ushort);
	virtual void WriteFloat(char const *,float);
	virtual void WriteBool(char const *, uint8_t);
	virtual void WriteString(char const *,zSTRING const &);
	virtual void WriteVec3(char	const *,zVEC3 const &);
	virtual void WriteColor(char const *,zCOLOR const&);

	virtual void WriteEnum(char	const *,char const *,int);

	virtual void WriteRaw(char	const *,void *,ulong);
	virtual void WriteRawFloat(char const	*,void *,ulong);

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

	virtual int32_t  ReadInt(char const* name);
	virtual void     ReadInt(char const* name,int& value);

	virtual uint8_t  ReadByte(char const* name);
	virtual void     ReadByte(char const* name, uint8_t& byte);

	virtual uint16_t ReadWord(char const* name);
	virtual void     ReadWord(char const* name,uint16_t& word);

	virtual float    ReadFloat(char const* name);
	virtual void     ReadFloat(char const* name, float& value);

	virtual bool     ReadBool(char const* name);
	virtual void     ReadBool(char const* name,zBOOL& val);

	virtual zSTRING  ReadString(char const* name);
	virtual void     ReadString(char const* name, zSTRING& str);

	virtual zVEC3    ReadVec3(char const* name);
	virtual void     ReadVec3(char const* name,zVEC3& vec);

	virtual zCOLOR   ReadColor(char const* name);
	virtual void     ReadColor(char const* name,zCOLOR& color);

	virtual uint32_t ReadEnum(char const* name);
	virtual void     ReadEnum(char const* name, ulong& enu);

	virtual void     ReadRaw(char const* name, void *, size_t size);
	virtual void     ReadRawFloat(char const* name,void *, size_t size);

	virtual void ReadObject(char const* name, zCObject* obj);
	virtual void ReadObject(zCObject* obj)
	{
		return ReadObject("", obj);
	}

	virtual void ReadChunkStart(char const *);
	virtual void ReadChunkStart(zSTRING&, ushort&);
	virtual void ReadChunkStartNamed(char const *,ushort &);

	virtual void SkipOpenChunk()
	{
		SkipChunk(1);
	}
	virtual void GetCurrentChunkVersion();

	virtual zFILE* GetFile();
	virtual void GetBufferString(zSTRING &);
	virtual void GetBuffer();
	virtual void EndOfArchive();
	virtual void Close();

	virtual void SetStringEOL(zSTRING const& eol)
	{
		stringEOL = eol;
	}

	virtual void GetStringEOL() const
	{
		return stringEOL;
	}

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

	virtual bool InProperties() const
	{
		return this->mode == ARC_MODE_ASCII_PROPS;
	}

	virtual bool InSaveGame() const
	{
		return inSaveGame;
	}

	virtual void InBinaryMode()
	{
		return mode == ARC_MODE_BINARY || mode == ARC_MODE_BIN_SAFE;
	}

	virtual void GetParentObject();
	virtual void OpenWriteBuffer(zCBuffer *,zTArchiveMode,int,int,int);
	virtual void OpenWriteFile(zFILE *,zTArchiveMode,int,int,int);

	virtual void WriteHeader(int);
	virtual void WriteHeaderNumObj();
	virtual void WriteASCIILine(char const *,char const*,zSTRING const&);

	virtual void StoreBuffer(void*, ulong);
	virtual void StoreString(char const*);
	virtual void StoreStringEOL(char const *);
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

private:
	zFILE*    file   = nullptr;
	zCBuffer* buffer = nullptr;

	zBOOL arcOwnsMedium = 0;
	zBOOL inSaveGame    = 0;
	zBOOL inBinaryMode  = 0;
	zBOOL rwMode        = 0;

	zCArray<zTChunkRecord> chunkRecords;
	zCArray<zCObject*> objectList;


	int __maybe_headerstart;
	int unk_2;
	int __matbe_headerend;

	int checksumEnabled    = 0;
	int arcFlags           = 0;
	int notInProperties    = 1;
	int noReadSearchCycles = 0;

	zSTRING tempString;

	int unk_1;

	zCSparseArray<zCObject*,zTWriteObjectEntry> writeObjectEntries;

	int __skipped_some_chunks;
	int unk_6[2];
};
