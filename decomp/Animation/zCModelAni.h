class zCModelAni : zCObject {
	Z_OBJECT(zCModelAni);
public:
	virtual ~zCModelAni();

	int GetAniID()
	{
		return aniId;
	}

	zSTRING const& GetAniName()
	{
		return aniName;
	}

private:
	zSTRING aniName;
};
