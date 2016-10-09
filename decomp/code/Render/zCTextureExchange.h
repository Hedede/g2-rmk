class zCTextureExchange {
	virtual ~zCTextureExchange() = default;

	virtual void Lock(int) = 0;
	virtual void Unlock() = 0;

	virtual void SetTextureInfo(zCTextureInfo const &) = 0;
	virtual void GetPaletteBuffer() = 0;

	virtual void GetTextureBuffer(int,void*&,int &) = 0;
	virtual void GetTextureInfo() = 0;

	virtual void CopyPaletteDataTo(void *) = 0;
	virtual void CopyTextureDataTo(int,void *,int) = 0;

	virtual bool HasAlpha() = 0;

	virtual void GetRGBAAtTexel(float,float);
};
