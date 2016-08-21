struct oCMsgDamage : public oCNpcMessage {
	Z_OBJECT(oCMsgDamage);
public:
	enum TDamageSubType : uint16_t {
		EV_DAMAGE_ONCE = 0x0,
		EV_DAMAGE_PER_FRAME = 0x1,
		NUM_SUBTYPES,
	};

	oCMsgDamage() = default;
	oCMsgDamage(TDamageSubType type)
		: oCMsgDamage()
	{
		subType = type;
	}

	virtual ~oCMsgDamage() = default;

	void Archive(zCArchiver& archive) override
	{
		oCNpcMessage::Archive(archive);
		archive.WriteRaw("damage_descriptor", &damDescriptor, sizeof(damDescriptor));
	}

	void Unarchive(zCArchiver& archive) override
	{
		oCNpcMessage::Unarchive(archive);
		archive.ReadRaw("damage_descriptor", &damDescriptor, sizeof(damDescriptor));
	}

	bool IsOverlay() override
	{
		return subType == EV_DAMAGE_PER_FRAME;
	}
	bool IsNetRelevant() override
	{
		return true;
	}
	int IsHighPriority() override
	{
		flags.HIGH_PRIORITY = 1;
		return -1;
	}
	bool IsDeleteable() override
	{
		return false;
	}

	int MD_GetNumOfSubTypes() override
	{
		return NUM_SUBTYPES;
	}
	zSTRING MD_GetSubTypeString(int type) override;

	void Pack(zCBuffer &,zCEventManager *) override {}
	void Unpack(zCBuffer &,zCEventManager *) override {}

private:
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
