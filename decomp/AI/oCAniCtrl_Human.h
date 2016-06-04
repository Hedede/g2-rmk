//######################################################
//
//  Keine Garantie auf diese beiden Klassen.
//  Besonders oCAniCtrl_Human erschien mir im
//  nachhinein eher nutzlos
//  daher habe ich wenig Anstrengung in sie gesteckt.
//  zCAIPlayer müsste aber noch stimmen und 
//  zumindest die "Species.d" Eigenschaften sind
//  für Modder interessant. Zum Beispiel könnte man
//  einen "Wasserläufer" Zauber bauen oder
//  machen, dass der Spieler steile Wände hochkommt.
//  Nutzung auf eigene Gefahr.
//
//######################################################
const int ANI_NUM = 10;
const int ANI_HITLIMB_MAX = 4;

class oCAniCtrl_Human : public zCAIPlayer {
	Z_OBJECT(oCAniCtrl_Human);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~oCAniCtrl_Human();
	virtual void StartStandAni();
	virtual void StartFallDownAni();
	virtual void Init(oCNpc* npc)
	{
		if ( npc ) {
			npc->AvoidShrink(1000);
			SetVob(npc);

			world = ogame->GetWorld();

			SetScriptValues();
			flags.zMV_DO_DETECT_WALK_STOP_CHASM = 1;
			InitAnimations();
		}
	}

	void Reset()
	{
		InitFightAnis(npc->GetWeaponMode());
		SetWalkMode(0);
		StartStandAni();
	}

	void StartUp(zCVob* vob)
	{
		zCAIPlayer::Begin(vob);
	}

	int GetActionMode()
	{
		return actionMode;
	}

	void StopTurnAnis()
	{
		model->FadeOutAnisLayerRange(20, 20);
	}

	bool IsAlwaysWalk()
	{
		return always_walk && npc->IsAPlayer();
	}

	void ToggleWalkMode(int nextAni)
	{
		StopTurnAnis();
		StartAni(togglewalk, nextAni);
		SetWalkMode(nextwalkmode);
	}

	void StrafeLeft()
	{
		if (actionMode != 5) {
			StopTurnAnis();
			StartAni(_t_strafel, 0);
			npc->SetBodyState(BS_WALK);
		}
	}

	void StrafeRight()
	{
		if (actionMode != 5) {
			StopTurnAnis();
			StartAni(_t_strafer, 0);
			npc->SetBodyState(BS_WALK);
		}
	}

	void SetFightAnis(int fmode);

	void StartStandAni()
	{
		model->StartAni(_s_walk, 0); //??
	}

	void StopCombineAni(int id)
	{
		model->FadeOutAni(id);
	}

	void SetComboHitTarget(zCVob* target)
	{
		hitTarget = target;
	}

	void LookAtTarget()
	{
		InterpolateCombineAni(lookTargetx, lookTargety, s_look);
	}

	void HitInterrupt();

	zCModelAniActive* GetLayerAni(int layer);
	int CheckLayerAni(int layer, zSTRING const&)
	{
		return GetLayerAni(int layer) != 0 ? -1 : 0;
	}

	void FirstPersonDrawWeapon() {}
	void DoAniWithOffset() {}

private:
	zREAL  angle_slide1;
	zREAL  angle_slide2;
	zREAL  angle_heading;
	zREAL  angle_horiz;
	zREAL  angle_ground;

	oCNpc* npc;
	zCVob* targetVob;
	zCVob* stopTurnVob;
	int    stopTurnVobSign;

	int hitlimb[ANI_HITLIMB_MAX];        //zCModelNodeInst* FightLimbs

	int actionMode;

	// Waffen toggeln
	int weaponmode;
	int weaponmode_last;
	int weaponmode_selected;
	int changeweapon;

	// Walkmode
	int walkmode;
	int nextwalkmode;
	int lastRunWalkSneak;
	zBOOL always_walk;
	zBOOL do_jump;
	int defaultSurfaceAlign;
	zREAL autoRollDirection;

	// Kopfbewegungen
	zREAL                      lookTargetx;
	zREAL                      lookTargety;

	// Fight
	int distance;
	zVEC3                  hitpos;
	zSTRING limbname;
	zREAL                   paradeBeginFrame;
	zREAL                   paradeEndFrame;

