//************************************************
//   Talente sehen so aus:
//************************************************
enum oENpcTalent
{
	NPC_TALENT_UNKNOWN = 0x0,
	NPC_TALENT_1H = 0x1,
	NPC_TALENT_2H = 0x2,
	NPC_TALENT_BOW = 0x3,
	NPC_TALENT_CROSSBOW = 0x4,
	NPC_TALENT_PICKLOCK = 0x5,
	NPC_TALENT_MAGE = 0x7,
	NPC_TALENT_SNEAK = 0x8,
	NPC_TALENT_REGENERATE = 0x9,
	NPC_TALENT_FIREMASTER = 0xA,
	NPC_TALENT_ACROBAT = 0xB,
	NPC_TALENT_PICKPOCKET = 0xC,
	NPC_TALENT_SMITH = 0xD,
	NPC_TALENT_RUNES = 0xE,
	NPC_TALENT_ALCHEMY = 0xF,
	NPC_TALENT_TAKEANIMALTROPHY = 0x10,
	NPC_TALENT_FOREIGNLANGUAGE = 0x11,
	NPC_TALENT_WISPDETECTOR = 0x12,
	NPC_TALENT_C = 0x13,
	NPC_TALENT_D = 0x14,
	NPC_TALENT_E = 0x15,
	NPC_TALENT_MAX = 0x16,
};

class oCNpcTalent : public zCObject {
	CLASSDEF_DEFINE;
public:
	oCNpcTalent() = default;
	virtual ~oCNpcTalent() = default;

	virtual void Archive(zCArchiver& archiver)
	{
		archiver.ReadInt("talent", talent);
		archiver.ReadInt("value", value);
		archiver.ReadInt("skill", skill);
	}

	virtual void Unarchive(zCArchiver& archiver)
	{
		archiver.WriteInt("talent", talent);
		archiver.WriteInt("value", value);
		archiver.WriteInt("skill", skill);
	}

	static void CreateTalentList(zCArray<oCNpcTalent*>& list)
	{
		// TRA4ENO
		list.DeleteList();
		//list = zCArray<oCNpcTalent*>(NPC_TALENT_MAX);
		for (int i = NPC_TALENT_MAX; i > 0; --i) {
			list.Insert(nullptr);
		}
	}

	//welches Talent? selbe Konstanten wie in Constants.d (z.B: NPC_TALENT_1H)
	oENpcTalent talent;
	int skill;
	int value;
};

zCClassDef oCNpcTalent::classdef("oCNpcTalent", "zCObject", oCNpcTalent::_CreateNewInstance, sizeof(oCNpcTalent), 0, 1, "");
