class zCEventMover : public zCEventMessage {
	Z_OBJECT(zCEventMover);
public:
	enum Type {
		GOTO_KEY_FIXED_DIRECTLY,
		_DISABLED_,
		GOTO_KEY_NEXT,
		GOTO_KEY_PREV,
	};

	virtual ~zCEventMover();
	virtual void IsNetRelevant();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
};

zSTRING zCEventMover::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case 0:  return "GOTO_KEY_FIXED_DIRECTLY";
	case 1:  return "_DISABLED_";
	case 2:  return "GOTO_KEY_NEXT";
	case 3:  return "GOTO_KEY_PREV";
	default: return "";
	}
}
