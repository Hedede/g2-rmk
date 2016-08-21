//zCZoneZFog, Eigenschaften aus dem Spacer bekanntrrun scripts
class zCZoneZFog : public zCZone {
	Z_OBJECT(zCZoneZFog);
public:
	virtual ~zCZoneZFog();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone	*> const &,zCWorld *);
	virtual void GetDefaultZoneClass();

private:
	zREAL       fogRangeCenter;
	zREAL       innerRangePerc;
	zCOLOR      fogColor;
	zBOOL       fadeOutSky;
	zBOOL       overrideColor;
};

class zCZoneZFogDefault : public zCZoneZFog {
	Z_OBJECT(zCZoneZFogDefault);
public:
	zCZoneZFogDefault() = default;
	virtual ~zCZoneZFogDefault() = default;

	virtual void Unarchive(zCArchiver& arc)
	{
		zCZoneZFog::Unarchive(arc);
		if ( world ) {
			world->GetActiveSkyControler()->SetFarZ(fogRangeCenter);
		}
	}

	virtual void ThisVobAddedToWorld(zCWorld* world);
	{
		zCVob::ThisVobAddedToWorld(world);
		this->world = world;
		world->GetActiveSkyControler()->SetFarZ(fogRangeCenter);
	}
};
