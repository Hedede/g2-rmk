class zCCSBlockPosition {
public:
	virtual ~zCCSBlockPosition();

	float pos;
	zCCSBlockBase* block;
};

// _andre/zCCsCutscene.cpp
class zCCSBlock : zCCSBlockBase {
	Z_OBJECT(zCCSBlock);
public:
	void Archive(zCArchiver&) override;
	void Unarchive(zCArchiver&) override;

	zCCSBlock() = default;
	virtual ~zCCSBlock() = default;

	void OrganizeRoles(zCArray<zCVob *>& roles, zCArray<zCCSRole *>& roleN) override
	{
		for (auto& pos : subBlocks) {
			pos.block->OrganizeRoles(roles, roleN);
		}
	}
	zSTRING GetRoleName() { return blockName; } override
	void SetRoleName(zSTRING& name) override { blockName = name; }

	void Play(zCEventManager* aiman) override
	{
		for (auto& pos : subBlocks) {
			if (pos.pos == 0.0)
				pos.block->Play(aiman);
		}
	}
	void Play(zCEventManager* aiman, float b1, float b2) override
	{
		for (auto& pos : subBlocks) {
			float min = subBlocks[0].block->GetMinTime();
			if (min == 0.0)
				min = 1.0;
			float bp = min * pos.pos;
			if (b1 < bp && bp < b2)
				pos.block->Play(aiman);
			// not quite sure what this means
			if (b1 >= bp)
				pos.block->Play(aiman, b1, b2);
		}
	}

	void Play(zCArray<zCEventManager *> const&) override
	{
		zWARNING("zCCSBlock::Play(Array): not allowed!"); // 450
	}
	void Play(zCArray<zCEventManager *> const&,float,float) override
	{
		zWARNING("zCCSBlock::Play(Array): not allowed!"); // 456
	}


	void GetMinTime() override;

	int IsFinished(zCEventManager * aiman) override
	{
		for (auto& pos : subBlocks) {
			if (!pos.block->IsFinished(aiman))
				return 0;
		}
		return 1;
	}
	int IsFinished(zCArray<zCEventManager *> const &) override
	{
		zWARNING("zCCSBlock::IsFinished(Array): not allowed!"); // 488
		return 1;
	}

	void NewBlock(int,int) override;
	void InsertBlock(zCCSBlockBase *,int) override;
	void DeleteChild(int) override;
	void GetNumOfChilds() override { return subBlocks.GetNum(); }
	void BlockCorrection() override;
	void BlockAsFocus(int n) override
	{
		//pseudocode
		Swap( subBlocks[0], subBlocks[n] );
	}

	zCCSBlockPosition* GetChildPos(int nr) override
	{
		if (nr < 0 || nr > GetNumOfChilds())
			return NULL;
		return subBlocks[nr];
	}

	zCCSBlockBase* GetChild(int nr) override
	{
		if (nr < 0 || nr > GetNumOfChilds())
			return NULL;
		return subBlocks[nr].block;
	}

private:
	zCArray<zCCSBlockPosition*> subBlocks;
	zSTRING blockName;
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

void zCCSBlock::DeleteChild(int pos)
{
	if ( pos < 0 || pos > subBlocks.numInArray )
		return;

	subBlocks.RemoveOrderIndex(pos);

	BlockCorrection();
}