	//so wie im Struct im Kommentar
	struct {
		zUINT8          canEnableNextCombo              : 1;
		zUINT8          endCombo                        : 1;
		zUINT8          comboCanHit                     : 1;
		zUINT8          hitPosUsed                      : 1;
		zUINT8          hitGraphical                    : 1;
		zUINT8          canDoCollisionFX                : 1;
	} flags;

	int          comboNr;
	int          comboMax;
	zREAL        lastHitAniFrame;
	zTModelAniID hitAniID;
	zCVob*       hitTarget;

	typedef struct {
		zTModelAniID    hitAni;
		int             hitOptFrame;
		int             hitEndFrame;
		int             comboStartFrame;
		int             comboEndFrame;
		int             comboDir;
	} oTComboInfo;

	//ANI_MAX_HIT = 10
	oTComboInfo comboInfo[ANI_MAX_HIT];

	// Kampfsystem - Tuning
	zREAL        anioffset_lastper;
	zREAL        anioffset_thisper;
	zREAL        anioffset;
	zTModelAniID anioffset_ani;

	// *******************************************************************(

	// Normal
	zTModelAniID s_dead1;
	zTModelAniID s_dead2;
	zTModelAniID s_hang;
	zTModelAniID t_hang_2_stand;

	zTModelAniID s_run               [ANI_NUM];
	zTModelAniID t_run_2_runl      [ANI_NUM];
	zTModelAniID t_runl_2_run    [ANI_NUM];
	zTModelAniID s_runl              [ANI_NUM];
	zTModelAniID t_runl_2_runr     [ANI_NUM];
	zTModelAniID t_runr_2_runl   [ANI_NUM];
	zTModelAniID s_runr              [ANI_NUM];
	zTModelAniID t_runr_2_run      [ANI_NUM];
	zTModelAniID t_runturnl          [ANI_NUM];
	zTModelAniID t_runturnr        [ANI_NUM];
	zTModelAniID t_runstrafel        [ANI_NUM];
	zTModelAniID t_runstrafer      [ANI_NUM];

	zTModelAniID t_run_2_runbl       [ANI_NUM];
	zTModelAniID t_runbl_2_run     [ANI_NUM];
	zTModelAniID s_runbl             [ANI_NUM];
	zTModelAniID t_runbl_2_runbr   [ANI_NUM];
	zTModelAniID t_runbr_2_runbl [ANI_NUM];
	zTModelAniID s_runbr             [ANI_NUM];
	zTModelAniID t_runbr_2_run     [ANI_NUM];


	zTModelAniID t_jumpb             [ANI_NUM];

	// Transitions
	zTModelAniID t_run_2_walk        [ANI_NUM];
	zTModelAniID t_walk_2_run      [ANI_NUM];
	zTModelAniID t_run_2_sneak       [ANI_NUM];
	zTModelAniID t_sneak_2_run     [ANI_NUM];

	zTModelAniID s_walk            [ANI_NUM];
	zTModelAniID t_walk_2_walkl    [ANI_NUM];
	zTModelAniID t_walkl_2_walk    [ANI_NUM];
	zTModelAniID s_walkl           [ANI_NUM];
	zTModelAniID t_walkl_2_walkr   [ANI_NUM];
	zTModelAniID t_walkr_2_walkl   [ANI_NUM];
	zTModelAniID s_walkr           [ANI_NUM];
	zTModelAniID t_walkr_2_walk    [ANI_NUM];
	zTModelAniID t_walkturnl       [ANI_NUM];
	zTModelAniID t_walkturnr       [ANI_NUM];
	zTModelAniID t_walkstrafel     [ANI_NUM];
	zTModelAniID t_walkstrafer     [ANI_NUM];

	zTModelAniID t_walk_2_walkbl   [ANI_NUM];
	zTModelAniID t_walkbl_2_walk   [ANI_NUM];
	zTModelAniID s_walkbl          [ANI_NUM];
	zTModelAniID t_walkbl_2_walkbr [ANI_NUM];
	zTModelAniID t_walkbr_2_walkbl [ANI_NUM];
	zTModelAniID s_walkbr          [ANI_NUM];
	zTModelAniID t_walkbr_2_walk   [ANI_NUM];

