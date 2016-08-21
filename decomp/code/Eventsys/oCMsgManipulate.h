class oCMsgManipulate : public oCNpcMessage {
	Z_OBJECT(oCMsgManipulate);
public:
	enum TManipulateSubType {
		EV_TAKEVOB  = 0x0,
		EV_DROPVOB  = 0x1,
		EV_THROWVOB = 0x2,
		EV_EXCHANGE = 0x3,
		EV_USEMOB   = 0x4,
		EV_TAKEMOB  = 0x10,
		EV_DROPMOB  = 0x11,
		EV_USEITEM  = 0x5,
		EV_USEITEMTOSTATE = 0xF,
		EV_INSERTINTERACTITEM   = 0x6,
		EV_REMOVEINTERACTITEM   = 0x7,
		EV_CREATEINTERACTITEM   = 0x8,
		EV_DESTROYINTERACTITEM  = 0x9,
		EV_PLACEINTERACTITEM    = 0xA,
		EV_EXCHANGEINTERACTITEM = 0xB,
		EV_USEMOBWITHITEM = 0xC,
		EV_CALLSCRIPT     = 0xD,
		EV_EQUIPITEM      = 0xE,
	};

	oCMsgManipulate() = default;
	oCMsgManipulate(TManipulateSubType type)
		: oCMsgManipulate()
	{
		subType = type;
	}
	oCMsgManipulate(TManipulateSubType type, int slot)
		: oCMsgManipulate(type)
	{
		targetState = slot;
	}
	oCMsgManipulate(TManipulateSubType type, zCVob* vob)
		: oCMsgManipulate(type)
	{
		paramVob = vob;
	}
	oCMsgManipulate(TManipulateSubType type, zCVob* vob, int target)
		: oCMsgManipulate(type, vob)
	{
		targetState = target;
	}
	oCMsgManipulate(TManipulateSubType type, zSTRING const& mobScheme, int target)
		: oCMsgManipulate(type)
	{
		targetState = target;
		schemeName = mobScheme;
	}
	oCMsgManipulate(TManipulateSubType type, zSTRING const& mobScheme, zSTRING const& slotName)
		: oCMsgManipulate(type)
	{
		targetState = target;
		itemSlot = slotName;
	}

	virtual ~oCMsgManipulate() = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	bool IsNetRelevant() override
	{
		return subType >= EV_INSERTINTERACTITEM && subType <= EV_EQUIPITEM;
	}

	int MD_GetNumOfSubTypes() override
	{
		return 18;
	}
	zSTRING MD_GetSubTypeString(int type) override;
	zSTRING MD_GetVobRefName() override;
	void MD_SetVobRefName(zSTRING const& name) override;
	{
		targetVobName = name;
	}
	void MD_SetVobParam(zCVob* vob) override
	{
		paramVob = vob;
	}
	int MD_GetTimeBehavior() override
	{
		switch ( subType ) {
		case EV_TAKEVOB:
		case EV_DROPVOB:
		case EV_THROWVOB:
		case EV_USEMOB:
		case EV_USEITEM:
		case EV_USEITEMTOSTATE:
		case EV_TAKEMOB:
		case EV_DROPMOB:
			return 2;
		default:
			return 0;
		}
	}
	float MD_GetMinTime() override
	{
		return 6.0;
	}

	void Pack(zCBuffer& buf,zCEventManager *) override;
	void Unpack(zCBuffer& buf,zCEventManager *) override;

private:
	zSTRING schemeName;
	zSTRING itemSlot;
	zCVob *paramVob = nullptr;
	int inState     = 0; // unsure
	float distance  = 0.5; // unsure
	int targetState = -1;
};

