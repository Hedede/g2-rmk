#include <Gothic/Game/oGuilds.h>

const int SENSE_SEE = 1;
const int SENSE_HEAR = 2;
const int SENSE_SMELL = 4;

const int NPC_FLAG_FRIEND = 1;
const int NPC_FLAG_IMMORTAL = 2;
const int NPC_FLAG_GHOST = 4;

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

	static int* CallScript(int funcIdx)
	{
		if ( funcIdx >= 0 )
			result = zparser.CallFunc(funcIdx);
		return 0;
	}

	static int* CallScript(zSTRING const& funcName)
	{
		auto idx = zparser.GetIndex(funcName);
		return CallScript(idx);
	}

	static bool IsInPerceptionRange(int perc, float range)
	{
		return perc < NPC_PERC_MAX; && range <= percRange[perc];
	}

	static int IsMessageAIEnabled()
	static int IsScriptStateAIEnabled();
	static void SetNpcAIDisabled(int b);

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
	virtual void SetVisual(zCVisual* vis)
	{
		zCVob::SetVisual(vis);
		zCVob::SetCollisionClass(&oCCollObjectCharacter::s_oCollObjClass);
	}


	// In debug info args are obnoxiously long: scriptInstanceName
	virtual bool GetScriptInstance(zSTRING*& name, int& index)
	virtual bool SetByScriptInstance(zSTRING const* name, int index);

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
	virtual bool IsMoreImportant(zCVob* vob1, zCVob* vob2);

	virtual void DoDoAniEvents();
	virtual void DoModelSwapMesh(zSTRING const &,zSTRING const &);
	virtual void DoTakeVob(zCVob *);
	virtual void DoDropVob(zCVob *);
	virtual void DoRemoveFromInventory(oCItem* item)
	{
		return RemoveFromInv(item) != 0;
	}
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



	virtual bool IsMonster() const
	{
		if (!IsHuman() && !IsOrc()) {
			if (guildTrue != GIL_DRAGON &&
			    guildTrue != GIL_FIREGOLEM &&
			    guildTrue != GIL_ICEGOLEM)
				return 1;
		}
		return 0;
	}
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

	zSTRING GetVisualBody() const
	{
		return body_visualName;
	}

	zSTRING GetVisualHead() const
	{
		return head_visualName;
	}

	zSTRING GetBloodTexture() const
	{
		return bloodTexture;
	}

	void GetAngles(zCVob* to, float& azi, float& elev)
	{
		GetAngles(to->GetPositionWorld(), azi, elev);
	}

	void SetFlag(int flag)
	{
		variousFlags &= flag;
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

	void SetAttitude(int att);
	void SetTmpAttitude(int att);

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

	int GetBluntDamage(oCItem* item);
	int GetWeaponDamage(oCItem* item);

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

	int HasPerception(int percId) const;
	int GetPerceptionFunc(int percId) const;

	void SetPerceptionTime(float time);

	void ClearPerception()
	{
		memset(percList, 0, sizeof(percList));
		percActive = 0;
	}

	int PercFilterNpc(oCNpc* other)
	{
		return 1;
	}

	int PercFilterItem(oCItem* item)
	{
		return !item->HasFlag(ITEM_NFOCUS);
	}

	int AssessWarn_S(zCVob* commiter);
	int AssessDefeat_S(zCVob* commiter);
	int AssessMurder_S(zCVob* commiter);
	int AssessRemoveWeapon_S(oCNpc* other);
	int AssessTheft_S(oCNpc* other);
	int AssessFakeGuild_S();
	int AssessCaster_S(oCSpell* spell);
	int AssessEnterRoom_S();
	int AssessOthersDamage_S(zCVob* commiter, zCVob* victim, int damage);
	int ObserveIntruder_S();


	static void CreateSoundPerception(int percType, zCVob* source, zVEC3 const& position, zCVob* victimVob, int setVictim)
	static int AssessQuietSound_S(zCVob* source, zVEC3 const& position);
	static int AssessFightSound_S(zCVob* source, zVEC3 const& position, zCVob* victim);


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

	void SetBodyStateModifier(int mod)
	{
		bodyState |= mod;
	}

	int ClrBodyStateModifier(int mod)
	{
		bodyState &= ~mod;
		return result;
	}

	int ModifyBodyState(int add, int remove)
	{
		bodyState |= add;
		bodyState &= ~remove;
		return bodyState & 0x7F;
	}

	bool HasBodyStateModifier(int bsmod) const
	{
		return bodyState & bsmod;
	}

	// 0x3F80
	static constexpr uint32_t bs_mods = BS_unknown1 |
	        BS_MOD_TRANSFORMED|BS_MOD_CONTROLLED|BS_MOD_BURNING|
	        BS_MOD_NUTS|BS_MOD_DRUNK|BS_MOD_HIDDEN;

	bool HasBodyStateFreeHands() const
	{
		if (bodyState & bs_mods == 0 )
			return bodyState & BS_FLAG_FREEHANDS;
		return 0;
	}

	int IsBodyStateInterruptable(oCNpc *this)
	{
		if (bodyState & bs_mods == 0)
			return bodyState & BS_FLAG_INTERRUPTABLE;
		return 0;
	}

	int IsAniMessageRunning() const
	{
		return aniMessageRunning;
	}

	struct oCNpcTimedOverlay {
		~oCNpcTimedOverlay() = default;
		zSTRING GetMdsName() const
		{
			return mdsName;
		}

		bool Process();

		zSTRING mdsName;
		float time;
	};


	void SetMovLock(int f);
	bool IsMovLock() const
	{
		return flags.movlock;
	}

	void SetWalkStopChasm(bool b)
	{
		if (GetAnictrl()) {
			GetAnictrl()->flags2.zMV_DO_DETECT_WALK_STOP_CHASM = b;
		}
	}

	bool GetWalkStopChasm()
	{
		return GetAnictrl() &&
		       GetAnictrl()->flags2.zMV_DO_DETECT_WALK_STOP_CHASM;
	}

	void SetSwimDiveTime(float swimSec, float diveSec)
	{
		swimtime = swimSec * 1000.0;
		divetime = diveSec * 1000.0;;
		divectr  = divetime;
	}

	void GetSwimDiveTime(float& swimTime, float& diveTime, float& diveCtr) const
	{
		swimTime = this->swimtime;
		diveTime = this->divetime;
		diveCtr  = this->divectr;
	}

	bool CanSwim() const
	{
		return swimTime > 0.0 || swimTime == -1000000.0;
	}

	bool CanDive() const
	{
		return diveTime > 0.0 || diveTime == -1000000.0;
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

private:
	void ForceRotation(float deg)
	{
		auto movmode = isInMovementMode;
		if ( movmode )
			EndMovement(1);

		anictrl->TurnDegrees(deg, 1);

		if ( movmode )
			BeginMovement();
	}

public:
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

	void CloseSpellBook(int removeall);
	void DestroySpellBook();

	bool HasSpell(int nr)
	{
		return spells & (1 << nr);
	}

	void DestroySpell(int nr)
	{
		if (HasSpell(nr)) {
			spells -= (1 << nr);
		}
	}

	void InsertActiveSpell(oCSpell* spell);
	oCSpell* IsSpellActive(int spellId);
	int GetActiveSpellLevel() const;
	int GetActiveSpellCategory() const;
	int GetActiveSpellNr() const;
	int GetActiveSpellIsScroll() const;

	int GetNumberOfSpells()
	{
		if (mag_book)
			return mag_book->GetNoOfSpells();
		return 0;
	}

	bool HasMagic()
	{
		return GetNumberOfSpells() > 0;
	}

	void SetFocusVob(zCVob* vob)
	{
		Set(focus_vob, vob);
	}

	zCVob* GetFocusVob()
	{
		return focus_vob;
	}

	oCNpc* GetFocusNpc()
	{
		return zDYNAMIC_CAST<oCNpc>(focus_vob);
	}

	void ClearFocusVob()
	{
		Release(focus_vob);
	}

	zCVob* GetCarryVob()
	{
		return carry_vob;
	}

	void SetTradeNpc(oCNpc* npc)
	{
		tradeNpc = npc;
	}

	oCNpc* GetTradeNpc()
	{
		return tradeNpc;
	}

	void CloseDeadNpc();

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

	void oCNpc::SetTalkingWith(oCNpc* other, int talking)
	{
		if ( !other ) {
			talkOther = nullptr;
			return;
		}

		if ( talking ) {
			talkOther = other;
		} else if ( talkOther == other ) {
			talkOther = 0;
		}
	}

	oCNpc* GetTalkingWith()
	{
		return talkOther;
	}

	void SetCanTalk(float timeSec)
	{
		canTalk = timeSec * 1000.0;
	}

	bool CanTalk() const
	{
		return canTalk <= 0.0;
	}

	bool IsWaitingForAnswer();

	void SetShowNews(int b)
	{
		flags.showNews = b;
	}

	int HasMissionItem();
	oCItem* GetTradeItem();
	oCItem* PutInInv(zSTRING const& name, int anz);

	bool IsInInv(oCItem* item, int amount)
	{
		return inventory.IsIn(item, amount);
	}

	// TODO: return type?
	oCItem* IsInInv(int inst, int amount)
	{
		return inventory.IsIn(inst, amount);
	}

	oCItem* IsInInv(zSTRING const& name, int amount)
	{
		return inventory.IsIn(name, amount);
	}

	oCItem* RemoveFromInv(int inst, int amount);
	oCItem* RemoveFromInv(oCItem* item, int amount);

	oCItem* GetFromInv(int instance, int amount);

	void EquipBestArmor();
	void EquipBestWeapon(int mode);

	TNpcSlot* GetInvSlot(zCVob* vob)
	{
		for (auto& slot : invSlots) {
			if (slot->object == vob)
				return slot;
		}
		return nullptr;
	}

	void RemoveFromAllSlots(int dropIt)
	{
		for (auto& slot : invSlots)
			RemoveFromSlot(slot, dropIt, 1);
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


	float GetJumpRange() const
	{
		return 300.0;
	}

	float GetClimbRange() const
	{
		return 500.0;
	}

	void ExitFightAI();
	int IsInFightMode_S();
	int GetWeaponMode()
	{
		if (fmode < 0 || fmode >= 8)
			fmode = 0;
		return fmode;
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

	//! Weapon or fist range
	int GetFightRangeDynamic() const;

	//! Full fight range
	int GetFightRange() const;

	struct oCNpc::oSActionBlock {
		int move[6];
		int numMoves;

		int GetOwnAction(int);
		void InitBlock(zCParser* parser, int action_id, int fa_id);
	};

	int FindNextFightAction();
	int GetCurrentFightMove();

private:
	int GetFightActionFromTable(int situationNr);

public:

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

	int InitAim(oCMsgAttack* csg, oCNpc** enemy, int* drawn, int* ammo, int killFormerMsg); // csg typo in original code
	int FinalizeAim(int startMelee, int standUp);

	int EV_Defend(oCMsgAttck* msg);
	int EV_AimAt(oCMsgAttack* msg);
	int EV_StopAim(oCMsgAttck*);
	int EV_AttackMagic(oCMsgAttack*);
	int EV_RemoveInteractItem(oCMsgManipulate*);
	int EV_EquipItem(oCMsgManipulate* msg);
	int EV_TakeMob(oCMsgManipulate* msg);
	int EV_DropMob(oCMsgManipulate* msg);

	int EV_EquipBestWeapon(oCMsgWeapon* msg);
	int EV_EquipBestArmor(oCMsgWeapon* msg);
	int EV_EquipArmor(oCMsgWeapon* msg);
	int EV_UnequipArmor(oCMsgWeapon*);

	int EV_StartFX(oCMsgConversation* msg);
	int EV_PlayAniSound(oCMsgConversation* msg);
	int EV_StopPointAt(oCMsgConversation* msg);
	int EV_StopLookAt(oCMsgConversation* msg);
	int EV_ProcessInfos(oCMsgConversation* msg);
	int EV_StopProcessInfos(oCMsgConversation* msg);
	int EV_Ask(oCMsgConversation* msg);
	int EV_WaitTillEnd(oCMsgConversation* msg);

	int EV_Turn(oCMsgMovement* msg);
	int EV_TurnToPos(oCMsgMovement*);
	int EV_TurnToVob(oCMsgMovement *);
	int EV_TurnAway(oCMsgMovement *);
	int EV_StandUp(oCMsgMovement *);
	int EV_Jump(oCMsgMovement* msg);
	int EV_CanSeeNpc(oCMsgMovement* msg);
	int EV_RobustTrace(oCMsgMovement* msg);
	int EV_GotoPos(oCMsgMovement* msg);


	int EV_DamagePerFrame(oCMsgDamage *);
	int EV_Strafe(oCMsgMovement *);
	int EV_AttackForward(oCMsgAttack *);
	int EV_PlaySound(oCMsgConversation *);
	int EV_AttackLeft(oCMsgAttack *);
	int EV_AttackRight(oCMsgAttack *);
	int EV_RemoveWeapon(oCMsgWeapon *);
	int EV_UseItemToState(oCMsgManipulate *);
	int EV_AttackFinish(oCMsgAttack *);
	int EV_OutputSVM_Overlay(oCMsgConversation *);
	int EV_GoRoute(oCMsgMovement *);
	int EV_DrawWeapon(oCMsgWeapon *);
	int EV_Parade(oCMsgAttack *);
	int EV_WhirlAround(oCMsgMovement *);
	int EV_DrawWeapon2(oCMsgWeapon *);
	int EV_OutputSVM(oCMsgConversation *);
	int EV_LookAt(oCMsgConversation *);
	int EV_SetWalkMode(oCMsgMovement *);
	int EV_TakeVob(oCMsgManipulate *);
	int EV_Output(oCMsgConversation *);
	int EV_RemoveWeapon2(oCMsgWeapon *);
	int EV_DamageOnce(oCMsgDamage *);
	int EV_Drink(oCMsgUseItem *);
	int EV_AttackRun(oCMsgAttack *);
	int EV_Exchange(oCMsgManipulate *);
	int EV_CastSpell(oCMsgMagic *);
	int EV_GotoFP(oCMsgMovement *);
	int EV_DropVob(oCMsgManipulate *);
	int EV_ForceRemoveWeapon(oCMsgWeapon *);
	int EV_UseItem(oCMsgManipulate *);
	int EV_ChooseWeapon(oCMsgWeapon *);
	int EV_PrintScreen(oCMsgConversation *);
	int EV_CreateInteractItem(oCMsgManipulate *);
	int EV_DrawWeapon1(oCMsgWeapon *);
	int EV_QuickLook(oCMsgConversation *);
	int EV_ThrowVob(oCMsgManipulate *);
	int EV_AttackBow(oCMsgAttack *);
	int EV_Unconscious(oCMsgState *);
	int EV_AlignToFP(oCMsgMovement *);
	int EV_Dodge(oCMsgMovement *);
	int EV_PlayAni(oCMsgConversation *);
	int EV_DoState(oCMsgState *);
	int EV_InsertInteractItem(oCMsgManipulate *);
	int EV_UseMob(oCMsgManipulate *);
	int EV_Cutscene(oCMsgConversation *);
	int EV_WaitForQuestion(oCMsgConversation *);
	int EV_DestroyInteractItem(oCMsgManipulate *);
	int EV_SndPlay(oCMsgConversation *);
	int EV_UnequipWeapons(oCMsgWeapon *);
	int EV_PointAt(oCMsgConversation *);
	int EV_ExchangeInteractItem(oCMsgManipulate *);
	int EV_ShootAt(oCMsgAttack *);
	int EV_GotoVob(oCMsgMovement *);
	int EV_RemoveWeapon1(oCMsgWeapon *);
	int EV_CallScript(oCMsgManipulate *);
	int EV_StopFX(oCMsgConversation *);
	int EV_PlaceInteractItem(oCMsgManipulate *);
	int EV_PlayAniFace(oCMsgConversation *);

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

	void DeleteHumanAI();

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

	bool IsFadingAway() const
	{
		return states.IsInState(-5);
	}

	bool IsDead() const
	{
		return attribute[ATR_HITPOINTS] <= 0;
	}

	bool IsDisguised() const
	{
		return guild != guildTrue;
	}

	void SetCSEnabled(bool enabled)
	{
		csAllowedAsRole = enabled;
	}


	void SetRoute(zCRoute* rt)
	{
		// appears to be macro or inlined func,
		// if (route) { delete route; route = rt} else {route = rt}
		if (route)
			delete route;
		route = rt;
	}

	// Robust trace
	RbtInit(zVEC3& tpos, zCVob* tvob)
	{
		RbtReset();
		RbtUpdate(tpos, tvob);
	}

	zCVob* GetRbtObstacleVob()
	{
		return rbt.obstVob;
	}

	float GetThrowSpeed(float dist, float angle)
	{
		return sqrt(dist * 981.0 / sin(angle * 0.017453292 + angle * 0.017453292)) * 0.8999999761581421;
	}


	int IsInGlobalCutscene();
	int GetCutsceneDistance();

	// остатки мультиплеера
	zSTRING oCNpc::GetInstanceByID(int nr) const;

	// useless
	void AI_Follow(oCNpc*) {};
	void AI_Flee(oCNpc*) {};
	void Follow() { }
	void SetToDrunk(float) {};
	void HealFromDrunk() {}
	int CanRecruitSC() { return 0; }
	void OpenScreen_Help() {}
	void ExchangeTorch() {}
	void DropInventory() {}
	void Activate(int cat, int nr) {}
	int GetCamp() { return 0; }

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

		struct {
			unsigned stand                : 1;
			unsigned dirChoosed           : 1;
			unsigned exactPosition        : 1;
			unsigned targetReached        : 1;
			unsigned standIfTargetReached : 1;
			unsigned waiting              : 1;
			unsigned isObstVobSmall       : 1;
			unsigned targetVisible        : 1;
			unsigned useChasmChecks       : 1;
		} flags;

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