	zTModelAniID t_runl_2_jump     [ANI_NUM];
	zTModelAniID t_runr_2_jump     [ANI_NUM];
	zTModelAniID t_jump_2_runl     [ANI_NUM];

	zTModelAniID t_stand_2_jumpuplow;
	zTModelAniID s_jumpuplow;
	zTModelAniID t_jumpuplow_2_stand;
	zTModelAniID t_stand_2_jumpupmid;
	zTModelAniID s_jumpupmid;
	zTModelAniID t_jumpupmid_2_stand;

	zTModelAniID s_sneak             [ANI_NUM];
	zTModelAniID t_sneak_2_sneakl    [ANI_NUM];
	zTModelAniID t_sneakl_2_sneak    [ANI_NUM];
	zTModelAniID s_sneakl            [ANI_NUM];
	zTModelAniID t_sneakl_2_sneakr   [ANI_NUM];
	zTModelAniID t_sneakr_2_sneakl   [ANI_NUM];
	zTModelAniID s_sneakr            [ANI_NUM];
	zTModelAniID t_sneakr_2_sneak    [ANI_NUM];
	zTModelAniID t_sneakturnl        [ANI_NUM];
	zTModelAniID t_sneakturnr        [ANI_NUM];
	zTModelAniID t_sneakstrafel      [ANI_NUM];
	zTModelAniID t_sneakstrafer      [ANI_NUM];

	zTModelAniID t_sneak_2_sneakbl   [ANI_NUM];
	zTModelAniID t_sneakbl_2_sneak   [ANI_NUM];
	zTModelAniID s_sneakbl           [ANI_NUM];
	zTModelAniID t_sneakbl_2_sneakbr [ANI_NUM];
	zTModelAniID t_sneakbr_2_sneakbl [ANI_NUM];
	zTModelAniID s_sneakbr           [ANI_NUM];
	zTModelAniID t_sneakbr_2_sneak   [ANI_NUM];

	zTModelAniID t_walkl_2_aim   [ANI_NUM];
	zTModelAniID t_walkr_2_aim   [ANI_NUM];
	zTModelAniID t_walk_2_aim    [ANI_NUM];
	zTModelAniID s_aim           [ANI_NUM];
	zTModelAniID t_aim_2_walk    [ANI_NUM];
	zTModelAniID t_hitl          [ANI_NUM];
	zTModelAniID t_hitr          [ANI_NUM];
	zTModelAniID t_hitback       [ANI_NUM];
	zTModelAniID t_hitf          [ANI_NUM];
	zTModelAniID s_hitf          [ANI_NUM];
	zTModelAniID t_aim_2_defend  [ANI_NUM];
	zTModelAniID s_defend        [ANI_NUM];
	zTModelAniID t_defend_2_aim  [ANI_NUM];
	zTModelAniID t_paradeL       [ANI_NUM];
	zTModelAniID t_paradeM       [ANI_NUM];
	zTModelAniID t_paradeS       [ANI_NUM];
	zTModelAniID t_hitfrun       [ANI_NUM];

	zTModelAniID t_stumble;
	zTModelAniID t_stumbleb;
	zTModelAniID t_fallen_2_stand;
	zTModelAniID t_fallenb_2_stand;

	zTModelAniID t_walk_2_walkwl;
	zTModelAniID t_walkwl_2_walk;
	zTModelAniID s_walkwl;
	zTModelAniID t_walkwl_2_walkwr;
	zTModelAniID t_walkwr_2_walkwl;
	zTModelAniID s_walkwr;
	zTModelAniID t_walkwr_2_walk;

	zTModelAniID t_walk_2_walkwbl;
	zTModelAniID t_walkwbl_2_walk;
	zTModelAniID s_walkwbl;
	zTModelAniID t_walkwbl_2_walkwbr;
	zTModelAniID t_walkwbr_2_walkwbl;
	zTModelAniID s_walkwbr;
	zTModelAniID t_walkwbr_2_walk;

