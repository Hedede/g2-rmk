class oCNpcMessage : public zCEventMessage {
	Z_OBJECT(oCNpcMessage);
public:
	oCNpcMessage() = default;
	virtual ~oCNpcMessage() = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual bool IsOverlay()
	{
		return false;
	}

	virtual void IsHighPriority()
	{
		return bitfield & HIGH_PRIORITY;
	}

	virtual bool IsJob()
	{
		return true;
	}

	virtual int GetIgnoreCutsceneMode()
	{
		return 0;
	}

	virtual void Delete()
	{
		flags.DELETED = true;
	}

	virtual bool IsDeleteable()
	{
		return true;
	}

	virtual bool IsDeleted()
	{
		return flags.DELETED;
	}

	virtual void SetInUse(bool inuse)
	{
		flags.IN_USE = inuse;
	}
	virtual bool IsInUse()
	{
		return flags.IN_USE;
	}

	virtual void SetHighPriority(bool ishp)
	{
		flags.HIGH_PRIORITY = ishp;
	}

private:
	zSTRING targetVobName;
	struct {
		unsigned HIGH_PRIORITY : 1; // 0x1,
		unsigned DELETED       : 1; // 0x2,
		unsigned IN_USE        : 1; // 0x4,
	} flags;
};

void oCNpcMessage::Archive(zCArchiver &archiver)
{
	zCEventMessage::Archive(archiver);
	if ( archiver.InSaveGame() ) {
		archiver.WriteBool("highPriority", IsHighPriority());
		archiver.WriteBool("inUse",        IsInUse());
		archiver.WriteBool("deleted",      IsDeleted());
	}
}

void oCNpcMessage::Unarchive(zCArchiver &archiver)
{
	zCEventMessage::Unarchive(archiver);
	if ( archiver.InSaveGame() )
	{
		bool tmp;
		archiver.ReadBool("highPriority", tmp);
		SetHighPriority(tmp);
		archiver.ReadBool("inUse",        tmp);
		SetInUse(tmp);
		archiver.ReadBool("deleted",      tmp);
		bitfield ^= (bitfield ^ tmp * DELETED) & DELETED;
	}
}
