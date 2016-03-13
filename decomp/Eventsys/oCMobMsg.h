class oCMobMsg : public zCEventMessag {
	Z_OBJECT(oCMobMsg);
public:
	enum Type {
		EV_STARTINTERACTION = 0,
		EV_STARTSTATECHANGE = 1,
		EV_ENDINTERACTION = 2,
		EV_UNLOCK = 3,
		EV_LOCK = 4,
		EV_CALLSCRIPT = 5,
	};

	virtual ~oCMobMsg();
	virtual void IsNetRelevant();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager	*);
};

zSTRING oCMobMsg::MD_GetSubTypeString(int type);
{
	switch ( type ) {
	case EV_STARTINTERACTION: return "EV_STARTINTERACTION";
	case EV_STARTSTATECHANGE: return "EV_STARTSTATECHANGE";
	case EV_ENDINTERACTION:   return "EV_ENDINTERACTION";
	case EV_UNLOCK:           return "EV_UNLOCK";
	case EV_LOCK:             return "EV_LOCK";
	case EV_CALLSCRIPT:       return "EV_CALLSCRIPT";
	default:                  return "";
	}
}

