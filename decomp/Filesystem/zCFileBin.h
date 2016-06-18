struct zCFileBIN {
	zCFileBIN() = default;
	~zCFileBIN()
	{
		if (file)
			BinClose();
	}

	void BinCreate(zSTRING const& fileName);
	void BinSetFile(zFILE* file);
	void BinClose();
	void BinWriteInt(int i);
	void BinWriteFloat(float f);
	void BinWriteString(zSTRING const& s);
	void BinStartChunk(unsigned short id);
	void BinEndChunk();
	void BinSkipChunk();

private:
	int chunk_start = -1;
	int unk1  = -1;
	int is_open = 0;
	zFILE *file = nullptr;
};

void zCFileBIN::BinCreate(zSTRING const& fileName)
{
	file = zfactory->CreateZFile(fileName);
	file->Create();
	chunk_start = -1;
}

void zCFileBIN::BinSetFile(zFILE *file)
{
	file = file;
	unk = -1;
	chunk_start = -1;
	is_open = 1;
} 

void zCFileBIN::BinWriteInt(int i)
{
	file->Write(&i, sizeof(int));
}

void zCFileBIN::BinWriteFloat(float f)
{
	file->Write(&f, sizeof(float));
}

void zCFileBIN::BinWriteString(zSTRING const& str)
{
	file->Write(str + "\n");
}

void zCFileBIN::BinEndChunk()
{
	if ( chunk_start != -1 ) {
		auto pos = file->Pos();
		auto sz = pos - chunk_start - 6;
		file->Seek(chunk_start + 2);
		file->Write(&sz, 4);
		file->Seek(pos);
	}
}

void zCFileBIN::BinStartChunk(unsigned short id)
{
	if ( file ) {
		BinEndChunk();

		unsigned dummy = 0;
		chunk_start = file->Pos();

		file->Write(&id, 2);
		file->Write(&dummy, 4); // size
	}
}

void zCFileBIN::BinClose()
{
	BinEndChunk();
	if ( !is_open ) {
		file->Close();
		Delete(file);
	}
	file = nullptr;
}

void zCFileBIN::BinSkipChunk()
{
	if ( file ) {
		if ( unk1 < file->Size() )
			file->Seek(unk1);
	}
}
