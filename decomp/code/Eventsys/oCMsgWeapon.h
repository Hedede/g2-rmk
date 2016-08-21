class oCMsgWeapon : public oCNpcMessage {
	Z_OBJECT(oCMsgWeapon);
public:
	enum TWeaponSubType {
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

	oCMsgWeapon()
		: oCNpcMessage()
	{
		mw_flags.firstTime = true;
	}
	oCMsgWeapon(TWeaponSubType type, int tmode, int useFist)
		: oCMsgWeapon()
	{
		subType = type;
		targetMode = tmode;
		mw_flags.useFist = useFist;
	}

	virtual ~oCMsgWeapon() = default;

	void Archive(zCArchiver& arc) override
	{
		arc.WriteInt("targetMode", targetMode);
		if ( !arc.InProperties() ) {
			arc.WriteBool("duringRun", flags << 31 >> 31);
			arc.WriteBool("initDone",  flags << 30 >> 31);
			arc.WriteBool("firstTime", flags << 29 >> 31);
		}
	}
	void Unarchive(zCArchiver& arc) override
	{
		arc.ReadInt("targetMode", targetMode);
		if ( !arc.InProperties() ) {
			uint32_t tmp;

			flags.duringRun = arc.ReadBool("duringRun", tmp);
			flags.initDone  = arc.ReadBool("initDone",  tmp);
			flags.firstTime = arc.ReadBool("firstTime", tmp);
		}
	}

	int MD_GetNumOfSubTypes() override
	{
		return 14;
	}
	zSTRING MD_GetSubTypeString(int type) override;
	int MD_GetTimeBehavior() override
	{
		return 0;
	}
	float MD_GetMinTime() override
	{
		return 0.0;
	}

private:
	int targetMode = 0;
	struct {
		unsigned duringRun  : 1; // 1
		unsigned initDone   : 1; // 2
		unsigned firstTime  : 1; // 4
		unsigned useFist    : 1; // 8
	} mw_flags;
	int aniId = -1;
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
