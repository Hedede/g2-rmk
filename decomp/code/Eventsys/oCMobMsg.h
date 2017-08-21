class oCMobMsg : public zCEventMessage {
	Z_OBJECT(oCMobMsg);
public:
	enum TMobMsgSubType {
		EV_STARTINTERACTION = 0,
		EV_STARTSTATECHANGE = 1,
		EV_ENDINTERACTION = 2,
		EV_UNLOCK = 3,
		EV_LOCK = 4,
		EV_CALLSCRIPT = 5,
	};

	oCMobMsg() = default;
	~oCMobMsg() = default;

	oCMobMsg(TMobMsgSubType subtype, oCNpc* npc)
		: npc{npc}
	{
		this->subtype = subtype
	}

	oCMobMsg(TMobMsgSubType subtype, oCNpc* npc, int flag)
		: oCMobMsg{subtype, npc}
	{
		flags.32 = flag;
	}

	virtual ~oCMobMsg();
	zBOOL IsNetRelevant() override { return 1 };
	void MD_GetNumOfSubTypes() override { return 5 };
	void MD_GetSubTypeString(int type) override;
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager	*);

private:
	oCNpc *npc = nullptr;
	int state = 0;
	int flags = 0;
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

