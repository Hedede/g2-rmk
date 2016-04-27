#include <Gothic/Game/oGuilds.h>

const int SENSE_SEE = 1;
const int SENSE_HEAR = 2;
const int SENSE_SMELL = 4;

enum NpcPercId {
	PERC_ASSESSPLAYER = 1,
	PERC_ASSESSENEMY = 2,
	PERC_ASSESSFIGHTER = 3,
	PERC_ASSESSBODY = 4,
	PERC_ASSESSITEM = 5,
	PERC_ASSESSMURDER = 6,
	PERC_ASSESSDEFEAT = 7,
	PERC_ASSESSDAMAGE = 8,
	PERC_ASSESSOTHERSDAMAGE = 9,
	PERC_ASSESSTHREAT = 10,
	PERC_ASSESSREMOVEWEAPON = 11,
	PERC_OBSERVEINTRUDER = 12,
	PERC_ASSESSFIGHTSOUND = 13,
	PERC_ASSESSQUIETSOUND = 14,
	PERC_ASSESSWARN = 15,
	PERC_CATCHTHIEF = 16,
	PERC_ASSESSTHEFT = 17,
	PERC_ASSESSCALL = 18,
	PERC_ASSESSTALK = 19,
	PERC_ASSESSGIVENITEM = 20,
	PERC_ASSESSFAKEGUILD = 21,
	PERC_MOVEMOB = 22,
	PERC_MOVENPC = 23,
	PERC_DRAWWEAPON = 24,
	PERC_OBSERVESUSPECT = 25,
	PERC_NPCCOMMAND = 26,
	PERC_ASSESSMAGIC = 27,
	PERC_ASSESSSTOPMAGIC = 28,
	PERC_ASSESSCASTER = 29,
	PERC_ASSESSSURPRISE = 30,
	PERC_ASSESSENTERROOM = 31,
	PERC_ASSESSUSEMOB = 32,
	PERC_MAX = 33;
};

