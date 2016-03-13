class zCTrigger : public zCTriggerBase {
	Z_OBJECT(zCTrigger);
public:
	virtual ~zCTrigger();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void OnTimer();
	virtual void TriggerTarget(zCVob *);
	virtual void UntriggerTarget(zCVob *);
	virtual void CanBeActivatedNow(zCVob *);
private:
	//Eigenschaften sollten weitgehend klar sein

	struct {
		zUINT8          reactToOnTrigger: 1;
		zUINT8          reactToOnTouch  : 1;
		zUINT8          reactToOnDamage : 1;
		zUINT8          respondToObject : 1;
		zUINT8          respondToPC     : 1;
		zUINT8          respondToNPC    : 1;
	} filterFlags;
	struct {
		zUINT8          startEnabled    : 1;
		zUINT8          isEnabled       : 1;
		zUINT8          sendUntrigger   : 1;
	} flags;

	zSTRING respondToVobName;
	zSWORD  numCanBeActivated;
	zREAL  retriggerWaitSec;
	zREAL  damageThreshold;
	zREAL  fireDelaySec;
	//vgl. Eigenschaft retriggerWaitSec
	zREAL  nextTimeTriggerable;
	zCVob* savedOtherVob;
	zSWORD countCanBeActivated;
};
