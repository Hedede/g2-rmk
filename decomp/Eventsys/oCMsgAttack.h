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

	virtual ~oCMsgAttack() = default;

	void Archive(zCArchiver& arc) override
	{
		oCNpcMessage::Archive(arc);

		arc.WriteInt("combo", combo);
		if ( !arc.InProperties() )
			arc.WriteBool("enableNextHit", flags & 1);
	}
	void Unarchive(zCArchiver& arc) override
	{
		oCNpcMessage::Unarchive(arc);

		arc.ReadInt("combo", combo);
		if ( !arc.InProperties() ) {
			uint32_t tmp;
			arc.ReadBool("enableNextHit", tmp);
			flags ^= (flags ^ tmp) & 1;
		}
	}

	bool IsOverlay() override
	{
		return subType == EV_AIMAT || subType == EV_DEFEND;
	}
	bool IsNetRelevant() override
	{
		return true;
	}

	int MD_GetNumOfSubTypes() override
	{
		return 12;
	}
	zSTRING MD_GetSubTypeString(int type) override;

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

