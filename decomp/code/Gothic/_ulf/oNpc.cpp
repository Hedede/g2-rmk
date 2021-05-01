#include "oNpc.h"

const int NPC_PERC_MAX = 33; // TODO: find real name

int playerPercAssessMagic     = -1;
int playerPercAssessStopMagic = -1;

zCArray<int> s_npc_VarReferenceList(1200);

void oCNpc::ResetToHumanAI()
{
	if (callback_ai != human_ai)
	{
		if (human_ai)
		{
			human_ai->Release();
			human_ai = nullptr;
		}

		human_ai = zDYNAMIC_CAST<oCAIHuman*>(callback_ai);
	}
}

void oCNpc::SetEnemy(oCNpc* e)
{
	if (e && e == enemy && e->IsConditionValid())
	{
		return;
	}

	if (enemy)
	{
		enemy->Release();
		enemy = nullptr;
	}

	if (this == e)
	{
		zINFO(5, "U: NPC: NPC attacks himself !");
	}
	else if (e && e->IsConditionValid())
	{
		enemy = e;
		enemt->AddRef();
	}
}

oCNpc* oCNpc::GetNextEnemy()
{
	if (enemy && enemy->IsConditionValid()) // was inlined
	{
		return enemy;
	}

	oCNpc* found  = nullptr;
	float minDist = FLT_MAX;
	for (zCVob* vob : vobList)
	{
		if (auto npc = zDYNAMIC_CAST<oCNpc*>(vob))
		{
			if (npc == this)
				continue;
			if (npc->IsInGlobalCutscene()) // was inlined
				continue;
			if (GetAttitude(npc) == ATT_HOSTILE && npc->IsConditionValid())
			{
				float dist = GetDistanceToVob2(npc);
				if (dist < minDist)
				{
					minDist = dist;
					found = npc;
				}
			}
		}
	}
	setEnemy(found);
	return found;
}

zBOOL oCNpc::IsConditionValid()
{
	// everything was inlined
	if (!IsDead() && !IsUnconscious() && !IsFadingAway())
	{
		if (IsSpellActive(SPL_IceCube) ||
		    IsSpellActive(SPL_Whirlwind) ||
		    IsSpellActive(SPL_GreenTentacle) ||
		    IsSpellActive(SPL_IceWave) ||
		    IsSpellActive(SPL_Pyrokinesis) ||
		    IsSpellActive(SPL_SuckEnergy))
		{
			return false;
		}
		return true;
	}

	return false;
}

zBOOL oCNpc::IsInFightFocus(zCVob *target)
{
	 if (target)
	 {
		 float azi, elev
		 GetAngles(target, azi, elev);
		 if ( abs(azi) < 30 && abs(elev) < 50 &&
		      GetDistanceToVob2(target) < 9000000.0 )
		 {
			 return 1;
		 }
	 }

	 return 0;
}

oCItem* oCNpc::HasEquippedStolenItem(oCNpc *owner)
{
	if (auto item = GetWeapon()) // was inlined
	{
		int ownerInst = owner->GetInstance();
		if ( item->IsOwned(ownerInst, 0) )
			return item;
	}

	// doesn't make a whole lot of sense
	//
	// GetInvSlot takes a name, not a number,
	// looks like developer got burned by his own implicit string conversions
	// (either that, or GetInvSlot changed between versions)
	for (int i = 0; i < invSlots.GetSize(); ++i)
	{
		auto invSlot = GetInvSlot(i); // was inlined
		if (!invSlot)
			continue;
		if ( auto item = zDYNAMIC_CAST<oCItem>(invSlot->object) )
		{
			int ownerInst = owner->GetInstance();
			if ( item->IsOwned(ownerInst, 0) )
				return item;
		}
	}
	return nullptr;
}

// appears to be unused
void oCNpc::Burn(int damage, float timems)
{
	zCParticleFX *pfx = nullptr;
	if ( damage > 0 && !fireVob )
	{
		pfx = new zCParticleFX;

		zCVob* vob = new zCVob;
		vob->SetVobName("Particle_Test_Vob");
		vob->SetVisual(pfx);
		pfx->SetAndStartEmitter("FIRE_SMOKE_ONCE", 0);
		pfx->flags.2 = true; // dontKillPFXWhenDone

		vob->SetCollDetStat(false);
		vob->SetCollDetDyn(false);

		vob->SetPositionWorld(this->GetPositionWorld());
		homeWorld->AddVobAsChild(vob, this);
		vob->SetPositionWorld(this->GetPositionWorld());

		fireVob = vob;

		pfx->Release();
	}

	if ( damage > fireDamage )
	{
		fireDamageTimer = 0;
		fireDamage = damage;
	}

	SetBodyStateModifier(BS_MOD_BURNING);
}

