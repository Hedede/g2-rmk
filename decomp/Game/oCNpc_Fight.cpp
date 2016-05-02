zSTRING GetFightMoveStringFromConstant(int move)
{
	switch ( move ) {
	case 0:                 return "NOT INITIALISED";
	case MOVE_RUN:          return "RUN";
	case MOVE_RUNBACK:      return "RUNBACK";
	case MOVE_JUMPBACK:     return "JUMPBACK";
	case MOVE_TURN:         return "TURN";
	case MOVE_STRAFE:       return "STRAFE";
	case MOVE_ATTACK:       return "ATTACK";
	case MOVE_SIDEATTACK:   return "SIDEATTACK";
	case MOVE_FRONTATTACK:  return "FRONTATTACK";
	case MOVE_TRIPLEATTACK: return "TRIPLEATTACK";
	case MOVE_WHIRLATTACK:  return "WHIRLATTACK";
	case MOVE_MASTERATTACK: return "MASTERATTACK";
	case MOVE_PREHIT:       return "PREHIT";
	case MOVE_COMBO:        return "COMBO";
	case MOVE_POSTHIT:      return "POSTHIT";
	case MOVE_TURNTOHIT:    return "TURNTOHIT";
	case MOVE_STORMPREHIT:  return "STORMPREHIT";
	case MOVE_PARADE:       return "STORMPREHIT";
	case MOVE_STANDUP:      return "STANDUP";
	case MOVE_WAIT:         return "WAIT";
	case MOVE_ONGROUND:     return "ONGROUND";
	case MOVE_STUMBLE:      return "STUMBLE";
	default:                return "UNKNOWN";
	}
}

zSTRING GetSituationStringFromConstant(int action)
{
	switch ( action ) {
	case ENEMY_PREHIT:          return "[ENEMY_PREHIT]";
	case ENEMY_STORMPREHIT:     return "[ENEMY_STORMPREHIT]";
	case ENEMY_TURNTOHIT_FOCUS: return "[Enemy_TurnToHit_focus]";
	case MY_W_COMBO:            return "[My_W_Combo]";
	case MY_W_RUNTO:            return "[My_W_RunTo]"; 
	case MY_W_STRAFE:           return "[My_W_Strafe]";
	case MY_W_FOCUS:            return "[My_W_Focus]";
	case MY_W_NOFOCUS:          return "[My_W_noFocus]";
	case MY_G_COMBO:            return "[My_G_Combo]";
	case MY_G_RUNTO:            return "[My_G_Runto]";
	case MY_G_STRAFE:           return "[My_G_Strafe]";
	case MY_G_FOCUS:            return "[My_G_Focus]";
	case MY_G_NOFOCUS:          return "[My_G_FK_noFocus]";
	case MY_FK_FOCUS:           return "[My_FK_Focus]";
	case MY_FK_NOFOCUS:         return "[My_G_FK_noFocus] (2)";
	case MY_FK_FOCUS_FAR:       return "[MY_FK_FOCUS_FAR]";
	case MY_FK_NOFOCUS_FAR:     return "[MY_FK_NOFOCUS_FAR]";
	case MY_FK_FOCUS_MAG:       return "[MY_MAG_NOFOCUS_FAR]";
	case MY_FK_NOFOCUS_MAG:     return "[MY_MAG_NOFOCUS_FAR]";
	default:                    return "[UNKNOWN]";
	}
}


// ------------------------------------
int oCNpc::oSActionBlock::GetOwnAction(int)
{
	if ( numMoves <= 0 )
		return move[0];
	return move[rand() % numMoves];
}

