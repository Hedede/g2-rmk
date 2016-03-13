class zCEventMusicControler : public zCEventMessage {
	Z_OBJECT(zCEventMusicControler);
public:
	enum Type {
		START_SPECIAL_SGT,
		STOP_SPECIAL_SGT,
	};

	virtual void Archive(zCArchiver& arc)
	{
		zCEventMessage::Archive(arc);
		arc.WriteString("SGT", fileName);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEventMessage::Unarchive(arc);
		arc.ReadString("SGT", fileName);
	}

	virtual ~zCEventMusicControler();
	virtual void MD_GetNumOfSubTypes();
	virtual zSTRING MD_GetSubTypeString(int type)
	{
		switch (type) {
		case START_SPECIAL_SGT: return "START_SPECIAL_SGT";
		case STOP_SPECIAL_SGT:  return "STOP_SPECIAL_SGT";
		default: return "";
		}
	}

	virtual void MD_GetTimeBehavior();
	virtual void MD_GetMinTime();
private:
	zSTRING fileName;
};
