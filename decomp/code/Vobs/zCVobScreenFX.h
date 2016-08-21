class zCVobScreenFX : public zCEffect {
	Z_OBJECT(zCVobScreenFX);
public:
	virtual ~zCVobScreenFX();

	virtual void Archive(zCArchiver& arc)
	{
		zCEffect::Archive(arc);
		if ( arc.InSaveGame() ) {
			arc.WriteRaw("blend", &blend, 48);
			arc.WriteRaw("cinema", &cinema, 48);
			arc.WriteRaw("fovMorph", &fovMorph, 48);
			arc.WriteRawFloat("fovSaved", &fovSaved, 8);
			arc.WriteRawFloat("fovSaved1st", &fovSaved1st, 8);
		}
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);
		if ( arc.InSaveGame() ) {
			arc.ReadRaw("blend", &blend, 48);
			arc.ReadRaw("cinema", &cinema, 48);
			arc.ReadRaw("fovMorph", &fovMorph, 48);
			arc.ReadRawFloat("fovSaved", &fovSaved, 8);
			arc.ReadRawFloat("fovSaved1st", &fovSaved1st, 8);
		}
	}

	virtual void OnMessage(zCEventMessage* msg, zCVob* source);
	virtual void OnTick();

private:
	struct zTScreenFXSet;

	zTScreenFXSet blend;
	zTScreenFXSet cinema;
	zTScreenFXSet fovMorph;
	zVEC2 fovSaved;
	zVEC2 fovSaved1st;
};

