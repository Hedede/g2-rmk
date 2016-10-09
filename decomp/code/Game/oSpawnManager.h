//_ulf/oSpawn.cpp
// struct oSSpawnNode
struct oTSpawnNode {
	oCNpc *   npc;
	zVEC3     spawnPos;
	zREAL     timer;
};

float SPAWN_INSERTTIME_MAX = 1000.0;
float SPAWN_INSERTRANGE = 5000.0;
float SPAWN_REMOVERANGE = 4500.0;

struct oCSpawnManager {
	static float GetSpawnTime() { return SPAWN_INSERTTIME_MAX; }
	static float GetInsertRange() { return SPAWN_INSERTRANGE; }
	static float GetSpawnTime() { return SPAWN_REMOVERANG; }

	static void SetSpawnTime(float v)   { SPAWN_INSERTTIME_MAX = v; }
	static void SetInsertRange(float v) { SPAWN_INSERTRANGE = v; }
	static void SetSpawnTime(float v)   { SPAWN_REMOVERANG = v; }

	oCSpawnManager() = default;
	~oCSpawnManager() { ClearList(); }

	void Archive(zCArchiver& arc);
	void Unarchive(zCArchiver& arc);

	bool IsSpawningEnabled() const
	{
		return spawningEnabled;
	}

	void SetSpawningEnabled(zBOOL b)
	{
		spawningEnabled = b;
	}

	oCNpc* SpawnNpc(int inst, zSTRING const& spawnPoint, float timer);

	void ClearList()
	{
		for (auto* node : spawnList) {
			node->npc->Release();
			delete node;
		}
		spawnList.Clear();
	}

private:
	void InsertInList(oTSpawnNode* node);
	{
		if ( node ) node->npc->AddRef();
		spawnList.InsertEnd(node);
	}

	void RemoveFromList(oTSpawnNode *node)
	{
		if ( node ) node->npc->Release();

		spawnList.Remove(node):
		delete node;
	}


private:
	zBOOL CanDeadNpcBeRemoved(oCNpc *npc);
	zBOOL UseDeadNpcRoutinePos(oCNpc *npc)

	bool InsertNpc(oCNpc *npc, zVEC3 const& spawnPos)

	void InitCameraPos()
	{
		oCVob* = oCNpc::player;
		if (auto aicam = zCAICamera::GetCurrent())
			vob = aicam->__csVob; // TODO
		if ( vob )
			camPos = vob->GetPostionWorld();
	}

	void CheckForInvalidDialogCamPos(float dist, oCNpc *npc);

private:
	zCArray<oTSpawnNode*> spawnList;

	zBOOL  spawningEnabled = true;
	zVEC3  camPos;
	//Verzögerungszeit des Spawnmanagers (Performancegründe)
	zREAL  insertTime = SPAWN_INSERTTIME_MAX;

	//war mal ne Kopierschutz Sache, böse Raubkopierer
	//hatten mit gepimpten immortal Flags zu kämpfen. jetzt ungenutzt.
	int  spawnFlags = 0;
};

//------------------------------------------------------------------------------
void oCSpawnManager::Archive(zCArchiver& arc)
{
	if (arc.InSaveGame()) {
		arc.WriteInt(arc, "NoOfEntries", spawnList.numInArray);

		int num = spawnList.GetNumInList();
		for (int i = 0; i < num; ++i) {
			arc.WriteObject("npc", spawnList[i]->npc);
			arc.WriteVec3("spawnPos", spawnList[i]->spawnPos);
			arc.WriteFloat("timer", spawnList[i]->timer);
		}
		arc.WriteBool("spawningEnabled", spawningEnabled);
		arc.WriteInt("spawnFlags", spawnFlags);
	}
}

void oCSpawnManager::Unarchive(zCArchiver& arc)
{
	if (arc.InSaveGame() ) {
		int noEntries = 0;
		arc->ReadInt("NoOfEntries", noEntries);

		if ( noEntries > 0 ) {
			auto node = new oTSpawnNode();

			auto npc = arc.ReadObject("npc", node->npc);
			if (!npc) {
				noEntries = -1;
				break;
			}

			arc.ReadVec3("spawnPos", node->pos);
			arc.ReadFloat("timer", node->timer);
			spawnList.Insert(node);
		}
		if ( noEntries < 0 ) {
			arc.spawningEnabled = 1;
			arc.spawnFlags = 0;
		} else {
			arc.ReadBool("spawningEnabled", spawningEnabled);
			arc.ReadInt("spawnFlags", spawnFlags);
		}
	}
}



//------------------------------------------------------------------------------
oCNpc* oCSpawnManager::SpawnNpc(int inst, zSTRING const& spawnPoint, float timer)
{
	oCWorld* wld = ogame->GetGameWorld();

	// Создание NPC
	auto npc = zDYNAMIC_CAST<oCNpc>(wld->CreateVob(VOB_TYPE_NPC, inst));
	if (!npc)
		return nullptr;

	// Вставка NPC в мир
	SpawnNpc(npc, spawnPoint, timer);
	npc->Release();

	return result;
}