zSTRING oCMsgManipulate::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_TAKEVOB:              return "EV_TAKEVOB";
	case EV_DROPVOB:              return "EV_DROPVOB";
	case EV_THROWVOB:             return "EV_THROWVOB";
	case EV_EXCHANGE:             return "EV_EXCHANGE";
	case EV_USEMOB:               return "EV_USEMOB";
	case EV_TAKEMOB:              return "EV_TAKEMOB";
	case EV_DROPMOB:              return "EV_DROPMOB";
	case EV_USEITEM:              return "EV_USEITEM";
	case EV_USEITEMTOSTATE:       return "EV_USEITEMTOSTATE";
	case EV_INSERTINTERACTITEM:   return "EV_INSERTINTERACTITEM";
	case EV_REMOVEINTERACTITEM:   return "EV_REMOVEINTERACTITEM";
	case EV_CREATEINTERACTITEM:   return "EV_CREATEINTERACTITEM";
	case EV_DESTROYINTERACTITEM:  return "EV_DESTROYINTERACTITEM";
	case EV_PLACEINTERACTITEM:    return "EV_PLACEINTERACTITEM";
	case EV_EXCHANGEINTERACTITEM: return "EV_EXCHANGEINTERACTITEM"
	case EV_USEMOBWITHITEM:       return "EV_USEMOBWITHITEM";
	case EV_CALLSCRIPT:           return "EV_CALLSCRIPT";
	case EV_EQUIPITEM:            return "EV_EQUIPITEM";
	default:                      return "";
	}
}

zSTRING oCMsgManipulate::MD_GetVobRefName()
{
	if (in(subType, EV_TAKEMOB, EV_DROPMOB, EV_DROPVOB, EV_USEITEM, EV_USEITEMTOSTATE, EV_EQUIPITEM))
		return "";
	return targetVobName;
}

void oCMsgManipulate::Archive(zCArchiver& arc)
{
	if (subType == EV_TAKEMOB)
		arc.WriteString("targetVobName", targetVobName);

	if (subType == EV_DROPVOB   || subType == EV_USEMOB     ||
	    subType == EV_DROPMOB   || subType == EV_TAKEMOB    ||
	    subType == EV_USEITEM   || subType == EV_CALLSCRIPT ||
	    subType == EV_EQUIPITEM || subType == EV_USEITEMTOSTATE)
		arc.WriteString("schemeName", schemeName);

	if (subType == EV_USEMOB || subType == EV_USEITEMTOSTATE)
		arc.WriteInt("targetState", targetState);
}

void oCMsgManipulate::Unarchive(zCArchiver& arc)
{
	if (subType == EV_TAKEMOB)
		arc.ReadString("targetVobName", targetVobName);

	if (subType == EV_DROPVOB   || subType == EV_USEMOB     ||
	    subType == EV_DROPMOB   || subType == EV_TAKEMOB    ||
	    subType == EV_USEITEM   || subType == EV_CALLSCRIPT ||
	    subType == EV_EQUIPITEM || subType == EV_USEITEMTOSTATE)
		arc.ReadString("schemeName", schemeName);

	if (subType == EV_USEMOB || subType == EV_USEITEMTOSTATE)
		arc.ReadInt("targetState", targetState);
}

void oCMsgManipulate::Pack(zCBuffer& buf, zCEventManager*)
{
	buf.Write(&subType,2);
	buf.WriteString(schemeName);
	buf.WriteString(itemSlot);
	unsigned tmp = !!paramVob;
	buf.Write(&tmp, 4);
	buf.Write(&targetState, 4);
	buf.WriteBool(inState, 32);
}

void oCMsgManipulate::Unpack(zCBuffer& buf, zCEventManager*)
{
	buf.Read(&subType,2);
	schemeName = buf.ReadString();
	itemSlot = buf.ReadString();

	paramVob = nullptr;
	unsigned tmp;
	buf.Write(&tmp, 4);
	if (tmp > 0)
		paramVob = ogame->GetWorld()->SearchVobByID(tmp, 0);

	buf.Read(&targetState, 4);
	inState = buf.ReadBool(32);
}
