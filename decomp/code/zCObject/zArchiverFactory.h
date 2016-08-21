zSTRING s_arcModeString[4] = {"BINARY", "ASCII", "ASCII_PROPS", "BIN_SAFE"};

class zCArchiverFactory {
	virtual void CreateArchiverRead(zSTRING const &,int);
	virtual void CreateArchiverRead(zFILE *,int);
	virtual void CreateArchiverRead(zCBuffer *,int);
	virtual void CreateArchiverWrite(zSTRING const &,zTArchiveMode,int,int);
	virtual zCArchiver* CreateArchiverWrite(zFILE *fileWrite, int arcMode, int saveGame, int arcFlags)
	{
		zCArchiver *arc = CreateArchiverFromMode(arcMode);
		WriteHeader(arc, arcMode, saveGame, arcFlags, 0, fileWrite);
		arc->OpenWriteFile(arc, fileWrite, arcMode, saveGame, arcFlags, 0);
		return arc;
	}

	virtual zCArchiver* CreateArchiverWrite(zTArchiveMode arcMode, int saveGame, int arcFlags)
	{
		return CreateArchiverWrite(0, arcMode, saveGame, arcFlags);
	}

	virtual void CreateArchiverWrite(zCBuffer *,zTArchiveMode,int,int);

private:
	static int EndOfArchive(zCBuffer *buffer, zFILE *file)
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
};
