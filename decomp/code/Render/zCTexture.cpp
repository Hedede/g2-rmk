void GiveFatal()
{
	zFATAL("D: This method should be overriden! Illegal zCResource class setup!");
}

class zCResource : public zCObject {
	Z_OBJECT(zCResource);
public:
	virtual ~zCResource();

	virtual void LoadResourceData()
	{
		GiveFatal();
	}

	virtual void ReleaseResourceData()
	{
		GiveFatal();
	}

	virtual size_t GetResSizeBytes()
	{
		GiveFatal();
		return 0;
	}

	virtual void GetCacheConfig(ulong &,ulong &)
	{
		GiveFatal();
	}

private:
	zCResource*  nextRes;
	zCResource*  prevRes;
	uint32_t     timeStamp;

	zCCriticalSection stateChangeGuard;

	uint8_t cacheState; // : 2;
	bool cacheOutLock;
	uint8_t cacheClassIndex;
	bool managedByResMan;;

	short cacheInPriority;
	short canBeCachedOut;
};



class zCTexture : public zCResource, public zCTextureExchange {
	Z_OBJECT(zCTexture);
public:
	virtual ~zCTexture();
	virtual void LoadResourceData();
	virtual void ReleaseResourceData();
	virtual void GetResSizeBytes();
	virtual void GetCacheConfig(ulong &,ulong &);
	virtual void GetChromaKeyingEnabled();
	virtual void SetChromaKeyingEnabled(int);

	virtual void HasAlpha();
private:
	zCTexture*  nextAni     [3];
	zCTexture*  prevAni     [3];
	int         actAniFrame [3];
	int         numAniFrames[3];

	bool hasAlpha;
	bool isAnimated;
	bool changingRealTime;
	bool isTextureTile;
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