void oCNpc::oSActionBlock::InitBlock(zCParser* parser, int action_id, int fa_id)
{
	static zSTRING FAI_NAMES[] = {
		"ENEMY_PREHIT", "ENEMY_STORMPREHIT", "ENEMY_TURNTOHIT_FOCUS",
		"MY_W_COMBO", "MY_W_RUNTO", "MY_W_STRAFE",
		"MY_W_FOCUS", "MY_W_NOFOCUS",
		"MY_G_COMBO", "MY_G_RUNTO", "MY_G_STRAFE",
		"MY_G_FOCUS", "MY_G_FK_NOFOCUS",
		"MY_FK_FOCUS", "MY_G_FK_NOFOCUS",
		"MY_FK_FOCUS_FAR", "MY_FK_NOFOCUS_FAR",
		"MY_FK_FOCUS_MAG", "MY_FK_NOFOCUS_MAG", 
	};

	memset(this, 0, sizeof(oCNpc::oSActionBlock));

	zSTRING instanceName = "FA_" + FAI_NAMES[action_id] + "_" + zSTRING(fa_id);

	parser->CreateInstance(instanceName, this);

	for (numMoves = 0; numMoves < 6; ++numMoves)
		if (!move[numMoves])
			break;
	}
}

// ------------------------------------
struct oCNpc::oSFightAI {
	oCNpc::oSActionBlock actions[19];

	void Init(zCParser* parser, int fa_id)
	{
		size_t i = 0;
		for (auto& action : actions) {
			action.InitBlock(parser, i++, fa_id);
		}
	}

	int GetOwnAction(int situation)
	{
		return actions[situation].GetOwnAction(situation);
	}
};


// ------------------------------------
int oCNpc::GetFightRangeDynamic() const
{
	oCItem* weap = GetWeapon();
	return weap ? weap->range : fightRangeFist;
}

int oCNpc::GetFightRange() const
{
	int baseRange = fightRangeBase;
	int weapRange = GetFightRangeDynamic();

	return baseRange + weapRange;
}

int oCNpc::GetFightActionFromTable(int situationNr)
{
	auto& action = FAIList[fighttactic].actions[situationNr];
	return action.GetOwnAction();
}

int oCNpc::GetCurrentFightMove()
{
	zCModel *model = GetModel();
	oCAniCtrl_Human *anictrl = GetAnictrl();

	if ( !model || !anictrl )
		return MOVE_WAIT;

	zCModelAni* ani;

	ani = model->GetAniFromAniID(anictrl->_t_turnl);
	if (model->IsAniActive(ani))
		return MOVE_TURN

	ani = model->GetAniFromAniID(anictrl->_t_turnr);
	if (model->IsAniActive(ani))
		return MOVE_TURN

	if ( anictrl->IsWalking() ) {
		ani = model->GetAniFromAniID(anictrl->_t_hitfrun);
		if (model->IsAniActive(ani))
			return MOVE_RUN_HIT_;
		return MOVE_RUN;
	}

	if ( anictrl->IsStanding() )
		return MOVE_WAIT;

	ani = model->GetAniFromAniID(anictrl->_s_walkbl);
	if (model->IsAniActive(ani))
		return MOVE_RUNBACK;

	ani = model->GetAniFromAniID(anictrl->_t_walkbl_2_walk);
	if (model->IsAniActive(ani))
		return MOVE_RUNBACK;

	ani = model->GetAniFromAniID(anictrl->_t_walkbr_2_walk);
	if (model->IsAniActive(ani))
		return MOVE_RUNBACK;

	ani = model->GetAniFromAniID(anictrl->_t_strafel);
	if (model->IsAniActive(ani))
		return MOVE_STRAFE;

	ani = model->GetAniFromAniID(anictrl->_t_strafer);
	if (model->IsAniActive(ani))
		return MOVE_STRAFE;

	ani = model->GetAniFromAniID(anictrl->_t_jumpb);
	if (model->IsAniActive(ani))
		return MOVE_JUMPBACK;

	int bodyState = GetBodyState();
	if (bodyState == BS_STUMBLE)
		return MOVE_STUMBLE;

	if ( GetBodyState() == BS_LIE )
		return MOVE_ONGROUND;

	if ( anictrl->IsParadeRunning() )
		return MOVE_PARADE;

	ani = model->GetAniFromAniID(anictrl->_t_hitl);
	if ( model->IsAniActive(ani) )
		return anictrl->IsInPreHit() != 0 ? MOVE_PREHIT_ : MOVE_POSTHIT_;

	ani = model->GetAniFromAniID(anictrl->_t_hitr);
	if ( model->IsAniActive(ani) )
		return anictrl->IsInPreHit() != 0 ? MOVE_PREHIT_ : MOVE_POSTHIT_;

	if ( anictrl->IsInPreHit() )
		return MOVE_PREHIT;

	if ( anictrl->IsInPostHit() )
		return MOVE_POSTHIT;

	return anictrl->IsInCombo() != 0 ? MOVE_COMBO : MOVE_WAIT;;
}

