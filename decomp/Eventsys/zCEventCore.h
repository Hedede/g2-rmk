class zCEventCore : public zCEventMessage {
	Z_OBJECT(zCEventCore);
public:
	enum Type {
		TRIGGER,
		UNTRIGGER,
		TOUCH,
		UNTOUCH,
		TOUCHLEVEL,
		DAMAGE,
	};

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCEventCore();
	virtual void IsNetRelevant();
	virtual int MD_GetNumOfSubTypes()
	{
		return 6;
	}
	virtual void MD_GetSubTypeString(int);
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);
};

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
