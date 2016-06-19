class oCMsgConversation : public oCNpcMessage {
	Z_OBJECT(oCMsgConversation);
public:
	enum Type {
		EV_PLAYANISOUND = 0x0,
		EV_PLAYANI = 0x1,
		EV_PLAYSOUND = 0x2,
		EV_LOOKAT = 0x3,
		EV_OUTPUT = 0x4,
		EV_OUTPUTSVM = 0x5,
		EV_CUTSCENE = 0x6,
		EV_WAITTILLEND = 0x7,
		EV_ASK = 0x8,
		EV_WAITFORQUESTION = 0x9,
		EV_STOPLOOKAT = 0xA,
		EV_STOPPOINTAT = 0xB,
		EV_POINTAT = 0xC,
		EV_QUICKLOOK = 0xD,
		EV_PLAYANI_NOOVERLAY = 0xE,
		EV_PLAYANI_FACE = 0xF,
		EV_PROCESSINFOS = 0x10,
		EV_STOPPROCESSINFOS = 0x11,
		EV_OUTPUTSVM_OVERLAY = 0x12,
		EV_SNDPLAY = 0x13,
	};

	virtual ~oCMsgConversation();

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	bool IsOverlay() override;
	virtual void Delete();

	int MD_GetNumOfSubTypes() override
	{
		return 0x18;
	}
	zSTRING MD_GetSubTypeString(int type) override;
	zSTRING MD_GetVobRefName() override
	{
		return targetVobName;
	}
	virtual void MD_SetVobRefName(zSTRING	const &);
	void MD_SetVobParam(zCVob* vob) override
	{
		target = vob;
	}
	int MD_GetTimeBehavior() override;
	float MD_GetMinTime() override;

private:
	zSTRING text;
	zSTRING name;
	oCNpc *target;
	zVEC3 positionWorld;
	int aniId;
	zCEventMessage *talkingWith;
	zCActiveSnd *voiceHandle;
	float timeSec;
	int number;
	int posX;
	int posY;
};

void oCMsgConversation::Archive(zCArchiver& arc)
{
	if (subType == EV_PLAYSOUND || subType == EV_CUTSCENE ||
	    subType == EV_PLAYANISOUND)
		arc.WriteString("text", text);
	
	if (subType == EV_PLAYANI     || subType == EV_PLAYANI_FACE ||
	    subType == EV_PLAYSOUND   || subType == EV_OUTPUTSVM    ||
	    subType == EV_CUTSCENE    || subType == EV_PLAYANISOUND)
		arc.WriteString("name", name);

	if (subType == EV_LOOKAT      || subType == EV_POINTAT ||
	    subType == EV_WAITTILLEND || subType == EV_QUICKLOOK )
		arc.WriteString("targetVobName", targetVobName);

	if (subType == EV_OUTPUT )
		arc.WriteInt("number", number);
}

void oCMsgConversation::Unarchive(zCArchiver& arc)
{
	if (subType == EV_PLAYSOUND || subType == EV_CUTSCENE ||
	    subType == EV_PLAYANISOUND)
		arc.ReadString("text", text);

	if (subType == EV_PLAYANI     || subType == EV_PLAYANI_FACE ||
	    subType == EV_PLAYSOUND   || subType == EV_OUTPUTSVM    ||
	    subType == EV_CUTSCENE    || subType == EV_PLAYANISOUND)
	{
		arc.ReadString("name", name);
		name.Upper();
	}

	if (subType == EV_LOOKAT      || subType == EV_POINTAT ||
	    subType == EV_WAITTILLEND || subType == EV_QUICKLOOK) {
		arc.ReadString("targetVobName", targetVobName);
		name = targetVobName; //?
	}

	if (subType == EV_OUTPUT)
		arc.ReadInt("number", number);
}


enum Type {
	EV_OUTPUT = 0x4,
	EV_OUTPUTSVM = 0x5,
	EV_CUTSCENE = 0x6,
	EV_WAITTILLEND = 0x7,
	EV_ASK = 0x8,
	EV_WAITFORQUESTION = 0x9,
	EV_STOPLOOKAT = 0xA,
	EV_STOPPOINTAT = 0xB,
};