	zTModelAniID _s_walk;
	zTModelAniID _t_walk_2_walkl;
	zTModelAniID _t_walkl_2_walk;
	zTModelAniID _s_walkl;
	zTModelAniID _t_walkl_2_walkr;
	zTModelAniID _t_walkr_2_walkl;
	zTModelAniID _s_walkr;
	zTModelAniID _t_walkr_2_walk;
	zTModelAniID _t_turnl;
	zTModelAniID _t_turnr;
	zTModelAniID _t_strafel;
	zTModelAniID _t_strafer;

	zTModelAniID _t_walk_2_walkbl;
	zTModelAniID _t_walkbl_2_walk;
	zTModelAniID _s_walkbl;
	zTModelAniID _t_walkbl_2_walkbr;
	zTModelAniID _t_walkbr_2_walkbl;
	zTModelAniID _s_walkbr;
	zTModelAniID _t_walkbr_2_walk;

	zTModelAniID s_jumpstand;
	zTModelAniID t_stand_2_jumpstand;
	zTModelAniID t_jumpstand_2_stand;
	zTModelAniID _t_jumpb;
	zTModelAniID _t_stand_2_jump;
	zTModelAniID _s_jump;
	zTModelAniID t_jump_2_stand;
	zTModelAniID _t_stand_2_jumpup;
	zTModelAniID _s_jumpup;

	zTModelAniID _t_jumpup_2_falldn;
	zTModelAniID _t_jump_2_falldn;

	zTModelAniID t_walkwl_2_swimf;
	zTModelAniID s_swimf;
	zTModelAniID t_swimf_2_walkwl;
	zTModelAniID t_walkwbl_2_swimb;
	zTModelAniID s_swimb;
	zTModelAniID t_swimb_2_walkwbl;
	zTModelAniID t_swimf_2_swim;
	zTModelAniID s_swim;
	zTModelAniID t_swim_2_swimf;
	zTModelAniID t_swim_2_swimb;
	zTModelAniID t_swimb_2_swim;
	zTModelAniID t_warn;
	zTModelAniID t_swim_2_dive;
	zTModelAniID s_dive;
	zTModelAniID t_divef_2_swim;
	zTModelAniID t_dive_2_divef;
	zTModelAniID s_divef;
	zTModelAniID t_divef_2_dive;
	zTModelAniID t_dive_2_drowned;
	zTModelAniID s_drowned;
	zTModelAniID t_swimturnl;
	zTModelAniID t_swimturnr;
	zTModelAniID t_diveturnl;
	zTModelAniID t_diveturnr;

	zTModelAniID _t_walkl_2_aim;
	zTModelAniID _t_walkr_2_aim;
	zTModelAniID _t_walk_2_aim;
	zTModelAniID _s_aim;
	zTModelAniID _t_aim_2_walk;
	zTModelAniID _t_hitl;
	zTModelAniID _t_hitr;
	zTModelAniID _t_hitback;
	zTModelAniID _t_hitf;
	zTModelAniID _t_hitfstep;
	zTModelAniID _s_hitf;
	zTModelAniID _t_aim_2_defend;
	zTModelAniID _s_defend;
	zTModelAniID _t_defend_2_aim;
	zTModelAniID _t_paradeL;
	zTModelAniID _t_paradeM;
	zTModelAniID _t_paradeS;
	zTModelAniID _t_hitfrun;

	zTModelAniID t_stand_2_iaim;
	zTModelAniID s_iaim;
	zTModelAniID t_iaim_2_stand;

	zTModelAniID t_iaim_2_idrop;
	zTModelAniID s_idrop;
	zTModelAniID t_idrop_2_stand;

	zTModelAniID t_iaim_2_ithrow;
	zTModelAniID s_ithrow;
	zTModelAniID t_ithrow_2_stand;

	zTModelAniID t_stand_2_iget;
	zTModelAniID s_iget;
	zTModelAniID t_iget_2_stand;
	zTModelAniID s_oget;

	zTModelAniID _t_stand_2_torch;
	zTModelAniID _s_torch;
	zTModelAniID _t_torch_2_stand;


	zTModelAniID hitani;

