//_bert\\zDisk.cpp 
void ReplaceDirSeparators(zSTRING& str)
{
	for (size_t i = str.0; i < str.Length(); ++i) {
		if (str[i] == '/')
			str[i] = '\\';
	}
}


struct zFILE_FILE : public zFILE {
	static bool InitFileSystem()
	{
		if ( !zFILE::s_virtPath )
			zFILE::s_virtPath = new zFILE_FILE();
		return zFILE::InitFileSystem();
	}
	static bool DeinitFileSystem()
	{
		if ( zFILE::s_virtPath )
			delete zFILE::s_virtPath;
		return zFILE::DeinitFileSystem();
	}

	zFILE_FILE() : zFILE() { Init(""); }
	zFILE_FILE(zSTRING const& path) : zFILE(path) { Init(path); }
	~zFILE_FILE() override
	{
		if (IsOpened())
			Close();
	}

	void Init(zSTRING const& path)
	{
		SetPath(path);
		handle = 0;
		unkf2 = 0;
		buf = 0;
		unkf1 = 0;
		unk1 = 0;
	}

	void SetMode(int32_t mode) override
	{
		this->mode = mode;
	}

	int32_t GetMode() const override
	{
		return mode;
	}

	void SetPath(zSTRING) override;
	void SetDrive(zSTRING) override;
	void SetDir(zSTRING) override;
	void SetFile(zSTRING) override;
	void SetFilename(zSTRING) override;
	void SetExt(zSTRING) override;

	FILE* GetFileHandle() override
	{
		return this->handle;
	}

	zSTRING GetFullPath() override;
	zSTRING GetPath()  override { return path; }
	zSTRING GetDirectoryPath() override { return dirPath; }
	zSTRING GetDrive() override { return drive; }
	zSTRING GetDir()   override { return dir; }
	zSTRING GetFile()  override { return filename + "." + ext; }
	zSTRING GetFilename() override { return filename; }
	zSTRING GetExt() override { return ext; }

	void SetCurrentDir() override;
	void ChangeDir(bool) override;

	void SearchFile(zSTRING const &,zSTRING const &,int) override;
	void FindFirst(zSTRING const &) override;
	void FindNext() override;
	void DirCreate() override;
	void DirRemove() override;
	void DirExists() override;

	void FileMove(zFILE *) override;
	void FileMove(zSTRING,bool) override;
	void FileCopy(zFILE *) override;
	void FileCopy(zSTRING,bool) override;
	bool FileDelete() override
	{
		if (IsOpened())
			Close();
		return remove(GetFullPath().Data());
	}

	bool IsOpened() override { return file != 0; }

	bool Create(zSTRING const& path) override
	{
		SetPath(path);
		return Create();
	}
	bool Create() override;
	bool Open(zSTRING const& path, bool writeMode) override
	{
		SetPath(path);
		return Open(writeMode);
	}
	bool Open(bool writeMode) override;
	bool Exists(zSTRING const& path) override
	{
		SetPath(path);
		return Exists();
	}
	bool Exists() override
	{
		if (file)
			return true;
		return _access(GetFullPath().Data(), 0) == 0;
	}
	void Close() override;

	int Reset() override { return Seek(0); }

	void Append() override { fseek(file, 0, SEEK_END); }
	void Size() override;
	int Pos() override { return ftell(handle); }
	int Seek(int32_t offset) override
	{
		fseek(file, offset, 0);
		return 0;
	}

	int Eof() override { return feof(file); }

	void GetStats(zFILE_STATS &) override;
	int Write(void const* src, long count) override;
	int Write(zSTRING const& str) override
	{
		return Write(str.Data);
	}
	int Write(char const *) override;
	int Read(zSTRING &) override;
	int Read(char* buf) override
	{
		fgets(buf, 1024, file);
	}
	int Read(void *,long) override;
	int ReadChar(char& c) override
	{
		Read(&c, 1) != 1 ? 2 : 0;
	}
	void SeekText(zSTRING const &) override;
	void ReadBlock(long,long) override;
	void UpdateBlock(zSTRING const &,long,long) override;
	void FlushBuffer() override;
	void ReadCodeline(zSTRING &) override;
	void ReadLines(zSTRING &,long) override;
	void DirStepInto(zSTRING) override;

private:
	zCBuffer* buf;

	zSTRING dir;
	zSTRING drive;
	zSTRING filename;
	zSTRING ext;
	zSTRING path;
	zSTRING dirPath;

	int unk;

	FILE* file; // handle
	int mode;

	int uunk[70];
	int unkf2;
}

size_t zFILE_FILE::FlushBuffer()
{
	int result = 0;
	if ( buf )
	{
		int bufSize = buf->GetSize();
		char* bufPtr = buf->GetBuffer();
		result = fwrite(bufPtr, bufSize, 1u, handle);
		buf->Reset();
	}
	return result;
};

int zFILE_FILE::Size()
{
	if ( IsOpened() ) {
		int tmp = Pos();
		fseek(this->handle, 0, SEEK_END);
		int size = ftell(this->handle);
		fseek(this->handle, tmp, SEEK_SET);
		return size;
	}
	zFAULT("B: zDSK: File not open: " + GetFile()); // 485
	return -1;
}

void zFILE_FILE::SetFilename(zSTRING str)
{
	ReplaceDirSeparators(str);

	filename = str;
	filename.Upper();

	SetCompletePath();
}

void zFILE_FILE::SetFilename(zSTRING str)
{
	int dot = str.Search(0, ".", 1);
	if (dot == -1)
		filename = str;
	
	str.Copied(filename, 0, dot);
	str.Copied(ext, dot + 1, str.Length());

	SetCompletePath();
}


void zFILE_FILE::SetExt(zSTRING str)
{
	ReplaceDirSeparators(str);

	ext = str;
	ext.Upper();

	SetCompletePath();
}

void zFILE_FILE::SetDrive(zSTRING str)
{
	ReplaceDirSeparators(str);

	drive = str;
	drive.Upper();
	SetCompletePath();
}

void zFILE_FILE::SetDir(zSTRING str)
{
	ReplaceDirSeparators(str);

	dir = str;
	dir.Upper();

	SetCompletePath();
}

zSTRING zFILE_FILE::SetCurrentDir()
{
	SetPath(zFILE::s_virtPathString);
	return GetPath();
}

long zFILE_FILE::Read(void *scr, long bytes)
{
	long ret = fread(scr, bytes, 1u, file);
	if ( ret < 0 )
		HandleError("Read", path, zERR_TYPE_WARNING);
	return bytes * ret;
}
