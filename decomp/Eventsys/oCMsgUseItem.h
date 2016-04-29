class oCMsgUseItem : public oCNpcMessage {
	Z_OBJECT(oCMsgUseItem);
public:
	virtual void _GetClassDef();
	virtual void Archive(zCArchiver& arc)
	{
		oCMsgState::Archive(arc);
		arc.WriteObject("item", item);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		oCMsgState::Unarchive(arc);
		item = arc.ReadObject("item", 0);
	}
	virtual ~oCMsgUseItem();
	virtual bool IsNetRelevant()
	{
		return true;
	}
	virtual void Pack(zCBuffer&,zCEventManager*);
	virtual void Unpack(zCBuffer&,zCEventManager *);

private:
	oCItem* item;
	int aniId;
	int status; // unsure, probably linked to .MDS
};
