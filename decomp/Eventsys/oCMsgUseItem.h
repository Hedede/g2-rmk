class oCMsgUseItem : public oCNpcMessage {
	Z_OBJECT(oCMsgUseItem);
public:
	virtual ~oCMsgUseItem() = default;

	void Archive(zCArchiver& arc) override
	{
		oCMsgState::Archive(arc);
		arc.WriteObject("item", item);
	}
	void Unarchive(zCArchiver& arc) override
	{
		oCMsgState::Unarchive(arc);
		item = arc.ReadObject("item", 0);
	}

	virtual bool IsNetRelevant()
	{
		return true;
	}

	void Pack(zCBuffer& buf,zCEventManager*) override
	{
		zBOOL hasItem = !!item;
		buf.Write(&hasItem, 4);
		bug.Write(&status, 4);
	}
	void Unpack(zCBuffer& buf,zCEventManager *) override
	{
		zBOOL hasItem; // бред
		buf.Read(&hasItem, 4);
		item = ogame->GetWorld()->SearchVobByID(hasItem);
		buf.Read(&status, 4);
	}

private:
	oCItem* item;
	int aniId;
	int status; // unsure, probably linked to .MDS
};
