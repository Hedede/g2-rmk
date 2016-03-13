class zCNetVobControl : public zCObject {
	Z_OBJECT(zCNetVobControl);
public:
	virtual ~zCNetVobControl();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc) {}

	virtual void Init(zCVob	*);
	virtual void IsLocalChained();
	virtual void IsLocalControlled();
	virtual void IsPlayerVob();
	virtual void SetLocalChained(int);
	virtual void SetCtrlPlayer(zCPlayerInfo	*);
	virtual void CopyCtrlState(zCVob *);
	virtual void Process();
	virtual void HandOverToPlayer(zCPlayerInfo *);
	virtual void CreateNetMessage(ushort const	&);
};

virtual void zCNetVobControl::Archive(zCArchiver& arc)
{
	if (!initialized)
	// 286
		zWARN("B: VOBCTRL: You must call zCNetVobControl::Init(zCVob* vob) first.");
}
