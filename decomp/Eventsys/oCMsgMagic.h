class oCMsgMagic : public oCNpcMessage {
	Z_OBJECT(oCMsgMagic);
public:
	// TMsgConversationSubType // typo in original code
	enum TMsgMagicSubType {
		EV_OPEN = 0,
		EV_CLOSE = 1,
		EV_MOVE = 2,
		EV_INVEST = 3,
		EV_CAST = 4,
		EV_SETLEVEL = 5,
		EV_SHOWSYMBOL = 6,
		EV_SETFRONTSPELL = 7,
		EV_CASTSPELL = 8,
		EV_READY = 9,
		EV_UNREADY = 10,
		NUM_SUBTYPES,
	};

	oCMsgMagic() = default;
	oCMsgMagic(TMsgMagicSubType type)
		: oCMsgMagic()
	{
		subType = type;
	}

	virtual ~oCMsgMagic() = default;

	void Archive(zCArchiver& arc) override
	{
		oCNpcMessage::Archive(arc);

		arc.WriteInt("what", what);
		arc.WriteInt("level", level);
		arc.WriteBool("removeSymbol", removeSymbol);
	}

	void Unarchive(zCArchiver& arc) override
	{
		oCNpcMessage::Unarchive(arc);

		arc.ReadInt("what", what);
		arc.ReadInt("level", level);
		arc.ReadBool("removeSymbol", removeSymbol);
	}

	bool IsNetRelevant() override
	{
		return false;
	}
	bool IsHighPriority() override
	{
		return subType != EV_CASTSPELL;
	}
	bool IsJob() override
	{
		return subType == EV_CASTSPELL;
	}

	int MD_GetNumOfSubTypes()
	{
		return NUM_SUBTYPES;
	}
	zSTRING MD_GetSubTypeString(int type) override;

	void Pack(zCBuffer& buf,zCEventManager*) override
	{
		buf.Write(&subType, 2u);
		buf.Write(&unk1, 4u);
		buf.Write(&removeSymbol, 4u);
		buf.Write(&level, 4u);
		buf.Write(&investMana, 4u);
		buf.Write(&unk2, 4u);
		buf.Write(&spellId, 4u);

		zBOOL hasTarget = !!targetVob;

		vuf.Write(&hasTarget, 4u);
		vuf.Write(&targetPos, 0xCu);
	}

	void Unpack(zCBuffer& buf,zCEventManager *) override
	{
		buf.Read(&subType, 2u);
		buf.Read(&unk1, 4u);
		buf.Read(&removeSymbol, 4u);
		buf.Read(&level, 4u);
		buf.Read(&investMana, 4u);
		buf.Read(&unk2, 4u);
		buf.Read(&spellId, 4u);

		targetVob = nullptr;

		int id;
		buf.Read(id, 4u);

		if ( id )
			targetVob = ogame->GetWorld()->SearchVobByID(buf, 0);

		buf.Read(&targetPos, 0xCu);
	}

private:
	int unk1  = 0;
	int level = 0;
	zBOOL removeSymbol = false;
	int investMana = 0;
	int unk2 = 0;

	zCVob* targetVob = 0;
	zVEC3  targetPos;

	int spellId = 0;
};

zSTRING oCMsgMagic::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_OPEN:          return "EV_OPEN";
	case EV_CLOSE:         return "EV_CLOSE";
	case EV_MOVE:          return "EV_MOVE";
	case EV_INVEST:        return "EV_INVEST";
	case EV_CAST:          return "EV_CAST";
	case EV_SETLEVEL:      return "EV_SETLEVEL";
	case EV_SHOWSYMBOL:    return "EV_SHOWSYMBOL";
	case EV_SETFRONTSPELL: return "EV_SETFRONTSPELL";
	case EV_CASTSPELL:     return "EV_CASTSPELL";
	case EV_READY:         return "EV_READY";
	case EV_UNREADY:       return "EV_UNREADY";
	default:               return "";
	}
}
