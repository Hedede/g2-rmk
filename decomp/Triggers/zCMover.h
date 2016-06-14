class zTMov_Keyframe {
	zPOINT3 pos;
	zCQuat quat;
};

class zCMover : public zCTrigger {
	Z_OBEJCT(zCMover);
public:
	zCMover()
		: zCTrigger()
	{
		SetCollisionClass(&zCCollObjectComplex::s_oCollObjClass);
		SetCollDetStat(0);

		filterFlags &= 0b11111001; // TODO: prettify
		filterFlags |= 1;

		flags1 &= 0b11001111;
		flags1 ^= 0b10000;

		ClearStateInternals();
	}

	virtual ~zCMover()
	{
		Release(sfxMoving);
		Release(soundOpenEnd);
	}

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void OnTrigger(zCVob* other, zCVob* instigator) override;
	void OnUntrigger(zCVob* other, zCVob* instigator) override;
	void OnTouch(zCVob *) override;
	void OnDamage(zCVob*, zCVob* vobAttacker, float damage, int, zVEC3 const&) override;
	{
		if (flags.isEnabled && filterFlags.reactToOnDamage &&
		    damage > 0.0 && damage > damageThreshold)
			TriggerMover(vobAttacker);
	}
	void OnMessage(zCEventMessage *,zCVob *) override;
	void OnTick() override;

	void PostLoad() override;

	void CanThisCollideWith(zCVob* other) override;
	void SetVisual(zCVisual	*) override;

	bool IsKeyToThisMover(zCVob* other)
	{
		if (respondToVobName.IsEmpty())
			return 1;

		return other->GetObjectName() == respondToVobName;
	}

	void AdvanceKeyframe_KF()
	{
		float adv = advanceDir * moveSpeedUnit;
		SetToKeyframe_KF(adv * ztimer.frameTimeFloat + actKeyframeF);
	}

	void FinishedClosing()
	{
		zCVob::SetSleeping(1);
		moverState = 2;
		zsound->PlaySound3D(soundCloseEnd, this, 1, 0);
		zsound->StopSound(soundMovingHandle);
		ShowDebugInfo(this);
	}

	void DoClose();
	void DoOpen();
	void InvertMovement();

private:
	void StartMovingSound();
	void MoveToKeyFrame(int idx);
	void FinishedOpening();
	void ClearStateInternals();
	void UpdateInternals();

private:
	zCArray<zTMov_Keyframe>   keyframeList;

	// internals
	zVEC3    actKeyPosDelta;
	zREAL    actKeyframeF;
	int      actKeyframe;
	int      nextKeyframe;
	zREAL    moveSpeedUnit;
	zREAL    advanceDir;

	enum zTMoverState {
		MOVER_STATE_OPEN,
		MOVER_STATE_OPENING,
		MOVER_STATE_CLOSED,
		MOVER_STATE_CLOSING
	};

	zTMoverState moverState;

	int numTriggerEvents;
	zREAL stayOpenTimeDest;

	zCModel*       model;
	zTSoundHandle  soundMovingHandle;
	zCSoundFX*     sfxMoving = nullptr;

	zREAL  moveSpeed          = 0.3;
	zREAL  stayOpenTimeSec    = 2.0;
	zREAL  touchBlockerDamage = 0.0;

	bool moverLocked     = false;
	bool autoLinkEnabled = false;
	bool autoRotate;
	// padding 1b

	enum zTMoverAniType {
		MA_KEYFRAME,
		MA_MODEL_ANI,
		MA_WAYPOINT
	};
	zTMoverAniType moverAniType;

	enum zTMoverBehavior    {
		MB_2STATE_TOGGLE,
		MB_2STATE_TRIGGER_CONTROL,
		MB_2STATE_OPEN_TIMED,
		MB_NSTATE_LOOP,
		MB_NSTATE_SINGLE_KEYS
	};
	zTMoverBehavior moverBehavior = 0;

	enum zTTouchBehavior {
		TB_TOGGLE,
		TB_WAIT
	};
	zTTouchBehavior touchBehavior = 0;

	// aniType: Keyframe
	enum zTPosLerpType {
		PL_LINEAR,
		PL_CURVE
	};
	zTPosLerpType posLerpType = 1;

	enum zTSpeedType {
		ST_CONST,
		ST_SLOW_START_END,
		ST_SLOW_START,
		ST_SLOW_END,
		ST_SEG_SLOW_START_END,
		ST_SEG_SLOW_START,
		ST_SEG_SLOW_END,
	};
	zTSpeedType speedType = 1;

	// sound
	zSTRING  soundOpenStart;
	zSTRING  soundOpenEnd;
	zSTRING  soundMoving;
	zSTRING  soundCloseStart;
	zSTRING  soundCloseEnd;
	zSTRING  soundLock;
	zSTRING  soundUnlock;
	zSTRING  soundUseLocked;
};

