class oCMsgAttack : public oCNpcMessage {
	Z_OBJECT(oCMsgAttack);
public:
	enum TAttackSubType {
		EV_ATTACKFORWARD = 0,
		EV_ATTACKLEFT = 1,
		EV_ATTACKRIGHT = 2,
		EV_ATTACKRUN = 3,
		EV_ATTACKFINISH = 4,
		EV_PARADE = 5,
		EV_AIMAT = 6,
		EV_SHOOTAT = 7,
		EV_STOPAIM = 8,
		EV_DEFEND = 9,
		EV_UNK1 = 10,
		EV_UNK2 = 11,
	};

	oCMsgAttack() = default;
	oCMsgAttack(TAttackSubType type, int ani, int combo)
		: oCMsgAttack()
	{
		aniId = ani;
		this->combo = combo;
	}
	oCMsgAttack(TAttackSubType type, zCVob* target, float frame)
		: oCMsgAttack()
	{
		paramVob = target;
		startFrame = frame;
	}

	virtual ~oCMsgAttack() = default;

	void Archive(zCArchiver& arc) override
	{
		oCNpcMessage::Archive(arc);

		arc.WriteInt("combo", combo);
		if ( !arc.InProperties() )
			arc.WriteBool("enableNextHit", flags & 1);
	}
	void Unarchive(zCArchiver& arc) override
	{
		oCNpcMessage::Unarchive(arc);

		arc.ReadInt("combo", combo);
		if ( !arc.InProperties() ) {
			uint32_t tmp;
			arc.ReadBool("enableNextHit", tmp);
			flags ^= (flags ^ tmp) & 1;
		}
	}

	bool IsOverlay() override
	{
		return subType == EV_AIMAT || subType == EV_DEFEND;
	}
	bool IsNetRelevant() override
	{
		return true;
	}

	int MD_GetNumOfSubTypes() override
	{
		return 12;
	}
	zSTRING MD_GetSubTypeString(int type) override;

	void Pack(zCBuffer& buf,zCEventManager *) override;
	void Unpack(zCBuffer& buf,zCEventManager *) override;

private:
	zBOOL combo = false;
	zCVob* paramVob = nullptr;

	int aniId = -1;
	float startFrame = 0;

	struct {
		unsigned f1 : 1;
		unsigned f2 : 1;
	} atk_flags;
};

zSTRING oCMsgAttack::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_ATTACKFORWARD: return "EV_ATTACKFORWARD";
	case EV_ATTACKLEFT:    return "EV_ATTACKLEFT";
	case EV_ATTACKRIGHT:   return "EV_ATTACKRIGHT";
	case EV_ATTACKRUN:     return "EV_ATTACKRUN";
	case EV_ATTACKFINISH:  return "EV_ATTACKFINISH";
	case EV_PARADE:        return "EV_PARADE";
	case EV_AIMAT:         return "EV_AIMAT";
	case EV_SHOOTAT:       return "EV_SHOOTAT";
	case EV_STOPAIM:       return "EV_STOPAIM";
	case EV_DEFEND:        return "EV_DEFEND";
	default:               return "";
	}
}

void oCMsgAttack::Pack(zCBuffer& buf, zCEventManager*)
{
	buf.Write(&combo,4);
	unsigned tmp = flags.f1;
	buf.Write(&tmp,4);
	buf.Write(&startFrame,4);


	zSTRING name;
	if (auto npc = dynamic_cast<oCNpc*>(paramVob)) {
		auto model = npc->GetModel();

		for (auto proto : model->prototypes) {
			if (proto->anis[aniId]) {
				name = proto->anis[aniId]->aniName;
			}
		}
	}

	buf.WriteString(name);
}

void oCMsgAttack::Unpack(zCBuffer& buf, zCEventManager*)
{
	unsigned tmp;
	buf.Read(&combo, 4);
	buf.Read(&tmp, 4);
	flags.f1 = tmp;
	
	buf.Read(&startFrame,4);
	auto name = buf.ReadString();
	if (auto npc = dynamic_cast<oCNpc*>(paramVob)) {
		auto model = npc->GetModel();
		auto ani = model->prototypes[0]->SearchAni(name);
		if (ani)
			aniId = ani->aniId;
		else
			aniId = -1;
	}
}
