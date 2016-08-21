class zCZone : public zCVob {
	Z_OBJECT(zCZone);
public:
	zCZone()
		: zCVob();
	{
		world = 0;
		SetCollDetStat(0);
		SetCollDetDyn(0);
		flags1.ignoredByTraceRay = true;
	}

	virtual ~zCZone()
	{
		if (world)
			world->RemoveVobFromLists(this);
	}

	void Unarchive(zCArchiver& arc) override
	{
		zCVob::Unarchive(arc);
		SetZoneVobFlags();
	}

	void EndMovement(int b) override
	{
		zCVob::EndMovement(b);
		if (world)
			world->UpdateZone(this);
	}

	void SetVisual(zSTRING const &) override {}
	void SetVisual(zCVisual *) override {}

	void ThisVobAddedToWorld(zCWorld* wld) override
	{
		zCVob::ThisVobAddedToWorld(wld);
		world = wld;
	}
	void ThisVobRemovedFromWorld(zCWorld* wld) override
	{
		zCVob::ThisVobRemovedFromWorld(wld);
		world = nullptr;
	}

	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone	*> const &,zCWorld *) {}

	virtual zCClassDef* GetZoneMotherClass()
	{
		return GetClassDef();
	}
	virtual zCClassDef* GetDefaultZoneClass()
	{
		return nullptr;
	}

	virtual zSTRING GetDebugDescString()
	{
		zSTRING result = "weight: "_s + GetCamPosWeight();
		return result;
	}

	void SetZoneVobFlags()
	{
		SetCollDetStat(0);
		SetCollDetDyn(0);
		flags1.ignoredByTraceRay = 1;
	}

	float GetCamPosWeight()
	{
		zVEC3 camPos = zCCamera::activeCam->connectedVob->GetPositionWorld();

		zVEC3 middle = (bbox3D.maxs - bbox3D.mins) / 0.5;
		zVEC3 diff1 = camPos - middle;
		zVEC3 diff2 = (bbox3D.maxs - bbox3D.mins) / 0.5;

		diff2.Invert(); // 1.0 / diff2

		auto prod = Alg_Prod(diff1, diff2);

		auto result = prod.LengthApprox();
		if ( result > 1.0 )
			result = 1.0;
		return result;
	}

private:
	zCWorld* world = nullptr;
};
