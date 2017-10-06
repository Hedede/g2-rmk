class zCCSBlockBase : public zCObject {
	Z_OBJECT(zCCSBlockBase);
public:
	void Archive(zCArchiver&) override { }
	void Unarchive(zCArchiver&) override { }

	virtual ~zCCSBlockBase();

	virtual zCCSBlockBase* GetChild(int) { return NULL; }
	virtual void OrganizeRoles(zCArray<zCVob *> &,zCArray<zCCSRole *> &) = 0;
	virtual zSTRING GetRoleName() { return ""; }
	virtual void SetRoleName(zSTRING &) {}

	virtual void Play(zCEventManager*,float,float) = 0;
	virtual void Play(zCEventManager*) = 0;
	virtual void Play(zCArray<zCEventManager *> const	&,float,float) = 0;
	virtual void Play(zCArray<zCEventManager *>	const &) = 0;
	virtual void GetMinTime() = 0;
	virtual void IsFinished(zCEventManager *) = 0;
	virtual void IsFinished(zCArray<zCEventManager *> const &) = 0;

	virtual zCEventMessage* GetCommand() { return NULL; }
	virtual void NewBlock(int,int) { return NULL; }
	virtual void InsertBlock(zCCSBlockBase *,int) {}
	virtual void DeleteChild(int) {}

	virtual int GetNumOfChilds() { return 0; }
	virtual int GetRoleNumOfChild(int) { return -1; }

	virtual void BlockCorrection() {}
	virtual void BlockAsFocus(int) {}
	virtual zCCSBlockPosition* GetChildPos(int) { return NULL; }
};
