class zCCSLib : public zCObject {
	Z_OBJECT(zCCSLib);

public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	zCCSBlock()
	{
		dummy = new zCCSBlock();
		blocks.Compare = zCSLibCompare
	}

	virtual ~zCCSLib()
	{
		Release(dummy);
		DeleteLib();
	}

	void DeleteLib()
	{
		for (auto block : blocks)
			Release(block);

		blocks.DeleteList();

		loaded = 0;
		wasChanged = 0;
	}

	int CompactLib();

	void Add(zCCSBlock* block)
	{
		if (loaded) {
			blocks.Insert(block);
			// strange, should be InsertSort,
			// but inlined code just inserts into the end
			// (In Gothic1Demo there was zCArray, not zCArraySort
			block->AddRef();
			Change();

			return blocks->GetNumInList();
		}
		return -1;
	}

	zCCSBlock* GetOU(int idx)
	{
		return blocks[idx];
	}

	void IsLoaded() const
	{
		return loaded;
	}

	void Change()
	{
		wasChanged = 1;
	}

	void WasChanged() const
	{
		return wasChanged;
	}

	int zCCSLib::NextFree()
	{
		return blocks.GetNumInList();
	}

	void RemoveFromLib(int index, int removeNow)
	{
		Release(blocks[index]);

		if ( removeNow )
			blocks.RemoveOrderIndex(index);
	}

	int zCCSLib::ValidateToken(zSTRING& tok)
	{
		dummy->SetRoleName(tok);

		return blocks.Search(dummy);
	}

private:
	zBOOL loaded;
	zBOOL wasChanged;
	zCArraySort<zCCSBlock*> blocks;
	zCCSBlock *dummy;
};

int zCSLibCompare(void* a, void* b)
{
	auto block1 = static_cast<zCCSBlock*>(a);
	auto block2 = static_cast<zCCSBlock*>(b);

	if ( !block1 )
		return 1;
	if ( !block2 )
		return -1;

	auto name1 = block1->GetRoleName();
	auto name2 = block2->GetRoleName();

	return name1.Compare(name2);
}

void zCCSLib::Archive(zCArchiver& arc)
{
	int num = blocks.GetNumInList();
	arc.WriteInt("NumOfItems", num);
	zINFO(3, "B: CSLib: Number of items: " + num + " (Arc)");

	for (int i = 0; i < num; ++i) {
		auto block = blocks[i];
		if (block)
			arc.WriteObject(block);
	}

	wasChanged = 0;
}

void zCCSLib::Unarchive(zCArchiver& arc)
{
	zINFO(3,"B: CSLib: loading library ...");

	int num = arc.ReadInt("NumOfItems");
	zINFO(3, "B: CSLib: Number of items: " + num + " (UnArc)");

	blocks.AllocAbs(15000);

	for (int i = 0; i < num; ++i ) {
		auto obj = arc.ReadObject(0);
		auto csblock = dynamic_cast<zCCSBlock>(obj);
		if ( csblock ) {
			InsertEnd(csblock);
			csblock->Release();
		}

		if ( !(i % 250) )
			sysEvent();
	}

	zINFO(3, "B: CSLib: Sorting library ...");

	blocks.QSort();

	loaded = 1;
	wasChanged = 0;

	zINFO(3,"B: CSLib: finished loading library");
}

int zCCSLib::CompactLib(zCCSLib *this)
{
	for (int i = 0; i < blocks.GetNumInList(); ++i) {
		if (blocks[i])
			continue;

		blocks.RemoveOrderIndex(i);
	}
	return blocks.GetNumInList();
}
