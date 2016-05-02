class oCMsgAttack : public oCNpcMessage {
	Z_OBJECT(oCMsgAttack);
public:
	enum Type {
		EV_ATTACKFORWARD = 0,
		EV_ATTACKLEFT = 1,
		EV_ATTACKRIGHT = 2,
		EV_ATTACKRUN = 3,
		EV_ATTACKFINISH = 4,
		EV_PARADE = 5,
		EV_AIMAT = 6,
		EV_SHOOTAT = 7,
		EV_STOPAIM = 8,
		EV_DEFEND = 9,
	};

	virtual void Archive(zCArchiver& arc)
	{
		oCNpcMessage::Archive(arc);

		arc.WriteInt("combo", combo);
		if ( !arc.InProperties() )
			arc.WriteBool("enableNextHit", flags & 1);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		oCNpcMessage::Unarchive(arc);

		arc.ReadInt("combo", combo);
		if ( !arc.InProperties() ) {
			uint32_t tmp;
			arc.ReadBool("enableNextHit", tmp);
			flags ^= (flags ^ tmp) & 1;
		}
	}
	virtual ~oCMsgAttack();
	virtual void IsOverlay();
	virtual void IsNetRelevant();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);

private:
	zBOOL combo;
	zCVob* paramVob;

	int unk2,unk3;

	int flags;
};

zSTRING oCMsgAttack::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_ATTACKFORWARD: return "EV_ATTACKFORWARD";
	case EV_ATTACKLEFT:    return "EV_ATTACKLEFT";
	case EV_ATTACKRIGHT:   return "EV_ATTACKRIGHT";
	case EV_ATTACKRUN:     return "EV_ATTACKRUN";
	case EV_ATTACKFINISH:  return "EV_ATTACKFINISH";
	case EV_PARADE:        return "EV_PARADE";
	case EV_AIMAT:         return "EV_AIMAT";
	case EV_SHOOTAT:       return "EV_SHOOTAT";
	case EV_STOPAIM:       return "EV_STOPAIM";
	case EV_DEFEND:        return "EV_DEFEND";
	default:               return "";
	}
}

