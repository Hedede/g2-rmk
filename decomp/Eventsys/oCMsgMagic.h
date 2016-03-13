class oCMsgMagic : public oCNpcMessage {
	Z_OBJECT(oCMsgMagic);
public:
	enum Type {
		EV_OPEN = 0,
		EV_CLOSE = 1,
		EV_MOVE = 2,
		EV_INVEST = 3,
		EV_CAST = 4,
		EV_SETLEVEL = 5,
		EV_SHOWSYMBOL = 6,
		EV_SETFRONTSPELL = 7,
		EV_CASTSPELL = 8,
		EV_READY = 9,
		EV_UNREADY = 10,
	};

	virtual void Archive(zCArchiver& arc)
	{
		oCNpcMessage::Archive(arc);

		arc.WriteInt("what", what);
		arc.WriteInt("level", level);
		arc.WriteBool("removeSymbol", removeSymbol);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		oCNpcMessage::Unarchive(arc);

		arc.ReadInt("what", what);
		arc.ReadInt("level", level);
		arc.ReadBool("removeSymbol", removeSymbol);
	}

	virtual ~oCMsgMagic();
	virtual void IsNetRelevant();
	virtual void IsHighPriority();
	virtual void IsJob();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void Pack(zCBuffer &,zCEventManager*);
	virtual void Unpack(zCBuffer &,zCEventManager *);
};

zSTRING oCMsgMagic::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_OPEN:          return "EV_OPEN";
	case EV_CLOSE:         return "EV_CLOSE";
	case EV_MOVE:          return "EV_MOVE";
	case EV_INVEST:        return "EV_INVEST";
	case EV_CAST:          return "EV_CAST";
	case EV_SETLEVEL:      return "EV_SETLEVEL";
	case EV_SHOWSYMBOL:    return "EV_SHOWSYMBOL";
	case EV_SETFRONTSPELL: return "EV_SETFRONTSPELL";
	case EV_CASTSPELL:     return "EV_CASTSPELL";
	case EV_READY:         return "EV_READY";
	case EV_UNREADY:       return "EV_UNREADY";
	default:               return "";
	}
}
