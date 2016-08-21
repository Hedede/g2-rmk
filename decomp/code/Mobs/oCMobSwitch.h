class oCMobSwitch : public oCMobInter {
	Z_OBJECT(oCMobSwitch);
public:
	virtual void Archive(zCArchiver& arc)
	{
		oCMobInter::Archive(arc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		oCMobInter::Unarchive(arc);
	}
	virtual ~oCMobSwitch();
};
