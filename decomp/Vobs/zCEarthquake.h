class zCEarthquake : public zCEffect {
	Z_OBJECT(zCEarthquake);
public:
	virtual ~zCEarthquake();
	virtual void Archive(zCArchiver& arc);
	{
		zCEffect::Archive(arc);

		arc.WriteGroupBegin("Earthquake");
		arc.WriteFloat("radius", sqrt(radius));
		arc.WriteFloat("timeSec", timeMs * 0.001);
		arc.WriteVec3("amplitudeCM", amplitudeCm);
		arc.WriteGroupEnd("Earthquake");
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);

		arc.ReadFloat("radius", radius);
		radius *= radius;

		arc.ReadFloat("timeSec", timeMs);
		timeMs *= 1000.0;

		arc.ReadVec3("amplitudeCM", amplitudeCm);
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
private:
	float radius;
	float timeMs;
	zVEC3 amplitudeCm;
};