void oCSpawnManager::SpawnNpc(oCNpc* npc, zVEC3 const& spawnPos, float timer)
{
	if ( !npc )
		return;

	if (!npc->states.hasRoutine || npc->IsDead()) {
		if ( npc->states.IsAIStateDriven() )
			npc->states.InitAIStateDriven(spawnPos);

		auto spawnNode = new oSSpawnNode;
		spawnNode->npc = npc;
		spawnNode->spawnPos = spawnPos;
		spawnNode->timer = timer;

		InsertInList(spawnNode);
	}
}

// spawnPoint -> spawnVobName
void oCSpawnManager::SpawnNpc(oCNpc* npc, zSTRING const& spawnPoint, float timer)
{
	zCWorld* world = ogame->GetWorld();
	zCWaypoint* wp = world->wayNet->GetWaypoint(spawnPoint);
	zVEC3 spawnPos;

	if ( wp ) {
		spawnPos = wp->GetPositionWorld();
	} else {
		oCVob* vob = world->SearchVobByName(spawnPoint);
		if ( !vob ) {
			zSTRING npcName = npc->GetInstanceName();

			zWARNING("U: SPAWN: Spawnpoint " + spawnPoint + " not found. Npc " + npcName + " cannot be spawned.");
			return;
		}

		spawnPos = vob->GetPositionWorld();
	}

	npc->spawnPoint = spawnPoint;
	SpawnNpc(npc, spawnPos, timer);
}


bool oCSpawnManager::InsertNpc(oCNpc *npc, zVEC3 const& spawnPos)
{
	float dist = (spawnPos - camPos).LengthApprox();
	if (dist < SPAWN_INSERTRANGE) {
		if (npc->flags.spawnInRange) {
			npc->Enable(spawnPos);
			return 1;
		}
	} else {
		npc->flags.spawnInRange = true;
	}
	return 0;
}

// inconsistent: here hungarian,
// in other places "spawnPos", "delayTime", "intanceNr", etc
oCNpc* oCSpawnManager::SummonNpc(int nInstance, zVEC3 const& posSpawn, float timeDelay)
{
	// CreateVob returns oCVob, should check that it is so in my code
	auto vob = GetGameWorld()->CreateVob(VOB_TYPE_NPC, nInstance);
	if ( auto npc = dynamic_cast<oCNpc>(vob) ) {
		npc->SetPositionWorld(posSpawn);
		ogame->GetGameWorld()->AddVob(npc);
		if (npc->SearchNpcPosition(posSpawn)) {
			npc->flags.spawnInRange = true;
			npc->flags.isSummoned = true;
			npc->Enable(posSpawn);
			SpawnNpc(npc, posSpawn, timeDelay);
			npc->Release();
			return npc;
		} else {
			ogame->GetWorld()->RemoveVob(npc);
		}
	}
	return nullptr;
}

void oCSpawnManager::DeleteNpc(oCNpc *npc)
{
	for (int i = 0; i < spawnList.GetNum(); ++i) {
		auto* node = spawnList[i];
		if (node->npc == npc) {
			Release(npc);
			// O(n^2) when could've been O(n)
			RemoveFromList(spawnList[i--]);
		}
	}
	ogame->GetWorld()->RemoveVob(npc);
}

void oCSpawnManager::DeleteAllSummoned()
{
	// O(n^3) when could've been O(n)
	for (auto node : spawnList) {
		if ( auto npc = spawnList->npc ) {
			if ( npc->flags.isSummoned )
				DeleteNpc(npc);
		}
	}
}

//------------------------------------------------------------------------------
zBOOL oCSpawnManager::CanDeadNpcBeRemoved(oCNpc *npc)
{
	if ( !npc ) return false;

	if ( npc->GetGuild() == GIL_DRAGON )
		return false;

	static bool removeOnlyEmpty {
		zoptions->ReadBool(zOPT_SEC_GAME, "spawnRemoveNpcOnlyIfEmpty", 0);
	};

	if (removeOnlyEmpty)
		return npc->inventory.IsEmpty(1,1);
	return !npc->HasMissionItem();
}

zBOOL oCSpawnManager::UseDeadNpcRoutinePos(oCNpc *npc)
{
	if ( !npc )
		return 1;

	static bool usePos {
		zoptions->ReadBool(zOPT_SEC_GAME, "spawnUseDeadNpcRoutinePos", 1);
	};

	if (usePos)
		return true;
	return npc->GetPositionWorld() == ZERO_VEC;
}

//------------------------------------------------------------------------------
void oCSpawnManager::CheckInsertNpcs()
{
	if ( spawningEnabled ) {
		insertTime += ztimer.frameTimeFloat;
		if ( insertTime >= SPAWN_INSERTTIME_MAX ) {
			insertTime -= SPAWN_INSERTTIME_MAX;
			InitCameraPos();
			CheckInsertNpc();
		}
	}
}

void oCSpawnManager::SpawnImmediately(int resetSpawnNodeTime)
{
	insertTime = SPAWN_INSERTTIME_MAX;
	if ( resetSpawnNodeTime ) {
		for (auto node : spawnList)
			node->timer = 0;
	}

	bool nolazy = zoptions->Parm("NOLAZYLOAD");

	auto& npcs = ogame->GetGameWorld()->voblist_npcs;
	for (auto npc : npcs) {
		if (npc)
			npc->AvoidShrink(1000);
		CheckRemoveNpc(npc);
	}

	CheckInsertNpcs();
}

