class oCMsgState : public oCNpcMessage {
	Z_OBJECT(oCMsgState);
public:
	enum Type {
		EV_STARTSTATE,
		EV_WAIT,
		EV_SETNPCSTOSTATE,
		EV_SETTIME,
		EV_APPLYTIMEDOVERLAY,
		NUM_SUBTYPES,
	};

	virtual ~oCMsgState() = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual void IsOverlay();

	int MD_GetNumOfSubTypes() override
	{
		return NUM_SUBTYPES;
	}
	zSTRING MD_GetSubTypeString(int type) override;
	int MD_GetTimeBehavior() override
	{
		return 1;
	}
	float MD_GetMinTime() override
	{
		switch ( this->subType ) {
		case EV_STARTSTATE:
		case EV_SETNPCSTOSTATE:
		case EV_SETTIME:
			return 0.0;
		case EV_WAIT:
			return time * 0.001;
		default:
			return 3.0;
		}
	}

	bool IsOverlay() const
	{
		return subType == EV_APPLYTIMEDOVERLAY;
	}

private:
	union {
		int hour, func;
	};
	int minute;
	int state;
	zSTRING waypoint;
	float time;
	oCNpc* other;
	oCNpc* victim;
	int flags;
};

zSTRING oCMsgState::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_STARTSTATE:        return "EV_STARTSTATE";
	case EV_WAIT:              return "EV_WAIT";
	case EV_SETNPCSTOSTATE:    return "EV_SETNPCSTOSTATE";
	case EV_SETTIME:           return "EV_SETTIME";
	case EV_APPLYTIMEDOVERLAY: return "EV_APPLYTIMEDOVERLAY";
	default:                   return "";
	}
}

void oCMsgState::Archive(zCArchiver& arc)
{
	oCNpcMessage::Archive(arc);
	if (subType == EV_WAIT || subType == EV_APPLYTIMEDOVERLAY)
		arc.WriteFloat("time", v2->other);
	if (subType == EV_SETTIME) {
		arc.WriteInt("hour", hour);
		arc.WriteInt("minute", minute);
	}
}

void oCMsgState::Unarchive(zCArchiver& arc)
{
	oCNpcMessage::Unarchive(arc);
	if (subType == EV_WAIT || subType == EV_APPLYTIMEDOVERLAY)
		arc.ReadFloat("time", v2->other);
	if (subType == EV_SETTIME) {
		arc.ReadInt("hour", hour);
		arc.ReadInt("minute", minute);
	}
}
