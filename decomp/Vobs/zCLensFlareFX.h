class zCLensFlareFX : public zCObject {
	Z_OBJECT(zCLensFlareFX);
public:
	virtual ~zCLensFlareFX();
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
};

void zCLensFlareFX::Archive(zCArchiver& arc)
{
	arc.WriteString("name", GetObjectName());

	int num = flares.GetNumInList();
	arc.WriteInt("numFlares", num);

	for (int idx = 0; idx < num; ++i) {
		auto flare = flares[idx];

		arc.WriteChunkStart(0, 0);

		auto tex = flare.material->texture;
		zSTRING texName = tex ? tex->GetObjectName() : "";

		arc.WriteString("texName", texName);

		arc.WriteEnum("type", "FT_CORONA;FT_GLOW;FT_FLARE", flare.type);

		float size = flare.size;
		if (flare.type == 1)
			size *= 0.01;
		else
			size *= 0.004;

		arc.WriteFloat("size", size);
		arc.WriteFloat("alpha", flare.alpha);
		arc.WriteFloat("rangeMin", flare.rangeMin);
		arc.WriteFloat("rangeMax", flare.rangeMax);
		arc.WriteFloat("posScale", flare.posScale);
		arc.WriteFloat("fadeScale", flare.fadeScale);

		arc.WriteChunkEnd();
	}

	arc.WriteFloat("fadeScale", fadeScale);
}

void zCLensFlareFX::Unarchive(zCArchiver& arc)
{
	zSTRING name = arc.ReadString("name");
	SetObjectName(name);

	int num = arc.ReadInt("numFlares");

	flares.AllocAbs(num);

	for (int i = 0; i < num; ++i) {
		zTVobLightLensFlare flare;

		zSTRING chunkName;
		uint16_t version;

		int bullshit[2] = {0, 0};
		if (arc.ReadChunkStart(chunkName, version)) {
			zSTRING texname;
			arc.ReadString("texName", texname);
			flare.type = arc.ReadEnum("type");
			arc.ReadFloat("size", flare.size);
			arc.ReadFloat("alpha", flare.alpha);
			arc.ReadFloat("rangeMin", flare.rangeMin);
			arc.ReadFloat("rangeMax", flare.rangeMax);
			arc.ReadFloat("posScale", flare.posScale);
			arc.ReadFloat("fadeScale", flare.fadeScale);

			arc.SkipOpenChunk();

			if ( flare.type == 1 )
				flare.size = flare.size * 100.0;
			else
				flare.size = 250.0 / flare.size;

			flare.material = new zCMaterial();
			flare.material->alphaFunc = 3;
			flare.material->SetTexture(texname);
			flare.material->color.color[3] = flare.alpha;
			flare.material->color.color[2] = -1;
			flare.material->color.color[1] = -1;
			flare.material->color.color[0] = -1;

			flares.Insert(flare);
			// flares[i] = flare; ?

			if (!bullshit[0])
				bullshit[0] = flare.type == 1 ? 1 : 0;
			if (!bullshit[1])
				bullshit[1] = flare.type != 1 ? 1 : 0;
		}
	}

	arc.ReadFloat("fadeScale", fadeScale);

	if ( bullshit[0] ) {
		if ( bullshit[1] )
			type = 1;
		else
			type = 0;
	} else {
		type = 2;
	}
}
