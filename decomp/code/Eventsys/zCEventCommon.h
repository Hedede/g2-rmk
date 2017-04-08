class zCEventCommon : public zCEventMessage {
	Z_OBJECT(zCEventCommon);
public:
	enum Type {
		ENABLE,
		DISABLE,
		TOGGLE_ENABLED,
	};
	virtual ~zCEventCommon();
	bool IsNetRelevant() override { return true; }
	int MD_GetNumOfSubTypes() override { return 4 };
	void MD_GetSubTypeString(int type) override;
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
