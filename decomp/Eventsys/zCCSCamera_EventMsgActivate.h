class zCCSCamera_EventMsgActivate : public zCEventMessage {
	Z_OBJECT(zCCSCamera_EventMsgActivate);
public:
	enum Type {
		EV_CAM_DEACTIVATE,
		EV_CAM_SETPOSREFERENCE,
		EV_CAM_SETTARGETREFERENCE,
	};

	virtual void Archive(zCArchiver& arc)
	{
		zCEventMessage::Archive(arc);
		arc.WriteString("refVobName", refVobName);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEventMessage::Unarchive(arc);
		refVobName = arc.ReadString("refVobName");
	}
	virtual ~zCCSCamera_EventMsgActivate();
	virtual void Delete();
	virtual void IsDeleted();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void MD_GetVobRefName();
	virtual void MD_SetVobRefName(zSTRING const&);
	virtual void MD_SetVobParam(zCVob*);
private:
	zSTRING refVobName;
};

zSTRING zCCSCamera_EventMsgActivate::MD_GetSubTypeString(int type)
{
	switch (type) {
	case 0:  return "EV_CAM_DEACTIVATE";
	case 1:  return "EV_CAM_SETPOSREFERENCE";
	case 2:  return "EV_CAM_SETTARGETREFERENCE";
	default: return "";
	}
}
