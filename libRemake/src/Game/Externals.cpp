#include <Gothic/Script/zParser.h>
#include <Logging/Log.h>
#include <Gothic/Types/zVEC3.h>
#include <Gothic/Game/zSession.h>
#include <Gothic/Game/oSpawnManager.h>
#include <Gothic/Game/oRtnManager.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Game/zWayNet.h>
#include <Gothic/Game/oItem.h>
#include <Gothic/Game/oNpc.h>
#include <ZenGin/zRigidBody.h>

zCParser& GetParser()
{
	return *reinterpret_cast<zCParser*>(0xAB628C);
}

int Wld_InsertItem()
{
	int instanceItem;
	std::string insertPoint;

	auto& parser = GetParser();
	auto& ogame  = Value<oCGame*>(0xAB0884);

	parser.GetParameter(insertPoint);
	parser.GetParameter(instanceItem);

	auto world = ogame->GetWorld();
	auto vob = oCItem::CreateItem(instanceItem);
	if ( vob ) {
		zVEC3 pos;
		zCWaypoint* wp = world->wayNet->GetWaypoint(insertPoint);
		if ( wp ) {
			pos = wp->GetPositionWorld();
		} else {
			auto posVob = world->SearchVobByName(insertPoint);
			if ( posVob ) {
				pos = posVob->GetPositionWorld();
			} else {
				g2::Warning("Wld_InsertItemGravity", "Position-Vob ", insertPoint, " not found.");
			}
		}
		vob->SetPositionWorld(pos);
		world->AddVob(vob);
		vob->GetRigidBody()->ApplyImpulseCM({0.0, 0.0, -100.0});
		vob->Release();
	} else {
		auto sym = parser.GetSymbol(instanceItem);
		std::string name = sym ? std::string(sym->name) : "[UNKNOWN]";
		g2::Warning("Wld_InsertItemGravity()", "item could not be inserted: itemname: ", name, ", position: ", insertPoint);
	}
	return 0;
}

int Wld_InsertNpc()
{
	zCParser& parser = GetParser();
	auto& ogame  = Value<oCGame*>(0xAB0884);

	int instanceNpc;
	std::string spawnPoint;

	parser.GetParameter(spawnPoint);
	parser.GetParameter(instanceNpc);

	oCSpawnManager* spawnManager = ogame->GetSpawnManager();
	oCNpc* npc = spawnManager->SpawnNpc(instanceNpc, spawnPoint, 0.0);
	if ( !npc )
	{
		auto sym = parser.GetSymbol(instanceNpc);
		std::string name = sym ? std::string(sym->name) : "[UNKNOWN]";
		g2::Warning("Wld_InsertNpc()", "called with illegal index. instance:", name, ", position: ", spawnPoint);
		return 0;
	}

	npc->flags.respawnOn = 0;
	if ( !ogame->inScriptStartup && npc->states.hasRoutine )
		rtnMan.UpdateSingleRoutine(npc);
	return 0;
}


void DefineExternals2(zCParser& parser)
{
	g2::Log("Parser", "Registering externals");
	parser.DefineExternal(
		"Wld_InsertItem", Wld_InsertItem,
		zPAR_TYPE_VOID,
		{zPAR_TYPE_INT, zPAR_TYPE_STRING});
}
