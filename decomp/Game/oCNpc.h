class oCNpc_States {
	virtual void Archive(zCArchiver&);
	virtual void Unarchive(zCArchiver&);
	virtual void PackState(zCBuffer&);
	virtual void UnpackState(zCBuffer&);
	int     state_vtbl;
	zSTRING state_name;
	oCNpc*  state_npc;
	class TNpcAIState {
		int      index;
		int      loop;
		int      end;
		int      timeBehaviour;
		zREAL    restTime;
		int      phase;
		zBOOL    valid;
		zSTRING name;
		zREAL    stateTime;
		int      prgIndex;
		zBOOL    isRtnState;
	};
	TNpcAIState curState;
	TNpcAIState nextState;
	int             lastAIState;
	zBOOL           hasRoutine;
	zBOOL           rtnChanged;
	oCRtnEntry*     rtnBefore;
	oCRtnEntry*     rtnNow;
	zCRoute*        rtnRoute;
	zBOOL           rtnOverlay;
	int             rtnOverlayCount;
	int             walkmode_routine;
	zBOOL           weaponmode_routine;
	zBOOL           startNewRoutine;
	int             aiStateDriven;
	zVEC3           aiStatePosition;
	oCNpc*          parOther;
	oCNpc*          parVictim;
	oCItem*         parItem;
	int             rntChangeCount;
};

const int oCMagFrontier_bitfield_isWarning  = ((1 << 1) - 1) << 0;
const int oCMagFrontier_bitfield_isShooting = ((1 << 1) - 1) << 1;



const int oCNpc_oTRobustTrace_bitfield_stand                = ((1 << 1) - 1) << 0;
const int oCNpc_oTRobustTrace_bitfield_dirChoosed           = ((1 << 1) - 1) << 1;
const int oCNpc_oTRobustTrace_bitfield_exactPosition        = ((1 << 1) - 1) << 2;
const int oCNpc_oTRobustTrace_bitfield_targetReached        = ((1 << 1) - 1) << 3;
const int oCNpc_oTRobustTrace_bitfield_standIfTargetReached = ((1 << 1) - 1) << 4;
const int oCNpc_oTRobustTrace_bitfield_waiting              = ((1 << 1) - 1) << 5;
const int oCNpc_oTRobustTrace_bitfield_isObstVobSmall       = ((1 << 1) - 1) << 6;
const int oCNpc_oTRobustTrace_bitfield_targetVisible        = ((1 << 1) - 1) << 7;
const int oCNpc_oTRobustTrace_bitfield_useChasmChecks       = ((1 << 1) - 1) << 8;

enum oEFightMode
{
	FMODE_NONE = 0x0,
	FMODE_FIST = 0x1,
	FMODE_MELEE = 0x2,
	FMODE_FAR = 0x5,
	FMODE_MAGIC = 0x7,
};

enum ATR_INDEX {
	ATR_HITPOINTS = 0,
	ATR_HITPOINTS_MAX = 1,
	ATR_MANA = 2,
	ATR_MANA_MAX = 3,
	ATR_STRENGTH = 4,
	ATR_DEXTERITY = 5,
	ATR_REGENERATEHP = 6,
	ATR_REGENERATEMANA = 7,
	ATR_INDEX_MAX = 8,
};

