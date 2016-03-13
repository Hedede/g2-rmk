class oCMsgMovement : public oCNpcMessage {
	Z_OBJECT(oCMsgMovement);
public:
	enum Type {
		EV_ROBUSTTRACE = 0x0,
		EV_GOTOPOS = 0x1,
		EV_GOTOVOB = 0x2,
		EV_GOROUTE = 0x3,
		EV_TURN = 0x4,
		EV_TURNTOPOS = 0x5,
		EV_TURNTOVOB = 0x6,
		EV_TURNAWAY = 0x7,
		EV_JUMP = 0x8,
		EV_WHIRLAROUND = 0xA,
		EV_SETWALKMODE = 0x9,
		EV_STANDUP = 0xB,
		EV_CANSEENPC = 0xC,
		EV_STRAFE = 0xD,
		EV_GOTOFP = 0xE,
		EV_DODGE = 0xF,
		EV_BEAMTO = 0x10,
		EV_ALIGNTOFP = 0x11,
	};

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~oCMsgMovement();
	virtual void Delete();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void MD_GetVobRefName();
	virtual void MD_SetVobRefName(zSTRING const&);
	virtual void MD_SetVobParam(zCVob*);
	virtual void MD_GetTimeBehavior();
	virtual void MD_GetMinTime();
};

zSTRING oCMsgMovement::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_ROBUSTTRACE: return "EV_ROBUSTTRACE";
	case EV_GOTOPOS:     return "EV_GOTOPOS";
	case EV_GOTOVOB:     return "EV_GOTOVOB";
	case EV_GOROUTE:     return "EV_GOROUTE";
	case EV_TURN:        return "EV_TURN";
	case EV_TURNTOPOS:   return "EV_TURNTOPOS";
	case EV_TURNTOVOB:   return "EV_TURNTOVOB";
	case EV_TURNAWAY:    return "EV_TURNAWAY";
	case EV_JUMP:        return "EV_JUMP";
	case EV_WHIRLAROUND: return "EV_WHIRLAROUND";
	case EV_SETWALKMODE: return "EV_SETWALKMODE";
	case EV_STANDUP:     return "EV_STANDUP";
	case EV_CANSEENPC:   return "EV_CANSEENPC";
	case EV_STRAFE:      return "EV_STRAFE";
	case EV_GOTOFP:      return "EV_GOTOFP";
	case EV_DODGE:       return "EV_DODGE";
	case EV_BEAMTO:      return "EV_BEAMTO";
	case EV_ALIGNTOFP:   return "EV_ALIGNTOFP";
	default:             return "";
	}
}

void oCMsgMovement::Archive(zCArchiver& arc)
{
	oCNpcMessage::Archive(arc);
	if (subType == EV_GOTOFP || subType == EV_GOROUTE || subType == EV_BEAMTO)
		arc.WriteString("targetName", targetName);

	if (subType == EV_GOTOVOB  || subType == EV_TURNTOVOB ||
	    subType == EV_TURNAWAY || subType == EV_WHIRLAROUND )
	{
		if ( !arc.InProperties() )
			arc.WriteObject("targetVob", targetVob);
		arc.WriteString("targetVobName", targetVobName);
	}

	if (subType == EV_GOTOPOS || subType == EV_TURNTOPOS || subType == EV_JUMP)
		arc.WriteVec("targetPos", targetPos);

	if (subType == EV_TURN)
		arc.WriteFloat("angle", angle);

	if (subType == EV_SETWALKMODE)
		arc.WriteInt(arc, "targetMode", targetMode);
}

void oCMsgMovement::Unarchive(zCArchiver& arc)
{
	oCNpcMessage::Unarchive(arc);

	if (subType == EV_GOTOFP || subType == EV_GOROUTE || subType == EV_BEAMTO)
		arc.ReadString("targetName", targetName);

	if (subType == EV_GOTOVOB  || subType == EV_TURNTOVOB ||
	    subType == EV_TURNAWAY || subType == EV_WHIRLAROUND )
	{
		if ( !arc.InProperties() )
			targetVob = arc.ReadObject("targetVob", 0);

		arc.ReadString("targetVobName", targetVobName);
	}

	if (subType == EV_GOTOPOS || subType == EV_TURNTOPOS || subType == EV_JUMP)
		arc.ReadVec3("targetPos", targetPos);

	if (subType == EV_TURN)
		arc.ReadFloat("angle", angle);

	if (subType == EV_SETWALKMODE)
		arc.ReadInt("targetMode", targetMode);
}
