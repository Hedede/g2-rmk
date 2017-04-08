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

	zCMaterial& operator=(zCMaterial const& other)
	{
		InitThisByMaterial(other);
	}

	~zCMaterial() override
	{
		if ( texture )
			texture.Release();

		if ( detailTexture )
			detailTexture.Release();

		if ( detailObject )
			delete detailObject;
	}

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;


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

	zSTRING const* zCMaterial::GetDetailObjectVisual() const
	{
		if ( !detailObject )
			return &zSTR_EMPTY;
		return detailObject;
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
	zCBspSector*  bspSectorFront; //outdoor
	zCBspSector*  bspSectorBack;  //outdoor
	zCTexAniCtrl texAniCtrl;
	zSTRING*  detailObjectVisualName;

	zREAL kambient;
	zREAL kdiffuse;

	float environmentalMappingStrength;

	struct {
		uint8_t smooth               : 1; // 1
		uint8_t dontUseLightmaps     : 1; // 2
		uint8_t texAniMap            : 1; // 4
		uint8_t lodDontCollapse      : 1; // 8
		uint8_t noCollDet            : 1; // 0x10
		uint8_t forceOccluder        : 1; // 0x20
		uint8_t environmentalMapping : 1; // 0x40
		uint8_t polyListNeedsSort    : 1; // 0x80
	} flags;

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
			SetDetailTexture(detinfo.name);
			scale = detinfo.scale;
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

// "SNOW" was added in G2
enum zTMat_Group {
	MAT_UNDEF,
	MAT_METAL,
	MAT_STONE,
	MAT_WOOD,
	MAT_EARTH,
	MAT_WATER,
	MAT_SNOW,
};
zSTRING s_MatGroupStrings[] = { "UNDEF", "METAL", "STONE", "WOOD", "EARTH", "WATER", "SNOW" };

// static
zSTRING& zCMaterial::GetMatGroupString(zTMat_Group grp)
{
	switch ( grp ) {
	case MAT_METAL:
		result = s_MatGroupStrings[1];
		break;
	case MAT_STONE:
		result = s_MatGroupStrings[2];
		break;
	case MAT_WOOD:
		result = s_MatGroupStrings[3];
		break;
	case MAT_EARTH:
		result = s_MatGroupStrings[4];
		break;
	case MAT_WATER:
		result = s_MatGroupStrings[5];
		break;
	case MAT_SNOW:
		result = s_MatGroupStrings[6];
		break;
	default:
		result = s_MatGroupStrings[0]; // UNDEF
		break;
	}
	return result;
}

zSTRING& zCMaterial::GetMatGroupString()
{
	return GetMatGroupString(this->matGroup);
}

void zCMaterial::SetMatGroupByString(zSTRING const& s)
{
	if (s == s_MatGroupStrings[1])
		matGroup = MAT_METAL;
	else if (s == s_MatGroupStrings[2])
		matGroup = MAT_STONE;
	else if (s == s_MatGroupStrings[3])
		matGroup = MAT_WOOD;
	else if (s == s_MatGroupStrings[4])
		matGroup = MAT_EARTH;
	else if (s == s_MatGroupStrings[5])
		matGroup = MAT_WATER;
	else if (s == s_MatGroupStrings[6])
		matGroup = MAT_SNOW;
	else
		matGroup = MAT_UNDEF;
}

void zCMaterial::RefreshAvgColorFromTexture()
{
	if ( texture ) {
		auto texInfo = texture->GetTextureInfo();
		zCOLOR tmp = texInfo.averageColor;
		tmp[3] = this->color[3];
		this->color = tmp;
		if (texture->cacheState == 0)
			texture->CacheOut();
	}
}

// static
zCMaterial* zCMaterial::SearchName(zSTRING& name)
{
	name.Upper();
	return zCMaterial::classDef.SearchHashTable(name);
}

zCTexture* zCMaterial::GetAniTexture()
{
	if ( texture ) {
		if ( texture->hasAlpha & 2 ) { // probably "flags.animated"
			aniCtrl.AdvanceAni(texture);
			return texture->GetAniTexture();
		}
	}

	return nullptr;
}

void zCMaterial::ApplyTexAniMapping(zCPolygon *poly)
{
	if ( flags.texAniMap ) {
		float x = ztimer.totalTimeFloat * this->texAniMapDir.x;
		float y = ztimer.totalTimeFloat * this->texAniMapDir.y;

		x = x - floor(x);
		y = y - floor(y);
		for ( int i = 0; i < poly->numClipVert; ++i)
			poly->clipFeat[i].texuv += zVEC2{x, y};
	}
}

void zCMaterial::InitThisByMaterial(zCMaterial const& other)
{
	zCMaterial::InitValues();

	if ( texture != other.texture ) {
		Release(texture);
		texture = other.texture;
		if ( texture ) {
			++texture->refCtr;
			AutoAssignDetailTexture();
		}
	}

	color = other.color;

	flags.smooth = other.smooth;
	smoothAngle  = other.smoothAngle;

	matGroup  = other.matGroup;
	alphaFunc = other.alphaFunc;

	flags.noLightmap      = other.flags.noLightmap;
	flags.lodDontCollapse = other.flags.lodDontCollapse;
	flags.noCollDet       = other.flags.noCollDet;
	flags.forceOccluder   = other.flags.forceOccluder;

	flags.texAniMapMode   = other.flags.texAniMapMode;
	texAniMapDir          = other.texAniMapDir;

	matUsage = other.matUsage;

	flags.environmentalMapping   = other.flags.environmentalMapping;
	environmentalMappingStrength = other.environmentalMappingStrength;

	waveMode  = other.waveMode;
	waveSpeed = other.waveSpeed;

	waveGridSize     = other.waveGridSize;
	waveMaxAmplitude = other.waveMaxAmplitude;

	ignoreSun = other.ignoreSun;
}

void zCMaterial::AutoAssignDetailTexture()
{
	if (!texture)
		return;
	auto texName = texture->GetObjectName();
	if (!texName)
		return;

	if (auto detInfo = zCMapDetailTexture::S_GetDetailTextureInfo(texName)) {
		if ( detailTexture ) {
			auto detName = detailTexture->GetObjectName();
			if (detName == detInfo.name) {
				detailObjectScale = detInfo.scale;
				return;
			}
			Release(detailTexture);
		}

		if ( detInfo.name ) {
			auto tex = zCTexture::Load(detInfo.name, 0);

			SetDetailTexture(tex);
			Release(tex);
		}

		detailObjectScale = detInfo.scale;
		return;
	}

	if ( detailObject ) {
		if ( detailTexture ) {
			auto detName = detailTexture->GetObjectName();
			if (*detailObject == detName) {
				// not a type on my part
				detailObjectScale = this->detailObjectScale;
				return;
			}

			Release(detailTexture);
		}

		if ( detailObject->Length() ) {
			auto tex = zCTexture::Load(*detailObject, 0);

			SetDetailTexture(tex);
			Release(tex);
		}

		detailObjectScale = this->detailObjectScale;
		return;
	}
}

zVEC3* zCMaterial::GetTexAniVector(zCPolygon* poly)
{
	if (!flags.texAniMap)
		return zVEC3::ZERO; // *made up*

	int n = 2;
	for (; n < poly->numVerts; ++i) {
		if ( poly->numVerts <= 3 )
			break;

		auto v0 = poly->vertex[0];
		auto v1 = poly->vertex[1];
		auto vN = poly->vertex[2];

		auto v1v0 = v1.pos - v0.pos;
		auto vNv1 = vN.pos - v1.pos;

		auto len = v1v0.Length();
		if (len != 0.0) {
			v1v0 /= len;
			auto len = vNv1.Length();
			if (len != 0.0) {
				vNv1 /= len;
				if (!v1v0.IsEqualEps(vNv1)) {
					if (!v1v0.IsEqualEps(-vNv1)) {
						break;
					}
				}
			}
		}
	}

	if ( n == poly->numVerts )
		return zVEC3::ZERO;

	zMAT3 mat; // CONSTRUCT(&mat, 12, 3, zVEC3::zVEC3);
	// (actually called mat in original code)

	mat[0][0] = 1.0;
	mat[1][0] = 1.0;
	mat[2][0] = 1.0;
	mat[0][1] = poly->feature[0]->texuv.x;
	mat[1][1] = poly->feature[1]->texuv.x;
	mat[2][1] = poly->feature[2]->texuv.x;
	mat[0][2] = poly->feature[0]->texuv.y;
	mat[1][2] = poly->feature[1]->texuv.y;
	mat[2][2] = poly->feature[2]->texuv.y;
	float det;
	mat = mat.Inverse(&det);
	if (det == 0.0)
		return zVEC3::ZERO;

	zVEC3 U;
	zVEC3 V;

	// not sure if this is correct, I was tired at the moment of writing,
	// and frustrated and confused by compiler's instruction reordering
	// like
	//
	// add esi, 4
	// esp + (offset_of_var-4) + esi
	//
	// instead of
	//
	// esp + (offset_of_var) + esi
	// add esi, 4
	//
	// anyway, this is barycentric coordinates, I should read how they are calculated

	for (int i = 0; i < 3; ++i) {
		auto v0 = poly->vertex[0];
		auto v1 = poly->vertex[1];
		auto vN = poly->vertex[n];
		auto pos0 = v0->Position[i];
		auto pos1 = v1->Position[i];
		auto posN = vN->Position[i];

		auto vec = mat * zVEC3{pos0, pos1, posN};

		U[i] = vec.y;
		V[i] = vec.z;
	}


	auto u = U * -texAniMapDir.x;
	auto v = V * -texAniMapDir.y;
	return u + v;
}