int oCMsgConversation::IsOverlay()
{
	switch (subType) {
	case EV_PLAYANI_NOOVERLAY:
	case EV_SNDPLAY:
	case EV_STOPPROCESSINFOS:
	case 21:
	case 22:
	case 23:
		return false;
	case EV_PLAYANISOUND:
	case EV_PLAYANI:
	case EV_PLAYSOUND:
	case EV_LOOKAT:
	case EV_POINTAT:
	case EV_QUICKLOOK:
	case EV_PLAYANI_FACE:
	case EV_PROCESSINFOS:
	case EV_OUTPUTSVM_OVERLAY:
		return true;
	default:
		return false;
	}
}


zSTRING oCMsgConversation::MD_GetSubTypeString(int type)
{
	switch ( type ) {
	case EV_PLAYANISOUND:      return "EV_PLAYANISOUND";
	case EV_PLAYANI:           return "EV_PLAYANI";
	case EV_PLAYSOUND:         return "EV_PLAYSOUND";
	case EV_LOOKAT:            return "EV_LOOKAT";
	case EV_OUTPUT:            return "EV_OUTPUT";
	case EV_OUTPUTSVM:         return "EV_OUTPUTSVM";
	case EV_CUTSCENE:          return "EV_CUTSCENE";
	case EV_WAITTILLEND:       return "EV_WAITTILLEND";
	case EV_ASK:               return "EV_ASK";
	case EV_WAITFORQUESTION:   return "EV_WAITFORQUESTION";
	case EV_STOPLOOKAT:        return "EV_STOPLOOKAT";
	case EV_STOPPOINTAT:       return "EV_STOPPOINTAT";
	case EV_POINTAT:           return "EV_POINTAT";
	case EV_QUICKLOOK:         return "EV_QUICKLOOK";
	case EV_PLAYANI_NOOVERLAY: return "EV_PLAYANI_NOOVERLAY";
	case EV_PLAYANI_FACE:      return "EV_PLAYANI_FACE";
	case EV_PROCESSINFOS:      return "EV_PROCESSINFOS";
	case EV_STOPPROCESSINFOS:  return "EV_STOPPROCESSINFOS";
	case EV_OUTPUTSVM_OVERLAY: return "EV_OUTPUTSVM_OVERLAY";
	case EV_SNDPLAY:           return "EV_SNDPLAY";
	default:                   return "";
	}
}

int oCMsgConversation::MD_GetTimeBehavior()
{
	switch ( subType ) {
	case EV_PLAYANISOUND:
	case EV_PLAYANI:
	case EV_PLAYSOUND:
	case EV_QUICKLOOK:
		return 1;
	case EV_LOOKAT:
	case EV_STOPLOOKAT:
	case EV_STOPPOINTAT:
	case EV_POINTAT:
		return 3;
	case EV_OUTPUT:
	case EV_OUTPUTSVM:
	case EV_CUTSCENE:
	case EV_WAITTILLEND:
	case EV_ASK:
	case EV_WAITFORQUESTION:
		return 2;
	default:
		return 0;
	}
}

float oCMsgConversation::MD_GetMinTime()
{
	switch ( subType ) {
	case EV_PLAYANI:
		auto ani = talkingWith; // probably its union?
		if ( ani )
			return ani->heh[0] / ani->flr * 1000.0 * 0.001;
		return 3.0;
	case EV_PLAYANISOUND:
	case EV_PLAYSOUND:
	case EV_SNDPLAY:
		auto sndTime = zsound->GetPlayingTimeMSEC(name);
		sndTime *= 0.001;
		if ( sndTime <= 0.0 )
			sndTime = text.Length() * 0.16666667 + 1.0;
		return sndTime;
	case EV_OUTPUT:
	case EV_OUTPUTSVM:
		return 2.0;
	case EV_LOOKAT:
	case EV_CUTSCENE:
	case EV_POINTAT:
		return 1.0;
	default:
		return 0.0;
	}
}