enum Sense {
	SENSE_SEE = 1,
	SENSE_HEAR = 2,
	SENSE_SMELL = 4,
};

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
	static oCNpc* player;

	static bool IsSlotFree(TNpcSlot* slot)
	{
		return slot && slot->object == nullptr;
	}

	static void ReleaseStatics()
	{
		oCNpc::ExitFightAI();
	}

	static void EnableDamageDebugOutput(zBOOL b)
	{
		oCNpc::isEnabledDamageDebug = b;
	}

	static bool HasFlag(unsigned flag, unsigned value)
	{
		return (flag & value);
	}

	static void SetPerceptionRange(int percId, float range)
	{
		if (percId < PERC_MAX)
			percRange[percId] = range;
	}

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);

	oCNpc();
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

	virtual int GetInstance() const
	{
		return instanz;
	}
	virtual zSTRING GetInstanceName() const
	{
		int typ, ele;
		return zparser.GetSymbolInfo(instanz, typ, ele);
	}

	virtual void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium &,oTSndMaterial &,int);

	virtual void SetWeaponMode(int);
	virtual void SetWeaponMode2(int);
	void SetWeaponMode2(zSTRING const& str);

	virtual void DoDie(oCNpc *);
	virtual void DoInsertMunition(zSTRING const &);
	virtual void DoRemoveMunition();
	virtual bool DoSetToFightMode(oCItem* item)
	{
		if (item) {
			SetToFightMode(item, 0);
			return 1;
		}
		return 0;
	}
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
	virtual bool DoPutInInventory(oCItem* item)
	{
		if (item) {
			PutInInv(item);
			return 1;
		}
		return 0;
	}
	virtual void DoThrowVob(zCVob *,float);
	virtual void DoExchangeTorch();

	virtual void GetSoundMaterial_MA(zCSoundManager::zTSndManMedium &,oTSndMaterial &,zSTRING);
	virtual void GetSoundMaterial_MH_HM(zCSoundManager::zTSndManMedium &,oTSndMaterial &);
	virtual bool IsAPlayer() const
	{
		return this == oCNpc::player;
	}
	virtual bool IsSelfPlayer() const
	{
		return IsAPlayer();
	}
	virtual void SetAsPlayer();



	virtual void IsMonster();
	virtual bool IsHuman() const
	{
		return guildTrue <= 16;
	}
	virtual void IsGoblin() const
	{
		return guildTrue == GIL_GOBBO ||
		       guildTrue == GIL_GOBBO_SKELETON ||
		       guildTrue == GIL_SUMMONED_GOBBO_SKELETON;

	}
	virtual void IsOrc() const
	{
		return guildTrue >= GIL_SEPARATOR_ORC;
	}
	virtual void IsSkeleton() const
	{
		// no GIL_SKELETON_MAGE, huh?
		return guildTrue == GIL_SKELETON ||
		       guildTrue == GIL_SUMMONED_SKELETON;
	}


	virtual void GetPlayerNumber();
	virtual void IsAniMessageRunning();
	virtual void ProcessNpc();
	virtual int AllowDiscardingOfSubtree()
	{
		return 1;
	}

	void HasFlag(int flag) const
	{
		return HasFlag(variousFlags, flag);
	}

	void SetSenses(int bf)
	{
		senses = bf;
	}

	bool HasSenseSee() const
	{
		return senses & SENSE_SEE;
	}

	bool HasSenseHear() const
	{
		return senses & SENSE_HEAR;
	}

	bool HasSenseSmell() const
	{
		return senses & SENSE_SMELL;
	}

	void SetKnowsPlayer()
	{
		knowsPlayer = true;
	}

	bool KnowsPlayer(int) const
	{
		return knowsPlayer;
	}

	bool KnowsPlayer(oCNpc*) const
	{
		return knowsPlayer;
	}

	// or DOCREATE?
	zCEventManager* GetEM(bool dontCreate = false);

	void SetAttribute(int atr, int val)
	{
		attribute[atr] = val;
		CheckModelOverlays();
	}

	int GetAttribute(int atr) const
	{
		return attribute[atr];
	}

	void ComleteHeal()
	{
		attribute[ATR_HITPOINTS] = attribute[ATR_HITPOINTS_MAX];
		attribute[ATR_MANA]      = attribute[ATR_MANA_MAX];
	}

	bool HasTalent(int nr, int skill) const
	{
		return GetTalentSkill(nr) >= skill;
	}

	void SetHitChance(int hc, int val)
	{
		hitChance[hc] = val;
	}

	int GetHitChance(size_t id) const
	{
		return hitChance[id];
	}

	void SetDamages(int arr[DAM_INDEX_MAX])
	{
		if (arr)
			memcpy(damage, arr, sizeof(damage));
	}

	int GetDamageByIndex(int idx) const
	{
		return damage[idx];
	}

	int GetFullDamage() const
	{
		int dmg = 0;
		for (int v : damage)
			dmg += v;
		return dmg;
	}

	void SetProtectionByIndex(int idx, int val)
	{
		protection[idx] = val;
	}

	int GetProtectionByIndex(int idx) const
	{
		return protection[idx];
	}

	int GetFullProtection() const
	{
		int prot = 0;
		for (int v : protection)
			prot += v;
		return prot;
	}

	void SetDamageMultiplier(float val)
	{
		damageMul = val;
	}

	float GetDamageMultiplier() const
	{
		return damageMul;
	}

	zSTRING GetName() const
	{
		return name[0];
	}

	int GetGuild() const
	{
		return guild;
	}

	zSTRING GetGuildName() const
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

	void ClearPerception()
	{
		memset(percList, 0, sizeof(percList));
		percActive = 0;
	}

	int AssessTheft_S(oCNpc* other)
	{
		CreatePassivePerception(PERC_ASSESSTHEFT, other, this);
		return 1;
	}

	int AssessFakeGuild_S()
	{
		if ( IsSelfPlayer() ) {
			CreatePassivePerception(PERC_ASSESSFAKEGUILD, this, 0);
			return 1;
		}

		return 0;
	}

	int AssessCaster_S(oCSpell* spell)
	{
		CreatePassivePerception(PERC_ASSESSCASTER, this, 0);
		return 1;
	}

	oCAIHuman* GetAnictrl()
	{
		return human_ai;
	}

	int GetBodyState() const
	{
		return bodyState & 0x7F;
	}

	int GetFullBodyState() const
	{
		return bodyState & 0xFFFFC07F;
	}

	int IsAniMessageRunning() const
	{
		return aniMessageRunning;
	}

	bool IsMovLock() const
	{
		return flags.movlock;
	}

	double GetTurnSpeed()
	{
		if (IsAPlayer())
			return 0.1;
		return speedTurn;
	}

	void SetFallDownHeight(float height)
	{
		fallDownHeight = height;
	}

	float GetFallDownHeight() const
	{
		if (overrideFallDownHeight)
			return 0.0;
		return fallDownHeight;
	}

	void SetFallDownDamage(int damage)
	{
		fallDownDamage = damage;
	}

	zCVob* GetSoundSource()
	{
		return soundVob;
	}

	oCMobInter* GetInteractMob()
	{
		return interactMob;
	}

	int GetPlayerNumber()
	{
		return 0;
	}

	oCItem* GetItem(int category, int slot)
	{
		return inventory2.GetItem(slot);
	}

	oCMag_Book* GetSpellBook()
	{
		return mag_book;
	}

	void CloseSpellBook(int removeall)
	{
		if (mag_book)
			mag_book->Close(removeall)
	}

	zCVob* GetFocusVob()
	{
		return focus_vob;
	}

	zCVob* GetCarryVob()
	{
		return carry_vob;
	}

	zCVob* GetRbtObstacleVob()
	{
		return rbt.obstVob;
	}

	void SetTradeNpc(oCNpc* npc)
	{
		tradeNpc = npc;
	}

	oCNpc* GetTradeNpc()
	{
		return tradeNpc;
	}

	void StartTalkingWith(oCNpc* other)
	{
		if (other) {
			this->talkOther = other;
			other->talkOther = this;
		}
	}

	void StopTalkingWith()
	{
		if (talkOther)
			talkOther->talkOther = nullptr;
		talkOther = nullptr;
	}

	oCNpc* GetTalkingWith()
	{
		return talkOther;
	}

	void SetCanTalk(float timeSec)
	{
		canTalk = timeSec * 1000.0;
	}

	void SetShowNews(int b)
	{
		flags.showNews = b;
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

	void CloseSteal() // static?
	{
		if (oCNpc::stealcontainer) {
			oCNpc::stealcontainer->Close();
			oCNpc::game_mode = 0;
		}
	}

	void CloseInventory()
	{
		inventory.Close();
		CloseTradeContainer();
	}

	oCItem* DetectItem(int flags, int);


	int GetFightRange() const;

	float GetJumpRange() const
	{
		return 300.0;
	}

	float GetClimbRange() const
	{
		return 500.0;
	}

	void GetFightRangeBase() const
	{
		return fightRangeBase;
	}

	void SetFightRangeBase(int range)
	{
		fightRangeBase = range;
	}

	int SetFightRangeFist() const
	{
		return fightRangeFist;
	}

	void SetFightRangeFist(int range)
	{
		fightRangeFist = range;
	}

	int GetFightRangeG() const
	{
		return fightRangeG;
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

	TActiveInfo GetActiveInfo() const
	{
		return s_activeInfoCache[this];
	}

	TActiveInfo& GetActiveInfoWriteable()
	{
		return s_activeInfoCache[this];
	}

	int EV_AttackMagic(oCMsgAttack*)
	{
		return 0;
	}

	void Fleeing()
	{
		ThinkNextFleeAction();
	}

	bool IsVoiceActive() const
	{
		return listOfVoiceHandles.GetNumInList() > 0;
	}

	void AI_ForceDetection()
	{
		vobcheck_time = 100000.0;
	}

	int GetAIState() const
	{
		return states.GetState();
	}

	int GetAIStateTime() const
	{
		return states.GetStateTime();
	}

	int IsAIState(int funcInst) const
	{
		return states.IsInState(funcInst);
	}

	bool IsUnconscious() const
	{
		return states.IsInState(ZS_Unconscious); // -4
	}

	bool IsDead() const
	{
		return attribute[ATR_HITPOINTS] <= 0;
	}

	bool IsDisguised() const
	{
		return guild != guildTrue;
	}


	// useless
	void AI_Follow(oCNpc*) {};
	void AI_Flee(oCNpc*) {};
	void Follow() { }
	void SetToDrunk(float) {};
	void HealFromDrunk() {}
	int CanRecruitSC() { return 0; }

private:
	// ----------------
	int     idx = 0;
	zSTRING name[5];
	zSTRING slot;
	zSTRING effect;
	int     npcType = 0;
	int     variousFlags = 0;
	int     attribute[NPC_ATR_MAX] = {};
	int     hitChance[NPC_HITCHANCE_MAX] = {};
	int     protection[DAM_INDEX_MAX] = {};
	int     damage[DAM_INDEX_MAX] = {};
	int     damagetype = 0;
	int     guild = GIL_NONE;
	int     level = 0;
	void*   mission[NPC_MIS_MAX] = {};
	int     fighttactic = 0;
	int     fmode = 0;
	int     voice = 0;
	int     voicePitch = 0;
	int     mass = 0;
	void*   daily_routine = 0;
	void*   startAIState = 0;
	zSTRING spawnPoint;
	int     spawnDelay = 0;
	int     senses = 0;
	int     senses_range = 0;
	int     aiscriptvars[100] = {};
	zSTRING wpname;
	zUINT32 experience_points = 0;
	zUINT32 experience_points_next_level = 0;
	zUINT32 learn_points = 0;
	int     bodyStateInterruptableOverride = 0;
	zBOOL   noFocus = 0;
	int     parserEnd;
	// ----------------

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
		~oTRobustTrace() = default();

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

	zCList<oCNpcTimedOverlay> timedOverlays;

	zCArray<oCNpcTalent*>     talents;
	int    spellMana = 0;

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

	oCNpcInventory   inventory;
	oCItemContainer* trader;
	oCNpc*           tradeNpc = 0;

	zREAL rangeToPlayer = std::numeric_limits<float>::max();

	zCArray<zTSoundHandle>  listOfVoiceHandles;
	int    voiceIndex = 0;
	zCArray<oCVisualFX*> effectList;

	struct {
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
	} flags;

	uint16_t body_TexVarNr   = 0;
	uint16_t body_TexColorNr = 0;
	uint16_t head_TexVarNr   = 0;
	uint16_t teeth_TexVarNr  = 0;

	uint8_t guildTrue = GIL_NONE;
	uint8_t drunk_heal = 0;
	uint8_t mad_heal   = 0;
	uint8_t spells     = 0;

	uint32_t bodyState         : 19;
	uint32_t aniMessageRunning : 1;

	int    instanz = -1;

	zSTRING mds_name;
	zSTRING body_visualName;
	zSTRING head_visualName;

	VEC3       model_scale{1.0, 1.0, 1.0};
	zREAL      model_fatness = 0.0;
	int        namenr = 0;
	zREAL      hpHeal;
	zREAL      manaHeal;

	zREAL      swimtime = 0.0;
	zREAL      divetime = 0.0;
	zREAL      divectr  = 0.0;

	zCVob*     fireVob         = 0;
	int        fireDamage      = 0;
	zREAL      fireDamageTimer = 0.0;

	int attitude     = ATT_NEUTRAL; //nok
	int tmp_attitude = ATT_NEUTRAL;
	zREAL attTimer   = 0.0;

	int   knowsPlayer = 0;

	struct TNpcPerc {
		int percID;
		int percFunc;
	}  percList[NPC_PERC_MAX];

	int    percActive      = 0;
	int    percActiveTime  = 0;
	zREAL  percActiveDelta = 0.0;

	zBOOL  overrideFallDownHeight = 0;
	zREAL  fallDownHeight = 500.0;
	int    fallDownDamage = 0;

	oCMag_Book*     mag_book = 0;
	zCList<oCSpell> activeSpells;

	int    lastHitSpellID  = -1;
	int    lastHitSpellCat = -1;

	zCArray<zSTRING>    activeOverlays;

	oCAskBox*      askbox = 0;
	int            askYes = 0;
	int            askNo  = 0;

	zREAL          canTalk = 1.0;
	oCNpc*         talkOther = 0;
	oCInfo*        info = 0;
	oCNews*        news = 0;
	oCMission*     curMission = 0;

	oCNewsMemory knownNews;

	zCVob*              carry_vob;
	oCMobInter*         interactMob  = 0;
	oCItem*             interactItem = 0;
	int                 interactItemCurrentState = -1;
	int                 interactItemTargetState  = 0;
	int                 script_aiprio    = 0;
	int                 old_script_state = 0;

	oCAIHuman*          human_ai;
	oCAniCtrl_Human*    anictrl;
	zCRoute*            route;

	zREAL               damageMul = 1.0;

	oCNpcMessage*       csg;
	oCNpcMessage*       lastLookMsg;
	oCNpcMessage*       lastPointMsg;

	zCArray<zCVob*> vobList;
	zREAL  vobcheck_time = 0.0;
	zREAL  pickvobdelay  = 0.0;
	zCVob*  focus_vob    = nullptr;

	zCArray<TNpcSlot*>  invSlot;
	zCArray<TNpcSlot*>  tmpSlotList;

	zREAL fadeAwayTime;
	zREAL respawnTime;
	zREAL selfDist = 0.0;

	int                fightRangeBase = 0;
	int                fightRangeFist = 0;
	int                fightRangeG    = 0;
	zREAL              fight_waitTime        = 0.0;
	zTModelAniID       fight_waitForAniEnd   = -1;
	zREAL              fight_lastStrafeFrame = 0;

	int                soundType = 0;
	zCVob*             soundVob  = 0;
	zVEC3              soundPosition;

	zCPlayerGroup*     playerGroup;
};
