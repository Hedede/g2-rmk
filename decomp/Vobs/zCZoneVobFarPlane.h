class zCZoneVobFarPlane : public zCZone {
	Z_OBJECT(zCZoneVobFarPlane);
public:
	virtual ~zCZoneVobFarPlane();

	virtual void Archive(zCArchiver& arc)
	{
		zCZone::Archive(arc);
		arc.WriteGroupBegin("ZoneVobFarPlane");
		arc.WriteFloat("vobFarPlaneZ", vobFarPlaneZ);
		arc.WriteFloat("innerRangePerc", innerRangePerc);
		arc.WriteGroupEnd("ZoneVobFarPlane");
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCZone::Unarchive(arc);
		arc.ReadFloat("vobFarPlaneZ", vobFarPlaneZ);
		arc.ReadFloat("innerRangePerc", innerRangePerc);

		if (innerRangePerc < 0.0)
			innerRangePerc = 0.0;
		else if (innerRangePerc > 1.0)
			innerRangePerc = 1.0;

		if ( vobFarPlaneZ > 20000.0 )
			vobFarPlaneZ = 20000.0;
	}

	virtual void ProcessZoneList(zCArraySort<zCZone	*> const &,zCArraySort<zCZone *> const &,zCWorld *);
	virtual void GetDefaultZoneClass();

private:
	float vobFarPlaneZ;
	float innerRangePerc;
};

class zCZoneVobFarPlaneDefault : public zCZoneVobFarPlane {
	Z_OBJECT(zCZoneVobFarPlaneDefault);
public:
	virtual ~zCZoneVobFarPlaneDefault();

	virtual void Unarchive(zCArchiver& arc)
	{
		zCZoneVobFarPlane::Unarchive(arc);

		if (world) {
			if (vobFarPlaneZ < 0.0)
				vobFarPlaneZ = 0.0;
			world->vobFarClipZ = vobFarPlaneZ;
		}
	}

	virtual void ThisVobAddedToWorld(zCWorld *);
};
