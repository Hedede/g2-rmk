class zCTexture : public zCResource, public zCTextureExchange {
	Z_OBJECT(zCTexture);
public:
	virtual ~zCTexture();
	void LoadResourceData() override;
	void ReleaseResourceData() override;
	void GetResSizeBytes() override;
	void GetCacheConfig(ulong &,ulong &) override;

	virtual bool GetChromaKeyingEnabled() { return false; }
	virtual void SetChromaKeyingEnabled(int) {}

	virtual void HasAlpha()
	{
		return hasAlpha;
	}

private:
	zCTexture*  nextAni     [3];
	zCTexture*  prevAni     [3];
	int         actAniFrame [3];
	int         numAniFrames[3];

	struct {
		unsigned hasAlpha         : 1;
		unsigned isAnimated       : 2;
		unsigned changingRealTime : 4;
		unsigned isTextureTile    : 8;
	} flags;
};


class zCTextureInfo {
	zTRnd_TextureFormat texFormat;
	int                 sizeX;
	int                 sizeY;
	int                 numMipMap;
	int                 refSizeX;
	int                 refSizeY;
	zCOLOR              averageColor;
};

class zCTextureHeader {
	uint32_t Version;
	uint32_t Signature;
};

class zCSurfaceCache_D3D::zCSlotIndex {
	int dynamic;
	int pixelformat;
	int mipmaps;
	int width;
	int height;
};

class zCTex_D3D : public zCTexture {
public:
	virtual ~zCTex_D3D();
	virtual void Lock(int);
	virtual void Unlock();
	virtual void SetTextureInfo(zCTextureInfo const &);
	virtual void GetPaletteBuffer();
	virtual void GetTextureBuffer(int,void *&,int &);
	virtual void GetTextureInfo();
	virtual void CopyPaletteDataTo(void *);
	virtual void CopyTextureDataTo(int,void *,int);
	virtual void ReleaseData();

private:
	int xtex_textureflag; //tends to be 0xDEADFACE
	struct {
		unsigned unk : 17;
		unsigned decompress : 1;  // << 17
		unsigned locked     : 1;  // << 18
		unsigned palsupport : 1;  // << 19
		unsigned miplocked  : 12; // << 20
	} xtex_bitfield;

	zCTextureInfo xtex_texinfo;

	zCSurfaceCache_D3D::zCSlotIndex xtex_slotindex;

	char*                      xtex_pPalettePtr;
	IDirectDrawPalette*        xtex_pddpal;
	int                        xtex_internalnumber;
	zBOOL                      xtex_alphaTex;
	IDirectDrawSurface7*       xtex_pddtex[12];
	IDirectDrawSurface7*       xtex_pddtemporarytex[12];
	int                        xtex_lastFrameUsed;
	void*                      xtex_buffer;
};

int zCTextureFileFormatInternal::ReadHeader(zFILE& file)
{
	zCTextureHeader header;
	// memset(&header.TexInfo, 0, sizeof(header.TexInfo));

	file.Read(&header, 36);
	if ( header.Version != 0 )
		return 0;

	memcpy(&this->texInfo, &header.TexInfo, sizeof(zCTextureInfo));
	return 1;
}

//--------------------------------------------------------------------------------
void zCTexture::InitValues()
{
	flags.hasAlpha = 0;
	flags.isAnimated = 0;
	
	for (int i : range(3)) {
		nextAni      = 0;
		prevAni      = 0;
		actAniFrame  = 0;
		numAniFrames = 0;
	}
}

// static
zCTexture* zCTexture::SearchName(zSTRING& name)
{
	name.Upper();
	return zCTexture::classDef.SearchHashTable(name);
}

zCTexture* zCTexture::GetAniTexture()
{
	zCTexture* result = this;
	if ( flags.isAnimated ) {
#if 0
		// I hate funtions like this, it looked like this:
		auto frame = actAniFrame;
		auto bullshit = -24 - this;
		for (...)
		{
			for (...) {
				if (!*(frame + result + bullshit))
					break;
				result = *(frame + result + bullshit);
			}

			++frame;
		}
#endif
		
		for (int i = 3; i > 0; --i) {
			if ( actAniFrame[i] ) {
				int e = actAniFrame[i];
				for (int j = 0; j<e;) {
					if (!result->nextAni[i])
						break;
					result = result->nextAni[i]; // moved here for clarity
				}
			}
		}
	}
	return result;
}

bool zCTexture::IsTextureTileBaseSlice()
{
	if ( flags.isTextureTile & 8 ) {
		int    i = 0;
		auto** p = prevAni;
		while (!*p) {
			++i;
			++p;
			if (i >= 3)
				return 1;
		}
	}
	return 0;
}
