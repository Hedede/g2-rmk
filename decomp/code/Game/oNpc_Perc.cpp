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

void oCNpc::SetPerceptionTime(float time)
{
	percActiveDelta = time;
	while (percActiveTime > percActiveDelta) {
		percActiveTime -= percActiveDelta;
	}
}

void oCNpc::DisablePerception(int percType)
{
	/* weird, might be broken?  */
	for (unsigned i = 0; i < percActive; ++i) {
		if ( percList[i].percID == percType ) {
			percList[i].percID = 0;
			percList[i].percFunc = -1;
			if ( i == percActive - 1 )
				--percActive;
		}
	}
}

void oCNpc::CreateSoundPerception(
int percType, zCVob* source, zVEC3 const& position, zCVob* victimVob, int setVictim)
{

	float r = percRange[percType];
	zTBBox3D bbox;

	bbox.mins = position;
	bbox.maxs = position + r;

	zCArray<zCVob> vobList;
	auto world = ogame->GetGameWorld();

	world->bspTree.bspRoot.CollectVobsInBBox3D(&vobList, &bbox);

	oCNpc* sourceNpc = zDYNAMIC_CAST<oCNpc>(source);
	int movmode = 0;
	if ( sourceNpc ) {
		movmode = sourceNpc->isInMovementMode & 3
		if (movmode != 0)
			sourceNpc->EndMovement(1);
	}

	for (auto vob : vobList) {
		if (vob == source)  // I moved it out of if (func >= 0...)
			continue; // 'cos too many conditions

		auto npc = zDYNAMIC_CAST<oCNpc>(vob);
		if ( npc ) {
			auto func = npc->GetPerceptionFunc();
			if (func >= 0 && !npc->IsDead() && !npc->IsAPlayer()) {
				if ( setVictim ) {
					oCNpc* victim = zDYNAMIC_CAST<oCNpc>(victimVob);
					zparser.SetInstance("VICTIM", victim);
				}

				oCNpc* other = zDYNAMIC_CAST<oCNpc>(source);
				zparser.SetInstance("OTHER", other);

				oCItem* item = zDYNAMIC_CAST<oCItem>(source);
				zparser.SetInstance("ITEM", item);

				// Check for inlined
				if ( !npc->IsSelfPlayer() ) {
					if ( source && !npc->vobList.IsInList(source)) {
						npc->vobList.InsertEnd(source);
						source->AddRef();
						++vob->refCtr;
					}
				}

				npc->soundType = 0;
				npc->soundVob = source;
				if ( source ) {
					auto pos = source->trafoObjToWorld.GetTranslation();
					npc->soundPosition = pos;
				} else {
					npc->soundPosition = position;
				}

				npc->states.StartAIState(func, 0, 0, 0.0, 0);
			}
		}
	}

	if ( sourceNpc && movmode )
		sourceNpc->BeginMovement();
}

int oCNpc::AssessRemoveWeapon_S(oCNpc *other)
{
	oCNpc::CreatePassivePerception(PERC_ASSESSREMOVEWEAPON, this, 0);
	return 1;
}

int oCNpc::AssessTheft_S(oCNpc* other)
{
	CreatePassivePerception(PERC_ASSESSTHEFT, other, this);
	return 1;
}

int oCNpc::AssessFakeGuild_S()
{
	if ( IsSelfPlayer() ) {
		CreatePassivePerception(PERC_ASSESSFAKEGUILD, this, 0);
		return 1;
	}

	return 0;
}

int oCNpc::AssessCaster_S(oCSpell* spell)
{
	CreatePassivePerception(PERC_ASSESSCASTER, this, 0);
	return 1;
}

int oCNpc::AssessEnterRoom_S(oCNpc *this)
{
	CreatePassivePerception(PERC_ASSESSENTERROOM, this, 0);
	return 1;
}

int oCNpc::AssessWarn_S(zCVob* commiter)
{
	CreatePassivePerception(PERC_ASSESSWARN, commiter, 0);
	return 1;
}

int oCNpc::AssessDefeat_S(zCVob* commiter)
{
	CreatePassivePerception(PERC_ASSESSDEFEAT, commiter, this);
	return 1;
}

int oCNpc::AssessMurder_S(zCVob* commiter)
{
	CreatePassivePerception(PERC_ASSESSMURDER, commiter, this);
	return 1;
}

int oCNpc::AssessOthersDamage_S(zCVob* commiter, zCVob* victim, int)
{
	CreatePassivePerception(PERC_ASSESSOTHERSDAMAGE, commiter, victim);
	return 1;
}

int oCNpc::AssessQuietSound_S(zCVob* source, zVEC3 const& position)
{
	g_bDontKillOUSound = 1;
	CreateSoundPerception(PERC_ASSESSQUIETSOUND, source, position, 0, 0);
	g_bDontKillOUSound = 0;
	return 1;
}


int oCNpc::AssessFightSound_S(zCVob* source, zVEC3 const& position, zCVob* victimVob)
{
	CreateSoundPerception(PERC_ASSESSFIGHTSOUND, source, position, victimVob, 1);
	return 1;
}

int oCNpc::ObserveIntruder_S()
{
	int movmode = isInMovementMode & 3;
	if (movmode)
		EndMovement(1);

	CreatePassivePerception(PERC_OBSERVEINTRUDER, this, 0);

	if (movmode)
		BeginMovement(this);

	return 1;
}

int oCNpc::AssessUseMob_S(oCMobInter* useMob)
{
	if ( useMob ) {
		if ( IsSelfPlayer() ) {
			rbt.obstVob = useMob;
			CreatePassivePerception(PERC_ASSESSUSEMOB, this, 0);
			return 1;
		}
	}
	return 0;
}
