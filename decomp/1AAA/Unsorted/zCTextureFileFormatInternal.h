class zCTextureFileFormatInternal {
 virtual void _GetClassDef();
 virtual void ~zCTextureFileFormatInternal(uint);
 virtual void LoadTexture(zFILE &,zCTextureExchange	*);
 virtual void LoadTexture(zSTRING	const &,zCTextureExchange *);
 virtual void SaveTexture(zFILE &,zCTextureExchange	*);
 virtual void SaveTexture(zSTRING	const &,zCTextureExchange *);
 virtual void GetFileExtension();
 virtual void CanSave();
 virtual void CanLoad();
};