int oCNpc::FindNextFightAction()
{
	if ( !enemy )
		return MOVE_WAIT;

	if ( !FreeLineOfSight(enemy) )
		return this->fighttactic != 1 ? MOVE_RUN : MOVE_TURN; // FAI_NAILED?


	oSFightAI fai = FAIList.array[fighttactic];

	int enemyInFocus = IsInFightFocus(enemy);

	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if ( lastAction == MOVE_WAIT_EXT && fight_waitTime > 0.0 )
			return MOVE_WAIT_EXT;

		int action = enemyInFocus ? MY_FK_FOCUS_MAG : MY_FK_NOFOCUS_MAG;

		return fai.GetOwnAction(action);
	}

	if ( GetWeaponMode() >= FMODE_FAR )
	{
		if ( lastAction == MOVE_WAIT_EXT && fight_waitTime > 0.0 )
			return MOVE_WAIT_EXT;

		int action = enemyInFocus ? MY_FK_FOCUS_FAR : MY_FK_NOFOCUS_FAR;

		return fai.GetOwnAction(action);
	}


	float range = 0.0;

	int inEnemyRange = enemy->IsInFightRange(this, &range);
	int inEnemyFocus = enemy->IsInFightFocus(this);

	int enemyInRange = IsInFightRange(enemy, &range);

	int myRange = GetFightRange() + fightRangeG;

	bool inRange = range >= (double)myRange;
	bool faraway = range >= 3000.0;

	float angle1;
	float angle2;
	GetAngles(enemy, angle1, angle2);
	bool inAngle = !(fabs(angle1) >= 90.0 || fabs(angle2) >= 50.0);

	int curMove = GetCurrentFightMove();
	int enemyMove = oCNpc::GetCurrentFightMove(enemy);
	int myMove = MOVE_UNDECIDED;

	if (inRange && !enemyInRange && IsNpcBetweenMeAndTarget(enemy))
		return MOVE_STRAFE;

	int action = 0;
	while ( 1 ) {
		switch ( action ) {
		case ENEMY_PREHIT:
			if ( inEnemyRange && inEnemyFocus && inAngle && enemyMove == MOVE_PREHIT )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case ENEMY_STORMPREHIT:
			if ( inEnemyRange && inEnemyFocus && inAngle && enemyMove == MOVE_STORMPREHIT )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case ENEMY_TURNTOHIT_FOCUS:
			if ( enemyInFocus && inEnemyRange && enemyMove == MOVE_TURNTOHIT )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_W_COMBO:
			if ( enemyInRange && enemyInFocus && curMove == MOVE_COMBO )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_W_RUNTO:
			if ( enemyInRange && enemyInFocus && curMove == MOVE_RUN )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_W_STRAFE:
			if ( enemyInRange && enemyInFocus && curMove == MOVE_STRAFE )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_W_FOCUS:
			if ( enemyInRange && enemyInFocus )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_W_NOFOCUS:
			if ( enemyInRange )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_G_COMBO:
			if ( inRange && enemyInFocus && curMove == MOVE_COMBO )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_G_RUNTO:
			if ( inRange && enemyInFocus && curMove == MOVE_RUN )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_G_STRAFE:
			if ( inRange && enemyInFocus && curMove == MOVE_STRAFE )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_G_FOCUS:
			if ( inRange && enemyInFocus )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_G_NOFOCUS:
			if ( inRange )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_FK_FOCUS:
			if ( faraway && enemyInFocus )
			{
				myMove = fai.GetOwnAction(action);
			}
			break;
		case MY_FK_NOFOCUS:
			if ( faraway )
			{
				myMove = fai.GetOwnAction(action);
			}
		default:
			break;
		}

		++action;

		if (action > ENEMY_TURNTOHIT_FOCUS) {
			if (fight_waitForAniEnd != -1)
				break;

			if (fight_waitTime > 0.0 && myMove == MOVE_UNDECIDED)
				return MOVE_WAIT;
		}

		if (action > MY_FK_NOFOCUS) {
			action = 0;
			myMove = fighttactic != 1 ? MOVE_RUN : MOVE_TURN;
		}

		if ( myMove != MOVE_UNDECIDED ) {
			int debug = zoptions.Parm("DEBUGFIGHTAI");
			if (!debug)
				return myMove;

			zINFO(5, "C: FIGHT:                                 ");
			zINFO(5, "C: FIGHT: enemyMove:" +
			      GetFightMoveStringFromConstant(enemyMove));
			zINFO(5, "C: FIGHT: curMove:" +
			      GetFightMoveStringFromConstant(curMove));
			zINFO(5,"C: FIGHT: Fight-Instance selected: " +
			      GetSituationStringFromConstant(action - 1));
			zINFO(5,"C: FIGHT: myMove:" +
			      GetFightMoveStringFromConstant(myMove));
			zInfo(5,"C: FIGHT: tactic:" + zSTRING(fighttactic));

			return myMove;
		}
	}

	if ( myMove != MOVE_UNDECIDED )
		fight_waitForAniEnd = -1;
	return myMove;
}

