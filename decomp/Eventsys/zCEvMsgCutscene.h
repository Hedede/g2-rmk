class zCEvMsgCutscene : public zCEventMessage {
	Z_OBJECT(zCEvMsgCutscene);
public:
	virtual ~zCEvMsgCutscene();
	virtual bool IsHighPriority()
	{
		return true;
	}
	virtual void Delete()
	{
		deleted = 1;
	};
	virtual bool IsDeleted()
	{
		return deleted;
	}

	enum TCutsceneSubType : uint16_t
	{
		EV_CS_STARTPLAY = 0x0,
		EV_CS_STOP = 0x1,
		EV_CS_INTERRUPT = 0x2,
		EV_CS_RESUME = 0x3,
	};

	zCEvMsgCutscene(TCutsceneSubType subType)
		: subType(subType)
	{
	}

	virtual int MD_GetNumOfSubTypes()
	{
		return 4;
	}
	virtual zSTRING MD_GetSubTypeString(int type);
	{
		switch ( type ) {
		case EV_CS_STARTPLAY: return "EV_CS_STARTPLAY";
		case EV_CS_STOP:      return "EV_CS_STOP";
		case EV_CS_INTERRUPT: return "EV_CS_INTERRUPT";
		case EV_CS_RESUME:    return "EV_CS_RESUME";
		default:              return "";
		}
	}

	void SetCutsceneName(zSTRING csName)
	{
		this->csName = csName.Upper();
	}

	zSTRING GetCutsceneName() const
	{
		return csName;
	}
private:
	zSTRING csName;
	int notCutscene;
	int isGlobalCutscene;
	int isMainRole;
	bool deleted;
};

zCEvMsgCutscene::classDef("zCEvMsgCutscene", "zCEventMessage", zCEvMsgCutscene::_CreateNewInstance, sizeof(zCEvMsgCutscene), 0, 0, "");
