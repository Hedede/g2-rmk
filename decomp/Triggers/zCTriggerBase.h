class zCTriggerBase : public zCVob {
	Z_OBJECT(zCTriggerBase);
public:
	virtual void Archive(zCArchiver& arc);
	{
		zCVob::Archive(arc);
		arc.WriteString("triggerTarget", triggerTarget);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
		arc.ReadString("triggerTarget", triggerTarget);
		triggerTarget.Upper();
	}

	zCTriggerBase()
		: zCVob()
	{
		SetCollisionClass(&zCCollObjectBoxPassThrough::s_oCollObjClass);
		SetCollDetStat(0);
		SetCollDetDyn(1);

		// 100001b
		flags1.showVisual = 1;
		flags1.ignoredByTraceRay = 1;
	}

	virtual ~zCTriggerBase() = default;

	virtual void OnTrigger(zCVob* otherVob, zCVob* vobInstigator)
	{
		zCArray<zCVob*> results;

		homeWorld->SearchVobListByName(triggerTarget, results);
		for (auto vob : results)
			vob->GetEM()->OnTrigger(this, vobInstigator);
	}

	virtual void OnUntrigger(zCVob* otherVob, zCVob* vobInstigator)
	{
		zCArray<zCVob*> results;

		homeWorld->SearchVobListByName(triggerTarget, results);
		for (auto vob : results)
			vob->GetEM()->OnUntrigger(this, vobInstigator);
	}

	virtual void OnTouch(zCVob* otherVob)
	{
		zCArray<zCVob*> results;

		homeWorld->SearchVobListByName(triggerTarget, results);
		for (auto vob : results)
			vob->GetEM()->OnTrigger(otherVob, otherVob);
	}

	virtual void OnUntouch(zCVob* otherVob)
	{
		zCArray<zCVob*> results;

		homeWorld->SearchVobListByName(triggerTarget, results);
		for (auto vob : results)
			vob->GetEM()->OnUntrigger(otherVob, otherVob);
	}

	virtual void GetTriggerTarget(int idx)
	{
		if (idx)
			return nullptr;
		return triggerTarget;
	}

private:
	zSTRING triggerTarget;
};