void oCSpawnManager::CheckForInvalidDialogCamPos(float dist, oCNpc *npc)
{
	auto aicam = zCAICamera::GetCurrent();
	if ( aicam && aicam->IsModeActive(&CamModDialog) ) {
		if (npc->IsAIState("ZS_TALK") && dist > 3000.0 )
			zFAULT("U:SPAWN:Shitty DialogCam is not in valid range... >30m. Your dialog-npc may disappear..."); // 503
	}
}

zVEC3 oCSpawnManager::GetNpcRoutinePos(oCNpc* npc)
{
	// моя, не нашел подходящей функции в асме
	if ( !npc->IsDead() || UseDeadNpcRoutinePos(npc) ) {
		if ( node->npc->states.hasRoutine ) {
			pos = oCRtnManager::GetRoutinePos(npc);
		} else {
			pos = npc->states.GetAIStatePosition();
		}
	} else {
		pos = node->npc->GetPositionWorld();
	}
}

int oCSpawnManager::CheckRemoveNpc(oCNpc *npc)
{
	if ( !npc )
		return 0;
	if ( !npc->homeWorld )
		return 0;

	if ( npc->GetEM()->GetCutsceneMode() )
		return 0;
	if ( npc->IsAPlayer() )
		return 0;
	if ( !npc->IsConditionValid() )
		if (!npc->IsDead()) return 0;

	InitCameraPos();
	zVEC3 pos = npc->GetPositionWorld();
	float dist = (pos - camPos).LengthApprox()
	if (dist <= SPAWN_REMOVERANGE )
		return 0;

	pos = GetNpcRoutinePos(npc);
	dist = (pos - camPos).LengthApprox()
	if ( dist < SPAWN_REMOVERANGE )
		return 0;
	CheckForInvalidDialogCamPos(dist, npc);
	if ( npc->flags.isSummoned ) {
		DeleteNpc(npc);
		return true;
	}
	npc->Disable();
	npc->flags.spawnInRange = true;
	if ( npc->IsDead() ) {
		npc->SpawnNpc(npc, pos, 0.0);
		return true;
	}
	if ( npc->states.IsAIStateDriven() ) {
		pos = npc->states.GetAIStatePosition();
		SpawnNpc(npc, pos, 0.0);
	}
	return true;
}

void oCSpawnManager::CheckInsertNpc()
{
	for (int i = 0; i < spawnList.GetNum(); ++i) {
		auto* node = spawnList[i];
		if ( !node ) break;
		auto* npc = node->npc;
		if ( !npc ) {
			zWARNING("N: SPAWN: Invalid NPC in List!"); // 237,
			RemoveFromList(spawnList[i--]);
			continue;
		}

		if ( node->timer > 0.0 ) {
			node->timer -= SPAWN_INSERTTIME_MAX;
			continue;
		}

		if ( !npc->IsDead() || CanDeadNpcBeRemoved(npc) ) {
			float dist = (npc->GetPositionWorld() - camPos).LengthApprox();
			if ( dist >= SPAWN_REMOVERANGE ) {
				zVEC3 pos = GetNpcRoutinePos(npc);
				float dist = (pos - camPos).LengthApprox();
				if ( dist >= SPAWN_REMOVERANGE ) {
					DeleteNpc(node->npc);
					break;
				}
			}
		} else {
			if (InsertNpc(npc, node->spawnPos))
				RemoveFromList(spawnPos[i--]);
		}
	}


	zTBBox3d bbox;
	bbox.mins = camPos - UNIT_VEC * SPAWN_INSERTRANGE;
	bbox.maxs = camPos + UNIT_VEC * SPAWN_INSERTRANGE;

	zCList<oCNpc*> npcs
	rtnMan.CollidesWithWayBox(bbox, &npcs);

	for (auto npc : npcs) {
		if ( npc->homeWorld || npc->IsAPlayer() )
			continue;

		zVEC3 pos = GetNpcRoutinePos(npc);
		InsertNpc(npc, pos);
	}
}

//------------------------------------------------------------------------------
void oCSpawnManager::ShowDebugInfo()
{
	int y = 500;
	screen->Print(0,    y, "Instance:");
	screen->Print(3000, y, "Distance:");
	screen->Print(5000, y, "Allowed:");
	screen->Print(7000, y, "Delay:");

	auto fonty = screen->FontY();
	y += 500;
	for (auto node : spawnList) {
		int dist = (node->spawnPos - camPos).LengthApprox();
		zSTRING inst = node->npc->GetInstanceName();
		bool allowed = node->npc->flags.spawnInRange;
		zSTRING delay = zSTRING(node->timer, 20);
		screen->Print(0,    y, dist);
		screen->Print(3000, y, inst);
		screen->Print(5000, y, allowed);
		screen->Print(7000, y, delay);
		y += fonty; // actually was fnction call
	}
}
