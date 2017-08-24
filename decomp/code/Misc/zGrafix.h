// Unused class: Since G1demo
// _Ulf/Zgrafix.cpp
struct TGfx_Gfx {
	int unk;
	int bpp;
	int w;
	int h;
	int unk1;
	char *pixels;
};

int zCGfx::GetSizex()
{
	if (!data)
		return 0;
	return data->w;
}

int zCGfx::GetSizey()
{
	if (!data)
		return 0;
	return data->h;
}

char* zCGfx::GetDataPtr()
{
	if (!data)
		return nullptr;
	return data->pixels;
}

int zCGfx::GetPixel(int x, int y)
{
	if (!data)
		return nullptr;
	int w = data->w;
	// Strange, why does it use word ptr if he allocates bytes?
	return ((WORD*)data->pixels)[x + y*w];
}

TGfx_Gfx* zCGfx::LoadRaw(zSTRING const& s, int w, int h, int bpp)
{
	zFILE::DirectFileAccess(1);

	TGfx_Gfx* gfx = nullptr;
	if (FILE* file = fopen( s.Data(), "rb" )) {
		gfx = new TGfx_Gfx; // 325
		gfx->unk = 0;
		gfx->pixels = new BYTE[ h*w ]; // 328
		fread( gfx->pixels, h * w * (bpp >> 3), 1, file );
		gfx->w = w;
		gfx->h = h;
		gfx->bpp = zrenderer->view_bpp;
	}
	zFILE::DirectFileAccess(0);
	return gfx;
}

TGfx_Gfx* zCGfx::LoadGfx(zSTRING const& s)
{
	zINFO(8, "U:(zGrafix) Loading Gfx : " + s); //830

	TGfx_Gfx* gfx = nullptr;
	if (s.Search(0, ".PCX", 1) > 0) {
		gfx = LoadPcx( s );
	} else if (s.Search(0, ".TEX", 1) > 0) {
		gfx = LoadRaw( s, 64, 64, 8 );
	} else if (s.Search(0, ".TGA", 1) > 0) {
		gfx = LoadTga( s );
	}

	if (!gfx)
		zINFO("U:(zGrafix) Failed.");
	return gfx;
}

int count = 0;
BYTE* Cache;
BYTE* CacheC;
char zCGfx::Readbyte(zFILE *file)
{
	if ( count < 0 || count >= 5000 ) {
		file->Pos();
		file->Size();
		file->Read(Cache, 5000);
		CacheC = Cache;
	}
	++count;
	return *CacheC++;
}
