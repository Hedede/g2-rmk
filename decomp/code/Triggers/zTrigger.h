class zCTrigger : public zCTriggerBase {
	Z_OBJECT(zCTrigger);
public:
	zCTrigger()
		: zCTriggerBase()
	{
		ClearStateInternals();
	}

	virtual ~zCTrigger()
	{
		Release(savedOtherVob);
	}

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

	void OnTouch(zCVob* vob) override
	{
		if (filterFlags.reactToOnTouch)
			ActivateTrigger(vob);
	}

	void OnUntouch(zCVob* vob) override
	{
		if (filterFlags.reactToOnTouch)
			DeactivateTrigger(vob);
	}

	void OnDamage(zCVob*, zCVob* inflictor, float damage, int,zVEC3 const &) override
	{
		if (filterFlags.reactToOnDamage && damage > 0.0 && damage > damageThreshold)
			ActivateTrigger(inflictor);
	}

	void OnMessage(zCEventMessage* msg, zCVob* source) override
	{
		if (auto common = dynamic_cast<zCEventCommon*>(msg)) {
			if (common->subType == 0)
				trg_flags.isEnabled = true;
			else if (common->subType == 1)
				trg_flags.isEnabled = false;
			else if (common->subType == 2)
				trg_flags.isEnabled = !trg_flags.isEnabled;
		}
	}

	void OnTimer() override
	{
		SetSleeping(1);
		if ( fireDelaySec > 0.0 ) {
			TriggerTarget(savedOtherVob);
			TriggerTargetPost();
			Release(savedOtherVob);
		}
	}

	virtual void TriggerTarget(zCVob* target)
	{
		if (triggerTarget)
			zCTriggerBase::OnTrigger(this, target);
	}
	virtual void UntriggerTarget(zCVob* target)
	{
		if (triggerTarget)
			zCTriggerBase::OnUnrigger(this, target);
	}

	virtual bool CanBeActivatedNow(zCVob* vob);

	zSTRING GetTriggerTarget() const
	{
		return triggerTarget;
	}

private:
	void ClearStateInternals()
	{
		trg_flags.isEnabled = trg_flags.startEnabled;

		nextTimeTriggerable = 0;

		Release(savedOtherVob);
	}

	void zCTrigger::ActivateTrigger(zCVob *vob)
	{
		if ( !IsOnTimer() && CanBeActivatedNow(vob) && trg_flags.isEnabled ) {
			if ( fireDelaySec <= 0.0 ) {
				TriggerTarget(vob);
				TriggerTargetPost();
			} else {
				Release(savedOtherVob);
				savedOtherVob = vob;
				AddRef(vob);
				SetOnTimer(fireDelaySec * 1000.0);
			}
		}
	}

	void DeactivateTrigger(zCVob *vob)
	{
		if ( !IsOnTimer() ) {
			if (trg_flags.isEnabled && trg_flags.sendUntrigger)
				UntriggerTarget(vob);
		}
	}

	void TriggerTargetPost()
	{
		if ( countCanBeActivated > 0 )
			--countCanBeActivated;
		nextTimeTriggerable = retriggerWaitSec * 1000.0 + ztimer.totalTimeFloat;
	}

private:
	//Eigenschaften sollten weitgehend klar sein

	struct {
		zUINT8          reactToOnTrigger: 1; // 1
		zUINT8          reactToOnTouch  : 1; // 2
		zUINT8          reactToOnDamage : 1;
		zUINT8          respondToObject : 1;
		zUINT8          respondToPC     : 1;
		zUINT8          respondToNPC    : 1;
	} filterFlags;
	struct {
		zUINT8          startEnabled    : 1; // 1
		zUINT8          isEnabled       : 1; // 2
		zUINT8          sendUntrigger   : 1; // 4
	} trg_flags;

	zSTRING respondToVobName;
	zSWORD  numCanBeActivated;
	zREAL  retriggerWaitSec = 0.0;
	zREAL  damageThreshold  = 0.0;
	zREAL  fireDelaySec = 0;
	//vgl. Eigenschaft retriggerWaitSec
	zREAL  nextTimeTriggerable = 0.0;
	zCVob* savedOtherVob = nullptr;
	zSWORD countCanBeActivated = -1;
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

bool zCTrigger::CanBeActivatedNow(zCVob *vob)
{
	if (!trg_flags.isEnabled)
		return false;
	if (!countCanBeActivated)
		return false;
	if (IsOnTimer())
		return false;
	if ( ztimer.totalTimeFloat < nextTimeTriggerable )
		return false;

	if ( !vob )
		return true;

	if (filterFlags.respondToObject && vob->vtbl->GetCharacterClass() == 0 ||
	    filterFlags.respondToPC     && vob->vtbl->GetCharacterClass() == 1 ||
	    filterFlags.respondToNPC    && vob->vtbl->GetCharacterClass() == 2 )
		return true;
	if (!respondToVobName)
		return false;
	return respondToVobName == vob->GetObjectName();
}
