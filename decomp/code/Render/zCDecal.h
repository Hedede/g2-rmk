class zCDecal : public: zCVisual {
	Z_OBJECT(zCDecal);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	zCDecal()
		: zCVisual()
	{
		decalDim.x = 25.0;
		decalDim.y = 25.0;
		decalOffset.x = 0;
		decalOffset.y = 0;
		material = new zCMaterial();
	}

	virtual ~zCDecal()
	{
		Release(material);
	}

	virtual void Render(zTRenderContext&);
	virtual void IsBBox3DLocal()
	{
		return 1;
	}
	virtual void GetBBox3D();

	virtual zSTRING GetVisualName() const
	{
		zCTexture* texture = material->texture;
		if ( texture )
			return texture->GetVisualName();
		return "";
	}

	virtual void SetVisualUsedBy(zCVob *);
	virtual void DynLightVisual(zCArray<zCVobLight *> const&,zMAT4*);
	virtual void GetRenderSortKey();
	virtual zSTRING const& GetFileExtension(int i)
	{
		if (i)
			return zSTR_EMPTY;

		static ext = ".TGA";
		return ext;
	}
	virtual void GetAlphaTestingEnabled();
	virtual void LoadVisualVirtual(zSTRING const&);

	void SetTexture(zSTRING& texName)
	{
		material->SetTexture(texName);
	}

	void SetDecalDim(float x, float y)
	{
		decalDim.x = x;
		decalDim.y = y;
	}
private:
	zCMaterial *material;
	zVEC2 decalDim;
	zVEC2 decalOffset;
	zBOOL decal2Sided;
	zBOOL ignoreDayLight = false;
};

//------------------------------------------------------------------------------
void zCDecal::CreateDecalMeshes()
{
	zCDecal::mesh1Sided = zCMesh::CreateQuadMesh(0);
	zCDecal::mesh2Sided = zCMesh::CreateQuadMesh(1);
	zCDecal::mesh1Sided->SetStaticLight(GFX_CYAN);
	zCDecal::mesh1Sided->SetLightDyn(GFX_CYAN);
	zCDecal::mesh2Sided->SetStaticLight(GFX_CYAN);
	zCDecal::mesh2Sided->SetLightDyn(GFX_CYAN);
}

void zCDecal::Archive(zCArchiver& arc)
{

	arc.WriteString( "name", GetVisualName());

	arc.WriteRawFloat("decalDim", decalDim, 8);
	arc.WriteRawFloat("decalOffset", decalOffset, 8);

	arc.WriteBool("decal2Sided", decal2Sided);

	zSTRING enumChoices = "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2";
	arc.WriteEnum("decalAlphaFunc", enumChoices, material->alphaFunc);

	float texFps = material->aniCtrl.FPS * 1000.0;
	arc.WriteFloat("decalTexAniFPS", texFps);
	arc.WriteByte("decalAlphaWeight", material->color.Alpha());
	arc.WriteBool("ignoreDayLight", ignoreDayLight);
}

void zCDecal::Unarchive(zCArchiver& arc)
{
	zSTRING texName;
	arc.ReadString("name", texName);

	arc.ReadRawFloat("decalDim", decalDim, 8);
	arc.ReadRawFloat("decalOffset", decalOffset, 8);

	decal2Sided = arc.ReadBool("decal2Sided");
	int alphaFunc = arc.ReadEnum("decalAlphaFunc");
	if (alphaFunc == 4) {
		zWARNING("C: zCDecal::Unarchive(): decal with texname" + texName +
		         "has subtractive alpha, which is not supported, setting to normal blend mode");
		alphaFunc = 2;
	}
	material->alphaFunc = func;

	float texFps = arc.ReadFloat("decalTexAniFPS");
	material->aniCtrl.FPS = texFps * 0.001;

	uint8_t alphaWeight = 0xFF;
	arc.ReadByte("decalAlphaWeight", alphaWeight);
	material->color.color[3] = alphaWeight;

	arc.ReadBool("ignoreDayLight", ignoreDayLight);
}
