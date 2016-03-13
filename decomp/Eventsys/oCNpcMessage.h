class oCNpcMessage : public zCEventMessage {
	Z_OBJECT(oCNpcMessage);
public:
	enum oCNpcMessage::Flags {
		HIGH_PRIORITY = 0x1,
		IN_USE = 0x4,
		DELETED = 0x2,
	};

	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);

	oCNpcMessage() = default;
	virtual ~oCNpcMessage() = default;

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
		bitfield |= DELETED;
	}

	virtual bool IsDeleteable()
	{
		return true;
	}

	virtual bool IsDeleted()
	{
		return bitfield & DELETED;
	}

	virtual void SetInUse(bool inuse)
	{
		bitfield ^= (bitfield ^ inuse * IN_USE) & IN_USE;
	}
	virtual bool IsInUse()
	{
		return bitfield & IN_USE;
	}

	virtual void SetHighPriority(bool ishp)
	{
		bitfield ^= (bitfield ^ ishp * HIGH_PRIORITY) & HIGH_PRIORITY;
	}
private:
	zSTRING targetVobName;
	int bitfield = 0;
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