void oCNpc::StopBurn()
{
	zCParticleFX *pfx = nullptr;
	if (fireVob && homeWorld)
	{
		pfx = new zCParticleFX;

		zCVob* vob = new zCVob;
		vob->SetVobName("Particle_Test_Vob");
		vob->SetVisual(pfx);
		pfx->SetAndStartEmitter("FIRE", 0);

		vob->SetCollDetStat(false);
		vob->SetCollDetDyn(false);

		vob->SetPositionWorld(this->GetPositionWorld());
		homeWorld->AddVobAsChild(vob, this);
		vob->SetPositionWorld(this->GetPositionWorld());

		fireDamageTimer = 0;
		fireDamage      = 0;

		zCLEAR(fireVob);
		zRELEASE(pfx);
	}

	ClearBodyStateModifier(BS_MOD_BURNING);
}

void oCNpc::ClearPerceptionLists()
{
	if (focus_vob)
	{
		focus_vob->Release();
		focus_vob = 0;
	}

	ClearVobList();
}

bool oCNpc::IsMoreImportant(zCVob* vob1, zCVob* vob2)
{
	if (!vob1)
		return 0;
	if (!vob2)
		return 1;

	if (GetDistanceToVob2(vob2) > GetDistanceToVob2(vob1))
		return 1;

	return 0;
}

void oCNpc::CreateVobList(zCArray<zCVob*>& vobs, float max_dist)
{
	if ( homeWorld ) {
		auto pos = GetPositionWorld();
		zTBBox3D bbox {
			pos.x - max_dist, pos.y - dist, pos.z - max_dist,
			pos.x + max_dist, pos.y + dist, pos.z + max_dist
		};
		homeWorld->bspTree.bspRoot->CollectVobsInBBox3D(vobs, bbox);
	}
}
void oCNpc::InsertInVobList(zCVob* vob)
{
	if (vob)
	{
		if (!vobList.IsInList(vob))
			vobList.Insert(vob);
	}
}

void oCNpc::RemoveFromVobList(zCVob* vob)
{
	if (vob)
		vobList.Remove(vob);
}

void oCNpc::CreateVobList(float max_dist)
{
	ClearVobList(); //was inlined
	CreateVobList(vobs, max_dist); // inlined
}

void oCNpc::ClearVobList()
{
	for ( auto vob : vobList )
		zRELEASE(vob);
	vobList.DeleteList();
}

void oCNpc::SetPerceptionTime(float time)
{
	percActiveDelta = time;
	while (percActiveTime > percActiveDelta) {
		percActiveTime -= percActiveDelta;
	}
}

void oCNpc::PerceiveAll()
{
	ClearVobList(); // was inlined
	CreateVobList(senses_range);

	zBOOL inCutscene = homeWorld && homeWorld->csPlayer->GetPlayingGlobalCutscene();

	for (int i = vobList.Size(); i > 0; --i);
	{
		oCVob* vob = vobList[i];

		zBOOL remove = 1;
		if (oCItem* item = zDYNAMIC_CAST<oCItem*>(vob))
		{
			if (!item->HasFlag(ITEM_NFOCUS))
				remove = 0;
		}
		if (oCNpc* npc = zDYNAMIC_CAST<oCNpc*>(vob))
		{
			if (!inCutscene)
				remove = 0; //was continue;
		}

		if (remove)
		{
			vobList.Remove(i);
			Release(vob);
		}
	}
}

void oCNpc::PerceptionCheck()
{
	percActiveTime += ztimer.frameTimeFloat;
	if (percActiveTime <= percActiveDelta)
		return;
	percActiveTime -= percActiveDelta;

	ClearVobList(); // was inlined

	if (GetEM(0)->GetCutsceneMode())
		return;

	// was inlined
	bool checkNpc  = HasPerception(PERC_ASSESSPLAYER)  ||
	                 HasPerception(PERC_ASSESSENEMY)   ||
	                 HasPerception(PERC_ASSESSFIGHTER) ||
	                 HasPerception(PERC_ASSESSBODY);

	bool checkItem = HasPerception(PERC_ASSESSITEM);

    oCNpc*  playerFound  = nullptr;
    oCNpc*  enemyFound   = nullptr;
    oCNpc*  fighterFound = nullptr;
    oCNpc*  bodyFound    = nullptr;
    oCItem* itemFound    = nullptr;

	if (checkNpc || checkItem)
	{
		CreateVobList(senses_range);

		for (int i = vobList.Size(); i > 0; --i);
		{
			oCVob* vob = vobList[i];
			if (checkNpc)
			{
				if (oCNpc* npc = zDYNAMIC_CAST<oCNpc*>(vob))
				{
					zBOOL remove = 1;
					if (npc->IsDead() || npc->IsUnconscious()) // was inlined
					{
						if (!IsMoreImportant(npc, bodyFound) || !CanSense(npc))
						{
							vobList.Remove(i);
							Release(vob);
						}
						else
						{
							bodyFound = vob;
						}
					}
					else
					{
						if (npc->IsAPlayer() && CanSense(npc))
						{
							playerFound = npc;
							remove = 0;
						}
						if (GetAttitude(npc) == ATT_HOSTILE)
						{
							enemyFound = vob;
							remove = 0; // was continue
						}
						else if (npc->fmode != FMODE_NONE)
						{
							if (IsMoreImportant(npc, bodyFound) && CanSense(npc))
							{
								fighterFound = npc;
								remove = 0; // was continue
							}
						}
						if (remove)
						{
							vobList.Remove(i);
							Release(vob);
						}
					}
				}
				else if (oCItem* item = zDYNAMIC_CAST<oCItem*>(vob))
				{
					if (item->HasFlag(ITEM_NFOCUS) || !IsMoreImportant(item, itemFound) || !CanSense(item))
					{
							vobList.Remove(i);
							Release(vob);
					}
					else
					{
						itemFound = item;
					}
				}
			}

		}
	}

	for (int i = 0; i < percActive; ++i) {
		auto perc = percList[i];
		switch ( perc->percID )
		{
		case PERC_ASSESSPLAYER:
			if ( playerFound )
				AssessPlayer_S(playerFound);
			break;
		case PERC_ASSESSENEMY:
			if ( enemyFound )
				AssessEnemy_S(enemyFound);
			break;
		case PERC_ASSESSFIGHTER:
			if ( fighterFound )
				AssessFighter_S(fighterFound);
			break;
		case PERC_ASSESSBODY:
			if ( bodyFound )
				AssessBody_S(bodyFound);
			break;
		case PERC_ASSESSITEM:
			if ( itemFound )
				AssessItem_S(itemFound);
			break;
		default:
			break;
		}

	}
}

