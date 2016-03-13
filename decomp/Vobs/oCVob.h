class oCVob : public zCVob {
public:
	virtual void Archive(zCArchiver& arc)
	{
		zCVob::Archive(arc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
	}

	virtual ~oCVob();
	virtual void Init()
	{
	}

	virtual int GetInstance()
	{
		return -1;
	}
};
