class zCMaterial : public zCObject {
	Z_OBJECT(zCMaterial);
public:
	zCMaterial()
	{
		InitValues();
	}

	zCMaterial(zSTRING const& name)
	{
		InitValues();
		SetObjectName(name);
	}

	zCMaterial(zCMaterial const& other)
	{
		InitThisByMaterial(other);
	}

	zCMaterial& operator = (zCMaterial const& other)
	{
		InitThisByMaterial(other);
	}

	virtual ~zCMaterial()
	{
		if ( texture )
			texture.Release();

		if ( detailTexture )
			detailTexture.Release();

		if ( detailObject )
			delete detailObject;
	}

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);


	zSTRING* GetName()
	{
		return GetObjectName();
	}

	void SetTexScale(zVEC2 scale)
	{
		texScale = scale;
	}

	zVEC2 GetTexScale() const
	{
		if ( texture && texScale.x == 1.0 ) {
			float x, y;
			texture->GetPixelSize(x, y);
			texScale.x = x;
			texScale.y = y;
		}

		return texScale;
	}

	void SetTexture(zCTexture* newTex);
	void SetTexture(zSTRING const& texName);

	void SetDetailTexture(zCTexture* tex)
	{
		Set(detailTexture, tex);
	}

	void SetDetailTexture(zSTRING const& texName);

	void RemoveTexture()
	{
		Release(texture);
	}

	void SetDetailObjectVisual(zSTRING const& name)
	{
		if (name.IsEmpty()) {
			Delete(detailObject);
			SetDetailTexture(0);
		} else if (detailObject) {
			*detailObject = name;
		} else {
			detailObject = new zSTRING(name);
		}
	}

	void SetWaveMode(uint32_t mode)
	{
		if (mode > 7)
			mode = 0;
		waveMode = mode;
	}

	void SetTexAniMappingDir(zVEC2 const& dir)
	{
		texAniMapDir = dir;
	}

	bool IsPortalMaterial() const;
private:
	zCArray<zCPolygon*> polyList;
	uint32_t polyListTimeStamp;

	zCTexture* texture;
	zCOLOR color;
	zREAL smoothAngle;
	zTMat_Group  matGroup;
	zCBspSector* bspSectorFront; // //outdoor
	zCBspSector*  bspSectorBack; ////outdoor
	zCTexAniCtrl texAniCtrl;
	zSTRING*  detailObjectVisualName;

	zREAL kambient;
	zREAL kdiffuse;

	float environmentalMappingStrength;

	uint8_t smooth : 1;
	uint8_t dontUseLightmaps : 1;
	uint8_t texAniMap : 1;
	uint8_t lodDontCollapse : 1;
	uint8_t noCollDet : 1;
	uint8_t forceOccluder : 1;
	uint8_t environmentalMapping: 1;
	uint8_t polyListNeedsSort : 1;

	uint8_t matUsage;
	uint8_t libFlag;
	zTRnd_AlphaBlendFunc rndAlphaBlendFunc;
	bool IgnoreSun;

	zTWaveAniMode  waveMode;
	zTFFT  waveSpeed;
	float  waveMaxAmplitude;
	float waveGridSize;

	zCTexture*  detailTexture;
	zREAL  detailTextureScale;
	zVEC2 texAniMapDelta;

	zVEC2 default_mapping;
	zVEC2 texScale;
};

void zCMaterial::InitValues()
{
	defaultMapping.x = 2.34375;
	defaultMapping.y = 2.34375;
	texScale.x = 1.0;
	texScale.y = 1.0;
	texAniMapDir.x = 0;
	texAniMapDir.y = 0;

	flags = 0;
	matUsage  = 0;
	ignoreSun = 0;

	kambient = 0;
	kdiffuse = 0.90;

	smoothAngle = 60.0;

	waveMode = 0;
	waveSpeed = 2;
	alphaFunc = 1;

	texture = 0;
	libFlag = 0;
	bspSectorFront = 0;
	bspSectorBack = 0;
	detailObject = 0;
	detailTexture = 0;
	polyListTimeStamp = -1;

	detailObjectScale = 1.0;
	environmentalMappingStrength = 1.0;

	waveMaxAmplitude = 30.0;
	waveGridSize = 100.0;

	color.red = 150;
	color.blue = 150;
	color.green = 150;
	color.alpha = 255;

	matUsage = zCMaterial::s_matUsageDefault;
}

