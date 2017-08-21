class zCCSAtomicBlock : public zCCSBlockBase {
	Z_OBJECT(zCCSAtomicBlock);
public:
	virtual void Archive(zCArchiver& arc);
	{
		arc.WriteObject(__msg);
	}

	virtual void Unarchive(zCArchiver&)
	{
		auto obj = arc.ReadObject(0);
		msg__ = dynamic_cast<zCEventMessage>(obj);
	}

	virtual ~zCCSAtomicBlock();
	virtual void OrganizeRoles(zCArray<zCVob *>	&,zCArray<zCCSRole *> &);
	virtual void Play(zCEventManager *,float,float);
	virtual void Play(zCEventManager *);
	virtual void Play(zCArray<zCEventManager *> const &,float,float);
	virtual void Play(zCArray<zCEventManager *> const &);
	virtual void GetMinTime();
	virtual void IsFinished(zCEventManager *);
	virtual void IsFinished(zCArray<zCEventManager *>	const &);
	virtual void GetCommand();
};
