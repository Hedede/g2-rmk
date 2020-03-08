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

// _dieter/zArchiver.cpp
zSTRING s_arcModeString[4] = {"BINARY", "ASCII", "ASCII_PROPS", "BIN_SAFE"};
zSTRING s_arcHeaderTag = "ZenGin Archive";

static int zCArchiverFactory::EndOfArchive(zCBuffer *buffer, zFILE *file)
{
	if ( file ) {
		if ( !file->Eof() ) {
			if ( file->Pos() + 3 <= file->Size() )
				return false;
		}
		return true;
	}

	return buffer->GetSize() - buffer->GetPos() < 0;
}

void zCArchiverFactory::ReadLine(zSTRING& line, zCBuffer *buffer, zFILE *file)
{
	if ( buffer )
	{
		char buf[1024];
		int i = 0;

		for (; i < 1023; ++i)
		{
			buffer->Read(&buf[i], 1);
			if (buf[i] == '\n')
				break;
		}

		if ( i >= 1023 )
			zFAULT("D: zCArchiverFactory: corrupt archive header, line too long"); // 145

		buf[i] = 0;

		line = buf;
	}
	else
	{
		file->Read(line);
	}
}

void zCArchiverFactory::ReadLineArg(zSTRING& line, zSTRING& arg, zCBuffer *buffer, zFILE *file)
{
	ReadLine(arg, buffer, file);

	line = arg.PickWord(1, " ", " ");
	arg  = arg.PickWord(2, " ", " ");
}

void zCArchiverFactory::ReadHeader(int arcFlags, zCBuffer *buffer, zFILE *file, zCArchiver*& arc, zTArchiveMode& arcMode, int& inSaveGame)
{
	arc = 0;
	arcMode = 0;
	inSaveGame = 0;

	zSTRING line;
	ReadLine(line, buffer, file);

	if (line != s_arcHeaderTag)
	{
		zFAULT("D: zCArchiverFactory(zCArchiverFactory::ReadHeader): file ist not a ZenGin Archive"); // 184
		return;
	}

	zSTRING arg;
	ReadLineArg(line, arg, buffer, file);

	int ver = arg.ToLong();

	if ( ver > 0 ) {
		ReadLine(line, buffer, file);

		auto classDef = zCClassDef::GetClassDef(line);
		if ( !classDef )
		{
			zFAULT("D: zCArchiverFactory(zCArchiverFactory::ReadHeader): could not create specified archiver"); //197
			return;
		}

		arc = classDef->CreateNewInstance();
	} else {
		arc = new zCArchiverGeneric; // 203
	}

	ReadLine(line, buffer, file);

	int mode = 0;
	for (; mode < 4; ++i)
	{
		if (line == s_arcModeString[mode])
			break;
	}

	arcMode = (zTArchiveMode)mode;

	if ( mode >= 4 )
	{
		zFAULT("D: zCArchiverFactory(zCArchiverFactory::ReadHeader): unknown arcMode"); // 216
		return;
	}

	while ( 1 )
	{
		int pos = file ? file->Pos() : buffer->GetPos();

		ReadLineArg(line, arg, buffer, file);

		if (line == "END")
			break;

		if (line == "saveGame")
		{
			inSaveGame = arg.ToLong();
		}
		else if (line == "objects")
		{
			// example of ver.0 archive: Lensflare.zen
			// in ver.1 they write "END" before "objects"
			if (ver <= 0)
			{
				if (file)
					file->Seek(pos)
				else
					buffer->SetPos(pos);
				break;
			}
		}

		if ( file )
		{
			if ( file->Eof() || (file->Pos() + 3 > file->Size()) )
				break;
		}
		else
		{
			if ( buffer->GetSize() - buffer->GetPos() < 0 )
				break;
		}
	}
}

zCArchiver* zCArchiverFactory::CreateArchiverRead(zCBuffer *buffer, int arcFlags)
{
	buffer.SetMode(0);
	unsigned int pos = 0;
	buffer.SetPos(pos);

	zCArchiver* arc = 0;
	zTArchiveMode arcMode = 0;
	int inSaveGame = 0;
	ReadHeader(arcFlags, buffer, 0, arc, arcMode, inSaveGame);
	if ( !arc )
		return 0;
	arc->OpenReadBuffer(buffer, arcMode, inSaveGame, arcFlags);
	return arc;
}

zCArchiver* zCArchiverFactory::CreateArchiverRead(zFILE *fileRead, int arcFlags)
{
	zTArchiveMode arcMode = 0;
	int inSaveGame = 0;
	ReadHeader(arcFlags, 0, fileRead, arc, arcMode, inSaveGame);
	if ( !arc )
		return 0;
	arc->OpenReadFile(fileRead, arcMode, inSaveGame, arcFlags);
	return arc;
}

