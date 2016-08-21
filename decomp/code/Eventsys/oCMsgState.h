class oCMsgState : public oCNpcMessage {
	Z_OBJECT(oCMsgState);
public:
	enum TStateSubType {
		EV_STARTSTATE,
		EV_WAIT,
		EV_SETNPCSTOSTATE,
		EV_SETTIME,
		EV_APPLYTIMEDOVERLAY,
		NUM_SUBTYPES,
	};

	oCMsgState() = default;
	oCMsgState(TStateSubType type)
		: oCMsgState()
	{
		subType = type;
	}
	oCMsgState(TStateSubType type, int func)
		: oCMsgState(type)
	{
		param = func;
	}
	oCMsgState(TStateSubType type, float time)
		: oCMsgState(type)
	{
		this->time = time;
	}
	oCMsgState(TStateSubType type, int func, int flags, zSTRING const& wp)
		: oCMsgState(type, func)
	{
		st_flags = flags;
		waypoint = wp;
	}

	virtual ~oCMsgState() = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	bool IsOverlay() override
	{
		return subType == EV_APPLYTIMEDOVERLAY;
	}

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
	//union { int hour, func; };
	int param  = -1;
	int minute = 0;
	int state  = -1;
	zSTRING waypoint;
	float time = 0.0;

	oCNpc* other  = nullptr;
	oCNpc* victim = nullptr;

	struct {
		unsigned f1 : 1;
		unsigned f2 : 2;
	} st_flags;
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
