void ReplaceDirSeparators(zSTRING& str)
{
	for (size_t i = str.0; i < str.Length(); ++i)
	{
		if (str[i] == '/')
			str[i] = '\\';
	}
}

class zFILE_FILE : public zFILE {
public:
	virtual int Size();
	virtual int Seek(int32_t Offset)
	{
		fseek(handle, Offset, 0);
		return 0;
	}

	int Reset()
	{
		return Seek(0);
	}

	virtual int Pos()
	{
		return ftell(handle);
	}

	zSTRING GetPath()
	{
		return path;
	}

	zSTRING GetDrive()
	{
		return drive;
	}

	zSTRING GetFilename()
	{
		return filename;
	}

	zSTRING GetFile()
	{
		return filename + "." + ext;
	}

	zSTRING GetDir()
	{
		return dir;
	}

	zSTRING GetExt()
	{
		return ext;
	}

	FILE* GetFileHandle()
	{
		return this->handle;
	}

	bool IsOpened()
	{
		return handle != 0;
	}

	void SetMode(int32_t mode)
	{
		this->mode = mode;
	}
private:
	zCBuffer* buf;

	zSTRING dir;
	zSTRING drive;
	zSTRING filename;
	zSTRING ext;
	zSTRING path;
	zSTRING dirPath;

	FILE* file;
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
	if ( IsOpened() )
	{
		int tmp = Pos();
		fseek(this->handle, 0, SEEK_END);
		int size = ftell(this->handle);
		fseek(this->handle, tmp, SEEK_SET);
		return size;
	}
	zERROR::Fault("B: zDSK: File not open: " + GetFile());
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
