class zCTextureFileHandler : public zCScanDirFileHandler {
public:
	zCTextureFileHandler()
		: zCScanDirFileHandler()
	{
		RegisterFileType("tga");
	}

	virtual zCTextureFileHandler();
	virtual void HandleFile(zSTRING const& ,char const*, _finddata_t);
};
