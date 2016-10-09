class zCTextureConvert : zCTextureExchange {
	virtual void ConvertTextureFormat(zCTextureInfo const &) = 0;
	virtual void SetDetailTextureMode(int) = 0;

	static int CalcNumMipMaps(unsigned x, unsigned y)
	{
		int result = 1;
		while ( x > 4 && y > 4 ) {
			++result;
			x >>= 1;
			y >>= 1;
		}
		if ( result > 0 )
			--result;
		return result;
	}

	static void CorrectPow2(int& xdim, int& ydim)
	{
		int cx = xdim;
		do {
			xdim = cx;
			cx &= cx - 1;
		} while (cx);
		int cy = ydim;
		do {
			ydim = cy;
			cy &= cy - 1;
		} while (cy);
	}

	bool SaveToFileFormat(zFILE *file, zCTextureFileFormat *fileFormat)
	{
		if ( fileFormat->CanSave() )
			return fileFormat->SaveTexture(file, this);
		return 0;
	}

	bool LoadFromFileFormat(zFILE *file, zCTextureFileFormat *fileFormat)
	{
		if ( fileFormat->CanLoad() )
			return fileFormat->LoadTexture(file, this);
		return 0;
	}

	int ConvertToNewFormat(zTRnd_TextureFormat const& format)
	{
		zCTextureInfo ti;
		GetTextureInfo(&ti);
		ti.texFormat = format;
		return ConvertTextureFormat(&ti);
	}

	int GenerateMipMaps()
	{
		zCTextureInfo ti;
		GetTextureInfo(&ti);

		ti.numMipMap = CalcNumMipMaps(ti.sizeX, ti.sizeY);
		return ConvertTextureFormat(&ti);
	}


};
