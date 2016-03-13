class zCZone : public zCVob {
	Z_OBJECT(zCZone);
public:
	virtual ~zCZone();
	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
		SetCollDetStat(0);
		SetCollDetDyn(0);
		flags1.ignoredByTraceRay = 1;
	}

	virtual void EndMovement(int);
	virtual void SetVisual(zSTRING const &);
	virtual void SetVisual(zCVisual *);
	virtual void ThisVobAddedToWorld(zCWorld*);
	virtual void ThisVobRemovedFromWorld(zCWorld*);
	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone	*> const &,zCWorld *);
	virtual void GetZoneMotherClass();
	virtual void GetDefaultZoneClass();
	virtual void GetDebugDescString();
};
