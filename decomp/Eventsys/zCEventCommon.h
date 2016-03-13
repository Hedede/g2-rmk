class zCEventCommon : public zCEventMessage {
	Z_OBJECT(zCEventCommon);
public:
	enum Type {
		ENABLE,
		DISABLE,
		TOGGLE_ENABLED,
	};
	virtual ~zCEventCommon();
	virtual void IsNetRelevant();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
};

zSTRING zCEventCommon::MD_GetSubTypeString(int type);
{
	switch (type) {
	case ENABLE: return "ENABLE";
	case DISBLE: return "DISBLE";
	case TOGGLE_ENABLED: return "TOGGLE_ENABLED";
	default: return "";
	};
}
