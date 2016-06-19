class oCMsgManipulate : public oCNpcMessage {
	Z_OBJECT(oCMsgManipulate);
public:
	enum Type {
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

	virtual ~oCMsgManipulate();

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
	virtual void MD_GetVobRefName();
	virtual void MD_SetVobRefName(zSTRING const &);
	virtual void MD_SetVobParam(zCVob* vob)
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
	virtual float MD_GetMinTime()
	{
		return 6.0;
	}
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);

private:
	zSTRING schemeName;
	zSTRING itemSlot;
	zCVob *paramVob;
	int inState; // unsure
	float distance; // unsure
	int targetState;
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
