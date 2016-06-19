class zCTrigger : public zCTriggerBase {
	Z_OBJECT(zCTrigger);
public:
	virtual ~zCTrigger();

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void OnTrigger(zCVob* otherVob, zCVob* vobInstigator) override
	{
		if ( filterFlags.reactToOnTrigger )
			ActivateTrigger(vobInstigator);
	}

	void OnUntrigger(zCVob* otherVob, zCVob* vobInstigator) override
	{
		if ( filterFlags.reactToOnTrigger ) {
			if (!IsOnTimer()) {
				if (flags.isEnabled && flags.sendUntrigger)
					ShowDebugInfo(vobInstigator);
			}
		}
	}

	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void OnTimer();
	virtual void TriggerTarget(zCVob *);
	virtual void UntriggerTarget(zCVob *);
	virtual void CanBeActivatedNow(zCVob *);

private:
	void ClearStateInternals()
	{
		flags.isEnabled = 0; //? flags = flags ^ (flags ^ 2 * flags) & 2

		nextTimeTriggerable = 0;

		Release(savedOtherVob);
	}

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

void zCTrigger::Archive(zCArchiver& arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteGroupBegin("Trigger");

	arc.WriteGroupBegin("ActivationFilter");

	if ( !arc.InProperties() ) {
		arc.WriteRaw("flags", flags, 1);
		arc.WriteRaw("filterFlags", filterFlags, 1);
	} else {
		arc.WriteBool("reactToOnTrigger", filterFlags.reactToOnTrigger);
		arc.WriteBool("reactToOnTouch", filterFlags.reactToOnTouch);
		arc.WriteBool("reactToOnDamage", filterFlags.reactToOnDamage);
		arc.WriteBool("respondToObject", filterFlags.respondToObject);
		arc.WriteBool("respondToPC", filterFlags.respondToPC);
		arc.WriteBool("respondToNPC", filterFlags.respondToNPC);
		arc.WriteBool("startEnabled", flags.startEnabled);
	}

	arc.WriteString("respondToVobName", respondToVobName);
	arc.WriteInt("numCanBeActivated", numCanBeActivated);
	arc.WriteFloat("retriggerWaitSec", retriggerWaitSec);
	arc.WriteFloat("damageThreshold", damageThreshold);

	arc.WriteGroupEnd("ActivationFilter");


	arc.WriteGroupBegin("FireBehavior");

	arc.WriteFloat("fireDelaySec", fireDelaySec);

	if ( arc.InProperties() )
		arc.WriteBool("sendUntrigger", flags.sendUntrigger);

	arc.WriteGroupEnd("FireBehavior");

	if ( arc.InSaveGame() ) {
		float nextTime = nextTimeTriggerable - _unk_starttime;
		if ( nextTime < 0.0 )
			nextTime = 0.0;
		arc.WriteFloat("nextTimeTriggerable", nextTime);
		arc.WriteObject("savedOtherVob", savedOtherVob);
		arc.WriteInt("countCanBeActivated", countCanBeActivated);
		arc.WriteBool("isEnabled", flags.isEnabled);
	}

	arc.WriteGroupEnd("Trigger");
}

void zCTrigger::Unarchive(zCArchiver& arc)
{
	Release(savedOtherVob);
	nextTimeTriggerable = 0;

	zCTriggerBase::Unarchive(arc);

	if ( !arc.InProperties(v7) ) {
		arc.ReadRaw("flags", flags, 1);
		arc.ReadRaw("filterFlags", filterFlags, 1);
	} else {
		filterFlags.reactToOnTrigger = arc.ReadBool("reactToOnTrigger");
		filterFlags.reactToOnTouch = arc.ReadBool("reactToOnTouch");
		filterFlags.reactToOnDamage = arc.ReadBool("reactToOnDamage");
		filterFlags.respondToObject = arc.ReadBool("respondToObject");
		filterFlags.respondToPC = arc.ReadBool("respondToPC");
		filterFlags.respondToNPC = arc.ReadBool("respondToNPC");
		flags.startEnabled = arc.ReadBool("startEnabled");
	}

	arc.ReadString("respondToVobName", respondToVobName);
	countCanBeActivated = arc.ReadInt("numCanBeActivated");
	numCanBeActivated = arc.ReadInt("numCanBeActivated");
	arc.ReadFloat("retriggerWaitSec", retriggerWaitSec);
	arc.ReadFloat("damageThreshold", damageThreshold);
	arc.ReadFloat("fireDelaySec", fireDelaySec);

	if ( arc.InProperties() )
		flags.sendUntrigger = arc.ReadBool("sendUntrigger");

	if ( arc.InSaveGame() ) {
		arc.ReadFloat("nextTimeTriggerable", nextTimeTriggerable);
		arc.ReadObject("savedOtherVob", savedOtherVob);
		countCanBeActivated = arc.ReadInt("countCanBeActivated");
		float nextTime = nextTimeTriggerable;
		if ( nextTime > 0.0 )
			nextTimeTriggerable = _unk_starttime + nextTimeTriggerable;

		flags.isEnabled = arc.ReadBool("isEnabled");
	}

	// for some reason in original it looks like:
#if 0
	if (...) {
		...
		_bool = arc.ReadBool("isEnabled");
	} else {
		flags.isEnabled = _bool;
	}
#endif
}
