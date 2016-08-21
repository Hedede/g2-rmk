//Danke an Nico für die folgenden drei Klassen:
struct oSMenuInfoAttribute {
	oSMenuInfoAttribute() = default;

	zSTRING Name;
	zSTRING Description;

	int    Value;
	int    MaxValue;

	enum {
		HP   = 0,
		DEX  = 1,
		MANA = 2,
		STR  = 3,
	} Type;
};

struct oSMenuInfoTalent {
	oSMenuInfoTalent() = default;
	zSTRING Name;
	zSTRING Description;
	zSTRING SkillEnum;
	int    Value;
	enum {
		MELEE_1H,
		MELEE_2H,
		BOW,
		CROSSBOW
	} Skill;
};

struct oSMenuInfoArmor {
	oSMenuInfoArmor() = default;
	int Value;
};

//oCMenu_Status ist eine Unterklasse und hat zusätzlich diese Eigenschaften:
//Hab mir gespart dafür nochmal eine Extraklasse zu schreiben, für diese Eigenschaften, siehe unten:
class oCMenu_Status : zCMenu {
public:
	virtual int HandleEvent(int);
	virtual ~oCMenu_Status();

	oCMenu_Status(zSTRING const& inst)
		: zCMenu(inst);
	{
	}
private:
	zCArray <oSMenuInfoAttribute> listAttributes;
	zCArray <oSMenuInfoTalent>    listTalents;
	zCArray <oSMenuInfoArmor>     listArmory;
};


int oCMenu_Status::HandleEvent(int a2)
{
	uint16_t key = zCInput::GetFirstBindedLogicalKey(zinput, a2);
	if ( key == 0x14 )
	{
		a2 = 1;
	}
	else if ( a2 != 1 && key != 14 )
	{
		char chr = GetCharFromDIK(a2, 0, 0);
		if ( chr >= ' ' && chr <= 0xFF )
		{
			CheatString = CheatString + chr;
		}
	}
	return zCMenu::HandleEvent(a2);
}
