class oCMsgWeapon : public oCNpcMessage {
	Z_OBJECT(oCMsgWeapon);
public:
	enum Type {
		EV_DRAWWEAPON,
		EV_DRAWWEAPON1,
		EV_DRAWWEAPON2,
		EV_REMOVEWEAPON,
		EV_REMOVEWEAPON1,
		EV_REMOVEWEAPON2,
		EV_CHOOSEWEAPON,
		EV_FORCEREMOVEWEAPON,
		EV_ATTACK,
		EV_EQUIPBESTWEAPON,
		EV_EQUIPBESTARMOR,
		EV_UNEQUIPWEAPONS,
		EV_UNEQUIPARMOR,
		EV_EQUIPARMOR,
	};

	virtual void Archive(zCArchiver& arc)
	{
		arc.WriteInt("targetMode", targetMode);
		if ( !arc.InProperties() ) {
			arc.WriteBool("duringRun", flags << 31 >> 31);
			arc.WriteBool("initDone",  flags << 30 >> 31);
			arc.WriteBool("firstTime", flags << 29 >> 31);
		}
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		arc.ReadInt("targetMode", targetMode);
		if ( !arc.InProperties() ) {
			uint32_t tmp;

			arc.ReadBool("duringRun", tmp);
			flags[0] = tmp;
			arc.ReadBool("initDone",  tmp);
			flags[1] = tmp;
			arc.ReadBool("firstTime", tmp);
			flags[2] = tmp;
		}
	}
	virtual ~oCMsgWeapon();
	virtual void MD_GetNumOfSubTypes();
	virtual void MD_GetSubTypeString(int);
	virtual void MD_GetTimeBehavior();
	virtual void MD_GetMinTime();
};

zSTRING oCMsgWeapon::MD_GetSubTypeString(int type);
{
	switch ( type ) {
	case EV_DRAWWEAPON:        return "EV_DRAWWEAPON";
	case EV_DRAWWEAPON1:       return "EV_DRAWWEAPON1";
	case EV_DRAWWEAPON2:       return "EV_DRAWWEAPON2";
	case EV_REMOVEWEAPON:      return "EV_REMOVEWEAPON";
	case EV_REMOVEWEAPON1:     return "EV_REMOVEWEAPON1";
	case EV_REMOVEWEAPON2:     return "EV_REMOVEWEAPON2";
	case EV_CHOOSEWEAPON:      return "EV_CHOOSEWEAPON";
	case EV_FORCEREMOVEWEAPON: return "EV_FORCEREMOVEWEAPON";
	case EV_ATTACK:            return "EV_ATTACK";
	case EV_EQUIPBESTWEAPON:   return "EV_EQUIPBESTWEAPON";
	case EV_EQUIPBESTARMOR:    return "EV_EQUIPBESTARMOR";
	case EV_UNEQUIPWEAPONS:    return "EV_UNEQUIPWEAPONS";
	case EV_UNEQUIPARMOR:      return "EV_UNEQUIPARMOR";
	case EV_EQUIPARMOR:        return "EV_EQUIPARMOR";
	default:                   return "";
	}
}
