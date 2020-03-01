#include <Gothic/Script/zParser.h>
#include <Logging/Log.h>
#include <Gothic/Types/zVEC3.h>
#include <Gothic/Game/zSession.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Game/zWayNet.h>
#include <Gothic/Game/oItem.h>
#include <ZenGin/zRigidBody.h>

int Wld_InsertItem()
{
	int instanceItem;
	std::string insertPoint;

	auto& ogame = Value<oCGame*>(0xAB0884);
	auto& parser = *reinterpret_cast<zCParser*>(0xAB628C);

	parser.GetParameter(insertPoint);
	parser.GetParameter(instanceItem);

	auto world = ogame->GetGameWorld();
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
		g2::Warning("Wld_InsertItemGravity()", "item could not be inserted: itemname: ", name, " ,position: ", insertPoint);
	}
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