void zCMover::Archive(zCArchiver& arc)
{
	zCTrigger::Archive(arc);

	arc.WriteGroupBegin("Mover");

	zSTRING enumStr = "2STATE_TOGGLE;2STATE_TRIGGER_CTRL;2STATE_OPEN_TIME;NSTATE_LOOP;NSTATE_SINGLE_KEYS";
	arc.WriteEnum("moverBehavior", enumStr, moverBehavior);
	arc.WriteFloat("touchBlockerDamage", touchBlockerDamage);
	arc.WriteFloat("stayOpenTimeSec", stayOpenTimeSec);
	arc.WriteBool("moverLocked", moverLocked);
	arc.WriteBool("autoLinkEnabled", autoLinkEnabled);
	arc.WriteBool("autoRotate", autoRotate);

	arc.WriteGroupBegin("Keyframe");
	int numKeyframes = keyframeList.GetNumInList();
	arc.WriteWord("numKeyframes", numKeyframes);

	if ( numKeyframes > 0 ) {
		arc.WriteFloat("moveSpeed", moveSpeed);
		arc.WriteEnum("posLerpType", "LINEAR;CURVE", posLerpType);
		enumStr = "CONST;SLOW_START_END;SLOW_START;SLOW_END;SEG_SLOW_START_END;SEG_SLOW_START;SEG_SLOW_END";
		arc.WriteEnum("speedType", enumStr, speedType);
		if ( !arc.InProperties() )
			arc.WriteRaw("keyframes", keyframeList.Data(), 28 * numKeyframes);
	}
	arc.WriteGroupEnd("Keyframe");

	if ( arc.InSaveGame(arc) ) {
		arc.WriteVec3("actKeyPosDelta", actKeyPosDelta);
		arc.WriteFloat("actKeyframeF", actKeyframeF);
		arc.WriteInt("actKeyframe", actKeyframe);
		arc.WriteInt("nextKeyframe", nextKeyframe);
		arc.WriteFloat("moveSpeedUnit", moveSpeedUnit);
		arc.WriteFloat("advanceDir", advanceDir);
		arc.WriteEnum("moverState", "", moverState);
		arc.WriteInt("numTriggerEvents", numTriggerEvents);
		arc.WriteFloat("stayOpenTimeDest", stayOpenTimeDest);
	}

	arc.WriteGroupBegin("Sound");
	arc.WriteString("sfxOpenStart", soundOpenStart);
	arc.WriteString("sfxOpenEnd", soundOpenEnd);
	arc.WriteString("sfxMoving", soundMoving);
	arc.WriteString("sfxCloseStart", soundCloseStart);
	arc.WriteString("sfxCloseEnd", soundCloseEnd);
	arc.WriteString("sfxLock", soundLock);
	arc.WriteString("sfxUnlock", soundUnlock);
	arc.WriteString("sfxUseLocked", soundUseLocked);
	arc.WriteGroupEnd("Sound");

	arc.WriteGroupEnd("Mover");
}

void zCMover::Unarchive(zCArchiver& arc)
{
	zCMover::Unarchive(arc);

	actKeyPosDelta = {};
	actKeyframe = 0;
	actKeyframeF = 0;
	nextKeyframe = 0;
	moveSpeedUnit = 0;
	advanceDir = 0;
	moverState = 2;
	numTriggerEvents = 0;
	stayOpenTimeDest = 0;
	model = 0;
	autoRotate = 0;

	if ( sfxMoving ) {
		zsound->StopSound(soundMovingHandle);
		Release(sfxMoving);
	}

	moverBehavior = arc.ReadEnum("moverBehavior");

	arc.ReadFloat("touchBlockerDamage", touchBlockerDamage);
	arc.ReadFloat("stayOpenTimeSec", stayOpenTimeSec);

	moverLocked = arc.ReadBool("moverLocked");
	autoLinkEnabled = arc.ReadBool("autoLinkEnabled");
	autoRotate = arc.ReadBool("autoRotate");

	int numKeyframes = arc.ReadWord("numKeyframes");

	if ( numKeyframes > 0 ) {
		arc.ReadFloat("moveSpeed", moveSpeed);
		posLerpType = arc.ReadEnum("posLerpType");
		speedType = arc.ReadEnum("speedType");
		if ( !arc.InProperties() ) {
			keyframeList.Resize(numKeyframes);

			arc.ReadRaw("keyframes", keyframeList.Data(), 28 * numKeyframes);
		}
	}

	if ( arc.InSaveGame(arc) ) {
		arc.ReadVec3("actKeyPosDelta", actKeyPosDelta);
		arc.ReadFloat("actKeyframeF", actKeyframeF);
		arc.ReadInt("actKeyframe", actKeyframe);
		arc.ReadInt("nextKeyframe", nextKeyframe);
		arc.ReadFloat("moveSpeedUnit", moveSpeedUnit);
		arc.ReadFloat("advanceDir", advanceDir);
		moverState = arc.ReadEnum("moverState");
		arc.ReadInt("numTriggerEvents", numTriggerEvents);
		arc.ReadFloat("stayOpenTimeDest", stayOpenTimeDest);
	}

	arc.ReadString("sfxOpenStart", soundOpenStart);
	arc.ReadString("sfxOpenEnd", soundOpenEnd);
	arc.ReadString("sfxMoving", soundMoving);
	arc.ReadString("sfxCloseStart", soundCloseStart);
	arc.ReadString("sfxCloseEnd", soundCloseEnd);
	arc.ReadString("sfxLock", soundLock);
	arc.ReadString("sfxUnlock", soundUnlock);
	arc.ReadString("sfxUseLocked", soundUseLocked);

	Release(sfxMoving);

	if ( soundMoving.Length() > 0 )
		sfxMoving = zsound->LoadSoundFX(soundMoving);

	if ( visual )
		model = dynamic_cast<zCMovel>(visual);

	if ( !arc.InSaveGame() )
		SetToKeyframe(0.0, 0.0);
}

