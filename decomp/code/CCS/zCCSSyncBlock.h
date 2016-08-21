class zCCSSyncBlock : public zCCSBlockBase {
	Z_OBJECT(zCCSSyncBlock);
public:
	virtual void Archive(zCArchiver& arc)
	{
		int num = Ass.GetNumInList();
		arc.WriteInt("numOfAss", num);
		for (int i = 0; i < num; ++i)
			arc.WriteInt("roleAss" + i, Ass[i]);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		int num = 0;
		arc.ReadInt("numOfAss", num);
		for (int i = 0; i < num; ++i) {
			int tmp;
			arc.ReadInt("roleAss" + i, tmp);
			Ass.Insert(tmp);
		}
	}

	virtual ~zCCSSyncBlock();
	virtual void Play(zCEventManager *);
	virtual void Play(zCArray<zCEventManager *> const&,float,float);
	virtual void Play(zCArray<zCEventManager *> const&);
	virtual void IsFinished(zCArray<zCEventManager *> const&);
	virtual void NewBlock(int,int);
	virtual void InsertBlock(zCCSBlockBase *,int);
	virtual void DeleteChild(int);
	virtual void GetRoleNumOfChild(int);
	virtual void BlockAsFocus(int);
private:
	zCArray<int> Ass;
};
