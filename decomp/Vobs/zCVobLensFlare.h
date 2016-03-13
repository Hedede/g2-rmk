class zCVobLensFlare : public zCEffect {
	Z_OBJECT(zCVobLensFlare);
public:
	virtual ~zCVobLensFlare();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	virtual void Render(zTRenderContext &);

private:
	zCLensFlareFX* flareFx;
};

void zCVobLensFlare::Archive(zCArchiver& arc)
{
	zCVob::Archive(arc);

	zSTRING fxName = flareFx ? flareFx->GetObjectName() : "";

	arc.WriteString("lensflareFX", fxName);
}

void zCVobLensFlare::Unarchive(zCArchiver& arc)
{
	zCEffect::Unarchive(arc);

	zSTRING fxName = arc.ReadString("lensflareFX");

	Release(flareFx);

	if ( fxName.Length() > 0 )
		flareFx= zCLensFlareFX::classDef.SearchHashTable(fxName);

	flags1.showVisual = 1;
}
