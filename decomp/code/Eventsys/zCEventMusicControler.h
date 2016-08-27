class zCEventMusicControler : public zCEventMessage {
	Z_OBJECT(zCEventMusicControler);
public:
	enum Type {
		START_SPECIAL_SGT,
		STOP_SPECIAL_SGT,
		NUM_SUBTYPES
	};

	void Archive(zCArchiver& arc) override
	{
		zCEventMessage::Archive(arc);
		arc.WriteString("SGT", fileName);
	}

	void Unarchive(zCArchiver& arc) override
	{
		zCEventMessage::Unarchive(arc);
		arc.ReadString("SGT", fileName);
	}

	~zCEventMusicControler() override;
	int MD_GetNumOfSubTypes() override
	{
		return NUM_SUBTYPES;
	}
	zSTRING MD_GetSubTypeString(int type) override
	{
		switch (type) {
		case START_SPECIAL_SGT: return "START_SPECIAL_SGT";
		case STOP_SPECIAL_SGT:  return "STOP_SPECIAL_SGT";
		default: return "";
		}
	}

	int MD_GetTimeBehavior() override { return 0; }
	void MD_GetMinTime() override { return 0.0; }

private:
	zSTRING fileName;
};
