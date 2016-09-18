struct zFILE_STATS {
	zDATE ctime;
	zDATE atime;
	zDATE mtime;
	int size;
};

class zFILE {
	zFILE() = default;
	zFILE(zSTRING const&) : zFILE() {}
	virtual ~zFILE() = default;
	virtual void SetMode(long) = 0;
	virtual void GetMode() = 0;
	virtual void SetPath(zSTRING) = 0;
	virtual void SetDrive(zSTRING) = 0;
	virtual void SetDir(zSTRING) = 0;
	virtual void SetFile(zSTRING) = 0;
	virtual void SetFilename(zSTRING) = 0;
	virtual void SetExt(zSTRING) = 0;
	virtual void GetFileHandle() = 0;

	virtual zSTRING GetFullPath() = 0;
	virtual zSTRING GetPath() = 0;
	virtual zSTRING GetDirectoryPath() = 0;
	virtual zSTRING GetDrive() = 0;
	virtual zSTRING GetDir() = 0;
	virtual zSTRING GetFile() = 0;
	virtual zSTRING GetFilename() = 0;
	virtual zSTRING GetExt() = 0;

	virtual void SetCurrentDir() = 0;
	virtual void ChangeDir(bool) = 0;
	virtual void SearchFile(zSTRING const &,zSTRING const &,int) = 0;
	virtual void FindFirst(zSTRING const &) = 0;
	virtual void FindNext() = 0;
	virtual void DirCreate() = 0;
	virtual void DirRemove() = 0;
	virtual void DirExists() = 0;
	virtual void FileMove(zFILE *) = 0;
	virtual void FileMove(zSTRING,bool) = 0;
	virtual void FileCopy(zFILE *) = 0;
	virtual void FileCopy(zSTRING,bool) = 0;
	virtual void FileDelete() = 0;
	virtual void IsOpened() = 0;
	virtual void Create(zSTRING const &) = 0;
	virtual void Create() = 0;
	virtual void Open(zSTRING const &,bool) = 0;
	virtual void Open(bool) = 0;

	virtual bool Exists(zSTRING const &) = 0;
	virtual bool Exists() = 0;

	virtual void Close() = 0;
	virtual void Reset() = 0;
	virtual void Append() = 0;
	virtual void Size() = 0;
	virtual void Pos() = 0;
	virtual void Seek(long) = 0;
	virtual void Eof() = 0;
	virtual void GetStats(zFILE_STATS &) = 0;
	virtual void Write(void const *,long) = 0;
	virtual void Write(zSTRING const &) = 0;
	virtual void Write(char const *) = 0;
	virtual void Read(zSTRING &) = 0;
	virtual void Read(char *) = 0;
	virtual void Read(void *,long) = 0;
	virtual void ReadChar(char &) = 0;
	virtual void SeekText(zSTRING const &) = 0;
	virtual void ReadBlock(long,long) = 0;
	virtual void UpdateBlock(zSTRING const &,long,long) = 0;
	virtual void FlushBuffer() = 0;
	virtual void ReadCodeline(zSTRING &) = 0;
	virtual void ReadLines(zSTRING &,long) = 0;

	virtual long GetFreeDiskSpace() { return 102400; }

	virtual void DirStepInto(zSTRING) = 0;

	static void DirectFileAccess(bool direct, zSTRING& fileName)
	{
		DirectFileConvert(fileName);
		DirectFileAccess(direct);
	}

protected:
	bool unk0 = 0;
	int  unk1 = 0;
};