	zTModelAniID help;
	zTModelAniID help1;
	zTModelAniID help2;
	zTModelAniID s_fall;
	zTModelAniID s_fallb;
	zTModelAniID s_fallen;
	zTModelAniID s_fallenb;
	zTModelAniID s_falldn;
	zTModelAniID _t_runl_2_jump;
	zTModelAniID _t_runr_2_jump;
	zTModelAniID _t_jump_2_runl;
	zTModelAniID s_look;
	zTModelAniID s_point;
	zTModelAniID dummy1;
	zTModelAniID dummy2;
	zTModelAniID dummy3;
	zTModelAniID dummy4;
	zTModelAniID s_weapon[ANI_NUM];
	zTModelAniID togglewalk;


	zTModelAniID t_stand_2_cast;
	zTModelAniID s_cast;
	zTModelAniID t_cast_2_shoot;
	zTModelAniID t_cast_2_stand;
	zTModelAniID s_shoot;
	zTModelAniID t_shoot_2_stand;

	int dummyLastVar;
};


void oCAniCtrl_Human::Archive(zCArchiver& arc)
{
	zCAIPlayer::Archive(arc);
	if ( arc.InSaveGame() ) {
		arc.WriteObject("aiNpc", npc);
		arc.WriteInt("walkMode", walkmode);
		arc.WriteInt("weaponMode",  weaponmode);
		arc.WriteInt("wmodeLast",   weaponmode_last);
		arc.WriteInt("wmodeSelect", weaponmode_selected);
		arc.WriteBool("changeWeapon", changeweapon);
		arc.WriteInt("actionMode", actionMode);
	}
}

void oCAniCtrl_Human::Unarchive(zCArchiver& arc)
{
	zCAIPlayer::Unarchive(arc);
	if ( arc.InSaveGame() ) {
		npc = arc.ReadObject("aiNpc", 0);

		Init(npc);
		if ( npc )
			npc->Release();

		walkmode = 0;
		actionMode = 0;

		arc.ReadInt("walkMode", walkmode);
		arc.ReadInt("weaponMode",  weaponmode);
		arc.ReadInt("wmodeLast",   weaponmode_last);
		arc.ReadInt("wmodeSelect", weaponmode_selected);
		arc.ReadBool("changeWeapon", changeweapon);
		arc.ReadInt("actionMode", actionMode);
	}
}

void oCAniCtrl_Human::SetFightAnis(int fm)
{
	_t_walk_2_aim = t_walk_2_aim[fm];
	_t_walkl_2_aim = t_walkl_2_aim[fm];
	_t_walkr_2_aim = t_walkr_2_aim[fm];
	_s_aim = s_aim[fm];
	_t_aim_2_walk = t_aim_2_walk[fm];

	if ( fm <= FMODE_2H ) {
		_t_hitl = t_hitl[fm];
		_t_hitr = t_hitr[fm];
		_t_hitf = t_hitf[fm];
		_t_hitfrun = t_hitfrun[fm];
		_t_paradeL = t_paradeL[fm];
	} else if ( fm == FMODE_BOW || fm == FMODE_CBOW ) {
		_t_hitback = t_hitback[fm];
		_t_hitf = t_hitf[fm];
		_s_hitf = s_hitf[fm];
	}

	int guild = npc->GetTrueGuild();

	int *gil_values = 0;
	if ( fm == FMODE_FIST ) {
		gil_values = TGilValues.fight_range_fist;
	} else if ( fm == FMODE_1H ) {
		gil_values = TGilValues.fight_range_1hs;
	} else if ( fm == FMODE_2H )
		gil_values = TGilValues.fight_range_2hs;
	} else {
		return;
	}

	int range = gil_values[guild];
	if (!range && guild < GIL_MAX )
		range = gil_values[GIL_HUMAN];

	if (fm == FMODE_FIST)
		npc->SetFightRangeFist(range);
}

zCModelAniActive* oCAniCtrl_Human::GetLayerAni(int layer)
{
	if ( model->active_ani_num <= 0 )
		return nullptr;

	zCModelAniActive* active = nullptr;
	for (unsigned i = 0; i < model->active_ani_num; ++i) {
		active = model->active_anis[i];
		if (active->ani->layer == layer)
			break;
	}

	return active;
}

void oCAniCtrl_Human::HitInterrupt()
{
	flags.endCombo = 1;
	// flags &= 0xC2; // dunno about that
	lastHitAniFrame = 0;
	model->StopAni(hitAniID);
	hitAniID = -1;
}
