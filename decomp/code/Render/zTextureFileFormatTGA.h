class zCTextureFileFormatTGA : public zCTextureFileFormat {
	Z_OBJECT(zCTextureFileFormatTGA);
public:
	~zCTextureFileFormatTGA() override;

	void LoadTexture(zFILE&,zCTextureExchange*) override;
	void LoadTexture(zSTRING const &,zCTextureExchange*) override;

	void SaveTexture(zFILE&,zCTextureExchange*) override;
	void SaveTexture(zSTRING const &,zCTextureExchange*) override;

	void GetFileExtension() override;

	bool CanSave() override { return false; }
	void CanLoad() override;
};
