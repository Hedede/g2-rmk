class zCCSBlockBase : public zCObject {
	Z_OBJECT(zCCSBlockBase);
public:
	virtual void Archive(zCArchiver&)
	{
	}
	virtual void Unarchive(zCArchiver&)
	{
	}
	virtual ~zCCSBlockBase();
	virtual void GetChild(int);
	virtual void GetRoleName();
	virtual void SetRoleName(zSTRING &);
	virtual void GetCommand();
	virtual void NewBlock(int,int);
	virtual void InsertBlock(zCCSBlockBase *,int);
	virtual void DeleteChild(int);
	virtual void GetNumOfChilds();
	virtual void GetRoleNumOfChild(int);
	virtual void BlockCorrection();
	virtual void BlockAsFocus(int);
	virtual void GetChildPos(int);
};
