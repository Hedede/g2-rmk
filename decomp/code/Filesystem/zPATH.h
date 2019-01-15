struct zPATH {
	zPATH(zSTRING name)
	{
		file = zfactory->CreateZFile( name );
	}
	zPATH() : zPATH("") { }
	~zPATH() { delete file; }

	int SearchFile(const zSTRING& fname, const zSTRING& searchDir, zBOOL rec)
	{
		return file->SearchFile(fname, searchDir, rec);
	}

	zSTRING GetFile() { return file->GetFile(); }
	zSTRING GetDrive() { return file->GetDrive(); }
	zSTRING GetDir() { return file->GetDir(); }
	zSTRING GetExt() { return file->GetExt(); }
	zSTRING GetFilename() { return file->GetFilename(); }
	zSTRING GetDirectoryPath() { return file->GetDirectoryPath(); }

private:
	zFILE *file;
};
