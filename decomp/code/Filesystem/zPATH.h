struct zPATH {
	int SearchFile(const zSTRING& fname, const zSTRING& searchDir, zBOOL rec)
	{
		return file->SearchFile(fname, searchDir, rec);
	}

private:
	zFILE *file;
};
