class zCTouchAnimate : public zCEffect {
	Z_OBJECT(zCTouchAnimate);
public:
	virtual ~zCTouchAnimate();

	virtual void Archive(zCArchiver& arc)
	{
		zCEffect::Archive(arc);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);

		Release(unk_obj);

		SetVobProperties();
	}

	virtual void OnTouch(zCVob*);
	virtual void OnUntouch(zCVob*);
	virtual void OnTick();
	virtual void SetVisual(zCVisual* visual)
	{
		zCEffect::SetVisual(visual);
		SetVobProperties();
	}

	virtual zSTRING GetSoundName() const;

private:
	void SetVobProperties()
	{
		SetCollisionClass(zCCollObjectBoxPassThrough::s_oCollObjClass);
		SetCollDetStat(0);
		SetCollDetDyn(1);

		// TODO:
		bitfield[0] ^= 0b100001;
	}
};
