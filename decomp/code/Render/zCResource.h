class zCTexture : public zCResource, public zCTextureExchange{
	CLASSDEF_DEFINE;
public:
	virtual ~zCTexture();
	virtual void LoadResourceData();
	virtual void ReleaseResourceData();
	virtual void GetResSizeBytes();
	virtual void GetCacheConfig(ulong &,ulong &);
	virtual void GetChromaKeyingEnabled();
	virtual void SetChromaKeyingEnabled(int);
	virtual void HasAlpha();
};