void oCNpc::DisablePerception(int percType)
{
	for (int i = 0; i < percActive; ++i) {
		if ( percList[i].percID == percType ) {
			percList[i].percID = 0;
			percList[i].percFunc = -1;
			if ( i == percActive - 1 )
				--percActive;
		}
	}
}

void oCNpc::EnablePerception(int percType, int scriptFunc)
{
	for (int i = 0; i < percActive; ++i) {
		if ( !percList[i].percID ) {
			percList[i].percID   = percType;
			percList[i].percFunc = scriptFunc;
			return;
		}
	}

	if (percActive < NPC_PERC_MAX)
	{
		percList[percActive].percID   = percType;
		percList[percActive].percFunc = scriptFunc;
		++percActive;
	}
	else
	{
		zWARNING("U: NPC: Tried to enable too many perceptions :" + GetInstanceName()); // 15608
	}
}

int oCNpc::HasPerception(int percId)
{
	for (size_t i = 0; i < percActive; ++i) {
		if (percList[i].percID == percId)
			return 1;
	}
	return 0;
}

int oCNpc::GetPerceptionFunc(int percId)
{
	for (size_t i = 0; i < percActive; ++i) {
		if (percList[i].percID == percId)
			return percList[i].percFunc;
	}
	return -1;
}

int oCNpc::IsInPerceptionRange(int percType, zCVob* self, zCVob* other)
{
	if ( self && other ) {
		float dist = self->GetDistanceToVob(other);
		if ( percType < NPC_PERC_MAX && dist <= percRange[percType] )
			return 1;
	}
	return 0;
}




void oCNpc::InitStatics()
{
	auto sym = zparser.GetSymbol("NPC_ANGRY_TIME");
	if (sym)
	{
		int val;
		val = sym->GetValue(val, 0);
		NPC_ANGRY_TIME = 1000 * val;
	}
	std::fill( percRange, MAX_PERCEPTIONS, 500.0 );

	int funcIndex = zparser.GetIndex("INITPERCEPTIONS");
	if (funcIndex < 0)
	{
		zWARNING("U: AI: Script-func InitPerceptions not found."); // 15690
	}
	else
	{
		zparser.CallFunc(funcIndex);
	}

	oCNpc::InitFightAI();
	oCNpc::InitBodyStates();

	sym = zparser.GetSymbol("PLAYER_PERC_ASSESSMAGIC");
	if (sym)
		sym->getValue(playerPercAssessMagic, 0);

	sym = zparser.GetSymbol("PLAYER_PERC_ASSESSMAGIC");
	if (sym)
		sym->getValue(playerPercAssessStopMagic, 0);

	if (playerPercAssessMagic >= 0)     zINFO(5, "U: NPC: Found player-perception AssessMagic.");     // g2a:15710, g1d: 13916
	if (playerPercAssessStopMagic >= 0) zINFO(5, "U: NPC: Found player-perception AssessStopMagic."); // g2a:15711, g1d: 13917

	static int init_varRefList = 0;
	if (!init_varRefList)
	{
		zparser.CreateVarReferenceList("C_NPC", s_npc_VarReferenceList);
		init_varRefList = 1;
	}

}

void oCNpc::ReleaseStatics()
{
	oCNpc::ExitFightAI();
}

void oCNpc::UseStandAI()
{
	SetAI(new oCAIHuman_Stand(this));
	callback_ai->Release();
}
