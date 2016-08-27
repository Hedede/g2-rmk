class zCTextureFileFormatTGA : public zCTextureFileFormat {
	Z_OBJECT(zCTextureFileFormatTGA);
public:
	~zCTextureFileFormatTGA() override;
	void LoadTexture(zFILE&,zCTextureExchange*) override;
	void LoadTexture(zSTRING const &,zCTextureExchange*) override;
	void SaveTexture(zFILE&,zCTextureExchange*) override;
	void SaveTexture(zSTRING const &,zCTextureExchange*) override;
	void GetFileExtension() override;
	void CanSave() override;
	void CanLoad() override;
};
