class zTMov_Keyframe {
	zPOINT3 pos;
	zCQuat quat;
};

class zCMover : public zCTrigger {
	Z_OBEJCT(zCMover);
public:
	virtual ~zCMover();
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void OnTrigger(zCVob *,zCVob *) override;
	void OnUntrigger(zCVob *,zCVob *) override;
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

	void CanThisCollideWith(zCVob *) override;
	void SetVisual(zCVisual	*) override;

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

private:
	struct zTMov_Keyframe {
		zPOINT3     pos;
		zCQuat      quat;
	};

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
	zCSoundFX*     sfxMoving;

	zREAL  moveSpeed;
	zREAL  stayOpenTimeSec;
	zREAL  touchBlockerDamage;

	bool moverLocked;
	bool autoLinkEnabled;
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
	zTMoverBehavior moverBehavior;

	enum zTTouchBehavior {
		TB_TOGGLE,
		TB_WAIT
	};
	zTTouchBehavior touchBehavior;

	// aniType: Keyframe
	enum zTPosLerpType {
		PL_LINEAR,
		PL_CURVE
	};
	zTPosLerpType posLerpType;

	enum zTSpeedType {
		ST_CONST,
		ST_SLOW_START_END,
		ST_SLOW_START,
		ST_SLOW_END,
		ST_SEG_SLOW_START_END,
		ST_SEG_SLOW_START,
		ST_SEG_SLOW_END,
	};
	zTSpeedType speedType;

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
