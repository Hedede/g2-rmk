struct oCMsgDamage : public oCNpcMessage {
	Z_OBJECT(oCMsgDamage);
public:
	enum Type {
		EV_DAMAGE_ONCE = 0,
		EV_DAMAGE_PER_FRAME = 1,
	};

	virtual void Archive(zCArchiver& archive)
	{
		oCNpcMessage::Archive(archive);
		archive.WriteRaw("damage_descriptor", &damDescriptor, sizeof(damDescriptor));
	}

	virtual void Unarchive(zCArchiver& archive)
	{
		oCNpcMessage::Unarchive(archive);
		archive.ReadRaw("damage_descriptor", &damDescriptor, sizeof(damDescriptor));
	}
	virtual ~oCMsgDamage();
	virtual void IsOverlay();
	virtual void IsNetRelevant();
	virtual void IsHighPriority();
	virtual void IsDeleteable();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);

	enum oCMsgDamage::TDamageSubType : uint16_t
	{
		EV_DAMAGE_ONCE = 0x0,
		EV_DAMAGE_PER_FRAME = 0x1,
	};

	oCNpc::oSDamageDescriptor damDescriptor;
};

zSTRING oCMsgDamage::MD_GetSubTypeString(int type)
{
	switch (type) {
	case EV_DAMAGE_ONCE:      return "EV_DAMAGE_ONCE";
	case EV_DAMAGE_PER_FRAME: return "EV_DAMAGE_PER_FRAME";
	default:                  return "";
	};
}