class oCNpc : public oCVob {
	CLASSDEF_DEFINE;
public:
	static bool IsSlotFree(TNpcSlot* slot)
	{
		return slot && slot->object == nullptr;
	}

	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual ~oCNpc();
	virtual void OnTouch(zCVob *);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual int GetCharacterClass()
	{
		return IsAPlayer() ? 1 : 2;
	}
	virtual void SetVisual(zCVisual *);
	virtual void GetScriptInstance(zSTRING * &,int &);
	virtual void SetByScriptInstance(zSTRING const *,int);
	virtual void GetCSStateFlags();
	virtual void ThisVobAddedToWorld(zCWorld *);
	virtual void ThisVobRemovedFromWorld(zCWorld *);
	virtual void ShowDebugInfo(zCCamera *);
	virtual void GetInstance();
	virtual void GetInstanceName();
	virtual void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium &,oTSndMaterial &,int);

	virtual void SetWeaponMode(int);
	virtual void SetWeaponMode2(int);
	void SetWeaponMode2(zSTRING const& str);

	virtual void DoDie(oCNpc *);
	virtual void DoInsertMunition(zSTRING const &);
	virtual void DoRemoveMunition();
	virtual void DoSetToFightMode(oCItem *);
	virtual void DoShootArrow(int);
	virtual void OnDamage(oCNpc::oSDamageDescriptor &);
	virtual void ResetPos(zVEC3 &);
	virtual void EmergencyResetPos(zVEC3 &);
	virtual void InitByScript(int,int);
	virtual void Disable();
	virtual void Enable(zVEC3 &);
	virtual void InitHumanAI();
	virtual void IsMoreImportant(zCVob *,zCVob *);
	virtual void DoDoAniEvents();
	virtual void DoModelSwapMesh(zSTRING const &,zSTRING const &);
	virtual void DoTakeVob(zCVob *);
	virtual void DoDropVob(zCVob *);
	virtual void DoRemoveFromInventory(oCItem *);
	virtual void DoPutInInventory(oCItem *);
	virtual void DoThrowVob(zCVob *,float);
	virtual void DoExchangeTorch();
	virtual void GetSoundMaterial_MA(zCSoundManager::zTSndManMedium &,oTSndMaterial &,zSTRING);
	virtual void GetSoundMaterial_MH_HM(zCSoundManager::zTSndManMedium &,oTSndMaterial &);
	virtual void IsAPlayer();
	virtual bool IsSelfPlayer();
	virtual void SetAsPlayer();
	virtual void IsMonster();
	virtual void IsHuman() const
	{
		return guildTrue <= 16;
	}
	virtual void IsGoblin();
	virtual void IsOrc();
	virtual void IsSkeleton();
	virtual void GetPlayerNumber();
	virtual void IsAniMessageRunning();
	virtual void ProcessNpc();
	virtual void AllowDiscardingOfSubtree();

	// or DOCREATE?
	zCEventManager* GetEM(bool dontCreate = false);

	int GetAttribute(int atr) const
	{
		return attribute[atr];
	}

	int GetHitChance(size_t id) const
	{
		return hitChance[id];
	}

	int GetDamageByIndex(int idx) const
	{
		return damage[idx];
	}

	int GetProtectionByIndex(int idx) const
	{
		return protection[idx];
	}

	zSTRING GetName() const
	{
		return name[0];
	}

	int GetGuild() const
	{
		return guild;
	}

	zSTRING oCNpc::GetGuildName() const
	{
		return oCGuilds::GetGuildName(this->guild);
	}

	void SetGuild(int guild)
	{
		this->guild = guild;
	}

	int GetTrueGuild() const
	{
		return guildTrue;
	}

	void SetTrueGuild(int guild)
	{
		this->guildTrue = guild;
	}

	int GetGuildAttitude(int guild) const;
	bool IsGuildFriendly(int guild) const
	{
		return GetGuildAttitude(guild) == ATT_FRIENDLY;
	}

	int oCNpc::AssessFakeGuild_S()
	{
		if ( IsSelfPlayer() ) {
			CreatePassivePerception(21, this, 0);
			return 1;
		}

		return 0;
	}

	double GetTurnSpeed()
	{
		if (IsAPlayer())
			return 0.1;
		return speedTurn;
	}

	int GetPlayerNumber()
	{
		return 0;
	}

	oCItem* GetItem(int category, int slot)
	{
		return inventory2.GetItem(slot);
	}

	// TODO: return type?
	oCItem* IsInInv(int inst, int amount)
	{
		return inventory2.IsIn(inst, amount);
	}

	oCItem* IsInInv(zSTRING const& name, int amount)
	{
		return inventory2.IsIn(name, amount);
	}

	oCItem* DetectItem(int flags, int);

	void Follow()
	{
	}

	int GetFightRange() const;

	void SetFightRangeBase(int range)
	{
		fightRangeBase = range;
	}

	void SetFightRangeFist(int range)
	{
		fightRangeFist = range;
	}

	void SetFightRangeG(int range)
	{
		fightRangeG = range;
	}

	struct TActiveInfo {
		TActiveInfo(oCNpc const*) = default;
		~TActiveInfo() = default;

		unsigned nr : 4;
	};