zCArchive* zCArchiverFactory::CreateArchiverRead(const zSTRING& fileNameRead, int arcFlags)
{
	zFILE *file = zfactory->CreateZFile(zfactory, fileNameRead);

	if ( !file->Exists() || !file->Open(0) || file->Eof() || (file->Pos() + 3 > file->Size()))
	{
		zWARNING("D: zArchiver(::OpenFile): file does not exist: " + fileNameRead); //282
	}
	else
	{
		// was inined... actually, not sure if this was inlined or just copy-pasted code
		// (I suspect the latter)
		zCArchiver* arc = CreateArchiverRead(file, arcFlags);
		if (arc)
			return arc;
	}

	// actually, it calls only dtor, but doesn't free the memory...
	// (possibly the archiver deletes it later?)
	delete file;
	return 0;
}

void zCArchiverFactory::WriteLine(const zSTRING& line, zCBuffer *buffer, zFILE *file)
{
	if ( buffer )
	{
		buffer->Write(line.Data(), line.Length());
		buffer->WriteByte('\n', 8);
	}
	else
	{
		file->Write(line);
		file->Write(zSTRING('\n'));
	}
}

void zCArchiverFactory::WriteLine(const char* line, zCBuffer *buffer, zFILE *file)
{
	auto length = strlen(line);
	if ( buffer )
	{
		buffer->Write(line, length);
		buffer->WriteByte('\n', 8);
	}
	else
	{
		file->Write(line, length);
		file->Write(zSTRING('\n'));
	}
}

void  zCArchiverFactory::WriteHeader(zCArchiver *arc, zTArchiveMode arcMode, int saveGame, char arcFlags, zCBuffer *buffer, zFILE *file)
{
	// all "WriteLine" were inlined except for the last one
	WriteLine( s_arcHeaderTag, buffer, file );
	WriteLine( "ver 1", buffer, file );

	auto classDef = arc->_GetClassDef();
	WriteLine( classDef->GetClassName(), buffer, file );
	WriteLine( s_arcModeString[arcMode], buffer, file );

	if ( saveGame )
	{
		WriteLine("saveGame 1");
	}
	else
	{
		WriteLine("saveGame 0");
	}

	if ( !(arcFlags & 1) )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		zSTRING date = "date " + st.wDay + "." + st.wMonth + "." + st.wYear + " " + st.wHour + ":" + st.wMinute + ":" + st.wSecond;
		WriteLine(date, buffer, file);

		char userName[200];
		int userNameSize = 200;
		if ( GetUserNameA(userName, &userNameSize) )
		{
			zSTRING user = "user " + userName;
			WriteLine(user, buffer, file);
		}
	}

	WriteLine("END", buffer, file);
}

zCArchiver* zCArchiverFactory::CreateArchiverFromMode(zTArchiveMode mode)
{
	if ( mode == zARC_MODE_BIN_SAFE )
	{
		return new zCArchiverBinSafe; // 361
	}
	else
	{
		return new zCArchiverGeneric; // 364
	}
	return 0;
}

zCArchiver* zCArchiverFactory::CreateArchiverWrite(zCBuffer *buffer, zTArchiveMode arcMode, int saveGame, int arcFlags)
{
	bool arcOwnsMedium = buffer == nullptr;
	if (arcOwnsMedium)
	{
		buffer = new zCBuffer(65536); // 373
	}

	zCArchiver *arc = CreateArchiverFromMode(arcMode);
	WriteHeader(arc, arcMode, saveGame, arcFlags, buffer, 0);
	arc->OpenWriteBuffer(arc, buffer, arcMode, saveGame, arcFlags, arcOwnsMedium);
	return arc;
}
zCArchiver* zCArchiverFactory::CreateArchiverWrite(zTArchiveMode arcMode, int saveGame, int arcFlags)
{
	return CreateArchiverWrite((zCBuffer*)nullptr, arcMode, saveGame, arcFlags);
}

zCArchiver* zCArchiverFactory::CreateArchiverWrite(zFILE *fileWrite, zTArchiveMode arcMode, int saveGame, int arcFlags)
{
	zCArchiver *arc = CreateArchiverFromMode(arcMode);
	WriteHeader(arc, arcMode, saveGame, arcFlags, 0, fileWrite);
	arc->OpenWriteFile(arc, fileWrite, arcMode, saveGame, arcFlags, 0);
	return arc;
}

 zCArchiver* zCArchiverFactory::CreateArchiverWrite(zSTRING const& fileNameWrite, zTArchiveMode arcMode, int saveGame, int arcFlags)
{
	auto fileWrite = zfactory->CreateFile(fileNameWrite);
	if (fileWrite->Create() == 0)
	{
		delete file;
		return nullptr;
	}

	zCArchiver *arc = CreateArchiverFromMode(arcMode);
	WriteHeader(arc, arcMode, saveGame, arcFlags, 0, fileWrite);
	arc->OpenWriteFile(arc, fileWrite, arcMode, saveGame, arcFlags, 1);
	return are;
}
