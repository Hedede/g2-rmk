class zCCSLib : public zCObject {
	Z_OBJECT(zCCSLib);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCSLib();
};

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
