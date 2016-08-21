class zCCSBlockPosition {
public:
	virtual ~zCCSBlockPosition();

	float pos;
	zCCSBlockBase* block;
};

class zCCSBlock : zCCSBlockBase {
	Z_OBJECT(zCCSBlock);
public:
	virtual void Archive(zCArchiver&);
	virtual void Unarchive(zCArchiver&);

	zCCSBlock() = default();
	virtual ~zCCSBlock();
	virtual void GetChild(int);
	virtual void OrganizeRoles(zCArray<zCVob *> &,zCArray<zCCSRole *> &);
	virtual void GetRoleName();
	virtual void SetRoleName(zSTRING &);
	virtual void Play(zCEventManager*,float,float);
	virtual void Play(zCEventManager*);
	virtual void Play(zCArray<zCEventManager *> const	&,float,float);
	virtual void Play(zCArray<zCEventManager *>	const &);
	virtual void GetMinTime();
	virtual void IsFinished(zCEventManager *);
	virtual void IsFinished(zCArray<zCEventManager *> const &);
	virtual void NewBlock(int,int);
	virtual void InsertBlock(zCCSBlockBase *,int);
	virtual void DeleteChild(int);
	virtual void GetNumOfChilds();
	virtual void BlockCorrection();
	virtual void BlockAsFocus(int);
	virtual void GetChildPos(int);
};

void zCCSBlock::Archive(zCArchiver& arc)
{
	arc.WriteString("blockName", blockName);

	int num = subBlocks.GetNumInList();
	arc.WriteInt("numOfBlocks", num);

	for (int i = 0; i < num; ++i) {
		 arc.WriteFloat("subBlock" + i, subBlocks.array[i].pos);
		 arc.WriteObject_p(subBlocks[i].block);
	}
}

void zCCSBlock::Unarchive(zCArchiver& arc)
{
	arc.ReadString("blockName", blockName);

	int numOfBlocks = 0;
	arc.ReadInt("numOfBlocks", numOfBlocks);

	for (int i = 0; i < numOfBlocks; ++i) {
		zCCSBlockPosition sub;
		sub.pos = arc.ReadFloat("subBlock" + i, arc);
		sub.block = arc.ReadObject(0);
		subBlocks.InsertEnd(sub);
	}
}
