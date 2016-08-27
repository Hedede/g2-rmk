class zCTextureFileFormat : public zCObject {
	Z_OBEJCT(zCTextureFileFormat);
public:
	~zCTextureFileFormat() override;

	virtual void LoadTexture(zFILE &,zCTextureExchange *) = 0;
	virtual void LoadTexture(zSTRING const &,zCTextureExchange*) = 0;
	virtual void SaveTexture(zFILE &,zCTextureExchange *) = 0;
	virtual void SaveTexture(zSTRING const &,zCTextureExchange*) = 0;
	virtual void GetFileExtension() = 0;
	virtual void CanSave() = 0;
	virtual void CanLoad() = 0;
};