void oCNpc::ExitFightAI()
{
	for (auto ptr : fightAI)
		delete ptr;

	fightAI.DeleteList();
}

int oCNpc::InitAim(oCMsgAttack* csg, oCNpc** enemy, int* drawn, int* ammo, int killFormerMsg)
{
	if ( enemy ) {
		*enemy = zDYNAMIC_CAST<oCNpc>(csg->paramVob);

		if (!*enemy) {
			*enemy = this->enemy;
		}

		if ( (*enemy)->attribute[ATR_HITPOINTS] <= 0 )
			*enemy = 0;
	}

	if ( drawn ) {
		auto fm = GetWeaponMode();
		*drawn = fm == FMODE_BOW || fm == FMODE_CBOW;
	}

	if ( ammo ) {
		oCItem* item = nullptr;
		if ( fmode > FMODE_FIST ) {
			TNpcSlot* slot;
			if ( fmode > FMODE_2H ) {
				if ( fmode == FMODE_CBOW )
					slot = GetInvSlot(NPC_NODE_RIGHTHAND);
				else
					slot = GetInvSlot(NPC_NODE_LEFTHAND);
			} else {
				slot = GetInvSlot(NPC_NODE_RIGHTHAND);
			}

			if ( slot )
				item = zDYNAMIC_CAST<oCItem>(slot->object);
		}

		*ammo = IsMunitionAvailable(item);
	}

	if ( killFormerMsg ) {
		for (unsigned i = 0; i < GetEM()->GetNumMessages(); ++i) {
			auto msg = GetEM()->GetEventMessage(i);
			auto msgatt = zDYNAMIC_CAST<oCMsgAttack>(msg);
			if (msg != csg && csg->subType == msgatt->subType) {
				msg->Delete();
			}
		}
	}
	return 1;
}

int oCNpc::FinalizeAim(int startMelee, int standUp)
{
	bool ret = 0;
	for (unsigned i = 0; i < GetEM()->GetNumMessages(); ++i) {
		auto msg = zDYNAMIC_CAST<oCMsgAttack>(GetEM()->GetEventMessage(i));
		if (msg && msg->subType == EV_AIMAT) {
			msg->Delete();
			ret = 1;
		}
	}

	if ( startMelee ) {
		auto msg = new oCMsgWeapon(EV_REMOVEWEAPON, 0, 0);
		msg->SetHighPriority(1);
		GetEM()->OnMessage(msg, this);
	}

	if ( standUp )
		StandUp(0, 1);

	return ret;
}
