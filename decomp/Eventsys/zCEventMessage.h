class zCEventMessage : public zCObject {
	Z_OBJECT(zCEventMessage);
public:
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver & archive)
	{
		subType = archive->ReadEnum("subType");
	}

	zCEventMessage() = default;
	virtual ~zCEventMessage();

	virtual void IsOverlay()
	{
		return false;
	}
	virtual void IsNetRelevant()
	{
		return false;
	}
	virtual bool IsHighPriority()
	{
		return false;
	}
	virtual void IsJob()
	{
		return false;
	}

	virtual bool GetIgnoreCutsceneMode()
	{
		return false;
	}

	virtual void Delete()
	{
	}
	virtual bool IsDeleteable()
	{
		return true;
	}
	virtual bool IsDeleted()
	{
		return false;
	}

	virtual void SetCutsceneMode(int csMode)
	{
		this->csMode = csMode;
	}
	virtual int GetCutsceneMode()
	{
		return csMode;
	}

	virtual size_t MD_GetNumOfSubTypes()
	{
		return 0;
	}
	virtual zSTRING MD_GetSubTypeString(int)
	{
		return "";
	}
	virtual void MD_GetVobRefName()
	{
		return "";
	}
	virtual void MD_SetVobRefName(zSTRING const&)
	{
	}
	virtual void MD_SetVobParam(zCVob *)
	{
	}
	virtual int MD_GetTimeBehavior()
	{
		return 0;
	}
	virtual float MD_GetMinTime()
	{
		return 0.0;
	}

	virtual void Pack(zCBuffer &,zCEventManager *)
	{
	}
	virtual void Unpack(zCBuffer &,zCEventManager *)
	{
	}

	void PackToBuffer(zCBuffer& buffer, zCEventManager* evman)
	{
		uint32_t type = _GetClassDef()->GetClassID() + (subType << 16);
		buffer.Write(&type, 4);

		Pack(buffer, evman);
	}

	static zCEventMessage* CreateFromBuffer(zCBuffer& buffer, zCEventManager *evman)
	{
		uint32_t id;
		buffer.Read(&id, 4);
		zCObject* newInst = zCClassDef::CreateNewInstanceID(uint16_t(id));
		auto msg = static_cast<zCEventMessage*>(newInst);
		msg->subType = uint16_t(id >> 16);
		msg->Unpack(buffer, evman);
		return msg;
	}

	uint16_t GetSubtype() const
	{
		return subType;
	}
private:
	uint16_t subType = 0;
	int csMode = 0;
};

void zCEventMessage::Archive(zCArchiver& archiver)
{
	if ( !archiver.InProperties() ) {
		archiver->WriteEnum(archiver, "subType", "", subType);
		return;
	}

	// TODO: maybe incorrect, lok at moverCOntroler
	zSTRING enum_str;
	for (int i = 0, num = MD_GetNumOfSubTypes(); i < num; ++i) {
		zSTRING subtype_str = MD_GetSubTypeString(i);
		enum_str += ";" + subtype_str;
	}

	archiver->WriteEnum(archiver, "subType", enum_str, subType);
}
