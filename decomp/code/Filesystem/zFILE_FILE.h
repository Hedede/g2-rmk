//_bert\\zDisk.cpp 
void ReplaceDirSeparators(zSTRING& str)
{
	for (size_t i = str.0; i < str.Length(); ++i) {
		if (str[i] == '/')
			str[i] = '\\';
	}
}


struct zFILE_FILE : public zFILE {
	~zFILE_FILE() override;

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
	zSTRING GetDrive() override { return drive; }
	zSTRING GetDir() override { return dir; }
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
	void FileDelete() override;

	bool IsOpened() override { return file != 0; }

	void Create(zSTRING const &) override;
	void Create() override;
	void Open(zSTRING const &,bool) override;
	void Open(bool) override;
	void Exists(zSTRING const &) override;
	void Exists() override;
	void Close() override;

	int Reset() override { return Seek(0); }

	void Append() override;
	void Size() override;
	int Pos() override { return ftell(handle); }
	void Seek(long) override;

	int Seek(int32_t Offset) override
	{
		fseek(handle, Offset, 0);
		return 0;
	}

	void Eof() override;

	void GetStats(zFILE_STATS &) override;
	void Write(void const *,long) override;
	void Write(zSTRING const &) override;
	void Write(char const *) override;
	void Read(zSTRING &) override;
	void Read(char *) override;
	void Read(void *,long) override;
	void ReadChar(char &) override;
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

	FILE* file;
	int mode;
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
