class zCTexConGeneric {
 virtual void ~zCTexConGeneric(uint);
 virtual void Lock(int);
 virtual void Unlock();
 virtual void SetTextureInfo(zCTextureInfo const &);
	virtual void GetPaletteBuffer();
 virtual void GetTextureBuffer(int,void * &,int &);
 virtual void GetTextureInfo();
 virtual void CopyPaletteDataTo(void *);
 virtual void CopyTextureDataTo(int,void	*,int);
 virtual void HasAlpha();
 virtual void ConvertTextureFormat(zCTextureInfo	const &);
 virtual void SetDetailTextureMode(int);
};