void zCMover::MoveToKeyframe(int idx)
{
	if ( advanceDir == 0.0 && moverBehavior == 4 ) {
		SetSleeping(0);

		moverState = 1;

		CLAMP(idx, 0, keyframeList.GetNumInList());

		nextKeyframe = idx;

		SetToKeyframe(actKeyframeF, 1.0);
	}
}

void zCMover::FinishedOpening()
{
	SetSleeping(1);

	moverState = 0;

	zsound->StopSound(&soundMovingHandle);
	zsound->PlaySound3D(&soundOpenEnd, this, 1, 0);

	if ( moverBehavior == 2 ) {
		stayOpenTimeDest = stayOpenTimeSec * 1000.0 + ztimer.totalTimeFloat;
		SetSleeping(0);
	}

	Init();
}

void zCMover::UpdateInternals()
{
	Release(sfxMoving);

	if ( soundMoving.Length() > 0 )
		sfxMoving = zsound->LoadSoundFX(soundMoving);

	model = dynamic_cast<zCModel*>(visual);
}

int zCMover::CanThisCollideWith(zCVob* other)
{
	if (auto trig = dynamic_cast<zCTriggerBase*>(other))
		return 0;
	if (other->flags1.staticVob)
		return 0;
	if (!other->HasParentVob())
		return 0;

	return !autoLinkEnabled || other->CanAutoLinkWith(this);
}

void zCMover::StartMovingSound()
{
	zTSound3DParams params;

	params.unk0 = 0;
	params.unk1 = 1.0;
	params.unk2 = -1.0;
	params.unk4 = 0;
	params.unk5 = 1.0;
	params.unk6 = 0;
	params.unk7 = -999999.0;
	params.unk3 = 1;
	soundMovingHandle = zsound->PlaySound3D(sfxMoving, this, 2, &params);
}

void zCMover::DoOpen()
{
	zCVob::SetSleeping(0);

	moverState = 1;

	SetToKeyframe(0, 1.0);

	zsound->PlaySound3D(soundOpenStart, this, 1, 0);

	StartMovingSound();
}

void zCMover::DoClose()
{
	SetSleeping(0);

	moverState = 3;

	SetToKeyframe(keyframeList.Size(), -1.0);

	zsound->PlaySound3D(soundCloseStart, this, 1, 0);

	StartMovingSound();
}

void zCMover::InvertMovement()
{
	if ( moverBehavior <= 2 ) {
		if ( moverState == 1 ) {
			moverState = 3;
		} else if ( moverState == 3 ) {
			moverState = 1;
		}
	}

	SetToKeyframe(actKeyframeF, -advanceDir);

	if ( moverAniType == 1 ) {
		for (unsigned i = 0; i < model->active_ani_num; ++i) {
			auto active = model->active_anis[i];
			active->SetDirection(active->someflag != -1);
		}
	}
}

void zCMover::OnUntrigger(zCVob* other, zCVob* instigator)
{
	if (trg_flags & 2) {
		zINFO(8,"D: zMOV: Untrigger Mover " + GetObjectName()); // _dieter/zVobMisc.cpp, 1285
		if ( moverBehavior == 1 && --numTriggerEvents <= 0 ) {
			if ( moverState == 0 ) {
				DoClose();
			} else if ( moverState == 1) {
				InvertMovement();
			}
		}
	}
}

void zCMover::OnTouch(zCVob *other)
{
	if ( autoLinkEnabled )
		other->CheckAutoLink(this);

	if ( trg_flags & 2 && other ) {
		if ( flags2.sleepingMode && advanceDir != 0.0 ) {
			if ( touchBlockerDamage > 0.0 ) {
				zVEC3 pos;
				other->GetEM()->OnDamage(this, this, touchBlockerDamage, 1, pos);
			}
		} else if ( trg_filterFlags & 2 ) {
			TriggerMover(other);
		}
	}
}

void zCMover::ClearStateInternals()
{
	actKeyPosDelta = zVEC3{0,0,0};
	actKeyframeF = 0;
	actKeyframe = 0;
	nextKeyframe = 0;
	moveSpeedUnit = 0;
	advanceDir = 0;
	moverState = 2;
	numTriggerEvents = 0;
	stayOpenTimeDest = 0;
	model = 0;
	autoRotate = 0;

	if ( sfxMoving ) {
		zsound->StopSound(soundMovingHandle);
		Release(sfxMoving);
	}
}
