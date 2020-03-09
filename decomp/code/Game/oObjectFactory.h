class oCObjectFactory : public zCObjectFactory {
	Z_OBJECT(oCObjectFactory);
public:
	virtual ~oCObjectFactory() = default;
	zCEventManager* CreateEventManager(zCVob *) override { return new zCEventManager; }
	oCCSManager* CreateCSManager() override { return new oCCSManager; }
	oCGameInfo* CreateGameInfo() override { return new oCGameInfo; }
	oCPlayerInfo* CreatePlayerInfo() override { return new oCPlayerInfo; }
	oCWorld* CreateWorld() override { return new oCWorld; }
	oCWaypoint* CreateWaypoint() override { return new oCWaypoint; }
	oCWay* CreateWay() override { return new oCWay; }
	virtual oCNpc* CreateNpc(int inst)
	{
		if (inst == -1)
			return new oCNpc;
		auto npc = new oCNpc;
		npc->InitByScript(inst, 1);
		return npc;
	}
	virtual oCItem* CreateItem(int inst)
	{
		if (inst == -1)
			return new oCItem;
		auto it = new oCItem;
		it->InitByScript(inst, 1);
		return it;
	}
};
