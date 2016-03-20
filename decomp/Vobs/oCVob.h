class oCVob : public zCVob {
	Z_OBJECT(oCVob);
public:
	oCVob() = default;
	virtual ~oCVob() = default;

	virtual void Archive(zCArchiver& arc)
	{
		zCVob::Archive(arc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
	}

	virtual void Init()
	{
	}

	virtual int GetInstance()
	{
		return -1;
	}
};
