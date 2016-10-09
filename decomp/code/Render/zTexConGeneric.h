class zCTexConGeneric : zCTextureConvert {
	~zCTexConGeneric() override;
	void Lock(int) override;
	void Unlock() override;
	void SetTextureInfo(zCTextureInfo const &) override;
	void GetPaletteBuffer() override;
	void GetTextureBuffer(int,void * &,int &) override;
	void GetTextureInfo() override;
	void CopyPaletteDataTo(void *) override;
	void CopyTextureDataTo(int,void	*,int) override;
	void HasAlpha() override;
	void ConvertTextureFormat(zCTextureInfo	const &) override;
	void SetDetailTextureMode(int) override;
};