private:
	int    idx;
	zSTRING name[5];
	zSTRING slot;
	zSTRING effect;
	int    npcType;
	int    variousFlags;
	int    attribute[NPC_ATR_MAX];
	int    hitChance[NPC_HITCHANCE_MAX];
	int    protection[oEDamageIndex_MAX];
	int    damage[oEDamageIndex_MAX];
	int    damagetype;
	int    guild;
	int    level;
	void*   mission[NPC_MIS_MAX];
	int    fighttactic;
	int    fmode;
	int    voice;
	int    voicePitch;
	int    mass;
	void*   daily_routine;
	void*   startAIState;
	zSTRING spawnPoint;
	int    spawnDelay;
	int    senses;
	int    senses_range;
	int    aiscriptvars[100];
	zSTRING wpname;
	zUINT32    experience_points;
	zUINT32    experience_points_next_level;
	zUINT32    learn_points;
	int     bodyStateInterruptableOverride;
	zBOOL   noFocus;
	int     parserEnd;
	int     bloodEnabled;
	int     bloodDistance;
	int     bloodAmount;
	int     bloodFlow;
	zSTRING bloodEmitter;
	zSTRING bloodTexture;
	zBOOL   didHit;
	zBOOL   didParade;
	zBOOL   didShoot;
	zBOOL   hasLockedEnemy;
	zBOOL   isDefending;
	zBOOL   wasAiming;
	oCNpc::TFAction     lastAction;
	oCNpc*              enemy;
	zREAL               speedTurn; // ok
	zBOOL               foundFleePoint;
	zBOOL               reachedFleePoint;
	zVEC3               vecFlee;
	zVEC3               posFlee;
	zCWaypoint*         waypointFlee;
	class oTRobustTrace {
		int    bitfield;    // 0x04C4 oCNpc_oTRobustTrace_bitfield_Xxx
		zVEC3    targetPos;
		zCVob*   targetVob;
		zCVob*   obstVob;
		zREAL     targetDist;
		zREAL     lastTargetDist;
		zREAL     maxTargetDist;
		zREAL     dirTurn;
		zREAL     timer;
		zVEC3     dirFirst;
		zREAL     dirLastAngle;
		zCArray<oTDirectionInfo*> lastDirections;
		int    frameCtr;
		zVEC3  targetPosArray[5];
		int    targetPosCounter;
		int    targetPosIndex;
		int    checkVisibilityTime;
		int    positionUpdateTime;
		int    failurePossibility;
	} rbt;

	zCList<oCNpcTimedOverlay>   timedOverlays_data;
	zCArray<oCNpcTalent*>    talents;
	int    spellMana;

	class oCMagFrontier {
		SetNPC(oCNpc *npc)
		{
			this->npc = npc;
		}
		oCVisualFX*     warningFX;
		oCVisualFX*     shootFX;
		oCNpc*          npc;
		int    bitfield;                                      // 0x0584 oCMagFrontier_bitfield_Xxx
	} magFrontier;

	oCNpc_States states; // ok

	oCNpcInventory inventory;
	oCItemContainer*    trader;
	oCNpc*    tradeNpc;

	zREAL    rangeToPlayer;
	zCArray<zTSoundHandle>  listOfVoiceHandles;
	int    voiceIndex;
	zCArray<oCVisualFX*> effectList;

	uint16_t showaidebug      : 1;
	uint16_t showNews         : 1;
	uint16_t csAllowedAsRole  : 1;
	uint16_t isSummoned       : 1;
	uint16_t respawnOn        : 1;
	uint16_t movlock          : 1;
	uint16_t drunk            : 1;
	uint16_t mad              : 1;
	uint16_t overlay_wounded  : 1;
	uint16_t inOnDamage       : 1;
	uint16_t autoremoveweapon : 1;
	uint16_t openinventory    : 1;
	uint16_t askroutine       : 1;
	uint16_t spawnInRange     : 1;

	uint16_t body_TexVarNr;
	uint16_t body_TexColorNr;
	uint16_t head_TexVarNr;
	uint16_t teeth_TexVarNr;

	uint8_t guildTrue;
	uint8_t drunk_heal;
	uint8_t mad_heal;
	uint8_t spells;

	uint32_t bodyState         : 19;
	uint32_t aniMessageRunning : 1;

	int    instanz;
	zSTRING mds_name;
	zSTRING body_visualName;
	zSTRING head_visualName;
	VEC3       model_scale;
	zREAL      model_fatness;
	int        namenr;
	zREAL      hpHeal;
	zREAL      manaHeal;
	zREAL      swimtime;
	zREAL      divetime;
	zREAL      divectr;
	zCVob*     fireVob;
	int        fireDamage;
	zREAL      fireDamageTimer;
	int        attitude; //nok
	int        tmp_attitude;
	zREAL      attTimer;
	int        knowsPlayer;

	struct TNpcPerc {
		int percID;
		int percFunc;
	}  percList[NPC_PERC_MAX];

	int    percActive;
	int    percActiveTime;
	zREAL           percActiveDelta;
	zBOOL           overrideFallDownHeight;
	zREAL           fallDownHeight;
	int             fallDownDamage;
	oCMag_Book*     mag_book;
	zCList<oCSpell>     activeSpells;
	int    lastHitSpellID;
	int    lastHitSpellCat;
	zCArray<zSTRING>    activeOverlays;
	oCAskBox*      askbox;
	int            askYes;
	int            askNo;
	zREAL          canTalk;
	oCNpc*         talkOther;
	oCInfo*        info;
	oCNews*        news;
	oCMission*     curMission;

	oCNewsMemory knownNews;

	zCVob*              carry_vob;
	oCMobInter*         interactMob;
	oCItem*             interactItem;
	int                 interactItemCurrentState;
	int                 interactItemTargetState;
	int                 script_aiprio;
	int                 old_script_state;
	oCAIHuman*          human_ai;
	oCAniCtrl_Human*    anictrl;
	zCRoute*            route;
	zREAL               damageMul;
	oCNpcMessage*       csg;
	oCNpcMessage*       lastLookMsg;
	oCNpcMessage*       lastPointMsg;
	zCArray<zCVob*>    vobList_array;
	zREAL     vobcheck_time;
	zREAL     pickvobdelay;
	zCVob*    focus_vob;
	zCArray<TNpcSlot*>  invSlot;
	zCArray<TNpcSlot*>  tmpSlotList;
	zREAL              fadeAwayTime;
	zREAL              respawnTime;
	zREAL              selfDist;
	int                fightRangeBase;
	int                fightRangeFist;
	int                fightRangeG;
	zREAL              fight_waitTime;
	zTModelAniID       fight_waitForAniEnd;
	zREAL              fight_lastStrafeFrame;
	int                soundType;
	zCVob*             soundVob;
	zVEC3              soundPosition;
	zCPlayerGroup*     playerGroup;
};
