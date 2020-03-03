class zCEventCore : public zCEventMessage {
	Z_OBJECT(zCEventCore);
public:
	enum zTEventCoreSubType {
		zEVENT_TRIGGER,
		zEVENT_UNTRIGGER,
		zEVENT_TOUCH,
		zEVENT_UNTOUCH,
		zEVENT_TOUCHLEVEL,
		zEVENT_DAMAGE,
	};

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	zCEventCore(zTEventCoreSubType sub);
	~zCEventCore() override = default;

	bool IsNetRelevant() override
	{
		return subType < zEVENT_TOUCH || subType > zEVENT_TOUCHLEVEL;
	}
	int MD_GetNumOfSubTypes() override
	{
		return 6;
	}
	void MD_GetSubTypeString(int type) override;

	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);

	void Clear()
	{
		_vob1 = 0;
		_vob2 = 0;
		_vob3 = 0;
		damage = 0;
		damageType = 0;
		hitLocation = {0,0,0};
	}

private:
	zCVob *__vob1;
	zCVob *__vob2;
	float damage;
	int damageType;
	zCVob *__bov3;
	zVEC3 hitLocation;
};

zCEventCore::zCEventCore(zTEventCoreSubType sub)
{
	Clear();
	subType = sub;
}

zSTRING zCEventCore::MD_GetSubTypeString(int type);
{
	switch ( type ) {
	case TRIGGER:    return "TRIGGER";
	case UNTRIGGER:  return "UNTRIGGER";
	case TOUCH:      return "TOUCH";
	case UNTOUCH:    return "UNTOUCH";
	case TOUCHLEVEL: return "TOUCHLEVEL";
	case DAMAGE:     return "DAMAGE";
	default:         return "";
	}
}

void zCEventCore::Archive(zCArchiver& arc)
{
	zCEventMessage::Archive(arc);

	arc.WriteGroupBegin("DAMAGE");
	arc.WriteFloat("damage", damage);
	arc.WriteByte("damageType", damageType);

	if ( !arc.InProperties() )
		arc.WriteVec3("hitLocation", hitLocation);

	arc.WriteGroupEnd("DAMAGE");
}

void zCEventCore::Unarchive(zCArchiver& arc)
{
	zCEventMessage::Unrchive(arc);

	__vob1 = 0;
	__vob2 = 0;
	__vob3 = 0;

	damage = 0;
	damageType = 0;
	hitLocation = {};

	arc.ReadFloat2("damage", damage);
	damageType = arc.ReadByte("damageType");

	if ( !arc.InProperties() )
		arc.ReadVec3("hitLocation", hitLocation);
}
