struct oTSpawnNode {
	oCNpc *   npc;
	zVEC3     spawnPos;
	zREAL     timer;
};

class oCSpawnManager {
public:
	void Archive(zCArchiver& arc);
	void Unarchive(zCArchiver& arc);
	oCNpc* SpawnNpc(int inst, zSTRING const& spawnPoint, float timer);

private:
	zCArray<oTSpawnNode*> spawnList;

	zBOOL  spawningEnabled;
	zVEC3  camPos;
	//Verzögerungszeit des Spawnmanagers (Performancegründe)
	zREAL  insertTime;

	//war mal ne Kopierschutz Sache, böse Raubkopierer hatten mit gepimpten immortal Flags zu kämpfen. jetzt ungenutzt.
	int  spawnFlags; 
};

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

oCNpc* oCSpawnManager::SpawnNpc(int inst, zSTRING const& spawnPoint, float timer)
{
	oCWorld* wld = static_cast<oCWorld *>(ogame->GetGameWorld());

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

		npc->AddRef();
		spawnList.InserEnd(spawnNode);
	}
}

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

		spawnPos = zVEC3(vob->trafoObjToWorld.m[0][3],
		                 vob->trafoObjToWorld.m[1][3],
		                 vob->trafoObjToWorld.m[2][3]);
	}

	npc->spawnPoint = spawnPoint;
	SpawnNpc(npc, spawnPos, timer);
}
