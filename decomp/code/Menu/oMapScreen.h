// _carsten/oMenu_Status.cpp
struct oCMapScreen {
	oCMapScreen();
	virtual ~oCMapScreen();
	void Show(int instance);
};

oCMapScreen::oCMapScreen()
{
	zINFO_BEGIN(6, "B: STAT: Init Map-Screen ..."); // 661,
	zINFO_END(6, "") // 663
}

void oCMapScreen::~oCMapScreen()
{
	zINFO_BEGIN(6, "B: STAT: Deinit Map-Screen ..."); // 668,
	zINFO_END(6, ""); // 670
}

void oCMapScreen::Show(int instance)
{
	zINFO_BEGIN(3, "B: STAT: Entering Map-Screen ..."); // 675,

	auto world = dynamic_cast<oCWorld*>(ogame->GetWorld());
	if ( ogame && world && oCNpc::player ) {
		auto name = "ITWR_MAP_" + world->GetWorldName();

		oCItem* map = nullptr;
		if (instance > 0)
			map = oCNpc::player.IsInInv(instance, 1);
		if (!map)
			map = CNpc::player.IsInInv(name, 1);
		if (map) {
			auto name = map->GetInstanceName();
			auto msg = new oCMsgManipulate(EV_USEITEM, name, -1);
			oCNpc::player->GetEM()->OnMessage(msg, oCNpc::player);
		}
	}

	zINFO_END(3, "") // 694
}