void zCMaterial::Archive(zCArchiver& arc)
{
	if ( texture ) {
		auto avgColor = texture->GetTextureInfo()->averageColor;
		this->color[3] = avgColor[3];

		if ( (texture->cacheState & 3) == 0 )
			texture->CacheOut();
	}

	arc.WriteString("name", GetObjectName());
	arc.WriteEnum("matGroup", "UNDEF;METAL;STONE;WOOD;EARTH;WATER;SNOW", matGroup);

	arc.WriteColor("color", color);
	arc.WriteFloat("smoothAngle", smoothAngle);

	zSTRING texName = texture ? texture->GetObjectName() : "";
	arc.WriteString("texture", texName);

	arc.riteString("texScale", GetTexScale().GetDescription());
	arc.WriteFloat("texAniFPS", aniCtrl.FPS * 1000.0);
	arc.WriteEnum("texAniMapMode", "NONE;LINEAR", texAniMap);

	arc.WriteString("texAniMapDir", texAniMapDir.GetDescription());
	arc.WriteBool("noCollDet", noCollDet);
	arc.WriteBool("noLightmap", noLightmap);
	arc.WriteBool("lodDontCollapse", lodDontCollapse);

	zSTRING detObj = detailObject ? *detailObject : "";
	arc.WriteString("detailObject", detObj);
	arc.WriteFloat("detailObjectScale", detailObjectScale);
	arc.WriteBool("forceOccluder", forceOccluder);
	arc.WriteBool("environmentalMapping", environmentalMapping);
	arc.WriteFloat("environmentalMappingStrength", environmentalMappingStrength);
	arc.WriteEnum("waveMode", "NONE;AMBIENT_GROUND;GROUND;AMBIENT_WALL;WALL;ENV;AMBIENT_WIND;WIND", waveMode);
	arc.WriteEnum("waveSpeed", "NONE;SLOW;NORMAL;FAST", waveSpeed);
	arc.WriteFloat("waveMaxAmplitude", waveMaxAmplitude);
	arc.WriteFloat("waveGridSize", waveGridSize);
	arc.WriteBool("ignoreSunLight", ignoreSun);
	arc.WriteEnum("alphaFunc", "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2", alphaFunc);
	arc.WriteGroupBegin("Internals");
	arc.WriteRawFloat("defaultMapping", &defaultMapping, 8);
	arc.WriteGroupEnd("Internals");
}

void zCMaterial::Unarchive(zCArchiver& arc)
{
	zSTRING tmpstr = arc.ReadString("name");
	SetObjectName(tmpstr);

	matGroup = arc.ReadEnum("matGroup");
	color = arc.ReadColor("color");
	smoothAngle = arc.ReadFloat("smoothAngle");

	smooth = smoothAngle > 0.0;
	zSTRING texName = arc.ReadString("texture");
	texName.Upper();
	zSTRING oldTexName = texName ? texture->GetObjectName() : "";
	if (oldTexName != texName) {
		Release(texture);
		auto newTex = zCTexture::Load(texName, 0);
		SetTexture(newTex);
		Release(newTex);
	}

	tmpstr = arc.ReadString("texScale");
	texScale.SetByDescription(tmpstr);

	aniCtrl.FPS = arc.ReadFloat("texAniFPS") * 0.001;
	texAniMapMode = arc.ReadEnum("texAniMapMode");

	tmpstr = arc.ReadString("texAniMapDir");
	texAniMapDir.SetByDescription(tmpstr);

	noCollDet = arc.ReadBool("noCollDet");
	noLightmap = arc.ReadBool("noLightmap");
	lodDontCollapse = arc.ReadBool("lodDontCollapse");
	tmpstr = arc.ReadString("detailObject");

	SetDetailObjectVisual(tmpstr);
	
	detailObjectScale = arc.ReadFloat("detailObjectScale");

	forceOccluder = arc.ReadBool("forceOccluder");

	environmentalMapping = ReadBool("environmentalMapping");
	environmentalMappingStrength = arc.ReadFloat("environmentalMappingStrength");

	SetWaveMode(arc.ReadEnum("waveMode"));
	waveSpeed = arc.ReadEnum("waveSpeed");// waveSpeed

	arc.ReadFloat("waveMaxAmplitude", waveMaxAmplitude);
	arc.ReadFloat("waveGridSize", waveGridSize);
	ignoreSun = arc.ReadBool("ignoreSunLight");

	alphaFunc = arc.ReadEnum("alphaFunc");

	if (alphaFunc == 4) {
		zWARNING("C: zCMaterial::Unarchive(): material with name" + GetObjectName() +
		         "has subtractive alpha, which is not supported, setting to normal blend mode");
		alphaFunc = 2;
	}

	if ( detailObjectScale < 0.0 )
		detailObjectScale = 1.0;
	if ( detailObject )
		SetDetailTexture(*detailObject);

	arc.ReadRawFloat("defaultMapping", &defaultMapping, 8);
	if ( color[3] < 0xFFu && alphaFunc == 1 )
		alphaFunc = 2;
}

void zCMaterial::SetDetailTexture(zSTRING const& texName)
{
	if ( detailTexture ) {
		if (texName == detailTexture->GetObjectName())
			return;
		Release(detailTexture);
	}

	if (texName.IsEmpty())
		return;

	auto tex = zCTexture::Load(texName, 0);
	SetDetailTexture(tex);
	Release(tex);
}

bool zCMaterial::IsPortalMaterial() const
{
	zSTRING name = GetObjectName();
	if (name.Length() < 2)
		return false;

	if (name[0] != 'P')
		return false;

	if (name[1] == ':')
		return true;

	if (name[1] == 'N' || name[1] == 'I')
		if (name[2] == ':')
			return true;

	return false;
}

void zCMaterial::SetTexture(zCTexture* newTex)
{
	if ( texture == newTex )
		return;

	Set(texture, newTex);

	if (!texture)
		return;

	zSTRING texname = texname->GetObjectName();
	if (!texname.IsEmpty()) {
		auto detinfo = zCMapDetailTexture::S_GetDetailTextureInfo(texname);

		float scale;
		if ( detinfo ) {
			SetDetailTexture(detinfo->self.name);
			scale = detinfo->self.scale;
		} else if (detailObject) {
			SetDetailTexture(*detailObject);
			scale = detailObjectScale;
		}
		detailObjectScale = scale;
	}
}

void zCMaterial::SetTexture(zSTRING& texName)
{
	texName.Upper();
	if ( texture ) {
		if (texture->GetObjectName() == texname)
			 return;
		Release(texture);
	}

	auto tex = zCTexture::Load(texName);
	SetTexture(tex);
	Release(tex);
}
