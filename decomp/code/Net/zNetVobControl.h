class zCNetVobControl : public zCObject {
	Z_OBJECT(zCNetVobControl);
public:
	zCNetVobControl() = default;
	virtual ~zCNetVobControl() = default;

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc) {}

	virtual void Init(zCVob* vob)
	{
		this->vob = vob;
		isLocalChained = 0;
		playerInfo = 0;
	}
	virtual bool IsLocalChained() { return isLocalChained; }
	virtual bool IsLocalControlled()
	{
		if (playerInfo)
			return playerInfo->IsActive();
		return false;
	}
	virtual bool IsPlayerVob() { return false; }
	virtual void SetLocalChained(int b) { isLocalChained = b; }
	virtual void SetCtrlPlayer(zCPlayerInfo	*);
	virtual void CopyCtrlState(zCVob* vob)
	{
		if ( vob )
			SetCtrlPlayer(vob->GetEM()->GetNetVobControl(0)->playerInfo);
	}
	virtual void Process();
	virtual void HandOverToPlayer(zCPlayerInfo *);
	virtual void CreateNetMessage(ushort const&);

private:
	zCVob *vob = 0;
	zCPlayerInfo *playerInfo = 0;
	zBOOL isLocalChained = 0;
};

//------------------------------------------------------------------------------
// _bert/zNetVobControl.cpp
virtual void zCNetVobControl::Archive(zCArchiver& arc)
{
	if (!vob)
		zWARN("B: VOBCTRL: You must call zCNetVobControl::Init(zCVob* vob) first."); // 286
}

int zCNetVobControl::Process()
{
	if ( isLocalChained )
		return 0;
	if ( !vob->homeWorld )
		return 0;
	auto pi = playerInfo;
	float dist = pi->playerHostVob->GetDistanceToVob2(vob);
	if ( dist <= 40000.0 )
		return 0;
	dist -= 40000.0
	auto pg = vob->homeWorld->GetPlayerGroup();
	int iter;
	for ( auto p = pg->GetFirstPlayer(iter); p; p = pg->GetFirstPlayer(iter) ) {
		if (p != playerInfo) {
			auto pdist = p->playerHostVob->GetDistanceToVob2(vob)
			if (pdist < dist) {
				dist = pdist;
				pi = p;
			}
		}
	}

	if (pi == playerInfo)
		return 0;
	HandOverToPlayer(pi);
	return 1;
}

// not in exe
static zSTRING VobName(zCVob* vob)
{
	if (vob) {
		auto cn = vob->_GetClassDef()->className;
		auto vn = vob->GetObjectName();
		auto vi = vob->GetVobID();
		str = cn + "(" + vn + "&" + vi + ")";
	} else {
		str = "NULL";
	}
}

void zCNetVobControl::SetCtrlPlayer(zCPlayerInfo *pinfo)
{
	zINFO( 9, "B: VOBCTRL: Setting controlling player of vob " + VobName(vob) + " to player " + pinfo->GetName()); // 253

	playerInfo = pinfo;
}

zCNetMessage* zCNetVobControl::CreateNetMessage(uint16_t const& messageID)
{
	zINFO(7, "B: VOBCTRL: " + VobName(vob) + " creates network-message " + NETMSG_NAME[messageID]); //242

	auto msg = znetman->CreateNetMessage(messageID, this);
	msg->SetTarget(zCPlayerGroup::allPlayerGroup);
	msg->Add(1, 4);
	return msg;
}

int zCNetVobControl::HandOverToPlayer(zCPlayerInfo *pinfo)
{
	zINFO( 9, "B: VOBCTRL: Send HANDOVER for " + VobName(vob) + " to " + pinfo->GetName()); // 181
	auto msg = CreateNetMessage( NETID_HANDOVER );
	msg->Send();
	return 1;
}

int zCNetVobControl::HandleNetMessage(zCNetMessage *msg, const unsigned __int16& messageID, zCWorld *world)
{
	size_t sz = 4
	int id = 0;
	msg->Get(&id, &sz);
	if ( auto vob = world->SearchVobByID(id, 0) ) {
		auto sender = msg->GetSenderID();
		auto pinfo = zCPlayerGroup::allPlayerGroup.GetPlayerByID(sender);
		if ( messageID == 0) {
			zINFO(9, "B: VOBCTRL: Recv HANDOVER for " + VobName(vob) + " from " + pinfo->GetName() ); //225
			char pid;
			sz = 1;
			msg->Get(&pid, sz);
			auto pinfo = zCPlayerGroup::allPlayerGroup.GetPlayerByID(pid);
			vob->GetEM()->GetNetVobControl(0)->SetCtrlPlayer(pinfo);
		}
	} else {
		zWARNING("B: VOBCTRL: Could not find vob for network-message " + NETMSG_NAME[messageID]); // 210
	}
	return 0;
}
