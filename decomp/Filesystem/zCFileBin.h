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

void zCFileBIN::BinSetFile(zFILE *file)
{
	this->file = file;
	this->unk[1] = -1;
	this->unk[0] = -1;
	this->unk[2] = 1;
} 
