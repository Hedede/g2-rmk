class zCCSCamera_EventMsg : public zCEventMessage {
	Z_OBJECT(zCCSCamera_EventMsg);
public:
	enum Type {
		EV_CAM_NOTHING,
		EV_CAM_PLAY,
		EV_CAM_PAUSE,
		EV_CAM_RESUME,
		EV_CAM_STOP,
		EV_CAM_GOTO_KEY,
		EV_CAM_SET_DURATION,
		EV_CAM_SET_TO_TIME,
	};

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCSCamera_EventMsg();
	virtual void IsHighPriority();
	virtual void IsJob();
	virtual void Delete();
	virtual void IsDeleted();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void MD_GetTimeBehavior();
	virtual void MD_GetMinTime();
};

zSTRING zCCSCamera_EventMsg::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_CAM_NOTHING:      return "EV_CAM_NOTHING";
	case EV_CAM_PLAY:         return "EV_CAM_PLAY";
	case EV_CAM_PAUSE:        return "EV_CAM_PAUSE";
	case EV_CAM_RESUME:       return "EV_CAM_RESUME";
	case EV_CAM_STOP:         return "EV_CAM_STOP";
	case EV_CAM_GOTO_KEY:     return "EV_CAM_GOTO_KEY";
	case EV_CAM_SET_DURATION: return "EV_CAM_SET_DURATION";
	case EV_CAM_SET_TO_TIME:  return "EV_CAM_SET_TO_TIME";
	default:                  return "";
	}
}


void zCCSCamera_EventMsg::Archive(zCArchiver& arc)
{
	zCEventMessage::Archive(arc);
	switch ( subType ) {
	case EV_CAM_GOTO_KEY:
		arc.WriteFloat("time", time);
		arc.WriteInt("key", key);
		arc.WriteEnum("kfType", "UNDEF;TARGET;CAM", kfType);
		break;
	case EV_CAM_PLAY:
	case EV_CAM_SET_DURATION:
	case EV_CAM_SET_TO_TIME:
		arc.WriteFloat("time", time);
		break;
	default:
		return;
	}
}

void zCCSCamera_EventMsg::Unarchive(zCArchiver& arc)
{
	zCEventMessage::Unarchive(arc);
	switch ( subType ) {
	case EV_CAM_GOTO_KEY:
		arc.ReadFloat("time", time);
		arc.ReadInt("key", key);
		arc.ReadEnum("kfType", kfType);
		break;
	case EV_CAM_PLAY:
	case EV_CAM_SET_DURATION:
	case EV_CAM_SET_TO_TIME:
		arc.ReadFloat("time", time);
		break;
	default:
		return;
	}
}
